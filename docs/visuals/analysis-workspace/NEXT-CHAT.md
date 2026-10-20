# Kopierfertiger Umsetzungsauftrag

**Aktuelle Nutzerfreigabe vom 20. Oktober 2026:** Der
[Grafikexport](../unified-workspace/EXPORT-EXAMPLE.md) gehört verbindlich zum
fertigen Design. [ADR 010](../../decisions/010-analysis-graphic-export.md) ist Teil
dieses nativen Folgeauftrags; der Export ist kein optionales Beispiel mehr.

Stand: 19. Oktober 2026. Der folgende Text kann vollständig in einen umsetzenden
Chat mit dem Projekt D:\Physim kopiert werden. Er beauftragt die native Umsetzung;
die Erstellung dieser Übergabe selbst hat noch keinen nativen Code geändert.

---

Arbeite in D:\Physim die bestehende native Auswertungsseite auf das vorbereitete
Design unter docs/visuals/analysis-workspace um. Die Vorlage wurde im vorherigen
Chat erstellt; jetzt soll die echte Analyseoberfläche entsprechend umgebaut
werden. Beginne nach der Bestandsprüfung mit der Implementierung und führe sie
bis zur geprüften Fertigstellung durch.

Lies zuerst AGENTS.md und D:/Skills/skills/AGENTS.md. Lies anschließend:

- docs/visuals/analysis-workspace/DESIGN.md
- docs/visuals/analysis-workspace/IMPLEMENTATION.md
- docs/design-direction.md
- docs/reports.md
- docs/decisions/010-analysis-graphic-export.md
- docs/visuals/unified-workspace/EXPORT-EXAMPLE.md

Öffne die interaktive Vorlage
D:/Physim/docs/visuals/analysis-workspace/index.html im Browser. Die visuellen
Referenzen im selben Verzeichnis sind analysis-dark.png, analysis-light.png,
analysis-comparison.png und analysis-report.png. Alle Dateien sind lokal
vorhanden; ein Vorschau-Server ist nicht erforderlich. Die Vorlage ist die
Gestaltungsgrundlage. Behalte die aktuelle App-Shell bei und übertrage den
Analysebereich in den bestehenden C-/SDL-/Nuklear-Renderer.

Die Anordnung innerhalb von Auswerten soll so aussehen:

- Kopf mit Titel, aktiver Auswahl, Analysecode, einer primären Analyseaktion
  sowie kleinen unabhängigen Schaltern für die linke und rechte Seite.
- Links eine durchsuchbare Liste echter Messläufe mit Mehrfachauswahl und
  eindeutig markierter Referenz. Laufimport bleibt über den echten Dateidialog
  erreichbar. Läufe & Berichte sowie die Workspace-Dateien bleiben erreichbar.
- In der Mitte die Tabs Messdaten und Analyseergebnis. Messdaten zeigen eine
  große Kurve mit Achsen/Einheiten, Legende, Zoom, Statistik und Export. Bei
  mehreren gewählten Läufen erscheinen deren tatsächliche Kurven und je eine
  Statistikzeile. Der Bericht verwendet den vorhandenen Ergebnisrenderer.
- Rechts Kanal-/Einheitenwahl, Details des ausdrücklich benannten aktiven Laufs
  und Herkunft. Vorhandene vollständige Modell-/Laufdetails bleiben zugänglich.
- Unten eine verständliche Rückmeldung zu Laden, Analyse, Fehler und Export.

Hell und Dunkel verwenden die vorhandenen semantischen Farben. Erhalte hohen
Kontrast, UI-/Textskalierung, Tastaturbedienung und native Accessibility.
Beide Seiten müssen unabhängig funktionieren: alle vier Kombinationen bei
breitem Fenster prüfen; die Mitte darf weder verschwinden noch null Breite
bekommen. Bei kleinem Fenster bleiben ausgeblendete Bereiche wieder erreichbar.
Der Zustand des Analysebereichs darf Entwickeln, Simulieren oder F1 nicht
ungewollt verändern. Docking und bestehende gespeicherte Layouts berücksichtigen.

Arbeite vom aktuellen lokalen Stand aus. Ausgangspunkte sind app/design_ui.inc,
app/main.c, app/library_actions.inc, app/library_ui.inc, app/plot_ui.inc,
app/report_ui.inc, app/channel_units_ui.inc, app/run_details_ui.inc und die
Docking-/Accessibility-Adapter. Die technische Übergabe enthält die konkrete
Zuordnung. Trenne neue Analyse-UI bei Bedarf in klar abgegrenzte Includes oder
Module; erneuere keine fremden Bereiche allein aus stilistischen Gründen.

Wichtig: Die Mehrfachauswahl für Analysemodule existiert bereits, die normale
Rohdatenvorschau lädt bislang einen einzelnen Datensatz. Implementiere den
Mehrlaufvergleich mit echten, begrenzt geladenen Daten und korrekter Zuordnung
von Zeitachse, Kanal, Dimension, Einheiten und Statusmasken. Kein Interpolieren
oder Paaren nach gleichem Sampleindex ohne expliziten fachlichen Vertrag.
Bewahre Auswahlreihenfolge, Referenzwahl und die Obergrenze von acht Eingaben.
Läufe ohne passenden Kanal erhalten einen erklärten Zustand statt falscher Werte.

Statistik bleibt über den gesamten jeweiligen Lauf berechnet und unabhängig vom
Zoom. Anzeigeeinheiten verändern keine gespeicherten Messwerte. Erhalte die
bestehenden CSV-/SVG-/PNG-Verträge, vollständige Exporte, Ausschnittexporte,
PNG-Größenwahl, Status-/Lückenbehandlung und Kennzeichnung reduzierter Vorschauen.
Die CSV-Umrechnung der Browserdemo ist kein Auftrag zur stillen Änderung des
nativen Rohdatenexports. Eine gegebenenfalls zusätzliche Exportvariante für den
Vergleich muss ihren Umfang, ihre Einheiten und ihre Eingaben klar benennen.

Implementiere den freigegebenen Grafikexport aus ADR 010 als Teil dieser
Auswertung: **Grafik exportieren…** neben den Zoomaktionen, Dialog mit Vorschau
links und Optionen rechts, PNG/SVG, Gesamtdiagramm/sichtbarer Ausschnitt,
Hell auf Weiß/aktuelle Darstellung und vier PNG-Größen. Standard ist PNG mit
2400 × 1700 Pixeln, gesamtes Diagramm und Hell auf Weiß. **Bild kopieren** und
**PNG/SVG speichern…** arbeiten auf demselben beim Öffnen festgehaltenen Datenstand;
Titel, Achsen, Einheiten, Legende, Referenz und gewählte Vergleichsläufe übernehmen.
Ausgabegröße ist unabhängig von Panelmaßen. Echte native Zwischenablage, Datei-
und Fehlerwege anbinden; denselben Ablauf für einzelne echte Berichtsdiagramme
bereitstellen. Mit den freigegebenen Dialogbildern und der nativen Abnahmeliste
aus ADR 010 prüfen. CSV bleibt separater Datenexport mit seinem bisherigen Vertrag.

Analyse starten führt weiter den echten C-/Physim-Analyseprozess aus; Analysecode
öffnet den vorhandenen Editor. Erhalte Speichern/Build/Dirty-Prüfungen,
Prozessgrenzen und Abbruch. Auch Analyseprojekte mit selbst erzeugten Daten und
null Eingaben bleiben lauffähig. Ein Bericht behält seine Herkunft; eine spätere
Laufwahl darf ihn nicht als Ergebnis der neuen Auswahl ausgeben. Laufende Worker
dürfen beim schnellen Wechseln keine veralteten Ergebnisse in die neue Auswahl
übernehmen.

Übertrage nur das Produktdesign. Keine Präsentationsüberschrift, Demo-Zustandswahl,
festen Pendelwerte, Demo-CSV oder simulierte Wartezeit in die App übernehmen.
Keinen WebView-Ersatz bauen. Bestehende Berichte mit eigenen Diagrammen,
Histogrammen, Tabellen und Herkunft dürfen nicht durch den Beispielbericht
ersetzt werden.

Prüfe die Umsetzung anhand von IMPLEMENTATION.md mit passenden bestehenden
Tests und gezielten neuen Regressionen. Führe echte Aufgaben in der nativen App
aus und vergleiche Fensterbilder mit den vier Referenzen. Browserprüfungen
allein sind keine Abnahme. Teste Mindestfenster, alle Seitenzustände, Hell/Dunkel,
hohen Kontrast, große Schrift, Tastatur, Fokuskehr sowie echte/fehlende/defekte
Laufdaten. Linux und macOS nur nach dort tatsächlich ausgeführten Builds/Tests
als geprüft melden; fehlende Plattform- oder Assistive-Nachweise offen benennen.

Ändere fremde aktive Arbeiten nicht. Dokumentiere Umsetzung, Tests, echte
Screenshots und begründete Abweichungen im Designordner. Committe abgeschlossene,
geprüfte Änderungen und pushe entsprechend AGENTS.md ohne Force-Push nach origin
und upstream. Räume nur nachweislich entbehrliche eigene Ausgaben auf; Quellen,
Referenzbilder, Nutzdaten und die gemeinsame WSL-Distribution Alpine-Dev erhalten.
