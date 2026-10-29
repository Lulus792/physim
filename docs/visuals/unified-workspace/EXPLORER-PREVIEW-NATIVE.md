# Explorer A: Vorschau, Orte und Fokus

Stand: 29. Oktober 2026. Ergänzt die [eingebettete Ansicht](EXPLORER-INLINE-NATIVE.md)
und gleicht die belegten Abweichungen der [freigegebenen A-Referenz](../design-options/APPROVED.md)
weiter an. Der [gemeinsame Nachweis](../../verification/explorer-preview-2026-10-29.json)
enthält Kommandos, Quell-/Laufzeitarchive, tatsächliche Ergebnisse und Gegenproben.

## Vorschau und Orte

Die Vorschau verwendet die tatsächlich wirksame CSS-Schrift `.file-preview pre`:
10,5 px bei normaler Größe, mit 1,85-facher Zeilenhöhe. Kurze Texte belegen ihre
natürliche Zeilenhöhe; lange Texte bleiben auf 240 px begrenzt und scrollen
vertikal. Umbruch, Zeichnung, Trefferprüfung, Auswahl und native Zeichenbereiche
verwenden die gemeinsame Abbildung auf unveränderte Originalbytes. Weicher
Umbruch fügt keine Zeilenenden in den kopierten oder veröffentlichten Text ein.
Die zwei möglichen Breiten mit/ohne äußeren Scrollbalken werden getrennt
zwischengespeichert; neue Ergebnisse und Kontextfreigabe entsorgen beide Karten.

Die reale kurze Prüfvorschau umfasst 53 UTF-8-Byte und erhält bei normaler Größe
58,275 px statt pauschal 240 px Höhe. Im geprüften dunklen Windows-Bild ist die
Öffnen-Aktion dadurch mit demselben langen Ort unmittelbar sichtbar. Umfangreiche
Metadaten dürfen die feste Vorschau weiterhin scrollbar machen.

Orte-Beschriftungen brechen innerhalb der Buttonbreite um und vergrößern ihre
Zeile. Der folgende Button folgt mit 4 px Abstand. Die tatsächlichen gezeichneten
Bytes und Glyphenrechtecke werden auf Vollständigkeit und Einhaltung der
Buttonflächen geprüft. Das große Linux-Bild zeigt „Geöffnetes Projekt“ vollständig
in zwei Zeilen. Dies ist keine vollständige Abnahme beliebiger 255-Byte-Ortsnamen
in allen DPI-/Textzuständen.

## Zwei tatsächliche Fokusfehler

Der neue Ablauf erreicht bei großer Schrift zunächst eine Dateiliste von
605 px Höhe in einem nur 594 px hohen Arbeitsbereich. Die alte Nachführung
versucht, den gesamten Listencontainer sichtbar zu machen, und pendelt zwischen
mehreren Scrollwerten. Die unveränderte Debug-Laufzeit liefert sechs konkrete
Messungen; der ursprüngliche Test läuft in den Timeout. Die Korrektur verfolgt
die ausgewählte Zeile und begrenzt übergroße Ziele auf die sichtbare Höhe.
Die Testbedingung für den abgeschlossenen Fokuswechsel bleibt bestehen.

Danach zeigt sich ein eigener Fehler beim Einblenden der Vorschau: Die Gruppe
liegt außerhalb der äußeren Ansicht und wird noch nicht gezeichnet, aber ein
altes Zielrechteck liegt scheinbar sichtbar darin. Die Diagnose misst
`preview_y=895,472`, `text_h=0`, `edit_active=0` und ein altes `control_y=339,69`
bei bereits beendetem Reveal. Der native Kopierablauf erhält deshalb keinen
Vorschautext. Die Zielrechtecke werden jetzt vor dem Zeichnen aus dem aktuellen
Inhalt, den Scrollwerten und der tatsächlichen verfügbaren Breite berechnet.
Auch eine ausgeclippte Gruppe kann dadurch zuverlässig erreicht werden.

Die Debuggerläufe werden nach den Messungen ausdrücklich beendet; sie sind
keine bestandenen GUI-Tests. Die folgenden regulären Abläufe bestehen mit
unveränderten Auswahl-/Kopiererwartungen auf Windows und Linux.

## Geprüfter Umfang und Grenzen

Beide abschließenden App-Läufe bestehen je sieben Gruppen: bestehender Picker,
freie Ansicht, Projektdialog B, unabhängige native Clients für beide Kontexte,
Vorschauablauf und unabhängiger Vorschauclient. Die vorhandenen sechs Dialog-
und sieben eingebetteten Zustände bleiben enthalten. Der Vorschauablauf prüft
normal/groß mit Größenwechsel, exakter Unicode-Kopie, Auswahl und abgewiesener
Texteingabe. Die reale lange Datei enthält 2731 UTF-8-Byte, Tabs, ein kombinierendes
Zeichen, Emoji und lange ungetrennte Textabschnitte.

Unabhängige UIA-/AT-SPI-Clients prüfen vollständigen Readonly-Text, unveränderte
Ablehnung eines Schreibversuchs, Auswahl der Originalbytes und Scrollen zum
letzten Zeichen. Der Windows-Helfer verlangt dabei ausdrücklich ein inaktives
Textfeld. Ein anfänglicher Versuch nach Aktivierung des Feldes wurde vor dem
Scrollaufruf abgewiesen; die korrigierte Fixture deaktiviert es durch einen
injizierten Mausklick. Der Helfer und seine Erwartungen bleiben erhalten.
Native Schnittstellen und injizierte SDL-Ereignisse sind kein Nachweis physischer
Tastaturbedienung oder Screenreader-Sprachausgabe.

Die [schmalen Einfügemarken](TEXT-CARET-NATIVE.md) werden im selben Stand ergänzt.
Zehn separate Editor-/Cursorfälle je Plattform bestehen; Linux nutzt dafür
ASan/UBSan mit Leak-Erkennung. Die Linux-GUI-Läufe verwenden ASan/UBSan ohne
Leak-Erkennung. Ein neuer 64-KiB-GUI-/DPI-/200-%-Text-/macOS-Nachweis wird hier
nicht behauptet. Bild-/Messlaufvorschau, API-Kontext, große native Collections
und die weiteren Aufgaben des [Explorer-Gesamtnachweises](EXPLORER-A-NATIVE.md)
bleiben offen; die vollständige Oberflächengestaltung ist noch nicht abgenommen.
