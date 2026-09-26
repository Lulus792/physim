# Arbeitsbereiche: Farben und Aktionszustände

Der aktuelle dunkle Farbsatz liegt zentral in `app/design_tokens.h`. Farben
benennen ihre Aufgabe (Text, Fläche, Auswahl, Primäraktion, Fehler), nicht den
Ort einer einzelnen Schaltfläche. Abstände und Radien sind ebenfalls dort
benannt. Das ist die Grundlage für spätere helle und kontrastreiche Varianten;
derzeit ist nur der dunkle Satz implementiert.

Eine Primäraktion ist die unmittelbar ausführbare nächste Handlung im jeweiligen
Arbeitsbereich. Ein deaktivierter Knopf erhält keine Primärfarbe. Status wird
zusätzlich als Text angezeigt, damit Farbe allein keine Information trägt.

| Bereich | Zustand | Primäraktion | Rückmeldung |
| --- | --- | --- | --- |
| Workspace | Kein Ordner | Ordner öffnen | Leerer Workspace |
| Workspace | Ordner ohne Projekt | Datei auswählen oder neues Projekt anlegen | Ordner geöffnet |
| Projektmanager | Eingaben vollständig | Projekt anlegen | Zielordner, Name, Vorlage, Sprachen |
| Entwickeln | Projekt geladen, keine laufende Aufgabe | Build | Quelldatei und Buildstatus |
| Simulieren | Projekt geladen, Build erforderlich | Build | Build erforderlich |
| Simulieren | Gebaut, kein aktiver Lauf | Neuer Lauf | Bereit |
| Simulieren | Lauf aktiv und steuerbar | Pause | Simulation läuft |
| Simulieren | Lauf pausiert und steuerbar | Fortsetzen | Pausiert |
| Simulieren | Runner startet oder stoppt | keine | Vorbereitung oder Stoppen im Status/Protokoll |
| Auswerten | Lauf vorhanden, Leerlauf | Analyse starten | Anzahl der Läufe und Messpunkte |
| Auswerten | Analyse läuft oder keine Daten vorhanden | keine | Verarbeitungsstatus oder leerer Zustand |

Die Workspace-Struktur aus [Designrichtung](design-direction.md) ist teilweise
angebunden. Die Dateivorschau ist schreibgeschützt; vollständige Dateibaum- und
Dokumentverwaltung sind noch ausstehend.
