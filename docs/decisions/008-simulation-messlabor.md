# Simulationsbereich: Messlabor als ausgewähltes Zielbild

Nutzerentscheidung vom 19. Oktober 2026: **Messlabor (Variante 02)** bildet die
Grundlage für den Simulationsbereich, mit folgenden anfänglichen Zuständen:

- Szenenbaum geschlossen, separat wieder erreichbar.
- Inspektor offen.
- Beide Messkurven zugeklappt; Messwerte und Zeitleiste sichtbar.
- Kurven über einen beschrifteten Schalter **Kurven anzeigen** ein-/ausblenden.
  Der bisherige kleine Richtungspfeil entfällt. Der Schalter zeigt Ein/Aus
  durch die Position seines runden Griffs, ergänzend durch den visuellen Zustand.

Die [interaktive Vorlage](../visuals/simulation-designs/index.html) startet mit
dieser Variante. Studio und Fokus bleiben Vergleichsentwürfe. Das Einblenden
der Kurven verändert weder Seitenzustände, Auswahl noch Simulationszeit; beim
Ausblenden erhält die Szene wieder die frei gewordene Höhe.

Die [Designspezifikation](../visuals/simulation-designs/DESIGN.md) und die Bilder
[Dunkel](../visuals/simulation-designs/labor-dark.png),
[Hell](../visuals/simulation-designs/labor-light.png) sowie
[Kurven eingeblendet](../visuals/simulation-designs/labor-dark-curves-open.png)
beschreiben das Zielbild. Die Schaltergestaltung ist der aktuelle Vorschlag für
den gewünschten Ersatz des Pfeils und kann weiter verfeinert werden.

Diese Entscheidung betrifft den Entwurf. Die native C-/SDL-/Nuklear-Anwendung
ist noch nicht umgestellt; die Vorlage ist kein Nachweis einer nativen
Windows-, Linux- oder macOS-Abnahme.
