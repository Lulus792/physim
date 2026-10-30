# ADR 023: Vervollständigung D mit fünf Zeilen und Variablenvorschlägen

Datum: 30. Oktober 2026. Status: **D vom Nutzer gewählt; Hintergrundton offen**.

Der Nutzer wählt Design D mit folgenden Ergänzungen:

- Standardmäßig **fünf sichtbare Einträge**. Das ist die Popup-Höhe, kein Limit
  von fünf gefundenen Kandidaten. Weitere Treffer bleiben scrollbar erreichbar.
  Später in Einstellungen A / Editor einstellbar, Standardwert 5.
- Die vertikale Scrollleiste horizontal schmaler und ruhiger gestalten.
- In Funktionsargumenten weiterhin vervollständigen, einschließlich sichtbarer
  Variablen und Funktionsparameter; keine Beschränkung auf API-Funktionsnamen.
- Code in der Kurzinfo mit demselben aktiven Syntaxschema wie im Editor zeichnen.
- Der genaue Hintergrundton der Kurzinfo ist ausdrücklich **noch nicht final**.
  Die Vorschau zeigt neutral, etwas heller und leicht kühl zur Feinabstimmung.

Maßgeblich: [APPROVED.md](../visuals/completion-typing-options/APPROVED.md),
[festgelegte Referenz](../visuals/completion-typing-options/index.html?approved=1&scenario=arguments),
[Gesamtauftrag](../visuals/unified-workspace/NEXT-CHAT.md).
Keine erneute Grundlayoutwahl D/E/F vorschalten. E/F bleiben Vergleichshistorie;
A/B/C waren zuvor verworfen. Automatische Anzeige nach [ADR 022](022-automatic-completion.md)
bleibt beauftragt. Die verbleibende Farbfrage blockiert die anderen Festlegungen nicht.

Die Browser-Fixture ergänzt einen begrenzten C-Scope-Lexer für Beispiele.
Native Umsetzung muss tatsächliche sprachabhängige Symbol-/Gültigkeitsbereiche,
Dokumentrevisionen und Rechte verwenden. Keine Variablen aus anderen Funktionen,
geschlossenen Blöcken, Kommentaren oder nach dem Cursor erfinden. Kein Versprechen
semantischer Typinferenz oder automatisch passender Laufzeitwerte.

Suchinsel, API-Karte, F1 H, A2 und alle anderen Freigaben erhalten. Browser-
Nachweise bestätigen keine fertige native Umsetzung oder Plattformabnahme.

## Nutzerpräzisierung: Parameterinfo am Aufruf

Zusätzlich zu Variablennamen möchte der Nutzer sehen, **welche Parameter die
aufgerufene Funktion erwartet**: vollständige Signatur, Namen, Typen und Anzahl;
der aktuell einzugebende Parameter wird hervorgehoben. Vorbilder sind die
[Signaturhilfe in VS Code](https://code.visualstudio.com/docs/languages/javascript#_signature-help)
und [Parameter Info in Visual Studio 2022](https://learn.microsoft.com/en-us/visualstudio/ide/using-intellisense?view=vs-2022#parameter-info).
Automatisch beim Aufruf und Argumentwechsel aktualisieren; Strg+Umschalt+Leertaste
als manuellen Zugang erhalten. Kandidaten-Kurzinfo und Aufrufsignatur bleiben
getrennte Hilfen. Die Signatur bleibt nach Übernahme eines Variablennamens sichtbar.

Der Nutzer hat das kurz gewünschte zusätzliche Beispiel mit Completion oberhalb
ausdrücklich zurückgenommen: **normale Liste unten beibehalten, keine zusätzliche
Oben-Variante gestalten**. Vorhandener Platzmangel-Fallback bleibt erhalten.
Die Browserreferenz zeigt erstes, zweites und verschachteltes Argument; genaue
Verträge und native Grenzen stehen in APPROVED.md.
