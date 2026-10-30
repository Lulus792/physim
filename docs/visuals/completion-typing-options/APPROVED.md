# Automatische Vervollständigung · D ist gewählt

**Nutzerwahl vom 30. Oktober 2026:** Design D mit fünf sichtbaren Zeilen,
schmaler Scrollleiste, Variablen in Funktionsargumenten und gemeinsamen
Syntaxfarben. **Nur der genaue Kurzinfo-Hintergrundton bleibt offen.**
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
    double speed = ps_vlength(ve);
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

## Ein Syntaxschema für Editor und Kurzinfo

Aktive native Rollen aus [design_tokens.h](../../../app/design_tokens.h),
`code_text`, `code_keyword`, `code_type`, `code_string`, `code_number`,
`code_preprocessor`, `code_comment`, in **beiden** Kontexten verwenden.
Keine unabhängigen hartcodierten Kurzinfo-Farben oder weiße Kopie der Signatur.
Themewechsel gilt gleichzeitig für Editor und Signatur/Deklaration.

| Rolle | Dunkle Werkbank | Helle Werkbank |
| --- | --- | --- |
| Schlüsselwort/Präprozessor | `#c9a6e8` | `#7844a0` |
| Typ | `#8abaff` | `#17569e` |
| Zeichenkette | `#b8cd8e` | `#44651e` |
| Zahl | `#e8ba91` | `#8b4a19` |
| Kommentar | `#969da8` | `#626b78` |
| Übriger Code | gemeinsame Textrolle | gemeinsame Textrolle |

Diese Werte bilden den gelesenen Werkbankstand ab; nativ die Rollen des aktiven
Themes verwenden. Hoher Kontrast/Systemfarben haben Vorrang. A2 bleibt eine
Auswahlfläche hinter den vollfarbigen Glyphen. Die Browserstudie zeichnet
Syntaxglyphen in einem `aria-hidden`-Spiegel über dem nativen Auswahlhintergrund;
Text/Caret/Undo bleiben an der einzigen Textarea-Quelle. Nativ vorhandenen
Editorrenderer und gemeinsam genutzte Syntaxregeln weiterverwenden.

## Kurzinfo-Hintergrund: Feinabstimmung noch offen

Es sind **keine drei neuen Layoutvarianten**. Nur die kleine Fläche der Kurzinfo
lässt sich im selben D vergleichen:

| Ton | Dunkel | Hell |
| --- | --- | --- |
| Neutral, vorläufiger Start | `#23252a` | `#f4f5f7` |
| Etwas heller | `#2a2c31` | `#fcfcfd` |
| Leicht kühl | `#222830` | `#eef2f6` |

Auswahl im Galerie-Feld „Kurzinfo-Ton“. Kein Ton als endgültige Nutzerfreigabe
behandeln. Syntaxrollen, Text, Verhalten, gewähltes Layout und fünf Zeilen sind
von dieser offenen Farbfrage unabhängig. Kein neues globales Theme ableiten.

Automatische Anzeige, bewusste Übernahme, Escape-/IME-/Timer-/Zielregeln und
vorhandene API-/F1-Zugänge bleiben wie in [README](README.md) und ADR 022.
[Freigabeprüfung](review/APPROVAL-VALIDATION.md) belegt nur die Browserreferenz;
native Funktionen, echte Preferences, Screenreader und Plattformen separat abnehmen.
