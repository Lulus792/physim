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

**Seitenleiste**, **Arbeitsbereich** und **Protokoll** sind verschiebbare Panels.
Die Seitenleiste enthält Dateibaum und Inspektor; der Arbeitsbereich zeigt weiterhin
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
anzeigen**, **Arbeitsbereich anzeigen** und **Protokoll einblenden** stellen es wieder
her. Ein globaler Arbeitsbereich-Tab zeigt den Arbeitsbereich ebenfalls wieder an.
**Ansicht → Panelanordnung zurücksetzen** stellt Seitenleiste links, Arbeitsbereich
rechts und Protokoll darunter wieder her. Das Umordnen lässt ungespeicherten
Quelltext und laufende Versuche bestehen. Frei platzierte Panels bleiben Teil des
Hauptfensters; separate Betriebssystemfenster und ein eigenständiger Inspektor
sind weitere Produktziele.

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
speichert ihn. Fenstergröße, maximierter Zustand und Panelanordnung bleiben dabei
erhalten; die Anordnung wird über **Ansicht** zurückgesetzt.

Einstellungen liegen im persönlichen Anwendungsordner, den SDL für Physim bereitstellt,
als `preferences.bin`. Sie gehören weder zum Projekt noch zu den Laufmetadaten.
Das aktuelle Format 3 speichert den vollständigen Panelbaum, aktive Paneltabs,
ausgeblendete Panels und frei platzierte Rechtecke. Dateien der Formate 1 und 2
bleiben lesbar und erhalten die Standardanordnung; Format 1 verwendet die dunkle Palette.
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
