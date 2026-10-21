# Grafikexport: nativer Umsetzungsstand

Der freigegebene [Grafikexport](EXPORT-EXAMPLE.md) nach
[ADR 010](../../decisions/010-analysis-graphic-export.md) bleibt vollständig
beauftragt. Dieser Stand ergänzt seine Datengrundlage; der neue Dialog,
seine Vorschau und die native Bildkopie sind noch nicht angebunden.

## Festgehaltene Diagrammdaten

`app/graphic_snapshot.c` stellt einen unveränderlichen, eigenen Diagrammstand
bereit. Ein ausgewähltes Berichtsdiagramm wird einschließlich Kurvenarten,
Masken, Einheiten, Titel, Herkunft, Referenz und expliziten Achsengrenzen kopiert.
Späteres Freigeben oder Ändern des ursprünglichen Berichts verändert diese Kopie
nicht. Eine weitere Kopie mit den gerade sichtbaren Achsengrenzen kann später
beim Öffnen des Dialogs den tatsächlich angezeigten Stand festhalten.

Der gemeinsame Messdatenpfad kann bis zu acht Läufe mit jeweils eigener Zeitachse
übernehmen. Kanäle werden anhand von Namen, Dimensionen und Winkel-/Skalarrolle
zugeordnet. Werte werden in die festgehaltene Anzeigeeinheit umgerechnet.
Linien und Punktdarstellung bleiben ausdrücklich wählbar; Histogramme werden
aus vorhandenen Berichtsdiagrammen kopiert und nicht aus beliebigen Messreihen
erfunden. Die erste Referenz wird vom Aufrufer übergeben, nicht erraten.

Alle zugeordneten Messstatuskanäle eines Laufs werden auf den bestehenden
Wertebereich 0/1/2 geprüft, einschließlich gerade nicht gezeichneter Kanäle.
Nur Status 1 trägt gültige Werte bei. Die vorhandene begrenzte Berichtsvorschau
erhält Spitzen, Endpunkte und Segmentgrenzen; `source_count` macht ihre Reduktion
sichtbar. Die vollständige Statistik gültiger Quellwerte wird separat in SI
festgehalten. Ein nicht darstellbares Statistikresultat bleibt ausdrücklich
als solches markiert und wird nicht zu einer erfundenen Nullstatistik.

Die ursprünglichen Zeitgrenzen bleiben auch bei ungültigen Randsamples erhalten.
Es gibt keine implizite gemeinsame Abtastung verschiedener Läufe. Jeder
Datensatz wird nach Übernahme geschlossen. Ein Fehler veröffentlicht keinen
halben Schnappschuss; das vorherige Ausgabeobjekt bleibt erhalten.
Kooperative Abbruchpunkte liegen an Phasen- und Blockgrenzen. Ein bereits
laufender Snapshot-/Reduktionsaufruf des Kerns wird nicht unterbrochen.

Die Vergleichsoberfläche muss diesen Pfad noch über eigene Worker-Aufträge mit
veraltete Antworten verwerfender Übernahme verwenden. Der Exportdialog soll
anschließend genau die bereits angezeigte Kopie übernehmen. Er soll keine
Messdateien nachträglich erneut öffnen, um ein scheinbar gleiches Bild zu erzeugen.

## Tatsächlich geprüfter Datenbaustein

Quellstand **graphic-export-7**, Basis `3d98554`. Windows mit MSVC/ASan und
Alpine-Dev mit ASan/UBSan/LSan bestehen jeweils **3/3** Gruppen:
`graphic_snapshot`, `report`, `series_mask`. Zusätzlich wurden beide nativen
Anwendungen erfolgreich gebaut. Die vorhandene Oberfläche bleibt in diesem
Baustein unverändert; daraus wird keine neue visuelle Abnahme abgeleitet.

Der neue Fall prüft unter anderem:

- Eigene Kurven-/Maskenkopien, Referenz, Herkunft und eingefrorenen Ausschnitt.
- Acht Eingaben, unterschiedliche Kanalreihenfolge und unabhängige Zeitachsen.
- 6.001 ursprüngliche Samples, erhaltene positive/negative Spitzen, Statuslücken
  und vollständige Statistik vor Reduktion.
- Umrechnung m/cm und rad/Grad, Winkel-/Skalartrennung, Linien und Punkte.
- Lange UTF-8-Dateinamen mit vollständigem Originalpfad und gültig gekürztem Label.
- Weiter nutzbare Kopien nach dem Löschen ausschließlich der Testquelldateien.
- Quotenfehler, acht Abbruchpunkte und sämtliche 5 beziehungsweise 9 beobachteten
  Allokationsfehlerstellen der Berichts- und Laufkonstruktion; keine verlorenen
  eigenen Blöcke oder veränderten vorherigen Ausgabeobjekte.

Eine Gegenprobe auf Stand 2 akzeptiert einen ungültigen Status in einem gerade
nicht ausgewählten Messkanal. Die korrigierte Prüfung aller zugeordneten
Statuskanäle weist denselben Fall ab. Die negativen Stände 4 bis 6 betreffen
zusätzliche Testannahmen zu namenlosen Kanälen: Modellregistrierung und
Analyse-/Series-Leser verlangen nichtleere Namen; der rohe Dateicodec kann
ein solches Feld zwar schreiben. Der abschließende Test erhält den tatsächlichen
`PS_INVALID`-Vertrag. Der frühe Abbruch der 6er-Testfunktion ließ ihr zuvor
erfolgreich erzeugtes Winkelobjekt am Prozessende offen; der finale Fall läuft
bis zur regulären Freigabe und besteht auch LeakSanitizer.

[Ausführungen, Quellen und Hashes](graphic-snapshot-2026-10-21/validation.json) ·
[Abschlussaudit](graphic-snapshot-2026-10-21/audit.json).
Die zehn separat erfassten lokalen Accessibility-Änderungen bleiben erhalten.
Lokale App-Builds enthalten diese Änderungen und beweisen keinen sauberen
Checkout nur dieses Patches. Es gibt keine aktuelle macOS-Ausführung.

## Verbleibende Umsetzung des gesamten Exportvertrags

| Teil | Noch erforderliche Arbeit |
| --- | --- |
| Datenansicht | Gemeinsamen Mehrlaufcache in die sichtbare Kurve, Referenz, Legende und Vergleichsstatistik anbinden; asynchrone Ergebnisse anhand der tatsächlichen Auswahl verwerfen. |
| Gemeinsame Darstellung | Eine native Grafikbeschreibung für Vorschau, PNG und SVG gemäß `graphic-export.js` aufbauen. Die bisherigen PNG-/SVG-Wege haben noch unterschiedliche Layouts. |
| Dialog und Zugänge | Freigegebenes Hell-/Dunkellayout einschließlich kleiner Fenster, Grafikbuttons und einzelner Berichtsdiagramme umsetzen. |
| Optionen | PNG/SVG, vier PNG-Größen, vollständige/sichtbare Grenzen und beim Öffnen festgehaltene Darstellung anbinden. |
| Speichern | Native Dateiauswahl, Abbruch, bestehende Dateien und Schreib-/Renderfehler mit derselben festgehaltenen Ausgabe prüfen. |
| Bildkopie | Tatsächliche Übergabe an die native Zwischenablage und unabhängiges Übernehmen des Bildes prüfen. |
| Bedienung und Abnahme | Dialogfokus, Escape/Rückkehr, Tastatur, große Schrift, Kontrast, Resize/DPI und sämtliche ADR-010-Fälle tatsächlich ausführen. |

Die noch offenen Teile bleiben beauftragt. Der geprüfte Datenbaustein erfüllt
keine dieser gesamten Produktabnahmen für sich allein.

## Geprüfte Plattformverträge für die folgende Anbindung

Die verwendete SDL-Version ist 3.2.30. Ihr
[Windows-Clipboardadapter](https://github.com/libsdl-org/SDL/blob/release-3.2.30/src/video/windows/SDL_windowsclipboard.c)
übergibt Bilder über `image/bmp` als `CF_DIB`. Andere angebotene Bildtypen
können in dieser Fassung lediglich im SDL-internen Speicher liegen. Ein
erfolgreiches Zurücklesen von `image/png` im gleichen Prozess beweist deshalb
keine echte Windows-Bildkopie. Das ist ein Quellbefund, noch kein ausgeführter
Bildkopienachweis.

[SDL_SetClipboardData](https://wiki.libsdl.org/SDL3/SDL_SetClipboardData) gehört
auf den Hauptthread und bietet Daten über einen späteren Callback an.
Der [festgelegte SDL-Kern](https://github.com/libsdl-org/SDL/blob/release-3.2.30/src/video/SDL_clipboard.c)
kann bei einem Plattformfehler weiterhin den registrierten Callback besitzen;
die folgende Anbindung muss diesen Besitz auch auf Fehlerpfaden behandeln.
[SDL_ShowSaveFileDialog](https://wiki.libsdl.org/SDL3/SDL_ShowSaveFileDialog)
arbeitet asynchron und kann seinen Ergebnis-Callback auf einem anderen Thread
aufrufen. Dialog-/Ausgabedaten müssen dafür eigene Lebenszeiten erhalten.
