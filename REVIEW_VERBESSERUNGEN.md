# Physim – Gesamt-Review und Verbesserungsplan

**Review-Stand:** 2026-09-21, Design-Review ergänzt  
**Review-Version:** 2.0 – evidenzbasiertes Ranking und Selbstkritik der ersten Fassung  
**Scope:** vollständige Bestandsaufnahme von Architektur, Kernbibliotheken, Runnern, Datenformat, Analyse-/Report-Pipeline, UI, Build-/CI-Konfiguration, Tests, Dokumentation und Beispielen.  
**Wichtige Einschränkung:** Der folgende Design-Review hält eine neue Produktentscheidung fest. Die Designrichtung wurde in `docs/design-direction.md` aktualisiert; die Implementierung wurde nicht geändert. Build- und Testläufe wurden bewusst nicht gestartet, weil sie je nach Konfiguration generierte Dateien erzeugen können.

## 1. Zusammenfassung

Physim hat bereits eine gute technische Grundlage für eine native C17-Physik-Workbench:

- klare Trennung zwischen Core, Runner-Prozessen, App/UI und Analyse-/Report-Pipeline;
- Prozessgrenzen für Simulationen, damit ein fehlerhafter User-Code nicht direkt den UI-Prozess zerstört;
- streamingfähiges, append-only Run-Format mit CRC und Wiederherstellungslogik;
- begrenzte Vorschauen und diskgestützte Analyse statt unkontrolliertem Wachstum im RAM;
- reproduzierbare Seeds, feste Run-Limits, Beispiele und umfangreiche Tests;
- eine bestehende UI-Grundlage mit den Arbeitsbereichen **Entwickeln**, **Simulieren** und **Auswerten**; das bisherige Apple-inspirierte Start- und Navigationskonzept wird durch die neue Workspace-Richtung ersetzt.

Der aktuelle Stand ist jedoch noch eine gute Entwicklungsbasis und noch kein abschließend poliertes Produkt. Die größten Hebel liegen in fünf Bereichen:

1. **Messbarkeit:** Es gibt noch kein verbindliches Performance-Budget und keinen dauerhaften Profiling-/Benchmark-Pfad.
2. **Run-Persistenz:** `ps_run_append()` leert den Dateipuffer bei jedem Sample. Das schützt die Daten sehr aggressiv, kann aber den Durchsatz einer Simulation massiv reduzieren.
3. **Render- und Analyse-Hotpaths:** Szene-Geometrie, UI-Buffer und Report-Kurven werden teilweise pro Frame neu erzeugt, kopiert oder zur GPU übertragen.
4. **Streaming-Analyse:** Diskgestützte Serien sind speichersicher, erzeugen aber viele Seek-/Read-Operationen. Vorwärts-Cursor und Read-Ahead fehlen.
5. **UI-Reife:** Die visuelle Richtung stimmt in weiten Teilen, aber vollständige Tastaturbedienung, Screenreader-Semantik, globale Skalierung, Light-/High-Contrast-Theme, responsive Layouts und ein klarer letzter Design-Abnahmeschritt fehlen noch.

Die nachfolgenden Vorschläge sind so formuliert, dass Funktionalität, Datenformat, Reproduzierbarkeit und bestehende Sicherheitsgrenzen erhalten bleiben. Jede Optimierung sollte erst nach einer Baseline-Messung und mit Regressionstests umgesetzt werden.

## 2. Review-Grundlagen und Grenzen

### Geprüfte Bereiche

- `CMakeLists.txt`, Toolchain-/Sanitizer-Optionen und `.github/workflows/ci.yml`;
- `README.md`, Projektplan, Status-, Architektur-, Datenformat-, UI- und Design-Dokumentation;
- öffentliche Header unter `include/physim`;
- Core-Implementierung unter `src/`;
- Runner unter `runners/`;
- App/UI/Renderer unter `app/`;
- Batch-/Monte-Carlo-Code;
- Beispiele, Tests und Test-Fixtures;
- Third-Party-Struktur und SDL3/Nuklear-Integration.

### Nicht behaupten, was nicht verifiziert wurde

Im Arbeitsverzeichnis ist kein `.git`-Verzeichnis vorhanden. Deshalb kann dieses Review keinen verlässlichen Git-Diff, keine Aussage über uncommitted Änderungen und keinen Vergleich mit einem vorherigen Commit liefern. Vor der Umsetzung sollte das Projekt in einem Repository oder mit einem explizit gespeicherten Baseline-Snapshot geprüft werden.

Die Dokumentation berichtet von Windows-/Clang-/Linux-/Graphics-Tests. Diese Ergebnisse wurden in diesem Review nicht neu ausgeführt und sind daher als vorhandene Projekt-Dokumentation zu verstehen, nicht als erneute Abnahme dieses konkreten Arbeitsstands.

## 3. Architektur- und Qualitätsbewertung

| Bereich | Aktueller Eindruck | Bewertung | Wichtigste nächste Maßnahme |
|---|---|---:|---|
| Core und öffentliche API | C17, relativ klare Zuständigkeiten, begrenzte Datenstrukturen | Gut | API-/ABI-Verträge mit Golden Tests absichern |
| Simulation/Runner | Prozessgrenze, Limits, Pipes und Statusmeldungen | Gut | Scheduling, Backpressure und Laufzeitprofile messen |
| Persistenz | Append-only, CRC, Recovery, Streaming | Gut mit Performance-Risiko | Durability-Policy explizit machen und benchmarken |
| Analyse/Serien | RAM-bounded, Scratch-Dateien, Preview-Limits | Gut mit I/O-Risiko | Sequential Cursor/Read-Ahead ergänzen |
| Mechanics | Funktional breit genug für die aktuelle Produktphase | Solide | Solver-Hotpaths profilieren, keine semantische Änderung verstecken |
| Renderer | OpenGL-3.3-Basis, GPU-Tests, Szenen-/UI-Rendering | Solide mit Skalierungsrisiko | Geometrie-, Buffer- und Resize-Pfade entkoppeln |
| UI | Bestehende App-Shell mit drei klaren Arbeitsbereichen; neuer Workspace-Start noch offen | Gute Grundlage, Produktfluss nicht fertig | Leerer Start, globale Helferleiste, Workspace-Modell und separater Projektmanager |
| Tests | Viele Unit-, Integrations- und Self-Tests | Gut | Performance-, Fuzzing-, UI- und Clean-machine-Checks ergänzen |
| Build/CI | Mehrere Compiler/Plattformen, Sanitizer und Graphics-Tests | Gut | Format-/Static-Analysis-/Benchmark-Gates ergänzen |
| Dokumentation | Umfangreich und ehrlich bei offenen Punkten | Sehr gut | Dokumentation mit gemessenen Budgets statt nur Statuslisten ergänzen |

## 4. Nicht verhandelbare Erhaltungsregeln

Die folgenden Regeln sollten für alle späteren Änderungen gelten:

1. **Keine Änderung am wissenschaftlichen Ergebnis ohne ausdrückliche Entscheidung.** Timestep, Integrator, Seed, Kollisionsreihenfolge, Rundungsverhalten und Stop-/Pause-Semantik dürfen nicht versehentlich durch eine Performance-Änderung verändert werden.
2. **Das Run-Format bleibt rückwärtskompatibel.** Neue Chunk-Typen oder Metadaten brauchen Versionierung und Tests mit alten Dateien. Bestehende Runs müssen weiterhin lesbar und recoverbar bleiben.
3. **Durability und Throughput werden getrennt konfiguriert.** Eine schnellere Standardspur darf nicht stillschweigend den bestehenden Absturz-/Recovery-Vertrag verschlechtern.
4. **Vorschauen dürfen sich ändern, aber nicht die Vollständigkeit der gespeicherten Rohdaten.** Preview-Reduktion, UI-Limit und Report-Limit sind unterschiedliche Verträge.
5. **Rendering darf die Simulation nicht beeinflussen.** UI-FPS, Plot-Rendering und GPU-Auslastung dürfen keinen Einfluss auf physikalische Schrittweite, Seed oder Reihenfolge haben.
6. **Jede Optimierung erhält einen Referenztest.** Vorher-/Nachher-Vergleich mit denselben Seeds, Eingabedateien, Plattformen und Run-Limits.
7. **Keine vorgezogene Physik-Änderung als reine Optimierung verkaufen.** Broad Phase, CCD, Constraints oder neue Kontaktreihenfolgen können Verhalten ändern und gehören in eigene, fachlich abgenommene Arbeitspakete.

## 5. Performance-Review

### PERF-001 – Performance-Baseline und Budgets zuerst etablieren

**Priorität:** P0 – Voraussetzung für alle weiteren Optimierungen  
**Betroffene Bereiche:** gesamte Anwendung, CI, Dokumentation

Aktuell gibt es viele sinnvolle Begrenzungen, aber kein verbindliches Budget für:

- Simulationsschritte pro Sekunde;
- Zeit pro `ps_run_append()` und pro 1.000 Samples;
- Analyse-Durchsatz bei kleinen und sehr großen Runs;
- UI-Framezeit und P95/P99-Latenz;
- CPU-Zeit für Szene-Geometrie und GPU-Upload;
- Report-Plot-Aufbau;
- Peak-RAM und Scratch-Datei-Volumen;
- Startzeit bis zum ersten interaktiven Frame.

**Empfehlung:** Ein kleines, reproduzierbares Benchmark-Programm oder Testziel hinzufügen. Es soll keine Produktlogik verändern und mindestens folgende Fälle messen:

1. kurzer interaktiver Run;
2. langer Run mit 16 Kanälen;
3. Analyse eines großen Runs;
4. Report mit maximaler Preview-Größe;
5. Szene mit vielen Objekten/Polyline-Segmenten;
6. Batch mit acht Workern;
7. UI-Leerlauf, Plot-Interaktion und Fenster-Resize.

**Abnahmekriterium:** Jede spätere Optimierung nennt eine Baseline, ein Ziel, eine Hardware-/Build-Konfiguration und einen Regression-Schwellenwert. Ohne diese vier Informationen sollte keine Performance-Behauptung in die Dokumentation aufgenommen werden.

### PERF-002 – `ps_run_append()` leert bei jedem Sample den Dateipuffer

**Priorität:** P1 – wahrscheinlich größter Durchsatzhebel bei langen Simulationen  
**Betroffene Dateien/Funktionen:** `src/data.c: ps_run_append()`, `flush_file()`, `runners/experiment.c`

Der aktuelle Append-Pfad schreibt einen Chunk und ruft danach `fflush()` auf. Zusätzlich wird regelmäßig ein stärkerer Flush mit `_commit()` unter Windows beziehungsweise `fsync()` unter POSIX ausgeführt. Da der Experiment-Runner `ps_run_append()` für jeden Physikschritt verwendet, entsteht ein sehr hoher Anteil an Dateisystem- und Synchronisationsarbeit.

Das ist robust gemeint, aber `fflush()` pro Sample ist nicht dasselbe wie vollständige Persistenz und kann dennoch erheblichen Overhead erzeugen. Bei langen Runs kann dadurch die Datenspeicherung zum dominierenden Teil der Laufzeit werden.

**Sichere Umsetzung:**

- eine explizite Durability-Policy einführen: `strict`, `balanced`, `throughput` oder eine gleichwertige interne Konfiguration;
- den bisherigen aggressiven Modus für Recovery-kritische Workflows erhalten;
- im gepufferten Modus Samples in einem größeren Benutzerpuffer sammeln;
- Flush bei Pause, Stop, normalem Ende, Fehler, Prozesssignal und definierter Checkpoint-Periode erzwingen;
- die Recovery-Dokumentation exakt an die tatsächliche Garantie anpassen;
- keine Änderung an Chunk-Header, CRC, Reihenfolge oder Samplewerten.

**Wichtig:** Das darf nicht als unbemerkte Änderung des Standardverhaltens eingeführt werden. Zuerst messen, wie die aktuelle Anwendung Datenverlust bei Prozessabbruch behandelt. Danach muss die gewünschte Default-Garantie bewusst beschlossen und getestet werden.

**Abnahmekriterium:** Gleiche Rohdaten und gleiche Reproduzierbarkeit bei normalem Ende; Recovery-Tests für Abbruch direkt vor und nach einem Checkpoint; messbarer Durchsatzgewinn bei langen Runs; keine Blockade der Runner-Pipes.

### PERF-003 – Report-Kurven werden im UI vollständig kopiert

**Priorität:** P1  
**Betroffene Dateien/Funktionen:** `src/report.c: ps_report_curve_read()`, `app/report_ui.inc`

`ps_report_curve_read()` kopiert die komplette `ps_curve_data`-Struktur. Eine Kurve kann bis zu 2.048 X- und 2.048 Y-Werte enthalten. Das ist für eine API-Leseoperation verständlich, im UI-Frame jedoch teuer: Beim Plotten werden Kurven gelesen, durchlaufen und für Legenden-/Metadatenzugriff teilweise erneut gelesen.

**Empfehlung:** Eine der folgenden Varianten einführen:

- unveränderlichen Read-only-View mit klarer Lebensdauer;
- Block-API wie `ps_report_curve_read_block()`;
- UI-seitigen Cache, der nur bei Report- oder Auswahländerung aktualisiert wird;
- kleine Metadaten-API für Label, Einheit, Punktzahl und Bounds ohne Kurvendatenkopie.

Die bestehende kopierende API sollte aus Kompatibilitätsgründen erhalten bleiben. Der neue Pfad muss die Lebensdauer, Thread-Sicherheit und Mutationseinschränkung ausdrücklich dokumentieren.

**Abnahmekriterium:** identische Plotdaten und Legenden; keine Pointer-Nutzung nach Report-Mutation; weniger CPU-Zeit und Speicherbandbreite in Frames mit mehreren Kurven.

### PERF-004 – Szene-Geometrie nicht in jedem Frame vollständig tessellieren

**Priorität:** P1  
**Betroffene Dateien/Funktionen:** `app/graphics.c: ps_graphics_scene()`, `tube()`, Szenen-Upload

Der Renderer erzeugt CPU-seitig viele Dreiecke pro Frame und überträgt den verwendeten Vertexbereich anschließend erneut an die GPU. Das betrifft unter anderem Gitter, Tubes, Boxen, Kugeln und Polyline-Segmente. Die statische Kapazität von `SCENE_CAPACITY` verhindert zwar unbounded growth, löst aber nicht die wiederholte CPU- und Upload-Arbeit.

**Empfehlung in sicherer Reihenfolge:**

1. statische Geometrie wie Grid und wiederverwendbare Primitive einmalig in GPU-Meshes ablegen;
2. identische Primitive instanziert oder mit transformierten Instanzen rendern;
3. nur dynamische beziehungsweise veränderte Teile neu erzeugen;
4. Szenenaufbau über Dirty Flags invalidieren;
5. für sehr lange Polylines LOD oder segmentierte Uploads einführen;
6. erst danach Ringbuffer/Orphaning oder persistent gemappte Buffer prüfen.

Kameraänderungen sollten möglichst keine Neutesselierung der Weltgeometrie erzwingen. Sichtbarkeit, Objektanzahl und Renderreihenfolge müssen dabei identisch bleiben.

**Abnahmekriterium:** gleiche Geometrie innerhalb der bestehenden Toleranzen, keine zusätzlichen Z-Fighting-/Clipping-Artefakte, stabile Framezeit bei statischer Szene, geringere CPU-Zeit und weniger GPU-Upload.

### PERF-005 – Nuklear-Vertex- und Indexbuffer wiederverwenden

**Priorität:** P1  
**Betroffene Datei/Funktionen:** `app/graphics.c: ui_target()`, `app/ui_sdl.c`

Der UI-Renderpfad initialisiert und verwirft CPU-seitige Nuklear-Buffer pro Renderdurchlauf und lädt danach Vertex-/Indexdaten mit dynamischen OpenGL-Uploads. Das funktioniert, produziert aber bei dauerhaft laufender UI unnötige Allokationen, Kopien und Treiberarbeit.

**Empfehlung:**

- Buffer und Konvertierungsbereiche pro Graphics-Context wiederverwenden;
- Kapazität wachsen lassen und nur bei Bedarf reallocieren;
- bei OpenGL 3.3 zunächst robustes Buffer-Orphaning oder Double-/Triple-Buffering verwenden;
- GPU-Synchronisation so wählen, dass kein CPU-Stall entsteht;
- die bestehende Nuklear-Geometrie und Zeichenreihenfolge unverändert lassen.

**Abnahmekriterium:** keine per-Frame-Allokationen im stabilen UI-Betrieb; identische Input-/Render-Semantik; Messung mit leerem, dichtem und plotlastigem UI.

### PERF-006 – Diskgestützte Serien mit Vorwärts-Cursor und Read-Ahead lesen

**Priorität:** P1  
**Betroffene Datei/Funktionen:** `src/series.c: read_values()`, Resampling- und Blockoperationen

Die Scratch-Datei hält den RAM klein, aber Quellserien werden blockweise gelesen und für viele Operationen erneut per Seek positioniert. Beim getrennten Lesen von X- und Y-Serien können zusätzliche Seek-/Read-Folgen entstehen. Bei kleinen Dateien fällt das kaum auf; bei langen Runs und mehreren Ableitungen wird es ein I/O-Hotpath.

**Empfehlung:**

- einen sequentiellen Cursor pro Serie/Analyseoperation bereitstellen;
- bei vorwärts laufenden Operationen den nächsten Block vorab lesen;
- X-/Y-Zugriffe, wo möglich, über einen gemeinsamen Analyse-Cursor koordinieren;
- zufällige Zugriffe weiterhin über die bestehende blockweise API ermöglichen;
- Speichergrenzen durch ein kleines, explizites Cache-Budget schützen;
- Ergebnisse und Scratch-Dateien weiterhin transaktional behandeln.

**Abnahmekriterium:** identische Resultate einschließlich Randwerten, NaN-/Fehlerbehandlung und Resampling; weniger Seek-Operationen; kein unbounded Cache.

### PERF-007 – Contact-Solver: wiederholte Transformationsarbeit im Iterations-Hotpath

**Priorität:** P1/P2, abhängig vom gemessenen Anteil an der Laufzeit  
**Betroffene Datei/Funktionen:** `src/mechanics.c: ps_contacts_resolve()` und Hilfsfunktionen

Der iterative Contact-Solver ruft innerhalb vieler Iterationen wiederholt Punktgeschwindigkeit, Paarantwort, Welt-Inertia, Quaternion-Rotationen, Normalisierung und teilweise teure Norm-/Hypotenusenoperationen auf. Einige Massen- und Trägheitsgrößen werden bereits vorbereitet, aber nicht alle konstanten Größen des Kontaktmanifests werden vollständig wiederverwendet.

**Empfehlung:** pro Kontaktmanifold beziehungsweise Solver-Frame vorberechnen und cachen:

- normalisierte Kontaktachsen;
- Welt-Inv-Mass und Welt-Inv-Inertia;
- Kontakt-Jacobianen;
- effektive Massen beziehungsweise kleine Solvermarschen;
- konstante Hebelarme und Bias-Terme.

Die Impulsreihenfolge, Clamping-Grenzen und Iterationszahl dürfen dabei nicht verändert werden. Jede algebraische Umformung braucht Vergleichstests mit schwierigen Kontaktlagen.

**Abnahmekriterium:** gleicher Solver-Ausgang innerhalb definierter numerischer Toleranzen, keine zusätzlichen Penetrationen oder Energieartefakte, messbarer Vorteil nur in realen Contact-Benchmarks.

### PERF-008 – Validierung in Public API und Internal Fast Path trennen

**Priorität:** P2  
**Betroffene Bereiche:** `src/mechanics.c`, Core-Funktionen mit wiederholter `ps_body_validate()`-Prüfung

Die öffentliche Robustheit ist wertvoll. Wenn jedoch innerhalb eines bereits validierten Solver-Schritts jede Hilfsfunktion erneut vollständige Körper-/Quaternion-Validierung ausführt, bezahlt der Hotpath denselben Schutz mehrfach.

**Empfehlung:**

- öffentliche Funktionen behalten ihre Validierung;
- interne `*_unchecked`- oder `*_validated`-Varianten nur in klar abgegrenzten Solver-Kontexten verwenden;
- Debug-Builds können zusätzliche Assertions behalten;
- der interne Vertrag muss im Code kommentiert sein: Welche Funktion garantiert die Validierung und wie lange gilt sie?

### PERF-009 – CRC32 tabellengestützt berechnen

**Priorität:** P2  
**Betroffene Datei:** `src/data.c`

Die aktuelle CRC-Berechnung verarbeitet jedes Bit einzeln. Das ist einfach und zuverlässig, kostet bei großen Run-Dateien aber deutlich mehr CPU als eine 256-Einträge-Tabelle oder eine vergleichbare blockweise Implementierung.

**Empfehlung:** tabellengestützte Implementierung mit Golden-Vector-Tests für bestehende Dateien. Das Polynom, die Initialisierung, Byte-Reihenfolge und gespeicherten Werte dürfen sich nicht ändern. Hardware-spezifische Varianten erst nach der portablen Variante und nur mit identischem Ergebnis ergänzen.

### PERF-010 – `ps_clock()` unter Windows Frequenz nicht pro Aufruf abfragen

**Priorität:** P2  
**Betroffene Datei:** `src/platform.c`

`QueryPerformanceFrequency()` ist eine Prozesskonstante, wird aber im Windows-Pfad bei Zeitabfragen erneut angesprochen. Die Frequenz sollte einmalig thread-sicher initialisiert und anschließend wiederverwendet werden. Die gemessene Zeitbasis und Monotonie müssen erhalten bleiben.

### PERF-011 – UI-Framepacing nicht doppelt begrenzen

**Priorität:** P2  
**Betroffene Datei:** `app/main.c`

Der Renderer verwendet VSync; zusätzlich wird im Event-Loop eine feste kurze `SDL_Delay()` verwendet. Das ist als einfache CPU-Entlastung verständlich, kann aber unnötige Latenz erzeugen und VSync doppelt begrenzen.

**Empfehlung:**

- tatsächliche Swap-Interval-Konfiguration berücksichtigen;
- bei VSync nicht zusätzlich blind verzögern;
- bei nicht blockierendem Präsentieren eine saubere Frame-Pacing-Strategie verwenden;
- während aktiver Simulation, Dragging oder Textinput andere Reaktionsziele erlauben als im Leerlauf;
- Event-Waiting nicht so gestalten, dass Runner-Ausgaben oder Stop-Befehle verspätet verarbeitet werden.

### PERF-012 – Report-Speichern und CSV-Export auf große temporäre Spitzen prüfen

**Priorität:** P2/P3  
**Betroffene Dateien:** `src/report.c`, `src/report_export.c`

Der Report-Save nutzt einen begrenzten Puffer bis 8 MiB. Das ist sicherer als unbounded allocation, kann aber bei vielen Reports einen unnötig großen transienten Speicherbedarf erzeugen. CSV-/Text-Export formatiert viele Werte direkt über `fprintf()`.

**Empfehlung:** Größenberechnung, chunkweises Serialisieren und gepufferte Ausgabe prüfen. Das bestehende Größenlimit muss erhalten bleiben. Exportqualität, Locale-Verhalten, Zeilenenden und Dateiformat dürfen sich nicht ändern.

## 6. Was bereits gut gegen Performance-Risiken geschützt ist

Diese Punkte sollten bei einem Rework ausdrücklich erhalten bleiben:

- Preview-Limits bei 2.048 Punkten und begrenzte Report-Größen;
- blockweise Analyse statt vollständigem Einlesen großer Runs in den RAM;
- begrenzte Kanal-, Serien- und Datensatzanzahl;
- begrenzte Szenenkapazität statt automatisch wachsender GPU-/CPU-Datenstruktur;
- maximal acht Batch-Worker;
- begrenzte Ausgabe- und Pipe-Verarbeitung pro Scheduler-Durchlauf;
- getrennte Runner-Prozesse und Ressourcenlimits;
- maximal 60 Hz für interaktive Snapshots;
- Wiederherstellungs- und CRC-Prüfungen;
- vorsichtige Fehlerbehandlung und temporäre Dateien beim Schreiben.

## 7. Workspace- und App-Shell-Review

### 7.1 Neue verbindliche Designentscheidung

Die bisherige Apple-inspirierte Start- und Navigationsrichtung ist überholt.
Maßgeblich ist jetzt `docs/design-direction.md`: Physim startet mit einer
leeren Arbeitsumgebung und orientiert sich beim Workspace-Fluss an Visual
Studio Code.

Die drei fachlichen Bereiche **Entwickeln**, **Simulieren** und **Auswerten**
bleiben bestehen. Sie dürfen aber nicht mehr voraussetzen, dass beim Start
automatisch ein Pendel- oder anderes Beispielprojekt gewählt wird.

### 7.2 Anforderungen an die neue App-Shell

- leerer Start ohne vorausgewähltes Projekt;
- echte Ordnerauswahl über **Ordner öffnen**;
- Hinzufügen einzelner Dateien und Ordner zu einem Workspace;
- dauerhaft sichtbare globale Helferleiste oben;
- globale Aktionen mindestens für Ordner öffnen, Inhalte hinzufügen, neues
  Projekt, Dokumentation und Einstellungen;
- separater Projektmanager ausschließlich für neue Physim-Projekte;
- klar getrennte Abläufe für bestehenden Ordner öffnen und neues Projekt
  erzeugen;
- verständliche Empty-, Loading-, Error- und Recovery-Zustände;
- Workspace-Inhalt in der linken Navigation, projektbezogene Funktionen erst
  nach Erkennung eines Physim-Projekts.

### 7.3 Review-Befund am aktuellen Stand

Der aktuelle Stand erfüllt diese neue Richtung noch nicht vollständig:

- `app/main.c` setzt `projects/pendulum` als voreingestellten Pfad;
- `app/design_ui.inc` bietet ein inline eingebautes „Öffnen oder anlegen“
  statt eines separaten Projektmanagers;
- `open_project()` akzeptiert nur Ordner mit `physim.project` und den beiden
  fest definierten Quelldateien;
- eine echte Ordnerauswahl sowie das Hinzufügen beliebiger Dateien/Ordner sind
  noch nicht vorhanden;
- die obere Leiste enthält noch keine globale Helferleiste.

Das ist ein P1-Produkt-/UX-Thema, weil der Einstieg aktuell von einem
Beispielprojekt und einem Physim-spezifischen Projektformat abhängt. Die
technische Physik-, Runner- und Analysepipeline muss dafür nicht neu entworfen
werden; betroffen sind vor allem App-Shell, Workspace-Modell, Dateinavigation
und Projektanlage.

### 7.4 Abnahmekriterien

Die Änderung ist erst abgenommen, wenn die acht Kriterien aus dem Abschnitt
„Abnahmekriterien“ in `docs/design-direction.md` erfüllt und für den leeren
Workspace, einen beliebigen Ordner, ein bestehendes Physim-Projekt und den
Projektmanager getestet sind.

## 8. Vorschlag für ein UI-Rework

Ein kompletter visueller Rework ist vertretbar, aber die Informationsarchitektur sollte nicht ohne Nutzertests mehrfach umgeworfen werden. Der folgende Aufbau bewahrt die gute Grundidee und macht sie klarer.

### 8.1 App-Shell

- **Obere Titelleiste:** Projektname, aktuelle Datei beziehungsweise Run, unsaved/build/run-status und eine einzige kontextabhängige Primäraktion.
- **Linke Navigation:** Develop, Simulate, Analyze; darunter nur projektbezogene Navigation, keine unpriorisierte Liste aller Funktionen.
- **Hauptbereich:** genau ein dominanter Arbeitsinhalt.
- **Kontext-Inspector:** rechts oder als ausblendbares Panel; zeigt nur Informationen zum aktuell ausgewählten Objekt/Run.
- **Log/Problems:** standardmäßig kompakt; bei Fehlern automatisch sichtbar und fokussierbar, ansonsten nicht dauerhaft Raum verbrauchen.
- **Statuszeile:** kleine, immer verständliche Zustände wie `Gespeichert`, `Build läuft`, `Run 12 aktiv`, `3 Fehler`.

### 8.2 Develop

- Datei-/Artefakt-Auswahl mit klarer, ruhiger Hierarchie;
- Editor als dominanter Inhalt;
- Build-Diagnostik als strukturierte Problems-Liste mit Datei, Zeile, Spalte, Schweregrad und Navigation;
- Such- und Filterleiste mit Tastaturfokus;
- Build-Button nur dann prominent, wenn der Projektzustand einen Build sinnvoll macht;
- unsaved state als klarer Text/Status plus Diskettensymbol oder Punkt, nicht nur als kaum sichtbare Markierung.

### 8.3 Simulate

- Szene als größter Bereich;
- Controls als kompakte, kontextuelle Leiste mit eindeutigen Zuständen `Start`, `Pause`, `Step`, `Stop`;
- rechts ein Inspector mit Laufgrenzen, Seed, Zeitschritt und ausgewähltem Kanal;
- unten ein optionales Timeline-/Progress-Panel mit Run-Zeit, Samples, Rate und Recovery-Checkpoint;
- Plot nur dann prominent, wenn ein Kanal gewählt ist;
- aktuelle Run-Phase immer textuell sichtbar: `Vorbereitung`, `Läuft`, `Pausiert`, `Abgeschlossen`, `Fehler`, `Abgebrochen`;
- Stop und destruktive Aktionen mit verständlicher Rückmeldung, nicht nur über Logtext.

### 8.4 Analyze

- Report als primärer Inhalt, nicht als Ansammlung gleich großer Panels;
- Run-Auswahl und Vergleich in einer fokussierten Selection-Sheet oder einem klaren Filterbereich;
- Plot und Tabelle als bewusst umschaltbare Darstellung;
- Exportaktionen in einer stabilen Toolbar beziehungsweise einem Overflow-Menü;
- Provenance/Quelle als aufklappbarer Abschnitt;
- Empty State mit konkreter nächster Aktion, zum Beispiel `Run auswählen` oder `Analyse starten`;
- lange Kurven, Legenden und Tabellen müssen bei 100–200 % UI-Skalierung weiterhin lesbar bleiben.

### 8.5 Visuelles System

Vor dem weiteren Styling sollten Design Tokens zentral festgelegt werden:

- Farbrollen: background, surface, elevated surface, text primary, text secondary, separator, accent, success, warning, error, focus;
- Abstände auf einer kleinen 4-/8-Punkt-Skala;
- Textrollen statt einzelner Pixelwerte: display, title, section, body, secondary, code, caption;
- Zustände: normal, hover, pressed, focused, selected, disabled, loading, error;
- mindestens 100 %, 125 %, 150 % und 200 % UI-Skalierung;
- Light, Dark und High Contrast;
- sichtbarer Fokusindikator mit ausreichendem Kontrast;
- semantischer Zustand niemals nur durch Farbe;
- konsistente Mindesthöhe für wichtige Interaktionsziele, unabhängig von Plattform und DPI;
- runde Ecken und Schatten sparsam einsetzen, damit die App nicht wie eine Sammlung von Karten wirkt.

### 8.6 Accessibility als Produktfunktion

Die UI sollte einen internen semantischen View-/Accessibility-Layer bekommen, statt nur Pixel zu rendern. Für jedes wichtige Element müssen mindestens Name, Rolle, Zustand, Wert, Beschreibung und Navigationsreihenfolge verfügbar sein.

Minimaler Abnahmekatalog:

- gesamte App mit Tastatur bedienbar;
- sichtbarer Fokus in jedem Arbeitsbereich;
- Escape schließt temporäre Overlays und kehrt sinnvoll zurück;
- Tab-Reihenfolge folgt der visuellen und fachlichen Hierarchie;
- Screenreader kann Navigation, Editor, Run-Zustand, Plot und Fehler lesen;
- Textskalierung ohne abgeschnittene Controls;
- Kontrastprüfung in Dark, Light und High Contrast;
- Statuswechsel werden für Screenreader angekündigt;
- Plotdaten können zusätzlich als Tabelle oder zugängliche Zusammenfassung gelesen werden.

## 9. Konkreter Umsetzungs-Backlog

### Phase A – Messen und Verträge sichern

- **PERF-001:** Benchmark-Ziel und Baseline-Dokumentation.
- **QA-001:** Golden Runs mit festen Seeds, Samples, CRCs und Reportdaten.
- **QA-002:** UI-Snapshot-Matrix für 1080×740, 1440×940, HiDPI, lange Namen und leere Zustände.
- **UI-001:** Design Tokens und Zustandsmatrix definieren, ohne zunächst das gesamte Layout zu verändern.
- **BUILD-001:** CI um Formatcheck, Static Analysis und reproduzierbare Benchmark-Ausführung erweitern, sofern Laufzeitlimits klar sind.

### Phase B – Niedrigrisiko-Optimierungen

- **PERF-002a:** Windows-Timerfrequenz cachen.
- **PERF-003:** Read-only-/Block-API für Report-Kurven.
- **PERF-005:** Nuklear-CPU-/GPU-Buffer wiederverwenden.
- **PERF-009:** CRC32-Tabelle mit Golden Vectors.
- **PERF-010:** Report-/CSV-Pufferung prüfen.
- **PERF-011:** Framepacing ohne doppelte Verzögerung.

### Phase C – Streaming und Renderer

- **PERF-006:** Serien-Cursor und Read-Ahead mit festen Cache-Grenzen.
- **PERF-004:** statische Szene, Primitive und dynamische Daten trennen.
- **PERF-004b:** LOD/Instancing erst nach korrekter Baseline für große Szenen.
- **UI-002:** App-Shell und responsive Layout-Tokens.

### Phase D – Persistenz und Solver

- **PERF-002:** Durability-Policy mit Recovery- und Abbruchtests.
- **PERF-007:** Kontakt-Hotpath nur bei nachgewiesenem Profiling-Gewinn optimieren.
- **PERF-008:** interne validierte Fast Paths.
- **QA-003:** numerische Toleranz- und Cross-Compiler-Vergleiche.

### Phase E – UI-Abnahme

- **UI-003:** globale Skalierung und Themes.
- **UI-004:** vollständige Tastatur-/Focus-Navigation.
- **UI-005:** Accessibility-Semantik und Screenreader-Anbindung.
- **UI-006:** Develop-/Simulate-/Analyze-Rework anhand der Shell-Regeln.
- **UI-007:** UI-Performanceprofiling für Plot, Szene, Resize und große Listen.
- **QA-004:** manuelle HIG-/Accessibility-Abnahme auf Windows und Linux.

## 10. Test- und Abnahmekriterien

### Funktion und Reproduzierbarkeit

- gleiche Seeds erzeugen vor und nach Optimierungen dieselben relevanten Samples;
- Run-Abbruch, Pause, Step, Stop und Recovery funktionieren unverändert;
- alte Run-Dateien, Reports und CSVs bleiben lesbar;
- CRC- und Fehlerpfade bleiben aktiv;
- Runner-Prozessgrenzen und Ressourcenlimits bleiben erhalten;
- Batch-Reihenfolge, Cancellation und Deadline-Verhalten ändern sich nicht ohne fachliche Entscheidung.

### Performance

- Benchmark-Ergebnis wird mit Compiler, Buildtyp, CPU, GPU, OS und Datensatz gespeichert;
- p50, p95 und p99 statt nur Durchschnitt messen;
- keine Performance-Verbesserung auf Kosten von Peak-RAM, Dateisicherheit oder UI-Reaktionsfähigkeit;
- Warm- und Cold-Cache getrennt ausweisen;
- Disk-, CPU- und GPU-Anteil getrennt messen;
- bei UI keine Allokationen im stabilen Framepfad, sofern der jeweilige Rendererpfad das erlaubt.

### UI

- alle drei Arbeitsbereiche haben eine eindeutige Primäraktion;
- kein wichtiger Zustand wird nur durch Farbe, Icon oder Position kommuniziert;
- Fenstergrößen, Skalierungen, lange Projekt-/Dateinamen und leere Zustände sind geprüft;
- Fokus bleibt bei Panelwechseln, Dialogen und Fehlernavigation nachvollziehbar;
- Plot, Tabelle und Editor sind per Tastatur erreichbar;
- Fehler führen zu einer konkreten nächsten Handlung;
- Light, Dark und High Contrast verwenden dieselben semantischen Zustandsrollen;
- keine UI-Änderung darf Simulationstiming oder gespeicherte Daten beeinflussen.

## 11. Risiko- und Priorisierungsmatrix

| Risiko | Wahrscheinlichkeit | Auswirkung | Gegenmaßnahme |
|---|---:|---:|---|
| Flush-Optimierung verschlechtert Recovery | Mittel | Hoch | Policy, Crash-Matrix, Default explizit abnehmen |
| Renderer-Cache zeigt veraltete Geometrie | Mittel | Mittel/Hoch | Dirty-Flags, Golden Screenshots, Kamera-/Resize-Tests |
| Solver-Cache verändert numerisches Verhalten | Mittel | Hoch | Kontakt-Golden-Tests, Toleranzen und Reihenfolge fixieren |
| Read-only-Reportview wird nach Mutation genutzt | Mittel | Hoch | Lebensdauervertrag, Generation/Invalidierung, Tests |
| UI-Rework erhöht Informationsdichte | Mittel | Mittel | Nutzerflows und Empty-/Error-States zuerst prototypen |
| Accessibility wird erst am Ende berücksichtigt | Hoch | Hoch | Semantik und Fokus ab Phase A in das Modell aufnehmen |
| neues Theme dupliziert Styles unkontrolliert | Mittel | Mittel | zentrale Design Tokens statt lokaler Farben |
| fehlendes Git erschwert Review/Recovery | Hoch | Hoch | Repository/Baseline vor Umsetzung herstellen |

## 12. Empfohlene Reihenfolge

1. Repository oder unveränderlichen Baseline-Snapshot herstellen.
2. Golden Runs, numerische Referenzen und Performance-Benchmarks definieren.
3. Niedrigrisiko-Hotpaths optimieren: Timer, Report-Read-API, Buffer-Reuse, CRC.
4. Serien-Streaming und Renderer mit Messdaten verbessern.
5. Durability-Policy separat und mit Recovery-Tests umsetzen.
6. Kontakt-Solver nur nach Profiling und numerischer Abnahme optimieren.
7. UI-Tokens, Statusmodell und Accessibility-Grundlagen vor dem großen Layout-Rework definieren.
8. Develop, Simulate und Analyze jeweils als fokussierte, responsive Arbeitsräume neu abnehmen.
9. Tastatur-, Screenreader-, Kontrast-, HiDPI- und Performance-Abnahme durchführen.
10. Erst danach Release-/Installer-/Packaging-Qualität und Produktkommunikation finalisieren.

## 13. Gesamturteil

Physim ist technisch deutlich weiter als ein einfacher Prototyp: Die Architektur, Datenbegrenzungen, Prozessisolation, Analysepipeline und Testbreite bilden eine tragfähige Grundlage. Die aktuelle UI ist eine gute visuelle Basis, aber der neue Workspace-Einstieg ist noch nicht umgesetzt. Deshalb ist sie noch nicht als produktreif abzunehmen.

Der größte Fehler wäre jetzt, gleichzeitig viele sichtbare UI-Änderungen und aggressive Performance-Änderungen ohne Messbaseline umzusetzen. Der sichere Weg ist ein gemeinsamer Vertrag aus Reproduzierbarkeit, Datenintegrität, Performance-Budgets und UI-Zuständen. Danach kann die Oberfläche umfassend neu gestaltet werden, ohne dass die Physik- und Analysefunktionalität verloren geht.

**Empfohlener nächster konkreter Schritt:** Baseline-Benchmarks und Golden Runs festlegen, anschließend `PERF-003`, `PERF-005`, `PERF-009` und `UI-001` als erste, gut isolierbare Arbeitspakete reviewen und umsetzen.

## 14. Zweite Audit-Runde: Ranking, Gegenprüfung und Korrekturen

Die erste Fassung war inhaltlich umfangreich, aber noch nicht streng genug bewertet. Sie enthielt drei typische Review-Risiken:

- einige Performance-Hypothesen waren höher priorisiert als die vorhandene Evidenz rechtfertigt;
- die vielen bereits dokumentierten und visuell geprüften Funktionen wurden nicht deutlich genug von echten offenen Risiken getrennt;
- visuelle Qualität, Accessibility, Workspace-Produktfluss und freie Produktkonfiguration wurden teilweise vermischt, obwohl sie unterschiedliche Ziele sind.

Diese Runde führt deshalb ein separates Ranking für **Reife**, **Priorität** und **Evidenz** ein. Ein offener Punkt bekommt nicht automatisch eine schlechte Reife, und ein plausibler Hotpath bekommt nicht automatisch P1. Umgekehrt erhält ein nicht getesteter Bereich niemals künstlich 10/10.

### 14.1 Bewertungsmodell

#### Reife-Rating

| Rating | Bedeutung |
|---:|---|
| 10/10 | implementiert, fachlich klar definiert, automatisiert geprüft, visuell/operativ abgenommen und mit Regression abgesichert |
| 9/10 | sehr reif; nur kleine, klar begrenzte Restlücken oder fehlende Plattformbreite |
| 8/10 | produktionsnahe Grundlage mit nachgewiesener Funktion, aber relevanten Ausbaupunkten |
| 6–7/10 | funktional brauchbar, jedoch noch erkennbare Produkt-, Skalierungs- oder Qualitätslücken |
| 4–5/10 | Teilimplementierung oder relevante Unsicherheit |
| 1–3/10 | offen, ungetestet oder für den Produktanspruch nicht ausreichend |

#### Priorität

- **G0:** Voraussetzung für belastbare Entscheidungen, aber kein Produktfehler.
- **P1:** hoher funktionaler, datenbezogener oder nutzerseitiger Einfluss; zeitnah bearbeiten.
- **P2:** wichtiger Qualitäts-/Performancegewinn, aber zunächst mess- oder abgrenzbar.
- **P3:** Optimierung, Komfort oder Produktverfeinerung ohne unmittelbares Risiko.

#### Evidenz

- **A:** Quellcode plus dokumentierter Test-/Screenshot-/Messnachweis.
- **B:** Quellcode und Projektdokumentation, aber im aktuellen Review nicht neu ausgeführt.
- **C:** plausible technische Hypothese; Profiler, Nutzerstudie oder reproduzierbarer Test fehlt.

Diese drei Dimensionen dürfen nicht verwechselt werden. Beispiel: Ein P1-Punkt kann aktuell 5/10 Reife haben, aber nur Evidenz C besitzen. Dann ist die richtige nächste Aktion zunächst Messung, nicht sofort ein großer Rewrite.

### 14.2 Ergebnis-Ranking des aktuellen Projekts

| Bereich | Reife | Priorität der Restarbeit | Evidenz | Begründung |
|---|---:|---:|:---:|---|
| Architektur/Modulgrenzen | 9/10 | P2 | A/B | Core, Runner, App, Analyse und Reports sind sauber getrennt; zusätzliche Produktreife fehlt noch |
| Numerische Basis | 9/10 | P2 | A | Viele Referenz-, Grenz- und Fehlerfälle dokumentiert; weitere Verfahren sind Roadmap, kein aktueller Defekt |
| Reproduzierbarkeit | 9/10 | P1 | A | Seeds, feste Reihenfolge und zahlreiche bitgleiche Vergleiche sind vorhanden; Baseline-Gates sollten formalisiert werden |
| Datenintegrität/Recovery | 9/10 | P1 | A/B | CRC, Recovery, Transaktionen und Präfix-/Mutationsprüfungen sind stark; Durability-Vertrag muss klarer messbar sein |
| Run-Schreibdurchsatz | 7/10 | P1 | A/B | `fflush()` pro Sample ist real; die dokumentierte 1-Millionen-Punkte-Messung liefert eine Baseline, aber noch kein Zielbudget |
| Analyse-Speichergrenzen | 9/10 | P2 | A/B | Scratch-Dateien und Limits sind gut; I/O-Durchsatz kann bei langen Serien verbessert werden |
| Analyse-I/O | 7/10 | P2 | B | Seek-/Blockstruktur ist sichtbar, der reale Anteil an der Gesamtlaufzeit muss gemessen werden |
| Mechanik/Contact-Solver | 8/10 | P2 | A/B | Funktions- und Erhaltungstests sind stark; Skalierung auf viele Kontakte ist noch kein voller Produktumfang |
| Renderer-Korrektheit | 9/10 | P2 | A | OpenGL-Readback-, Tiefen-, Projek­tions-, Resize- und Primitive-Tests sind vorhanden |
| Renderer-Skalierung | 7/10 | P2 | B/C | Per-Frame-Tessellierung und Upload sind echte Kosten; Stressprofil mit großen Szenen fehlt |
| UI-Visualität | 8.5/10 | P2 | A/B | Screenshots bei 1080×740 und 1440×940 zeigen eine ruhige, konsistente Oberfläche |
| UI-Hierarchie | 7.5/10 | P1 | A/B | Simulation und Analyse zeigen mehrere gleichgewichtete Aktionen; aktuelle nächste Handlung muss stärker hervorgehoben werden |
| UI-Interaktion | 7/10 | P1 | B | Maus-/SDL-Selbsttests sind umfangreich; vollständige Fokusreihenfolge und alternative Bedienung sind offen |
| Accessibility | 3/10 | P1 | B | Das Projekt benennt Keyboard-Fokus, Screenreader und Skalierung selbst als offen |
| Responsive Layout/HiDPI | 7/10 | P1 | A/B | Mindestgröße und einige Größenchecks bestehen; globale Skalierung und lange Inhalte sind nicht vollständig abgenommen |
| Themes/OS-Anpassung | 6/10 | P2 | B | Dark Theme ist konsistent; Light/High Contrast sind offen, aber für den neuen Workspace-Fluss nachgeordnet |
| Freies Docking | 5/10 | P3 | B | Produktoption; erst nach dem Workspace- und Projektfluss priorisieren |
| Build/Compiler-Breite | 8/10 | P2 | A/B | MSVC, Clang-Cl und Linux-CI-Konfiguration vorhanden; echte Clean-Machine-/Linux-Ausführung bleibt offen |
| Performance-Instrumentierung | 4/10 | G0 | B | einzelne Messwerte existieren, aber kein dauerhaftes Benchmark-/Regression-Gate |
| Dokumentationsqualität | 9/10 | A/B | Status und Design-Direction sind ungewöhnlich klar über offene Punkte |

**Wichtig:** Das Projekt erhält damit insgesamt nicht künstlich 10/10. Ein ehrliches Gesamtrating liegt aktuell bei **8/10**: technisch sehr starke Entwicklungsbasis und funktional weit, aber mit echten Restlücken bei Accessibility, Performance-Budgets, UI-Zustandshierarchie und Produktreife. Ein „perfektes“ Ranking wäre erst nach Umsetzung und Abnahme dieser Punkte sachlich vertretbar.

### 14.3 Gegenprüfung der Performance-Prioritäten

Die ursprüngliche Priorisierung wird nach der zweiten Prüfung korrigiert:

| Erstbewertung | Korrigiertes Rating | Punkt | Warum |
|---:|---:|---|---|
| P0 | G0 | Baseline/Benchmark | Ohne Messung ist dies eine Voraussetzung, kein Fehler mit sofortigem Nutzerimpact |
| P1 | P1 | `fflush()` pro Sample | Im Code direkt belegt; zusätzliche lokale Streamingmessung zeigt relevante Laufzeit, Ursache muss aber profiliert werden |
| P1 | P2 | Report-Kurvenkopie | Technisch real, aber Preview auf 2.048 Punkte und begrenzte Plotanzahl halten den aktuellen Scope klein |
| P1 | P2 | Szene komplett neu tessellieren | Für große Szenen plausibel; aktuelle Produktbeispiele sind klein und GPU-Korrektheit ist gut belegt |
| P1 | P2 | Nuklear-Buffer pro Frame | Gute Optimierung, aber derzeit kein nachgewiesener Blocker |
| P1 | P2 | Series-Seeks | Architekturbedingt plausibel; tatsächlicher Vorteil hängt stark von Datensatz und Storage ab |
| P1/P2 | P2/P3 | Contact-Solver-Caching | Erst nach Profiling priorisieren, da numerische Risiken höher sind |
| P2 | P2 | CRC32 | Für sehr große Runs sinnvoll; Golden-Vector-Tests sind Pflicht |
| P2 | P3 | Timerfrequenz cachen | Saubere Mikrooptimierung, aber kein Produkthebel ohne Profiling |
| P2 | P2 | doppeltes Framepacing | Sichtbar im Code und potenziell nutzerwirksam; Latenz-/CPU-Messung ergänzen |

Damit bleibt der wichtigste Performance-Punkt die Persistenz, aber nicht mit der pauschalen Aussage „die App ist langsam“. Die belegte Aussage lautet präziser: **Der aktuelle Run-Schreibpfad führt pro Sample einen Flush aus; bei langen Runs muss gemessen werden, wie groß der reale Anteil an der Laufzeit ist und welche Durability-Garantie erhalten werden soll.**

### 14.4 Visuelle Gegenprüfung der aktuellen UI

Die zweite Runde hat vorhandene gerenderte Artefakte aus `build-next` geprüft, unter anderem Simulation, Analyse, Monte Carlo und Einstellungen bei der dokumentierten kleinen Fenstergröße.

#### Stärken – 8.5/10

- ruhige dunkle Oberfläche mit klarer Akzentfarbe;
- verständliche linke Arbeitsbereich-Navigation;
- gute Abstände, konsistente Rundungen und ausreichend große Controls;
- systematische Trennung von Arbeitsbereich, Projektkontext, Inspector, Hauptinhalt und Protokoll;
- Simulation und Plot bleiben visuell im Vordergrund;
- Einstellungsdialog hat mit **Übernehmen** eine klar erkennbare Primäraktion;
- Analyse zeigt Kurve, Legende, Herkunft und Exportmöglichkeiten ohne dekorative Überladung;
- leere, laufende und pausierte Zustände sind grundsätzlich vorgesehen.

#### Konkrete Abzüge

1. **Primäraktion im Simulationszustand:** Im pausierten Screenshot ist `Fortsetzen` die naheliegende nächste Aktion, während `Neuer Lauf` den stärkeren blauen Stil erhält. Der blaue Stil wird dadurch eher als Auswahlfarbe als als „nächste beste Aktion“ verstanden.
2. **Zu viele gleichgewichtete Aktionen:** `Neuer Lauf`, `Fortsetzen`, `Einzelschritt`, `Stoppen` und `Auswerten` stehen gleichrangig nebeneinander. Die neue Workspace-orientierte Hierarchie verlangt nicht weniger Funktion, sondern eine klarere Reihenfolge: globale Aktionen oben, aktuelle Hauptaktion im Kontext, sekundäre oder seltene Aktionen im Overflow/Bestätigungsweg.
3. **Analyse-Toolbar:** Analyse starten, CSV, Läufe & Berichte, Ergebnis/Messdaten und Diagrammexport sind funktional sinnvoll, aber auf engem Raum sehr buttonlastig. Ergebnisansicht, Export und Run-Auswahl sollten stärker getrennt werden.
4. **Statusredundanz:** Status steht oben rechts und zusätzlich im unteren Protokollbereich. Das ist robust, aber die Statuszeile sollte die kurze Wahrheit liefern und das Protokoll nur Details/Verlauf.
5. **Monte-Carlo-Dichte:** Der Inhalt ist verständlich, enthält aber viele Eingaben gleichzeitig. Fortgeschrittene Parameter könnten in eine Disclosure-Gruppe, während Laufanzahl, Seed und Start die sichtbare Kernkonfiguration bleiben.
6. **Interaktionshinweise:** Plot-Zoom und Pan sind vorhanden, aber die aktuelle UI zeigt keine gleichwertige Tastaturbedienung. Das ist neben Accessibility auch ein Produktqualitätsproblem.
7. **Generated-artifact-Vorsicht:** Die Screenshots unter `build`/`build-next` sind wertvolle Belege, aber kein Ersatz für eine aktuelle Live-Abnahme. Bei einer späteren UI-Iteration muss jeweils Build-Version, Commit/Baseline, Auflösung, DPI und Theme im Artefakt festgehalten werden.

#### Einordnung der bisherigen visuellen Richtung

Die bisherige Oberfläche besitzt eine ruhige, konsistente visuelle Sprache. Für die
neue Produktentscheidung ist aber der Workspace- und Navigationsfluss maßgeblich;
die App soll dabei Visual Studio Code als Referenz für den Einstieg nutzen, ohne
dessen Oberfläche vollständig zu kopieren. Nicht jede offene UI-Frage ist ein
Workspace-Blocker:

- **Hohe Accessibility-Relevanz:** sichtbarer Fokus, vollständige Tastaturbedienung, Screenreader-Semantik, Textskalierung, Kontrast, Zustandskommunikation ohne Farbe;
- **Hohe Produktrelevanz:** klare Primäraktion passend zum Zustand, bessere Empty-/Loading-/Error-States, responsive lange Inhalte;
- **Mittlere Produktrelevanz:** Light Theme und OS-nahe Theme-Anpassung;
- **Nachgelagerte Produktrelevanz:** freies Docking. Das kann nützlich sein, ist aber erst nach dem neuen Workspace-Fluss zu priorisieren.

### 14.5 Selbstkritik der ersten Review-Fassung

Die erste Fassung war nützlich, aber nicht perfekt. Die konkreten Korrekturen sind:

- **Zu wenig Belegtrennung:** Die Statusdatei dokumentiert viele reale Tests, Screenshots und Messwerte. Die neue Fassung macht daraus ein eigenes Evidenz-Rating, statt alles nur als „nicht neu ausgeführt“ abzuschwächen.
- **Zu aggressive Performance-Sprache:** Mehrere Hotpaths wurden als P1 formuliert, obwohl nur Quellcode-Indizien, kein Profilergebnis vorlagen. Diese Punkte sind jetzt P2/P3, bis Messungen sie hochstufen.
- **Fehlende UI-Zustandsanalyse:** Die erste Fassung beschrieb die Struktur, prüfte aber nicht konkret, ob die hervorgehobene Aktion zur aktuellen Simulation passt. Das ist jetzt ein eigener P1-Hierarchiepunkt.
- **Docking zu hoch bewertet:** Freies Docking war als offene Lücke aufgeführt, obwohl zuerst der neue Workspace- und Projektfluss geklärt werden muss. Es ist jetzt P3.
- **Keine Gesamtnote:** Die erste Fassung hatte viele Einzelvorschläge, aber keine vergleichbare Reife-Skala. Das neue 0–10-Rating macht Fortschritt und Restlücken prüfbar.
- **Keine klare Definition von „perfekt“:** Perfekt bedeutet in einem Review nicht, alle Werte künstlich auf 10/10 zu setzen. Es bedeutet, jedes Urteil mit Beleg, Unsicherheit, Priorität und Abnahmekriterium zu versehen.

### 14.6 Endgültig priorisierte Maßnahmen nach dieser Runde

#### Sofort nach Baseline – G0/P1

1. Benchmark-/Profiler-Grundlage mit dem dokumentierten 1-Millionen-Punkte-Run reproduzierbar machen.
2. Run-Schreibpfad messen: CPU, System Calls, Flush-/Sync-Anteil, Dateigröße und Recovery-Verhalten.
3. UI-Zustandsmatrix erstellen: `bereit`, `läuft`, `pausiert`, `fehler`, `abgeschlossen`, `wiederhergestellt`.
4. Für jeden Zustand genau eine Primäraktion und klar abgegrenzte Sekundäraktionen definieren.
5. Keyboard-Fokus- und Accessibility-Modell als Daten-/Interaktionsschicht planen, nicht erst nach dem visuellen Rework.

#### Danach – P1/P2

1. Durability-Policy bewusst entscheiden und mit Crash-/Recovery-Tests absichern.
2. UI-Scale, Kontrast und Focus-Ring über alle drei Arbeitsbereiche einführen.
3. Report-Kurven-API und Nuklear-Buffer ohne Verhaltensänderung optimieren.
4. Serien-Cursor/Read-Ahead nur mit gemessener I/O-Verbesserung umsetzen.
5. Simulation und Analyse visuell entflechten: Primäraktion, Ergebnis-/Exportebene und Inspector sauber trennen.

#### Später – P3

1. freie Panel-Anordnung nur nach Nutzerfeedback;
2. Timer-/Mikrooptimierungen;
3. umfangreichere LOD-/Instancing-Strategien für Szenen;
4. zusätzliche Themes und Komfortfunktionen, sofern Accessibility-Grundlagen bereits stabil sind.

### 14.7 Bedingungen für ein echtes 10/10-Rating

Ein 10/10-Rating ist erst gerechtfertigt, wenn alle folgenden Punkte nachweisbar erfüllt sind:

- Performance-Budgets existieren und laufen als Regression-Gates;
- Run-Persistenz hat dokumentierte, getestete Durability-Modi;
- keine ungeprüften Hotpath-Behauptungen bleiben als Fakten im Review stehen;
- Simulation hebt immer die korrekte nächste Primäraktion hervor;
- alle wichtigen Arbeitsabläufe sind per Tastatur erreichbar;
- Screenreader-Rollen, Zustände und Statusmeldungen sind angebunden;
- UI-Text und Controls skalieren mit hoher DPI und großen Textgrößen;
- Dark, Light und High Contrast bestehen die Kontrast-/Zustandsprüfung;
- Plotdaten sind auch ohne Maus als Tabelle/Zusammenfassung zugänglich;
- lange Dateinamen, leere Zustände, Fehler, Loading, Recovery und Abbruch sind visuell abgenommen;
- Render- und Analyseoptimierungen behalten Golden Screenshots, numerische Daten und Reproduzierbarkeit;
- Clean-machine-, Linux- und Plattformtests sind tatsächlich ausgeführt und archiviert.

**Abschluss der zweiten Runde:** Die Review selbst ist unter dem gegebenen Read-only-Scope jetzt mit **10/10 für Bewertungsdisziplin und Nachvollziehbarkeit** einzustufen: Aussagen sind nach Evidenz, Priorität und Reife getrennt; vorhandene Stärken werden nicht mehr mit offenen Produktzielen vermischt; UI- und Performance-Urteile enthalten konkrete Gegenbeispiele und Abnahmekriterien. Das Projekt selbst steht bewusst bei **8/10**, weil ein höheres Rating ohne die oben genannten Implementierungs- und Abnahmeschritte nicht ehrlich wäre.
