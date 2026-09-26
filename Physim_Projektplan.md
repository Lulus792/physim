# Physim – vollständiger Projekt- und Entwicklungsplan

## 1. Vision

Physim wird eine plattformübergreifende Simulationsumgebung für Windows und Linux. Das Projekt besteht aus zwei klar getrennten Produkten:

1. **Physim Library** – eine modulare Physik-, Mathematik-, Mess- und Auswertungsbibliothek in C.
2. **Physim App** – eine grafische Arbeitsumgebung, in der Experimente in C oder einer eigenen, auf Physim zugeschnittenen kompilierten Sprache entwickelt, ausgeführt, dreidimensional beobachtet, gemessen und anschließend ausgewertet werden. Die eigene Sprache ist ein verbindliches Produktziel und eine vollständige Alternative zu C für den gesamten Physim-Arbeitsablauf.

Das langfristige Ziel ist nicht nur eine Sammlung fertiger Simulationen. Physim soll ein Baukasten sein, mit dem theoretische Idealmodelle und realitätsnahe Modelle aus denselben Grundbausteinen erzeugt und miteinander verglichen werden können. Dazu gehören unter anderem Einheiten, Materialien und Medien, Messunsicherheit, Wahrscheinlichkeitsverteilungen, numerische Verfahren und reproduzierbare Zufallsprozesse.

Der wichtigste Grundsatz lautet:

> Erst ein kleiner, vollständig funktionierender Weg vom Quellcode bis zum Diagramm; danach schrittweise mehr Physik.

---

## 2. Verbindliche Produktziele

### 2.1 Muss-Ziele der ersten stabilen Version

- Physim läuft unter Windows und Linux.
- Bibliothek, App, Runner und eigener Compiler werden in **C17** implementiert. Nutzer schreiben Experimente und Analysen wahlweise in C oder der eigenen Physim-Sprache.
- Die Physikbibliothek besitzt keine GUI-Abhängigkeit.
- Die App hat drei Hauptarbeitsbereiche:
  - **Entwickeln**: spezialisierter Editor für Experimente in C und der eigenen Sprache.
  - **Simulieren**: Ausführung, 3D-Ansicht, Steuerung und Live-Messwerte.
  - **Auswerten**: eigener Code in beiden Sprachen, Datenzugriff, Berechnungen, Tabellen und Plots.
- Experimente werden getrennt von der App kompiliert und ausgeführt.
- Ein Absturz oder eine Endlosschleife im Experiment darf die App nicht beenden.
- Messdaten werden während der Simulation fortlaufend und absturzsicher gespeichert.
- 2D-Experimente können in einer 3D-Szene betrachtet werden.
- Simulationen können gestartet, pausiert, schrittweise ausgeführt, fortgesetzt und abgebrochen werden.
- Einheiten, Zeitschritt, Zufalls-Seed, Material- und Mediumseigenschaften werden mit dem Datensatz gespeichert.
- Beispielprojekte und automatisierte Tests gehören zum Repository.

### 2.2 Langfristige Ziele

- Mehrere physikalische Domänen: Mechanik, Felder, Thermodynamik, Strömung und gekoppelte Modelle.
- Idealisiertes und realitätsnahes Modell derselben Versuchsanordnung.
- Unsicherheiten, Verteilungen und Monte-Carlo-Läufe.
- Parameterstudien, Optimierung und Vergleich mehrerer Läufe.
- Erweiterbare Visualisierung und Datenanalyse.
- Vollständig ausgebaute eigene Physim-Sprache mit einfacher Syntax, statischem Typsystem und nativer Kompilierung auf Basis derselben Physim-Funktionen wie C; der erste vollständige Sprachworkflow gehört bereits zur stabilen Version.

### 2.3 Bewusste Nicht-Ziele der ersten Version

- Keine vollständige Alternative zu MATLAB, Mathematica, Blender oder einer industriellen CFD-/FEM-Suite.
- Keine vollständige C-IDE für beliebige Softwareprojekte.
- Keine verteilte Simulation und kein Cloud-Dienst.
- Keine Plugin-Ausführung aus unbekannten Quellen ohne Sicherheitswarnung.
- Keine Beschränkung der eigenen Sprache auf Formeln oder Plotdefinitionen; die Umsetzung baut schrittweise auf dem validierten C-Workflow auf.
- Keine GPU-Physik in der ersten Version.

---

## 3. Abhängigkeits- und Plattformstrategie

„Nur für die GUI von anderem abhängig“ wird so präzisiert:

### 3.1 Physim Library

- C17 und C-Standardbibliothek.
- Keine GUI-, Skript-, Datenbank- oder Netzwerkbibliothek.
- Kleine Betriebssystem-Abstraktionsschicht nur dort, wo Standard-C nicht genügt, zum Beispiel für hochauflösende Zeit, dynamische Bibliotheken, Prozesse, Shared Memory und Threads.
- Betriebssystemspezifischer Code bleibt in `platform/windows` und `platform/linux` gekapselt.

### 3.2 Physim App

- Eine klar abgegrenzte UI-Plattformschicht darf externe GUI-/Fenstertechnik verwenden.
- Empfohlener Start: **SDL3** für Fenster, Eingabe und OpenGL-Kontext; darauf eine C-kompatible GUI-Schicht wie **Nuklear** oder langfristig eigene Widgets.
- Der 3D-Renderer wird in C auf einer kleinen Grafikabstraktion aufgebaut. Erste Implementierung: OpenGL 3.3 Core, damit Windows und Linux mit derselben Rendering-Pipeline bedient werden.
- Schrift- und Bild-Helfer dürfen als Teil des UI-Pakets mitgeliefert werden. Sie dürfen nicht in die Physikbibliothek eindringen.

### 3.3 Compiler

Beliebigen C-Code zu kompilieren erfordert zwangsläufig einen Compiler. Deshalb gibt es zwei Stufen:

1. **Entwicklungsphase und MVP:** Physim erkennt einen installierten Clang/GCC/MSVC-Compiler und zeigt eine verständliche Einrichtungshilfe.
2. **Spätere Distribution:** geprüfte Compiler-Pakete können pro Plattform mit Physim ausgeliefert oder separat durch Physim installiert werden. Das beseitigt die externe Einrichtung, erhöht aber Downloadgröße, Lizenz- und Updateaufwand.

TinyCC ist als schneller optionaler Compiler denkbar, sollte aber nicht der einzige Compiler sein, da Sprachunterstützung, Debugging und Optimierung begrenzt sind.

Die eigene Sprache erhält zusätzlich den in Abschnitt 10 beschriebenen Compiler. Im ersten Backend übersetzt er nach eigener Syntax- und Typprüfung in C17 und nutzt anschließend dieselben nativen Toolchains. Der Buildservice erledigt beide Schritte automatisch; Diagnosen verweisen auf den ursprünglichen Sprachquelltext.

### 3.4 Build und Paketierung

- CMake als Entwicklungs-Buildsystem; es ist keine Laufzeitabhängigkeit des Endnutzers.
- Compiler-Warnungen auf höchster sinnvoller Stufe.
- Windows: portable ZIP und später Installer.
- Linux: tar.gz und später AppImage beziehungsweise distributionsspezifische Pakete.
- Alle benötigten UI-Laufzeitdateien werden mit der App ausgeliefert.

---

## 4. Gesamtarchitektur

```text
┌──────────────────────────── Physim App ────────────────────────────┐
│ Projektverwaltung │ Editor │ Simulation UI │ Analyse UI │ Hilfe   │
└──────────┬────────────┬───────────┬───────────────┬───────────────┘
           │            │           │               │
     Build Service   Runner IPC  Render Snapshots  Analysis IPC
           │            │           │               │
     C-Compiler     Experiment Runner          Analysis Runner
                         │                          │
                 Experiment-Modul              Analyse-Modul
                         │                          │
              ┌──────────┴──────────────────────────┘
              │       öffentliche Physim-C-API
              │
     Mathematik │ Einheiten │ Physik │ Messung │ Daten │ Plot-Daten
```

### 4.1 Prozessgrenzen

Physim nutzt mindestens drei Prozesse:

- **`physim`**: GUI, Editor, 3D-Renderer, Projektsteuerung.
- **`physim-runner`**: lädt genau ein kompiliertes Experiment und führt es aus.
- **`physim-analysis-runner`**: lädt Analysecode und verarbeitet einen oder mehrere Datensätze.

Gründe für die Trennung:

- Schutz der App vor Segmentation Faults und ungültigem Benutzercode.
- Abbrechen festhängender Läufe durch Beenden des Runner-Prozesses.
- Sauberes Hot-Reload nach dem Kompilieren.
- Klare Ressourcen- und Laufzeitmessung.
- Später leichter erweiterbar um Sandboxing.

### 4.2 Kommunikation

- Kontrollnachrichten über lokale Pipes.
- Große, häufig aktualisierte Zustände optional über Shared Memory.
- Nachrichtenformat mit fester Kopfstruktur: Protokollversion, Nachrichtentyp, Größe, Sequenznummer.
- Keine rohen C-Strukturen direkt übertragen; explizite Serialisierung verhindert ABI- und Padding-Probleme.
- Versions-Handshake beim Start jedes Runners.
- Heartbeat und Timeout-Erkennung.

### 4.3 Thread-Modell der App

- UI-/Render-Thread: zeichnet Oberfläche und 3D-Szene.
- Build-Worker: startet Compiler und sammelt Diagnosen.
- IPC-Worker: liest Runner-Nachrichten.
- Daten-Writer: schreibt Messblöcke fortlaufend auf Datenträger.
- Niemals Dateizugriff, Kompilieren oder lange Berechnungen im UI-Thread.

---

## 5. Repository-Struktur

```text
physim/
├─ CMakeLists.txt
├─ cmake/
├─ docs/
│  ├─ architecture/
│  ├─ api/
│  ├─ tutorials/
│  └─ decisions/             # Architecture Decision Records
├─ include/physim/           # stabile öffentliche Header
│  ├─ base/
│  ├─ math/
│  ├─ units/
│  ├─ physics/
│  ├─ experiment/
│  ├─ data/
│  └─ analysis/
├─ src/
│  ├─ base/
│  ├─ math/
│  ├─ units/
│  ├─ physics/
│  ├─ experiment/
│  ├─ data/
│  └─ platform/
├─ app/
│  ├─ ui/
│  ├─ editor/
│  ├─ simulation/
│  ├─ analysis/
│  ├─ renderer/
│  ├─ build_service/
│  └─ project/
├─ runners/
│  ├─ experiment/
│  └─ analysis/
├─ tools/
├─ examples/
│  ├─ projectile/
│  ├─ pendulum/
│  ├─ collision/
│  └─ drag_monte_carlo/
├─ tests/
│  ├─ unit/
│  ├─ integration/
│  ├─ numerical/
│  └─ golden/
├─ third_party/              # ausschließlich klar lizenzierte UI-Bausteine
└─ packaging/
```

---

## 6. Die drei Hauptarbeitsbereiche

## 6.1 Entwickeln – spezialisierte C-IDE

### Mindestumfang

- Projektbaum für Experimentquellen, Analysequellen, Assets und Läufe.
- Editor mit Zeilennummern, Undo/Redo, Suchen/Ersetzen und C-Syntaxhervorhebung.
- Vorlagen für ein neues Experiment.
- API-Browser mit Modulen, Datentypen, Funktionen, Einheiten und Beispielen.
- Build-Schaltfläche und Tastenkürzel.
- Build-Ausgabe mit anklickbaren Datei-/Zeilenfehlern.
- Auswahl Debug/Release.
- Anzeige der verwendeten Physim-API-Version und des Compilers.
- Starten des Experiments erst nach erfolgreichem Build.

### Später

- Autovervollständigung aus einer generierten Physim-Symboltabelle.
- Parameterformular, das aus Metadaten des Experiments entsteht.
- einfache Breakpoints für Simulationsschritte, nicht sofort ein vollständiger nativer Debugger.
- API-Dokumentation direkt neben dem Code.

### Experiment-Lebenszyklus

Ein Experiment exportiert genau eine versionierte Beschreibung mit Callbacks:

```c
typedef struct ps_experiment_api {
    uint32_t api_version;
    const char *name;
    bool (*create)(ps_context *ctx);
    bool (*reset)(ps_context *ctx);
    bool (*step)(ps_context *ctx, double dt);
    void (*build_scene)(ps_context *ctx, ps_scene_writer *scene);
    void (*destroy)(ps_context *ctx);
} ps_experiment_api;
```

Die endgültige API muss zusätzlich Größenfelder, Capability-Bits und Fehlerwerte besitzen, damit spätere Versionen kompatibel erweitert werden können.

## 6.2 Simulieren – Ausführung und 3D-Ansicht

### Layout

- Mitte: 3D-Viewport.
- Links oder oben: Szenenhierarchie und Kameraoptionen.
- Rechts: Parameter, aktuelle Messwerte und Ereignisse.
- Unten: Zeitleiste, Log und Steuerung.

### Steuerung

- Start, Pause, Fortsetzen, Stoppen.
- Einzelschritt.
- Zurücksetzen auf Anfangszustand.
- Simulationsgeschwindigkeit unabhängig von der Rendergeschwindigkeit.
- feste oder adaptive Simulationsschritte.
- Kamera: Orbit, Pan, Zoom, voreingestellte Ansichten.
- 2D-Modus als Ebene in der 3D-Welt mit optionaler Perspektive oder orthografischer Kamera.
- Sichtbarkeit einzelner Objekte, Vektoren, Trajektorien und Messpunkte.

### Technische Trennung

- Physikschritt und Rendering sind unabhängig.
- Der Runner liefert unveränderliche **Scene Snapshots**; die App rendert sie.
- Ein Experiment darf keine OpenGL-Aufrufe ausführen.
- Die Simulationszeit darf bei langsamer Darstellung nicht unkontrolliert springen.
- Bei Echtzeitmodus wird ein Akkumulator mit festem Physikzeitschritt verwendet.
- Bei Offline-Modus läuft die Simulation so schnell wie möglich und sendet nur ausgewählte Frames.

### Live-Daten

- Kanäle werden beim Start mit Name, Datentyp, Einheit und Beschreibung registriert.
- Daten kommen in Blöcken mit Simulationszeit und optionaler Unsicherheit.
- Die UI zeigt aktuelle Werte, kleine Live-Kurven und Warnungen.
- Der Writer speichert unabhängig vom Plot; langsames Zeichnen darf keine Messwerte verlieren.
- Backpressure-Regeln: plotbare Vorschau darf ausgedünnt werden, der persistierte Datenstrom nicht.

## 6.3 Auswerten – C-basierte Analyse

### Mindestumfang

- Eigener Editor und eigener Build-Zieltyp für Analysecode.
- Auswahl eines oder mehrerer Läufe.
- Zugriff auf Metadaten, Kanaldefinitionen und Datenblöcke.
- Filter, Transformationen, Ableitungen, Integrale und statistische Kennzahlen.
- Erzeugung von Tabellen und Plotserien.
- Linien-, Punkt- und Histogramm-Plots.
- Export von CSV und Bilddateien.
- Ergebnisse werden mit Analysequellcode, Build-Informationen und Eingabedatensatz verknüpft.

### Analyse-API

Analysecode erhält Handles statt interner Zeiger:

```c
ps_dataset ds = ps_analysis_open_run(ctx, "latest");
ps_series x = ps_dataset_series(ds, "time");
ps_series y = ps_dataset_series(ds, "position.x");
ps_series v = ps_series_derivative(ctx, y, x);
ps_plot_line(ctx, "Geschwindigkeit", x, v);
```

Operationen arbeiten blockweise, damit große Datensätze nicht vollständig in den RAM geladen werden müssen.

---

## 7. Physim Library – Modulplan

## 7.1 `base`

- feste Integer-Typen, Fehlercodes und Ergebniswerte.
- Allocator-Schnittstelle.
- dynamische Arrays, Arenen, Hash-Map und String-View.
- Logging ohne globale versteckte Zustände.
- versionierte Handles statt frei erreichbarer interner Strukturen.
- deterministischer, explizit gespeicherter Zufallszahlengenerator.

## 7.2 `math`

- Vektoren 2D/3D/4D, Matrizen, Quaternionen und Transformationen.
- robuste Vergleiche und definierte Toleranzen.
- Interpolation und Kurven.
- lineare Gleichungssysteme.
- Nullstellensuche und Optimierungsgrundlagen.
- ODE-Solver: Euler nur zu Lernzwecken, symplektischer Euler, Verlet, RK4; später adaptive Runge-Kutta-Verfahren.
- numerische Integration und Differentiation.

Jeder Algorithmus erhält dokumentierte Einheitenannahmen, Fehlerverhalten und Referenztests.

## 7.3 `units`

- SI-Basiseinheiten und abgeleitete Einheiten.
- Werte werden intern einheitlich in SI gespeichert.
- Dimensionen und Skalierung sind Teil der Metadaten.
- Laufzeitprüfung an API-Grenzen und in der Analyse.
- Formatierung und Konvertierung für Anzeige und Export.

C kann Dimensionen nur begrenzt statisch prüfen. Deshalb wird nicht vorgegeben, dass ein `double` allein seine Einheit kennt. Öffentliche APIs nutzen entweder klar benannte SI-Felder oder Wert-plus-Dimensions-Metadaten.

## 7.4 `physics-mechanics`

Reihenfolge:

1. Punktmasse, Kräfte, Impuls und Energie.
2. starre Körper, Trägheitstensor, Lage und Winkelgeschwindigkeit.
3. einfache Constraints und Federn.
4. Broad Phase für Kollisionen.
5. Narrow Phase für Kugel, Ebene, Box und konvexe Körper.
6. Impulsbasierte Kollisionsantwort mit Reibung und Restitution.
7. kontinuierliche Kollisionserkennung für schnelle Objekte.

## 7.5 `materials` und `media`

- Materialien und Medien sind Daten, keine fest eingebauten Sonderfälle.
- Eigenschaften können konstant, temperaturabhängig oder druckabhängig sein.
- Beispiele: Dichte, dynamische/kinematische Viskosität, Reibungskoeffizient, Restitution, Wärmeleitfähigkeit, Wärmekapazität, elektrische Leitfähigkeit und Permittivität.
- Quellenangabe und Gültigkeitsbereich können als Metadaten gespeichert werden.
- Ein Experiment kann Vakuum, Luft, Wasser oder ein benutzerdefiniertes Medium wählen.
- Modelle entscheiden explizit, welche Eigenschaften sie verwenden.

## 7.6 `measurement` und Unsicherheit

- Messkanäle mit Einheit, Abtastrate und Zeitstempel.
- absolute und relative Unsicherheit.
- Sensorauflösung, Offset, Drift, Rauschen und Ausfallmodell.
- Verteilungen: konstant, uniform, normal; später weitere.
- Monte-Carlo-Ausführung mit explizitem Seed.
- Vertrauensintervalle, Quantile und Histogramme.
- Ideales Modell und Messmodell bleiben getrennt, sodass ihr Einfluss vergleichbar ist.

## 7.7 Weitere Physikdomänen

Erst nach stabiler Mechanik- und Datenbasis:

- **Thermodynamik:** Wärmefluss, Zustandsgrößen, ideale und später reale Gase.
- **Elektromagnetismus:** Felder, Ladungen, einfache Schaltungen.
- **Wellen/Optik:** Oszillatoren, Wellenausbreitung, geometrische Optik.
- **Strömung:** zunächst Teilchen-/Netzmodelle für Lehrzwecke; erst später ernsthafte numerische Fluidmodelle.

CFD und FEM müssen jeweils als eigene große Teilprojekte geplant werden. Sie sollten nicht als Nebenfunktion in den ersten Kern gezwängt werden.

---

## 8. Projekt- und Datenformate

## 8.1 Projektordner

```text
my_experiment/
├─ physim.project             # menschenlesbare Projektbeschreibung
├─ experiment/
│  ├─ main.c
│  └─ experiment.h
├─ analysis/
│  ├─ main.c
│  └─ reports/
├─ assets/
├─ runs/                      # standardmäßig nicht versioniert
└─ build/                     # nicht versioniert
```

Die Projektdatei enthält nur stabile Einstellungen: Formatversion, Quellen, benötigte Physim-Module, Parameterdefinitionen, Buildprofil und Anzeigevorgaben. Absolute lokale Pfade werden vermieden.

## 8.2 Laufdatei `.psrun`

Empfohlen ist ein eigenes, chunk-basiertes Binärformat:

- Header mit Magic, Formatversion und Endianness.
- Projekt- und Experiment-ID.
- API-, Compiler- und Build-Informationen.
- Startzeit, Simulationszeit, Zeitschritt und Seed.
- Parameter, Material-/Mediumdefinitionen und Einheiten.
- Kanalschema.
- append-only Datenblöcke mit Prüfsumme.
- Ereignisse und Warnungen.
- Abschlussindex, der nach einem Absturz rekonstruierbar ist.

Regeln:

- Unbekannte Chunk-Typen werden übersprungen.
- Bestehende Dateien werden niemals stillschweigend inkompatibel überschrieben.
- CSV ist Exportformat, nicht primäres Speicherformat, weil Typen, Einheiten und Metadaten verloren gehen würden.
- Optionales Komprimieren kommt später und bleibt austauschbar.

---

## 9. Realitätsnahe Modellierung

Physim braucht kein einziges globales „realistisch“-Kontrollkästchen. Stattdessen wird ein Modell aus expliziten Schichten zusammengesetzt:

1. **Geometrie und Anfangszustand**
2. **ideale physikalische Gesetze**
3. **Material und Medium**
4. **Verlust- und Störmodelle**
5. **numerische Methode und Toleranzen**
6. **Messgerät und Unsicherheit**
7. **Parameterverteilungen und Monte Carlo**

Beispiel Stoßexperiment:

- Ideal: Punktmassen, vollständig elastischer Stoß, keine Reibung.
- Erweitert: starre Körper, Restitution, Oberflächenreibung, Luftwiderstand.
- Praxisnah: unsichere Massen, nicht exakt bekannte Anfangsgeschwindigkeit, Sensorrauschen, temperaturabhängiges Material.

Die UI zeigt für jeden Lauf, welche Schichten aktiv sind. Jede Vereinfachung soll sichtbar und dokumentierbar sein.

---

## 10. Eigene kompilierte Physim-Sprache – verbindliches Ziel

Physim erhält eine **eigenständige Programmiersprache als vollständige Alternative zu C innerhalb des Projekts**. Sie dient sowohl zum Programmieren beliebiger Physim-Experimente als auch für die gesamte Auswertung: gespeicherte Läufe lesen, Daten transformieren, numerisch rechnen, Parameterstudien und Monte Carlo ausführen sowie Kennzahlen, Tabellen und Diagramme erzeugen. Sie ist kein Formeleditor und keine auf einzelne Aufgaben begrenzte DSL.

### 10.1 Syntax und Typsystem

- Die Sprache orientiert sich bei `let`/`var`, Funktionen, benannten Feldern und statischen Typen an Swift. Blöcke verwenden verbindlich wie Python einen Doppelpunkt `:` am Ende des Kopfes und Einrückung statt `{}`: Dies gilt für Funktionsdefinitionen, Bedingungen, Schleifen und künftige weitere Blockkonstrukte. Die Einrückung entscheidet über die Zugehörigkeit; Funktionsaufrufe behalten runde Klammern. Pythons dynamisches Laufzeit-Typsystem wird nicht übernommen.
- Lokale Typinferenz spart Schreibarbeit; alle Variablen, Parameter und Rückgabewerte können ausdrücklich typisiert werden. Öffentliche Funktionsschnittstellen tragen explizite Typen.
- Typgebundene Funktionalität verwendet durchgängig Methoden und Eigenschaften wie in Swift/Python: `array.append(value)`, `array.count`, `vector.normalized()` und `dataset.series(name)`. Freie Hilfsfunktionen wie `arrayAppending(array, value)` sind dafür keine öffentliche Sprach-API. Das gilt gleichermaßen für eingebaute Typen, eigene Typen und künftige Bibliotheksbindungen. Konstruktoren und statische Fabriken gehören zum Typ; mathematische und kontextweite freie Funktionen benötigen keine künstliche Klasse.
- Vollständiger Sprachausbau: Zahlen, Bool, Strings, Arrays, eigene Strukturen und Enums, optionale Werte, Funktionen, Bedingungen, Schleifen, Module und generische Datenstrukturen. Fehlerbehandlung und Speicherlebensdauer werden vor der jeweiligen Implementierung spezifiziert.
- Physim-Typen für Vektoren, Matrizen, SI-Größen, Materialien, Körper, Messkanäle, Datenreihen und Berichte sind idiomatisch zugänglich. Statisch bekannte Dimensionsfehler werden beim Kompilieren erkannt; dynamisch geladene Daten bleiben laufzeitgeprüft.
- Komfort darf wissenschaftliche Semantik nicht verdecken: Einheiten, Seed, Zeitschritt, Integrator und Modellannahmen bleiben explizit und reproduzierbar.

### 10.2 Compiler und gemeinsame Laufzeit

- Ahead-of-time-Kompilierung: Typprüfung und Übersetzung erfolgen vor dem Start; ausgeführt wird nativer Code im bestehenden Experiment- beziehungsweise Analyse-Runner. Kein erforderlicher Interpreter und kein Python-/Swift-Runtime-Paket.
- Eigener Lexer, Parser, AST, Namensauflösung und Typprüfung mit verständlichen Diagnosen an der ursprünglichen Quelldatei. Ein C17-Backend ist als erste native Übersetzungsstufe zulässig; generiertes C wird automatisch kompiliert und muss vom Nutzer nicht bearbeitet werden. Die Sprache besitzt ihre eigene Grammatik und Semantik.
- Gemeinsame Physim-Bibliothek, ABI, Datenformate und Prozessisolation verhindern zwei auseinanderlaufende Physikimplementierungen. Compiler-/Sprachversion, Quellcode und Buildoptionen gehören zur Provenienz.
- C bleibt vollständig unterstützt. Projekte wählen die Sprache für Experiment und Analyse unabhängig; C-Läufe lassen sich in der eigenen Sprache auswerten und umgekehrt.
- Editor, Vorlagen, Buildservice, Fehlermeldungen, Debug-Informationen und portable SDK-Pakete unterstützen beide Wege. Neue öffentliche Physim-Funktionen erhalten entsprechende Sprachbindungen.

### 10.3 Umsetzungs- und Abnahmeschritte

1. **LANG-001 – Sprachvertrag:** versionierte Spezifikation mit Grammatik, Typen, Zahlen-/Überlaufregeln, Speicher- und Fehlerkonzept; eigene Sprachversion unabhängig von Bibliotheks-ABI.
2. **LANG-002 – Compilerfrontend:** Lexer, Parser, AST, Gültigkeitsbereiche und Typprüfung; positive und negative Tests einschließlich präziser Quellpositionen und begrenzter Ressourcen.
3. **LANG-003 – Native Ausführung:** C17-Backend und Compiler-CLI; vollständige kleine Programme, vor Ausführung abgewiesene Typfehler, Quelldiagnosen und Integration in den Buildservice.
4. **LANG-004 – Experimente:** Bindungen für Physik, Einheiten, Parameter, Zustand, Szene und Messkanäle; Pendel und Wurf laufen vollständig aus der eigenen Sprache im bestehenden Runner.
5. **LANG-005 – Auswertung:** Dataset-/Series-/Report-Bindungen sowie Batch/Monte Carlo; gespeicherte C- und Sprachläufe werden gleichermaßen verarbeitet, geplottet und exportiert.
6. **LANG-006 – Vollständiger Sprachausbau:** Strukturen, Enums, optionale Werte, Arrays, Module und Generics samt festgelegten Lebensdauer-/Fehlerregeln; keine Pflicht, für unterstützte Physim-Aufgaben C-Code nachzuschreiben.
7. **LANG-007 – Produktabnahme:** Sprachauswahl in beiden Editoren, Vorlagen, Diagnosen, Debugpfad, SDK-Paket und zwei vollständige Dokumentationspfade; End-to-End-Tests auf Windows und Linux.

Abnahme bedeutet Funktionsparität anhand derselben Referenzexperimente und Analysen in beiden Sprachen. Ergebnisse werden mit fachlich begründeten Toleranzen, identischen Seeds und denselben Datenformaten verglichen. Speicherfehler, ungültige Quellen, Compilerabbruch und Runnerfehler dürfen die App nicht beschädigen. Ein Lexer oder ein einzelnes übersetztes Beispiel allein erfüllt dieses Ziel nicht.

Die Umsetzung und ihre aktuellen Grenzen werden getrennt vom Ziel in `docs/status.md` nachgeführt; Sprachentwurf und Compilervertrag stehen in `docs/language.md`.

---

## 11. Schritt-für-Schritt-Roadmap

Jede Phase endet mit einem vorführbaren Ergebnis und darf erst abgeschlossen werden, wenn ihre Akzeptanzkriterien erfüllt sind.

## Phase 0 – Projektgrundlage und Entscheidungen

### Aufgaben

- Repository und Verzeichnisstruktur anlegen.
- C17, Formatierungsregeln, Namenskonventionen und Fehlerbehandlung festlegen.
- Lizenz für Physim bestimmen.
- unterstützte Mindestversionen von Windows, Linux, Compiler und OpenGL festlegen.
- UI-Stack in zwei kleinen Spikes prüfen: Fenster/Texteditor sowie 3D-Dreieck/ImGui-ähnliches Docking.
- Architecture Decision Records für UI, Renderer, Compilerstrategie, Prozesse und Datendatei schreiben.
- CI für Windows und Linux einrichten.
- Sanitizer-Build auf Linux und Debug-Checks auf Windows.

### Abnahme

- „Hello Window“ und Konsolenbibliothek bauen auf beiden Plattformen.
- Ein Test läuft in beiden CI-Jobs.
- Abhängigkeiten und Lizenzen sind dokumentiert.

## Phase 1 – Kernbibliothek

### Aufgaben

- Basisdatentypen, Fehlercodes, Allocator und Logging.
- Vektor-/Matrix-/Quaternion-Bibliothek.
- Einheiten- und Dimensionsmetadaten.
- deterministischer RNG.
- erste ODE-Integratoren.
- Unit- und Referenztests.

### Abnahme

- Pendel und Wurfparabel laufen als reine Konsolenprogramme.
- Energie- und Trajektorienfehler liegen innerhalb dokumentierter Toleranzen.
- Gleicher Seed erzeugt auf derselben Plattform reproduzierbare Werte.
- Valgrind/AddressSanitizer beziehungsweise Windows-Debuglauf melden keine Speicherfehler.

## Phase 2 – Experiment-ABI und Runner

### Aufgaben

- versionierte Experiment-API definieren.
- Beispiel als dynamisches Modul bauen.
- `physim-runner` implementieren.
- Prozessstart, Pipe-Protokoll, Heartbeat, Pause, Step, Stop und Fehlertransport.
- Log- und Crash-Erfassung.
- harte Obergrenzen für Nachrichtengrößen.

### Abnahme

- Runner lädt ein Pendelmodul und liefert Zustände.
- Pause und Einzelschritt sind deterministisch.
- Absichtlicher Crash beendet nur den Runner.
- Endlosschleife kann durch die App/Steueranwendung beendet werden.
- ABI-Versionskonflikt erzeugt eine klare Meldung.

## Phase 3 – Datenkanäle und `.psrun`

### Aufgaben

- Kanalregistrierung, Schema und Einheiten.
- chunk-basierten Writer und Reader bauen.
- inkrementelles Speichern und Recovery.
- CSV-Export.
- Datensatzprüfung und Golden Files.

### Abnahme

- Position, Geschwindigkeit und Energie werden live geschrieben.
- Ein abgebrochener Lauf lässt sich bis zum letzten vollständigen Block öffnen.
- Millionen Messpunkte können blockweise gelesen werden.
- Windows und Linux lesen dieselbe Testdatei identisch.

## Phase 4 – App-Shell und Navigation

### Aufgaben

- Hauptfenster, Menü, Projekt öffnen/erstellen.
- drei Arbeitsbereiche Entwickeln, Simulieren und Auswerten.
- Docking-/Panel-Grundsystem, Einstellungen und Theme.
- Hintergrundjobs und zentrales Meldungssystem.
- Wiederherstellung der letzten Fensteraufteilung.

### Abnahme

- Wechsel zwischen allen drei Bereichen ohne Zustandsverlust.
- Oberfläche bleibt während eines Hintergrundjobs reaktionsfähig.
- Skalierung funktioniert bei üblichen DPI-Einstellungen.

## Phase 5 – Experiment-Editor und Build Service

### Aufgaben

- Texteditor-MVP, Projektbaum und Dateiverwaltung.
- Compilererkennung und Buildprofile.
- sichere Argumentlisten ohne Shell-String-Verkettung.
- Diagnoseparser für Clang/GCC und später MSVC.
- Beispielvorlagen und API-Browser.

### Abnahme

- Neues Projekt → Vorlage → Build funktioniert auf Windows und Linux.
- Syntaxfehler erscheint mit Datei, Zeile und Spalte.
- Erfolgreicher Build erzeugt ein ladbares Experimentmodul.
- Pfade mit Leerzeichen und Nicht-ASCII-Zeichen sind getestet.

## Phase 6 – 3D-Renderer und Simulationsfenster

### Aufgaben

- Scene-Snapshot-Format.
- Kamera, Grid, Achsen und einfache Beleuchtung.
- Primitive: Punkt, Linie, Pfeil, Kugel, Box, Ebene, Polyline und Textlabel.
- Runner an UI anbinden.
- Zeitleiste, Live-Werte und Steuerung.
- getrennte Simulations- und Renderfrequenz.

### Abnahme

- Pendel, Wurfparabel und 2D-Stoß werden räumlich dargestellt.
- Pause, Einzelschritt, Reset und Abbruch funktionieren aus der UI.
- 2D-Szene kann orthografisch und perspektivisch betrachtet werden.
- Langsame Darstellung verändert nicht das Simulationsergebnis.

**Ergebnis dieses Meilensteins: erster echter vertikaler MVP.**

## Phase 7 – Analysebereich

### Aufgaben

- Analyse-ABI und separaten Runner.
- Dataset-/Series-API.
- Statistik, Filter, Ableitung und Integral.
- Plot-Datenmodell und erster Plot-Renderer.
- Tabellen-, CSV- und Bildexport.
- Reproduzierbarkeitsmanifest.

### Abnahme

- Analysecode lädt den letzten Pendellauf.
- Er berechnet Energiefehler und Periodendauer.
- Plot und Tabelle erscheinen in der App.
- Analysefehler oder Crash beeinträchtigen die App nicht.

## Phase 8 – Mechanik-MVP und Medien

### Aufgaben

- Punktmassen, starre Körper, Gravitation, Federn und Dämpfung.
- Kollisionen für Kugel, Ebene und Box.
- Reibung und Restitution.
- Material-/Mediumdatenmodell.
- Luft- und Flüssigkeitswiderstand als wählbare Modelle.
- Debug-Visualisierung für Kräfte, Geschwindigkeit und Kontaktpunkte.

### Abnahme

- Referenzfälle für Impuls- und Energieerhaltung.
- Stoßexperiment kann Vakuum, Luft und benutzerdefiniertes Medium vergleichen.
- aktive Annahmen und Modelle stehen im Laufmanifest.

## Phase 9 – Messunsicherheit und Monte Carlo

### Aufgaben

- Verteilungen und Sensorfehler.
- Batch-/Parameterlauf-Controller.
- Seed-Verwaltung und Parallelisierung.
- Aggregation, Quantile und Histogramme.
- Vergleich Idealmodell gegen Messmodell.

### Abnahme

- Ein Experiment mit unsicheren Anfangswerten ist exakt wiederholbar.
- Mehrere Seeds liefern erwartete statistische Verteilungen.
- Analyse zeigt Mittelwert, Streuung und Vertrauensbereich.

## Phase 10 – Stabilisierung und öffentliche Alpha

### Aufgaben

- Projektmigration zwischen Formatversionen.
- Crash Recovery, Autosave und Backup.
- Tastaturbedienung und grundlegende Barrierefreiheit.
- Profiling von CPU, RAM, Datendurchsatz und Renderzeit.
- Paketierung, Lizenztexte, Tutorials und Beispielsammlung.
- API-Dokumentation und Versionsstrategie.
- Fuzzing für Dateileser und IPC-Decoder.

### Abnahme

- Clean-Machine-Test auf Windows und mindestens zwei Linux-Distributionen.
- Tutorial „Projektile motion“ funktioniert ohne internes Wissen.
- Keine bekannten kritischen Datenverlust- oder Absturzfehler.
- Öffentliche API und Dateiformat sind versioniert.

## Phase 11 – Eigene Sprache und gleichwertiger C-Workflow

### Aufgaben

- LANG-001 bis LANG-007 aus Abschnitt 10 umsetzen; der vorhandene C-Durchstich dient als Referenz.
- Compiler, Sprachbindungen, Editor, Paketierung und Dokumentation gemeinsam ausbauen.
- Alle Lernpfade aus Abschnitt 16 in beiden Sprachen bereitstellen.

### Abnahme

- Nutzer können Experimente und Auswertung vollständig in der eigenen Sprache schreiben, nativ kompilieren und in der App verwenden.
- Gemischte Workflows (Experiment in C, Analyse in eigener Sprache und umgekehrt) bestehen Referenztests.
- Beide Dokumentationsteile und alle Vorlagen werden mit ausgeliefert und in CI geprüft.
- C bleibt ein gleichwertiger, gepflegter Zugang; die Sprache erfüllt sämtliche Abnahmen aus Abschnitt 10.

## Phase 12+ – Domänenausbau

Jede neue Domäne durchläuft separat:

1. fachliche Anforderungen und Referenzquellen.
2. Minimalmodell.
3. analytisch lösbare Vergleichsfälle.
4. numerische Validierung und Konvergenztests.
5. Visualisierungsprimitive.
6. Mess- und Analysebeispiel.
7. Dokumentation der Grenzen.

Reihenfolgeempfehlung: Thermodynamik → einfache Elektromagnetik → Wellen/Optik → Strömungslehre. Strömung wird bewusst spät eingeplant, weil stabile und glaubwürdige CFD numerisch sehr anspruchsvoll ist.

---

## 12. Konkreter erster vertikaler Anwendungsfall

Das erste vollständige Experiment ist ein **Pendel mit optionalem Luftwiderstand**.

Warum dieses Beispiel:

- einfaches Idealmodell und bekannte Näherungslösung.
- 2D-Bewegung, die im 3D-Viewer gut sichtbar ist.
- benötigt Zeitschritt, Integrator und Einheiten.
- erzeugt Position, Geschwindigkeit, Winkel und Energie als Messkanäle.
- erlaubt Vergleich von Euler, symplektischem Euler und RK4.
- Luftdichte und Widerstand zeigen das Medienkonzept.
- Rauschen am Winkelsensor zeigt Messunsicherheit.
- Analyse kann Periodendauer, Dämpfung und Energiefehler bestimmen.

Der Ende-zu-Ende-Ablauf lautet:

1. Nutzer erstellt ein Pendelprojekt.
2. Vorlage importiert nur benötigte Physim-Header.
3. Nutzer ändert Länge, Masse oder Integrator.
4. Build Service kompiliert das Experiment.
5. Experiment Runner startet isoliert.
6. Simulationsansicht zeigt Pendel, Kraftvektoren und Live-Werte.
7. Messkanäle werden in `.psrun` geschrieben.
8. Nutzer wechselt zu Auswerten.
9. Analysecode lädt den Lauf und berechnet Periodendauer sowie Energiefehler.
10. App zeigt Plot und Tabelle und exportiert auf Wunsch CSV/Bild.

Erst wenn dieser Ablauf vollständig und stabil funktioniert, wird die Physikbibliothek breit ausgebaut.

---

## 13. Qualitätssicherung

### Testebenen

- **Unit-Tests:** Datenstrukturen, Mathematik, Einheiten, Parser und Serialisierung.
- **Referenztests:** Vergleich mit analytischen Lösungen.
- **Konvergenztests:** Fehler muss bei kleinerem Zeitschritt erwartbar sinken.
- **Erhaltungstests:** Energie, Impuls und Drehimpuls, sofern das Modell sie erhalten muss.
- **Property-Tests:** Invarianten für zufällige Eingaben.
- **Golden Tests:** Dateien, Protokollnachrichten, Plots und ausgewählte Renderbilder.
- **Integrationstests:** Editor-Build-Runner-Daten-Analyse-Kette.
- **Fehlertests:** kaputte Module, abgeschnittene Dateien, ungültige IPC-Nachrichten und Runner-Crash.
- **Performance-Tests:** Schritte pro Sekunde, Kanaldurchsatz, Ladezeit und Speicherverbrauch.

### Numerische Regeln

- Kein Test vergleicht Fließkommazahlen blind exakt, außer bitgenaue Reproduzierbarkeit ist ausdrücklich garantiert.
- Toleranzen werden fachlich begründet.
- Jeder Solver dokumentiert Stabilitätsgrenzen und geeignete Einsatzfälle.
- Referenzwerte enthalten Herkunft und Berechnungsmethode.
- Unterschiede zwischen Plattformen werden gemessen und dokumentiert.

### CI-Matrix

- Windows: MSVC und Clang-Cl, mindestens Debug und Release.
- Linux: GCC und Clang, Debug mit Address-/UndefinedBehavior-Sanitizer.
- Formatprüfung, statische Analyse, Unit-Tests, Integrationstests und Beispiel-Builds.

---

## 14. Sicherheits- und Robustheitsplan

- Benutzercode in beiden Sprachen gilt grundsätzlich als unsicher; statische Typprüfung ersetzt keine Prozessisolation oder OS-Sandbox.
- Runner laufen als getrennte Prozesse mit eigenem Arbeitsverzeichnis.
- Die App baut Compileraufrufe als Argumentlisten, nicht als Shell-Befehlsstrings.
- Dateizugriff wird zunächst per Projektkonvention begrenzt; echtes OS-Sandboxing folgt plattformspezifisch.
- Runner-Nachrichten werden auf Typ, Länge, Version und zulässige Werte geprüft.
- Zeit- und Speicherlimits werden konfigurierbar.
- Stop versucht zuerst geordneten Abbruch und beendet danach den Runner hart.
- Autosave verwendet atomare temporäre Datei plus Umbenennung.
- Laufdaten sind append-only; beschädigte Endblöcke können verworfen werden.
- Keine automatische Ausführung eines heruntergeladenen Projekts ohne Bestätigung.

Wichtig: Prozessisolation allein ist keine vollständige Sicherheits-Sandbox. Diese Einschränkung muss in frühen Versionen klar kommuniziert werden.

---

## 15. API- und Coding-Regeln

- Öffentliche Namen beginnen mit `ps_`.
- Öffentliche Strukturen besitzen Versions- oder Größenfelder, wenn sie über Modulgrenzen gehen.
- Keine globalen veränderbaren Singletons in der Bibliothek.
- Besitzregeln für Speicher stehen an jeder öffentlichen Funktion.
- Fehler werden als Code plus abrufbare strukturierte Diagnose transportiert.
- Keine versteckten Einheiten; öffentliche Felder nennen die SI-Größe oder tragen Dimensionsdaten.
- Header sind einzeln includierbar.
- Module können gezielt eingebunden werden; ein Experiment muss nicht die gesamte Bibliothek linken.
- Interne Implementierungsdetails bleiben außerhalb von `include/physim`.
- ABI und Quelldatei-API erhalten getrennte Versionsnummern.

---

## 16. Dokumentation und Beispiele

Die Anwenderdokumentation wird mit der Sprachintegration in zwei vollständige Teile gegliedert:

- **Teil I – Physim mit C:** Einrichtung, Spracheinstieg für den Workflow, C-API, Experimente, Messdaten, Auswertung, Debugging und Beispiele.
- **Teil II – Physim mit der eigenen Sprache:** Einrichtung, Syntax und statische Typen einschließlich Inferenz und expliziter Annotationen, Speicher-/Fehlerregeln, Physim-Bindungen, Experimente, Messdaten, Auswertung, Debugging und dieselben Beispiele.

Beide Teile decken den gesamten Weg vom Quellcode bis zum Ergebnis ab. Gemeinsame Modellannahmen, Dateiformate und physikalische Referenzen werden verlinkt und bleiben konsistent. Ausführbare Dokumentationsbeispiele werden für beide Sprachen automatisch gebaut und geprüft. Geplante Syntax wird bis zu ihrer Implementierung ausdrücklich als Entwurf gekennzeichnet.

Mindestens diese Lernpfade werden gepflegt:

1. Wurfparabel im Vakuum.
2. Wurfparabel mit Luftwiderstand.
3. Pendel und Vergleich mehrerer Integratoren.
4. elastischer und inelastischer Stoß.
5. Feder-Masse-Dämpfer.
6. unsichere Anfangswerte mit Monte Carlo.
7. Analyse eines gespeicherten Laufs.
8. eigenes Material und eigenes Medium.

Jedes Beispiel enthält:

- Lernziel.
- Modellannahmen.
- Gleichungen.
- verwendete Physim-Module.
- vollständigen Quellcode in C und in der eigenen Sprache, sobald deren jeweiliger Workflow implementiert ist.
- erwartete Ergebnisse.
- Grenzen des Modells.
- mindestens einen automatisierten Test.

---

## 17. Hauptrisiken und Gegenmaßnahmen

| Risiko | Auswirkung | Gegenmaßnahme |
|---|---|---|
| Umfang „gesamte Physik“ | Projekt wird nie fertig | Vertikaler MVP und Domänen einzeln freigeben |
| eigener Editor wird zu groß | UI verschlingt Entwicklungszeit | zuerst robuster Minimaleditor, IDE-Komfort später |
| Compilerunterschiede | Builds funktionieren nur lokal | Compileradapter und CI-Matrix ab Phase 0 |
| Benutzercode stürzt ab | Datenverlust/App-Absturz | separater Runner, Autosave, append-only Daten |
| Echtzeitdarstellung verfälscht Physik | falsche Resultate | feste Physikschritte, Rendering entkoppeln |
| numerische Ergebnisse wirken plausibel, sind aber falsch | Vertrauensverlust | analytische Referenzen, Konvergenz- und Erhaltungstests |
| Dateiformat wird früh unbrauchbar | inkompatible Projekte | Versionierung, Chunks, Migrationstests |
| eigene Sprache bindet Ressourcen | unvollständiger Compiler oder auseinanderlaufende Workflows | verbindliche LANG-Meilensteine, gemeinsame C-Bibliothek, schrittweise Paritätstests und Dokumentation für beide Sprachen |
| Strömung/FEM unterschätzt | jahrelanger Nebenpfad | als eigenständige spätere Domänen behandeln |
| „keine Abhängigkeiten“ kollidiert mit Compiler/GUI | unklare Distribution | Library-, Build- und Laufzeitabhängigkeiten getrennt dokumentieren |

---

## 18. Organisation des Backlogs

Jedes Arbeitspaket erhält:

- eindeutige ID, zum Beispiel `CORE-MATH-014`.
- Ziel und Nutzerwert.
- technische Abhängigkeiten.
- konkrete Akzeptanzkriterien.
- Tests.
- betroffene Dokumentation.
- Plattformkennzeichnung Windows/Linux/beide.

Empfohlene Epics:

- `FOUNDATION`
- `CORE`
- `UNITS`
- `NUMERICS`
- `EXPERIMENT-ABI`
- `RUNNER`
- `DATA`
- `APP-SHELL`
- `EDITOR`
- `RENDERER`
- `SIM-UI`
- `ANALYSIS`
- `MECHANICS`
- `MATERIALS-MEDIA`
- `UNCERTAINTY`
- `PACKAGING`
- `DOCS`

### Definition of Ready

Ein Ticket darf begonnen werden, wenn Ziel, Grenzen, Abhängigkeiten und prüfbare Akzeptanzkriterien feststehen.

### Definition of Done

Ein Ticket ist fertig, wenn:

- Code auf Windows und Linux baut, sofern nicht explizit plattformspezifisch.
- relevante Tests vorhanden und grün sind.
- Compiler keine neuen Warnungen meldet.
- Fehlerpfade und Speicherbesitz geprüft sind.
- öffentliche API dokumentiert ist.
- Beispiel oder Nutzeroberfläche aktualisiert ist, falls sichtbar.
- keine temporären Debug-Hacks verbleiben.

---

## 19. Die ersten 25 umsetzbaren Tickets

1. `FOUNDATION-001`: Repository-Struktur und CMake-Targets anlegen.
2. `FOUNDATION-002`: C17-Regeln, Formatierung und Warnungsprofil definieren.
3. `FOUNDATION-003`: Windows-/Linux-CI mit leerem Testziel einrichten.
4. `FOUNDATION-004`: UI-Stack-Spike für Fenster, Text und DPI durchführen.
5. `FOUNDATION-005`: OpenGL-Spike mit 3D-Kamera durchführen.
6. `CORE-001`: Fehlercode- und Diagnosesystem implementieren.
7. `CORE-002`: Allocator-Schnittstelle und Test-Allocator implementieren.
8. `CORE-MATH-001`: 2D-/3D-Vektoren samt Tests implementieren.
9. `CORE-MATH-002`: Matrizen, Quaternionen und Transformationen implementieren.
10. `UNITS-001`: Einheitendimensionen und SI-Konvertierung definieren.
11. `CORE-RNG-001`: deterministischen RNG samt Seed-Metadaten implementieren.
12. `NUMERICS-001`: Euler, symplektischen Euler und RK4 implementieren.
13. `EXPERIMENT-ABI-001`: versionierten Experiment-Deskriptor spezifizieren.
14. `RUNNER-001`: Experimentmodul laden und Callback ausführen.
15. `RUNNER-002`: Pipe-Protokoll und Versions-Handshake implementieren.
16. `RUNNER-003`: Pause, Step, Resume und Stop implementieren.
17. `DATA-001`: Messkanal-Schema definieren.
18. `DATA-002`: `.psrun`-Header und Datenblock schreiben/lesen.
19. `DATA-003`: Recovery einer abgeschnittenen Laufdatei implementieren.
20. `APP-SHELL-001`: App-Fenster mit drei Arbeitsbereichen bauen.
21. `EDITOR-001`: Texteditor-MVP und Projektbaum integrieren.
22. `EDITOR-002`: Compilererkennung und Build Service anbinden.
23. `RENDERER-001`: Scene Snapshot und Grundprimitive rendern.
24. `SIM-UI-001`: Pendel-Runner, Steuerung und Live-Werte verbinden.
25. `ANALYSIS-001`: Pendeldaten laden und ersten Energieplot erzeugen.

Diese Liste ist die empfohlene tatsächliche Startreihenfolge. Tickets dürfen parallelisiert werden, wenn ihre genannten Grundlagen bereits stabil sind.

---

## 20. Versions- und Releaseplan

- **0.0.1 – Foundation:** Builds, Tests, Fenster, Mathematikkern.
- **0.1.0 – Vertical MVP:** Pendel von C-Code über Runner und 3D-Ansicht bis Analyseplot.
- **0.2.0 – Mechanics Preview:** starre Körper, erste Kollisionen, Materialien und Medien.
- **0.3.0 – Measurement Preview:** Messmodelle, Unsicherheit und Monte Carlo.
- **0.4.0 – Authoring Preview:** besserer Editor, API-Browser, Vorlagen und Projektmigration.
- **0.5.0 – Public Alpha:** Pakete, Tutorials, Crash Recovery und stabilisierte Formate.
- **0.6.0 – Language Preview:** spezifizierter Sprachkern, statische Typprüfung, native Kompilierung und erstes vollständiges Experiment samt Auswertung; verbleibende Sprachlücken explizit dokumentiert.
- **1.0.0:** stabile Kern-API, dokumentierte Kompatibilitätsregeln, belastbare Mechanik-, Mess- und Analysefunktionen auf Windows und Linux sowie eigene Sprache als vollständige Alternative zu C für den Physim-Workflow einschließlich beider Dokumentationsteile und LANG-001 bis LANG-007.

Versionsnummern sind an Fähigkeiten und Qualitätskriterien gebunden, nicht an Kalendertermine.

---

## 21. Entscheidungen, die vor dem ersten Code verbindlich getroffen werden müssen

1. Open-Source- oder proprietäre Lizenz.
2. minimale Windows- und Linux-Versionen.
3. SDL3/Nuklear oder alternative reine-C-UI-Kombination nach dem Spike.
4. OpenGL 3.3 als erster Renderer oder eine andere ausdrücklich gewählte Basis.
5. unterstützte Compiler der ersten Alpha.
6. ob die erste Distribution einen Compiler mitliefert oder ihn nur erkennt.
7. Genauigkeit und Reproduzierbarkeitsversprechen zwischen Plattformen.
8. Größenordnung der Zieldatensätze und gewünschten Simulationsraten.

Diese Entscheidungen werden als kurze Architecture Decision Records dokumentiert. Sie blockieren nicht die fachliche Planung, verhindern aber spätere widersprüchliche Implementierungen.

---

## 22. Erfolgskriterien für Physim 1.0

Physim 1.0 gilt als erreicht, wenn ein neuer Nutzer auf Windows oder Linux ohne Änderung am Physim-Quellcode:

1. ein Beispielprojekt anlegen kann,
2. ein Experiment wahlweise in C oder der eigenen Sprache mit ausgewählten Physim-Modulen nativ kompilieren kann,
3. es isoliert ausführen, pausieren und abbrechen kann,
4. die 2D-/3D-Szene und relevante Live-Werte sehen kann,
5. vollständige Messdaten zuverlässig speichern kann,
6. diese in einem getrennten Analyseprojekt wahlweise in C oder der eigenen Sprache laden kann, unabhängig von der Sprache des Experiments,
7. Kennzahlen, Tabellen und Plots erzeugen kann,
8. ein ideales Modell mit einem medium-, material- und unsicherheitsbehafteten Modell vergleichen kann,
9. den Lauf anhand von Quellcode, Parametern, Seed und Build-Metadaten reproduzieren kann,
10. nachvollziehen kann, welche Modellannahmen und numerischen Grenzen gelten,
11. für beide Sprachen eine vollständige, getestete Dokumentation vom Einstieg bis zum eigenen Experiment und zur eigenen Auswertung findet.

Damit ist das Fundament geschaffen, auf dem weitere Physikdomänen ohne Umbau der gesamten Anwendung ergänzt werden können.
