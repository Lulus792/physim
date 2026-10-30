# ADR 022: API-Vorschläge beim Tippen automatisch anzeigen

Datum: 30. Oktober 2026. Status: **vom Nutzer beauftragt**.

Der Nutzer möchte automatisches Anzeigen während des Tippens. Die bisherigen
Popup-Entwürfe A/B/C gefallen ihm ausdrücklich nicht und sind verworfen.
Neue Entwürfe sollen sich an Editoren/IDEs und Neovim-Plugins orientieren.

## Bindender Umfang

Automatisches Anzeigen im bestehenden nativen Editor standardmäßig aktivieren.
Strg+Leertaste bleibt als manueller Zugang erhalten. Vorhandenen lokalen API-Katalog,
Sprach-/Memberregeln, Dokumentidentitäten, Bezeichnerersetzung, Undo und die
Referenzzugänge weiterverwenden. Automatisches **Anzeigen** ist keine Anweisung,
Namen ohne ausdrückliche Übernahme in den Code einzusetzen.

Die [neuen getrennten Entwürfe D/E/F](../visuals/completion-typing-options/README.md)
sind noch nicht ausgewählt. Sie implementieren das gewünschte automatische
Anzeigen in der Browser-Fixture und zeigen kompakte Zeilen mit separater Kurzinfo.
Der [Gesamtauftrag](../visuals/unified-workspace/NEXT-CHAT.md) trennt diesen
Funktionsauftrag von der weiterhin offenen optischen Entscheidung.

## Getestete Entwurfsdetails

Die Studie verwendet zwei Präfixzeichen, eine 140-ms-Verzögerung nach Eingabe,
Physim-Member nach Punkt auch ohne Blattpräfix und 280 ms für die Zusatzinfo.
Automatisch ist zunächst kein Eintrag zur Übernahme ausgewählt; erst Pfeile/
Ctrl+N/P oder Klick wählen. Enter/Tab behalten sonst Zeilenumbruch/Einrückung.
Escape unterdrückt das Popup für den laufenden Bezeichner, manuelles Öffnen
überstimmt dies. Diese konkreten Werte/Bedienfeinheiten sind nachvollziehbare
Entwurfsvorgaben, keine Behauptung einer bereits erfolgten nativen Implementierung.

Kommentare, Zeichenketten, Nur lesen, ungültige Auswahl und laufende IME-
Komposition nicht mit Vorschlägen überdecken. Kein automatisches Commit beim
Weiterführen von Leerzeichen, Klammern oder anderen Satzzeichen. Veraltete Timer,
Vorschläge und Zusatzinfos bei Cursor-/Dokument-/Versions-/Fensterwechsel verwerfen.

Kein neuer LSP-/KI-Dienst, keine semantische Typanalyse und kein neuer Provider-
Worker beauftragt. Der Katalog bleibt synchron; UI-Anzeigeverzögerung ist kein
Katalog-Ladezustand. Einstellungen gegebenenfalls in die bestehende Editor-
Kategorie von Einstellungen A einpassen, keine neue Einstellungsrichtung.

Alle bisherigen Freigaben einschließlich Suchinsel, API-Karte, F1 H, A2,
Kontextmenüs A und Fortschritt B bleiben bestehen. Browserprüfung ist keine
native Windows-/Linux-/macOS- oder echte Screenreader-/IME-Abnahme.
