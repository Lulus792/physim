# F1: Layoutstudien, Runde 3

Stand: 16. Oktober 2026. Ausgangspunkt: Runde 2 auf `0089185`.
Die native F1-App ist unverändert. Es handelt sich um klickbare Designstudien.

## Übernommene Nutzerentscheidungen

- Die gemeinsame linke Themen-/Abschnittsnavigation aus E/G ist die Grundlage.
- Rechts wird der Modellbereich aus Entwurf C verwendet: Grafik, Ausgangswerte
  und Hinweise zum Versuch. Modell und Beispiel sind direkt wählbare Tabs.
- Beide Seiten sind **unabhängig** ein- und ausklappbar, über kompakte Icons.
  Es gibt keine breiten Ein-/Ausblenden-Beschriftungen in der Oberfläche.
- C und Physim werden **einmal global für das Dokument** gewählt. Das betrifft
  Lernweg, Anleitungen, Navigation, Suche, Beispielcode und Quellenangabe.
  Im rechten Bereich gibt es keine zusätzliche lokale Sprachwahl.
- Gemeinsame Physikerklärungen werden einmal verwendet. Sprachabhängige
  Arbeitsabläufe unterscheiden sich inhaltlich, nicht nur im Codeausschnitt.
- B und aufklappbare Untermenüs für Themen-/Abschnittssprünge bleiben verworfen.

## Vorschau

[`index.html`](index.html) direkt im Browser öffnen. Alle benötigten Quellen
und Assets sind lokal vorhanden. Keine Installation oder Netzwerkzugriffe nötig.
Die aktuelle Galerie zeigt drei Verfeinerungen derselben akzeptierten Struktur:

| Entwurf | Position der Schalter | Eingeklappter Zustand |
| --- | --- | --- |
| **H · Schalter an den Enden** | Je ein 32-px-Icon links und rechts im Fensterkopf | Beide Seiten verschwinden vollständig; Icons bleiben im Fensterkopf |
| **I · Schmale Seitenstreifen** | Icons direkt im Kopf des jeweiligen Paneels | Je ein 40-px-Streifen bleibt mit dem Wiederöffnen-Icon sichtbar |
| **J · Layout-Schalter rechts** | Zwei 32-px-Icons gemeinsam oben rechts | Beide Seiten verschwinden vollständig; die zwei Icons bleiben sichtbar |

Bilder: [gemeinsame Übersicht](overview-round3.png), [H](balanced.png),
[I](rails.png), [J](controls.png), [I mit geschlossenen Seiten](rails-closed.png),
[globale Physim-Wahl](global-language.png).
Die vorherige Galerie ist unter [`round2.html`](round2.html) weiter verfügbar.
Deren Bilder und Quellen bleiben als frühere Entwürfe erhalten.

Die äußeren Erläuterungen und Größenumschalter gehören nur zur Studie.
Der Fensterrahmen ist eine Darstellung; die Fensterknöpfe sind dekorativ.
Die Galerie verkleinert kein echtes Desktopfenster: Bei Browserbreiten unter
760 px kann man innerhalb der Vorschau horizontal scrollen. Die umgebende
Galerie selbst passt sich an die Browserbreite an.

## Verhalten

Linkes und rechtes Icon steuern nur ihre jeweilige Seite. Alle vier Kombinationen
sind möglich. Lesestelle, aktueller Artikel, globale Sprache und gewählter rechter
Tab bleiben erhalten. Die rechte Scrollposition wird beim Wiederöffnen restauriert.
Die Auswahl der globalen Sprache bleibt beim Wechsel von H/I/J bestehen.
Suche öffnet nur die linke Trefferleiste wieder, falls diese geschlossen ist.

Die Icons sind eigene einfache SVG-Fensterkonturen mit einem linken bzw. rechten
Paneelstreifen. Sie besitzen dynamische zugängliche Namen, `aria-controls`,
`aria-expanded`, Browser-Tooltips und sichtbaren Tastaturfokus. Die Grafik ist
20 px groß, die normale Schaltfläche 32 px. Auf groben Zeigegeräten wird die
Schaltfläche auf 40 px vergrößert. Eingeklappte Inhalte verschwinden aus Fokus-
und Lesefolge; das jeweilige Wiederöffnen-Icon bleibt erreichbar.

Die rechten Tabs unterstützen Pfeiltasten und Home/End. Abschnittssprünge setzen
den Fokus auf ihre Überschrift. Meldungen nutzen eine höfliche Live-Region.
Es gibt keine verschachtelten Navigationsmenüs oder Hover-Pflichtaktionen.

**Globaler Sprachwechsel:** C und Physim im Dokumentkopf steuern den gesamten
Beispielbestand. Die Seitenleiste zeigt jeweils nur den gewählten Lernweg.
Im Pendelartikel ändern sich Einordnung, Einführung und der Abschnitt zur
Versuchsvorbereitung. C erklärt Funktionen und Ausgabepuffer, Physim typisierte
Funktionen, Rückgabewerte und das Bearbeiten der `.phys`-Quelle. Der Beispiel-Tab
übernimmt dieselbe Sprache einschließlich Quellenangabe. Die Modellgleichungen
bleiben gemeinsamer Inhalt. Die Suchvorschläge stammen aus dem gewählten Lernweg.

Die Größenwahl 16/32 px betrifft den **Lesetext**. Seitennavigation, Kopf und
Modellkontext behalten ihre kompakte UI-Schrift. Bei 760 px bleiben die beiden
Seitenbereiche seitlich; über die Icons lässt sich mehr Lesebreite schaffen.

## Beispiele und Quellen

Tatsächlich eingesehene offizielle Vorbilder am 16. Oktober 2026:

- [VS Code: Custom Layout](https://code.visualstudio.com/docs/configure/custom-layout),
  Abschnitte Secondary Side Bar und Customize Layout control: getrennte
  Layout-Icons für die Sichtbarkeit der Seitenleisten. Die verlinkten offiziellen
  UI-Abbildungen wurden als Referenzen geöffnet.
- [Windows NavigationView](https://learn.microsoft.com/en-us/windows/apps/develop/ui/controls/navigationview),
  Abschnitte LeftCompact, LeftMinimal und Pane: Navigation kann zu Icons bzw.
  einem einzelnen Paneelknopf reduziert werden.

H/I/J sind eigene Ableitungen dieser Bedienmuster. Es werden weder offizielle
Screenshots noch fremde Icon-Assets mitgeliefert. Die Apple-Sidebars-Webseite
lieferte über das verfügbare Webwerkzeug nur eine JavaScript-Hülle; daraus wird
kein neuer inhaltlicher Nachweis abgeleitet.

Pendelmodell und Ausgangswerte basieren auf `docs/pendulum-tutorial.md`.
Die knappen Sprachanleitungen sind Designbeispiele auf Grundlage von
`docs/c-workflow.md` und `docs/physim-workflow.md`. Die Ausschnitte stammen aus dem
Pendel-Lernpfad bzw. `examples/language/pendulum_integrator.phys`; es sind keine
vollständigen Programme. Die Suche bildet nur die lokale Beispielsammlung ab,
keinen Volltextindex aller nativen F1-Seiten. Einige weitere Themen bleiben
explizit verkürzte Layoutplatzhalter und zeigen keinen erfundenen Modellkontext.

## Tatsächlich ausgeführte Prüfung

Windows, Microsoft Edge 155.0.4283.45, Playwright mit echter Browserbedienung.
48 Kombinationen: drei Entwürfe, zwei Fensterbreiten (1080/760 px), zwei
Lesetextgrößen (16/32 px), vier unabhängige Seitenzustände. Geprüft wurden
interner horizontaler Überlauf, erreichbare Wiederöffnen-Icons und Iconmaße.
Zusätzlich je Entwurf: Lesestelle nach Schließen beider Seiten und erneutem
Öffnen; globale C-/Physim-Wirkung auf Artikel, Anleitung, Lernweg, Beispiel und
Suche; Fortbestand beim Layoutwechsel; rechter Tab beim Schließen/Wiederöffnen;
keine lokalen Sprachschalter; Enter/Leertaste für Icons und Pfeiltasten für Tabs;
leere und erfolglose Suche. Galerie/Vorschau-Scroll bei 390/320 px geprüft.
Keine JavaScript-Laufzeitfehler. Neue Einzelansichten, Übersicht, geschlossene
Seiten und globale Physim-Ansicht visuell geprüft.

Aktuelle Quellen: `index.html`, `refinements.js`, `refinements.css`, gemeinsam
mit `content.js`, `layouts.css` und `model.svg`. Die archivierte Runde 2 verwendet
`round2.html` und `layouts.js`. PNGs sind gelieferte Designassets; temporäre
Prüfskripte, Ergebnisse und zusätzliche Bilder liegen ignoriert in
`build/f1-layout-review/`.

Kein nativer Build nötig: Renderer, Parser und registrierte F1-Seiten wurden
nicht verändert. Keine native Plattform-, Screenreader-, DPI-, Linux- oder
macOS-Abnahme aus der Browserprüfung ableiten.
