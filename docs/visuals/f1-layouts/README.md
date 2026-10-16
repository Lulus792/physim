# F1: vier Layoutstudien

Stand: 16. Oktober 2026. Grundlage ist der lokale Physim-Stand `f79ab79`.
Die Studien zeigen Vorschläge für die Aufteilung des eigenständigen F1-Fensters.
Sie verändern die native App nicht und legen keine neue verbindliche
Produktrichtung fest. Alle Entwürfe verwenden eine zurückhaltende Graustufenpalette;
verglichen werden Informationshierarchie, Navigation und verfügbare Lesefläche.

## Ansehen und ausprobieren

[`index.html`](index.html) lässt sich direkt im Browser öffnen. Es benötigt weder
Installation noch Netzwerk oder externe Schriften. [`overview.png`](overview.png)
zeigt alle vier Fenster nebeneinander. Einzelansichten einschließlich der
Erläuterungen: [A](classic.png), [B](read.png), [C](learn.png), [D](find.png).

Oben wählt man das Layout, die maximale Fensterbreite (1080/760 logische Pixel)
und die Textgröße von Artikel und Navigation (16/32 Pixel). Ist das Browserfenster
schmaler als die gewählte Breite, passt sich die Vorschau an die verfügbare Fläche
an. Der äußere Studienrahmen gehört nicht zur vorgeschlagenen F1-Oberfläche.

| Variante | Aufteilung bei 1080 px und 16 px Text | Zweck | Abwägung |
| --- | --- | --- | --- |
| A · Das Handbuch | Themen 224 px / Artikel flexibel / Inhaltsverzeichnis 176 px | Häufiges Nachschlagen und Wechseln zwischen Lernwegen und Referenz | Zwei Navigationsflächen beanspruchen Lesebreite |
| B · Die Lesefläche | Aufklappbare Themenauswahl oben / zentrierter Artikel bis 760 px einschließlich Innenabstand | Lange Erklärungen, Formeln und paralleles Arbeiten neben der Simulation | Themen sind erst nach Öffnen der Auswahl sichtbar |
| C · Der Lernpfad | Schritte 205 px / Aufgabe flexibel / Modellkontext 280 px | Geführtes Lernen von Modell über Versuch bis Auswertung | Für freie Referenznavigation weniger geeignet |
| D · Die Antwortsuche | Suchergebnisse 330 px / Artikelvorschau flexibel | Eine konkrete Antwort im Kontext nachschlagen | Ergebnisliste reduziert die Artikelbreite |

Bei 760 px und normalem Text wandert A's Inhaltsverzeichnis in den Artikel,
C's zusätzliche Modellspalte entfällt, und D behält eine schmalere Ergebnisliste.
Bei 32 px Text erhalten A und C eine aufklappbare Navigation über die volle Breite,
statt ihre schmalen festen Seitenleisten beizubehalten. D stapelt Ergebnisse und
Artikel. Unter 620 px verfügbarer Vorschaufläche wird auch bei normalem Text
entsprechend umgeordnet. Artikel und Navigation bleiben separat scrollbar.

## Interaktionen und Zustände

- **A:** Themenwechsel, hervorgehobene Auswahl und Abschnittssprünge. Die Themen
  sind eine kleine Beispielsammlung. Die weiteren Mechanikseiten sind ausdrücklich
  als verkürzte Layoutplatzhalter gekennzeichnet.
- **B:** Themenauswahl öffnen, Abschnitt wählen, zum Artikel springen. Das
  aufklappbare Verzeichnis im Artikel bietet dieselben Abschnittssprünge.
- **C:** Alle vier Lernschritte direkt wählen oder mit Zurück/Weiter wechseln.
  Am ersten bzw. letzten Schritt ist die passende Aktion deaktiviert. Schritt 2
  zeigt umschaltbare C-/Physim-Ausschnitte und eine Kopieraktion mit Rückmeldung,
  falls die Browser-Zwischenablage nicht verfügbar ist. Der jeweils vorherige
  Lernschritt bleibt beim Wechsel zwischen Layoutvarianten erhalten.
- **D:** Tippen in die Suche wechselt zur Trefferansicht. Die Suche filtert die
  Titel und Kurztexte der lokalen Beispielsammlung; sie bildet keinen Volltextindex
  aller 185 nativen F1-Seiten nach. Leere Eingabe zeigt drei Vorschläge; eine
  erfolglose Suche zeigt einen Hinweis und erhält die letzte Artikelvorschau.
- **Tastatur:** Native Schaltflächen und Eingaben, sichtbarer Fokus, Sprung zum
  Artikeltitel. Escape schließt Themenauswahl bzw. kompakte Navigation und setzt
  den Fokus zurück. Zustandsmeldungen nutzen eine höfliche Live-Region.

Es gibt keine Netzwerkanfragen, Ladeanimationen oder tatsächlichen Simulations-
und Projektaktionen. Die Fensterknöpfe sind als dekorative Rahmendarstellung
gekennzeichnet. Die Modellgrafik ist ein vereinfachtes Layoutasset; das vorhandene
Fachdiagramm wird nicht ersetzt.

## Inhaltsgrundlage und Entwurfsempfehlung

Ausgewertet wurden `app/documentation_window.inc`, `app/documentation_ui.inc`,
`docs/guide.md`, `docs/design-direction.md` sowie `docs/pendulum-tutorial.md`.
Modellannahmen, Gleichungen, Ausgangswerte und Vergleichsaufgaben stammen aus dem
Pendel-Lernpfad. Der C-Ausschnitt ist dessen Zustandsableitung; der Physim-Ausschnitt
ist aus `examples/language/pendulum_integrator.phys` übernommen und für die schmale
Darstellung umbrochen. Das vollständige Physim-Beispiel enthält auch optionale
Widerstands-/Energiebilanzfunktionen; im hier gezeigten Vakuumkontext bleibt
`airDensity=0`. Die Beispiele sind keine vollständigen ausführbaren Programme.

Als Ausgangspunkt bietet sich **A** an. **B** kann als Lesemodus, **C** als
Tutorialansicht und **D** als Zustand der globalen Suche ergänzen. Das ist ein
Entwurfsvorschlag; die vier Varianten sind zum Vergleichen eigenständig dargestellt.

## Tatsächlich ausgeführte Prüfung

Windows, Microsoft Edge 155.0.4283.45, Chromium-Engine, automatisierte echte
Browserbedienung mit Playwright. 16 Kombinationen aus vier Layouts, zwei
Fensterbreiten und zwei Textgrößen wurden auf horizontalen Überlauf geprüft.
Zusätzlich wurden alle vier Varianten bei 390 und 320 px Browserbreite mit
32 px Artikeltext ausgeführt. Artikelwechsel, Abschnittssprünge, Themenmenü,
Lernschritte einschließlich der Randzustände, Sprachwechsel, Suche mit Treffern,
leerer Eingabe und ohne Treffer, Fokusaktion sowie kompakte Navigation mit Escape
wurden bedient und geprüft. Keine JavaScript-Laufzeitfehler.

Die Einzelbilder, die gemeinsame Übersicht sowie die schmalen Ansichten mit
großem Text wurden visuell geprüft. Die Übersicht verkleinert die Fenster nur
für den Vergleich; die HTML-Vorschau dient der Beurteilung in Originalgröße.
Prüfausgaben und temporäre Bilder liegen ignoriert unter
`build/f1-layout-review/`; die gelieferten PNGs sind bewusste Designassets.

Keine native UI-, Screenreader-, Monitorwechsel-, Linux- oder macOS-Abnahme.
Kein nativer Build erforderlich: ausschließlich eigenständige Designartefakte,
keine Änderung an Renderer, Parser oder registrierten Handbuchseiten.
