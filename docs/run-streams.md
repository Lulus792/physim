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
Diese Erweiterung behauptet keine abgeschlossene gesamte Handle-Migration oder
zusätzliche direkte Physim-Sprachbindungen für Low-Level-Streams. C- und Physim-
Experimente nutzen den gemeinsamen umgestellten Runner.
