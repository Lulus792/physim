# F1-Layout H: ausgewähltes Zielbild

Entscheidung des Nutzers vom 16. Oktober 2026: **H** ist die Grundlage für die
neue native F1-Dokumentationsoberfläche. Das helle/dunkle Design ist inzwischen
im bestehenden C-/SDL-/Nuklear-Renderer umgesetzt. Die
[native Abnahme](../visuals/f1-layouts/NATIVE-H.md) enthält Fensterbilder,
ausgeführte Tests und verbleibende Plattformgrenzen.

- Links: gemeinsame Themen-/Abschnittsnavigation aus E/G, direkte Sprungziele.
- Rechts: Modellkontext aus Entwurf C, Tabs Modell/Beispiel.
- Je ein kleines Icon an den äußeren Enden der Kopfleiste; beide Seiten unabhängig
  ein-/ausklappbar. Keine langen Umschaltbeschriftungen oder gekoppelten Aktionen.
- C/Physim einmal global für Lernweg, Anleitung, Suche und Beispiele.
- Gleiche Geometrie und Bedienung in Hell/Dunkel; vorhandene semantische Paletten
  nutzen und hohen Kontrast sowie native Dokumentationsfunktionen erhalten.

Die [Designspezifikation](../visuals/f1-layouts/DESIGN-H.md) enthält Zustände,
Maße, Farben, Bedienregeln, Quellcode-Einstiegspunkte und Abnahmekriterien.
[Hell](../visuals/f1-layouts/h-light.png), [Dunkel](../visuals/f1-layouts/h-dark.png)
und die [Zustandsübersicht](../visuals/f1-layouts/h-final-overview.png) sind die
visuellen Referenzen. Der [ursprüngliche Folgeauftrag](../visuals/f1-layouts/NEXT-CHAT.md)
bleibt als historische Abnahmegrundlage erhalten.

Der gemeldete Fehler beim linken Einklappen war im Mockup eine automatische
Raster-Neuplatzierung: Der Artikel wurde null Pixel breit. Explizite Spalten
beheben dies. Der dauerhafte Browserprüfer `tools/check_f1_design_h.cjs` verlangt
positive Artikelbreite und unveränderte Position/Breite der jeweils anderen Seite.
32 Hell-/Dunkel-/Größen-/Seitenkombinationen und zusätzliche Bedienfälle bestehen
unter Windows/Edge. Das ersetzt keine native Plattform- oder Accessibility-Abnahme.

Diese Entscheidung ergänzt ADR 004–006. Sie ersetzt weder deren Inhalts-/Formel-
und Tabellenverträge noch den globalen Workspace-/Menüentwurf außerhalb von F1.
