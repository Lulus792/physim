# Gewählte Werkbank und Suchinsel aus Glas

Stand: 19. Oktober 2026. Verantwortlich: UI/UX-Design. Die native App bleibt
unverändert. Dieser Ordner enthält die aktuelle Browserreferenz der gewählten
Werkbank und ausschließlich die noch offenen Glasvarianten.

## Festgelegter Aufbau

- Schalter im Panelkopf; geschlossen erscheinen dieselben Schalter an den Dateitabs.
- Kompakte Dateitabs und drei gleich breite Arbeitsbereich-Tabs.
- Workspace mit Pfadlinien; `assets`, `runs`, `build` behalten Reihenfolge und Ebene.
- Profil und Buildstatus unten; Suche und Speichern direkt an den Dateitabs.
- Schwebende Suchinsel. Die Suchkarte und die verworfenen Layoutvarianten entfallen.

Die separate Pfadzeile ist entfernt. Der aktive Dateiname steht in der
Editorstatuszeile. Die Codefläche beginnt direkt unter der Dateileiste, ohne
zusätzlichen Zwischenraum. Code und Zeilennummern haben 6 px oberen Innenabstand;
die Suchinsel sitzt mit 4 px Abstand an der Oberkante der Codefläche statt zuvor
10 px unterhalb der zusätzlichen 28-px-Pfadzeile. Das bringt Suchinsel und
Dateiinhalt jeweils 34 px näher an die Dateileiste. Dies sind Browsermaße der
Gestaltung, keine native Plattformabnahme.

Öffnen, Schließen und Aufklappen der Ersetzung verändern Position und Größe des
Codes nicht. Die Suche ist nichtmodal, verwendet das aktive Dokument, sucht
Groß-/Kleinschreibung beachtend und ersetzt jeweils einen Treffer. Enter/↓ und
Umschalt+Enter/↑ navigieren vor- bzw. rückwärts mit Umbruch. Escape schließt die
Suche und stellt den Fokus am Suchicon wieder her. Die Galerie startet bewusst
mit geöffneter Suchinsel, damit das Material sofort vergleichbar ist.

## Drei noch offene Materialvarianten

| Entwurf | Transparente Grundfläche dunkel / hell | Unschärfe | Wirkung |
| --- | --- | --- | --- |
| Seidenglas | 74 % / 73 % Deckung | 14 px | Ruhig gefilterter Hintergrund, zurückhaltende Lichtkante |
| Klarglas | 24 % / 24 % Deckung | 2 px | Deutlichere Durchsicht auf Code und Syntaxfarben |
| Kristallkante | 45 % / 45 % Deckung | 7 px | Stärkere Lichtkante, sanfter Glanz, rundere Form |

Die Sättigung wird leicht angehoben. Nur die vorübergehende Suchinsel erhält
diesen Effekt; Code, Workspace und Kontext bleiben feste Leseflächen. Eingaben,
Pfeilaktionen und Trefferzahl haben stärkere eigene Flächen für lesbare Inhalte.
Die zusammengesetzte Farbe hängt vom Code darunter ab.

Dies sind eigene CSS-Studien mit `backdrop-filter`, keine native Apple-Material-
Implementierung. Apples Materialrichtlinie ordnet Liquid Glass der funktionalen
Bedienebene zu und empfiehlt bei textreichen Kontrollen einen stärker gefilterten
Hintergrund. Daraus wird hier die ruhigere Seidenglasvariante als Startvergleich
abgeleitet; Klarglas ist eine bewusst transparentere Vergleichsstudie.
Quelle: [Apple HIG Materials](https://developer.apple.com/design/human-interface-guidelines/materials),
am 19. Oktober über das offizielle
[DocC-Dokument](https://developer.apple.com/tutorials/data/design/human-interface-guidelines/materials.json)
eingesehen. Es wird keine exakte optische Gleichheit mit Apple behauptet.

Ohne `backdrop-filter`, bei erhöhter Kontrastanforderung, erzwungenen
Kontrastfarben oder reduzierter Transparenz wird eine deckende Fläche verwendet.
„Transparenz reduzieren“ erlaubt den direkten Vergleich. Größe, Suchablauf und
Fokusführung bleiben dabei gleich. Es gibt keine Animation.

## Größenverstellung

Workspace und Codekontext werden horizontal an den Trennlinien verändert.
Das Protokoll erhält seine Höhe aus dem Editorraum; die Codefläche passt sich an.
Unter 951 px wird der Kontext nach unten gestapelt und in der Höhe verändert;
unter 621 px gilt dies auch für den Workspace. Alle Inhaltsbereiche scrollen intern.

Standardbreiten 218/255 px, Mindestbreiten 140/180 px, Höchstbreiten 480 px;
mindestens 300 px bleiben für die mittlere Editorspalte reserviert. Der
Desktop-Arbeitsraum ist 760 px hoch, der Protokollinhalt zunächst 86 px.
Trennlinien haben 9 px Trefferfläche, Separatorrolle, Namen und Größenwerte.
Pfeiltasten ändern 10 px, mit Umschalt 40 px; Home/End setzen Grenzen, Enter oder
Doppelklick den Standard. Escape verwirft eine laufende Ziehbewegung.
Größen bleiben beim Ein-/Ausklappen für die Sitzung erhalten; Neuladen setzt sie zurück.

## Demo und spätere native Umsetzung

Der Code ist ein gekürzter Pendelausschnitt, keine vollständige Experimentdatei.
Die Projektdatei ist illustrativ. Speichern markiert nur den Demostand; der
Demo-Build simuliert Erfolg bzw. den bekannten Bezeichnerfehler. Dateisystem,
Compiler und Projektmanager sind nicht angebunden. Globale Menübeschriftungen
außer Build und die Arbeitsbereich-Tabs sind statische Shellreferenzen.

Für die native Umsetzung bestehende Editor-, Docking-, Build-, Such- und
Sicherungsfunktionen verwenden; CSS-Geometrie und Glaseffekt nicht als bereits
plattformgeprüft übernehmen. Native Accessibility, Systemtextskalierung,
Monitorwechsel sowie Linux-/macOS-Builds sind separat offen.

## Bereinigung

Auf ausdrücklichen Nutzerwunsch wurden 33 ersetzte Artefakte entfernt:
verworfene Gesamt-/Detailstudien, Suchkarte, alte Referenzbilder, alte
Vergleichsoberflächen, nicht mehr benötigte Stile/Skripte und zwei alte Prüfer.
Die entfernten Dateien belegten 3.709.179 Bytes (rund 3,54 MiB). Aktuelle
Such-/Resize-Logik bleibt erhalten; ein konsolidierter Prüfer deckt die Oberfläche
ab. Frühere Dateien bleiben über die Git-Historie wiederherstellbar.
Andere Projektstudien, fremde lokale Arbeiten und Plattformbelege wurden nicht angefasst.

## Tatsächlich ausgeführte Prüfung

Windows/Edge 155.0.4283.62, 19. Oktober 2026:
`node tools/check_develop_designs.cjs --export`, Exitcode 0.

- 24 Layoutfälle: 1440/1080/760/500 px × drei Glasvarianten × hell/dunkel.
  Kein Seitenüberlauf, Editor mindestens 200 px breit. Kein zusätzlicher Abstand
  unter den Dateitabs; Suchinsel 4 px unter dem Codebeginn.
- Öffnen und Erweitern der Suche erhalten Codegeometrie und Scrollposition.
- 18 Kontrolltext-Kombinationen auf Eingaben, Pfeilaktionen und Trefferanzeige: konservativ
  über vollständig schwarzem und weißem Untergrund jeweils mindestens 4,5:1.
  Das ist eine begrenzte Kontrastprüfung der Bedienflächen, keine vollständige
  Kontrastabnahme aller möglichen Codehintergründe.
- Transparente Hintergrundfarbe und aktiver Blur für alle Materialien;
  deckender Fallback ohne Geometrieänderung für jede Variante geprüft.
- Vor-/Rückwärtssuche, Ersetzen, Escape/Fokus, erhaltene Dateiänderungen, sichere
  Syntaxdarstellung, Speichern, Demo-Build und Diagnosesprung bedient.
- Maus-/Tastaturgrößenänderung, Wiederöffnen, Abbruch, Reset, Scrollgleichlauf,
  gestapelte Bereiche bei 760/500 px, erzwungene Farben und Leerzustand geprüft.
- Keine JavaScriptfehler. Alle sechs Referenzzustände und die Übersicht visuell
  gesichtet. Ein Screenshot belegt keine native Material- oder Screenreaderqualität.

Prüfer: [check_develop_designs.cjs](../../../tools/check_develop_designs.cjs).
[Glasvergleich](glass-overview.png),
[Seidenglas dunkel](glass-satin-dark.png), [Seidenglas hell](glass-satin-light.png),
[Klarglas dunkel](glass-clear-dark.png), [Klarglas hell](glass-clear-light.png),
[Kristallkante dunkel](glass-crystal-dark.png), [Kristallkante hell](glass-crystal-light.png).
