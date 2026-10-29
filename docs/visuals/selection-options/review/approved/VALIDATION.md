# A2-Freigabe und verbindliche Referenz

29. Oktober 2026 · Windows / Headless Microsoft Edge 156.0.4314.50.
Die [Nutzerfreigabe](../../APPROVED.md) ersetzt A durch A2. Die früheren
Vergleichs- und A2-Nachweise bleiben eingefroren; deren damalige offene
Auswahlstatus gelten nicht mehr als aktuelle Anweisung.

Ausgeführt:

```powershell
$env:NODE_PATH='C:/Users/angel/.cache/codex-runtimes/codex-primary-runtime/dependencies/node/node_modules'
node tools/check_selection_options.cjs
node --check docs/visuals/selection-options/selection.js
node --check tools/check_selection_options.cjs
```

Ergebnis: **Exit 0**, sämtliche 64 bisherigen Layouts und die Maus-/Tastatur-/
Kontrastprüfungen bestehen. Zusätzlich geprüft: A2 als Standard; freigegebener
Modus ignoriert `variant=c`, verbirgt die Vergleichskarten und erhält A2 über
alle vier Inhaltsarten in beiden Themes sowie nach Neuladen. Der Rückweg in
die Vergleichshistorie verändert die festgelegte Referenz nicht. Keine
JavaScript-Laufzeitfehler. Exakte Resultate: [validation.json](validation.json).

Die unveränderten A2-Farbrollen und die transparente Textmarkierung wurden an
der [freigegebenen Referenz](approved-a2.png) angesehen. Die Aufnahme zeigt auch
den Tastaturfokus als zusätzliche gestrichelte Linie. Keine neue Farbabwandlung.
[source-hashes.json](source-hashes.json) bindet Quellen, Spezifikation und UI-Liste;
der Bericht nennt den Vorgänger-Commit, weil vor dem Commit geprüft wurde.

Dokumentation: 225 lokale Verweise in zehn betroffenen Markdown-Einstiegsdateien
einschließlich Freigabe, ADR, Gesamtauftrag und UI-Designbacklog geprüft; alle
Ziele vorhanden. Die Freigabe ersetzt die bisherigen aktuellen Wartestatus in
Projektregeln und Übergabe. Die neue Liste trennt 18 offene Detailentwürfe,
acht Fachoberflächen im bereits vereinbarten Gesamtumfang und sechs zusätzliche
Komfortideen von den schon freigegebenen Designs. Sie behauptet keine neuen
nativen Funktions- oder Plattformnachweise und verschiebt keine 1.0-Anforderung.

Native A2-Umsetzung und deren Abnahme bleiben Aufgabe des umsetzenden Chats
gemäß Spezifikation. Dieser Nachweis bestätigt das Designpaket und seine Referenz.
