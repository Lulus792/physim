# Monte Carlo und Laufserien

Eine Laufserie wiederholt dasselbe gebaute Experiment mit verschiedenen expliziten
Seeds. Jeder Lauf bekommt einen eigenen Runner-Prozess und Arbeitsordner. Bis zu
acht Läufe können gleichzeitig rechnen. Der Controller sammelt
den letzten Kanalwert und seinen Messstatus bei derselben Endzeit. Ohne
Parameterangabe gelten die Modellstandards; der Experimentcode entscheidet,
wie er den Seed verwendet. Die App kann einen benannten Parameter linear
variieren und weitere Parameter konstant halten.

Der [vollständige Lernpfad zu unsicheren Anfangswerten](monte-carlo-tutorial.md)
enthält passende Experimente und Archivanalysen in C und Physim.

## In der App

1. Ein Projekt anlegen, zum Beispiel „Wurf mit Unsicherheit“, und mit F5 bauen.
2. Links „Monte Carlo“ öffnen. Anzahl, Schritte, dt, Startseed, Kanal und die Zahl
   gleichzeitiger Läufe einstellen (1–8).
   Für eine Parameterstudie „Parameterstudie“ aktivieren, einen vom Experiment
   definierten Parameter wählen und Start- und Endwert innerhalb seiner Grenzen
   eingeben. Die App liest Definitionen nach jedem erfolgreichen Build aus dem Modul.
   Weitere Werte unter „Simulieren → Inspector → Laufeinstellungen →
   Experimentparameter“ gelten in allen Läufen der Serie konstant.
3. „Serie starten“ wählen. Voreinstellung: 256 Läufe, 200 Schritte, dt 0,005 s,
   Seed 42 und Kanal `position.x`. Die Endzeit ist dann eine Sekunde. Die App
   verwendet zunächst höchstens vier gleichzeitige Läufe, begrenzt durch die
   Anzahl logischer CPU-Kerne. Ein gleichzeitiger Lauf bedeutet sequenzielle Ausführung.
4. Abgeschlossene und aktive Läufe ablesen oder „Abbrechen“ wählen. Die App bleibt bedienbar.
5. Der fertige Bericht öffnet sich unter „Auswerten“. Die Seedserie liefert
   Histogramm und Tabellen, die Parameterstudie eine Endwertkurve. Plots und Tabellen
   lassen sich mit den üblichen CSV-/SVG-Funktionen exportieren. Später steht der
   Bericht unter „Läufe & Berichte“ auch ohne Build zur Verfügung.

Nach einer vollständigen Serie startet **Ersten Serienlauf auswerten** die gebaute
Analysequelle mit dem ursprünglichen Pfad von `run-0001.psrun`. Gewöhnliche
Analysen werten diesen einen Lauf aus; die Lernpfadanalyse liest darüber die
256 Nachbardateien im Serienordner. Die Rohdateien bleiben dort erhalten.

„Anleitung“ öffnet dieses Dokument direkt in Physim. Die API-Dokumentation und
alle öffentlichen Header bleiben über F1 erreichbar.

Ein bereits laufender Build, Einzelversuch oder Ladevorgang muss zuerst enden.
Die Quelle muss gespeichert und gebaut sein. Während einer Serie sind weitere
Builds, Simulationen und Projektwechsel gesperrt. Das Schließen der App beendet
alle noch aktiven Runner und bewahrt die bis dahin gespeicherten Dateien.

## Seeds, Dateien und Wiederholung

Der nullbasierte Laufindex i erhält `Startseed + i`. Ein 64-Bit-Überlauf wird
vor dem Start abgewiesen. Der Ausgabeordner wird exklusiv erstellt; bestehende
Serien werden niemals überschrieben. Unter `runs/<Zeitstempel>-batch/` liegen:

- `series.txt`: Version 5 für Seedserien und Parameterstudien mit explizitem
  Messstatus; ältere Versionen 2–4 stammen aus Serien ohne fehlende Endwerte.
  Konfiguration einschließlich Parallelität, Kanal, fester Parameterwerte,
  ursprüngliche Pfade und Seedregel.
- `experiment.dll` beziehungsweise `experiment.so`: verwendetes Modul als Kopie.
- `experiment.c` beziehungsweise `experiment.phys`: beim Start eingefrorener Editorinhalt bei App-Starts;
  im Kommandozeilenmodus optional eine Kopie der angegebenen Quelldatei.
- `run-0001.psrun` usw.: vollständige Messreihen mit Seed und Modellmetadaten.
- `work-0001/` usw.: eigener Arbeitsordner je Lauf. Relative Dateien des Moduls
  liegen hier und kollidieren nicht mit gleichnamigen Dateien anderer Läufe.
- `completed.csv`: fortlaufendes Journal mit Index, konkretem Seed, Dateiname,
  Endzeit, Messstatus und gegebenenfalls Endwert jedes vollständig geprüften Laufs, in Abschlussreihenfolge.
  Nach jedem Eintrag wird der Schreibpuffer an
  das Betriebssystem übertragen (flush), sodass abgeschlossene Zeilen bei einem
  geregelten Abbruch erhalten bleiben. Dies ist keine Stromausfallgarantie (fsync).
- `endpoints.csv`: dieselben akzeptierten Endwerte in aufsteigender Indexreihenfolge.
  Diese Datei entsteht am regulären Ende, auch bei Abbruch oder Fehler. Einträge
  können dann Lücken haben; es werden keine fehlenden Messwerte ergänzt.
- `status.txt`: `complete`, `cancelled` oder `failed`, gestartete/akzeptierte Läufe,
  höchste gleichzeitig aktive Prozesszahl, `valid`/`missing` und Fehler.
- `summary.psreport`: nur bei einer vollständig erfolgreichen Serie.

Bei einer Parameterstudie enthält das Manifest zusätzlich Name, Start- und
Endwert sowie die lineare Regel. `completed.csv` und `endpoints.csv` erhalten
eine Spalte `parameter_value`; jeder `.psrun`-Lauf speichert seinen tatsächlichen
Parameterwert und die vom Experiment definierten Grenzen in den Metadaten.
Weitere konstante Werte stehen im Manifest als `fixed_parameter.<name>`
und in jedem Lauf unter `parameter.<name>`.
Der Bericht zeigt eine Kurve aus Parameterwert und Kanalendwert. Die Statistik
einer Seedserie mit Histogramm, Quantilen und Konfidenzintervall wird für diese
gezielt verschiedenen Parameterwerte nicht verwendet.

Die App legt zusätzlich eine Berichtskopie direkt in `runs/` ab. Die Laufbibliothek
durchsucht aktuell keine Unterordner; die einzelnen Rohläufe stehen über
„Serienordner öffnen“ und die Analyse-API zur Verfügung. Fehlgeschlagene und
abgebrochene Serien haben keinen Gesamtbericht; auch eine unvollständige Datei
noch aktiver oder noch nicht ausgewerteter Läufe kann im Ordner verbleiben.
Ein Prozessfehler oder Zeitlimit beendet die gesamte Serie einschließlich der
anderen aktiven Runner. Das Journal und die bereits geprüften Endwerte bleiben erhalten.
Fehlender `status.txt` bedeutet
keinen bestätigten Abschluss, etwa nach einem harten Prozess-/Systemabbruch.

Auch beim harten Beenden nur des Controller-Prozesses endet jeder aktive Runner:
Er überwacht die ausschließlich dafür verwendete Standardeingabe-Pipe mit
`--parent-watch`. Ein geschlossener Elternanschluss beendet den Prozess mit Code
125, selbst wenn ein Modellcallback hängt. Bereits abgeschlossene Laufdateien und
Journalzeilen bleiben erhalten; ein Gesamtbericht wird dabei nicht erzeugt.
Die Überwachung gilt für die mitgelieferten Runner und schützt gegen unbeabsichtigte
verwaiste Prozesse. Sie ist keine Sicherheitsgrenze für absichtlich manipulierenden
Modulcode. Einzelne Offline-Aufrufe verwenden die Option nur mit einer offenen Pipe;
interaktiver Betrieb, Parameterabfrage und gewöhnliche Eingabedateien werden abgewiesen.

Gleiche Seeds, identischer Modulcode, gleiche Schritte beziehungsweise Zielzeit-/Integratorgrenzen und dieselbe Laufumgebung
reproduzieren die Messwerte, sofern der Code ausschließlich explizit gesetzte RNGs
verwendet. Ein eigener Zugriff auf Uhrzeit, externe Dateien oder Betriebssystem-Zufall
kann diese Eigenschaft aufheben. Der Snapshot umfasst keine externen Modulabhängigkeiten;
kompiler- und plattformübergreifend bitidentische Ergebnisse werden nicht zugesichert.

Die Laufnummer bestimmt den Seed und den Dateinamen bereits vor dem Prozessstart.
Die Statistik wird stets in Laufindex-Reihenfolge aufgebaut. Ein anderer
Parallelitätsgrad verändert daher bei deterministischem Experimentcode weder
Endwert-CSV noch Histogramm oder Kennzahlen. Das Abschlussjournal und die
Parallelitätsangabe im Bericht dürfen sich unterscheiden. Mehr Prozesse sind
bei sehr kurzen Läufen oder knappen CPU-/Datenträgerressourcen nicht automatisch schneller.

## Bedeutung der Statistik

Das Histogramm zählt alle gültigen Endwerte in gleich breiten Klassen; die letzte Klasse
schließt den größten Wert ein. Die Klassenanzahl ist die aufgerundete Quadratwurzel
der Zahl gültiger Endwerte. Gleiche Endwerte ergeben eine einzelne Klasse.

Mittelwert und Stichproben-Standardabweichung beschreiben die gültigen Endwerte. Bei nur
einem gültigen Endwert ist keine Standardabweichung ausgewiesen. Median sowie 2,5-%- und
97,5-%-Quantil werden linear interpoliert: `h=(n-1)*p`, zwischen den benachbarten
sortierten Werten. Das entspricht Typ 7 in der
[R-Referenz zu Quantilen](https://www.stat.ethz.ch/R-manual/R-devel/library/stats/html/quantile.html).

Ab 200 gültigen Endwerten erscheint zusätzlich ein angenähertes 95-%-Konfidenzintervall des
Mittelwerts: `Mittelwert ± 1,959963984540054 * s / sqrt(n)`.
Die Normalnäherung setzt unabhängige Stichproben und eine geeignete Verteilung mit
endlicher Varianz voraus. Die Laufzahl allein garantiert ihre Güte nicht: Starke
Schiefe, seltene Extremereignisse oder korrelierte Werte können sie unzuverlässig
machen. Es handelt sich weder um ein exaktes Student-t-Intervall noch um einen
automatischen Verteilungstest. Siehe
[NIST zu Konfidenzgrenzen des Mittelwerts](https://itl.nist.gov/div898/software/dataplot/refman1/auxillar/conflimi.htm).

Das Konfidenzintervall schätzt die Unsicherheit des Mittelwerts. Die Quantile
beschreiben dagegen den empirischen Wertebereich der Stichprobe; sie sind kein
Konfidenzintervall des Mittelwerts und kein kalibriertes Vorhersageintervall.

## Vorlage: Wurf mit Unsicherheit

Ein Körper von 1 kg startet bei x=-2 m, y=0 im Vakuum mit g=9,80665 m/s².
Es gibt keinen Boden und keine Kollision. RK4 integriert die Bewegung.
Einmal pro Lauf werden unabhängige Anfangsgeschwindigkeiten aus Normalverteilungen
gezogen: vx mit Mittelwert 3 m/s und Standardabweichung 0,15 m/s; vy mit 5 m/s
und 0,25 m/s. Die gezogenen Werte stehen in den Modellmetadaten.

- `position.x/y`, `velocity.x/y`, `energy`: tatsächliche Modellbewegung.
- `nominal.x/y`: Sollbahn mit den mittleren Anfangsgeschwindigkeiten.
- `sensor.x/y`: Messung mit 100 Hz, normalverteiltem Rauschen (0,02 m
  Standardabweichung), 0,01 m Offset, 0,002 m/s Drift, 0,005 m Auflösung und
  5 % Ausfallwahrscheinlichkeit. Die Bewegung bleibt unabhängig vom Sensor.
- `sensor.x.status` / `sensor.y.status`: 0 = nicht fällig, 1 = gültig, 2 = ausgefallen.
  Nur Status 1 darf in eine Messstatistik eingehen. Zeit, Standardunsicherheit und
  übersprungene Rasterplätze stehen in weiteren Kanälen. Anleitung und Bericht:
  [Messungen & Sensoren](measurement.md).

Bei t=1 s hat `position.x` theoretisch den Mittelwert 1 m und die
Standardabweichung 0,15 m. `nominal.x` ist für jeden Seed genau 1 m;
Der Sensor enthält zusätzlich Rauschen, systematische Effekte und Quantisierung.
Einzelne Monte-Carlo-Stichproben weichen von den theoretischen Modellwerten ab.
Bei einem Kanal mit zugehörigem `.status` entscheidet der Status am letzten
Simulationspunkt über dessen Aufnahme in die Statistik. Nur Status 1 ist gültig.
Die Serie läuft auch bei nicht fälligen oder ausgefallenen Endmessungen weiter;
der Bericht weist diese Fälle ausdrücklich aus. Siehe [fehlende Endwerte](#fehlende-endwerte).

## Kommandozeile und Grenzen

```text
physim-batch <runner> <module> <neuer-ordner> <kanal> <läufe> <schritte> <dt> <seed> [quelle] [--workers N] [--param name=wert ...] [--sweep name=start:end]
```

Für eine Parameterstudie ergänzt `--sweep name=start:end` einen gleichmäßigen,
inklusive Start- und Endwert verteilten Sweep. Mindestens zwei Läufe und
verschiedene endliche Grenzen sind erforderlich. Der Parameter muss vom
Experiment mit `parameter(...)` definiert sein; dessen Bereichsprüfung gilt
für jeden einzelnen Lauf. Beispiel:

```text
physim-batch <runner> <module> <neuer-ordner> speed 5 1 0.005 42 --sweep initialSpeed=1:3
```

`--param name=wert` setzt einen endlichen Wert für jeden Lauf; die Option ist
für unterschiedliche Parameternamen wiederholbar. Der variierte Parameter
darf nicht zusätzlich als fester Wert angegeben werden. Der Controller prüft
Namen, Doppelungen und Zahlen vor dem Anlegen des Ausgabeordners; das
Experiment prüft die deklarierten Grenzen.

Bei einem stochastischen Experiment ändern sich im Sweep auch die Seeds. Um
allein den Parametereffekt zu beurteilen, muss das Experiment die
Zufallsstreuung passend kontrollieren.

Alle Pfade müssen absolut sein. Der Elternordner der Ausgabe muss existieren.
Ctrl+C bricht geregelt ab (Exitcode 130); Fehler liefern 1, ungültige Argumente 2.
Der interne Controller ist noch kein öffentliches SDK-API.
Ohne `--workers` arbeitet die CLI mit einem Runner. N muss zwischen 1 und 8 liegen.
Bei weniger Läufen als N werden nur entsprechend viele Prozesse gestartet.

Grenzen: 1–1000 Läufe, 1–100000 Schritte, insgesamt
höchstens fünf Millionen Samples und 30 Sekunden Laufzeitlimit je Runner.
dt muss positiv und höchstens eine Sekunde sein; die App bietet mindestens 1 µs.
Kanalnamen bestehen aus ASCII-Buchstaben, Zahlen, Punkt, Bindestrich oder Unterstrich.
Einheiten und Endzeit müssen in allen Dateien übereinstimmen. Bei adaptiven
Schritten dürfen Anzahl und Zeitraster der akzeptierten Schritte je Lauf abweichen. Unvollständige
Rohdateien, Prozessfehler und nicht darstellbare numerische Ergebnisse stoppen die Serie.
Rohdaten können je nach Kanalzahl mehrere hundert MB belegen; es gibt noch keine
Speicherplatzvorprüfung. Fortsetzung und Aggregation fehlender Endwerte sind
weiter unten beschrieben. Die allgemeine Sensor-API ist in
[Messungsreferenz](reference/measurement.md) verfügbar.


## Gemeinsame Zielzeit und adaptive Serien

**Gemeinsame Endzeit** vergleicht den Kanalwert aller Läufe zum selben Zeitpunkt.
Die Endzeit kann zwischen zwei Rasterpunkten liegen; der Runner verkürzt dann
seinen letzten Schritt. **Schritte pro Lauf** wird in diesem Modus zum Budget
maximal akzeptierter Schritte. Reicht es nicht aus, stoppt die Serie mit Fehler;
Rohdaten bleiben als gültiger Präfix erhalten und es entsteht kein Gesamtbericht.
Ohne Zielzeitauswahl gilt weiterhin das feste Raster `Schritte × dt`.

**Adaptive Schritte** aktiviert die gemeinsame Endzeit automatisch. Das Modell
muss den adaptiven Callback anbieten. `dt` ist die Startdauer; Minimum und Maximum
begrenzen die späteren Schritte. Start, Ziel und Grenzen erlauben wissenschaftliche
Schreibweise. Es gilt `0 < Minimum ≤ dt ≤ Maximum ≤ 1 s` und `0 < Endzeit ≤ 1e9 s`.
Bei einem kürzeren Restintervall senkt der Host die dem Modell übergebene
Mindestdauer auf diesen Rest. Der letzte akzeptierte Schritt muss genau die
Zielzeit erreichen; Zwischenwerte werden nicht als Endwerte extrapoliert.
Eine nicht erreichbare Toleranz bleibt ein Fehler, auch im letzten Schritt.

Die C- und Physim-Pendelvorlagen bieten `length` (0,1–10 m) und `initialAngle`
(-1,5–1,5 rad). Die bisherigen Standards sind 1,5 m und 0,45 rad. Eine Studie über
die Länge kann beispielsweise drei Werte von 0,5 bis 2,5 m bis 0,7 s vergleichen.
Wähle Kanal `angle`, Startschritt 0,1 s, Minimum `1e-6`, Maximum 0,2 s und ein
Budget von 1000 Schritten. Verschiedene Pendellängen benötigen unterschiedlich
viele akzeptierte Schritte, erreichen aber dieselbe Endzeit. Ihre adaptiven
C-/Physim-Läufe sind in den ausgeführten Tests kanalweise verglichen.

Die CLI stellt dieselbe Steuerung bereit:

```text
physim-batch <runner> <pendulum-module> <neuer-ordner> angle 3 1000 0.1 42 --until 0.7 --adaptive --min-dt 1e-6 --max-dt 0.2 --sweep length=0.5:2.5 --workers 3
```

`--until` ohne `--adaptive` verwendet feste Schritte mit verkürztem Endintervall.
`--adaptive` in einer Serie verlangt eine positive `--until`-Zielzeit;
Mindest-/Höchstschrittoptionen verlangen den adaptiven Modus. Die Grenzen gelten
auch für die CLI, einschließlich Samplebudget und Seed-Überlauf. Die einzelne
Runner-CLI unterstützt `--until` nur im Offline-Modus; `--steps` ist dann das
Schrittbudget. Bestehende CLI-Aufrufe ohne Zielzeit behalten ihre Bedeutung.

Manifestversion 4 und jeder `.psrun`-Lauf speichern Zielzeit, Schrittmodus,
Startdauer und Budget, bei adaptiven Läufen auch die ursprünglichen Grenzen.
`terminal_step=clip_to_target` dokumentiert das Endintervall. Quellen, Modul,
Parameter und Seeds werden wie bisher eingefroren. Der Controller prüft CRC,
Footer, Start bei 0, strikt steigende Zeiten, Intervallgrenzen, passende Metadaten
und den exakten Zielzeitpunkt. Er akzeptiert nur vollständig geprüfte Endwerte.
Abbruch und Fehler beenden alle verbleibenden Prozesse und erhalten die bereits
geprüften Journal-/CSV-Einträge. Parallelität verändert bei deterministischem
Modell weder Messwerte noch Berichtsdaten.

Für die Analyse verschiedener adaptiver Läufe sind deren tatsächliche Zeitspalten
maßgeblich. Resampling kann gemeinsame Zwischenzeitpunkte herstellen; ein
Vergleich nur nach Schrittnummern ist bei unterschiedlichen Rastern nicht geeignet.
Die Physim-Analyse `analysis_batch_endpoints.phys` und C-Analysen lesen die
Rohdateien unabhängig von der Sprache des Modells.


Experimentparameter können jetzt eine eigene Anzeigeeinheit deklarieren. Die
Pendelvorlagen verwenden `m` für `length` und `rad` für `initialAngle`. Ohne
Deklaration bleibt die Einheit unbekannt; die X-Achse erhält dann keinen Suffix.
Kanalachsen behalten ihre registrierten SI-Einheiten.

## Einheiten von Parameterstudien

`ps_parameter_define_unit` in C und `parameterWithUnit` in Physim erklären Symbol,
Skala und sieben SI-Dimensionen. Standard, Grenzen, Rückgabewert und Runner-Override
sind **SI-Zahlen**. Die Anzeigeeinheit ändert weder das Modell noch dessen Ableitung.
Eine Zentimeterdeklaration mit Skala 0,01 und Standard 1,5 bedeutet deshalb
1,5 m; das Formular zeigt 150 cm. Nach einem kompatiblen Einheitenwechsel beim
Neubau bleibt eine gültige SI-Auswahl erhalten. Ein Wechsel der bekannten Dimension
setzt sie auf den neuen Standard zurück.

In der App sind Experimentparameter und Start-/Endwert einer Studie in der
angegebenen Anzeigeeinheit einzugeben. 50 bis 250 cm erzeugen dieselben Modellwerte
wie 0,5 bis 2,5 m. Der Controller gibt dem Runner SI-Overrides, prüft in jedem Lauf
identische vollständige Deklarationen des Studienparameters und erzeugt eine Kurve in der Anzeigeeinheit.
PNG und SVG verwenden die Beschriftung `length [cm]`; ein geladener Bericht enthält
Skala und Dimensionen, sodass `Kurven-X × Skala` wieder den SI-Parameterwert ergibt.

Projektdateien, `--param`, die CLI-Option `--sweep`, Rohdaten und `parameter_value`
in `completed.csv`/`endpoints.csv` verwenden bei deklarierten Einheiten weiterhin
SI. Eine CLI-Studie von 50 bis 250 cm lautet daher weiterhin
`--sweep length=0.5:2.5`. Das Serienmanifest ergänzt
`parameter_value_storage=SI` sowie die `parameter_unit`, `parameter_scale` und
`parameter_dimension`-Felder des untersuchten Parameters. Die bestehenden
Manifestversionen bleiben lesbar; die neuen Felder sind ergänzende Metadaten.

`parameter(...)` und `ps_parameter_define` bleiben für untypisierte Zahlen
verfügbar. Unbekannt ist kein Synonym für dimensionslos: Ein ausdrücklich
registriertes `PS_ONE` beziehungsweise eine `Unit` mit Symbol `1` zeigt `[1]`.
Beschreibungen werden nicht als Einheit interpretiert. Es gibt keine affine
Umrechnung; die Skala muss positiv und endlich sein. Nicht darstellbare Anzeigen,
unvollständige oder doppelte Einheitenfelder und widersprüchliche Deklarationen
führen zu Fehlern statt zu einer stillschweigenden Umrechnung.


Die Parameterparser erhalten auch darstellbare nichtnullige subnormale Zahlen
(zum Beispiel `1e-310`) in Formularen, Projektdateien, CLI und Metadaten. Die
Parser unterscheiden sie von Eingaben wie `1e-999`, die vollständig zu Null
unterlaufen. Anzeigen, die einen vorhandenen nichtnulligen SI-Wert zu Null
runden würden, werden abgewiesen; ein erfolgloser Neubau erhält die alte Auswahl.
Positive und negative Null bleiben bei der Eingabe unterscheidbar. Unveränderte
Standards, Grenzen und Auswahlen werden direkt aus ihren SI-Werten übernommen,
sodass etwa 0,29 m bei einer Zentimeteranzeige exakt 0,29 m bleibt.


## Unterbrochene Serien fortsetzen

**Archivierte Serie fortsetzen …** im Bereich **Laufserien** wählt den alten
Serienordner. Physim übernimmt dessen archiviertes Experiment und Einstellungen
und legt im aktuellen Projekt einen neuen Serienordner an. Der aktuelle
Experimenteditor und seine ungespeicherten Änderungen bleiben erhalten.
Abgeschlossene, journalisierte Läufe werden erneut geprüft und bytegleich
kopiert; nur fehlende Laufindizes starten neue Prozesse. Seeds bleiben `base+i`,
Parameterwerte und feste/adaptive Zeitvorgaben bleiben unverändert. Das Ergebnis
enthält sämtliche Endwerte in fester Indexreihenfolge. `reused` in `status.txt`
zeigt die Zahl übernommener Läufe, `started` nur neu gestartete Prozesse.

CLI, mit absoluten Pfaden:

```sh
physim-batch --resume /sdk/bin/physim-runner /project/runs/old-series /project/runs/new-series
```

Der neue Ausgabeordner darf noch nicht existieren. Auch eine bereits vollständige
Serie kann übernommen werden; dann starten keine neuen Runner. Die Fortsetzung
kann erneut abgebrochen und ihr neuer Ordner später weitergeführt werden.

Neue Serien speichern vor dem ersten Kindprozess `resume.bin`: versionierte
Konfiguration, CRC-Prüfsumme und Dateigröße/FNV-1a-Fingerprints von Runner,
archiviertem Modul und optionalem Quellsnapshot. Änderungen an diesen Dateien oder
an der gespeicherten Konfiguration verhindern die Wiederaufnahme. Ein verschobener
Runner ist erlaubt, wenn seine Bytes identisch sind. Serien aus früheren Versionen
ohne diesen Checkpoint lassen sich nicht mit dieser Funktion fortsetzen.
Die Fingerprints erkennen Änderungen; sie sind keine kryptografische Authentifizierung.

`completed.csv` ist die Grundlage: Index, Seed, Dateiname, Zeit, Parameterwert und
Endwert müssen zu Konfiguration und vollständiger Messdatei passen. Kanal- und
Parametereinheiten werden abgeglichen. Ein unvollständig geschriebener letzter
Journal-Eintrag wird übergangen. Andere beschädigte Einträge oder veränderte
Messdateien werden abgewiesen; die alte Serie bleibt unverändert. Ein vollständiger
Rohlauf ohne Journal-Eintrag wird neu gerechnet, weil sein erfolgreicher
Prozessabschluss nicht dokumentiert ist. Unterbrochene Rohläufe bleiben im
ursprünglichen Ordner erhalten.

Die neue Serie verwendet neue Arbeitsverzeichnisse. Wiederholbarkeit setzt wie
bisher voraus, dass das Modell nur den expliziten Seed als Zufallsquelle und die
archivierte Konfiguration nutzt; externe Dateien, Uhrzeit oder Prozesszustand
werden nicht wiederhergestellt. Journalisierte nicht fällige oder ausgefallene
Endmessungen werden mit ihrem Status übernommen und nicht erneut gemessen.
Ältere Journale ohne Statusspalte werden gelesen; ihre Einträge müssen mit
gültigen Endmessungen übereinstimmen.


## Fehlende Endwerte

Kanäle ohne zugehörigen `.status` liefern reguläre numerische Endwerte. Bei
Messkanälen bezeichnet Status 0 eine zu diesem Zeitpunkt nicht fällige Messung,
Status 1 einen gültigen Wert und Status 2 einen Sensorausfall. Der Controller
prüft weiterhin die vollständige Messdatei, Zeitachse und Einheit. Unbekannte
Statuswerte, beschädigte Dateien, Prozessfehler und Zeitlimits bleiben Fehler.

Ein erfolgreich beendeter Lauf zählt auch mit fehlendem Endwert als abgeschlossen.
`completed.csv` und `endpoints.csv` enthalten jede geprüfte Laufnummer samt
Seed, Zeitpunkt und `status`. Bei Status 0 oder 2 bleibt `value` leer. Ein gültiger
Wert von null bleibt dagegen `0` mit Status 1. Fehlende Endwerte werden weder
auf null gesetzt noch durch einen früheren Wert oder eine Interpolation ersetzt.
Bei Abbruch fehlen nicht fertig geprüfte Laufindizes weiterhin ganz.

Histogramm, Mittelwert, Streuung und Quantile verwenden ausschließlich gültige
Endwerte in fester Laufindexreihenfolge. Die Mindestzahl 200 für das angenäherte
Mittelwert-KI bezieht sich ebenfalls auf gültige Endwerte. Sobald Werte fehlen,
zeigt eine zusätzliche Tabelle die Anzahl und den Anteil gültiger, nicht fälliger
und ausgefallener Endmessungen. Ohne gültige Endwerte besteht der Bericht nur
aus dieser Messabdeckung, ohne Mittelwert, Streuung oder Histogramm.

Diese Statistik beschreibt die tatsächlich gemessene Teilmenge. Selektive,
vom Signal abhängige Ausfälle können sie verzerren. Ein Konfidenzintervall für
die Gesamtheit erfordert zusätzlich von den Endwerten unabhängige Ausfälle;
eine Korrektur für informative Ausfälle wird nicht automatisch vorgenommen.

Parameterstudien mit fehlenden Endwerten zeigen gültige Punkte als Scatterplot.
Es gibt keine Verbindungslinien über Messlücken und keine erfundenen Werte an
den fehlenden Parameterpunkten. Sind alle Endwerte gültig, bleibt die bisherige
Linienkurve erhalten. Das CSV bewahrt die vollständigen Parameterpositionen
und ihre Statuswerte, auch wenn der Plot nur gültige Punkte enthält.
