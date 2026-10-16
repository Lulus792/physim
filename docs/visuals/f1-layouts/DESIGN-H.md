# Verbindliches Zielbild: F1-Design H

Vom Nutzer am 16. Oktober 2026 ausgewählt. Diese Datei ist der Einstieg für die
spätere native Umsetzung. Der aktuelle Auftrag liefert einen geprüften
Designprototyp und die Übergabe; die native App ist noch nicht umgestellt.

## Maßgebliche Artefakte

- [Interaktive Vorlage](index.html): H, standardmäßig Dunkel; Hell umschaltbar.
- [Hell](h-light.png), [Dunkel](h-dark.png), [Vergleich und getrennte Seiten](h-final-overview.png).
- [Nur links geschlossen](h-dark-left-closed.png), [nur rechts geschlossen](h-dark-right-closed.png),
  [beide geschlossen](h-dark-both-closed.png). Dieselben drei Zustände gibt es als `h-light-*.png`.
- Quellen: `index.html`, `final-h.css`, `final-h.js`, `refinements.css`,
  `refinements.js`, `layouts.css`, `content.js`, `model.svg`.
- [Kopierfertiger Umsetzungsauftrag](NEXT-CHAT.md), [Entscheidung](../../decisions/007-f1-layout-h.md).

H ist die einzige ausgewählte Variante. B ist verworfen. A, C, E, F, G, I und J
sind frühere Vergleiche. Von E/G stammt die linke Navigation, von C der rechte
Modellkontext. `round2.html` und `round3.html` sind nur Archive.

## Informationsarchitektur

Ein eigenständiges, skalierbares F1-Fenster enthält einen kompakten Fensterkopf,
eine gemeinsame Kopfleiste, drei Inhaltsbereiche und eine schmale Statuszeile.

Die Kopfleiste enthält in dieser Reihenfolge: **linkes Paneel-Icon**, Handbuch-
Kennzeichnung, **globale Sprachwahl C/Physim**, Suche, **rechtes Paneel-Icon**.
Die beiden Paneel-Icons bleiben auch nach dem Schließen ihrer Seite erreichbar.
Ihre Position ist an den äußeren Enden der Kopfleiste. Keine langen sichtbaren
Texte wie „Seitenleiste ausblenden“ und keine zusammengefasste Schließaktion.

Links stehen Themen und die direkten Abschnittssprünge des aktuellen Artikels.
Alle Sprungziele sind ohne Öffnen zusätzlicher Untermenüs zugänglich. Der aktive
Artikel und der aktive Abschnitt sind durch Position, Markierung und Zustand
erkennbar. Lange Listen dürfen vertikal scrollen.

Mittig steht der vollständige Artikel mit bestehendem Inhalt, Links, Formeln,
Code, Bildern und Tabellen. Die Lesebreite wird sinnvoll begrenzt; frei gewordener
Platz durch Schließen einer Seite darf genutzt werden. Es gibt keinen Verlust
oder Ersatz bestehender Handbuchinhalte durch die wenigen Mockup-Beispiele.

Rechts liegen zwei Tabs: **Modell** und **Beispiel**. Modell zeigt zum Thema
passende Grafik, Ausgangswerte, Einheiten und Hinweise; Beispiel den Ausschnitt
des global gewählten Sprachlernwegs mit Kopieraktion. Der Inhalt ist an den
Artikel gebunden. Für Themen ohne passende Kontextdaten gibt es einen klaren
Leerzustand statt eines erfundenen Pendelmodells. Keine lokale Sprachwahl rechts.

## Geometrie

Alle Maße sind logische Referenzmaße, keine physischen Bildschirmpixel.

| Größe | Zielbild bei 1080 px | Zielbild bei 760 px |
| --- | --- | --- |
| Linker Bereich, geöffnet | 250 px | 220 px |
| Rechter Bereich, geöffnet | 250 px | 220 px |
| Artikel, beide Seiten offen | ca. 578 px | ca. 318 px |
| Geschlossener Seitenbereich | 0 px | 0 px |
| Paneel-Icon / normale Trefferfläche | 20 px / 32 × 32 px | gleich |
| Kopfleiste / Paneelkopf | ca. 60 px / 46 px | gleich |
| Artikel-Innenabstand horizontal | 28 px | 20 px |
| Maximale Artikelbox inkl. Innenabstand | 810 px | verfügbare Fläche |

Fenster-Mindestgröße der bestehenden App: 760 × 540. Die Vorlage zeigt für den
Vergleich 1080 px Breite mit 680 px Inhaltsbereich. Ihre feste Vorschauhöhe ist
**keine** Vorgabe für ein festes natives Fenster. Native Inhalte verwenden die
tatsächlich verfügbare Fensterhöhe und scrollen unabhängig.

Die Vorschau trennt Lesetext (16/32 px) von kompakter UI-Schrift (ca. 12–13 px).
Die native Umsetzung muss zusätzlich bestehende Schrift-, Codeschrift- und
DPI-Einstellungen respektieren. Diese Werte dürfen keine Obergrenze für
Benutzerschriftgrößen werden. Vorhandene Formelskalierung und zugängliche
Lesetexte bleiben erhalten. Bei großen Schriften messen, umbrechen und scrollen,
statt Beschriftungen mitten im Wort zu verlieren oder den Artikel auf null Breite
zu drücken. Seiten bleiben an der Seite; keine neuen aufklappbaren Inhaltsmenüs.

## Unabhängige Zustände: zwingender Vertrag

| Links | Rechts | Sichtbarer Inhalt |
| --- | --- | --- |
| offen | offen | Navigation / Artikel / Kontext |
| geschlossen | offen | verbreiterter Artikel / unveränderter Kontext rechts |
| offen | geschlossen | unveränderte Navigation links / verbreiterter Artikel |
| geschlossen | geschlossen | volle nutzbare Lesefläche |

Die linke Aktion verändert ausschließlich den linken Sichtbarkeitswert, die
rechte Aktion ausschließlich den rechten. Theme- und Sprachwechsel dürfen
keinen der beiden Werte ändern. Suche darf die linke Trefferliste wieder öffnen;
der rechte Zustand bleibt dabei unverändert. Ein Artikelwechsel soll beide
gewählten Sichtbarkeiten erhalten. Rechts bleibt der gewählte Tab beim Schließen
und Wiederöffnen erhalten. Lesestelle und separate Scrollpositionen erhalten.
Eine dauerhafte Speicherung über Appstarts ist nicht festgelegt; dafür bestehende
Projektkonventionen prüfen, ohne diese Funktion für den Layoutauftrag zu erfinden.

**Belegter Fehler der Runde 3:** Nach `display:none` links platzierte CSS Grid die
Lesefläche automatisch in der ersten, nun null Pixel breiten Spalte. Rechts
rückte in die mittlere Spalte. Die linke Aktion änderte zwar nur einen booleschen
Wert, das sichtbare Layout war trotzdem falsch. Die frühere Prüfung übersah die
nullbreite Lesefläche. Die korrigierte Vorlage weist Navigation, Artikel und
Kontext explizit den Spalten 1, 2 und 3 zu. Native Rechtecke müssen dieselben
Eigenschaften aus zwei unabhängigen Zuständen berechnen.

## Globale C-/Physim-Wahl

Eine Wahl im Dokumentkopf gilt für den gesamten Dokumentationskontext:
Lernweg, Navigation, Anleitung, Suche, Beispielcode und Quellenangabe. Gemeinsame
physikalische Erklärungen bleiben ein Inhalt; sprachabhängige Abläufe und
Referenzseiten werden passend ausgewählt. Keine Duplikate mit nur örtlich
wechselnden Schnipseln. Die Wahl bleibt auf gemeinsamen Fachseiten bestehen.

Die vorhandenen C-/Physim-Lernwege und Topic-Zuordnungen nutzen und bei Bedarf
explizit erweitern. Die sechs Beispielthemen, generischen Texte und statischen
Pendelwerte des HTML-Prototyps sind keine vollständige Datenquelle für die App.
Beim Sprachwechsel den entsprechenden Lernweg/Artikel zuordnen; gemeinsame
Artikel dürfen mit erhaltener Sprache stehen bleiben. Für nicht zuordenbare
sprachspezifische Seiten einen nachvollziehbaren Einstieg des gewählten Lernwegs
verwenden. Suchindex und Suchumfang müssen dieselbe Sprachwahl respektieren.

## Farbvertrag Hell/Dunkel

Geometrie und Bedienung sind identisch. Die Farbwerte leiten sich aus den
vorhandenen `PS_UI_LIGHT`/`PS_UI_DARK`-Rollen in `app/design_tokens.h` ab.

| Rolle | Hell | Dunkel | Vorhandene Rolle |
| --- | --- | --- | --- |
| Artikelgrund | `#f3f5f8` | `#191a1e` | `doc_body` |
| Seitenflächen | `#f6f7fa` | `#1e1f23` | `window` |
| Kopf / Paneelköpfe | `#f2f3f6` | `#202125` | `header` |
| Haupttext | `#252a33` | `#e1e3e9` | `doc_text` |
| Nebentext | `#4e5764` | `#a6a9b1` | `muted` |
| Trennlinien | `#b4bcc8` | `#37393f` | `divider` |
| Suchfeld / Codefläche | `#ffffff` | `#1b1c20` | `edit` |
| Auswahl | `#c0d6f7` | `#265699` | `selection_active` |
| Auswahltext | `#191e26` | `#edeef2` | `text` |
| Akzent / Fokusring | `#0049a3` | `#64aaff` | `accent` |

Mockup-Fensterkontur: `#858f9d` hell / `#53555d` dunkel. Die äußere Galerieleinwand
ist keine Appfläche. Native Palette und semantische Rollen wiederverwenden,
keine unverbundenen Farbkonstanten im Renderer verteilen. Im echten Renderer
Diagramme mit passenden Farben zeichnen; die CSS-Filterdarstellung des kleinen
transparenten Mockup-SVGs ist kein neuer Vertrag für native Fachgrafiken.

Bestehender hoher Kontrast bleibt funktional erhalten. Er wird durch diese zwei
Zielbilder weder ersetzt noch als bereits neu abgenommen erklärt.

## Bedienung und Zugänglichkeit

Icons tragen eindeutige Namen („Themennavigation …“ / „Modellbereich …“), den
korrekten Offen-/Geschlossen-Zustand und einen Tooltip. Grafik und Zustand sind
nicht nur farblich unterscheidbar. Tastaturfokus ist in beiden Themes sichtbar.
Geschlossene Inhalte dürfen weder Fokus noch Eingaben oder veraltete native
Accessibility-Bereiche abfangen. Beim Schließen eines fokussierten Bereichs
den Fokus auf seinen weiterhin sichtbaren Schalter zurückführen.

Direkte Abschnittssprünge behalten eine logische Lesestelle. Die rechten Tabs
sind mit Tastatur bedienbar; Fokus und Auswahl bleiben nachvollziehbar.
Themen-/Such- und Artikelfokus müssen weiter isoliert von Editor und Simulation
bleiben. Theme-/Sprachwechsel verändern keine Simulationsdaten.

## Einstiegspunkte für die native Umsetzung

| Datei / Gruppe | Vorhandener Vertrag / Aufgabe |
| --- | --- |
| `app/documentation_window.inc` | Eigenes F1-Fenster, Mindestgröße, Fonts/DPI, Rechtecke und native UI-Gruppen |
| `app/documentation_ui.inc` | Topic-Registry, C-/Physim-Tracks, Themen, Inhalt, Suche, Fokus, Verlauf und Artikel |
| `app/main.c` | App-Zustand einschließlich `doc_navigation_hidden`, `doc_track`, `doc_contents`; Eingaben/Testabläufe |
| `app/design_tokens.h`, `app/design_ui.inc` | Gemeinsame Paletten und Kontrollzustände |
| `app/documentation_math.inc`, `app/documentation_inline_math_tests.inc` | Formelpositionen, Lesereihenfolge und Font-/Texture-Lebenszeiten erhalten |
| `app/documentation_tables.inc`, `app/documentation_media.inc` | Echte Tabellen-, Link-, Medien- und Kontextdaten erhalten |
| `app/documentation_keyboard_tests.inc` | Vorhandene GUI-/Tastatur-/Dichte-Abnahme um neue Zustände ergänzen |
| `tests/check_display_workflow.py` | Reale Dokumentations-, Navigation-, Sidebar-, Formel- und Fensterfälle |
| `tests/test_documentation_tracks.py`, `tests/check_documentation_anchors.py` | Lernwege und vollständige Themen-/Ankerbeziehungen |

`doc_navigation_hidden` kann die linke Seite abbilden; rechts braucht einen
eigenständigen Zustand. Das ist eine Verantwortungszuordnung, keine Vorgabe
eines endgültigen neuen Feldnamens. Die Topic-Registry und alle bestehenden
Seiten müssen erhalten bleiben. Kein HTML-WebView als Ersatz für den nativen
Renderer, keine Vollmigration auf einen neuen UI-Stack.

## Abnahme im nächsten Chat

1. Beide Themes mit allen vier Zuständen tatsächlich anzeigen; Rechtsanker nach
   linkem Klick und Linksanker nach rechtem Klick messen. Artikelbreite positiv
   und nutzbar verlangen, nicht nur Zustandsflags vergleichen.
2. 1080 × 780 sowie Mindestfenster 760 × 540, bestehende 16/32-px-Schriftfälle,
   Scrollen, Resize und reale DPI-/Monitorwechsel entsprechend verfügbarer Geräte.
3. Globale Sprache über Artikel, gemeinsame Fachseiten, Suche, Beispiele,
   Verlauf und Wiederöffnen prüfen; keine lokale Sprachwahl einführen.
4. Rechte Tabs, leere/erfolglose Suche, fehlenden Kontext, Codekopieren,
   Formeln/Inline-Formeln, Tabellen, Links und Medien wirklich bedienen.
5. Fokus beim Schließen, Wiederöffnen, Tastaturwege und native Accessibility
   einschließlich verborgener/alter Bereiche prüfen; hohe Kontraste erhalten.
6. Nachweis unveränderter Editorquellen und Simulationsdaten, passende vorhandene
   Tests und sinnvoll ergänzte Regressionen; Bildabnahme derselben Revision.
7. Windows/Linux/macOS nur anhand tatsächlich ausgeführter Builds und Tests
   bewerten; offene Plattformen konkret benennen. Geprüfte Arbeit committen und
   ohne Force-Push nach `origin` und `upstream` übertragen.

## Bereits belegt und noch offen

`tools/check_f1_design_h.cjs` besteht unter Windows mit Edge 155.0.4283.45:
32 Kombinationen aus Hell/Dunkel, 1080/760 px, 16/32 px Lesetext und vier
Seitenzuständen. Zusätzlich positive Artikelbreite, unabhängige Anker,
Lesestelle, globale Sprache, Themewechsel ohne Zustandsverlust, Tastatur und
Suche. Zehn Text-/Flächenpaare liegen rechnerisch zwischen 6,30:1 und 13,55:1.
Kein JavaScript-Laufzeitfehler. Acht finale Fensterbilder und die Übersicht
wurden als Designassets erzeugt und visuell geprüft.

Der genaue alte Fehler ist durch den Vorherlauf unter
`build/f1-layout-review/h-collapse-before.json` belegt: Artikel 578 → 0 px,
rechter Kontext 250 → 828 px bei linkem Klick. Temporäre Ausgaben sind ignoriert.
Der dauerhafte Prüfer verlangt nun positive Geometrie und unveränderte Anker.

Dies ist Browserabnahme des Zielbilds. Native Umsetzung, echte Screenreader-
Aufgaben, DPI-/Monitorwechsel und neue Linux-/macOS-Abnahme sind noch offen.
