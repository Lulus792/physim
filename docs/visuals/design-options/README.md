# Physim – getrennte Designvarianten

**Weitere Freigabe vom 29. Oktober 2026:** Speichern, Dateikonflikte und
Wiederherstellung sind im **Design B2 mit gemeinsamer Git-Diff-Ansicht** gewählt.
[Spezifikation](../file-safety-options/APPROVED.md) ·
[festgelegte Referenz](../file-safety-options/index.html?approved=1&topic=save).
Die frühere Übernahmesperre dieser Runde ist aufgehoben.

Zusätzliche, eigenständige [Studie zu Markierung und Rechtsklick-Menüs](../selection-options/index.html)
vom 29. Oktober 2026: **A2 · Dezenter Schleier ist als finales Markierungsdesign
freigegeben** und ersetzt A. [Verbindliche Spezifikation](../selection-options/APPROVED.md).
Die unten freigegebene Bereichskombination bleibt erhalten.

Stand: **25. Oktober 2026. Status: Auswahl vom Nutzer freigegeben.**

Der Nutzer hat mehrere voneinander getrennte Designs für Protokoll und fehlende
Inhalte, einen Dialog zum Erstellen eines Projekts und einen eigenen Datei-Explorer
beauftragt. Zusätzlich soll eine vollständige Leeransicht des derzeit umgesetzten
Layouts entstehen. Anschließend hat er die konkrete Kombination ausgewählt:
**Leeransicht bestätigt; Protokoll C; Szene & Werte A; Auswertung A;
Läufe/Serien A; Neues Projekt B; Datei-Explorer A.**
[Verbindliche Spezifikation](APPROVED.md) · [ADR 011](../../decisions/011-selected-workspace-additions.md).

[Gemeinsame freigegebene Referenz](index.html?approved=1&topic=empty) ·
[Vollständige freigegebene Leeransicht](index.html?approved=1&topic=empty&standalone=1) ·
[Alle Varianten vergleichen](index.html)

Die Dateien funktionieren direkt lokal ohne Server, Installation, externe Fonts
oder Netzwerkressourcen. Der lokale Vorschau-Server kann dieselbe Galerie unter
`http://127.0.0.1:8791/design-options/` anzeigen.

## Drei Richtungen, einzeln kombinierbar

| Richtung | Idee | Typische Anordnung |
| --- | --- | --- |
| **A · Werkbank** | Kompakt und direkt | Liste / Arbeitsfläche / feste Details; Projektdialog mit Vorschau links und Einstellungen rechts, analog zum Export |
| **B · Fokus** | Ruhig und geführt | Kartenübersicht, gezielte Detailansicht, große Szene; Projektanlage in drei Schritten |
| **C · Navigator** | Struktur und Kontext | Herkunft bzw. Hierarchie / Inhalt / Eigenschaften; Projektvorlagen und Dateiordner als eigene Spalten |

Die Unterschiede sind strukturell und interaktiv; die Varianten verwenden
absichtlich dieselben semantischen Farben und dieselben Beispielinhalte.
Es gibt **18 neue Varianten**, jeweils drei für diese sechs Themen:

| Thema | A | B | C |
| --- | --- | --- | --- |
| Fehler, Warnungen und Protokoll | [Werkbank](index.html?topic=protocol&variant=a) | [Fokus](index.html?topic=protocol&variant=b) | [Navigator](index.html?topic=protocol&variant=c) |
| Szene, Kräfte, Texte und Werte | [Werkbank](index.html?topic=simulation&variant=a) | [Fokus](index.html?topic=simulation&variant=b) | [Navigator](index.html?topic=simulation&variant=c) |
| Auswertung und variable Berichte | [Werkbank](index.html?topic=analysis&variant=a) | [Fokus](index.html?topic=analysis&variant=b) | [Navigator](index.html?topic=analysis&variant=c) |
| Läufe, Aufzeichnungen und Serien | [Werkbank](index.html?topic=runs&variant=a) | [Fokus](index.html?topic=runs&variant=b) | [Navigator](index.html?topic=runs&variant=c) |
| Neues Projekt als Popup | [Vorschau + Formular](index.html?topic=project&variant=a) | [Drei Schritte](index.html?topic=project&variant=b) | [Vorlagen + Konfiguration](index.html?topic=project&variant=c) |
| Eigener Datei-Explorer | [Liste + Vorschau](index.html?topic=files&variant=a) | [Kacheln + Detail](index.html?topic=files&variant=b) | [Spaltennavigation](index.html?topic=files&variant=c) |

Die ausgewählte Kombination ist jetzt in [selection.js](selection.js) festgehalten.
Die Referenz mit `approved=1` verwendet pro Thema automatisch die freigegebene
Richtung. Sie öffnet den Projektdialog stets als B und den darin verwendeten
Explorer stets als A. Die Vergleichsgalerie bleibt erhalten; ihre Bedienelemente
ändern die dokumentierte Freigabe nicht.

## Vollständiger Arbeitsraum ohne Projekt

[Direkt im vollständigen Fenster ansehen](index.html?topic=empty&standalone=1).
Im Galeriethema **Ohne Projekt** gibt es keine A/B/C-Auswahl: Hier wird die
bisherige gemeinsame Designsprache für den leeren Start zusammengeführt.

- Menüleiste, drei Bereichsreiter, Kontext-/Statusleisten, leere Seitenbereiche
  und Protokoll sind als vollständige Oberfläche vorhanden.
- **Entwickeln:** keine Datei, leere Workspace-/Codekontextbereiche, Projekt- und
  Ordneraktionen statt vorgetäuschtem Editorinhalt.
- **Simulieren:** keine Szene/Objekte/Messwerte; Start, Schritt, Stop und Reset
  deaktiviert. **Auswerten:** keine Laufbibliothek oder Diagramme; Analyse und
  Export deaktiviert.
- Zusätzliche getrennte Beispiele für „Ordner ohne Projekt“ und einen vorhandenen
  letzten Workspace. Der letzte Workspace ist ausdrücklich ein Beispielort.
- Bereichsreiter unterstützen Pfeiltasten/Pos1/Ende. Seitenbereiche lassen sich
  umschalten. Menüs sind tastaturbedienbar; es gibt keine aktive Buildaktion.
- „Neues Projekt“ und „Ordner öffnen“ zeigen in der freigegebenen Referenz
  **Projekt B und Explorer A**.
  Auch nach einer Projektvorschau bleibt die Leeransicht leer.

Die Palette und Grundmaße stammen aus `unified-workspace/workspace.css`, die
Bereichsstruktur aus den gewählten Referenzen. Für die Zustände wurden auch
`app/design_ui.inc` (`!a->loaded`, `workspace_open`, letzter Workspace) und
`app/project_manager_ui.inc` gelesen. Dies ist eine vollständige **HTML-Referenz**,
kein Screenshot einer abgenommenen nativen Leeransicht und kein Pixelgleichheitsbeleg.

## Was sich ausprobieren lässt

- Protokoll nach Schweregrad/Herkunft/Text filtern, Detail und Originalmeldung
  öffnen, Beispielquellstelle zeigen, sichtbare Beispiele als TXT herunterladen.
  Kopieren meldet fehlende Browserunterstützung oder verweigerte Berechtigung.
- Kräfte separat auswählen, Beschriftungen und Vektoren umschalten; Hierarchie
  und alle zehn Szenentypen im Baum ansehen. Modellparameter und adaptive
  Grenzen prüfen, elf Messkanäle in Listen-/Kartenformen vergleichen.
- Berichtsläufe ändern und den festgehaltenen Beispielbericht gezielt aktualisieren;
  fehlende Kanäle, wiederhergestellte Daten, leere Bibliothek und Analysefehler
  über die Zustandswahl ansehen.
- Parameterstudie und Teilfehler auswählen, Beispielserie abbrechen und einen
  Zeitpunkt in einer langen Aufzeichnung wählen. Start-/Antwort-/Limitfälle sind
  eigene Zustände; es läuft kein echter Runner.
- Projektname, eine von acht vorhandenen Modellvorlagen, Experiment-/Analysesprache
  und „Nur Auswertung“ wählen. Ein eigener Explorer wählt den Zielordner. Der
  dreistufige Ablauf behält Eingaben beim Zurückgehen; Duplikat-/Schreibfehler
  erscheinen innerhalb des Dialogs. Erfolg erzeugt nur eine Zusammenfassung.
- Im Explorer Ordner per Doppelklick oder Aktion öffnen, über Pfad/Zurück/Nach oben
  navigieren, suchen, sortieren, Listen/Kacheln vergleichen, versteckte Einträge
  einblenden, Dateiinhalte ansehen und einen **nur im Speicher** existierenden
  Beispielordner anlegen. Fehlende Dateien, externe Änderungen und Zugriffsfehler
  sind bewusst gestartete Beispiele.

## Isolation und Freigabe

Diese Referenz lädt ausschließlich `options.css`, `data.js`, `selection.js`, `options.js` und
`dialogs.js` aus ihrem eigenen Ordner. Sie importiert keine Bridge, keinen
Iframe und keinen Zustand der aktuellen gemeinsamen Vorschau. Native Quellen
werden durch diese Designarbeit nicht verändert. Keine echten Datei-, Build-,
Simulations-, Import- oder Erstellungsaufträge; Daten verschwinden beim Neuladen.

Die bestehende gemeinsame Vorschau bleibt als bisheriger Stand erhalten.
Die neue kombinierte Referenz und [APPROVED.md](APPROVED.md) sind für die
ausgewählten Ergänzungen maßgeblich. Der frühere Protokollentwurf vom
24. Oktober wird bei der Anordnung durch **C** ersetzt. Seine gemeinsamen
Verhaltensanforderungen gelten weiter. Der bereits freigegebene Grafikexport
bleibt verbindlich und wird separat geöffnet.

Die direkte Nutzerauswahl ist die Grundlage für die weitere native Umsetzung.
Eine erneute Variantenfreigabe ist für die ausgewählten Teile nicht nötig.
Browserprüfungen bleiben von nativer Funktions- und Plattformabnahme getrennt.

## Inhaltsabgleich und Grenzen

Der [Abdeckungsnachweis](COVERAGE.md) ordnet die Punkte aus dem
[Inhaltsinventar](../unified-workspace/CONTENT-INVENTORY.md) den neuen Entwürfen zu.
Kräfte haben hier ausdrücklich definierte Beispielwerte und eigene Maßstäbe.
Eine allgemeine Zuordnung von `PS_ARROW` zu Newtonwerten ist weiterhin ein neuer
Datenvertrag und wird nicht aus Geometrie abgeleitet.

Browserprüfung und Bilder: [Prüfbericht](review/VALIDATION.md).
Kein nativer Drei-Plattform-, Screenreader-, Physik- oder Performance-Nachweis.
Die Szene ist eine illustrative SVG-Grafik, kein neuer 3D-Renderer. Der Explorer
zeigt einen Datensatz von Beispielorten, keine echte Datenträgeransicht.
