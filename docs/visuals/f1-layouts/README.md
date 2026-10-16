# F1: Layoutstudien, Runde 2

Stand: 16. Oktober 2026. Grundlage: bisherige Layoutstudien auf `108d5f6` und der
lokale Physim-Projektstand. Die native F1-App wurde nicht verändert.

## Verbindliches Feedback für diese Studien

- B (Lesefläche mit aufklappbaren Navigationsmenüs) ist ausgeschieden.
- Themen und Abschnittssprünge gehören an die Seite und sind direkt anwählbar.
- Die **gesamte Seitenleiste** darf ein- und ausgeklappt werden. Das bedeutet
  keine aufklappbaren Untermenüs für einzelne Themen oder Abschnitte.
- Der Schwerpunkt bleibt auf Aufteilung und Bedienung, mit einer gemeinsamen
  zurückhaltenden Graustufenpalette.

## Vorschau und Bilder

[`index.html`](index.html) direkt im Browser öffnen oder den bereits gestarteten
lokalen Vorschau-Server nutzen. Alle Quellen und Assets sind lokal; keine
Installation, Netzwerkanfragen, externen Schriften oder Buildschritte nötig.

- [`overview-new.png`](overview-new.png): E, F, G und E mit eingeklappter Leiste.
- [`overview.png`](overview.png): alle sechs aktuellen Varianten.
- Neue Einzelansichten: [E](outline.png), [F](navigator.png), [G](reference.png).
- Überarbeitete Einzelansichten: [A](classic.png), [C](learn.png), [D](find.png).
- [E ohne Seitenleiste](outline-collapsed.png).

B ist aus Auswahl und aktueller Bildübersicht entfernt. Ein alter Link mit
`?layout=read` zeigt nun E. Die frühere Fassung bleibt in der Git-Historie.
Die Bedien- und Erläuterungsflächen außerhalb des Fensterrahmens gehören nur zur
Studie und sind kein vorgeschlagener Bestandteil der nativen F1-Oberfläche.

## Sechs Aufteilungen

| Variante | 1080 px, 16 px Text | Schwerpunkt |
| --- | --- | --- |
| **E · Alles links, neu** | 280 px gemeinsame Themen-/Abschnittsleiste, Artikel daneben | Alle Sprungziele an einem Ort; Abschnitte direkt unter dem aktiven Artikel |
| **F · Doppelte Navigation, neu** | 160 px Bereiche, 230 px Artikel/Abschnitte, Lesefläche daneben | Zwei sichtbare Navigationsebenen ohne verschachtelte Menüs |
| **G · Text & Beispiel, neu** | 224 px Seitennavigation, Artikel, 330 px Code-/Beispielbereich | Erklärung und umschaltbare C-/Physim-Ausschnitte parallel lesen |
| A · Das Handbuch | 220 px Themen, Artikel, 190 px Abschnittsnavigation | Klassische Aufteilung mit beiden Navigationsflächen |
| C · Der Lernpfad | 210 px Lernschritte, Aufgabe, 260 px Modellkontext | Vier direkt wählbare Lernschritte und feste Vor-/Zurück-Aktionen |
| D · Die Antwortsuche | 292 px Trefferliste, Artikelvorschau, 178 px Abschnitte | Fundstelle im Artikelkontext lesen |

**Ein- und Ausklappen:** Der beschriftete Knopf bleibt im Fensterkopf erreichbar.
Er blendet die seitliche Navigation als Ganzes aus oder ein; in A und D betrifft
das auch die rechte Abschnittsleiste. Modell-/Codeflächen sind eigenständige
Inhalte und bleiben davon getrennt. Artikelauswahl und Lesestelle werden erhalten.
Eine Lesemarke gleicht die geänderten Zeilenumbrüche aus. Ein anschließender
Scroll- oder Navigationsvorgang aktualisiert den Bezug für das Wiedereinblenden.
Die Wahl bleibt beim Wechsel der Layoutvariante erhalten. Suche blendet ihre
Trefferleiste wieder ein, damit neue Treffer direkt erreichbar sind.

**760 px / großer Text:** Die Navigation bleibt seitlich und direkt zugänglich.
A und D integrieren die Abschnittslinks links. F ordnet die beiden Navigationsebenen
innerhalb der linken Leiste untereinander an. G zeigt das Beispiel unter dem
Artikel als separat scrollbareren Inhaltsbereich. C erhält die Modellinformation
im Artikel statt in einer zusätzlichen rechten Spalte. Bei 32 px Text gelten die
entsprechenden platzsparenden Anordnungen auch im breiten Fenster. Es entstehen
keine aufklappbaren Inhaltsmenüs.

Die Desktopvorschau beginnt passend zum nativen Mindestfenster bei 760 px. In
schmaleren Browserfenstern kann man innerhalb des Vorschau-Rahmens horizontal
scrollen; die umgebende Galerie passt sich weiterhin an die Browserbreite an.
Das ist kein Entwurf für eine mobile F1-App.

## Interaktionen und Inhaltsgrenzen

Themenwechsel und Abschnittssprünge, aktive Markierungen beim Scrollen,
Bereichswechsel in F, C-/Physim-Wechsel und Kopieren in G, alle vier Lernschritte
in C einschließlich deaktivierter Randaktionen und Suche in D sind bedienbar.
Die Schaltflächen funktionieren mit der Tastatur. Die Seitenleistenaktion trägt
`aria-expanded` und verweist auf die Navigationsflächen. Eingeklappte Bereiche
sind aus Fokus- und Lesefolge entfernt. Abschnittssprünge setzen den Fokus auf
die Zielüberschrift. Statusmeldungen werden über eine höfliche Live-Region gemeldet.

Die Suche filtert Titel und Kurztexte einer kleinen lokalen Beispielsammlung,
keinen Volltextindex aller 185 nativen Handbuchseiten. Leere Eingabe zeigt drei
Vorschläge; erfolglose Suche zeigt einen Hinweis und erhält die Artikelvorschau.
Einige weitere Themen sind ausdrücklich als verkürzte Layoutplatzhalter markiert.

Pendelannahmen, Gleichungen, Ausgangswerte und Vergleichsaufgaben stammen aus
`docs/pendulum-tutorial.md`; C-/Physim-Ausschnitte aus diesem Lernpfad bzw.
`examples/language/pendulum_integrator.phys`. Es sind keine vollständigen
Programme. Die Modellgrafik ist ein vereinfachtes Designasset.

## Prüfung

Windows, Microsoft Edge 155.0.4283.45, echte Browserbedienung mit Playwright:
48 Kombinationen aus sechs Layouts, zwei Fensterbreiten (1080/760 px), zwei
Textgrößen (16/32 px) und ein-/ausgeblendeter Navigation auf internen horizontalen
Überlauf geprüft. Ein-/Ausklappen einschließlich Artikelauswahl und Rückkehr zur
Lesestelle, direkte Sprünge, Themen-/Bereichswechsel, Sprachen, Lernschrittgrenzen,
Suchtreffer, leere Suche, erfolglose Suche, Wiederöffnen der Trefferleiste und
Tastaturbedienung der Seitenleistenaktion ausgeführt. Alte B-Links und die
Galerie samt horizontalem Vorschau-Scrollen bei 390/320 px Browserbreite geprüft.
Keine JavaScript-Laufzeitfehler. Neue Einzelansichten, Übersicht und Ansichten mit
großem Text visuell geprüft.

Quellen: `index.html`, `layouts.css`, `layouts.js`, `content.js`, `model.svg`.
PNG-Dateien sind gelieferte Designassets; Prüfausgaben und temporäre Bilder liegen
ignoriert unter `build/f1-layout-review/`.

Keine Änderung an Renderer, Parser oder registrierten Handbuchseiten; kein
nativer Build erforderlich. Keine native Plattform-, Screenreader-, DPI- oder
Linux-/macOS-Abnahme aus diesen Browserstudien ableiten.
