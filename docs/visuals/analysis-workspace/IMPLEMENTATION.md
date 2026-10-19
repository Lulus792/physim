# Technische Übergabe: Analysebereich nativ umarbeiten

Stand: 19. Oktober 2026. Quellstand beim Abgleich: `ccc5bb9` auf
`codex/f1-design-h`. Diese Notiz ist ein Implementierungsauftrag, kein Nachweis
einer bereits erfolgten nativen Umstellung. Vor Arbeitsbeginn den inzwischen
aktuellen Stand und lokale Änderungen erneut prüfen.

## Verantwortlicher Umfang

Umsetzen: Anordnung und Interaktionen der bestehenden **Auswerten**-Ansicht nach
[DESIGN.md](DESIGN.md), einschließlich echter Mehrlaufvorschau. Die bestehende
App-Shell, der separate Analyseeditor und der fachliche Analyseprozess bleiben
Grundlage. Die HTML-Vorlage ist eine lokale visuelle und interaktive Referenz.

## Bestand und Ansatzpunkte

Alle Pfade in der Tabelle beziehen sich auf die Repositorywurzel. Funktionsnamen
sind die belastbareren Suchanker; Zeilennummern können sich verschieben.

| Vorhandene Stelle | Funktion im Bestand | Änderung für die Vorlage |
| --- | --- | --- |
| `app/design_ui.inc`: `workspace_panel` | Zweig für Auswerten, Analyseaktionen, Rohdaten, Statistik, `report_view` | Kopf und mittlere Fläche ordnen; Fachaktionen weiter aufrufen. Analysecode bleibt im vorhandenen Editorzweig. |
| `app/design_ui.inc`: `sidebar_panel`, `inspector_panel` | Workspace-/Dateinavigation; rechts bisher auch Kamera/Laufgrenzen | Analysebezogene Laufwahl und Kanal-/Laufdetails kontextuell einfügen. Die bisherigen Funktionen in anderen Arbeitsbereichen erhalten. |
| `app/main.c`: `app`, `load_dataset`, `request_dataset` | Ein Datensatz `a->data`, asynchrones Laden, begrenzte Vorschau und Statistik über alle gelesenen Samples | Begrenzte Vorschauen und Statistik mehrerer Läufe bereitstellen. Neue Auswahl-/Ladezustände mit expliziter Lebensdauer führen. |
| `app/library_actions.inc`: `select_run`, `reference_run`, `refresh_library`, `open_run_path`, `open_library_item` | Pfadbasierte Auswahl, Referenzreihenfolge, Bibliothek, Lauf-/Berichtöffnung | Gemeinsame Aktionen für neue Laufliste verwenden; aktive Vorschau und Mehrfachauswahl bewusst auseinanderhalten. |
| `app/library_ui.inc`, `app/library.h` | Bestehende Bibliotheksdarstellung und Katalog | Suche/Laufliste auf echte Katalogdaten aufbauen; `Läufe & Berichte` bleibt erreichbar. |
| `app/main.c`: `analyze`, `request_report_path`, `request_report` | Echte Analyse bzw. Rohdatenexport, Bericht laden | Aktionen umplatzieren; Eingaben beim Start festhalten und Bericht-Herkunft korrekt zuordnen. |
| `app/report_ui.inc`: `report_view`, `report_plot`, `report_export` | Ergebnisdarstellung und Exporte | In den mittleren Ergebnistab integrieren; bestehende Inhalts- und Exportmöglichkeiten erhalten. |
| `app/report_details.inc`, `app/run_details_ui.inc` | Analyseherkunft und vollständige Modell-/Laufdetails | Zusammenfassung im rechten Bereich; vollständige Detailansichten bleiben zugänglich. |
| `app/plot_ui.inc`, `app/plot_view.c`, `app/plot_view.h` | Zoom, Pan, Ausschnitte, Plotsteuerung | Wiederverwenden und für echte Vergleichskurven erweitern; Statistik bleibt unabhängig vom sichtbaren Ausschnitt. |
| `app/channel_units_ui.inc`, `app/channel_units.c` | Persönliche Anzeigeeinheiten und Konversion | Kanal-/Einheitenwahl im Inspektor anbinden; Dimensionen und Darstellbarkeit weiter prüfen. |
| `app/docking_ui.inc`, `app/docking.c`, `app/docking.h` | Dockrechtecke, Ein-/Ausblenden und gespeicherte Layouts | Analysebezogene Seitenschalter mit Docking abstimmen; keine globalen Seiteneffekte durch lokale Analysezustände. |
| `app/design_tokens.h`, vorhandene Font-/UI-Geometrie | Farben, Rundungen, Abstände und skalierbare Darstellung | Rollen übernehmen; Demo-Pixelmaße in logische Größen und vorhandene UI-Skalierung übersetzen. |
| `app/accessibility.c`, `app/accessibility_native.c`, Plattformadapter | Native Namen/Rollen/Zustände, Fokus und Aktionen | Neue Steuerungen und Sichtbarkeiten anbinden. Ausgeblendete Inhalte dürfen keine erreichbaren unsichtbaren Aktionen behalten. |

Die Architekturentscheidung für Analyse-spezifische Panels darf im bestehenden
Docking oder in einer abgegrenzten Unterteilung der Arbeitsfläche liegen. Entscheidend
sind die geforderte Geometrie und Zustandsisolation, kein bestimmter Dateizuschnitt.
Nicht das gespeicherte Dockingformat allein für einen optischen Umbau ersetzen.

## Unterschiede zwischen Vorlage und echter App

1. **Ein Datensatz gegenüber Mehrfachauswahl.** `a->selected_runs` und
   `PS_ANALYSIS_MAX_INPUTS` bilden bereits bis zu acht Analyseeingaben ab.
   `request_dataset` lädt jedoch `a->last_run` in `a->data`. Mehrfachauswahl allein
   erzeugt damit noch keine vergleichende Rohdatenansicht. Benötigt werden echte,
   begrenzte Vorschauen und vollständige Statistiken je gewähltem Lauf. Die
   Auswahlreihenfolge aus `select_run` und die explizite Referenz aus `reference_run`
   sind maßgeblich; Such-/Sortierreihenfolge darf sie nicht still ersetzen.
2. **Aktiver Lauf und gewählte Läufe.** Eine Checkbox entscheidet über Vergleich
   und Analyseeingaben. Der ausdrücklich markierte aktive Lauf bestimmt den
   Detailbereich. Die Detailansicht darf nicht implizit `last_run` zeigen, wenn
   optisch ein anderer Lauf aktiv ist. Suche verändert keine Auswahl. Beim
   Entfernen des aktiven Laufs nachvollziehbar auf einen verbleibenden wechseln.
3. **Metadaten.** Modellname, Dauer, Zeitpunkt, Kanalanzahl und Parameter stammen
   aus Lauf/Katalog; unbekannte Felder als unbekannt behandeln. Keine festen
   Pendelparameter oder pauschal „Vollständig“ aus dem Mockup übernehmen.
4. **Analyse ohne Messdatei.** `analyze` unterstützt `analysis_only` mit null
   Eingaben. Der Leerzustand der Demo darf diesen gültigen Ablauf nicht sperren.
5. **Berichte.** `report_view` zeigt echte, vom Analysemodul erzeugte Inhalte.
   Tabellen/Kennzahlen der Browserdemo sind keine neue Berichtsvorlage. Ohne
   erzeugte `.psreport`-Datei bleiben vorhandene Exportdateien und verständliche
   Rückmeldung erreichbar; ein Prozessabschluss allein verspricht keinen Bericht.
6. **CSV.** Der Rohdatenweg ruft `analyze(a, true)` auf; der Runner verwendet
   `ps_run_export_csv`. Dies ist bisher ein anderer Vertrag als der auf den
   gewählten Kanal und die Anzeigeeinheit beschränkte Download der Browserdemo.
   Den vorhandenen vollständigen Rohdatenexport erhalten und Eingabe/Umfang/
   Einheiten benennen. Für den neuen Vergleich eine explizite zusätzliche Variante
   vorsehen, falls nötig, ohne bestehende Spalten oder Einheiten still zu ändern.
7. **Sichtbarkeit.** Im Prototyp bedeutet Ausblenden nur eine Browser-Spalte.
   In der App hängen Docking, Tastatur, Fokus und Accessibility daran. Seitenzustände
   der Analyse dürfen andere Arbeitsbereiche nicht ungewollt überschreiben.

## Fachliche und asynchrone Invarianten

- Kurven verwenden den Zeitvektor des jeweiligen Laufs. Unterschiedliche
  Samplezahlen, Startzeiten und Abtastraten bleiben zulässig. Keine Zuordnung
  allein nach Sampleindex und keine ungekennzeichnete Interpolation.
- Kanäle über Identität und kompatible Dimensionen zuordnen. Fehlende oder
  inkompatible Kanäle pro Lauf benennen. Gültige andere Läufe bleiben verständlich
  dargestellt; kein Nullsignal und keine fremde Statistik als Ersatz.
- Die bestehenden Statusmaskenregeln erhalten: nur gültige Messwerte gehen in
  Statistik ein; Ausfälle bleiben Lücken. Fehlende Statistik bzw. Standardabweichung
  bei weniger als zwei gültigen Werten korrekt als nicht verfügbar zeigen.
- Grenzen von Vorschau und Gesamtbestand sichtbar unterscheiden. Zoom ändert
  weder Statistik noch Exportumfang; Gesamt- und Ausschnittexporte bleiben benannt.
- Neue Vergleichs-Caches/Worker benötigen feste Eingaben und eine verlässliche
  Zuordnung zur aktuellen Auswahl. Veraltete Ergebnisse nicht übernehmen; Worker
  vor Freigabe ihrer Daten kontrolliert beenden/abholen. UI-Thread nicht mit
  vollständigem Lauflesen blockieren und keinen unbegrenzten Samplebestand laden.
- Auswahl und Referenz beim Analysestart als Eingabesatz festhalten. Herkunft
  nach Abschluss aus tatsächlichen Eingaben/Artefakten herleiten. Wird die Auswahl
  später geändert, den bestehenden Bericht weiter korrekt beschriften und eine
  neue Analyse anbieten. Einheitswahl in Messdaten schreibt Berichte nicht um.
- Bestehende Speichern-/Build-/Dirty-Prüfungen, Prozesslimits, Diagnoseinformationen,
  Quellcode-/Buildkopien und Verhalten bei Abbruch bleiben erhalten.
- Quelldateien und Rohdaten nicht verändern. Ausgaben nach bestehendem Verfahren
  unter neuem Namen erzeugen; keine bestehenden Berichte/Exporte überschreiben.

## Umsetzung in prüfbaren Schritten

1. Aktuellen Stand und eigene Änderungen abgrenzen. Native Ausgangsansicht mit
   echten Daten dokumentieren. Relevante Tests vor dem Umbau gezielt ausführen.
2. Kopf, mittlere Tabs und unabhängige Seitenzustände im nativen Renderer umsetzen;
   Geometrie, Skalierung und Docking prüfen.
3. Echte Laufliste, aktive Auswahl, Import, Kanal-/Einheitenwahl und Details anbinden.
4. Mehrlaufvorschauen und Statistiken mit begrenzten Ladepfaden ergänzen und die
   fachlichen Gegenfälle prüfen.
5. Vorhandenen Analyse-/Berichts-/Exportweg anbinden; Herkunft und Abbruch prüfen.
6. Visuelle und native Bedienabnahme ausführen, Befunde korrigieren und tatsächliche
   Nachweise dokumentieren. Keine bestandene Browserprüfung als native Abnahme zählen.

## Vorhandene Prüfeinstiege

CLI-Optionen wurden am 19. Oktober 2026 mit `python tools/build.py --help`
gegengeprüft. Die folgenden Tests sind **für die Umsetzung vorgesehen**; ihre
Auflistung behauptet keine heutige native Ausführung oder einen bestandenen Umbau.
`python` steht für den tatsächlich verfügbaren Python-Interpreter ab 3.10.

```text
python tools/build.py --config Debug --test --test-filter docking --test-filter library --test-filter channel_units --test-filter plot_view
python tools/build.py --config Debug --test-display --test-filter plot_workflow --test-filter plot_input_isolation --test-filter channel_units_workflow --test-filter analysis_projects_workflow --test-filter library_selection_workflow --test-filter report_details_workflow --test-filter run_details_16 --test-filter run_details_22
```

Die Unit-Testnamen sind in `tools/native_tests.py`, die Workflow-Namen in
`tests/native_integration_cases.json` vorhanden. Die Workflow-Fixtures liegen u. a.
in `app/plot_tests.inc`, `app/library_selection_tests.inc`,
`app/analysis_project_tests.inc`, `app/report_details_tests.inc` und
`app/run_details_tests.inc`. Bestehende Tests nur dort an neue Interaktionen
anpassen, wo sich der echte Bedienweg ändert; fachliche Assertions erhalten.
Für Vergleich, Seitenschalter und Herkunft sinnvolle neue Regressionen ergänzen.
Nach berührtem Code weitere passende Analyse-/Report-/Masken- und Accessibility-
Prüfungen auswählen. Ein nativer Testlauf benötigt vorhandene lokale Toolchain/SDL;
Fensterprüfungen außerdem eine geeignete grafische Sitzung.

Der lokale Prototyp lässt sich separat mit
`node tools/check_analysis_design.cjs` prüfen; Node/Playwright nach bestehender
Projektanleitung auflösen. Das verändert die Referenzbilder nicht. Nur ein
bewusster Lauf mit `--export` erneuert die vier versionierten Designreferenzen.

## Abnahme der nativen Umsetzung

| Fall | Nachweis |
| --- | --- |
| Breite Normalansicht | Echte Fensterbilder Hell/Dunkel mit nachvollziehbarer Übereinstimmung zu den Referenzen; aktuelle Shell erhalten. |
| Vier Seitenkombinationen | Sichtbarkeit, tatsächliche Rechtecke und nutzbare Breite der Mitte prüfen; jeweils andere Seite bleibt korrekt. |
| Größen-/Themewechsel | Wiederöffnen geschlossener Bereiche, aktiver Kanal, Auswahl, rechter Kontext und Ausschnitte bleiben konsistent. |
| Ein, zwei, acht Läufe | Echte unterschiedliche Signale, richtige Quellen und Statistiken; neunte Auswahl begrenzt; Referenzreihenfolge korrekt. |
| Unterschiedliche Datensätze | Abweichende Zeitvektoren, fehlender Kanal, inkompatible Dimensionen, Statuslücken, reduzierte Vorschau und rekonstruierter Lauf. |
| Statistik/Zoom/Einheit | Statistik über den gesamten jeweiligen Lauf, korrekte Konversion, keine Veränderung gespeicherter Daten. |
| Analyse/Editor | Echte C- und Physim-Module, fehlender Build/ungespeicherter Code, Abbruch, Diagnose, null Eingaben bei geeigneten Analyseprojekten. |
| Ergebnis/Herkunft | Eigenes Modul mit Kurven/Histogramm/Tabelle, alter Bericht nach neuer Auswahl, Bericht ohne aktuelle Messdaten und Modul ohne Bericht. |
| Export | CSV vollständig; SVG/PNG vollständig und Ausschnitt; PNG-Größenwahl; klare Eingaben/Einheiten, neue Dateinamen, Inhalte tatsächlich prüfen. |
| Laden/Fehler | Leerer Workspace/Projekt, keine Auswahl, Suche ohne Treffer, defekte/fehlende Datei und schnelle Auswahlwechsel ohne veraltete Daten. |
| Bedienung | Tastatur, sichtbarer Fokus, Rückkehr vom Editor/Dialog, Fokus nach Ausblenden sowie native Namen/Rollen/Zustände. |
| Skalierung | Reale Mindestfenstergröße, große Schrift, vorhandene DPI-Optionen, Hell/Dunkel/Hochkontrast; keine unerreichbare Aktion. |
| Andere Arbeitsbereiche | Entwickeln, Simulieren, F1, Workspace-Navigation und gespeicherte Docklayouts bleiben funktionsfähig. |

Die [Desktop-Abnahmematrix](D:/Skills/skills/ui-ux-designer/knowledge/desktop-review.md)
bei der Umsetzung beachten. Screenshots mit Zustand, Größe, Skalierung, Theme,
Plattform und Revision beschreiben. Ein Screenshot ist kein Screenreadernachweis.
Linux-/macOS-Aussagen nur aus realen dortigen Builds/Tests ableiten. Auf diesem
Rechner gegebenenfalls die gemeinsame WSL-Distribution `Alpine-Dev` verwenden;
keine zweite Projekt-Distribution anlegen.

Ergebnis der späteren Umsetzung: native Änderungen, Regressionen, tatsächliche
Screenshots und ein neuer Umsetzungs-/Prüfbericht im Designordner, anschließend
Commit und Push in beide Repositories gemäß AGENTS.md. Die bisherigen vier
Prototypbilder bleiben als Soll-Referenzen erhalten.
