# Indizierte Laufdateien

Finalisierte `.psrun`-Dateien enthalten jetzt einen Abschlussindex. Er ergänzt
Format 1 um optionale CRC-geschützte Chunks; bestehende Reader überspringen sie.
Der vorhandene Footer und die öffentlichen Reader-/Writerlayouts bleiben erhalten.
Experiment-Runner schreiben ihn beim erfolgreichen Abschluss automatisch, für
C und Physim gleichermaßen. Ein Abbruch benötigt keinen Index, um den bisherigen
lesbaren Präfix wiederherzustellen.

Die C-API in `physim/run_index.h` besitzt eine geöffnete Datei und Checkpoints
aus einem expliziten Allocator. Beim Öffnen validiert sie einmal alle lesbaren
Mess- und Szenenblöcke. Für jede 256. Messzeile und jede aufgezeichnete Szene
speichert sie Position und ursprüngliche Zeit. Ein vorhandener Abschlussindex
gilt nur dann als bestätigt, wenn sämtliche Einträge mit diesen tatsächlich
gelesenen Blöcken übereinstimmen. Alte Dateien erhalten dieselben Checkpoints
durch Rekonstruktion im Speicher. Die Quelldatei wird dabei nicht verändert.

```c
#include <physim/run_index.h>

ps_run_index *index = NULL;
ps_result result = ps_run_index_open("runs/example.psrun",
    ps_allocator_default(), 100000, &index);
if (result == PS_OK || result == PS_RECOVERED) {
    ps_run_index_info info = {
        .struct_size = sizeof info, .version = PS_RUN_INDEX_VERSION
    };
    result = ps_run_index_get_info(index, &info);
    double time, values[PS_MAX_CHANNELS];
    if (result == PS_OK && info.samples > 1000)
        result = ps_run_index_read(index, 1000, 1, &time, values);
    ps_run_index_destroy(index);
}
```

`PS_OK` beim Öffnen bedeutet einen validierten Footer, `PS_RECOVERED` einen
geprüften Präfix ohne gültigen Abschluss. **Beide Ergebnisse liefern einen
besitzenden Handle**, der geschlossen werden muss. Andere Ergebnisse erhalten
den Ausgabezeiger. `maximum_entries` begrenzt die gemeinsame Zahl aller
Messcheckpoints und Szenen und muss positiv sein; Allokationsfehler sind
`PS_MEMORY`, ausgeschöpfte Grenzen `PS_LIMIT`. Allocator und dessen Userdaten
müssen bis zum Zerstören leben. Handles werden nicht kopiert oder nebenläufig
verwendet. Die einmal geöffnete Datei bleibt auch nach Umbenennung dieselbe.

`ps_run_index_get_info` kopiert Metadaten, Schema, Kanalzahl, Mess-/Szenenzahl,
Checkpointzahl und die Flags `complete` und `persisted`. Der Aufrufer initialisiert
Größe und Version. `complete` bestätigt den Footer; `persisted` zusätzlich die
vollständige Übereinstimmung des gespeicherten Index mit dem Dateipräfix.
Ein semantisch ungültiger Index mit gültigen CRCs wird verworfen und im Speicher
rekonstruiert. Bei beschädigtem CRC/Tail endet der geprüfte Präfix dort, wie beim
bisherigen Streamingreader. Ungültige Messwerte oder Szenen liefern einen Fehler.

Messabfragen sind nullbasiert. Ein Aufruf liest höchstens 256 Zeilen und schreibt
`times[count]` sowie `values[count * channels]` in zeilenweiser Reihenfolge.
Er sucht zum letzten Checkpoint vor der ersten Zeile und validiert alle
übersprungenen und ausgewählten Messblöcke erneut. Szenenabfragen suchen direkt
zum aufgezeichneten Snapshot und prüfen erneut CRC, Version und Geometrie.
Ein nicht vorhandener Bereich meldet `PS_EOF`; Fehler erhalten alle Ausgaben.
Weder Zeiten noch Zeilennummern werden interpoliert. Nichtmonotone Messzeiten
ändern die Reihenfolge nicht. Dateien dürfen während der Nutzung nicht verändert
werden; das erneute Prüfen ausgewählter Blöcke ist keine Änderungssynchronisierung.

Writerfinalisierung scannt die aufgezeichneten Daten einmal zusätzlich und
schreibt Indexseiten mit höchstens 255 Einträgen. Dieser Schritt verwendet nur
begrenzte lokale Puffer und keinen Heap. Ein Schreib-/Validierungsfehler schließt
die Datei ohne erfolgreich bestätigten Footer. Der Preis des Index ist derzeit
ein zusätzlicher linearer Finalisierungsscan; auch das geprüfte Öffnen ist linear.
Der Index beschleunigt anschließende gezielte Abfragen. Kompression, neue
Messdatentypen und ein ungeprüfter Schnellöffnungsmodus sind damit nicht implementiert.
Physim-Experimente speichern den Index automatisch über denselben Runner.
Physim 0.176.0 ergänzt eigene Abfragen mit `RunIndex`, `RunBlock` und `RunSnapshot`.

## Gezielte Abfragen in Physim

```text
var run = RunIndex("runs/example.psrun", 100000)
let block = run.read(1000, 32)
let times = block.times()
let position = block.column(0)
let savedScene = run.snapshot(0)
let copy = run
run.close()
assert(!run.isOpen() && copy.isOpen())
```

`RunIndex(path, maximumEntries)` prüft dieselben C-Daten im gemeinsamen
64-MiB-Sprachbudget. `sampleCount`, `snapshotCount`, `checkpointCount` und
`channelCount` sind Methoden; `isComplete()` und `isPersisted()` zeigen den
Abschluss-/Indexstatus. Ein rekonstruierbarer Präfix liefert einen offenen Wert.
`metadata()`, `channelName(i)`, `channelSymbol(i)` und `channelDescription(i)`
liefern eigene Strings. `channelDimension(i, axis)` liest die sieben SI-Exponenten.
In einem Analysemodul liefert `inputPath(i)` einen eigenen String mit dem
ausgewählten Eingabepfad, geprüft gegen `inputCount()`.

Kopien teilen einen automatisch verwalteten Dateibesitzer. `close()` verändert
nur die Referenz des jeweiligen `var`-Werts; andere Kopien bleiben offen. Nach
der letzten Referenz werden Datei und Checkpoints freigegeben. Arrays, optionale
Werte, Strukturfelder, Rückgaben und Closures verwenden dieselben Besitzregeln.
Die Handlekopien werden in diesem synchronen Sprachmodell nicht nebenläufig
verwendet. Wiederholtes `close()` ist erlaubt; weitere Abfragen am geschlossenen
Wert lösen `PS_INVALID` aus. `isOpen()` bleibt abfragbar.

`RunBlock` hält unabhängige Zeilen und bleibt nach Schließen des Index verwendbar.
`count()`, `channelCount()`, `time(row)` und `value(row, channel)` lesen die Werte.
`times()` und `column(channel)` liefern eigene Float64-Arrays. `RunSnapshot`
kopiert den vollständigen gespeicherten Zustand ohne geliehene Zeiger. Seine
Methoden lesen Zeit, Pausestatus, Messwerte, Objekt-/Punktzahl, IDs, Eltern, Form,
Farbe, lokale Position, Ausdehnung/Scale, Radius, Quaternion, Text und
Polyline-Bereiche/Punkte. `worldPoint(index, local)` und `transform(index)`
verwenden denselben Core-Transformationsvertrag. Objektindizes sind Slots,
keine stabilen IDs; alle Indizes sind nullbasiert und geprüft.

`attempt` fängt Abfrage-/Allokationsfehler ab und erhält alle bestehenden Werte.
Ein nicht vorhandener C-Bereich (`PS_EOF`) wird zu `PS_INVALID`, beschädigte
ausgewählte Blöcke (`PS_RECOVERED`) zu `PS_CORRUPT`; andere Fehlercodes bleiben
erhalten. Erfolgreich rekonstruierte Dateien lösen beim Öffnen keinen Fehler aus.
Bereiche werden nicht interpoliert oder an eine andere Zeitreihe angepasst.
Das [Analysebeispiel](../examples/language/run_index_analysis.phys) erzeugt
aus einem gelesenen Block ausdrücklich ausgerichtete Reihen und übernimmt
die Kanal-Dimensionen. [Wert-/Besitzprüfungen](../examples/language/run_index_values.phys)
verwenden die durch den C-Prüfer erzeugten Fixtures.

Eine Million Zeilen erzeugen 3907 Messcheckpoints. Die ausführbare Prüfung
kontrolliert diesen Fall mit einem Allocatorbudget von 600000 Bytes, zusätzlich
Szenen, Dateien ohne Index, Recovery, falsche Checkpoints trotz gültiger CRC,
unbekannte Indexversionen, Ausgabeschutz und jede fehlgeschlagene Allokationsstelle.
Ein unabhängiger Pythonreader prüft Dateiformat, CRC, jede gespeicherte Position
und das Verhalten eines Readers, der die neuen Chunktypen überspringt.
