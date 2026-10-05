# `.psrun` Format 1 und IPC 3

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
| 5 | optionale Szene: Snapshotversion u32 = 1, danach der unten beschriebene Snapshotkopf, Werte, Objekte und Punkte |

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
Geometrie einschließlich Orientierung, IDs, Labels und Polyline-Punkten.
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

Header (20 Bytes): Magic u32 `0x5053494D`, Version u32 = 3, Typ u32,
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
STEP im pausierten Zustand führt genau einen festen Zeitschritt aus.
Die Geschwindigkeit gehört zur Zeitsteuerung und ändert keine Modul-ABI,
Snapshotfelder oder Messdateiformate. CLI: `--interactive --speed 0|0.1..16`;
ohne Angabe gilt 1×. Ohne `--interactive` bleibt der CLI-Modus mit `--steps` immer Offline.

Snapshotkopf (24 Bytes): Zeit f64, Kanalzahl u32, Objektzahl u32, Pausestatus u32
(ausschließlich 0/1), Punktzahl u32. Danach Kanalwerte f64, Objekte und Punkte.

Jedes Objekt belegt genau 172 Wire-Bytes:

| Offset | Feld |
| --- | --- |
| 0 | Form u32: Kugel=0, Linie=1, Box=2, Pfeil=3, Punkt=4, Ebene=5, Polyline=6, Label=7 |
| 4 | RGBA u32 |
| 8 / 32 | a.xyz / b.xyz, jeweils drei f64 |
| 56 | Radius f64 |
| 64 | Quaternion x/y/z/w, vier f64 |
| 96 | 64 Bytes UTF-8-Label inklusive NUL; ungenutzte Bytes werden mit null geschrieben |
| 160 / 164 | erster Polyline-Punkt / Punktzahl, je u32 |
| 168 | optionale Objekt-ID u32; 0 anonym, sonst eindeutig innerhalb der Szene |

Nach den Objekten folgen `Punktzahl` Weltkoordinaten, jeweils x/y/z als f64 (24 Bytes).
Maximal 32 Objekte, 96 Punkte und 16 Kanäle ergeben eine Payload von 7960 Bytes.
Polylinien enthalten mindestens zwei Punkte und referenzieren ausschließlich ihren
gültigen Bereich im gemeinsamen Punktpuffer. Keine externen Speicheradressen werden
übertragen. Der Decoder prüft alle Größen, Zahlen und Texte vor Übernahme der Szene;
bei Fehler bleiben die Ausgaben unverändert. IPC 1/2 und Modul-ABI 1/2 sind inkompatibel
und werden abgewiesen. Das Messdateiformat bleibt unverändert.

`stdout` ist im interaktiven Runner für IPC reserviert. Direkte printf-/stderr-Ausgaben
aus Experimentcode verletzen derzeit den Kanal; die App erkennt dies und stoppt den
Runner. Ein eigener strukturierter Logkanal ist noch zu ergänzen.
