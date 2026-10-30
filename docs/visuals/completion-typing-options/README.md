# D09 · Automatische Vervollständigung D mit Ergänzungen

Stand: **30. Oktober 2026 · D mit Ergänzungen gewählt.** Fünf sichtbare Einträge,
schmale Scrollleiste, Variablen in Funktionsargumenten und dieselben Codefarben
wie im Editor. **Nur der genaue Kurzinfo-Hintergrundton bleibt offen.**
[APPROVED.md](APPROVED.md), [ADR 023](../../decisions/023-selected-completion-d.md),
[Auto-Anzeige ADR 022](../../decisions/022-automatic-completion.md) und
[Gesamtübergabe](../unified-workspace/NEXT-CHAT.md) beachten. E/F bleiben Historie;
A/B/C wurden zuvor verworfen. Keine neue Grundlayoutwahl nötig.

[D · Argument-Variablen](index.html?approved=1&scenario=arguments&theme=dark) ·
[D · API-Namen](index.html?approved=1&scenario=normal) ·
[D · Zweiter Parameter](index.html?approved=1&scenario=signature-second) ·
[D · Verschachtelter Aufruf](index.html?approved=1&scenario=signature-nested) ·
[historischer Vergleich E](index.html?approved=0&variant=e) ·
[historischer Vergleich F](index.html?approved=0&variant=f).
Server: `http://127.0.0.1:8791/completion-typing-options/`.
Auch lokal als Datei; keine Netz-/Font-Abhängigkeit und keine Plugininstallation.

## Vorbilder und eigene Ableitungen

Die folgenden Primärquellen wurden am 30. Oktober 2026 gelesen. Die Studie
übernimmt Interaktions-/Layoutmuster und zeichnet eigene UI mit Physim-Farben;
keine kopierten Marken-Assets oder Behauptung identischer IDE-Funktionen.

| Quelle | Gelesenes Muster | Eigene Ableitung |
| --- | --- | --- |
| [VS Code IntelliSense](https://code.visualstudio.com/docs/editing/intellisense) | Symboltypen, automatische Vorschläge, seitliche Kurzinfo, Strg+Leertaste zum Umschalten | Flache Zeilen, Rollen-/Typ-Spalte und separates Dokumentationsfenster. Kein VS-Code-Language-Service übernommen. |
| [VS Code Signature Help](https://code.visualstudio.com/docs/languages/javascript#_signature-help) | Automatisch bei `(`/`,`, aktueller Parameter, Strg+Umschalt+Leertaste | Separate Signatur des bestehenden Aufrufs, bleibt nach Namensübernahme sichtbar. |
| [Visual Studio 2022 Parameter Info](https://learn.microsoft.com/en-us/visualstudio/ide/using-intellisense?view=vs-2022#parameter-info), [C++ Parameter Help](https://learn.microsoft.com/en-us/visualstudio/ide/visual-cpp-intellisense?view=vs-2022#parameter-help) | Erwartete Anzahl, Namen und Typen; aktueller Parameter fett, Parameterinfo getrennt von Liste und Quick Info | Originalsignatur mit aktiver Parameterfläche und Zähler; keine erfundenen Überladungen. Offizielle Dokumentation gelesen, keine lokale VS-2022-Instanz getestet. |
| [blink.cmp · Completion, v1.10.2](https://cmp.saghen.dev/configuration/completion) | Keyword-/Triggerauslöser, konfigurierbares Preselect/Auto-Insert, Spalten, getrennte verzögerte Dokumentation | Dichte monospaced Liste, `[API]` als echte Quelle; bewusste Auswahl statt automatischem Code-Preview. 140/280 ms sind eigene Physim-Entwurfswerte. |
| [nvim-cmp](https://github.com/hrsh7th/nvim-cmp) | Konfigurierbare Quellen, getrennte Dokumentations-/Completion-Fenster, Key-Mappings | Kleine Rahmen, Tastaturzugang und ruhige Zeilen. Keine zusätzlichen Buffer-/LSP-/Snippetquellen vortäuschen. |
| [IntelliJ IDEA · Code completion](https://www.jetbrains.com/help/idea/auto-completing-code.html) | Automatisches Popup, manueller Zugang und ausdrückliche Übernahme | Parameter-/Typangaben in der Zeile. Physims Bezeichnerersetzung bleibt erhalten; kein Import-/Klammer-/Smart-Completion-System übernommen. |

## Neue Gestaltungsrichtungen

| Variante | Darstellung |
| --- | --- |
| D · Editor | 400 breite Liste, etwa 28 hohe Zeilen, Rollenicon links, Name und Typ rechts; separate kompakte Kurzinfo. |
| E · Neovim | 380 breite monospaced Liste, etwa 25 hohe Zeilen, 1er Rahmen und kaum Rundung/Schatten; `[API]` rechts. Schlichtes separates Dokumentationsfenster. |
| F · IDE | 530 breite Liste, etwa 30 hohe Zeilen, Namen/Parameter und Typ in Spalten; schmale Varianten kürzen Zusatzspalten, volle Signatur in Kurzinfo. |

Breiten/Höhen sind logische CSS-Einheiten, durch Editorraum begrenzt. Keine
große Überschrift oder Close-Buttonzeile im Popup. Vorschlagsnamen bleiben
einzeilig; gekürzte Darstellung ändert den echten Namen/Einfügebereich nicht.
Kurzinfo enthält den vollständigen Namen, die originale Signatur und vorhandene
Dokumentation. Platzierung rechts/links, bei wenig Raum ober-/unterhalb; Inhalte
bleiben scrollbar. A2-Auswahlfläche/-Textfarben bleiben erhalten.

## Automatisches Anzeigen ausprobieren

1. Im Code `ps_v` tippen. Ab zwei Bezeichnerzeichen erscheinen nach **140 ms**
   Pause passende Namen. Während weiterer Eingabe schließen/verwerfen alte
   Ergebnisse; der neue Stand wird erneut geprüft. Cursor und Fokus bleiben im Code.
2. Im Physim-Beispiel öffnet `direction.` die verfügbaren Instanzmember, auch ohne
   zwei zusätzliche Blattzeichen. Statische Namen wie `Mat4.trans` bleiben getrennt.
   Die vorhandene lexikalische Empfängerregel ist keine semantische Typinferenz.
3. Automatisch ist **noch kein Übernahmeeintrag ausgewählt**. ↓ wählt den ersten,
   ↑ den letzten; weitere Pfeile oder Ctrl+N/P navigieren. Dann übernehmen
   Enter/Tab. Namensklick übernimmt direkt. Hover zeigt nur eine Vorschau/Kurzinfo
   und aktiviert keine Tastaturübernahme. Der Text wird nicht probeweise ersetzt.
4. Ohne bewusste Auswahl bleibt Enter ein Zeilenumbruch und Tab eine Einrückung.
   Umschalt+Tab verlässt das Codefeld ohne Übernahme; modifizierte Tab-Kombinationen
   werden nicht als Einfügebefehl abgefangen.
   Satzzeichen/Leerzeichen übernehmen keinen Kandidaten. Die ursprüngliche
   Bezeichnerersetzung erhält Unicode und den Empfänger; keine Aufrufklammern.
5. Nach einer Auswahl/Mouse-Vorschau erscheint nach **280 ms** die Zusatzinfo.
   `i` oder Strg+Leertaste im offenen Popup schaltet sofort. Im geschlossenen
   Zustand öffnet Strg+Leertaste manuell mit erster Übernahmeauswahl.
6. Escape schließt und unterdrückt erneutes automatisches Anzeigen für denselben
   laufenden Bezeichner. Neuer Bezeichner/Member oder Strg+Leertaste erlaubt es wieder.
7. Umschalt+F1 bzw. der Referenzbutton bindet denselben Symbolnamen an die
   vorhandene API-Karte. Die Studie zeigt nur eine Demo-Rückmeldung; der Link
   zur bestehenden F1-Designreferenz verwendet deren eigene Beispieldaten.
8. „Tippbeispiel neu starten“ setzt ausdrücklich den Beispieltext zurück und
   spielt echte Eingabeereignisse ab. Danach selbst weitertippen. Der Galerie-
   Schalter deaktiviert Auto-Anzeige, manuelles Öffnen bleibt erreichbar.
9. Die Argumentbeispiele zeigen zusätzlich die erwartete Aufrufsignatur. Mit
   Komma bzw. Cursorwechsel wechselt der aktive Parameter; innere Kommata zählen
   nicht für den äußeren Aufruf. Strg+Umschalt+Leertaste öffnet diese Hilfe manuell.
   Liste schließen/Name übernehmen lässt die Signatur stehen; weiteres Escape
   schließt sie. Die normale Liste bleibt unten. Das zusätzliche Oben-Beispiel
   hat der Nutzer zurückgenommen; kein neuer Oben-Entwurf.

Die genauen Schwellen/Verzögerungen und die initial nicht gewählte Zeile sind
eigene Entwurfsentscheidungen. Die Nutzeranforderung **automatisch anzeigen**
ist bindend; die Optik wurde inzwischen als D mit Ergänzungen gewählt.

## Auslöser und Abbruch

Kein automatisches Popup bei leerem/einstelligem normalen Präfix, Kommentar,
Zeichenkette, Nur lesen, Textauswahl, ungültigem Member oder fehlenden Treffern.
Keine störenden Fehlermeldungen pro Tastendruck. Paste/Drop und Undo/Redo lösen
keine automatische Anzeige aus; manuell kann anschließend nachgefragt werden.
Während IME-Komposition bleibt die Liste geschlossen; erst der fertige Text wird
geprüft. Browser-IME-Ereignisse sind kein Nachweis echter OS-Eingabemethoden.

Timer halten Datei, Revision und Cursor fest. Vorschläge/Referenzaktionen halten
zusätzlich Originaltext und Ersetzungsbereich fest. Bei Cursor-/Auswahl-/Dokument-
oder Versionswechsel, Codescroll und Fensterfokusverlust verwerfen. Ausstehende
Dokumentationsanzeige darf kein inzwischen anderes Symbol zeigen. Ein alter
Einfügeklick ändert weder den früheren noch einen neu aktiven Editor.

140-ms-UI-Verzögerung bedeutet keinen neuen asynchronen Katalog-Provider.
Native Completion bleibt lokal/synchron. „Dokumentation lädt“ ist weiterhin nur
ein gekennzeichneter Zustandsentwurf, kein neuer Worker-Auftrag. Fehlende Doku
sperrt keinen vorhandenen Katalognamen; Originalsignatur bleibt verfügbar.

## Bestand und native Übergabe

Der unveränderte [eingefrorene Katalog](../completion-options/catalog.js) umfasst
1.264 echte lokale Symbole. [Generator](../../../tools/build_completion_preview_data.py)
und `--check` prüfen Namen/Signaturen/Referenzauszüge gegen den Bestand.
Native Bindung: [completion.c](../../../app/completion.c),
[completion_ui.inc](../../../app/completion_ui.inc),
[api_reference_ui.inc](../../../app/api_reference_ui.inc) und
[main.c](../../../app/main.c). Keine neue LSP-/KI-/Projektindex-Engine.

Automatische Öffnung im bestehenden Editor-Eingabefluss ergänzen, nicht durch
ein zweites Text-/Dokumentmodell. Echte Dirty-/Build-/Undo-/Redo-Verträge und
Byte-/Rune-/Glyphkonvertierung erhalten. Debounce und Zusatzinfo auf dem UI-Owner
planen; keine veralteten Zeiger oder nachträglich andere aktive Datei verwenden.
Eine Option in die bestehende Editor-Kategorie von Einstellungen A einpassen;
deren Transaktions-/Übernehmen-Vertrag erhalten. Galerie-Schalter ist kein
persistentes Preferences-Backend.

**D gemäß Ergänzungen nativ umsetzen**, A/B/C oder E/F nicht erneut zur Wahl stellen.
Fünf sichtbare Zeilen begrenzen die Höhe, nicht den Ergebnisbestand. Anzahl
später in Einstellungen A / Editor; Scope-Helfer und gemeinsamer Syntaxrenderer
der Browserstudie illustrieren die Ergänzungen. [Genaue Grenzen und Werte](APPROVED.md).
Hintergrundton neutral/heller/kühl bleibt eine Farbprobe. Native Integration
und Betriebssystem-/Screenreader-/IME-Abnahme bleiben offen.
Suchinsel, API-Karte, F1 H, A2, Kontextmenüs A und alle bisherigen Freigaben erhalten.
Code-/Auswahllogik verwendet nur lokale Browser-Fixtures; kein Speichern,
Kompilieren, Systemclipboard oder Local Storage.

[Prüfbericht](review/VALIDATION.md) · [Maschinenbericht](review/validation.json).
[Parameterhilfe: neuer Nachweis](review/SIGNATURE-VALIDATION.md).
