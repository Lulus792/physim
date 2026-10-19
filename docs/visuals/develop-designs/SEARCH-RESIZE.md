# Werkbank: schwebende Suche und größenverstellbare Bereiche

19. Oktober 2026. Verantwortlich: UI/UX-Design. Die ausgewählte Werkbank mit
Panelköpfen, kompakten Tabs und Workspace-Pfadlinien bleibt Grundlage. Zwei neue
Suchentwürfe vergleichen die Überlagerung des Codes; noch keine Nutzerwahl
zwischen diesen Suchentwürfen. Die native App wurde nicht verändert.

## Zwei Suchentwürfe

| Entwurf | Darstellung | Vorteil / Abwägung |
| --- | --- | --- |
| 01 · Suchinsel | Flache, bis zu 540 px breite Fläche oben rechts über dem Editor; Trefferzahl und kompakte Vor-/Zurückaktionen | Kurze Wege bei häufiger Suche; verdeckt einen breiteren, aber flachen Ausschnitt |
| 02 · Suchkarte | Bis zu 310 px breite Karte mit eigenem Kopf, Suchfeld und optionalem Ersetzen | Weniger Überdeckung in der Breite, klarere Gruppierung; braucht mehr Höhe |

Beide Entwürfe sind innerhalb der Codefläche absolut positioniert. Öffnen,
Schließen und Erweitern um „Ersetzen“ ändern weder Dateitabs, Breadcrumbs noch
Geometrie der Codefläche. Beim Öffnen bleibt auch die Scrollposition erhalten.
Das gezielte Navigieren zu einem Treffer darf anschließend innerhalb des
Editors scrollen. Die Suchfläche bleibt dabei oben rechts verankert.

Die zwei Vergleichsknöpfe außerhalb des App-Fensters öffnen die jeweilige Suche.
Beim normalen Start ist sie geschlossen; Suchinsel ist als Vergleichsstil
vorbelegt. Das Suchicon und Ctrl/Command+F öffnen denselben Entwurf. Enter bzw.
Pfeil nach unten navigiert vorwärts, Umschalt+Enter bzw. Pfeil nach oben rückwärts,
mit Rücksprung am Dokumentende/-anfang. Der kleine Aufklapppfeil blendet Ersetzen
ein. Escape schließt die Suche und stellt den Fokus am Suchicon wieder her.

Die Suche ist ein nichtmodaler Bereich mit Suchrolle. Code und andere Aktionen
bleiben erreichbar. Treffermenge und „keine Treffer“ werden als Text vermittelt.
Suchtext, Ersetzung und Treffer beziehen sich auf das aktive Demodokument,
unterscheiden Groß-/Kleinschreibung und verwenden keinen regulären Ausdruck.
Der Lexer zeigt weiterhin nur die normale Syntax; mehrere Treffer werden nicht
zusätzlich im Quelltext markiert. Ein angewählter Treffer verwendet die native
Textauswahl. Die überlagerte Fläche kann Text verdecken; Escape gibt ihn frei.

Bei schmalen Codeflächen schrumpft die Suche auf deren verfügbare Breite.
Geschlossene Flächen, klare Konturen und Schatten trennen sie vom Code; Blur
oder Animation sind nicht erforderlich. Erzwungene Kontrastfarben behalten
eine sichtbare Kontur und die vorhandene Textarea-Darstellung.

Referenzen: [Vergleich](search-overview.png),
[Suchinsel dunkel](search-island-dark.png), [Suchinsel hell](search-island-light.png),
[Suchkarte dunkel](search-card-dark.png), [Suchkarte hell](search-card-light.png).

## Größenverstellbare Bereiche

Mit „Tabs“ sind hier die Inhaltsbereiche der Werkbank gemeint. Die Dateireiter
behalten ihre inhaltsabhängige Breite; das Codefenster erhält den verbleibenden
Platz zwischen den verstellbaren Panels.

- **Workspace:** rechte Trennlinie ziehen, um die Breite zu ändern.
- **Codekontext/API:** linke Trennlinie ziehen, um die Breite zu ändern.
- **Protokoll:** obere Trennlinie ziehen, um die Höhe zu ändern. Die Höhe der
  Codefläche wächst oder schrumpft entsprechend innerhalb desselben Arbeitsraums.
- **Schmale Fenster:** unter 951 px liegt der Codekontext unterhalb des Editors
  und seine obere Trennlinie verändert die Höhe. Unter 621 px gilt das auch für
  den Workspace. Die Bereiche scrollen intern.

Die Trennlinien besitzen eine 9 px breite Trefferfläche, Resize-Cursor und
Akzentmarkierung bei Hover oder Fokus. Sie sind per Tab erreichbar und tragen
Separatorrolle, Ausrichtung, Name sowie Minimal-, Maximal- und Istwert.
Pfeiltasten verändern die Größe um 10 px, mit Umschalt um 40 px. Home/End setzen
auf die jeweilige Grenze. Enter oder Doppelklick stellt die Standardgröße des
Bereichs wieder her. „Panelgrößen zurücksetzen“ setzt alle Bereiche zurück.
Escape während des Ziehens verwirft die laufende Größenänderung.

Standardbreiten: Workspace 218 px, Kontext 255 px. Der Desktop-Arbeitsraum ist in
der Vorschau 760 px hoch; das Protokoll hat zunächst 86 px Inhaltshöhe.
Mindestbreiten 140/180 px, Höchstbreiten 480 px; verfügbare Breite und mindestens
300 px für die mittlere Editorspalte begrenzen weiter. Protokollinhalt 60–300 px.
Gestapelte Nebenbereiche sind 140–540 px hoch. Beim Verkleinern des Fensters
werden Breiten an den verfügbaren Raum angepasst.

Die Größen bleiben beim Schließen/Wiederöffnen der Panels und Variantenwechsel
für die laufende Sitzung erhalten. Nach Neuladen gelten wieder Standardgrößen.
Die Suchkarte selbst und globale Arbeitsbereich-Tabs sind nicht frei skalierbar.
Eine native Umsetzung muss diese Größen an reale Schriftgrößen und Dockinglogik
anpassen, statt die Browsermaße ungeprüft zu übernehmen.

Größenvergleich: [Dunkel](workbench-resized-dark.png), [Hell](workbench-resized-light.png).

## Prüfung

Windows/Edge 155.0.4283.62, 19. Oktober 2026:
`node tools/check_workbench_search_resize.cjs --export`, Exitcode 0.
16 Suchfälle (1440/1080/760/500 px × zwei Entwürfe × hell/dunkel):
Codeposition und -größe beim Öffnen und Erweitern auf Ersetzen unverändert,
Scrollposition beim Öffnen unverändert, Suchfläche innerhalb der Codefläche,
kein horizontaler Seitenüberlauf.

Tatsächlich bedient: Vor-/Rückwärtssuche inklusive Umschalt+Enter und Umbruch,
Ersetzen, null Treffer, Ctrl+F und Escape mit Fokusrückkehr; Ziehen aller drei
Trennlinien; erhaltene Breite nach Schließen/Wiederöffnen; Tastaturänderung,
Minimalgröße, Enter-Rücksetzen, Escape-Abbruch und Größenreset. Bei 760/500 px
wurde die Höhenänderung gestapelter Panels geprüft. Erzwungene Kontrastfarben
und Reduced Motion wurden zusätzlich als begrenzter Browser-Smokecheck verwendet.
Keine JavaScriptfehler. Alle vier Suchreferenzen sowie der Vergleich wurden
visuell gesichtet.

Nach visueller Kontrolle der Größenänderung wurde zusätzlich geprüft, dass
Code, Syntaxdarstellung und Zeilennummern auch am maximalen Scrollende nach
dem Resize dieselbe Scrollposition behalten. Die untere Innenfläche der
Zeilennummern ist dafür an den Editor angeglichen; Resize synchronisiert die
Scrollflächen erneut. Der Such-/Resize-Prüfer wurde danach erfolgreich erneut ausgeführt.

Zusätzlich `node tools/check_workbench_details.cjs --export`: Exitcode 0,
24 Layoutfälle, alle 54 Detail-/Themekombinationen und bestehende Bedienabläufe
erfolgreich, keine JavaScriptfehler. Die aktuellen Referenzbilder dieser
Detailrunde und der ausgewählten Kombination wurden neu exportiert.

Das ist eine Browserabnahme. Native Build-/Dockingintegration, echte
Screenreader, OS-Textskalierung und Linux-/macOS-Laufzeitprüfungen bleiben offen.
Die [vorhandenen Demogrenzen](DESIGN.md) gelten weiterhin.

Quellen: [Such-/Resize-Umsetzung](search-resize.js), [Stile](search-resize.css),
[Browserprüfer](../../../tools/check_workbench_search_resize.cjs).
