# D09 · Browserprüfung der Popup-Details

Datum: **30. Oktober 2026**. Ergebnis: **bestanden, Designauswahl offen**.
Basis vor dieser Änderung: `b6b27ddb2d3a3515b80a985dca05e6d8986a4295`.
Genaue geprüfte Quellen und gemeinsame CSS-Abhängigkeit als normalisierte
SHA-256 in [source-hashes.json](source-hashes.json).

## Kommandos und Umgebung

Windows, Edge **156.0.4314.50**, Playwright headless. HTTP-Server auf
`127.0.0.1:8791`, Wurzel `D:/Physim/docs/visuals`.

```powershell
python tools/build_completion_preview_data.py --check
node --check docs/visuals/completion-options/completion.js
node --check tools/check_completion_options.cjs
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/completion-options/index.html'
node tools/check_completion_options.cjs
git diff --check
```

Der [Maschinenbericht](validation.json) enthält Browser, Plattform, Basisstand,
Layoutanzahl, Prüfgruppen und Seitenfehler. Der deterministische Generatorvergleich
bestätigt alle **1.264** Einträge des lokalen Katalogs samt Referenzauszügen.
Das Fixture-Asset enthält zusätzlich die Hashes der gelesenen Katalog-/Referenzdateien.

## Geprüfte Aufgaben

- **216 Layoutfälle:** drei Popup-Varianten × zwölf Szenarien × drei Themes ×
  zwei Breiten (1440/500). Keine horizontale Seitenüberbreite; sichtbare Popups
  liegen innerhalb des Editorrahmens. Kein Popup bei leerer Treffermenge,
  Kommentar/Zeichenkette oder Nur lesen; lokale Erklärung bleibt erreichbar.
- **Alle Varianten:** Auf/Ab/Pos1/Ende, Enter, Tab und Klick; genau der ganze
  Bezeichner wird ersetzt, angrenzender Unicode-Präfix bleibt unverändert.
  Dirty-Stand, Fokus zum Editor und Undo; Escape ohne Textänderung. Physim-
  Instanz-/statische Member erhalten den Empfänger; C-Typen werden als Namen eingefügt.
- **Identitäten:** alte DOM-Einfügeaktion nach Revision oder Dokumentwechsel
  verworfen; andere Dokumententwürfe erhalten. Neue aktive Datei nicht durch
  einen alten Vorschlag verändern. Auswahl bzw. Cursorbewegung schließt das Popup.
- **Referenz:** exakt ausgewähltes Symbol und Quellpfad in Demo-Rückmeldung,
  kein Code-Edit. Umschalt+F1 für `ps_channel_add` mit originalen Hinweisen
  `context`/Experimentkontext und `unit`/PS_RADIAN. Fehler/Laden sperren nur
  Referenzaktion, nicht Katalogsignatur/Einfügen; manuelles Fixture-Ladeende.
- **Grenzen:** Strings, C-Punkt-/Pfeilmember, überlanger Präfix und markierter
  Text erzeugen keine unerlaubte Ergänzung. Inhalte und Meldungen bleiben erhalten.
- **Anpassbarkeit:** 360 Pixel mit 200 % Wurzelschrift; Popup klappt über den
  unteren Cursor, realer Codescroll schließt. Forced Colors/Reduced Motion mit
  Tastatureinfügen, zweifache Browser-Pixeldichte. Kein realer OS-DPI-Monitorwechsel.

## Visueller Abgleich und Korrekturen

[A dunkel](a-dark.png), [B dunkel](b-dark.png), [C dunkel](c-dark.png),
[B schmal/hell/lange Signatur](b-narrow-light.png) wurden visuell geprüft.
Geprüft: gleiche Werkbank/A2-Rollen, Popup-Unterschiede, Rollen/Modulnamen,
vollständige Signaturen, intern erreichbare Infos/Aktionen, Randbegrenzung.
Präfix im Code wird nicht als ausgewählter Text vorgetäuscht.

Der erste Typ-Test erwartete `ps_vec3`; der echte Katalog beginnt bei diesem
Präfix mit `ps_vec2`. Die Erwartung wurde an die unveränderte Katalogreihenfolge
angepasst, die Produktliste nicht umsortiert.

Bei C verschob die erste Hover-Ausklappung das Klickziel. Jetzt verändern nur
Pfeiltasten die ausführliche aktive Zeile; Hover bewegt die Geometrie nicht.
Leseklicks in der Kurzinfo fügen nichts ein, Referenz liegt als eigener Button
außerhalb der Optionen. A erhält eine begrenzte scrollbare Liste und separat
scrollbare Info, damit die Kurzinfo nicht halb unter dem Fuß verschwindet.

Die erste Scrollprobe hatte keinen überlangen Quelltext und konnte nicht scrollen.
Die Abschlussprobe verwendet tatsächlich überlangen Inhalt und eine echte
Scrollpositionsänderung. Verspätete unveränderte Scrollereignisse nach Öffnen
werden von einer nachträglichen Benutzerscrollbewegung unterschieden.

## Grenzen und Übergabe

Reine Browser-Fixture: kein echter Datei-/Clipboard-/Speicherzugriff, kein
Compiler/Simulationslauf. Kein neuer Worker, LSP, KI-Assistent oder typgeprüfter
Aufrufparameter. Referenzanforderung bindet in dieser Studie kein echtes natives
F1-Fenster; die vorhandene H-Designreferenz behält ihre eigenen Beispieldaten.
Native Quellen wurden gelesen; für diese getrennte Designstudie kein neuer
nativer Build ausgeführt. Parallele native Änderungen bleiben außerhalb des Patches.

DOM-Rollen/Tastaturtests sind keine echte Screenreaderabnahme. Native Popup-
Verdeckung, Fenster-/Dokumentidentitäten, Undo/Redo, UTF-8/Rune-/Glyphpositionen
und tatsächliche Windows-/Linux-/macOS-/Assistive-Laufzeiten nach Auswahl prüfen.
Keine Variante durch diesen Prüfbericht freigegeben. [Übergabe](../README.md#darstellung-fokus-und-übergabe).
