# Protokoll: schnelle Klicks und Fokusübergabe

Stand: 26. Oktober 2026. Der native Protokollbereich behält nun Druck- und
Loslassereignisse einschließlich ihrer jeweiligen Position bis zur Verarbeitung
des Frames. Ein vollständiger schneller Klick geht dadurch nicht mehr verloren,
wenn die Maustaste beim Zeichnen schon wieder losgelassen ist. Der Trenner
verwendet den Druckpunkt auch dann, wenn der Zeiger inzwischen außerhalb seiner
schmalen Trefffläche liegt. Dies ist ein Eingabe-Nachlauf zur bestehenden
Gestaltung, keine vollständige C-/H-/Oberflächenabnahme.

Der [maschinenlesbare Nachweis](../../verification/protocol-pointer-2026-10-26.json)
verknüpft Quellstände, genaue Befehle, Binärhashes und unveränderte lokale Logs.
Alle neuen Dateien liegen auf D:. F: wurde nicht angesprochen.

## Belegte Ursache und Änderung

Die alte Gegenprobe zeigt zwei Übergänge bei bereits losgelassener Taste:
`transitions=2 down=0 activated=0`. Der bisherige Drucktest verlangte gleichzeitig
`down=true`. Die Desktop-Diagnose zeigte außerdem einen begonnenen kurzen Zug,
dessen Zeiger beim ersten Rendern bereits außerhalb des Trenners lag.

Nuklear speichert nun pro Maustaste die Druck-/Loslasskanten und deren Positionen.
Die gemeinsamen Physim-Aktionen, Optionen, Schalter und Auswahlköpfe benutzen
diese Druckkante mit dem Druckpunkt. Das Löschen des bisherigen `clicked`-Zählers
verbraucht weiterhin die gesamte Eingabe; ein geschlossener Menüeintrag darf
keinen darunterliegenden Befehl zusätzlich auslösen. Deaktivierte und außerhalb
des Eltern-Clips liegende Ziele bleiben gesperrt.

Fensteraktivierung und Editfelder berücksichtigen denselben Druckpunkt.
Schnelle Cursorplatzierung und Textauswahl stimmen in den ausgeführten Fällen
mit der entsprechenden Eingabe über mehrere Frames überein. Der Protokolltrenner
fordert den Fokus vor dem Zeichnen anderer Fenster an und verarbeitet auch die
letzte Loslassposition. Sein späterer zweiter Fokusauftrag entfällt.

## Tatsächlich ausgeführte Prüfungen

| Umgebung | Prüfung | Ergebnis |
| --- | --- | --- |
| Windows / MSVC Release | Acht native Gruppen: schnelle Klicks, Edit-Clipping, Umbruch, Readonly, Clipboard, Scrollen und Accessibility | 8/8 bestanden. |
| Alpine-Dev / GCC Debug, ASan/UBSan | Dieselben acht Gruppen | 8/8 bestanden. |
| Windows und Linux | Ergänzte schnelle Textauswahl gegen mehrere Frames und schnelles Öffnen eines Auswahlmenüs | Je 1/1 bestanden; gleicher Cursor und gleiche Auswahlgrenzen, unveränderte Textbytes; das Menü bleibt im Folgeframe geöffnet. |
| Windows | Aktionslayout, Originaltext und unabhängige F1-Edit-UIA | 3/3 Gruppen bestanden; sechs Aktions- und fünf Originaltextfälle. |
| Linux | Navigator, Aktionslayout, Originaltext und unabhängige F1-Edit-AT-SPI | 4/4 Gruppen bestanden; acht Navigator-, sechs Aktions- und fünf Originaltextfälle. |
| Linux / nativer Navigator | Suche mit Down+Up in einem SDL-Poll-Durchlauf, danach ebenso schneller Trennerklick und Shift+Up | Suchfokus wird übernommen; anschließend Trennerfokus und Höhe 340 → 380. Kein programmatischer Fokusauftrag ersetzt diesen Klickpfad. |
| Windows / unabhängiger UIA-Client | Allgemeine lesende Fokusabfrage auch im separaten F1-Fenster | 1/1 Gruppe bestanden; Property und `GetFocusedElement` stimmen in drei aufeinanderfolgenden Abfragen überein. |

Die Unit-Gegenprobe vor der Korrektur scheitert an der ersten schnellen
Klickerwartung. Die später ergänzten Cursor-/Auswahlfälle liegen hinter dieser
Erwartung und wurden im alten fehlgeschlagenen Lauf nicht ausgeführt.
Die nachfolgenden reinen Test-/Probeänderungen verändern die GUI-Programme nicht;
deren Hashgleichheit wurde vor der abschließenden Archivierung geprüft.

## Windows-Desktop: Nachweis weiterhin offen

Die früheren Beobachtungen werden präzisiert, ohne ihre eingefrorenen Berichte
umzuschreiben. Die von Computer Use gemeldete Fokuszeile genügte nicht als
Nachweis des tatsächlichen Fokus. Die getrennte native UIA-Abfrage kontrolliert
deshalb sowohl die Ziel-Property als auch das globale `GetFocusedElement`.

Der frühere Computer-Use-Aufruf `Shift_L+Up` lieferte in der nativen Diagnose
SDL-Scancode 96 / Key 1073741920 mit Modifikator 4097. Das ist der Nummernblock-
Pfad, während SDL-Up den Scancode 82 hat. Dieser Aufruf beweist daher keinen
physischen Pfeiltastenschritt. Die Anwendung wurde für diesen Werkzeugbefund
nicht auf eine andere Tastenbelegung umgestellt.

Im abschließenden Desktopversuch lieferte Computer Use bei zwei Beobachtungen
nur ein schwarzes Bild ohne sichtbare Physim-Arbeitsfläche. Ein Aktivierungsaufruf
änderte daran nichts; der letzte native Zustand hatte keinen Host-Tastaturfokus.
Es wurden keine Klicks oder Tasten gesendet. Die eigene interaktive Fixture
wurde anschließend gezielt beendet; ihr Exit ist ein absichtlicher Abbruch,
kein bestandener Test und kein beobachteter Programmabsturz. Desktopbilder
wurden nicht gespeichert. Direkte Windows-Maus-/Tastaturabnahme bleibt offen.

## Zusätzlicher offener F1-Befund

Die unabhängige Windows-Prüfung `accessibility_edit_scroll_windows` scheitert
weiterhin vor `ScrollIntoView`: Die Zielstelle ist verborgen, die Suche wird
aber noch mit `focus=1 active=1` gemeldet. Der Prüfer verlangt für diesen Fall
ein inaktives Feld. Derselbe gezielte Vorbedingungsfehler tritt mit dem
hashgeprüften Programmstand vor der Klickkorrektur auf. Er ist damit kein
Nachweis einer Regression dieses Patches; die genaue Ursache des F1-Fokuszustands
bleibt zu untersuchen. Der Guard und die Scroll-Erwartungen wurden beibehalten.
Der frühere erfolgreiche Nachweis erhält nur seinen damaligen Geltungsbereich.

## Aufbewahrung und weitere Arbeit

Die Zustandsfolge steht als Text/JSON im Nachweis. Die GUI-Prüfprogramme behalten
ihre gezielten Abschlussbilder für Layout-/Originaltextkontrollen; neue
Desktopaufnahmen oder weitere versionierte Vergleichsbilder wurden nicht
angelegt. Quellüberlagerungen sowie Programme, Bibliotheken und Symbole sind
hashgeprüft archiviert. Laufzeitarchive enthalten gegenüber dem jeweiligen
Vorgänger nur geänderte Dateien. Gegenproben und offene Fehler bleiben erhalten.
Der Laufzeitindex ordnet alle 206 aktuellen Programm-, Bibliotheks-, Symbol-
und Metadatendateien ihren hashgeprüften Archiveinträgen zu, ohne sie nochmals
auszupacken. Die zwei nur für den alten F1-Gegenlauf wiederhergestellten
Programmkopien mit 5.647.872 Byte wurden nach erneuter Archiv- und Prozessprüfung
entfernt. Ihre Originalbytes, Quellstände und Fehlerlogs bleiben erhalten;
der Nachweis enthält die Wiederherstellungspositionen und die Bereinigungsquittung.

Andere ältere Komponenten mit eigenen Druck-/Hoverpfaden benötigen ihre
gesonderte Prüfung. Offen bleiben insbesondere der direkte Windows-Fokus,
der genannte F1-Scrollnachweis, tatsächliches Screenreader-Lesen, macOS,
zusätzliche DPI-/Monitorzustände sowie die übrige C-/H-/Gesamtgestaltung aus
[NATIVE-FIDELITY.md](NATIVE-FIDELITY.md). Die 12-KiB-Originaltextabschnitte,
Detailtypografie und Blockabstände werden durch diese Eingabekorrektur nicht
als abgeglichen erklärt.
