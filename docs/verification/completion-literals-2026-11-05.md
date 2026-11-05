# Physim-Stringkontext der Vervollständigung — 5. November 2026

Ein offener mehrzeiliger String mit einem enthaltenen Anführungszeichen ließ
fälschlich eine API-Abfrage für `Ve` zu. Der echte Sprachlexer erkannte denselben
Text als offenen String. Ursache war eine unabhängige Umschaltung einzelner
Anführungszeichen im Completion-Kontext.

Die Physim-Abfrage verwendet nun für jeden String ein tatsächliches Sprachlexer-
Token mit begrenztem vollständigem Resttext. Liegt der Cursor im String oder in
einem noch offenen/teilweisen Begrenzer, bleibt die Liste geschlossen. Ein
abgeschlossener String wird vollständig übersprungen; anschließend funktioniert
die Automatik wieder. Mehrzeilige Begrenzer, Escape-Sequenzen und verschachtelte
Interpolationsstrings folgen der Compilergrammatik. Der C-Zweig bleibt erhalten;
keine neue Abhängigkeit, kein Worker und keine Änderung an Optik oder Fristen.

[Originalkommandos, Quell-/Artefakthashes und Phasenergebnisse](completion-literals-2026-11-05.json).
Die reguläre Regression wurde zuerst am alten Code ausgeführt: strenger C17-
Build ohne Warnungen, Exit **1** an Literal-Fall 0 (`PS_OK` statt `PS_INVALID`).
Mit unverändertem Testquelltext nach Korrektur Exit **0**, elf Kontexte einschließlich
vollständiger Strings, innerer Quotes, Interpolation und Teilbegrenzern. ASan/UBSan
mit `detect_leaks=1`. Kein tatsächlich ausgeführter nativer Vorherlauf behauptet.

Windows Release/MSVC und Linux Debug/GCC ASan/UBSan danach jeweils **Unit 5/5 +
GUI 6/6**. Windows `run-npz0r953` / `run-j9qf8uxz`, Linux `run-bu5mih99` /
`run-femnnlmv`; Unit-/GUI-Appbytes je Plattform identisch. Units: Completion,
Locals, Help, Syntax, API. GUI: manuelle Auswahl, normale Automatik, erzwungene
Info-Frist, Kurzinfo, lokale Namen und API-Referenz, jeweils C/Physim soweit der
bestehende Fall das vorsieht. Neue Physim-Eingabeschritte prüfen tatsächliche
automatische Unterdrückung, manuelle Ablehnung, Quellerhaltung und Wiederaufnahme
nach dem geschlossenen String; beide Automatikvarianten liefern den neuen Marker.
Keine zusätzlichen Bilder für diese Schritte. Parser-/Checker-/Controller-Units
ohne relevante Änderung und geschlossene F1-/Protokoll-/Settings-Matrizen nicht wiederholt.

Snapshot **`completion-literals-first`**, 178 eigene / 1.128 Compiler-/Testeingaben;
alle Quell-, 248 Windows-/135 Linux-Laufzeitdateien und referenzierten Archivbytes
gelesen. Lokale Handles beendet. 34 durch unveränderte Renderer erneut erzeugte
Nachbarfall-BMPs entfernt: **108.695.892 Bytes**. Vor dem ersten Löschen alle Ziele
und vorhandenen Ersatzreferenzen geprüft; ursprüngliche Fehlerberichte, Quell-/
Laufzeitarchive und sechs ausgewählte Locals-Ansichten erhalten.
[Bereinigungsbeleg](completion-literals-cleanup-2026-11-05.json).

Die gehosteten ARM-/Intel-Gruppen **8/8** zu `b64cc8ca` sind im
[separaten Mac-Nachlauf](completion-locals-macos-after-2026-11-05.json) geschlossen;
sie enthalten diese anschließende Stringkorrektur nicht. Der Nutzer-Mac blieb aus
und unberührt. Neue Mac-Abnahme, physische Eingabe/IME/AT/DPI/Monitor/Vollbild,
echte C-Scopes, separate Parameterhilfe und übrige freigegebene UI bleiben offen.
