# Eingefrorene native Zeitleistenbelege

23. Oktober 2026. Teilabgleich auf Basis ca16fc4; keine Gesamtabnahme.
Der zugehörige [Bericht](../SIMULATION-TIMELINE-NATIVE.md) nennt Verhalten,
Abweichungen und Prüfgrenzen. `validation.json` ordnet die Läufe zu;
`prior-runs.json` enthält auch fehlgeschlagene Vorstufen. Vollständige
Prozess-/Ergebnisdatensätze sind verlustfrei komprimiert.

- `source0` hält den ursprünglichen Stand, `source1` die erste Aufteilung fest.
- Quelle 2 entdeckt Leser-Sharing unter Windows und eine zu frühe Range-Prüfung.
- Quelle 3 korrigiert diese Vorrichtungsfehler und entdeckt den gefilterten Fokusverlust.
- Quelle 4 lässt nur den gezielten eigenen Fokusverlust durch. Der native
  Linux-Designablauf besteht; der Python-Parser verwechselt den abschließenden
  Marker mit einem Geometriedatensatz. Die Kontextgegenprobe entdeckt echte
  Wiederherstellung des verworfenen Snapshots mit Zeit 5.
- Quelle 5 trennt den Parser, entsorgt Capture und entfernt Tooltip und zusätzlichen
  Trennerrahmen. Der Kontext zeigt nun separat die alte Leerlaufzeit 8;
  Windows entdeckt den um zwei Pixel versetzten Spurbeginn.
- Quelle 6 korrigiert Leerlaufzeit und Spurbeginn. Linux besteht sechs Gruppen,
  Windows fünf von sechs; der bestehende Zeitleistentest lief nach später
  Fokusübernahme wegen seiner langen echten Aufzeichnung aus.
- Quelle 7 wartet vor dem Runnerstart auf echten Hostfokus und entfernt nur
  den zusätzlichen generischen Fokusrahmen der Zeitleistenrange. Die ursprünglichen
  Zeitgrenzen und die Ein-Pixel-Toleranz bleiben unverändert.

`source*.json.gz` umfasst den lokalen Quellgraphen mit den separat erfassten
zwölf fremden Dateien. `public-source.json` grenzt die 240 eigenen/zugehörigen
Quellen und 18 unveränderten Fontressourcen davon ab. Die fremden Arbeitsdateien
wurden weder überschrieben noch mit diesem Teilabgleich committed.

`verify-negative.py` spielt die tatsächlichen negativen Zustände und Pixel
gegen die finalen Belege durch. `references.json` hält die Originalreferenzen,
`exports.json` die 16 unveränderten kanonischen Exporte fest. Die PNGs stammen
unverändert aus den nativen App-Prozessen. Nur die Ausgangsaufnahme wurde vom
BMP verlustfrei nach PNG konvertiert; `before-capture.json` bestätigt RGB-Gleichheit.
Weder Browserbilder noch retuschierte Nachweise werden als native Abnahme verwendet.

Native UI-Schriftgrößen sind keine Prüfung eines physischen DPI-/Monitorwechsels.
Die Kontextprobe ist ausdrücklich synthetisch; die separate Designprobe verwendet
den tatsächlichen adaptiven Runner. macOS und praktische Screenreader-Aufgaben
sind hier nicht nachgewiesen. F1-Lifecycle und die Gesamtgestaltung bleiben offen.
