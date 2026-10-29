# Dateisicherheit im Design B2

Status: **vom Nutzer ausdrücklich freigegeben, 29. Oktober 2026**.

Der Nutzer hat B2 mit einer gemeinsamen Dateiansicht wie bei einem Git-Diff
für **Speichern, Dateikonflikte und Wiederherstellung** angenommen und die
Übernahme ins fertige Design sowie die genaue Umsetzung im anderen Chat
beauftragt. Die vorherige Einstufung „getrennter Entwurf / Freigabe offen“
ist aufgehoben. Keine weitere Variantenentscheidung vorschalten.

Maßgeblich sind die [verbindliche Spezifikation](../visuals/file-safety-options/APPROVED.md),
die [festgelegte Referenz](../visuals/file-safety-options/index.html?approved=1&topic=save)
und die sechs [ausgewählten B2-Referenzbilder](../visuals/file-safety-options/review/b2/VALIDATION.md).
A/B/C bleiben Vergleichshistorie. B2 ergänzt die bereits gewählten Bereiche;
Projektdialog B und Explorer A werden dadurch nicht umbenannt oder ersetzt.

Links stehen betroffene Dateien, rechts eine gemeinsame Änderungsansicht mit
Minus-/Pluszeilen, unverändertem Kontext, alten/neuen Zeilennummern und echten
Änderungszählern. Die Quelldatei beziehungsweise aktuelle externe Fassung ist
die Minus-Seite, Editorentwurf oder Sicherung die Plus-Seite. Diese Darstellung
führt keine Inhalte automatisch zusammen und ist keine Git-Integration.

Die Freigabe umfasst den zusammenhängenden Ablauf und seine gezeigten Fehler-
und Abbruchzustände: Speichern je Datei mit erhaltenen Teilerfolgen, Bestätigung
vor Verwerfen/Ersetzen, Konflikt bei erneuter externer Änderung und getrenntes
Laden einer Sicherung mit erst anschließendem bewussten Speichern. Die in der
Browserdemo nur vorgemerkte Kopie braucht nativ einen echten Speicherablauf.

Vorhandene Autosave-, Backup-, Rechte-, UTF-8- und Fensterverträge erhalten.
Insbesondere [ADR 015](015-independent-task-windows.md) gilt: eigene
OS-Aufgabenfenster mit weiter bedienbarem Hauptfenster, keine globale Sperre
aus der HTML-Testhülle übernehmen. [A2](016-selected-marking-a2.md),
[Grafikexport](010-analysis-graphic-export.md) und
[Bereichsauswahl](011-selected-workspace-additions.md) bleiben verbindlich.

Die native Arbeit wird am aktuellen lokalen Stand fortgesetzt; HTML/JavaScript
sind Gestaltungsreferenz, keine WebView-Vorgabe. Die Browserprüfungen belegen
keine fertigen nativen Dateioperationen oder Plattformabnahme.
Die [Gesamtübergabe](../visuals/unified-workspace/NEXT-CHAT.md) und die
[UI-Designliste](../visuals/unified-workspace/UI-DESIGN-BACKLOG.md) sind aktualisiert.
