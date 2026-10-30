# Eigenständige Aufgabenfenster: native Teilstände

Stand: 30. Oktober 2026. [Nutzerauftrag und Vertrag](../../decisions/015-independent-task-windows.md).
Grafikexport, Projektanlage B und Datei-/Ordnerwahl A besitzen eigene Betriebssystemfenster. Die gesamte
angeforderte Dialogumstellung ist noch nicht abgeschlossen. Nachweise:
[Grafikexport](../../verification/task-windows-2026-10-29.json) und
[Projektanlage B](../../verification/project-task-window-2026-10-30.json) sowie
[Datei-/Ordnerwahl A](../../verification/picker-task-window-2026-10-30.json).

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

## Rückgaben an den richtigen Auftrag

Die Datei-/Ordnerantworten verwenden jetzt einen CPU-Auftrag mit eigenem Ziel
und monotoner Anfrageversion. Ein nativer Callback hält nur diesen Auftrag,
keinen App-, Fenster- oder UI-Zeiger. Er kopiert seine Antwort, veröffentlicht
sie atomar und gibt seinen eigenen Bezug frei. Schließen, Ersetzen und Shutdown
entfernen den Besitzerbezug; auch ein später Callback bleibt dadurch gültig,
ohne einen inzwischen neu geöffneten Dialog verändern zu können.

Der UI-Thread übernimmt ausschließlich aktuelle Antworten. Dateiarbeiter werden
vor der Übernahme abgefragt, damit Schließen der Auswahl und Übernahme im
Auftraggeber vor demselben nächsten UI-Frame geschehen. Ein erfolgreicher
Projekt-/Workspace-Wechsel verwirft die alten Aufträge zum Hinzufügen von
Dateien/Ordnern, Importieren von Läufen und Fortsetzen einer Serie. Der unabhängige
Zielordnerauftrag der Projektanlage bleibt erhalten.

Die [Prüfaufzeichnung](../../verification/dialog-requests-2026-10-29.json)
trennt Thread-/Lebenszeitprüfungen, tatsächliche B/A-Bedienabläufe und negative
Gegenproben. Diese Zuordnung ist die Grundlage für mehrere unabhängige
Auftraggeber. Der damalige Teilnachweis stellt die Fenster allein noch nicht um;
Projektanlage B und anschließend Picker A sind inzwischen in den folgenden Schritten umgesetzt.

## Projektanlage B

B besitzt ein eigenes SDL-/OS-Fenster, eigene UI-/Accessibility- und Grafikressourcen.
Seine Helfer erhalten ausdrücklich den Formularkontext. Gemeinsame unveränderliche
Schriften bleiben bis nach dem Schließen der Aufgabenfenster erhalten. Hauptfenster
und herausgelöste Bereiche behalten ihre Layouts und Eingabe; die alten globalen
B-Sperren und Schließmutationen beim Navigieren entfallen.

Ein erneuter Aufruf aktiviert das bestehende Fenster und erhält Namen, Zielordner,
Vorlage, Sprachen und Schritt. Ein Projekt- oder Workspace-Wechsel im Hauptfenster
verwirft diese Angaben nicht. Nur die erfolgreiche Anlage aus diesem Formular
schließt es. Ein Erstellungsfehler erhält Workspace, vorherigen Projekteingabepfad,
Beispielauswahl und vorhandene Dateien. Escape/OS-Schließen betreffen B; die native
Freigabe erfolgt außerhalb des Zeichnens. Die Fokuskorrektur prüft eine noch lebende
SDL-Fenster-ID und übersteuert keinen anderen tatsächlich fokussierten Auftraggeber.
Hauptprojektwechsel verwerfen jetzt sechs alte Hauptkontext-Anfragerouten;
der unabhängige Zielordnerauftrag aus B bleibt erhalten.

Die kompakten Umbrüche folgen der freigegebenen CSS-Grenze von 560 logischen
Pixeln: Kopf, Schrittanzeige, einspaltige Felder/Karten, Ort und Fußaktionen.
Textmessungen bestimmen auch mehrzeilige Hinweise, Karten und Fehlerrahmen.
Bei laufender Schriftänderung bleibt die fokussierte Aktion sichtbar; passt der
Inhalt wieder vollständig, entfällt der alte Scrollversatz.

Die [Prüfaufzeichnung](../../verification/project-task-window-2026-10-30.json)
trennt die Quellstände und ihre tatsächlichen Ergebnisse:

- Windows Release und Linux X11/Xvfb/Openbox mit ASan/UBSan bestehen jeweils
  68 tatsächliche Projektanlagen mit Bauen, Aufzeichnen/Importieren und Auswerten:
  alle 32 Vorlagen-/Sprachkombinationen sowie zwei Analyseprojekte bei 16 und 22 px.
- Unabhängige UIA-/AT-SPI-Clients wählen alle acht Radiokarten, machen verdeckte
  Karten sichtbar und beobachten die weiter aktivierte Suche im Hauptfenster.
- Der Abschlussablauf prüft sechs Themes/Größen, unterschiedliche HWND-/X11-Handles,
  echte geroutete Formular-/Navigationsgesten, Wiederaufruf, eigenes Resize,
  Schriftwechsel, Projekt-/Workspace-Wechsel, Abbruch eines Kindes und Shutdown.
  Die Domänenwechsel selbst verwenden die normalen App-Einstiegspunkte direkt.
- Ein tatsächlicher Anlagefehler mit vorhandener Quelldatei prüft Dateierhalt,
  unveränderten Workspace und Text innerhalb des Fehlerrahmens, auch bei 320 px
  Breite und 22 px Schrift. Bilder ergänzen diese Text-/Geometrienachweise gezielt.
- Drei kompilierte Windows-Gegenvarianten erkennen fremd verbrauchte Eingabe,
  fehlenden Fußzeilenumbruch und verlorene Sichtbarkeit nach Schriftwechsel.
  Die unveränderten Produktionsprogramme bestehen jeweils denselben Ablauf.
  Das [Prüfwerkzeug](../../../tools/check_project_window_counterexample.py) ist versioniert.

Ein Linux-Analyseablauf erreichte zunächst während fortschreitender SDK-Kompilation
seine 120-s-Grenze. Der separate Nachlauf mit unverändertem Programm und Prüfer
besteht alle vier Anlage-/Auswertungs-/Wiederöffnungsprüfungen bei derselben Grenze.
Dies belegt keine behobene Produktursache oder allgemeine Leistungszusage.
Der neue Fehlerprüffall verglich zunächst ein frisch geöffnetes Standardfenster mit
740 px; der gezielte Nachlauf setzt dessen beabsichtigte Größe ausdrücklich.
Die ursprünglichen Inhalts-, Erhaltungs- und Geometrieprüfungen bleiben bestehen.

Dieser eingefrorene B-Nachweis entstand noch mit A als Overlay. Der folgende
A-Teilstand ersetzt diese damalige Einschränkung; der historische Nachweis bleibt erhalten.

## Datei-/Ordnerwahl A

Jeder Auswahlauftrag besitzt ein eigenes SDL-/OS-Fenster, eigene UI-/Grafikressourcen
und einen eigenen Dateiarbeiter mit Navigation, Suchtext, Vorschau und Editzuständen.
Die freie Dateienansicht im Hauptfenster besitzt einen separaten Kontext. Die
globale Hauptfenstersperre während einer Auswahl entfällt. Zwei unabhängige
Auswahlfenster können gleichzeitig geöffnet bleiben, etwa Hauptfenster und
Zielordnerwahl aus B. Nur B wartet während seiner eigenen Unterwahl auf deren Ergebnis.

Die Zuordnung verwendet weiterhin Auftrag und Anfragegeneration. Ein erneuter
Aufruf aktiviert das vorhandene Fenster und erhält die Eingaben. Ein Hauptprojektwechsel
verwirft nur seine eigenen Auswahlaufträge; der Zielordnerauftrag aus B bleibt erhalten.
Schließen von B beendet nur dessen Unterwahl. Verspätete Antworten und veraltete
Bestätigungen der eingebetteten Ansicht ändern kein inzwischen anderes Ziel.

Fensteranlage und Freigabe erfolgen außerhalb des Zeichnens. Die Rückgabe prüft
die noch existierende Auftraggeber-ID und überschreibt keinen fremden Tastaturfokus.
Die Standard-Titelleiste kommt als OS-Rahmen hinzu; die freigegebene Komponente
bleibt in der Clientfläche, maximal 1180 logische Pixel breit und bei breiteren
Fenstern zentriert. Bei 560 logischen Pixeln greifen die kompakten Umbrüche;
Orte, Kopf und lange Ordner-Auswahlaktion bleiben bei 320 Pixeln und 22-Pixel-Schrift erreichbar.

Die tatsächlichen Windows-/Linux-Abläufe prüfen sechs Theme-/Größenzustände,
verschiedene HWND-/X11-Handles, Navigation und tatsächliches Bearbeiten/Speichern
im Hauptfenster bei zwei geöffneten Auswahlen. Ein herausgelöstes Protokollfenster
nimmt währenddessen eigene Texteingabe an und kehrt beim Schließen zurück.
Geprüft werden außerdem echte Hauptordnerübernahme, getrennte B-Rückgabe,
Wiederaufruf, Schriftwechsel, schmale/breite Fenster, Quellschließen und Shutdown.
Externe UIA-/AT-SPI-Clients prüfen die eigenen Auswahlfenster und die freie Dateienansicht.
Die genaue Quell-/Laufzuordnung, Fehlversuche und Gegenproben stehen im A-Nachweis.

Eine kompilierte Windows-Gegenvariante, die wieder Eingaben des Hauptfensters
verschluckt, scheitert an der tatsächlichen Bereichsumschaltung in Schritt 10.
Eine zweite Gegenvariante verschiebt nur den Orte-Text um 2,8 Pixel; derselbe
unabhängige CSS-Prüfer weist die falsche Einrückung zurück. Beide unveränderten
Kontrollprogramme bestehen die jeweils gleichen sechs Zustände.

Der nachfolgende [Dateizeilen-/Kachelabgleich](EXPLORER-LAYOUT-NATIVE.md) übernimmt
inzwischen Listen-/Kachelmaße und mehrere Metadaten-/Fußschriftrollen. Der
vollständige visuelle Abgleich von A bleibt offen, einschließlich weiterer
Kopftypografie, Informationszustände und unabhängiger API-Prosa-/Tabellenmetriken.
Insbesondere besitzt der Dateiname ausdrücklich 1,6 Zeilenhöhe; andere Texte
erben 1,55. Ein pauschaler Austausch wäre falsch.
Dieser Fensterteilstand bestätigt daher keine vollständige 1:1-Umsetzung.
Physische Eingabe, echte Screenreader, weitere DPI-/Monitorzustände und macOS
bleiben gesonderte Abnahme.

## Noch umzusetzen und abzunehmen

| Aufgabe | Stand |
| --- | --- |
| Neues Projekt B | Eigenes OS-Fenster und Windows-/Linux-Teilabnahme umgesetzt; Unterwahl A ebenfalls unabhängig. |
| Datei-/Ordnerwahl A | Eigene Fenster pro Auftraggeber und Anfragegeneration umgesetzt, einschließlich Unteraufruf aus B; vollständiger Vorlagenabgleich bleibt offen. |
| Workspace, Layouts, Kanaleinheiten, Laufdetails | Aufgabenoberflächen inventarisieren und ebenfalls unabhängig hosten. |
| Bestätigungen, Wiederherstellung und weitere Dialoge | Fachaktion schützen, Hauptfenster nicht pauschal sperren; Schließen und veraltete Ziele gesondert prüfen. |
| Plattform-/Assistive-Abnahme | Unabhängige native UIA-/AT-SPI-Aufgaben für das neue Exportfenster, echte Screenreader, weitere DPI-/Monitorzustände und macOS bleiben offen. |

Diese offenen Punkte sind keine Behauptung technischer Unmöglichkeit. Die
[freigegebene Markierung A2](../selection-options/APPROVED.md) und die
[gesamte Referenzmatrix](NATIVE-FIDELITY.md) bleiben zusätzliche offene native Arbeit.
