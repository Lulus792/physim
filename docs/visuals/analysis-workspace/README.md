# Analysebereich: Übergabe zur nativen Umsetzung

Die [gemeinsame Designvorschau](../unified-workspace/index.html) verbindet diese
Auswertung mit Werkbank und Messlabor; sie verwendet dort erzeugte Beispielmessdaten.

Laufliste, Details und Diagrammhöhe sind mit demselben Resizing wie in Entwickeln
verstellbar. [Bedienvertrag und native Übergabe](../unified-workspace/RESIZING.md).

Stand: 19. Oktober 2026. Vorbereitet sind Design, interaktive Referenz,
Referenzbilder, ein kopierfertiger Auftrag und eine technische Abnahmeliste.
Die native Auswertungsseite wurde mit dieser Übergabe noch nicht umgebaut.

1. [NEXT-CHAT.md](NEXT-CHAT.md) ist der vollständige Auftrag zum Kopieren in den
   umsetzenden Chat. Er verweist auf alle benötigten lokalen Unterlagen.
2. [DESIGN.md](DESIGN.md) beschreibt Aufbau, Zustände und die bisherige
   Browserprüfung.
3. [IMPLEMENTATION.md](IMPLEMENTATION.md) ordnet die Vorlage dem aktuellen C-Code
   zu, klärt Unterschiede zur Demo und benennt die native Abnahme.

## Visuelle Referenz

- [Interaktive Vorschau](index.html), lokal ohne Installation/Server im Browser öffnen.
- [Messdaten dunkel](analysis-dark.png) und [hell](analysis-light.png).
- [Vergleich mehrerer Läufe](analysis-comparison.png).
- [Analyseergebnis](analysis-report.png).

Die äußere Präsentationsfläche, die Vorschauzustandswahl und die synthetischen
Pendelwerte gehören nicht in die native App. Die aktuelle App-Shell bleibt die
Grundlage; die Referenz bestimmt die Anordnung innerhalb von **Auswerten**.
Ein früherer localhost-Link ist für die Übergabe nicht erforderlich.

Der Browserprüfer liegt unter
[`tools/check_analysis_design.cjs`](../../../tools/check_analysis_design.cjs).
Sein Erfolg belegt ausschließlich den Prototyp, keine native Umsetzung.

## Prüfung dieser Übergabe

Am 19. Oktober 2026: vier Übergabedokumente mit 22 lokalen Links und 44 genannten
Quellreferenzen erfolgreich auf vorhandene Ziele geprüft. Build-CLI über
`tools/build.py --help` abgeglichen. Der unveränderte Prototyp wurde mit
`node tools/check_analysis_design.cjs` unter Windows/Edge 155.0.4283.62 erneut
erfolgreich geprüft (Exitcode 0, acht Layoutfälle, keine JavaScriptfehler).
Die vorhandenen Designbilder wurden dabei nicht überschrieben. Neue native
Builds oder Tests wurden für diese Dokumentationsübergabe nicht ausgeführt.
