# `.psrun` Format 1 und IPC 4

## Datendatei

Alle Integer: unsigned little-endian. Alle Messwerte: IEEE-754 binary64 little-endian.
Keine rohen C-Strukturen werden serialisiert.

Header (16 Bytes): `PSRUN17\n` (8 Bytes), Formatversion u32 = 1,
Endianness-Marker u32 = `0x01020304`.

Jeder Chunk: Typ u32, Payloadlänge u32, CRC32 der Payload u32, Payload.
CRC32: reflektiertes Polynom `0xEDB88320`, Initialwert und abschließendes XOR
`0xFFFFFFFF`, Testvektor `123456789` → `0xCBF43926`.

| Typ | Inhalt |
| --- | --- |
| 1 | UTF-8-Metadaten in `key=value`-Zeilen, ohne abschließendes NUL |
| 2 | Kanalzahl u32, dann je Kanal 48 Bytes Name, 16 Einheit, 96 Beschreibung, 7 int8 SI-Exponenten |
| 3 | Zeit f64 und genau Kanalzahl f64-Messwerte |
| 4 | Gesamtzahl Messpunkte u64; Abschlussmarker |
| 5 | optionale Szene: Snapshotversion u32 = 1 oder 2, danach Snapshotkopf, Werte, Objekte und Punkte |

Metadaten und Schema stehen unmittelbar nach dem Header. Strings im Schema sind
NUL-terminiert und auf ihre Feldbreite begrenzt. Die Payloadobergrenze ist 8192 Bytes.
Bei definierten Experimentparametern enthält der Metadatenchunk je Name die
Zeilen `parameter.<name>`, `parameter_default.<name>`,
`parameter_min.<name>` und `parameter_max.<name>` mit dezimalen binary64-Werten.
`parameter.<name>` ist der im Lauf wirksame Wert nach einem möglichen Override.
Unbekannte Chunktypen innerhalb dieser Obergrenze werden nach CRC-Prüfung übersprungen.
Eine Schemaänderung innerhalb eines Laufs ist nicht zulässig.

Die Snapshotversion ist unabhängig von Dateiformat, Modul-ABI und Pipe-Version.
Der Snapshot enthält Zeit, zugehörige Kanalwerte, Pausestatus und validierte
Geometrie einschließlich Orientierung, IDs, Labels und Polyline-Punkten. Version 2
enthält zusätzlich Gruppen und Eltern-IDs. Version 1 bleibt lesbar und erhält
Eltern-ID 0 für sämtliche Einträge.
Seine Kanalzahl muss dem Dateischema entsprechen. Der Block fügt keinen Messpunkt
hinzu: Der Abschlusszähler zählt ausschließlich Typ 3. Gleiche Snapshotzeiten
sind bei RUN-/PAUSE-Rückmeldungen erlaubt; die Zeitleiste zeigt dann den letzten
Zustand dieses Zeitpunkts. Messwertleser überspringen die optionalen Blöcke nach
CRC-Prüfung. Ältere Dateien benötigen keine Konvertierung.

Interaktive Läufe archivieren die ausgegebenen Szenen einschließlich Anfang und
regulärem Ende. Die Offline-CLI zeichnet mit `--record-scenes` zusätzlich
ausgewählte Zustände in Abständen von mindestens 1/60 Simulationssekunde auf;
der letzte Zustand wird auch bei kürzerem Restabstand gespeichert. Die normale
CLI ohne diesen Schalter speichert weiterhin nur Messwerte. Alle Physikschritte
bleiben unabhängig von der Szenenfrequenz als Typ 3 erhalten.

`ps_run_snapshot_next` liest die Szenen mit einem eigenen Reader, validiert dabei
auch übersprungene Messpunkte und prüft den Abschlusszähler. Ungültige Szenen,
unbekannte Snapshotversionen und unvollständige/CRC-fehlerhafte Enden werden gemeldet;
die ausgegebene Szene bleibt bei einem Lesefehler unverändert.

Jeder Punkt wird mit `fflush` an das OS übergeben. Alle 100 Punkte und beim Abschluss
folgt `fsync`/`_commit`. Bei einem reinen Prozessabsturz bleiben vollständige Blöcke
lesbar. Bei Stromausfall sind Daten seit dem letzten erfolgreichen Sync möglicherweise
verloren. Das Format verspricht keine atomare Speicherung eines gerade geschriebenen
Blocks; stattdessen wird ein unvollständiger/CRC-fehlerhafter Endblock erkannt.

Die erste Version besitzt einen Abschlusszähler, noch keinen Zufallszugriffsindex.
Recovery arbeitet sequentiell bis zum letzten gültigen Block.

## Runner-Pipe

Header (20 Bytes): Magic u32 `0x5053494D`, Version u32 = 4, Typ u32,
Payloadlänge u32 (maximal 8192), Sequenznummer u32 (je Richtung ab 0).
Unvollständige Frames werden gesammelt, falsche Versionen, Sequenzen und Größen abgewiesen.

Typen: HELLO=1, RUN=2, PAUSE=3, STEP=4, STOP=5, SNAPSHOT=6,
ERROR=7, BYE=8, HEARTBEAT=9, SPEED=10. Kontrollbefehle außer HELLO und SPEED tragen keine Payload.
Host-HELLO enthält ABI-Version u32. Runner-HELLO enthält Name und Kanaltitel als
UTF-8-Zeilen. Der Runner bleibt bis zum Handshake pausiert; Frist: 10 Sekunden.
Heartbeat: 500 ms. Die GUI markiert mehr als drei Sekunden ohne Nachricht;
Stop beendet nach einer Sekunde auch festhängenden Code.

SPEED trägt genau acht Bytes: einen little-endian f64-Faktor. Null bedeutet
Offline, endliche Faktoren von 0,1 bis 16 regeln die Echtzeitgeschwindigkeit.
Der Befehl ist erst nach HELLO zulässig; falsche Länge, nicht endliche oder
außerhalb des Bereichs liegende Werte beenden den Runner mit einem Protokollfehler.
Ein Wechsel verwirft das Echtzeitkonto und lässt den Pausestatus unverändert.
RUN und PAUSE melden ihren Zustand sofort als Snapshot, auch ohne neuen Physikschritt.
STEP im pausierten Zustand führt im festen Modus einen festen Zeitschritt aus;
im adaptiven Modus einen akzeptierten Schritt innerhalb der konfigurierten Grenzen.
Die Geschwindigkeit gehört zur Zeitsteuerung und ändert keine Modul-ABI,
Snapshotfelder oder Messdateiformate. CLI: `--interactive --speed 0|0.1..16`;
ohne Angabe gilt 1×. Ohne `--interactive` bleibt der CLI-Modus mit `--steps` immer Offline.

Snapshotkopf (24 Bytes): Zeit f64, Kanalzahl u32, Objektzahl u32, Pausestatus u32
(ausschließlich 0/1), Punktzahl u32. Danach Kanalwerte f64, Objekte und Punkte.

Jedes Objekt belegt in Snapshotversion 2 genau 176 Wire-Bytes:

| Offset | Feld |
| --- | --- |
| 0 | Form u32: Kugel=0, Linie=1, Box=2, Pfeil=3, Punkt=4, Ebene=5, Polyline=6, Label=7, Gruppe=8 |
| 4 | RGBA u32 |
| 8 / 32 | a.xyz / b.xyz, jeweils drei f64 |
| 56 | Radius f64 |
| 64 | Quaternion x/y/z/w, vier f64 |
| 96 | 64 Bytes UTF-8-Label inklusive NUL; ungenutzte Bytes werden mit null geschrieben |
| 160 / 164 | erster Polyline-Punkt / Punktzahl, je u32 |
| 168 | optionale Objekt-ID u32; 0 anonym, sonst eindeutig innerhalb der Szene |
| 172 | Eltern-ID u32; 0 Wurzel, sonst eine vorhandene Szenen-ID |

Gruppen sind benannte Einträge mit eindeutiger nichtnull ID und ohne Geometrie.
Elternbeziehungen dürfen weder auf fehlende IDs zeigen noch Zyklen bilden. Alle
Koordinaten bleiben Weltkoordinaten; Eltern führen keine Transformation aus.
Version 1 verwendet weiterhin 172 Objektbytes und Formen 0–7. Der ausdrücklich
deklarierte Versionswert bestimmt die Länge; ein gekürzter Version-2-Block wird
nicht als Version 1 umgedeutet. `ps_snapshot_decode_version` liest beide Versionen,
`ps_snapshot_decode` die aktuelle Version 2.

Nach den Objekten folgen `Punktzahl` Weltkoordinaten, jeweils x/y/z als f64 (24 Bytes).
Maximal 32 Einträge einschließlich Gruppen, 96 Punkte und 16 Kanäle ergeben
eine Payload von 8088 Bytes.
Polylinien enthalten mindestens zwei Punkte und referenzieren ausschließlich ihren
gültigen Bereich im gemeinsamen Punktpuffer. Keine externen Speicheradressen werden
übertragen. Der Decoder prüft alle Größen, Zahlen und Texte vor Übernahme der Szene;
bei Fehler bleiben die Ausgaben unverändert. IPC 1/2/3 und Modul-ABI 1/2 sind inkompatibel
und werden abgewiesen. Das Messdateiformat bleibt unverändert.

Die Modul-ABI bleibt 3: `parent_id` nutzt die bisherigen vier Padding-Bytes am
Ende von `ps_object`; Objekt- und Szenengröße bleiben unverändert. C-Module
aktivieren `PS_EXPERIMENT_SCENE_HIERARCHY` in `ps_experiment_api.capabilities`.
Ohne diesen Vertrag setzt der Runner Eltern-IDs nach dem Callback auf 0, da alte
Module diese Bytes nicht initialisieren müssen. Neue Sprachmodule melden die
Fähigkeit automatisch. Hierarchie benötigt den aktuellen Runner; alte Module
mit ABI 3 bleiben auf dem aktuellen Runner verwendbar.

`stdout` ist im interaktiven Runner für IPC reserviert. Direkte printf-/stderr-Ausgaben
aus Experimentcode verletzen derzeit den Kanal; die App erkennt dies und stoppt den
Runner. Ein eigener strukturierter Logkanal ist noch zu ergänzen.


## Adaptive Zeiten und ABI-3-Erweiterung

Der numerische Dateichunk bleibt unverändert. Adaptive Messungen speichern den
akzeptierten Zeitfortschritt statt `Schrittnummer × dt`. `dt_s` in den Metadaten
ist die Startdauer; `step_mode=adaptive`, `minimum_dt_s` und `maximum_dt_s` beschreiben
die Hostgrenzen. Die Szenenfrequenz verändert weder das adaptive Raster noch die
Anzahl numerischer Messungen. Die Analyse und Zeitleiste lesen die tatsächlichen
Zeitwerte. SDK-Analysen arbeiten mit diesem unregelmäßigen Raster.

`ps_experiment_api.adaptive_step` ist ein optionales Feld nach `destroy`.
`PS_EXPERIMENT_API_BASE_SIZE` benennt die Größe bis zu diesem Feld. Der aktuelle
Runner akzeptiert weiterhin einen eingefrorenen ABI-3-Descriptor ohne diesen Tail.
Er liest den neuen Callback nur bei Adaptive-Auswahl, gesetztem
`PS_EXPERIMENT_ADAPTIVE_STEPS`-Bit, ausreichendem `struct_size` und einem gültigen
Funktionszeiger. ABI-Version, Kontextstruktur und Pipe-Version bleiben erhalten.
C-Module ohne Callback initialisieren das neue Feld mit `NULL`.

Der Callback erhält Vorschlag, Mindest- und Höchstdauer und liefert
`ps_step_interval` mit `elapsed_s` und `next_s`. Er aktualisiert Modell und
Kanalwerte bei Erfolg und lässt `context->time_s` unverändert. Der Host prüft
endlichen positiven Fortschritt und die Grenzen, setzt die tatsächliche Zeit
und `context->dt_s` auf die akzeptierte Dauer und speichert genau einen Messpunkt.
Der nächste Vorschlag steuert die nächste akzeptierte Iteration; beim Reset wird
wieder die konfigurierte Startdauer verwendet. Ungültige Berichte oder numerische
Fehler beenden den Runner mit lesbarem Fehler und erhalten den gültigen Dateipräfix.
Sie schreiben keinen zusätzlichen Punkt und keinen erfolgreichen Footer.


## Zielzeit im Offline-Runner

`--until` ist eine positive endliche Zielzeit bis `1e9 s` und erfordert den
Offline-Modus. `--steps` begrenzt dann die Zahl akzeptierter Schritte. Erschöpftes
Budget vor der Zielzeit liefert einen Fehler und lässt den gültigen Präfix ohne
Erfolgsfooter zurück. Ohne `--until` behält `--steps` die bisherige Bedeutung.

Im festen Modus bleiben vollständige Rasterpunkte unverändert; ein Restintervall
endet an der Zielzeit. Adaptive Schritte verwenden den jeweils akzeptierten
Fortschritt. Ist der verbleibende Rest kürzer als der konfigurierte Mindestschritt,
erhält der Callback diese Restdauer als effektives Minimum. Sein erfolgreicher
Bericht muss die Zielzeit genau erreichen; der Host extrapoliert keine Messwerte.
Es gelten weiterhin Fehlertoleranzen, Budget und positive endliche Zeitfortschritte.

Zusätzlich speichert der Metadatenblock `end_time_s` und `maximum_accepted_steps`.
`dt_s` bleibt die Startdauer. Fehlender Platz für Modus, Ziel, Grenzen oder
Modulidentität verhindert das Anlegen der Datei. Numerische Chunks und Footer
behalten Format 1. Die Serienprüfung verlangt passenden Seed, Konfiguration,
strikt steigende Zeiten, gültige Intervallgrenzen und exakt dieselbe Endzeit
für jeden akzeptierten Lauf. Verschiedene adaptive Raster und Punktzahlen sind zulässig.
