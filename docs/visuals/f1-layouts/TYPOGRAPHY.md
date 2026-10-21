# F1 H: CSS-Schriftrollen in der nativen Darstellung

Folgearbeit vom 21. Oktober 2026 auf Grundlage von `b0f7a12` und dem
[Artikel-/Aktionsabgleich](SECONDARY-ACTIONS.md). Referenz bleiben die
unveränderten H-Bilder sowie `layouts.css`, `refinements.css` und `final-h.css`.
Der gesamte [native Designauftrag](../unified-workspace/NATIVE-FIDELITY.md)
ist damit noch nicht abgeschlossen.

Der nachfolgende [Artikel-/Zeilenabgleich](ARTICLE-FLOW.md) setzt die hier noch
offenen Zeilenboxen, Blockabstände, negative Titel-Laufweite und erste
Absatzrolle um. Seine aktuellen Prüfungen und verbleibenden Unterschiede sind
getrennt dokumentiert; die folgenden Ergebnisse bleiben die historischen
Nachweise dieses Schriftgrößenschritts.

## Änderung

Die Schriftgröße einer CSS-em-Box ist nicht dasselbe wie die von stb/Nuklear
angeforderte Höhe zwischen Ober- und Unterlänge. Beispielsweise hat die lokale
Windows-Schrift Segoe UI 2048 Einheiten je em und eine hhea-Spanne von 2724
Einheiten. Der Faktor beträgt daher 1,330078125. Eine unbereinigte Nuklear-Höhe
16 entspricht hier ungefähr 12 CSS-Pixeln. F1 hatte diese beiden Größen bisher
gleichgesetzt. Die Werkbank hatte die Metriken bereits vor dem Atlasaufbau erfasst.

F1 behält jetzt die Metriken jeder tatsächlich geladenen Primärschrift, solange
deren TTF-Daten noch vorhanden sind. Diese Werte werden zusammen mit den
Schriftbanken erst nach erfolgreichem Atlasaufbau übernommen. Ein fehlgeschlagener
Ladevorgang verändert weder die vorhandenen Banken noch ihre Metriken.
Beim Ersetzen oder Zerstören werden die zugehörigen Werte ebenfalls ersetzt
beziehungsweise zurückgesetzt.

Eigene persistente Nuklear-Fonthandles bilden daraus die Rollen der Vorlage:

| Rolle | CSS-Größe bei UI 16 / Leseschrift 16 |
| --- | --- |
| Artikeltext und Hervorhebung | 16 px |
| Haupttitel | 26,4 px; bei Leseschrift 32 entsprechend der großen H-Variante 41,6 px |
| Zweite / dritte Überschrift | 20 / 16,8 px |
| Codeblock / Inline-Code | 12,8 / 14,4 px, proportional zur gewählten Codegröße |
| Kopfsteuerung und Kontext-Tabs | 12 px |
| Themen / Abschnittssprünge | 13 / 12 px |
| Gruppenkategorie / kleine Beschriftung | 10 / 11 px |
| Kontextüberschrift / Kontexttext | 15 / 12 px |
| Artikelherkunft | 11,2 px, proportional zur Leseschrift |

Die Kopf-/Navigationsrollen folgen der UI-Größe und bleiben unabhängig vom
Lesezoom. Echte aktive Fettschrift, semantische Farben und die unabhängigen
Seitenzustände bleiben erhalten. Die Codebank wird unmittelbar für die
Darstellungsgröße der H-Codeblöcke gebacken. Das vermeidet die nachträgliche
Verkleinerung größerer Glyphen bei Codegrafiken. Rahmenzeilen verwenden denselben
gemessenen und gezeichneten Font sowie einen ganzzahligen logischen Glyphenabstand,
weil `nk_draw_text` seine Textpositionen ganzzahlig speichert.

Der Formelsatz verwendet ausdrücklich die gewählte Leseschriftgröße und die
reale Pixeldichte. Die Kalibrierung einer Textschrift verändert damit nicht
versehentlich die Variantenauswahl, Texturberechnung oder Größe der TeX-Formeln.
Die vorhandenen Formelquellen und gerasterten Formelassets bleiben unverändert.

## Fehlergegenfälle des ersten Ansatzes

Stand 64 fragte die Fontmetriken zu spät ab. `nk_font_atlas_cleanup` hatte die
TTF-Blobs bereits freigegeben; der Metrikhelfer lieferte deshalb seinen Ersatzwert
1. Die erste Rollenprüfung allein erkannte diese falsche Grundlage nicht.
Die tatsächliche Bildsichtung zeigte insbesondere zu kleine Bedienbeschriftungen.
Die neue Prüfung fordert zusätzlich die aufbewahrten Primärmetriken und liest
die tatsächlich eingereihten Titel-Zeichenbefehle aus.

Die Mathematikprüfung erkannte unterbrochene Unicode-Rahmen nach der Verkleinerung
größer gebackener Codeglyphen. Ganzzahliger Zeilenabstand allein genügte in Stand
65 nicht; die Pixelprüfung blieb rot. Die Codebank in Stand 66 wird für ihre
tatsächliche H-Größe gebacken. Die verbleibende Naht liegt an den geglätteten
Randzeilen der Glyphen: Im tatsächlichen Windows-Bild erreichen die Verbindungen
nur 69–70 statt etwa 220 Helligkeitswerten. Stand 67 lässt die vollständigen
Glyphen deshalb um eine logische Randzeile überlappen. Die Geometrieprüfung
berücksichtigt diese Überlappung. Unverändert bleiben die Pixelanforderung an
jede Zeile, die Helligkeitsschwelle 80, der Nachweis der schließenden Querlinie
und das gezielt unterbrochene Gegenbeispiel. Die Pixelprüfung wird nicht durch
eine reine Metadatenprüfung ersetzt.

## Ausgeführte Prüfungen und Aufnahmen

[Prüfdaten mit Quellständen, Binärhashes, Befehlen und Bildherkunft](typography-2026-10-21/validation.json)
halten positive und negative Läufe getrennt fest. Stand 68 unterscheidet sich
von Stand 67 ausschließlich in zwei Testdateien: Die native Zusatzprüfung
berücksichtigt die Überlappung; die unveränderte Rollenprüfung ist als separat
aufrufbare Python-Funktion ausgegliedert. Die Darstellungslogik ist identisch.

| Plattform / Lauf | Tatsächliches Ergebnis |
| --- | --- |
| Windows, `run-uyz9en3h` (67) | 8/11: Fenster, Tastatur, Verlauf, Zoom, Fonttransaktionen, Dichte, Detailtext und Inline-Formeln bestanden. Mathematik stoppt an der alten Zusatzprüfung. Seiten und H scheitern weiterhin am Hostfokus. |
| Windows, `run-5zn2k9w1` (68) | Mathematik 1/1 vollständig bestanden, mit Plattform- und gebündelten Fonts. Damit neun verschiedene Prüfgruppen positiv bei unveränderter Darstellungslogik; kein bestandener aktueller Windows-Seiten-/H-Fokusablauf. |
| Linux, `run-vfnj6p6z` (67) | 10/11 bestanden einschließlich H; Mathematik stoppt ausschließlich an derselben alten Zusatzprüfung. |
| Linux, `run-fook4ps4` (68) | Mathematik 1/1 vollständig bestanden. Damit alle elf verschiedenen Prüfgruppen über zwei Läufe bei unveränderter Darstellungslogik positiv geprüft. |

Vor der Testanpassung bestand die Pixelprüfung bereits auf den tatsächlichen
67er-Rahmenaufnahmen beider Plattformen. Nach der Anpassung besteht zusätzlich
der vollständige native Mathematikablauf mit seinen Sichtbarkeits-, Glyphen-,
Link- und Font-Gegenprüfungen. Er umfasst 185 Seiten, 1082 Links sowie
183 Zeichen in 42 Fontvarianten; auch der ausdrücklich erzwungene gebündelte
Fontpfad wird vollständig ausgeführt. Die 48 Inline-Fälle, Größen- und
Themewechsel bestehen im breiten 67er-Lauf.

Die eigenständige Rollenprüfung besteht auf allen 56 tatsächlich aufgenommenen
H-Zuständen beider Plattformen, einschließlich der eingereihten Titelbefehle.
Der erste falsche Stand 64 wird wegen fehlender aufbewahrter Primärmetriken
abgewiesen. Diese begrenzte Gegenprüfung macht den fehlgeschlagenen
Windows-Fokusablauf nicht erfolgreich. Der direkte Vergleich freier H-Flächen
besteht in Hell/Dunkel auf beiden Plattformen jeweils elf Prüfungen; er bewertet
keine vollständige Typografie oder Oberfläche.

Die 20 PNGs sind unveränderte, verlustfreie RGB-Konvertierungen nativer BMPs
beziehungsweise eine verlustfreie Übernahme der tatsächlichen Dichte-PNGs.
Die Prüfdaten nennen Ursprung, Lauf und Pixelhash. Aktuelle Beispiele:

- [Windows, normale H-Ansicht](typography-2026-10-21/windows-h-dark.png),
  [Linux, normale H-Ansicht](typography-2026-10-21/linux-h-dark.png).
- [Windows, großer Formelsatz](typography-2026-10-21/windows-math-32.png),
  [Linux, großer Formelsatz](typography-2026-10-21/linux-math-32.png).
- [Windows, durchgehende Codegrafik](typography-2026-10-21/windows-grid-32.png),
  [Linux, gebündelte Fonts](typography-2026-10-21/linux-bundled-grid-32.png).
- [Mindestfenster](typography-2026-10-21/windows-h-minimum.png),
  [große Schrift](typography-2026-10-21/windows-h-large.png),
  [tatsächlicher doppelter Framebuffer](typography-2026-10-21/linux-density-200.png).
- Gegenfälle: [zu späte Metrikabfrage](typography-2026-10-21/counterexample-64-metric-fallback.png),
  [sichtbare Rahmennaht](typography-2026-10-21/counterexample-66-grid-seams.png).

## Speicherpflege

Nach erfolgreicher Gegenprüfung wurden 762 abgelöste automatische Aufnahmen
aus erfolgreichen Prüfgruppen der Stände 64–66 entfernt: 1.779.577.914 Bytes,
rund 1,78 GB. [Umfang und Dateihashes](typography-2026-10-21/cleanup.json).
Die aktuellen 67er-/68er-Ausgaben, sämtliche H-/Mathematikversuche, negative
Befunde, öffentliche Referenzbilder, Protokolle, Prüfdaten, Quellen und
Abhängigkeiten bleiben erhalten. Es wurden ausschließlich erzeugte
`docs-*.bmp`-/`docs-*.png`-Dateien direkt in den ersetzten Testabläufen gelöscht.

## Noch erforderlicher Abgleich

Diese Änderung gleicht Schriftgrößen und ihre Skalierungsgrundlage an. Eine
vollständige H-Abnahme benötigt weiterhin die Zeilenboxen, Absatz- und
Überschriftenabstände, Laufweiten, Kategorien, Einzüge sowie alle Zustände.
Insbesondere die negative Titellaufweite und die gesonderte erste Absatzrolle
sind noch zu übertragen. Auch die tatsächliche Glyphenrasterung ist gesondert
mit den Referenzen zu vergleichen. Diese Unterschiede sind weitere Restarbeit.

Windows verwendet hier Segoe UI / Segoe UI Semibold; Linux verwendet die
tatsächlich vorhandenen beziehungsweise gebündelten Primärschriften. Identische
Glyphenformen über Plattformen hinweg sind deshalb nicht belegt. Dafür müssten
Vorlage und Laufzeit denselben verfügbaren Font verwenden. Der aktuelle
Windows-Hostfokus, macOS, physische Monitor-/DPI-Wechsel und externe Screenreader
benötigen weiterhin eigene Nachweise.
