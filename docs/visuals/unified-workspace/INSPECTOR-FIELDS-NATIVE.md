# Inspektorköpfe und Modellwertkarten

Arbeitsstand: 23. Oktober 2026, Grundlage `207ce2d0c2868cf8ceae4b386c4c61a19583059f`.
Dies ist ein weiterer Teilabgleich. Die gesamte Oberfläche und der vollständige
Inspektor sind weiterhin nicht abgenommen. Der abschließende Windows-Fokuslauf
ist derzeit noch offen; die verfügbaren Nachweise werden nach seinem Ende ergänzt.

Maßgeblich sind das ausgewählte [B · Wertefelder](../simulation-designs/MODEL-EDITING.md),
[die dunkle Detailreferenz](../simulation-designs/inspector-fields-dark.png),
[die helle Detailreferenz](../simulation-designs/inspector-fields-light.png)
und die gemeinsame Messlaborpalette. Die spätere `inspector-variants.css`
überschreibt die ursprüngliche `inspector.css`: 13-px-Abschnittsschrift und
42-px-Mindestzeilen gelten für B; 12/36 px gehören zum nicht ausgewählten
Kompaktvergleich. Die native App führt keine zusätzliche Designauswahl ein.

## Umgesetzter Teil

Die Panelköpfe erhalten die vorgegebenen Schriftrollen und Innenabstände.
Der Inspektorkopf ist bei Standardschrift 46,85 px hoch und zeigt rechts
„Alle Funktionen“. Der zusätzliche X-Button entfällt bei den festen
Messlaborseiten. Ihr Schließzugang bleibt über die bestehenden Bereichsaktionen
Szenenbaum und Inspektor verfügbar; das Ziehen am bestehenden Kopf und native
Zusatzfenster bleiben erhalten. Andere Arbeitsbereiche und alte Layoutzugänge
verwenden weiterhin ihre bisherigen Headerpfade.

Abschnittsköpfe verwenden 42 px Mindesthöhe, 13-px-Titel, 10-px-Zusatzinformation,
Plus/Minus und eine einzelne ein Pixel breite Trennlinie. Lange Titel umbrechen;
große Schrift darf die Zeile vergrößern. Die bisherige zusätzliche
Nuklear-Baumeinrückung innerhalb der Gruppen entfällt. Nach offenem Inhalt
bleibt der vorgegebene 14-px-Abstand erhalten, ohne ihn durch die allgemeine
Mindesthöhe für kleine Widgets erneut zu vergrößern.

Modellwertkarten erhalten die vorhandenen tatsächlichen Bindungen: 8 px Radius,
11 px Innenabstand plus Kontur, Bezeichnung über dem 19-px-Wert und 35 px hohe
Eingabefläche bei Standardschrift. Der Kartenabstand beträgt 10 px.
Dezimalkomma, Punkt, Validierung, vorgemerkte Änderungen und Neukompilieren
bleiben am bisherigen Fachpfad. Kompilierte Werte und zulässige Bereiche
stammen weiterhin aus den tatsächlichen Modellbeschreibungen.

Die feste innere Fußzeile ist bei einer Zeile 37 px hoch. Die native
Beschriftung „Modell · Lauf · Ansicht“ ersetzt den illustrativen Datenhinweis
der Browserdemo. Bei Bedarf bricht sie um. Der Inhalt erhält seine eigene
Scrollfläche. Die native Scrollleiste liegt in deren rechtem Innenabstand:
Die zuvor reservierte Breite steht dem Inhalt wieder zur Verfügung, ohne
Nuklears vorhandene vertikale Scroll-/Drag-Verarbeitung zu ersetzen.
Horizontales Scrollen bleibt unterdrückt. Damit reichen die Karten bei einer
320-px-Spalte von x=1137 bis x=1424 im 1440-px-Fenster, entsprechend der Vorlage.

## Gefundene Fehler

Die erste neue Kartenzeichnung benutzte `nk_layout_space_bounds` als Zeilenursprung.
Im festgehaltenen Nuklear-Snapshot liefert die Funktion aber den Clip-Ursprung.
Dadurch überdeckten die Modellkarten andere Abschnitte, während die tatsächliche
Eingabe weiter an ihrer richtigen Zeile lag. Beide bisherigen Funktionssuiten
bestanden trotzdem jeweils 5/5. Das [negative native Bild](inspector-tree-2026-10-23/negative-card-clip-origin.png)
bleibt erhalten: Im vorher sichtbaren Objekt-Titel liegen null helle Titelpixel.
Die neue Zeichnung verwendet `nk_layout_space_to_screen` für den wirklichen
Zeilenursprung; der Bildprüfer verbindet Karten- und Eingaberechteck zusätzlich
über ihren tatsächlichen senkrechten Abstand.

Die zunächst mittig auf der Kartenkante gezeichnete Kontur lag links zur Hälfte
außerhalb des Inhaltsclips. Das [native Gegenbild](inspector-tree-2026-10-23/negative-half-card-border.png)
zeigt dort nur halbe Deckung. Die Kontur liegt nun vollständig innerhalb der
Karte. Eine fehlende Kontur darf im Pixelprüfer nicht bestehen.

Eine davon getrennte Annahme des neuen Prüfers verlangte auf einer Trennlinie
volle Linienfarbe in einer einzelnen Bildzeile. Bei y=282,85 verteilt die
Rasterung ihre Deckung auf zwei Zeilen. Der korrigierte Prüfer verlangt
zusammen weiterhin genau eine Pixelbreite und den richtigen Linienschwerpunkt.
Eine leere Vergleichsfläche muss durchfallen. Die reine Farbannahme wurde
korrigiert; die ursprünglichen Geometrieanforderungen wurden nicht gelockert.

Bei der Bildkontrolle des zunächst bestandenen Standes 4 zeigte sich ein
weiterer echter Fehler: In der 208-px-Spalte zerfielen bei großer nativer
Schrift „Inspektor“ und „Alle Funktionen“ innerhalb der Wörter. Das
[unveränderte Linux-Bild](inspector-tree-2026-10-23/negative-header-word-wrap.png)
bleibt als Gegenbeleg erhalten. Stand 5 verwendet bei fehlender Gesamtbreite
zwei vollständige Textzeilen im bestehenden Kopf. Bei ausreichender Breite
bleibt die ursprüngliche horizontale Anordnung erhalten. Der Prüfer verlangt
nun zusätzlich die vollständigen nativen Textbefehle innerhalb des Kopfes
und tatsächlich sichtbare Schriftpixel für beide Beschriftungen.

## Tatsächlich ausgeführte Prüfungen

Stand 1 bestand unter Windows und Linux je fünf Funktionsgruppen: Herauslösen
und Rückkehr, Szenensichtbarkeit bei Schrift 16/22, vollständiger Messlaborablauf
und Modellwertänderungen in C/Phys einschließlich Kompilierung und Fehlerpfad.
Der neue Bildfehler war dadurch nicht ausgeschlossen.

Stand 4 besteht unter Linux alle sechs Gruppen einschließlich des neuen
Inspektorprüfers; unter Windows besteht der neue Prüfer mit beiden
Schriftquellen. Nach diesem Lauf wurde der unabhängige Wortumbruchfehler im
Bild entdeckt. Stand 5 besteht den erweiterten Prüfer unter Linux mit beiden
Schriftquellen. Der erste Windows-Nachlauf von Stand 5 erhält in Phase 0
keinen tatsächlichen SDL-Tastaturfokus und läuft nach 120 Sekunden vor
Compiler-/Runnerstart aus. Das ist kein bestandener Geometrienachweis.
Der begleitete Windows-Abschluss der Kopfkorrektur ist noch offen.

Der neue Prüfweg verwendet je Schriftquelle einen tatsächlich kompilierten
C-Pendelversuch und einen echten pausierten Runner. Er prüft sechs Varianten:
Dunkel/Hell mit 320-px-Inspektor bei 1440×1000 sowie dunkle Schrift 18, helle
Schrift 20/22 und Hochkontrast 22 mit minimaler 208-px-Spalte bei 1080×740.
Enter öffnet und schließt den Objektabschnitt, der native Fokus bleibt erhalten.
Die Modellkarten werden über echte Zeigerereignisse erreicht; nur die vier
schmalen Varianten benötigen tatsächlich Mausradereignisse. Die zwei breiten
Varianten passen ohne Scrollen. Quelltext, pausierter Szenensnapshot, Zeit und
aufgezeichnete Messdatei bleiben unverändert. Geometrie, Konturdeckung und
sichtbare Wertglyphen werden aus den nativen Bildern geprüft.

## Noch erforderlicher Abgleich

Die Modellwertkarten und Köpfe ersetzen keine vollständige Umsetzung aller
neun Gruppen. Insbesondere ausgewählter Objektkopf/Avatar, weitere Wertefelder,
Szenenbaum-Inhalt, Kamera-/Darstellungscontrols und zusätzliche Zustände sind
weiter gegen die Vorlage abzugleichen. Die echten zusätzlichen Bereichs- und
Kompilierungsinformationen sind keine festen Demowerte. Masse und RK4 werden
keinen beliebigen Objekten ohne entsprechende Metadaten zugeschrieben.

Die neuen Abschnittsaktionen sind native Buttons mit tatsächlicher Tastaturaktion
und einem separat veröffentlichten geöffneten/geschlossenen Textzustand.
Ein eigener nativer Expand/Collapse-Vertrag und praktische Screenreader-Aufgaben
sind damit noch nicht nachgewiesen. Ebenso fehlen hier macOS und physische
DPI-/Monitorwechsel. Abweichungen der vorhandenen Regular-/Bold-Schriftrollen,
Einheitentypografie und der gesamten Oberfläche bleiben weiter zu prüfen.
Für diese offenen Punkte wird keine technische Unmöglichkeit behauptet.
