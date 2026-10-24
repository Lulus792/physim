# Physim · Gemeinsame Designvorschau

Neu am 24. Oktober: [Protokoll mit Fehlern, Warnungen, Info und Debug](index.html?example=protocol#develop),
[Gestaltungsvertrag](PROTOCOL-DESIGN.md) und [Liste fehlender Demo-Inhalte](CONTENT-INVENTORY.md).
Die interaktiven Beispiele sind von aktuellen Sitzungsvorgängen getrennt.
Kräfte, Szenentexte und weitere echte Modellinhalte sind im Inventar erfasst;
die neue Protokollreferenz ist noch keine native Umsetzung oder Nutzerabnahme.

Stand: 20. Oktober 2026. [Vorschau öffnen](index.html).
Die Datei lässt sich direkt lokal im Browser öffnen, ohne Installation, Server
oder externe Ressourcen.

Der laufende [native Referenzabgleich](NATIVE-FIDELITY.md) führt Umsetzung,
begrenzte Nachweise und die noch offenen Unterschiede aller vier Bereiche.
Der [native Grafikexport-Umsetzungsstand](GRAPHIC-EXPORT-NATIVE.md) dokumentiert
die geprüfte Datenbasis, [Zeichenbeschreibung und SVG](GRAPHIC-FIGURE-NATIVE.md)
sowie den [nativen Rasteradapter mit PNG und Vorschautextur](GRAPHIC-RASTER-NATIVE.md).
Der [sichtbare native Berichtsdialog](GRAPHIC-DIALOG-NATIVE.md) ergänzt jetzt
Vorschau, Optionen, Speichern und echte Bildkopie. Der spätere
[sichtbare Messlaufvergleich](ANALYSIS-COMPARISON-NATIVE.md) ergänzt acht echte
Kurven, Referenz, Statistik und denselben Exportzugang. Die vollständige
Design-/Plattformabnahme des Exports bleibt weiter beauftragt.
Der folgende [native Materialabgleich](GRAPHIC-MATERIAL-NATIVE.md) ergänzt den
Hintergrundfilter und bei großer Schrift proportional gezeichnete Auswahlfelder.
Der [Schatten-/Maßabgleich](GRAPHIC-SHADOW-NATIVE.md) ergänzt Referenzgröße,
korrekte Inhaltsmaße, 480-/680-/800-px-Fälle und verlustfreie PNG-Aufnahmen.
Eine Browserprüfung ist weiterhin keine vollständige native Abnahme.
Die [aktuellen Nutzerbefunde](USER-REVIEW-2026-10-21.md) und der folgende
[native Komponentenabgleich](COMPONENTS-NATIVE.md) führen weitere sichtbare
Fehler und deren erste geprüfte Korrekturen. Die übrige Oberfläche bleibt weiter
abzugleichen; volle Designgleichheit ist nicht erreicht.
Der [native Auswertungsstand](ANALYSIS-NATIVE.md) trennt den neuen Aufbau von
den noch offenen Mehrlauf-, Export- und vollständigen Bildabgleich-Aufgaben.
Der anschließende [Abgleich der Auswertungsfelder](ANALYSIS-CONTROLS.md) enthält
aktuelle native Aufnahmen und ausdrücklich begrenzte Kontur-/Farbvergleiche.
Die [kompakte Einheitenwahl und große Schrift](ANALYSIS-UNITS.md) dokumentieren
den nächsten nativen Teilstand einschließlich echter Speicher-/Abbruchfälle.
Der [Werkbank-Kontext](WORKBENCH-CONTEXT.md) führt den aktuellen Abgleich der
gefüllten API-Karte, dokumentierter Beispiele und des Protokollkopfs fort.
Der [Folgeabgleich der Kontextspalte](WORKBENCH-CONTEXT-FLOW.md) ergänzt volle
Breite, CSS-Zeilenboxen, Gliederungsdetails und Scrollzugang bei kurzen Fenstern.
Der aktuelle [F1-Kontextdetail-Abgleich](../f1-layouts/CONTEXT-DETAILS.md) korrigiert
Codekopfkontur und Bildunterschrift. Windows/Linux bestehen sechs native Gruppen;
Modelle sind mit System-/Offline-Schriften bei 1×/2× gezeichnet. Verbleibende
Schrift-/Raster-/Plattformgrenzen und die Linux-Speicherbedingung sind benannt.
Die vorhergehende [F1-Codevorschau](../f1-layouts/CODE-PREVIEW.md) erhält lange Tokens,
Leerraum und die vollständigen Quellbytes. Horizontaler Tastatur-/Rad-/Ziehzugriff,
Unicode-Konformität, Windows-Hostfokus und Lizenzkopien sind separat geprüft.
112 vollständige native H-Bildpaare bleiben unverändert; die vollständige
Designabnahme bleibt offen.
Der vorhergehende [F1-SVG-Schriftabgleich](../f1-layouts/SVG-LABELS.md) ergänzt echte
kursive Serifen im Modell samt Offline-Font, überprüften Lebenszeiten und
nativen Vergleichsbildern. Er belegt keine vollständige H-Gleichheit.
Der vorhergehende [F1-Navigations-/Kontextabgleich](../f1-layouts/NAV-CONTEXT.md)
übernimmt Seitenleistenmaße, Panelkante, Modelltabelle, Beispielkopf und die
intrinsischen Reiterbreiten. Native Aufnahmen und verbleibende Unterschiede
sind mit den tatsächlich ausgeführten Prüfungen verknüpft.
Der vorhergehende [F1-Scrollabgleich](../f1-layouts/SCROLL.md) vereint Herkunft und
Artikel in einer scrollenden Fläche und erhält die volle Navigationsbreite.
Er ergänzt [Paarabstände](../f1-layouts/KERNING.md),
[Artikelzeilen](../f1-layouts/ARTICLE-FLOW.md),
[Schriftrollen](../f1-layouts/TYPOGRAPHY.md) und
[Artikelaktionen](../f1-layouts/SECONDARY-ACTIONS.md). Die Berichte trennen ihre
jeweiligen historischen Prüfstände. Vollständige H-Gleichheit, Linux-Schrift,
Rasterung, weitere Kontextdetails und die Gesamtoberflächenabnahme bleiben offen.

**Für die native Umsetzung in einem anderen Chat:**
[kopierfertiger Auftrag](NEXT-CHAT.md) · [Bestand und Abnahmeliste](IMPLEMENTATION.md)
· [gemeinsamer Resizing-Vertrag](RESIZING.md).

**Als Teil des fertigen Designs freigegeben:** [Grafikexport](EXPORT-EXAMPLE.md)
mit PNG/SVG, Gesamt-/Ausschnittswahl, Darstellung und Bildgröße.
[Bedienbare Referenz](../analysis-workspace/index.html?example=export) ·
[verbindliche Produktentscheidung und native Abnahme](../../decisions/010-analysis-graphic-export.md).
Der umsetzende Chat soll diesen Dialog nativ mit implementieren.

**Ergänzung vom 20. Oktober:** Entwickeln, Simulieren und Auswerten verwenden
denselben Resizing-Baustein. Auswerten erhält verstellbare Lauflisten-/Detailbreiten
und die Diagrammhöhe gegenüber Statistik. Simulieren verwendet die durchgehenden
Werkbank-Trennlinien und dieselben 10/40-Schritte, Home/End-Grenzen,
Enter/Doppelklick und Escape-Abbruch. [Aktueller Nachweis](resizing-validation.json).

Die drei gewählten Entwürfe bilden einen Arbeitsraum: [Werkbank Klarglas Kühl](../develop-designs/README.md),
[Messlabor mit Wertefeldern B](../simulation-designs/README.md) und
[Auswertungsfläche](../analysis-workspace/README.md). Eine Menüzeile, drei gleich
breite Bereichstabs, semantische Farben, Hell/Dunkel und ein gemeinsamer
Projektzustand verbinden sie. Die einzelnen Referenzen bleiben separat nutzbar.

## Durchgängiger Ablauf

1. In **Entwickeln** `main.c` bearbeiten. Die bekannten Zahlenkonstanten
   `length_m`, `gravity_m_s2`, `initial_angle_rad` und optional `mass_kg` steuern
   das Pendelbeispiel. Speichern gilt für diese Sitzung; F5 baut die Vorschau.
2. In **Simulieren** einen neuen Lauf starten, pausieren, schrittweise bewegen,
   historische Zeitpunkte betrachten und Kurven einblenden. Szenenbaum startet
   geschlossen, Inspektor offen, Kurven geschlossen. Modellwerte im Inspektor
   ändern denselben Quelltext und lösen denselben Vorschau-Build aus. Ein
   bestehender Lauf behält sein Modell; der nächste Lauf übernimmt den neuen Stand.
3. **Messlauf auswerten** übernimmt den erreichten Zeitraum und einen eigenen
   Modellschnappschuss. In **Auswerten** laufen Diagramm, Einheiten, Statistik,
   Mehrlaufvergleich, Bericht und CSV auf diesen übernommenen Punkten.
   **Analysecode** öffnet `analysis.c` in der Werkbank. Läufe erscheinen auch im
   Workspace-Ordner `runs` und führen von dort zur jeweiligen Auswertung.

Beim Bereichswechsel bleiben Dokumente, Auswahl, Suche, Scrollpositionen,
Panelgrößen und Sichtbarkeiten erhalten. Eine laufende Simulation pausiert.
Bis zu acht Messläufe bleiben in der Sitzung erhalten; weitere Übernahmen werden
mit Hinweis abgelehnt, ohne alte Läufe zu löschen. Berichte behalten ihre
ursprüngliche Auswahl und Einheit. **Datei → Projektkopie herunterladen** sichert
Demodateien, Modellstand und Messläufe als JSON; ein Rückimport ist nicht enthalten.
Neuladen beginnt eine neue Sitzung.

## Gestaltung und Umsetzung

Die Shell nutzt dieselben semantischen Rollen für Text, Nebenbeschriftungen,
Flächen, Linien, Akzent, Auswahl und Zustandsfarben in allen Bereichen.
Die kühl getönte Suchinsel gehört zur Werkbank; Szene und Kurven behalten ihre
fachlichen Darstellungen. Tastaturfokus und ausgewählte Tabs besitzen zusätzlich
zu Farbe erkennbare Zustände. Gemeinsame Menüs sind per Tastatur erreichbar;
Pfeile/Pos1/Ende wechseln die Bereichstabs, Escape schließt Menüs und Hilfe.
Alt 1/2/3 wechselt den Bereich auch aus einem eingebetteten Arbeitsbereich.

`workspace.js` besitzt den gemeinsamen Projektzustand. Die drei dauerhaften
Frames erhalten die vorhandenen DOM-/Editor-/Panelzustände und isolieren deren
bisherige Styles und IDs. `bridge.js` wird ausschließlich mit `?unified=…` in
einem Frame aktiv. Nachrichten werden nach Protokoll und tatsächlichem
Absenderfenster zugeordnet; die Auswertung prüft Laufstruktur, endliche Werte,
aufsteigende Zeiten und begrenzte Datenmengen. Lokale `file:`-Vorschauen nutzen
denselben Transport wie HTTP, ohne direkten Zugriff auf Frame-Dokumente.

Es handelt sich um eine funktionierende **Designvorschau**. Kein nativer Compiler,
Runner oder Analyseprozess ist angeschlossen. Vorschau-Builds prüfen ausschließlich
den expliziten Zahlenvertrag des Beispiels, keinen beliebigen C-Code. Die Bewegung
verwendet die bestehende Kleinwinkelnäherung; weitergehende Kamera-/Runneroptionen
bleiben illustrative Bedienzustände aus den Referenzen. Beim Export werden nur
Punkte bis zur erreichten Live-Zeit mit demselben aktiven Modell erzeugt, keine
zukünftigen Messwerte oder fremden Demo-Läufe in die Auswertung übernommen.

## Ausgeführte Prüfung

Windows / Edge 155.0.4283.62, Playwright, 20. Oktober 2026:

- Vollständige Nutzeraufgabe Code ändern/speichern → Build → zwei vollständige
  Läufe mit unterschiedlicher Länge → Vergleich und Bericht. CSV numerisch gegen
  die aktive Modellformel geprüft: 301 Punkte von 0 bis 6 s, Anfangswinkel und
  Endwert stimmen überein.
- Modellwerte aus dem Inspektor zurück zum Quelltext, veraltete Berichtsherkunft,
  Buildfehler ohne Datenverlust, Analysecode im Editor und erhaltene Seitenzustände.
- 24 Layoutfälle: drei Bereiche × 1440/1080/760/500 Pixel × Hell/Dunkel,
  jeweils 1000 Pixel hoch. Kein horizontaler Überlauf, gleiche Textfarben,
  keine doppelte Menü-/Tabzeile.
- Alle acht Simulation-Seitenzustände, vier Analyse-Seitenzustände, kurzer
  Einzelschritt-Lauf mit unterscheidbaren Zeitbeschriftungen und Zoom,
  zusätzliche Pixeldichte 2, erzwungene Farben, reduzierte Bewegung und
  deckender Suchmaterial-Fallback, Tastaturmenüs/Tabs und Dialogfokusrückkehr.
- Die bestehenden drei separaten Browserprüfer bestehen weiterhin:
  Werkbank acht Layouts, Simulation 24 Layout-/16 Kurvenfälle, Auswertung acht
  Layouts; jeweils ohne JavaScriptfehler.

[Historischer Nachweis der ersten Zusammenführung](validation.json),
[aktueller Nachweis einschließlich Resizing](resizing-validation.json).
Die sechs echten Browseraufnahmen
unten zeigen die geprüfte gemeinsame Oberfläche mit bearbeiteten Beispielwerten.
Linux/macOS, echte Screenreader und physische DPI-/Monitorwechsel wurden für
diese Vorschau nicht ausgeführt. Die vorhandene native App wurde hier nicht umgebaut.

| Bereich | Dunkel | Hell |
| --- | --- | --- |
| Entwickeln | [Werkbank](develop-dark.png) | [Werkbank](develop-light.png) |
| Simulieren | [Messlabor](simulate-dark.png) | [Messlabor](simulate-light.png) |
| Auswerten | [Laufvergleich](analyze-dark.png) | [Laufvergleich](analyze-light.png) |

Wiederholen mit Node und den gebündelten Playwright-Abhängigkeiten (`NODE_PATH`):

```text
node tools/check_unified_workspace.cjs
node tools/check_unified_workspace.cjs --export
node tools/check_workspace_resizing.cjs
node tools/check_develop_designs.cjs
node tools/check_inspector_variants.cjs
node tools/check_simulation_designs.cjs
node tools/check_analysis_design.cjs
```

`--export` erneuert ausschließlich die sechs gemeinsamen Referenzbilder.
Rohbilder liegen ignoriert unter `build/unified-workspace-review/` und werden
nach Sichtprüfung entfernt. Quellen und Referenzen der einzelnen Entwürfe bleiben
erhalten. Bereinigungsumfang steht im maschinenlesbaren Nachweis.
