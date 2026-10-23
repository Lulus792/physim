# Native Inspektorköpfe und Modellwertkarten

23. Oktober 2026, Grundlage 207ce2d. Dies ist ein Teilabgleich; der
[Bericht](../INSPECTOR-FIELDS-NATIVE.md) nennt Umfang und verbleibende Arbeit.
Die Benennung des Ordners schließt die geplante weitere Szenenbaumprüfung ein;
dieser Stand verändert dort nur den Panelkopf, nicht den vollständigen Baum.

Die ursprünglichen CSS-/PNG-Referenzen sind in `references.json` gehasht.
`validation.json` ordnet die aktuellen Läufe, `prior-runs.json` die tatsächlich
ausgeführten Vorstufen einschließlich der Fehler zu. Ergebnisse, vollständige
Prozessspuren und strukturierte Geometrie werden verlustfrei komprimiert.

- Quelle 0 ist der vorherige lokale Stand.
- Quelle 1 ändert Köpfe und Karten. Je fünf Funktionsgruppen bestehen auf beiden
  Plattformen, obwohl die Karten am Clip-Ursprung andere Texte verdecken.
- Quelle 2 korrigiert den Zeilenursprung, Innenbreite und Fußzeile. Beide nativen
  Systemschriftprozesse bestehen; der Python-Prüfer verlangt fälschlich volle
  Linienfarbe in einer einzelnen Zeile einer fraktional positionierten Kontur.
- Quelle 3 prüft stattdessen Deckung und Schwerpunkt. Die Kartenkontur liegt
  vollständig innerhalb des Clips. Linux besteht beide Schriftquellen; Windows
  erhält in Phase 0 keinen Tastaturfokus und führt keine UI-Aufgaben aus.
- Quelle 4 verbessert die kurze Fußzeile und unterscheidet tatsächliches Scrollen
  von bereits ohne Mausrad erreichbaren Karten. Linux besteht sechs Gruppen,
  Windows beide Schriftquellen des neuen Prüfers nach manuellem Anklicken.
  Die anschließende Bildkontrolle findet einen unabhängigen Kopf-Wortumbruch.
- Quelle 5 stapelt die vollständigen Kopftexte nur bei unzureichender Breite und
  prüft deren vollständige native Textbefehle plus sichtbare Glyphen. Linux
  besteht beide Schriftpfade. Der erste Windows-Nachlauf wartet in Phase 0
  vergeblich auf tatsächlichen Tastaturfokus; weitere Ergebnisse sind im
  aktuellen Manifest aufgeführt.

`source*.json.gz` hält den tatsächlichen lokalen Quellgraphen einschließlich
separat erfasster fremder Änderungen fest. `public-source.json` enthält 243
zugehörige Quellen und 18 Fontressourcen, ohne die zwölf fremden Arbeitsdateien
zu übernehmen. Drei der fremden Dateien sind im lokalen App-Quellgraphen
enthalten. Die nativen Apps sind deshalb kein Clean-Checkout-Nachweis des
anschließenden Commits. Die übrigen fremden Dateien wurden ebenfalls weder
geändert noch mit diesem Teilabgleich committed.

Die ausgewählten PNGs sind unveränderte native Readbacks. Nur das ursprüngliche
negative BMP wurde RGB-identisch in PNG umgewandelt, wie `negative-card.json`
belegt. Keine Retusche, Skalierung oder Browseraufnahme dient als native Abnahme.
`verify-negative.py` prüft die realen Gegenbilder sowie ausgewählte positive
Bild-/Eingabenachweise. Der vollständige Live-Prüfer bleibt
`tests/check_inspector_design.py`.

Tatsächlicher SDL-Tastaturfokus und native Eingabeproben ersetzen keine praktische
Screenreader-Aufgabe, keinen physischen DPI-/Monitorwechsel und keinen macOS-Lauf.
Die übrigen Inspektorinhalte, der Szenenbaum und die Gesamtoberfläche bleiben offen.
