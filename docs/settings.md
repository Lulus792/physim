# Einstellungen und Fensteraufteilung

**Einstellungen** in der oberen Menüzeile oder **Ctrl+,** öffnet die persönliche Darstellung.
Ein Wechsel hierhin lässt Editorinhalte, Simulation und Analyse im Hintergrund bestehen.
Das geöffnete Protokoll wird während der Einstellungsansicht vorübergehend eingeklappt,
damit die Bedienelemente auch im kleinen Fenster Platz haben.

## Tastaturbedienung

Tab beginnt mit der UI-Schriftgröße und führt über Übernehmen, Abbrechen,
Standardwerte, Darstellung, Codeschrift, Sicherungsintervall und die sieben
Ansichtsoptionen. Shift+Tab geht zurück; am Ende beginnt der Pfad wieder von
vorn. Ein sichtbarer Rahmen markiert die aktive Gruppe, die bei Bedarf in den
sichtbaren Bereich scrollt. Pfeiltasten wechseln die Werte einer Auswahlgruppe,
Leertaste oder Enter aktiviert eine Option oder Schaltfläche. Escape verwirft
den Entwurf und kehrt zur vorherigen Arbeitsfläche zurück. F10 öffnet weiterhin
die Hauptmenüs. Ein Mausklick setzt die Bedienung mit dem Zeiger fort.

## Schrift und Sicherung

Die Code-Schriftgröße lässt sich zwischen 12, 14, 16, 18, 20 und 22 logischen Pixeln wählen.
Eine Vorschau zeigt die neue Größe. Sie gilt nach **Übernehmen** für die
Projekteditoren und das Protokoll. Ohne eigenen Lesezoom übernimmt das Handbuch
diese Größe ab 16 Pixeln auch für seine Codeblöcke; 12/14 Pixel im Editor verkleinern
das Handbuch nicht unter seine Mindestlesegröße von 16 Pixeln.
Die **Schriftgröße der Oberfläche** ist davon unabhängig wählbar: 16, 18, 20
oder 22 logische Pixel gelten für Navigation, Menüs, Beschriftungen, Diagramme und den
normalen Text im Handbuch. Auch ein bereits geöffnetes Handbuch übernimmt nach
**Übernehmen** die neuen Größen, solange kein eigener Lesezoom aktiv ist.
Die Plus-/Minus-Schaltflächen im Handbuch wählen unabhängig davon eine gemeinsame
Lesegröße für Text und Code bis 32 Pixel. Die mittlere Größenanzeige setzt diesen
Lesezoom zurück; dann gelten wieder die getrennten UI- und Codeschriftgrößen.
Kleine Zeilen und lange Schaltflächenbeschriftungen
passen sich an; umfangreiche Ansichten bleiben scrollbar. Abbrechen erhält die
bisherige Größe. Standardwerte wählen 12 Pixel für Code und 16 Pixel für die
Oberfläche. Die Werkbank verwendet für ihre kompakten Menüs, Panelköpfe und
Statusangaben abgestufte Schriftrollen; größere UI-Einstellungen skalieren sie
entsprechend. Bereits gespeicherte Schriftgrößen werden nicht zurückgesetzt.

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

Unter Windows veröffentlicht die native UI-Automation-Anbindung jetzt
Schaltflächen, Checkboxen, Auswahlgruppen und Slider mit Namen, Rollen,
Zuständen und Fokus. Ein unabhängiger Client hat die tatsächlichen
Einstellungen bedient: Checkbox umschalten, Schrift-/Theme-Auswahl ändern
und einen Button fokussieren. F1 veröffentlicht außerdem den vollständigen
Lesetext sowie Suchfelder, Links und Tabellen über die native Anbindung.
Die [Accessibility-Dokumentation](accessibility.md) nennt die vorhandenen
Schnittstellen und ihre Prüfgrenzen. Praktische Screenreader-Aufgaben bleiben
offen; die nativen Schnittstellen allein belegen ihre Bedienbarkeit nicht.

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
| Alt + Pfeiltasten | Kamera drehen; im 2D-Modus innerhalb der Ebene verschieben |
| Alt + Shift + Pfeiltasten | Kameraziel nach rechts/links/oben/unten verschieben |
| Alt + Bild auf / Bild ab | Heran-/herauszoomen |
| Alt + Pos1 | Freie Standardkamera einschließlich Projektion wiederherstellen |
| Alt + 2 | 2D-Modus ein-/ausschalten |
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
rechts, oben und unten teilen dessen Fläche. Auch die Titelzeile bildet eine
Tabgruppe, und die seitlichen beziehungsweise unteren Ränder dienen als Teilungsziele.
Im Arbeitsbereich bewegt der Punktgriff links das Panel; die Datei-Tabs daneben
wechseln und sortieren Dateien. Die Leisten einschließlich der Schließen-Icons
haben eine durchgehende, dezent abgesetzte Oberfläche.
Beim Loslassen außerhalb dieser Ziele
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
Das aktuelle Format 5 speichert den vollständigen Panelbaum, aktive Paneltabs,
ausgeblendete Panels und frei platzierte Rechtecke. Dateien der Formate 1 und 2
bleiben lesbar und erhalten die Standardanordnung; Format 1 verwendet die dunkle Palette.
Format 3 behält seinen fünfteiligen Panelbaum, aktive Tabs und freie Rechtecke
unverändert. Sein zusätzlicher Inspektor beginnt ausgeblendet und lässt sich über
**Ansicht → Inspektor anzeigen** rechts neben dem Arbeitsbereich öffnen.
Format 4 speichert den Vier-Panel-Baum und die unabhängige Inspektorbreite.
Format 5 ergänzt die unabhängige UI-Schriftgröße; Formate 1–4 laden dafür
16 Pixel, ohne ihre bisherigen Einstellungen zu verändern.
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


## Anzeigeeinheiten für Messkanäle

In **Simulieren** öffnet **Einheit …** neben dem ausgewählten Kanal die Auswahl.
In **Auswerten → Messdaten** heißt die Schaltfläche **Anzeigeeinheit …**.
Passende Presets berücksichtigen die sieben SI-Dimensionen des Kanals, etwa
`m`, `cm`, `mm`, `km` für eine Länge oder `m/s` und `km/h` für eine Geschwindigkeit.
Eigene Symbole mit bis zu 63 UTF-8-Bytes und ein positiver endlicher Faktor sind
ebenfalls möglich. Der Faktor gibt SI-Einheiten pro Anzeigeeinheit an:
`Anzeige = SI / Faktor`. Für Zentimeter ist er `0.01`; die Vorschau zeigt einen
aktuellen gültigen Wert oder ersatzweise `1 SI`. Einheiten mit Offset, etwa Grad
Celsius, sind hier nicht unterstützt. Symbole enthalten keine Steuerzeichen
oder Leerzeichen am Anfang und Ende.

**Anwenden** speichert die Auswahl. **SI wiederherstellen** entfernt sie für diesen
Kanal; **Abbrechen** oder Escape verwirft die Eingabe. Die Auswahl wirkt auf
Live-Werte, Live-Kurven, Messdaten-Diagramme und Statistiken des gesamten Laufs.
Die Statistiktabelle lässt sich bei schmalem Platz horizontal scrollen;
abgekürzte Kanalnamen, Symbole oder Zahlen sind per Tooltip vollständig lesbar.
Positive Skalierung erhält den ausgewählten Diagrammausschnitt. Messausfälle
und nicht fällige Sensorwerte bleiben ausgeschlossen. Rohdaten, Modellparameter,
Geometrie und CSV-Export ändern sich nicht. Analyseberichte verwenden weiterhin
die vom Analysecode angegebenen Einheiten.

Überlauf und Unterlauf vorhandener Werte oder Statistiken verhindern das
Anwenden. Können spätere Live-Werte nicht mehr dargestellt werden, erscheint
**nicht darstellbar**; betroffene Kurvenwerte werden als Lücken mit einem Hinweis
gezeigt. Diese Prüfung ändert die Rohmessung nicht.

Bis zu 64 persönliche Auswahlen werden als `channel-units.bin` neben
`preferences.bin` gespeichert. Kanalname und SI-Dimensionen identifizieren eine
Auswahl; derselbe Name mit anderer Dimension erhält sie nicht. Unbekannte oder
beschädigte Dateien bleiben erhalten. Die Auswahl zeigt den Fehler und sperrt
Änderungen, bis **Datei bewusst zurücksetzen** den Katalog ausdrücklich leert.
Speichern schreibt und schließt zunächst eine temporäre Datei und ersetzt dann
den Katalog. Schreibfehler erhalten Datei und aktive Auswahl. Bloßes Öffnen und
Beenden schreiben ihn nicht. Bei mehreren Instanzen gewinnt der zuletzt
vollständig gespeicherte Stand; Stromausfälle beim Austausch sind nicht abgesichert.
