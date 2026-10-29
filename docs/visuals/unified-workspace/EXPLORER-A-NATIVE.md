# Explorer A: native Umsetzung

Stand vom 29. Oktober 2026. Maßgeblich bleiben
[ADR 011](../../decisions/011-selected-workspace-additions.md) und die
[freigegebene Auswahl](../design-options/APPROVED.md). Explorer A ist für freie
Datei-/Ordnernavigation, Messlaufauswahl und den Unteraufruf aus Projektdialog B
ausgewählt. Dateisystemmodell, asynchroner IO-Arbeiter und native A-Oberfläche
sind jetzt eingebunden. Die produktiven Öffnen-/Ordner-/Importaufrufe verwenden
die gemeinsame Oberfläche. Die bisherige SDL-Systemdialoganbindung bleibt für
ihre ausdrücklich benannten alten Adaptertests erhalten. Dies ist weiterhin
ein Teilstand der freigegebenen Gestaltung.

## Geprüfte Grundlage

`app/file_browser.h/.c` hält eine eigene, flache Ordner-Momentaufnahme. Die
bestehende Workspace-Baumansicht erhält ihre bisherige Struktur. Einträge
besitzen ihre UTF-8-Pfade, Typ, Größe, Änderungszeit, Hidden-Zustand und etwaige
Lesefehler. Such- und Modusfilter unterscheiden Dateien, Ordner und Messläufe.
Die Inhalts-/Formatprüfung einer `.psrun` bleibt beim vorhandenen Importer.

Neue Ordner werden einzeln und exklusiv angelegt. Kollisionen mit vorhandenen
Dateien/Ordnern ändern weder diese Objekte noch den Rückgabepfad. Fehlgeschlagene
oder abgebrochene Auflistungen erhalten den vorherigen Snapshot. Früher Stopp
meldet eine begrenzte Liste und nur die bekannte Mindestzahl ausgelassener
Einträge. Die UI darf diese Momentaufnahme bei einem Fehler nicht als Inhalt
eines erfolgreich geöffneten anderen Ortes darstellen.

Der [kompakte Modellnachweis](../../verification/explorer-a-model-2026-10-28.json)
enthält tatsächliche Kommandos, Quell-/Binärhashes, Rohberichte, Gegenproben,
Archivverweise und Grenzen. Die abschließenden Tests bestehen auf Windows und
Linux, dort mit ASan/UBSan und Leak-Prüfung. Geprüft sind unter anderem:

- Unicode-Namen, reale Größen/Zeiten, Casefold-Suche einschließlich `straße`/`STRASSE`;
- versteckte Einträge, zusätzlich das echte Windows-Hidden-Attribut;
- Listenwachstum auf 200 Einträge, deterministische Sortierung und Begrenzung auf 127;
- fehlende Pfade, Datei statt Ordner, verschwundene Auswahl und kooperativer Abbruch;
- echte Verzeichnispfade mit 605 beziehungsweise 606 UTF-8-Bytes;
- exklusives Anlegen, vorhandene Ziele, ungültige Namen und Rückgabepfaderhalt;
- tatsächliche POSIX-Symlink-/`..`-Auflösung beim Lesen und Anlegen;
- eine reale Windows-Junction, deren Ziel nachträglich gelöscht wird.

Die Junction-Gegenprobe scheitert mit dem unveränderten vorherigen Modell:
Linkattribute bleiben lesbar und melden weiter einen Ordner. Die korrigierte
Bestätigung prüft das tatsächliche Ziel über einen Metadaten-Handle und weist
den verschwundenen Zielordner ab. Dafür wurde innerhalb des eigenen Testordners
eine Junction angelegt. Developer Mode und Windows-Sicherheitsrichtlinien
wurden nicht verändert; direkte Datei-Symlinks sind auf diesem Rechner mangels
Privileg nicht als geprüft ausgewiesen.

Eine einzelne 200-Einträge-Auflistung dauerte in den dokumentierten Läufen etwa
4,2 ms auf Windows und 109,6 ms unter WSL auf dem eingebundenen D:-Datenträger.
Das sind Beobachtungen dieser Läufe, kein allgemeines Latenzversprechen. Sie
begründen zusätzlich, Dateisystemarbeit außerhalb der Zeichnung auszuführen.
Für diesen Schritt wurden **keine Bilder** erzeugt oder übernommen.

## Geprüfter Hintergrundbetrieb und Textvorschau

`app/file_browser_worker.h/.c` übernimmt Ordnerlesen, Textvorschau, erneute
Auswahlprüfung und exklusive Ordneranlage außerhalb der Zeichnung. Genau ein
Auftrag läuft, daneben bleibt nur die jüngste wartende Anfrage erhalten.
Pfad-/Namenskopien und Ergebnisse haben eindeutigen Besitz; der Arbeiter erhält
keinen App-, UI- oder GL-Zeiger. Ein atomar als beendet gemeldeter Auftrag wird
vor der Übernahme verbunden. Nur seine weiterhin aktuelle Anfrageversion darf
ein Ergebnis liefern. Abbruch entwertet auch fertig berechnete Ergebnisse,
ohne auf laufende Dateisystemarbeit zu warten. Endgültiges Zerstören verbindet
den Arbeiter vor der Speicherfreigabe. Ein bereits angelegter Ordner bleibt
beim späteren Abbruch erhalten; dieser verspricht keine Rücknahme des mkdir.

Die Textvorschau öffnet eine reguläre Datei schreibgeschützt über den nativen
Handle und liest maximal 64 KiB UTF-8-Inhalt plus wenige Prüfbytes. Ein optionales
UTF-8-BOM wird entfernt. Ein angeschnittenes letztes Zeichen wird ganz ausgelassen;
fehlerhafte Codierung und binäre Steuerzeichen werden abgewiesen. Abschneidung
muss später in der Oberfläche erkennbar sein. Die Größe stammt aus dem geöffneten
Handle; gleichzeitiges externes Schreiben verhindert eine Zusage atomarer
Dateimomentaufnahmen. Es gibt keine Ersatzzeichen oder erfundenen Vorschauinhalte.

Der [Arbeiternachweis](../../verification/explorer-worker-2026-10-28.json) belegt
je zwei bestandene Windows-/Linux-Gruppen, unter Linux mit ASan/UBSan und
Leak-Prüfung. Zusätzliche Test-Haltepunkte halten tatsächliche Arbeiter mit
Semaphoren nach dem Lesen beziehungsweise während des Textlesens an. Geprüft sind:

- Alte fertige Ergebnisse dürfen neuere Orte nicht ersetzen; Zwischenanfragen
  werden zusammengefasst. Die kompilierte Gegenvariante ohne Versionsvergleich
  scheitert an diesem Ablauf.
- Überschriebene Aufruferpuffer ändern keine Aufträge; übertragene Listen bleiben
  bei anderen Aufträgen erhalten. Abbruch während des Lesens, erneute Anfrage,
  Schließen und Neuöffnen geben keine alten Ergebnisse zurück.
- BOM, leere Datei, exakte Grenze, alle inneren Schnittpositionen von Zwei-/Drei-/
  Vierbytezeichen, ungültige Sequenzen und binäre Steuerzeichen.
- Verschwundene Dateien, exklusive Anlage/Kollision, Erhalt einer gültigen
  wartenden Anfrage nach abgewiesener Eingabe und Abbruch nach ausgeführtem mkdir.
- Reale Textdateien mit 591/593 UTF-8-Pfadbytes auf Windows/Linux sowie tatsächliche
  POSIX-Link-/`..`-Ziele auch beim Öffnen der Vorschau.

Die erste Linux-Prüfung scheiterte an einer ungeeigneten Testvoraussetzung:
DrvFS auf D: unterstützt keine benannten FIFOs (`errno=95`, gesondert geprüft).
Der endgültige Test weist einen echten anonymen Pipe-Handle über `/proc/self/fd`
ohne wartendes Öffnen ab. Dafür entstehen keine temporären Dateien außerhalb
von D:. Eine benannte FIFO auf einem anderen Dateisystem und der macOS-Zweig
sind damit nicht geprüft. Auch allgemeine Racefreiheit oder ThreadSanitizer
werden nicht behauptet. Es wurden keine Bilder benötigt.

## Native Einbindung

`app/file_browser_ui.inc` besitzt Verlauf, aktuelle Auswahl, Such-/Sortierzustand
und die übertragenen Listen/Vorschauen. Orte, Breadcrumbs beziehungsweise
editierbarer Pfad, Dateiliste/Raster, feste Vorschau und Fußaktionen verwenden
die Maße, Schriftrollen und Symbolpfade der ausgewählten A-Referenz. Bei schmalen
Fenstern liegt die Vorschau unter der Liste; große Schrift erhält einen äußeren
Scrollweg. Ein fehlgeschlagener Ortswechsel zeigt den fehlgeschlagenen Pfad und
einen Fehler, keine alte Liste unter einem neuen Ortsnamen.

Auswahl allein öffnet weder Dokument noch Workspace. Erst die bestätigte,
erneut geprüfte Auswahl wird über das bestehende Workspace-Ereignis weitergegeben.
Die Elternordnerwahl in Projektdialog B kehrt in dessen erhaltene Eingaben zurück.
„Datei → Dateien …“ bietet die freie Navigation. Messlaufimport, Serienfortsetzung
und die übrigen Ordneraufrufe sind an dieselbe Oberfläche angeschlossen; die
vollständigen bereichsübergreifenden Import-/Serienaufgaben benötigen eigene
abschließende Abnahme.

Vier eigene Editierzustände erhalten Cursor, Auswahl und Undo getrennt für
Pfad, Suche, neuen Ordnernamen und schreibgeschützte Vorschau. Ein gemeinsamer
temporärer Nuklear-Zustand hatte beim Wechsel von ausgewähltem Vorschautext in
die leere Suche eine Bereichsassertion ausgelöst. Die eigene Zustandsführung
beseitigt diesen konkret reproduzierten Fehler. Pfadeingaben bleiben während
eines laufenden Ortswechsels erhalten und werden erst mit Enter geöffnet.

Native Windows-/Linux-Fixtures prüfen sechs Zustände je Oberfläche: dunkel,
hell, hoher Kontrast, große Schrift, schmales Fenster und große Schrift im
schmalen Fenster. Sie bedienen tatsächliche Widgets, kopieren echten
Vorschautext über die Betriebssystem-Zwischenablage, suchen, wechseln Liste/Raster,
legen exklusiv einen Ordner an, behandeln fehlende Pfade, kehren zurück, brechen
ab, wählen den Elternordner in B und öffnen eine echte Datei. Die SDL-Eingaben
dieser Fixtures sind injiziert und kein physischer Tastaturnachweis. Unter
Windows wird ausschließlich die dokumentierte CRLF-Umwandlung von CF_UNICODETEXT
akzeptiert; alle anderen UTF-8-Bytes müssen übereinstimmen.

Die vergrößerten nativen Editierpuffer veröffentlichen auch Pfadfelder mit
4096 Byte Kapazität. Unabhängige UIA-/AT-SPI-Clients lesen und ersetzen echte
4095-Byte-Werte einschließlich Unicode und prüfen die unveränderte Rückgabe
nach Ablehnung von 4096 Byte. AT-SPI prüft zusätzlich Einfügen und Löschen in
3000-Byte-Texten. Die neue Kernregression weist den unveränderten alten
Produktionscode wegen des fehlenden 4096-Byte-Feldes ab; die korrigierte Kontrolle
besteht ASan/UBSan und Leak-Prüfung. Diese Clientprüfungen belegen keine
Screenreader-Sprachausgabe und keinen auf dem Dateisystem existierenden
4095-Byte-Pfad.

Der [Einbindungsnachweis](../../verification/explorer-ui-2026-10-29.json) führt
die Quell-/Laufzeitsicherungen, 15 unverändert gültige Kerntests je Plattform,
die native Oberfläche und ihre unabhängigen Clients zusammen. Die abschließende
Linux-Runde besteht 5/5 Gruppen. Windows besteht 4/5; ein erster UIA-Wertabruf
hat einen Timeout. Wiederholungen mit unverändertem Programm einschließlich
demselben Runner und dessen Umgebung bestehen, die ursprüngliche Ursache bleibt
offen. Es wird deshalb kein vollständig grüner Windows-Gesamtlauf behauptet.
Der Katalogtest prüft zusätzlich die Plattformzuordnung einschließlich des
Darwin-Modells; das ist kein auf macOS ausgeführter Build.

## Verbleibende Umsetzung und Abnahme

Messlauf-/Bildvorschau und passender C-API-Kontext fehlen noch; die Oberfläche
zeigt dafür keinen erfundenen Demo-Inhalt. Die freie Dateinavigation ist inzwischen
[in den Arbeitsbereich eingebettet](EXPLORER-INLINE-NATIVE.md). Ordner- und
Messlaufauswahl bleiben eigene Dialoge derselben A-Komponente. Beide Kontexte
behalten unabhängige Arbeiter, Navigation und Editierzustände; ein Unteraufruf
aus B ersetzt die freie Auswahl und Vorschau nicht.

Der folgende [Vorschau- und Fokusabgleich](EXPLORER-PREVIEW-NATIVE.md) übernimmt
natürliche Texthöhe bis 240 px, weichen Umbruch und die wirksame Vorschauschrift
von 10,5 px. Zeichnung, Auswahl und native Zeichenbereiche nutzen dieselbe
Abbildung auf die Originalbytes. Lange Orte-Buttons brechen vollständig um.
Die neue Fokusnachführung erreicht die ausgewählte Zeile und aktualisiert
Vorschauziele auch außerhalb des sichtbaren Ausschnitts. Unabhängige Clients
prüfen Originaltext, Readonly, Auswahl und Scrollen; beide Plattformen bestehen
den abschließenden gemeinsamen Lauf mit je sieben Gruppen.

Das Listenmodell hält bis zu 8192 Einträge; gezeichnet und als native Zeilen
veröffentlicht wird der sichtbare Ausschnitt. Durchgängige Tastatur- und
Screenreader-Aufgaben für große Listen und deren vollständige native
Collection-Semantik sind noch zu prüfen. Insbesondere sind nicht alle
Breadcrumb-Vorfahren in die eigene Tabfolge aufgenommen; Alt + Aufwärts und
Ctrl + L ersetzen die vollständige Tababnahme nicht. Die Sortierauswahl lässt
sich per Tastatur umschalten, öffnet dabei aber noch nicht denselben Popupablauf
wie der Mausklick.

Lange Pfade bis in die vorhandenen Öffnen-/Importfunktionen bleiben separat zu
prüfen; ein langer Vorschaupfad beweist keine durchgehende Unterstützung. Echte
Screenreader-Aufgaben, zusätzliche DPI-/Monitorzustände und macOS sind offen.

Zusätzlich offen: genaue deutsche Browserkollation statt der aktuellen
SDL-Casefold-Reihenfolge, tatsächlich verweigerte ACL-Zugriffe, echte SMB-Freigaben,
Speicherfehler-Injektion und macOS-Laufnachweise. Das 4096-Byte-Pfadlimit und die
8192-Einträge-Grenze werden ausdrücklich behandelt. Kooperativer Abbruch kann
keinen bereits laufenden Kernelaufruf unterbrechen. Diese Grenzen sind keine
pauschale Behauptung, das freigegebene Design sei technisch unmöglich.

Der [physische Windows-Pfeiltastennachweis für die Protokollhöhe](PROTOCOL-PHYSICAL-KEYS-NATIVE.md)
ist am 29. Oktober bestanden; er ist keine Abnahme der Explorer-Tastaturbedienung.
Das Gesamtziel einschließlich Explorer A und vollständiger nativer
Oberflächenabnahme bleibt offen. Der frühere F1-UIA-Scrollbefund ist im
[Reihenfolgenachlauf](../f1-layouts/UIA-SCROLL-ORDER.md) separat geschlossen.
