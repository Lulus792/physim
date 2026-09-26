# Den Arbeitsbereich bedienen

Die drei Hauptbereiche sind **Experiment**, **Simulieren** und **Auswerten**.
Ein Wechsel ändert die Ansicht, ohne einen laufenden Prozess zu stoppen.
Projektdateien enthalten dein Modell und deine Auswertung; `runs/` enthält
gespeicherte Messläufe und Ergebnisse.

## Projekt anlegen oder öffnen

Links unter **Öffnen oder anlegen** wählst du Vorlage, Zielordner und Sprache der
Auswertung. **Anlegen** erzeugt ein Projekt; **Öffnen** lädt ein vorhandenes.
C-Vorlagen verwenden `main.c`; Sprachvorlagen verwenden `main.phys`.
Die Auswertung liegt in `analysis.c` oder `analysis.phys`. Die App legt die passende
CMake-Konfiguration an. Arbeite für Varianten in getrennten Projektordnern.

Pendel eignet sich für Integratoren, Wurf für Bahnen, Kugelstoß und Boxstoß für
Kontakte, Box auf Ebene für Reibung, Feder–Masse–Dämpfer für Energie,
Auftrieb für Medien und Wurf mit Unsicherheit beziehungsweise Sensorwurf für
Messmodelle. Die Physim-Vorlagen sind als solche im Vorlagennamen gekennzeichnet.

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

Ziehe die Trennlinie neben der Seitenleiste oder über dem Protokoll, um Platz zu
verteilen. **Einstellungen / Ctrl+,** bietet Codeschrift, Autosave und Darstellung.
Übernehmen speichert, Abbrechen verwirft den Einstellungsentwurf.

- **Ctrl+1 / Ctrl+2 / Ctrl+3:** Experiment / Simulation / Auswertung.
- **Ctrl+4:** Läufe und Berichte.
- **Ctrl+S:** Quellen speichern.
- **Ctrl+F:** In der aktuellen Ansicht suchen.
- **Ctrl+L:** Protokoll ein- oder ausblenden.
- **F5:** Speichern und bauen.
- **F6:** Simulation starten oder pausieren.
- **F1:** Eigenständiges Handbuchfenster öffnen.
- **Ctrl+,**: Einstellungen.

Im Hilfefenster sucht Ctrl+F nur im Dokument, die linke Suche in allen Inhalten.
Esc schließt die Hilfe. Die Arbeitsbereichskürzel werden dort nicht ans Hauptfenster
weitergegeben. Vollständige Tastaturfokussierung und Screenreader-Unterstützung
sind noch nicht implementiert.
