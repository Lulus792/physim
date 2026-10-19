# Kopierfertiger Auftrag: Simulieren nach Messlabor umarbeiten

Stand: 19. Oktober 2026. Diesen Text ab der Trennlinie vollständig in einen
umsetzenden Chat mit D:\Physim kopieren. Die Vorbereitung und die Browservorlage
ändern noch keinen nativen Anwendungscode. Der Nutzer hat ausdrücklich bestätigt,
dass **Simulieren** gemeint ist. Für **Auswerten** gibt es eine separate Übergabe
unter `docs/visuals/analysis-workspace/`; diese wird hier nicht ersetzt.

---

Arbeite in D:\Physim den bestehenden nativen Arbeitsbereich **Simulieren** auf
das ausgewählte Design **Messlabor** um. Implementiere es im vorhandenen
C-/SDL-/Nuklear-/OpenGL-Aufbau bis zur geprüften Fertigstellung.

Für die Informationen im Inspektor ist **B · Wertefelder** verbindlich ausgewählt:
Bezeichnung oberhalb eines größeren Werts, dezente Werteflächen und klar
zugeordnete Einheiten. Eigenschaften bei passender Breite zweispaltig, Eingaben
als gut lesbare Wertefelder. Nutze `inspector-fields-dark.png` und
`inspector-fields-light.png` als Detailreferenzen. Die Abschnittsstruktur bleibt
erhalten. A/C sind lediglich Vergleichsentwürfe, kein Auftrag für drei native Modi.

Lies zuerst AGENTS.md und D:/Skills/skills/AGENTS.md. Lies danach:

- docs/decisions/008-simulation-messlabor.md
- docs/visuals/simulation-designs/DESIGN.md
- docs/visuals/simulation-designs/IMPLEMENTATION.md
- docs/visuals/simulation-designs/MODEL-EDITING.md
- docs/design-direction.md
- docs/workspace.md

Öffne D:/Physim/docs/visuals/simulation-designs/index.html im Browser; dort ist
Messlabor der Standard. Maßgebliche Fensterbilder im selben Ordner sind
labor-dark.png, labor-light.png, labor-dark-curves-open.png und
labor-light-curves-open.png. Die erweiterte rechte Seite und ihre Gruppen
zeigt zusätzlich inspector-dark-overview.png bzw. inspector-light-overview.png.
Die lokale Vorlage funktioniert ohne Server. Erhalte die bestehende App-Shell
mit kompakter Menüzeile und den drei gleich breiten Arbeitsbereich-Tabs.

Verbindlicher Anfangszustand von Simulieren:

- Szenenbaum geschlossen, über die linke Bereichsaktion wieder erreichbar.
- Inspektor rechts offen, separat schließbar und wieder erreichbar.
- Messkurven ausgeblendet, Messwerte und Zeitleiste sichtbar.
- **Kurven anzeigen** ist ein beschrifteter Ein-/Aus-Schalter mit rundem Griff,
  ohne den früheren kleinen Richtungspfeil. Der Text bleibt in beiden Zuständen
  gleich. Der Zustand muss auch für Tastatur und native Accessibility erkennbar sein.

Die große Mitte gehört der echten Szene. Laufsteuerung und Tempo stehen darüber,
Messwerte sowie optionale Kurven darunter und die Zeitleiste im unteren Bereich.
Zugeklappte Kurven geben ihre Höhe an die Szene zurück. Szenenbaum, Inspektor und
Kurven sind unabhängig; ihre Schalter dürfen weder die Simulation verändern noch
Auswahl, Kamera, betrachtete Zeit oder jeweils andere Seitenzustände zurücksetzen.
Bewahre manuell gewählte Zustände beim Wechsel der Arbeitsbereiche. Wende den
Anfangszustand nicht in jedem Frame neu an und überschreibe keine gespeicherten
benutzerdefinierten Docking-Anordnungen stillschweigend. Eine neue Speicherung
dieser drei Sichtbarkeiten über Appstarts ist nicht beauftragt.

Zusätzliche Nutzeranforderung: Alle Inhaltsbereiche müssen in ihrer Größe
anpassbar sein. Breiten von Szenenbaum und Inspektor, Aufteilung von Szene und
Messbereich, Zeitleistenhöhe und Verhältnis der beiden Kurven über sichtbare
Trenner bedienen können. Vorhandenes Docking verwenden; Mindestgrößen und
Textskalierung berücksichtigen. Pointerdrag sowie passende Pfeiltasten und
Rücksetzen anbieten. Reale neue Rechtecke an Szene/Plots übergeben. Innerhalb der
Abschnitte das gewählte Wertefelder-Design B umsetzen; Schrift-/DPI-Einstellungen,
Einheiten, Validierung, Fokus und Bearbeitbarkeit erhalten.

Modellwerte wie Fadenlänge sollen im Inspektor bearbeitbar sein. Ergänze dafür
die Gruppe Modellwerte und den vollständigen Vertrag aus MODEL-EDITING.md:
Änderungen vormerken, Hinweis **Modellwerte geändert**, **Neu kompilieren** und
**Verwerfen** dauerhaft erreichbar. Bei aktivem/pausiertem Lauf ausdrücklich
**Stoppen & neu kompilieren** anbieten. Tippen verändert keinen laufenden Versuch.
Nach erfolgreichem Build einen neuen Lauf anbieten; erst dieser verwendet die
neuen Artefakte. Fehler und neuere Eingaben behalten, alte Workerabschlüsse nicht
auf neue Änderungen anwenden. Benutze eine explizite typisierte Bindung zum
Modell/Build bzw. zur unterstützten Quellendefinition; keine pauschalen
Textersetzungen in C-/Physim-Code. Gegebenenfalls einen eng begrenzten,
versionierten deklarativen Bindungsweg ergänzen. Die vorhandenen deklarierten
Laufparameter bleiben ein separater Vertrag und brauchen nicht pauschal einen Build.

Der Inspektor muss alle bestehenden fachlichen Funktionen behalten. Ordne sie in
aufklappbare Gruppen mit unabhängigem Scrollbereich; Kopf und Schließzugang bleiben
erreichbar. Die Vorlage enthält Objekt, Modellparameter, Laufeinstellungen,
Laufgrenzen, Messkanal und Einheit, Ereignisse, Kamera und Darstellung sowie
Aufzeichnung. Erhalte insbesondere:

- Modellabhängige Parameter mit Namen, Beschreibung, Einheit, Bereich, Standard,
  Eingabevalidierung und Standardaktion. Nur deklarierte Parameter zeigen.
- Zeitschritt, adaptive Schritte mit Start/Minimum/Maximum, Zufallsseed und bis
  zu 32 verschiedene Schritt-Haltepunkte nach dem bestehenden Validierungsvertrag.
- Speicher- und Zeitlimit mit bisheriger Bedeutung von 0 und bisheriger Sperrlogik.
- Messkanalauswahl, Anzeigeeinheiten, Gültigkeitsstatus und Hinweise auf
  nicht darstellbare Werte. Messwerte folgen der tatsächlich betrachteten Zeit.
- Ereignisse, wichtige Warnungen und Protokollzugang. Verdeckte Gruppen dürfen
  Warnungen und fehlende Runner-Antworten nicht unbemerkt machen.
- Vorhandene Objektwahl/Hierarchie, lokale und Weltkoordinaten, Sichtbarkeit,
  2D-/3D-Modus, orthografische Projektion, Ebenen, Blickrichtungen, Orbit, Neigung,
  Zoom, Maus-/Tastatursteuerung, Darstellungsfilter und Speichern der Projektvorgabe.

Szene/Kamera/Darstellung liegen im Bestand bei Simulieren teils links. Die neue
rechte Gruppe darf diese Funktionen als zusätzlichen Zugang zusammenführen;
beide Zugänge müssen denselben Zustand verwenden. Keine Funktion durch eine
aus der Demo abgelesene statische Zahl ersetzen. Masse und Integrator sind nicht
pauschal aus beliebigen Szenenobjekten ableitbar; nur echte vorhandene Metadaten
zeigen. Die Pendelparameter der Vorschau sind Beispiele, keine allgemeine Modell-API.
Native Bearbeitungsmöglichkeiten dürfen nicht zu bloßer Anzeige werden.

Die zwei optionalen Kurven müssen echte verfügbare Kanäle und deren Einheiten
verwenden; keine Browser-Pendelformeln oder festen Kanalnamen übernehmen.
Baue auf den vorhandenen begrenzten Live-Verläufen, Plotfunktionen und der
Zeitleiste auf. Weniger Kanäle, ungültige Messungen, Lücken, reduzierte Verläufe
und Aufzeichnungen ohne Szene benötigen verständliche Zustände. Aufzeichnung
und Live-Zustand bleiben getrennt: Scrubbing und Wiedergabe dürfen den Runner
nicht zurücksetzen. **Live** kehrt zur aktuellen Szene zurück. Der Zeitzeiger
der Kurven bezieht sich auf denselben Zeitpunkt wie Szene und Messwerte.

Übernimm die vorhandenen Aktionen für Build/Neuer Lauf, Run/Pause, Schritt, Stop,
Reset und Geschwindigkeit. Erhalte Initialisierung, HELLO-/Timeoutverhalten,
Abbruch, Limits und Reset in eine neue Messdatei. Bisherige Läufe bleiben erhalten.
Offline bedeutet nativ maximale Geschwindigkeit, nicht die 16×-Abkürzung der Demo.
Auch Analyseprojekte mit importierter Szene und ihre reine Wiedergabe müssen
weiter funktionieren. Entwickeln, Auswerten und F1 bleiben fachlich erhalten.

Arbeite vom aktuellen lokalen Stand aus. Die wichtigsten Ansatzpunkte stehen in
IMPLEMENTATION.md: app/design_ui.inc, app/main.c, app/scene_tree_ui.inc,
app/timeline_ui.inc, app/plot_ui.inc, app/channel_units_ui.inc, app/docking_ui.inc,
app/layout_ui.inc und die vorhandenen Accessibility-Adapter. Nutze die
vorhandenen semantischen Themes und UI-/Textskalierung. Erhalte Hochkontrast,
Tastaturbedienung, sichtbaren Fokus und dessen Rückkehr nach dem Schließen.
Vermeide voneinander abweichende Zustände in Inspector und Hauptfläche.

Keine Galerie, Demo-Zustandsauswahl, Beispielprotokolle, festen Pendelwerte oder
illustrative Kameraprojektion in die native App kopieren. Baue keinen WebView-
Ersatz. Der Browserprototyp ist eine Gestaltungsreferenz, kein Backend und kein
Beweis, dass die native Umsetzung bereits funktioniert.

Prüfe die Umsetzung anhand von IMPLEMENTATION.md mit passenden bestehenden und
gezielten neuen Regressionen. Führe echte Aufgaben in der nativen App durch,
vergleiche echte Fensterbilder mit den Referenzen und dokumentiere begründete
Abweichungen. Prüfe beide Themes, hohen Kontrast, große Schrift, Mindestfenster,
alle acht Kombinationen aus Szenenbaum/Inspektor/Kurven, Größenwechsel,
unterschiedliche Modelle, Lauf-/Fehlerzustände, historische Daten und Eingabegrenzen.
Linux/macOS nur nach tatsächlich ausgeführten Builds und Tests als geprüft melden.

Ändere fremde aktive Arbeiten nicht. Halte native Umsetzung, Tests, Fensterbilder
und verbleibende Grenzen im Designordner fest. Committe abgeschlossene geprüfte
Änderungen und pushe gemäß AGENTS.md nach origin und upstream ohne Force-Push.
Bereinige nur nachweislich entbehrliche eigene Ausgaben; Quellen, Referenzbilder,
Nutzdaten und die gemeinsame WSL-Distribution Alpine-Dev bleiben erhalten.
