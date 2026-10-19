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

## Ergänzung vom 19. Oktober: Daten, Größen und Modellwerte

Die Aufteilung und Abschnitte sind vom Nutzer bestätigt. Für die Informationen
innerhalb der Abschnitte hat der Nutzer **B · Wertefelder** ausgewählt: Bezeichnung
über größerem Wert, dezente einzelne Flächen und klar zugeordnete Einheiten.
Die Vorschau startet mit B; A/C bleiben nur Vergleich. Alle Inhaltsbereiche sollen resizable sein. Modellwerte
wie Fadenlänge sollen im Inspektor bearbeitbar sein und anschließend eine sichtbare
Neukompilierung anbieten. Laufende Daten bleiben bis zu einem neuen Build und
neuen Lauf an ihren bisherigen Stand gebunden. Der konkrete native Bindungs-,
Build- und Größenvertrag steht in [MODEL-EDITING.md](../visuals/simulation-designs/MODEL-EDITING.md).
