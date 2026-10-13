# Globale Befehle in aufklappbaren Menüs

Entscheidung vom 13. Oktober 2026: Die oberste Leiste zeigt beschriftete Menüs
in der Reihenfolge Datei, Bearbeiten, Ansicht, Build, Hilfe, Einstellungen.
Speichern, Suchen und Build erhalten keine freien Aktionsicons im Fensterkopf.
Die Fensterknöpfe bleiben als native Fensterfunktionen rechts.

Der Nutzer wünscht Menüeinträge mit Icons, angelehnt an Visual Studio 2022.
Die vorhandenen semantischen Farbrollen werden weiterverwendet. Menüeinträge
haben eine gemeinsame Oberfläche, eine 24-Pixel-Iconspalte, einen Textbeginn
38 Pixel innerhalb der Zeile und rechtsbündige Tastenkürzel. Ihre Breite folgt
dem tatsächlich gemessenen Label und Tastenkürzel. Trennlinien beginnen nach
der Iconspalte und gruppieren verwandte Aktionen. Alle Icons sind originale,
auflösungsunabhängig gezeichnete Konturen im bestehenden Renderer.

## Interaktionszustände

| Zustand | Verhalten |
| --- | --- |
| Geschlossen | Nur der Menüname erscheint im Fensterkopf. |
| Offen / ausgewählt | Der Menüname ist hervorgehoben; andere Menüs sind unmittelbar erreichbar. |
| Hover / Tastaturfokus | Eine gemeinsame Zeilenmarkierung und bei Tastaturbedienung ein Fokusrahmen zeigen die Aktion. |
| Nicht verfügbar | Icon und Text sind gedimmt; die Aktion wird weder mit Maus noch über den Accessibility-Aufruf ausgeführt. |
| Laufender Build | Projekt bauen ist deaktiviert; die vorhandene Statusanzeige beschreibt den Job. |
| Abbruch | Escape schließt auch ein per Maus geöffnetes Menü. |
| Tastaturwechsel | F10, Pfeile, Pos1/Ende und Enter bedienen die Menüzeile. Ein globaler Ctrl-/Cmd-Shortcut schließt das Menü und führt die bestehende Aktion aus. |
| Aktivierung | Der Mausklick gehört dem Menü bis zum Ende des Frames; die darunterliegende Navigation und Panels erhalten diesen Klick nicht. |

Ein wiederverwendbarer Erkenntnisgewinn ist der letzte Punkt: In einer direkt
neu gezeichneten Oberfläche kann nach dem Schließen des Popups derselbe Klick
sonst zwei Aktionen auslösen. Der Regressionsfall wählt Neuladen über einer
Dateibaumzeile und Build über der Arbeitsbereich-Navigation. Die neue Menüaktion
darf weder ein Dokument wechseln noch den Arbeitsbereich umschalten.

## Prüfung und Grenzen

Die lokale Abnahme verwendet Windows 11 Pro, SDL 3.2.30 und MSVC 14.38.
Tatsächliche SDL-Eingaben und Screenshotprüfung ergänzen die Geometrie- und
Accessibility-Modelltests. Die konkreten Ergebnisverzeichnisse und restlichen
Prüflücken stehen im [Plattformnachweis](../platform-validation.md).
Eine Prüfung mit einem Screenreader, physischen DPI-Wechseln sowie Linux/macOS
ist für diese Änderung noch offen. Die Barrierefreiheitsnamen und Zustände der
Menübefehle werden über die vorhandenen Adapter bereitgestellt; dies ersetzt
keine assistive Abnahme.

Referenz: Microsofts [Menus and Commands for Visual Studio](https://learn.microsoft.com/en-us/visualstudio/extensibility/ux-guidelines/menus-and-commands-for-visual-studio?view=vs-2022).
Die Entscheidung, sämtliche Menüaktionen mit einem Icon zu versehen, folgt
ausdrücklich dem Nutzerauftrag.
