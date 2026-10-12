# Versionierte Run-Streams

`physim/run_stream.h` verwaltet Laufdateien in einem opaken `ps_run_store`.
Reader- und Writer-Handles enthalten Besitzer, Slot und Generation; alle
Operationen prüfen diese Kennung. Sie geben keine internen `FILE*` oder
mutable Zustandsfelder heraus. `ps_run_reader_describe` und
`ps_run_writer_describe` liefern unabhängige Wertkopien.

Der Store verwendet einen ausdrücklich übergebenen Allocator. Dessen Descriptor
wird kopiert; Callback-Code und Userdaten müssen bis zur Zerstörung leben.
C-stdio- und Betriebssystempuffer liegen außerhalb dieses Allocators. Ein Store
unterstützt höchstens acht Reader und acht Writer. Er benötigt externe
Synchronisierung; unabhängige Stores teilen keinen umschaltbaren Zustand.

Nach Release/Abort sind alle Kopien des betreffenden Handles ungültig.
Wiederverwendung eines Slots erhöht die Generation. Bei `UINT32_MAX` wird der
Slot endgültig stillgelegt, damit ein alter Handle nie durch Zählerüberlauf
wieder gültig wird. Fremde Besitzer, Nullgenerationen und ungültige Slots werden
abgewiesen. Handles dürfen nicht nach Zerstörung ihres Stores verwendet werden;
ein Storezeiger selbst ist kein gegen Speicherfreigabe abgesicherter Handle.

```c
#include <physim/run_stream.h>

ps_run_store *store;
ps_result result = ps_run_store_create(ps_allocator_default(), &store);
if (result == PS_OK) {
    ps_run_read_handle reader;
    result = ps_run_reader_open(store, "experiment.psrun", &reader);
    if (result == PS_OK) {
        double time, values[PS_MAX_CHANNELS];
        size_t count;
        while ((result = ps_run_reader_next(store, reader, &time, values,
                                            PS_MAX_CHANNELS, &count)) == PS_OK) {
            /* Eine vollständig validierte Messzeile; time ist in Sekunden. */
        }
        ps_run_reader_release(store, reader);
    }
    ps_run_store_destroy(store);
}
```

Die Lesekapazität muss den gesamten Kanalplan aufnehmen. Nur `PS_OK` verändert
Zeit, Werte und Anzahl; alle anderen Status erhalten diese Ausgaben.
`PS_EOF` bedeutet einen finalisierten Lauf, `PS_RECOVERED` einen lesbaren
unvollständigen/beschädigten Präfix. Nach einem terminalen Ergebnis bleibt der
Handle terminal. Ein neuer Reader beginnt erneut am Dateianfang. Messzeilen-
und Snapshot-Lesen teilen denselben Cursor; für unabhängige Durchläufe werden
getrennte Reader benötigt. Ein Snapshot-Fehler erhält die Ausgabe.

Writer legen Dateien weiterhin exklusiv an und überschreiben vorhandene Läufe
nicht. Append benötigt genau die deklarierte Zahl endlicher Kanalwerte.
`ps_run_writer_release` schreibt Index/Footer und invalidiert den Handle auch
bei einem Abschlussfehler. `ps_run_writer_abort` schließt ausdrücklich ohne
Footer; validierte bereits geschriebene Zeilen bleiben wiederherstellbar.
Ein fehlgeschlagener Dateischreibvorgang kann einen unvollständigen Dateipräfix
hinterlassen; fehlgeschlagene Opens/Creates verändern den Ausgabehandle nicht.

`ps_run_store_destroy` schließt alle Reader und finalisiert noch lebende Writer.
Es gibt alle Ressourcen auch bei Fehlern frei und liefert den ersten
Writer-Abschlussfehler. Für bekannte Fehlerläufe ist vor der Zerstörung Abort
zu verwenden, damit kein Erfolgsfooter geschrieben wird. Experiment-Runner und
App-Importvalidierung benutzen diesen Weg tatsächlich.

Die Funktionen aus `data.h` bleiben als bisherige Low-Level-Schnittstelle
verfügbar. Ihr öffentlicher Zustand wird durch diese Erweiterung nicht nachträglich
opak. Auch weitere Ressourcenschnittstellen müssen noch gegen die allgemeine
Handle-Forderung des Projektplans abgeglichen werden. Dataset/Series besitzen
bereits Generationen; Reportelemente sind append-only Besitzer-/Index-Handles.
Diese Erweiterung behauptet keine abgeschlossene gesamte Handle-Migration.
C- und Physim-Experimente nutzen den gemeinsamen umgestellten Runner.

## Fortlaufend in Physim lesen

`RunStore()` erzeugt einen besitzenden Store. `store.openReader(path)` öffnet
einen `RunReader` mit eigenem Dateicursor. Beide Werte sind Referenzhandles:
Kopien eines Stores teilen den Store, Kopien eines Readers teilen seinen
Cursor. `reader.close()` schließt diesen Cursor für alle Kopien;
`store.close()` schließt sämtliche zugehörigen Reader für alle Storekopien.
Diese Methoden sind daher auch auf `let`-Werten erlaubt. `isOpen()` prüft
den tatsächlich lebenden Store beziehungsweise Slot und Generation.

```physim
let store = RunStore()
let reader = store.openReader("experiment.psrun")
while reader.next():
    let row = reader.row()
    let time = row.time(0)
    let firstChannel = row.value(0,0)
    // row ist ein unabhängiger RunBlock mit genau einer Messzeile.
if reader.isRecovered():
    // Der validierte Präfix ist vorhanden; der Rest war unvollständig/beschädigt.
    let acceptedSamples = reader.sampleCount()
reader.close()
store.close()
```

`next()` liefert true für eine vollständig akzeptierte Messzeile und false
für `PS_EOF` oder `PS_RECOVERED`. Ein gültiger gelesener Abschluss ist über
`isComplete()` erkennbar. `statusCode()` nennt den unveränderten Core-Resultcode;
andere Fehler sind mit `attempt` fangbar. Ein terminaler Cursor bleibt terminal.
`row()` kopiert die letzte akzeptierte Zeile als besitzenden `RunBlock`.
Ein Fehler beim Kopieren verbraucht keine neue Zeile; dieselbe akzeptierte Zeile
kann danach erneut kopiert werden. Vor dem ersten erfolgreichen `next()` ist
`row()` ungültig. Nach EOF/Recovery bleibt die letzte akzeptierte Zeile erhalten.

Für Szenen öffnet man einen eigenen Reader und verwendet
`nextSnapshot()` sowie `snapshot()`. Der zurückgegebene `RunSnapshot` kopiert
sämtliche Szenenwerte, IDs, Hierarchie und Punkte. Er bleibt nach dem nächsten
Leseschritt und nach dem Schließen verwendbar. Messzeilen- und Szenentraversierung
dürfen auf einem Physim-Cursor nicht gemischt werden; der erste Aufruf legt
seinen Modus fest. Ein abgewiesener Moduswechsel verändert weder Cursor noch
letzten akzeptierten Wert. Separate Reader erlauben beide Durchläufe.

`metadata()`, `channelName(index)`, `channelSymbol(index)` und
`channelDescription(index)` liefern unabhängige besitzende Strings.
`channelDimension(index,axis)` liest einen der sieben SI-Exponenten;
`channelCount()` nennt die aufgezeichneten Kanäle. Indizes sind nullbasiert;
die synthetische Analysezeitachse zählt hier nicht mit. `sampleCount()` nennt
die vom Core bereits verarbeiteten Messzeilen und meldet `PS_LIMIT`, wenn die
Zahl nicht mehr in Int64 passt. Ein neuer Reader startet am Dateianfang.

Ein Store erlaubt höchstens acht gleichzeitig offene Reader. Geschlossene Slots
sind wiederverwendbar; alte Readerkopien bleiben ungültig. Letzte Besitzer
geben Dateien und explizit allokierten Speicher automatisch frei. Temporäre
Werte dürfen wie andere Sprachwerte bis zum Ende ihres Blocks leben; `close()`
ist der ausdrückliche Weg, Dateicursor und Slots früher freizugeben. Arrays,
Optionals, generische Strukturen, Rückgabewerte und Funktionsübergaben behalten
dieselben Besitzregeln. Ein Reader hält seinen Store bis zur letzten Readerkopie
am Leben, auch wenn der ursprüngliche Storewert seinen Gültigkeitsbereich verlässt.

Der Allocator des Stores verwaltet seine Readerbesitzer. Jede Operation benötigt
externe Serialisierung, falls ein solcher Store außerhalb des normalen
einthreadigen Sprachablaufs geteilt wird. C-stdio-/OS-Puffer zählen weiterhin
nicht zur Sprach-Speicherbilanz. Die C-/Physim-Tests provozieren Store-, Reader-
und Zeilenallokationsfehler, prüfen sämtliche Slots und zählen die nach
Programmende verbleibenden expliziten Bytes.

Die schreibenden Low-Level-Stream-Funktionen bleiben in dieser Stufe C-APIs.
Die neue Lesebindung ersetzt weder den bestehenden `RunIndex` für wahlfreien
Zugriff noch Dataset/Series für Analysen und schließt die allgemeine
C-/Physim-Funktionsparität des Projektplans noch nicht vollständig ab.


Der SDK-Prüfer (`tools/verify-native-sdk.py --stream-only`) prüft den C- und
Physim-Lesezugang auch nach Verschiebung des Pakets. Er verwendet ausschließlich
installierte öffentliche Header, das installierte Archiv und einen aus den
SDK-Quellen neu gebauten Core. Standalone-Speicherbereinigung, beschädigte
Dateien und tatsächliche Experiment-/Analyse-Runner gehören zu dieser Prüfung.

## Fortlaufend in Physim schreiben

`RunStore.configure(dt, seed, modelMetadata)` konfiguriert vor der ersten
Writer-Erstellung einen endlichen positiven Zeitschritt, den Seed und höchstens
2047 UTF-8-Metadatenbytes. Der Int64-Seed wird wie beim RNG als 64-Bit-
Bitmuster übernommen. `addChannel(name, unit, description)` deklariert
höchstens 16 eindeutige kanonische SI-Kanäle; es liefert den nullbasierten Index.
Konfiguration und Schema gehören zum gemeinsamen Store. Änderungen gelten nur
für künftig erzeugte Writer; jede Datei erfasst bei ihrer Erstellung das dann
vorliegende Schema. Bestehende Leser werden dadurch nicht verändert.

`createWriter(path, name)` erzeugt einen `RunWriter` exklusiv; vorhandene Dateien
werden nicht ersetzt. Ein konfigurierter Zeitschritt und mindestens ein Kanal
sind erforderlich. Höchstens acht Writer sind gleichzeitig geöffnet. Kopien
teilen den Writer und seinen Zähler. Ein Writer hält den Store am Leben, auch
wenn die ursprüngliche Store-Variable ihren Gültigkeitsbereich verlässt.

`append(time, values)` schreibt genau eine vollständige endliche SI-Messzeile.
Die Zahl der Float64-Werte muss exakt dem erfassten Schema entsprechen.
`appendSnapshot(snapshot)` schreibt einen unabhängigen vorhandenen RunSnapshot,
mit gleicher Kanalzahl, samt Zeit, Messwerten, Pause und Szene. Ein Snapshot
fügt keine Messzeile hinzu. `sampleCount()` und `channelCount()` lesen die
Core-Zähler. `close()` finalisiert die Datei; `abort()` schließt ohne Footer.
Beide Operationen invalidieren sämtliche Kopien, auch bei einem I/O-Fehler.
Alte Kopien bleiben nach Wiederverwendung eines Slots ungültig.

Ungültige Eingaben und fehlgeschlagene Besitzerallokationen ändern keinen
bestehenden Writer und erzeugen keine Datei. Fehlgeschlagene Datei-I/O kann
entsprechend dem C-Vertrag einen recoverbaren unvollständigen Rest hinterlassen.
Explizites Schließen meldet Finalisierungsfehler als fangbaren Fehler. Bei der
letzten automatischen Besitzerfreigabe werden Dateien finalisiert und Ressourcen
auch bei I/O-Fehlern freigegeben; dieser Destruktor liefert keinen Fehler zurück.
Für einen überprüften Abschluss ist daher `close()` erforderlich. Store-close
finalisiert alle noch lebenden Writer und invalidiert Leser und Writer gemeinsam.


```physim
let store = RunStore()
store.configure(0.01, 42, "model=measured-motion")
let metres = Unit(1,0,0,0,0,0,0,1,"m")
let channel = store.addChannel("position.x", metres, "Measured position")
let writer = store.createWriter("measured-motion.psrun", "Recorded motion")
writer.append(0, [1.0])
writer.append(0.01, [1.1])
writer.close()
store.close()
```

Der SDK-Prüfer prüft ebenfalls Writer, Besitzerallokationen und bytegenaue
C-/Physim-Archivparität einschließlich Szenen und Index. Der Schreibzugang ist
in Standalone-, Experiment- und Analysemodulen verfügbar; die allgemeine
Sprach- und Ressourcenmigration bleibt davon unabhängig.

## Ereignisse und Warnungen

`ps_run_writer_event(store, writer, &record)` schreibt einen validierten
`ps_log_record`. `ps_run_reader_event_next(store, reader, &record)` liest ihn
mit eigenem Cursor. Ein Record enthält Level 1–4, endliche Sekundenzeit und
eine begrenzte UTF-8-Nachricht. Fehler bewahren die Leseausgabe; ungültige
Schreibangaben schreiben keine Bytes. EOF/Recovery bleiben für das Handle
terminal. Messpunkte und Szenen werden durch Ereignisse nicht hinzugefügt.

```text
writer.appendEvent(3,0.25,"Warnung: α")
let events = store.openReader("experiment.psrun")
while events.nextEvent():
    print(events.eventMessage())
events.close()
```

Physim-Cursor erlauben jeweils nur Mess-, Szenen- oder Ereignistraversierung.
Kopien teilen den Cursor und den letzten akzeptierten Record. Die Getters
liefern vor dem ersten Ereignis einen fangbaren Fehler; die Nachricht wird
als besitzender String kopiert. Nach EOF bleibt der letzte Record verfügbar.
Nutze separate Reader, um mehrere Arten durchzulaufen. Der Low-Level-C-Reader
erfordert ebenfalls einen separaten Cursor für jede Traversierung.
