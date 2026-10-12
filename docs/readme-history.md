# Frühere Physim Einführung

Diese frühere Einführungsfassung bleibt als historische Beschreibung erhalten.
Für die aktuelle Nutzung beginne im [Handbuch](guide.md) oder [User Guide](user-guide.md).

## Frühere Übersicht

**Installieren und mit Oberfläche starten:**
[Windows](#schnellstart-unter-windows) · [Linux / Debian und Ubuntu](#linux) ·
[macOS / Apple Silicon und Intel](#macos).
Die Anleitungen enthalten Installation, Build, Tests und den Start der App.

**Oberfläche nach der Installation erneut starten:** Im Terminal in den
Physim-Repository-Ordner wechseln und den passenden Befehl ausführen:

| System | Oberfläche starten (Debug-Build) |
| --- | --- |
| Windows / PowerShell | `.\build\native\Debug\bin\physim.exe` |
| Linux / Terminal | `./build/native/Debug/bin/physim` |
| macOS / Terminal | `./build/native/Debug/bin/physim` |

Die vollständigen Installationsbefehle stehen in den oben verlinkten Abschnitten.
Unter macOS beschreibt die Anleitung außerdem, wie sich `Physim.app` zum
Starten per Doppelklick erstellen lässt. Linux benötigt eine grafische
Desktop-Sitzung. `--test` prüft den Build ohne Fenster; der anschließende
Startbefehl öffnet die Oberfläche.

**Projektwissen in SecondBrain:** [Anbindung und Öffnen](../docs/secondbrain.md) ·
[Einstieg ins Projektgedächtnis](../brains/physim/START.md).

**Neu hier?** Das [Handbuch mit Lernpfaden](../docs/guide.md) erklärt Bedienung,
Experimente, Auswertung und alle öffentlichen Funktionen. In der App öffnet **F1**
dieselben Inhalte offline, mit themenübergreifender Suche und kopierbaren Beispielen.

Physim ist eine native C17-Arbeitsumgebung für reproduzierbare Physikexperimente.
Der erste implementierte Durchstich führt von bearbeitbarem C-Code über einen
separaten Simulationsprozess und gespeicherte Messwerte bis zur C-Auswertung.

**Entwicklungsstand: 0.1.0-dev, noch keine stabile 1.0.** Der verbindliche Langfristplan
steht in [Physim_Projektplan.md](../Physim_Projektplan.md). Der konkrete Umsetzungsstand
und noch offene Abnahmekriterien stehen in [docs/status.md](../docs/status.md).

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
Der [Sprachentwurf](../docs/language.md) beschreibt den Stand.
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
Ein [getestetes Sprachtutorial](../docs/language-tutorial.md) führt vom eigenen
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
