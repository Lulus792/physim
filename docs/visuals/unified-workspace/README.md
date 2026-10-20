# Physim · Gemeinsame Designvorschau

Stand: 20. Oktober 2026. [Vorschau öffnen](index.html).
Die Datei lässt sich direkt lokal im Browser öffnen, ohne Installation, Server
oder externe Ressourcen.

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

[Maschinenlesbarer Nachweis](validation.json). Die sechs echten Browseraufnahmen
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
node tools/check_develop_designs.cjs
node tools/check_simulation_designs.cjs
node tools/check_analysis_design.cjs
```

`--export` erneuert ausschließlich die sechs gemeinsamen Referenzbilder.
Rohbilder liegen ignoriert unter `build/unified-workspace-review/` und werden
nach Sichtprüfung entfernt. Quellen und Referenzen der einzelnen Entwürfe bleiben
erhalten. Bereinigungsumfang steht im maschinenlesbaren Nachweis.
