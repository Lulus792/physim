# Suchfokus: tatsächlicher Nachlauf auf beiden Mac-Architekturen

Zur Quelle `7c1870c4` ist der Editorlauf `42081349550` vollständig erfolgreich.
Die Originalarchive wurden getrennt gelesen und anhand der offiziellen
SHA-256-Digests geprüft:

| Architektur | Job | Tatsächlicher Umfang | Originalnachweis |
| --- | --- | --- | --- |
| Intel | 127434902003 | 14/14 Daten, 12/12 UI-Gruppen | [Intel](macos-search-focus-intel-2026-11-05.json) |
| Apple Silicon | 127434901591 | 14/14 Daten, 12/12 UI-Gruppen | [ARM](macos-search-focus-arm-2026-11-05.json) |

Die ARM-Runner-Image-Kennung `macos-15-arm64` wurde zusätzlich im Original-
Joblog gelesen. Beide Architekturen enthalten jeweils zwölf tatsächliche
Suchfeld-/Schaltflächen-Fokussequenzen über C, Ph und drei Editoren. Der
separate Peerprozess bestätigt den tatsächlichen OS-Fokuswechsel; die Text-
und Tastenereignisse bleiben geskriptet. Das belegt keine physische Tastatur-
oder vollständige VoiceOver-Abnahme.

Alle 22 Archiveinträge wurden gelesen. Die 18 enthaltenen Bildinhalte sind
bytegleich zu den früher vollständig geprüften Referenzeinträgen; es wurden
keine neuen Bilder extrahiert. Der zuerst gelesene Intel-Einzelbericht nennt
noch den damaligen ARM-Wartestand. Dieser nachfolgende ARM-Nachweis ergänzt ihn;
der eingefrorene ursprüngliche Bericht wurde nicht nachträglich verändert.

Der getrennte [Workspace-Nachlauf](macos-workspace-7c-intel-2026-11-05.json)
besteht unter Intel ebenfalls 4/4 Gruppen samt 68 tatsächlich gebauten,
aufgezeichneten und ausgewerteten Projekten bei 16/22 px. Dessen ARM-Job wurde
vor dem ersten Schritt abgebrochen und bleibt eine offene Prüfdimension.
Diese beiden Workflows nicht miteinander zu einer vollständigen Mac-Abnahme
vermischen. Der private Nutzer-Mac blieb ausgeschaltet und wurde nicht kontaktiert.
