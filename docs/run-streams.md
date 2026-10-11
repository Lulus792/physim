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
