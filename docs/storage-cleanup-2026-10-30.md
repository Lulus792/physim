# Speicherpflege nach den Aufgabenfenster-Prüfungen

Am 30. Oktober 2026 wurden 15 ersetzte Rohdateien der Windows-Gegenproben entfernt:
fünf Testprogramme mit ihren PDBs und Build-Metadaten. Dateivolumen:
**53.073.159 Byte (etwa 50,6 MiB)**. Es gab keine rekursive Bereinigung.

Betroffen sind ausschließlich die Testnamen `physim-project-window-counter`,
`physim-project-footer-counter`, `physim-project-footer-wrap-counter`,
`physim-project-foreign-input-counter` und `physim-project-font-reflow-counter`
im isolierten Windows-Build. Die Läufe waren vorher beendet.

Vor jeder Entfernung wurden Zielpfad, Datei-SHA-256 und der zugehörige Archivinhalt
gegen den eingefrorenen `project-final-byte-index.json` geprüft. Die exakten
Programm-, Debug- und Metadatenbytes bleiben wiederherstellbar in den Archiven.
Produktionsprogramm und Bibliotheken, Quellen, Referenzen, Bilder, Fehlerbelege,
Testberichte, Abhängigkeiten und WSL-Datenträger bleiben erhalten.

Der kompakte lokale Bericht liegt unter
`build/validation/explorer-inline-2026-10-29/project-counter-raw-cleanup.json`.
Der anschließende Stand `project-cleanup` hält den verbleibenden Laufzeitbestand
erneut fest; die vorherigen Nachweise wurden nicht verändert. Physim-Daten liegen
weiterhin auf D:. F: wurde nicht verwendet.

Das [versionierte Gegenprobenwerkzeug](../tools/check_project_window_counterexample.py)
kann neue Varianten aus einem fertig gebauten, unveränderlichen Quellstand erzeugen.
Die [native Teilabnahme](visuals/unified-workspace/TASK-WINDOWS-NATIVE.md) unterscheidet
die geprüfte Projektanlage von der weiterhin offenen Ordnerwahl A.
