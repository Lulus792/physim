# Dateitabs, Dateizeichen und erreichbare Simulationsbereiche

Stand: 23. Oktober 2026, auf Basis `6d8c3b2`. Dieser Teilabgleich führt die
[Nutzerbefunde](USER-REVIEW-2026-10-21.md)
zu Workspace und gemeinsamen Bedienelementen fort. Die Gesamtoberfläche ist
weiterhin nicht vollständig abgenommen.

Die Werkbank misst Tabbreiten aus echtem Dateinamen, Schriftzeichen und
Innenabständen. Vorgaben sind 28 px Höhe, 5 px Streifenrand, 3 px Abstand und
11 px horizontales Innenmaß bei Standardschrift. Schließen bleibt über Maus
und Tastatur erreichbar; das Kreuz erscheint beim Zeigerkontakt oder Fokus
innerhalb des vorhandenen Randes. Lange Namen bleiben vollständig erhalten
und lassen sich bis zum Ende horizontal erreichen. Dirty-Markierung,
Dokumentwechsel, Speichern, Abbruch und Umordnen bleiben echte Editoraktionen.

C und P verwenden eine separate kräftige Monospace-Schrift mit 12 CSS-px,
keine künstlich verbreiterten UI-Buchstaben. Windows lädt Consolas Bold;
die geprüfte Linux-Umgebung lädt DejaVu Sans Mono Bold. Der Offline-Modus
verwendet den unveränderten Noto Sans Mono SemiBold aus dem in der
Fontmanifestdatei festgehaltenen Quellstand. Dessen Lizenz ist bytegleich
zur bereits mitgelieferten gemeinsamen Noto-Lizenz. Die Projektdatei erhält
die kleine Raute der Vorlage. Verzeichnis-Chevrons liegen auf der Pfadlinie;
die Linie wird hinter dem Chevron ausgespart.

Ein ergänzender nativer Rücklauf zeigte einen bestehenden Mindesthöhenfehler:
Compilerwarnungen hielten unter Windows das Protokoll offen. Im 1080×740-Client
blieben dem Workspace nur 208 px und dem Messbereich 27 px. Sein Titelzentrum
lag im Protokolltrenner; Herausziehen konnte deshalb nicht beginnen.
Die rekursive Mindestgröße berücksichtigt jetzt Szene und Messbereich sowie
das sichtbare Protokoll. Gespeicherte Wunschgrößen bleiben erhalten. Im
geprüften kleinen Client ergeben sich Szene 180 px, Messbereich 130 px,
Protokoll 68 px und Zeitleiste 101 px statt des weiterhin gespeicherten
120-px-Wunsches. Mehr Inhalt im optionalen Protokoll benötigt dort vertikales
Scrollen oder ein größeres beziehungsweise herausgelöstes Fenster: Die
Summe aller Wunschhöhen passt nicht gleichzeitig in diesen Client.

Die kurze Protokollfläche verwendet einen gemeinsamen vertikalen Scrollfluss
und keinen horizontalen Scrollbalken. Eine zusätzliche Bildkontrolle fand
anschließend einen eigenen Textfehler: Der normale Nuklear-Button meldete ein
erreichbares Zeilenrechteck, verwarf aber sein sehr niedriges Textrechteck schon
vor der eigentlichen Glyphenzeichnung. Die kompakte Diagnose zeichnet deshalb
die volle Schrifthöhe in derselben Fläche und behält native Aktivierung und Fokus.
Der unabhängige Rasterprüfer akzeptiert 937 Textpixel im korrigierten Windows-
Bild und verwirft das unveränderte vorherige Bild mit 0 Textpixeln.

## Tatsächlich ausgeführte Prüfungen

Die [Prüfdaten](workbench-tabs-2026-10-23/validation.json) enthalten getrennte
Quellstände, Programmhashes, Befehle, Rückgabewerte und Grenzen. Die internen
Nummern sind Prüfstände dieses Arbeitsschritts, keine Produktversionen.

| Ausführung | Ergebnis und Umfang |
| --- | --- |
| Windows Release, Stand 6 | 2/2: Dateitab-Designmatrix und großer schmaler Inspektor-/Teilfensterablauf. Zusätzlich Grafikdialog 1/1. |
| Alpine-Dev, Debug ASan/UBSan, Stand 6 | Dieselben 2/2; zusätzlich Grafikdialog 1/1. Tatsächlich gebaut und ausgeführt. |
| Windows Debug, Stand 4 | 5/5: bisheriger Dateiablauf, Workspace-Baum, Werkbank, neue Tabmatrix und F1-Schrift-/Formelprobe. |
| Alpine-Dev, Stand 4, Projekt-Arbeitsverzeichnis | 5/6: Baum, Werkbank, Docking, großer Inspektor und Tabmatrix bestehen; bisheriger Dateiablauf erreicht seine feste 15-s-App-Grenze. |
| Alpine-Dev, Stand 4, ext4-Arbeitsdaten | Dateiablauf besteht in 9,99 s, F1-Schrift-/Formelprobe in 48,09 s. Ursprüngliche innere Grenzen 15/45 s und äußere Grenzen 65/75 s bleiben unverändert. |
| Installiertes Windows-Release-SDK, Stand 4 | Echter Start mit erzwungenen Offline-Schriften besteht; 15 primäre Hauptfensterschriften einschließlich des neuen Zeichensatzes. Neue Schriftdatei und gemeinsame Lizenz stimmen mit Quelle und Paketmanifest überein. Das Paket enthält noch den Stand vor der letzten Protokollkorrektur. |

Die acht kanonischen Exporte je Plattform bleiben [bytegleich](workbench-tabs-2026-10-23/exports.json)
zum vorherigen plattformspezifischen Prüfstand. Vorhandene separate lokale
Accessibility-Änderungen liegen den Testbuilds zugrunde; ihre zwölf Rohhashes
sind gesichert, die Änderungen werden nicht in diesen Commit übernommen.

Die Tabmatrix startet je Plattform einmal mit System- und einmal mit Offline-
Schriften. Jeder Prozess prüft Dunkel/Hell bei 16 sowie Hell/hohen Kontrast bei
22 im 1080×740-Client, Hover, nativen Tastaturfokus, Enter-Aktivierung und Schließen.
Eine echte Datei mit 105 Byte langem Namen bleibt vollständig im Tab und lässt
sich bis zum sichtbaren Ende scrollen und per Maus schließen. Die abschließende
Dirty-Aufnahme setzt ausdrücklich nur einen Anzeigezustand; echte Bearbeitung,
Speichern und Abbruch gehören zum getrennten bisherigen Dateiablauf.

Windows vergleicht zusätzlich die erste aktive Tabfläche direkt mit beiden
ursprünglichen PNGs. Die farbigen Außenbounds treffen jeweils
`(223, 124)–(285, 151)`; die berechnete Breite beträgt 63,0371 logische Pixel.
Das belegt diese Fläche bei Standardschrift, keine vollständige Glyphenidentität.
Die nativen Zeichenprüfungen erfassen druckbare ASCII-Glyphen, gleiche Monospace-
Advances und die separate starke Quellschrift.

Die F1-Probe inventarisiert 185 Seiten und 183 unterschiedliche druckbare
Quell-/Formel-Lesezeichen über 42 vollständige F1-Schriftvarianten, zusätzlich
drei Modellzeichen in vier kursiven Serifenvarianten. Der Offline-Lauf prüft
alle 57 primären Hauptfenster-/F1-Textschriften. Das sind Zeichen-/Raster- und
Bediennachweise; die vollständige H-Gestaltungsabnahme bleibt getrennt offen.
Bei ext4 bleiben Programm und Quellen auf `/mnt/d/Physim`; nur Arbeitsdaten und
Aufnahmen liegen unter `/tmp`. Daraus folgt keine allgemeine Dateisystemursache
für jeden Zeitfehler und kein Hardware-DPI-Nachweis.

Die großen Teilfensterfälle prüfen Szene ≥180, Messbereich ≥130, erhaltene
Wunschhöhen, Titel außerhalb der Trenner, sechs unterschiedliche echte Fenster,
Rückkehr, Messkurven, native Tastaturänderung, Wiedergabe, F1 und Editor-Undo.
Windows verwendet dabei echte Compilerdiagnosen (2 bzw. 14). Bei warnungsfreiem
Linux-Build ergänzt der Stand 6 ausdrücklich eine synthetische Diagnose nur als
Darstellungsfixture; sie wird nicht als echter Compilerfehler ausgegeben.
Die letzte Zeile wird per Rad erreicht, ihre Textpixel werden geprüft und der
Scrollstand kehrt zum Anfang zurück. Das [Gegenbeispiel](workbench-tabs-2026-10-23/file-tabs-compact-counterexample.json)
lässt sich mit [verify-negative.py](workbench-tabs-2026-10-23/verify-negative.py)
erneut auswerten.

Frühere Fehler bleiben erhalten: Stand 1 scheitert unter Windows an einem
ungeeigneten PNG-Referenzdecoder im neuen Prüfer, nicht am nativen Tabablauf;
Stand 2 verwendet dafür den bereits vorhandenen Pillow-Prüfweg. Dessen normale
Aufnahmen klären außerdem den vorherigen Fokuszustand der Fixture und gleichen
die Köpfe bei großer Schrift an. Stand 2/3 belegen die verdrängte Messfläche.
Der gemountete Linux-F1-Lauf von Stand 2 überschreitet die äußeren 75 s.
Stand 5 besteht seine damaligen Geometrieprüfungen, zeigt aber noch das leere
Diagnosefeld; erst der unabhängige neue Pixelvergleich verwirft diesen Zustand.
Ein einmal falsch benannter Grafik-Testfilter startet keine Tests; der korrigierte
Katalogfall `graphic_dialog` besteht danach auf beiden Plattformen.

## Native Aufnahmen und offene Unterschiede

33 [Aufnahmen](workbench-tabs-2026-10-23/pictures.json) erhalten die originalen
nativen PNG-Bytes. Zwei Rückkehrbilder sind verlustfrei von BMP nach PNG verpackt;
Maße und sämtliche RGB-Bytes werden dabei gleich geprüft. Referenzbilder werden
nicht verändert. Vollständige Aufnahme-/Loghashes stehen im
[Artefaktverzeichnis](workbench-tabs-2026-10-23/artifacts.json).

| Zustand | Tatsächliche Aufnahme |
| --- | --- |
| Werkbank, Dunkel | [Windows](workbench-tabs-2026-10-23/windows-dark-normal.png) · [Linux](workbench-tabs-2026-10-23/linux-dark-normal.png) |
| Werkbank, Hell | [Windows](workbench-tabs-2026-10-23/windows-light-normal.png) · [Linux](workbench-tabs-2026-10-23/linux-light-normal.png) |
| Große Schrift | [Windows](workbench-tabs-2026-10-23/windows-large-normal.png) · [Linux](workbench-tabs-2026-10-23/linux-large-normal.png) |
| Letzte Diagnose | [Vorher, leer](workbench-tabs-2026-10-23/negative-source5-log-text.png) · [Windows korrigiert](workbench-tabs-2026-10-23/windows-c-contrast-large-narrow-log-end.png) · [Linux-Fixture](workbench-tabs-2026-10-23/linux-c-contrast-large-narrow-log-end.png) |
| Alle Bereiche zurückgeholt | [Windows](workbench-tabs-2026-10-23/windows-panels-returned.png) · [Linux](workbench-tabs-2026-10-23/linux-panels-returned.png) |

Die aktuelle Linux-Aufnahme zeigt bei großer Schrift weiter eine übergroße
Tempo-Chevronfläche und einen umgebrochenen Zurücksetzen-Button. Diese sichtbaren
Fehler sind trotz erfolgreicher Bedienprüfung **nicht abgenommen** und der nächste
konkrete Simulationsabgleich. Der ältere Analysecode-Editorweg verwendet außerdem
noch die frühere Tabdarstellung; Werkbank und Analysecode-Zustand müssen dort
zusammenhängend angeglichen werden. Eine Modellnotiz benötigt einen tatsächlichen
Quell-/Datenvertrag; die Pendelformel der Demo darf nicht anderen Modellen als
eigene Notiz zugeordnet werden.

Weitere Icons, Schriften, Glyphenraster, alle Seiten-/Hover-/Fokuszustände und
die restliche Oberfläche bleiben gemäß [Gesamtliste](NATIVE-FIDELITY.md) offen.
Es gibt hier keine ausgeführte macOS-Umgebung und keinen neuen physischen
DPI-/Monitor- oder vollständigen externen Screenreader-Nachweis. Vollständige
1:1-Übereinstimmung oder volle Punktzahl wird damit nicht behauptet.
