# Schmale Einfügemarken in Textfeldern

Auftrag vom 29. Oktober 2026: Die weißen Block-Einfügemarken in der App durch
richtige Textcursor ersetzen. Der Nutzer hat ausdrücklich die Einfügemarken
beim Schreiben in Textfeldern gemeint.

## Ursache und gemeinsame Korrektur

Der bisherige gewöhnliche Nuklear-Textfeldpfad beachtet `cursor_size` nur am
Zeilenende. Innerhalb eines Wortes zeichnet er einen ganzen Zeichenblock und
das Zeichen nochmals invertiert darüber. Deshalb reicht die bereits vorhandene
Physim-Einstellung `cursor_size = 1` für Suche, Pfad- und Dialogfelder nicht aus.
In einer bewusst hohen Prüffläche liefert der unveränderte Renderer einen
6 × 110 px großen Block bei 12 px Schrift.

Die lokale Änderung in `third_party/nuklear.h` zeichnet an Anfang, Mitte und
Ende dieselbe schmale Einfügemarke mit der konfigurierten Breite und der
Schrifthöhe. Sie übermalt das Zeichen nicht mehr als invertierten Block. Die
vorhandenen Theme-Farben gelten weiter. Das erfasst gewöhnliche ein-/mehrzeilige
Felder und die darin verwendeten Zahlen-/Property-Editoren über denselben
Zeichenpfad. Die bearbeiteten Bytes, Einfüge-/Überschreiblogik und Auswahl werden
dadurch nicht verändert.

Der Codeeditor besitzt bereits eine eigene schmale, blinkende Einfügemarke;
der umbrochene native Textpfad verwendet ebenfalls bereits `cursor_size`.
Diese Pfade werden in den gemeinsamen Text-/Editorprüfungen berücksichtigt.
Die lokale Bibliotheksanpassung ist in der
[Abhängigkeitsnotiz](../../../third_party/README.md) beschrieben.

## Tatsächlicher Prüfstand

`tests/test_text_cursor.c` prüft tatsächliche Zeichenbefehle und nicht nur den
gesetzten Stylewert. Die Matrix enthält einzeilige, mehrzeilige und
schreibgeschützte Felder, Anfang/Mitte/Ende, Unicode und zwei Größen. Die
18 Einfügemarken müssen genau 1 × 12 beziehungsweise 2 × 24 px groß sein;
eine Textauswahl darf keine zusätzliche Einfügemarke erzeugen. Die Textbytes
müssen erhalten bleiben.

Der alte Renderer wird von dieser Regression verworfen. Der korrigierte
Linux-Gegenlauf besteht ASan/UBSan mit ausdrücklich aktiver Leak-Erkennung.
Ein erster Testentwurf hatte einen nicht zugesagten NUL-Abschluss des dynamischen
Nuklear-Strings angenommen; der endgültige Test prüft Länge und die tatsächlichen
Bytes. Die dabei durch den frühen Testabbruch entstandene Leak-Meldung ist kein
nachgewiesenes Produktleck.

Die abschließenden Windows-Release- und Linux-Debug-Läufe bestehen jeweils
zehn Cursor-/Editorfälle: unter anderem die neue Zeichenprüfung, Auswahl/Kopie,
Readonly, Scrollen, Undo und umbrochene Texte. Der Linux-Lauf nutzt ASan/UBSan
und aktive Leak-Erkennung. Getrennte Ausgabeverzeichnisse erhalten die gerade
geprüfte GUI-Laufzeit; Programme und Symbole wurden archiviert und zurückgelesen.
Die gemeinsamen Explorer-/Projektdialogprüfungen bestehen je sieben Gruppen
auf beiden Plattformen; dort ist die Linux-GUI-Leak-Erkennung deaktiviert.

Kommandos, Quell-/Binärhashes, Archive, Gegenproben und Grenzen stehen im
[kompakten gemeinsamen Nachweis](../../verification/explorer-preview-2026-10-29.json).
Die spezifische F1-Schrift-/DPI-Matrix, alle einzelnen App-Felder mit physischer
Eingabe und macOS wurden für diese Änderung nicht erneut vollständig abgenommen.
Die Korrektur betrifft den gemeinsamen Zeichenpfad; die vollständige
Plattform-/Designabnahme des Projekts bleibt offen.
