# Profilaufzeichnung tatsächlicher App-Abläufe

Die App kann ihre eigene Framearbeit aufzeichnen. Die Aufzeichnung ist standardmäßig
ausgeschaltet und verändert keine Projekt-, Simulations- oder Anzeigeeinstellungen.
Zum Start im Repository beziehungsweise gebauten Paket:

```sh
PHYSIM_PROFILE_DIR=/pfad/zu/neuem-profil ./bin/physim
```

Das Elternverzeichnis muss vorhanden sein; der Profilordner darf noch nicht
existieren. In PowerShell `$env:PHYSIM_PROFILE_DIR = 'C:\neues-profil'` setzen und
`bin/physim.exe` starten. Nach der Messung die Variable aus der Shell entfernen.
Der Parameter gehört zum gestarteten Prozess, nicht zu gespeicherten Einstellungen.
Normales Öffnen eines Projekts und sämtliche Bedienaktionen bleiben verfügbar.

## Aufzeichnung und Fehlerverhalten

Ein eigener SDL-Thread erstellt den Ausgabeordner exklusiv und schreibt
`frames.csv` und unter Schema 2 `processes.csv`. Der UI-Thread stellt ausschließlich Kopien numerischer Messwerte
in eine Queue mit maximal 1.024 Frames. Ein Produzent und ein Consumer verwenden
SDL-Atomics mit Speicherbarrieren; im UI-Pfad gibt es keine Mutex-/Dateiwartezeit.
Der Writer kopiert höchstens 32 Frames pro Batch und gibt Slots erst danach frei.
Die Queue wächst nicht; bei voller Queue oder fehlender Ressourcenauskunft wird
der ausgelassene Frame gezählt. Dateien wachsen mit der Dauer der Aufzeichnung.

Beim normalen Beenden werden zuerst die bisherigen Projekt-/Workspace-/Einstellungs-
Speicherabläufe und Jobabschlüsse ausgeführt. Danach leert und schließt der Writer
die Profilqueue. Er erstellt `status.json` mit geschriebenen/ausgelassenen Frames,
Vollständigkeitsflag und Messumfang. Frame- und Prozessrecords besitzen getrennte
Geschrieben-/Ausgelassen-Zähler; das gemeinsame Flag verlangt beide vollständigen
Recordfolgen. Ein Crash, Abbruch oder Schreibfehler kann
eine unvollständige CSV ohne Abschlussstatus hinterlassen. Die App meldet
Profilfehler über stderr und setzt ihre normale Arbeit fort. Bestehende
Ausgabeordner und Dateien werden nicht überschrieben.

`complete=true` bezieht sich auf die aufgezeichneten Frames, nicht auf eine
erfolgreiche Simulation, erfolgreiche Projektspeicherung oder wissenschaftliche
Produktabnahme. Die Profilaufzeichnung ist Diagnosematerial, kein `.psrun`-Archiv.
Die App erzeugt dabei keine GPU-Zeitabfragen und wartet nicht auf GPU-Messergebnisse.

## Gemessene Größen

| Feldgruppe | Bedeutung |
| --- | --- |
| frame/time_seconds | Frameindex und Zeit seit Eintritt in `main`; ohne OS-/Bibliotheksladen vor `main` |
| startup_ready_seconds | Zeit bis nach dem ersten abgeschlossenen Hauptfenster-Swap und dem zugehörigen Handbuch-Renderaufruf; derselbe Wert in allen Zeilen |
| interval_seconds | Abstand zum vorherigen Messende, einschließlich vorheriger Messinstrumentierung und Schleifenpause |
| frame_seconds | Aktive Iteration vor Ressourcenauskunft/Queuepublikation; schließt die abschließende Schleifenpause aus |
| event_seconds | Eingabebeginn, SDL-Ereignisse und Eingabeverarbeitung |
| work_seconds | Job-/Runner-Pump, bestehende Autosave-/Dokumentarbeit und Teststeuerung bei einem Selbsttest |
| ui_seconds | UI-Aufbau und darin ausgeführte Callbacks, einschließlich Szenenaufbau |
| render_seconds/present_seconds | Hauptfenster-UI-Submission und Swap, getrennt |
| documentation_seconds | Handbuch-Renderaufruf, gegebenenfalls eigener Swap und Handbuch-Testaufnahme |
| capture_seconds/captured | Hauptfenster-Testaufnahme und Vorbereitung; captured markiert auch Handbuch-Testaufnahmen |
| user_cpu_seconds/system_cpu_seconds | Kumulative Prozesszeiten aller Appthreads, einschließlich Profilwriter, ohne Kindprozesse |
| peak_resident_bytes | Prozess-Lebenszeithöchstwert nach derselben OS-Abrechnung wie der [Datenbenchmark](performance.md#prozessressourcen-und-logischer-datendurchsatz) |
| runner_bytes/job_bytes | Tatsächlich in dieser Iteration aus Experimentpipe beziehungsweise aktiver Jobpipe gelesene Bytes |
| has_scene/scene_* | Letzter erfolgreicher Szenenaufruf dieser Iteration; ohne Aufruf sind Szenenwerte null und has_scene=false |
| rendered/ui_* | Erfolgreiche Hauptfenster-UI-Submission und deren Konvertierung/Upload/Geometriebytes; bei Fehler keine Übernahme alter UI-Statistik |

Der Pipeumfang enthält Framing, Metadaten und sonstige empfangene Nachrichten;
Jobbytes können Compiler-/Analyseausgabe enthalten. Dies misst die beobachtete
Leserate der App, keine physische Datei-/Netzrate oder produzierte Samples/s.
Batchworker-Pipes, ausgehende Steuernachrichten und direkt geschriebene Messdateien
sind nicht enthalten. Bei ausgelassenen Frames fehlen deren Bytewerte ausdrücklich.
Runner-/Compiler-/Analysewerte gehören nicht zu den App-Prozesssnapshots.
Sie werden nach beobachtetem Prozessabschluss separat erfasst; siehe unten.

Szenenzeiten sind Teil der UI-Aufbauzeit; UI-Konvertierungs-/Uploadzeiten sind
Teil der Renderzeit. Sie dürfen nicht zusätzlich zur Frameaufteilung summiert
werden. GPU-Arbeit kann noch ausstehen. Der [Szenenbenchmark](scene-rendering.md)
misst das optionale OpenGL-Serverintervall separat.

Die vorhandenen Dateizugriffe innerhalb von App-Callbacks bleiben Bestandteil
der gemessenen Arbeit. Diese Aufzeichnung behauptet nicht, die gesamte App auf
asynchrone I/O umgestellt zu haben. Die Instrumentierung, zusätzliche Threadarbeit
und Ausgabe beeinflussen CPU/RAM/Timing; es handelt sich um Beobachtungen unter
aktivierter Aufzeichnung. Die Peakwerte sind keine phasenlokalen Speicherbudgets.

## Auswertung

Im Repository mit Python 3.10+ und Standardbibliothek:

```sh
python3 tools/app_profile.py /pfad/zum/profil --output neuer-bericht.json
```

Die Auswertung liest die Artefakte und legt ausschließlich eine neue Berichtdatei
an. Sie prüft Schema, Abschluss-/Zeilenzahl, Framefolge, endliche nichtnegative
Werte, monotone Prozesswerte, Frameintervalle und fehlende/veraltete Szenenwerte.
Eine Aufzeichnung mit ausgelassenen Frames wird standardmäßig abgewiesen;
`--allow-partial` erlaubt eine Auswertung mit erhaltenem Unvollständigkeitsflag.
Eine CSV ohne Abschlussstatus wird nicht als abgeschlossene Messung ausgegeben.

Der Bericht enthält Median/P95/P99/Maximum für aktive Frames und ihre Phasen,
Startbereitschaft, beobachtete CPU-Differenzen, Peak-RAM, empfangene Bytes und
Leseraten. Nearest-Rank bestimmt die Perzentile. Die Zeitstatistik verwendet nur
erfolgreich gerenderte Frames ohne markierte Testaufnahmen; die Aufnahme-/Fehler-
anzahl bleibt sichtbar. CPU-Differenzen reichen vom ersten bis letzten erhaltenen
Snapshot und schließen daher den vorherigen Startaufwand aus. Rohdaten-/Status-
SHA-256 identifizieren die unveränderten Eingaben.

Live-/gleichzeitige Prozessbaum- und vollständige Mehrworkerressourcen, echte Eingabe-bis-Anzeige-Latenz,
GPU-Auslastung und OS-Prozessstart vor `main` benötigen weitere Messstrecken.
[Ausgeführte Prüfungen und Grenzen](platform-validation.md).


## Ressourcen abgeschlossener eigener Prozesse

Schema 2 ergänzt `processes.csv`. Jedes beobachtete Ende eines von der App
verwalteten Experiments oder Jobs ergibt genau einen Record, auch bei Fehler,
Timeout, erzwungenem Stop oder beim Schließen noch laufender Prozesse. Die
Reihenfolge ist unabhängig von Frameindizes. PID, beobachtete Abschlusszeit,
Jobart, App-Exitcode, Timeout und verfügbare Ressourcen stehen im Record.
Ein monotoner Sequenzindex unterscheidet Records auch bei späterer PID-Wiederverwendung.
Die App setzt die Lebenszyklusmarkierung bei jedem erfolgreichen Start zurück.

Jobart 0 bezeichnet das Experiment, 2 den Builder, 5 die Parameterbeschreibung,
3 Analyse und 4 CSV-Export. CPU-Zeiten sind kumulierte Benutzer-/Systemzeiten,
Peakbytes ein OS-Höchstwert. `usage_available=false` lässt alle drei Nutzungsfelder
in der CSV leer; JSON verwendet null. Nicht verfügbare Werte sind keine gemessenen
Nullwerte. Im Bericht bleiben Art und Anzahl fehlender Ressourcenauskünfte sichtbar.

Der private Plattformdienst erfasst Werte beim Reaping beziehungsweise vor dem
Schließen des Prozesshandles. Er fragt keine später möglicherweise wiederverwendete
PID ab und verwendet nicht das kumulative `RUSAGE_CHILDREN` des App-Prozesses.
Der Cache bleibt nach Poll/Kill/Close gültig und wird beim nächsten Start oder
fehlgeschlagenen Start invalidiert; fehlgeschlagene Getter ändern keine Ausgaben.

`usage_scope=2` bezeichnet die [POSIX-wait4-Abrechnung](https://man7.org/linux/man-pages/man2/wait4.2.html).
Diese kann abgewartete Nachfahren enthalten. Der Peak ist kein gleichzeitig
aufsummierter Prozessbaum-RAM; die [Linux-Abrechnung](https://man7.org/linux/man-pages/man2/getrusage.2.html)
verwendet Höchstwerte und rechnet KiB ausdrücklich in Bytes um. Fork-/Startaufwand
und OS-Regeln können die Werte beeinflussen. macOS liefert Bytewerte.
`usage_scope=1` bezeichnet den direkten Windows-Prozess mit Prozesszeiten und
Working-Set-Abrechnung; dieser Pfad wurde in dieser Etappe nicht ausgeführt.
Scope 0 bedeutet fehlende Zuordnung. Die Plattformen werden nicht durch eine
bloße gemeinsame Feldbezeichnung als funktionsgleich erklärt.

Die Reporterfassung prüft Prozesszeilenzahl, Sequenzfolge, endliche Werte, Zeitfolge,
Scope und konsistente Nichtverfügbarkeit. Ausgelassene Prozessrecords machen
auch bei lückenlosen Frames die Aufzeichnung unvollständig. Schema 1 bleibt ohne
Prozess-CSV lesbar und erzeugt keine erfundenen Kindprozesswerte.
Die abgeschlossenen Werte sind keine Live-Auslastungsanzeige, kein gleichzeitig
beobachteter Gesamtbaum und keine vollständige Aufzeichnung intern verwalteter
Batchworker. GPU-Auslastung und Eingabe-bis-Anzeige-Latenz bleiben eigene Messstrecken.
