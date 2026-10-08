# Den Arbeitsbereich bedienen

Die drei Hauptbereiche **Entwickeln**, **Simulieren** und **Auswerten** stehen als
horizontale, gleich breite Tabs über die gesamte Fensterbreite direkt unter der
kompakten Menüzeile. Menü und Fensterknöpfe liegen zusammen ganz oben im Fenster.
Ziehe den freien Bereich neben den Menüs, um das Fenster zu verschieben;
die Fensterränder dienen zur Größenänderung.
Ein Wechsel ändert die Ansicht, ohne einen laufenden Prozess zu stoppen.
Projektdateien enthalten dein Modell und deine Auswertung; `runs/` enthält
gespeicherte Messläufe und Ergebnisse.

Die Projektdatei speichert auch Buildprofil, Experimentparameter, Zeitschritt,
Zufallsseed und Simulationsgeschwindigkeit. Beim erneuten Öffnen stehen diese Werte wieder bereit.
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
Die Pendelvorlagen zeigen Gewichtskraft und Stangenkraft als beschriftete
Pfeile mit 0,05 m/N. Das C-Pendel zeigt bei eingeschaltetem Luftwiderstand
zusätzlich dessen Kraft in Violett; der Widerstand wirkt entgegen der
Tangentialgeschwindigkeit. Die roten/grünen Kräfte beziehen sich auf eine
starre, masselose Stange und bleiben vom Geschwindigkeitspfeil unterschieden.
Der [Pendellernpfad](pendulum-tutorial.md) erklärt Gleichungen und Maßstäbe.

Neue Pendelprojekte mit Physim-Auswertung verwenden automatisch die
Pendelanalyse aus dem Lernpfad. Sie zeigt Winkel und Energieabweichung,
vergleicht gespeicherte Läufe und berechnet die mittlere Periodendauer aus
positiven Nulldurchgängen. Das gilt auch für ein C-Experiment mit
Physim-Auswertung. Ein kurzer oder ruhender Lauf erhält keine erfundene
Periodendauer; die Zusammenfassung und seine Kurven bleiben sichtbar.
Vorhandene Projektquellen werden beim Öffnen nicht ersetzt. Andere Vorlagen
verwenden weiterhin ihre zum Modell passende Auswertung.

Die C-Standardauswertung ergänzt bei Pendeldaten das Diagramm **Mechanische
Energieänderung** mit `E - E(0)` in Joule. Im Vakuum zeigt es den numerischen
Energiefehler; mit Luftwiderstand enthält die Änderung auch dissipierte Energie.
Die Tabelle enthält die maximale Energieabweichung und, bei genügend positiven
Nulldurchgängen, die mittlere Periodendauer. Neben den Diagrammexporten wird
`<Analysepräfix>-energy.csv` mit sämtlichen Zeit-, Energie- und Änderungswerten
geschrieben. Die vorhandene Energiebilanz gedämpfter Modelle bleibt getrennt
auswertbar. [Nachweise](platform-validation.md#energieplot-und-vollständiger-csv-im-c-pendelablauf).

Neue C- und Physim-Pendelvorlagen besitzen dieselben sieben Parameter:

| Parameter | Einheit | Standard | Bedeutung |
| --- | --- | --- | --- |
| `length` | m | 1,5 | Stangenlänge |
| `initialAngle` | rad | 0,45 | Anfangsauslenkung |
| `mass` | kg | 1 | Masse des Pendelkörpers |
| `airDensity` | kg/m3 | 0 | Homogene Mediumdichte; 1,225 aktiviert die Referenzluft |
| `dragCoefficient` | 1 | 0,47 | Konstanter quadratischer Widerstandskoeffizient |
| `area` | m2 | 0,01 | Angeströmter Querschnitt |
| `sensorNoise` | rad | 0 | Standardabweichung des gaußverteilten Winkelsensorrauschens |

Die gemeinsame Medium-API berechnet den Widerstand entgegen der Geschwindigkeit.
Ein positiver Widerstand benötigt positive Dichte, positiven Koeffizienten und
positiven Querschnitt. Velocity Verlet unterstützt diesen geschwindigkeitsabhängigen
Term nicht; die Vorlage weist diese Kombination beim Start ab. Die übrigen
Pendelvarianten verwenden denselben Term auch in adaptiven Schritten.

`angle` bleibt der wahre Modellwinkel; `sensor.angle` enthält zusätzliches
Messrauschen. Derselbe Laufseed und dieselbe Schrittfolge liefern nach Reset
dieselben Messwerte. Rauschen verändert keine Kräfte oder Zustandsintegration.
Das Modell verwendet eine masselose starre Stange und ein homogenes ruhendes
Medium mit konstantem Widerstandskoeffizienten; es enthält keine Strömungs- oder
Reynoldsmodellierung. Der Vakuumlernpfad zum Integratorvergleich bleibt gesondert.

Die Pendelauswertungen ergänzen die Tabelle **Observed amplitude decay** mit
erkannter Spitzenzahl, mittlerem logarithmischem Dekrement und beobachteter
Abnahmerate samt Spannweite. Spitzen- und Intervall-CSVs behalten sämtliche
Werte. Wachsende Amplituden liefern negative Raten, kurze oder ruhende Läufe
keinen erfundenen Wert. Die Auswertung benutzt den wahren Modellwinkel;
Messraster und Integrator beeinflussen die beobachteten Spitzen. Sie passt
keinen konstanten physikalischen Widerstandskoeffizienten an.
[Verfahren und Grenzen](pendulum-tutorial.md#beobachtete-amplitudenabnahme).

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
Das Verschieben eines Workspace auf einen anderen Rechner ist noch offen.
Ein Prozessabbruch sichert keinen neuen Workspace;
ungespeicherte Projektquellen haben die separate [Autosave-Wiederherstellung](autosave.md).
Allgemeine Dokumente bieten beim erneuten Öffnen der Datei ihre separat im
persönlichen App-Datenverzeichnis gespeicherte Sicherung zur Wiederherstellung an.

## Benannte Workspaces

**Datei → Workspaces …** verwaltet bis zu acht benannte Arbeitsumgebungen.
Öffne einen Ordner, richte zusätzliche Pfade und Dokumente ein und wähle dort
**Workspace speichern**. Gespeichert werden Hauptordner, bis zu 32 zusätzliche
Pfade, die Reihenfolge von bis zu 16 offenen Dokumenten, aktives Dokument,
Hauptbereich sowie Cursor, Auswahl und Scrollposition der Projekteditoren und
Dokumente. Es werden Ansichten gespeichert; der Text kommt beim Öffnen aus den
aktuellen Dateien. Einen vorhandenen Namen ersetzt bewusstes Speichern.
Namen unterscheiden Groß- und Kleinschreibung und dürfen bis zu 63 UTF-8-Bytes
ohne Steuerzeichen oder Rand-Leerzeichen enthalten.

Wähle einen Eintrag und **Öffnen**, um ihn wiederherzustellen. Der vollständige
Ordnerpfad erscheint als Hinweis über der Pfadzeile; lange Pfade zeigen in der
Zeile ihr Ende. Vor dem Wechsel werden die aktuellen Projektänderungen und
Dokumente gespeichert. Ein Speicherfehler hält den bisherigen Workspace offen.
Eine vorhandene Projektdatei und ihre beiden Quellen werden geladen, bevor
die bisherige Arbeitsumgebung freigegeben wird; Lesefehler verhindern den Wechsel.
Laufende Jobs, offene Dialoge und ungelöste Wiederherstellungen sperren **Öffnen**.
**Workspace speichern** kann auch während einer Simulation die aktuelle Ansicht
erfassen, ohne den Lauf oder ungespeicherten Text zu ändern.

Wiederöffnen startet weder Build noch Simulation. Nicht erreichbare Hauptordner
bleiben im Katalog; fehlende zusätzliche Pfade und Dokumente werden wie bei der
letzten Arbeitsumgebung behandelt. Nach externen Kürzungen werden Editoransichten
auf den vorhandenen Text begrenzt. Autosaves erfordern weiterhin eine bewusste
Wiederherstellung. Theme, Schriftgröße und [Panelanordnungen](settings.md#benannte-panelanordnungen)
werden separat verwaltet.

**Löschen** entfernt nur den gewählten Katalogeintrag. Projektdateien, Laufdaten
und der separate letzte Workspace bleiben erhalten. **Schließen** oder Escape
verlässt die Verwaltung. Geänderte Ansichten überschreiben einen benannten
Eintrag erst beim ausdrücklichen Speichern; das Beenden speichert weiterhin
die letzte Arbeitsumgebung in `workspace.bin`.

Der persönliche Katalog liegt als `workspaces.bin` neben `workspace.bin`.
Format 1 enthält versionierte Workspace-Zustände und prüft Namen, Pfade,
Ansichten, Größenlimits und CRC-Prüfsummen. Speichern, Löschen und bewusstes
Zurücksetzen schreiben eine geschlossene temporäre Datei und ersetzen danach
die bisherige. Fehler erhalten Datei und Katalog. Beschädigte oder unbekannte
Dateien sperren die Verwaltung; nur **Datei bewusst zurücksetzen** ersetzt sie
durch einen leeren Katalog. Bei mehreren App-Instanzen gewinnt der zuletzt
vollständig gespeicherte Stand. Ein Stromausfall während des Austauschs ist
nicht abgesichert. Die vorhandenen Workspace-Dateien der Formate 1 und 2 bleiben
lesbar. Ein alter letzter Workspace lässt sich öffnen und anschließend unter
einem Namen speichern; er wird nicht automatisch in den Katalog übernommen.

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

Dort wird auch das Projektformat angezeigt. Bei Format 1 aktualisiert
**Projektformat aktualisieren** die Beschreibung ausdrücklich auf Format 2 und
legt die vorherige Fassung als `physim.project.bak` ab. Vorher müssen Änderungen
an Experiment, Analyse und Projekteinstellungen gespeichert oder verworfen sein.
Laufende Jobs und ungelöste Wiederherstellungen sperren die Aktion. Wurde die
Beschreibung seit dem Öffnen extern geändert, muss das Projekt erneut geöffnet
werden; die externe Fassung wird nicht überschrieben. Quellen und archivierte
Läufe bleiben erhalten. Öffnen und normales Speichern migrieren nicht automatisch.
Für die Kommandozeile: `physim-build --migrate-project --project PROJEKTORDNER`.
Dieser eigenständige Befehl benötigt keinen Compiler und startet keinen Build.
[Format und Speichergrenzen](data-format.md#eigenständige-analyseprojekte).

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

**Geschwindigkeit** unter den Steuerknöpfen bietet 0,25×, 0,5×, 1×, 2×, 4×,
8× und 16× Echtzeit sowie **Offline**. Bei 1× entspricht eine Simulationssekunde
ungefähr einer realen Sekunde, sofern das Modell schnell genug berechnet wird.
Offline berechnet ohne Zeitvorgabe so schnell wie möglich. Die Ansicht erhält
höchstens 60 reguläre Momentaufnahmen pro Sekunde; jeder Physikschritt wird gespeichert.
Du kannst die Geschwindigkeit im laufenden oder pausierten Modell ändern.
Eine Änderung startet weder einen neuen Lauf noch setzt sie einen pausierten Lauf fort.
**Einzelschritt** berechnet im festen Modus genau den eingestellten Zeitschritt.
Im adaptiven Modus akzeptiert er genau einen Modellschritt innerhalb der gewählten Grenzen.
Die Wahl bleibt nach Reset und beim erneuten Öffnen des gespeicherten Projekts erhalten.

Die Geschwindigkeit verändert weder `dt` noch Seed, Parameter oder Messwerte an
denselben Simulationszeitpunkten. Ein Echtzeitkonto sammelt verstrichene Zeit für
feste Schritte. Nach Pause, Geschwindigkeitswechsel oder längerer Verzögerung wird
übermäßiger Rückstand verworfen: Das Modell überspringt keine Physikschritte und
versucht keinen unbegrenzten Sprung zur aktuellen Echtzeit. Langsame Modelle oder
eine blockierte Datenübertragung können daher hinter der gewünschten Echtzeit zurückbleiben.

Die **Zeitleiste** unter der 3D-Ansicht erlaubt einen Rückblick auf aufgezeichnete
Zustände. Klicke auf die Leiste oder ziehe ihren Griff; die Pfeile wechseln zum
vorherigen/nächsten Zustand. Szene und angezeigter Kanalwert gehören zum selben
gespeicherten Zeitpunkt. Die senkrechte Markierung im Diagramm zeigt die Auswahl.
Bei **Rückblick** nennt der Fensterinhalt zusätzlich die aktuelle Simulationszeit.
Der Versuch kann im Hintergrund weiterlaufen. **Live** folgt wieder dem Runner;
bei einem beendeten Lauf heißt die Schaltfläche **Letzter**.

**Abspielen / Anhalten** und die **Leertaste** geben aufgezeichnete Zustände in
Echtzeit wieder. Die Leertaste gilt im Simulationsbereich, solange kein Eingabefeld
oder Menü aktiv ist. Wiedergabe und Rückblick verändern weder Modell noch Seed,
Messdatei oder Laufsteuerung. **Pause/Fortsetzen** und **Einzelschritt** steuern
weiterhin den Versuch; **Zurücksetzen** beendet zusätzlich den Rückblick.

Szenen werden direkt in der `.psrun`-Datei gespeichert. Nach dem Öffnen eines Laufs
unter **Läufe & Berichte** stehen sie wieder unter **Simulieren** bereit, ohne
ein Experimentmodul auszuführen. Auch gültige Zustände eines rekonstruierten Laufs
bleiben verfügbar. Ältere Dateien ohne Szenen erhalten eine Zeitleiste für ihre
Messwerte; die 3D-Ansicht weist auf die fehlende Aufzeichnung hin.
Die App liest Dateien im Hintergrund und hält höchstens 2048 Vorschauzustände im
Speicher. Bei langen Läufen werden Zwischenzustände ausgedünnt; Anfang und letzter
Zustand bleiben erhalten. Ein bereits betrachteter Zustand bleibt währenddessen
unverändert. Die Datei enthält weiterhin alle aufgezeichneten Szenen und alle
Messpunkte. Es wird keine Geometrie zwischen Zuständen interpoliert. Fehler der
Szenenaufzeichnung erscheinen in der 3D-Ansicht; vorhandene Messdaten bleiben auswertbar.

**Zurücksetzen / F7** beendet einen laufenden oder pausierten Runner und öffnet
einen neuen Lauf **pausiert bei 0 Sekunden**. **Einzelschritt** berechnet danach
genau einen festen beziehungsweise einen akzeptierten adaptiven Schritt; **Fortsetzen** startet die laufende Simulation.
Seed, Zeitschritt und Experimentparameter bleiben erhalten. Nach einem bereits
beendeten Lauf gelten die aktuell gespeicherten Laufeinstellungen.
Jeder Neustart erhält eine eigene Messdatei mit Quellcode-Snapshot und Laufgrenzen;
die bisherigen Läufe bleiben unter **Läufe & Berichte** verfügbar.

Die App wartet im Hintergrund auf das Prozessende und das Lesen des alten
Datensatzes. Nach einer Sekunde ohne Stop-Antwort beendet sie den Runner;
vollständige Messblöcke bleiben rekonstruierbar. **Stoppen** funktioniert auch,
wenn die Initialisierung noch keine Antwort geliefert hat. Ein fehlgeschlagener
Neustart erhält den bisherigen angezeigten Zustand und meldet den Fehler.
Erst ein gültiger Anfangszustand ersetzt Messwerte, Live-Verlauf und Szenenauswahl;
die Kamera bleibt erhalten. Geänderte Quellen müssen vor dem Zurücksetzen
gespeichert und erfolgreich gebaut werden.

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
Gruppen und Elternbeziehungen bilden einen aufklappbaren Szenenbaum. Das Ausblenden
eines Elternknotens wirkt auf seine Nachfahren; deren eigene Sichtbarkeitswahl
bleibt erhalten. Die Auswahl eines Objekts öffnet seinen Weg im Baum. Szene und
Messdaten bleiben beim Umordnen der Anzeige unverändert. Alte aufgezeichnete
Szenen ohne Beziehungen erscheinen als flache Liste.
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

Ziehe eine Paneltitelzeile, um Seitenleiste, Arbeitsbereich und Protokoll anzuordnen.
Die fünf Ziele in der Mitte eines anderen Panels bilden eine Tabgruppe oder teilen
den Platz links, rechts, oben oder unten. Außerhalb dieser Ziele entsteht ein frei
platziertes Panel im Hauptfenster. Escape bricht das Verschieben ab. Trennlinien
und der Griff rechts unten verändern die Größe. **Ansicht** stellt ausgeblendete
Panels und die Standardanordnung wieder her. Der nächste Start lädt die Anordnung
mit ausgewählten Tabs und den Positionen frei platzierter Panels.
[Panelanordnung und Grenzen](settings.md#panelanordnung)

**Einstellungen / Ctrl+,** bietet unabhängige UI- und Codeschrift, Autosave und Darstellung.
Übernehmen speichert, Abbrechen verwirft den Einstellungsentwurf.
Die Darstellung lässt sich zwischen Dunkel, Hell und Hoher Kontrast wählen;
die Auswahl gilt auch für Editor, Diagramme und das geöffnete Hilfefenster.

**F10** aktiviert die Hauptmenüleiste. **Pfeil links/rechts** oder **Tab/Shift+Tab**
wechseln zwischen Datei, Ansicht, Hilfe und Einstellungen. **Pfeil hoch/runter**
öffnet Datei oder Ansicht und wählt verfügbare Einträge; deaktivierte Aktionen
werden übersprungen. **Home/End** wählen den ersten/letzten verfügbaren Eintrag.
**Enter** oder **Leertaste** führt die Auswahl aus. **Escape** oder erneut **F10**
beendet die Menübedienung. Ein Rahmen markiert den Tastaturfokus. Ein Klick
außerhalb oder ein Fokuswechsel zu einem anderen Fenster schließt das Menü.
Währenddessen verändern die Menütasten und Texteingaben keinen geöffneten Editor.
Enter führt die beim Tastendruck gewählte Aktion aus; nach Escape kannst du
im zuvor aktiven Editor ohne erneuten Mausklick weiterschreiben.

- **Ctrl+1 / Ctrl+2 / Ctrl+3:** Entwickeln / Simulieren / Auswerten.
- **Ctrl+4:** Läufe und Berichte.
- **Ctrl+S:** Quellen speichern.
- **Ctrl+F:** In der aktuellen Ansicht suchen.
- **Ctrl+L:** Protokoll ein- oder ausblenden.
- **F5:** Speichern und bauen.
- **F6:** Simulation starten oder pausieren.
- **F7:** Simulation auf den pausierten Anfangszustand zurücksetzen.
- **F1:** Eigenständiges Handbuchfenster öffnen.
- **F10:** Hauptmenüleiste mit der Tastatur bedienen.
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


## Adaptive Simulationsschritte

Aktiviere im Inspector unter **Laufeinstellungen** vor dem Start **Adaptive Schritte**.
`dt` ist dann die Startdauer. **Start (s)**, **Minimum (s)** und **Maximum (s)** erlauben normale
Zahlen oder wissenschaftliche Schreibweise, etwa `1e-6`. Es gilt
`0 < Minimum ≤ dt ≤ Maximum ≤ 1 s`. Unvollständige oder ungültige Eingaben verhindern
Speichern und Start. Die Wahl und Grenzen werden im Projekt gespeichert;
ältere Projekte erhalten feste Schritte. Änderungen sind während des Laufs gesperrt.

Das Experiment muss adaptive Schritte unterstützen. Die C- und Physim-Pendelvorlagen
verwenden dafür Dormand–Prince 5(4); ihre bisherigen festen Integratoren bleiben
über den festen Modus erreichbar. Ein Modell ohne adaptive Schnittstelle wird mit
lesbarer Runnerdiagnose abgewiesen. Eine passende Schrittsteuerung ist Teil des
Modells, nicht automatisch für beliebige Kraft-, Kontakt- oder Zufallsmodelle ableitbar.

Die Geschwindigkeit bleibt unabhängig von der Physik. Ein verworfener numerischer
Versuch speichert keine Messung; jeder akzeptierte Schritt speichert genau einen
Messpunkt mit tatsächlicher Zeit. Einzelschritt akzeptiert einen Schritt, Pause
hält die Zeit fest. Reset verwendet wieder die Startdauer, Grenzen, Parameter und
den Seed und erhält die alte Laufdatei. Zeitleiste und wiedergeöffnete Daten verwenden
auch die unregelmäßigen Zeiten. Die eigene Analyse verwendet die Zeitspalte für
Ableitungen, Integrale und Diagramme.

Die CLI bietet denselben Weg:

```sh
build/native/Release/bin/physim-runner \
  build/native/Release/bin/pendulum.so build/adaptive-pendulum.psrun \
  --adaptive --dt .005 --min-dt 1e-8 --max-dt .1 --steps 500 --record-scenes
```

Unter Windows ist das Modul `pendulum.dll`, das Programm hat `.exe`.
`--steps` zählt akzeptierte Schritte; 500 adaptive Schritte haben keine vorab feste
Endzeit. Ohne Grenzen sind Minimum `1e-8 s` und Maximum `0.1 s` eingestellt.
Grenzoptionen erfordern `--adaptive`. Die Monte-Carlo-/Parameterstudien-Steuerung bietet feste Raster oder eine
[gemeinsame Zielzeit mit adaptiven Schritten](monte-carlo.md#gemeinsame-zielzeit-und-adaptive-serien).

Die Messdatei hält Startschritt, Modus, Grenzen, Seed, Modulidentität und Modell-
Metadaten fest. Für Reproduktion müssen auch Ableitung, Toleranzen und
Versuchsbudget Teil des Modellcodes oder seiner Metadaten sein. Bei zu wenig
Metadatenplatz startet der adaptive Runner keine unvollständig beschriebene Datei.
Ein anderer Integrator, andere Toleranzen oder andere Grenzen können die Messzeiten
und damit zeitabhängige Sensor-/Zufallsaufrufe ändern. Ein Vergleich solcher Modelle
verwendet gemeinsame Zeitpunkte beziehungsweise Resampling.

### Zugriffsrechte beim Speichern

Unter macOS und Linux erhalten gespeicherte Quellen und ihre `.bak`-Kopien die
bisherigen gewöhnlichen Lese-, Schreib- und Ausführungsrechte (beispielsweise
`0600` oder `0755`). Zwischenfiles werden exklusiv mit `0600` erzeugt. Neue
Hauptquellen und Autosaves bleiben `0600`. Set-ID-Bits, ACLs und erweiterte
Attribute werden bei der Ersetzung nicht übernommen. Auf Windows gelten die
geerbten Rechte der exklusiv erzeugten CRT-Dateien; deren ACL-Erhaltung wurde
hier nicht geprüft.

### Handbuch per Tastatur

Im Handbuch beginnt Tab beim C-Lernweg; Shift+Tab führt rückwärts. Der Fokuspfad
umfasst beide Lernwege, Themen/Inhalt, globale Suche und Kategorie, die
angezeigten Themen oder Überschriften, Zurück/Start/Arbeitsbereich, Dokumentensuche,
Treffersteuerung, Lesebereich sowie Links und Codekopieren. Der sichtbare Rahmen
scrollt zum fokussierten Eintrag. Enter oder Leertaste aktiviert eine Auswahl;
Pfeile wechseln die Kategorie. In Suchfeldern funktionieren die üblichen
Texttasten; Tab schließt die Eingabe ab, ohne das letzte Zeichen zu verlieren.
Im Lesebereich scrollen Pfeile und Bild auf/ab; Home und End springen zum Anfang
und Ende. Ctrl+F (macOS Cmd+F) fokussiert die Dokumentensuche. Escape schließt
das Handbuch. Ein Mausklick setzt die Zeigerbedienung fort.

### Projekte per Tastatur anlegen

Öffne **Datei → Neues Projekt** mit F10, Pfeiltasten und Enter. Tab führt durch
Zielordner, Ordnerwahl, Namen, Projekttyp, Vorlage, Experiment- und Auswertungssprache,
Anlegen und Zurück. Shift+Tab geht rückwärts; reine Analyseprojekte überspringen
Vorlage und Experimentsprache. Der sichtbare Fokus scrollt im kleinen Fenster mit.
Pfeile wechseln die Auswahl eines Felds; Enter oder Leertaste bedient die Aktionen.
In den Textfeldern bleiben die üblichen Text- und Clipboard-Befehle verfügbar.
Tab schließt bereits eingegangene Texteingaben ab. Escape und Zurück verlassen
das Formular. Validierungsfehler erscheinen vollständig im Formular; vorhandene
Projektdateien werden erhalten. **Ordner wählen** verwendet denselben nativen
Dialog wie bei Mausbedienung.

Für C-Experimente wählen Kugelstoß, Wurf mit Unsicherheit und Boxstoß nun dieselben
passenden Physim-Auswertungen wie ihre Physim-Experimentvorlagen. Die allgemeine
Positionsanalyse benötigt `position.x`; Kugelstoß stellt stattdessen
`a.position`/`b.position` bereit und benötigt die eigene Stoßauswertung.

Pendelprojekte speichern `velocity.x`, `velocity.y` und `speed` direkt in `m/s`.
Die Komponenten behalten das Vorzeichen, der Betrag ist nichtnegativ und
entspricht `length * abs(angular_velocity)`. Sie erscheinen wie die übrigen
Messkanäle in Live-Werten, gespeicherten Läufen und Rohdatenexporten; die
positionsbasierte Ableitung in der Auswertung bleibt eine eigene Schätzung.

Allgemeine Pendelprojekte besitzen zusätzlich den Laufparameter `integrator`:
`0` Euler, `1` symplektischer Euler, `2` RK4, `3` Velocity Verlet,
`4` Dormand–Prince 5(4). Wähle eine ganze Zahl im Parameterformular; die
Vorlagen behalten ihre bisherigen Standardverfahren. Jede Instanz erhält ihre
eigene Auswahl, die auch in den Laufmetadaten steht. Verlet benötigt
`airDensity = 0`, `dragCoefficient = 0` oder `area = 0`, weil dieser
Positionsbeschleuniger keinen geschwindigkeitsabhängigen Widerstand integriert.
Der adaptive Laufmodus verwendet weiterhin ausdrücklich Dormand–Prince,
unabhängig von der Auswahl für feste Ausgabeschritte; seine Metadaten geben
beide Einstellungen getrennt an.

Das unabhängige SDK-Prüfkit bietet `--pendulum-only` für die komplette
Pendel-Daten-/Analysekette: verschobenes Paket, installierte und aus den
Paketquellen neu gebaute Bibliothek, beide Sprachen, fünf auswählbare Verfahren,
neun SI-Kanäle, adaptive Läufe, Spitzen-API und vollständige Amplituden-CSVs.
Der Modus prüft die Pendelkette ohne Oberfläche; die vollständige SDK-Prüfung
enthält dieselbe Gegenprobe zusätzlich zu den übrigen Prüfungen.
