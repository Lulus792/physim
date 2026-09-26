# ADR 002: OpenGL-3.3-Core-Renderer

Datum: 2026-09-16. Status: implementiert, lokal unter Windows geprüft.

`app/graphics.c` kapselt Kontext, Funktionszeiger, Shader, Buffer und Framebuffers.
Grafikaufrufe bleiben im UI-Thread. SDL lädt Funktionen nach Erzeugung des aktuellen
Kontexts. Ohne OpenGL 3.3 Core beendet sich die App mit Diagnose; automatisierte
Testmodi öffnen keinen Fehlerdialog. Core und Runner bleiben unabhängig von Grafik.

Die Szene verwendet GLSL 330, Lambert-Beleuchtung plus Umgebungslicht und einen
24-Bit-Tiefenpuffer. Ein Framebuffer mit bis zu vier Samples wird in eine RGBA8-Textur
aufgelöst. Nuklear zeichnet diese in den Viewport. Nur die Szenentextur wird vertikal
gedreht, Font-UVs bleiben unverändert. Scissor-Rechtecke und Viewportgrößen
berücksichtigen das Verhältnis zwischen Fensterkoordinaten und Framebufferpixeln.

Die Kugeltessellation entsteht beim Start. Weltkoordinaten, Normalen und Farben der
begrenzten Szene werden pro Frame in einen wiederverwendeten CPU-Puffer geschrieben
und per Streaming-VBO hochgeladen. Der erste Renderer verwendete Modul-ABI und IPC 1;
die Szenenerweiterung verwendet nun ABI/IPC 3 (siehe API- und Datenformatdokumentation).
Die Orbitkamera unterstützt Pan, Zoom, Perspektive und Orthografie. Ihre explizite
Basis bleibt auch bei exakter Draufsicht definiert.

`--renderer-test [bild.bmp]` liest vom tatsächlichen GPU-Framebuffer zurück und prüft:

- identische Pixel bei umgekehrter Reihenfolge verdeckter Körper;
- unterschiedliche perspektivische und orthografische Darstellung;
- identische Pixel nach einem Größenwechsel und Rückkehr;
- Abschneiden hinter der Kamera und Zurückweisung extremer Koordinaten;
- gültige Draufsicht und Ausblenden der Vektoren;
- mehrteilige UTF-8-Texte und Zurücksetzen der Eingabe bei Fokusverlust.

Der optionale Bildpfad erzeugt eine Szene mit Box, Kugel, Linie und Pfeil zur visuellen
Prüfung. Der App-Selbsttest erfasst Editor, Simulation und Analyse. Diese Prüfungen
ersetzen keine Abnahme auf Linux oder weiteren Grafiktreibern.

Punkt, Ebene, Polyline, Label, orientierte Boxen, Picking und individuelle
Objektsichtbarkeit sind inzwischen ergänzt. RGBA-Transparenz zeichnet zunächst
opake Geometrie mit Tiefenschreiben. Ein wiederverwendeter Indexpuffer sortiert
teiltransparente Dreiecke nach der Kameratiefe ihres Schwerpunkts. Diese werden
mit Tiefentest, ohne Tiefenschreiben und mit Source-Alpha-Mischung gezeichnet.
Der Alpha-Kanal der fertigen Szenentextur bleibt 1, damit die UI sie nicht erneut
transparent mischt. Alpha 0 erzeugt weder Mesh noch Trefferfläche.
Sich durchdringende oder zyklisch verdeckende transparente Flächen können mit
dieser Näherung Sortierartefakte zeigen. Größere instanzierte Szenen bleiben offen;
die gesamte Phase 6 ist damit noch nicht vollständig abgeschlossen.

Der GPU-Test prüft Alpha-Mischwerte, zwei transparente Ebenen in vertauschter
Objektreihenfolge, opake Verdeckung sowie unsichtbare und auswählbare Geometrie
in beiden Projektionsarten. Der optionale Bildpfad erzeugt zusätzlich
`<bild.bmp>-alpha.bmp` mit halbtransparenter Box und Kugel.

## Diagrammexport als PNG

Ein eigener Nuklear-Kontext zeichnet das vollständige Ergebnisdiagramm auf Papierweiß.
Er teilt ausschließlich den Fontatlas mit der App. Ein separater RGB8-Framebuffer
rendert 1200 × 850 logische Einheiten auf 2400 × 1700 Pixel. Fenstergröße, interaktiver
Zoom und Szenen-Framebuffer bleiben davon unabhängig. Nach dem Readback werden die
Bildzeilen umgedreht; `app/png.c` schreibt RGB8 ohne Alpha mit exklusiver Dateierzeugung.
Die Nuklear-Geometrie verwendet 32-Bit-Indizes, damit auch acht Punktkurven mit je
2048 Punkten ohne Indexüberlauf exportiert werden können.

Der kleine Schreiber verwendet Filter None und gespeicherte DEFLATE-Blöcke, einen
Block pro Zeile, mit Adler32 und PNG-CRC. Dadurch ist keine zusätzliche Laufzeitbibliothek
nötig, die Ausgabe benötigt allerdings etwa 12,3 MB. Referenzen:
[PNG-Spezifikation](https://www.w3.org/TR/png-3/),
[zlib-Datenformat](https://www.rfc-editor.org/rfc/rfc1950),
[DEFLATE-Datenformat](https://www.rfc-editor.org/rfc/rfc1951).
`tests/verify_png.py` prüft unabhängig mit Pythons zlib/CRC-Implementierung Pixelwerte,
Histogrammhöhen, Bildorientierung und alle acht Legendenmarker. Pillow dient zusätzlich
zur visuellen Kontrolle. Die App braucht weder Python noch Pillow.
