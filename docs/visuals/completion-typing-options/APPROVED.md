# Automatische Vervollständigung · D ist gewählt

**Nutzerwahl vom 30. Oktober 2026:** Design D mit fünf sichtbaren Zeilen,
schmaler Scrollleiste, Variablen in Funktionsargumenten und gemeinsamen
Syntaxfarben. **Abschließend samt Parameterhilfe und neutraler Kurzinfo freigegeben.**
Die letzte Nutzerpräzisierung legt VS-Code-/VS-2022-orientierte Codefarben fest,
insbesondere **Funktionen gelb**. Diese ersetzt die zuvor abgebildete native Palette.
[ADR 023](../../decisions/023-selected-completion-d.md),
[Auto-Anzeige ADR 022](../../decisions/022-automatic-completion.md) und
[Gesamtübergabe](../unified-workspace/NEXT-CHAT.md) beachten.

[Freigegebene Referenz](index.html?approved=1&scenario=arguments&tone=neutral) ·
[API-Namen](index.html?approved=1&scenario=normal) ·
[Signatur/Parameter](index.html?approved=1&scenario=parameters).
Standardaufrufe und `approved=1` erzwingen D; `approved=0` lässt E/F nur zum
historischen Vergleich anzeigen. Keine erneute Grundlayoutwahl nötig.

## Liste und Einstellungen

Die flache D-Liste bleibt etwa 400 logische Einheiten breit. Namen einzeilig,
Typ rechts, kleine Rollenicons links. **Fünf sichtbare Zeilen** bestimmen die
Standardhöhe: tatsächliche Zeilenhöhe × 5 plus dünner Fuß/Rahmen. Keine starre
Pixelhöhe, die bei großer Schrift halbe Einträge erzeugt. Bei weniger Treffern
weiter schrumpfen, bei wenig Editorraum auf diesen begrenzen.

Alle Treffer bleiben im Bestand. Scrollen bzw. Pfeilnavigation erreicht auch
Eintrag 6 und folgende; der Zähler meldet die Gesamtzahl. Keine providerseitige
Kürzung auf fünf. Listen-/Kurzinfo-Scrollleiste mit schmalem etwa 6-px-Thumb
bzw. systemfähigem Thin-Fallback, ruhige transparente Spur und abgerundeter Thumb.
Kein horizontaler Scrollbedarf allein durch lange Vorschlagsnamen.

Einstellungen A / Editor um **„Sichtbare Vervollständigungen“**, Standard **5**,
ergänzen. Wert im bestehenden Preferences-Modell speichern und mit dessen
Übernehmen/Abbrechen/Standardwert-Vertrag behandeln. Galerie-Numerik 1–20 ist nur
ein Prüfschalter, kein bereits implementiertes natives Backend. Der Nutzer
wünscht spätere Einstellbarkeit; dies ist Teil des nativen Folgeauftrags.

## Variablen in Argumenten

Nach `(` und `,` innerhalb eines Funktionsaufrufs passende sichtbare Namen
anbieten, auch ohne zwei bereits getippte Bezeichnerzeichen. Beispiel:

```c
void step(double dt) {
    ps_vec3 velocity = ps_v3(0.0, 1.0, 0.0);
    ps_vec3 gravity = ps_v3(0.0, -9.81, 0.0);
    double damping = 0.02;
    ps_vec3 result = ps_vadd(ve, gravity);
}
```

Bei `ve` wird `velocity` vorgeschlagen; im leeren Argument sind unter anderem
`velocity`, `gravity`, `damping` und der Funktionsparameter `dt` sichtbar.
Variablen haben eine eigene Rolle, ihren deklarierten Typ und die Quellstelle.
Ihre Kurzinfo bietet keinen erfundenen API-Handbucheintrag. Beim Einfügen nur
den Argumentbezeichner ersetzen, Callee, Klammern und andere Argumente erhalten.
Undo bleibt ein zusammenhängender Schritt.

Sichtbare Variablen/Parameter vor API-Namen zeigen, Präfix weiterhin beachten.
Geschlossene oder fremde Funktionsblöcke, Namen aus Kommentaren/Strings und
erst nach dem Cursor deklarierte Namen auslassen; innere Deklarationen können
äußere Namen überdecken. Eine Variable nicht schon in ihrer eigenen noch
unvollständigen Initialisierung als fertige vorherige Deklaration anbieten.

Der Browser-[Scope-Helfer](code-context.js) illustriert begrenzte C-Deklarationen,
Parameter und Blockbereiche. Er ist **kein vollständiger C-/Physim-Parser**.
Native Integration nutzt/revidiert die tatsächlichen sprachabhängigen
Symbol-/Scope-Daten. Der Auftrag gilt für Variablenvorschläge im unterstützten
Editor, nicht nur die handgeschriebene Fixture. Keine semantische Typkompatibilität,
Präprozessoranalyse, Includes oder tatsächlichen Variablenwerte vortäuschen.

## Parameterhilfe: erwartete Argumente sichtbar machen

Neben der Liste sichtbarer Namen eine **eigene kompakte Aufrufsignatur** zeigen:
Rückgabetyp, Funktionsname, vollständige Parameternamen und Typen. Den aktuellen
Parameter fett mit dezenter A2-Fläche und Unterkante hervorheben; seine Syntaxfarben
erhalten. Darunter beispielsweise „Parameter 2 von 2 · ps_vec3 b“. Die Hilfe
beschreibt den bestehenden Aufruf, die Kandidaten-Kurzinfo den gewählten Vorschlag.

[Erstes Argument](index.html?approved=1&scenario=arguments),
[zweites Argument](index.html?approved=1&scenario=signature-second) und
[verschachtelter Aufruf](index.html?approved=1&scenario=signature-nested) sind
interaktive Referenzen. Die normale Completion-Liste bleibt **unten**, unter der
Signatur mit kleinem Abstand; keine Überdeckung. Das zusätzlich gewünschte
Oben-Beispiel wurde vom Nutzer zurückgenommen. Bestehendes Ausweichen bei echtem
Platzmangel erhalten, keine neue Gestaltungsvariante daraus ableiten.

Beim Öffnen eines bekannten Aufrufs automatisch zeigen, bei Komma/Cursorwechsel
aktualisieren. **Strg+Umschalt+Leertaste** ruft die Parameterhilfe manuell auf;
Strg+Leertaste bleibt Completion/Kandidaten-Kurzinfo. Übernahme eines Namens oder
Schließen der Liste schließt nicht zugleich die Aufrufsignatur. Escape schließt
zuerst die offene Liste, weiteres Escape die Parameterhilfe für diesen Aufruf;
manueller Zugang öffnet sie erneut. Nach Verlassen/Schließen des Aufrufs, Datei-
oder ungültigem Zielwechsel entfernen; Fokusverlust pausiert die Anzeige.

Kommata in inneren Aufrufen, Zeichenketten, Kommentaren, Klammern und Indizes
dürfen den äußeren Parameter nicht weiterschalten. Beim inneren Aufruf dessen
Signatur, danach wieder die äußere anzeigen. Bei zu vielen Argumenten ehrlich
„Argument 3 · erwartet 2 Parameter“ melden, keinen falschen letzten Parameter
markieren. Keine unbekannte Signatur, Überladung oder Argumentbeschreibung erfinden.

Vorbild sind [VS Code Signature Help](https://code.visualstudio.com/docs/languages/javascript#_signature-help)
und [Visual Studio 2022 Parameter Info](https://learn.microsoft.com/en-us/visualstudio/ide/using-intellisense?view=vs-2022#parameter-info).
Die Browserreferenz bindet nur echte Funktionssignaturen des eingefrorenen
Katalogs. Nativ tatsächliche sprachabhängige Aufruf-/Signaturdaten verwenden,
einschließlich verfügbarer benutzerdefinierter Funktionen; keine vollständige
C-/Physim-Analyse aus dem begrenzten Browserhelfer ableiten. Bestehende
Dokumentrevisionen, IME-, Fokus-, Escape- und Accessibility-Verträge einhalten.

## Ein Syntaxschema für Editor und Kurzinfo

Die letzte Nutzerpräzisierung ist verbindlich: Codefarben an VS Code und Visual
Studio 2022 orientieren, **Funktionsnamen gelb**. Die konkrete Palette stammt aus
den offiziellen VS-Code-Themes [Dark+](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/dark_plus.json)
und [Light+](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_plus.json)
samt [dunkler](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/dark_vs.json)
und [heller Basis](https://github.com/microsoft/vscode/blob/main/extensions/theme-defaults/themes/light_vs.json),
gelesen am 30. Oktober 2026. Keine Behauptung einer identischen universellen
VS-2022-Palette: dessen Farben hängen von Theme, Sprache und Einstellungen ab.

Nativ gemeinsame Rollen in [design_tokens.h](../../../app/design_tokens.h) und
Syntaxrenderer erweitern: `code_function`, `code_variable`, `code_control`
zusätzlich zu `code_text`, `code_keyword`, `code_type`, `code_string`,
`code_number`, `code_preprocessor`, `code_comment`. Das ist ein verbindlicher
**nativer Folgeauftrag**, keine bereits ausgeführte Änderung dieser Headerdatei.
Editor, Completion-Namen und -Icons, Kandidaten-Kurzinfo und Aufrufsignatur aus
denselben aktiven Rollen zeichnen. Keine unabhängigen Kurzinfo-Farben oder weiße
Kopie der Signatur. Themewechsel gilt gleichzeitig für alle Codekontexte.

| Rolle | Dunkle Werkbank | Helle Werkbank |
| --- | --- | --- |
| Funktion | `#dcdcaa` · sanftes Gelb | `#795e26` · lesbares Goldbraun |
| Typ | `#4ec9b0` | `#1f6b81` |
| Variable/Parameter | `#9cdcfe` | `#001080` |
| Schlüsselwort/Präprozessor | `#569cd6` | `#0000ff` |
| Kontrollfluss (`if`, `return`, …) | `#c586c0` | `#af00db` |
| Zeichenkette | `#ce9178` | `#a31515` |
| Zahl | `#b5cea8` | `#098658` |
| Kommentar | `#6a9955` | `#008000` |
| Übriger Code/Zeichen | `#d4d4d4` | `#000000` |

Der helle Typfarbton ist aus Light+ `#267f99` auf `#1f6b81` abgedunkelt,
damit kleine Typtexte auch auf der aktiven Parameterfläche lesbar bleiben.
Das ist eine eigene Kontrastanpassung für Physims bestehende Flächen.

Die frühere pauschale Regel „alle `ps_`-Namen sind Typen“ ist damit **ersetzt**:
`ps_vec3` ist türkis, `ps_vadd`/`ps_v3` sind gelb. Eigene Funktionen wie `step`
ebenfalls als Funktion zeichnen; `velocity` und Parameter `a`/`b` sind Variablen.
Nativ tatsächliche sprachabhängige Symbolrollen verwenden, nicht den Namenspräfix
als Typbeweis. Der Browserhelfer nutzt echte Katalogrollen plus begrenzte
Aufrufsyntax; er liefert keine vollständige semantische Klassifizierung.

Hoher Kontrast/Systemfarben haben Vorrang; die eigene Kontrastvorschau verwendet
für Funktionen `#ffff00`. In Forced Colors übernimmt der Systemtext. Auswahl-
und Präfixmarkierung dürfen Funktionsnamen nicht wieder blau oder weiß umfärben;
Präfix durch Fettschrift kenntlich machen. A2 bleibt eine
Auswahlfläche hinter den vollfarbigen Glyphen. Die Browserstudie zeichnet
Syntaxglyphen in einem `aria-hidden`-Spiegel über dem nativen Auswahlhintergrund;
Text/Caret/Undo bleiben an der einzigen Textarea-Quelle. Nativ vorhandenen
Editorrenderer und gemeinsam genutzte Syntaxregeln weiterverwenden.

## Kurzinfo-Hintergrund: neutral freigegeben

Mit „Ansonsten gefällt mir das so“ hat der Nutzer die gezeigte neutrale Referenz
einschließlich Parameterhilfe zur Umsetzung freigegeben. Die zuvor offene
Farbfrage ist damit abgeschlossen. Die beiden anderen Töne bleiben Historie:

| Ton | Dunkel | Hell |
| --- | --- | --- |
| **Neutral · freigegeben** | `#23252a` | `#f4f5f7` |
| Etwas heller · Historie | `#2a2c31` | `#fcfcfd` |
| Leicht kühl · Historie | `#222830` | `#eef2f6` |

Standard/`approved=1` erzwingt D und neutral, auch bei alten Vergleichs-URLs;
die Farbwahl ist dort verborgen. Nur `approved=0` lässt historische Töne prüfen.
Signaturfläche verwendet weiterhin die bestehende Feldrolle, Parameterhilfe die
bestehende Headerrolle. Der Auftrag betrifft Codefarben und diese Referenz,
keine Änderung der restlichen Werkbank-/A2-/Statuspalette.

Automatische Anzeige, bewusste Übernahme, Escape-/IME-/Timer-/Zielregeln und
vorhandene API-/F1-Zugänge bleiben wie in [README](README.md) und ADR 022.
[Freigabeprüfung](review/APPROVAL-VALIDATION.md) belegt nur die Browserreferenz;
native Funktionen, echte Preferences, Screenreader und Plattformen separat abnehmen.
Die ergänzte [Parameterhilfe-Prüfung](review/SIGNATURE-VALIDATION.md) hält den
präzisierten Aufrufvertrag und die neuen Referenzbilder separat fest.
Der [abschließende Farbnachweis](review/COLOR-VALIDATION.md) dokumentiert die
neuere Freigabe; ältere eingefrorene Berichte bleiben historisch unverändert.
