# F1: Reihenfolge der unabhängigen UIA-Scrollprüfung

Stand: 26. Oktober 2026. Der im
[Zeiger-Nachlauf](../unified-workspace/PROTOCOL-POINTER-NATIVE.md) noch offene
F1-Scrollnachweis ist mit einer belegten Synchronisierung des Testablaufs
geschlossen. Der Scrolladapter und seine Erwartungen wurden dabei beibehalten.
Das ist ein begrenzter API-/Testnachweis, keine vollständige H-, Screenreader-
oder Windows-Desktopabnahme.

Der [kompakte Bericht](../../verification/f1-uia-scroll-order-2026-10-26.json)
nennt Quellen, Programme, Befehle, rohe Ergebnisse und die Gegenprobe.
Alle Ausgaben bleiben auf D:. F: wurde nicht angesprochen.

## Ursache statt vermuteter Scrollregression

Der unabhängige Client setzt zuerst den Feldwert und fordert danach ausdrücklich
den Feldfokus an. Bereits `SetValue` fordert im vorhandenen Adapter Fokus an.
Die native Testzustandsmaschine sah diesen ersten Fokus und erzeugte Tab,
während der Client noch in seinem anschließenden `SetFocus`-Aufruf steckte.
Der spätere Fokusabschluss aktivierte die Suche erneut.

Die tatsächlich gemessene Reihenfolge, relativ zum ersten Clientaufruf:

| Zeitpunkt | Beobachtung |
| --- | --- |
| 154,485 ms | Client beginnt `SetFocus`. |
| 164,130 ms | Native Testzustandsmaschine erzeugt Tab. |
| 201,107 ms | „Vorheriger Treffer“ ist fokussiert, Suchfeld inaktiv. |
| 387,712 ms | Client beendet `SetFocus`. |
| 399,059 ms | Suche ist erneut aktiv und fokussiert. |

Beide Seiten protokollieren UTC-Nanosekunden desselben Windows-Rechners.
Die Spur ist eine Reihenfolgebeobachtung dieses Laufs, kein Performancebenchmark.
Der Scrollprüfer stoppte korrekt vor `ScrollIntoView`, weil er ein inaktives
Feld voraussetzt. Aus diesem Abbruch folgt kein belegter Fehler des Scrolladapters.

## Korrektur und unveränderte Anforderungen

Der Python-Treiber schreibt nach Rückkehr des vollständigen Edit-/Fokusclients
eine explizite Bestätigung. Erst danach erzeugt die native Testzustandsmaschine
Tab, weiterhin unter Prüfung von Wert, tatsächlichem Suchfokus und Edit-Aktivität.
Es gibt keine zusätzliche Wartezeit, die die Reihenfolge nur zufällig herstellen
soll. Die Zeitdiagnose ist nur bei `PHYSIM_UIA_SCROLL_TRACE` aktiv.
Im bestandenen Lauf beendet der Client seinen Fokusaufruf bei 418,515 ms;
der native Test erzeugt Tab erst bei 428,244 ms.

Die anschließende unabhängige Probe verlangt weiterhin:

- Zielbereich zunächst verborgen, Suchfeld inaktiv und ohne Fokus;
- Zielbereich nach `ScrollIntoView` tatsächlich sichtbar;
- unveränderten vollständigen Wert, Auswahl, Cursor und Fokus nach dem Scrollen;
- dieselben Bedingungen sowohl am Textende mit `Ziel ᵣ` als auch am Anfang
  mit `Anfang θ`.

Der korrigierte Windows-Lauf besteht mit `visible=0 focus=0 active=0` vor und
`visible=1` nach beiden Scrollaufrufen. Der anschließende allgemeine F1-Edit-Test
einschließlich globaler Suche, Unicode-Auswahl, Tab-Ausgang und Fokusidentität
besteht ebenfalls.
Der entsprechende Quellstand wurde unter `Alpine-Dev` mit GCC, ASan/UBSan
gebaut; der unabhängige F1-Edit-AT-SPI-Ablauf besteht mit 1/1 Gruppe.
Dieser Linux-Nachlauf prüft Feldwert, Auswahl und Fokusführung, keinen
Windows-UIA-Scrollaufruf und keine physische Linux-Ausgabe.

## Gezielte Gegenprobe

Der neue Python-Treiber wurde zusätzlich unverändert gegen das gesicherte
Programm ohne diese native Bestätigungssperre ausgeführt. Der alte Stand
ignoriert die neue Bestätigungsdatei und scheitert erneut gezielt an
`visible=0 focus=1 active=1`. Der neue Treiber allein verdeckt den ursprünglichen
Reihenfolgefehler somit nicht. Vorheriges Programm, Quellen und Fehlerlogs
bleiben als unveränderte Gegenbelege erhalten.

## Nachweise und Grenzen

Für diesen Reihenfolge- und API-Nachweis wurden keine Bilder erzeugt.
Die Rohspuren, Zeitfolge und Erwartungen stehen als Text/JSON im Bericht.
Quellüberlagerungen und Laufzeitänderungen werden vor ihrem Ersetzen archiviert
und vollständig nachgelesen. Bereits gesicherte Programme bleiben über ihre
Archive referenziert; wiederhergestellte Gegenprobe-Kopien können nach
Hashvergleich gezielt entfernt werden.
Die zwei wiederhergestellten Gegenprobe-Programmkopien mit 5.650.944 Byte
wurden nach erneuter Archiv- und Prozessprüfung entfernt. Ihre exakten
Archiveinträge und die Quittung sind im Nachweis verknüpft. Die aktuellen
Programme, Symbole, Quellen und Rohlogs bleiben erhalten.

Der direkte Windows-Desktopnachweis aus dem Zeiger-Nachlauf bleibt offen.
Die damaligen schwarzen Computer-Use-Aufnahmen liefern weiterhin keinen
sichtbaren Eingabe-/Fokusnachweis. macOS, tatsächliche Screenreader-Nutzung,
weitere DPI-/Monitorzustände und die vollständige visuelle H-Abnahme sind
hiermit ebenfalls nicht bewiesen. Die eingefrorenen älteren Berichte behalten
ihren damaligen Geltungsbereich; dieser Nachlauf ersetzt nur den bezeichneten
offenen Scrollprüfungsbefund.
