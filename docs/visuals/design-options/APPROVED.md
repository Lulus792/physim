# Verbindlich ausgewähltes Design

**Nutzerfreigabe: 25. Oktober 2026.** Der Nutzer hat die Leeransicht bestätigt
und pro Thema die folgenden Varianten aus der getrennten Galerie ausgewählt.
Diese Entscheidung ersetzt „Auswahl offen“ und die frühere Übernahmesperre.
[ADR 011](../../decisions/011-selected-workspace-additions.md) hält sie fest.

[Gemeinsame ausgewählte Referenz](index.html?approved=1&topic=empty) ·
[Vollständige freigegebene Leeransicht](index.html?approved=1&topic=empty&standalone=1)

| Bereich | Auswahl | Direkte Referenz | Ausgewähltes Referenzbild |
| --- | --- | --- | --- |
| Ohne Projekt | Bestätigte Leeransicht | [Öffnen](index.html?approved=1&topic=empty&standalone=1) | [Entwickeln](review/empty-develop-dark.png), [Simulieren](review/empty-simulate-dark.png), [Auswerten](review/empty-analyze-dark.png) |
| Protokoll | **C · Navigator** | [Öffnen](index.html?approved=1&topic=protocol) | [C](review/protocol-c.png) |
| Szene & Werte | **A · Werkbank** | [Öffnen](index.html?approved=1&topic=simulation) | [A](review/simulation-a.png) |
| Auswertung | **A · Werkbank** | [Öffnen](index.html?approved=1&topic=analysis) | [A](review/analysis-a.png) |
| Läufe und Serien | **A · Werkbank** | [Öffnen](index.html?approved=1&topic=runs) | [A](review/runs-a.png) |
| Neues Projekt | **B · Fokus** | [Öffnen](index.html?approved=1&topic=project) | [B](review/project-b.png) |
| Datei-Explorer | **A · Werkbank** | [Öffnen](index.html?approved=1&topic=files) | [A](review/files-a.png) |

## Umsetzung für den anderen Chat

- Den aktuellen nativen Stand weiterführen. Die Freigabe betrifft das finale Design;
  sie behauptet keine bereits umgesetzte Datei-/Projekt-/Runnerfunktion dieser
  HTML-Beispiele. Bestehende native Funktionen wiederverwenden und abgleichen.
- **Protokoll C:** Herkunftsnavigation, Meldungsliste und zugehöriger Detail-/
  Originaltextbereich. Die gemeinsamen Anforderungen aus dem älteren
  [Protokollvertrag](../unified-workspace/PROTOCOL-DESIGN.md) gelten für Filter,
  Originaltext, echte Quellstellen und Auswirkungen weiter. Bei der Anordnung
  ist die ausgewählte Variante C maßgeblich. Fehler-Level allein stoppt keinen Lauf.
- **Szene & Werte A:** Szenenbaum, Arbeitsfläche und feste Eigenschaften;
  Kraft-/Geschwindigkeitsmaßstäbe getrennt halten, Texte, Typen, Parameter,
  adaptive Einstellungen und Kanäle aus dem echten Modell beziehen. Die
  Beispielzuordnung von Kräften erteilt einem beliebigen `PS_ARROW` keine
  Newton-Semantik; Metadaten-/Messkanalzuordnung ausdrücklich definieren.
- **Auswertung A:** Laufbibliothek, variable Berichtsinhalte und Herkunft in der
  Werkbank. Bestehende native Diagramme, Statistik, Berichte und die bereits
  freigegebene Exportgestaltung erhalten. Kein fest verdrahteter Pendelbericht.
- **Läufe/Serien A:** kompakte Lauftabelle mit Status, Teilfehlern, ausgewähltem
  Ergebnis, Zeitleiste und Studieneinstellungen. Aufzeichnungen/Fehler anderer
  Läufe bleiben bei einem Teilfehler oder Abbruch erhalten.
- **Neues Projekt B:** Startpunkt → Projektangaben → Prüfen. Eingaben beim
  Zurückgehen erhalten, Experiment-/Analysesprache unabhängig, acht vorhandene
  Modellvorlagen, optional reines Analyseprojekt, echter Zielordner und überprüfbare
  Erstellungsfehler. Der native Abschluss legt das wirkliche Projekt an.
- **Explorer A:** Orte, Dateiliste und feste Vorschau; dieselbe Gestaltung für
  freie Dateinavigation, Messlaufauswahl und Ordnerwahl im Projektdialog.
  Reale Pfade, Berechtigungen, lange Namen, externe Änderungen und fehlende
  Dateien mit den vorhandenen nativen Adaptern behandeln.
- **Leeransicht:** alle drei Arbeitsbereiche ohne geladene Inhalte, passende
  deaktivierte Build-/Lauf-/Analyse-/Exportaktionen und erreichbare Ordner-/
  Projektaktionen. Kein automatisch geladenes Demoexperiment.

Die gemischte Auswahl gilt auch für Unteraufrufe: **Protokoll C → Neues Projekt B
→ Datei-/Ordner-Explorer A**. Ein globaler Stilwert darf diese Zuordnung nicht
überschreiben. Die ausgewählte Browserreferenz demonstriert genau diesen Ablauf.

Die bestehenden gemeinsamen Resizing-, Fokus-, Tastatur-, Theme- und
Materialkonventionen weiterverwenden. A/B/C-Auswahlleisten und reine
Demo-/Vergleichshinweise gehören nicht in die produktive Anwendung.

## Verhältnis zu bisherigen Quellen

Die Gesamtgrundlage bleibt [NEXT-CHAT.md](../unified-workspace/NEXT-CHAT.md).
Diese Auswahl ergänzt die gewählten drei Arbeitsbereiche und entscheidet die
neuen Inhalts-/Dialogvarianten. Bei widersprüchlichen Varianten-/Wartestatusangaben
ist diese direkte Nutzerentscheidung maßgeblich. Die alte Galerie mit allen
18 Entwürfen und ihre Prüfbelege bleiben als Vergleichsstand erhalten.

Der [Grafikexport](../unified-workspace/EXPORT-EXAMPLE.md) und
[ADR 010](../../decisions/010-analysis-graphic-export.md) bleiben unverändert
freigegeben. Die alten Browserprüfungen sind historische Gestaltungsbelege,
keine native Abnahme. Neue Umsetzungsbelege müssen die echten nativen Abläufe
und die tatsächlich geprüften Plattformen benennen.

Die [geprüfte Kombination](review/approved/VALIDATION.md) dokumentiert die
bereichsübergreifenden Aufrufe und den Rücksprung in das ursprüngliche Layout.
