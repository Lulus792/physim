# Zusammenhängende Dateiendungen im Workspace

Bei großer Schrift zerlegte der allgemeine Textumbruch Dateinamen wie
`completion-note.phys` mitten in der Erweiterung. Der Dateibaum verwendet für
mehrzeilige Dateien jetzt den vorhandenen Unicode-Zeilenplan mit einer
Dateinamensregel: Ein passender letzter Namensteil samt Erweiterung bleibt
zusammen. Im Beispiel entsteht `completion-` / `note.phys`. Ist dieser Teil zu
breit, folgen als Rückfälle der letzte Basisgraphem mit Erweiterung, die
Erweiterung allein und schließlich der normale graphemsichere Umbruch.

Alle Zeichen und Original-Bytebereiche bleiben erhalten. Höhe und Zeichnung
verwenden denselben temporären Zeilenplan. Einzeilige Dateien, Ordner, allgemeine
Texte, Symbole, Farben, Baumreihenfolge und die entfernte Checkboxspalte erhalten
ihren bestehenden Vertrag. Eine Erweiterung, die selbst breiter als die
Textspalte ist, muss weiterhin umbrechen; vollständiger Inhalt bleibt das Ziel.

Die vorhandenen Umbruchtests prüfen die unveränderten anderen Modi und zusätzlich
Dateiendungen, Bindestrich-/Leerzeichen-Namen, Dotfiles, überlange Endungen,
kombinierte Grapheme, exakte Spannen und Fehlerbeibehaltung. Der native Projektbaum
prüft bei22px unabhängig die wirklichen Zeichenbefehle: vollständiger Name,
zusammenhängendes `note.phys`, Glyphen innerhalb des Clips und anschließendes
Öffnen genau dieser Datei. Derselbe reale Font und Platz reproduzieren mit der
alten allgemeinen Umbruchroutine eine geteilte Erweiterung. Je Plattform genügt
eine gezielte neue Dateibaumaufnahme.

Die zusätzlich ausgeführte Werkbankgruppe enthielt eine veraltete Fixture:
Sie erwartete152px Protokollhöhe ohne diese Vorgabe zu setzen; der gemeinsame
Standard war schon vorher340px. Die Fixture setzt ihre Referenzmaße nun explizit.
Ihre alten Protokoll-Tastaturwerte beziehen sich jetzt auf den bereits vorhandenen
C-Vertrag. Die normale Protokollimplementierung wurde dabei nicht geändert.

Der Eingabeabgleich deckte außerdem zwei nicht übernommene vorhandene Testdateien
auf. Sie sind ausdrücklich im303-Dateien-Manifest enthalten. Vor dem gültigen
Abschluss wurden alle neun eigenen Quell-/Test-/Workflowpfade bytegleich gegen
Arbeitsbaum, Testkopie und Manifest gelesen. Die ursprünglichen Versuche bleiben
getrennt erhalten und werden nicht als Prüfung dieser beiden Änderungen ausgegeben.

Kommandos, Ergebnisse, Originalversuche und Quell-/Programmbindung stehen im
[strukturierten Nachweis](filename-wrap-2026-11-05.json). Die vollständige
Oberflächen-, physische und Accessibility-Abnahme bleibt ein eigener Auftrag.
