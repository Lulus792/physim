# Läufe und Berichte

Ein Experiment kann bis zu 16 benannte `Float64`-Parameter definieren. Im CLI-Lauf
überschreibt `--param name=wert` einen Standardwert; die Option darf für
unterschiedliche Namen wiederholt werden. Unbekannte, doppelte, nicht endliche
oder außerhalb der deklarierten Grenzen liegende Werte brechen den Lauf vor dem
Schreiben ab. Die `.psrun`-Metadaten speichern je Parameter den wirksamen Wert
unter `parameter.<name>` sowie Standardwert und Grenzen unter
`parameter_default.<name>`, `parameter_min.<name>` und `parameter_max.<name>`.
Nach einem erfolgreichen Build zeigt die App die vom Experiment gemeldeten
Parameter unter **Simulieren → Inspector → Laufeinstellungen → Experimentparameter**.
Werte können innerhalb der angezeigten Grenzen bearbeitet oder auf den Standard
zurückgesetzt werden. Die App prüft die Eingaben vor dem Start und übergibt sie
dem isolierten Runner. Speichern, Starten und Schließen schreiben gültige
Auswahlwerte als `parameter.<name>=<wert>` in `physim.project`. Nach dem erneuten
Öffnen übernimmt der nächste Build diese Werte; liegt ein gespeicherter Wert
außerhalb neuer Grenzen, gilt wieder der Standardwert. Für mehrere Werte bietet
**Monte Carlo → Parameterstudie**
einen linearen Sweep; die [Laufserien-Anleitung](monte-carlo.md) beschreibt die
Zuordnung und Auswertung.

```powershell
physim-runner.exe experiment.dll run.psrun --steps 100 --param initialSpeed=4.5
```

**Läufe & Berichte** in der Seitenleiste oder Ctrl+4 öffnet die Dateien des aktuellen
Projekts. **Messläufe** zeigt `.psrun`, **Analyseberichte** zeigt `.psreport` aus dem
Ordner `runs`. Die Liste wird im Hintergrund geladen und nach Änderungsdatum sortiert.
Ctrl+F filtert nach Dateiname. **Aktualisieren** liest Änderungen außerhalb der App ein.

## Gespeicherte Ergebnisse öffnen

**Ansehen** lädt einen Messlauf in die Auswertung. **Öffnen** lädt einen gespeicherten
Bericht mit seinen Diagrammen und Tabellen. Dafür ist kein Build erforderlich, auch
nach dem erneuten Öffnen des Projekts. Beschädigte Berichte ersetzen kein vorhandenes
Ergebnis. Die Dateiliste prüft zunächst nur Namen und Dateiinformationen; Inhalte
werden erst beim Öffnen geprüft.

## Läufe vergleichen

1. Mit F5 die aktuelle `analysis.c` beziehungsweise `analysis.phys` bauen.
2. Unter **Messläufe** zwei bis acht Dateien mit den Kästchen auswählen.
3. **Auswahl vergleichen** startet die Analyse im separaten Prozess.
4. In der Auswertung zwischen Diagrammen und Kennzahlentabelle wechseln.

Die mitgelieferte Vorlage vergleicht Position und abgeleitete Geschwindigkeit.
Sie verwendet `position.x`, bei der Stoßvorlage `a.position`. Die Kurven behalten
ihre eigenen Zeitpunkte und Laufzeiten. Einheiten müssen zu Meter und Sekunde passen.
Die Herkunft des Ergebnisses nennt die zugehörigen Eingabedateien und das Verfahren.

Das zusätzliche Diagramm **Positionsdifferenz zu Lauf 1** zeigt für jeden weiteren
Lauf seine Position minus der Position des zuerst ausgewählten Laufs. Dafür wird
Lauf 1 ausdrücklich linear auf die Zeitpunkte des anderen Laufs interpoliert.
Berücksichtigt werden nur vorhandene Zielzeitpunkte im gemeinsamen Zeitbereich;
Zeitachsen werden nicht verschoben und außerhalb der Quelle wird nicht extrapoliert.
Ohne gemeinsame Zielzeitpunkte entfallen Differenzkurve und Tabellenzeile.
Bei einem einzigen gemeinsamen Zeitpunkt erscheint ein Punkt mit Standardabweichung 0.
Die Tabelle gewichtet jeden Zielmesspunkt gleich; sie ist kein zeitgewichteter Mittelwert.
Eine Änderung der Auswahlreihenfolge ändert die Referenz. Auswahl leeren und die
gewünschte Referenz zuerst anhaken, um sie zu wechseln.

Auswahl und Ansicht sind unabhängig: **Ansehen** ändert die Vergleichsauswahl nicht.
Ein neuer Simulationslauf ersetzt die Auswahl durch diesen Lauf. **Auswahl leeren**
entfernt alle Häkchen. Bei genau einem ausgewählten Lauf startet die normale Analyse.
**CSV exportieren** in der Auswertung exportiert den aktuell geladenen Messlauf.
Die Vergleichsvorlage schreibt zusätzlich vollständige Reihen nach
`<präfix>-run-1.csv`, `<präfix>-run-2.csv` usw. Diagrammexporte enthalten dagegen die
angezeigten, gegebenenfalls reduzierten Punkte. `<präfix>-difference-N.csv` enthält
alle gemeinsamen Zielzeitpunkte, die Originalposition, die interpolierte Position
von Lauf 1 und die Differenz. Die Interpolation und ihre Grenzen beschreibt die
[Datenreihen-API](series.md).

## Eigene Mehrlaufanalyse

`ps_analysis_api.run_many` ist eine optionale Erweiterung von ABI 3. Der Runner
prüft `struct_size`, bevor er dieses Feld liest. ABI-3-Module ohne diesen Callback behalten ihre
Einlauf-Funktion; ein Mehrlaufauftrag wird ohne passenden Callback abgewiesen.
`run` bleibt erforderlich. Ein neues Modul ohne Mehrlaufanalyse setzt `run_many`
auf NULL. Beispiel für ein Modul mit beiden Funktionen:

```c
static const ps_analysis_api api = {
    sizeof(ps_analysis_api), PS_ABI_VERSION, "Meine Analyse", analyze, analyze_many
};
```

`run_many` erhält geliehene Pfadstrings in Auswahlreihenfolge und einen gemeinsamen
Ausgabepräfix. Der Callback läuft synchron. Ein Auftrag mit genau einem Eingang
verwendet immer `run`. Der Runner bietet auch einen Kommandozeilenaufruf:

```powershell
physim-analysis-runner.exe analysis.dll --runs vergleich lauf-a.psrun lauf-b.psrun
```

Vor dem Callback schreibt der Runner `<präfix>.inputs.csv` mit Modul und Eingängen:
Rolle, Index, Pfad, Dateigröße und FNV-1a-64-Fingerabdruck. Bestehende Ausgaben werden
nicht überschrieben. FNV identifiziert Inhalte, ist jedoch kein kryptographischer
Nachweis. Externe Änderungen zwischen Fingerabdruck und Analyse werden nicht gesperrt.
Die App kopiert außerdem den Analysequellcode nach `<präfix>.source.c`.

## Grenzen

Die Liste durchsucht nur den direkten `runs`-Ordner und zeigt höchstens 4096 Dateien.
Bei Überschreitung erscheint eine Warnung; die enthaltene Teilmenge ist nicht
garantiert die neueste. Ungültige oder unlesbare Einträge werden übersprungen und
gezählt. Projektübergreifende Auswahl, weitere Interpolationsverfahren und Plotzoom
sind offen.
