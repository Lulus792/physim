# Explorer A: native Umsetzung

Stand vom 28. Oktober 2026. Maßgeblich bleiben
[ADR 011](../../decisions/011-selected-workspace-additions.md) und die
[freigegebene Auswahl](../design-options/APPROVED.md). Explorer A ist für freie
Datei-/Ordnernavigation, Messlaufauswahl und den Unteraufruf aus Projektdialog B
ausgewählt. Dateisystemmodell und asynchroner IO-Arbeiter samt Textvorschau sind
implementiert und geprüft; der bisherige Systemdialog bleibt bis zur Einbindung
der A-Oberfläche produktiv.

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

## Verbleibende Umsetzung und Abnahme

Die [Modulentscheidung](../../decisions/013-native-explorer-filesystem.md)
legt die nächste Integration fest. Noch offen sind Navigationscontroller mit
Verlauf und Auswahl-/Vorschaubezug, die native A-Anordnung mit Orten/Liste/Vorschau
und die tatsächlichen Aufrufe aus allen Bereichen. Messlauf-/Bildvorschau und
passender C-API-Kontext sind zusätzlich zur Textvorschau umzusetzen. Die Module
sind über native Tests, noch nicht über die produktive APP-Quellliste gebaut.
Die sichtbare Oberfläche wird dadurch nicht als ersetzt oder abgenommen bezeichnet.
Auch lange Pfade bis in die vorhandenen Öffnen-/Importfunktionen sind separat
zu prüfen; ein langer Vorschaupfad beweist diese durchgehende Unterstützung nicht.

Der native Abgleich muss anschließend Tastatur, Fokuskehr, große Schrift,
Fenstergrößen, Themes, Material-Fallback, Screenreader und die Auswahl echter
Dateien/Ordner/Messläufe prüfen. Verdeckte beziehungsweise virtualisierte Zeilen
müssen erreichbar bleiben; die bestehende Zahl an Accessibility-Knoten darf
nicht unbemerkt die größere Dateiliste abschneiden.

Zusätzlich offen: genaue deutsche Browserkollation statt der aktuellen
SDL-Casefold-Reihenfolge, tatsächlich verweigerte ACL-Zugriffe, echte SMB-Freigaben,
Speicherfehler-Injektion und macOS-Laufnachweise. Das 4096-Byte-Pfadlimit und die
8192-Einträge-Grenze werden ausdrücklich behandelt. Kooperativer Abbruch kann
keinen bereits laufenden Kernelaufruf unterbrechen. Diese Grenzen sind keine
pauschale Behauptung, das freigegebene Design sei technisch unmöglich.

Die manuelle Windows-Pfeiltastenprüfung bleibt auf Nutzerwunsch bis zu seiner
erneuten Rückmeldung vertagt. Das Gesamtziel einschließlich Explorer A,
F1-Linkkorrekturen und vollständiger nativer Oberflächenabnahme bleibt offen.
