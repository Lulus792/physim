# Teil I – Physim mit C

Dieser Lernweg führt von der Einrichtung bis zum gespeicherten Ergebnis in C17.
Experiment und Analyse dürfen beide C bleiben. Die gleichen physikalischen
Referenzmodelle stehen im [Physim-Lernweg](physim-guide.md); du brauchst für diesen
Teil keine Physim-Syntax zu lernen. API/ABI 3, Entwicklungsstand 0.1.0-dev.

## 1. Einrichten und das erste Projekt öffnen

Installiere auf macOS die Xcode Command Line Tools, unter Windows Visual Studio
C++ Build Tools mit Windows SDK oder unter Linux GCC beziehungsweise Clang.
Die [vollständigen Plattformbefehle und SDK-Prüfungen](build.md) unterscheiden
Quellbuild, installiertes SDK und Grafiktests. Python steuert nur den direkten
Build; für Physim-Projekte ist kein CMake nötig.

Lege **Pendel** mit **C-Auswertung** in einem neuen Ordner an. F5 speichert und
baut die Quellen. F6 startet den Versuch, **Stoppen** finalisiert die Laufdatei.
**Analyse starten** erzeugt einen Bericht aus dem Archiv.
[Bedienung, Dateien und Tastenkürzel](workspace.md).

## 2. C für den Physim-Workflow

[C-Einstieg mit ausführbarem Beispiel](c-workflow.md) erklärt Funktionen,
Strukturen, Wertkopien, Arrays, Pointer, Ausgabeparameter und Fehlerbehandlung.
Das Beispiel berechnet 9 J und einen Mittelwert von 12,5 J. Du setzt anschließend
nur die dort gezeigten Konzepte ein, um einen Zustand und dessen Lebensdauer im
Experiment zu beschreiben. [Einheiten](reference/units.md) halten SI-Zahlen und
Darstellungsskalen auseinander; [Speicher](memory.md) erklärt Heap und Allocator.

## 3. Ein vollständiges Experiment schreiben

[Experiment und Analyse Schritt für Schritt](experiment-tutorial.md) enthält
vollständige C-Quellen für gleichförmige Bewegung. Ein Lauf von einer Sekunde
liefert 1,5 m; die abgeleitete Geschwindigkeit bleibt 1,5 m/s. Dort lernst du
create, reset, step, Szene und destroy samt eigenem Zustand.
[Experiment-API](reference/experiment.md), [Kanäle und Geometrie](api.md).

## 4. Messdaten und Fehler erhalten

[Messungen und Sensoren](measurement.md) trennt das physikalische Modell von
Sensorwert, Messstatus und Standardunsicherheit. Gültige Werte haben Status 1;
fehlende Messungen werden nicht durch null ersetzt. [Logging](logging.md) und
[Diagnosen](diagnostics.md) verwenden ausdrückliche Sinks und eigene Werte.
[Dateiformat](data-format.md) erklärt CRC, finalisierte Läufe, rekonstruierbare
Präfixe und aufgezeichnete Szenen. [Laufindex](run-index.md) ermöglicht gezielte
Rohdatenabfragen. Ein Pfad allein bestätigt keine vollständige Datei.

## 5. Aus Archiven Diagramme und Exporte erzeugen

[Datenreihen](series.md) zeigt Analysecontext, Dataset, gemeinsam ausgerichtete
Series, Ableitung und Integration. [Berichte](reports.md) führt durch Tabellen,
Histogramme sowie CSV-, SVG- und PNG-Ausgabe. Jeder Handle gehört seinem Context
oder Bericht; schließe und zerstöre erfolgreiche Konstruktionen.
[Unabhängige Analyse eines gespeicherten Laufs](saved-run-tutorial.md) benötigt
keinen erneuten Experimentbuild. [Laufbibliothek](runs.md) erklärt Auswahl und
Wiederöffnung. [Archivierte Serien in C](batch-language.md) verwendet die
öffentlichen Hostdienste aus [batch.h](reference/batch.md).

## 6. Debuggen und die Modellgrenzen prüfen

[Fehler finden](troubleshooting.md) behandelt Compiler, Runner, leere Szene,
fehlende Kanäle und Export. Verwende Rückgabecodes, Diagnosefelder und die
Quelldatei des fehlgeschlagenen Aufrufs. Bei Speicherfehlern bleiben Ausgaben
nach ihrem dokumentierten Vertrag erhalten; bereits gespeicherte Dateien sind
separat zu prüfen. [Numerik](numerics.md) erklärt Schrittweitenverfeinerung und
Integratorwahl. Die [Buildanleitung](build.md) enthält reale Sanitizer- und
Fuzzing-Befehle sowie die Grenzen ihrer Plattformnachweise.

## 7. Dieselben acht Modelle untersuchen

Jede Seite enthält Lernziel, Annahmen, Gleichungen, vollständige C- und Physim-
Quellen, Erwartung, Modellgrenzen und automatisierte Prüfungen. Wähle dort jeweils
**Experiment in C** und **Analyse in C**; gemeinsame Modelltexte bleiben verlinkt.

1. [Wurfparabel im Vakuum](projectile-tutorial.md): exakte Bewegung, 1 m nach einer Sekunde bei vx=3 m/s und x0=-2 m.
2. [Wurf mit Luftwiderstand](projectile-drag-tutorial.md): RK4, quadratische Kraft und Schrittweitenverfeinerung.
3. [Pendel und fünf Integratoren](pendulum-tutorial.md): nichtlineare Periode, Energiefehler und adaptive Zeitachsen.
4. [Elastischer und inelastischer Stoß](collision-tutorial.md): Ereigniszeit, Impuls, Energie und Restitution.
5. [Feder–Masse–Dämpfer](spring.md): vier Dämpfungsregime und mechanische Energie plus Dissipation.
6. [Unsichere Anfangswerte mit Monte Carlo](monte-carlo-tutorial.md): 256 Seeds, Quantile und begrenzte Aussagen des Konfidenzintervalls.
7. [Gespeicherten Lauf analysieren](saved-run-tutorial.md): Ableitung, Rückintegration und unveränderte Archive.
8. [Eigenes Material und Medium](material-tutorial.md): Masse aus Dichte, Auftrieb, Stokes-Widerstand und Gültigkeitsgrenzen.

Der zusätzliche [Thermodynamik-Einstieg](thermodynamics.md) untersucht
ideales Gas und isolierten Wärmeaustausch mit vollständigen Quellen beider Sprachen.

Der zusätzliche [Elektromagnetismus-Einstieg](electromagnetism.md) erklärt
Ladungen, Felder und den vollständigen RC-Versuch beider Sprachen.

## Nachschlagen und weitergehen

[C-API nach Aufgabe](api.md), [vollständige C-Funktionen](reference/core.md),
[Mechanik](reference/mechanics.md), [Numerik](reference/numerics.md),
[Analyse](reference/analysis.md), [Berichte](reference/report.md).
F1 enthält für jeden öffentlichen C-Bereich eine vollständige Referenz mit
SDK-Verträgen. **Teil I · C** bringt dich zu diesem Lernweg; **Start** bleibt hier,
wenn du eine gemeinsame Fachseite öffnest. Für den anderen Weg wähle
[Teil II – Physim](physim-guide.md) oder die gleichnamige Schaltfläche.
[Gemeinsamer Handbuchstart](guide.md), [aktueller Umsetzungsstand](status.md).
