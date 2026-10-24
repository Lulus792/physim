# Nativer Szenenrahmen

Fortsetzung auf Grundlage von `90f69f1f55099c15690cd45b3739c88635f7a346`.
Der gesamte Oberflächenabgleich einschließlich Entwickeln, Auswerten und F1
bleibt offen. Dieser Bericht behandelt den oberen Kameraüberzug und die untere
Ansichtsleiste des Messlabors.

## Referenz und Umsetzung

Maßgeblich sind `scene-top` und `scene-bottom` in
`simulation-designs/index.html`, ihre Kaskade in `simulation.css`,
`inspector.js`/`simulation.js`, die gemeinsame Palette in `bridge.js` sowie
`RESIZING.md`. Eine neue Browserausführung ist damit nicht behauptet.

Die obere Zeile liegt wie in der Vorlage über dem tatsächlichen Szenenbild:
11-px-Ansichtsbezeichnung, 10-px-Zustand auf einer leisen Fläche und rechts die
10-px-Kameraauswahl mit dünner Kontur, 6-px-Radius und kleinem Chevron. Die
Auswahl ist ein absolut positioniertes Feld im bestehenden nativen Popupweg.
Das gemeinsame Combo-API erhält dafür einen Aufruf mit bereits zugewiesenem
Rechteck; die bisherigen Aufrufer reservieren ihre Zeile weiterhin wie zuvor.

Die untere Leiste steht im normalen Layout unter dem Bild: mindestens 32 px,
9-px-Ansichts-/Einheitenhinweis und eine transparente 10-px-Aktion „Zentrieren“.
Der 3D-Maushinweis entfällt wie in der Vorlage bei schmalem Fenster. Bei wenig
Platz stehen Ansicht und Aktion untereinander. Die Maximalmaße der möglichen
Beschriftungen reservieren den Platz vorab, damit ein Kamerazug den Bereich
nicht durch wechselnde Texte verschiebt. Die CSS-Maße gelten bei Basisschrift
16; größere Schrift skaliert sie wie die übrigen nativen Messlabor-Komponenten.

Beide Kameraauswahlen und der Inspektor verwenden denselben tatsächlichen
Kamerazustand. Perspektive/Vorne/Seite/Oben rufen die bestehenden nativen Presets
auf. Frei gedrehte Ansichten heißen „Eigene Ansicht“; eine orthografische
Standardrichtung wird als solche benannt. Der untere Hinweis nennt im 2D-Modus
die tatsächliche Ebene. Diese zusätzlichen Anzeigen vermeiden falsche Aussagen
über die native Kamera, welche die illustrative Demo nicht vollständig abbildet.

„Zentrieren“ setzt denselben Standard einschließlich Zielpunkt, Abstand,
Projektion und 3D-Modus wie die vorhandene Standardaktion. Es verändert keine
Modellquelle und startet keinen Lauf. Zustand und Hinweise stammen aus dem
vorhandenen Runner-/Wiedergabe-/Kamerazustand.

## Eingabe, Beschriftung und Mindestgrößen

Der Kameraüberzug wird vor der unteren Aktion in die native Bedienfolge
eingetragen. Die neue Probe verlangt Tab zur unteren Aktion und Shift-Tab zurück,
ohne das erwartete Ziel direkt anzufokussieren. Menüs behalten ihren bestehenden
nativen Enter-/Escape- und Fokus-Rückkehrweg.

Ein Druck auf die obere Auswahl führt nicht zusätzlich eine Objektauswahl im
darunterliegenden Szenenbild aus. Das Mausrad über der unteren Leiste zoomt nicht
die Kamera. Der eigentliche Orbit-/Pan-/Pick-Bereich ist das echte Bildrechteck.
Die vorhandene Beschriftungsplatzierung berücksichtigt zusätzlich die drei
oberen UI-Flächen und ihren Fokusabstand. Physikalische Objekte bleiben an ihren
Modellpositionen; verschobene Beschriftungen behalten den vorhandenen Hinweisstrich.

Die 180-px-Mindestgröße aus `RESIZING.md` bezeichnet nun ausdrücklich den ganzen
Szenenbereich einschließlich der neuen unteren Leiste. Bild und Leiste müssen
diesen Bereich lückenlos zusammensetzen; das Bild behält mindestens 64 px. Bei
größerem tatsächlichem Schriftbedarf steigt die native Mindesthöhe, wie in der
Referenz zugelassen. Der Separator bleibt unter der Leiste. Die Messbereich-
Mindestgröße und gespeicherte Wunschgrößen bleiben erhalten. Die bisherigen
Mindestgrößenprüfer werden auf Rahmen, Bild und Leiste gemeinsam umgestellt;
ein kleiner gewordenes Bild allein beweist keinen Verlust der Rahmenmindesthöhe.

Die Szene verwendet denselben Zeichner auch im herausgezogenen OS-Fenster.
Herausziehen und Rückkehr laufen weiter über den bestehenden Panelkopf. Die
gemeinsame App-Breite bestimmt die Designvariante auch dort; die tatsächliche
Bereichsbreite bestimmt weiterhin Umbruch und Platzbedarf. Damit schaltet ein
herausgezogenes Fenster bei gleicher Inhaltsbreite nicht plötzlich auf andere
Abstände und ausgeblendete Hinweise um.

## Bisherige Läufe und begrenzte Aussagen

Stand 1 scheitert vor Laufzeit an einem falschen angenommenen Feldnamen. Die
Compilerlogs und der anhand der Quellhashes rekonstruierte vollständige öffentliche
Quelldelta bleiben erhalten. Stand 2 verwendet das vorhandene `timeline_browsing`.

Stand 2 besteht acht von zehn ausgeführten Gruppen. Die neue native Systemschrift-
Bedienfolge besteht alle sechs Darstellungen. Die Bildauswertung trifft in zwei
Menübildern zusätzlich das überlagernde Wort „Oben“ im Rechteck der unteren Aktion;
dies ist kein zweiter Buttontext. Die vollständige Caption bleibt in allen nicht
überlagerten Zuständen exakt zu prüfen. Der andere fehlgeschlagene Lauf benutzt
noch die alte Annahme „Bildhöhe = Szenenhöhe“. Die neue Leiste gehört ausdrücklich
zum Szenenbereich und muss in dieser Prüfung mitgezählt werden.

Stand 3 ergänzt die native Tabfolge und einen kontrollierten Kamerapose-Randfall:
Eine echte Modellbeschriftung wird durch eine mit der Projektionsfunktion berechnete
Kameraverschiebung an die obere Auswahl gebracht. Die alte Platzierungsregel ohne
UI-Sperrflächen wird als geometrisches Gegenbeispiel gerechnet; die tatsächliche
neue Beschriftung muss sichtbar außerhalb aller oberen UI-Flächen stehen. Dies
ist keine nachträglich behauptete Aufnahme eines alten vollständigen App-Laufs.

Die neue Probe hält Quelle, Szenendaten, pausierte Zeit, Auswahl und Messdatei
fest. Tatsächlich abgeschlossene Läufe und ihre Quellstände werden in den
[eingefrorenen Belegen](scene-frame-2026-10-24/README.md) ausgewiesen.

Stand 3 besteht die neue Szenenprobe mit beiden Schriftquellen, sechs
Darstellungen und je neun Zuständen: 108 Bild-/Geometriesätze. Stand 4 hat
denselben Quellgraphen und besteht elf weitere Gruppen. Ein zusätzlicher
nativer Fensterlauf dieses Stands zeigt jedoch die abweichende schmale
Designvariante im externen Fenster; diese begrenzte Stilabweichung wird in
Stand 5 korrigiert.

Stand 5 besteht Szenenprobe, normales Herausziehen/Rückkehr und dieselben
Fensterabläufe bei großer Schrift. Vier tatsächliche Fensterläufe prüfen die
Szenenauswahl, relativen Abstände, Leistenhöhe und Sichtbarkeit der Hinweise
vor/nach dem Herausziehen bei gleicher Inhaltsbreite. Sechs unterschiedliche
OS-Fenster werden jeweils außerhalb des Hauptfensters erfasst und zurückgeholt.
Dies geschieht unter Alpine/Xvfb/Openbox, nicht an einem physischen Zweitmonitor.

Stand 6 besteht die zehn übrigen Gruppen am identischen Quellgraphen und an
unveränderten Binärdateien: Zeitleiste, Kameraebenen 16/22, Projektspeicherung
16/22, Sichtbarkeit 16/22, Messlabor, Szenenbaum und Kamera. Zusammen mit Stand 5
sind dies 13 unterschiedliche aktuelle Gruppen. Der reguläre Testkatalog und
seine Erwartungen bleiben erhalten. Der nur lesende SDK-Helfer führt dieselben
getrackten Fälle auf einem unabhängigen X-Display aus und verbietet Kompilierung.

Der unabhängige Belegprüfer bestätigt alle Artefakthashes, 12 geometrische
Beschriftungsgegenbeispiele und vier reale externe Fensterabläufe. Im lokalen
Fußzeilenrechteck fehlen dem Vorgänger die neuen Textpixel; der aktuelle native
Lauf zeichnet sie. Dies ist ausdrücklich kein vollflächiger pixelidentischer
Vergleich mit der illustrativen HTML-Demo. Der Windows-Releasebuild von Stand 5
ist erfolgreich; dessen neue GUI-Bedienfolge ist noch nicht ausgeführt.

Ein erfolgreicher Windows-Build ersetzt keine Windows-GUI-Abnahme. Praktische
Screenreader-Aufgaben, physische DPI-/Monitorwechsel und macOS bleiben offen.
Die bisherigen Button-/Popup-Semantiken sind außerdem kein vollständiger neuer
plattformübergreifender ComboBox-/Expand-Collapse-Vertrag.
