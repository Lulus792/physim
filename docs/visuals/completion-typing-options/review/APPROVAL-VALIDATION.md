# D gewählt · fünf Zeilen, Argumentvariablen und Syntaxfarben

Datum: **30. Oktober 2026**. Basis vor der Ergänzung:
`9151e5eb186f1ce73edee01db4957cc8ead2453b`.
Ergebnis: **Browserprüfung bestanden**. D ist gewählt, Hintergrundton noch offen.

Windows, Edge **156.0.4314.50**, Playwright headless, HTTP auf `127.0.0.1:8791`
mit `D:/Physim/docs/visuals` als Wurzel.

```powershell
node --check docs/visuals/completion-typing-options/code-context.js
node --check docs/visuals/completion-typing-options/completion.js
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/completion-typing-options/index.html'
node tools/check_completion_typing_options.cjs
node tools/check_completion_d.cjs
git diff --check
```

## Nachweise

- [D-Freigabeprüfung](approval-validation.json): **54 Layoutfälle**, drei Themes
  × drei Hintergrundproben × API-/Argument-/Parameterbeispiel × 1440/500 Breite.
  Default und alte E-Parameter erzwingen D, Variantenwahl verborgen. Kein
  horizontaler Seitenüberlauf; Popup und Kurzinfo liegen im Editorrahmen.
- Fünf tatsächliche Zeilenhöhen als Standard, weiterer Inhalt bleibt scrollbar.
  Reduktion auf drei per Galerie-Prüfschalter geprüft; schmale Scrollleiste
  geometrisch gemessen. Kein Abschneiden des Ergebnisbestands.
- `ps_vlength(ve)` ergänzt `velocity`; Undo stellt den Originaltext wieder her.
  Leeres Argument bietet unter anderem `dt` und `gravity`. Echte Eingabe von
  `(` nach Funktionsname und Leerzeichen nach `,` öffnet weiter die lokalen Namen.
- Scope-Gegenfälle: Variablen aus fremder Funktion, geschlossenem innerem Block,
  fremdem Parameter sowie gerade unvollständige Initialisierung werden nicht
  angeboten. Eigener Parameter und vorherige Deklaration bleiben erreichbar.
- Editor/Kurzinfo verwenden denselben Syntaxhelfer und dieselben CSS-Rollen.
  Tatsächliche Keyword-/Typfarben sind gleich; HTML aus Quelltext wird escaped.
  Farbrollen und `ps_`-Behandlung gegen den gelesenen nativen Werkbankstand abgeglichen.
- Drei unterschiedliche Hintergrundproben gemessen, keine davon als finale
  Nutzerentscheidung ausgegeben. 360 Pixel und 200 % Wurzelschrift geprüft.
- [Regressionsbericht](approval-regression.json): **150 Vergleichs-/Verhaltensfälle**
  einschließlich Auto-Anzeige, expliziter Übernahme, Escape, IME-Ereignissen,
  Fokus, Timer, Dateiwechsel, Forced Colors und zweifacher Browserdichte bestanden.
  Dieser Lauf prüft die D-/Scope-Ergänzung vor der letzten Syntaxklassifizierungs-
  Angleichung; der abschließende 54-Fälle-Lauf prüft den finalen gemeinsamen Renderer.

Der erste Farbvergleich verwendete einen Editor-Fixturetext ohne Typ-Token.
Das Parameterbeispiel enthält nun eine echte `ps_context`-Deklaration, sodass
Keyword- und Typfarben in beiden Kontexten tatsächlich verglichen werden.
Keine Produktionseinstellung oder native Datenquelle durch den Test ersetzt.

## Referenzen und Grenzen

[Fünf Zeilen](d-five.png), [Argumentvariable](d-arguments.png),
[Syntaxfarben](d-syntax.png) wurden visuell geprüft. Finale Quell-/Prüfdateien
und CSS-/Katalogabhängigkeiten stehen in [approval-source-hashes.json](approval-source-hashes.json).
Die bisherigen [D/E/F-Vergleichsbelege](VALIDATION.md) bleiben unverändert historisch.

Der Browser-Scope-Lexer unterstützt begrenzte C-Deklarationen/Parameter und
geschweifte Blockbereiche. Keine vollständige C-/Physim-Semantik, Präprozessor-,
Include-, Funktionspointer-, For-Initialisierer- oder Typkompatibilitätsanalyse
behauptet. Nativ tatsächliche sprachabhängige Gültigkeitsbereiche anbinden und
weitere Grammatik-/Scope-Gegenfälle prüfen. Keine Handbuchreferenz für lokale
Variablen erfinden.

Kein natives Preferences-Backend, Datei-/Clipboard-/Compilerzugriff oder neuer
Worker. Kein neuer nativer Build für diese Vorschau ausgeführt. DOM-/Browser-
Prüfung ist keine echte Screenreader-/OS-IME-/Windows-/Linux-/macOS-Abnahme.
Parallele native Änderungen bleiben außerhalb dieses Designpatches.

[Verbindliche Übergabe und offene Farbfrage](../APPROVED.md).
