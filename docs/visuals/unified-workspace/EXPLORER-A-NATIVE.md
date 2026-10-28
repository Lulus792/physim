# Explorer A: native Umsetzung

Stand vom 28. Oktober 2026. Maßgeblich bleiben
[ADR 011](../../decisions/011-selected-workspace-additions.md) und die
[freigegebene Auswahl](../design-options/APPROVED.md). Explorer A ist für freie
Datei-/Ordnernavigation, Messlaufauswahl und den Unteraufruf aus Projektdialog B
ausgewählt. Der aktuelle Schritt implementiert und prüft das Dateisystemmodell;
der bisherige Systemdialog bleibt bis zur Einbindung produktiv.

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

## Verbleibende Umsetzung und Abnahme

Die [Modulentscheidung](../../decisions/013-native-explorer-filesystem.md)
legt die nächste Integration fest. Noch offen sind Controller mit
Anfragegeneration und Verlauf, Auswahl-/Vorschaubezug, die native A-Anordnung
mit Orten/Liste/Vorschau und die tatsächlichen Aufrufe aus allen Bereichen.
Das Modell ist dafür über seinen Test gebaut, aber noch nicht in die produktive
APP-Quellliste aufgenommen. Die sichtbare Oberfläche wird durch diesen
Modellschritt nicht als ersetzt oder abgenommen bezeichnet.

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
