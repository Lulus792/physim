# Einstellungen und Fensteraufteilung

**Einstellungen** in der oberen Menüzeile oder **Ctrl+,** öffnet die persönliche Darstellung.
Ein Wechsel hierhin lässt Editorinhalte, Simulation und Analyse im Hintergrund bestehen.
Das geöffnete Protokoll wird während der Einstellungsansicht vorübergehend eingeklappt,
damit die Bedienelemente auch im kleinen Fenster Platz haben.

## Schrift und Sicherung

Die Code-Schriftgröße lässt sich zwischen 16, 18, 20 und 22 logischen Pixeln wählen.
Eine Vorschau zeigt die neue Größe. Sie gilt nach **Übernehmen** für beide C-Editoren,
Codeblöcke im Dokumentationsbrowser und das Protokoll. Navigation und normaler
Dokumentationstext behalten ihre Systemschriftgröße.

Automatische Sicherungen lassen sich alle 10, 30, 60 oder 120 Sekunden erstellen.
Standard sind 30 Sekunden. Nach dem Übernehmen beginnt das neue Intervall;
gespeichert wird nur bei ungesicherten Änderungen. Die Sicherung umfasst weiterhin
beide C-Editoren gemeinsam. Sie ersetzt das bewusste Speichern nicht.
Details stehen unter [Autosave und Wiederherstellung](autosave.md).

## Darstellung und Panelgrößen

**Darstellung** bietet **Dunkel**, **Hell** und **Hoher Kontrast**. Nach
**Übernehmen** verwenden Navigation, Editor, Protokoll, Diagramme und das
Dokumentationsfenster die gewählte Palette. Auch ein bereits geöffnetes
Hilfefenster wechselt mit. Die Auswahl bleibt beim nächsten Start erhalten;
**Abbrechen** bewahrt die bisherige Darstellung. **Standardwerte** wählt Dunkel
im Entwurf aus. Szenenobjekte behalten ihre eigenen Farben.

Hoher Kontrast verwendet helle Schrift auf schwarzem Grund, gelbe Hervorhebungen
und sichtbare Rahmen um Eingabefelder und Schaltflächen. Die neuen Paletten
prüfen Textkontraste rechnerisch; vollständige Tastatur- und Screenreader-Bedienung
ist weiterhin ein offenes Projektziel.

Ein **Linksklick in die Szene** wählt das vorderste sichtbare Objekt unter dem
Mauszeiger. Der Inspektor zeigt dessen ID beziehungsweise Listenposition und
bietet **Auswahl ausblenden**. Ein Klick auf den Hintergrund hebt die Auswahl auf.
Beschriftungen können über ihre Textfläche ausgewählt werden. Raster und Achsen
sind nicht auswählbar; ausgeblendete Objekte werden nicht getroffen.
Die Auswahl folgt vorhandenen Objekt-IDs beim Umsortieren und wird beim Entfernen
des Objekts oder bei einem neuen Lauf aufgehoben. Ohne ID gilt die Listenposition.
Rechte und mittlere Maustaste bedienen weiterhin die Kamera.

Im Arbeitsbereich **Simulation** stehen zusätzlich diese Tastenkürzel bereit:

| Kürzel | Aktion |
| --- | --- |
| Alt + Pfeiltasten | Kamera drehen |
| Alt + Shift + Pfeiltasten | Kameraziel nach rechts/links/oben/unten verschieben |
| Alt + Bild auf / Bild ab | Heran-/herauszoomen |
| Alt + Pos1 | Standardkamera einschließlich Projektion wiederherstellen |
| Alt + N / Alt + P | Nächsten/vorherigen eingeblendeten Szeneneintrag auswählen |
| Alt + H | Ausgewählten Eintrag ausblenden |

Die Objektauswahl läuft zyklisch durch die Szenenliste und überspringt ausgeblendete
Formen, leere Beschriftungen und Alpha 0. Sie kann auch Einträge außerhalb des
aktuellen Kamerabilds auswählen. Auswahl und Ausblenden folgen vorhandenen IDs.
Gehaltene Pfeil-/Zoomtasten wiederholen ihre Aktion; Auswahl und Ausblenden lösen
pro Tastendruck einmal aus. Editor, Analyse und Wiederherstellungsdialog verwenden
diese Kürzel nicht. AltGr beziehungsweise Ctrl+Alt wird nicht als Kameraaktion
behandelt. Diese Ergänzung ersetzt noch keine vollständige Tastaturbedienung der App.

Im Simulationsinspektor lassen sich unter **Szeneneinträge** einzelne Einträge
ausblenden. Die Nummer bezeichnet ihre Position in der vom Experiment gelieferten
Liste; Form und gegebenenfalls Beschriftung helfen bei der Zuordnung.
**Alle einblenden** hebt diese Auswahl auf. Die allgemeinen Darstellungsschalter
für Vektoren, Pfade und Beschriftungen gelten zusätzlich.

Bei Szenen mit Gruppen oder Elternbeziehungen erscheint ein **Szenenbaum**.
Der Pfeil links klappt Untereinträge auf oder zu; dies ändert nur den Baum.
Das Sichtbarkeitskästchen steuert den Eintrag und seine Nachfahren. Ein Kind
behält seine eigene Wahl, wenn ein Elternknoten ausgeblendet wird. Deshalb kann
ein ausgeblendetes Kind weiterhin angekreuzt und mit grauer Schrift erscheinen;
beim Wiedereinblenden des Elternknotens gilt seine bisherige Wahl erneut.
**Alt + N/P** wählt auch sichtbare Gruppen. Ein neu ausgewähltes Objekt öffnet
seinen Weg im Baum. Gruppen enthalten keine Geometrie, und Eltern ändern keine
Weltkoordinaten. Neue Pendelvorlagen zeigen die Aufhängung und die bewegte Masse
in einer gemeinsamen Hierarchie. Die aufgezeichneten Beziehungen bleiben beim
Wiederöffnen und in der Zeitleiste erhalten.

Die Auswahl bleibt bei Bewegungen erhalten und gilt nur für den aktuellen Lauf.
Objekte mit einer von null verschiedenen `ps_object.id` werden als **ID …**
angezeigt. Ihre Auswahl folgt der ID auch bei geänderter Listenposition oder Form.
Entfernte IDs werden vergessen; ein später erneut eingefügtes Objekt erscheint sichtbar.
Ein neuer Lauf setzt die Auswahl zurück. Für anonyme Objekte (`id = 0`) gilt
weiter die Listenposition; eine geänderte Anzahl oder Formenfolge setzt deren
Auswahl zurück. Auch der aufgeklappte Zustand benannter Baumzweige folgt ihren IDs;
er ist sitzungsbezogen.
Messwerte und Simulation werden durch Ausblenden nicht
verändert; bei langen Listen kann die Seitenleiste gescrollt werden.

Vektoren, Flugbahnen, Punkte, Beschriftungen, Raster/Achsen und orthografische Ansicht
lassen sich ein- oder ausblenden. Diese Auswahl entspricht den Schaltern im
Simulationsinspector und gilt für die nächste Sitzung. Ausgeblendete Objekte bleiben
in den Szenendaten erhalten; die physikalische Rechnung verändert sich dadurch nicht.

## Panelanordnung

**Seitenleiste**, **Arbeitsbereich**, **Inspektor** und **Protokoll** sind verschiebbare Panels.
Die Seitenleiste enthält Dateibaum, Dokumente und Projektzugänge. Der eigene
Inspektor zeigt Szenenbaum, Auswahl, Kamera, Darstellung, Lauf- und
Experimentparameter sowie Ressourcenlimits. Er lässt sich unabhängig vom
Dateibaum platzieren und ist auch beim Bearbeiten des Codes erreichbar.
Der Arbeitsbereich zeigt weiterhin
den ausgewählten Editor, die Simulation oder die Auswertung. Die drei globalen
Arbeitsbereich-Tabs und die Menüzeile bleiben im Fensterkopf.

Ziehe eine Paneltitelzeile mit gedrückter linker Maustaste. Über einem anderen
angedockten Panel erscheinen fünf Ziele: Die Mitte bildet eine Tabgruppe; links,
rechts, oben und unten teilen dessen Fläche. Beim Loslassen außerhalb dieser Ziele
bleibt das Panel frei im Hauptfenster stehen. Die Vorschau zeigt seine Position.
**Escape** bricht das Verschieben ab. Ein Klick auf einen Paneltab wählt dessen Inhalt.

Trennlinien zwischen angedockten Panels verändern die Aufteilung. Frei platzierte
Panels lassen sich über ihre Titelzeile verschieben und mit dem Griff rechts unten
vergrößern oder verkleinern. Die Position wird auf das Hauptfenster begrenzt;
bei einer kleineren Fenstergröße bleibt das Panel erreichbar. Der Arbeitsbereich
hat als frei platziertes Panel mindestens 640 × 580 logische Pixel. Kleine
angedockte Bereiche lassen sich vertikal scrollen.

Das **×** in einer Titelzeile blendet das aktive Panel aus. **Ansicht → Seitenleiste
anzeigen**, **Arbeitsbereich anzeigen** und **Inspektor anzeigen** und **Protokoll einblenden** stellen es wieder
her. Ein globaler Arbeitsbereich-Tab zeigt den Arbeitsbereich ebenfalls wieder an.
**Ansicht → Panelanordnung zurücksetzen** stellt Seitenleiste links, Arbeitsbereich
in der Mitte, Inspektor rechts und Protokoll darunter wieder her. Das Umordnen lässt ungespeicherten
Quelltext und laufende Versuche bestehen. Frei platzierte Panels bleiben Teil des
Hauptfensters; separate Betriebssystemfenster bleiben ein weiteres Produktziel.

Physim merkt sich bei normalem Beenden Fenstergröße, maximierten Zustand, Panelanordnung,
ausgewählte Paneltabs, Positionen und Größen frei platzierter Panels,
Protokollsichtbarkeit, den aufgeklappten Darstellungsinspector und den letzten
Hauptarbeitsbereich. Die Fenstergröße wird beim Start auf den verfügbaren Bildschirm
begrenzt; die unterstützte Mindestgröße bleibt 1080 × 740. Die Fensterposition und
weitere Inspectorgruppen werden derzeit nicht wiederhergestellt. Ein explizit geöffnetes
Projekt beginnt im Editor. Kameraausrichtung, Zoom und Szenenfokus bleiben sitzungsbezogen.

## Übernehmen, Abbrechen und Standardwerte

**Übernehmen** speichert den Entwurf und wendet ihn sofort an.
**Abbrechen** verwirft den Entwurf. Verschieben, Ausblenden und Größenänderung von
Panels sind unabhängige Layoutänderungen und werden dadurch nicht zurückgenommen.
**Standardwerte** setzt den Entwurf auf die Vorgaben zurück; erst **Übernehmen**
speichert ihn. Fenstergröße, maximierter Zustand, Panelbaum und freie Rechtecke bleiben dabei
erhalten. Die Standardbreiten und Protokollhöhe werden zurückgesetzt; der
vollständige Panelbaum wird über **Ansicht** zurückgesetzt.

Einstellungen liegen im persönlichen Anwendungsordner, den SDL für Physim bereitstellt,
als `preferences.bin`. Sie gehören weder zum Projekt noch zu den Laufmetadaten.
Das aktuelle Format 4 speichert den vollständigen Panelbaum, aktive Paneltabs,
ausgeblendete Panels und frei platzierte Rechtecke. Dateien der Formate 1 und 2
bleiben lesbar und erhalten die Standardanordnung; Format 1 verwendet die dunkle Palette.
Format 3 behält seinen fünfteiligen Panelbaum, aktive Tabs und freie Rechtecke
unverändert. Sein zusätzlicher Inspektor beginnt ausgeblendet und lässt sich über
**Ansicht → Inspektor anzeigen** rechts neben dem Arbeitsbereich öffnen.
Format 4 speichert den Vier-Panel-Baum und die unabhängige Inspektorbreite.
Alle normalen Physim-Instanzen desselben Benutzerkontos teilen diese Datei;
bei gleichzeitiger Nutzung gewinnt der zuletzt vollständig gespeicherte Stand.

Beim Speichern wird zuerst eine neue temporäre Datei vollständig geschrieben und
geschlossen, dann die bisherige ersetzt. Ein Fehler ersetzt die alte Datei nicht.
Beschädigte oder unbekannte Dateiversionen führen zu Standardwerten mit einem Hinweis
in den Einstellungen. Die ursprüngliche Datei bleibt bis zum ausdrücklichen
Übernehmen erhalten. Ein Stromausfall während des Dateiaustauschs ist nicht abgesichert.
App-Tests verwenden eigene Dateien im Testordner und berühren die persönlichen
Einstellungen nicht.

Der letzte Workspace wird separat in `workspace.bin` gespeichert. Hauptordner und
zusätzliche Pfade lassen sich auf dem leeren Startbildschirm ausdrücklich wieder
öffnen oder vergessen. [Workspace wieder öffnen](workspace.md)


Bei einer neuen Anordnung sitzt der Inspektor rechts. Seine Trennlinie verändert
seine Breite unabhängig von der Seitenleiste. **Panelanordnung zurücksetzen**
kehrt zu dieser Anordnung zurück. Schließen, Wiederanzeigen, Tabgruppen und
Verschieben lassen Quelltexte und Runnerzustand bestehen; die Darstellung eines
Objekts lässt sich auch im frei platzierten Inspektor umschalten.

Wird der Simulationsbereich schmal, verteilen sich seine sechs Steuerungen auf
zwei Spalten. Die Zeitleiste zeigt ihre vier Schaltflächen und den Schieberegler
auf getrennten Reihen. Die Szene bleibt mindestens 160 logische Pixel hoch;
kleinere Panelhöhen können vertikales Scrollen erfordern. Die globale Mindestgröße
1080 × 740 bleibt bestehen.

## Benannte Panelanordnungen

**Ansicht → Panelanordnungen …** öffnet die Verwaltung. Gib einen Namen ein und
wähle **Anordnung speichern**, um die aktuelle Aufteilung zu sichern. Bis zu acht
Anordnungen sind möglich. **Anwenden** stellt die ausgewählte Anordnung wieder her;
**Löschen** entfernt sie. Ein bereits vorhandener Name wird beim Speichern ersetzt.
Namen unterscheiden Groß- und Kleinschreibung und dürfen bis zu 63 UTF-8-Bytes
enthalten, ohne Steuerzeichen oder Leerzeichen am Anfang und Ende.

Gespeichert werden Panelbaum, aktive Tabs, verborgene Panels, freie Rechtecke,
Seitenleisten- und Inspektorbreite, Protokollhöhe und die aufgeklappte
Protokollansicht. Freie Rechtecke werden beim Anzeigen auf das aktuelle Fenster
begrenzt. Auch eine Anordnung mit ausschließlich verborgenen Panels lässt sich
über die Menüzeile wieder öffnen. **Schließen** oder Escape verlässt die Verwaltung.
Quelltexte, aktive Dokumente, Arbeitsbereich, Theme, Schriftgröße, Kamera,
Projektparameter, Lauf und Messdaten bleiben beim Anwenden erhalten.

Der persönliche Katalog liegt als `layouts.bin` neben `preferences.bin`.
Format 1 ist versioniert und mit einer CRC-Prüfsumme geschützt. Speichern, Löschen
und bewusstes Zurücksetzen schreiben jeweils eine temporäre Datei und ersetzen
die bisherige erst nach vollständigem Schreiben und Schließen. Bei einem
Schreibfehler bleiben der bisherige Katalog und die Datei erhalten. Das Beenden
oder bloße Anwenden schreibt den Katalog nicht. Die aktive Anordnung wird wie
bisher beim normalen Beenden in den Einstellungen gespeichert.

Beschädigte oder unbekannte Katalogdateien bleiben erhalten; die Verwaltung
zeigt den Ladefehler und sperrt Speichern, Anwenden und Löschen. Nur **Datei bewusst
zurücksetzen** ersetzt den Katalog durch eine leere Datei. Gleichzeitige Instanzen
teilen den persönlichen Katalog; der zuletzt vollständig gespeicherte Stand
gewinnt. Stromausfälle während des Dateiaustauschs sind nicht abgesichert.
Diese Anordnungen speichern keine Projekt- oder Workspace-Pfade. Mehrere benannte
Workspaces und separate Panelfenster bleiben weitere Produktziele.
