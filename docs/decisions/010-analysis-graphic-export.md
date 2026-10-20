# Grafikexport als verbindlicher Teil der Auswertung

Status: vom Nutzer freigegeben, 20. Oktober 2026. Bestandteil des fertigen Designs.
Die Freigabe betrifft Gestaltung und Verhalten; sie bestätigt keine bereits
abgeschlossene native Implementierung.

## Entscheidung und maßgebliche Referenz

Der Nutzer hat die bedienbare Exportvorschau ausdrücklich angenommen und die
Übernahme in das fertige Design sowie die weitere native Implementierung im
anderen Chat beauftragt. Der bisherige Status „zusätzlicher Designvorschlag“ ist
damit aufgehoben. Weitere Designvarianten oder eine erneute Designauswahl sind
für diesen Export nicht erforderlich.

- [Freigegebene Spezifikation](../visuals/unified-workspace/EXPORT-EXAMPLE.md).
- [Interaktive Referenz](../visuals/analysis-workspace/index.html?example=export).
- [Dialog dunkel](../visuals/unified-workspace/export-dialog-dark.png) und
  [hell](../visuals/unified-workspace/export-dialog-light.png).
- [Tatsächliche Beispielausgabe als PNG](../visuals/unified-workspace/export-example.png)
  und [SVG](../visuals/unified-workspace/export-example.svg).
- [Kopierfertiger Gesamtauftrag](../visuals/unified-workspace/NEXT-CHAT.md).

## Verbindlicher Produktvertrag

1. **Zugang:** Ein beschrifteter Button **Grafik exportieren…** direkt am Diagramm
   neben den Zoomaktionen. Derselbe Dialogablauf gilt für einzelne exportierbare
   Grafiken in Analyseberichten. Ohne exportierbare Daten bleibt der Zugang
   deaktiviert und der bestehende Leer-/Fehlerzustand verständlich.
2. **Dialog:** Links eine saubere Exportvorschau, rechts Format, Umfang,
   Darstellung und PNG-Größe; unten **Bild kopieren** und **PNG/SVG speichern…**.
   Titel **Grafik exportieren**, ruhige semantische Flächen und die freigegebenen
   Hell-/Dunkelansichten übernehmen. Bei schmalem Fenster untereinander anordnen;
   Optionen, Vorschau und Abschlussaktionen bleiben erreichbar.
3. **Format und Größe:** PNG ist Standard mit **2400 × 1700 Pixeln**. Weitere
   Größen: 1200 × 850, 3600 × 2550 und 4800 × 3400. SVG bleibt skalierbar;
   die PNG-Größenwahl ist im SVG-Modus deaktiviert. Exportgröße und Proportionen
   hängen nicht von Panelgröße, Fenstergröße oder Bildschirm-DPI ab.
4. **Umfang:** Standard **Gesamtes Diagramm**, alternativ **Sichtbarer Ausschnitt**.
   Nativ die tatsächlichen sichtbaren Achsengrenzen übernehmen; die Browserreferenz
   demonstriert die Zeitgrenzen. Ohne Ausschnitt ist die zweite Auswahl deaktiviert.
5. **Darstellung:** Standard **Hell auf Weiß**, alternativ **Wie in der Auswertung**
   mit der beim Öffnen geltenden Darstellung. Ein dunkles App-Theme erzeugt
   standardmäßig weiterhin eine helle, gut einfügbare Grafik.
6. **Bildinhalt:** Titel, Achsen, Einheiten, Legende, Referenz und alle im betreffenden
   Diagramm ausgewählten Vergleichsläufe. Bedienelemente, Crosshair und Seitenbereiche
   gehören nicht in die Datei. Messdaten, Masken, Lücken und Einheiten folgen den
   echten nativen Exportverträgen; kein Abzeichnen eines UI-Screenshots.
7. **Festgehaltene Eingaben:** Beim Öffnen Daten, Auswahl, Referenz, Kanal,
   Anzeigeeinheit, Achsengrenzen, Titel und Darstellung eindeutig festhalten.
   Vorschau, Speichern und Kopieren verwenden denselben Stand. Spätere Auswahl-,
   Lade- oder Workeränderungen dürfen den offenen Export nicht still verändern.
   Der Export verändert weder Zoom noch Statistik, Bericht oder Laufdaten.
8. **Ausgabe:** Vorschau und gespeicherte Datei verwenden dieselbe native
   Darstellung und denselben Datenstand. Bild kopieren übergibt ein wirklich
   erzeugtes Bild an die native Zwischenablage. Fehlende Unterstützung und Fehler
   wahrheitsgemäß anzeigen; Speichern bleibt als Alternative verfügbar.
   Ein echter Dateidialog und verständliche Erfolgs-/Fehlerrückmeldung gehören dazu.
9. **Bedienung:** Tastaturzugang, erkennbare Namen/Rollen/Zustände, Dialogfokus,
   Escape/Abbrechen und Fokus-Rückgabe zum Exportbutton. UI-/Textskalierung,
   hoher Kontrast und reduzierte Effekte erhalten.

## Native Umsetzung und Kompatibilität

Die bestehende C-/SDL-/Nuklear-/OpenGL-Anwendung bleibt Grundlage. Die vorhandenen
PNG-/SVG-Gesamt-/Ausschnittswege und Größenwahl aus `app/report_ui.inc` sowie die
zugehörigen Plotrenderer und Exportfunktionen weiterverwenden beziehungsweise
gezielt erweitern. Echte native Zwischenablage- und Dateidialogwege anbinden.
Die genaue Modulaufteilung entscheidet die implementierende Arbeit am aktuellen
Code; die Browserdateien werden nicht als WebView oder JavaScript-Renderer in
die Anwendung eingebaut.

CSV bleibt als eigener Datenexport mit seinem bisherigen Umfang und Vertrag
erhalten. Bericht-Herkunft, Mehrlaufzuordnung, unabhängige Zeitvektoren, Statusmasken,
vollständige beziehungsweise ausdrücklich reduzierte Daten und Ressourcenlimits
weiter beachten. Mehr Bildpixel erzeugen keine zusätzlichen Messpunkte.

Die festgelegten Panelplätze und Zusatzfenster aus [ADR 009](009-fixed-workspace-windows.md)
sowie der gemeinsame [Resizing-Vertrag](../visuals/unified-workspace/RESIZING.md)
bleiben gültig. Der Exportdialog verändert keine Bereichsanordnung und erhält
den Fokusbezug zum auslösenden Diagramm beziehungsweise dessen Fenster.

## Abnahme des nativen Folgeauftrags

- [ ] Dialog in Auswerten und an einzelnen echten Berichtsdiagrammen mit den beiden
      freigegebenen Aufnahmen visuell abgleichen; begründete Abweichungen dokumentieren.
- [ ] PNG in allen vier Größen und skalierbares SVG tatsächlich erzeugen und öffnen;
      Maße, Hintergrund, Texte, Achsen, Einheiten, Legende und Kurven prüfen.
- [ ] Gesamtdiagramm und gezoomten/verschobenen Ausschnitt prüfen, auch bei mehreren
      Läufen mit unterschiedlichen Zeiten und Datenlücken.
- [ ] Offener Export bleibt bei anschließendem Auswahl-/Einheiten-/Datenwechsel an
      seinen ursprünglichen Eingaben gebunden; Vorschau und Datei stimmen überein.
- [ ] Echte Zwischenablageübergabe und Einfügen des Bildes auf den tatsächlich
      verfügbaren Plattformen prüfen; fehlende Plattformnachweise offen lassen.
- [ ] Abbruch, Speichern-/Renderfehler, fehlende Daten und Ressourcenlimits testen;
      keine falsche Erfolgsmeldung und kein Verlust bestehender Daten.
- [ ] Fenster-/Panelresize und Bildschirm-DPI ändern keine gewählten Exportmaße;
      Tastatur, Fokuskehr, große Schrift und Kontrast bleiben nutzbar.

Die [Browserprüfung](../visuals/unified-workspace/export-validation.json) belegt
das Beispiel einschließlich Downloads. Sie ersetzt keine dieser nativen Prüfungen.
