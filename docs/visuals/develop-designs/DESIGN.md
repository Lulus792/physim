# Designvertrag: Arbeitsbereich Entwickeln

Stand 19. Oktober 2026. Verantwortlich: UI/UX-Design. Drei Vergleichsentwürfe,
noch keine Produktentscheidung. Die [Galerie](index.html) ist ein eigenständiger
Browserprototyp; die native C-/SDL-/Nuklear-Oberfläche bleibt unverändert.

## Aufgabe und gemeinsamer Rahmen

Der Nutzer öffnet einen Ordner, bearbeitet Experimentquellen, schlägt die Physim-API
nach, baut das Projekt und löst Compilerdiagnosen auf. Ein erfolgreicher Build
ermöglicht anschließend den Wechsel nach Simulieren. Die Informationsarchitektur
bleibt auf Physim-Experimente ausgerichtet.

Die [bestehende Shell](../../design-direction.md) bestimmt den kompakten
Fensterkopf und die drei gleich breiten Arbeitsbereich-Tabs. Das Buildmenü ist
der gemeinsame Einstieg in den Build; F5 bleibt erhalten. Die darunterliegende
Projektzeile zeigt Projekt, Quellsprache, Profil und Buildstatus. Panelaktionen
gehören zum Editor und sind von den globalen Aktionen getrennt.

Die äußere Galerie, Variantenwahl und Zustandswahl gehören ausschließlich zur
Präsentation. Der native Start bleibt leer. Im Prototyp dient ein geladenes
Pendelbeispiel zum Vergleichen der Layouts. Globale Menüpunkte außer Build sowie
die Arbeitsbereich-Tabs sind bewusst statische Beschriftungen. Fensterknöpfe
sind dekorativ und nicht bedienbar.

## Varianten

| Variante | Anordnung bei 1440 px Browserbreite | Schwerpunkt |
| --- | --- | --- |
| Werkbank | Projekt 218 px, flexibler Editor, Kontext 255 px; Protokoll offen | Orientierung und schnelles Wechseln zwischen Dateien, Symbolen und Diagnose |
| API-Atelier | Projekt 185 px, flexibler Editor, API 330 px; Protokoll offen | Lesbare Referenz mit Signatur, Einheiten und einfügbarem Beispiel |
| Fokus | Projekt und Kontext geschlossen, Protokoll reduziert; größere Codeschrift | Zusammenhängende Arbeit am Code mit jederzeit erreichbaren Nebenbereichen |

Die Panelzustände bleiben pro Variante für die Sitzung erhalten. Dateiinhalt,
aktives Dokument, Erscheinung und Demo-Buildstatus werden gemeinsam beibehalten.
Die Kontextflächen enthalten einen manuell zusammengestellten Pendelkontext;
sie analysieren den bearbeiteten Code nicht dynamisch. Das API-Atelier verbirgt
die Symbolübersicht zugunsten der größeren Referenzkarte.

## Inhalt und visuelle Regeln

- Ruhige, geschlossene Flächen; keine Abhängigkeit von Blur oder Transparenz.
- Semantische Rollen für Text, Sekundärtext, Hintergrund, Editor, Auswahl, Akzent,
  Erfolg, Fehler und Syntax. Dunkle Grundfarben folgen der vorhandenen
  [Palette](../../../app/design_tokens.h); helle Farben werden separat abgestimmt.
- Raster überwiegend 4/8/12/16 px, Kontrollradien 5–7 px, Fensterradius 10 px.
- Systemschrift für Navigation; Consolas/Liberation Mono mit generischem
  Monospace-Fallback für Quelltext. Keine externen Fonts, Iconfonts oder Assets.
- Code 12 px mit 23 px Zeilenhöhe, im Fokus 14/26 px. Sekundäre Referenzlabels
  liegen bei 9–11 px. Diese kompakten Studienmaße sind keine native Mindestgröße:
  Textskalierung und persönliche Schriftgröße müssen in der nativen Umsetzung
  berücksichtigt und mit realen Lesetests geprüft werden.
- Zustände verwenden verständlichen Text und ergänzende Zeichen. Erfolg und
  Fehler bleiben ohne Farbwahrnehmung unterscheidbar.
- Editierbarer Text und Syntaxdarstellung teilen exakt Schrift, Abstände und
  Scrollposition. Im erzwungenen Kontrastmodus erscheint der native Text des
  Eingabefelds ohne die überlagerte Syntaxdarstellung.

Unter 950 px werden Kontext und API unterhalb des Editors angeordnet; unter
620 px folgt auch der Projektbaum darunter. Keine Panelaktion verschwindet.
Lange Codezeilen scrollen innerhalb des Editors. Das responsive Verhalten dient
der Vorschau; die native Dockinglogik muss separat mit den tatsächlichen
Fenstergrößen und Schriftmetriken abgestimmt werden.

## Zustände und Interaktionen

| Zustand | Rückmeldung / nächste Aktion |
| --- | --- |
| Leerer Workspace | Ordner öffnen oder neues Projekt; Build nicht verfügbar |
| Ordner ohne Projekt | Datei innerhalb der Demo bearbeiten; Build bleibt gesperrt |
| Physim-Projekt, ungebaut | Projekt bauen über Menü oder F5 |
| Ungespeichert | Kennzeichnung am Dokument, in Fensterkopf und Statuszeile; Speichern markiert nur den Demostand |
| Build läuft | Klarer Status und Protokoll; Profilwechsel, Speichern und Codeeingabe gesperrt; Abbrechen im Buildmenü |
| Build erfolgreich | Erfolg mit Demozeit und Hinweis auf Simulieren; keine automatische Navigation |
| Compilerfehler | Fehleranzahl und anklickbare Datei-/Zeilenmeldung; Fokus und Auswahl springen zum fehlerhaften Bezeichner |
| Build abgebrochen | Explizite Rückmeldung; letzter gültiger Stand beziehungsweise ungebauter Zustand |
| Profil gewechselt | Vorheriger Build gilt als veraltet; erneuter Build erforderlich |

Die Zustandswahl pinnt den gewählten Zustand; insbesondere endet ein dort gewählter
laufender Build nicht automatisch. Ein über F5 oder Menü gestarteter Demo-Build
endet nach einer Sekunde. Nur der bekannte Beispielbezeichner `gravity_m_s`
erzeugt die Demo-Fehlerantwort; beliebiger Text wird nicht kompiliert oder validiert.
Die 0,84 s sind eine illustrative Angabe, keine gemessene Compilerleistung.

Dateiwechsel bewahren ungespeicherte Inhalte. Die Suche arbeitet im aktiven
Dokument, unterscheidet Groß-/Kleinschreibung und ersetzt jeweils einen Treffer.
Escape schließt Suche oder Buildpopup und stellt den Fokus zur auslösenden
Schaltfläche zurück. Alle tatsächlichen Aktionen sind über Tab und native
Schaltflächenaktivierung erreichbar. F5, Ctrl/Command+F und Ctrl/Command+S werden
in der Vorschau behandelt. Die globale Menüzeile ist kein vollständig
implementiertes natives Menüsystem.

„Beispiel einfügen“ ergänzt einen Messkanalaufruf im Experimentausschnitt. Die
Bezeichner und die Signatur von `ps_channel_add` sind mit
[`experiment.h`](../../../include/physim/experiment.h) und dem
[`Pendelbeispiel`](../../../examples/pendulum/main.c) abgeglichen. Die drei
Demodateien sind gekürzt, insbesondere `physim.project` ist eine illustrative
Zusammenfassung und keine parsebare Referenz des nativen Formats.
Die Projektmanager-Aktion öffnet in dieser begrenzten Vorschau das Demoprojekt
und benennt den Unterschied im Protokoll; es wird kein Ordner angelegt.

## Zuordnung zur späteren nativen Umsetzung

| Bestandteil | Bestehender Einstieg |
| --- | --- |
| Menüzeile und Arbeitsbereichauswahl | [`toolbar_ui.inc`](../../../app/toolbar_ui.inc) |
| Projekt-/Editorinhalte und Builddiagnosen | [`design_ui.inc`](../../../app/design_ui.inc) |
| Dokumenttabs und Wechsel | [`file_tabs_ui.inc`](../../../app/file_tabs_ui.inc) |
| Panelgrößen und Sichtbarkeit | [`docking_ui.inc`](../../../app/docking_ui.inc) |
| Farben und Größenrollen | [`design_tokens.h`](../../../app/design_tokens.h) |

Bestehende echte Build-, Dateisicherungs-, Wiederherstellungs-, Such- und
Dokumentfunktionen weiterverwenden. Neue visuelle Anordnung darf keine
Demoimplementierungen in die Anwendung übernehmen. Den Codekontext erst mit
echten Symbolen und Dokumentation verbinden; Compiler- und SDK-Angaben aus der
tatsächlich verwendeten Umgebung anzeigen. Fehlerpfade müssen echte
Compilerpositionen verwenden. Die bestehende manuelle Arbeitsbereichauswahl
bleibt maßgeblich.

Plattformadapter behalten native Fenstersteuerung, Dateidialoge, Menüs und
Tastenkürzel. Unter macOS Command-Zuordnung und AX, unter Windows UIA und unter
Linux AT-SPI real prüfen. Die Windows-Anordnung der Referenzbilder ist keine
Vorgabe für native macOS-Fensterknöpfe.

## Abnahme und Grenzen

Der reproduzierbare Browserprüfer liegt unter
[`check_develop_designs.cjs`](../../../tools/check_develop_designs.cjs).
Er prüft Layout, Tokenkontraste und konkrete Bedienabläufe. Ein Browsercheck
bestätigt keine vollständige Tastatur-/Screenreader-Abnahme der nativen App.

Am 19. Oktober 2026 unter Windows mit Edge 155.0.4283.62 tatsächlich ausgeführt:
`node tools/check_develop_designs.cjs --export`, Exitcode 0. Alle 24 Layoutfälle
(1440/1080/760/500 px × hell/dunkel × drei Varianten) ohne horizontalen
Seitenüberlauf, Editorbreite jeweils mindestens 200 px. 24 geprüfte
Text-/Hintergrundkombinationen erreichen mindestens 4,5:1. Keine JavaScriptfehler.
Konkrete Abläufe: dateiweise Änderungen erhalten, sichere Syntaxdarstellung von
HTML-Text, Speichern, Tastatursuche mit Ersetzen/Fokusrückkehr, unabhängige
Panelzustände, Beispiel einfügen, Fehlernavigation, Korrektur und Demo-Build,
Abbrechen, Profilwechsel sowie allgemeine Ordner ohne Buildfreigabe.

Zusätzlich ausgeführt: Layout-Smokechecks bei Browser-DPI 1,5×/2× und vergrößerten
Kontroll-/Begleittexten, erzwungene Kontrastfarben und Reduced Motion. Diese
begrenzten Checks sind keine vollständige 200-%-Systemtext- oder Displayabnahme.
Alle sechs Designbilder und die Übersicht visuell gesichtet: klare Gewichtung
zwischen Projekt, Code und Hilfe; kein sichtbarer Überlauf in den Referenzzuständen.
Die Bilder wurden bei 1440 px Browserbreite, DPR 1 und erfolgreichem Demo-Build
aufgenommen. Der zusätzliche Fehlerbeleg zeigt Fokus/Hell mit Compilerfehler.
Die nativen App-Builds wurden für diese reinen Designartefakte nicht ausgeführt.

Offen für eine native Umsetzung: reale Systemtextskalierung bis 200 %, native
Screenreader, IME, Mehrfachauswahl, Undo-Verhalten nach programmgesteuerter
Einfügung, sehr große Quellen, reale Docklayout-Persistenz, Monitorwechsel,
vollständige Menübedienung und echte Linux-/macOS-Builds. Die Textarea verwendet
Browserbearbeitung; programmatische Einfügungen haben keinen eigenen Undo-Stack.
Der vereinfachte Syntaxlexer ist kein C-Parser und keine Codevervollständigung.

Die Desktop-Prüfmatrix des UI/UX-Skills dient der Abgrenzung. Die lokalen
Wissensnotizen zu Hierarchie, Interaktion und Plattformkonventionen wurden gelesen.
Ein erneuter Abruf der offiziellen Apple-HIG-Kapitel Layout/Sidebars am 19. Oktober
lieferte nur JavaScript-Platzhalter; die verlinkten Markdown-Abrufe waren über
das Webwerkzeug nicht lesbar. Es werden daraus keine neuen oder versionsabhängigen
Richtlinienbehauptungen abgeleitet.
