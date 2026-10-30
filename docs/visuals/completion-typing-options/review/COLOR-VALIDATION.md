# Abschließende Freigabe D · gemeinsame Codefarben

Datum: **30. Oktober 2026**. Basis vor der Änderung:
`7e6a39457a691c2d23d3616f769a95d8b89eb077`.
Windows, Edge **156.0.4314.50**, Playwright headless, HTTP auf
`127.0.0.1:8791`, Wurzel `D:/Physim/docs/visuals`.

Der Nutzer gibt D einschließlich Parameterhilfe und gezeigter neutraler Kurzinfo
zur nativen Umsetzung frei. Letzte Präzisierung: Codefarben an VS Code und Visual
Studio 2022 orientieren, insbesondere Funktionen gelb. Frühere offene Hintergrund-
vermerke und Werkbank-Farbwerte sind damit überholt; eingefrorene Nachweise
bleiben unverändert historisch. Maßgeblich ist [APPROVED.md](../APPROVED.md).

```powershell
node --check docs/visuals/completion-typing-options/code-context.js
node --check docs/visuals/completion-typing-options/completion.js
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_completion_d.cjs
node tools/check_signature_help.cjs
python tools/build_completion_preview_data.py --check
git diff --check
```

## Soll und beobachtetes Ergebnis

- [54 D-Layout-/Verhaltensfälle und Farbprüfungen](color-validation.json):
  D und neutral sind auch bei alten Varianten-/Ton-URLs festgelegt. Fünf sichtbare
  Zeilen, weitere Treffer scrollbar, Variable-/Scope-/Undo-Regeln, gemeinsame
  Syntaxfarben, HTML-Escaping und 360 Pixel mit 200 % Wurzelschrift erhalten.
  Historische Töne sind nur außerhalb der freigegebenen Referenz wählbar.
- Funktionsfarbe stimmt in Editor, Aufrufsignatur, Completion-Name inklusive
  fettem Präfix und Kandidaten-Kurzinfo überein. Eigene Funktion `step` ebenso
  gelb wie `ps_vadd`. `ps_vec3` bleibt eine andere Typfarbe; Variablen/Parameter
  verwenden dieselbe Rolle in Editor, Liste, Kurzinfo und aktiver Parameterhilfe.
- Gegenfälle: `ps_vadd()` in Kommentar/Zeichenkette erzeugt keine Funktionsspans;
  `return` wird als Kontrollfluss unterschieden. Kein pauschales `ps_`→Typ mehr.
- Gemessener Mindestkontrast der tatsächlich gezeigten Syntaxtexte im Argument-
  beispiel, Kurzinfo und Signatur: dunkel **5,22:1**, hell **4,58:1**, eigene
  Kontrastdarstellung **10,32:1**. Komponierte Eltern-/Parameterflächen berücksichtigt.
  Das ist keine pauschale Aussage über jede mögliche Auswahl-/Systemkonfiguration.
- Der erste helle Lauf fand nur **3,48:1** für den übernommenen Light+-Typfarbton
  auf der aktiven Parameterfläche. Auf `#1f6b81` abgedunkelt, erneute Prüfung
  bestanden. Diese eigene Physim-Kontrastanpassung ist in der Palette vermerkt.
- [18 Parameterhilfe-Regressionsfälle](color-signature-regression.json): erstes,
  zweites und verschachteltes Argument × drei Themes × 1440/500 Pixel;
  aktive Namen/Typen, keine Überdeckung, normale Liste unten, Einfügen,
  Escape/manueller Zugang, innere Kommata, Dateiwechsel und kleine Darstellung.
- JS-Syntax, Generator gegen echte lokale Katalogquellen und Whitespace geprüft.

[Dunkle Codefarben](color-dark.png) und [helle Codefarben](color-light.png)
wurden visuell geprüft. [Quellmanifest](color-source-hashes.json) hält die
geprüften Quellen, CSS-/Katalogabhängigkeiten und Prüfskripte fest; SHA-256 nach
CRLF→LF-Normalisierung. Ältere Referenzbilder nicht ersetzt.

## Herkunft und verbleibende native Arbeit

Offizielle Primärquellen gelesen am 30. Oktober 2026:
[Dark+](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/dark_plus.json),
[Light+](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_plus.json),
[dunkle Basis](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/dark_vs.json),
[helle Basis](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_vs.json).
Die konkrete Palette folgt diesen Quellen mit der genannten Anpassung;
VS-2022-Orientierung ist kein Nachweis einer identischen Palette für alle Sprachen.

Der andere Chat muss gemeinsame native Token-/Syntaxrollen ergänzen und alle
Codekontexte anbinden. Browser-Katalog-/Aufrufheuristik ist kein vollständiger
Sprachdienst. Keine native Datei geändert, kein nativer Build oder echte
Screenreader-/OS-/Linux-/macOS-Abnahme ausgeführt. A2, fünf Zeilen, automatische
Anzeige und alle bisherigen Designfreigaben bleiben bestehen.

[Entscheidung](../../../decisions/023-selected-completion-d.md) ·
[kopierfertige Übergabe](../../unified-workspace/NEXT-CHAT.md).
