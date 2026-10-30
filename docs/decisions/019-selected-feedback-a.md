# Hinweise und Rückmeldungen im Design A · Leiser Hinweis

Status: **vom Nutzer ausdrücklich freigegeben, 30. Oktober 2026**.

Der Nutzer hat A gewählt und die Übernahme ins fertige Design sowie die genaue
Übergabe an den umsetzenden Chat beauftragt. Die vorherige Übernahmesperre für
D05 ist aufgehoben. A im bestehenden nativen Auftrag mit umsetzen, keine
erneute Variantenentscheidung vorschalten. B/C bleiben Vergleichshistorie.

Maßgeblich sind die [verbindliche Spezifikation](../visuals/feedback-options/APPROVED.md),
die [festgelegte Referenz](../visuals/feedback-options/index.html?approved=1&scenario=burst)
und die [Freigabeprüfung](../visuals/feedback-options/review/APPROVAL-VALIDATION.md).
Maximal zwei kurze Erfolgskarten rechts unten, einzeln ausblendbar; ein kleiner
Verlauf am Rand ist über „Rückmeldungen“ und die Statuszeile erreichbar.
Während der Verlauf offen ist, sind die Karten verborgen.

Pfadfehler stehen am Feld, Kopierfehler an der Exportaktion. Ursache, erhaltene
Eingaben, Wiederholen und vorhandene Speicheralternative bleiben sichtbar;
keine zweite vollständige Fehlerkarte. Veraltete Ergebnisse bleiben lokal
gekennzeichnet, bis eine passende echte Neuberechnung erfolgreich ist.
Das Schließen einer Meldung behebt keine Ursache und verändert keine Daten.

Wiederholungen derselben Ursache/Zieloperation werden mit Zähler gruppiert.
Der kurze Verlauf verwendet denselben Ereignisbestand wie Protokoll C, keine
zweite unabhängige Fehlerwahrheit. Ziele und Aktionen müssen an echte
Dokument-/Projekt-/Lauf-/Fenster- und Versionsidentitäten gebunden werden.
Fixture-Gruppierung nur nach Typ reicht dafür nativ nicht aus.

Die sechssekündige Ausblendfunktion bleibt ein optional gezeigtes Verhalten;
in der ausgewählten Referenz ist sie anfangs ausgeschaltet. Fokus/Mouseover
und verborgene Seite pausieren sie; Verlauf und lokale Fehler bleiben erhalten.
Der Demo-Prüfschalter ist keine zusätzliche freigegebene Einstellungsseite.

Bestehende Freigaben Protokoll C, A2, B2, Einstellungen A, Bereichsauswahl und
Grafikexport bleiben gültig. Lokale Rückmeldungen in echten Aufgabenfenstern
nach [ADR 015](015-independent-task-windows.md) zeigen; Hauptfenster bleibt
bedienbar. Die Browserwerkbank ist Referenz, keine WebView- oder neue
Dialoggestaltung. Native Umsetzung und echte Plattformabnahme bleiben offen.
[Gesamtauftrag](../visuals/unified-workspace/NEXT-CHAT.md) und Designliste führen
D05 als entschieden; D06/D07 bleiben eigene offene Entwurfsrunden.
