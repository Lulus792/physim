# Den Arbeitsbereich bedienen

Die drei Hauptbereiche **Entwickeln**, **Simulieren** und **Auswerten** stehen als
horizontale, gleich breite Tabs über die gesamte Fensterbreite direkt unter der
kompakten Menüzeile. Menü und Fensterknöpfe liegen zusammen ganz oben im Fenster.
Ziehe den freien Bereich neben den Menüs, um das Fenster zu verschieben;
die Fensterränder dienen zur Größenänderung.
Ein Wechsel ändert die Ansicht, ohne einen laufenden Prozess zu stoppen.
Projektdateien enthalten dein Modell und deine Auswertung; `runs/` enthält
gespeicherte Messläufe und Ergebnisse.

Die Projektdatei speichert auch Buildprofil, Experimentparameter, Zeitschritt
und Zufallsseed. Beim erneuten Öffnen stehen diese Werte wieder bereit.
Änderungen an Zeitschritt oder Seed benötigen keinen neuen Build.

## Projekt anlegen oder öffnen

Physim startet mit einer leeren Arbeitsumgebung. **Ordner öffnen** in der oberen
Menüzeile unter **Datei** lädt einen bestehenden Ordner. Enthält er ein Physim-Projekt, werden dessen
Quellen geöffnet; weitere Textdateien kannst du im Editor bearbeiten.
**Datei → Datei hinzufügen** und **Datei → Ordner hinzufügen** ergänzen die Arbeitsumgebung um
bis zu 32 weitere Pfade.
Abbrechen im Systemdialog erhält den Workspace und seine Statusmeldung. Auch
ein leerer Rückgabepfad des Linux-Zenity-Backends gilt als Abbruch.

Der **Workspace** in der Seitenleiste zeigt einen aufklappbaren Dateibaum.
Ein Klick auf einen Ordner öffnet oder schließt dessen Inhalt; ein Klick auf eine
Textdatei öffnet sie im Editor. Innerhalb jedes Ordners stehen
Unterordner vor Dateien, jeweils nach ihrem Namen sortiert. Zusätzliche Dateien
und Ordner erscheinen als eigene Wurzeleinträge unter **Hinzugefügt**.
Der Mauszeiger zeigt den vollständigen Pfad als Hinweis.

**Dateibaum aktualisieren** liest externe Änderungen ein und behält die gerade
aufgeklappten Zweige bei. Geschlossene Unterordner werden erst beim Öffnen gelesen.
Nicht erreichbare Pfade und Lesefehler werden im Baum angezeigt. Die Anzeige ist
auf 8192 Einträge und 64 Unterordnerebenen begrenzt; beim Erreichen der Grenze
erscheint ein Hinweis. Zuklappen gibt Platz für andere Zweige frei. Hinzugefügte
Wurzeleinträge bleiben auch bei einem großen Hauptordner sichtbar.

**Datei → Neues Projekt** öffnet den Projektmanager. Dort wählst du Zielordner, Namen,
Vorlage sowie Experiment- und Analysesprache. **Projekt anlegen** erzeugt das Projekt.
C-Vorlagen verwenden `main.c`; Sprachvorlagen verwenden `main.phys`.
Die Auswertung liegt in `analysis.c` oder `analysis.phys`. Die App erzeugt und pflegt
`physim.project`. Der eigene Buildprozess liest diese Datei und ruft den Compiler
direkt auf. Alle erzeugten Builddateien liegen unter `build/Debug` beziehungsweise
`build/Release`. Eine `CMakeLists.txt` wird weder erzeugt noch benötigt.
Arbeite für Varianten in getrennten Projektordnern.

**Build-Einstellungen → Debug/Release** gehört zum jeweiligen Projekt. Speichern,
Bauen und normales Beenden übernehmen die Auswahl in `physim.project`; erneutes
Öffnen stellt sie wieder her. Ein Profilwechsel erfordert einen neuen Build.

Pendel eignet sich für Integratoren, Wurf für Bahnen, Kugelstoß und Boxstoß für
Kontakte, Box auf Ebene für Reibung, Feder–Masse–Dämpfer für Energie,
Auftrieb für Medien und Wurf mit Unsicherheit beziehungsweise Sensorwurf für
Messmodelle. Die Physim-Vorlagen sind als solche im Vorlagennamen gekennzeichnet.

## Letzten Workspace wieder öffnen

Beim normalen Beenden merkt sich Physim den zuletzt geöffneten Hauptordner und
seine hinzugefügten Dateien und Ordner. Beim nächsten Start zeigt die leere
Arbeitsumgebung **Letzten Workspace öffnen** samt gespeichertem Pfad.
Der Klick öffnet die Quellen und stellt die zusätzlichen Pfade wieder her.
Build und Simulation werden erst durch die jeweiligen Aktionen gestartet.

Ein nicht mehr erreichbarer Hauptordner lässt den Eintrag erhalten. Fehlende
zusätzliche Pfade werden gemeldet und bleiben gespeichert, damit beispielsweise
ein zeitweise getrenntes Laufwerk später wieder erreichbar sein kann.
**Eintrag vergessen** entfernt die gespeicherte Auswahl. Deine Projektdateien bleiben erhalten.

Der Zustand liegt als `workspace.bin` neben den persönlichen Einstellungen.
Beschädigte oder unbekannte Versionen werden gemeldet und unverändert erhalten;
**Workspace zurücksetzen** verwirft den gespeicherten Zustand ausdrücklich.
Ohne geöffneten Ordner wird beim Beenden kein neuer Zustand gespeichert.
Bei mehreren App-Instanzen gilt der zuletzt vollständig gespeicherte Zustand.
Gespeichert werden absolute Pfade, die Reihenfolge der geöffneten Textdokumente,
das aktive Dokument und der zuletzt geöffnete Hauptbereich. Die beiden Projekteditoren
und zusätzliche Dokumente behalten Cursor, Textauswahl und Scrollposition.
Wiederöffnen liest den aktuellen Dateiinhalt. Nach externen Kürzungen werden Cursor,
Auswahl und Scrollposition auf den vorhandenen Text begrenzt. Fehlende oder nicht
mehr lesbare Textdokumente werden übersprungen und gemeldet; die übrigen Dateien
öffnen sich weiter. Vorhandene Autosaves werden zur Wiederherstellung angeboten.
Die ältere Workspace-Datei ohne Dokumentansichten bleibt lesbar; beim nächsten
Speichern entsteht das neue Format.
Das Verschieben eines Workspace auf einen anderen Rechner und mehrere benannte
Workspaces sind noch offen. Ein Prozessabbruch sichert keinen neuen Workspace;
ungespeicherte Projektquellen haben die separate [Autosave-Wiederherstellung](autosave.md).
Allgemeine Dokumente bieten beim erneuten Öffnen der Datei ihre separat im
persönlichen App-Datenverzeichnis gespeicherte Sicherung zur Wiederherstellung an.

## Weitere Textdateien bearbeiten

Bis zu 16 Dokumente können gleichzeitig geöffnet sein. **Geöffnete Dokumente**
in der Seitenleiste und die Auswahl über dem Editor wechseln zwischen ihnen.
Jede Datei behält ihren eigenen Bearbeitungsstand. UTF-8-Textdateien dürfen
höchstens 256 KiB groß sein; Binärdateien und ungültiges UTF-8 werden abgewiesen.

**Ctrl+S** speichert das aktive Dokument und legt die vorherige Fassung als
`.bak` ab. Externe Änderungen blockieren das Speichern, damit sie nicht
überschrieben werden. **Neu laden** übernimmt den aktuellen Dateiinhalt.
**Schließen / Ctrl+W** schließt das Dokument. Bei ungespeicherten Änderungen
bieten beide Aktionen Speichern, Verwerfen und Abbrechen.

Vor Workspace-Wechsel, Build und normalem Beenden werden offene Dokumente
gespeichert. Scheitert das Speichern, bleibt der Vorgang beim betroffenen
Dokument stehen. Dokumente werden beim bewussten Wiederöffnen des gespeicherten
Workspace nach einem Neustart wiederhergestellt. Build, Simulation und Analyse
starten dabei nicht.

Bei geöffnetem Projekt machen Änderungen an zusätzlichen Dokumenten den letzten
Build ungültig: Auch Header, Builddateien oder geladene Modelldaten können das
Ergebnis beeinflussen. Dasselbe gilt beim Neuladen extern geänderter Dateien.
Nach Änderungen während eines Builds ist ein erneuter Build nötig, auch wenn
die Änderungen inzwischen gespeichert wurden.

## Editor und Build

**Ctrl+S** speichert. **F5 / Build** speichert beide Quellen und baut Experiment
und Analyse im Hintergrund. Ein Stern beziehungsweise der Hinweis auf ungespeicherte
Änderungen bedeutet, dass der Editor neuer als die gespeicherte Quelle ist.
Nach Änderungen muss neu gebaut werden, damit der nächste Lauf sie verwendet.
Unter **Build-Einstellungen** stehen Debug und Release zur Verfügung.

**Ctrl+F** öffnet die Suche im aktuellen Editor. Die Suchleiste bietet Suchen und
Ersetzen; kontrolliere insbesondere bei Ersetzungen von kurzen Namen das Ergebnis.
Das Protokoll zeigt Compilerfehler. Anklickbare Quelldiagnosen führen zur betroffenen
Stelle. **Analysecode bearbeiten** öffnet den zweiten Editor; **Zum Bericht** führt zurück.
Einzelne Quelldateien dürfen höchstens 256 KiB UTF-8-Text enthalten.
**Tab** fügt vier Leerzeichen als einen Bearbeitungsschritt ein. **Ctrl+Z** macht
ihn rückgängig; **Ctrl+Shift+Z**, **Ctrl+Y** oder **Ctrl+R** stellt ihn wieder her.
Auf macOS gelten **Cmd+Z** und **Cmd+Shift+Z** (alternativ **Cmd+R**).
Dies gilt auch für zusätzlich
geöffnete Textdateien.

Automatische Sicherungen ersetzen Ctrl+S nicht. Nach einem Abbruch bietet Physim
die Wiederherstellung an. Extern geänderte Dateien werden als Konflikt angezeigt.
[Sicherungen wiederherstellen](autosave.md)

## Simulation steuern

Stelle vor dem Start im Inspector **Laufeinstellungen** Zeitschritt und Seed ein.
Der Zeitschritt ist die Simulationszeit pro Schritt in Sekunden, keine Wartezeit
der Oberfläche. Ein kleinerer Wert erzeugt mehr Messpunkte und kostet Rechenzeit.
**F6** startet oder pausiert; **Fortsetzen** setzt denselben Lauf fort.
**Einzelschritt** ist zum Untersuchen eines pausierten Modells gedacht.
**Stoppen** beendet den Lauf und ermöglicht die Auswertung.
**Neuer Lauf** beginnt wieder am Anfang mit demselben eingestellten Seed.

Unter **Laufgrenzen** lassen sich Speicher in MiB und reale Laufzeit in Sekunden
begrenzen. Null bedeutet ohne Grenze. Die Zeit schließt Pausen ein; Änderungen
gelten für neu gestartete Simulationen und Auswertungen. Monte-Carlo-Serien haben
ein eigenes Zeitlimit. Ein abgebrochener Lauf kann als teilweise wiederhergestellt
lesbar sein; kennzeichne solche Daten bei der Interpretation.

## Szene untersuchen

- Rechte Maustaste ziehen: Kamera um das Ziel drehen.
- Mittlere Maustaste ziehen: Kameraziel verschieben.
- Mausrad: heran- oder herauszoomen.
- Linksklick: sichtbares Objekt auswählen; Klick auf freien Hintergrund hebt die Auswahl auf.
- **Vorne**, **Seite**, **Oben**, **Standard**: definierte Kameraansichten.
- **Orthografische Ansicht**: parallele Projektion ohne perspektivische Verkleinerung.

Unter **Szeneneinträge** kannst du Objekte einzeln ausblenden; **Alle einblenden**
macht sie wieder sichtbar. Vektoren, Flugbahnen, Punkte, Beschriftungen und Raster
haben zusätzliche gemeinsame Schalter. Dies verändert keine Messwerte.
Objekte mit stabiler ID bleiben auch beim Umsortieren richtig zugeordnet.
[Kameratastatur und Darstellung](settings.md)

## Messdaten und eigene Berichte

Nach Stop lädt **Auswerten** die Daten. Wähle einen Kanal über dem Diagramm.
Mittelwert, Stichprobenstandardabweichung, Minimum und Maximum beziehen sich auf
den ganzen Lauf. Ein Strich steht für fehlende Werte beziehungsweise eine
Standardabweichung mit weniger als zwei gültigen Messungen.
Bei Sensoren werden für die Vorschau und Statistik gültige Messungen berücksichtigt.
Im Rohdatenexport bleiben Status und alle aufgezeichneten Zeilen erhalten.

**Analyse starten** führt deine Analyse aus. **Analyseergebnis** zeigt ihre
Diagramme und Tabellen; **Messdaten** wechselt zurück. Das Diagrammmenü wählt
einen Plot. Mausrad zoomt um den Zeiger, Ziehen verschiebt den Ausschnitt;
**Zoom +**, **Zoom -** und **Alles zeigen** sind die entsprechenden Schaltflächen.
Ein Ausschnitt beeinflusst weder Statistik noch gespeicherte Messwerte.

**CSV exportieren** bei Messdaten enthält alle Messpunkte. Die Exportaktionen
beim Analyseergebnis beziehen sich auf den ausgewählten Plot oder die Tabelle.
SVG und PNG können den gesamten Plot oder den eingestellten Ausschnitt exportieren;
die PNG-Skalierung bestimmt die Bildauflösung. Dateien erhalten eigene Namen,
vorhandene Ausgaben werden nicht stillschweigend ersetzt.
[Diagramme, Tabellen und Exportdetails](reports.md)

## Frühere Läufe und Serien

**Läufe & Berichte / Ctrl+4** zeigt gespeicherte Ergebnisse. Suche und Filter
helfen beim Finden. Öffne Messdaten oder einen Bericht direkt, ohne neu zu bauen.
Wähle bis zu acht Läufe für die Mehrlaufanalyse; der erste ausgewählte Lauf ist
bei der Vergleichsvorlage die Referenz. Bei unterschiedlichen Zeitrastern muss
die Analyse eine gemeinsame Zuordnung herstellen.
[Läufe auswählen und vergleichen](runs.md)

**Monte Carlo** wiederholt ein gebautes Modell mit expliziten Seeds. Wähle
Laufanzahl, Parallelität, Dauer und auszuwertenden Kanal. Die Ergebnisstatistik
bezieht sich auf dessen Endwerte. Abbruch und fehlgeschlagene Einzelläufe sind
im Ergebnis zu berücksichtigen.
[Monte Carlo Schritt für Schritt](monte-carlo.md)

## Fenster, Hilfe und Tastenkürzel

Auf macOS gilt für die unten aufgeführten App-Befehle **Cmd** anstelle von **Ctrl**.

Ziehe die Trennlinie neben der Seitenleiste oder über dem Protokoll, um Platz zu
verteilen. **Einstellungen / Ctrl+,** bietet Codeschrift, Autosave und Darstellung.
Übernehmen speichert, Abbrechen verwirft den Einstellungsentwurf.

- **Ctrl+1 / Ctrl+2 / Ctrl+3:** Entwickeln / Simulieren / Auswerten.
- **Ctrl+4:** Läufe und Berichte.
- **Ctrl+S:** Quellen speichern.
- **Ctrl+F:** In der aktuellen Ansicht suchen.
- **Ctrl+L:** Protokoll ein- oder ausblenden.
- **F5:** Speichern und bauen.
- **F6:** Simulation starten oder pausieren.
- **F1:** Eigenständiges Handbuchfenster öffnen.
- **Ctrl+,**: Einstellungen.

Im Texteditor gelten außerdem die plattformüblichen Sprungbefehle:

| Bewegung | Windows / Linux | macOS |
| --- | --- | --- |
| Wort zurück / vor | Ctrl+Pfeil links / rechts | Option+Pfeil links / rechts |
| Zeilenanfang / -ende | Home / End | Cmd+Pfeil links / rechts |
| Dokumentanfang / -ende | Ctrl+Home / End | Cmd+Pfeil hoch / runter |

Mit zusätzlichem **Shift** wird der Text bis zum Ziel ausgewählt.

Im Hilfefenster sucht Ctrl+F nur im Dokument, die linke Suche in allen Inhalten.
Esc schließt die Hilfe. Die Arbeitsbereichskürzel werden dort nicht ans Hauptfenster
weitergegeben. Vollständige Tastaturfokussierung und Screenreader-Unterstützung
sind noch nicht implementiert.
