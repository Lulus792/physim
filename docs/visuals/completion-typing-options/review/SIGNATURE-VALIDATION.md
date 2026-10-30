# Parameterhilfe am Aufruf · ergänzter Browsernachweis

Datum: **30. Oktober 2026**. Basis vor dieser Ergänzung:
`d228b16e89148f3ffefe6886a797b3577bdcf18f`.
Windows, Edge **156.0.4314.50**, Playwright headless, HTTP auf
`127.0.0.1:8791`, Wurzel `D:/Physim/docs/visuals`.

Ergebnis: **Browserprüfung bestanden**. Design D bleibt gewählt. Dieser neue
Nachweis ergänzt die eingefrorene [D-Freigabeprüfung](APPROVAL-VALIDATION.md);
deren Dateien und ältere Bilder bleiben unverändert.

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
$env:PHYSIM_PREVIEW_URL='http://127.0.0.1:8791/completion-typing-options/index.html'
node tools/check_signature_help.cjs
node tools/check_completion_d.cjs
node tools/check_completion_typing_options.cjs
node --check docs/visuals/completion-typing-options/code-context.js
node --check docs/visuals/completion-typing-options/completion.js
python tools/build_completion_preview_data.py --check
git diff --check
```

## Soll und beobachtetes Ergebnis

- [18 Parameterhilfe-Layoutfälle](signature-validation.json): erstes, zweites und
  verschachteltes Argument × dunkel/hell/hoher Kontrast × 1440/500 Pixel.
  Die originale Signatur und jeweils `ps_vec3 a` bzw. `ps_vec3 b` sind sichtbar;
  aktive Stelle, Namen, Typen und Anzahl entsprechen dem echten Katalog.
- Liste und Signatur passen gleichzeitig in den Editorrahmen, überdecken sich
  nicht; im normalen Beispiel steht die Liste unten. Zusätzlich 360 Pixel bei
  200 % Wurzelschrift geprüft. Kein zusätzliches Oben-Beispiel erstellt, da
  der Nutzer diesen Wunsch ausdrücklich zurückgenommen hat.
- Variablenname übernehmen schließt die Liste und lässt die Aufrufsignatur
  sichtbar. Escape schließt die Hilfe; Strg+Umschalt+Leertaste öffnet sie erneut,
  der Editor behält Fokus. Schließen des Aufrufs und Dateiwechsel entfernen sie.
- Innerer Aufruf wählt dessen aktiven Parameter; nach dessen Ende wieder den
  äußeren. Kommata in Strings, Kommentaren und Indizes schalten nicht falsch
  weiter. Überzählige Argumente markieren keinen irreführenden letzten Parameter.
- [54 D-Regressionsfälle](signature-d-regression.json): fünf Zeilen, weitere
  Treffer scrollbar, schmale Scrollleiste, einstellbare Galeriehöhe, Scope-
  Gegenfälle, Variablenübernahme/Undo, gleiche Editor-/Kurzinfo-Syntaxfarben,
  drei weiterhin offene Hintergrundproben und schmale/große Textdarstellung.
- [150 allgemeine Regressionsfälle](signature-regression.json): bisherige
  Vergleichslayouts und Auto-Anzeige, bewusste Übernahme, Escape, IME-Ereignisse,
  Timer, Fokus, Dateiwechsel und bestehende Sonderzustände erhalten.
- Syntaxprüfungen und Kataloggenerator-Check bestanden. Kein Katalogsymbol
  oder Signaturtext für diese Parameterhilfe erfunden.

[Erstes Argument](signature-first.png), [zweites Argument](signature-second.png)
und [verschachtelter Aufruf](signature-nested.png) sind kompakte visuelle
Referenzen. Erste und zweite Ansicht visuell geprüft. Der
[Quellmanifest](signature-source-hashes.json) bindet HTML, JS, CSS, Katalog und
Prüfskripte; SHA-256 nach CRLF→LF-Normalisierung.

## Quellen und Grenzen

[VS Code Signature Help](https://code.visualstudio.com/docs/languages/javascript#_signature-help)
und [Visual Studio 2022 Parameter Info](https://learn.microsoft.com/en-us/visualstudio/ide/using-intellisense?view=vs-2022#parameter-info)
wurden als offizielle Primärquellen ausgewertet. Eigene Physim-Darstellung und
Interaktion daraus abgeleitet; keine lokale VS-2022-Instanz ausgeführt.

Der Browserhelfer kennt begrenzte C-Aufrufe, Parameterranges und echte Katalog-
signaturen. Keine vollständige C-/Physim-Semantik, Überladungsauflösung,
benutzerdefinierte Signaturindizierung oder Präprozessor-/Includeanalyse.
Native Integration muss tatsächliche sprachabhängige Daten und Revisionen
verwenden. Kein neues natives Backend, Build oder echter Screenreader-/OS-IME-/
Linux-/macOS-Nachweis. Laufende native Änderungen sind außerhalb dieses Patches.
Der genaue Kurzinfo-Hintergrundton bleibt ausdrücklich offen.

[Verbindliche Spezifikation](../APPROVED.md) und
[kopierfertiger Auftrag](../../unified-workspace/NEXT-CHAT.md).
