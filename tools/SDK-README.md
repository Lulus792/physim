# Physim – portables Paket

Dieses Paket ist bereits gebaut. Den gesamten Ordner zusammen entpacken und
verschieben, damit Programme, Bibliotheken, Vorlagen und Handbuch zusammenbleiben.
`physim-sdk.json` nennt Plattform, Buildprofil und enthaltene Dateien mit Prüfsummen.
`"app": false` kennzeichnet ein Paket ohne Oberfläche und Projektbuilder.

## Oberfläche starten

Die folgenden Befehle im entpackten Paketordner ausführen:

**Windows, PowerShell:**

```powershell
.\bin\physim.exe
```

Die App lässt sich auch per Doppelklick auf `bin/physim.exe` öffnen.
Voraussetzungen: Windows 10 ab 1903 oder Windows 11 und ein Treiber mit OpenGL 3.3 Core.

**Linux, Terminal in einer grafischen Desktop-Sitzung:**

```sh
./bin/physim
```

Das unter Debian 12 gebaute CI-Release-Paket ist unter Debian 12 und Ubuntu 24.04
mit X11/Mesa geprüft.
Die App benötigt OpenGL 3.3 Core. SDL ist im Paket enthalten;
die Systembibliotheken des Desktops und ein passender Grafiktreiber werden benötigt.
Fehlende Laufzeitbibliotheken unter Debian 12 oder Ubuntu 24.04 installieren:

```sh
sudo apt-get update
sudo apt-get install -y zenity fonts-dejavu-core libx11-6 libxext6 libxrandr2 libxcursor1 \
  libxi6 libxfixes3 libxss1 libxtst6 libwayland-client0 libwayland-cursor0 \
  libwayland-egl1 libxkbcommon0 libegl1 libgl1 libgl1-mesa-dri libdbus-1-3
./bin/physim
```

Zum Öffnen der App sind Python, CMake und ein Compiler nicht erforderlich.
Zenity ermöglicht Datei- und Ordnerdialoge, wenn kein XDG-Portal verfügbar ist.
Die jeweiligen Paketprüfungen und Grenzen stehen im
[Plattformprüfstand](docs/platform-validation-history.md).

**macOS, Terminal:**

```sh
./bin/physim
```

Ein separat ausgeliefertes `Physim.app` stattdessen im Finder öffnen. Das Paket muss
zur Mac-Architektur passen (Apple Silicon oder Intel). Geprüft wird macOS 15.
Die App verwendet OpenGL 4.1 Core. Die lokalen App-Pakete sind ad hoc signiert;
Developer-ID-Signierung und Notarisierung für öffentliche Releases sind noch offen.

## Bestehendes Projektformat aktualisieren

Gültige Experimentprojekte im Format 1 lassen sich ausdrücklich auf Format 2
aktualisieren. Im Terminal aus dem Paketordner:

```sh
./bin/physim-build --migrate-project --project /pfad/zum/projekt
```

Unter Windows lautet das Programm `bin/physim-build.exe`. Die vorherige
Beschreibung bleibt als `physim.project.bak` erhalten. Quellen, Messdateien und
Buildcache bleiben unverändert; Format 2 wird nicht erneut geschrieben.
Die App bietet dieselbe Aktion unter **Build-Einstellungen**.
[Vertrag und Grenzen](docs/data-format.md#eigenständige-analyseprojekte).

## Eigenes Projekt

Für neue C- und Physim-Projekte ist zusätzlich ein C17-Compiler erforderlich:

- **Windows:** Visual Studio 2022 oder Build Tools mit „Desktopentwicklung mit C++“
  einschließlich Windows SDK installieren. Physim erkennt die Installation automatisch.
- **Debian / Ubuntu:** `sudo apt-get install build-essential`.
- **macOS:** `xcode-select --install` ausführen und den Installationsdialog abschließen.

In der App **Datei → Neues Projekt** wählen, eine Vorlage auswählen und das Projekt
anlegen. **Build / F5**, **Simulieren** und **Auswerten** führen durch den Ablauf.
Physim erzeugt und pflegt `physim.project`; erzeugte Compilerdateien liegen im
Projektordner unter `build/Debug` oder `build/Release`. Nutzerprojekte benötigen
weder CMake noch Python. Eigene Quelldateien und Laufdaten bleiben beim Projekt.

**F1** öffnet das mitgelieferte Handbuch. Einstieg und Beispiele:
[Lernpfade](docs/guide.md), [C-Tutorial](docs/experiment-tutorial.md) und
[Physim-Sprachtutorial](docs/language-tutorial.md).

## Mitgeliefertes Pendel ohne Oberfläche ausführen

Diese Befehle verwenden bereits gebaute Module und benötigen keinen Compiler.
Sie funktionieren auch mit einem Paket ohne Oberfläche. Im Paketordner ausführen;
Messwerte und Berichte entstehen in `runs/`.

**Windows, PowerShell:**

```powershell
New-Item -ItemType Directory -Force runs | Out-Null
.\bin\physim-runner.exe .\bin\pendulum.dll .\runs\pendel.psrun --steps 4000 --dt 0.005 --seed 42
.\bin\physim-analysis-runner.exe .\bin\pendulum_analysis.dll .\runs\pendel.psrun .\runs\pendelbericht
.\bin\physim-analysis-runner.exe --csv .\runs\pendel.psrun .\runs\pendel.csv
```

**Linux und macOS, Terminal:**

```sh
mkdir -p runs
./bin/physim-runner ./bin/pendulum.so ./runs/pendel.psrun --steps 4000 --dt 0.005 --seed 42
./bin/physim-analysis-runner ./bin/pendulum_analysis.so ./runs/pendel.psrun ./runs/pendelbericht
./bin/physim-analysis-runner --csv ./runs/pendel.psrun ./runs/pendel.csv
```

## Inhalt und Entwicklung

- `bin/`: Programme, Beispielmodule und benötigte mitgelieferte Laufzeitbibliotheken.
- `include/` und `lib/`: öffentliche C-Header und die Kernbibliothek.
- `src/`: Quellen der Kernbibliothek zum eigenen Neubau.
- `examples/`: C- und Physim-Beispielquellen.
- `docs/` und `licenses/`: Handbuch, API-Anleitung und Lizenzen.

Der vollständige Quellcode der App und ihre Entwicklungswerkzeuge liegen im
[Physim-Repository](https://github.com/PhysicSimulator/physim). Dessen README enthält
die Befehle, um Physim selbst zu bauen. Dieses Paket enthält die ausgelieferten
Programme und das SDK, nicht den vollständigen Entwicklungs-Checkout.
