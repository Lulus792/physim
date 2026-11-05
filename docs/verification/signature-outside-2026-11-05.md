# Parameterhilfe: Ziehen über den Kartenrand

Stand: 5. November 2026, isolierter Quellstand `signature-outside-checked`.
Die vorhandene Eingabezuordnung im Hauptfenster besteht die erweiterte native
Aufgabe: Scrollgriff drücken, zunächst innerhalb der Spur bewegen, anschließend
bei gedrückter Taste über den Rand der Hilfe in den Quelleditor ziehen und dort
loslassen. Quelle, Cursor, Auswahl und Editorscroll bleiben erhalten. Danach
wird tatsächlich eine `1` getippt und wieder gelöscht; die Parameterhilfe,
Vorschlagsliste und Nullparameter-/Fehlerzustände funktionieren weiter.

Windows Release besteht den direkten Nachlauf derselben zuvor gebauten
Binärdatei nach Aktivierung der beiden Testfenster per Computer Use. Die
ursprüngliche registrierte Windows-Prüfung erhielt keinen echten Fokus und
bleibt als fehlgeschlagener Versuch erhalten. Linux Debug mit ASan/UBSan besteht
den registrierten Ablauf 1/1. Beide Varianten prüfen C und Physim mit den drei
bestehenden Editorzielen; der neue Ziehweg läuft im langen Dateieditorfall.
Acht gezielte aktuelle Bilder wurden geöffnet; Workspace-Häkchen bleiben entfernt.
GUI-Leakerkennung ist wie bisher für den Grafiktreiber ausgeschaltet.

Es war keine neue Änderung am normalen Appverhalten nötig. Die Entwicklung
korrigierte zwei Testannahmen: Eine Zahl am Cursor hat keine Namensvorschläge;
vor dem separaten Strg+Leertaste-Test wird deshalb das leere Argument hergestellt.
Außerdem beendete die generische äußere 15-Sekunden-Frist den Windows-Prozess
vor Ablauf der vorgesehenen 30-Sekunden-Fokuswartezeit. Nur die Signatur-
Selbsttests erhalten deshalb 60 Sekunden Gesamtzeit; echte Fokusprüfung,
interne 30 Sekunden und äußere Prozessfrist von 75 Sekunden bleiben begrenzt.

[Kommandos, Quellstände, Originalversuche und Bilder](signature-outside-2026-11-05.json)
stehen einmal im strukturierten Nachweis. Die Aufnahme des kurzlebigen Windows-
Fensters durch Computer Use endete teilweise bereits nach dessen erfolgreichem
Testende; die Abnahme stützt sich auf die tatsächlichen App-Ausgaben und ihre
eigenen Bilder, nicht auf einen behaupteten Computer-Use-Screenshot.

Dies prüft die Kartenüberschreitung innerhalb desselben Hauptfensters mit
geskripteten SDL-Ereignissen. OS-Fenstergrenze, tatsächlicher Capture-Verlust,
ausgegliederter Workspace, weitere überlappende Ziele und Assistive-Bedienung
bleiben eigene Aufgaben. Der [abgeschlossene Mac-Nachlauf](signature-thumb-macos-2026-11-05.json)
gehört zur früheren Produktkorrektur `05e43dc0`; er enthält diese neuen Tests
und die spätere Workspace-Häkchenentfernung noch nicht.
