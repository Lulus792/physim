# Inhaltsinventar: Was die Demo noch nicht vollständig zeigt

Stand: 24. Oktober 2026. Nutzerauftrag: Inhalte ermitteln, die die Gestaltung
beeinflussen, besonders Kräfte und Text in Simulieren. Grundlage ist der lokale
Projektstand bei `7faf5e2` einschließlich vorhandener laufender nativer Arbeiten.
Diese Liste bewertet **die Abdeckung der HTML-Demo**, nicht die Fertigstellung
der nativen Anwendung. Bestehende native Funktionen sind dadurch nicht erneut
zu implementieren. Kein vollständiges Inventar aller Physikalgorithmen.

**P1** = für das fertige Layout zuerst berücksichtigen; **P2** = anschließend
mit repräsentativen Daten ergänzen. „Teilweise“ bedeutet: Bedienelemente oder ein
Beispiel existieren, der tatsächliche Inhaltsumfang fehlt aber.

## Simulieren

| Prio | Fehlender oder nur teilweise gezeigter Inhalt | Lokaler Nachweis / tatsächlicher Stand | Einfluss auf das Design |
| --- | --- | --- | --- |
| P1 | Mehrere Kräfte neben Geschwindigkeit | `examples/pendulum/main.c`, Funktion `scene`: Gewicht und Stangenkraft, Luftwiderstand bei aktivem Widerstand. Demo hat nur Geschwindigkeitsvektor. | Überlagerte Pfeile, unterscheidbare Namen/Symbole, Einzel-Sichtbarkeit und verständliche Legende berücksichtigen. Kräfte nicht mit Geschwindigkeiten gleich skalieren. |
| P1 | Textbeschriftungen in der Szene | Dasselbe Beispiel: Aufhängung, Pendelmasse und Krafttexte; `PS_LABEL`, `ps_object.text[64]` in `include/physim/experiment.h`. Demo hat keine modellgetriebenen Labels. | Text über Geometrie, lange/mehrsprachige Namen, Clipping und Überschneidungen; ein Text-Sichtbarkeitsschalter und Eigenschaften für Beschriftungen. |
| P1 | Mehr als die fünf festen Demoobjekte | `PS_SPHERE`, `PS_LINE`, `PS_BOX`, `PS_ARROW`, `PS_POINT`, `PS_PLANE`, `PS_POLYLINE`, `PS_LABEL`, `PS_GROUP`, `PS_FRAME`; maximal 32 Objekte, 96 Szenenpunkte im aktuellen Vertrag. | Baum und Inspektor müssen sich nach Typ/Inhalt füllen: Formwerte, Text, Pfadpunkte, Gruppen und Koordinatenrahmen. Nicht jedem Objekt Masse zuordnen. |
| P1 | Hierarchie, lokale Koordinaten, wechselnde Objekte | `ps_scene_group/frame/set_parent`, stabile optionale IDs in `experiment.h`; `app/scene_tree_ui.inc`, `app/scene_view.h`. Demo flacher, fester Baum. | Einrückung, Auf-/Zuklappen, Auswahl nach ID, unsichtbare Eltern, verschwundene Auswahl und lokale/Weltwerte auseinanderhalten. |
| P1 | Perspektive/2D/3D mit beliebiger Geometrie | `app/laboratory_camera.inc`, `app/laboratory_scene.inc`; API mit Position, Orientierung, Transparenz. Demo zeigt einen planaren Pendelversuch. | Z-Positionen, Blickrichtungen, Tiefenüberdeckung, transparente Flächen und Beschriftungen außerhalb des Bilds prüfen. |
| P1 | Mehr und andere Modellparameter | Pendel definiert Länge, Anfangswinkel, Masse, Luftdichte, Widerstandskoeffizient, Fläche, Sensorrauschen und Integrator. Demo bearbeitet vor allem Länge/Masse. | Variable Kartenanzahl, längere Beschreibungen, SI-/Anzeigeeinheiten, Min/Max/Standardwerte, Integerwahl und Kombinationen mit Fehlermeldung. Native Grenzen statt Demo-Grenzen verwenden. |
| P1 | Adaptive Schritte und abhängige Einstellungen | `app/laboratory_fields.inc`: Unterstützungshinweis, Minimum/Maximum und Startschritt. Pendel unterstützt mehrere Integratoren und lehnt z. B. Verlet mit Geschwindigkeitswiderstand ab. | Zusätzliche Felder/Erklärung und längere Validierungstexte ohne verdeckten Scrollbereich. Demo-Änderungen sind kein Solverlauf. |
| P1 | Beliebige Messkanäle und Livewerte | Aktuell bis 16 Kanäle (`experiment.h`). Pendel: Winkel, Winkelgeschwindigkeit, Position x/y, Energie, verrauschter Sensorwinkel, Geschwindigkeit x/y/Betrag, dissipierte Energie, Energiebilanz. Demo zeigt nur eine kleine feste Auswahl. | Lange Kanalnamen/Beschreibungen, negative/exponentielle/fehlende Werte, beliebige kompatible Einheiten und unterschiedliche Kurvenbereiche. |
| P1 | Echte Modellmeldungen, Laufabbrüche und Warnungen | `log.h`, `diagnostic.h`, `app/main.c`, `app/laboratory_tail.inc`: aktuelle Ereignisse, letzte Warnung, ausgelassene Meldungen. Alte Demo-Ereignisse sind statisch; neuer Protokollentwurf hat bewusst getrennte Beispiele. | Dauerhafter Hinweis auch bei zugeklappter Gruppe; Lauf + Modellzeit + Ursache, kein Abbruch allein durch Log-Level. Siehe [Protokollentwurf](PROTOCOL-DESIGN.md). |
| P1 | Lauf wird gestartet / keine Runner-Antwort / Limits / Aufzeichnungsfehler | Native Prozess-/Laufzustände in `main.c`, `app/laboratory_transport.inc`, `app/laboratory_tail.inc`; Demo simuliert lediglich Zustände. | Verzögerter Start, Stop/Abbruch, Speicher-/Zeitgrenze, Schreibfehler und tatsächlich erhaltene Daten unterschiedlich benennen. |
| P2 | Reale gespeicherte Zeitleisten und unvollständige Aufzeichnungen | `app/timeline.*`, `include/physim/run_index.h`, `run_stream.h`: Messungen/Szenen, Index, validierter wiederhergestellter Präfix. Demo hat nur kurzen synthetischen Verlauf. | Lange Dauern, nicht verfügbare Szenen, Ende/Live getrennt, Warnung „unvollständig/wiederhergestellt“, keine erfundenen Zwischenbilder. |
| P2 | Laufserien und Parameterstudien | `app/batch_ui.inc`, `app/batch_actions.inc`: Seeds, Parametervariation, Endkanalvergleich. Nicht in der gemeinsamen Drei-Bereich-Demo abgebildet. | Eigener Inhalts-/Navigationsumfang, Fortschritt, Teilfehler, Abbruch und Ergebnislisten; nicht kommentarlos entfernen. |

### Kräfte: fachlich wichtiger Unterschied

Im echten Pendel verwenden Kraftpfeile **0,05 m/N**, der Geschwindigkeitspfeil
einen eigenen Maßstab. Luftwiderstand ist standardmäßig deaktiviert; erst positive
Luftdichte, Koeffizient und Fläche erzeugen diesen Pfeil. Ein weiterer Pfeil ist
also nicht automatisch ein Fehler oder immer sichtbar.

Die öffentliche Szenenstruktur kennt **Geometrie und Beschriftungen**, aber kein
generisches Feld „dies ist eine Kraft in Newton“. Aus einem `PS_ARROW` lässt
sich nur seine räumliche Geometrie ablesen. **Keine Newtonwerte aus Pfeillängen
erfinden.** Für eine allgemeine Kraftlegende mit Betrag/Einheit braucht man einen
ausdrücklich zugeordneten Messkanal bzw. definierte zusätzliche Metadaten.
Diese Zuordnung ist ein **neuer Design-/Datenvertrag**, keine bereits belegte API.

## Entwickeln und Protokoll

| Prio | Fehlender oder teilweise gezeigter Inhalt | Lokaler Nachweis / tatsächlicher Stand | Einfluss auf das Design |
| --- | --- | --- | --- |
| P1 | Mehrere echte Compilerdiagnosen und Warnungen | `app/compiler_diagnostic.*`, `app/design_ui.inc`, `app/diagnostic_tests.inc`; bisheriger Demo-Build kennt nur einen absichtlich eingebauten Fehler. | Mehrere Dateien/Zeilen/Spalten, Compilerformate, volle Originaltexte, Fehler vs. Warnungen; neuer [Protokollentwurf](PROTOCOL-DESIGN.md) ergänzt die Referenz. |
| P1 | Dateioperationen mit echter Projektstruktur | `app/file_tabs_ui.inc`, `app/workspace_catalog*`, `app/main.c`; Demo hält drei feste Dateien nur im Speicher. | Lange Pfade, viele Tabs, fehlende/extern geänderte Dateien, Speicherkonflikte, ungültige Namen und Fehler ohne Datenverlust. |
| P1 | Mehrere Sprachen und reale Buildprozesse | `include/physim/language_sdk.h`, `app/main.c`, Sprachbeispiele unter `examples/language`; Browser führt weder C noch `.phys` aus. | Sprachspezifischer Kontext, Buildprofil, Compiler-/Linkerausgabe, Fortschritt und Abbruch; Beispielerfolg nicht als nativen Build ausgeben. |
| P2 | Umfangreiche API-/Dokumentationsinhalte | `app/documentation*`, `app/design_ui.inc`; Demo hat eine begrenzte API-Karte/Gliederung. | Lange Signaturen, Tabellen, Code, Fehler-/Warnhinweise und keine Treffer im bestehenden Kontextfluss. Vorhandene F1-Referenz weiterverwenden. |

## Auswerten

| Prio | Fehlender oder teilweise gezeigter Inhalt | Lokaler Nachweis / tatsächlicher Stand | Einfluss auf das Design |
| --- | --- | --- | --- |
| P1 | Echte Laufbibliothek, Herkunft und Dateifehler | `app/analysis_ui.inc`, `include/physim/run_index.h`; Demo feste bzw. bis acht Sitzungsläufe. | Lange Namen/Pfade, unterschiedliche Metadaten, keine Treffer, Ladefortschritt, fehlende Dateien und unvollständige/wiederhergestellte Läufe sichtbar halten. |
| P1 | Unterschiedliche Kanalschemata/Einheiten mehrerer Läufe | `app/analysis_comparison.*`, `app/analysis_ui.inc`; Demo verwendet dasselbe kleine Pendelschema. | Inkompatible/fehlende Kanäle, Einheitenwahl, Legende und Statistik ohne vorgetäuschte Vergleichbarkeit. |
| P1 | Echte Ausgaben des Analysemoduls | `app/analysis_ui.inc`, `app/analysis_layout.inc` und native Analysepfade; Demo erzeugt einen festen Beispielbericht. | Variierende Texte, Tabellen, mehrere Diagramme, leere Ausgabe und Fehler samt nachvollziehbarem Quellbezug. Nicht alle Berichte auf die Pendelstatistik festlegen. |
| P2 | Sehr viele Messpunkte und Grenzwerte | Block-/Indexverträge `include/physim/run_index.h`; Demo begrenzte synthetische Punktzahl. | Lange Zahlen, kleine/große/skalenungleiche Werte, Zoom, sichtbarer Ausschnitt, Statistik und Kurvenausdünnung getrennt prüfen. |
| P1 | Reale Export-/Dateidialog-/Zwischenablagefehler | Verbindlicher [Grafikexport](EXPORT-EXAMPLE.md); die Browserreferenz bietet echte Downloads, aber keine native OS-Abnahme. | Abbrechen, Ziel nicht schreibbar, Bildkopie verweigert/fehlgeschlagen und erneuter Versuch. Freigegebenen Export vollständig erhalten. |

## Neue Vorschläge gegenüber belegtem Bestand

Die oben belegten Inhalte sollen mit realen Daten im fertigen Design Platz finden.
Zusätzlich **vorgeschlagen**, ohne vorhandene Unterstützung vorzutäuschen:

1. Semantische Kraft-/Geschwindigkeitslegende mit Namen, Symbol und separatem
   Maßstab; Werte nur bei definierter Messkanal-/Metadatenzuordnung.
2. Umgang mit kollidierenden Szenenbeschriftungen: lesbare Kontur/Hinterlegung,
   ausgewählten Text hervorheben, vollständigen Text im Inspektor anbieten.
   Nicht das automatische Anordnen aller Labels als bereits implementiert ausgeben.
3. Strukturiertes gemeinsames Meldungsmodell für Hostkontext, Schweregrad,
   optionale Quellposition und Originaltext, gespeist aus den vorhandenen Quellen.
   Die Browserbeispiele sind Gestaltungsbelege, kein Nachweis nativer Verarbeitung.

## Prüffälle für die Fortsetzung

- Ein Pendel mit Gewicht/Stangenkraft und eines mit aktivem Luftwiderstand;
  Geschwindigkeit und Kraftmaße auseinanderhalten, Nullpfeil und Label prüfen.
- Eine Szene mit allen zehn Typen, Gruppen/Koordinatenrahmen, langen Labels,
  Transparenz, verschwundener Auswahl und 32 Objekten; hell/dunkel, klein/groß.
- Parameter-/Kanalgrenzen aus dem tatsächlichen Modell beziehen; abhängige
  Solverfelder, ungültige Kombinationen und lange Einheiten fehlerfrei darstellen.
- Protokoll mit mehreren Fehlern/Warnungen, fehlender Quellstelle, Unicode,
  Flutunterdrückung und fehlgeschlagenem Kopieren; Filtersicht verändert keine Daten.
- Bibliothek mit vollständigem und wiederhergestelltem Lauf, inkompatiblen
  Kanälen, vielen Messpunkten, variabler Analyseausgabe und fehlgeschlagenem Export.

Nächster Schritt ist der native Abgleich mit diesen repräsentativen Inhalten,
ergänzt um gezielte Browserbeispiele nach Bedarf. Diese Inventarliste allein ist
keine native Design- oder Funktionsabnahme.
