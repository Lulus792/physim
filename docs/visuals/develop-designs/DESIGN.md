# Designvertrag: Werkbank / Klarglas Kühl

Stand: 19. Oktober 2026. Die Nutzerwahl ist abgeschlossen: **Werkbank mit
Panelköpfen, kompakten Tabs, Workspace-Pfadlinien und Suchinsel in Klarglas Kühl**.
Die native Anwendung ist noch nicht umgebaut. Für den nächsten Chat gelten
[Auftrag](NEXT-CHAT.md) und [technische Übergabe](IMPLEMENTATION.md).

## Aufbau

- Linker Workspace-Schalter im Panelkopf; geschlossen erscheint er links der Dateitabs.
- Rechter Kontextschalter fest am äußersten rechten Rand der oberen Panelzeile.
  Er bleibt beim Öffnen/Schließen an exakt derselben Stelle, immer rechts von
  Suche und Speichern. Bei schmalen Fenstern bleibt er oben erreichbar.
- Kompakte Dateitabs mit markierter aktiver Datei und Dirty-Anzeige. Drei gleich
  breite Hauptbereich-Tabs für Entwickeln, Simulieren, Auswerten in der Referenz.
- Workspace mit Pfadlinien. `assets`, `runs`, `build` behalten Reihenfolge und
  Ebene der Referenz; reale Namen/Inhalte kommen aus dem geöffneten Workspace.
- Profil und tatsächlicher Buildstatus unten, Suche und Speichern an den Dateitabs.
- Kein großes Projektbanner, keine zusätzliche Toolbar-/Pfadzeile vor dem Code.
  Aktiver Dateiname und Cursorinformationen stehen in der Editorstatuszeile.

Codefläche und Zeilennummern beginnen direkt unter der Dateileiste, ohne weiteren
Zwischenraum, mit 6 px oberem Innenabstand. Die Suchinsel sitzt innerhalb der
Codefläche bei `top:4px; right:8px`, Breite 540 px, maximal `100% - 16px`.
Sie darf Code überlagern und besitzt ihren eigenen begrenzten Eingabebereich.
Die Zahlen sind logische Maße der Browserreferenz bei 100 %; native Schrift-/DPI-
Skalierung und Inhaltsminima müssen berücksichtigt werden.

## Suche und Icons

Öffnen, Schließen und Aufklappen der Ersetzung ändern weder Position noch Größe
noch den bisherigen Scrollstand des Editors. Es wird keine Suchzeile im normalen
Layout reserviert. Erst Treffernavigation scrollt zur Auswahl. Die Suche ist
nichtmodal und verwendet die aktive Datei, beachtet Groß-/Kleinschreibung und
ersetzt jeweils einen Treffer. Keine zusätzliche Regex-/Glob-Oberfläche beauftragt.

Ctrl/Command+F öffnet/fokussiert die Suche. Enter oder der Abwärtsbutton sucht
vorwärts; Umschalt+Enter oder der Aufwärtsbutton rückwärts, jeweils mit Umbruch.
Die Chevron-Aktion links blendet die Ersetzungszeile ein/aus. Escape bzw. das
Schließenkreuz schließt die Suche und gibt den Fokus an das Suchicon zurück.
Trefferzahl und Leerzustand geben eine verständliche Rückmeldung.

Die Icons aus [search-resize.js](search-resize.js) sind eigene SVG-Pfade mit
16 × 16 Viewbox, 1,5 px Strichstärke und runden Linienabschlüssen/-verbindungen:
Lupe in der Eingabe, Chevron für Aufklappen, Vor-/Zurück-Chevrons, Schließenkreuz,
Ersetzen-Symbol neben der Textaktion. Treffernavigation teilt eine gemeinsame
Fläche; Aufklappen und Schließen besitzen im Ruhezustand keinen eigenen Rahmen.
Tooltips, zugängliche Namen und sichtbare Fokusindikatoren gehören zum Design.
Dekorative Iconpfade sind keine eigenständigen Accessibility-Ziele.

Die Vorschau startet bewusst mit offener Suche, um das Material zu zeigen.
Das ist kein Auftrag, die native App bei jedem Start mit offener Suche zu laden.

## Material und Farben

Nur die vorübergehende Suchinsel erhält den Glaseffekt. Editor, Workspace und
Kontext bleiben feste Leseflächen. Gewählt ist Klarglas mit der 16-px-Rundung
der früheren Kristallkante, **Farbgebung Kühl**. Neutral und Warm sind verworfen.

Verbindliche Werte aus [glass.css](glass.css):

| Eigenschaft | Dunkel | Hell |
| --- | --- | --- |
| Haupttext/Icons | `#edf4fc` | `#182a39` |
| Sekundärtext | `#cfdfef` | `#344b60` |
| Glasgrundfläche | `rgba(36,53,73,.24)` | `rgba(232,243,255,.24)` |
| Eingabe | `rgba(21,31,46,.68)` | `rgba(244,250,255,.68)` |
| Icon-/Buttonfläche | `rgba(31,45,61,.90)` | `rgba(239,247,255,.86)` |
| Treffer-/Hinweisfläche | `rgba(18,28,41,.90)` | `rgba(245,250,255,.92)` |
| Glaskante | `rgba(196,220,247,.40)` | `rgba(141,171,204,.60)` |

Gemeinsam: Hintergrundblur 2 px, Sättigung 1,15, Außenradius 16 px,
Innenradius 9 px, Insel-Innenabstand 7 px, Reihenabstand 5 px, Eingabehöhe 30 px.
Die Navigation verwendet zwei 27 px breite Buttons; Schließen/Aufklappen sind
28 px breit. Kleine Fenster dürfen diese auf 25 px reduzieren.

Ein schwacher weißer Verlauf erzeugt die Lichtkante: dunkel 160° mit 4 % Weiß
bis transparent bei 50 %, hell 135° mit 25 % Weiß bis transparent bei 65 %.
Rahmen 1 px, Schatten `0 7px 22px #0003`, innerer Lichtsaum
`inset 0 1px 0 #ffffff18`. Die endgültige zusammengesetzte Farbe hängt vom
Inhalt darunter ab; die stärkeren Bedienflächen sichern Lesbarkeit.

Die Vorschau ist eine eigene CSS-Annäherung mit `backdrop-filter`, kein nativer
Apple-Materialbaustein. Die native Umsetzung braucht einen echten Filter des
darunterliegenden Codeinhalts. Die gewählte transparente Normaldarstellung darf
nicht still durch eine deckende Fläche ersetzt werden.

Ohne Filterunterstützung, bei reduziertem Transparenzbedarf, erhöhtem Kontrast
oder erzwungenen Kontrastfarben gilt eine deckende Fläche mit den semantischen
App-Farben. Größe, Position, Fokus und Bedienung bleiben gleich. Die manuelle
Prüfoption „Transparenz reduzieren“ in der HTML-Seite dient dem direkten Vergleich.
Es gibt keine Animation.

## Panelgrößen und Bedienung

Workspace und Kontext werden horizontal an ihren Trennlinien verändert. Das
Protokoll erhält seine Höhe aus dem Editorraum; der Code erhält den Rest.
Alle Inhaltsbereiche scrollen intern. „Tabs resizen“ meint hier die Panelbereiche,
nicht eine frei veränderliche Breite einzelner Dateireiter.

Browserreferenz: Breiten 218/255 px, Minima 140/180 px, Maxima jeweils 480 px,
mindestens 300 px für die Editorspalte reserviert. Desktop-Arbeitsraum 760 px
hoch; Protokollinhalt zunächst 86 px, mindestens 60 und höchstens 300 px.
Unter 951 px wird der Kontext nach unten gestapelt, unter 621 px auch der
Workspace. Gestapelte Nebenbereiche sind in der Höhe von 140–540 px verstellbar.
Diese Demo-Defaults ersetzen keine bestehenden nativen Nutzereinstellungen.

Trennlinien haben 9 px Trefferfläche, Separatorrolle, Namen, Orientierung und
Größenwerte. Pfeiltasten ändern 10 px, Umschalt 40 px; Home/End setzen Grenzen,
Enter oder Doppelklick den Standard. Escape bricht die laufende Ziehbewegung ab
und stellt deren Ausgangsgröße wieder her. Ein-/Ausklappen erhält die Größen.
Die Demo speichert sie nur während der Sitzung, nativ ist vorhandene persistente
Dockingverwaltung zu verwenden. Die separate Reset-Prüfaktion der HTML-Seite
gehört nicht als zusätzliche Demo-Toolbar in die native App.

## Grenzen der Referenz

Gekürzter Pendelcode, Projektdatei, Symbolgliederung, Modellnotiz und Buildumgebung
sind illustrativ. Speichern markiert nur Demostatus; Build simuliert Erfolg bzw.
einen bekannten Bezeichnerfehler. Dateisystem, Compiler und Projektverwaltung
sind nicht angebunden. Globale Menülabels außer Build und die Hauptbereich-Tabs
sind statische Shellreferenzen. Die nächste Umsetzung erhält die echten Funktionen.

## Bereinigung und tatsächlich ausgeführte Prüfung

Verworfene Layout-/Materialstudien und ihre Prüfer wurden in den vorangegangenen
Designrunden entfernt. Nach der Farbwahl sind zusätzlich die vier Neutral-/Warm-
Bilder mit zusammen 414.708 Bytes (rund 405 KiB), deren Auswahl und Materialregeln
entfernt. Die Übersicht enthält jetzt ausschließlich Kühl in Hell/Dunkel.
Git-Historie und andere Projektstudien bleiben erhalten.

Am 19. Oktober 2026 auf Windows/Edge **155.0.4283.62** ausgeführt:
`node tools/check_develop_designs.cjs --export`, Exitcode **0**.

- 8 Layoutfälle: 1440/1080/760/500 px × Hell/Dunkel. Kein Seitenüberlauf;
  Editor mindestens 200 px breit; 0 px Zusatzabstand unter den Dateitabs,
  Suchinsel 4 px unter dem Codebeginn.
- Kühl als einzige und voreingestellte Variante geprüft. Öffnen und Erweitern
  der Suche erhalten Codegeometrie und Scrollstand. Der rechte Kontextschalter
  bleibt in allen 8 Fällen beim Schließen/Öffnen an identischer Position, rechts
  von Suche/Speichern, und behält den Fokus.
- 6 Kontrolltext-Kombinationen auf Eingabe, Pfeilfläche und Trefferzahl:
  konservativ über schwarzem/weißem Untergrund jeweils mindestens 4,5:1.
  Dies ist keine vollständige Kontrastabnahme beliebiger Inhalte.
- Transparenz, 2 px Blur, 16 px Radius, SVGs und deckender Fallback ohne
  Geometrieänderung geprüft. Vor-/Rückwärtssuche, Ersetzen, Escape/Fokus,
  Dateiänderungen, sichere Syntaxdarstellung, Save, Demo-Build und Diagnosesprung bedient.
- Maus-/Tastaturresizen, Größen nach Wiederöffnen, Abbruch, Reset, Scrollgleichlauf,
  gestapelte Panels, erzwungene Farben, Reduced Motion und Leerzustand geprüft.
- Keine JavaScriptfehler. Beide finalen Bilder und die Detailübersicht visuell geprüft.

Prüfer: [check_develop_designs.cjs](../../../tools/check_develop_designs.cjs).
Referenzen: [Dunkel](glass-cool-dark.png), [Hell](glass-cool-light.png),
[Suchdetail](glass-overview.png). Generierte Prüfausgaben werden nach Abnahme
aus dem ignorierten Buildbereich entfernt; die drei Referenzen bleiben versioniert.
Native Materialqualität, Accessibility, DPI und Linux/macOS sind damit noch
nicht geprüft. Die separate native Abnahmeliste steht in IMPLEMENTATION.md.
