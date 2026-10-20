# Bedienbares Beispiel: Grafikexport

Stand: 20. Oktober 2026. Ergänzender Designvorschlag, keine native Umsetzung.

[Beispiel direkt öffnen](../analysis-workspace/index.html?example=export).
Die URL öffnet den Dialog automatisch mit zwei synthetischen Vergleichsläufen.
Im normalen gemeinsamen Arbeitsraum öffnet **Grafik exportieren…** neben den
Zoomaktionen denselben Dialog für die tatsächlich übernommenen Beispielmessdaten.

## Gestaltung und Verhalten

- Links die saubere Grafikvorschau, rechts Format, Zeitbereich, Darstellung und
  PNG-Größe. Unten **Bild kopieren** und **PNG/SVG speichern…**.
- PNG oder SVG; Standard PNG mit 2400 × 1700 Pixeln. Weitere vorhandene Größen:
  1200 × 850, 3600 × 2550, 4800 × 3400. SVG bleibt skalierbar.
- Gesamtes Diagramm ist Standard. Sichtbarer Ausschnitt übernimmt die beim
  Öffnen geltenden Zeitgrenzen; ohne Zoom ist diese Auswahl deaktiviert.
- Hell auf Weiß ist Standard. **Wie in der Auswertung** übernimmt die beim
  Öffnen geltende helle oder dunkle Darstellung.
- Titel, Kanal/Einheiten, Legende, Referenz und alle ausgewählten Laufkurven
  gehören zur Ausgabe. Bedienelemente, Crosshair und Seitenbereiche entfallen.
- Ausgabeformat und Proportionen sind unabhängig von der Panelgröße. Die Vorschau
  und die tatsächlich heruntergeladene Datei werden aus demselben SVG erzeugt.
  Größere PNGs erzeugen mehr Bildpixel, keine zusätzlichen Messpunkte.
- Auswahl, Einheiten, Daten und Ausschnitt werden beim Öffnen kopiert. Spätere
  Quellenänderungen verändern keinen bereits geöffneten Export. Zoom und Statistik
  der Arbeitsfläche bleiben unverändert.
- PNG und SVG lassen sich tatsächlich über den Browser herunterladen. Kopieren
  verwendet PNG und die Browser-Zwischenablage, soweit verfügbar und erlaubt.
  Bei fehlender Unterstützung ist die Aktion deaktiviert; bei verweigerter
  Berechtigung bleibt eine verständliche Meldung mit Speicheralternative.
- Escape schließt den Dialog und gibt Fokus an den Exportbutton zurück.
  Dateidialog, Browserdownloadziel und echtes „Datei anzeigen“ bleiben Aufgabe
  des Browsers; die Vorlage behauptet keinen Zugriff auf unbekannte Speicherpfade.

## Technische Zuordnung

`analysis-workspace/graphic-export.js` zeichnet die Exportgrafik aus einem beim
Öffnen angeforderten Datenschnappschuss. `analysis.js` liefert die gewählten
Läufe, den Kanal, die umgerechneten Anzeigewerte und die sichtbaren Zeitgrenzen.
`graphic-export.css` gestaltet den Dialog. Das Beispiel greift nicht auf native
APIs zu und lädt keine Webressourcen.

Für die spätere native Umsetzung bleibt dies ein zusätzlicher überprüfbarer
Entwurf. Vorhandene PNG-/SVG-Gesamt-/Ausschnittswege und Größenwahl aus
`app/report_ui.inc` / `docs/reports.md` weiterverwenden. Native Datenmasken,
Plotregeln, Speichergrenzen und bestehende CSV-Verträge erhalten. Der Dialog
sollte künftig für einzelne echte Berichtsdiagramme denselben Ablauf anbieten;
die hier vorhandenen Demo-Berichte enthalten noch keine solchen Diagramme.
Keinen Browserrenderer in die native App übernehmen und keine Demoformel als
Ersatz für echte Berichtsdaten verwenden.

## Ausgeführte Prüfung

`node tools/check_graphic_export.cjs` prüft tatsächliche PNG-Downloads in drei
Größen, echte SVG-Ausgabe, weißen Hintergrund, Legende/Einheiten, Gesamt-/
Ausschnittsgrenzen, konstante Exportdaten bei Quellenänderung, vier Dialogbreiten,
Escape/Fokuskehr und Integration mit übernommenen Simulationspunkten. Der
Clipboard-Payload und die Ablehnungsbehandlung werden über einen isolierten
Browseradapter geprüft, ohne die OS-Zwischenablage des Nutzers zu verändern.
Eine echte OS-Zwischenablage- oder native Plattformabnahme wird nicht behauptet.

[Prüfmanifest](export-validation.json) · [Dialog dunkel](export-dialog-dark.png) ·
[Dialog hell](export-dialog-light.png) · [Beispiel-PNG](export-example.png) ·
[Beispiel-SVG](export-example.svg).
