# Nativer Kanalkopf und Kurvendarstellung

Folgestand: Der [Abgleich der ruhigen Diagrammköpfe](chart-captions-2026-10-24/README.md) entfernt den unten noch beschriebenen zusätzlichen Chevron und die Hoverfläche. Der folgende Bericht dokumentiert seinen ursprünglichen Quellenstand.

Fortsetzung nach `9704967366e2b44b8d03c0aa3d08356bc813e4bc`. Der gesamte
Oberflächenauftrag bleibt offen. Dieser Schritt behandelt den Kanalkopf,
Farben, Strichstärke und den Platzbedarf der kompakten Messkurven.

## Referenz und Umsetzung

Maßgeblich sind `chart-label`, `chart-grid`, `curve`, `metric-line` und die
Medienregeln in `simulation-designs/simulation.css` sowie die gemeinsame Palette
aus `unified-workspace/bridge.js`. Die Einzelvorlage hat andere ursprüngliche
Akzentwerte; die gemeinsame Shell überschreibt sie. Eine neue Browserausführung
ist damit nicht behauptet.

Der reale Kanalname steht nun links, seine tatsächliche Anzeigeeinheit rechts.
Die bisherigen eckigen Klammern und der gemeinsame linke Textblock entfallen
in den Kurvenköpfen. Beide Texte verwenden 10-px-Schrift, die Einheit die gedämpfte
Farbrolle. Lange Metadaten werden vollständig umbrochen. Die Kanalauswahl bleibt
ein echter nativer Button mit Popup und Tastaturweg; ein kleiner Chevron weist
auf diese zusätzliche Funktion hin. Die illustrative Vorlage zeigt hier nur
zwei Textspannen. Diese Funktionsanpassung ist keine technische Unmöglichkeit,
den Ruhezustand noch enger an die Vorlage anzugleichen.

Kurve und Unterstrich des ersten Slots verwenden die gemeinsame Akzentrolle
`#8abaff` / `#17569e`. Der zweite Slot behält die bereits passenden Vektorfarben
`#f5a657` / `#914800`. Das horizontale Gitter verwendet wie die Vorlage die
Grenzlinienfarbe. Hoher Kontrast behält die unterscheidbare native Kurvenpalette;
die konkrete Browserdarstellung in erzwungenen Farben hängt vom OS-Thema ab.

Der Platzbedarf berücksichtigt Schriftgröße, vollständigen Kanalkopf, 9 px
Abstand, Zeichenfläche und einen gegebenenfalls umbrochenen Offsethinweis.
Die Mindesthöhe wächst mit der App-Schrift. Gestapelte Kurven erhalten ihren
Abstand und eigene notwendige Höhen. Der Scrollweg bleibt der bestehende
Messbereich. Bei zu wenig Platz für drei Zeitmarken entfällt nur die mittlere;
Endpunkte und Einheiten bleiben erhalten. Leere, noch einzelne, vollständig
ungültige und nicht darstellbare Messungen erhalten unterschiedliche, wahre
Hinweise. Eine einzelne Probe wird nicht als schon fertige Kurve angekündigt.

Beim Wechsel eines Kanals bleiben Karten, gerade gezeichneter Kopf und Kurve
für ein vollständiges Bild an denselben vorbereiteten Daten gebunden. Die neue
Wahl gilt beim nächsten vollständigen Bild. Der bisher mögliche Wechsel nur
der Kurvendaten am Ende desselben Zeichnungsdurchlaufs entfällt. Dieser Befund
folgt aus der geprüften Eingabe-/Zeichenreihenfolge; eine eigenständige alte
Frame-Aufnahme dieses Übergangs wird nicht behauptet.

## Nachweise und Grenzen

Die vorhandene Probe baut das echte C-Pendel und prüft reale Tastatureingabe,
Kanalwahl, Anzeigeumrechnung, vollständige Sichtbarkeit fokussierter Köpfe und
ihre tatsächlichen Glyphen. Zusätzlich wird die Kanalbindung in jedem
gezeichneten Bild kontrolliert. Verdeckte, nicht fokussierte Feldteile werden
anhand der tatsächlichen Clipgrenze unterschieden, nicht als sichtbarer Text
behauptet.

Die zusätzliche native Komponententafel verwendet ausdrücklich kontrollierte
Daten: null/ein Punkt, ungültige Werte, Lücken, extreme Offsets, lange Metadaten
und nicht darstellbare Einheiten. Es sind keine vom Pendel gemessenen Ausfälle.
Quelle, reale Samples, Szene, Zeit und Messdatei des pausierten Projekts bleiben
dabei erhalten. Sieben Fälle in sechs Darstellungen und zwei Schriftquellen
ergeben 84 tatsächliche PNG-/Text-/Geometrieprüfungen. Der Prüfer verlangt zwei
getrennte Segmente und eine leere Mitte bei der Lücke; Offsets dürfen die
Zeichenfläche nicht überlagern.

Stand 1 besteht beide Windows-/Linux-Builds und die beiden nativen Systemschrift-
Bedienfolgen. Die anschließenden Bildprüfungen scheitern: Eine nicht fokussierte
Zeile liegt nach Rückscrollen nur teilweise im Clip; die zusätzliche Prüftafel
liegt zudem hinter dem Workspace. Stand 2 erfasst den echten Clip und hebt die
Tafel an. Der reale Messbereichlauf besteht dann mit beiden Schriftquellen.
Die Tafel wird jedoch nach dem Workspace noch mit der allgemeinen App-Palette
gezeichnet; die neue strenge Farbprüfung verwirft sie. Stand 3 gibt der Tafel
denselben Palette-Kontext wie dem Produkt und besteht sämtliche 84 Fälle.
Stand 2 → 3 ändert nur diese ausschließlich im Testmodus benutzte Datei.

Die unveränderte native Vorgängeraufnahme wird von der neuen Akzentprüfung
tatsächlich verworfen. Frühe verdeckte/falsch gefärbte Prüfbilder und ihre
Quellstände bleiben als Gegenbelege erhalten. Aktuelle abgeschlossene Läufe
stehen in den [eingefrorenen Nachweisen](curve-heads-2026-10-24/README.md).

Die aktive Physim-Erweiterung `NK_INCLUDE_FLOAT_COMMANDS` erhält bereits
Subpixelkoordinaten und Strichstärken. Eine zunächst aus dem unveränderten
Nuklear-Standard abgeleitete Ganzzahlgrenze trifft auf diesen Build nicht zu.
Es ist dafür keine Rendereränderung erforderlich. Unterschiedliche
Systemschriften und deren Rasterung schließen weiterhin eine pauschale
Pixelgleichheit zwischen Windows, Linux und Browser aus. Das ersetzt keinen
noch ausstehenden Abgleich der übrigen sichtbaren Komponenten.

Stand 5 entfernt deshalb die unnötige Strich- und Eckenrundung auf Ganzzahlen.
Die tatsächlichen Kurvenbefehle enthalten bei den geprüften App-Schriftgrößen
2 / 2,25 / 2,50 / 2,75 logische Pixel. Beide finalen Darstellungsgruppen bestehen
mit beiden Schriftquellen: 60 echte Messbereichbilder und 84 kontrollierte
Komponentenbilder, zusammen 144. Der neue Befehlstest würde eine erneute
Ganzzahlrundung verwerfen.

Stand 6 und 7 bestehen sechs weitere ursprüngliche Prüfgruppen gegen genau
dieselben Binärdateien und denselben Quellgraphen. Zusammen sind acht
unterschiedliche aktuelle Gruppen bestanden: Messbereich, Kurvenrandfälle,
normale/große externe Fenster, Anzeigeeinheiten, reale Statusmasken, Messlabor
und Szenenrahmen. Die beiden Nachläufe verwenden getrennte X-Displays und
schreibgeschützte SDK-Anbieter. Kompilierung ist gesperrt; Quell-, Font- und
Binärhashes werden vorher/nachher geprüft. Windows-Releasebuild 5 ist erfolgreich.
Diese Belege betreffen vorhandene Lücken in der dargestellten Vorschau; die
Verdünnung sehr langer Live-Historien muss gesondert auf Lückenerhalt geprüft
werden. Es wird daraus kein vollständiger Langzeitnachweis abgeleitet.

Aktuelle Windows-GUI, praktische Screenreader-Aufgaben, physische DPI-/Monitor-
Wechsel und macOS bleiben offen. Drei separat erhaltene fremde AT-SPI-Dateien
sind im lokalen Linux-Kompiliergraphen enthalten; dies ist kein Clean-Checkout-
Nachweis. Die neuen Auswahlfelder behalten den vorhandenen nativen Button-/
Popup-Vertrag; ein vollständiger plattformübergreifender ComboBox-Vertrag wird
hierdurch nicht nachgewiesen.

## Speicherpflege

Aus dem vollständig erfolgreichen, inzwischen ersetzten Nachlauf
`run-egt4z71n` wurden 246 automatisch erzeugte PNG-/BMP-Rohbilder entfernt,
561.180.705 Byte (rund 535 MiB). Die eingefrorenen Messbereich-Belege wurden
danach erneut erfolgreich geprüft. Ausgewählte Referenzen, Fehlerbelege,
Quellen, Testprojekte, Messdateien und Logs bleiben erhalten. Die aktuellen
Läufe liegen in anderen Verzeichnissen.
