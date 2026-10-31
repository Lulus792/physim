# macOS: Einstellungenfenster und Menüanschluss

Quellstand `479a89549c28f02e59226f922c3f200650771611`, gehostete GitHub-Runner.
Der Nutzer-Mac bleibt ausgeschaltet; kein Zugriff darauf.

Apple Silicon: Settings-/Typografie-Lauf `41170676474`, Job `124447773891`,
terminal erfolgreich, **9/9 registrierte Fälle** über vier Aufrufe. Das Artefakt
wurde tatsächlich heruntergeladen, sein SHA-256 gegen den GitHub-Digest geprüft
und jede Ergebnisdatei gelesen. [Einzelfälle und Kommandos](settings-a-macos-arm-2026-10-31.json).
Der Intel-Settings-Job und die separate ARM-Menüregression waren beim Anschluss
noch eingereiht; das ist keine gemeinsame macOS-Vollabnahme.

Die separate Intel-Menüregression `41170676487`, Job `124447773804`, ist
**fehlgeschlagen, 4/7**, Ergebnis `run-h_tnqynh`. Menüadapter, App-Start und beide
Tastaturmenüfälle bestanden. `toolbar_small`, `toolbar_large` und
`toolbar_input_isolation` scheitern identisch in Stufe 12: Nach Öffnen aus
Auswerten und Escape im eigenen Einstellungsfenster bleibt korrekt `tab=2`,
die alte Fixture verlangt aber `tab=0`. Der vorige Umbau hatte nur die Öffnungs-
und Escape-Stufe auf das eigene Fenster portiert.

Die Korrektur verlangt ausdrücklich den erhaltenen `settings_previous_tab`,
wartet auf tatsächlichen Fokus im Hauptfenster und ruft erst dann das echte
AppKit-Hilfemenü auf. Maximieren, Wiederherstellen, Minimieren, native Titelleiste
und echtes Schließen bleiben danach vollständig Teil desselben Ablaufs.
Keine Produktänderung und keine gelockerte allgemeine Workspace-Erwartung.
Der neue Mac-Nachlauf muss dies tatsächlich bestätigen; bloßer Fixture-Code
oder die vorherigen vier Erfolge reichen nicht.

Isolierter Anschluss `mac-settings-toolbar-return`: gegenüber
`settings-a-window-visual` genau diese eine Compilerdatei geändert. Die
Windows-/Linux-Runtimes sind unverändert und alle referenzierten Archivbytes
zurückgelesen; **keine neue lokale Ausführung dieses Apple-only-Zweigs behaupten**.
Die parallel begonnene Einheitenfenster-Implementierung im Arbeitsverzeichnis
ist ungetestet und gehört weder zu diesem Anschluss noch zu dessen Commit.
