# Physim lernen und nachschlagen

Mit Physim schreibst du ein physikalisches Modell, zeichnest Messwerte auf und
wertest sie reproduzierbar aus. Du brauchst dafür keine Header zu lesen.
Dieses Handbuch verbindet Bedienung, ausführbare Beispiele, Modellgrenzen und
eine vollständige C-Funktionsreferenz. Die Physim-Sprache besitzt eine eigene
Bibliotheksreferenz. Stand: 0.1.0-dev, API und ABI 3.

## Dein erstes Ergebnis

1. Öffne links **Öffnen oder anlegen**, wähle **Pendel**, gib einen neuen Projektordner an und drücke **Anlegen**.
2. Drücke **F5**. Der Build speichert Experiment und Auswertung. Warte auf **Build erfolgreich**; Fehler erscheinen im Protokoll.
3. Wechsle zu **Simulieren** und starte mit **F6**. Nach einigen Schwingungen drücke **Stoppen**.
4. Unter **Auswerten** siehst du Messdaten und Statistik. **Analyse starten** führt die zum Projekt gehörende Auswertung aus.
5. Wähle im **Analyseergebnis** ein Diagramm oder eine Tabelle. Exportiere CSV für Zahlen, SVG für Vektorgrafik oder PNG für ein Bild.

Erwartung: Die Pendelposition schwingt. Mit Widerstand nimmt ihre Amplitude ab.
Der Lauf liegt im Projekt unter `runs/`; das Schließen der App löscht ihn nicht.
Ändere anschließend nur den Anfangswinkel, baue neu und erzeuge einen zweiten Lauf.
Über **Läufe & Berichte** kannst du beide auswählen und vergleichen.

[Alle Bedienelemente und Tastenkürzel](workspace.md)

## Wähle deinen Lernweg

- **Experimentieren ohne neue API:** Starte mit einer Vorlage, ändere einen Parameter, wiederhole den Lauf und vergleiche die Ergebnisse. [Feder und Dämpfung](spring.md), [Stöße und Medien](mechanics.md), [Auftrieb](buoyancy.md).
- **Ein eigenes Modell in C:** Ein vollständiges, kleines Modul erklärt Zustand, Zeit, Kanäle, Szene und Aufräumen. [C-Experiment Schritt für Schritt](experiment-tutorial.md).
- **Physim-Sprache:** Wähle eine Sprachvorlage; Experiment und Analyse dürfen unterschiedliche Sprachen verwenden. [Eigenes Experiment und Auswertung Schritt für Schritt](language-tutorial.md), [Sprachvertrag](language.md), [Werte, Arrays und Methoden](language-values.md), [alle Bibliotheksfunktionen](reference/language-library.md).
- **Gespeicherten Lauf auswerten:** Erstelle ein unabhängiges Analyseprojekt in C oder Physim, importiere ein Archiv und prüfe Ableitung, Rückintegration sowie gespeicherte Ergebnisse ohne erneute Simulation. [Vollständiger Lernpfad](saved-run-tutorial.md).
- **Mehrere Läufe mit Sprachcode auswerten:** Kombiniere C- und Physim-Läufe, sammle Endwerte und erzeuge Diagramm, Histogramm, Tabelle und CSV. [Mehrlaufanalyse in Physim](language-analysis-tutorial.md).
- **Wurfparabel im Vakuum:** Baue dasselbe Experiment und dieselbe Auswertung in C und Physim-Sprache; prüfe die Werte gegen die exakte Lösung. [Vollständiger Lernpfad](projectile-tutorial.md).
- **Wurf mit Luftwiderstand:** Ergänze eine quadratische Kraft, integriere mit RK4 und vergleiche C und Physim-Sprache an denselben Messdaten. [Vollständiger Lernpfad](projectile-drag-tutorial.md).
- **Pendel und Integratoren:** Vergleiche fünf Verfahren in C und Physim anhand desselben Vakuummodells, untersuche Energiefehler und Schwingungsdauer und stelle feste sowie adaptive Zeitraster gemeinsam dar. [Vollständiger Lernpfad](pendulum-tutorial.md).
- **Elastischer und inelastischer Stoß:** Vergleiche Restitution und ungleiche Massen in C und Physim, prüfe Impuls und Energiebilanz sowie die Stoßzeit zwischen Messpunkten. [Vollständiger Lernpfad](collision-tutorial.md).
- **Unsichere Anfangswerte:** Wiederhole einen Vakuumwurf mit 256 Seeds in C und Physim und vergleiche archivierte Endwerte, Quantile und ein Konfidenzintervall des Mittelwerts. [Vollständiger Lernpfad](monte-carlo-tutorial.md).
- **Feder–Masse–Dämpfer:** Vergleiche vier Dämpfungsfälle in C und Physim, trenne dissipierte Arbeit vom numerischen Fehler und prüfe die Energiebilanz. [Vollständiger Lernpfad](spring.md).
- **Eigenes Material und Medium:** Leite die Kugelmasse aus einer eigenen Materialdichte ab, kombiniere Gewicht, Auftrieb und viskosen Widerstand und prüfe C-/Physim-Läufe gegen die analytische Lösung. [Vollständiger Lernpfad](material-tutorial.md).
- **Messwerte auswerten:** Öffne einen Lauf, leite Position ab, filtere ungültige Sensorwerte und erzeuge eigene Diagramme. [Datenreihen](series.md), [Berichte und Export](reports.md), [Vergleich mehrerer Läufe](runs.md).
- **Unsicherheit untersuchen:** Trenne Modell, Messung, Messstatus und Standardunsicherheit. [Sensoren](measurement.md), [Monte Carlo](monte-carlo.md).

## Funktionen nach Aufgabe finden

- Vektoren, Rotation, Matrizen und räumliche Kurven: [Mathematik](math.md).
- Zeitschritte, adaptive Integration, Gleichungssysteme, Nullstellen und Minimum: [Numerik](numerics.md).
- Einheiten und dimensionsbehaftete Größen: [Einheitenreferenz](reference/units.md).
- Körper, Kräfte, Kontakte, Reibung, Gelenke, Kontaktketten und kontinuierliche Kugelkollisionen: [Mechanik](mechanics.md), [persistente C-/Physim-Kontaktzustände](contact-world.md), [Kollisionserkennung](reference/collision.md).
- Zufallszahlen und Fehlercodes: [Grundlagenreferenz](reference/core.md).
- Kanäle, Formen, Objekt-IDs und Szenengrenzen: [API-Überblick](api.md), [Experimentreferenz](reference/experiment.md).
- Dateien lesen und schreiben: [Datenreferenz](reference/data.md),
  [Szenenzustände](reference/snapshot.md), [Dateiformat](data-format.md).
- Speicher und eigene Datenstrukturen: [Allocator und Arena](memory.md), [Arrays](array.md), [String-Views](string-view.md), [Hashmap](hashmap.md).

## Das Handbuch verwenden

**F1** oder **Handbuch & Referenz** öffnet ein eigenes Fenster. Du kannst es neben
dem Editor lassen; die Simulation bleibt unabhängig bedienbar. **Themen** bietet
die Bereiche Lernen, Fachthemen, Funktionsreferenz und Projektunterlagen.
Header sind ausschließlich im optionalen Zusatzbereich erreichbar.

**Alle Inhalte durchsuchen** links sucht auch innerhalb anderer Dokumente.
Gib beispielsweise `ps_series_derivative`, `Sensor`, `Reibung` oder `Export` ein.
Wähle ein Ergebnis: Im Dokument wird der Begriff markiert und der erste Treffer
angesprungen. **Suche zurücksetzen** stellt die Themenauswahl wieder her.
**Ctrl+F** sucht nur im geöffneten Dokument; **Vorheriger/Nächster** wechseln
zwischen Abschnitten mit Treffern. **Inhalt** links springt zu einem Kapitel.
**Zurück** wechselt zum zuletzt geöffneten Dokument, **Start** zu dieser Seite.
Codeblöcke lassen sich mit **Code kopieren** vollständig übernehmen.

**Arbeitsbereich** hebt das Hauptfenster nach vorn. **Esc** oder das Schließen des
Hilfefensters blendet nur die Hilfe aus; F1 stellt Dokument und Suchposition wieder her.
Die Schriftgröße der Codebeispiele folgt den Einstellungen.

## Ergebnisse verstehen und Probleme lösen

Ein Diagramm ist eine Vorschau; große Datenreihen können dafür reduziert werden.
Messdaten-CSV und Reihenexport enthalten die vollständigen Daten. CSV eines
Ergebnisdiagramms enthält dagegen dessen dargestellte Punkte. Gleiche Seeds machen
Zufallsströme wiederholbar; für einen Vergleich müssen außerdem Modell, Zeitschritt
und Eingabedaten passen. Eine höhere Integratorordnung ersetzt keinen Konvergenztest.

[Fehler finden und beheben](troubleshooting.md)

[Einstellungen und Darstellung](settings.md)

[Autosave und Wiederherstellung](autosave.md)

[SDK bauen und installieren](build.md)
