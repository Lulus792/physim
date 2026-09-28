# Umsetzungsstand

Stand: erster Entwicklungsdurchstich, ergänzt am 2026-09-28. Der Projektplan ist die Roadmap;
dieses Dokument unterscheidet implementierten Code von noch offenen Produktzielen.

Aktueller [Plattformnachweis](platform-validation.md): Ubuntu 24.04 mit GCC und Clang
bestand 276 Tests ohne Fenster, 35 Grafik-/Fenstertests, das installierte SDK
und acht vollständige C-App-Abläufe. macOS 15 auf Apple Silicon und Intel bestand
dieselben Prüfungen. Die verschiebbaren Mac-App-Pakete bestanden zusätzlich
Signaturprüfungen und vollständige C-/Physim-Sprachabläufe nach dem Verschieben.
Weitere Linux-Umgebungen bleiben in Prüfung. Ältere Linux-Hinweise in den folgenden Entwicklungsnotizen geben
den damaligen Prüfstand wieder.

Projektdateien speichern jetzt zusätzlich den Zeitschritt und den vollständigen
64-Bit-Seed. Die App prüft beide vor Speichern und Laufstart. Modelltests bestehen
mit MSVC, Clang und AddressSanitizer auf Windows. Der App-Test prüft Neustart,
Release-Build, Ablehnung eines überlaufenden Seeds und einen echten Runnerlauf
mit 0,125 Sekunden und Seed 18446744073709551615 einschließlich der Messdatei.

Ein zusätzlicher direkter Entwicklungsbuild (`tools/build.py`) baut jetzt auch
Physim selbst über die Compiler-/Archiviererprogramme. Bibliotheken, Sprachcompiler,
Runner, Projektbuilder und Oberfläche benötigen dabei keinen CMake-Aufruf.
Windows/MSVC besteht Debug und Release sowie den SDL-freien Build und die drei
Core-/Numerik-/Mechanik-Referenzprogramme. Vollständige C- und Physim-App-Abläufe
bestehen mit dem direkt gebauten Debug-Programm. Der Buildtest prüft Unicodepfade,
unveränderte Ausgaben, Headeränderungen, Compiler-/Linkerfehler mit erhaltenem
Programm, beschädigte Cacheausgaben und die Sperre gleichzeitiger Builds.
Der direkte Buildschritt besteht außerdem in allen acht CI-Kombinationen unter
Windows (MSVC/ClangCL, Debug/Release), Linux (GCC/Clang) und macOS (Apple Silicon/Intel).
Die Linux-/Mac-Prüfung schließt vollständige C- und Physim-App-Abläufe ein.
[Nachweise und genaue Abdeckung](platform-validation.md#direkter-build-von-physim).
Die komplette Testsuite bleibt vorerst CMake-gestützt; auch der SDL-Quellbuild
verwendet weiterhin dessen eigenes Buildsystem.

Der direkte Builder kann nun außerdem ein SDK in einen neuen Ordner installieren.
Es enthält die App, Werkzeuge, alle acht C-Beispielmodule, Analysemodul, öffentliche
Header, Kernbibliothek, SDK-Quellen, Vorlagen, Dokumentation und Lizenzen. Windows
verwendet dafür Release und die Visual-Studio-Laufzeitbibliotheken. Mac-App-Pakete
können dieses SDK über `package-macos.py --sdk` ohne CMake übernehmen.
`verify-native-sdk.py` prüft ein verschobenes Paket mit unabhängigen Headern,
15 Sprachprogrammen, mitgelieferten Modulen und neu gebauten C-/Physim-Projekten.
Bei dieser Prüfung wurde die Windows-Compilererkennung für unterschiedlich
geschriebene Umgebungsvariablennamen korrigiert; die Windows-API übernimmt jetzt
die Suche nach `ProgramFiles(x86)` unabhängig von der Großschreibung.

Der direkte Testläufer übernimmt jetzt 51 vorhandene C-Referenztests ohne CTest;
41 davon benötigen kein SDL. Der vollständige übernommene Satz besteht lokal
mit MSVC Debug, die 41 SDL-freien Fälle zusätzlich mit Clang Release,
einschließlich Protokoll-/Messdatei-/Berichtsmutationen und der
erwarteten Sprach-Laufzeitfehler. Die Prüfbedingungen der C-Tests bleiben erhalten.
Eigene Unterordner isolieren ihre Dateien; JSON-Berichte halten Kommando, Ausgabe,
Dauer und Status fest. Fehler stoppen nachfolgende Fälle nicht, führen aber zu
einem fehlgeschlagenen Gesamtlauf. Der Test des Läufers prüft erwartete Exitcodes,
Diagnosetexte, Timeout, fehlende Programme, Buildfehler und unvollständige Erfolge.
Die übrigen Sprach-, Runner- und Grafikabläufe bleiben bis zu ihrer eigenen
Übernahme an CTest gebunden.

Elf Sprachgruppen für Array- und Stringoperationen laufen jetzt ebenfalls direkt:
elf übersetzte Laufzeitprogramme und 86 Diagnosefälle in elf Gruppen. Damit enthält
der direkte Testkatalog 73 Einträge beziehungsweise 63 ohne SDL. Die 29 ausgewählten
Sprachtests bestehen lokal mit MSVC Debug und Clang Release. Die elf bisherigen
CTest-Diagnoseprüfungen bestehen mit demselben JSON-Katalog weiterhin. Der Buildtest
prüft erzeugte C-Quellen unter Unicodepfaden und hält ihre Objektdateien im Buildordner.
Der Testläufer prüft außerdem, dass fehlgeschlagene Übersetzung keinen alten Code
ausführt und ein Diagnosefehler spätere Fälle nicht unterdrückt. Die neuen
Sprachgruppen bestehen inzwischen im direkten Build- und Testschritt aller acht
Plattformkombinationen zum Commit `199a8f7`.

Die folgende Erweiterung übernimmt alle verbleibenden Array-/String-Diagnosegruppen,
weitere vorhandene Laufzeitprogramme und das Experimentmodul für sicheres Entfernen
von Arrayelementen. Der direkte Katalog enthält damit 134 Tests, davon 124 ohne SDL.
36 Gruppen prüfen 297 abgelehnte Programme sowie zwei Ausgaben mit LF-/CRLF-Eingabe;
47 Programme prüfen normales Verhalten, zwei erwartete Laufzeitfehler und den
Modulablauf einschließlich Reset, Szene und Freigabe. Alle 134 direkten Tests bestehen
lokal mit MSVC Debug, die 90 Sprachtests zusätzlich mit Clang Release. Alle 82
betroffenen CTest-Prüfungen bestehen ebenfalls. Die Einstiegspunkte verwenden denselben
Katalog; die Umwandlung bereits vorhandener CRLF-Zeilenenden wurde für CMake unter
Windows korrigiert. Der direkte Build- und Testschritt dieses Stands besteht
in allen acht CI-Plattformkombinationen zum Commit `430df15`.

Neun weitere Diagnosegruppen und acht Laufzeitprogramme prüfen Generics,
Funktions-/Methodenüberladungen, Strukturdefaults und eigene Initialisierer direkt.
Importierte Fehlerfälle erhalten getrennte Quelldateien; die erwarteten Diagnosen
verweisen weiterhin auf den richtigen Import und dessen Zeile. Ein weiterer Ablauf
baut generische Experiment-/Analysemodule und prüft ihre separaten Runner und
Ausgabedateien. Damit umfasst der direkte Katalog 152 Tests, davon 142 ohne SDL.
Alle 108 Sprachtests bestehen lokal mit MSVC Debug und Clang Release; alle 100
betroffenen CTest-Prüfungen bestehen ebenfalls. Der Läufertest prüft außerdem,
dass Emissions-, Build-, Prozessfehler und fehlende Dateien Folgeschritte verhindern.
Dieser Stand besteht inzwischen im direkten CI-Schritt aller acht
Plattformkombinationen zum Commit `0a937f8`.

Weitere 16 Gruppen übernehmen Kontrollfluss, optionale Werte, Enums, verzögerte
Initialisierung, Unicode, Zahlkonvertierung und Ganzzahlfehler in den direkten Läufer.
32 zusätzliche Programme prüfen auch Closures, Bereichszuweisungen, importierte
Enums und die Erholung des `attempt`-Experimentmoduls nach einem Fehler. Erwartete
Laufzeitfehler müssen Exitcode 70 und ihre genaue Quelldiagnose liefern. Die
Byteprüfung mehrzeiliger Strings vergleicht weiterhin das erste erzeugte Literal;
ein späteres passendes Literal kann ein falsches erstes Ergebnis nicht verdecken.
Der direkte Katalog umfasst jetzt 200 Tests, davon 190 ohne SDL. Alle 156 Sprachtests
bestehen lokal mit MSVC Debug und Clang Release, ebenso die 147 zugehörigen
CTest-Prüfungen. Der direkte CI-Schritt zum Commit `d7fb0b0` besteht inzwischen
in allen acht Plattformkombinationen.

Physikalische Einheiten, Materialien, Medien und Kugelauftrieb werden jetzt
ebenfalls direkt geprüft, einschließlich statischer Dimensionsfehler und
Quelldiagnosen bei ungültigen Laufzeitwerten. Drei weitere Analyseabläufe prüfen
Reihenwerte, Quantile und CSV-Spalten. Exportprüfungen vergleichen den vollständigen
CSV-Inhalt und verlangen, dass abgelehnte Exporte keine CSV-Datei anlegen.
Der direkte Katalog enthält jetzt 222 Tests, davon 212 ohne SDL. Alle 178 Sprachtests
bestehen lokal mit MSVC Debug und Clang Release; die 163 zugehörigen CTest-Prüfungen
bestehen ebenfalls. Der Läufertest erkennt falsche Inhalte, ungültiges UTF-8,
fehlende Dateien und unerwünschte Ausgaben. Der direkte CI-Schritt zum Commit
`0923b6e` besteht auf Linux mit GCC/Clang, beiden macOS-Architekturen sowie Windows
mit MSVC Debug und ClangCL Debug/Release; MSVC Release ist noch offen.

Compiler-CLI und Modulimporte laufen nun ebenfalls direkt. 52 Compileraufrufe
prüfen Host-Beschränkungen, Unicodepfade, Diagnosepositionen, Größenbegrenzung,
Abhängigkeiten, Importzyklen und die Suchreihenfolge. Die lokale Modulpriorität
wird zusätzlich anhand der tatsächlich aufgelisteten Datei geprüft. Drei weitere
Programme prüfen Importe, zusätzliche Suchpfade und einen Fehler aus einer
importierten Funktion. Der direkte Katalog umfasst 228 Tests, davon 218 ohne SDL.
Alle 184 Sprachtests bestehen lokal mit MSVC Debug und Clang Release; die 169
zugehörigen CTest-Prüfungen bestehen ebenfalls mit den gemeinsamen Erwartungen.
Der CI-Nachweis dieser Erweiterung steht noch aus.

| Bereich | Implementiert | Noch offen |
| --- | --- | --- |
| Eigene Sprache | verbindliches Ziel als vollständige C-Alternative, C17-Lexer/Parser mit `:`-/Einrückungsblöcken, skalare und nominale Struktur-/Enumtypen mit typisierten und besitzenden Nutzdaten sowie optionale Werte mit Wertsemantik und struktureller Gleichheit, normale/mutierende/statische Strukturmethoden, eigene Struct-Initialisierer mit Überladung nach Parameterform und Parametertyp auch bei generischen Typen, `physimc --check`/`--emit-c`/`--emit-experiment`/`--emit-analysis`, C17-Backend/CMake für Programme und erste Experiment-/Analysemodule, Vec2/Vec3/Vec4, Quaternionen und Mat3/Mat4, Einheiten/Kanäle, explizite PCG32-Wertströme, starre Körper mit Impulsen/Quaternionrotation, Kontaktpaare mit Reibung/Rückprall, Distanzgelenke mit lokalen Ankern, gemeinsamer Körpergruppen-Solver mit besitzenden Ergebniswerten, lineare Kugel-Sweeps und Hüllquader-Kandidatenpaare, gemeinsamer Integrator, Messdaten/Szene, Dataset-/Series-/Plot-/Table-Handles einschließlich erzeugter Datenreihen, gemeinsame Messstatusauswahl, Statistik/Diagramme/Tabellen/Exporte, abgefangene Laufzeitfehler mit Quelldiagnosen, erste App-Vorlagen mit Editor/Build und Quellsnapshots | vollständiger semantischer Sprachvertrag, weitere Werttypen und Fallmuster, Überladungsauflösung für weitere Ausdrücke, erweiterte Module, vollständige Experiment- und Analysebindungen, vollständige Integration beider Editoren, vollständiger Sprachausbau und zwei getestete Dokumentationsteile (LANG-001 bis LANG-007) |
| Foundation | C17, CMake, MIT, Windows-/POSIX-/Darwin-Schicht, erfolgreiche Windows-CI und Linux-CI mit GCC und Clang, macOS-CI und geprüfte App-Pakete für Apple Silicon und Intel | weitere macOS-Versionen und reale Mac-Grafikhardware, öffentliche Mac-Signierung/Notarisierung, Wayland, Clean-Machine-Tests |
| Mathematik | Vec2/3/4 mit skalierter Normalisierung, Mat3/4 mit Inversion, Quaternion-Verknüpfung und Rotationsinterpolation, affine/projektive Punkttransformation sowie Richtungs-/Normalentransformation, absolute/relative Vergleiche, Euler/RK4, symplektischer Euler, Verlet, RK45, linearer Solver, Bisektion, Minimierung, kubische räumliche Bézierkurven mit Tangente und Unterteilung | weitere Kurven-/Interpolationsverfahren, Events/dichte Ausgabe, steife Verfahren |
| Basis | Fehlercodes, expliziter RNG, explizite Allocatoren mit Fehlerprüfungen, feste Arenen, eigene Speicherdomänen für Berichte/Analysekontexte, Test-Allocator mit Fehler-Injektion und Bytebudget, dynamische Arrays mit Größenlimit und Selbstkopien, begrenzte String-Views ohne Kopie, Hashmap mit eigenen Schlüsseln und Größenlimits | strukturierte Diagnosen, Allocator-Anbindung weiterer Subsysteme |
| Einheiten | SI-Dimensionen, Konvertierung, Einheitenalgebra, Quantity-Rechnung und Dimensionsprüfung von Datenreihen | benutzerdefinierte Anzeigeeinheiten in der GUI |
| Runner | versionierte Modul-ABI, Handshake, Pause/Step/Run/Stop, Heartbeat, Crash-/Hang-Isolation | Ressourcenlimits, eigener Logkanal, echtes OS-Sandboxing |
| Daten | CRC-Chunks, Streaming, Recovery, CSV, Seed-/Modellmetadaten | Index, mehr Datentypen, komprimierte Blöcke, Schemaerweiterung |
| App | leerer Workspace-Einstieg mit gespeicherter Ordnerauswahl und bewusster Wiederöffnung, kompakte Menüleiste, Projektmanager, aufklappbarer Dateibaum und bis zu 16 editierbare Textdokumente mit separaten Autosaves, drei Arbeitsbereiche, integrierte Offline-Dokumentation, Systemtypografie, Einstellungen mit Code-Schriftgröße und Autosave-Intervall, gespeicherte Darstellung/Fenstergröße/Maximierung, vergrößerbare Seitenleiste und Protokoll, Vorlagen, Editor, direkter Projektbuild ohne CMake mit Ausgaben unter `build/`, Diagramme | vollständige Ablösung von CMake für Physim selbst, mehrere benannte Workspaces und Wiederherstellung von Dokumentansichten, freies Docking, weitere Panelzustände, UI-weite Schriftvergrößerung, helle/kontrastreiche Themes, vollständige Barrierefreiheit |
| Editor | C- und Physim-Dateien bearbeiten, sprachspezifische Syntaxfarben, Zeilennummern, Debug/Release, anklickbare Compilerdiagnosen, öffentlicher Header-Browser | Completion |
| Szene | OpenGL 3.3 Core, Tiefenpuffer, MSAA, alle acht Grundprimitive, orientierte Boxen/Ebenen, RGBA-Transparenz mit Dreieckssortierung, UTF-8-Labels, Wurf-Flugbahn, Grid/Achsen, Kamera, Ansichten und Sichtbarkeit und Mausklickauswahl einzelner Szeneneinträge mit optionalen Objekt-IDs | artefaktfreie Transparenz bei sich durchdringenden Flächen, Szenenhierarchie, Zeitleiste |
| Analyse | eigener C-Editor/Runner, Dataset-/Series-Handles, blockweise Transformationen mit Einheitenprüfung, eigene Ergebnisplots/-tabellen, Linien/Punkte/Histogramme mit Zoom am Mauszeiger, Verschieben, separaten Ausschnitten und Achsenoffsets, PNG-/SVG-Export des sichtbaren Ausschnitts, CSV/SVG und verlustfrei komprimiertes PNG mit vier Größen von 1200 × 850 bis 4800 × 3400 Pixeln, Statistik, Ableitung, Integral, gleitendes Mittel, Periode, Energieabweichung, Auswahl und Vergleich von bis zu acht Läufen, gemeinsame Statusauswahl von Datenreihen, lineares Resampling, Nearest/Previous im SDK und Differenzkurven, frühere Berichte öffnen | weitere Interpolationsverfahren/Transformationen |
| Mechanik | starre Körper mit Kugel-/Boxträgheit, Quaternionrotation, Drehmomente/Impulse, Kugel–Kugel/Kugel–Ebene/Kugel–Box/Box–Ebene/Box–Box, iterative Paar- und Graph-Solver mit Coulomb-Reibung/Restitution (bis zu 128 Körper und 512 Kontakte), Distanzgelenk mit lokalen Ankern und Driftkorrektur, gemeinsamer Geschwindigkeits-Solver für Kontakte und bis zu 256 Gelenke, Feder/Dämpfung, Stokes-/quadratischer Widerstand, Kugelstoß-, Boxstoß- und Bodenkontaktvorlagen mit Debug-Vektoren, Feder–Masse–Dämpfer mit dissipierter Arbeit und Energiebilanz, archimedischer Auftrieb und Kugel-Eintauchvolumen samt Auftriebsmittelpunkt, Auftriebsvorlage mit Kraftanzeige und Energiebilanz | automatische Kontaktverwaltung, Warmstart, gemeinsame nichtlineare Positionsprojektion, weitere Gelenke, Box-CCD, erweiterte Stoffmodelle |
| Unsicherheit | PCG32, geprüfte konstante/uniforme/normale Verteilungen, öffentliche Sensor-API mit Einheiten, Zeitraster, Auflösung, Offset, Drift, Rauschen, Ausfällen und Standardunsicherheit, getrennte Modell-/Soll-/Messwerte, gültigkeitsbewusste Vorschau/Statistik/CSV, Batchcontroller in App/CLI mit bis zu acht Runnern, expliziten Seeds, eigenen Arbeitsordnern, Abbruch und fester Auswertungsreihenfolge, Endwert-Histogramm/Typ-7-Quantile, Normalnäherung des Mittelwert-KI ab 200 Läufen und lineare Parameterstudien in CLI und App mit Kurvenbericht | Wiederaufnahme, Batchstatistik mit fehlenden Endwerten, Erhaltung von Messlücken als explizite Masken, korrelierte Sensor-/Unsicherheitsmodelle, weitere Konfidenzverfahren und Verteilungsdiagnostik |
| Produktreife | Quellen-Snapshots, versionierte Dateien, Backup beim Speichern, Autosave beider C-Editoren mit wählbarem Intervall (Standard 30 Sekunden), Wiederherstellung mit Erkennung extern geänderter Quellen | gleichzeitige Autoren/Dateizusammenführung, Projektmigration, Installer, Leistungsbudget, abdeckungsgeführte Langzeit-Fuzzing-Kampagne |

Keine Kennzeichnung als stabile 1.0 und keine Behauptung, dass alle 25 Starttickets
oder die Phasen 0–10 bereits abgenommen sind. Die offene Arbeit bleibt nachvollziehbar
an den Kriterien des ursprünglichen Projektplans orientiert.

Die globale Leiste verwendet jetzt kompakte Menüs für **Datei** und **Ansicht**
sowie direkte Zugänge zu Hilfe und Einstellungen. Die Arbeitsbereiche liegen
als 30 Pixel hohe, gleich breite Tabs über die gesamte Fensterbreite direkt darunter.
Die Tab-Auswahl hat eine abgerundete, leicht eingerückte, neutral graue Fläche.
Die Menüzeile hat einen einheitlichen Hintergrund ohne einzelne Buttonflächen
im Ruhezustand. Nur Hover und das offene Menü werden hervorgehoben.
Ein gemeinsames Popup ermöglicht den direkten
Wechsel in beide Richtungen sowie zu Hilfe und Einstellungen mit einem Klick.
Bei offenem Menü wechselt auch das Bewegen des Mauszeigers zwischen Datei und
Ansicht das Popup. Menüeinträge sind bündig ausgerichtet, Tastenkürzel stehen
rechts; die Einträge erhalten nur beim Überfahren eine eigene Farbfläche.
Windows 11 zeichnet abgerundete äußere Ecken im normalen Fenstermodus.
Menüwechsel und Darstellung wurden per SDL-Test und echter Windows-Eingabe geprüft.
Menüs und Fensterknöpfe teilen sich den 32 Pixel hohen Fensterkopf; die zusätzliche
native Titelzeile entfällt. SDL-Hit-Testing ermöglicht Verschieben und Größenänderung.
Die großen Seitenleisten-Schaltflächen entfallen; der gemeinsame Kopfbereich ist
64 Pixel hoch. UI-Tests prüfen
Menüaktionen, deaktivierte Einträge, Tab-Wechsel und Tastaturbedienung bei
1080 × 740 und 1440 × 940. Einstellungen, Dokumentationsfenster und die
Workspace-Abläufe bestehen weiterhin. Maximieren, Wiederherstellen, Minimieren und
Schließen über die integrierten Fensterknöpfe sowie die Größenänderungs- und
Verschieberegionen sind ebenfalls geprüft. Zusätzlich wurden Verschieben,
Größenänderung, Darstellung und Schließen mit echten Windows-Mauseingaben geprüft.
Der Wiederherstellungsdialog behält
seine eigenen Fensterknöpfe und verwendet denselben sicheren Schließpfad.

Der Workspace besitzt einen aufklappbaren Dateibaum. Verzeichnisse werden beim
Öffnen gelesen und vor Dateien sortiert; Aktualisieren erhält aufgeklappte Pfade
je Wurzeleintrag. Die frühere Grenze von 128 flachen Einträgen wurde durch eine
sichtbar begrenzte Baumansicht mit bis zu 8192 Einträgen und 64 Ebenen ersetzt.
Zusätzliche Wurzeleinträge behalten ihre Plätze, fehlende Pfade und Lesefehler
werden angezeigt. Bis zu 16 UTF-8-Textdateien mit jeweils höchstens 256 KiB
öffnen sich als unabhängige editierbare Dokumente mit Suche und Ersetzen.
Binärdateien, ungültiges UTF-8 und größere Dateien werden abgewiesen.
Speichern prüft externe Änderungen und erhält die vorherige Datei als Backup.
Schließen und Neuladen bieten bei Änderungen Speichern, Verwerfen und Abbrechen.
Workspace-Wechsel, Build und normales Beenden speichern offene Dokumente;
ein Konflikt blockiert den Übergang und zeigt das betroffene Dokument.
Projektquellen verwenden weiterhin ihre bestehenden Editoren.
Modell- und App-Tests für Dokumente bestehen unter MSVC Debug bei beiden
Fenstergrößen. Allgemeine Dokumente haben jetzt eigene Autosaves außerhalb des
Workspace im persönlichen App-Datenverzeichnis. Beim erneuten Öffnen derselben
Datei werden Wiederherstellen, Verwerfen und Schließen ohne Auswahl unterstützt.
Externe Änderungen werden vor dem Wiederherstellen angezeigt; beschädigte
Sicherungen bleiben erhalten und pausieren nur Autosave der betroffenen Datei.
Explizites Verwerfen aktiviert Autosave wieder. Das Intervall folgt den Einstellungen.
Der Prozess-Test beendet den Schreiber ohne normalen Speicher-/Aufräumpfad und
prüft zwei unabhängige Dokumente, zeitgesteuerte Sicherung, Änderungen gleicher
Länge, alle Wiederherstellungsentscheidungen und unveränderte Quelldateien.
Die Modelltests prüfen zusätzlich fremde Pfade im Cache, beschädigte Daten,
leere Entwürfe und Erhalt der vorherigen Sicherung bei fehlgeschlagenem Schreiben.
Eine nach dem Öffnen beschädigte Sicherung bleibt auch beim normalen Speichern
erhalten. Explizites Zurücksetzen und das vollständige Rückgängigmachen eines
wiederhergestellten Entwurfs werden ebenfalls geprüft.
Pfadänderungen/gelöschte Quellen und automatisches Öffnen der Dokumente nach
Neustart bleiben offen.

Die Dokument-Wiederherstellung besteht in separaten Prozessen mit MSVC Debug,
Clang Debug und MSVC AddressSanitizer. Letzterer prüft zusätzlich den gesamten
Dokumentablauf einschließlich Entfernen und Verschieben offener Editorzustände.
Dabei wurde ein Heap-Lesezugriff außerhalb der Grenzen im eingebundenen
TrueType-Parser beim Laden der Systemschrift gefunden und behoben: Konturen
mit einem einzelnen Kurvenkontrollpunkt bleiben jetzt innerhalb ihrer Grenzen.
Der synthetische `font_shape`-Test besteht unter MSVC Debug und AddressSanitizer.
Zusätzliche Dokumentänderungen und das Neuladen extern geänderter Dateien machen
den Build ungültig. Eine Änderungsnummer bindet den erfolgreichen Build samt
Parameterabfrage an den ursprünglichen Bearbeitungsstand und das Buildprofil.
Zwischenzeitliches Speichern kann einen veralteten Build nicht wieder freigeben.
Der neue App-Test `documents_build` baut ein Projekt mit zusätzlichem Header und
prüft Änderungen während Konfiguration und Parameterabfrage, erfolgreiche
Neubauten sowie die Startsperre nach Bearbeiten und externem Neuladen.
Beim fehlgeschlagenen Speichern aus dem Schließen-Dialog bleibt die Auswahl
zwischen Speichern, Verwerfen und Abbrechen erhalten. Tab-Einrückung mit vier
Leerzeichen ist jetzt in allen Editoren ein einzelner Undo-/Redo-Schritt.
Nachweis unter Windows: Dokumentabläufe und der echte Buildtest bestehen mit
MSVC Debug und Clang Debug; Clang prüft zusätzlich beide Menügrößen und die
Dokument-/Clipboard-Modelle (7/7). Beide Modelltests bestehen auch mit MSVC
AddressSanitizer. Die bestehenden Unicode- und Mehrzeilen-Sprachvorschauen
bestehen unter MSVC. Linux bleibt offen.

Zusätzliches verbindliches Ziel, konkretisiert: Alle Projektdateien bleiben im
Projektordner; Buildprodukte liegen gesammelt unter `build/`. Die App erzeugt
`physim.project` und keine `CMakeLists.txt`. `physim-build` übernimmt direkte
Compileraufrufe, Headerabhängigkeiten über vorverarbeitete Quellen, getrennte
Debug-/Release-Ausgaben und den Schutz vor gleichzeitigen Builds. Der neue Weg
verwendet keine CMake-Installation für Nutzerprojekte. Vorhandene CMake-Dateien
bleiben unberührt. Projekt-Autosaves, Backups und Laufdaten behalten ihre bisherigen
Pfade im Projekt. Allgemeine Dokument-Sicherungen liegen weiterhin im App-Datenbereich.
Langfristig soll auch Physim selbst ohne CMake gebaut werden. Das bleibt offen,
ebenso wie der Nachweis des neuen Projektbuilders auf Linux und auf einem frischen Windows-System.
Nachweis unter Windows: Der native Builder und echte Runner bestehen mit MSVC
und Clang für C- und Physim-Projekte in Debug/Release. Geprüft sind unveränderte
Builds, transitive Headeränderungen, Compiler-/Linkerfehler mit erhaltenen Modulen,
gesperrte Buildordner, ungültige Projektdateien, Unicodepfade, Projektverschiebung
und ignorierte alte CMake-Konfigurationen. Die App-Abläufe für reine Physim- und
gemischte Projekte, Änderungen während des Builds, Laufserien, Autosaves sowie
beide Menügrößen bestehen ebenfalls. Neue Projektbuilds schreiben ausschließlich
in den jeweiligen `build/`-Unterordner; saubere Quellen werden beim Bauen nicht
erneut gespeichert und erzeugen damit keine unnötigen Sicherungskopien.

Die Projektdatei wird von App und Builder über einen gemeinsamen Leser geprüft.
Das Buildprofil wird mit den Experimentparametern gespeichert und beim erneuten
Öffnen wiederhergestellt; Projekte ohne Profileintrag verwenden Debug. Der
Builder verwendet das gespeicherte Profil, wenn kein `--profile` angegeben ist.
Der Schreiber erhält Kommentare, zusätzliche Einträge und die vorhandenen
Zeilenenden, prüft die Daten vor dem Ersetzen und legt bei Änderungen eine
Sicherung der Projektdatei an. Unveränderte gespeicherte Einstellungen werden
nicht erneut geschrieben. Der Modelltest besteht mit MSVC, Clang und MSVC
AddressSanitizer; der App-Test prüft Neustart, Release-Build, Wechsel zu Debug,
Parameterauswahl sowie erhaltene Erweiterungseinträge und unveränderte Quellen.
Der Modelltest deckt 200 Dateien, verschachtelte und doppelt eingebundene Ordner,
Sortierung, Aktualisieren, fehlende Pfade, ungültige Eingaben sowie Eintrags- und
Tiefengrenzen ab. Er besteht unter MSVC Debug, Clang Debug und MSVC mit
AddressSanitizer (RelWithDebInfo). Der App-Test öffnet und schließt Ordner per
SDL-Mausereignissen, liest eine verschachtelte Datei und prüft zusätzliche Wurzeln.
Die Baumansicht samt griechischen Zeichen in der Vorschau wurde bei 1080 × 740
visuell geprüft. Der Linux-Lauf bleibt ausstehend.

Die App speichert jetzt beim normalen Beenden den Hauptordner und bis zu 32
zusätzliche Pfade. **Letzten Workspace öffnen** stellt sie nach einem leeren
Start ausdrücklich wieder her; **Eintrag vergessen** entfernt die Auswahl.
Die separate versionierte Datei prüft Größenlimits, absolute UTF-8-Pfade und CRC.
Fehlende Pfade bleiben erhalten, beschädigte Dateien werden bis zum ausdrücklichen
Zurücksetzen nicht überschrieben. Mehrere benannte Workspaces und die
Wiederherstellung einzelner Dokumentansichten sind weiterhin offen.
Die gezielten Tests bestehen unter Windows mit MSVC Debug und Clang Debug.
Sie prüfen Dateigrenzen und beschädigte Inhalte sowie separate App-Prozesse für
Speichern, Öffnen per UI-Klick, fehlende Pfade, Vergessen und leeren Neustart.
Die bisherigen Workspace-, Einstellungs- und Autosave-Abläufe bestehen ebenfalls;
die neue Startansicht wurde bei 1080 × 740 visuell geprüft. Ein Linux-Lauf dieser
Erweiterung steht aus.

Sprachvertrag 0.167.0 ergänzt die Swift-benannten Arraymethoden
`starts(with:)` und `elementsEqual(_:)` für gleich typisierte Arrays mit
`Equatable`-Elementen. Der Compiler prüft Labels, Elementtypen und
Vergleichbarkeit; beide Operationen werten ihre Eingaben einmal aus und
verändern sie nicht. Laufzeit-, Negativ- und Experimenttests decken leere,
verschachtelte, besitzende und eigene Strukturwerte ab.

Sprachvertrag 0.166.0 ergänzt `Array.split` mit den Swift-Labels `separator:`,
`maxSplits:` und `omittingEmptySubsequences:` für `Equatable`-Elemente. Das
Ergebnis ist ein besitzendes `[[T]]` mit unabhängigen Teilarrays. Die
Typprüfung weist unpassende Separatoren und nicht vergleichbare Elemente ab;
Laufzeit- und Speicherfehlertests prüfen Grenzfälle und Rollback. Ein
Experimentmodul verwendet die neue Methode. Veraltete Array-Namen sind auch
aus der Typvorschau entfernt und werden in Negativtests zurückgewiesen. Der
Clang-Gesamtbuild, 199 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Die
gezielten Tests bestehen auch unter MSVC Release und ASan Debug.

Sprachvertrag 0.165.0 erweitert `String.split` um Swifts Labels
`maxSplits:` und `omittingEmptySubsequences:`. Leere Teilstücke werden
standardmäßig ausgelassen; bei `false` bleiben sie erhalten. Ein nichtnegatives
Limit zählt nur Teilstücke vor dem verbleibenden Rest; ausgelassene leere
Teilstücke verbrauchen keinen Split. Der leere Trenner bleibt als
Physim-Erweiterung definiert. Laufzeit-,
Negativ- und Besitztests sowie ein Experimentmodul prüfen die neue Signatur.
Der Clang-Gesamtbuild, 197 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.164.0 ergänzt die Swift-benannten `Int64`-Methoden
`isMultiple(of:)` und `signum()`. Die Vielfachheitsprüfung behandelt den
Divisor null und den kleinsten `Int64`-Wert mit Divisor -1 ohne Fehler oder
Überlauf. Laufzeit- und Negativtests prüfen Grenzwerte, Argumentlabels,
Typfehler und einmalige Auswertung von Empfänger und Argument; ein
Experimentmodul verwendet beide Methoden. Der Clang-Gesamtbuild, 196
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Die
gezielten Tests bestehen auch unter MSVC Release und ASan Debug.

Sprachvertrag 0.163.0 ergänzt `Comparable` als generischen Constraint für
`Int64`, `Float64` und `String` unter Swifts Protokollnamen. `<`, `<=`, `>` und
`>=` ordnen nun auch Strings lexikografisch nach Unicode-Skalaren. Der
Constraint gilt für Funktionen, Methoden, Strukturen und Enums;
Spezialisierungen mit `Bool` und Arrays werden abgewiesen. Tests decken
Unicode-Vergleiche, generische Sortierung, negative Typfälle und ein
Experimentmodul ab. Der Clang-Gesamtbuild, 196 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen. Gezielte Tests bestehen
unter MSVC Release und ASan Debug.

Sprachvertrag 0.162.0 erkennt vollständige Enum-Nutzdatenabdeckung durch
mehrere unbewachte `switch`-Zweige. Der Checker prüft Bool-Kombinationen und
verschachtelte optionale Felder auch über mehrere Nutzdatenfelder hinweg.
Unbegrenzte Skalarfelder benötigen für die vollständige Abdeckung eine
Bindung oder `_`; Guards tragen nicht dazu bei. Laufzeit- und Negativtests
decken fehlende Kombinationen, bewachte Zweige, String-/Bool-Felder und ein
Experimentmodul ab. Der Clang-Gesamtbuild, 194 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen. Gezielte Tests bestehen
unter MSVC Release und ASan Debug.

Sprachvertrag 0.161.0 erkennt die vollständige Abdeckung verschachtelter
Optionalwerte im `switch` ohne unnötiges `default`: Auf jeder Ebene werden
unbewachte `nil`- und `some`-Muster zusammengeführt; ein innerer `Bool` ist
durch `true` und `false` vollständig. Guards zählen nicht zur Abdeckung.
Laufzeit- und Negativtests prüfen verschachtelte Werte einschließlich
`String`-Bereichsmustern, und ein Experimentmodul verwendet den neuen Pfad.
Der Clang-Gesamtbuild, 194 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen;
gezielte Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.160.0 erweitert feste geschlossene und halboffene
`String`-Bereichsmuster auf `Optional.some` und Enum-Nutzdaten, auch in
verschachtelten optionalen Feldern. Die Typprüfung erkennt ungültige Grenzen
und vollständig verdeckte Muster; teilweise Überlappung bleibt in
Fallreihenfolge zulässig. Der C-Emitter vergleicht die Stringgrenzen in allen
diesen Mustern. Laufzeit- und Diagnosetests prüfen Grenzen, Unicode,
Überdeckung, Guards und Experimentmodule. Der Clang-Gesamtbuild,
194 Sprachtests, beide App-Workflows und die Referenzprüfung bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.159.0 ergänzt feste geschlossene und halboffene
`String`-Bereichsmuster im direkten `switch`. Die Typprüfung vergleicht
decodierte UTF-8-Inhalte, weist leere Bereiche und überlappende Fälle ab
und berücksichtigt Guards. Der C-Emitter prüft dieselben Grenzen ohne
temporäre Stringallokation. Tests decken Unicode, Grenzwerte, mehrere
Muster, Guards, Diagnosen und ein Experimentmodul ab. Der
Clang-Gesamtbuild, 192 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release
und ASan Debug.

Sprachvertrag 0.158.0 ergänzt `String.sorted()`/`sorted(by:)` sowie
`String.min()`/`max()` mit und ohne `by:` unter Swifts Methodennamen.
Die Methoden ordnen Unicode-Skalare als eigene Strings; Sortierung ist
stabil, leere Strings liefern `[]` beziehungsweise `nil`. Eine gemeinsame
Umwandlung in ein Array besitzender Skalarwerte nutzt die vorhandenen
Sortier- und Extremfunktionen. Tests prüfen Unicode, Vergleicher,
gebundene und lokale Methoden, Closures, Snapshots, abgefangene Fehler,
Allokationsfehler und ein Experimentmodul. Der Clang-Gesamtbuild, 190
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.157.0 ergänzt `String.first` und `String.last` mit Swifts
Eigenschaftsnamen. Die Werte sind unabhängige `String?` mit je einem
Unicode-Skalar; leere Strings liefern `nil`. Tests prüfen Unicode,
Momentaufnahmen, einmalige Empfängerauswertung, unveränderliche Eigenschaften
und ein Experimentmodul. Eine Überschneidung der internen Bindungskennungen
mit Physikfeldern wurde beim Gesamtbuild entdeckt und beseitigt; eine
Kompilierprüfung schützt den Kennungsbereich. Der Clang-Gesamtbuild, 188
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.156.0 ergänzt `Array.forEach(_:)` und `String.forEach(_:)`
mit Swifts Methodennamen. Beide rufen eine `Void`-Funktion in
Elementreihenfolge auf; Strings liefern Unicode-Skalare als einzelne Strings.
Tests prüfen leere Eingaben, Funktionswerte, Closures, gebundene und lokale
Methoden, Snapshots, Fehlerbereinigung und ein Experimentmodul. Der
Clang-Gesamtbuild, 186 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.155.0 ergänzt `String.reduce(_:_:)` mit Swifts Methodennamen.
Die Methode verarbeitet Unicode-Skalare in Quellreihenfolge und gibt bei
leerem String den Startwert unverändert zurück. Der Akkumulator kann auch
ein besitzender Wert sein. Tests prüfen skalare und besitzende Akkumulatoren,
leere Eingaben, Closures, gebundene und lokale Methoden, Snapshots,
Fehlerbereinigung und ein Experimentmodul. Der Clang-Gesamtbuild, 184
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.154.0 ergänzt `String.compactMap(_:)` und
`String.flatMap(_:)` mit Swifts Methodennamen. Die erste Methode sammelt
vorhandene Ergebnisse von `func(String) -> U?`, die zweite fügt Teilarrays
von `func(String) -> [U]` zusammen. Beide verarbeiten Unicode-Skalare in
Quellreihenfolge und erzeugen unabhängige Arrays mit geometrisch wachsendem
Puffer. Tests prüfen Unicode, leere Eingaben, optionale und verschachtelte
Werte, Structs, Closures, gebundene und lokale Methoden, Snapshots,
Fehlerbereinigung und ein Experimentmodul. Der Clang-Gesamtbuild, 182
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.153.0 ergänzt `String.map(_:)` mit Swifts Methodennamen und
Array-Ergebnis. `func(String) -> U` verarbeitet jeden Unicode-Skalar als
eigenen String. Der Ausgabepuffer wächst geometrisch; besitzende Elemente
werden nach ihren Wertregeln kopiert und bei Fehlern freigegeben. Tests prüfen
Unicode, leere Eingaben, Arrays, Structs, Enums, Funktionswerte, Closures,
gebundene und lokale Methoden, Snapshots, Fehlerbereinigung und ein
Experimentmodul. Der Clang-Gesamtbuild, 178 Sprachtests, beide App-Workflows
und die Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release
und ASan Debug.

Sprachvertrag 0.152.0 ergänzt `String.first(where:)`, `last(where:)`,
`firstIndex(where:)` und `lastIndex(where:)` mit Swifts Methodennamen.
Passende Unicode-Skalare werden als unabhängige `String?` und nullbasierte
Skalarindizes als `Int64?` geliefert; die letzten Treffer werden rückwärts
gesucht. Tests prüfen Unicode, leere Eingaben, Closures, gebundene und lokale
Methoden, Snapshots, Fehlerbereinigung und ein Experimentmodul. Ein
Speichertest prüft die rückwärts laufende Skalariteration auch bei
Allokationsfehlern und ungültigen Cursorpositionen. Der
Clang-Gesamtbuild, 176 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.151.0 ergänzt `String.contains(where:)` und
`String.allSatisfy(_:)` mit Swifts Methodennamen und Kurzschlussregeln.
Das Prädikat `func(String) -> Bool` erhält Unicode-Skalare als einzelne
Strings; leere Eingaben liefern `false` beziehungsweise `true` ohne Aufruf.
Tests prüfen Unicode, Closures, gebundene und lokale Methoden, Snapshots,
Fehlerbereinigung und ein Experimentmodul. Der Clang-Gesamtbuild, 174
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.150.0 ergänzt Swifts Methodennamen `String.filter(_:)`.
Das Prädikat `func(String) -> Bool` erhält jeden Unicode-Skalar als eigenen
String; passende Skalare werden in Quellreihenfolge zu einem unabhängigen
String zusammengesetzt. Der Ausgabepuffer wächst geometrisch und wird bei
einem abgefangenen Fehler vollständig freigegeben. Laufzeit- und Negativtests
prüfen Unicode, leere Werte, Funktionswerte, Closures, gebundene und lokale
Methoden, Snapshots, Fehlerbereinigung und ein Experimentmodul.
Der Clang-Gesamtbuild, 172 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.149.0 ergänzt `String.prefix(while:)` und
`String.drop(while:)` mit Swifts Methodennamen. Das Prädikat hat den Typ
`func(String) -> Bool` und erhält je einen Unicode-Skalar als eigenen String.
Beim ersten `false` endet die Prüfung; die Ergebnisse sind unabhängige Strings
an Skalargrenzen. Laufzeit- und Negativtests prüfen Unicode, leere Werte,
Closures, gebundene und lokale Methoden, Snapshots, Fehlerbereinigung und ein
generiertes Experimentmodul. Der Clang-Gesamtbuild, 170 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen. Gezielte Tests bestehen unter
MSVC Release und ASan Debug.

Sprachvertrag 0.148.0 ergänzt Swifts `Array.prefix(while:)` und
`Array.drop(while:)` für `func(T) -> Bool`. Beide stoppen die Prädikatsprüfung
beim ersten `false`; `prefix` liefert die passende Anfangsfolge und `drop` den
Rest ab diesem Element. Physim liefert eigenständige `[T]`-Werte. Der Parser
akzeptiert dafür das Schlüsselwort `while` als Argumentlabel, ohne die
Zählerform `prefix(count)` zu verändern. Laufzeit- und Negativtests decken
leere und vollständig passende Arrays, besitzende Elemente, Closures,
gebundene und lokale Methoden, Snapshots, Fehler sowie ein generiertes
Experimentmodul ab. Der Clang-Gesamtbuild, 168 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen; gezielte Tests bestehen unter
MSVC Release und ASan Debug.

Der Array-Ergebnisaufbau für `map`, `filter`, `compactMap` und `flatMap` verwendet
im Sprachvertrag 0.147.0 nun einen privaten, geometrisch wachsenden Puffer
statt einer vollständigen Arrayersetzung je Element. Ein Laufzeittest baut 512
Elemente mit acht Blockallokationen auf und prüft Snapshots, Bytebudget sowie
Rollback bei Allokations- und Elementkopierfehlern. Größere String-Ergebnisse
laufen auch im generierten Code. Der Clang-Gesamtbuild, 166 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen; die fünf gezielten Laufzeittests
bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.147.0 ergänzt Swifts `Array.flatMap(_:)` für Transformationen
`func(T) -> [U]`. Die Ergebnisarrays werden in Eingabereihenfolge zu einem
eigenständigen `[U]` zusammengefügt; leere Teilarrays tragen nichts bei.
Laufzeit- und Negativtests decken freie Funktionen, Closures, gebundene und
lokale Methoden, verschachtelte und besitzende Elemente, Snapshots und Fehler
ab. Ein generiertes Experimentmodul verwendet dieselbe Methode. Der
Clang-Gesamtbuild, 166 Sprachtests, beide App-Workflows und die generierte
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.146.0 ergänzt Swifts `Array.compactMap(_:)`: Eine Funktion
`func(T) -> U?` wird für jedes Eingabeelement einmal aufgerufen, `nil` wird
ausgelassen und vorhandene Werte bilden ein eigenständiges `[U]`. Tests decken
freie Funktionen, Closures, gebundene und lokale Methoden, verschachtelte
Optionals, besitzende Werte, Snapshots, Fehlerbereinigung und ein generiertes
Experimentmodul ab. Der Clang-Gesamtbuild, 164 Sprachtests, beide App-Workflows
und die generierte Referenzprüfung bestehen. Gezielte Tests bestehen unter
MSVC Release und ASan Debug.

Sprachvertrag 0.145.0 ergänzt `String.prefix(_:)`, `suffix(_:)`,
`dropFirst(_:)` und `dropLast(_:)` mit Swifts Methodennamen. Physim schneidet
wie bei seinen Stringindizes an Unicode-Skalargrenzen und liefert einen
unabhängigen `String` statt `Substring`; die Drop-Methoden verwenden ohne
Argument eins. Laufzeit-, Negativ-, Speicher- und Experimentmodultests decken
die API ab. Der Clang-Gesamtbuild, 162 Sprachtests, beide App-Workflows und
die generierte Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC
Release und ASan Debug.

Sprachvertrag 0.144.0 ergänzt Swifts Arraynamen `prefix(_:)`, `suffix(_:)`,
`dropFirst(_:)` und `dropLast(_:)`. Die beiden Drop-Methoden erlauben die
Swift-Vorgabe von einem Element; größere Anzahlen werden begrenzt, negative
Anzahlen abgewiesen. Physim liefert wegen seiner vorhandenen Slice-Wertsemantik
einen unabhängigen `[T]`-Wert statt `ArraySlice`. Der Clang-Gesamtbuild,
160 Sprachtests, beide App-Workflows und die generierte Referenzprüfung
bestehen. Gezielte Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.143.0 ergänzt Swifts `array.removeSubrange(_:)` und
`array.replaceSubrange(_:with:)` für ausdrücklich begrenzte halboffene und
geschlossene Bereiche. Beide Methoden verwenden die vorhandene geprüfte
Bereichs- und Ersetzungslogik; Snapshots, Selbstersetzung und verschachtelte
Arraypfade bleiben gültig. Ungültige Grenzen verhindern die Auswertung des
Ersatzarrays. Der Clang-Gesamtbuild, 158 Sprachtests, beide App-Workflows und
die generierte Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC
Release und ASan Debug.

Sprachvertrag 0.142.0 ergänzt Swifts `array.swapAt(_:_:)` für veränderliche
Arraypfade. Zwei positionale Indizes müssen vorhandene Elemente bezeichnen;
ein Selbsttausch ist wirkungslos. Die Implementierung kopiert vor dem Tausch,
damit Snapshots und besitzende Elemente gültig bleiben und Kopierfehler keine
teilweise Mutation veröffentlichen. Die gesamte Sprachtestsuite mit 156 Tests,
beide App-Workflows und die generierte Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.141.0 ergänzt Swifts `array.removeFirst()` und
`array.removeLast()` sowie die Varianten mit positionaler Anzahl. Die
Einzelelementformen liefern einen eigenen `T`-Wert, die Anzahlformen `Void`.
Leere Arrays und ungültige Anzahlen melden Laufzeitfehler. Laufzeit-,
Negativ-, Allocator- und Experimentmodultests prüfen besitzende Rückgaben,
Snapshots, verschachtelte Empfänger und Fehler-Rollback.
Der Clang-Gesamtbuild, 154 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.140.0 ergänzt Swifts `array.append(contentsOf:)` und
`array.insert(contentsOf:at:)` für gleich typisierte Arrays. Selbstkopien,
besitzende Elemente und verschachtelte Zugriffspfade behalten Wertsemantik;
fehlgeschlagene Kopien veröffentlichen keinen Teilwert. Laufzeit-, Negativ-,
Allocator- und Experimentmodultests prüfen die neuen Methoden.
Der Clang-Gesamtbuild, 152 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.139.0 ergänzt Swifts `array.removeAll()` und
`array.removeAll(where:)`. Die Prädikatform arbeitet auf einem festen Snapshot,
bewahrt die Reihenfolge der übrigen Elemente und veröffentlicht die Mutation
erst nach erfolgreicher Auswahl und Kopie. Laufzeit-, Negativ-, Allocator- und
Experimentmodultests prüfen besitzende Werte, verschachtelte Empfänger,
Closures, gebundene Methoden und Fehler.
Der Clang-Gesamtbuild, 150 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.138.0 ergänzt Swifts `array.min()` und `array.max()` sowie
`min(by:)` und `max(by:)`. Leere Arrays liefern `nil`; für eigene Elementtypen
entscheidet die Vergleichsfunktion. Bei Gleichständen bleibt das erste
Element ausgewählt. Laufzeit- und Negativtests prüfen primitive und besitzende
Typen, Closures, gebundene Methoden und fehlerhafte Vergleichsfunktionen.
Der Clang-Gesamtbuild, 148 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.137.0 ergänzt Swifts `array.sorted(by:)` und `array.sort(by:)`
für Vergleichsfunktionen beliebiger Array-Elementtypen. Die Reihenfolge ist
stabil; eine fehlgeschlagene mutierende Sortierung lässt den Empfänger
unverändert. Laufzeit-, Negativ- und Allocator-Tests decken besitzende Werte,
Closures, gebundene Methoden, verschachtelte Empfänger und Fehler ab.
Der Clang-Gesamtbuild, 146 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.136.0 ergänzt `array.last(where:)`,
`array.firstIndex(where:)` und `array.lastIndex(where:)`. Die Suchrichtung
bestimmt den ersten geprüften Wert; alle drei Ergebnisse sind optional.
Gezielte Laufzeit- und Negativtests prüfen frühes Beenden, leere Arrays,
fehlende Treffer und besitzende Elemente. Der Clang-Gesamtbuild, 145
Sprachtests und beide App-Workflows bestehen. Ein Experimentmodultest prüft
alle vier Prädikatsuchen unter Clang und MSVC Release; der Programmlauf besteht
zusätzlich unter MSVC Release und ASan Debug. Die generierte Sprachreferenz
ist geprüft.

Sprachvertrag 0.135.0 ergänzt die Swift-Eigenschaften `array.isEmpty`,
`array.first` und `array.last` sowie `array.first(where:)`. Die Endpunkte
liefern `T?`, bei leerem Array `nil`, und kopieren auch besitzende Elemente
unabhängig; die Prädikatsuche endet beim ersten Treffer. Laufzeit- und
Negativtests prüfen leere Arrays, verschachtelte und optionale Elemente,
Snapshots sowie unveränderliche Eigenschaften. Der vollständige Clang-Build,
145 Sprachtests und zwei App-Workflows bestehen; der gezielte Laufzeittest
besteht zusätzlich unter MSVC Release und ASan Debug. Die generierte
Sprachreferenz ist geprüft. Der Checker weist Zuweisungen an eingebaute
schreibgeschützte Eigenschaften nun ohne Absturz zurück.

Sprachvertrag 0.134.0 gleicht sichere Arrayoperationen an Swift an:
`array.popLast()` entfernt das letzte Element und liefert `T?`, bei einem
leeren Array `nil`. Für sichere Indexzugriffe dienen nun
`attempt(array[index])` und `attempt(text[index])`; die nicht zu Swift
passenden `get(at:)`-Methoden sind entfernt. Entsprechend ersetzt
`attempt(array.remove(at: index))` den kurzzeitig eingeführten Aufruf
`removeIfPresent(at:)`. Die Prädikatmethoden heißen jetzt
`contains(where:)` und `allSatisfy(_)` statt `any` und `all`.
Die String-Wiederholung nutzt nun Swifts Konstruktorform
`String(repeating: text, count: n)` statt `text.repeated(count: n)`.
`Array(repeating: value, count: n)` ersetzt die bisherige Arraymethode
`repeated(count:)` mit Swift-Semantik für ein einzelnes Element. String-Trimmen
verwendet `trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)`;
UTF-8-Bytes werden über `text.utf8.count` gezählt.
Die nicht mutierenden Arraynamen `appending`, `inserting` und `removing` sind
entfernt; Kopien verwenden `+` oder `var` mit den Swift-Namen `append`,
`insert` und `remove`. Die entfernten abweichenden Namen sind in negativen
Sprachtests abgewiesen. Der volle
Clang-Build, 143 Sprachtests und beide App-Workflows bestehen; MSVC Release
und ASan Debug bestehen die gezielten Laufzeittests. Die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.133.0 ergänzt `String.get(at:)` als sicheren Zugriff auf einen
Unicode-Skalar. Gültige Indizes liefern einen besitzenden optionalen String,
ungültige `nil`; Empfänger und Index werden je einmal ausgewertet. Laufzeit-
und Negativtests prüfen Unicode, Grenzen, Besitz und Typfehler. Der Clang-
Gesamtbuild, 142 Sprachtests und zwei App-Workflows bestehen. MSVC Release
besteht beide gezielten Tests, der ASan-Debug-Build den Laufzeittest. Die
generierte Sprachreferenz ist geprüft.

Sprachvertrag 0.132.0 ergänzt `attempt(ausdruck)` als erste lokale Behandlung
von Laufzeitfehlern. Der Ausdruck liefert einen Wert als Optional oder `nil`
bei einem abgefangenen Fehler. Temporäre Besitzer und Rekursionstiefe werden
bei Fehlern wiederhergestellt; bereits sichtbare Seiteneffekte bleiben bestehen.
Gezielte Tests decken verschachtelte Aufrufe, Besitz, wiederholte Fehler und
den Experimentmodul-ABI-Pfad ab. Der Clang-Gesamtbuild, 140 Sprachtests und
zwei App-Workflows bestehen; MSVC Release baut und besteht die drei gezielten
`attempt`-Tests. Die generierte Sprachreferenz ist geprüft.
AddressSanitizer fand eine zunächst zu spät ausgeführte Bereinigung im
Erfolgszweig; nach Korrektur bestehen die eigenständigen und Modul-Tests auch
im ASan-Debug-Build.

Sprachvertrag 0.131.0 ergänzt `array.get(at:)` für sichere Arraylesezugriffe.
Die Methode liefert einen unabhängigen optionalen Elementwert oder `nil` bei
ungültigem Index, auch für besitzende und bereits optionale Elemente. Gezielte
Laufzeit- und Negativtests prüfen Grenzen, Typen, Kopien und Auswertung genau
einmal. Der Clang-Gesamtbuild, 137 Sprachtests und zwei App-Workflows
bestehen; MSVC Release baut und besteht die beiden gezielten Array-get-Tests.

Sprachvertrag 0.130.0 ergänzt `Int64.parse` und `Float64.parse` als
fehlerverträgliche Textkonvertierungen mit optionalem Ergebnis. Ungültige
Schreibweisen und Bereichsüberschreitungen liefern `nil`; die bestehenden
Konvertierungsaufrufe behalten ihre Quelldiagnosen. Compiler-, Laufzeit- und
Negativtests prüfen Grenzen, Argumenttypen und die Verwendung mit `guard`.
Der Clang-Gesamtbuild, 135 Sprachtests und zwei App-Workflows bestehen;
MSVC Release baut und besteht die beiden Parse-Tests. Die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.129.0 ergänzt `guard … else:` für frühe Rückkehr aus Funktionen.
`guard let` und `guard var` halten den entpackten optionalen Wert im
umgebenden Block verfügbar, einschließlich gleichnamiger Bindung.
Die Typprüfung verlangt einen auf allen Pfaden zurückkehrenden `else`-Block
und verhindert Sprünge, die ihn umgehen. Compiler- und Laufzeittests prüfen
Typen, Sichtbarkeit, Eigentum und Kontrollfluss. Der Clang-Gesamtbuild,
133 Sprachtests und zwei App-Workflows bestehen; unter MSVC Release bestehen
die gezielten Lexer-, Parser-, Checker- und Guard-Tests.

Sprachvertrag 0.128.0 ergänzt `\(ausdruck)`-Interpolation in ein- und
mehrzeiligen Strings. Der Lexer erfasst verschachtelte Ausdrücke; der Parser
erhält ihre Quellpositionen. Die Typprüfung akzeptiert `Bool`, `Int64`,
`Float64` und `String`; das Backend wertet jeden Ausdruck einmal aus und
räumt Zwischenwerte nach dem Zusammenfügen auf. Laufzeit-, Typ-, Syntax- und
Mutationstests decken die Funktion ab, einschließlich CRLF-Normalisierung.
Der Clang-Gesamtbuild, 131 Sprachtests und zwei App-Workflows bestehen;
unter MSVC Release bestehen die gezielten Lexer-, Parser-, Checker- und
Interpolationstests.

Sprachvertrag 0.127.0 ergänzt den rechtsassoziativen bedingten Ausdruck
`bedingung ? wennWahr : wennFalsch` mit `Bool`-Bedingung, gleichem Werttyp in
beiden Zweigen und verzögerter Auswertung. Die C17-Ausgabe erhält besitzende
Ergebnisse auch beim Aufräumen der Zweig-Zwischenwerte. Laufzeit- und
Negativtests prüfen Auswertung, Typen und Syntax; die Mutationstests decken
auch unvollständige `?`-Ausdrücke ab. Der Clang-Gesamtbuild, 129 Sprachtests
und zwei App-Workflows bestehen. Unter MSVC Release bestehen die vier
gezielten Parser-, Checker- und Ausdruckstests.

Sprachvertrag 0.126.0 ergänzt `String.trimmed()` auf Basis der Unicode-15.0-
Eigenschaft `White_Space`. Die Methode erhält innere Zeichen, lässt U+200B und
U+FEFF stehen und wahrt den Eingabewert auch bei Allokationsfehlern.
Compiler- und Laufzeittests prüfen die Methode und eigene gleichnamige Methoden.
Der Clang-Gesamtbuild, 127 Sprachtests und beide Kollisions-Workflows bestehen;
der gezielte MSVC-Build und fünf Sprachtests bestehen ebenfalls.

Sprachvertrag 0.125.0 ergänzt `[T].repeated(count:)` für alle Array-Elementtypen.
Die Laufzeit prüft den Zähler und die Ergebnisgröße vor der Allokation und
räumt bei Kopierfehlern besitzende Elemente auf. Compiler- und Laufzeittests
decken Skalare, Strings, verschachtelte Arrays und Strukturen ab.
Der Clang-Gesamtbuild, 125 Sprachtests und beide Kollisions-Workflows bestehen;
der gezielte MSVC-Build und fünf Sprachtests bestehen ebenfalls.

Sprachvertrag 0.124.0 ergänzt `String.repeated(count:)` mit expliziter
`Int64`-Typprüfung, nichtnegativem Zähler, Größenlimit und besitzendem
Stringergebnis. Der native Stringtest prüft Unicode, Grenzwerte und
Allokationsfehler; Sprachtests prüfen Aufruf, Typfehler und Quelldiagnose.
Der Clang-Gesamtbuild, 123 Sprachtests und beide Kollisions-Workflows bestehen;
der gezielte MSVC-Build und fünf Sprachtests bestehen ebenfalls. Nach der
Kopieroptimierung wurden die drei betroffenen Tests auf beiden Compilern
erneut ausgeführt.

Sprachvertrag 0.123.0 ergänzt Unicode-Skalar-Escapes `\u{...}` in beiden
Stringformen. Der Lexer weist ungültige Skalare und U+0000 vor der Ausführung ab;
Typprüfung und C17-Emitter dekodieren gültige Zeichen identisch, auch für
`switch`-Fallmuster. Lexer-, Laufzeit- und Duplikattests decken die Regel ab.
Der Clang-Gesamtbuild, 121 Sprachtests und beide Kollisions-Workflows bestehen;
der MSVC-Build und sieben gezielte Sprachtests bestehen ebenfalls.

Sprachvertrag 0.122.1 vereinheitlicht die Stringdekodierung von Typprüfung und
C17-Emitter. `switch` erkennt nun gleiche einzeilige und dreifach zitierte
Muster, auch bei CRLF im Quelltext, als Duplikat. Ein ausdrückliches `\r`
bleibt von einem normalisierten Zeilenumbruch unterscheidbar. Compiler- und
Laufzeittests prüfen beide Seiten. Der Clang-Build und alle 119 Sprachtests sowie
beide Kollisions-Workflows bestehen; der MSVC-Build und zehn gezielte Sprachtests
einschließlich Editor-Vorschau bestehen ebenfalls.

Sprachvertrag 0.122.0 ergänzt `Int64.abs`, `Int64.min`, `Int64.max` und
`Int64.clamp` als statische Methoden. Sie erhalten die volle 64-Bit-Genauigkeit;
`abs` am kleinsten Wert und vertauschte Clamp-Grenzen melden die Quellposition.
Positive Laufzeitfälle prüfen Grenzen, benannte Argumente und einmalige
Auswertung; negative Fälle prüfen Typen, Argumentzahl und Laufzeitfehler.
Der vollständige Clang/Ninja-Build und 118 Tests im Sprachfilter bestehen.
Zehn betroffene Tests bestehen zusätzlich unter MSVC Release; Kugelstoß- und
Boxstoß-App-Abläufe bestehen im Clang-Build, der Boxstoß-Ablauf auch unter MSVC.
Die generierte Bibliotheksreferenz enthält die vier neuen Methoden.

Sprachvertrag 0.121.0 ergänzt dreifach zitierte mehrzeilige Strings. Der Lexer
verfolgt Zeile und Spalte auch innerhalb des Literals; der C17-Emitter speichert
echte CRLF-, CR- und LF-Zeilenumbrüche einheitlich als LF. Escapes behalten
ihre ausdrückliche Bedeutung. Native Laufzeittests decken leere Werte,
Anführungszeichen und `switch`-Muster ab; ein separater Test prüft die
emittierten Bytes bei gemischten Quell-Zeilenumbrüchen.
Der vollständige Clang/Ninja-Build und 116 gezielte Sprachtests bestehen;
Lexer-, CLI-, Experiment-, Analyse-, Kollisions- und Laufzeittests bestehen
zusätzlich unter MSVC Release.

Die Boxstoß-Sprachvorlage ergänzt den Sprachvertrag 0.120.0 ohne neue
Bindungen. Winkel, Versatz, Rückprall und Reibung sind zur Laufzeit einstellbar.
Der Paritätstest vergleicht die drei C-Referenzvarianten über 401 Zeitpunkte
in allen 15 Messkanälen und die Szene vor und nach dem Stoß. Die eigene
Sprachauswertung liest C- und Sprachläufe. Der App-Workflow prüft Editor,
Build, Simulation, Analyse, Exporte sowie das Speichern und erneute Laden
eines geänderten Rückprallparameters. Paritäts- und App-Test bestehen unter
MSVC Release und Clang/Ninja. Der Workspace-Test legt die Sprachvorlage auch
über den Projektmanager an und prüft Experiment- und Analysequellen.
Auch die 115 nicht als Workflow markierten Sprach- und Boxstoßreferenztests
bestehen im Clang/Ninja-Build.

Sprachvertrag 0.120.0 bindet die C-Einzelkontaktantwort als
`Contacts.resolveSingle`. Eine neue Kugelstoß-Sprachvorlage reproduziert die
vier C-Referenzvarianten für Vakuum, Luft, benutzerdefiniertes Medium und
Reibung mit Anfangsrotation, jeweils mit elf Kanälen und 401 Messpunkten.
Die Vakuumszene stimmt vor und nach dem Stoß überein. Die eigene
Sprachauswertung und der App-Workflow bestehen
unter MSVC und Clang; beide Kreuzrichtungen von C- und Physim-Auswertung sind
ebenfalls geprüft. Ungültige Kontaktzahl und Rückprallparameter liefern
Quelldiagnosen. Medium, Widerstand, Reibung, Rotation, Geschwindigkeit und CCD
sind Experimentparameter; der App-Test prüft Reibung und Rotation bis zur
Laufdatei und zur erneuten Projektöffnung. Der vollständige Clang/Ninja-Build und 113 Sprachtests
bestehen; sechs betroffene Sprachtests bestehen zusätzlich unter MSVC.

Sprachvertrag 0.119.0 schreibt die tatsächliche Sprachversion und die getrennte
Compilerversion in generierte Experiment- und Analysemodule. Deren gespeicherte
Läufe und Berichte tragen damit die Version des übersetzten Quelltexts statt
des früheren festen Entwicklungswerts. Der vollständige Clang/Ninja-Build und
112 gezielte Sprachtests bestehen; die betroffenen Experiment-, Analyse-,
Sensor- und CLI-Tests bestehen zusätzlich unter MSVC.

Die App fragt nach dem Build den Parameterkatalog über `physim-runner --describe`
in einem getrennten Prozess ab. Der Inspector zeigt daraus Eingabefelder mit
Beschreibung, Standardwert und Grenzen; ein interaktiver Lauf übergibt die
gewählten Werte und speichert sie in `.psrun`. Die Laufserien-Ansicht startet
lineare Parameterstudien und lädt deren Kurvenbericht. Der Sprach-Wurf-Workflow
prüft App-Override und App-Parameterstudie bis zur gespeicherten Datei und zum
Bericht. Ausgewählte Parameterwerte bleiben außerdem in `physim.project` über
das erneute Öffnen erhalten. Katalogparser und Runner-Discovery sind separat getestet; die beiden
gezielten Tests bestehen auch unter MSVC.

Sprachvertrag 0.118.0 bindet `Submersion.sphere` an die gemeinsame
Kugel-Eintauchgeometrie. Volumen und Schwerpunktabstand sind kopierbare,
unveränderliche Werte; positive und negative Sprachtests prüfen Geometrie,
Typfehler und Laufzeitdiagnosen. Ein weiteres Sprachbeispiel bildet die
C-Auftriebsvorlage mit zwölf Kanälen ab und wird über 401 Zeitpunkte sowie
26 Szeneobjekte verglichen. Die Sprachauswertung erzeugt drei Diagramme und
einen CSV-Export. Der App-Workflow prüft Projektanlage, Build, Lauf,
Auswertung und erneutes Öffnen unter MSVC und Clang. Der Projektmanager zeigt
dazu passende Vorlagennamen für die gewählte Sprache.

Laufserien übergeben jetzt zusätzlich feste Parameterwerte aus dem Inspector.
Die CLI akzeptiert dafür wiederholte `--param name=wert`-Argumente. Ein
Sprachintegrationstest prüft den gemeinsamen Effekt eines linearen Sweeps
und eines konstanten zweiten Parameters in allen Rohdaten und im Bericht.

Der Batch-CLI-Controller führt nun lineare Studien über einen benannten
Physim-Experimentparameter aus. Jeder Lauf erhält einen eigenen Wert und Seed;
Manifest, Rohdateien und CSV halten die Zuordnung fest. Der Bericht zeigt
Endwert gegen Parameter anstelle einer irreführenden Monte-Carlo-Statistik.
Ein End-to-End-Test prüft drei Sprachläufe, Metadaten, CSV und Bericht.
Die App-Steuerung wurde im folgenden Schritt ergänzt.
Clang/Ninja und MSVC bestehen den neuen Sweep-Test; fünf weitere gezielte
Clang-Tests für Batch, Parameter-API, Einzellauf und Referenz bestanden ebenfalls.

Sprachvertrag 0.117.0 ergänzt benannte Experimentparameter mit Standardwert,
inklusiven Grenzen, Runner-Overrides und gespeicherten wirksamen Werten in `.psrun`.
Ein Sprach-Experiment prüft den Standardwert, einen Override und ungültige Eingaben
vom CLI bis zum gelesenen Lauf. Das App-Parameterformular wurde später ergänzt.
Der vollständige Clang/Ninja-Build besteht. Von 197 CTests bestanden 196 sofort;
der Test der veralteten Tutorial-Codekopie bestand nach deren Aktualisierung
im gezielten Wiederholungslauf. Alle 18 generierten Referenzdokumente sind aktuell.
Der MSVC-Debug-Build und beide gezielten Parametertests bestehen ebenfalls.

Sprachvertrag 0.116.0 ergänzt `Medium.stokesDrag` mit der Viskosität des
Mediums und dem gemeinsamen geprüften Kugel-Widerstandsmodell. Ungültige
Radien melden die Quellposition.
Der vollständige Clang-Build und die Sprachsuite (115/115), fünf gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.115.0 bindet `Material` mit geprüfter Dichte, Rückprall und
Reibung als Werttyp. Der Materialwert liefert die Solverparameter und im
Box-Stoßexperiment die Körpermasse.
Der vollständige Clang-Build und die Sprachsuite (114/114), fünf gezielte
MSVC-Tests einschließlich Kontakt-Workflow und 18 Referenzdokumente bestehen.

Sprachvertrag 0.114.0 bindet `Medium` als Werttyp an die gemeinsame
C-Bibliothek: geprüfte eigene Medien, Luft/Wasser/Vakuum, SI-Eigenschaften
und quadratische Widerstandskraft mit Quellfehlern.
Das Wurfexperiment verwendet die Bindung; der vollständige Clang-Build und
die Sprachsuite (112/112), sechs gezielte MSVC-Tests und 18
Referenzdokumente bestehen.

Sprachvertrag 0.113.0 ergänzt Skalarmultiplikation und -division sowie
Vorzeichenoperatoren für `Quantity`; Einheit und statische Dimension bleiben
erhalten. Division durch null erhält die Quellposition des Operators.
Der vollständige Clang-Build und die Sprachsuite (108/108), sechs gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.112.0 ergänzt `+`, `-`, `+=` und `-=` für `Quantity`-Werte
mit nativer Ausführung, Einheitenumrechnung und statischer Dimensionsprüfung.
Der vollständige Clang-Build und die Sprachsuite (107/107), fünf gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.111.0 wertet begrenzte ganzzahlige Arithmetik in bekannten
Dimensionsexponenten statisch aus. Der vollständige Clang-Build und die
Sprachsuite (105/105), drei gezielte MSVC-Tests und 18 Referenzdokumente
bestehen.

Sprachvertrag 0.110.0 prüft statisch bekannte SI-Dimensionen bei Konvertierung
und Addition/Subtraktion von `Quantity`. Literale, Einheitenalgebra und
unveränderliche Aliase tragen die Dimension; dynamische Werte bleiben
laufzeitgeprüft. Vollständig dimensionierte Typannotationen sind noch offen.
Der vollständige Clang-Build und die Sprachsuite (105/105), drei gezielte
MSVC-Tests und 18 generierte Referenzdokumente bestehen.

Sprachvertrag 0.109.0 ergänzt `array.sorted()` und `array.sort()` für
`[Int64]`, `[Float64]` und `[String]`. Die geordnete Kopie und die mutierende
Form halten vorhandene Array-Snapshots und räumen fehlgeschlagene Kopien auf.
Der vollständige Clang-Build und die Sprachsuite (104/104), drei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.108.0 ergänzt `Series.quantile(probability)` mit Typ-7-Interpolation
für gespeicherte und abgeleitete Reihen. Die gemeinsame Series-Bibliothek stellt
dieselbe Funktion auch der C-API bereit.
Der vollständige Clang-Build, die Sprach- und Series-Suite (103/103), zwei
gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.107.0 ergänzt das blockweise Lesen eines begrenzten
Series-Ausschnitts als unabhängiges `[Float64]`. Damit können Physim-Analysen
eigene Arrayberechnungen auf gespeicherten oder abgeleiteten Reihen ausführen.
Der vollständige Clang-Build und die Sprachsuite (101/101), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.106.0 bindet den CSV-Export von 1 bis 32 zugeordneten
Datenreihen als `Series.exportColumns` ein. Der Analyse-Runner prüft einen
dreispaltigen Export samt Einheiten sowie leere und nicht zugeordnete Eingaben.
Der vollständige Clang-Build und die Sprachsuite (100/100), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.105.0 erlaubt typisierte `var`-Deklarationen ohne sofortigen
Initialwert. Die erste einfache Zuweisung initialisiert den Wert; frühe Zugriffe
auf lokale oder globale Variablen führen zu einer Quelldiagnose. Besitzende
Werttypen und Closure-Captures folgen denselben Regeln.
Der vollständige Clang-Build und die Sprachsuite (99/99), drei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.104.0 ergänzt geordnete, explizite Modulsuchpfade für CLI und
CMake. Lokale Module haben Vorrang; kanonische Dateipfade verhindern mehrfaches
Laden desselben Moduls über verschiedene Importwege. Die Suchpfade gelten auch
für transitive Imports und werden beim CMake-Neubau berücksichtigt.
Der vollständige Clang-Build und die Sprachsuite (97/97), zwei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.103.0 ergänzt `any` und `all` für Arrays mit typisierten
Prädikaten, früher Beendigung und festgelegtem Verhalten für leere Arrays.
Die Auswertung bewahrt den Array-Snapshot; Laufzeit- und Diagnosetests prüfen
Kurzschluss, Aufrufreihenfolge, Funktionswerte und Typfehler. Der vollständige
Clang-Build und die Sprachsuite (95/95), zwei gezielte MSVC-Tests und 18
Referenzdokumente bestehen.

Sprachvertrag 0.102.0 ergänzt Überladung konkreter und generischer Methoden
auf Strukturen und Enums, auch für statische und gebundene Methodenwerte.
Die Auswahl berücksichtigt Empfängerart, Argumente, Constraints und erwartete
Funktionstypen; mutierende Methoden prüfen die Veränderlichkeit nach der
Auswahl. Tests umfassen generische und importierte Typen sowie Mehrdeutigkeit.
Der vollständige Clang-Build und die Sprachsuite (93/93), vier gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.101.0 erweitert die Überladungsauflösung freier Funktionen
auf generische Vorlagen. Argumente, explizite Typargumente, Constraints,
erwartete Rückgabetypen und spezialisierte Funktionsreferenzen entscheiden
zwischen konkreten und generischen Kandidaten, auch über Modulgrenzen.
Methodenüberladung und weitere Sprachziele bleiben offen. Der vollständige
Clang-Build und die Sprachsuite (91/91), sechs gezielte MSVC-Tests und 18
Referenzdokumente bestehen.

Sprachvertrag 0.100.0 ergänzt Überladung nichtgenerischer freier Funktionen
nach Argumentform und Parametertyp, einschließlich Modulimporten und
Funktionswerten mit erwartetem Typ. Gleichrangige Aufrufe, fehlende Treffer
und doppelte Signaturen werden diagnostiziert. Generische freie Funktionen
und Methoden bleiben für die allgemeine Überladungsauflösung offen. Der
vollständige Clang-Build und die Sprachsuite (89/89), vier gezielte MSVC-Tests
sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.99.0 ergänzt `Array.reduce(initial, combine)` mit
`func(U, T) -> U`. Der Compiler leitet den Akkumulatortyp aus der
Funktionssignatur ab, prüft den Startwert und verwaltet besitzende
Zwischenwerte bei jedem Schritt. Tests decken leere Arrays, Auswertungsreihenfolge,
Closures, Methoden, gemischte Element- und Akkumulatortypen und ungültige
Signaturen ab. Der vollständige Clang-Build und die Sprachsuite (87/87),
vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.98.0 ergänzt `Array.map(transform)` mit aus `func(T) -> U`
abgeleitetem Ergebnistyp. Das C-Backend übernimmt besitzende Rückgabewerte
in ein eigenständiges `[U]` und gibt temporäre Werte nach jeder Kopie frei.
Tests prüfen Zahlen, Strings, Strukturen, verschachtelte Arrays,
Funktionswerte, Enumfall-Konstruktoren, Closures, gebundene Methoden und
ungültige Signaturen. Der vollständige Clang-Build und die Sprachsuite
(85/85), vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.97.0 ergänzt `Array.filter(predicate)` für Funktionswerte vom
Typ `func(T) -> Bool`. Der C-Emitter verarbeitet freie Funktionen, Closures,
gebundene Methoden und lokale Funktionen; das Ergebnis besitzt seine Elemente.
Tests prüfen Auswahlreihenfolge, Eingabesnapshot, Kopien besitzender Werte und
ungültige Prädikatsignaturen. Der vollständige Clang-Build und die Sprachsuite
(83/83), vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.96.0 erlaubt feste `Float64`-Bereiche auch in
`Optional.some(...)` und Enum-Nutzdatenmustern. Die Typprüfung erkennt
vollständig verdeckte Teilmuster; der C-Emitter vergleicht die beiden Grenzen
direkt am Nutzdatenwert. Positive und negative Tests decken optionale Werte
und Enums ab. Der vollständige Clang-Build und die Sprachsuite (81/81), fünf
gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.95.0 ergänzt feste `Float64`-Bereiche im direkten `switch`.
Der Checker prüft endliche, nichtleere Grenzen und Überschneidungen mit
Literalen und anderen Bereichen. Der C-Emitter vergleicht die Grenzen mit
offener oder geschlossener Obergrenze. Der vollständige Clang-Build und die
Sprachsuite (81/81), vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.94.0 ergänzt `Series.minimum()` und `maximum()` für
nichtleere Reihen. Die blockweise Berechnung bleibt bei sehr großen endlichen
Wertespannweiten nutzbar, auch wenn die Varianz nicht darstellbar ist.
Der vollständige Clang-Build und die Sprachsuite (81/81), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.93.0 ergänzt eine optionale `String`-Diagnose für `assert`.
Der Compiler prüft Argumentzahl und Typ, erhält die normale Auswertungsreihenfolge
und gibt bei Fehlschlag die Quellposition mit dem angegebenen Text aus.
Der vollständige Clang-Build und die Sprachsuite (81/81), drei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Der Sprach-Lernpfad enthält nun eine Mehrlaufanalyse mit C- und Physim-Eingaben.
Der eingebettete Code wird gegen das ausgeführte Beispiel synchron geprüft;
das App-Handbuch führt direkt zu dieser Anleitung. Gemischte Lauf- und
Berichtsprüfung sowie Quelldokument-Abgleich bestehen unter Clang und MSVC;
der Clang-Dokumentationsfenstertest lädt die neuen Inhalte.

Sprachvertrag 0.92.0 erlaubt das Auslesen von Einheitensymbol, Maßstab und
SI-Exponenten jeder `Series`, auch nach Transformationen. `isAlignedWith`
prüft die Zuordnung zweier gültiger Reihen vor gemeinsamen Operationen.
Der vollständige Clang-Build und die Sprachsuite (81/81), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.91.0 macht gespeicherte Messkanäle ohne vorab bekannten Namen
zugänglich: `Dataset.channelName`, `channelUnitSymbol`, `channelDescription`
und `channelExponent` prüfen Indizes und liefern eigenständige Textkopien.
Der vollständige Clang-Build und die Sprachsuite (81/81), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.90.0 bindet `Series.slice(first, count)` und `Series.name()` an
das C-Analyse-SDK. Ausschnitte kopieren ihre Werte und erhalten für passende
Zeit- und Messreihen die Zuordnung; Namen werden als eigenständige Strings
kopiert. Der vollständige Clang-Build und die Sprachsuite (81/81) bestehen;
der gezielte MSVC-Analysetest und alle 18 Referenzdokumente sind geprüft.

Sprachvertrag 0.89.0 ergänzt Host- und Dataset-Provenienz: `runSeed()` liefert
das vollständige Laufseed-Bitmuster in Experimenten; Analysen lesen
Datensatz- und Kanalzahl, Recovery-Status und eigene Metadatenkopien aus
`Dataset`. Gemischte C-/Sprachläufe und Seedwerte bis `UINT64_MAX` werden
in den nativen Runner-Tests geprüft. Der vollständige Clang-Build und die
Sprachsuite (81/81), drei gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.88.0 ergänzt den bedingten Optional-Ersatzoperator `??`.
Er bindet schwächer als logisches Oder, gruppiert von rechts und wertet den
Ersatz nur bei `nil` aus. Typprüfung, C-Emitter und Besitzverwaltung sind für
skalare, Array-, String- und Structwerte sowie verschachtelte Optionals geprüft.
Der vollständige Clang-Build und die Sprachsuite (81/81) bestanden; fünf
gezielte MSVC-Tests und 18 Referenzdokumente wurden ebenfalls geprüft.

Sprachvertrag 0.87.0 löst explizit spezialisierte generische
Initialisiererwerte bei mehreren `init`-Methoden anhand des erwarteten
Funktionstyps auf. Verschachtelte Formen wie `T?` und `[T]` sowie
generische Structs sind geprüft; fehlende oder mehrdeutige Signaturen
erhalten gezielte Compilerdiagnosen.

Sprachvertrag 0.86.0 ergänzt Struct-Initialisierer als Funktionswerte.
Ein erwarteter Funktionstyp wählt überladene Initialisierer anhand ihrer
Signatur aus; generische Structs, explizit spezialisierte Initialisierer und
importierte Structs sind abgedeckt. Mehrdeutige Signaturen und fehlende
Spezialisierung erhalten eigene Compilerdiagnosen.

Sprachvertrag 0.85.0 ergänzt rekursive `Optional.some`-Nutzlastmuster in
optionalen Werten und Enumfeldern. Der Checker prüft innere Typen,
Fallabdeckung und verdeckte Muster; der C-Emitter kopiert und räumt auch
verschachtelte besitzende Bindungen korrekt auf. Generische und importierte
Enums sind in Laufzeittests enthalten.
Der vollständige Clang-Build, die Sprachsuite (81/81), sechs gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.84.0 erlaubt direkte `Float64`-Werte im `switch`. Feste
endliche Zahlenliterale werden binär exakt verglichen; der Checker erkennt
wertgleiche Fälle, verlangt `default` und lehnt variable oder Bereichsmuster ab.
Der vollständige Clang-Build, die Sprachsuite (81/81), drei gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.83.0 erweitert feste `Int64`-Bereiche auf optionale und
Enum-Nutzlastmuster. Die Typprüfung verlangt gültige Literalgrenzen;
Erreichbarkeit und vollständige Abdeckung berücksichtigen Teilbereiche.
Laufzeittests umfassen generische und importierte Enums.
Der vollständige Clang-Build, die Sprachsuite (81/81), sechs gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.82.0 ergänzt `Int64`-Bereichsmuster im `switch` mit geschlossener
oder offener Obergrenze. Der Checker verlangt feste gültige Grenzen und erkennt
Überschneidungen mit unbewachten Fällen; Guard-Fallbacks bleiben möglich.
Der vollständige Clang-Build, die Sprachsuite (81/81), drei gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.81.1 korrigiert die Erreichbarkeitsprüfung von `Float64`-
Nutzlastmustern: verschiedene Schreibweisen desselben binären Werts, auch
gerundete Werte und ±0, gelten als Duplikate. Die Prüfung verwendet wie die
Laufzeit einen festen Dezimalpunkt und weist nicht endliche Literale zurück.
Der vollständige Clang-Build, die Sprachsuite (81/81), sechs gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.81.0 erlaubt `nil` als Teilmuster für optionale Nutzlasten in
Enums und verschachtelten Optionalwerten. Dabei bleiben äußerer und innerer
Leerwert unterscheidbar. Typprüfung, Fallabdeckung, Duplikate und Laufzeitverhalten
sind auch für generische und importierte Enumfälle geprüft.
Der vollständige Clang-Build und die Sprachsuite (81/81), sieben gezielte
MSVC-Tests sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.80.0 ergänzt Literalvergleiche in Enum- und Optional-Nutzlastmustern
für `Bool`, `Int64`, `Float64` und `String`. Teilmuster zählen nur ihren tatsächlich
verglichenen Wert; umfassende Muster und `default` sichern die Fallabdeckung.
Typfehler, fehlende Abdeckung und unerreichbare Zweige sind geprüft.
Der vollständige Clang-Build, die Sprachsuite (81/81), fünf gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.79.0 macht Enumfälle mit Nutzlast zu speicherbaren
Konstruktor-Funktionswerten. Indirekte Aufrufe erzeugen eigene Enumwerte;
String-Nutzlasten, mehrere Felder, Arrays und Optionalwerte als Ablage,
generische sowie importierte Enums und Typfehler sind geprüft.
Der vollständige Clang-Build und die Sprachsuite (81/81), die gezielten
MSVC-Tests (5/5) sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.78.0 ergänzt generische Enums mit typisierten und besitzenden
Nutzdaten. Konstruktoren können Typargumente aus Nutzlast oder erwartetem Typ
ableiten; Methoden, Fallmuster, Raw Values, Imports, Funktionswerte, rekursive
Optionalfelder und Constraints verwenden die spezialisierte Enumidentität.
Der vollständige Clang-Build und die Sprachsuite (81/81), die gezielten
MSVC-Tests (3/3) sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.77.0 ergänzt qualifizierte Modulimporte aus Unterordnern und
optionale Aliase. Der Loader teilt identische transitive Abhängigkeiten,
meldet fehlende Dateien und Zyklen am Importort; CMake beobachtet nun auch
Unterordner der Einstiegsmodule.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (4/4) sowie 18 Referenzdokumente bestehen. Eine berührte
Unterordnerdatei löste den erneuten CMake-Build des Einstiegsmoduls aus.

Sprachvertrag 0.76.0 erlaubt mutierende Instanzmethoden als gebundene
Funktionswerte. Die Bindung kopiert den Empfänger; Kopien des Funktionswerts
teilen seinen veränderlichen Zustand. Structs mit besitzenden Strings/Arrays,
Enums mit besitzender Nutzlast, generisch spezialisierte Methoden, Array- und
Optionalspeicherung sowie
skalare Analyse-Callbacks sind geprüft. Das ursprüngliche Objekt bleibt
unverändert; gleichzeitige Zugriffe auf denselben Wert sind nicht unterstützt.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (2/2) sowie 18 Referenzdokumente bestehen. Das Analysemodul wurde
auch mit MSVC gebaut.

Sprachvertrag 0.75.0 spezialisiert generische benannte lokale Funktionen im
lexikalischen Deklarationsbereich. Jede verwendete Spezialisierung erzeugt
einen besitzenden Funktionswert; Inferenz, explizite Typargumente, Constraints,
Rekursion, verschachtelte Closures und numerische Analyse-Callbacks sind geprüft.
Später deklarierte lokale Werte bleiben unsichtbar. Mutierende Methoden als
Funktionswerte wurden in 0.76.0 ergänzt.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (6/6) sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.74.0 ergänzt benannte lokale Funktionen als besitzende Closures.
Sie können lokale Werte aus mehreren Schachtelungsebenen erfassen, über ihren
Gültigkeitsbereich hinaus zurückgegeben werden und sich selbst rekursiv aufrufen.
Direkte Aufrufe erlauben benannte Argumente; gespeicherte Werte bleiben
positionsgebunden. Native Programme und Analyse-Callbacks prüfen diese Fälle.
Generische lokale Funktionen wurden in 0.75.0 und mutierende Methoden als
Werte in 0.76.0 ergänzt.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (6/6) und 18 Referenzdokumente bestehen.

Sprachvertrag 0.73.0 ergänzt anonyme Funktionen mit typisierten Parametern und
eingerückten Körpern. Closures besitzen Kopien verwendeter lokaler Werte,
einschließlich verschachtelter und generisch spezialisierter Fälle; direkte
und gespeicherte Aufrufe sowie skalare und ODE-Callbacks sind geprüft.
Erfasste Werte sind innerhalb der Closure unveränderlich. Modulvariablen
bleiben globale Zugriffe. Benannte lokale Funktionen wurden in 0.74.0 ergänzt;
mutierende Methoden als Werte in 0.76.0.
Der vollständige Clang-Build und die Sprachsuite (79/79), gezielte MSVC-Tests
(7/7) sowie 18 erzeugte Referenzdokumente bestehen.

Sprachvertrag 0.72.0 führt gebundene, nicht mutierende Instanzmethoden als
besitzende Funktionswerte ein. Die Empfängerkopie überlebt lokale Gültigkeits-
bereiche, kann in Arrays und Optionalwerten liegen und wird über die
Analyse-Callback-Brücke aufgerufen. Mutierende Methoden bleiben als Werte offen.
Die vollständige Clang-Sprachsuite besteht mit 78/78 Tests; die gezielten
MSVC-Tests und der neue Enum-Empfänger mit besitzender Nutzlast bestehen.
Der zusätzliche MSVC-AddressSanitizer-Lauf liefert hier keine Diagnose:
sowohl der Funktionswerttest als auch der unveränderte `memory`-Test
überschreiten ihr Zeitlimit ohne Ausgabe.

Sprachvertrag 0.71.0 übergibt typisierte Funktionswerte an skalare Suche,
ODE-Integratoren und Verlet-Integration. Die native Callback-Brücke wählt das
Ziel zur Laufzeit, übernimmt den Modulzustand und prüft die Signatur vor dem Build.

Sprachvertrag 0.70.0 erlaubt explizit spezialisierte generische Funktionen und
statische Methoden als Funktionswerte, auch aus importierten Modulen und
generischen Strukturen. Parser, Typprüfung und nativer Aufruf wurden für den
Analyse-Runner geprüft.

Sprachvertrag 0.69.0 erweitert Funktionswerte um statische Methoden von
Strukturen und Enums, einschließlich spezialisierter generischer Strukturen
und importierter Typen. Die indirekten Aufrufe verwenden denselben Modulzustand.

Sprachvertrag 0.68.0 führt strukturell typisierte Werte freier Funktionen ein.
Programme, importierte Module und die Analyse-Runner-ABI prüfen Übergabe,
Rückgabe, indirekte Aufrufe, generische Argumente, Struct-/Array-/Optional-
Speicherung sowie besitzende String-Ergebnisse. Gebundene Methoden und
Closures waren zu diesem Stand offen.

Sprachvertrag 0.67.0 erlaubt absteigende `for`-Bereiche mit negativem,
von null verschiedenem `by`-Schritt. Die Codeerzeugung prüft beide
`Int64`-Überlaufgrenzen; Slice-Indizes bleiben auf positive Schritte
beschränkt. Native Referenzen prüfen Richtung, Kontrollfluss und Grenzwerte.

Sprachvertrag 0.66.0 verbindet komponentenweise RK45-Toleranzen mit dem
besitzenden `OdeResult`-Bericht. Native und Analyse-Referenzen prüfen
unterschiedlich skalierte Zustände, Berichtszähler und Fehlerpfade.

Sprachvertrag 0.65.0 stellt die Berichte für Bisektion und Goldener-Schnitt-
Minimierung als `ScalarResult` bereit. Der Wert enthält Koordinate,
Funktionswert, Intervall und Iterations-/Auswertungszähler. Native und
Analyse-Referenzen prüfen beide Verfahren sowie Fehlerpfade.

Sprachvertrag 0.64.0 stellt den RK45-Bericht als kopierbaren `OdeResult` mit
eigenem Zustandsarray bereit. Native Referenzen prüfen Vorwärts- und
Rückwärtsintegration, Berichtsfelder, Kopien, Array- und Optionalwerte sowie
Fehlerpfade; das Analysemodul verwendet dieselbe Runner-ABI.

Sprachvertrag 0.63.0 bindet die komponentenweisen absoluten RK45-Toleranzen
der C-API als `rk45IntegrateWithTolerances`. Das Array muss für jede
Zustandskomponente einen positiven Wert enthalten und wird vor den Callbacks
kopiert. Native Referenzen prüfen zwei unterschiedlich skalierte Größen,
unveränderte Eingaben und ungültige Toleranzen; das Analysemodul führt die
Funktion über die Runner-ABI aus.

Sprachvertrag 0.62.0 bindet Velocity Verlet mit einem benannten
Beschleunigungs-Callback an die C-Numerik. Ein Phasenarray enthält 1–32
Positionen und danach gleich viele Geschwindigkeiten; das Ergebnis besitzt
seinen Speicher selbst. Native Referenzen prüfen Umkehrbarkeit, 32
Freiheitsgrade sowie Fehlerpfade. Analyse-Runner und Pendel-Experiment verwenden
die Callback-Brücke; das Pendel wird über 4.000 Schritte mit der C-Verlet-
Referenz verglichen, einschließlich Szene und gespeicherter Runnerdatei.

Sprachvertrag 0.61.0 ergänzt `rk45IntegrateWithSteps` mit Anfangs-, Mindest-
und Höchstschritt. Die RK45-Pendelvorlage übernimmt damit die vollständigen
Schrittweitenoptionen der C-Referenz. Native Programme und das Analysemodul
prüfen die konfigurierte Integration; ungültige Optionen erzeugen
Quelldiagnosen. Numerische Berichte und komponentenweise Toleranzen fehlen noch
(Stand 0.61).

Sprachvertrag 0.60.0 bindet den adaptiven Dormand–Prince-Integrator als
`rk45Integrate` mit benannter Physim-Ableitungsfunktion, Vorwärts- und
Rückwärtsintegration, Toleranzen und Schrittbudget ein. Die Bibliotheksdiagnose
wird als Laufzeitfehler mit Quellposition gemeldet. Native Referenzen prüfen
Oszillator und zeitabhängige Gleichung sowie Dimensions-, Options-, Budget-
und Callbackfehler; das Analysemodul führt RK45 über die Runner-ABI aus.
`pendulum_rk45.phys` verwendet die Funktion als vollständiges Experiment.
Sechs Messkanäle und die Szene entsprechen über 4.000 Schritte der C-RK45-
Vorlage; ein Runnerlauf prüft die gespeicherten Messdaten.
Numerische Berichte und weitere RK45-Optionen bleiben offen (Stand 0.60).

Sprachvertrag 0.59.0 bindet den allgemeinen Euler- und RK4-Schritt mit
Ableitungs-Callbacks aus Physim-Funktionen an die gemeinsame C-Integrationsroutine.
Zustand und Ableitung sind besitzende `[Float64]`-Werte mit 1–32 Komponenten;
Zwischenwerte, Dimensionen und Eingaben werden geprüft. Die native Referenz
deckt den harmonischen Oszillator, eine zeitabhängige Gleichung, die obere
Dimensionsgrenze sowie Callback-Abbruch mit Speicherbilanz ab. Die Analysemodul-
Referenz führt die Callback-Brücke auch über die Runner-ABI aus. Beide Gesamtbuilds
und je 77/77 Sprach-CTest-Einträge bestehen unter Clang Debug und MSVC Release.
Der Referenzgenerator beschreibt auch die nach 0.53 ergänzten Sprachbindungen;
alle neun Dokumentations-CTest-Einträge bestehen unter Clang Debug.
Das vollständige Pendel-Experiment `pendulum_integrator.phys` nutzt `rk4Step`
im Modul. Der ABI-Test vergleicht alle sechs Kanäle und die Szene über 4.000
Schritte mit dem C-Pendel; Clang Debug und MSVC Release bestehen.

Sprachvertrag 0.58.0 bindet Bisektion und Goldener-Schnitt-Minimierung der
gemeinsamen Numerikbibliothek über benannte skalare Sprachfunktionen ein.
Der Compiler prüft `func(Float64) -> Float64`, erzeugt eine native Callback-Brücke
für eigenständige Programme und Hostmodule und meldet ungültige Intervalle,
Iterationsgrenzen sowie fehlende Konvergenz mit Quellposition.
Zu diesem Stand waren ODE-Funktionswerte und vollständige numerische Berichte
noch offen. Gesamtbuilds und vollständige Sprachtests bestehen unter
Clang Debug und MSVC Release (jeweils 77/77 CTest-Einträge einschließlich
zweier benötigter Testvorbereitungen).

Sprachvertrag 0.57.0 bindet den linearen Löser der gemeinsamen C-Bibliothek als
`linearSolve(coefficients, rhs, pivotTolerance)` an. Koeffizienten liegen
zeilenweise in einem `[Float64]`; der neue Ergebniswert besitzt seinen Speicher
selbst. Der native Referenztest prüft das C-3×3-System, ein 32×32-System,
unveränderte Eingaben und getrennte Diagnosen für Form, Singularität, Toleranz
und numerischen Bereich. Ein 33er-Grenzfall wird vor dem festen Löserpuffer
abgewiesen und räumt sein Eingabearray auf. Der native Test besteht unter Clang
Debug und MSVC Release.
Beide Gesamtbuilds und alle Sprachtests bestehen ebenfalls in beiden
Konfigurationen (jeweils 74/74).

Sprachvertrag 0.56.0 bindet die kubische räumliche Bézierkurve der gemeinsamen
C-Bibliothek als kopierbaren `Bezier3`-Wert ein. `position` und `tangent` werten
die Kurve aus; `splitLeft` und `splitRight` liefern umparametrisierte Teilkurven.
Die native Referenz prüft das kubische Polynom an 101 Stellen, Teilkurven und
Ableitungen sowie ungültige Parameter und numerischen Überlauf. Beide
Gesamtbuilds und alle Sprachtests bestehen unter Clang Debug und MSVC Release
(jeweils 74/74). Allgemeine Integrator-Callbacks bleiben als größere
Sprachbindung offen.
Der App-Editor zeigt `Bezier3` im Physim-Quelltext als Typ; die gezielte
Editoraufnahme mit der nativen Referenzdatei wurde visuell geprüft.

Sprachvertrag 0.55.0 erlaubt `_` als Verwerfmuster in Nutzdatenzweigen von
Enums und in `Optional.some(_)`. Andere Felder bleiben für Guard und Zweigkörper
gebunden; verworfene besitzende Werte werden nicht kopiert. Positive native
Referenzen mit Speicherbilanz und negative Gültigkeitsbereichsprüfungen bestehen.
Beide Gesamtbuilds und alle Sprachtests bestehen unter Clang Debug und MSVC
Release (jeweils 74/74). Weitere Fallmuster bleiben offen.

Sprachvertrag 0.54.0 ergänzt `min`, `max` und `clamp` für `Float64`.
Gleiche Werte behalten bei `min`/`max` den linken Operanden; `clamp` prüft die
Reihenfolge der Grenzen und meldet einen Laufzeitfehler mit Quellposition.
Die native Referenz prüft positive und benannte Aufrufe, einmalige
Argumentauswertung und vertauschte Grenzen. Nach der Versionsanhebung bestehen
der native Sprachtest, alle 63 Compiler-/Laufzeittests und zehn Sprach-App-Tests
jeweils unter Clang Debug und MSVC Release (74/74).

Die Physim-Pendelvorlage besitzt jetzt auch eine native RK4-Variante
(`examples/language/pendulum_rk4.phys`). Ihre vier Integrationsstufen stehen im
Sprachquelltext. Der ABI-Referenztest vergleicht alle sechs Messkanäle über
4.000 Schritte bei dt=0,005 s mit dem Standard-C-Pendel und prüft Reset,
Szenenposition und Integratormetadaten. MSVC Release und Clang Debug bestehen.

Sprachvertrag 0.47.0 ergänzt `0x`-/`0X`-Ganzzahlliterale für Farben, Bitmuster
und Seeds. Der Lexer weist fehlende Ziffern und ungültige Suffixe ab; die
Typprüfung setzt die vorzeichenbehafteten `Int64`-Grenzen auch in `Float64`-
Kontexten durch. Enum-Rohwerte, `switch`-Muster und generiertes C verwenden
denselben Zahlenwert; das RK4-Pendel prüft damit auch die Farben seiner
Szenenobjekte gegen die C-Referenz. Die vollständigen Sprachtests bestehen
unter MSVC Release (63/63) und Clang Debug (72/72).

Sprachvertrag 0.48.0 ergänzt bitweises `Int64`-Und/Oder/Xor/Not sowie Links-
und Rechtsschieben. Die Laufzeit verwendet definierte 64-Bit-Muster und
weist Schiebezahlen außerhalb 0–63 mit Quellposition ab. Verschachtelte
generische Typen bleiben trotz des `>>`-Tokens parsbar. Die native Referenz
prüft Grenzwerte, Operatorbindung und Auswertungsreihenfolge. Beide
Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (63/63) und
Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.49.0 ergänzt `&=`, `|=`, `^=`, `<<=` und `>>=` für veränderliche
`Int64`-Ziele, auch für Felder und Arrayelemente. Der Parser trennt in
Typkontexten `>=` und `>>=` wieder in schließende Generics und `=`, sodass
verschachtelte Typen ohne zusätzliche Leerzeichen initialisiert werden können.
Lexer-, Parser-, Typ- und native Referenzen decken die neuen Schreibweisen ab.
Beide Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (63/63)
und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.50.0 ergänzt Binär- (`0b`/`0B`) und Oktalliterale
(`0o`/`0O`). Lexer und Typprüfung weisen ungültige Ziffern sowie Werte
außerhalb des `Int64`-Bereichs ab. Die C-Ausgabe verwendet den geprüften
Zahlenwert auch für `Float64`-Kontexte.
Beide Gesamtbuilds und vollständigen Sprachtests bestehen unter MSVC Release
(63/63) und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.51.0 erlaubt `_` als Zifferntrenner innerhalb aller
Ganzzahlbasen sowie in Dezimalbruchteil und Exponent. Der Lexer weist
fehlplatzierte Trennzeichen ab; Typprüfung und native C-Ausgabe verarbeiten
die bereinigten Werte, einschließlich beider `Int64`-Grenzen.
Beide Gesamtbuilds und vollständigen Sprachtests bestehen unter MSVC Release
(63/63) und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.52.0 ergänzt Instanz-, mutierende, statische und generische
Methoden für Enums. Die Typprüfung erkennt Kollisionen mit Fällen und
Raw-Value-Mitgliedern. Native Referenzen prüfen Nutzdaten mit Besitzsemantik,
Methoden auf importierten Enumtypen und die Ersetzung von `self`.
Beide Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (63/63)
und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows. Die
nachträglich erweiterte Kopierprobe bestand in beiden nativen Tests erneut.

Sprachvertrag 0.53.0 erlaubt Unicode-15.0-Bezeichner in UTF-8-Quellen.
Generierte Tabellen definieren Start- und Fortsetzungszeichen unabhängig von
der Systemlocale. Compiler und native Tests prüfen Unicode-Namen für Typen,
Felder, Funktionen, Parameter und importierte Module; verschiedene
Normalisierungsformen bleiben unterscheidbar.
Beide Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (64/64)
und Clang Debug (73/73), einschließlich nativer Unicode-Modulimporte und
der neun Clang-App-Workflows.

Beide App-Editoren verwenden für Physim-Bezeichner jetzt dieselbe
Unicode-15.0-Erkennung wie der Compiler. Die Farbdarstellung trennt C- und
Physim-Schlüsselwörter, berücksichtigt Zifferntrenner und verschachtelte
Physim-Blockkommentare. Der Editor lädt zusätzlich Glyphen für kombinierende
Zeichen, Griechisch, Kyrillisch und Latin Extended Additional. Die Aufnahme
von `unicode_identifiers.phys` zeigt `π` und lateinische Unicode-Bezeichner
in Clang Debug und MSVC Release lesbar; zuvor wurde `π` als `?` angezeigt.
Der reproduzierbare `language_editor_unicode_preview`-Test, Workspace-Test
und vollständige Sprach-App-Workflow bestehen in beiden App-Builds.

Die Simulationsansicht wählt ihre hervorgehobene Aktion jetzt nach dem aktuellen
Zustand: Bei fehlendem Build `Build`, im Leerlauf `Neuer Lauf`, während eines
steuerbaren Laufs `Pause` und nach Pause `Fortsetzen`. Farben, Abstände und
Radien der dunklen Oberfläche liegen in `app/design_tokens.h`; die
[Zustandsmatrix](ui-states.md) beschreibt die Entscheidung.

Der neue Workspace-Einstieg ist begonnen: normaler Start ohne vorausgewähltes
Projekt, globale Helferleiste, echte SDL-Datei-/Ordnerauswahl, Öffnen beliebiger
Ordner, schreibgeschützte Textvorschau, zusätzliche Datei-/Ordnerpfade und ein
getrennter Projektmanager mit Zielordner, Name, Vorlage und beiden Sprachen.
Der lokale `workspace_workflow` prüft leeren Start, Ordner ohne Projekt,
Dateivorschau, Projekterzeugung und Rückkehr zum allgemeinen Workspace.
Dateibaum, mehrere editierbare Dokumente, Workspace-Persistenz und Prüfungen
für große oder ungültige Textdateien sind inzwischen ergänzt; siehe den aktuellen
Stand oben. Offen bleibt die manuelle Abnahme des nativen Auswahldialogs auf
Windows und Linux. Die [Designrichtung](design-direction.md) ist daher noch
nicht vollständig abgenommen.

Der erste durchgängige Sprachlernweg steht in
[Eigenes Experiment in der Physim-Sprache](language-tutorial.md). Die beiden
kopierbaren Quellen für gleichförmige Bewegung und Auswertung werden als native
Sprachmodule gebaut und mit demselben Referenztest wie das C-Tutorial geprüft:
201 Positionswerte, Reset, Szene und abgeleitete Geschwindigkeit. Die F1-Hilfe
enthält den Lernweg; ein Quellabgleich hält die Codeblöcke mit den getesteten
Dateien synchron. Der [Vakuumwurf-Lernpfad](projectile-tutorial.md) ergänzt
vollständige C- und Physim-Quellen für Experiment und Auswertung. Ein gemeinsamer
Referenztest prüft 201 Messungen, fünf Kanäle, Energie, Reset, Szene und
Geschwindigkeitsbericht; ein Quellabgleich hält alle vier Codeblöcke mit den
gebauten Dateien synchron. Der [Luftwiderstands-Lernpfad](projectile-drag-tutorial.md)
ergänzt die gleiche Vier-Quellen-Kette mit quadratischer Kraft und RK4. Sein
Paritätstest vergleicht C- und Sprachläufe mit 201 gespeicherten Zeilen,
Energieabnahme, Spur, Reset und beiden Geschwindigkeitsberichten. Die übrigen
Lernpfade und die vollständige LANG-007-Dokumentation bleiben offen.
Die App-Vorlage „Wurf mit Luftwiderstand“ verwendet bei Sprachwahl jetzt
`projectile_drag.phys` statt des Vakuummodells; der vollständige Sprachworkflow
ist als App-Test registriert.
Die Vorlage „Wurf mit Unsicherheit“ verwendet bei Sprachwahl jetzt
`uncertain_projectile.phys` mit denselben Anfangsverteilungen, Sensorparametern,
Seed-Strömen und 15 Kanälen wie die C-Variante. Der Runner-Paritätstest prüft
zwei 64-Bit-Seeds mit je 201 Zeilen und lässt C- sowie Sprachauswertung beide
Laufdateien verarbeiten. Die App-Workflows bestanden unter Clang und MSVC.

Die Sprache unterstützt nun Gleichheit und Ungleichheit zwischen zwei
optionalen Werten desselben Typs, sofern deren Inhalt `Bool`, `Int64`,
`Float64`, `String`, ein einfaches Enum oder ein Vektor ist. Fehlende Werte
vergleichen wie erwartet gleich; vorhandene Werte vergleichen ihren Inhalt.
Optionale Arrays, Strukturen und weitere SDK-Werte bleiben ausgeschlossen.
Die Sprachversion 0.24.0 verwendet jetzt besitzende, unveränderliche UTF-8-Strings.
`+` verkettet Strings; Variablen, Funktionen, Strukturen, Arrays und Optionals
halten geteilte Werte korrekt am Leben. Dynamisch erzeugte `Unit`- und
`Quantity`-Symbole bleiben bis zum Modulende gültig und werden nach Inhalt
zusammengefasst. Eine native Referenz und der interne Speichertest bestanden
unter MSVC und Clang; die Analyse-Referenz prüft zusätzlich dynamische Titel und
Einheitensymbole über die Modulgrenze. Weitere Stringfunktionen und der übrige LANG-001-Ausbau
bleiben offen. Sprachvertrag 0.25.0 ergänzt Unicode-Skalarindizes, bereichsweise
und geschrittete Stringausschnitte sowie `count`, `utf8ByteCount` und `isEmpty`.
Schreibzugriffe auf Stringindizes weist der Compiler ab; die positive Referenz
und negative Compilerfälle bestanden unter MSVC und Clang.
Sprachvertrag 0.26.0 ergänzt `String.contains` und `String.firstIndex(of:)` mit
Unicode-Skalarindizes. Die native Referenz und Fehlertypprüfung bestehen unter
MSVC und Clang; weitere Stringfunktionen und LANG-001-Arbeit bleiben offen.
Sprachvertrag 0.27.0 ergänzt `String.lastIndex(of:)`, `hasPrefix` und
`hasSuffix`. Leere Suchtexte und Unicode-Skalarindizes sind festgelegt;
die Sprachtests bestanden unter MSVC und Clang. LANG-001 bleibt weiter offen.
Sprachvertrag 0.28.0 ergänzt die unveränderliche Ersetzung von Stringteilen
einschließlich leerer Suchtexte an Unicode-Skalargrenzen. Native Referenz,
Speicherfehlerfälle und die Sprachtests bestanden unter MSVC und Clang. Der
übrige Sprachausbau bleibt offen.
Sprachvertrag 0.29.0 ergänzt `String.split(separator:)` mit besitzenden
`[String]`-Ergebnissen, erhaltenen Leerfeldern und Unicode-Skalarteilung.
Laufzeit-, Speicherfehler- und Compilerprüfungen bestanden unter MSVC und
Clang. Der übrige LANG-001-Ausbau bleibt offen.
Sprachvertrag 0.30.0 ergänzt `[String].joined(separator:)` für die umgekehrte
Textverarbeitung von `split`. Ergebnis und Eingaben besitzen unabhängige
Lebensdauer. Die Sprachtests bestanden unter MSVC und Clang; LANG-001 bleibt
darüber hinaus offen.
Sprachvertrag 0.31.0 ergänzt Stringkonvertierungen für `Bool`, `Int64` und
`Float64` sowie streng geprüfte Dezimaltext-Eingaben für beide Zahlentypen.
Die Prozesslocale war für Float64 noch maßgeblich. Native Fehlerfälle und die
Sprachtests bestanden unter MSVC und Clang; LANG-001 bleibt offen.
Sprachvertrag 0.32.0 verwendet für Float64-Literale und Dezimaltextumwandlung
einen festen C-Dezimalpunkt, auch wenn der Host eine andere Zahlenlocale setzt.
Stringkonvertierung und `print(Float64)` schreiben denselben Dezimalpunkt.
Der Localetest lief mit tatsächlich umgestellter Zahlenlocale unter MSVC und
Clang; beide vollständigen Builds, jeweils 60 Sprachtests und die native
Sprachsuite bestanden. Ein Linux-Build steht noch aus.
Sprachvertrag 0.33.0 ergänzt `array.lastIndex(of:)` für alle gleichheitsfähigen
Elementtypen. Die Suche läuft rückwärts, liefert einen optionalen Index und
wertet Empfänger sowie Suchwert je einmal aus. Positive und negative
Compilerfälle sowie native Referenzen mit verschachtelten und besitzenden
Werten bestanden unter MSVC und Clang. Nach vollständigem Neubau bestanden
61/61 Sprachtests unter MSVC und 70/70 unter Clang einschließlich neun
App-Workflows.
Sprachvertrag 0.34.0 ergänzt `array.reversed()` für beliebige Elementtypen.
Der neue Wert ist unabhängig vom Original. Kopierfehler bei jedem Teilschritt
sind im Speichertest abgedeckt; die nativen Beispiele mit Strings und
verschachtelten Arrays sowie positive und negative Compilerfälle bestanden
unter MSVC und Clang. Nach vollständigem Neubau bestanden 62/62 Sprachtests
unter MSVC und 71/71 unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.35.0 ergänzt das mutierende `array.reverse()` einschließlich
verschachtelter `var`-Zugriffspfade. Ein Fehler beim Kopieren eines Elements
ändert weder Empfänger noch bereits vorhandene Kopien. Gezielte Compiler-,
Laufzeit- und Speicherfehlerprüfungen bestanden unter MSVC und Clang. Nach
vollständigem Neubau bestanden 62/62 Sprachtests unter MSVC und 71/71 unter
Clang einschließlich neun App-Workflows.
Sprachvertrag 0.36.0 ergänzt `for` über Strings. Die Schleife liefert pro
Unicode-Skalar einen eigenen Stringwert, hält eine feste Eingabemomentaufnahme
und räumt Werte auch bei `break` und `continue` auf. Compiler-, Laufzeit- und
Allocator-Fehlerfälle bestanden unter MSVC und Clang. Nach vollständigem
Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72 unter Clang
einschließlich neun App-Workflows.
Sprachvertrag 0.37.0 ergänzt `String.reversed()` für Unicode-Skalare. Der
unveränderliche Eingabestring bleibt erhalten; das Ergebnis besitzt eigenen
Speicher. Gezielte Compiler-, Laufzeit- und Allokationsfehlerprüfungen sowie
native Experiment- und Analysepfade bestanden unter MSVC und Clang. Nach
vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72 unter
Clang einschließlich neun App-Workflows.
Sprachvertrag 0.38.0 erlaubt mehrere `static init`-Methoden pro Struktur.
Parameteranzahl und benannte Parametersätze wählen den Initialisierer; gleiche
Signaturen und mehrdeutige positionale Aufrufe sind Compilerfehler. Lokale,
importierte und generische Structs sowie ausdrückliche `.init`-Aufrufe wurden
unter MSVC und Clang geprüft. Nach vollständigem Neubau bestanden 63/63
Sprachtests unter MSVC und 72/72 unter Clang einschließlich neun App-Workflows.
Typbasierte Überladung war in dieser Fassung noch offen.
Sprachvertrag 0.39.0 ergänzt typbasierte `init`-Überladung für konkrete Structs.
Gleiche Parameternamen mit unterschiedlichen Typen sind möglich; exakte
Argumenttypen haben Vorrang vor der `Float64`-Verwendung eines Ganzzahlliterals.
Nicht typisierte `nil`-Argumente mit mehreren passenden Signaturen und
Argumente ohne passenden Typ erhalten eindeutige Compilermeldungen. Die
Referenz prüft benannte und positionale Aufrufe, geänderte Parameterreihenfolge,
importierte Typen und generische Formüberladung. Nach vollständigem Neubau
bestanden 63/63 Sprachtests unter MSVC und 72/72 unter Clang einschließlich
neun App-Workflows. Typbasierte Überladung generischer Initialisierer war in
dieser Fassung noch offen.
Sprachvertrag 0.40.0 löst `init`-Überladungen auch für generische Structs nach
Parametertypen auf. Der Checker leitet Typparameter pro passender Signatur ab,
bevor er die konkrete Struktur spezialisiert. `[T]`, `T?` und verschachtelte
generische Struct-Typen können so gegen `T` unterschieden werden; explizite
und kontextuelle Typargumente sowie importierte generische Structs sind in
der Referenz geprüft. Doppelte
Templatesignaturen, mehrdeutige Aufrufe und nach Spezialisierung kollidierende
Signaturen werden abgewiesen. Kontextabhängige Auflösung untypisierter
Argumentausdrücke und eigene Methodentypparameter für `init` waren in dieser
Fassung noch offen.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.41.0 nutzt den Parametertyp jetzt auch für `nil` und
Arrayliterale bei der Wahl einer `init`-Überladung. Ein einzelner optionaler
beziehungsweise Array-Kandidat kann damit ausgewählt werden; mehrere passende
Kandidaten bleiben mehrdeutig. Generische Konstruktoren leiten Typen aus
nichtleeren Arrayliteralen und späteren Argumenten ab; ein bereits bekannter
Ergebnistyp gibt den Parametern den benötigten Kontext. Gezielte Compiler- und
Laufzeitreferenzen bestehen unter MSVC und Clang. Nach vollständigem Neubau
bestanden 63/63 Sprachtests unter MSVC und 72/72 unter Clang einschließlich
neun App-Workflows. Die Typableitung weiterer Ausdrucksformen und eigene
Typparameter für `init` bleiben offen.
Sprachvertrag 0.42.0 berücksichtigt `Optional.some(...)` bei der Wahl einer
`init`-Überladung. Sein Inhalt wird gegen den optionalen Parametertyp geprüft;
die Referenz umfasst skalare Literale, berechnete Werte, benannte Argumente,
generische Typableitung aus Optional-Werten und Arrayliteralen sowie
verschachtelte Arrayinhalte mit Ergebniskontext. Mehrdeutige und typfremde
Fälle bleiben Compilerfehler.
Eigene Typparameter für `init` und weitere kontextabhängige Ausdrucksformen
stehen weiterhin aus.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.43.0 leitet den skalaren generischen Typ eines `init`-Aufrufs
unabhängig von der Argumentreihenfolge aus einem späteren `Float64`-Wert ab.
Frühere Ganzzahlliterale erhalten diesen Kontext; typisierte `Int64`-Variablen
bleiben unverändert. Compiler- und Laufzeitreferenzen prüfen beide Fälle.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.44.0 reicht bei generischen `init`-Aufrufen den während der
Überladungswahl bestimmten Typ an die vollständige Argumentprüfung weiter.
`T?`, `[T]` und verschachtelte generische Structs erhalten dadurch den Kontext
eines späteren Arguments, auch bei umgekehrter Reihenfolge benannter Argumente.
Ein typisierter `Int64`-Wert bleibt bei verlangtem `Float64` ein Compilerfehler.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.45.0 erlaubt eigene Typparameter und Constraints auf `init`.
Generische Initialisierer funktionieren bei konkreten, generischen und
importierten Structs, mit abgeleiteten oder expliziten Methodentypargumenten
und zusammen mit konkreten sowie unterschiedlich geformten generischen
Überladungen. Gleiche Signaturen nach Umbenennung der Methodentypparameter,
unpassende Constraints und nicht ableitbare Typargumente werden abgewiesen.
Explizite Methodentypargumente wählen bei überladenen Initialisierern eine
generische Signatur.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.46.0 bestimmt generische Funktionstypparameter vor der
vollständigen Argumentprüfung, sofern alle Typen aus den Ausdrücken ableitbar
sind. Dadurch geben spätere `Float64`-Argumente früheren Ganzzahlliteralen
auch in `T?` und `[T]` den richtigen Kontext. Die Referenz umfasst freie,
Instanz- und statische Methoden; typisierte `Int64`-Werte bleiben unverändert.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Die Sprachversion `0.1.0` ist jetzt getrennt von App-Version und SDK-ABI
definiert und erscheint in `physimc --version` sowie im generierten C-Kopf.
Compilerprüfung, natives Optional-Beispiel und vollständige native Sprachsuite
bestanden lokal unter MSVC Debug.

Die Sprache kann jetzt aus berechneten `[Float64]`-Werten eigene `Series`
erzeugen und Reihen mit gemeinsamem Raster koppeln. Die neue C-API prüft
Einheiten, endliche Werte, Alignment und Scratch-Budget vor der Übernahme.
`analysis_monte_carlo.phys` erzeugt aus einem expliziten Seed 1024 Ziehungen
und daraus ohne Eingabedatei Diagramm, Histogramm, Tabelle und CSV. Der Analyse-Runner-Test
vergleicht alle Ziehungen und Kennzahlen mit der gemeinsamen C-Messbibliothek.
Damit ist ein vollständiger kleiner Monte-Carlo-Auswertungspfad in der Sprache
vorhanden. `analysis_batch_endpoints.phys` fasst außerdem die Endwerte von C-
und Sprachläufen in einer gemeinsamen Datenreihe zusammen; der Runner-Test
prüft beide Eingabeformate, Plot, Histogramm und Tabelle. Die übergeordnete
LANG-005-Abnahme bleibt wegen weiterer Batch- und Auswertungsfunktionen offen.
Die betroffenen Serien- und Analyseprüfungen bestanden unter MSVC Debug,
Clang/Ninja und MSVC AddressSanitizer.

Das C17-Backend erzeugt jetzt `#line`-Zuordnungen für Funktionen,
Strukturfelder und Anweisungen. Ein gezielt ausgelöster nativer Compilerfehler
meldet die ursprüngliche `.phys`-Datei; die native Sprachsuite prüft dies mit
MSVC und Clang. Das erste Mehrdatei-Modulsystem mit qualifizierten Namen,
transitiven Imports und Dateidiagnosen ist implementiert und unter beiden
Compilern geprüft. `switch` unterstützt mehrere Enumfälle pro Zweig und prüft
Duplikate sowie vollständige Abdeckung. Erweiterte Modulauflösung und ein durchgängiger Debugger
bleiben offen.

Freie generische Funktionen werden für konkrete Aufruftypen spezialisiert;
Imports, Arrays, optionale und nominale Werte sowie rekursive Aufrufe sind
damit ausführbar. Generische Instanzmethoden, mutierende und statische Methoden
sind ebenfalls ausführbar. Das Beispiel und negative Compilerfälle bestehen
unter MSVC und Clang. Explizite Typargumente für freie Funktionen und Methoden
sind ebenfalls möglich. Generische Strukturen werden aus Konstruktorfeldern,
Ergebniskontext oder expliziten Typargumenten spezialisiert; Felder, Methoden,
verschachtelte Werte und optionale Rekursion sind nativ ausführbar. Die
Sprachversion wurde dafür auf 0.6.0 angehoben. Eingebaute Constraints
`Numeric`, `Scalar`, `Equatable` und `Vector` werden bei Spezialisierungen
geprüft. Eigene Protokolle und die vollständige LANG-006-Abnahme bleiben offen.

## Durchgeführte lokale Prüfungen

- Optionale `switch`-Muster (2026-09-23): `nil`, `Optional.some` und
  `Optional.some(name)` unterscheiden die beiden Varianten und binden den
  Inhalt als unabhängigen Wert. Guards, vollständige Abdeckung, verschachtelte
  Optionals, generische Rückgaben und besitzende Arrays sind geprüft;
  Sprachvertrag 0.16.0.

- Skalare `switch`-Muster (2026-09-23): `Bool`, `Int64` und `String` ergänzen
  Enumfälle. Feste Literale, mehrere Werte pro Zweig, Guards, doppelte Muster,
  vollständige Bool-Abdeckung und erforderliches `default` für offene
  Wertebereiche sind geprüft; Sprachvertrag 0.15.0.

- Eigene Struct-Initialisierer (2026-09-23): `static func init(...) -> Typ`
  steuert `Typ(...)`; im Initialisierer bleibt der direkte Feldkonstruktor
  verfügbar. Parameternamen, Rückgabetyp und ungültige Deklarationen werden
  geprüft. Generische Typargumente werden aus Initialisiererparametern oder
  explizit bestimmt; importierte Typen und besitzende Rückgaben laufen nativ.
  Sprachvertrag 0.14.0.

- Struktur-Feldvorgaben (2026-09-23): `let`- und `var`-Felder können einen
  typgeprüften Ausdruck als Vorgabe tragen. Der erzeugte Konstruktor akzeptiert
  ausgelassene Vorgabefelder, wertet sie bei jedem Aufruf im Modulkontext aus
  und verwaltet auch besitzende Werte. Generische Spezialisierungen und
  Laufzeit-/Compilerfehler sind geprüft; Sprachvertrag 0.13.0.

- Numerische Sprachbibliothek (2026-09-23): Trigonometrische Umkehrfunktionen,
  `atan2`, Exponentialfunktion, Logarithmen, Potenz, skalierte zweidimensionale
  Länge und drei Rundungsfunktionen erweitern die `Float64`-Operationen.
  Ungültige Definitionsbereiche und nicht endliche Ergebnisse erhalten
  Laufzeitdiagnosen mit Quellposition; Sprachvertrag 0.12.0.

- Enum-Raw-Values (2026-09-23): Explizite `Int64`-Werte und fortlaufende
  implizite Werte sind geprüft; doppelte Werte und Überläufe liefern
  Quelldiagnosen. `rawValue` sowie `Type.fromRawValue` funktionieren auch
  bei importierten Enums und an beiden `Int64`-Grenzen. Sprachvertrag 0.11.0.

- Bewachte Enum-Auswahl (2026-09-23): `case Muster if Bool-Ausdruck` prüft den
  Guard nach der Nutzdatenbindung. Ein falscher Guard setzt die Auswahl beim
  nächsten Zweig fort; unbewachte Fälle oder `default` sichern die vollständige
  Abdeckung. `break` und ein `continue` der äußeren Schleife funktionieren auch
  bei besitzenden Bindungen. Sprachvertrag 0.10.0.

- Enum-Nutzdaten (2026-09-23): Fälle tragen benannte typisierte
  Felder. Konstruktoren unterstützen positionsgebundene und benannte Argumente;
  `switch` bindet Felder im jeweiligen Zweig oder prüft nur den Falltag.
  Importierte Enums, Arrays, optionale Werte und Strukturfelder sind nativ
  ausführbar. Sprachvertrag 0.9.0; besitzende Arrays, optionale und
  verschachtelte Strukturwerte sowie über `Optional` rekursive Enums werden
  mit fallabhängigen Kopier- und Freigabefunktionen ausgeführt. Strukturelle
  Gleichheit vergleicht Enums, Strukturen, Arrays und Optionals rekursiv,
  sofern alle enthaltenen Typen vergleichbar sind.

- Bedingte optionale Bindungen (2026-09-20): `if let`, `if var`, `while let`
  und `while var` binden eine unabhängige Inhaltskopie im erfolgreichen Block.
  Der Initialisierer sieht den äußeren Gültigkeitsbereich und wird einmal
  je Prüfung ausgewertet; eine Typannotation beschreibt den Inhaltstyp.
  `else if let`, Namensverdeckung und genau eine Ebene bei verschachtelten
  Optionals sind unterstützt. Gebundene Besitzer werden beim normalen Ende,
  `return`, `break`, `continue` und Laufzeitfehler aufgeräumt. Mutation einer
  lokalen `var`-Kopie verändert die Quelle nicht. `optional_bindings.phys`
  prüft diese Regeln, 20000 verkettete Werte, 1001 Bedingungsauswertungen,
  verschachtelte Schleifen-/Switch-Abbrüche und 300000 Durchläufe. Die native
  Suite prüft zusätzlich die Speicherbilanz nach normalem Ende und zwei
  Fehlerprogrammen im Initialisierer beziehungsweise im gebundenen Block.
  Sensorwurf und Sensoranalyse verwenden die Bindung für Messposition und
  Standardabweichung; ihre C-/Berichtsreferenzen bleiben unverändert.
  Alle 36 Clang-Prüfungen bestanden: 32 Sprachprüfungen einschließlich acht
  App-Abläufen sowie vier Dokumentationsprüfungen. Nachweis:
  `build-language-binding-evidence/clang-suite.log`; visuell geprüfte
  `simulation.bmp` und `analysis.bmp` liegen im selben Ordner.

- Optionale Werte im Sprachkern (2026-09-20): `T?`, `nil`, `Optional.some(value)`,
  `hasValue`, `unwrap()` und `valueOr(fallback)` sind statisch geprüft und im
  C17-Backend umgesetzt. Konstruktion ist ausdrücklich; numerische Literale
  erhalten den erwarteten Inhaltstyp. Verschachtelte Optionals behalten jede
  Ebene, und Vergleiche mit `nil` funktionieren in beiden Reihenfolgen.
  Optionale Inhalte verwenden die gemeinsame Besitzverwaltung und das
  64-MiB-Budget. `nil` allokiert nicht; Kopien teilen unveränderlichen Speicher,
  ausgepackte Werte bleiben unabhängig. Rekursive optionale Strukturfelder
  werden wie Arraywerte iterativ freigegeben. `optional_values.phys` prüft
  Skalare, Arrays, Strukturen, Enums, Matrizen und besitzende Solverergebnisse,
  Argumentreihenfolge, 20000 verkettete Werte sowie 300000 Schleifendurchläufe
  mit `continue`/`break`. Die native Suite kontrolliert nach erfolgreichem Ende
  und fünf Fehlerprogrammen ausdrücklich null verbleibende Allokationsbytes,
  auch nach ausgeschöpftem Speicherbudget und Fehlern in Ersatz-/Konstruktorargumenten.
  Der Sensorwurf speichert seine letzte gültige Messposition als `Vec3?`;
  die Analyse stellt eine bei zu wenigen Messpunkten fehlende Standardabweichung
  als `Quantity?` dar. Sensorreferenz, Berichte und Reset nach fehlgeschlagenem
  `unwrap` werden geprüft. Bedingte Bindungen werden oben ergänzt. Implizite
  Umwandlungen zwischen `T` und `T?` sind nicht implementiert; `valueOr` wertet
  den Ersatz immer aus.
  Alle 35 Clang-Prüfungen bestanden: 31 Sprachprüfungen einschließlich acht
  App-Abläufen sowie vier Dokumentationsprüfungen. Nachweis:
  `build-language-optional-evidence/clang-suite.log`; visuell geprüfte
  `simulation.bmp` und `analysis.bmp` liegen im selben Ordner.
  Acht Prüfungen bestanden auch unter MSVC/AddressSanitizer: optionale Werte,
  Sensorreferenz, Berichte, Parser, Compiler-Mutationen, native Sprachsuite,
  Array-Besitzbasis und Fehler-Aufräumlogik. Nachweis: `asan-tests.log`.
  Das installierte und verschobene SDK bestand mit 14 Sprachprogrammen, neun
  Sprachexperimenten, C-/Physim-Auswertungen und eigenständigen Headern:
  `build-clang-ninja/SDK Test ä c4c32bc68d/verification.log`.

- Matrizen und Koordinatensysteme in der Sprache (2026-09-20): `Mat3` und `Mat4`
  binden die gemeinsame Mathematikbibliothek ein: Spaltenkonstruktoren,
  Einheitsmatrizen, Elementzugriff, Produkt, Transposition, Inversion,
  Matrix-Vektor-Produkte, Translation/Rotation/Skalierung und TRS sowie getrennte
  Punkt-, Richtungs- und Normalentransformationen. Matrixwerte funktionieren in
  Strukturfeldern, verschachtelten Arrays, eigenen Methoden und Rückgaben.
  Ungültige Indizes, nichtaffine Richtungs-/Normalentransformationen, Singularität,
  ungültige Pivot-Toleranzen und nichtendliche Ergebnisse erhalten Quelldiagnosen.
  `coordinate_frames.phys` prüft analytische Werte, projektive Division,
  Normalenorthogonalität, gedrehte Trägheit und Energie, Wertkopien und 200
  weitere Koordinatensysteme. Die Vorlage **Starrer Körper · Physim-Sprache**
  berechnet ihren Kraftangriffspunkt jetzt über `Mat4.trs`; die bestehende
  C-Referenz prüft weiterhin alle 15 Kanäle über 1002 Zustände, Szene, Reset,
  Instanztrennung und 201 gespeicherte Runner-Zeilen. 13 zusätzliche native
  Fehlerprogramme prüfen die Matrixdiagnosen an der ursprünglichen Quelle.
  Alle 35 Clang-Prüfungen bestanden: 30 Sprachprüfungen einschließlich acht
  App-Abläufen, native Mathematikreferenz und vier Dokumentationsprüfungen.
  Direkte Matrixarrays mit Kopie, Ersetzung, Anhängen, Entfernen und Slicing
  bestanden zusätzlich nach Ergänzung des Beispiels. Nachweise:
  `build-language-matrix-evidence/clang-suite.log`, `clang-matrix-arrays.log`
  sowie visuell geprüfte `simulation.bmp` und `analysis.bmp` im selben Ordner.
  Sechs Prüfungen bestanden unter MSVC/AddressSanitizer: Matrixwerte,
  Körperreferenz, native Sprachsuite, Compiler-Mutationstests, Array-Besitzbasis
  und native Mathematik. Nachweis: `asan-tests.log` im selben Evidenzordner.
  Das installierte und verschobene SDK bestand mit 13 Sprachprogrammen, neun
  Sprachexperimenten, Auswertungen in C und Physim sowie eigenständigen Headern:
  `build-clang-ninja/SDK Test ä d4dc6ddf8c/verification.log`.

- Lineare Kugelkontakte und Kollisionskandidaten in der Sprache (2026-09-20):
  `Sweep`, `Aabb` und `CollisionPair` binden die gemeinsame C-Kollisionsbibliothek
  ein. Kugel–Kugel- und Kugel–Ebene-Abfragen liefern ersten Kontakt und
  Bewegungsanteil; `contacts()` verbindet das Ergebnis mit dem Kontaktsolver.
  Statische und überstrichene Hüllquader liefern sortierte Kandidatenpaare für
  bis zu 1024 Körper. Ergebnisarrays besitzen automatisch verwalteten Speicher
  im Sprachbudget und werden ohne zweiten großen Zwischenpuffer aufgebaut.
  `sweeps.phys` prüft Randkontakte, Anfangsüberlappung, verfehlte Kontakte,
  relative Bewegung, große Koordinaten, Wertkopien und alle 523776 Paare bei
  ausgeschöpfter Kapazität. `fast_sphere.phys` und die App-Vorlage
  **Schnelle Kugel · Physim-Sprache** berechnen wiederholte elastische Wandstöße
  mit 80 m/s, Kontaktzeitpunkt und Restbewegung. Sieben Kanäle stimmen über
  2084 Zustände bei vier Schrittweiten mit der C-Referenz überein; zusätzlich
  werden analytische Bahn und Energie geprüft. Ein überschrittenes Budget von
  16 Stößen verwirft den berechneten Schritt mit Quelldiagnose. Tests prüfen
  außerdem Instanztrennung, Reset, Szene, 101 Runner-Zeilen, sieben wiederholte
  Fehlerfälle und Speicherbilanz bei Allokationsfehlern.
  Alle 34 Clang-Prüfungen bestanden: 29 Sprachprüfungen einschließlich acht
  App-Abläufen, native Kandidatensuche und vier Dokumentationsprüfungen.
  Der native Kugel-Sweep-Test bestand separat. Nachweise:
  `build-language-collision-evidence/clang-suite.log`, `clang-native-sweep.log`
  sowie visuell geprüfte `simulation.bmp` und `analysis.bmp` im selben Ordner.
  Sieben Prüfungen bestanden auch unter MSVC/AddressSanitizer: Sweep-Werte
  und Modulreferenz, Körpergruppenregression, native Sprachsuite,
  Compiler-Mutationstests sowie native Kandidatensuche und Kugel-Sweeps.
  Nachweis: `asan-tests.log` im selben Evidenzordner.
  Das installierte, verschobene SDK bestand mit zwölf Sprachprogrammen und neun
  Sprachexperimenten einschließlich Auswertung des Wandstoßmodells in C und Physim:
  `build-clang-ninja/SDK Test ä ea069165d2/verification.log`.
  Beschleunigte Bahnen, Quader-CCD und eine allgemeine automatische
  Mehrkörper-Ereignissteuerung bleiben offen.

- Gemeinsamer Körpergruppen-Solver in der Sprache (2026-09-20):
  `ContactConstraint`, `JointConstraint` und `ConstraintResult` binden den
  gemeinsamen Kontakt-/Distanzgelenksolver über Körperarrays ein. Indizes,
  Weltanker, Geometrie, alle drei Kapazitäten und Ergebniszugriffe werden geprüft.
  Der Ergebniswert besitzt automatisch verwalteten Speicher im 64-MiB-Sprachbudget;
  Kopien, Array-/Strukturfelder, Rückgaben und Ersetzungen verwenden die gemeinsame
  Aufräumlogik. Körperarrays aus `bodies()` sind unabhängig. Compilerdeskriptoren
  unterstützen jetzt auch besitzende Bibliothekswerte und eine 64-Bit-Typmaske.
  `constraint_graph.phys` prüft gemeinsame Abstützung, Rückprall, unvollständige
  Konvergenz, widersprüchliche Gelenke, 2000 Ergebnisersetzungen und gleichzeitig
  128 Körper, 512 Kontakte und 256 Gelenke. `coupled_bodies.phys` und die App-Vorlage
  **Gekoppelte Körper · Physim-Sprache** verbinden zwei Kugeln über ein Gelenk und
  lösen Bodenkontakte gemeinsam. Referenzen vergleichen alle 16 Messkanäle über
  vier Sekunden bei 1 und 2 ms sowie Szene, Instanztrennung, Reset und 801
  gespeicherte Runner-Zeilen. 13 Fehlerfälle prüfen Diagnosen und Reset;
  Allokationsfehler beim Ergebnis und beim Körperarray erhalten bestehende Werte
  und die Speicherbilanz. Alle 31 Clang-Prüfungen bestanden: 26 Sprachprüfungen
  einschließlich sieben App-Abläufen, C-Graphreferenz und vier Dokumentationsprüfungen.
  Nachweise: `build-language-graph-evidence/clang-language-tests.log`,
  `final-graph-test.log`, visuell geprüfte `simulation.bmp` und `analysis.bmp`.
  Sieben betroffene Prüfungen bestanden auch unter MSVC/AddressSanitizer:
  Graphwerte/-referenz, Kontakt-/Gelenkregression, native Sprachsuite,
  Compiler-Mutationstests und Array-Besitzbasis. Nachweis:
  `asan-language-tests.log` im selben Evidenzordner.
  Das installierte, verschobene SDK bestand mit elf Sprachprogrammen und acht
  Sprachexperimenten einschließlich Auswertung der Körpergruppe in C und Physim:
  `build-clang-ninja/SDK Test ä 9c34da98e1/verification.log`.
  Automatische Kontaktverwaltung, Warmstart, Quader-CCD und weitere Gelenkarten
  bleiben offen; ein erfolgreiches Iterationsbudget garantiert keine Konvergenz.

- Distanzgelenke in der Sprache (2026-09-20): `DistanceJoint` und `JointResult`
  verwenden die gemeinsame Mechanikbibliothek mit lokalen Ankern, Driftstabilisierung,
  unabhängigen Körperergebnissen, Impuls und Längen-/Geschwindigkeitsfehler.
  `ps_distance_joint_validate` teilt die Konfigurationsprüfung zwischen C-Solver
  und Sprachkonstruktor. `distance_joints.phys` prüft Impulsbilanz, gedrehte
  außermittige Anker, transformierte statische Körper, Array-/Strukturkopien,
  Stabilisierung und Schrittweitenverfeinerung. `joint_pendulum.phys` und die
  App-Vorlage **Gelenkpendel · Physim-Sprache** zeigen einen aufgehängten Quader
  mit 14 Messkanälen. C-Referenzen prüfen alle Kanalwerte über vier Sekunden
  bei 4 und 2 ms, Szenengeometrie/Orientierung, unabhängige Instanzen, Reset,
  abnehmenden Längendrift und 401 gespeicherte Runner-Zeilen. Sieben Fehlerfälle
  prüfen ungültige Konfiguration, singuläre Anker, Schrittweiten, Überlauf,
  Quelldiagnosen und Wiederherstellung durch Reset. 29 Clang-Prüfungen bestanden:
  23 Sprachprüfungen einschließlich sechs App-Abläufen, zwei Mechanikprüfungen
  und vier Dokumentationsprüfungen. Nachweise:
  `build-language-joint-evidence/clang-language-tests.log` und nach Verbesserung
  der Kamerasicht `final-joint-tests.log`, `simulation.bmp`, `analysis.bmp` im
  selben Evidenzordner. Die Offline-Referenz enthält jetzt ebenfalls Körper-,
  Kontakt- und Gelenkaufrufe und besteht die Prüfung gegen das Compilerregister.
  Acht betroffene Prüfungen bestanden auch unter MSVC/AddressSanitizer:
  Gelenkwerte/-referenz, Körper-/Kontaktregression, native Sprachsuite,
  Compiler-Mutationstests und beide C-Gelenksolver. Nachweis:
  `asan-language-tests.log` im selben Evidenzordner. Ein erster Aufruf ohne
  Visual-Studio-Umgebung scheiterte vor Testbeginn an der fehlenden ASan-DLL
  im Suchpfad (`asan-missing-runtime.log`); mit `vcvars64.bat` bestanden alle acht.
  Das installierte, verschobene SDK bestand mit zehn Sprachprogrammen und sieben
  Sprachexperimenten einschließlich Auswertung des Gelenkpendels in C und Physim:
  `build-clang-ninja/SDK Test ä fd5a298906/verification.log`.
  Der zunächst offene Körpergruppen-Solver wird oben ergänzt; weitere Gelenkarten
  und Quader-CCD bleiben offen. Lineare Kugel-Sweeps werden oben ergänzt.

- Kontakte in der Sprache (2026-09-20): `Contacts`, `ContactSolver` und
  `ContactResult` binden die gemeinsame Erkennung für Kugel–Kugel,
  Kugel–Ebene, Kugel–Quader, Quader–Ebene und Quader–Quader sowie den
  iterativen Paarsolver mit Coulomb-Reibung und Rückprall ein. Eigenschaften
  bleiben schreibgeschützt; Auflösung liefert beide Körper als unabhängige
  Ergebniswerte sowie Kontaktimpulse und Restfehler. Das Beispiel `contacts.phys`
  prüft analytische elastische und reibungsbehaftete Stöße, leere Kontakte,
  mehrere Kontaktpunkte, Projektion und Kopien. Die App-Vorlage
  **Box auf Ebene · Physim-Sprache** und `box_contacts.phys` entsprechen der
  vorhandenen C-Vorlage. Referenzen vergleichen alle elf Kanäle, Dimensionen
  und Szenen über je 2001 Zustände bei 1 und 2 ms, unabhängige Instanzen,
  Reset und 801 gespeicherte Runner-Zeilen einschließlich Bodenkontakt.
  Acht Fehlerfälle prüfen Einstellungen, Geometrie, Indizes, Quelldiagnosen
  und Reset nach Abbruch. Alle 20 Sprachprüfungen einschließlich fünf
  App-Workflows bestanden unter Clang/Debug. Nachweis und visuell geprüfte
  Szenen-/Analyseansichten: `build-language-contact-evidence/clang-language-tests.log`,
  `simulation.bmp` und `analysis.bmp` im selben Evidenzordner.
  Sechs betroffene Prüfungen bestanden unter MSVC/AddressSanitizer, einschließlich
  Kontaktwerten, C-Referenz, Körper-/Sensorregression, nativer Sprachsuite und
  Compiler-Mutationstests: `asan-language-tests.log` im selben Evidenzordner.
  Nach atomarer Übernahme des berechneten Schrittzustands bestanden Kontaktreferenz
  und App-Workflow erneut; `final-contact-tests.log`,
  `final-asan-contact-test.log` und die aktualisierten Bilder dokumentieren
  den Endstand. Das installierte und verschobene SDK bestand mit neun
  Sprachprogrammen und sechs Sprachexperimenten einschließlich C-Auswertung
  des Kontaktversuchs und eigenständiger Header-Prüfung:
  `build-clang-ninja/SDK Test ä b577979dcb/verification.log`.
  Die zunächst offene Distanzgelenkbindung wird oben ergänzt; gemeinsame
  Körpergruppen-Solver und lineare Kugel-Sweeps werden oben ergänzt.

- Starre Körper in der Sprache (2026-09-20): `Body.sphere` und `Body.box`
  verwenden die gemeinsame Mechanikbibliothek. Lesbare SI-Eigenschaften,
  geprüfte Zustandsänderungen, Impulse, Kräfte/Drehmomente, Quaternionrotation,
  Punktgeschwindigkeit und kinetische Energie sind angebunden. Körper haben
  unabhängige Wertkopien, auch in Arrays und Strukturen; Mutationen verlangen
  veränderbare Empfänger und veröffentlichen nur erfolgreiche Ergebnisse.
  Das eigenständige Beispiel `rigid_body.phys` prüft analytische Impuls- und
  Energiewerte sowie Kopien und Argumentauswertung. `spinning_body.phys` und
  die App-Vorlage **Starrer Körper · Physim-Sprache** zeigen einen angetriebenen
  Quader mit 15 Messkanälen. C-Referenzen prüfen zwei Schrittfolgen,
  unabhängige Instanzen, Reset, Orientierung, Szene, gespeicherte Messwerte,
  Quelldiagnosen und unveränderte Zustände nach Fehlern. Alle 17 Sprachprüfungen
  einschließlich vier App-Workflows bestanden unter Clang/Debug; Nachweis:
  `build-language-body-evidence/clang-language-tests.log`.
  Fünf betroffene Prüfungen bestanden unter MSVC/AddressSanitizer ebenfalls,
  einschließlich Körperwerten, C-Referenz, Sensorregression, nativer Sprachsuite
  und Compiler-Mutationstests: `asan-language-tests.log` im selben Evidenzordner.
  Nach Verbesserung der Vektorsichtbarkeit bestanden Körperreferenz und
  App-Workflow erneut; `final-body-tests.log`, `simulation.bmp` und `analysis.bmp`
  dokumentieren den geprüften Endstand. Das installierte, verschobene SDK
  bestand mit acht Sprachprogrammen und fünf Sprachexperimenten einschließlich
  C-Auswertung des starren Körpers:
  `build-clang-ninja/SDK Test ä 830e2d87fa/verification.log`.
  Beim Gesamtbuild wurde außerdem die benötigte `dwmapi`-Verknüpfung des
  optionalen Windows-UI-Benchmarks ergänzt. Die zunächst offene Kontaktbindung
  wird oben ergänzt, ebenso die Distanzgelenk- und Körpergruppenbindung. Diese Bindung schließt
  LANG-004 und LANG-006 noch nicht vollständig ab.

- Gültige Messreihen und Berichtstabellen in der Sprache (2026-09-20):
  `Series.select` filtert bis zu 32 zusammengehörige Spalten mit gemeinsamer
  Ergebniszuordnung. `Table`, `row` und `export` binden Berichtstabellen mit
  Einheitenumrechnung und CSV-Export ein; `Plot.points` zeichnet Messpunkte
  ohne Verbindung über Ausfälle. Die Sensorwurf-Vorlage verwendet bei
  Sprachauswertung `analysis_sensors.phys` mit Verfügbarkeit, Fehlerhistogramm,
  Messfehlerstatistik und mittlerer Standardunsicherheit. Null oder ein gültiger
  Messwert werden ausdrücklich behandelt. Compiler und Backend unterstützen
  typisierte Arrayargumente und besitzende Bibliotheksrückgaben. Referenzen
  prüfen Arraykopien, unabhängige Auswahlzuordnungen, Rohreihenfreigabe,
  Zentimeter-Meter-Konvertierung, Tabellenaliase, CSV-Inhalte, Grenzen und
  Wiederholung nach Fehlern. Alle 14 Sprachprüfungen einschließlich drei
  App-Workflows bestanden unter Clang/Debug; sieben betroffene Prüfungen
  bestanden auch unter MSVC/AddressSanitizer. Zwei ASan-Prüfungen liefen bei
  paralleler Buildlast ins Zeitlimit und bestanden im separaten Wiederholungslauf.
  Nachweise: `build-language-report-evidence/clang-language-tests.log`,
  `asan-first-attempt.log` und `asan-retry.log` im selben Evidenzordner.
  Nach der Kürzung einer Tabellenüberschrift bestanden Report- und Sensor-App-Test
  erneut; finale Logs und geprüfte `analysis.bmp`/`statistics.bmp` liegen ebenfalls
  dort. Das installierte und verschobene SDK bestand einschließlich der neuen
  Sensorauswertung mit Punktkurve, Tabellenwerten und Exporten:
  `build-language-report/SDK Test ä 4cbf5745be/verification.log`.
  Die laufende Entwicklungs-App blockierte zunächst ihre ausführbare Datei;
  der vollständige Build und die App-Prüfungen erfolgten deshalb in
  `build-language-report`. Nach Freigabe der Datei wurden App und Compiler auch
  in `build-clang-ninja/bin` erfolgreich aktualisiert. LANG-005 bleibt wegen weiterer Bindungen und
  sprachgesteuerter Batchausführung offen.

- Sensoren in der Sprache (2026-09-20): `Distribution`, `SensorConfig`, `Sensor`
  und `Measurement` binden die gemeinsame Messbibliothek ein. `read`, `reset`
  und `resetForRun` sind mutierende Methoden mit einmaliger Empfängerauswertung
  und Veröffentlichung erst nach Erfolg, auch in verschachtelten Arrays und
  Strukturmethoden. Sensoren besitzen unabhängige Wertkopien; Messwerte und
  Gültigkeit, Zeit, Unsicherheit, Rasterindex und ausgelassene Slots bleiben
  getrennt. Explizite Seeds sowie Laufseed-XOR-Streams erhalten alle 64 Bits
  und verändern den Modellgenerator nicht. Die neue App-Vorlage
  **Sensorwurf · Physim-Sprache** zeichnet Soll-/Modell-/Sensorwerte auf.
  Native Referenzen prüfen Einheitenumrechnung, Zeitraster, Ausfälle,
  Unsicherheitsrechnung, Kopien, Argument-Seiteneffekte und Fehlerdiagnosen.
  Vier Seeds einschließlich beider 64-Bit-Grenzfälle, unabhängige Instanzen,
  Reset, gespeicherte Runnerdaten und Sensorverwendung in Analysen sind gegen
  die C-API geprüft. Alle 13 Sprachtests einschließlich dreier App-Abläufe
  bestehen mit Clang Debug; die sechs ausgewählten Compiler-/Integrationsprüfungen
  bestehen mit MSVC AddressSanitizer. Der neue App-Test prüft auch die
  Gültigkeitsmaskierung der Sensorstatistik. Der installierte, verschobene
  SDK-Neubau besteht einschließlich Sensorwurf mit C- und Sprachauswertungen.
  Nachweise: `build-language-sensor-evidence/clang-language-tests.log`,
  `build-language-sensor-evidence/asan-language-tests.log` und
  `build-clang-ninja/SDK Test ä ed99d42ce1/verification.log`.
  Die Bildprüfung zeigte eine verdeckte Sensormarkierung; ein gelbes Kreuz am
  Messort ist jetzt auch neben der Modellkugel erkennbar. Sensorreferenz und
  vollständiger Sensor-App-Ablauf bestehen erneut. Simulation und Ergebnisplot
  wurden bei 1080 × 740 visuell geprüft; Bilder und abschließendes Testprotokoll
  liegen unter `build-language-sensor-evidence/`.

- Polylinien in der Sprache (2026-09-20): `polyline(points, radius, color, id)`
  bindet `[Vec3]`-Werte an die gemeinsame Szenen-API. Damit sind alle acht
  Szenenprimitive aus der Sprache erreichbar. Literale, Ausschnitte und
  Funktionsrückgaben behalten ihre Wertsemantik; eine Szene besitzt eigene
  Punktkopien. Native Prüfungen decken benannte Argumente in Quellreihenfolge,
  spätere Änderungen am Quellarray, exakt 96 Punkte, beide Kapazitätsgrenzen,
  leere/einpunktige Arrays, ungültige Radien/Farben, doppelte IDs und falsche
  Aufrufphasen ab. Ein separates Modul prüft den impliziten Elementtyp von `[]`
  ohne sonstige Vec3-Deklaration. Fehlgeschlagene Szenen bleiben vollständig leer
  und sperren weitere Schritte bis zum Reset. Die Feder-Vorlage verwendet jetzt
  eine Spirale mit 65 Punkten, geprüft gegen die C-Vorlage. Alle elf Sprachtests
  einschließlich beider App-Abläufe bestehen mit Clang Debug; Typprüfung und
  Experimentintegration bestehen zusätzlich mit MSVC AddressSanitizer.
  Nachweise: `build-polyline-evidence/clang-language-tests.log` und
  `build-polyline-evidence/asan-language-tests.log`.
  Der installierte, verschobene SDK-Neubau besteht ebenfalls einschließlich
  des Sprach-Federexperiments und seiner Auswertungen.
  Nachweis: `build-clang-ninja/SDK Test ä f11a8b0fb4/verification.log`.

- Mengenwerte mit Einheit (2026-09-20): `Quantity(value, unit)` verbindet einen
  endlichen Zahlenwert mit einer Einheit. `converted`, `adding`, `subtracting`,
  `multiplied` und `divided` verwenden Methodenaufrufe; `value` und `unit` sind
  schreibgeschützte Eigenschaften. Typannotation, Rückgaben, Arrays und eigene
  Strukturen einschließlich mutierender Methoden sind unterstützt. Die native
  Referenz prüft Umrechnung, Rechnung und unabhängige Kopien; Fehlerfälle prüfen
  inkompatible Dimensionen, Nulldivisor sowie numerischen Über- und Unterlauf.
  Wurf und Analysereferenz verwenden `Quantity`; der Editor kennt den Typnamen.
  Die Dimensionsprüfung bleibt dynamisch, statische SI-Typparameter bleiben offen.
  Alle fünf betroffenen Sprachtests bestehen mit Clang Debug; der installierte,
  verschobene SDK-Neubau besteht ebenfalls.
  Nachweis: `build-clang-ninja/SDK Test ä 3a8f1fe8d0/verification.log`.

- Einheitenalgebra in der Sprache (2026-09-20): `unit.multiplied`, `divided`,
  `powered` und `isCompatible` sind als Instanzmethoden gebunden. Ergebnisse
  behalten Wertsemantik und besitzen explizite Symbole. Native Prüfungen decken
  Dimensionen, Maßstäbe, negative/Nullpotenzen, benannte Argumente in Quellreihenfolge,
  Int8-Grenzen sowie Exponentenüberlauf, Maßstabsüberlauf und Unterlauf auf null ab.
  Das Wurfbeispiel erzeugt Geschwindigkeit und Energie durch Einheitenalgebra;
  die Analysereferenz verwendet eine zusammengesetzte Integraleinheit.
  Typprüfung, native Programme, CLI, Experiment- und Analyseintegration bestehen
  mit Clang Debug und MSVC AddressSanitizer. Der installierte, verschobene SDK-Neubau einschließlich
  Sprachprogrammen sowie Experimenten und Auswertungen besteht ebenfalls.
  Nachweis: `build-clang-ninja/SDK Test ä 29c5f771fa/verification.log`.

- Statische Strukturmethoden (2026-09-20): `static func` wird über den Typ
  aufgerufen, beispielsweise `Particle.atRest(position)`. Es gibt keinen
  `self`-Empfänger. Die Typprüfung weist verwechslungsbedingte Aufrufe über
  Instanzen sowie Instanzmethodenaufrufe ohne Empfänger ab. Benannte Argumente,
  Vorwärtsaufrufe, Rekursion und besitzende Rückgaben sind abgedeckt.
  Alle sieben ausgewählten Sprachtests bestehen mit Clang Debug und MSVC
  AddressSanitizer (Lexer, Parser, Typprüfung, CLI, native Programme,
  Experiment- und Analyseintegration).

- Mutierende Strukturmethoden (2026-09-20): `mutating func` erlaubt Änderungen
  über `self` und verlangt am Aufrufort einen veränderlichen Struktur-/Indexpfad.
  Die Empfängerkopie wird erst nach erfolgreicher Rückkehr veröffentlicht.
  Prüfungen decken skalare und besitzende Rückgaben, `Void`, frühe Rückkehr,
  Selbstersetzung, rekursive/verschachtelte Methoden, kopierte Arrayfelder,
  einmalige Indizes und konkurrierende Seiteneffekte im Argument ab. Ein reines
  Strukturprogramm prüft den Codepfad ohne Arraylaufzeit. Der Experimentfehlerfall
  verändert verschachtelte Empfängerkopien und prüft nach abgefangenem Fehler beim
  Reset den unveränderten globalen Zustand. Die Analyse prüft eine mutierende
  Methode auf einem eigenen Datenreihen-Wrapper. Pendel und Partikel verwenden
  jetzt `advance` direkt auf Arrayelementen.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer.
  Der Editor hebt `mutating` und `self` hervor; der vollständige Sprach-App-Ablauf
  besteht mit dem aktualisierten Pendelbeispiel.
  Auch der installierte, verschobene SDK-Neubau mit allen Beispielen besteht.
  Nachweis: `build-clang-ninja/SDK Test ä fcdaa9f42a/verification.log`.

- Eigene Strukturmethoden (2026-09-20): `func`-Deklarationen innerhalb einer
  Struktur verwenden `self` als unveränderlichen Empfänger. Auflösung nach Typ,
  benannte Argumente, Vorwärtsaufrufe, Rekursion, Aufrufketten und Strukturen ohne
  Felder sind implementiert. Native Referenzen prüfen eigene Methoden namens
  `append`/`count`, unabhängige Arrayfelder, besitzende Rückgaben, einmalige
  Auswertung sowie Freigabe bei Assertion und Rekursionsgrenze. Die semantische
  Mutationsprüfung enthält eigene Methodendeklarationen. Partikel und Pendel
  verwenden `advanced`; die Analysereferenz ruft eine eigene Statistikmethode auf.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer;
  die nachträglich ergänzte Analysereferenz besteht ebenfalls auf beiden Compilern.
  Der installierte, verschobene SDK-Neubau besteht mit allen Beispielen.
  Nachweis: `build-clang-ninja/SDK Test ä c5a4a7b8c2/verification.log`.
  Statische Methoden wurden im nachfolgenden Schritt ergänzt (siehe oben).

- Einheitliche Methoden-API (2026-09-20): typgebundene Operationen verwenden
  Empfängeraufrufe, unter anderem `array.append(value)`, `array.remove(at: index)`,
  `vector.normalized()`, `rotation.rotate(vector)`, `channel.sample(value)`,
  `dataset.series(name)` und `series.derivative(time)`. Arraylängen sind über
  `.count` lesbar. Konstruktoren heißen `Unit`, `Channel` und `Dataset`;
  `Quat.axisAngle` ist ein statischer Konstruktor. Die frühere globale Form dieser
  Operationen ist entfernt. Methoden werden nach Empfängertyp aufgelöst und
  können verkettet werden; der Empfänger wird vor den Argumenten einmal ausgewertet.
  Mutierende Arraymethoden unterstützen veränderliche Struktur-/Indexpfade und
  veröffentlichen den aufgebauten Wurzelwert erst nach erfolgreicher Änderung.
  Beispiele, Sprachreferenz und native Fehlerfälle sind auf die Methoden-API umgestellt.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer;
  beide App-Abläufe (vollständig eigene Sprache sowie gemischtes C/Sprachprojekt)
  bestehen ebenfalls. Die vergrößerte native Testsuite erhält 180 Sekunden für
  ihren Build und 300 Sekunden insgesamt; Buildfehler melden nun den Prozessstatus.
  Das installierte, verschobene SDK besteht mit allen Beispielen.
  Nachweis: `build-clang-ninja/SDK Test ä 162d82e893/verification.log`.
  Eigene statische Strukturmethoden wurden anschließend ergänzt (siehe oben).

- Erster Arrayindex (2026-09-23): `array.firstIndex(of: value)` gibt den
  ersten gleichen Index als `Int64?` oder `nil` zurück. Verschachtelte und
  besitzende Elemente, leere Arrays, Typfehler, Auswertungsreihenfolge und
  eine Million wiederholte Suchaufrufe sind unter MSVC und Clang geprüft;
  Sprachvertrag 0.23.0. Der lokale MSVC-ASan-Build hängt schon beim bisherigen
  `array_concat`-Programm ohne CPU-Fortschritt; eine ASan-Bestätigung dieses
  Ausbaus liegt daher nicht vor.

- Array-Suche (2026-09-23): `array.contains(value)` verwendet die vorhandene
  strukturelle Gleichheit für vergleichbare Elementtypen. Leere, verschachtelte
  und besitzende Arrays sowie Typfehler sind nativ geprüft; Sprachvertrag 0.22.0.

- Array-Einfügen (2026-09-23): `array.insert(value, at: index)` verändert
  einen veränderlichen Arraypfad, `array.inserting(value, at: index)` liefert
  eine unabhängige Kopie. Beide akzeptieren die Länge als Einfügeposition,
  prüfen Grenzen und veröffentlichen erst nach erfolgreicher Kopie;
  Sprachvertrag 0.21.0.

- Bereichsschrittweiten (2026-09-23): `for i in 0..<10 by 2` und
  `array[1..<end by 2]` prüfen positive `Int64`-Schritte und werten sie
  einmal aus. Schrittweise Bereichszuweisungen behalten die Arraylänge und
  verlangen ein Ersatzarray mit genau der Zahl ausgewählter Elemente.
  Fehler veröffentlichen keine Teiländerung; Sprachvertrag 0.20.0.

- Arrayverkettung (2026-09-23): `a + b` erzeugt aus zwei gleich typisierten
  Arrays einen unabhängigen Wert. `+=` schreibt auf veränderlichen Namen,
  Feld-/Indexpfaden und ganzen Bereichen. Leere Literale nutzen Kontext vom
  anderen Operanden oder Ergebnistyp; Speicher- und Grenzfehler veröffentlichen
  keine Teiländerung. Referenztests prüfen verschachtelte Besitzer,
  Auswertungsreihenfolge und nur einmal ausgewertete Bereichsgrenzen;
  Sprachvertrag 0.19.0.

- Arraybereiche mit ausgelassenen Grenzen (2026-09-23): In Arrayindizes und
  Bereichszuweisungen dürfen Unter- und/oder Obergrenze fehlen. Der Anfang
  ist dann null, das Ende die Arraylänge; ein fehlendes geschlossenes Ende
  ergibt auf einem leeren Array ebenfalls einen leeren Bereich. Parser,
  Typprüfung, C-Emission und Laufzeitprüfung teilen diesen Vertrag.
  Eigenständige Bereiche und `for` behalten explizite Grenzen;
  Sprachvertrag 0.18.0.

- Bereichszuweisungen (2026-09-23): `array[start..<end] = replacement` und
  die geschlossene Form ersetzen Bereiche veränderlicher Arraypfade. Die
  Ersatzlänge darf abweichen; leere Bereiche fügen ein und leere Ersatzarrays
  entfernen. Der Compiler prüft Typ und Veränderlichkeit und wertet Empfänger,
  Grenzen und Ersatzarray je einmal in dieser Reihenfolge aus. Laufzeitgrenzen
  werden vor dem Ersatzarray geprüft; die transaktionale Array-Ersetzung
  veröffentlicht verschachtelte Feld- und Indexpfade erst bei Erfolg.
  Referenztests prüfen Kopien, Alias-Ersatz, verschachtelte Besitzer,
  Seiteneffekte, Grenzfehler und Speicherbilanz mit MSVC und Clang.

- Array-Ausschnitte (2026-09-20): `array[start..<end]` und `array[start...end]`
  erzeugen unabhängige `[T]`-Werte mit Indexbeginn null. Die Typprüfung verlangt
  ganzzahlige Grenzen und weist Ausschnitte als Zuweisungsziele ab. Native
  Referenzen prüfen Verschachtelung, Strukturen mit Arrayfeldern, Rückgabe,
  Schleifen, Seiteneffekte in den Grenzen und Fehlerexits ohne verbleibende
  Array-Allokationen. Die Besitztests vergleichen 98 Grenzkombinationen sowie
  extreme Ganzzahlgrenzen und injizieren Fehler bei Block- und Elementkopien.
  Das Partikelbeispiel bearbeitet eine unabhängige Teilgruppe; Experiment- und
  Analysefehlerprüfungen halten Ausschnitte über abgefangene Laufzeitfehler.
  Weggelassene Grenzen, Schrittweiten und Bereichszuweisungen waren damals offen.
  Die oben dokumentierten Methoden-/SDK-Prüfungen schließen diese Ausschnittfälle ein.

- Arraygrößen ändern (2026-09-20): `arrayAppending(array, element)` und
  `arrayRemoving(array, index)` erzeugen veränderte Wertkopien. Typprüfung,
  Kontext für angehängte Elemente und Auswertung von links nach rechts sind
  implementiert. Native Referenzen prüfen unabhängige Kopien, verschachtelte
  Arrays, Strukturen mit Arrayfeldern, Seiteneffekte, leere Ergebnisse und
  negative/zu große Indizes sowie Entfernen aus einem leeren Array. Fehlgeschlagene
  Operationen räumen ihre Besitzer vor dem Fehlerexit auf. Pendel-, Partikel-
  und Analysebeispiele verwenden diese Funktionen. Mutierende Methoden und
  Bereichszuweisungen waren damals weiterer Ausbau.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer.
  Der installierte, verschobene SDK-Neubau besteht mit allen Beispielen;
  Nachweis: `build-clang-ninja/SDK Test ä 727102c290/verification.log`.

- Direkte Arrayiteration (2026-09-20): `for element in array:` wertet das
  Array einmal aus und durchläuft einen unabhängigen Snapshot. Die unveränderliche
  Schleifenvariable erhält den Elementtyp, auch bei verschachtelten Arrays und
  Strukturen mit besitzenden Feldern. Native Prüfungen decken Änderungen am
  ursprünglichen Array, einmalige Auswertung, leere Arrays, verschachtelte Schleifen,
  `break`, `continue` und besitzende Rückgabewerte ab. Pendel und Partikelbeispiel
  verwenden die neue Syntax. Alle neun Sprach-/Besitztests bestehen mit Clang Debug
  und MSVC AddressSanitizer; der verschobene SDK-Neubau mit allen Beispielen besteht.
  Ein zusätzlicher Fehlerexit innerhalb einer verschachtelten Arrayiteration
  bestätigt auf beiden Compilern die vollständig ausgeglichene Allocator-Bilanz.
  SDK-Nachweis: `build-clang-ninja/SDK Test ä 0802818f96/verification.log`.

- Native Spracharrays (2026-09-20): alle drei Ausgabearten unterstützen Literale,
  `arrayCount`, gelesene/veränderte Indizes, verschachtelte Arrays, besitzende
  Strukturfelder, Funktionsparameter/-rückgaben und persistente Modulwerte.
  Kopien bleiben unabhängig; indizierte Zuweisungen werten jeden Index und die
  rechte Seite einmal aus und veröffentlichen den aufgebauten Wurzelsnapshot.
  Generierte Kopier-/Zerstörungsfunktionen und Aufräumketten decken auch
  Kurzschlussauswertung, Schleifen, Switch, `break`, `continue` und Rückgabe ab.
  Ein 64-MiB-Budget begrenzt die Arrayblöcke jeder Programmausführung/Modulinstanz.
  Die Freigabe tief verschachtelter Werte nutzt eine Arbeitsliste statt C-Rekursion.
  Native Referenzen prüfen 10.000 verschachtelte Knoten, negative/zu große Indizes,
  Teilkonstruktion, Speichererschöpfung und eine vollständig ausgeglichene
  Allocator-Bilanz auch nach Fehlerexit. Das neue Partikelbeispiel vergleicht drei
  Bewegungen nach 100 Schritten analytisch; Pendel und Analyse nutzen Arraywerte.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer;
  beide Sprach-App-Abläufe und der verschobene SDK-Neubau bestehen ebenfalls.
  SDK-Nachweis: `build-clang-ninja/SDK Test ä 788d125a4e/verification.log`.
  Mutierende Methoden und Bereichszuweisungen waren damals weiterer Ausbau.

- Array-Typprüfung (2026-09-20): strukturell kanonisierte `[T]`-Typen,
  homogene Literale mit Kontext für leere/numerische Elemente, verschachtelte
  Indizes, Funktionsparameter/-ergebnisse, Arrays eigener Strukturen/Enums und
  veränderliche Feld-/Indexpfade sind statisch geprüft. Arraytypen bleiben von
  nominalen Typen getrennt; verschachtelte Annotationen und Typvergleiche verwenden
  das begrenzte semantische Arbeitsbudget. Einzelbyte-Mutationen prüfen zusätzlich
  die Gültigkeit kanonischer Elementtypverweise.
  Alle neun Sprach-/Besitztests bestehen mit Clang nach Korrektur der neuen
  CLI-Erwartung auf den vorhandenen Backend-Fehlercode 2. Typprüfung und native
  Regression bestehen zusätzlich mit MSVC AddressSanitizer.
  **Native Array-Codeerzeugung bleibt offen:** alle drei Ausgabearten weisen
  besitzende Arraytypen mit Quelldiagnose ab, bevor C-Code geschrieben wird.

- Besitzbasis für Spracharrays (2026-09-20): unveränderliche, referenzgezählte
  Blöcke ermöglichen Wertkopien ohne Allokation. Bereichsänderungen veröffentlichen
  erst nach erfolgreicher Elementkonstruktion einen neuen Block. Prüfungen decken
  2.000 Änderungen gegen ein Referenzmodell, Selbst-Einfügen, verschachtelte Kopien,
  gültigkeitsgeprüfte Indizes, Größen-/Referenzüberlauf, Spitzenbudgets und jeden
  Allokationsfehler einer Änderung mit besitzenden Elementen ab. Teilkopien werden
  zurückgerollt, ohne die alte Besitzbilanz oder Lesepointer zu ändern.
  Aufräumketten zerstören lokale Besitzer vor `longjmp` und vor dem Fehlerexit;
  Bereichsmarken, umgekehrte Reihenfolge und getrennte verschachtelte Traps sind
  geprüft. Beide neuen Werttests bestehen mit Clang Debug und MSVC AddressSanitizer;
  die sieben bisherigen Sprachtests bestehen mit Clang, native Programme zusätzlich
  mit AddressSanitizer. Die CTest-Suite umfasst jetzt 84 Tests.
  **Noch keine Array-Ausführung in der Sprache:** Typprüfung, generierte Besitzer,
  persistente Modulwerte und Indexzuweisungen bleiben anzubinden. Vertrag und
  offene Punkte: [Array-Werte und Speicherlebensdauer](language-values.md).

- Quaternion-Rechnung in der Sprache (2026-09-20): `normalizeQuat`,
  `conjugateQuat`, `multiplyQuat` und `slerpQuat` binden die gemeinsame
  Mathematikbibliothek ein. Native Referenzen prüfen das nicht kommutative
  Hamilton-Produkt, unveränderte Größen bei roher Multiplikation, Reihenfolge
  zusammengesetzter Rotationen, Rückrotation, große/kleine Normalisierungen,
  antipodale Quaternionen und kleine Interpolationswinkel. Nullrotationen,
  Produktüberlauf und Interpolationswerte außerhalb [0, 1] liefern Quelldiagnosen.
  Das ausgelieferte `rotation_path.phys` prüft einen zusammengesetzten
  Rotationspfad an 101 Punkten gegen eine analytische Referenz auf 1e-12.
  Alle sieben Sprachtests bestehen mit Clang Debug; Typprüfung und native
  Ausführung zusätzlich mit MSVC AddressSanitizer. Der verschobene SDK-Neubau
  mit sechs eigenständigen Sprachprogrammen besteht:
  `build-clang-ninja/SDK Test ä b65741742a/verification.log`.

- Vierdimensionale Sprachvektoren (2026-09-20): `Vec4` ist ein eigener Werttyp
  mit vier veränderlichen Feldern entlang eines `var`-Pfads, Funktionsübergabe,
  Strukturkopien und denselben geprüften Operatoren wie `Vec2`/`Vec3`.
  `dot4`, `length4` und `normalize4` verwenden die gemeinsame Mathematikbibliothek.
  Native Prüfungen decken die vierte Komponente, einmalige Auswertung,
  sehr große/kleine Werte, Nullvektoren, Division durch null und Überlauf ab;
  falsche Dimensionen, Quaternion-Verwechslungen und unveränderliche Ziele
  werden vor Ausführung abgewiesen. Das ausgelieferte `phase_space.phys`
  integriert zwei harmonische Freiheitsgrade über 1.000 RK4-Schritte und prüft
  analytischen Zustand und Energie auf 1e-10.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer.
  Der verschobene SDK-Neubau besteht einschließlich des neuen Beispiels:
  `build-clang-ninja/SDK Test ä 043f075317/verification.log`.

- Sprachintegration und SDK-Auslieferung (2026-09-20): Der vollständige
  Clang-Debug-Build und alle 80 bisherigen CTest-Prüfungen bestehen unter Windows.
  Zwei neu registrierte, anschließend ausgeführte App-Prüfungen bestehen ebenfalls:
  reine Sprachprojekte und C-Experimente mit Sprach-Analyse prüfen Quelldiagnosen,
  Fehlerkorrektur, native Builds, Laufsteuerung, Analyse, Exporte und Wiederöffnen.
  Beide behalten Projekte und Screenshots in eindeutigen Unicode-Pfaden.
  Der erweiterte SDK-Test baut nach Installation und Verschieben alle C-Vorlagen
  und Sprachbeispiele neu. Vier Sprachprogramme sowie Pendel, Wurf und Feder mit
  jeweils beiden Sprach-Analysen bestehen; der installierte Compiler wurde aus
  dem verschobenen SDK gefunden. Messdateien und Berichte werden wieder eingelesen.
  Nachweis: `build-clang-ninja/SDK Test ä a860977fa8/verification.log`.
  Die Suite umfasst damit 82 Tests; Linux ist weiterhin nicht lokal ausgeführt.

- Verkürzte Sprachzuweisungen (2026-09-20): `+=`, `-=`, `*=`, `/=` und `%=`
  verwenden dieselbe Typprüfung und geprüfte Arithmetik wie ausgeschriebene
  Zuweisungen. Native Referenzen prüfen ganze Zahlen, Vektoren, verschachtelte
  Felder, Schleifen und einmalige rechte Auswertung nach Lesen des alten Werts.
  Überlauf und Zugriff auf noch nicht initialisierte Globale werden abgefangen;
  unveränderliche Felder und unpassende Typen scheitern vor Codeerzeugung.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  Experiment-, native und Frontendtests zusätzlich mit MSVC Release.
  Wurf und Feder nutzen die Syntax und bestehen auch aus dem installierten SDK
  die bisherigen Bewegungs- und Energietests.

- Sprachbindungen für Kraftgesetze (2026-09-20): axiale Feder/Dämpfung,
  Stokes-/quadratischer Kugelwiderstand und Auftrieb verwenden die geprüften
  Mechanikfunktionen. Native Referenzen prüfen Vorzeichen, Kompression, axiale
  Dämpfung, Widerstandsleistung, Auftrieb und Fehler bei Parametern, Singularität
  und Überlauf. `spring.phys` integriert Bewegung und dissipierte Arbeit mit
  im Sprachquelltext formulierten RK4-Stufen. 2.000 Zeitschritte mit dt=0,002 s
  vergleichen alle elf Kanäle gegen die C-Vorlage; die analytische Auslenkung
  wird auf 2e-9 m, die Gesamtenergiebilanz auf 2e-8 J geprüft.
  Zusätzlich stimmen die 13 Szeneobjekte und 65 Federpunkte nach dem Start
  und nach 2.000 Schritten überein. Der vollständige Sprachprojekt-Workflow
  besteht unter MSVC und Clang.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  Experiment-, native und Typprüfertests zusätzlich mit MSVC Release.
  Das aus dem installierten SDK gebaute Federmodul besteht dieselben Referenzen.
  Ein SDK-Runner-Lauf schreibt 2.001 Messzeilen und wird mit der Sprach-Auswertung
  erfolgreich zu Bericht, Geschwindigkeits-CSV und Positions-SVG verarbeitet.
  Allgemeine Integrator-Callbacks und die Polyline-Federdarstellung bleiben offen.

- Vektorrechnung in der Sprache (2026-09-20): `Vec2`/`Vec3` unterstützen
  Addition/Subtraktion, Vorzeichen, Skalierung von beiden Seiten, Division und
  exakten Komponentenvergleich. Dot/Cross, Länge und Normalisierung verwenden
  die gemeinsame Mathematikbibliothek. Typfehler, Reihenfolge und einmalige
  Operandenauswertung, verschachtelte Ausdrücke, Nullvektoren, große Komponenten,
  winzige Divisoren sowie Überlauf-/Nullteilerdiagnosen sind geprüft.
  Die Wurfvorlage verwendet jetzt Vektorgleichungen und das Skalarprodukt für
  Energie; Bewegungs-/Energietests und gespeicherte Runner-Werte bleiben korrekt.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  Experiment-, native und Typprüfertests zusätzlich mit MSVC Release. Die aus
  dem installierten SDK gebaute Wurfvorlage besteht dieselben Experimenttests.

- Erweiterte Sprach-Szenenbindung (2026-09-20): `Quat` mit Wertsemantik,
  `axisAngle` und normalisierendem `rotate` sowie Box, Ebene, Pfeil, Punkt und
  UTF-8-Label ergänzen Kugel und Linie. Native Referenzen prüfen Strukturkopien,
  Komponentenänderung und 90°-Rotation. Experimenttests prüfen die Geometrie,
  Orientierung, Farbe, ID und Beschriftung aller sieben Formen sowie Reset.
  Nullquaternionen, nichtpositive Größen, Steuerzeichen, doppelte IDs,
  UInt32-Überlauf und falsche Aufrufphasen werden abgefangen; Teilszenen werden
  verworfen und lassen sich nach Reset wieder aufbauen. Alle sieben Sprachtests
  bestehen mit Clang Debug und MSVC AddressSanitizer; Experiment-, native und
  Typprüfertests zusätzlich mit MSVC Release. `scene_shapes.phys` wurde aus dem
  installierten SDK gebaut und besteht dieselben Szene-/Rotationstests.
  Die Sprachbindung für Polylinien bleibt offen.

- Zufallsverteilungen in Sprachexperimenten (2026-09-20): `randomUniform` und
  `randomNormal` verwenden die geprüfte C-Verteilungsbibliothek und den Generator
  der jeweiligen Experimentinstanz. Tests vergleichen vier Seeds einschließlich
  0 und UINT64_MAX, unverbrauchte Ziehungen bei degenerierten Verteilungen,
  unabhängige Instanzen, Reset mit gleichem/geändertem Seed und 201 aufgezeichnete
  Runner-Werte gegen C-Referenzen. Ungültige Parameter und Aufrufe aus Initialisierung
  oder Szenenaufbau werden mit Quelldiagnosen abgefangen; der Generator bleibt bei
  fehlgeschlagenen Ziehungen unverändert. Alle sieben Sprachtests bestehen mit
  Clang Debug und MSVC AddressSanitizer; Experiment-, CLI- und Typprüfertests
  zusätzlich mit MSVC Release. Das installierte SDK baut `random_samples.phys`,
  dessen Runner-Lauf 201 Messzeilen schreibt. Explizite, kopierbare Sprachströme
  sind inzwischen mit `Rng(seed)`, `sample` und `reseed` auch in eigenständigen
  Programmen und Analysen verfügbar. `Rng.forRun` und `reseedForRun` leiten
  getrennte Ströme aus dem Seed eines Experimentlaufs ab; Tests vergleichen
  ihre Werte und Resets mit der C-Referenz. Weitere Unsicherheitsmodelle bleiben offen.

- Explizite Sprach-Zahlenkonvertierungen (2026-09-20): `Int64(wert)` und
  `Float64(wert)` sind typgeprüfte Aufrufe mit einmaliger Argumentauswertung.
  Float64 nach Int64 prüft den Bereich vor dem Abschneiden; Int64 nach Float64
  verwendet explizite Rundung zur nächsten Zahl mit geradem Gleichstandsfall.
  Native Grenztests prüfen Identität, beide Vorzeichen, Nachkommastellen,
  2^53-Rundung, beide Int64-Grenzen und fehlschlagende Rückkonvertierung.
  Die Analyse-Referenz berechnet einen eigenen Mittelwert aus der Messpunktzahl;
  eine zusätzliche Fehlerreferenz prüft abgefangenen Konvertierungsüberlauf.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  native Tests und Typprüfung bestehen zusätzlich mit MSVC Release. Das neue
  `sampling.phys` wurde aus dem installierten SDK gebaut und ausgeführt:
  die numerische Verschiebung -13,6133 stimmt innerhalb von 1e-12 mit der
  analytischen Lösung überein.

- Enum-Auswahl mit `switch` (2026-09-20): Auswahl und Fallblöcke verwenden `:`
  und Einrückung. Die statische Prüfung verlangt vollständige Fallabdeckung oder
  ein abschließendes `default` und weist doppelte/fremde Fälle sowie dynamische
  Fallmuster ab. Native Referenzen prüfen einmalige Auswertung, Rückgaben,
  automatisches Zweigende, verschachtelte Auswahl und `break`/`continue` mit
  Schleifen. Mutationstests enthalten jetzt ebenfalls Auswahlblöcke.
  Die sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  native Ausführung und Frontendtests bestehen zusätzlich mit MSVC Release.
  Auswahl ist derzeit auf einfache Enums beschränkt; Nutzdaten und erweiterte
  Fallmuster gehören weiterhin zum offenen Sprachausbau.

- Einfache Sprach-Enums (2026-09-20): `enum Name:` mit eingerückten `case`-Zeilen,
  nominale Typprüfung, qualifizierte Fälle, Gleichheit, Kopien, Strukturfelder und
  Funktionsparameter/-ergebnisse sind implementiert. Parser- und Typfehler sowie
  Einzelbyte-Mutationen ergänzen einen nativ ausgeführten Zustandsautomaten.
  Die sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  der native Enumtest sowie Parser-/Checker-Tests bestehen auch mit MSVC Release.
  Das Flugphasen-Beispiel wurde aus dem installierten SDK gebaut und liefert
  die geprüfte Landung bei 1,02 s. Enums mit Nutzdaten bleiben offen;
  LANG-006 ist damit noch nicht vollständig erfüllt.

- Weitere Sprach-Datenreihenoperationen (2026-09-20): Integral mit einheitengeprüftem
  Anfangswert, affine Transformation, Summe/Differenz/Produkt/Quotient und explizites
  lineares/Nearest/Previous-Resampling verwenden die gemeinsame Series-Bibliothek.
  Numerische Quellreferenzen prüfen Trapezwerte, Anfangswert, Offset, Quotient und
  Zwischenpunkte einschließlich der Nearest-Gleichstandsregel. Ein C-Lauf wird
  explizit auf das Raster eines Sprachlaufs resampelt, mit dessen Position verrechnet
  und nach Schließen der Quelle weiter gelesen. Fehlerreferenzen prüfen falsche
  Integraleinheiten, unabhängige Zuordnungen, Extrapolation und Nullteiler.
  Das Beispiel `analysis_integral.phys` erstellt Positionsrekonstruktion und
  Diskretisierungsresiduum mit Diagrammen und Exporten.
  Die Analyse-Referenzen bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer einschließlich der nativ erzeugten Analysequellen.
  Das Integralbeispiel wurde aus dem installierten SDK gebaut und mit dessen
  Runnern ausgeführt. Der Vakuumwurf liefert 41 exportierte Residuen mit maximal
  1e-12 m zulässiger Abweichung; Bericht und SVG wurden ebenfalls erzeugt.

- Analyseeditor für die Physim-Sprache (2026-09-20): Beim Anlegen wählt
  „Sprache der Auswertung“ unabhängig vom Experiment C oder die erste
  Physim-Auswertungsvorlage. `analysis=analysis.phys` wird von App und CMake
  berücksichtigt; unbekannte und doppelte Einträge werden abgewiesen.
  Laden/Speichern, Backups, Autosave, Syntaxfarben, Tab-Leerzeichen, Builds,
  Quelldiagnosen und archivierte Analysequellen verwenden den gewählten Dateityp.
  Der neue App-Test `language_full` prüft den absichtlichen Analyse-Typfehler,
  die Zeilennavigation, den korrigierten Build, Simulation, zwei Sprachdiagramme,
  CSV/SVG, den bytegleichen Quellsnapshot und erneutes Öffnen beider Sprachdateien.
  `language_mixed` besteht mit C-Experiment und Sprach-Auswertung. Der volle
  Sprachablauf besteht ebenfalls aus dem installierten SDK bei 1080 × 740 in
  einem Pfad mit Umlaut; Editor und Bericht wurden visuell geprüft.
  Analyseeditor und Bericht gehören damit zum ersten durchgängigen Sprachablauf.
  Vollständige API-/Vorlagenparität, weitere Sprachtypen, Debugger und Linux-Abnahme
  bleiben offen; LANG-005 bis LANG-007 sind weiterhin nicht vollständig abgenommen.

- Erste Sprach-Auswertung (2026-09-20): `physimc --emit-analysis` und
  `physim_add_analysis` erzeugen Module für den vorhandenen Analyse-Runner.
  Der Einstieg `analyze()` arbeitet mit geprüften `Dataset`-/`Series`-/`Plot`-Handles,
  ausgewählten Eingabeläufen, Statistik, Ableitung, gleitendem Mittel, Linienplots,
  Histogrammen sowie CSV-/SVG-Export. Ein Bericht wird als normale `.psreport`
  mit Quellprovenienz gespeichert. Kontext, Scratchdateien und Bericht werden
  bei Erfolg und abgefangenen Fehlern freigegeben. Die Referenz verarbeitet
  gemeinsam eine in C erzeugte Polynomreihe und den echten Sprachwurf-Runnerlauf;
  Berichtwerte, SI-Dimensionen und Exporte werden gelesen und geprüft. Zusätzliche
  Quellassertions prüfen Statistik, Einzelwerte, gleitende Mittel, Handlefreigabe
  und erneutes Datasetöffnen. Zwei Fehlerquellen prüfen veraltete Handles und
  inkompatible Kurveneinheiten im Runner und bei wiederholten direkten ABI-Aufrufen.
  Alle sieben Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer mit instrumentierten erzeugten Modulen. Das aus dem installierten
  SDK gebaute Analysebeispiel besteht dieselben Referenzen mit dem installierten
  Analyse-Runner und dem ebenfalls aus dem SDK gebauten Sprachexperiment.
  Tabellen, weitere Series-Funktionen, Sprach-Batchsteuerung und Analyseeditor-
  Auswahl bleiben offen; dies ist keine vollständige LANG-005-/LANG-007-Abnahme.

- Erste Sprachintegration in der App (2026-09-20): Die Projektvorlagen
  „Pendel · Physim-Sprache“ und „Vakuumwurf · Physim-Sprache“ erzeugen `main.phys`
  zusammen mit der vorhandenen C-Auswertung. Projektbeschreibung, Laden,
  Speichern/Backups, Autosave, Editorbeschriftung/Syntaxfarben, CMake-Build,
  Quelldiagnosen und Lauf-/Batch-Quellsnapshots berücksichtigen die Sprache.
  Tab fügt im Sprachexperiment vier Leerzeichen ein. Die Offline-Dokumentation
  enthält den Sprachvertrag und den aktuellen App-Einstieg. CMake reicht
  Compilerdiagnosen ohne eigene Zeilenumbrüche weiter, sodass MSBuild und App
  die ursprüngliche Quellposition erhalten. Die vorhandenen vollständigen
  App-Selbsttests bestehen für Sprachpendel, Sprachwurf mit Leerzeichen/Umlaut
  im Projektpfad und ein C-Pendelprojekt: absichtlicher Typ-/Compilerfehler,
  Zeilennavigation, korrigierter Build, Simulation, Pause/Einzelschritt/Stop,
  C-Auswertung, Plots/Tabellen, Exporte, Vergleich und erneutes Projektöffnen.
  Der Editor bei 1440 × 940 wurde visuell geprüft. Derselbe vollständige
  Sprachpendeltest besteht aus dem installierten SDK-Paket bei 1080 × 740;
  Editor und Quelldiagnose wurden dort ebenfalls visuell geprüft. Der Test
  vergleicht den archivierten Originalquelltext mit dem gespeicherten Editorinhalt.
  Die Batchreferenz besteht
  einschließlich archivierter `.phys`-Quelle; nativer Sprachbuild und
  Experiment-ABI-Referenzen bestehen weiterhin. Ein Physim-Analyseeditor,
  vollständige Sprach-/SDK-Parität und Linux-Abnahme bleiben offen.

- Erste Sprach-Experimentmodule (2026-09-20): `--emit-experiment` und
  `physim_add_experiment` erzeugen native Module für die vorhandene ABI.
  Globale Werte leben pro Instanz; Initialisierung, Create, Reset, Step, Szene und
  Destroy laufen über einen generierten Adapter. Fehler werden an der ABI-Grenze
  abgefangen, fehlgeschlagene Creates freigegeben und fehlerhafte Instanzen bis
  zum erfolgreichen Reset gesperrt. Quelldiagnosen erreichen den echten Runner
  auch bei Create- und Szenenfehlern während Pause/Handshake.
  Vec2/Vec3, mathematische Funktionen, Einheitenkonvertierung, Kanalhandles,
  Messwerte, Metadaten, Kugeln/Linien und der gemeinsame symplektische Integrator
  sind angebunden. Negative Typprüfungen, SDK-Quellmutationen und native
  Mathematik-/Einheitenfehler ergänzen die Referenzen. Das Sprachpendel stimmt
  über 800 Schritte und sechs Kanäle mit der symplektischen C-Variante überein;
  unabhängige Instanzen und Reset sind geprüft. Der Vakuumwurf stimmt mit der
  analytischen Lösung überein. Zwei echte Runnerdateien mit jeweils 201 Samples
  werden vollständig gelesen und gegen Referenzen geprüft, einschließlich
  Compiler-, Quell- und Modulprovenienz. Drei Fehlerexperimente prüfen zusätzlich
  vollständige Fehlernachrichten im interaktiven Protokoll und Fehlerstatus.
  Alle sechs Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer mit instrumentierten erzeugten Programmen/Modulen. Die
  vorhandenen Protokoll-, Szenen- und Runner-Isolationsprüfungen bestehen mit
  Clang ebenfalls. Die aus dem installierten SDK gebauten Module bestehen
  dieselben Referenzen mit dem installierten Runner; auch die beiden
  eigenständigen SDK-Sprachbeispiele laufen erfolgreich.
  Die Beispiele ersetzen noch nicht die vollständigen C-Vorlagen mit RK4,
  Luftwiderstand, Sensorrauschen und Flugbahnen. Analyse-ABI, weitere SDK-Funktionen
  und App-Sprachintegration bleiben offen; LANG-004 bis LANG-007 sind nicht abgenommen.

- Strukturtypen und Zustandswerte (2026-09-20): `struct` verwendet `:` mit
  eingerückten, explizit typisierten `let`-/`var`-Feldern. Parser, Typprüfung und
  Backend unterstützen Konstruktoren, vorwärts deklarierte Feldtypen, nominale
  Typidentität, verschachtelte Wertkopien, Feldzugriff/-zuweisung und Übergabe/Rückgabe
  von Strukturen. Negative Referenzen prüfen unbekannte/doppelte Felder, fehlende
  Initialisierung, Typverwechslungen, unveränderliche Zugriffspfade, temporäre
  Zuweisungsziele und zyklische beziehungsweise zu große/tiefe Layouts.
  Byte-Mutationen prüfen zusätzlich Bindungs- und Typreferenzgrenzen. Native
  Referenzen sichern Kopierunabhängigkeit, Konstruktor-Auswertungsreihenfolge und
  Laufzeitfehler bei globalem Feldzugriff vor Initialisierung ab. Alle fünf
  Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC AddressSanitizer
  einschließlich instrumentierter erzeugter Programme. `examples/language/motion.phys`
  berechnet einen Wurf mit konstantem Schwerefeld in 100 Schritten und besteht
  seine Referenz gegen die analytische Lösung; der Anfangszustand bleibt als
  unabhängiger Wert erhalten. Das Beispiel wurde auch aus dem installierten SDK
  mit automatisch gefundenem Compiler gebaut und ausgeführt. Die Experiment-ABI und
  App-Anbindung sind damit noch nicht implementiert.

- Natives skalares Sprachbackend (2026-09-20): `physimc --emit-c` und
  `physim_add_program` erzeugen nach statischer Prüfung echte C17-Programme.
  `language_native` baut und startet eine Referenz mit Seiteneffekten, benannten
  Argumenten, Kurzschlusslogik, gegenseitiger Rekursion, Schattenvariablen,
  Strings, Fließkommarechnung und Schleifen bis an beide Int64-Grenzen.
  70.000 Byte lange String- und Zahlenliterale werden zusätzlich nativ geprüft.
  14 Fehlerprogramme prüfen Überläufe, Nullteiler, nicht endliche Fließkommazahlen,
  globale Zugriffe vor Initialisierung, Rekursionstiefe und Assertions mit Status 70
  und ursprünglicher Quelldiagnose. Ein nachträglicher Typfehler stoppt den
  CMake-Build und erhält das vorherige vollständige generierte C. MSVC Release,
  Clang Debug und MSVC AddressSanitizer bestehen; ASan instrumentiert auch die
  erzeugten nativen Programme. Das Beispiel `examples/language` läuft im
  Entwicklungsbaum und aus einem installierten SDK ohne manuelle C-Bearbeitung
  mit den Ergebnissen `9`, `1`, `45`. Dies nimmt eigenständige skalare Programme ab,
  noch nicht die Experiment-/Analyse-ABI, App-Anbindung oder LANG-003 bis LANG-007
  insgesamt. Laufzeitvertrag und Grenzen: `docs/language.md`.

- Skalare Typprüfung und Compiler-CLI (2026-09-20): `language_checker` prüft
  `Int64`-/`Float64`-/`Bool`-/`String`-Typen, Literalinferenz, beide Int64-Grenzen,
  lexikalische Bindungsreferenzen, unveränderliche Werte, Vorwärtsaufrufe und
  gegenseitige Rekursion, benannte/positionale Argumente, Rückgabepfade und
  Schleifenkontrolle. Negative Referenzen prüfen gezielt semantische Fehler nach
  erfolgreichem Parsing. AST-Tiefe, Pufferkapazität und Arbeitsbudget werden
  absichtlich überschritten; alle Präfixe und Einzelbyte-Mutationen eines kleinen
  Moduls prüfen Diagnosegrenzen. `language_cli` prüft das tatsächliche Werkzeug
  `physimc --check` mit gültigem Beispiel, Syntax-/Typfehlern, leerer und zu großer
  Datei, fehlender Datei, Rückgabecodes sowie UTF-8-Pfaden mit Leerzeichen/Umlauten.
  Alle vier Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer. Der Compiler erzeugt noch keinen nativen Code; weitere
  Werttypen, Laufzeitregeln und LANG-003 bis LANG-007 bleiben offen.

- Einrückungssyntax (2026-09-20): Funktions-, Bedingungs- und Schleifenblöcke
  verwenden jetzt verbindlich `:` mit Zeilenumbruch und Einrückung statt `{}`.
  Parserreferenzen prüfen AST-Zuordnung bei mehreren Dedents und äußerem `else`,
  mehrzeilige Aufrufe, Kommentare, LF/CR/CRLF, Dateiende ohne letzten Zeilenumbruch,
  abweichende Blockbreiten, falsche Dedents, Tabs, leere/einzeilige Blöcke und
  abgewiesene Klammersyntax. Tief verschachtelte Einrückungsblöcke sowie sämtliche
  Präfixe/Einzelbyteersetzungen eines gültigen eingerückten Moduls prüfen weiterhin
  die Ressourcen- und Speichergrenzen. Lexer- und Parsertests bestehen unter
  MSVC Release, Clang Debug und MSVC AddressSanitizer. Typprüfung, Backend und
  App-Anbindung bleiben offene Schritte; dies ist keine Abnahme der gesamten Sprache.

- Sprachfrontend (2026-09-20): erster Lexer und CTest `language_lexer`.
  Referenzen für Schlüsselwörter, Funktions-/Typnotationssyntax, Zahlen,
  längste Operatoren, UTF-8-Strings, verschachtelte Kommentare, CR/LF/CRLF,
  stabile EOF-/Fehlerdiagnosen und explizit begrenzte Puffer. Sämtliche
  Bytepositionen eines Beispielquelltexts werden mit allen 256 Bytewerten
  verändert; zusätzlich werden alle gekürzten Präfixe auf Bereichsgrenzen und
  Fortschritt geprüft. MSVC Release, Clang Debug (Ninja) und MSVC
  AddressSanitizer bestehen. Die ClangCL-Integration in Visual Studio fehlt
  lokal; der vorhandene Clang-Treiber wird über Ninja verwendet.
  Diese erste Prüfung allein nimmt weder Parser noch Typsystem oder ausführbare
  Sprachprogramme ab. Die nachfolgende Parserprüfung wird separat dokumentiert.
  LANG-001 bleibt semantisch unvollständig. Details: `docs/language.md`.

- Optionales libFuzzer-Ziel (Dokumentationsabgleich 2026-09-20):
  `PHYSIM_BUILD_FUZZERS` und `physim-protocol-libfuzzer` existieren bereits.
  Die vorhandenen lokalen Buildprotokolle bestätigen das Linken; die
  Kampagnenprotokolle zeigen einen Windows-ASan-Startfehler statt einer
  erfolgreichen Kampagne. `docs/fuzzing.md` trennt jetzt diese offene
  Laufzeitabnahme von den deterministischen IPC-/Dateileser-Tests.

- Report-Mutationen (2026-09-19): 7.402 deterministische Fälle mit Linien-, Punkt-
  und Histogrammkurven, UTF-8 und Tabelle. Alle Kürzungen/Bitflips ohne CRC-Anpassung
  werden ausdrücklich abgewiesen; CRC-korrigierte Nutzdatenmutationen erreichen die
  Inhaltsprüfung. 2.320 akzeptierte Berichte lassen sich über die öffentliche API
  vollständig neu aufbauen. Jede der sechs Allokationen des gültigen Berichts wird
  separat zum Scheitern gebracht; Ausgabeerhalt und vollständige Freigaben bestehen.
  MSVC Release, Clang Debug und AddressSanitizer bestehen. Der neue CTest
  `report_mutations` und die Replay-Funktion sind in `docs/fuzzing.md` dokumentiert.
  Die Kampagne ergab keinen weiteren Leserfehler. Maximale Berichte und
  abdeckungsgeführte Langzeitkampagnen bleiben separate Prüfungen.

- Run-Datei-Mutationen (2026-09-19): 7.554 reproduzierbare Fälle mit vollständigen
  Kürzungen/Bitflips, extremen Chunklängen und Nutzdatenmutationen samt gültiger CRC.
  Ein NaN im letzten Kanal reproduzierte eine Teilausgabe aus `ps_run_next`.
  Messpunkte werden nun vollständig validiert, bevor Zeit und Werte übernommen
  werden; Fehler/EOF erhalten die Ausgabeparameter. MSVC Release, Clang Debug und
  AddressSanitizer bestehen alle Mutationen (2.059 erfolgreiche Opens, 3.858 gültige
  Messpunkte). Bestehende Core-/Reihentests bestehen unter MSVC. CTest, Reproduktion
  und Grenzen sind dokumentiert. Formatversion und Verhalten gültiger Dateien bleiben
  erhalten. Report-Mutationen und abdeckungsgeführte Langzeitkampagnen bleiben offen.

- IPC-Mutationskampagne (2026-09-19): neuer CTest `protocol_mutations` mit 183.462
  reproduzierbaren Snapshot-/Frame-Eingaben, vollständigen Kürzungen und Bitflips,
  Vierbyte-Mutationen und festen Extremwerten. Der Verbrauchspfad prüft Längen vor
  der Headeraddition und verhindert deren Überlauf; `ps_wire_peek` verändert
  Ausgaben nur bei einem vollständigen Frame. Invarianten prüfen Fehleratomizität,
  gültige dekodierte Daten, kanonischen Roundtrip, Restbytes und Sequenzen.
  MSVC Release, Clang Debug und AddressSanitizer bestehen; vorhandene Szenen- und
  Runner-Isolationsreferenzen bestehen ebenfalls unter MSVC. Reproduktion und
  Grenzen sind in `docs/fuzzing.md` beschrieben. Strukturierte Dateileser-Kampagnen
  und abdeckungsgeführtes Langzeit-Fuzzing bleiben offen.

- UTF-8-Zwischenablage (2026-09-19): Ein reproduzierter Fehler im vendorten
  Nuklear-Pastepfad vermischte Byte- und Zeichenzahlen. Der lokale Patch verwendet
  UTF-8-Bytes für den Text und Unicode-Skalare für Cursor/Undo; eine ersetzte Auswahl
  wird mit einem Undo-Schritt wiederhergestellt. Platzreservierung erfolgt vor dem
  Löschen, sodass Puffer-/Allocator-Fehler Text, Auswahl und Undo erhalten.
  Ungültiges UTF-8 wird abgewiesen, SDL begrenzt Einfügungen auf 262144 Bytes.
  Referenzen mit Umlauten, Eurozeichen, Emoji, Zeilenumbruch, Undo/Redo, Ersetzung
  und Fehler-Injektion bestehen unter MSVC Release, Clang Debug und AddressSanitizer.
  Der SDL-Eingabe-/Renderer-Test und der vollständige Pendel-App-Ablauf bestehen
  unter Windows. Tests verändern die Systemzwischenablage nicht. Der Patch samt
  Prüfsumme ist in `third_party/README.md` dokumentiert; Undo-Historienlimits bleiben.

- Gemischter Constraint-Graph (2026-09-19): `ps_constraints_resolve_graph`
  iteriert Kontakte und bis zu 256 Distanzgelenke gemeinsam bei weiterhin maximal
  128 Körpern und 512 Kontaktpunkten. Restitution verwendet Anfangsgeschwindigkeiten;
  die Restfehler werden am gemeinsamen Endzustand ausgewertet. Gekoppelte
  Bodenabstützung und Rückprall, knappe Iterationsbudgets, widersprüchliche Gelenke,
  Kontaktprojektion, gleichzeitig ausgeschöpfte Kapazitäten und Fehleratomizität
  bestehen unter MSVC Release, Clang Debug und AddressSanitizer sowie gegen das
  installierte SDK. Ohne Gelenke ist das Ergebnis bitgleich zur bisherigen API.
  Direkte gemeinsame Positions-/Orientierungsprojektion und Warmstart bleiben offen.

- Distanzgelenk (2026-09-19): `ps_distance_joint_resolve` ergänzt eine bilaterale
  Geschwindigkeitsbedingung mit körperfesten Ankern, Drehimpulsen, statischer Welt
  und einstellbarer Driftkorrektur. Impuls, Längenfehler und Geschwindigkeitsrestfehler
  sind abrufbar. Analytische Impuls-/Drehmomentreferenzen, Kreisbewegung mit
  Zeitschrittverfeinerung, statische Partner und Fehleratomizität bestehen unter
  MSVC Release, Clang Debug und AddressSanitizer sowie separat gegen das installierte
  SDK. Gemeinsame Iteration mit anderen Gelenken/Kontakten und automatische
  Integrationssteuerung bleiben offen; Stabilisierung kann Energie zuführen.

- Kontaktgraph (2026-09-19): `ps_contacts_resolve_graph` löst bis zu 128 Körper
  und 512 Kontaktpunkte gemeinsam, einschließlich statischer Weltkontakte,
  Restitution, Coulomb-Reibung und iterativer Positionskorrektur. Impulse und
  verbleibende Normalgeschwindigkeits-/Projektionsfehler sind abrufbar.
  Kontaktkette, gestützter Dreier-Stapel, Reihenfolgevergleich, unzureichende
  Iterationszahl, volle Kapazität und Fehleratomizität bestehen unter MSVC Release,
  Clang Debug und AddressSanitizer. Der Paarvergleich ist bei Reibung bitgleich;
  bestehende Boxstoß- und Bodenkontaktreferenzen bestehen unter MSVC Release.
  Der Graph-Test besteht separat gegen das installierte SDK. Kontakte erzeugt
  weiterhin der Aufrufer; Warmstart, Gelenke und automatische Verwaltung bleiben offen.

- CCD-Stoßvorlage (2026-09-19): neue Kugelstoßprojekte verwenden im Vakuum
  standardmäßig den Sweep bis zum Kontakt, Impulsantwort und Restbewegung.
  Geschwindigkeit und Verfahren stehen in den Metadaten. Medien mit Widerstand
  bleiben diskret; explizites CCD mit einem Medium wird abgewiesen.
  Runner-Referenzen mit 100 m/s prüfen jeden Messpunkt bei 50-ms-/3-ms-Schritten,
  korrekte Endposition, Energie und einen Impuls; der diskrete Vergleich tunnelt
  erwartungsgemäß. MSVC Release und Clang Debug bestehen, ebenso die bisherigen
  Medien-/Reibungsreferenzen. Der vollständige App-Ablauf mit neuem Projekt,
  Build, Simulation, Analyse und erneutem Laden besteht unter Windows.
- Kontinuierliche Kugelkontakte (2026-09-19): `ps_sweep_spheres` und
  `ps_sweep_sphere_plane` bestimmen den ersten Kontakt entlang linearer
  Verschiebungen. `ps_aabb_swept_sphere` deckt den gesamten Weg für die Broad
  Phase ab. Durchtunneln, Streifkontakt, Anfangsüberlappung, bewegte Partner,
  Intervallgrenzen, lange Wege/kleine Ziele und Fehleratomizität bestehen mit
  MSVC Release, Clang Debug und AddressSanitizer. Eine separate MSVC-Referenz
  prüft den elastischen Rückprall samt Restzeit und Energie. Der vollständige
  Sweep-Test besteht auch separat gegen das installierte SDK. Beschleunigte
  Bahnen, Box-CCD und eine automatische Mehrkörper-Ereignissteuerung bleiben offen.
- Broad Phase (2026-09-19): `physim/collision.h` erzeugt konservative Hüllboxen
  für Kugeln/orientierte Boxen und deterministisch sortierte Kandidatenpaare.
  Feste Arbeitspuffer, bis zu 1024 Hüllboxen, keine Allokation; zu kleine
  Ausgabepuffer liefern die benötigte Paarzahl ohne Teilausgabe. Vergleiche mit
  vollständigen Paarprüfungen, 523.776 dicht gepackte Paare, rotierte Eckpunkte,
  Kapazitäts-/Fehlerfälle und Boxkontakt-Abdeckung bestehen unter MSVC Release,
  Clang Debug und AddressSanitizer. Die API ist im Header-Browser verfügbar.
  Der SDK-Neubau mit verschobenem Installationspfad besteht für alle acht
  Vorlagen; der separat gebaute Prüfer ruft die neue Broad-Phase-API auf.
  Der Dokumentations-Fenstertest lädt alle Themen einschließlich des neuen Headers.
  Exakte Kontakte, Kollisionsfilter, bewegte Hüllvolumen und Mehrkörperlösung
  bleiben separate Schritte.
- Szenentastatur (2026-09-19): Alt-Kürzel für Orbit, Pan, Zoom, Standardkamera,
  zyklische Auswahl eingeblendeter Einträge und Ausblenden der Auswahl. Die
  Auswahl überspringt Formfilter, leere Labels und Alpha 0 und folgt Objekt-IDs
  beim Umsortieren. SDL-Ereignistests prüfen Aktionen, Zoom-/Neigungsgrenzen,
  Tastenwiederholung, leere Szenen und die Abgrenzung zu Editor, Analyse und
  Ctrl+Alt. MSVC Release und Clang Debug bestehen den gesamten Maus-/Tastaturablauf
  auch mit eingestreuten Desktop-Ereignissen. Auswahl und Kameradarstellung wurden
  visuell geprüft. Der Ablauf besteht auch aus der neuen portablen Installation.
  Vollständige Tastaturbedienung der App bleibt offen.
- Installations-/SDK-Prüfung (2026-09-19): `tools/verify-sdk.cmake` installiert in
  einen neuen Pfad mit Leerzeichen/Umlaut, verschiebt das SDK und baut alle acht
  Vorlagen sowie die Analyse aus den installierten Quellen. Alle öffentlichen
  Header werden einzeln kompiliert. Installierte Runner erzeugen je 201 Samples
  und einen Bericht; ein separat gebauter Leser prüft Zeitraster, endliche Werte,
  vollständigen Laufabschluss und lesbaren Bericht. MSVC Release und Clang Debug
  bestehen. Die CI führt diese Prüfung nun für Windows/Linux aus und archiviert
  Protokolle; der Linux-App-Test umfasst auch die Auftriebsvorlage.
  Lokal ist keine WSL-Distribution oder Docker-Laufzeit verfügbar. Eine tatsächlich
  ausgeführte Linux-Abnahme und ein Test auf sauberem Zielsystem bleiben offen.
- RK45-Diagnosen (2026-09-19): additive Schnittstelle
  `ps_ode_integrate_diagnosed` meldet Abbruchursache, Zeitpunkt, Komponente und
  Stufe in einem Objekt des Aufrufers. Die bisherige Schnittstelle und ihre
  Fehleratomizität bleiben erhalten. Alle Diagnosezweige sowie numerische
  Referenzen bestehen unter MSVC Release, Clang Debug und AddressSanitizer.
  Der separat gegen das installierte SDK gebaute Numeriktest und die
  Pendel-Referenzläufe mit RK4, RK45 und Verlet bestehen ebenfalls.
  Strukturierte Diagnosen für andere Bibliotheksbereiche bleiben offen.
- Gemeinsame Zeilenauswahl (2026-09-19): `ps_series_select` filtert 1–32
  ausgerichtete Reihen über einen dimensionslosen Selektor, zum Beispiel
  Sensorstatus 1. Zeit und Messwerte erhalten gemeinsam eine neue Samplezuordnung;
  separate Auswahlen werden nicht versehentlich kombiniert. Die Standardanalyse
  nutzt die API für gültige Sensorpunkte. Blockgrenzen, Statistik/Ableitung über
  Messlücken, Resampling-Zuordnung, CSV, leere/vollständige Auswahlen, 32 Spalten,
  Lebensdauer, Scratch-Limit und Handle-Limit bestehen mit MSVC Release und
  Clang Debug. Die Sensor-Referenz prüft die tatsächlichen Berichtspunkte,
  einschließlich Ausdünnung über 2.048 Punkte, Totalausfall und Einzelmessung.
  Reihen-, Resampling- und Auswahltests bestehen unter AddressSanitizer; der
  Auswahltest wurde separat gegen das installierte SDK gebaut und ausgeführt.
  Der Sensorbericht besteht mit Runner und Analysemodul der portablen Ausgabe.
  Die Auswahl entfernt Zeilen; explizite Gültigkeitsmasken bei unveränderter
  Zeilenanzahl bleiben offen. Ableitung/Integral verbinden behaltene Punkte.
- Szenentransparenz (2026-09-19): RRGGBBAA-Alpha wird für Meshes ausgewertet;
  opake Geometrie schreibt Tiefe, transparente Dreiecke werden von hinten nach
  vorn gemischt. Alpha 0 zeichnet keine Geometrie/Beschriftung und lässt sich
  nicht anklicken. GPU-Pixelprüfungen für Mischwerte, zwei transparente Ebenen,
  vertauschte Objektliste, opake Verdeckung und Auswahl bestehen in Perspektive
  und Orthografie unter MSVC Release und Clang Debug. Beide Builds bestehen
  außerdem Diagrammbedienung und UI-Rendering. Halbtransparente Box und Kugel
  wurden visuell geprüft. Der Renderer-Test besteht auch aus der frisch
  installierten portablen Ausgabe. Sich durchdringende Flächen können Sortierartefakte
  zeigen; eine ordnungsunabhängige Transparenz ist noch nicht implementiert.
- Szenenauswahl (2026-09-19): Linksklick trifft die vorderste tatsächlich
  gezeichnete Dreiecksgeometrie; Beschriftungen werden über ihre Textfläche
  gewählt. Inspektoranzeige und Ausblenden der Auswahl folgen vorhandenen IDs.
  Sieben Mesh-Grundformen in perspektivischer/orthografischer Ansicht,
  Tiefenverdeckung, Hintergrund, ausgeblendete Vektoren und ungültige Koordinaten
  bestanden im GPU-Test unter MSVC Release und Clang Debug. Der Maustest prüft
  Kugel, Beschriftung, Hintergrund und Umsortieren; die Bedienung wurde visuell
  geprüft. Clang bestand auch den Ablauf mit eingestreuten Desktop-Ereignissen.
  Der zusätzliche Fall „umsortieren, dann Auswahl ausblenden“ und der vollständige
  Ablauf aus der portablen Installation bestanden unter MSVC Release.
- IDs in Vorlagen (2026-09-19): alle acht Standardexperimente vergeben feste
  IDs nach Objektbedeutung, auch bei bedingt sichtbaren Sensoren und Pfaden.
  Kurzlebige Kontaktpunkte/-impulse bleiben anonym. Neue SDK-Konstruktoren
  `ps_scene_add_id`, `ps_scene_polyline_id` und `ps_scene_label_id` prüfen
  ID-Duplikate ohne Teiländerung der Szene oder ihres Punktpuffers.
  481 Szenen je Modul (3.848 insgesamt) bestehen mit MSVC Release und Clang
  Debug, einschließlich tatsächlicher Positionsänderungen in der Objektliste.
  Zehn physikalische Referenz-/Vorbereitungstests bestehen weiterhin.
  Der Szenentest besteht unter AddressSanitizer. Der ID-Test bestand erneut
  mit allen acht Modulen aus der frisch installierten portablen Ausgabe.
- Objekt-IDs / ABI 3 (2026-09-19): optionales `ps_object.id`, eindeutige nichtnull
  IDs pro Szene und explizite Übertragung in IPC 3 (172 Bytes pro Objekt).
  Die Sichtbarkeit folgt vorhandenen IDs bei Reihenfolge- und Formänderungen;
  entfernte IDs werden vergessen, ein neuer Lauf setzt die Auswahl zurück.
  Anonyme Einträge behalten die Positionszuordnung. Snapshot-Rundlauf,
  doppelte IDs, Mausklicks und Umsortieren bestanden unter MSVC Release und
  Clang Debug. Der sichtbare Szenenausschnitt bleibt beim Umsortieren pixelgleich.
  Runner- und Analyseprüfungen weisen ABI-2-Module ab; Pendellauf und Analyse
  bestehen mit neu gebauten ABI-3-Modulen. `.psrun` und `.psreport` bleiben
  unverändert. Vorhandene Experiment- und Analysemodule müssen neu gebaut werden.
  Alle 51 Tests ohne Display bestanden nach dem Versionswechsel. Die beiden
  Szenentests bestanden unter AddressSanitizer; der vollständige Export- und
  Sichtbarkeitsablauf bestand auch aus der frischen portablen Installation.
- Szeneneinträge (2026-09-19): einzelne Listenpositionen ausblenden und wieder
  einblenden, zusätzlich zu den vorhandenen Formfiltern. Bewegung erhält die
  Auswahl; neuer Lauf, geänderte Anzahl oder Formenfolge setzen sie zurück.
  Randpositionen 0/31, Bewegung und Strukturwechsel bestanden im Komponententest;
  Mausklicks bestanden unter MSVC Release sowie Clang Debug, auch mit Störereignissen.
  Ein unabhängiger Bildvergleich findet nach dem Ausblenden keine roten Kugelpixel
  mehr (vorher 637), bei unverändert 810 blauen Kugelpixeln. Die Bedienung wurde
  visuell geprüft. Dauerhafte Objekt-IDs sind weiterhin offen.
- Diagramm-Testeingang (2026-09-19): fremde Maus- und Fokusereignisse können
  künstliche Zoom-/Drag-Ereignisse überlagern. Ein reproduzierbarer Störlauf
  scheiterte vor der Korrektur in Stufe 2 und bestand danach vollständig.
  Ausschließlich im Plot-Testmodus werden eigene markierte Mausereignisse
  verarbeitet; die normale App behält ihre Eingabebehandlung. Neuer CTest
  `plot_input_isolation` streut Mausbewegungen, Loslassen und Fokusverlust ein.
  Normaler und gestörter Ablauf bestanden jeweils dreimal unter MSVC Release
  und Clang Debug. Dies belegt den behobenen Störpfad, nicht jede mögliche
  Ursache früherer sporadischer UI-Testfehler.
- Resampling-Verfahren (2026-09-19): `ps_series_resample` bietet zusätzlich zur
  linearen Interpolation Nearest (bei Gleichstand früherer Punkt) und Previous
  (letzten Wert halten). Alle Verfahren prüfen den vollständigen Quellbereich,
  extrapolieren nicht und liefern Reihen mit der Zuordnung des Zieldatensatzes.
  Referenzen mit 3.073 Zielpunkten, Blockgrenzen, exakten Quellpunkten,
  Gleichständen, Extremwerten, Speicherquoten und Datensatz-Lebensdauer bestanden
  mit MSVC Release, Clang Debug und AddressSanitizer; die lineare API bleibt
  kompatibel. Derselbe Resampling-Test wurde separat gegen die installierten
  SDK-Header und Bibliothek kompiliert und erfolgreich ausgeführt.
- SVG-Ausschnitt (2026-09-19): eigener Knopf und SDK-Funktion mit expliziten
  Achsengrenzen. SVG-Clip-Pfade begrenzen Kurven, ohne Titel und Legende abzuschneiden;
  Achsenoffsets erhalten lesbare Teilstriche bei großen Grundwerten. Berichtstest
  und vollständiger Mausklick-/Exportablauf bestanden unter MSVC Release und Clang
  Debug; der Berichtstest bestand auch unter AddressSanitizer. Unabhängige
  XML-Prüfungen bestätigen Referenzgeometrie, Clipping, Legende und Achsenwerte.
  Auch der Exportablauf aus der frischen portablen Installation bestand.
  Die Bedienung wurde im App-Bild geprüft. Eine visuelle Prüfung der SVG-Datei
  im Browser blieb wegen fehlgeschlagener Browser-Verbindung aus.
- PNG-Ausschnitt (2026-09-19): eigener Exportknopf für die aktuellen Achsengrenzen,
  mit Titel, Einheiten, Legende und wählbarer Auflösung. Mausklick und PNG-Ausgabe
  bestanden unter MSVC Release sowie Clang Debug. Clang scheiterte zunächst in
  Zoom-Stufe 34; der isolierte Lauf nach dem letzten Build bestand. Unabhängige
  Pixelprüfung bestätigt eine durchquerende Linie mit beiden Endpunkten außerhalb,
  einen an allen Grenzen beschnittenen Balken und ausgeschlossene Streupunkte.
  Ungültige Grenzen erzeugen keine Datei. Fünf vollständige Standardexporte bleiben
  pixelgleich; die neue Bedienung wurde visuell geprüft. Der Plot-Workflow bestand
  auch aus der frischen portablen Installation. SVG-Ausschnitte sind offen.
- PNG-Kompression (2026-09-19): zeilenweises DEFLATE mit statisch eingebundener
  zlib 1.3.2, ohne zusätzliche Codec-DLL. Neun Exporte einschließlich aller vier
  Auflösungen sind nach unabhängiger Pillow-Dekodierung pixelgleich zur vorherigen
  Version. Die fünf Standarddiagramme schrumpfen von je 12.270.651 Byte auf
  71.791–227.240 Byte; das größte Testbild von 49.021.251 auf 622.412 Byte.
  CRC, Zlib-Datenstrom, Randgrößen, Zeilenpadding und Zufallsbilddaten wurden
  unabhängig mit Python geprüft. MSVC Release: PNG, Decoder, UI-Rendering und
  Plot-Workflow bestanden. Clang Debug: PNG, Decoder und UI-Rendering bestanden;
  Plot-Workflow scheiterte zunächst in Zoom-Stufe 5 und bestand isoliert erneut.
  PNG-Test unter AddressSanitizer und Plot-Workflow der frischen portablen
  Installation bestanden. Linux wurde lokal nicht ausgeführt.
- PNG-Größen (2026-09-19): Auswahl von 1200 × 850, 2400 × 1700 (Standard),
  3600 × 2550 und 4800 × 3400 Pixeln direkt am Diagrammexport. Logisches Layout
  und vollständiger Diagramminhalt bleiben erhalten; fünf Standardexporte sind
  bytegleich zur vorherigen Version. Pillow dekodiert alle vier Größen mit den
  erwarteten Maßen. Mausklicks auf die Auswahl, Exporte, ungültige Skalen und
  Größenlimits sowie fortgesetztes UI-Rendering bestanden unter MSVC Release
  und Clang Debug. Auswahl und Export wurden visuell geprüft. Derselbe
  Plot-Workflow bestand auch aus der portablen Installation.
- Hashmap (2026-09-19): neuer SDK-Baustein mit eigenen Byte-Schlüsseln,
  Werten fester Größe, explizitem Allocator und Schlüssel-/Eintragslimits.
  5.000 Referenzoperationen, 64 Schlüssel im selben Bucket, Löschungen,
  Traversierungsschutz und Allokationsfehler bestanden unter MSVC Release,
  Clang Debug und AddressSanitizer. Fünf gezielte Prüfungen einschließlich des
  Dokumentationsfensters bestanden im separaten Release-Build `build-hashmap-release`.
  Dieser Build umgeht die vom laufenden bisherigen Physim-Fenster gesperrte EXE.
  Das Beispiel wurde mit dem SDK-CMake-Modul aus portablen Quellen gebaut und
  für vorhandene/fehlende Schlüssel samt unveränderter Fehlerausgabe geprüft.
  [Verträge und Beispiel](hashmap.md).
- String-Views (2026-09-19): neuer SDK-Baustein für begrenzte Text-/Bytebereiche
  ohne Allokation, mit Vergleich, Suche, Teilbereichen, Trennen, ASCII-Trimmen
  und überlappendem Kopieren. 5.000 Suchreferenzen und alle 65.536 Bytepaar-
  Vergleiche bestanden unter MSVC Release, Clang Debug und AddressSanitizer.
  Drei gezielte Release-Prüfungen einschließlich Dokumentationsfenster bestanden.
  UTF-8 wird byteweise behandelt; Speicherlebensdauer bleibt beim Aufrufer.
  Das Parserbeispiel wurde mit dem SDK-CMake-Modul aus portablen Quellen gebaut
  und mit einem nichtterminierten Puffer sowie unveränderten Fehlerausgaben geprüft.
  [Verträge und Parserbeispiel](string-view.md).
- Dynamische Arrays (2026-09-19): neuer SDK-Baustein mit explizitem Allocator,
  Elementlimit, geometrischem Wachstum, Selbstkopien und unveränderten Daten bei
  Fehlern. 5.000 Referenzoperationen, Fehler-Injektion, Bytebudget, Größenüberlauf
  und benachbarte Arenablöcke bestanden unter MSVC Release, Clang Debug und
  AddressSanitizer. Vier gezielte Release-Prüfungen einschließlich Speicher-API
  und Dokumentationsfenster bestanden. Das Dokumentationsbeispiel wurde mit dem
  SDK-CMake-Modul aus portablen Quellen gebaut und ausgeführt. [Anleitung](array.md).
- Räumliche Kurven (2026-09-19): kubische Bézierauswertung mit Position und
  Parametertangente sowie geometrische Unterteilung ohne Heap-Allokation.
  Analytische Polynomreferenz, 1.000 deterministische Unterteilungen, skalierte
  Tangenten, Endpunkte, Aliasierung und Zahlenextreme sind geprüft. Der
  Mathematiktest bestand mit MSVC Release, Clang Debug und AddressSanitizer.
  Das dokumentierte Szenenbeispiel wurde gegen die portable SDK-Installation
  mit Warnungen als Fehler gebaut und als gültige 33-Punkte-Polyline geprüft.
  [Konventionen und Szenenbeispiel](math.md).
- Auftriebsvorlage (2026-09-19): achte App-Vorlage mit Wasseroberfläche,
  Kraftpfeilen, zwölf Kanälen und vier Analyseplots. Die gemeinsame Analyse
  unterstützt nun auch reine Y-Bewegung. 6086 Runner-Messpunkte prüfen
  gedämpftes Schwimmen, Gleichgewicht, neutrales Schweben, Sinken/Steigen,
  Energiebilanz und RK4-Verfeinerung (Fehlerverhältnis 15,80). Elf gezielte
  Release-Tests bestanden, außerdem die Referenz unter Clang und AddressSanitizer.
  App-Workflow und Dokumentationsfenster bestanden; Simulations- und
  Energieansicht wurden visuell geprüft. Derselbe Workflow bestand aus der
  portablen Installation mit neu gebautem SDK-Projekt. [Anleitung](buoyancy.md).
- Auftrieb (2026-09-19): neue SDK-Funktionen für archimedische Kraft und die
  teilweise eingetauchte Kugel mit Volumen/Schwerpunkt. Unabhängige Quadratur
  über 101 Eintauchtiefen, Schweben, Sinken/Steigen, rückstellende Kraft und
  numerische Fehlerverträge sind geprüft. Elf gezielte Release-Prüfungen
  einschließlich bestehender Mechanikexperimente und Dokumentation bestanden;
  Auftriebs-/Mechaniktests zusätzlich mit Clang Debug und AddressSanitizer.
  Nutzung und Modellgrenzen stehen in [mechanics.md](mechanics.md).
- CRC32 (2026-09-19, PERF-009): unveränderliche 256-Einträge-Tabelle ersetzt
  die bitweise Berechnung. 31 unabhängige zlib-Referenzen bis 8 MiB, Ausrichtung,
  Einzelbytes, Bitfehler und ein historischer Bericht mit bytegleichem Roundtrip
  sind geprüft. 52 Release-Tests, sieben Clang-Tests und beide CRC-Tests unter
  AddressSanitizer bestanden. Der portable SDK-/App-Selbsttest mit Sensorvorlage,
  Build, Simulation, Analyse, Export und erneutem Öffnen bestand ebenfalls.
  Schema/Samples/Abschluss des Referenzlaufs und
  Exporte bleiben unverändert. Lokal etwa 4,6–5,4-facher CRC-Durchsatz und
  58 Prozent kürzere Lesezeit im 16-Kanal-Benchmark; Flush-Regeln bleiben gleich.
  [Nachweise und Grenzen](crc.md).

- UI-Zeichenpuffer (2026-09-19): Vertex-/Indexspeicher bleibt am Grafik-Kontext
  erhalten; kontrolliertes Wachstum und OpenGL-3.3-Orphaning sind implementiert.
  Referenztests vergleichen sämtliche Geometriedaten und Zeichenbefehle gegen
  den vorherigen Konvertierungspfad. Erschöpfung und alle vier Allokationsfehler
  des Referenzablaufs sind einschließlich Wiederverwendung und Freigabe geprüft,
  auch mit MSVC AddressSanitizer. Neun relevante Release-Prüfungen bestehen,
  darunter die acht Fenster-/Grafiktests. Vier gezielte Clang-Debug-Prüfungen
  (Konvertierung, UI-Messung, Plot-Ablauf und Renderer) bestehen ebenfalls.
  Der portable Boxstoß-App-Test baut neue SDK-Module und besteht Simulation,
  Analyse, Export und Wiederöffnung; die Szene wurde visuell geprüft. Der
  reguläre Build unter `build/bin` ist aktualisiert und hat Konvertierungs-,
  UI-Render- und Fenstertest bestanden. Alle sechs Benchmark-Bilder sowie die
  fünf PNG-Exporte sind bytegleich zum bisherigen Build; elf Diagrammbereiche
  sind pixelgleich. Die neue Messstrecke protokolliert CPU-Median/P95/P99,
  Uploadbytes und Pufferallokationen: nach Aufwärmen null statt 2/14/19 je Bild;
  Konvertierungszeit in dichter UI und achtkurvigem Plot lokal etwa 36 Prozent
  geringer. Details und Grenzen: [UI-Rendering](ui-rendering.md).
  PERF-005 ist für Konvertierungspuffer und GPU-Upload umgesetzt; Messung der
  gesamten App-Allokationen sowie vollständige Frame-/Eingabelatenz bleiben offen.

- Leistungsmessung und Berichtsanzeige (2026-09-19): optionaler nativer Benchmark
  für 16-Kanal-Runs, vollständiges Rücklesen, Analyse-Snapshots, Statistik,
  Ableitung und acht Berichtskurven. Rohdaten, Build-/Hostdaten und Fingerabdrücke
  werden archiviert; ein optionaler Vergleich prüft eine 20-Prozent-Schwelle.
  Die Oberfläche verwendet jetzt unveränderliche Kurven-Views statt zweier
  Vollkopien pro Kurve und Frame. Kopier-API, Datenformat und ABI bleiben erhalten.
  Identische Inhalte, Fehlerfälle und Lebensdauer geprüft; Berichtstest auch mit
  AddressSanitizer bestanden. 47 MSVC-Release-Tests einschließlich sieben
  Fensterprüfungen plus der anschließend ergänzte Python-Wrapper-Test bestanden.
  Clang-Debug: 45 von 46 Tests im ersten Gesamtlauf, der Batch-Test mit
  250-ms-Runnerlimit bestand anschließend ohne parallele Last; beide neuen
  Benchmark-Tests bestanden danach ebenfalls. Kein neuer Linux-Lauf.
  Der portable Pendelablauf baut neue SDK-Module und besteht Simulation,
  Pause, Analyse, Exporte und erneutes Öffnen. Fünf PNG-Exporte sind bytegleich
  zum vorherigen Build; die Diagrammbereiche von elf Plot-Screenshots sind
  pixelgleich (Zoom, Pan, Histogramm, Punkte, Sensor und Wiederöffnung).
  Messwerte, Abdeckungsgrenzen und Wiederholungsanleitung:
  [Leistungsmessung](performance.md). PERF-001 bleibt teilweise offen;
  PERF-003 ist für den Anzeigezugriff umgesetzt.

- Speicherverwaltung: Überlauf, Resize-Fehler, bytegenaue Freigaben, Ausrichtung,
  Arena-Erschöpfung und Reset geprüft. Fehler an allen acht Allokationsstellen
  des Berichtablaufs und sieben Stellen beim Laden hinterlassen keine lebenden
  Testblöcke. Fehlgeschlagene Änderungen erhalten den Bericht bytegleich.
  Alle 670 abgeschnittenen Präfixe einer Referenzdatei, SVG-/CSV-Exportfehler,
  echte Datenreihen und Arena-Besitzer geprüft. Beide neuen Tests bestehen auch
  mit MSVC AddressSanitizer und als eigenständige C17-Consumer gegen installierte
  SDK-Header und Bibliothek. Der portable Sensor-Wurf-App-Test baut neue Module
  einschließlich der Speicherverwaltung und prüft Simulation, Analyse, Export und
  erneutes Öffnen. Der F1-Test lädt das neue Thema und `memory.h`.

- Mathematik: Vec2/3/4, subnormale und maximale Komponenten, normalisierte
  Quaternionen, Aliasierung und transaktionale Fehler geprüft. Je 2000 Fälle
  prüfen allgemeine Matrixinversion mit beidseitigen Residuen, Rotationen gegen
  unabhängige Rodrigues-Referenzen sowie Transformationen mit Scherung/Spiegelung.
  Die neue Quaternion-Normalisierung wird auch im Renderer verwendet.
  Ein eigenständiger C17-Consumer gegen die installierten Header und die statische
  Bibliothek besteht dieselben Referenztests. Der portable App-Ablauf baut die
  Boxstoßvorlage mit den ausgelieferten SDK-Quellen einschließlich `math.c`, prüft
  Simulation, Analyse, Exporte und erneutes Öffnen. Der F1-Test lädt auch das neue
  Mathematikthema und `math.h`. Die sieben Fensterprüfungen sind als `display`
  markiert; die Linux-CI startet sie unter Xvfb/Openbox. Tatsächliche Ausführung
  unter Linux bleibt offen, da lokal keine WSL-Distribution vorhanden ist.

- Einstellungen: Dateiformat, Bereichsprüfung, Prüfsumme, jede abgeschnittene
  Dateilänge und Erhaltung bestehender Daten bei Fehlern geprüft. Sieben App-Starts
  im eigenen Testordner prüfen Schriftwahl, Darstellungsschalter, Übernehmen,
  Abbrechen, Standardwerte, Mausziehen beider Panelgrenzen und Wiederherstellung.
  Normale Fenstermaße und maximierter Zustand bleiben erhalten. Ein künstlich
  vorgerückter App-Timer schreibt vor Ablauf des gewählten Intervalls keine Datei,
  danach eine echte Autosave-Datei und plant die nächste Sicherung korrekt.
  Beschädigte Einstellungen bleiben beim Beenden bytegleich erhalten.
  Die Oberfläche und der vergrößerte Code-Editor wurden visuell geprüft;
  das Einstellungsformular bleibt bei 1080 × 740 bedienbar.
  Dieselben sieben Neustarts und der Dokumentationsfenstertest bestanden aus
  dem portablen Paket. Ein dort neu gebautes Boxstoß-Projekt bestand den
  vollständigen App-Ablauf einschließlich Analyse, Export und erneutem Öffnen.

- Box–Box: 12000 deterministische Paare gegen unabhängige Eckpunktprojektionen
  geprüft, darunter 363 Trennungen, die erst eine Kantenkreuzachse erkennt. 6245
  überlappende Paare liefern gültige Oberflächenpunkte, normierte Richtungen,
  unveränderte Ergebnisse bei Wiederholung und dieselben Kontakte mit umgekehrten
  Normalen bei Vertauschung von A/B. Fläche, Kante, Ecke, achtpunktige Fläche,
  Einschluss, nahezu parallele Boxen, Größen von 1e-150 bis 1e150 und transaktionale
  Eingabefehler geprüft. Zufällige Geschwindigkeiten/Rotationen bestätigen Gesamtimpuls
  und Drehimpuls bei reiner Impulsantwort; inelastische Reibungsstöße erhöhen die
  kinetische Energie nicht. Drei echte Runner-Läufe mit jeweils 401 Samples prüfen
  elastischen Stoß (0,36 J), Restitution 0,5 (0,09 J) sowie gedrehten, versetzten Stoß
  mit Reibung (Endenergie 0,171159938192 J). Alle Messpunkte und Metadaten geprüft.
  Der neue App-Ablauf baut die Boxstoßvorlage, prüft vier Kontaktpunkte und zwei Boxen
  im Szenensnapshot, Pause/Step/Stop, Analyse, Export, Laufvergleich und erneutes Öffnen.
  Derselbe Ablauf besteht aus dem portablen Paket mit frisch aus dessen SDK
  gebauten Modulen. Simulation und Analyse wurden bei 1080 × 740 visuell geprüft.

- MSVC 19.38: Release einschließlich 46 CTest-Prüfungen bestanden; Debug vor dem Rendererwechsel geprüft.
- Clang-Cl 19.1.5 mit Ninja: Debug-Build und dieselben 46 CTest-Prüfungen bestanden.
- PNG: unabhängige Dekodierung mit Python-zlib, CRC-Prüfung und Pillow; exakte
  Referenzpixel einschließlich Zeilenpadding, 1 × 1, 8192 × 1 und 1 × 8192 geprüft.
  Der App-Test klickt den PNG-Export und vergleicht den Export aus einer gezoomten
  Ansicht bytegenau mit dem vollständigen Diagramm. Erneutes Exportieren auf einen
  existierenden Pfad wird abgewiesen. Linien, Punkte, Histogramme, große Achsenoffsets
  und acht Kurven mit insgesamt 16384 Punkten werden exportiert. Histogrammhöhen,
  Orientierung, alle acht Legendenmarker und Reduktionshinweis sind geprüft;
  Beschriftungen, lange Legenden und die weiterhin bedienbare App wurden visuell geprüft.
  PNG-App-Test und interner Dokumentationsbrowser bestehen auch im frisch installierten
  portablen Paket. Dessen vollständiger App-Test baut die Unsicherheitsvorlage aus dem
  mitgelieferten SDK und prüft Simulation, Analyse, Export, Laufvergleich und erneutes Öffnen.
- Diagrammbedienung: Zoom um einen vorgegebenen Anker, Verschieben an die Grenzen,
  minimale Ausschnittgröße, extreme Achsenwerte und Linien-Clipping mit numerischen
  Referenzen geprüft. Der App-Test injiziert Mausbewegung, Mausrad und Mausklicks:
  Linien-, Punkt- und Histogrammzoom, Ziehen, alle drei Zoomschaltflächen,
  unabhängige Diagramm-/Kanalansichten und Rücksetzen beim Öffnen eines Berichts.
  Mausrad außerhalb des Diagramms ändert den Ausschnitt nicht. CSV aus gezoomtem
  Histogramm bleibt bytegleich mit dem vollständigen Export. Ungültige Sensorzeilen
  bleiben auch beim Zoom unsichtbar. Kleine Schwankungen um große Achsenoffsets,
  abgeschnittene Kurven und Bedienelemente bei 1080 × 740 visuell geprüft.
  Vollständiger App-Ablauf mit Build, Simulation, Analyse, Export und Laufvergleich
  der Unsicherheitsvorlage bestanden. Vorschau-Reduktion und Statistik bleiben
  unabhängig vom gewählten Ausschnitt; Exporte umfassen den vollständigen Berichtsplot.
  Diagrammbedienung und integrierter Dokumentationsbrowser bestehen auch aus dem
  frisch installierten portablen Paket.
- Sensoren: Verteilungsmomente, 100000 uniforme/normale Ziehungen, Einheiten,
  Zeitraster, verspätete Abfragen, übersprungene Indizes, Drift/Offset, Quantisierung,
  Standardunsicherheit, Ausfallrate und transaktionale Fehler geprüft. Über 20000
  Rasterplätze bleiben gemeinsame Rauschwerte auch bei zusätzlichen Abfragen,
  ausgelassenen Indizes und geänderter Ausfallwahrscheinlichkeit identisch.
  Drei reale 30-Sekunden-Läufe mit je 6001 Modellzeilen prüfen normalen, idealen
  und vollständig ausgefallenen Sensor bei bitgleicher Physik. Alle gültigen
  Exportzeilen, Statuszahlen, Fehlerkennzahlen und die auf 2048 Punkte reduzierte
  Vorschau stimmen mit Referenzwerten überein. Einzelmessung, falsche Statuswerte,
  falsche Statuseinheit und negative Standardunsicherheit sind ebenfalls geprüft.
  Batch-Endwerte mit Status 0/2 werden abgewiesen, Status 1 wird korrekt aggregiert.
  Vollständiger App-Ablauf mit Unsicherheitsvorlage, Analyse, Export, Laufvergleich
  und erneutem Öffnen bestanden; Sensorplot, Messdatenvorschau und Statuszählung
  bei 1080 × 740 visuell geprüft. Anleitung und öffentlicher Header sind offline
  über die interne Dokumentation verfügbar; interne Verweise auf beide Ziele
  sowie ihre Darstellung wurden geprüft. Derselbe vollständige App-Ablauf besteht
  aus dem portablen Paket mit frisch aus dessen SDK gebauten Modulen. Der normale
  Referenzsensor liefert 2857 gültige Werte, 144 Ausfälle und 3000 nicht fällige Zeilen.
- Parallele Laufserien: dieselben 256 Würfe mit einem und vier Runnern liefern
  bitgleiche Endwert-CSV, Histogrammwerte und Statistik. Acht Wiederholungsläufe
  verwenden acht Runner; ein einzelner längerer Lauf prüft die Begrenzung auf
  die tatsächliche Laufzahl und das abschnittsweise Einlesen. Eine echte
  Vier-Prozess-Barriere erzwingt gleichzeitige Ausführung; Lauf 1 darf erst nach
  den Journal-Einträgen von Lauf 2–4 enden. Ergebnisreihenfolge, separate relative
  Dateien, Teilergebnisse mit Indexlücken und monotone Fortschrittszahlen geprüft.
  Nach Abbruch, Prozessfehler und Zeitlimit bei starkem stdout-Verkehr bestätigen
  Betriebssystemabfragen das Ende sämtlicher gestarteter Test-Runner.
  App-Abläufe mit zwei, vier und acht Runnern prüfen den Parallelitätsregler, erneuten
  Start, Abbruch und das Schließen während einer aktiven Serie über SDL-Ereignisse.
  Die portable CLI liefert auch für die acht höchsten 64-Bit-Seeds bei einem
  beziehungsweise acht Runnern bytegleiche CSV-Dateien. Fortschrittsanzeige und
  Abbruchzustand bei 1080 × 740 visuell geprüft. Der Acht-Runner-Ablauf bestand
  aus dem portablen SDK-Paket mit frisch gebautem Experiment; die korrigierte
  Balkenfüllung ist im aufgenommenen Fortschrittsbild sichtbar.
- Monte Carlo: 256 reale Vakuumwürfe über je 201 Samples liefern für x(1 s)
  einen Mittelwert von 1,00946 m und eine Streuung um den theoretischen Mittelwert
  von 0,15023 m (Soll: 1 m bzw. 0,15 m). Acht komplette Wiederholungsläufe
  reproduzieren alle Modell- und Sensorkanäle bitgenau; jeder Punkt stimmt mit
  der analytischen Bahn und Energiebilanz überein. Andere Seeds ändern die Werte.
  Statistik/Typ-7-Quantile/Normal-KI gegen Zahlenreferenzen geprüft, ebenso
  konstante Daten, Einzelwert, nichtendliche/überlaufende Werte, fehlender Kanal,
  wechselnde Einheit, existierender Ausgabeordner, Prozessabsturz, Zeitlimit und
  Abbruch eines hängenden Runners. App-Ablauf mit realem Build, 256 Läufen,
  Ergebnisanzeige, interner Anleitung und Abbruch per SDL-Ereignissen bestanden;
  Einrichtung, Histogramm und KI-Tabelle bei 1080 × 740 visuell geprüft.
  Die portable CLI verarbeitet auch die drei höchsten 64-Bit-Seeds ohne
  Rundung/Überlauf und erhält konstante Sollbahnwerte.
  Vollständiger App-Ablauf der Unsicherheitsvorlage mit Analyse, Export, Vergleich
  und erneutem Öffnen sowie der Monte-Carlo-Ablauf bestanden aus dem portablen SDK
  mit frisch gebauten Modulen. Sollbahn und Modellbahn bei 1080 × 740 visuell geprüft.
- Feder–Masse–Dämpfer: vier reale Läufe für ungedämpfte, unterkritische, kritische
  und überkritische Bewegung über je zehn Sekunden/2001 Samples gegen geschlossene
  Lösungen bestanden. Kräfte, integrierte dissipierte Arbeit, Energie und Bilanz
  geprüft; Zeitschritthalbierung bestätigt RK4-Konvergenz. Ungültige Schritte und
  Ankerdurchquerung verändern Zustand und Messwerte nicht; Reset reproduziert den
  folgenden Schritt. Alle Punkte der drei Energiekurven sowie Bilanzkennzahl und
  gewählter Manifestkanal geprüft. App-Ablauf mit Build, Analyse, Export, Vergleich
  und erneutem Öffnen bestanden; Federdarstellung und Energieplot bei 1080 × 740
  visuell geprüft. Derselbe Ablauf bestand aus dem portablen SDK-Paket mit frisch
  gebautem Experiment und Analyse. Die Bilanzabweichung des zehnsekündigen
  Standard-Referenzlaufs beträgt maximal 3,42e-9 J; der RK4-Fehlerquotient liegt bei 16.
- Autosave: Unicode und Größenlimits, sämtliche abgeschnittenen Präfixe,
  Byte-Mutationen, ungültiger Text bei gültiger CRC sowie fehlgeschlagene
  Schreib-/Umbenennungsoperationen geprüft. Ein App-Prozess sichert beide geänderten
  Editorinhalte und endet mit `_Exit` ohne reguläres Speichern. Neue Prozesse prüfen
  Wiederherstellen und Verwerfen über SDL-Mausereignisse, externe Quelländerungen,
  Schließen ohne Auswahl und den Erhalt beschädigter Sicherungen. Vor der Auswahl
  starten Ctrl+S/F5 weder Speichern noch Build. Dialog bei 1080 × 740 visuell geprüft.
  Der vollständige normale App-Ablauf mit Pendel bestand ebenfalls. Die fünf
  Wiederherstellungsfälle und der Dokumentationsfenstertest bestanden zusätzlich
  aus dem installierten portablen Paket.
- Boxkontakte: orientierte Kugel–Box-/Box–Ebene-Geometrie, Mehrpunktstoß,
  zweidimensionale Coulomb-Reibung bei ungleichen Trägheitswerten, redundante Punkte,
  transaktionale Fehler und 2000 Schritte ruhenden Bodenkontakt geprüft. Die echte
  geneigte Boxvorlage besteht zehn Sekunden mit 2001 Messpunkten gegen Energie-,
  Bodenabstands-, Kontaktimpuls- und Ruhezustandsreferenzen.
  Vollständiger App-Ablauf einschließlich Vergleich und erneutem Öffnen bestanden,
  auch aus dem portablen SDK mit neu gebautem Experiment bei 1080 × 740.
  Die Bildprüfung deckte eine vom Boden verdeckte Box auf; der korrigierte
  Standardblick wird durch einen GPU-Pixeltest und eine erneute Bildprüfung abgesichert.
- Mechanik: Trägheit, rotierte Hauptachsen, Kräfte/Impulse, gyroskopischer Term,
  Quaternionrotation, Konvergenz, elastischer/inelastischer Stoß, Erhaltung von
  linearem Impuls und Drehimpuls bei Berührung, Reibung und Widerstand geprüft.
  Vier echte Stoßläufe prüfen Vakuum, Luft, eigenes Medium und Reibung/Rotation
  über jeweils 401 Samples einschließlich Modellmetadaten. App-Ablauf mit
  Orientierungsmarkierungen, Geschwindigkeitsvektoren und Kontaktpunkt bestanden,
  auch aus dem portablen SDK-Paket mit neu gebautem Experiment bei 1080 × 740.
- Lineares Resampling: analytische lineare/quadratische Referenzen, Bereichsgrenzen,
  vollständige Achsenprüfung, Einheitenfehler, Blockübergänge, extreme Zahlen,
  Lebensdauer und Plattenbudget bestanden. Differenzberichte und volle CSV mit
  teilweise überlappenden, getrennten und nur an einem Zeitpunkt überlappenden
  Laufzeiten gegen Referenzen geprüft; Stoß-Differenzplot in der App visuell geprüft.
  Portables SDK mit Pendel neu gebaut und vollständigen App-Test bestanden;
  dessen Differenz-CSV zusätzlich unabhängig mit linearer Interpolation nachgerechnet.
- Mehrlaufanalyse: verschiedene Zeitraster, Einheitenfehler, volle CSV und
  Ableitungs-/Statistikreferenzen geprüft. Analyseabsturz, Endlosschleife und
  Kompatibilität mit einem historischen ABI-2-Modul separat geprüft.
- Laufverwaltung: Dateiliste und Auswahl über SDL-Ereignisse, Vergleich zweier
  echter Läufe, Projekt erneut geöffnet, frühere Messdaten/Berichte ohne Build
  geladen und Dateifilter geprüft. Unicode-Projektpfad eingeschlossen.
  Derselbe Ablauf bestand aus dem portablen Paket mit der Stoßvorlage bei 1080 × 740;
  Vergleich und wieder geöffnete Berichte wurden visuell geprüft.
- Eigene Analyseberichte: Roundtrip, alle abgeschnittenen Dateipräfixe, 300 Mutationen,
  Einheiten-/Skalenprüfung, Spitzen in reduzierten Linien, Histogrammzählung und Exporte.
  Angezeigte Pendelkurve gegen die vollständige CSV sowie Kennzahlen gegen die Referenz geprüft.
- App-Ablauf mit erzeugten Linien-/Punkt-/Histogrammplots und Tabellen bei 1440 × 940
  und 1080 × 740 geprüft; SVG-/CSV-Exporte über injizierte Mausklicks ausgelöst.
- Portables Paket: 20-s-Pendellauf mit 4003 Punkten und sieben Periodenintervallen,
  Ergebnistabelle und Exporte bestanden. Ein absichtlich beschädigter Bericht wurde
  im Hintergrund abgewiesen; das vorherige Ergebnis blieb in der App erhalten.
- Datenreihen: Blockgrenzen, SI-Algebra, Handle-Lebensdauer, unveränderliche Snapshots,
  zeitliche Zuordnung, Disk-Limit und Recovery geprüft. Abgeleitete Pendelgeschwindigkeit
  und integrierte Position gegen die physikalische Referenz des echten Laufs geprüft.
- Dokumentation: alle mitgelieferten Themen und Header geladen; F1 und Ctrl+F mit
  SDL-Ereignissen sowie Texteingabe und Sprung zur Fundstelle geprüft. Der Leser
  unterstützt einen begrenzten Markdown-Umfang; Tabellen sind derzeit Textzeilen.
- Szenen-API/IPC 2: maximale Payload, alle abgeschnittenen Längen, fehlerhafte
  UTF-8-Texte/Punktbereiche, 5000 deterministische Mutationen und Ablehnung von IPC/ABI 1.
- Native SDL-App: Fenstersmoke und kompletter App-Selbsttest bestanden.
- Neue Oberfläche bei 1440 × 940 und 1080 × 740 visuell geprüft; Such-/Protokoll-
  Kurzbefehle und Navigation zusätzlich über injizierte SDL-Ereignisse geprüft.
- RK45, Verlet, linearer Solver, Bisektion, Minimierung und Einheitenalgebra mit
  analytischen Referenzen und Fehlerfällen geprüft; drei Pendelverfahren im Runner.
- API-2-App-Abläufe mit Pendel, Wurf inklusive Flugbahn und Stoß inklusive Labels
  bestanden; Stoß zusätzlich direkt aus dem portablen SDK-Paket.
- Reales ABI-1-Modul vor Ausführung abgewiesen; Messdatei aus dem vorherigen
  Entwicklungsstand erfolgreich mit dem neuen Runner nach CSV exportiert.
- OpenGL 3.3 Core auf NVIDIA RTX 2080 Ti: GPU-Readbacktests für Verdeckung,
  Projektion, Größenwechsel, Clipping, Draufsicht, Vektoren, rotierte Boxen/Ebenen,
  Punkte, Polylinien und Labelprojektion bestanden.
- SDL-Eingaberegression für mehrere Unicode-Zeichen pro Ereignis und Fokusverlust.
- App-Selbsttest einschließlich `#error`, erkanntem Compilerfehler, Zeilennavigation,
  korrigiertem Build, Pause/Step/Stop und Analyse bestanden.
- Projektpfad mit Leerzeichen und `ä` erfolgreich getestet.
- Fehlerhafte UTF-8-Quelldateien werden abgewiesen, ohne den aktuellen Editorinhalt
  zu ändern; Projektdateien werden erst nach vollständigem Laden gemeinsam übernommen.
- CSV-Felder mit Kommas und Anführungszeichen, CRC-Verfälschung und unbekannte
  Erweiterungschunks sind durch Regressionstests abgedeckt.
- 4001 Messpunkte / 20 s Pendellauf: Energieabweichung max. 3.40e-10 J,
  Periode 2.488805869 s aus sieben Periodenintervallen.
- Zusätzlicher analytischer Referenztest gegen die elliptische Pendelperiode bestanden.
- Screenshots von Editor, Simulation und Analyse visuell geprüft.
- Installiertes portables Windows-Paket mit eigenem SDK: vollständiger App-Test bestanden.
- Streamingtest: 1.000.001 Punkte / 68.001.526 Bytes; Schreiben 16,8 s,
  Analyse 2,4 s auf diesem Rechner (Release). Das ist eine lokale Messung,
  keine garantierte Leistung für andere Hardware.

Die Windows-Desktopautomationsschnittstelle war nicht erreichbar. Die UI wurde
deshalb mit dem eingebauten App-Test und dessen gerenderten Screenshots geprüft;
ein zusätzlicher externer OS-Mausklick-/Tastaturtest steht aus.
