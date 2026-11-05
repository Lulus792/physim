# Parameterhilfe: Scrollgriff bedienbar machen

Fortsetzung des [Umbruch- und Scrolllayouts](signature-wrap-2026-11-05.md),
5. November 2026. Die Gestaltung bleibt erhalten.

Der sichtbare Scrollgriff ließ sich zunächst nicht ziehen: Die innere Nuklear-
Gruppe erbte den Nur-Lesen-Zustand der Hintergrundkarte. Die Freigabe gilt jetzt
nur während der Verarbeitung dieser Scrollgruppe. Anschließend gelten wieder
der vorherige Kartenstatus und der Editorfokus.

Die native Regression findet den Griff anhand seiner tatsächlichen Zeichenbefehle
und führt Drücken, Bewegen und Loslassen über SDL aus. Der alte Stand scheitert
unter Windows bei Schritt 33: gültiger aktueller Editor, echter Fensterfokus,
aber Scrollwert weiterhin null. Mit der Korrektur bewegt derselbe Ablauf den
Inhalt, während Hinweis, Cursor, Editorscroll und Zeilenplan erhalten bleiben.
Er prüft C und Physim und anschließend erneut manuelle Parameterhilfe,
Vorschlagsliste, Nullparameter und Fehlerfarbe.

Ein weiterer Linux-Fehlversuch zeigte einen zweiten echten Produktfehler.
Die Vermutung eines zu frühen Testinputs bestätigte sich nicht; ein zusätzlicher
Warteschritt löste ihn nicht. Der Debugger erfasste alle sechs Textbytes in
`nk_edit_buffer`, dort aber ein Nur-Lesen-Fenster und anschließend `in=NULL`.
Ein Hardware-Watchpoint bewies den Auslöser: `completion_auto_update` ruft
`completion_open_kind` und darin `completion_close` auf. Dieses Zurücksetzen
löscht auch den Tastaturbesitzer des Editors. Im Folgeframe bleibt intern der
Kopfbereich aktiv; die ankommenden Zeichen werden nicht verarbeitet.

Das Auffrischen von Vorschlägen erhält jetzt den bereits vorhandenen selben
Editorbesitzer, auch bei null Treffern. Tatsächliche Besitzerwechsel und die
übrigen Schließwege behalten ihren Vertrag. Der Test erzwingt die automatische
Abfrage in allen drei C-/Physim-Editorzielen und verlangt erhaltenen Besitz
sowie die anschließend tatsächlich eingefügten Zeichen. Die fünf bestehenden
GUI-Regressionsgruppen werden wegen dieser gemeinsamen Produktänderung mitgeprüft.

Das Testsetup fordert außerdem einmal Fensterfokus über `SDL_RaiseWindow` an
und wartet bis zu 30 Sekunden auf `SDL_GetKeyboardFocus`. Ein abgelehnter Fokuswunsch bleibt ein
Fehler; synthetische Fokusereignisse oder ein umgangener Anzeige-Guard werden
nicht verwendet. Frühere Windows-Buildläufe erhielten keinen Fokus; ein direkter
Nachlauf derselben damaligen Binärdatei bestand die vollständige Drag-Aufgabe.
Diese Zwischenstände bleiben von der endgültigen Sechs-Gruppen-Prüfung getrennt.

Kommandos, Quellstände, Originalfehler, Programmhashes und endgültige Ergebnisse
stehen im [strukturierten Nachweis](signature-thumb-2026-11-05.json).
Die Bilder werden mit den bereits geöffneten Vergleichsbildern auf exakte
Bytegleichheit geprüft; abweichende Zustände gesondert ansehen. Ein gezieltes
Bild des ursprünglichen Drag-Fehlers bleibt erhalten. Die Mac-CI lädt zusätzlich
nur bei einem solchen Fehler das zugehörige Fehlerbild hoch.

Der [Mac-Nachlauf zum vorherigen Umbruchstand](signature-wrap-macos-first-2026-11-05.json)
ist abgeschlossen: ARM und Intel jeweils 11/11 Daten und 8/9 GUI. Beide
Original-ZIPs und alle Einträge sind geprüft. Die neue Diagnose belegt auf
beiden Hosts echten OS-Fokus null, gültige Quelle und deshalb pausierten HUD.
Die neue Fokusvorbereitung benötigt ihren eigenen gehosteten Nachlauf; dessen
Erfolg wird nicht vorweggenommen. Der private Nutzer-Mac bleibt ausgeschaltet.

Dieser Schritt prüft Ziehen innerhalb der Scrollspur. Drag über die Karten-
oder Fenstergrenze, ausgegliederter Workspace, echte Fokuswechsel und
Screenreader-/AX-Aufgaben bleiben offen. Die vollständige freigegebene Oberfläche,
physische DPI-/IME-Abnahme und weitere Signaturverträge bleiben ebenfalls Teil
des Auftrags; dieser Nachweis ist keine volle Punktzahl.
