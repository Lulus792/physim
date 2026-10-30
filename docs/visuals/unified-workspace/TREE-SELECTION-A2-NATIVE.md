# A2 im Workspace- und Szenenbaum

Der native Baumrenderer übernimmt die [freigegebene Markierung A2](../selection-options/APPROVED.md):
transparente Fläche, 3-px-Kante und Häkchen. Schrift-, Datei-, Typ- und
Statusfarben bleiben erhalten. Tastaturfokus verwendet eine eigene gestrichelte
Kontur in der gemeinsamen Fokusfarbe.

## Umsetzung und Bedienung

Im Workspace bleibt die vorhandene Hierarchie mit Pfadlinien und Aufklappzeichen
erhalten. Eine zusätzliche Auswahlspalte beansprucht 20 px plus 9 px Abstand;
Namen werden mit der verbleibenden Breite gemessen und umbrechen vollständig.
Dateisymbole behalten ihre eigene Rolle. Ordner dienen weiterhin dem Auf- und
Zuklappen; ein Auswahlkästchen erscheint bei den tatsächlich auswählbaren Dateien.

Im Szenenbaum sind Auswahl, Gruppen-Aufklappen und Sichtbarkeit getrennte Ziele.
Das Häkchen löst dieselbe Objektauswahl aus wie die bestehende Zeile und erzeugt
keinen zusätzlichen Tastaturstopp. Typ-/Gruppensymbole bleiben erhalten;
zugängliche Zielnamen enthalten weiterhin die echte Objektidentität. Der gemeinsame
Renderer gilt auch für den bisherigen erweiterten Szenenbaum-Aufruf.

Bei schmalen Seitenleisten und großer Schrift umbrechen die Bedienelemente und
Beschriftungen. Bei 200% Schrift in einer 150-px-Seitenleiste passen Auswahl,
Aufklappzeichen, Typsymbol und Auge samt Abständen nicht nebeneinander. Deshalb
erhalten Aufklappzeichen und Typsymbol dort eine zweite Kontrollzeile; der Name
steht darunter. Diese Anpassung verhindert Überlappung und erhält alle Funktionen.

Ein Klick zum Schließen eines offenen Anwendungsmenüs ändert die Auswahl nicht.
Die bestehende Einzelauswahl bleibt die Grundlage dieses Umsetzungsschritts.
Mehrfachauswahl, Kontextmenüs A und die A2-Konturen/Griffe der 3D-Szene sind
weitere verbindliche Aufgaben.

## Gezielte Prüfung

Die native Gruppe `tree_selection_a2` prüft Workspace und Szenenbaum mit dunklem
und hellem Theme, hohem Kontrast sowie 22- und 32-px-Schrift. Die Fixture verwendet
echte Dateien beziehungsweise eine ausdrücklich gesetzte kleine Testszene.
Zeilen werden über echte SDL-Radereignisse sichtbar gemacht. Gemessen werden
die tatsächlichen Framebuffer-Pixel, Originaltext-/Farb-/Geometriebefehle und
Glyphenbreiten gegenüber dem realen Clip. Auswahl, Ordner-/Gruppen-Aufklappen,
Sichtbarkeit und Menüschluss werden mit Zeigerereignissen ausgeführt; Quelltext,
Szenendefinition und Kamera bleiben unverändert. Unter Linux kommt der tatsächliche
Host-/Tastaturfokus hinzu. Die Windows-Fixture behauptet keinen entsprechenden
Fokusnachweis.

32 px sind eine ausdrücklich gesetzte Layoutbelastung der Baumrenderer. Die
vorhandenen Einstellungen bieten 16 bis 22 px; die Probe ist weder eine neue
32-px-Einstellungsoption noch eine praktische OS-/Monitor-Abnahme bei 200% DPI.
Der Prüfer verlangt den tatsächlichen Skalierungswert, damit ein abgewiesener
32-px-Wunsch nicht versehentlich als bestandene 200%-Probe zählt.

Der vorhandene `tree_design`-Ablauf bleibt die separate Prüfung mit einem echten
kompilierten Pendellauf, sechs Darstellungsvarianten, System-/Paketfont, Auswahl,
Elternsichtbarkeit, Filtern, Kamera und erhaltenem Quell-/Laufbestand.

Der [kompakte Nachweis](../../verification/tree-selection-a2-2026-10-30.json)
belegt je zehn bestandene Zustände unter Windows Release und Linux mit
ASan/UBSan. Der Skalierungswert der 32-px-Fälle ist tatsächlich 2,0.
Die bestehenden Workspace-/Hierarchie-/A2-Listenabläufe bleiben bestanden;
der korrigierte Projektbaumablauf besteht separat auf beiden Plattformen.
Der unveränderte Produktionsrenderer besteht unter Linux außerdem 72 Zustände
des echten Pendellaufs über beide Schriftquellen.

Sechs getrennte native Gegenbeispiele müssen scheitern: deckende Auswahlfläche,
fehlendes Häkchen, geänderte Schriftfarbe sowie defekte Auswahl-, Sichtbarkeits-
und Aufklappaktion. Die unveränderte normale Szene besteht; EXE und PDB des
normalen Programms bleiben bytegleich. Ursprüngliche Fixture-/Harnessfehler,
Kommandos, Hashes und ihre Korrekturen sind im Nachweis kenntlich.

Praktische Tastatur-/Screenreader-/Monitor-/DPI-/macOS- und gesamte
Oberflächenabnahme bleiben offen. Dieser Schritt schließt A2 in den beiden
Baumansichten ab, nicht den gesamten Markierungs- und Oberflächenauftrag.

Nach Auswertung wurden 119 erfolgreiche temporäre Rohbilder entfernt
(16.519.836 logische Bytes). Fünf visuelle Referenzen, kompakte Pixel-/Zustands-
berichte, Quell-/Programmarchive und Fehler-/Gegenbelege bleiben erhalten;
die Bereinigung ist im Nachweis mit Pfaden und Hashes erfasst.
