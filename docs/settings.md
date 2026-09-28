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

Die Auswahl bleibt bei Bewegungen erhalten und gilt nur für den aktuellen Lauf.
Objekte mit einer von null verschiedenen `ps_object.id` werden als **ID …**
angezeigt. Ihre Auswahl folgt der ID auch bei geänderter Listenposition oder Form.
Entfernte IDs werden vergessen; ein später erneut eingefügtes Objekt erscheint sichtbar.
Ein neuer Lauf setzt die Auswahl zurück. Für anonyme Objekte (`id = 0`) gilt
weiter die Listenposition; eine geänderte Anzahl oder Formenfolge setzt deren
Auswahl zurück. Eine Szenenhierarchie ist noch nicht implementiert.
Messwerte und Simulation werden durch Ausblenden nicht
verändert; bei langen Listen kann die Seitenleiste gescrollt werden.

Vektoren, Flugbahnen, Punkte, Beschriftungen, Raster/Achsen und orthografische Ansicht
lassen sich ein- oder ausblenden. Diese Auswahl entspricht den Schaltern im
Simulationsinspector und gilt für die nächste Sitzung. Ausgeblendete Objekte bleiben
in den Szenendaten erhalten; die physikalische Rechnung verändert sich dadurch nicht.

Die senkrechte Trennlinie rechts neben der Seitenleiste verändert deren Breite.
Die waagerechte Linie über dem geöffneten Protokoll verändert seine Höhe.
Mit gedrückter linker Maustaste ziehen; die hervorgehobene Linie zeigt den Griff.
Die Bereiche haben Mindest- und Höchstgrößen, damit die Arbeitsfläche nutzbar bleibt.
Dies sind feste, vergrößerbare Panels; frei verschiebbare oder ablösbare Fenster
sind noch nicht implementiert.

Physim merkt sich bei normalem Beenden Fenstergröße, maximierten Zustand, Panelgrößen,
Protokollsichtbarkeit, den aufgeklappten Darstellungsinspector und den letzten
Hauptarbeitsbereich. Die Fenstergröße wird beim Start auf den verfügbaren Bildschirm
begrenzt; die unterstützte Mindestgröße bleibt 1080 × 740. Die Fensterposition und
weitere Inspectorgruppen werden derzeit nicht wiederhergestellt. Ein explizit geöffnetes
Projekt beginnt im Editor. Kameraausrichtung, Zoom und Szenenfokus bleiben sitzungsbezogen.

## Übernehmen, Abbrechen und Standardwerte

**Übernehmen** speichert den Entwurf und wendet ihn sofort an.
**Abbrechen** verwirft den Entwurf. Direktes Ziehen der Panelgrenzen ist eine unabhängige
Layoutänderung und wird dadurch nicht zurückgenommen.
**Standardwerte** setzt den Entwurf auf die Vorgaben zurück; erst **Übernehmen**
speichert ihn. Die aktuelle Fenstergröße und der maximierte Zustand bleiben dabei erhalten.

Einstellungen liegen im persönlichen Anwendungsordner, den SDL für Physim bereitstellt,
als `preferences.bin`. Sie gehören weder zum Projekt noch zu den Laufmetadaten.
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
