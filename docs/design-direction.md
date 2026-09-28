# Designrichtung: kompakte Menüs und Arbeitsbereich-Tabs

Verbindliche Produktentscheidung, 2026-09-21: Physim soll sich beim Einstieg im
Arbeitsablauf an Visual Studio Code orientieren: zuerst eine leere
Arbeitsumgebung, danach öffnet der Nutzer einen Ordner oder fügt Inhalte hinzu.

Die Orientierung an Visual Studio Code betrifft den Workspace- und
Navigationsfluss, nicht eine vollständige Kopie seiner Oberfläche. Physims
Editor, Simulation, 3D-Ansicht und Auswertung bleiben eigenständige
Arbeitsbereiche.

Aktualisierte Produktentscheidung, 2026-09-28: Die globale Bedienung folgt einer
kompakten, an Apple-Apps orientierten Menüzeile. Die großen Werkzeugschaltflächen
entfallen. **Entwickeln**, **Simulieren** und **Auswerten** stehen als horizontale
Tabs direkt darunter über die gesamte Fensterbreite, mit drei gleich großen
Bereichen. Menü und Fensterknöpfe bilden einen gemeinsamen Fensterkopf ohne
zusätzliche Betriebssystem-Titelzeile.
Der leere Workspace-Start und der getrennte Projektmanager bleiben bestehen.

## Verbindliches Zielbild

### 1. Leerer Start

- Beim Start ist kein Beispielprojekt und kein Pendelprojekt vorausgewählt.
- Die zentrale Fläche zeigt einen klaren Empty State für eine noch leere
  Arbeitsumgebung.
- Die erste Aktion ist verständlich und sichtbar: **Ordner öffnen**.
- Zusätzlich kann der Nutzer einzelne Dateien oder weitere Ordner zur
  Arbeitsumgebung hinzufügen.
- Ein vorhandener Ordner darf zunächst als Workspace geöffnet werden, auch wenn
  er noch keine `physim.project`-Datei enthält.

### 2. Kompakte Menüzeile und Tabs

Oben im Fenster gibt es eine dauerhaft sichtbare, flache Menüzeile, die in allen
Arbeitsbereichen verfügbar bleibt. Ihre Aktionen sind zusammengefasst:

- **Datei:** Ordner öffnen, Datei oder Ordner hinzufügen, neues Projekt, speichern.
- **Ansicht:** Protokoll, Dateibaum aktualisieren, Läufe und Berichte, Monte Carlo.
- **Hilfe** und **Einstellungen:** kompakte direkte Zugänge.

Darunter liegen drei schmale, gleich breite Tabs über die gesamte Fensterbreite.
Ihre Auswahlfläche ist leicht eingerückt, abgerundet und neutral grau.
Die Menüzeile hat einen durchgehenden Hintergrund; ihre Beschriftungen
haben im Ruhezustand keine eigenen Buttonflächen. Nur Hover und Auswahl werden
abgerundet hervorgehoben. Geöffnete Menüs bleiben hervorgehoben; ein Klick auf ein anderes
Menü wechselt unmittelbar dorthin. Bei offenem Menü genügt auch das Bewegen des
Mauszeigers zwischen Datei und Ansicht. Popup-Einträge teilen eine gemeinsame
Fläche, mit rechts ausgerichteten Tastenkürzeln und Markierung beim Überfahren.
Referenzen: Apples [Menüleiste](https://developer.apple.com/design/human-interface-guidelines/the-menu-bar)
und [Ansichtsumschalter](https://developer.apple.com/design/human-interface-guidelines/segmented-controls).
Das normale App-Fenster erhält abgerundete
äußere Ecken über den Windows-Compositor. Die Menüzeile sitzt direkt am oberen Fensterrand;
der freie Bereich neben den Menüs dient zum Verschieben des Fensters.
Die Tab-Reihe ersetzt die Arbeitsbereich-Schaltflächen der Seitenleiste; diese
beginnt mit dem Workspace. Die Menüzeile ist die zentrale Stelle für globale Aktionen. Arbeitsbereich-
spezifische Aktionen wie Build, Simulation oder Analyse bleiben im jeweiligen
Kontext und werden nicht unpriorisiert in die globale Leiste verschoben.

### 3. Separater Projektmanager

**Neues Projekt** öffnet eine eigene Projektmanager-Ansicht. Dieser Manager ist
nur für das Anlegen neuer Physim-Projekte zuständig und nicht der normale Weg,
einen bestehenden Ordner zu öffnen.

Der Projektmanager führt durch:

- Zielordner und Projektnamen
- Auswahl einer Vorlage
- Auswahl von C oder Physim als Experiment-/Analysesprache
- Erzeugung der benötigten Projektdateien
- anschließendes Öffnen des erzeugten Projekts im Workspace

Das Öffnen eines bestehenden Ordners bleibt davon getrennt und darf keine
Projektvorlage erzeugen oder vorhandene Dateien überschreiben.

### 4. Workspace nach dem Öffnen

Nach dem Öffnen zeigt die linke Navigation den Inhalt der Arbeitsumgebung:

- Ordner und Dateien
- geöffnete Dokumente
- Physim-Projektfunktionen, falls ein `physim.project` erkannt wurde
- Build-, Simulations- und Auswertungsbereiche, sobald sie für den Workspace
  verfügbar sind

Die drei fachlichen Bereiche **Entwickeln**, **Simulieren** und **Auswerten**
bleiben erhalten. Sie sind jedoch vom anfänglichen Workspace-Zustand getrennt:
Ein leerer oder beliebiger Ordner kann geöffnet werden, ohne sofort in einen
projektgebundenen Editorzustand gezwungen zu werden.

### 5. Zustände und Rückmeldung

Die Oberfläche muss mindestens zwischen diesen Zuständen unterscheiden:

- leerer Workspace
- Ordner geöffnet, noch kein Physim-Projekt erkannt
- Physim-Projekt geöffnet
- Projektmanager geöffnet
- Datei-/Ordnerauswahl aktiv
- laufender Build, laufende Simulation oder laufende Analyse

Der aktuelle Ordner bzw. Workspace, ungespeicherte Änderungen und laufende
Aktionen müssen in der oberen Leiste verständlich sichtbar sein.

## Abnahmekriterien

Die neue Designrichtung gilt als umgesetzt, wenn:

1. ein normaler App-Start ohne Projekt und ohne Pendel-Vorauswahl möglich ist;
2. ein Ordner über eine echte Ordnerauswahl geöffnet werden kann;
3. Dateien und Ordner zu einem geöffneten Workspace hinzugefügt werden können;
4. **Neues Projekt** einen getrennten Projektmanager öffnet;
5. das Öffnen eines bestehenden Ordners und das Erstellen eines neuen Projekts
   getrennte, nachvollziehbare Abläufe sind;
6. die kompakte Menüzeile in Entwickeln, Simulieren und Auswerten verfügbar
   bleibt und die drei Bereiche direkt darunter als Tabs erreichbar sind;
7. bestehende Physim-Projekte weiterhin direkt geöffnet und bearbeitet werden
   können;
8. leere, nicht erkannte oder unvollständige Ordner eine verständliche nächste
   Aktion anbieten, statt nur mit einem Projektformatfehler abzubrechen.

## Abgrenzung

Zusätzliches Endziel vom 2026-09-28, konkretisiert: Alle Projektdateien bleiben im
selben Projektordner. Physim erzeugt und pflegt seine eigene Projektdatei;
Buildprodukte und Zwischenergebnisse liegen gesammelt unter `build/`.
Laufdaten und Sicherungen bleiben ebenfalls beim Projekt. Nutzerprojekte benötigen keine
`CMakeLists.txt` und keine CMake-Installation; Physim verwaltet Compileraufrufe
und Abhängigkeiten selbst. Langfristig soll die gesamte Buildverwaltung ohne
CMake auskommen. Die verbindlichen Migrations- und Abnahmekriterien stehen im
[Projektplan](../Physim_Projektplan.md#2-verbindliche-produktziele).

Das Ziel ist eine klare Workspace-zentrierte App-Shell. Es ist nicht das Ziel,
Physim zu einer allgemeinen C-IDE für beliebige Softwareprojekte auszubauen.
Physim-spezifische Build-, Simulations- und Analysefunktionen bleiben an ein
erkanntes oder neu erzeugtes Physim-Projekt gebunden.
