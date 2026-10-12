# Physim Handbuch

Physim verbindet ein physikalisches Modell mit einer sichtbaren Szene,
gespeicherten Messwerten und einer reproduzierbaren Auswertung. Beginne mit
einem fertigen Beispiel und überprüfe sein Ergebnis. Danach kannst du eigene
Modelle in C oder in der Physim-Sprache schreiben.

## Den passenden Einstieg wählen

| Dein Ziel | Einstieg |
| --- | --- |
| Die App benutzen und einen ersten Lauf erhalten | [User Guide](user-guide.md) |
| Ein physikalisches Modell auswählen | [Examples](examples.md) |
| Ein eigenes Experiment mit C schreiben | [C Lernweg](c-guide.md) |
| Ein eigenes Experiment mit Physim schreiben | [Physim Lernweg](physim-guide.md) |
| Eine konkrete Funktion nachschlagen | [C Referenz](reference/core.md) oder [Physim Referenz](reference/language-library.md) |
| Ein Problem lösen | [Fehlerbehebung](troubleshooting.md) |

Die Installation steht in [README](../README.md); der
[SDK Build](build.md) enthält die ausführlichen Werkzeug- und Paketwege.
Physim ist eine Entwicklungsversion. Der [Status](status.md) trennt vorhandene
Funktionen und noch offene Produktziele.

## Ein Ergebnis vorhersagen und prüfen

Für dein erstes Experiment reicht die [Pendelvorlage im User Guide](user-guide.md).
Lege das Projekt an, baue es mit F5, starte mit F6 und stoppe den Lauf mit F9.
Unter Auswerten erzeugst du daraus einen Bericht. Beschreibe anschließend,
welche Kurve du erwartet hast und ob das gespeicherte Ergebnis dazu passt.

Ein Modell, ein Zeitschritt und eine Messung beantworten unterschiedliche Fragen.
Die [Examples](examples.md) zeigen, wie du diese Unterschiede an vollständigen
C-/Physim-Versuchen untersuchst. Ihre Originalquellen und physikalischen
Prüfungen bleiben zusammen auffindbar.

## Arbeitsabläufe und Erklärungen

- [Arbeitsbereich](workspace.md): Projekte, Editor, Build, Szene und Tastenkürzel.
- [Messung](measurement.md): Modell, Sensorwerte, Messstatus und Unsicherheit.
- [Datenreihen](series.md): passende Werte ableiten, integrieren und vergleichen.
- [Berichte](reports.md): Diagramme, Tabellen und CSV-/SVG-/PNG-Export.
- [Läufe](runs.md): Archive auswählen und spätere Analysen wiederholen.
- [Einstellungen](settings.md) und [Wiederherstellung](autosave.md): Darstellung und gespeicherte Arbeit.

Die weiterführenden [CFD](cfd-project-plan.md)- und [FEM](fem-project-plan.md)-
Pläne beschreiben spätere räumliche Solver und deren eigene Abnahmen.

## Offline nachschlagen und Projektwissen pflegen

F1 öffnet das Handbuch neben dem Editor. Themen, Inhalt und Suche führen zu
passenden Abschnitten; Code kopieren übernimmt vollständige Beispielblöcke.
Die C- und Physim-Lernwege bleiben als getrennte Wege erreichbar.
Diagramme erscheinen nativ im Lesebereich. Die
[räumliche Pendelerklärung](spatial-explanations.md) ergänzt Regler und eine
eingebettete 3D-Szene. Externe Quellenlinks kopieren die Adresse; das Handbuch
bleibt in Physim geöffnet.
[Bedienung der Hilfe](workspace.md#fenster-hilfe-und-tastenkürzel).

[SecondBrain](secondbrain.md) verbindet dieses Handbuch mit dem Auftrag,
aktuellem Stand, Entscheidungen und maßgeblichen Quellen. Die
[Grundlagen der Dokumentation](documentation-research.md) erläutern den Aufbau
und die untersuchten Vorbilder.
