# D08 · Prüfung der getrennten Suchentwürfe

Datum: **30. Oktober 2026**. Status: **Browserstudie geprüft; nicht freigegeben**.
Die vorhandene Suchinsel bleibt, keine erneute Wahl erforderlich. Basis vor der Änderung:
`b4eaea5b486605a29f907d17e1dd50c27a186345`. Der genaue geprüfte Inhalt der vier
Quell-/Prüfdateien steht als normalisierter SHA-256 in
[source-hashes.json](source-hashes.json).

## Umgebung und Kommandos

Windows, Edge **156.0.4314.50**, Playwright headless. HTTP-Server auf
`127.0.0.1:8791` mit `D:/Physim/docs/visuals` als Wurzel.

```powershell
node --check docs/visuals/search-options/search.js
node --check tools/check_search_options.cjs
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/search-options/index.html'
node tools/check_search_options.cjs
git diff --check
```

Der [Maschinenbericht](validation.json) enthält Laufzeit, Browser, Basisstand,
ausgeführte Prüfgruppen und Ergebnis. Quellen-/Dokumentationslinks separat auf
existierende Ziele geprüft. Farbkontrast der benannten opaken Textrollen sowie
komponierter Auswahl-/Trefferflächen in [contrast.json](contrast.json) geprüft.
Das ist keine vollständige automatische oder assistive Accessibility-Abnahme.
Nach dem Abschlusslauf wurden nur Galerie-/Statusbeschriftungen zur Doppelung
korrigiert; [HTTP-Inhaltsprüfung](status-check.json) bestätigt den endgültigen
Hinweis, dass die bestehende Suche bleibt und keine erneute Wahl nötig ist.

## Soll und geprüftes Verhalten

- **144 Layoutfälle:** drei Anordnungen × acht Szenarien × drei Themes × zwei
  Breiten (1440/500). Keine horizontale Seitenüberbreite; Such-/Ersetzungsaktionen
  sichtbar und erreichbar. Fälle: mehrere/keine Treffer, leer, Nur lesen, Unicode,
  langer Text/Pfad, Laden und Quellenfehler.
- **Alle Varianten:** wörtliche Groß-/Kleinschreibung, Zähler, Wrap in beide
  Richtungen, Einzelersetzung und Löschen mit leerem Ersatz, gemeinsame Ersetzung
  mit einem Undo-Schritt, Abbruch und Verwerfen bei Query-/Ersetzungs-/Tabwechsel.
  Andere ungespeicherte Dateien erhalten; richtiger Dateibezug und UTF-8-Grenzen.
- **Versionsschutz:** Dokumentänderung verwirft Prüfung; nachträglich aktivierter
  alter Bestätigungsknopf verändert nichts. Ersetzung durch denselben Suchtext
  terminiert. Lange Fundstelle scrollt horizontal und vertikal in den Codebereich.
- **Fokus und Darstellung:** Escape/Ctrl+F geben Fokus zurück bzw. öffnen die
  Suche. C-Fundstelle wählt den Bereich und fokussiert Code. Pfeiltasten wechseln
  Tabs. A2-Textbereich bleibt bei Suchfeldfokus erhalten. Eingabetext wird escaped.
- **Anpassbarkeit:** 360 Pixel und 200 % Wurzelschrift einschließlich Code,
  Ersetzungsprüfung/Abbruch, reduzierte Transparenz, Forced Colors/Reduced Motion,
  zweifache Browser-Pixeldichte. Kein echter Monitor-/OS-DPI-Wechsel behauptet.

## Visueller Abgleich

Die vollständigen Werkbankaufnahmen wurden nach dem Lauf geprüft:
[A dunkel](a-dark.png), [B dunkel](b-dark.png), [C dunkel](c-dark.png),
[C schmal/hell/Unicode](c-narrow-light.png). Unterschiedliche Anordnung, erhaltene
Werkbankfarben, Suchtreffer/A2, Dateibezug, Aktionsgrenzen und Lesbarkeit abgleichen.
Die Galerie/Prüfschalter sind keine native UI-Vorgabe.

## Gefundene und behobene Befunde

Ein anfänglicher Prüfzähler erwartete vier statt der fünf tatsächlichen wörtlichen
Teilzeichenfolgen; Erwartung an den vorhandenen Suchvertrag angepasst. Eine
doppelte lokale Variablenbenennung bei der Cursorbindung wurde vor Abnahme
korrigiert und per Syntaxprüfung geprüft.

Bei der erweiterten Eingabe-/Versionsprobe lieferten die Browser-Eingaben mehrfach
denselben unveränderten Text. Der vorherige Handler legte dafür leere Undo-Schritte
an. Nun werden Text-No-ops vor der Historie verworfen. Eine gezielte Gegenprobe
und der vollständige Abschlusslauf prüfen, dass eine echte Textänderung mit einem
Undo wieder zum vorherigen Inhalt zurückkehrt. Veraltete Bestätigung bleibt wirkungslos.

## Grenzen / nächste Verantwortung

Keine native Datei-I/O, Clipboard-, Speicher- oder Buildoperation, kein Projekt-
Dateisystemscan. „Alle ersetzen“ bleibt eine gekennzeichnete Entwurfserweiterung.
Die native Suche wurde gelesen, für diese reine Designstudie nicht neu gebaut.
Keine native Windows-/Linux-/macOS- oder reale Screenreaderabnahme. Parallele
native Änderungen sind nicht Bestandteil dieses Designpatches.

Der nachträgliche Nutzerhinweis bestätigt die Doppelung dieser Layoutfrage:
Grundgestaltung und Einzelersetzung bestanden bereits. [Bestandsabgleich](../README.md#bestandsabgleich-und-verbleibende-detailarbeit)
beachten; die Zusatzstudie ist keine neue Pflichtentscheidung oder Freigabe.
D09 ist die nächste neue Designrunde; bestehende Verträge bleiben verbindlich.
