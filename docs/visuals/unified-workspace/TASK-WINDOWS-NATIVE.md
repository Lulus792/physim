# Eigenständige Aufgabenfenster: erster nativer Stand

Stand: 29. Oktober 2026. [Nutzerauftrag und Vertrag](../../decisions/015-independent-task-windows.md).
Der Grafikexport ist umgestellt. Die gesamte angeforderte Dialogumstellung ist
noch nicht abgeschlossen. [Kommandos, Quellen und Laufzeiten](../../verification/task-windows-2026-10-29.json).

## Grafikexport

Der Export besitzt ein eigenes SDL-/Betriebssystemfenster, einen eigenen
Nuklear-/Accessibility-Kontext und eigene Grafikressourcen. Schriftressourcen
werden über die vorhandene GL-Freigabe geteilt; es gibt keine zweite App- oder
Simulationsinstanz. Der Renderer bekommt den Dialogkontext ausdrücklich und
zeichnet ihn nach Hauptfenster und herausgelösten Bereichen. Er tauscht keine
App-/Projektkopien aus. Die Betriebssystem-Titelleiste ist der zusätzliche native
Fensterrahmen; der freigegebene Inhalt und die maximale 1080-px-Komponente bleiben
innerhalb seiner Clientfläche erhalten.

Der Export fängt ausschließlich die Eingabe seines eigenen Fensters ab. Die
bisherige Hauptfenstersperre und die Sperren für Explorer, Zeitleiste und weitere
herauszulösende Bereiche entfallen für ihn. Der Aufruf eines schon geöffneten
Exports aktiviert dessen Fenster und erhält seine Optionen und Daten. Sein
Speicherdialog erhält das Exportfenster als tatsächlichen Auftraggeber.

Der unveränderliche Snapshot und sein ursprünglicher Ausgabeordner bleiben
erhalten, wenn im Hauptfenster Auswahl, Einheiten oder Quellen wechseln.
Schließen/Escape betreffen das Exportfenster. Die Fokuskorrektur ist an einen
noch sichtbaren Auftraggeber gebunden und wird bei anderem Tastaturfokus verworfen.
Ausgabe, UI/GL-Ressourcen und Fenster werden vor den gemeinsamen Schriftressourcen
freigegeben. Die bereits besessenen Clipboard-/Speicherbytes überleben den Dialog.

## Tatsächlich ausgeführte Prüfungen

Windows 11 Pro 26200, Release: sechs Gruppen bestanden. Linux Alpine 3.24.2
unter WSL2, X11/Xvfb/Openbox, Debug mit ASan/UBSan: ebenfalls sechs Gruppen
bestanden. GUI-LSan ist wie bei der bestehenden Grafikmatrix deaktiviert.

- Der neue Ablauf prüft unterschiedliche echte HWND-/X11-Handles, unterschiedliche
  UI-/Grafikbesitzer, Navigieren im Hauptfenster, bytegleiche Exportausgabe,
  unabhängiges Resize, Escape, erneutes Öffnen und eine injizierte
  `WINDOW_CLOSE_REQUESTED`-Nachricht. Das ist kein physischer Tastatur-/Titelknopfnachweis.
- Die bestehende Exportgruppe behält PNG/SVG, Größen 1–4, Ausschnitte, drei Themes,
  schmale Fenster, große Schrift und den 2x-Komponentenstresstest. Letzterer ist
  weiterhin keine neu eingeführte globale 32-px-Schrifteinstellung.
- Ein unabhängiger Win32-CF_DIB-/X11-Clipboard-Client liest die Bildpixel nach
  Freigabe von Dialog und Raster. Gespeicherte Dateien werden mit dem kanonischen
  Vorschauraster verglichen; vorhandene Speicher-/Abbruch-/Fehlerwege bleiben geprüft.
- Vier Vergleichsgruppen behalten acht Kurven, eingefrorene Einheiten/Ausschnitte,
  geänderte Live-Auswahl und entfernte ursprüngliche Laufdateien bei geöffnetem Export.
- Eine tatsächlich kompilierte Windows-Gegenvariante verbraucht wieder die Eingabe
  anderer Fenster. Derselbe neue Ablauf scheitert dann beim Wechsel des
  Hauptbereichs in Schritt 4002. Die unveränderte Produktions-EXE besteht ihn.
  Der erste Gegenprobenaufbau wurde vor dem Kompilieren vom Builder abgewiesen,
  weil die generierte Quelle außerhalb seines Quell-/Buildordners lag; die
  korrigierte Quelle liegt im lokalen Buildordner. Dies war kein App-Fehler.

Die alten Gesten adressieren jetzt ausdrücklich das Exportfenster; Screenshot-
Readback erfolgt nach dessen eigenem Zeichnen. Neue Clientgrößen für erstmals
geöffnete Fenster werden ausdrücklich geprüft. Die inneren CSS-Maße, Konturen,
Originalausgaben und Clipboard-Erwartungen wurden erhalten.

## Noch umzusetzen und abzunehmen

| Aufgabe | Stand |
| --- | --- |
| Neues Projekt B | Eigene Fensterressourcen, globale Sperren/Schließmutationen und Rückkehr noch umstellen. Formularwerte erhalten. |
| Datei-/Ordnerwahl A | Eigene Fenster pro Auftraggeber und Anfragegeneration, einschließlich Unteraufruf aus B. Späte Antworten dürfen kein anderes Ziel verändern. |
| Workspace, Layouts, Kanaleinheiten, Laufdetails | Aufgabenoberflächen inventarisieren und ebenfalls unabhängig hosten. |
| Bestätigungen, Wiederherstellung und weitere Dialoge | Fachaktion schützen, Hauptfenster nicht pauschal sperren; Schließen und veraltete Ziele gesondert prüfen. |
| Plattform-/Assistive-Abnahme | Unabhängige native UIA-/AT-SPI-Aufgaben für das neue Exportfenster, echte Screenreader, weitere DPI-/Monitorzustände und macOS bleiben offen. |

Die alte Modalität des Projektdialogs ist damit ausdrücklich noch nicht beseitigt.
Diese offenen Punkte sind keine Behauptung technischer Unmöglichkeit. Die
[freigegebene Markierung A2](../selection-options/APPROVED.md) und die
[gesamte Referenzmatrix](NATIVE-FIDELITY.md) bleiben zusätzliche offene native Arbeit.
