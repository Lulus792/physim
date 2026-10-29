# Prüfstand der getrennten Dateisicherheitsentwürfe

**29. Oktober 2026 · Auswahl A/B/C offen.** Windows, Headless Microsoft Edge
156.0.4314.50 mit Playwright. Nur die eigenständige HTML-Vorschau geprüft;
keine native Speicher-, Wiederherstellungs- oder Plattformabnahme.

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_file_safety_options.cjs
node --check docs/visuals/file-safety-options/safety.js
node --check tools/check_file_safety_options.cjs
```

**Final: Exit 0, 132 Layouts bestanden, keine JavaScript-Laufzeitfehler.**
[validation.json](validation.json) nennt Vorgänger-Commit, Umgebung und Ergebnisse;
[source-hashes.json](source-hashes.json) bindet die vor dem Commit geprüften
geänderten Quellen und die Referenzbilder. Der Vorgänger-Hash allein beschreibt
die neuen Dateien nicht.

## Tatsächlich ausgeführte Aufgaben

- **Layout:** drei Richtungen × elf Thema-/Szenariozustände × zwei Themes ×
  1440/500 logische Pixel Breite, jeweils 1100 Pixel Fensterhöhe. Kein horizontaler
  Seiten- oder Dialogüberlauf. Die ersten neun Hauptzustände visuell angesehen.
- **Speichern in allen Richtungen:** vollständiger Erfolg, erster Schreibfehler,
  Teilfehler nach erster gespeicherter Datei, erneuter Versuch ohne Rechteänderung,
  danach ausdrückliche Reparatur des virtuellen Schreibzugriffs und Abschluss.
  Teilweise gespeicherte Dateien bleiben gespeichert; der Projektwechsel wartet.
- **Abbrechen / Verwerfen:** explizite Bestätigung, Rückkehr ohne Datenverlust,
  bestätigtes Verwerfen sowie Abbruch während des verzögerten Beispiel-Speicherns.
  Eine spätere Timerantwort verändert den abgebrochenen Ergebnisstand nicht.
- **Dateikonflikte:** geschützter Kopiename, Eingabe über Zurück/Weiter erhalten,
  gelöschte Datei ohne Ladeaktion, Entwurf weiter bearbeiten sowie externe Fassung
  nur nach Bestätigung. Eine zweite externe Änderung verhindert den Abschluss,
  bis der neue Stand erneut geprüft und bestätigt wurde.
- **Wiederherstellung:** Projektquellen als Paar und zusätzliches Dokument einzeln,
  Laden ohne Speichern, späterer Speicherablauf mit genau diesem Umfang,
  bewusstes Verwerfen mit Rückweg, unlesbare und fehlende Sicherungen ohne
  erfundene geladene Entwürfe. Gespeicherte Dateien bleiben im Weiterarbeitsweg
  gespeichert; die beschädigte Sicherung wird nicht gelöscht.
- **Verbundener Ablauf:** Wiederherstellung mit externen Änderungen → Speichern
  blockiert → Konfliktprüfung → externe Fassung bewusst laden → übrige Entwürfe
  weiter prüfen. Die Entscheidung über eine Quelle autorisiert keine Änderung
  an einer anderen ebenfalls extern geänderten Quelle.
- **Tastatur / Popup:** tatsächlicher Browserdialog für A/B/C, zwölf Tab-Schritte
  innerhalb des Popups, Escape aus Bestätigung zuerst zurück und danach Abbruch,
  Fokus-Rückgabe. Radiofokus bleibt beim Neuzeichnen erhalten. Escape nach bereits
  abgeschlossenem Laden schließt nur das Popup und erhält das Ergebnis.
- **Weitere Browserzustände:** 500×850 mit 200% Wurzel-Schriftgröße und erreichbarem
  Abschluss des geführten Wiederherstellens, erzwungene Systemfarben, reduzierte
  Bewegung und separate Browser-Pixeldichte 2×.

Bei den frühen Prüfläufen wurden zwei Ursachen behoben: C zeigte zunächst nur
die aktuell betrachtete Quelle; nun sind beide Projektquellen als gemeinsame
Sicherung sichtbar und zur reinen Inhaltsvorschau wählbar. Der native HTML-
Fokusumlauf allein ließ einen Tab-Schritt aus dem Popup gelangen; die Testhülle
begrenzt ihn nun ausdrücklich. Danach wurden die betroffenen Aufgaben und die
vollständige aktuelle Runde erneut erfolgreich ausgeführt.

## Referenzbilder

Alle folgenden Bilder: 1440×1100 Browserfenster, Dunkel, 1× Pixeldichte;
Aufnahme der jeweiligen vollständigen Dialog-Clientfläche, kein Bildschirmfoto
der nativen App. Hell ist in den Layoutprüfungen enthalten.

| Thema | A · Kompakt | B · Vergleichen | C · Schritt für Schritt |
| --- | --- | --- | --- |
| Speichern | [A](save-a.png) | [B](save-b.png) | [C, Schritt 1](save-c.png) |
| Dateikonflikt | [A](conflict-a.png) | [B](conflict-b.png) | [C, Schritt 1](conflict-c.png) |
| Wiederherstellung | [A](recovery-a.png) | [B](recovery-b.png) | [C, Schritt 1](recovery-c.png) |

## Grenzen und nächste Zuständigkeit

Die Beispiele schreiben keine Dateien. Kopienamen, Daten und Speichervorgänge
existieren nur im Browserzustand. Es gibt keinen automatischen Merge und keinen
neuen nativen Dateidialog. Die HTML-Modalsperre ist eine Testhülle, kein Entwurf
gegen den bestehenden nativen Vertrag eigenständiger OS-Aufgabenfenster aus
ADR 015. Native Ownership und Weiterarbeit im Hauptfenster bleiben erhalten.

Nach einer Nutzerwahl Inhalte/Abläufe an vorhandene native Funktionen anbinden
und am dann aktuellen Stand prüfen. Echte Teilfehler, Dateirechte, OS-Fenster,
Screenreader, physische DPI-Wechsel sowie Windows/Linux/macOS bleiben gesondert
nachzuweisen. Vorher keine dieser Varianten ins fertige Design übernehmen.
