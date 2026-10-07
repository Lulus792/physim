# Teil II – Physim mit der eigenen Sprache

Dieser Lernweg führt mit Physim 0.178.0 von der Einrichtung bis zum gespeicherten
Ergebnis. Experiment und Analyse können vollständig Physim bleiben; du brauchst
für die unterstützten Aufgaben keinen C-Code nachzuschreiben. Die Sprache ist
weiter ein Entwicklungsvertrag. Dieselben Modelle und Archive verwendet
[Teil I – C](c-guide.md); Quellsprachen dürfen im Projekt kombiniert werden.

## 1. Einrichten und die Sprache wählen

Die App liefert `physimc`, Runner, Builder und SDK mit. Für die native Übersetzung
benötigst du die in der [Buildanleitung](build.md) genannten C17-Werkzeuge.
Der eigene Quellcode bleibt `.phys`; der Builder erzeugt C ausschließlich unter
`build/`. Lege **Pendel · Physim-Sprache** an und wähle auch für die Auswertung
**Physim-Sprache**. F5 prüft und baut, F6 startet, **Stoppen** erhält den Lauf.
[Projekte, Dateien und Tastenkürzel](workspace.md).

## 2. Syntax und statische Typen lernen

[Physim-Einstieg mit ausführbarem Beispiel](physim-workflow.md) zeigt let und var,
Inferenz und explizite Annotationen, Funktionen, benannte Argumente, Strukturen,
Arrays, Schleifen und optionale Werte. Das Beispiel berechnet dieselben 9 J und
12,5 J wie der C-Einstieg. [Sprachvertrag](language.md) beschreibt die tatsächlich
implementierte Grammatik, Zahlen- und Überlaufregeln, Module und Generics.
Die [Wertsemantik](language-values.md) vertieft eigene Methoden und Sammlungen.
Noch nicht implementierte Syntax darf nicht als ausführbarer Weg vorausgesetzt werden.

## 3. Speicher und Fehler verstehen

Werte werden kopiert: Änderungen an einer Struktur oder einem Array verändern
keine frühere Kopie. Besitzende Werte wie String, RunIndex, ContactWorld und Batch
räumen Ressourcen auch bei Rückkehr oder abgefangenem Fehler auf. Dataset und
Series gehören ihrem Analysecontext. `attempt(expression)` liefert bei einer
abfangbaren Laufzeitdiagnose nil; bereits sichtbare Seiteneffekte bleiben bestehen.
[Lebensdauer und Budgets](language-values.md), [strukturierte Diagnosen](diagnostics.md).
Ein falscher statischer Typ verhindert native Ausführung; ein ungültiger
Laufzeitindex oder numerischer Bereich liefert eine Quellposition.

## 4. Ein vollständiges Experiment bauen

[Experiment und Analyse in Physim](language-tutorial.md) enthält alle Quellen
für gleichförmige Bewegung und Auswertung. Channel und Unit definieren die
Messung, create/reset/step/scene die Lebensphasen. Nach einer Sekunde erwarten
wir 1,5 m und aus der Ableitung 1,5 m/s. Der Builder verwaltet Zustand,
Ressourcen und native Module. [Physim-Bindungen A–Z](reference/language-library.md)
zeigt die erlaubten Signaturen und welche Aufrufe einen Experiment- oder
Analysehost benötigen. [Persistente Kontakte](contact-world.md) und
[Szenenkoordinaten](scene-frames.md) ergänzen komplexere Modelle.

## 5. Messdaten aufzeichnen und auswerten

[Sensoren](measurement.md) trennt Modellwerte von gültigen, nicht fälligen und
verworfenen Messungen. [Logging](logging.md) und [Diagnosen](diagnostics.md)
liefern explizite Fehlerdaten. [Archive](data-format.md) und
[RunIndex-/RunBlock-Werte](run-index.md) beschreiben unabhängiges Nachlesen.
Die [Analyse mehrerer C-/Physim-Läufe](language-analysis-tutorial.md) verwendet
Dataset, Series, Diagramm, Histogramm, Table und CSV-Export ohne gemeinsamen
Zeitgitter zu erzwingen. [Berichte und PNG/SVG](reports.md),
[Auswahl und Wiederöffnung](runs.md), [Batch aus Sprachcode](batch-language.md).
`Batch` startet, pausiert und setzt dieselben archivierten Runner wie C fort.

## 6. Debuggen und fachlich verifizieren

Die App springt von einer strukturierten Diagnose zur betroffenen Quellposition.
[Fehlersuche](troubleshooting.md) behandelt Typfehler, Compiler, Runner, fehlende
Kanäle und Exporte. [Numerik](numerics.md) erklärt Integratorwahl, Toleranzen und
Schrittweitenverfeinerung; ein erfolgreicher Build beweist kein korrektes Modell.
Die [Buildanleitung](build.md) prüft Compiler, Speicherlebensdauer und reale
Runner mit Sanitizern. [Plattformnachweise](platform-validation.md) unterscheiden
lokale Ergebnisse von noch nicht bestätigten Releasegates.

## 7. Dieselben acht Modelle untersuchen

Die gemeinsamen Modellseiten bewahren Gleichungen und Annahmen für beide Wege.
Wähle jeweils **Experiment in Physim** und **Analyse in Physim**. Beide Quellen
sind vollständig und werden gegen dieselben physikalischen Erwartungen geprüft.

1. [Wurfparabel im Vakuum](projectile-tutorial.md): exakte Lösung und gleiche archivierte Messwerte.
2. [Wurf mit Luftwiderstand](projectile-drag-tutorial.md): quadratische Kraft, RK4 und Verfeinerung.
3. [Pendel und fünf Integratoren](pendulum-tutorial.md): nichtlineare Periode, Energie und adaptive Reihen.
4. [Elastischer und inelastischer Stoß](collision-tutorial.md): Impuls, Restitution und Energieabrechnung.
5. [Feder–Masse–Dämpfer](spring.md): vier Dämpfungsfälle und Bilanzfehler.
6. [Unsichere Anfangswerte mit Monte Carlo](monte-carlo-tutorial.md): dieselben 256 Seeds, Quantile und Konfidenzgrenzen.
7. [Gespeicherten Lauf analysieren](saved-run-tutorial.md): unabhängiges Analyseprojekt mit Ableitung und Rückintegration.
8. [Eigenes Material und Medium](material-tutorial.md): eigene Dichte und Viskosität, Referenzlösung und Grenzen.

## Nachschlagen und weitergehen

[Sprachvertrag](language.md), [Werte und Methoden](language-values.md),
[alle Sprachbindungen](reference/language-library.md). F1 ist vollständig offline
benutzbar. **Teil II · Physim** öffnet diesen Lernweg; **Start** bleibt hier,
wenn du ein gemeinsames Fachthema öffnest. [Teil I – C](c-guide.md) erklärt
denselben Workflow mit expliziten C-Ressourcen und Rückgabecodes.
[Gemeinsamer Handbuchstart](guide.md), [aktueller Umsetzungsstand](status.md).
