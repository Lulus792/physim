# Native Messbereich-Belege

Zum [Messbereich-Abgleich](../MEASUREMENTS-NATIVE.md). Der Gesamtauftrag bleibt
offen. Quellstände, fehlgeschlagene Prüfer und bestandene Bedienfolgen werden
nicht nachträglich miteinander gleichgesetzt.

Stand 1: Windows-Build erfolgreich, Linux 4/6. Die neue Probe scheitert an ihrer
Fokusreihenfolge; die große Messlaborprobe erwartet den nun unterhalb des
sichtbaren Ausschnitts liegenden Kurventrenner ohne Scrollen. `source1-rounded-line.png`
ist außerdem eine echte Aufnahme des externen Messfensters: Der nominell 2 px
hohe Unterstrich wird als 6–10 px hoher Keil gezeichnet. `rounded-line-before.json`
enthält den unveränderten BMP-/PNG-/RGB-Nachweis und die gemessenen Pixelspalten.

Stand 2: Die native Systemschrift-Bedienfolge besteht sechs Darstellungen samt
Formatter-Grenzfällen. Die anschließende Bildprüfung verwirft jedoch das Foto
des umgerechneten Werts, weil der Prüfer noch vor der Zeichnung Kanäle und
Einheiten zurücksetzte. Das Bild und die vorherige normale Zahl-/Einheitenzeile
bleiben als `source2-*` erhalten. Dies ist kein bestandener Gesamtbildlauf.

Stand 3 trennt Aufnahmeanforderung und Mutation. Der native Messbereichlauf
`run-4910gcc_` besteht mit System- und mitgelieferter Schrift, je sechs
Darstellungen und fünf Zuständen: 60 PNG-/Geometrie-/Textprüfungen. Die normale
Kartenfläche wird in dieser Probe ausdrücklich auf 240 px, die Wunschhöhe mit
Kurven auf 420 px gesetzt; reale Fenstergrenzen klemmen diese Wünsche weiterhin.
Die Quelle, Samples, Szene, Zeit und CRC der einzigen Messdatei bleiben erhalten.
Die Umrechnung ändert nur die Anzeige desselben tatsächlich gemessenen Werts.

Stand 4 verwendet denselben Quellgraphen und die fertigen Binärdateien aus
Stand 3. Der reguläre getrackte Runner führt fünf weitere unveränderte Fälle
auf einem eigenen Xvfb/Openbox-Display aus. Der Helfer verbietet Kompilierung
und vergleicht alle Binärhashes vorher/nachher. Seine endgültigen Ergebnisse
stehen in `validation.json`; vorherige Ergebnisse sind keine aktuelle Abnahme.

Abschluss dieses Teilstands: Stand 3 besteht 1/1 Gruppe, Stand 4 besteht 5/5
(`run-egt4z71n`), zusammen sechs unterschiedliche Gruppen an derselben App.
Die große Messlaborprobe zeigt den Trenner durch vier echte Radereignisse und
besteht danach die bestehenden Größen-/Drag-/Tastaturbedingungen. Der
anschließende Page-Down-Nachweis bewegt den Offset von 72 auf 138. Windows-
Releasebuild 3 ist erfolgreich; er ersetzt keine neue Windows-GUI-Abnahme.

Die ausgewählten aktuellen PNGs bleiben bytegleich erhalten. Die externen
Messfensterbilder werden RGB-identisch aus den echten App-BMPs konvertiert;
`panel-captures.json` bindet beide Dateien und Pixelhashes. Native OS-Rahmen
gehören nicht zum App-Framebuffer. `all-frame-hashes.json` bindet alle 60 im
abgeschlossenen Messbereichlauf verwendeten PNGs, auch die nicht ausgewählten.

`check_frozen_measurements.py` prüft Artefakthashes, gleiche Quellgraphen und
die Pixel-Gegenbeispiele erneut. Aufruf vom Repository:
`python docs/visuals/unified-workspace/measurements-2026-10-24/check_frozen_measurements.py`.
Der Belegprüfer benötigt Pillow zum Lesen der RGB-identischen BMP-Konvertierung;
die App und die native Messbereichprobe erhalten dadurch keine neue Abhängigkeit.

Zwölf fremde Arbeitsdateien werden erhalten und nicht übernommen. Drei davon
sind im lokalen Linux-Kompiliergraphen enthalten; dies ist kein Clean-Checkout-
Nachweis. Windows ist hier nur gebaut. Praktische Screenreader-/DPI-/Monitor-
und macOS-Prüfungen sowie weitere leere/ungültige/lange Kanalszenarien, der
weitere Kanalkopf-Abgleich und die rollenexakten Kurvenfarben bleiben offen.
Die gesamte Oberfläche ist nicht fertig.
