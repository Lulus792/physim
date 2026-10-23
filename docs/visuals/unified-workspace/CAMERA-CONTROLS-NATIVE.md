# Kamera und Darstellung: native B-Felder

Fortsetzung des vollständigen Referenzabgleichs auf Grundlage von
`8910ac03bfe4152a04fd6c5b6984da4d928c2ace`, begonnen am 23./24. Oktober 2026.
Dieser Teilabgleich schließt die Gesamtoberfläche nicht ab.

## Aktueller geprüfter Stand

Stand 8 besteht die eigenständige native Kameraprüfung mit **120 geprüften
Aufnahmen**, jeweils 60 mit System- und mitgelieferter Schrift. Dazu gehören
sechs Theme-/Schriftzustände, native Menüwahl und Escape-Rückkehr, gesperrte
Regler und ein gesperrtes offenes Menü, echte positive/negative Home-/End-
Grenzen, Spurklick, fortgesetzter Drag aus einer festen Ansicht und vollständige
Speicherbeschriftung. Textbefehle, sichtbare Pixel und Geometrie werden gemeinsam
geprüft. Quelle, Szene, pausierte Zeit und Messdatei bleiben erhalten.

Die weiteren sieben Gruppen bestehen in Stand 7: Kameraebenen,
Projektansicht und Sichtbarkeit jeweils bei Schrift 16/22 sowie Baum mit beiden
Schriftquellen. Der einzige Unterschied zwischen Stand 7 und 8 liegt in der
Wartefolge des Kamera-Prüfers. Produktcode und alle sieben Regressionsprüfungen sind
identisch. Das Belegmanifest führt diese acht Produktgruppen mit ihren
jeweiligen tatsächlichen Quellständen und Binärhashes getrennt auf.

Der MSVC-Release-Build von Stand 8 ist erfolgreich. Daraus folgt keine aktuelle
Windows-GUI-/Screenreader-/DPI-Abnahme; auch macOS wurde hier nicht ausgeführt.
Die übrigen unten benannten Teile der Gesamtoberfläche bleiben offen.

Maßgeblich sind die Gruppe `camera-group` in
`simulation-designs/index.html`, `simulation.css`, `inspector.css`, die endgültige
B-Kaskade aus `inspector-variants.css`, das Verhalten in `inspector.js` und die
gemeinsame Palette aus `bridge.js`.

## Umsetzung und echte Zustände

Die Messlabor-Kameragruppe besitzt einen durchgehenden Inhalt. Die zusätzlichen
alten Unterabschnitte „Darstellung“ und „Kamera“ entfallen dort. „Blickrichtung“
verwendet ein Dropdown mit Standard/Vorne/Seite/Oben; „2D-Ebene“ ein getrenntes
Dropdown mit XY/YZ/XZ. Beschriftungen verwenden 11 px, Dropdownwerte 13 px,
eine mindestens 32 px hohe Fläche, 5 px Radius und eine dünne Kontur. Der
Chevron ist 8 px breit. Bei langer Beschriftung und großer Schrift wächst
die Höhe mit vollständigen Textzeilen.

Beide Zugänge verwenden dieselben Kamerawerte. Die drei Grundfilter bleiben
links im unteren Darstellungsbereich; der erweiterte linke Inhalt wiederholt
sie nicht. Rechts stehen zusätzlich die vollständigen Anzeigeoptionen. Die
Kameraoptionen bleiben links erreichbar, wie im ursprünglichen Auftrag verlangt.

Ein frei gedrehter Blick heißt „Eigene Ansicht“. Dadurch behauptet das Feld
keine unzutreffende Standard- oder Achsenansicht. Diese zusätzliche Anzeige
beschreibt einen tatsächlichen nativen Zustand; die vier auswählbaren Presets
bleiben diejenigen der Vorlage. Standard verwendet den tatsächlichen
`PS_CAMERA_DEFAULT` einschließlich Ziel und Abstand. Die Achsenpresets behalten
den bestehenden Abstand, das Ziel und den 2D-Modus und wählen ihre tatsächliche
Blickrichtung. Das bestehende native Kameraverhalten wird nicht durch die
illustrativen SVG-Werte der Browserdemo ersetzt.

Im 2D-Modus wird die Ebenenauswahl aktiv. Orbit und Neigung bleiben sichtbar
und gesperrt. Die orthografische Projektion ist weiterhin getrennt wählbar;
auch eine perspektivische 2D-Ansicht bleibt möglich. Die Ebenenwahl ändert die
tatsächliche Kameraorientierung. Wird eine bereits geöffnete Ebenenauswahl
durch den globalen 2D-Shortcut gesperrt, wird ihr Menü geschlossen.

Orbit und Neigung zeigen zwei Nachkommastellen mit `rad`, Zoom eine mit `m`,
jeweils mit Dezimalkomma. Beschriftung und aktueller Zahlenwert stehen bei
ausreichender Breite nebeneinander und sonst untereinander. Die Werte werden
aus der echten Kamera gelesen. Die Regler verwenden einen dünnen gefüllten
Verlauf und runde 16-px-Griffe. Acht Pixel Innenraum an beiden Enden halten
den vollständigen Griff innerhalb der Eingabefläche. Native Range-Werte, Bereiche und Schritte bleiben
an dieselben Zahlen gebunden. Das gemeinsame Slider-API erhält zusätzlich
eine Variante mit eigenem Fokusbild; bestehende Aufrufer behalten ihren Pfad.

„Als Projektvorgabe speichern“ verwendet die vollständige, bei Bedarf
umbrechende 12-px-Beschriftung, 8 px Innenabstand und 10 px Abstand nach oben.
Die Aktion schreibt weiterhin die echte Projektansicht über den bestehenden
asynchronen Speicherweg. Fehlende Ladung, laufender Speicherauftrag und
Wiederherstellung sperren sie. Der globale Status meldet das tatsächliche
Ergebnis. Maus-/Tastaturhinweise beschreiben die vorhandene native Steuerung.

## Ausgeführte Prüfungen und Bildbefunde

Stand 1 besteht unter Alpine-Dev mit GCC, ASan/UBSan und Xvfb/Openbox **7/7**:
Kameraebenen, gespeicherte Projektansicht und Sichtbarkeit jeweils bei Schrift
16/22 sowie den Baumprüfer mit beiden Schriftquellen. Dazu gehören tatsächliche
Presetwahl im neuen Menü, Mausorbit, Pan, Zoom, Tastaturshortcuts, perspektivische
2D-Projektion, Projekt-Neuöffnung, ungespeicherte Quellen und der externe
Speicherkonflikt. Der Windows-Release-Build besteht ebenfalls.

Die Bildkontrolle findet trotz dieser erfolgreichen Funktionsprüfungen einen
linken Einzug von 8 statt 15 px. Die bisherige Änderung von `layout->bounds`
verschiebt Nuklears tatsächlichen Zeilenursprung `layout->at_x` nicht. Stand 2
verschiebt deshalb beide gemeinsam und stellt sie nach dem erweiterten Inhalt
wieder her. Die tatsächliche Zuweisung des Widget-X-Werts im festgehaltenen
Nuklear-Header und die unveränderte negative Aufnahme bleiben erhalten.

Der ergänzte native Kamera-Prüfweg verwendet einen tatsächlich kompilierten
C-Pendelversuch und pausierten Runner. Er betrachtet sechs Theme-/Schriftzustände
mit System- und mitgelieferten Schriften: freie Ansicht und Escape, Preset per
Zeiger, Ebene per nativer Tastatur, gesperrte Orbitregler, Zoomgrenzen per
Home/End, Orbit per Zeiger und die erreichbare Speicheraktion. Er verbindet
native Textbefehle, Rechtecke und sichtbare Pixel. Quelle, Szenensnapshot,
pausierte Zeit und aufgezeichnete Messdatei müssen erhalten bleiben.
Stand 2 besteht die sieben bisherigen Gruppen, scheitert aber im neuen
Prüfweg bei Ende: Der echte Kamerazoom bleibt bei 6 statt 12 m. Im nativen
Tastaturadapter fehlt die Behandlung von Pos1/Ende für Slider. Stand 3 ergänzt
diese Behandlung ausschließlich für fokussierte native Slider. Negative
Orbitgrenzen und das Schließen eines inzwischen gesperrten Ebenenmenüs
gehören zur erweiterten Prüfung. Diese Teilprüfungen bestehen in Stand 3;
der Lauf scheitert danach am Klick in die Orbitspur, weil Nuklear nur das
Ziehen am bestehenden Griff behandelt. Stand 4 ergänzt für diese drei
Kameraregler den Sprung auf den tatsächlichen, quantisierten Spur-Klick.
Der zusätzliche Griff-Drag bleibt als unabhängige Zeigeraktion geprüft.
Stand 4 endet später bei 22 px an einem unerfüllbaren kombinierten
Kopf-/Feld-Scrollziel des Prüfers. Stand 5 verlangt bei einer tatsächlich
zu hohen Kombination stattdessen das vollständige erreichbare Auswahlfeld,
mit protokollierten Höhen. Außerdem beseitigt er den seitlich abgeschnittenen
Fokus: Nur zum Zeichnen der äußeren Linie wird der vorhandene horizontale
Innenabstand benutzt; die vertikale Sichtgrenze bleibt erhalten. Die Probe
verlangt sichtbare linke Fokuslinien bei Regler und Checkbox.
Der native Systemschrift-Lauf von Stand 5 durchläuft alle sechs Zustände.
Die anschließende strenge Bildprüfung findet jedoch den bei freier Blickrichtung
neu umbrochenen Feldtext: Nach der Zeigeraktion liegt das untere Reglerende
bei 541,4877 statt innerhalb des bis 541,125 reichenden Ausschnitts.
Stand 6 ergänzt ausschließlich drei erneute Scrollaufrufe im Kamera-Prüfer
nach den Layoutänderungen. Produktcode, Fokusdarstellung und Baumprüfer
bleiben gegenüber Stand 5 identisch.

Bei 22 px benötigt Kopf plus erstes Feld tatsächlich 248,9437 px; nach den
Sichtabständen bleiben 224,7062 px verfügbar. Der Prüfer verlängert deshalb
keine Frist und verkleinert keine Schrift, sondern betrachtet die vollständig
erreichbaren Inhalte beim Scrollen. Stand 6 besteht die sieben ausgeführten Gruppen mit 120 Kamerabildern.

Die nächste Gestenprüfung berücksichtigt außerdem, dass der Wechsel von
„Oben“ zu „Eigene Ansicht“ das Feld bei schmaler Spalte und 22 px Schrift
zwischen 44 und 73,8375 px Höhe ändern kann. Stand 7 reserviert deshalb die
für sämtliche Beschriftungen nötige Zeilenzahl bereits vorher. Die normale
32-px-Höhe bleibt bei ausreichender Breite erhalten. Das höhere Feld in der
schmalen großen Schrift ist eine bewusste Anpassung für vollständigen Text
und einen unverrückten Regler während des Ziehens. Die HTML-Demo hat keine
eigene Beschriftung für einen frei gedrehten nativen Blick.

Die Probe zieht aus der kanonischen Ansicht mit zwei aufeinanderfolgenden
Mausbewegungen auf unveränderter ursprünglicher Y-Position und verlangt
den tatsächlichen zweiten Orbitwert. Ein separater Spurklick mit weiterem
Drag und konstante Feldhöhen aller Aufnahmen ergänzen den Nachweis.
Der endgültige Gesamtlauf wird separat ausgewiesen.

Eine getrennte GDB-Aufnahme von Stand 2 setzt den Zoom kontrolliert auf 12 m
und belegt den alten, über die rechte Eingabegrenze ragenden Griff. Die App
scheitert danach weiterhin an Pos1. Dieses Bild ist ausdrücklich nur ein
Geometrie-Gegenbeleg, kein bestandener Eingabe- oder Sanitizer-Test.

Die tatsächlich abgeschlossenen Stände und Ergebnisse stehen im
[Belegmanifest](camera-controls-2026-10-24/validation.json).

## Verbleibender Gesamtauftrag

Die Szenenfläche besitzt noch nicht ihren vollständigen oberen Kamera- und
unteren Ansichtsrahmen aus der Vorlage. Dieser weitere native Umsetzungsbereich
ist offen und keine technische Unmöglichkeit. Ebenso bleiben andere
Inspektorgruppen, Werkbank, Auswertung und F1 im Gesamtabgleich.

Die neuen Dropdowns verwenden den vorhandenen nativen Button-/Popup-Vertrag.
Ein vollständiger plattformspezifischer ComboBox-/Expand-Collapse-Vertrag und
praktische Screenreader-Aufgaben sind hier nicht nachgewiesen. Ein aktueller
Windows-Kamera-GUI-Lauf, macOS und physische DPI-/Monitorwechsel bleiben
ebenfalls offen. Schriftgrößen 16 bis 22 ersetzen diese Prüfungen nicht.
