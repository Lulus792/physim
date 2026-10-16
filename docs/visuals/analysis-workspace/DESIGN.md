# Analysebereich · Entwurf 01

Stand: 16. Oktober 2026. Vorschlag zur Gestaltung, noch keine vom Nutzer ausgewählte
Produktentscheidung und keine native Implementierung.

[Interaktive Vorschau](index.html) · [Dunkel](analysis-dark.png) ·
[Hell](analysis-light.png) · [Laufvergleich](analysis-comparison.png) ·
[Analyseergebnis](analysis-report.png)

## Ziel und Grundlage

Die Hauptaufgabe lautet: einen gespeicherten Messlauf verstehen und mit anderen
Läufen vergleichen. Die Kurve erhält die größte Fläche; Laufauswahl, Kanal,
Einheit und Herkunft bleiben erreichbar. Grundlage sind der lokale Projektstand,
[die bestehende Designrichtung](../../design-direction.md),
`app/design_tokens.h`, die Auswertungsansicht in `app/design_ui.inc` und
[die vorhandenen Berichtsfunktionen](../../reports.md).

Die kompakte Menüzeile und die drei gleich breiten Arbeitsbereich-Tabs bleiben
Teil der App-Shell. Der Entwurf verwendet solide Flächen, Systemschrift und die
vorhandenen Dunkel-/Hell-Farbrollen. Er benötigt weder Glasmaterial noch zusätzliche
Schrift-, Icon- oder Netzwerkabhängigkeiten. Alle Zeichen und Diagramme sind lokal.

## Aufbau

| Bereich | Inhalt und Verhalten |
| --- | --- |
| Arbeitsbereich-Kopf | Titel, aktuelle Auswahl, Analysecode und eine Hauptaktion: Analyse starten bzw. ausgewählte Läufe vergleichen. |
| Links | Suchbare Messläufe mit Dauer, Punktzahl und unterscheidenden Parametern. Checkboxen ermöglichen Mehrfachauswahl. |
| Mitte | Tabs Messdaten und Analyseergebnis. Messdaten zeigen Kanal, beschriftete Kurve, Zoom, Statistik und CSV-Export. |
| Rechts | Kanal und Anzeigeeinheit, Details des zuletzt ausgewählten Laufs und Herkunft. Der aktive Lauf ist ausdrücklich benannt. |
| Unten | Aktionsstatus sowie sichtbare Kennzeichnung der Vorschau mit Beispieldaten. |

Regelbreiten: links 232 px, rechts 242 px; Mitte flexibel. Bis 1200 px werden die
Seiten auf 210/220 px reduziert. Unter 1000 px ist der Inspektor zunächst
eingeklappt und kann statt der Laufliste geöffnet werden. Bis 760 px zeigt die
Vorschau jeweils eine Arbeitsfläche; beide Kopfaktionen bleiben erreichbar.
Diese Browser-Breakpoints sind Entwurfswerte, keine neue native Mindestgröße.
Auf breiten Fenstern lassen sich links und rechts unabhängig ein-/ausblenden.
Ein Themewechsel verändert die Auswahl und Seitenzustände nicht.

## Interaktion und Datenvertrag

- Einzel- und Mehrfachauswahl verwenden dieselbe Arbeitsfläche. Verschiedene
  Läufe bekommen Farbe, Laufnummer und Linienmuster. Der erste dargestellte Lauf
  ist als Referenz gekennzeichnet; eine numerische Differenzberechnung wird damit
  nicht behauptet. Die Vorschau hat drei Beispielläufe; die native Obergrenze
  bleibt der bestehende Vertrag von bis zu acht Eingaben.
- Die Statistik bezieht sich ausdrücklich auf den gesamten Lauf. Zoom verändert
  weder Statistik noch Exportumfang. In der Vorschau zoomen die beschrifteten
  Aktionen um die Zeitachsenmitte; die native App behält zusätzlich ihre
  vorhandenen Maus-/Pan-Interaktionen und kanalbezogenen Ausschnitte.
- Der Kanalwechsel aktualisiert Kurve, Einheit und Statistik. Der Einheitenwechsel
  konvertiert alle dargestellten Werte und den CSV-Export gemeinsam. Mittelwert,
  Stichprobenstandardabweichung, Minimum und Maximum werden aus denselben
  synthetischen Messpunkten berechnet, die die Kurve zeichnet.
- Der CSV-Download der Vorschau enthält alle 1001 synthetischen Messpunkte pro
  gewähltem Lauf. Die native Umsetzung muss vollständige Laufdaten verwenden und
  eine etwaige reduzierte Bildschirmvorschau weiterhin kennzeichnen.
- Analyse starten öffnet das Ergebnis und zeigt einen abbrechbaren Arbeitszustand.
  Die Vorschau berechnet nach kurzer Wartezeit eine Beispielstatistik; sie startet
  weder Compiler noch Analyseprozess und erzeugt keine `.psreport`-Datei.
- Ein Ergebnis behält seine Laufauswahl, Kennzahlen und Einheit. Ändert sich die
  Laufauswahl, wird die Abweichung im Bericht sichtbar und eine neue Analyse
  angeboten. Der Messkanal im Inspektor betrifft weiterhin die Messdaten; er
  schreibt keinen bestehenden Bericht um.
- Analysecode öffnet in der Vorschau einen Dialog, der den Übergang erläutert.
  Bei nativer Umsetzung führt diese Aktion zum vorhandenen Analyseeditor und
  erhält dessen Speichern-/Dirty-Verhalten.

Der native Laufimport soll wieder **Messlauf importieren …** heißen und den
vorhandenen Dateidialog verwenden. Die Vorschau heißt bewusst
**Beispielmesslauf laden**: Sie aktiviert ausschließlich lokale Beispieldaten.
Die globale Menüleiste und die inaktiven Arbeitsbereichsnamen bilden nur den
Kontext ab. Die Vorschau greift nicht auf Projekte oder das Dateisystem zu.

## Zustände

| Zustand | Rückmeldung und nächste Aktion |
| --- | --- |
| Ohne Messlauf | Erklärung, Simulation bzw. Import als nächster Schritt; in der Vorschau Beispieldaten laden. |
| Keine Auswahl | Auswahlaufforderung; Analyse deaktiviert. |
| Laden | Klar benannter Ladevorgang, keine leere Kurvenfläche mit scheinbar gültigen Werten. |
| Ladefehler | Lesbare Ursache und erneuter Versuch. |
| Messdaten bereit | Kurve, vollständige Statistik, Herkunft und Export. |
| Vergleich | Mehrere gekennzeichnete Kurven und eine Statistikzeile pro Lauf. |
| Analyse läuft | Ergebnisfläche mit Status und Abbrechen; vorhandene Messdaten bleiben verfügbar. |
| Ergebnis bereit | Kennzahlen, Ergebnistabelle und dokumentierte Herkunft. |
| Auswahl nach Bericht geändert | Bericht bleibt an seine Eingaben gebunden; neue Analyse wird angeboten. |
| Suche ohne Treffer | Eigene Rückmeldung, Auswahl bleibt erhalten. |

## Native Übergabe

Der lokale C-/Nuklear-Code bleibt Grundlage. Dieser Auftrag ändert ausschließlich
den Designprototyp. Bei Umsetzung bestehende Lade-/Jobzustände und Funktionen
wie `analyze`, `report_view`, `channel_display_unit`, `plot`, `open_library` und
`run_details_open` anbinden. Die linke Laufliste ergänzt bzw. integriert den
vorhandenen Bibliotheksweg; Workspace-Dateien müssen über die bestehende Navigation
weiter erreichbar bleiben. Herkunft und Modell-/Laufdetails dürfen in der Umsetzung
gegenüber dem aktuellen Stand nicht verloren gehen.

Der Bericht in der Vorschau ist nur ein Beispiel. Native `.psreport`-Inhalte können
eigene Diagramme, Histogramme und Tabellen haben; sie behalten den vorhandenen
Renderer und alle PNG-/SVG-/CSV-Exportaktionen einschließlich Ausschnitt und
Pixelgrößenwahl. Der Entwurf stellt diese Funktionen nicht vollständig dar.
Maskierte Sensordaten, rekonstruierte Läufe, numerische Fehler und reduzierte
Vorschauen benötigen ihre bestehenden Statushinweise. Die Beispielkurven allein
sind keine Abnahme dieser fachlichen Fälle.

Für Windows, macOS und Linux gelten dieselben Inhaltsrollen. Fenstertasten,
Menüintegration, Dateidialoge, Shortcuts und Assistive-APIs bleiben plattformspezifisch.
Die abgebildeten Fenstertasten sind Darstellung, keine Betriebssystemsteuerung.
Der native Hochkontrastmodus muss zusätzlich zum hier gezeigten Hell/Dunkel erhalten
bleiben. Browser-Forced-Colors verwendet Systemfarben und Linienmuster.

## Ausgeführte Prüfung

Windows, Edge 155.0.4283.45, Playwright, 16. Oktober 2026:

```text
node tools/check_analysis_design.cjs --export
Exitcode 0
```

Node und Playwright über `load_workspace_dependencies` ermitteln; den gelieferten
Modulpfad als `NODE_PATH` setzen. Ohne `--export` gehen Bilder und `review.json`
nach `build/analysis-design-review/`; mit `--export` werden die vier oben verlinkten
Designbilder erneuert. Die Designbilder sind absichtlich versionierte Referenzen.

- Acht Layoutfälle: 1440/1080/760/500 px, jeweils Hell und Dunkel; kein horizontaler
  Seitenüberlauf, sichtbare Diagrammfläche.
- Laufvergleich, Suche, Kanal-/Einheitenwechsel, vollständiger CSV-Download,
  Analyse/Abbruch und Herkunft eines vorhandenen Berichts geprüft.
- Beide Seitenleisten unabhängig geprüft; auch Wiederöffnen nach Wechsel von einem
  breiten Fenster mit geschlossenen Seiten in die kompakte Ansicht.
- Ergebnistabs per Pfeiltasten/Home/End, Dialog per Escape und Fokuskehr zum
  Auslöser geprüft. Native Buttons, Checkboxen und Selects bleiben per Tab erreichbar.
- Acht Text-/Flächenkontraste berechnet: 4,77:1 bis 15,62:1. Dies deckt die
  ausgewählten Textrollen ab, keine vollständige Accessibility-Zertifizierung.
- 200-%-Vergrößerung als Sichtbarkeits-Smoke-Test der Hauptaktion geprüft; eine
  vollständige Abnahme aller Inhalte bei vergrößerter Schrift bleibt offen.
- Keine JavaScriptfehler. Dunkel-, Hell-, Vergleichs- und Ergebnisbild visuell
  auf Hierarchie, Abstände, Beschriftungen und überlappende Inhalte geprüft.

Keine neue native Windows-, Linux- oder macOS-Ausführung. Screenreaderbedienung,
echte DPI-/Monitorwechsel, Hochkontrast, große reale Datenbestände und die komplette
Desktop-Matrix bleiben Teil der späteren nativen Umsetzung und Abnahme.

## Entwurfsreferenzen

Am 16. Oktober 2026 wurden die offiziellen Markdown-Fassungen dieser Apple-HIG-
Kapitel gelesen. Die konkreten Physim-Aufteilung und Maße sind eigene Ableitungen:

- [Split views](https://developer.apple.com/design/human-interface-guidelines/split-views):
  benachbarte Inhaltsflächen, sichtbare Auswahl, dünne Trenner und wiederherstellbare
  ausgeblendete Bereiche.
- [Charts](https://developer.apple.com/design/human-interface-guidelines/charts):
  Linien für zeitliche Verläufe und beschriftete Achsen als Interpretationshilfe.

Die Physim-spezifischen Daten-/Exportverträge haben Vorrang vor der Vorschau.
