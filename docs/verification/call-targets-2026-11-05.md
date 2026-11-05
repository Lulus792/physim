# Echte Aufrufziele für die spätere Parameterhilfe

Stand: 5. November 2026. **Parser-/Modellgrundlage; noch keine sichtbare
Aufrufsignatur oder Signaturauflösung.** Die vollständige Parameterhilfe bleibt
nach [ADR 023](../decisions/023-selected-completion-d.md) und
[APPROVED.md](../visuals/completion-typing-options/APPROVED.md) beauftragt.
[Kompakter Nachweis und Originalberichte](call-targets-2026-11-05.json).
Quellcommit **`c9b9c00f`** in beiden Repositories;
[Quellabgleich und gehosteter Mac-Anschluss](call-targets-commit-2026-11-05.json).

Beide Sprachparser liefern jetzt neben `(` und Argumentnummer ursprüngliche
Bytebereiche für Aufrufziel, Namen und gegebenenfalls Empfänger. C verwendet
seine tatsächliche Grammatik/Fehlererholung; Physim die echten Primary-/Member-/
Typargument-Knoten während des Parsens. Kein nachgebauter Browser-Lexer und keine
Auswertung einer beliebigen, nachträglich als gültig ausgegebenen Teil-AST.

`foo(inner(...), ...)` wechselt zum inneren Ziel und anschließend mit sämtlichen
Metadaten zurück zum äußeren. Gruppierung und explizite Typargumente bleiben
Quelltextbestandteile. Bei `obj.run(...)` ist Member-Syntax ausdrücklich markiert;
ein fehlender Empfänger darf später nicht zur gleichnamigen globalen Funktion
umgedeutet werden. `factory()(`, `callbacks[0](` oder `(*callback)(` bleiben ohne
geratenen Funktionsnamen. Diese Daten belegen **keine** Deklarations-, Typ- oder
Rückgabewertauflösung. C-Zielbereiche stammen aus tatsächlichen Knoten, Physim-
Bereiche reichen vom Primary-Beginn bis zur Aufrufklammer einschließlich Trivia;
separate Namens-/Empfängerbereiche bleiben maßgeblich für die spätere Auflösung.

Das vorhandene besitzer-/sprach-/quell-/cursorgebundene Modell übernimmt die
C-Daten in die gemeinsame Struktur. Die 887 Frischvergleiche sowie atomaren
Speicherfehlerprüfungen vergleichen die neuen Felder mit. Keine Änderung an
Vorschlagsbestand, sichtbarer Gestaltung oder Parser-/Quellbudgets; kein Worker.

Tatsächlich ausgeführt:

- Windows Release und Linux Debug ASan/UBSan jeweils **Unit 9/9 + GUI 6/6**,
  identische Appbytes innerhalb der jeweiligen Plattformgruppe.
- **36 neue Grammatikfälle** über das echte Modell: 20 C/16 Physim, verschachtelte
  Aufrufe, Gruppierungen, Member, Generics, UTF-8/UCN/`$`, Quellspannen, unbekannte
  indirekte Ziele und Ausschluss von Deklarations-/Literalzuständen.
- Zusätzlich strenger Linux-C17-Build mit `-Wall -Wextra -Werror`, ASan/UBSan und
  `detect_leaks=1`, ohne Warnung/Sanitizerdiagnostik. Native Units behalten ihre
  Leakprüfung; GUI behält die bestehende Font-/Runtime-Konfiguration mit
  `detect_leaks=0`. Keine pauschale GUI-Leakfreiheit behauptet.
- Der tatsächliche vorige C-Sammler, das Modell und der Physim-Parser aus
  `9292b67e`, gegen das erweiterte aktuelle Datenlayout kompiliert, fallen konkret
  für C und Physim mit fehlendem Namen durch. Nur für den alten vierfeldrigen
  Initialisierer wird im Negativbuild die Missing-Field-Warnung ausgeschaltet;
  keine Verhaltenskorrektur. Absichtliche frühe Fehlerexits überspringen Cleanup,
  daher nur dort `detect_leaks=0`, positive Prüfungen weiterhin 1.
- **1.240** erfasste Compiler-/Test-/Besitzereingaben gegen echte Git-Index-Blobs
  gelesen; Vendor bytegenau, sonst nur CRLF/LF normalisiert. Finaler Snapshot
  `call-targets-ci`: 1.221 Compiler-/Testeingaben und 282 eigene Dateien.

Der zweite Snapshot aktualisiert ausschließlich die nachträglich ergänzte,
lokal nicht ausgeführte Mac-Workflowdatei; die bereits geprüften Programme ändern
sich dabei nicht. Der gehostete Workflow sieht nun vier passende Daten-Units vor
den acht GUI-Gruppen vor. **Erst tatsächlich ausgeführte Tests können die neue
Mac-Datengrundlage bestätigen.** Der private Mac bleibt aus und unberührt.
Der alte Cache-Nachlauf `41968549203` zu `9292b67e` ist mit Intel/ARM je 8/8 und
Originalarchiven abgeschlossen und belegt diese folgende Erweiterung nicht.

Alle 34 im notwendigen GUI-Nachlauf entstandenen BMPs sind bytegleich zu
vorhandenen geschützten Referenzen; **108.695.892 Bytes** entfernt, Original-
JSON/Logs/Quellen/Archive und Referenzen erhalten. Auch die zunächst anders
zugeordnete Linux-Aufnahme stimmt mit der älteren Referenz vollständig überein
(beide SHA-256 gleich, null abweichende Pixel). Für diese Datenarbeit entsteht
keine neue Bildserie. F: und geschützte SDK-/VM-/Toolchainbestände unberührt.

Nächster Schritt bleibt die vollständige Parameterhilfe: tatsächliche API- und
Benutzerfunktionsdeklarationen samt Parametern/geschriebenen Typen in beiden
Sprachen erschließen, dann den unabhängigen nativen HUD-Zustand und Renderer
anschließen. Vollständige Signatur, Anzahl, aktive A2-/Fett-/Unterkantenmarkierung,
gemeinsame Syntaxrollen, normale Liste darunter, verschachtelte/zu viele Argumente,
Strg+Umschalt+Leertaste, unabhängiges Übernehmen/Escape sowie Fokus/IME/Revision/
Accessibility bleiben verbindlich. Große Editierlatenz und übrige freigegebene
UI-/physische Abnahme bleiben ebenfalls offen.
