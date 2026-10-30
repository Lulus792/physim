# Fortschritt B · Aufgabenleiste mit Pop-out

Status: **vom Nutzer ausgewählt, 30. Oktober 2026**.

Der Nutzer hat B mit der Ergänzung gewählt, die Aufgabenanzeige als eigenes
Fenster herauszulösen. Damit ist D06 gestalterisch entschieden. Die vorherige
Übernahmesperre ist aufgehoben; B mit Pop-out im bestehenden nativen Auftrag
mit umsetzen, keine erneute Variantenwahl vorschalten. A/C bleiben Historie.

Maßgeblich: [Spezifikation](../visuals/progress-options/APPROVED.md),
[festgelegte Referenz](../visuals/progress-options/index.html?approved=1&task=series),
[Prüfung](../visuals/progress-options/review/APPROVAL-VALIDATION.md).
Standard ist die kompakte Aufgabenleiste B, Details lassen sich aufklappen.
Ein Icon-only-Button mit Tooltip „In eigenem Fenster öffnen“ lagert die
Anzeige derselben Aufgabe aus. Die kompakte Icon-Fassung hat der Nutzer
anschließend ausdrücklich gewünscht. Auch erneutes Anzeigen erfolgt per Icon. Hauptfenster
bleibt bedienbar, Leiste zeigt weiterhin Zustand und Fenster-/Rückholzugang.

Kein neuer Auftrag oder Versuch beim Auslagern. Zähler, Phasen, Details,
Abbruchanforderung, Resultate und Fehler gehören zum gemeinsamen Aufgabenmodell.
Erneuter Aufruf aktiviert das vorhandene Fenster. „Zur Leiste“, Fenster-Schließen
und Escape im Pop-out holen die Anzeige zurück; sie brechen die Aufgabe nicht ab.
Abbrechen ist eine gesonderte Aktion mit Bestätigungszustand.

Die Browserreferenz verwendet ein tatsächliches `window.open`-Pop-up mit
gemeinsamem Beispielzustand. Browser/App können es als Fenster oder Tab
darstellen oder blockieren. Das ist kein natives Physim-Fenster und kein Beleg
für native Worker-/Fensterfunktion. Nativ ein echtes eigenständiges OS-Fenster
nach [ADR 015](015-independent-task-windows.md) mit zentralem Aufgabenzustand
umsetzen; die bisherigen Projekt-/Explorer-/Exportfenster bleiben erhalten.

Bekannte Gesamtzahl versus unbekannter Umfang, ehrliche Abbruchfähigkeit,
Abbruch angefordert/bestätigt, Teilresultate, Fehler, Erfolg-Abbruch-Rennen und
nicht unterbrechbarer Exportabschluss bleiben Teil des gewählten Ablaufs.
Ereignisse an Rückmeldungen A/Protokoll C anbinden; keine doppelten Aufträge
oder Erfolgsmeldungen aus beiden Fenstern. Alle bisherigen Freigaben erhalten.

[Gesamtauftrag](../visuals/unified-workspace/NEXT-CHAT.md) und Designliste führen
D06 als entschieden; D07 bleibt die nächste neue Entwurfsrunde.
