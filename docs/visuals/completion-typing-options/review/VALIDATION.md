# Neue Runde D/E/F · Browserprüfung

Datum: **30. Oktober 2026**. Ergebnis: **bestanden**. Automatische Anzeige ist
beauftragt; optische Auswahl D/E/F bleibt offen. Basis des Abschlusslaufs:
`8840b9c93df5ef8f7a178cfc89b6fb577806041d`. Normalisierte SHA-256 der geprüften
Quellen und CSS-/Katalogabhängigkeiten in [source-hashes.json](source-hashes.json).

## Ausgeführt

Windows, Edge **156.0.4314.50**, Playwright headless; HTTP auf `127.0.0.1:8791`,
Wurzel `D:/Physim/docs/visuals`.

```powershell
python tools/build_completion_preview_data.py --check
node --check docs/visuals/completion-typing-options/completion.js
node --check tools/check_completion_typing_options.cjs
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/completion-typing-options/index.html'
node tools/check_completion_typing_options.cjs
git diff --check
```

[Maschinenbericht](validation.json): 150 Layoutfälle, alle Prüfgruppen bestanden,
keine JavaScript-Seitenfehler. Der Generatorvergleich bestätigt weiterhin den
unveränderten Katalog mit 1.264 lokalen Namen und Referenzauszügen.

## Soll und Ist

- **150 Layoutzustände:** D/E/F × zwölf Kontexte × Hell/Dunkel × 1440/500 Breite,
  zusätzlich hoher Kontrast für alle drei bei beiden Breiten. Keine horizontale
  Seitenüberbreite; Liste und separate Dokumentation im Editorrahmen. Initial
  keine Übernahmeauswahl. Dokumentation über `i` in allen verfügbaren Fällen erreichbar.
- **Echte Tasteneingabe:** `p` bleibt ohne Popup, `ps` öffnet, weitere Eingabe
  filtert. Fokus bleibt im Code. Auf/Ab und Ctrl+N wählen; Enter/Tab übernehmen
  ausdrücklich den Namen, Undo stellt den getippten Stand wieder her.
- **Keine passive Übernahme:** Enter ohne Auswahl erzeugt nur einen Zeilenumbruch,
  Tab nur eine Einrückung. Umschalt+Tab verlässt das Feld. Strg+Leertaste im
  offenen Popup zeigt/versteckt die Doku, ohne einen Kandidaten auszuwählen.
- **Escape/manuell:** nach Escape bleibt derselbe Bezeichner trotz weiterer
  Eingabe ohne Auto-Popup; Strg+Leertaste öffnet erneut und erlaubt Übernahme.
  Auto-Schalter aus verhindert Tippauslöser, manuelles Öffnen bleibt möglich.
- **Physim:** `direction.` öffnet Member ohne Blattpräfix. Übernahme bewahrt den
  Empfänger. Lexikalischer Katalog, keine semantische Typanalyse behauptet.
- **Eingabegrenzen:** simulierte Paste öffnet nicht. Während synthetischer
  IME-Komposition keine Vorschläge, nach Abschluss Anzeige. Dokumentationsfehler
  und -ladefall verändern keine vorhandenen Katalognamen.
- **Identitäten/Timer:** Invalidation während der 140-ms-Pause verhindert die
  Anzeige; alte DOM-Einfügeaktion verworfen. Dateiwechsel verwirft Menü und
  280-ms-Zusatzinfo. Doku/Shift+F1 für `ps_channel_add` behält Symbol/Originaltext.
- **Anpassbarkeit:** 360 Pixel mit 200 % Wurzelschrift und separater Doku,
  Forced Colors/Reduced Motion mit Übernahme sowie zweifache Browser-Pixeldichte.

## Visuelle Prüfung und Befund

Geprüfte Referenzen: [D Liste](d-list.png), [E Liste](e-list.png),
[F Liste](f-list.png), [D Doku](d-docs.png), [E Doku](e-docs.png),
[F Doku](f-docs.png), [E schmal/hell](e-narrow-light.png).
Flache einzeilige Namen, Typ-/Quellspalten, separate Signatur/Kurzinfo, A2-Fläche,
gekürzte Zusatzspalten und intern scrollbar vollständige Inhalte abgleichen.

Die erste Ctrl+N-Probe deckte auf, dass der generische Modifier-Abbruch bereits
beim alleinigen Control-Keydown schloss. Modifier-only-Ereignisse werden jetzt
ignoriert; Ctrl+N sowie zweimal Strg+Leertaste wurden im Abschlusslauf geprüft.
Fertige IME-Zustände werden an das Dokument gebunden; Reset/Dateiwechsel verwirft
die alte Komposition. Das ersetzt keine reale OS-IME-Probe.

## Grenzen

Keine native Datei-/Clipboard-/Speicheroperation und kein neuer Compiler-/
Simulationslauf. Der UI-Timer ist kein asynchroner API-Provider. Referenz-Laden
bleibt eine gekennzeichnete Zustands-Fixture. Die bestehende F1-Designreferenz
hat ihre eigenen Daten; kein echter nativer Symbolsprung in dieser Browserseite.

Native Quellen wurden gelesen, nicht in dieser Designrunde umgebaut. Parallele
native Änderungen sind nicht Teil des Patches. Echte Screenreader-, OS-IME-,
Monitor-/DPI- und native Windows-/Linux-/macOS-Abnahmen bleiben offen.
Die früheren eingefrorenen A/B/C-Belege bleiben historische Daten; ihre
Gestaltungsrunde ist ausdrücklich verworfen. [Übergabe](../README.md#bestand-und-native-übergabe).
