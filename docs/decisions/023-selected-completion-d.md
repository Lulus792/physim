# ADR 023: Vervollständigung D mit fünf Zeilen und Variablenvorschlägen

Datum: 30. Oktober 2026. Status: **D samt Parameterhilfe, neutralem Hintergrund
und präzisierten Codefarben abschließend freigegeben**.

Der Nutzer wählt Design D mit folgenden Ergänzungen:

- Standardmäßig **fünf sichtbare Einträge**. Das ist die Popup-Höhe, kein Limit
  von fünf gefundenen Kandidaten. Weitere Treffer bleiben scrollbar erreichbar.
  Später in Einstellungen A / Editor einstellbar, Standardwert 5.
- Die vertikale Scrollleiste horizontal schmaler und ruhiger gestalten.
- In Funktionsargumenten weiterhin vervollständigen, einschließlich sichtbarer
  Variablen und Funktionsparameter; keine Beschränkung auf API-Funktionsnamen.
- Code in der Kurzinfo mit demselben aktiven Syntaxschema wie im Editor zeichnen.
- Der Hintergrundton war zunächst offen. Die spätere Freigabe unten legt die
  gezeigte neutrale Gestaltung fest; andere Töne bleiben Vergleichshistorie.

Maßgeblich: [APPROVED.md](../visuals/completion-typing-options/APPROVED.md),
[festgelegte Referenz](../visuals/completion-typing-options/index.html?approved=1&scenario=arguments),
[Gesamtauftrag](../visuals/unified-workspace/NEXT-CHAT.md).
Keine erneute Grundlayoutwahl D/E/F vorschalten. E/F bleiben Vergleichshistorie;
A/B/C waren zuvor verworfen. Automatische Anzeige nach [ADR 022](022-automatic-completion.md)
bleibt beauftragt. Die abschließende Farbpräzisierung steht unten.

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

## Abschließende Freigabe und Codefarben

Der Nutzer bestätigt anschließend die Gestaltung für den umsetzenden Chat und
präzisiert: Farben für Code an Visual Studio 2022 und VS Code orientieren,
**Funktionen sind gelb**. Damit D samt Aufrufsignatur, fünf Zeilen und gezeigter
neutraler Kurzinfo als fertige Referenz aufnehmen; keine weitere Farb-/Layoutwahl
vorschalten. Die vorherige offene Hintergrundfrage ist erledigt.

Konkrete Rollwerte aus den offiziellen VS-Code-Themes Dark+/Light+ gemäß
APPROVED.md verwenden: Funktionen dunkel `#dcdcaa`, hell `#795e26` als
kontrastreiche Goldbraun-Variante; Typen türkis, Variablen/Parameter hellblau,
Schlüsselwörter blau, Kontrollfluss violett, Strings warm und Kommentare grün.
Alle Codekontexte teilen die Rollen, einschließlich Completion-Namen und aktiver
Parameterhilfe. Native Token-/Syntaxintegration ist Aufgabe des umsetzenden Chats.
Die frühere Abbildung der vorhandenen Werkbankfarben und pauschale `ps_`-Typfarbe
sind ersetzt. Systemkontrast und A2-Verhalten erhalten. Browsernachweis ist keine
native Abnahme. [Abschließender Nachweis](../visuals/completion-typing-options/review/COLOR-VALIDATION.md).
