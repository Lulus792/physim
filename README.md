# Physim

**Installieren und mit Oberfläche starten:**
[Windows](#schnellstart-unter-windows) · [Linux / Ubuntu](#linux) ·
[macOS / Apple Silicon und Intel](#macos).
Die Anleitungen enthalten Installation, Build, Tests und den Start der App.

**Neu hier?** Das [Handbuch mit Lernpfaden](docs/guide.md) erklärt Bedienung,
Experimente, Auswertung und alle öffentlichen Funktionen. In der App öffnet **F1**
dieselben Inhalte offline, mit themenübergreifender Suche und kopierbaren Beispielen.

Physim ist eine native C17-Arbeitsumgebung für reproduzierbare Physikexperimente.
Der erste implementierte Durchstich führt von bearbeitbarem C-Code über einen
separaten Simulationsprozess und gespeicherte Messwerte bis zur C-Auswertung.

**Entwicklungsstand: 0.1.0-dev, noch keine stabile 1.0.** Der verbindliche Langfristplan
steht in [Physim_Projektplan.md](Physim_Projektplan.md). Der konkrete Umsetzungsstand
und noch offene Abnahmekriterien stehen in [docs/status.md](docs/status.md).

Eine eigene, statisch typisierte und nativ kompilierte Sprache als vollständige
Alternative zu C für Experimente und Auswertung ist verbindlich geplant.
Lexer, Parser mit Python-artigen `:`-/Einrückungsblöcken und eine erste statische
Prüfung für skalare Typen, eigene Strukturen, einfache Enums, Namen und Funktionen sind implementiert. `physimc` und
der direkte Build übersetzen eigenständige Programme über C17 zu nativen
Executables und erste Experimentmodule für den bestehenden Runner. Pendel und
Vakuumwurf mit Messkanälen und Szene liegen unter `examples/language`.
Die App bietet Sprachvorlagen für Pendel, Wurf mit Luftwiderstand,
Kugelstoß mit Medien, Box auf Ebene, Feder–Masse–Dämpfer, Wurf mit Unsicherheit,
Boxstoß und Auftrieb. Der Boxstoß hat vier Laufzeitparameter und eine eigene
Auswertung für beide Körper. Weitere Sprachbeispiele für gekoppelte Körper, rotierende Körper,
Gelenke und schnelle Kugeln liegen unter `examples/language`. Editor, Build, Quelldiagnosen und der
Simulations-/Auswertungsablauf sind für Sprachprojekte verfügbar.
Beim Anlegen lässt sich auch für die Auswertung die Physim-Sprache auswählen.
Nutzerprojekte werden über die automatisch gepflegte `physim.project` gebaut.
Der mitgelieferte Buildprozess startet den Compiler direkt und sammelt erzeugte
Dateien unter `build/Debug` oder `build/Release` im Projektordner. Dafür sind keine
`CMakeLists.txt` und keine CMake-Installation nötig; ein C17-Compiler bleibt erforderlich.
Die erste Vorlage liest Positionen, berechnet Ableitungen und erzeugt Diagramme
sowie CSV-/SVG-Exporte. Ein Beispiel liegt in `examples/language/analysis.phys`.
Die Auftriebsvorlage verwendet `Submersion.sphere` und besitzt eine eigene
Auswertung mit Diagrammen und CSV-Export. Ihr Lauf und ihre Szene werden gegen
die C-Vorlage geprüft.
Der Sensorwurf verwendet die gemeinsame Sensorbibliothek mit Zeitraster,
Rauschen, Ausfällen und Standardunsicherheit. Sollbahn, Modellbahn, Messwerte
und Messstatus bleiben getrennt; die Auswertung kann in C oder Physim erfolgen.
Die Physim-Auswertung filtert zusammengehörige Reihen nach gültigem Messstatus,
zeigt Messpunkte sowie Tabellen für Verfügbarkeit, Messfehler und Unsicherheit
und exportiert die Ergebnisse als CSV und SVG.
Der [Sprachentwurf](docs/language.md) beschreibt den Stand.
Seit Sprachversion 0.121.0 sind auch dreifach zitierte mehrzeilige Strings mit
reproduzierbaren LF-Zeilenumbrüchen verfügbar.
Sprachversion 0.122.0 ergänzt geprüfte `Int64`-Methoden für Betrag, Minimum,
Maximum und Begrenzung.
Sprachversion 0.123.0 erlaubt Unicode-Zeichen mit `\u{...}` in Stringliteralen.
Sprachversion 0.124.0 ergänzte String-Wiederholung; die aktuelle Syntax ist
`String(repeating: text, count: n)`.
Sprachversion 0.125.0 ermöglicht dieselbe Operation für Arrays aller Elementtypen.
Die String-API unterstützt Unicode-bewusstes
`trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)`.
Ein [getestetes Sprachtutorial](docs/language-tutorial.md) führt vom eigenen
Experiment bis zum Bericht und verwendet dieselbe Bewegungsreferenz wie das C-Tutorial.
Der starre Körper verwendet dieselbe Mechanikbibliothek wie C: Kräfte, Impulse,
Translations- und Rotationsenergie sowie Quaternionrotation. Das Beispiel zeigt
einen durch eine außermittige Kraft angetriebenen Quader ohne Kontakte.
Die Kontaktvorlage ergänzt Kugel-/Quaderkontakte, Reibung und Rückprall über
den gemeinsamen Solver. Ihr Modell und ihre Messwerte werden gegen die
entsprechende C-Vorlage geprüft.
Das Gelenkpendel verbindet einen Quader über einen lokalen Anker mit der Welt.
Distanzgelenke liefern korrigierte Körperwerte, Impulse und Restfehler; die
Sprachversion wird einschließlich Schrittweitenverfeinerung gegen C geprüft.
Körpergruppen können Kontakte und Distanzgelenke gemeinsam lösen. Die neue
Vorlage zeigt zwei verbundene Kugeln mit Bodenkontakt und zeichnet Impulse
sowie die verbleibenden Kontakt- und Gelenkfehler auf.
Lineare Kugel-Sweeps liefern den ersten Kontakt entlang einer Bewegung.
Die schnelle Kugel verarbeitet mehrere Wandstöße samt Restzeit pro Schritt;
AABB-Hüllkörper und Kandidatenpaare unterstützen die Kollisionssuche in Gruppen.
`Mat3` und `Mat4` ergänzen lokale und globale Koordinatensysteme, Matrixprodukte,
Inversion sowie Punkt-, Richtungs- und Normalentransformation. Der Kraftangriffspunkt
der Körpervorlage verwendet diese gemeinsame Mathematik.
Optionale Werte `T?` stellen fehlende Ergebnisse ausdrücklich dar. Sie unterstützen
`nil`, `Optional.some`, geprüften Zugriff und Kopien auch für Arrays und eigene
Strukturen. Der Sensorwurf und seine Auswertung verwenden diese Sprachfunktion.
Mit `if let` und `while let` lässt sich vorhandener Inhalt direkt für einen Block
binden; `if var` und `while var` erzeugen veränderliche lokale Kopien.

## Schnellstart unter Windows

Das bereits gebaute portable Paket startet über `bin/physim.exe`. Beim folgenden
Entwicklungsbuild liegt die Anwendung unter `build/native/Debug/bin/physim.exe`.

Voraussetzungen für den Entwicklungsbuild: Windows 10 ab 1903 / Windows 11, Visual
Studio 2022 mit **Desktopentwicklung mit C++** (enthält den C-Compiler und Windows SDK),
Python ab 3.10 sowie einen Grafiktreiber
mit **OpenGL 3.3 Core**. Für eigene Experimente
benötigt die App einen C17-Compiler; CMake ist dafür nicht erforderlich.

```powershell
# SDL3 installieren, falls third_party/SDL3-3.2.30 noch nicht vorhanden ist:
powershell -ExecutionPolicy Bypass -File tools/bootstrap-windows.ps1
python tools/build.py --config Debug --test
.\build\native\Debug\bin\physim.exe
```

Zum späteren Starten genügt der letzte Befehl. Nach Quellcodeänderungen den
Buildbefehl erneut ausführen. Er baut auch die Oberfläche und prüft die Tests
ohne Fenster. Optionale Fenster- und Grafiktests starten mit
`python tools/build.py --config Debug --test-display`.

Die App lädt nur explizit gebauten und gestarteten Experimentcode. Die mitgelieferten
Vorlagen werden nicht automatisch ausgeführt. C-Code läuft mit den Rechten des
Benutzers; die Prozessgrenze ist Absturzschutz, keine Sicherheits-Sandbox.

## Ein Pendel untersuchen

Weitere Vorlagen sind Wurf, Kugelstoß, Box auf Ebene, Feder–Masse–Dämpfer,
Wurf mit Unsicherheit, **Boxstoß** und **Auftrieb**. Die [Mechanikanleitung](docs/mechanics.md)
erklärt den elastischen und schrägen Stoß orientierter Boxen und ihre Messkanäle.

1. App starten. **Datei → Neues Projekt** öffnen, Zielordner und Namen eingeben, **Pendel** als Vorlage wählen und **Projekt anlegen** drücken. Ein bestehendes Projekt lässt sich über **Datei → Ordner öffnen** laden.
2. `main.c` enthält Länge, Masse, Winkel, Luftdichte, Sensorrauschen und Integrator.
3. **Build / F5** speichert die Quellen und baut Experiment und Analyse im Hintergrund.
4. Unter **Simulieren** starten. Pause, Einzelschritt und Stop steuern den Runner.
5. Nach Stop lädt die App die Messdaten im Hintergrund. **Auswerten** zeigt Kurven
   und Statistik. **Analyse starten** führt `analysis.c` oder `analysis.phys` in einem weiteren Prozess aus.
   Dessen **Analyseergebnis** zeigt eigene Diagramme und Tabellen. Die Vorlage erzeugt
   Geschwindigkeit, Phasenraum, Histogramm und Kennzahlen. Die Auswahl oberhalb des
   Diagramms wechselt die Ansicht; CSV-, SVG- und PNG-Export liegen direkt darunter.
6. Im Projektordner `runs/` liegen `.psrun`, Quellcodekopien, Statistik-CSV,
   SVG-Plot und Analysemanifest. Die Vorlage schreibt außerdem `-derived.csv` mit
   abgeleiteter Geschwindigkeit, gleitendem Mittel und rekonstruierter Position.
   **CSV exportieren** exportiert alle Messpunkte.

Eigene Berichte verwenden [report.h und das Ergebnisformat](docs/reports.md).
**Läufe & Berichte / Ctrl+4** öffnet auch ältere Messläufe und Analyseberichte ohne
Neubau. Bis zu acht angehakte Läufe lassen sich mit dem gebauten Analysemodul
vergleichen. Die Vorlage überlagert Position und Geschwindigkeit und schreibt eine
Kennzahlentabelle. Eine zusätzliche Positionsdifferenz zeigt Abweichungen vom ersten
ausgewählten Lauf mit expliziter linearer Interpolation im gemeinsamen Zeitbereich.
Anleitung und API: [Läufe und Berichte](docs/runs.md).
Diagrammvorschauen sind bei großen Läufen reduziert und entsprechend gekennzeichnet;
Kennzahlentabellen und Histogramme der Vorlage verarbeiten sämtliche Werte.
Ergebnisdiagramme und gespeicherte Messdaten lassen sich mit dem Mausrad vergrößern
und durch Ziehen verschieben. „Alles zeigen“ setzt den Ausschnitt zurück.
Statistik und Exporte behalten ihren vollständigen Umfang.

Die OpenGL-Szene zeigt Kugeln, drehbare Boxen/Ebenen, Linien, Pfeile, Punkte,
Polylinien und Beschriftungen mit Tiefenpuffer
und bis zu vierfacher Kantenglättung. Rechte Maustaste: Orbit; mittlere Maustaste:
Verschieben; Mausrad: Zoom. Schieberegler und Ansichten für vorne, Seite, oben und
Standard sind ebenfalls vorhanden. Orthografische Projektion ist umschaltbar. Die Vorschau
wird bei großen Läufen ausgedünnt; die gespeicherten Messdaten bleiben vollständig.
**Neuer Lauf** beginnt nach Stop einen neuen Lauf mit demselben Seed.
Zeitschritt und Seed stehen im Inspector unter **Laufeinstellungen**.

Die Wurfvorlage zeigt zusätzlich ihre Flugbahn. Pfade, Punkte, Vektoren und Labels
sind getrennt ausblendbar. **Kugelstoß & Medien** verwendet starre Kugeln mit
Rotation und Kontaktimpulsen. In `main.c` sind Vakuum, Luft, Wasser oder ein eigenes
Medium sowie Reibung und Anfangsrotation wählbar. Kraft-/Geschwindigkeitspfeile und
ein Kontaktmarker ergänzen die Szene. **Box auf Ebene** lässt eine geneigte Box
fallen, mit mehreren Kontaktpunkten aufsetzen und durch Reibung zur Ruhe kommen.
Die Messung enthält auch Bodenabstand und normalen Kontaktfehler. Anleitung und Modellgrenzen:
[Mechanik und Medien](docs/mechanics.md).

**Feder–Masse–Dämpfer** zeigt eine horizontal geführte Masse mit Feder- und
Dämpfungskraft. Die Auswertung trennt mechanische und dissipierte Energie und
prüft ihre Bilanz. Ein Lernpfad mit vier Dämpfungsfällen steht unter
[Feder–Masse–Dämpfer](docs/spring.md), auch direkt in der App über F1.

**Monte Carlo** wiederholt ein gebautes Experiment mit expliziten Seeds und bis zu
acht gleichzeitigen Runnern und wertet die Endwerte eines Kanals aus. Fortschritt,
Abbruch, Histogramm, Quantile und ein
gekennzeichnetes Näherungsintervall für den Mittelwert sind integriert.
**Wurf mit Unsicherheit** trennt Sollbahn, zufällige Anfangsgeschwindigkeit und
Sensormessung mit Rauschen, Auflösung, Offset, Drift und Ausfällen. Gültige Werte,
Standardunsicherheit und Messstatus bleiben getrennt. Der Bericht vergleicht die
drei Signale und exportiert gültige Sensorwerte. Die C- und Physim-Vorlagen
verwenden gleiche Seeds und Messkanäle; beide Auswertungen lesen beide Laufdateien.
[Sensor-API und Beispiel](docs/measurement.md)
stehen offline unter F1 bereit. [Anleitung und Grenzen](docs/monte-carlo.md) öffnen sich auch direkt
über „Anleitung“ in der Monte-Carlo-Ansicht.

Das aktuelle SDK verwendet **API/ABI 3**: bereits gebaute
Module bitte neu bauen. Bestehende `.psrun`-Messdaten bleiben lesbar.

## Oberfläche

Die kompakte Menüzeile bündelt globale Werkzeuge unter **Datei** und **Ansicht**;
**Hilfe** und **Einstellungen** sind direkt erreichbar. Die Arbeitsbereiche
**Entwickeln**, **Simulieren** und **Auswerten** liegen als schmale, gleich breite Tabs
über die gesamte Fensterbreite direkt darunter. Die Menüzeile ist zusammen mit den
Fensterknöpfen in den oberen Fensterkopf integriert.

Beim normalen Beenden merkt sich Physim den Hauptordner und hinzugefügte Dateien
und Ordner. **Letzten Workspace öffnen** stellt diese Auswahl beim nächsten Start
auf Wunsch wieder her. [Workspace und Wiederöffnung](docs/workspace.md)
Die Seitenleiste zeigt einen aufklappbaren Dateibaum mit sortierten Ordnern,
verschachtelten Dateien und zusätzlichen Wurzeleinträgen. Aktualisieren erhält
aufgeklappte Zweige; fehlende Pfade und Anzeigegrenzen werden gekennzeichnet.
Bis zu 16 UTF-8-Textdateien lassen sich parallel bearbeiten, suchen, ersetzen
und mit Schutz vor externen Änderungen speichern. Ihre automatischen Sicherungen
liegen außerhalb des Workspace und werden beim erneuten Öffnen der Datei zur
[Wiederherstellung](docs/autosave.md) angeboten.

**Einstellungen** in der Menüzeile oder **Ctrl+,** öffnet Code-Schriftgröße,
Autosave-Intervall und Darstellungsoptionen. Seitenleiste und Protokoll lassen sich
an ihren Trennlinien vergrößern; Fenstergröße, Maximierung und Darstellung bleiben
bei normalem Beenden für den nächsten Start erhalten.
Anleitung: [Einstellungen und Fensteraufteilung](docs/settings.md), auch direkt über F1.

Ungespeicherte Änderungen beider Editoren werden standardmäßig alle 30 Sekunden separat
gesichert. Nach einem Abbruch bietet die App beim erneuten Öffnen die
Wiederherstellung an und erkennt inzwischen extern geänderte Quellen.
Anleitung und Grenzen: [Autosave und Wiederherstellung](docs/autosave.md), auch über F1.

Die Gestaltungsrichtung folgt Apples Prinzipien für klare Hierarchie, zurückhaltende
Farben und schrittweise sichtbare Details. Die App verwendet eine ruhige dunkle
Oberfläche, Systemschrift für Bedienelemente und eine separate Codeschrift im Editor.
Kamera und Sichtbarkeit liegen im Inspector; das Protokoll lässt sich einklappen.
Windows nutzt lokal vorhandenes Segoe UI und Consolas, Linux DejaVu und macOS
Helvetica und Menlo, jeweils mit
Fallback. Diese Systemschriften werden nicht mit dem Paket verteilt.

Ctrl+1/2/3 wechselt den Arbeitsbereich, Ctrl+4 öffnet Läufe und Berichte,
Ctrl+F sucht in der aktuellen Ansicht (im Editor mit Ersetzen), Ctrl+L zeigt das
Protokoll. F5 baut und F6 startet oder pausiert den Lauf. Die Mindestfenstergröße
beträgt 1080 × 740. Vollständige Tastaturfokussierung und Screenreader-Unterstützung
sind noch offen. Die verbindliche Richtung steht in [docs/design-direction.md](docs/design-direction.md).

**API-Dokumentation** in der Seitenleiste oder **F1** öffnet die Dokumentation direkt
in einem eigenen, frei verschiebbaren Fenster, auch ohne geöffnetes Projekt.
Es kann neben Editor oder Simulation geöffnet bleiben. Themen, öffentliche Header und
Abschnittsnavigation sind offline verfügbar. Ctrl+F sucht im aktuellen Dokument;
Codebeispiele lassen sich kopieren. **Zurück zum Arbeitsbereich** fokussiert das Hauptfenster,
während die Dokumentation offen bleibt. Schließen oder Escape blendet nur das
Dokumentationsfenster aus; F1 öffnet es mit Thema, Suchtext und Leseposition erneut.
Externe Quellen sind ausdrücklich als Browserlinks gekennzeichnet.
Der Leser unterstützt Überschriften, Absätze, Listen, Links und Codeblöcke;
Tabellen erscheinen derzeit als Textzeilen.

## Linux

### Installieren und mit Oberfläche starten (Ubuntu 24.04)

Die folgenden Befehle im Terminal ausführen. Zum Starten ist eine grafische
Desktop-Sitzung mit einem OpenGL-3.3-Core-fähigen Treiber erforderlich.
Bei einem vorhandenen Checkout die beiden Befehle zum Klonen und Wechseln
überspringen und im Physim-Repository beginnen.

**1. Compiler, Buildwerkzeuge und Grafikabhängigkeiten installieren:**

```sh
sudo apt-get update
sudo apt-get install -y build-essential git cmake ninja-build pkg-config python3 \
  libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxfixes-dev \
  libxss-dev libxtst-dev libwayland-dev libxkbcommon-dev \
  libegl1-mesa-dev libgl1-mesa-dev fonts-dejavu-core
```

Für andere Distributionen die entsprechenden Pakete aus der
[SDL-Linux-Anleitung](https://wiki.libsdl.org/SDL3/README-linux) installieren.
Für den folgenden SDL-Quellbuild wird CMake ab 3.24 benötigt.

**2. Physim herunterladen und SDL 3.2.30 lokal bauen:**

```sh
git clone https://github.com/PhysicSimulator/physim.git
cd physim
git clone --depth 1 --branch release-3.2.30 https://github.com/libsdl-org/SDL.git build-sdl-source
cmake -S build-sdl-source -B build-sdl -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DSDL_TESTS=OFF \
  -DCMAKE_INSTALL_PREFIX="$PWD/build-sdl-install"
cmake --build build-sdl --parallel
cmake --install build-sdl
```

**3. Physim mit Oberfläche bauen, prüfen und starten:**

```sh
python3 tools/build.py --config Debug --sdl "$PWD/build-sdl-install" --test
./build/native/Debug/bin/physim
```

Der erste Befehl baut auch die Oberfläche und führt die Tests ohne Fenster aus.
Der zweite öffnet die App. Zum späteren Starten genügt im Repository
`./build/native/Debug/bin/physim`. Nach Quellcodeänderungen beide Befehle
erneut ausführen. Physim selbst wird dabei direkt mit Python und dem Compiler
gebaut; CMake und Ninja werden oben für SDL verwendet.
SDL und Buildausgaben bleiben in den lokalen `build*`-Ordnern; für die
Build- und Installationsbefehle ist kein `sudo` nötig.

**Optional: Oberfläche und Fenster automatisch prüfen:**

Die App zuerst schließen und diesen Befehl in der grafischen Desktop-Sitzung
ausführen. Die Tests öffnen und schließen selbstständig Fenster.

```sh
python3 tools/build.py --config Debug --sdl "$PWD/build-sdl-install" --test-display
```

Ubuntu 24.04 mit GCC und Clang
hat Build, 276 Tests ohne Fenster, 35 Grafik-/Fenstertests unter Xvfb/Mesa, das
installierte SDK und alle acht vollständigen C-App-Abläufe bestanden.
[Prüfstand und verbleibende Plattformgrenzen](docs/platform-validation.md).

## macOS

### Installieren und mit Oberfläche starten (Apple Silicon und Intel)

**1. Xcode Command Line Tools installieren:**

```sh
xcode-select --install
```

Die Installation im angezeigten Dialog abschließen, bevor es weitergeht.
Sind die Werkzeuge bereits installiert, kann dieser Schritt entfallen.

**2. Homebrew und Buildwerkzeuge installieren:**

Falls Homebrew noch fehlt, den offiziellen
[Homebrew-Installer](https://brew.sh/) im Terminal starten und dessen Anweisungen
einschließlich der Schritte zur Shell-Einrichtung befolgen:

```sh
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Homebrew für das aktuelle Terminal aktivieren und die Werkzeuge installieren:

```sh
if [ -x /opt/homebrew/bin/brew ]; then
  eval "$(/opt/homebrew/bin/brew shellenv)"
else
  eval "$(/usr/local/bin/brew shellenv)"
fi
brew install cmake ninja python
```

**3. Physim herunterladen und SDL 3.2.30 lokal bauen:**

Bei einem vorhandenen Checkout die ersten beiden Befehle überspringen und im
Physim-Repository beginnen. Die Befehle bauen für die Architektur des Macs;
auf Apple Silicon ein natives Terminal verwenden.

```sh
git clone https://github.com/PhysicSimulator/physim.git
cd physim
git clone --depth 1 --branch release-3.2.30 https://github.com/libsdl-org/SDL.git build-sdl-source
cmake -S build-sdl-source -B build-sdl -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -DSDL_TESTS=OFF \
  -DCMAKE_INSTALL_PREFIX="$PWD/build-sdl-install"
cmake --build build-sdl --parallel
cmake --install build-sdl
```

**4. Physim mit Oberfläche bauen, prüfen und starten:**

```sh
python3 tools/build.py --config Debug --sdl "$PWD/build-sdl-install" --test
./build/native/Debug/bin/physim
```

Der erste Befehl baut einschließlich Oberfläche und führt die Tests ohne Fenster
aus. Der zweite öffnet die App. Später genügt im Repository
`./build/native/Debug/bin/physim`. Nach Quellcodeänderungen beide Befehle erneut
ausführen. CMake und Ninja werden nur für den obigen SDL-Build benötigt;
Physim selbst baut direkt mit Python und Apple Clang.

**Optional: Oberfläche und Fenster automatisch prüfen:**

Die App zuerst schließen und in einer angemeldeten grafischen macOS-Sitzung
ausführen. Die Tests öffnen und schließen selbstständig Fenster.

```sh
python3 tools/build.py --config Debug --sdl "$PWD/build-sdl-install" --test-display
```

**Optional: eine App zum Öffnen im Finder erzeugen:**

```sh
python3 tools/build.py --config Release --sdl "$PWD/build-sdl-install" --install build/native/SDK
python3 tools/package-macos.py --sdk build/native/SDK --output build/Physim.app
open build/Physim.app
```

Danach lässt sich `build/Physim.app` auch per Doppelklick öffnen oder in den
Programme-Ordner kopieren. Das Release-Paket enthält SDL und das Physim-SDK.
Für ein erneutes Paket neue Ausgabeordner wählen, etwa `build/native/SDK-neu`
und `build/Physim-neu.app`, und beim Paketbefehl denselben neuen SDK-Pfad angeben.
Die Werkzeuge überschreiben keine vorhandenen SDK- oder App-Ordner.
Das Paket wird lokal ad hoc signiert;
eine Apple-Notarisierung für die öffentliche Verteilung ist noch offen.

macOS 15 auf Apple Silicon und Intel hat jeweils 311 CTest-Prüfungen, das
verschobene SDK und acht vollständige C-App-Abläufe bestanden. Die `.app`-Pakete
sind zusätzlich nach dem Verschieben mit C- und Physim-Projekten geprüft.
Nachweise, Downloads und verbleibende Abnahmen stehen in
[Plattformprüfung](docs/platform-validation.md).

Nutzerprojekte benötigen nur den C17-Compiler und Physim, keine CMake-Datei.
Die App verwendet OpenGL 4.1 Core, macOS-Systemschriften und **Cmd** für
Speichern, Suchen und Editorbefehle.

## Direkter Build ohne CMake

Ein zusätzlicher [direkter Buildweg](docs/build.md#physim-direkt-ohne-cmake-bauen)
baut Physim selbst einschließlich Oberfläche über Python und den C17-Compiler,
ohne CMake oder Ninja für Physim aufzurufen. Er verwendet eine vorhandene
SDL-Installation. Auch [SDK- und App-Pakete](docs/build.md#sdk-und-portable-pakete-ohne-cmake)
lassen sich direkt erzeugen. Der direkte Testkatalog umfasst auch die Fenster-
und Benchmark-Prüfungen. Die bisherigen CMake-Abläufe bleiben vorerst für den
Vergleich sowie ihre Sanitizer- und SDK-Prüfungen verfügbar.

Nach der Installation der Werkzeuge und SDL aus der jeweiligen Plattformanleitung
im Physim-Repository ausführen. Zusätzlich ist Python ab 3.10 erforderlich;
die obigen Linux- und macOS-Befehle installieren es bereits.

**Linux und macOS, einschließlich Oberfläche:**

```sh
python3 tools/build.py --config Debug --sdl "$PWD/build-sdl-install" --test
./build/native/Debug/bin/physim
```

**Windows, einschließlich Oberfläche:**

```powershell
python tools/build.py --config Debug --test
.\build\native\Debug\bin\physim.exe
```

Zum späteren Starten genügt jeweils der zweite Befehl. Nach Quellcodeänderungen
beide Befehle erneut ausführen. `--test` führt die bisher auf den direkten
Buildweg übertragenen Tests aus. Der SDL-Quellbuild aus der Linux- und
macOS-Anleitung verwendet weiterhin CMake.

Fenster- und Grafiktests lassen sich in einer grafischen Desktop-Sitzung separat
mit `python3 tools/build.py --test-display` starten (Windows: `python` statt
`python3`). Einzelne Abläufe, Voraussetzungen und Linux-CI-Befehle stehen in der
[Buildanleitung](docs/build.md#fenster--und-grafiktests-direkt-ausführen).

## Ohne Oberfläche

Optional unter Linux und macOS nur Bibliothek und Kommandozeilenprogramme bauen
(C17-Compiler und Python ab 3.10 erforderlich, SDL entfällt). Die Befehle im
Physim-Repository ausführen:

```sh
python3 tools/build.py --config Debug --no-app --build-dir build/native/Core --test
./build/native/Core/bin/physim-runner ./build/native/Core/bin/pendulum.so ./pendel.psrun --steps 4000 --dt 0.005 --seed 42
./build/native/Core/bin/physim-analysis-runner ./build/native/Core/bin/pendulum_analysis.so ./pendel.psrun ./pendelbericht
./build/native/Core/bin/physim-analysis-runner --csv ./pendel.psrun ./pendel.csv
```

Unter Windows nach dem oben beschriebenen Build:

```powershell
.\build\bin\physim-runner.exe .\build\bin\pendulum.dll .\pendel.psrun --steps 4000 --dt 0.005 --seed 42
.\build\bin\physim-analysis-runner.exe .\build\bin\pendulum_analysis.dll .\pendel.psrun .\pendelbericht
.\build\bin\physim-analysis-runner.exe --csv .\pendel.psrun .\pendel.csv
```

Unter Linux und macOS heißen Projektmodule `.so`, Programme haben kein `.exe`. Ausgabedateien werden
absichtlich exklusiv angelegt: für erneute Läufe einen neuen Namen verwenden.

## Struktur

Reproduzierbare Messfälle für Speicherung, Analyse und Berichtskurven lassen sich
mit `python3 tools/build.py --config Release --benchmarks` bauen (Windows: `python`).
Aufruf, Referenzprüfungen und Vergleichsschwellen:
[Leistungsmessung](docs/performance.md).
Die [UI-Messstrecke](docs/ui-rendering.md) prüft außerdem wiederverwendbare
Zeichenpuffer, Größenwechsel und PNG-Exporte mit identischen Vergleichsbildern.

- `include/physim`: öffentliche, GUI-unabhängige C-API
- `src`: Mathematik, Einheiten, RNG, Mechanik, Daten, Analyse, Prozess- und IPC-Code
- `runners`: getrennte Hosts für Experiment und Analyse
- `app`: SDL3-/Nuklear-Oberfläche, Buildsteuerung und OpenGL-3.3-Core-Renderer
- `examples`: Pendel, Wurf mit Luftwiderstand, Kugelstoß mit Medien, Box auf Ebene, Feder–Masse–Dämpfer
- `tests`: Numerik, Daten-Recovery, IPC und Prozessisolation
- `docs`: API, Datenformat, Entscheidungen, Fortschritt

## Lizenz

Eigener Code: [MIT](LICENSE). SDL3: zlib-Lizenz. Nuklear: MIT/Public Domain nach Wahl;
Physim nutzt die MIT-Option. Quellen und Lizenztexte: [third_party/README.md](third_party/README.md).
