# Native Korrekturen nach den Nutzeraufnahmen

Die [sieben Nutzeraufnahmen](USER-REVIEW-2026-10-21.md) zeigen tatsächliche
Umsetzungslücken. Der folgende geprüfte Teilstand korrigiert gemeinsame
Auswahlsymbole, Szenenzeilen, Workspace-Chevrons und Inspektorabschnitte.
Er erfüllt noch keine vollständige Designabnahme der Oberfläche.

## Sichtbare Änderungen

- Auswahlfelder verwenden schlanke Chevrons und behalten ihre bisherigen
  Aktionen. Die kurze Profilwahl und die Tempoauswahl erhalten jeweils passende
  Innenabstände; negative Symbolhöhen werden vermieden.
- Workspace-Ordner erhalten die schmalen Aufklapppfeile der Vorlage.
- Szenenzeilen besitzen getrennte Auswahl-, Aufklapp- und Sichtbarkeitsflächen.
  Formzeichen folgen den tatsächlichen Objektarten. Vollständige Namen umbrechen;
  begrenzte Einrückung erhält dafür Platz. Gruppen, Koordinatenrahmen, Objekt-IDs
  und Sichtbarkeitsvererbung bleiben an den realen Szenendaten gebunden.
- Inspektorabschnitte verwenden die vorgegebenen einpixeligen Trennlinien und
  eine an 43 logischen Pixeln ausgerichtete Zusammenfassung, die bei großer
  Schrift mitwachsen darf. Plus/Minus bleibt erhalten: Es gehört zur Vorlage.
- Modellmeldungen und Einstellungen liegen in derselben Inspektor-Scrollfläche.
  Modellfehler sind vollständig umbrechender Text statt eines kleinen eigenen
  Scrollfelds. Der übergroße Leerraum vor dem Szenenbaum ist reduziert.

| Tatsächliche native Aufnahme | Bild |
| --- | --- |
| Windows, Hierarchie mit Formzeichen und getrennten Sichtbarkeitsflächen | [Bild](user-components-2026-10-22/windows-hierarchy.png) |
| Windows, Workspace mit schmalen Chevrons | [Bild](user-components-2026-10-22/windows-workspace.png) |
| Windows, große Schrift nach tatsächlichen Sichtbarkeitsaktionen | [Bild](user-components-2026-10-22/windows-visibility-large.png) |
| Linux, großer schmaler Messlaborzustand | [Bild](user-components-2026-10-22/linux-laboratory-light-large-narrow.png) |
| Inspektor im Neubauzustand mit geschlossenen Abschnitten | [Bild](user-components-2026-10-22/windows-model-build.png) |
| Große Schrift mit echter Fehlermeldung im synthetischen Sichtbarkeitsfall | [Bild](user-components-2026-10-22/windows-error-large.png) |

Diese PNGs sind verlustfreie Formatumwandlungen nativer BMP-Aufnahmen. Quelle,
Maße, Bildhash und vollständige RGB-Gleichheit sind im Prüfmanifest erfasst.
Sie sind weder Browserbilder noch eine Abnahme sämtlicher Vorlagepixel.
Die historisch benannte Datei `model-values-build-error.bmp` zeigt bereits den
anschließenden Neubau. Ihr Dateiname ist kein visueller Fehlernachweis; sie wird
hier ausdrücklich als Buildzustand geführt. Der Fehlerzustand selbst wird im
C-/Physim-Ablauf logisch geprüft. Dessen genauer Aufnahmezeitpunkt bleibt für
einen echten visuellen Buildfehlernachweis zu korrigieren.

## Fehlernachweise und Korrekturen

Der erste neue Baumstand verlor für außerhalb des Ausschnitts liegende Zeilen
seine Layoutziele. Die echte Hierarchieprobe blieb nach Tastaturauswahl beim
Scrollen stehen. Vollständige Layoutrechtecke bleiben jetzt erhalten, während
Maus- und Assistive-Treffflächen weiter am sichtbaren Ausschnitt begrenzt sind.
Die unveränderten Szenendaten und aufgezeichneten Zustände werden weiterhin
per CRC, Picking und Sichtbarkeitsaktionen geprüft.

Die erste Chevron-Anpassung kollidierte mit bestehenden Innenabständen der
kurzen Profil- und Tempoauswahl. Die gemessenen nativen Bilder zeigten dadurch
ein umgedrehtes Symbol. Die jeweiligen Feldabstände sind korrigiert.

Die ältere Sichtbarkeitsprobe wartete außerdem auf Steuerelemente innerhalb der
anfangs geschlossenen Messlabor-Kameragruppe. Eine tatsächlich ausgeführte
Gegenprobe mit der vorhandenen älteren Release-Datei scheitert ebenfalls in
dieser Phase. Der erste direkte Starter dieser Gegenprobe scheiterte bereits
an seinem Arbeitsverzeichnis; er ist kein App-Gegenbeweis. Die aktuelle Fixture
öffnet die echten Abschnitte ausdrücklich und verlangt vorhandene Zielrechtecke.
Kamera-, Masken-, Geometrie- und Unverändertheitsprüfungen bleiben erhalten.

Danach zeigte das kleine Fenster die echte Engstelle des feststehenden
Modellbanners. Der gemeinsame Scrollbereich behebt dies bei Standardschrift.
Mit großer Schrift blieb ein weiterer Fehler: Das kleine innere Fehlerfeld
bewegte sich unter den stationären Mausradzeiger und fing dessen Eingaben ab.
Eine Diagnose im eigenen Linux-Prozess bestätigt zunächst Schritte von 14 px
und anschließend einen Stillstand bei 56 px, obwohl das Ziel noch weit außerhalb
des Ausschnitts liegt. Die abschließende Korrektur entfernt genau dieses innere
Scrollfeld; dieselbe unverändert strenge große Sichtbarkeitsprobe besteht danach
auf Windows und Linux. Die Deadlines wurden nicht erhöht.
[Aufgezeichnete fehlerhafte innere Scrollfläche](user-components-2026-10-22/counter-error-inner-group.png) ·
[Umgebrochene Fehlermeldung danach](user-components-2026-10-22/windows-error-large.png).

Die erste Debuggerprobe endete vor den Geometrieprüfungen in der Fixtureeinrichtung.
Eine weitere Probe am Funktionseinstieg liefert unplausible Parameterwerte und
wird nicht als Geometriebeleg verwendet. Die gültige Probe stoppt erst nach
der lokalen Einrichtung; Argument und tatsächlich gespeichertes Zielrechteck
stimmen in allen zwölf erfassten Beobachtungen überein. Unter dem Debugger ist
Leakprüfung deaktiviert und der eigene Prozess wird absichtlich beendet.
Dies ist Diagnostik, kein bestandener App-/Sanitizerlauf. Die normalen finalen
Tests laufen davon getrennt mit Sanitizern.

## Tatsächlich ausgeführter Abschluss

Quellstand **user-components-4**, Basis `6231003`. Windows/MSVC/ASan besteht
**6/6 Gruppen**, Alpine-Dev/GCC/ASan/UBSan **7/7 Gruppen**. Beide führen
`plot_workflow`, `workspace_tree_workflow`, `hierarchy_workflow`,
`scene_visibility_16`, `scene_visibility_22` und `model_values_workflow` aus.
Die Modellprüfung kompiliert echte C- und Physim-Projekte einschließlich
vorgemerkter neuerer Änderungen und tatsächlicher Buildfehler.
Linux führt zusätzlich den vollständigen Messlaborablauf in Hell, Dunkel,
Kontrast, großer schmaler Ansicht und weiteren tatsächlichen Modellen aus.

Die vollständige Windows-Messlaborgruppe bleibt separat offen: Ihr früherer
Lauf wartet bei Phase 0 vergeblich auf Windows-Tastaturfokus. Sie gehört deshalb
nicht zu den sechs finalen Windows-Erfolgen. Ihre Ablehnung bleibt im Manifest
erhalten. Die eigenen SDL-Eingabeproben sind keine externe Screenreader- oder
physische Monitor-/DPI-Abnahme. Eine aktuelle macOS-Ausführung fehlt.
Zehn fremde Accessibility-Änderungen sind über alle vier Stände unverändert
separat erfasst; die lokalen Apps enthalten sie. Dies ist kein Clean-Checkout-Test.

[Quellen, Programme, Ausführungen, Fehlerläufe und Bildhashes](user-components-2026-10-22/validation.json) ·
[Abschlussaudit](user-components-2026-10-22/audit.json).

## Weiterhin offen

Der große Analysebericht verwendet weiterhin seine alte Bedien-/Exportanordnung.
Der freigegebene native Exportdialog nach ADR 010 ist noch anzubinden. Zoomaktionen,
Tooltips, Pfadanzeige, weitere Kontrollformen und alle Zustände müssen weiter
direkt gegen die Vorlage abgeglichen werden. Simulationsleiste, Szenenaktionen,
vollständiger Inspektorinhalt und Zeitleiste benötigen weitere Angleichung.
Die übrige Oberfläche einschließlich F1 sowie die vollständige Tastatur-,
Assistive-, Größen- und Plattformabnahme bleiben Teil des ursprünglichen
[Gesamtauftrags](NATIVE-FIDELITY.md). Für diese noch fehlende Umsetzung wird
keine technische Unmöglichkeit behauptet und keine volle Punktzahl vergeben.
