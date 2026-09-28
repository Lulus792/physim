# Bauen und testen

## Projekte in der App bauen

Physim erzeugt `physim.project` beim Anlegen und pflegt die Projekteinstellungen.
F5 startet den mitgelieferten `physim-build`, der C- und Physim-Quellen direkt über
den C17-Compiler in Experiment- und Analysemodule übersetzt. Eine `CMakeLists.txt`
oder CMake-Installation wird dafür nicht benötigt. Bestehende CMake-Dateien
werden nicht gelesen, überschrieben oder automatisch gelöscht.

Das unter **Build-Einstellungen** gewählte Profil wird beim Speichern, Bauen oder
normalen Beenden als `profile=Debug` beziehungsweise `profile=Release` in der
Projektdatei gespeichert. Beim erneuten Öffnen wird es zusammen mit den
Experimentparametern wiederhergestellt. Ältere Projektdateien ohne Profileintrag
verwenden Debug. Auch `physim-build` liest dieses Profil; ein explizites
`--profile` überschreibt es für den jeweiligen Aufruf.
App und Builder verwenden denselben Leser für das Projektformat. Doppelte oder
ungültige bekannte Einträge werden abgewiesen. Kommentare und zusätzliche
Projekteinträge bleiben beim Speichern erhalten; ein unverändertes Speichern
schreibt die Projektdatei nicht erneut. Geänderte Projektdateien werden mit
Sicherungskopie und Prüfung auf Änderungen während des Schreibvorgangs ersetzt.

Auch Zeitschritt und Zufallsseed werden als `simulation.dt` und `simulation.seed`
in der Projektdatei gespeichert und beim Öffnen wiederhergestellt. Ohne diese
Einträge gelten 0,005 Sekunden und Seed 42. Der Zeitschritt muss eine positive,
normale endliche Gleitkommazahl bis einschließlich einer Sekunde sein. Seeds
werden als Dezimalzahl von 0 bis 18446744073709551615 akzeptiert. Ungültige Werte
blockieren Speichern und Laufstart. Änderungen dieser Laufwerte benötigen
keinen neuen Modulbuild; jeder Lauf archiviert die verwendeten Werte.

Unter Windows werden Visual Studio 2022 C++ Build Tools und ein Windows SDK
automatisch gefunden; eine Developer-Konsole ist nicht nötig. Unter Linux und macOS wird
`cc` verwendet. Die Umgebungsvariable `PHYSIM_CC` kann einen anderen Compiler
angeben, etwa `clang-cl.exe` unter Windows oder `clang` unter Linux und macOS.
Auf macOS stellen die Xcode Command Line Tools (`xcode-select --install`) den
Compiler und das System-SDK bereit.

Der Projektordner behält seine Quellen, `physim.project` und Ergebnisse in `runs/`.
`build/Debug` und `build/Release` enthalten Module, generiertes C, Objektdateien,
Debugsymbole und den Buildzustand. Physim vergleicht vorverarbeitete Quellen,
um auch Änderungen an indirekt eingebundenen Headern zu erkennen. Unveränderte
Objekte und Module werden wiederverwendet. Ein fehlgeschlagener Compiler- oder
Linkeraufruf veröffentlicht keine neuen Module; der nächste Build holt das Linken
nach. Gleichzeitige Builds im selben Ausgabeordner werden abgewiesen.
Quellen mit zeitabhängigen Makros wie `__TIME__` können bei jedem Build erneut
übersetzt werden, weil sich ihr vorverarbeiteter Inhalt ändert.

Die nachfolgenden CMake-Anleitungen betreffen den Bau von Physim selbst und die
bisherige SDK-Anbindung. Deren vollständige Ablösung bleibt ein eigenes Ziel.

## Toolchains

Windows: CMake 3.24+, Visual Studio 2022 mit C/C++-Desktop-Workload und Windows SDK.
Linux: GCC oder Clang, CMake 3.24+, Make/Ninja. Der Core braucht nur libc, libm
und die jeweilige Betriebssystemschicht. SDL3 und Nuklear werden nur für die App benötigt.
Die App setzt einen OpenGL-3.3-Core-Treiber voraus. Grafiktests auf Linux-Servern
können Mesa mit Xvfb verwenden; der CI-Workflow konfiguriert dies. Für den echten
Maximierungs-/Wiederherstellungstest startet er zusätzlich den Fenstermanager Openbox.

Nuklear ist unter `third_party` eingecheckt. SDL3 für Windows wird aus dem offiziellen
3.2.30-Release bezogen. Das Bootstrap-Skript prüft SHA-256 vor dem Entpacken.

Für Linux und macOS SDL3 aus derselben Version bauen:

```sh
git clone --depth 1 --branch release-3.2.30 https://github.com/libsdl-org/SDL.git build-sdl-source
cmake -S build-sdl-source -B build-sdl -DCMAKE_BUILD_TYPE=Release -DSDL_TESTS=OFF -DCMAKE_INSTALL_PREFIX="$PWD/build-sdl-install"
cmake --build build-sdl --parallel
cmake --install build-sdl
cmake -S . -B build -DCMAKE_PREFIX_PATH="$PWD/build-sdl-install" -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

SDL benötigt systemabhängig X11-/Wayland-Entwicklungspakete. Der CI-Workflow zeigt
die Ubuntu-Pakete. Bibliothek und Runner können mit `PHYSIM_BUILD_APP=OFF` unabhängig
von sämtlichen UI-Paketen gebaut werden.

macOS verwendet Apple Clang, die Xcode Command Line Tools und OpenGL 4.1 Core.
Die obigen Befehle bauen jeweils für die Architektur des Macs (Apple Silicon oder
Intel). Für Fensterprüfungen beim Konfigurieren `-DPHYSIM_GRAPHICS_TESTS=ON`
ergänzen und die Tests in einer angemeldeten grafischen Sitzung ausführen.
Die App startet mit `./build/bin/physim`. Für Speichern, Suche und Editorbefehle
gilt auf macOS **Cmd** anstelle von **Ctrl**. Die aktuellen Plattformnachweise
stehen in [Plattformprüfung](platform-validation.md).

## Tests

Das Handbuch unter `docs/guide.md` und seine Funktionsreferenzen werden mit dem SDK
installiert. Nach Änderungen an öffentlichen Funktionen oder Sprachbindungen:

```powershell
python tools/generate-reference.py
python tools/generate-reference.py --check
```

Der Generator prüft für jede C-Funktion einen deutschen Erklärungstext und übernimmt
Signaturen, Typen und SDK-Verträge. Sprachmethoden werden mit der tatsächlichen
Empfänger-/Fabrikschreibweise dokumentiert. Das C-Tutorial übernimmt seine Codeblöcke
aus `examples/documentation`; `documentation_example` prüft Bewegung, Szene, Reset
und die daraus berechnete Geschwindigkeit. `documentation_reference` erkennt
veraltete Referenzen und Tutorialcode (wenn Python verfügbar ist).
`documentation_window` prüft mit aktivierten Grafiktests alle registrierten Seiten,
Suche und das unabhängige Hilfefenster.

### Installiertes SDK prüfen

Nach einem vollständigen Build prüft dieser plattformunabhängige Ablauf auch
die installierten Quellen und Werkzeuge:

```sh
cmake -DBUILD_DIR=build -DCONFIG=Release -P tools/verify-sdk.cmake
```

`CONFIG` muss zur gebauten Konfiguration passen, beispielsweise `Debug` auf
Linux oder beim entsprechenden Visual-Studio-Build. Generator und Compiler
werden aus dessen CMake-Cache übernommen; bei Ninja muss die ursprüngliche
Compilerumgebung weiterhin eingerichtet sein.

Das Skript installiert in einen neuen Ordner mit Leerzeichen und Umlaut,
verschiebt das SDK und baut einen separaten Verbraucher mit dem installierten
`PhysimExperiment.cmake`. Alle acht Vorlagen und das Analysemodul werden aus den
installierten Quellen neu kompiliert; jeder öffentliche Header wird in einer
eigenen Übersetzungseinheit eingebunden. Die installierten Runner führen jede
Vorlage aus. Eine gegen das SDK gebaute Prüfung liest alle 201 Samples, prüft
Zeitpunkte und endliche Werte und lädt den erzeugten Bericht samt CRC-Prüfung.
Zusätzlich werden alle installierten Sprachbeispiele über `PhysimLanguage.cmake`
neu gebaut, wobei der Compiler aus dem verschobenen SDK gefunden werden muss.
Sieben eigenständige Sprachprogramme laufen mit ihren eingebauten Referenzprüfungen.
Sprach-Pendel, -Wurf und -Feder werden jeweils mit beiden Sprach-Analysemodulen
ausgewertet; die Prüfung kontrolliert Messzeilen und die erwarteten Berichtsplots.
Dies ist eine Paket-/Integrationsprüfung; die physikalischen Referenztests
bleiben zusätzlich erforderlich.

Unter `build/SDK Test ä <Kennung>/` bleiben `verification.log`, erzeugte Dateien
und bei Erfolg `PASSED.txt` erhalten. Wiederholungen verwenden neue Ordner.
Die Windows- und Linux-CI führen den Ablauf aus und archivieren das Protokoll.
Eine konfigurierte CI ist kein Beleg für einen erfolgreich ausgeführten Lauf.

### Fachliche und technische Prüfungen

- Speicher: Multiplikationsüberlauf, Resize-Fehler, exakte Freigabegrößen, Arena-
  Ausrichtung/Grenzen und Wiederverwendung. Ein Test-Allocator löst jeden
  Allokationsfehler im Berichtablauf und Laden aus; bestehende Berichte bleiben
  bytegleich. SVG-/CSV-Export, abgeschnittene Dateien und Analysekontexte mit
  echtem Datensatz werden auf vollständiges Aufräumen geprüft. [Details](memory.md).

- Mathematik: Vec2/3/4 einschließlich subnormaler und maximaler Werte, allgemeine
  Matrixinversion mit beidseitigen Residuen, Quaternionen gegen unabhängige
  Rodrigues-Rotation, TRS und projektive Punkte, Normalen bei Scherung/Spiegelung,
  Aliasierung und unveränderte Ausgaben bei Fehlern. Details: [math.md](math.md).

- Mechanik: Kugel-/Boxträgheit, rotierte Hauptachsen, Drehmoment und Impuls,
  Quaternionrotation, gyroskopischer Term, Konvergenz erster Ordnung,
  Impuls-/Energieerhaltung, inelastischer Stoß, Coulomb-Reibung, Stokes- und
  quadratischer Widerstand, Federn und transaktionale Fehlerbehandlung.
- Stoßreferenz: echte Runner-Läufe im Vakuum, in Luft, eigenem Medium und mit
  Anfangsrotation/Reibung. Alle 401 Samples gegen Impuls-/Energie- und
  Widerstandsreferenzen geprüft, einschließlich Modellmetadaten.
- Boxkontakte: orientierte Kugel–Box-/Box–Ebene-Geometrie, akkumulierte Impulse,
  zweidimensionale Coulomb-Reibung, redundante Punkte und ruhender Bodenkontakt.
  Die echte Boxvorlage läuft zehn Sekunden; alle 2001 Samples werden gegen
  Energiegrenzen, Eindringung und den abschließenden Ruhezustand geprüft.
- Feder–Masse–Dämpfer: echte Läufe für vier Dämpfungsfälle mit je 2001 Samples gegen
  geschlossene Lösungen; Kräfte, Energie, dissipierte Arbeit und Bilanz geprüft.
  RK4-Konvergenz bei Zeitschritthalbierung sowie alle Energiekurven und die
  Bilanzkennzahl im erzeugten Analysebericht werden separat geprüft.

- Core: Quaternionen, Einheiten, reproduzierbarer RNG, Normalverteilungsstatistik,
  RK4-Konvergenz, symplektische Energiegrenze, Impulsaustausch, Ableitung, Integration.
- Daten: CRC-Golden-Vektor, Roundtrip, CSV, Abschneiden an jeder Dateiposition.
- IPC: Teilframes, falsche Größen, Snapshotvalidierung.
- Datenreihen: Blockgrenzen, Einheiten, Handle-Lebensdauer, zeitliche Zuordnung,
  Snapshot-Stabilität, Speicherplatzlimit, Recovery und physikalische CSV-Referenzen.
- Ergebnisberichte: Roundtrip, jedes abgeschnittene Dateipräfix, CRC und 300 Mutationen,
  Einheiten/Skalen, erhaltene Spitzen, vollständige Histogrammzählung und Exporte.
  Der Runner-Bericht wird gegen CSV sowie Energie-/Periodenreferenzen geprüft.
- Dokumentation: Parser, Unicode-Suche und Fehlerfälle. `physim --docs-test <bildordner>`
  prüft zusätzlich das separate Fenster über F1, das Laden aller Themen, Suche über
  SDL-Textereignisse, unabhängige Eingaben, Größenänderung sowie Schließen und
  Wiederöffnen mit erhaltener Leseposition.
- Autosave: gemeinsamer Snapshot beider Quellen und ihrer Ausgangstexte, Unicode,
  maximale Dateigröße, jedes abgeschnittene Präfix, Mutationen und fehlgeschlagene
  temporäre Schreib-/Umbenennungsoperationen. `autosave_workflow` beendet einen
  schreibenden App-Prozess abrupt und startet eine neue Instanz. SDL-Mausereignisse
  prüfen Wiederherstellen/Verwerfen, Konflikthinweis bei externen Änderungen,
  Schließen ohne Auswahl sowie Erhalt beschädigter Sicherungen.
- Runner: echter Handshake, deterministischer Einzelschritt, Pause/Fortsetzung,
  absichtlicher Modulabsturz, Endlosschleife, harte Terminierung und falsche ABI.
- End-to-end: 4000 Pendelschritte und separater Analyse-Runner.
- Laufverwaltung: Unicode-Dateinamen, Sortierung, Dateifilter, Pfadgrenzen und
  Erhalt der alten Liste bei fehlgeschlagener Aktualisierung.
- Mehrlaufanalyse: verschiedene Zeitraster, Ableitungs-/Statistikreferenz,
  interpolierte Differenzkurven und vollständige CSV gegen analytische Referenzen,
  teilweise überlappende Laufzeiten, ein gemeinsamer Zeitpunkt und getrennte Bereiche,
  Eingabemanifest und Ablehnung falscher Dimensionen. Analyseisolation mit echtem
  Absturz, Endlosschleife und kurzem ABI-3-Modul ohne Mehrlauf-Callback.
- Resampling: lineare und quadratische Referenzen, exakte Endpunkte, Blockgrenzen,
  vollständige Monotonieprüfung, ungültige Bereiche/Einheiten, sehr große Werte,
  Einzelpunkt, Lebensdauer und transaktionaler Abbruch bei erschöpftem Plattenbudget.
- Grafik: `physim --renderer-test [bild.bmp]` prüft tatsächliche GPU-Pixel für
  Verdeckung, Projektionen, Größenwechsel, Clipping, Draufsicht und Vektoren.
  Zusätzlich werden UTF-8-Eingabe und Fokusverlust geprüft. Der optionale Bildpfad
  speichert eine Szene mit räumlichen Primitiven einschließlich Rotation und Polyline.
- App: `physim --smoke` zeichnet acht Frames und beendet sich. `physim --self-test
  <neuer-absoluter-projektpfad> [pendulum|projectile|collision|box_floor|spring|uncertain_projectile]` erzeugt ein Projekt, prüft zunächst einen absichtlich
  eingefügten Compilerfehler samt Diagnose, korrigiert und baut es, simuliert, pausiert,
  schaltet einen Einzelschritt, stoppt und analysiert. Editor-, Simulations- und
  Auswertungsansicht werden als BMP im Testprojekt gespeichert. Anschließend prüft
  der Test über injizierte SDL-Ereignisse Arbeitsbereichwechsel, Suchleiste, Protokoll
  und Navigation. Weitere Bilder zeigen Suchleiste und geöffneten Inspector.
  Mit `PHYSIM_TEST_SMALL=1` läuft derselbe Ablauf bei 1080 × 740.
  Er prüft außerdem Ergebnisdiagramme, Tabellen und deren CSV-/SVG-Schaltflächen.
  Anschließend vergleicht er zwei Läufe mit unterschiedlichen Zeitschritten,
  öffnet das Projekt erneut und lädt frühere Berichte sowie Messdaten ohne Build.
  Auswahl, Öffnen und Dateifilter werden über SDL-Maus-/Tastaturereignisse geprüft.
  `PHYSIM_TEST_LONG=1` verlängert die Testsimulation auf 20 s, etwa für mehrere Pendelperioden.

`cmake -S . -B build -DPHYSIM_GRAPHICS_TESTS=ON` nimmt Grafiktest und Fenstersmoke
und Dokumentationsfenster sowie Autosave-Neustarttests und den Monte-Carlo-App-Test
in CTest auf, zusätzlich zu Speicherverwaltung, Mathematik und weiteren Kernprüfungen.
Die konkrete Testliste der gewählten Konfiguration zeigt `ctest -N`.
Standardmäßig ist diese Option aus, sodass
Core-/Runner-Tests ohne Display laufen. Die Grafiktests wurden lokal mit NVIDIA
OpenGL 3.3 ausgeführt. Der Windows-CI-Job prüft Build und Core/Runner; Linux-CI soll
Grafik unter Xvfb/Mesa prüfen. Das CTest-Label `display` erfasst die Tests,
die ein Fenster benötigen: `ctest -LE display` prüft ohne Anzeige, `ctest -L display`
unter Xvfb mit Openbox prüft die Fensterabläufe. Die CI läuft bei jedem Push in
beiden Repositories. Den konkreten Prüfstand zeigt
[GitHub Actions](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Auf dem lokalen Windows-Rechner ist keine WSL-Distribution installiert.

`physim --plot-test <neuer-absoluter-ordner>` prüft Diagrammzoom und Verschieben
über SDL-Ereignisse sowie den PNG-Export per Schaltfläche. Es erzeugt außerdem
`figure-0.png` bis `figure-3.png` und `figure-dense.png` als Bildreferenzen.
Nur in diesem Testmodus werden markierte Test-Mausereignisse verarbeitet;
Desktop-Mausbewegungen und Fokusverlust verändern den Testeingang nicht.
`physim --plot-test-noise <neuer-absoluter-ordner>` streut zusätzlich in jedem
Frame fremde Mausbewegungen, Loslassen der Maustaste und Fokusverlust ein.
CTest führt diesen vollständigen Ablauf als `plot_input_isolation` aus.
Normale App-Eingaben und deren Fokusverlust-Behandlung bleiben unverändert.
`png` prüft den PNG-Schreiber ohne Grafik. Nach CTest lassen sich beide Ergebnisse
mit `python tests/verify_png.py <build-ordner> <plot-test-ordner>` unabhängig
mit Python-zlib und erwarteten Pixelwerten überprüfen; zusätzliche Python-Pakete
sind für diesen Prüfschritt nicht nötig.

`batch_reference` prüft 256 echte Wurfläufe gegen die analytische Endwertverteilung,
dieselbe Serie mit vier Runnern, acht vollständige Wiederholungsläufe mit acht Runnern,
andere Seeds, Statistikreferenzen, Schemafehler, Absturz, Zeitlimit und Abbruch.
Endwert-CSV und numerische Berichtsdaten müssen bei unterschiedlicher Parallelität
bitgleich bleiben. `batch_parallel` erzwingt vier gleichzeitig aktive Prozesse
über eine Barriere sowie einen absichtlich verspäteten ersten Lauf. Er prüft
getrennte Arbeitsordner, lückenhafte Teilergebnisse, Kindprozess-Ende bei Abbruch,
Fehler und starkem stdout-Verkehr mit Zeitlimit sowie die CLI-Option `--workers`.
`physim --batch-test <neuer-absoluter-projektpfad> [2..8]`
baut die Unsicherheitsvorlage und prüft die Laufserie mit injizierten SDL-Klicks
einschließlich Änderung der Parallelität per UI, Bericht, Anleitung, erneutem Start,
Abbruch und Schließen während einer weiteren Serie; Screenshots liegen im Testprojekt.
`batch_workflow` erzeugt dafür bei jeder CTest-Ausführung einen eigenen Projektordner.

Lokal wurde neben MSVC auch Clang-Cl 19.1.5 mit Ninja geprüft. Das optionale
`tools/build-clang.cmd` ist auf den Standardpfad von Visual Studio 2022 Community
eingestellt; bei anderer Edition den Pfad anpassen. Das zusätzliche Visual-Studio-
Toolset `ClangCL` muss für diesen Ninja-Weg nicht installiert sein.

RK4-Test: beim Halbieren von dt von 0.1 auf 0.05 im harmonischen Oszillator wird
ein Fehlerquotient zwischen 14 und 18 erwartet (globale Ordnung 4), absoluter Fehler
bei t=1 unter 1e-7. Symplektischer Test: 10000 Schritte mit dt=0.005 und Energiefehler
unter 0.002 bei Anfangsenergie 0.5. RNG-Momententest: 100000 Werte, Toleranz 0.04 für
Mittelwert 2 und Standardabweichung 3. Diese Toleranzen sind keine pauschale Garantie
für beliebige Experimente.

Der zusätzliche Pendel-Referenztest vergleicht den echten Runner-Datensatz mit
`T = 4 sqrt(L/g) K(sin(theta0/2))`. Zehn Glieder der Binomialreihe für das vollständige
elliptische Integral reichen hier für einen Abschneidefehler unter 1e-12. Die
gemessene Periode darf wegen zeitdiskreter Nulldurchgänge um 2e-6 s abweichen;
die maximale Energieabweichung über 20 Sekunden muss unter 1e-8 J bleiben. Das gilt
für RK4 und RK45. Verlet wird bei demselben dt gegen 5e-5 s Periodenfehler und
1e-4 J Energieabweichung geprüft; sein Konvergenztest bestätigt zusätzlich Ordnung 2.
Weitere Verträge und Tests sind in [numerics.md](numerics.md) beschrieben.

`preferences` prüft das Einstellungsformat, Grenzen, Prüfsumme, alle abgeschnittenen
Dateilängen und die Erhaltung der bisherigen Datei bei Fehlern. `settings_workflow`
startet die App siebenmal mit einem eigenen Einstellungsordner. SDL-Ereignisse prüfen
Schriftwahl, Schalter, Übernehmen, Abbrechen, Standardwerte und Ziehen der Panelgrenzen.
Neustarts prüfen Fenstermaße, Maximierung, Sichtbarkeit und Schrift. Eine reale
Autosave-Datei bestätigt das gewählte Intervall; beschädigte Einstellungen bleiben
unverändert. Die normalen Benutzereinstellungen werden von diesen Tests nicht gelesen.

## AddressSanitizer unter Windows

Die beiden neuen Speichertests können im Quellcheckout zusätzlich unter Windows
mit `tools\check-memory-asan.cmd` gegen MSVC AddressSanitizer ausgeführt werden.
Das Skript verwendet Visual Studio 2022 Community mit installiertem ASan und legt
einen eigenen `build-asan-memory`-Ordner an. Es baut RelWithDebInfo ohne inkrementelles
Linken. Der vollständige Core wird instrumentiert; ausgeführt werden hier nur
`memory` und `memory_owners`, keine vollständige Sanitizer-Abnahme der App.

## Portables Paket

Die Sprachvorlagen lassen sich mit dem vollständigen App-Test prüfen. Jeder Test
benötigt einen neuen, noch nicht belegten Projektordner:

```powershell
.\build\bin\physim.exe --self-test D:/PhysimTest/Sprachpendel language_pendulum
.\build\bin\physim.exe --self-test D:/PhysimTest/Sprachwurf language_projectile
.\build\bin\physim.exe --self-test D:/PhysimTest/Sprachworkflow language_full
.\build\bin\physim.exe --self-test D:/PhysimTest/Gemischt language_mixed
```

Der Elternordner muss existieren. Die Tests prüfen den absichtlichen Typfehler,
die ursprüngliche Quelldiagnose, den korrigierten nativen Build, Simulation,
gespeicherte Originalquelle, bestehende C-Auswertung, Exporte und Projektneuladen.
Für die kleinere Oberfläche `PHYSIM_TEST_SMALL=1` setzen. Der Compiler `physimc`
wird als Abhängigkeit der App gebaut und mit dem SDK installiert.
`language_full` prüft zusätzlich einen absichtlichen Fehler in `analysis.phys`,
die Korrektur, den Sprachbericht mit zwei Diagrammen, Exporte und den unveränderten
Analyse-Quellsnapshot sowie beide Sprachdateien nach erneutem Projektöffnen.
`language_mixed` prüft denselben Analyseablauf mit einem C-Pendel als Experiment.

Die reproduzierbare IPC-Mutationskampagne ist als CTest `protocol_mutations`
registriert. Ablauf, Invarianten und Reproduktion stehen in [fuzzing.md](fuzzing.md).

```powershell
cmake --build build --config Release --parallel
cmake --install build --config Release --prefix build/package
cd build
cpack -C Release
```

Das Paket enthält die App, Runner, Beispielmodule, SDK-Quellen und Header, Vorlagen,
Dokumentation und Lizenzen. Ein C17-Compiler muss für Nutzerprojekte auf dem Zielsystem
eingerichtet sein. Ein Clean-Machine-Test und Linux-Distributionspakete sind noch offene Releasegates.

### macOS-App-Paket

Auf einem Mac mit abgeschlossenem Build erzeugt dieser Befehl ein neues App-Paket:

```sh
python3 tools/package-macos.py --build build --config Release --output build/Physim.app
open build/Physim.app
```

Das Ziel darf noch nicht existieren. Das Paket enthält App, Runner, Compiler,
SDL und das SDK; relative Bibliothekspfade erlauben das Verschieben. Die Ressourcen
liegen in `Contents/Resources`. Neue Projekte werden unter `Dokumente/Physim`
vorgeschlagen und können in einem anderen gewählten Ordner angelegt werden.
Die lokale Ad-hoc-Signatur wird geprüft. Für eine öffentliche Verteilung mit
Developer ID und Notarisierung fehlen noch die Apple-Entwicklerzugänge.
Die macOS-CI verschiebt das Paket in einen Pfad mit Leerzeichen und Umlaut und
prüft daraus vollständige C- und Physim-Sprachprojekte.
Erfolgreich geprüfte Pakete stehen im jeweiligen CI-Lauf als
`physim-app-macos-15` (Apple Silicon) oder `physim-app-macos-15-intel` bereit.
