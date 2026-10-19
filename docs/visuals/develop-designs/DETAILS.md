# Werkbank: Schalter, Tabs und Workspace

19. Oktober 2026. Nutzerentscheidung: Werkbank als Grundlayout. Der Nutzer
wünscht Varianten für Auf-/Zuklappen, Tabs und Workspace und empfindet die
zusätzliche Werkzeugzeile als Platzverlust. Die Unterordnerposition bleibt erhalten.
Verantwortlich: UI/UX-Design. Native Anwendung unverändert.

**Ausgewählte Kombination:** Schalter im **Panelkopf**, **kompakte Tabs aus
Statusdock** und **Pfadlinien** im Workspace. Diese Nutzerentscheidung ist die
Startansicht der Vorschau und lässt sich über „Ausgewählte Werkbank“ jederzeit
wiederherstellen. Die Schalter stehen damit nicht im Statusdock. Die drei
ursprünglichen Vorbelegungen bleiben zum Vergleichen erreichbar.
Referenz: [Dunkel](selected-workbench-dark.png), [Hell](selected-workbench-light.png).

Ergänzung: [Zwei schwebende Suchentwürfe und größenverstellbare Bereiche](SEARCH-RESIZE.md)
erweitern diese Auswahl. Die Suche überlagert jetzt den Code ohne Layoutverschiebung;
Workspace, Codekontext und Protokoll besitzen ziehbare Trennlinien.

## Gemeinsame Änderung

Projektkopf und separate Werkzeugleiste entfallen. Der Projektname bleibt im
Fenstertitel und Workspace sichtbar. Suche und Speichern stehen als beschriftete
SVG-Iconaktionen mit Tooltip und zugänglichem Namen rechts an den Dateitabs.
Die Suchfelder erscheinen erst beim Öffnen der Suche. Profil und Buildstatus
nutzen die vorhandene untere Statuszeile. Das Buildmenü und F5 bleiben erhalten.
Die zusätzliche Experimentkarte im Workspace entfällt, damit der Baum ruhiger ist.

Bei 1440 px Browserbreite beginnt die Codefläche 140 px unterhalb des App-Rands;
im ursprünglichen Werkbankentwurf waren es 261,5 px. **121,5 px weniger Kopfbereich**
bei allen drei Vorbelegungen. Das ist ein gemessener Browservergleich bei DPR 1,
kein Nachweis eines nativen Fensters mit diesen Maßen. Die Galerie außerhalb
der App ist von dieser Messung ausgenommen.

## Drei Vorbelegungen

| Variante | Auf-/Zuklappen | Tabs | Workspace |
| --- | --- | --- | --- |
| A · Randgriffe | Zwei dauerhaft erreichbare, 25 px breite Randstreifen (schmal 23 px) | Aktive Linie unten; ruhige Dokumenttitel | Leise Zeilen, markierte Auswahl |
| B · Panelköpfe | Schalter im jeweiligen Bereichskopf; geschlossen erscheint derselbe Schalter an den Dateitabs | Verbundene Karten mit rundem oberen Rand | Feine Pfadlinie und Ordnersymbole |
| C · Statusdock | Beschriftete Projekt-/Kontextschalter in der vorhandenen Statuszeile | Kompakte, abgerundete Tabflächen | Sanfte Flächen für Quellen und Projektwurzel |

A kostet etwas horizontale Breite, hält die Schalter jedoch unabhängig von den
Panels an festen Rändern. B platziert die Aktion nahe am Inhalt und gibt die
Randbreite frei; die Schalter wechseln beim Schließen an die Dateitabs. C bündelt
die Aktionen unten und lässt mehr Platz an den Tabs; die Mauswege werden länger.
Die ausgewählte Kombination verbindet die Panelköpfe und Pfadlinien aus B mit
den kompakten Tabs aus C.

Die drei Auswahlfelder außerhalb des App-Fensters erlauben **27 Kombinationen**
aus Schalterposition, Tabstil und Workspace. Die Variantenknöpfe setzen jeweils
eine vollständige Vorbelegung. Panelzustände werden beim Wechsel zwischen den
Vorbelegungen für diese Sitzung bewahrt. Dateiinhalte und das aktive Dokument
bleiben beim Vergleich erhalten. Die eigene Mischung ist flüchtig und wird
nicht als native Einstellung gespeichert.

Der Tabstil wird sowohl an den drei gleich breiten Arbeitsbereich-Tabs als auch
an den Dateitabs gezeigt. Die globalen Bereiche bleiben dabei statische
Beschriftungen der Designvorschau. Ihre Anzahl, Reihenfolge und gleiche Breite
folgen weiterhin dem [bestehenden Rahmen](../../design-direction.md).

Die Dateien und die Unterordner `assets`, `runs`, `build` behalten ihre Reihenfolge
und Hierarchie. Alle Varianten zeigen Ordnersymbole und dieselben Aufklapppfeile.
Pfadlinien sind rein dekorativ; die Flächenvariante trennt die Ordnergruppe
zusätzlich durch eine feine Linie. Es werden keine echten Dateien eingelesen.

## Bedienung und Grenzen

Die tatsächlichen Demoaktionen werden mit der ursprünglichen Werkbank geteilt.
Auch umplatzierte Schalter behalten ID, Name, `aria-controls` und
`aria-expanded`; nach dem Umsetzen bleibt der Tastaturfokus am Schalter.
Das Wiederöffnen ist ohne Hover erreichbar. Wenn kein Workspace geladen ist,
verschwinden auch die leeren Randstreifen. Alle übrigen
[Demo-Einschränkungen](DESIGN.md) gelten weiter: kein Compiler oder nativer
Projektmanager, keine Dateisystemänderungen, keine vollständige API-Dokumentation.

Die erste Galerie bleibt unter [original.html](original.html) mit ihren
Referenzbildern erhalten. Die aktuellen Details werden separat über
`workbench-details.css` und `workbench-details.js` auf die gemeinsame Demo gelegt.
Für eine spätere native Umsetzung sollen die gewählten Details in die echten
Docking-/Tabkomponenten übertragen werden; keine HTML-Schicht übernehmen.

## Tatsächlich ausgeführte Prüfung

Windows, Edge 155.0.4283.62, 19. Oktober 2026:
`node tools/check_workbench_details.cjs --export`, Exitcode 0.

- 24 Layoutfälle: 1440/1080/760/500 px × drei Vorbelegungen × hell/dunkel;
  kein horizontaler Seitenüberlauf und mindestens 200 px Editorbreite.
- 54 Kombinationen: alle 27 Mischungen in beiden Themes bei 1440 px;
  Editorbreite jeweils über 400 px, kein Seitenüberlauf.
- Projekt und Kontext in jedem Layout schließen und mit Enter wieder öffnen;
  der Schalter bleibt sichtbar und behält den Fokus, auch nach Umplatzierung.
- Reihenfolge der Unterordner und gleiche Breite der Arbeitsbereich-Tabs geprüft.
- Codeänderungen bei Variantenwechsel erhalten; Suche, Speichern, F5-Build und
  Fehlersprung nach Verlegung der Aktionen erfolgreich bedient.
- Leerzustand, Ordner aufklappen, erzwungene Kontrastfarben, Reduced Motion und
  Browser-DPI 1,5×/2× mit begrenzten Layout-Smokechecks geprüft.
- Keine JavaScriptfehler. Übersicht und alle sechs Referenzzustände visuell
  gesichtet; kein sichtbarer Überlauf, weiterhin dominante Codefläche.

Nach der Nutzerwahl erneut mit demselben Prüfer ausgeführt: 24 Layoutfälle,
54 Kombinationen und zusätzlich Startbelegung sowie Wiederherstellung der
Auswahl geprüft. Die gewählte Kombination in Hell und Dunkel separat exportiert
und visuell gesichtet. Keine JavaScriptfehler, Exitcode 0.

Die vorhandenen semantischen Farben bleiben erhalten. Die erste Galerie wurde
mit `node tools/check_develop_designs.cjs` erneut geprüft: Exitcode 0,
24 Layoutfälle, 24 Textkontraste von mindestens 4,5:1, keine JavaScriptfehler.
44 lokale Dokumentverweise wurden auf vorhandene Ziele geprüft. Native Screenreader,
Systemtextskalierung, echte Monitorwechsel, Linux und macOS sind weiterhin
offen. Browser-DPI-Smokechecks sind keine vollständige Plattformabnahme.

Referenzen: [Übersicht](detail-overview.png),
[A dunkel](detail-edges-dark.png), [A hell](detail-edges-light.png),
[B dunkel](detail-headers-dark.png), [B hell](detail-headers-light.png),
[C dunkel](detail-dock-dark.png), [C hell](detail-dock-light.png).
Prüfer: [check_workbench_details.cjs](../../../tools/check_workbench_details.cjs).
