# Automatisch sichern und wiederherstellen

Physim sichert ungespeicherte Änderungen in `main.c` beziehungsweise `main.phys`
und `analysis.c` beziehungsweise `analysis.phys`
standardmäßig alle 30 Sekunden gemeinsam im Projektordner. Unter
[Einstellungen](settings.md) lassen sich 10, 30, 60 oder 120 Sekunden wählen.
Die Quelldateien werden dadurch nicht
geändert. Der Hinweis im Editor zeigt an, ob eine automatische Sicherung vorliegt.
Ctrl+S und Build/F5 speichern weiterhin die eigentlichen Quelldateien; vor dem
Ersetzen einer vorhandenen Quelldatei wird deren bisheriger Inhalt als `.bak` kopiert.
Beim regulären Schließen oder Projektwechsel speichert die App geänderte Quellen.

## Nach einem Abbruch

Öffne denselben Projektordner erneut. Liegt eine Sicherung mit abweichendem Inhalt
vor, zeigt Physim zuerst die Wiederherstellung:

- **Änderungen wiederherstellen** lädt beide gesicherten Texte in die Editorfenster.
  Prüfe sie und speichere mit Ctrl+S oder baue mit F5. Erst dabei ändern sich die
  Quelldateien. Wiederherstellung allein baut oder startet keinen Code.
- **Gespeicherte Dateien verwenden** öffnet die vorhandenen Quellen und verwirft die
  automatische Sicherung.
- Schließen des Fensters ohne Auswahl erhält die Sicherung für das nächste Öffnen.

Wurden die Quelldateien seit der Sicherung extern geändert, zeigt der Dialog einen
entsprechenden Hinweis. Physim erkennt dies durch Vergleich mit den vollständig
gesicherten Ausgangstexten. Es gibt keine automatische Zusammenführung: Wiederherstellen
lädt die gesicherten Editorinhalte beider Dateien. Vor dem anschließenden Speichern
gegebenenfalls die externen Änderungen separat kopieren.

## Grenzen und Fehler

Zwischen Änderung und nächster Sicherung kann das gewählte Intervall liegen. Eine
blockierte Anwendung oder ein fehlgeschlagener Schreibzugriff kann diesen Zeitraum
verlängern. Die Sicherung ist zusätzlicher Schutz und ersetzt kein bewusstes Speichern.
Pro Quelldatei werden maximal 256 KiB gültiger UTF-8-Text unterstützt.
Ein Projekt sollte jeweils nur in einer Physim-Instanz bearbeitet werden; mehrere
gleichzeitige Autoren und deren Zusammenführung werden noch nicht unterstützt.

Eine beschädigte oder unlesbare Sicherung wird nicht in den Editor übernommen und
nicht automatisch überschrieben. Die gespeicherten Quellen bleiben bearbeitbar;
die Seitenleiste meldet **Autosave pausiert**. Zum erneuten Aktivieren die Datei
`.physim-autosave` außerhalb der App an einen anderen Ort sichern, aus dem Projektordner
entfernen und das Projekt erneut öffnen. Eine allein zurückgebliebene
`.physim-autosave.tmp` stammt von einem unvollständigen Schreibvorgang und wird beim
Öffnen ignoriert. Die letzte vollständig übernommene Sicherung bleibt maßgeblich.

## Speicherformat

`.physim-autosave` ist ein privates App-Format mit Kennung `PSAUTO01`. Es enthält
beide Editorinhalte, beide zuletzt gespeicherten Ausgangstexte und den UTC-Zeitstempel
der Sicherung. Vier Längenfelder begrenzen den Gesamtinhalt auf 1 MiB Text; eine
CRC-32 schützt Header und Inhalt vor unbemerkter Beschädigung. Beim Lesen werden
auch Version, reservierte Felder, UTF-8 und das exakte Dateiende geprüft.

Die App schreibt zuerst eine vollständige temporäre Datei, schließt sie und ersetzt
danach die vorherige Sicherung durch Umbenennen. Beide Editorinhalte gehören dadurch
immer zur selben Sicherung. Es gibt keine Zusage für Stromausfälle, defekte Datenträger
oder besondere Netzwerkdateisysteme. Die beiden eigentlichen Quelldateien werden
beim bewussten Speichern einzeln ersetzt, nicht als gemeinsame Dateisystemtransaktion.
