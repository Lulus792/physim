# Speicherpflege: weiterer Nachlauf auf D:

Alle Physim-Daten bleiben auf D:. Die [geprüfte Rückführung](storage-cleanup-2026-10-25.md)
ist abgeschlossen; in diesem Nachlauf wurde F: weder abgefragt noch beschrieben.
`D:/Physim/build` ist weiterhin ein echtes Verzeichnis. Wrapper, Builder und
lokale Konfiguration verwenden diesen Stamm; die gemeinsame WSL-VM bleibt erhalten.

## Tatsächlich ausgeführt

| Prüfung oder Bereinigung | Ergebnis |
| --- | --- |
| D:-Regeln unter Windows und `Alpine-Dev` | Je drei Befehle bestanden: Pfadprüfung, Speicherregression sowie echte Temporärdatei unter D: mit unverändert weitergegebenem Kindprozess-Exitcode 7. |
| Ersetzte Windows-Diagnosebuilds unter `build/validation/protocol-wrap-2026-10-26/4-crash` | 224 `.obj`-Dateien mit 52.237.964 Byte Dateigröße entfernt, ausschließlich aus `windows-asan-build/obj` und `windows-asan-build-fixed/obj`. |
| Übriger Diagnosebestand | Alle 433 Dateien vorher und nachher hashgleich, einschließlich Programme, Bibliotheken, Debug-Symbole, Logs, Fehlerbilder und Linux-Stackdiagnose. |
| Arbeitsstand | 7.641 versionierte und nicht ignorierte lokale Dateien sowie Git-Index unverändert geprüft. Bestehende lokale UI-Arbeit wurde nicht geändert oder als geprüft bewertet. |
| Neue Bildschirmbilder | 0. Dieser Auftrag lässt sich mit Text, Pfaden und Prüfsummen belegen. |

Vor dem Löschen wurden die absoluten Zielpfade, sämtliche Pfadbestandteile,
Git-Ignorierung und Dokumentationsverweise geprüft. Die ausgewählten Bäume
enthielten keine Links; unter Windows und WSL liefen keine Prozesse dieser
Prüfstände. Der aktuelle isolierte Folgebuild, andere Projekte, Abhängigkeiten,
Nutzdaten und `D:/WSL/Alpine-Dev/ext4.vhdx` blieben unberührt. Es wurden keine
Verzeichnisse pauschal geleert. Die Dateigröße ist keine Messung physisch
freigegebener Laufwerksbytes.

Der erste Planungslauf stoppte vor jeder Löschung: Windows hatte dem an Git
übergebenen Zeilenende ein CR hinzugefügt. Der Nachlauf verwendet nullterminierte
Bytepfade; danach stimmen alle 224 Ignorierungsnachweise exakt überein.

Die [kompakte maschinenlesbare Quittung](verification/storage-crash-objects-2026-10-26.json)
verknüpft Befehle, Ergebnisse und Prüfsummen. Dateigenaue Pläne und rohe Logs
liegen unter `D:/Physim/build/maintenance/2026-10-26-storage-next`. Bestehende
eingefrorene Berichte bleiben unverändert. Aus diesem Nachlauf folgt keine
neue UI-, Fokus-, Screenreader- oder Plattformabnahme.

## Weniger doppelte Nachweise

Die [Speicheranleitung](development-storage.md) verweist jetzt auch auf eine
[kleine JSON-Vorlage](templates/native-validation.json): Quellstand, Laufzeit
und Umgebung einmal pro Lauf, danach Aktion/Soll/Ist/Ergebnis pro Fall.
Markdown hält Ergebnis und Grenzen fest und verlinkt die Messwerte. Eine neue
Programmkopie oder ein weiteres Bild braucht eine zusätzliche Aussage; für
unveränderte Laufzeiten genügt der vorhandene hashgeprüfte Archivindex.

Referenzbilder, eingefrorene Nachweise und offene Fehlerbelege bleiben geschützt.
Erfolgreiche Rohbilder erst nach Abnahme und Verwendungsprüfung entfernen;
fehlende visuelle Abnahme durch Textnachweise nicht als erledigt markieren.
