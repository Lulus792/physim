# Modellwerte bearbeiten, neu kompilieren und Bereiche skalieren

Zusätzlicher Nutzerauftrag vom 19. Oktober 2026. Messlabor und die Abschnittsstruktur
bleiben bestehen. Gewünscht sind mehrere ruhigere Datendarstellungen, veränderbare
Größen der Inhaltsbereiche sowie editierbare Modellwerte wie Fadenlänge mit
sichtbar angebotenem Neukompilieren. Dies ergänzt ADR 008 und die native Übergabe.

## Drei Vorschläge für dieselben Informationen

| Variante | Darstellung innerhalb der unveränderten Abschnitte |
| --- | --- |
| A · Ruhige Zeilen | Bezeichnung links, bündiger Wert rechts, dezente Trenner. Nähe zur früheren Informationsdarstellung; vorläufiger Standard für den Vergleich. |
| B · Wertefelder | Label über größerem Wert, ruhige einzelne Flächen; stärkere Betonung der Zahlen. |
| C · Kompakt | Engere, durchgehende Zeilen und kleinere Abstände; mehr Werte gleichzeitig sichtbar. |

Die drei Varianten sind neue Vorschläge, noch keine Nutzerentscheidung für eine
bestimmte Typografie. Auswahl, Feldwerte, aufgeklappte Gruppen, Lauf, Zeit und
Bereichsgrößen bleiben beim Darstellungswechsel erhalten. Hell/Dunkel verwenden
dieselben Rollen. Fadenlänge und Masse zeigen deutsche Zahlen wie **1,50**; die
Beispieleingabe akzeptiert Dezimalkomma und Punkt. Einheiten bleiben am Feld sichtbar.

Vorlage: [index.html](index.html), oben **Darstellung im Inspektor**.
Vergleiche: [Dunkel](inspector-styles-dark.png), [Hell](inspector-styles-light.png),
[beide Themes](inspector-styles-comparison.png).

## Veränderbare Inhaltsbereiche

Im Desktopentwurf gibt es sechs beschriftete, fokussierbare Trenner:

- Breite des Szenenbaums und Breite des Inspektors getrennt.
- Höhe des Messwert-/Kurvenbereichs gegenüber der Szene.
- Höhe der Zeitleiste gegenüber dem oberen Arbeitsbereich.
- Breitenverhältnis der beiden Kurven.
- Gesamthöhe der Arbeitsfläche als Browser-Demonstration der freien Höhenverteilung.

Ziehen per Maus/Pointer mit Capture, passende Pfeiltasten am Trenner,
Shift für feinere Schritte; Home/Doppelklick stellt die Vorgabe wieder her.
Name, Orientierung, aktuelle Größe und Grenzen sind im Browser über die
Separator-Semantik verfügbar. Ausgeblendete Seiten zeigen keinen toten Trenner.

Defaultwerte der Browservorlage: links 190 px, rechts 320 px, Arbeitsfläche 660 px,
Messbereich 150 px geschlossen/330 px mit Kurven, Zeitleiste 120 px. Die Bereiche
werden gegen die verbleibende nutzbare Mitte begrenzt. Unter 651 px zeigt die
Vergleichsseite gestapelte, scrollbar zugängliche Inhalte ohne Desktop-Trenner.
Die Maße und dieser Breakpoint sind keine neue native Mindestfenstergröße.

Native Umsetzung: an bestehendes Docking und dessen Größen-/Layoutzustände anbinden.
Mindestgrößen aus aktuellem Fenster, Inhalt und Schrift ableiten; keine
überdeckten Kontrollen und keine Nullbreite/-höhe. Größenwechsel verändert keine
Fachdaten. Szene und Plots reagieren auf ihre echten neuen Rechtecke und DPI.
Menü-/Fensterknöpfe bleiben reguläre App-Shell, keine frei skalierten Inhaltspanels.
Eine bestehende persönliche Anordnung bleibt erhalten. Persistenz zusätzlicher
neuer Dimensionen nur in einem kompatibel erweiterten Layoutvertrag vornehmen.

## Editieren mit notwendigem Build

Die neue Gruppe **Modellwerte** zeigt quellcode- bzw. buildgebundene Werte wie
Fadenlänge und Masse. Die davon getrennte Gruppe **Laufparameter** behält die
vorhandenen vom Modell deklarierten Laufparameter: Sie benötigen nach bisherigem
Vertrag nicht pauschal eine Neukompilierung. Die native Umsetzung darf diesen
Bestandsvertrag nicht still umdefinieren.

| Zustand | Anzeige und nächste Aktion |
| --- | --- |
| Unverändert | Kompilierter Stand am Feld; kein dauernder Buildhinweis. |
| Modellwert geändert | Markierte Gruppe und sichtbare Leiste außerhalb des scrollenden Inhalts: **Modellwerte geändert**, **Neu kompilieren**, **Verwerfen**. |
| Aktiver oder pausierter Lauf | Änderungen zunächst vormerken; alter Lauf bleibt auf seinem ursprünglichen Stand. Hauptaktion heißt ausdrücklich **Stoppen & neu kompilieren**. |
| Ungültiger Wert | Feldbezogene Ursache; Build nicht ausführen. Eingabe und vorheriger kompilierter Stand bleiben erhalten. |
| Build läuft | Sichtbarer Fortschritt und eingefrorener Eingabesatz. Nativ vorhandene Abbruch-/Diagnosemöglichkeiten erhalten. |
| Build erfolgreich | **Neuer Lauf** anbieten. Erst dessen Start verwendet das neue Modell. Aktiven Lauf oder frühere Aufzeichnungen nicht umetikettieren. |
| Build fehlgeschlagen | Änderungen bleiben vorgemerkt, Fehler/Diagnose und erneuten Versuch anbieten; letzten gültigen Build nicht als erfolgreich ersetzt ausgeben. |
| Verwerfen | Nur die vorgemerkten Modellwerte auf den zugehörigen kompilierten Stand zurücksetzen; keine anderen Editoränderungen oder Laufdaten verwerfen. |

Der Desktopprototyp demonstriert Validierung, Änderungshinweis, explizites Stoppen,
Buildfortschritt, Verwerfen und Übernahme im neuen Beispiellauf. Er führt **keinen
Compiler** aus, schreibt **keine Quelldatei** und ersetzt keine native Fehlerabnahme.
Die Kleinwinkel-Beispielkurven verwenden nach einem neuen Lauf die neue Länge;
die Szene bleibt eine illustrative Projektion. Aktive Werte und vorgemerkte Werte
sind getrennte Zustände.

## Erforderliche native Datenbindung

Der aktuelle Parameterkatalog ist kein allgemeiner sicherer Schreibzugriff auf
beliebige C-/Physim-Konstanten. Vor Umsetzung pro editierbarem Modellwert eine
explizite, typisierte Bindung an Modell-/Buildkonfiguration bzw. unterstützte
Quellendefinition festlegen: stabile Identität, Datei/Modellzuordnung, Typ,
Einheit/Skalierung, gültiger Bereich und Bedeutung für Build/Lauf. Fehlt die
Bindung, keine vermeintlich funktionierende Schreibaktion anbieten.

Falls dafür eine neue deklarative Modellbeschreibung oder ein Build-Konfigurations-
weg nötig ist, ihn begrenzt ergänzen und für C und Physim dokumentieren und testen.
Keine beliebigen Textersetzungen nach dem sichtbaren Namen „Fadenlänge“ und keine
ungeprüften Regex-Schreibzugriffe auf Nutzercode. Quellen/Projektwerte bleiben
nachvollziehbar, reproduzierbar und versionsverträglich; der vorhandene Editor-
und Dirty-Vertrag ist einzuhalten. Externe oder zwischenzeitliche Änderungen über
Revision/Snapshot erkennen und als Konflikt behandeln, statt sie zu überschreiben.

Builds bekommen einen festen Eingabesatz. Spätere Eingaben während eines Builds
bleiben eigene neue Änderungen; ein alter Workerabschluss darf sie nicht als
kompiliert markieren. Erst gültigen erfolgreichen Build veröffentlichen, davor
den letzten Stand erhalten. Prozessgrenzen, Limits und Abbruch beibehalten.
„Stoppen & neu kompilieren“ verlangt einen kontrollierten Stopp vor Neubau;
kein stilles Unterbrechen allein beim Tippen. Neue Läufe verwenden neue Artefakte,
bestehende Läufe und Quellsnapshots bleiben zu ihrem damaligen Modell gehörig.

## Ausgeführte Browserprüfung

Windows, Edge 155.0.4283.62. `node tools/check_inspector_variants.cjs --export`
prüft 24 Layoutfälle aus drei Stilen × zwei Themes × vier Fensterbreiten,
alle sechs Trenner per Tastatur, einen echten Pointerdrag mit Capture,
Rücksetzen, unveränderte Zeit/Auswahl beim Stilwechsel sowie den Änderungs-/
Buildablauf mit Dezimalkomma, explizitem Stoppen, Übernahme erst beim neuen Lauf,
Verwerfen und ungültigen Werten. Native Compiler-/Quellbearbeitungs- und
Plattformnachweise stehen noch aus. Die übrige Browserabnahme bleibt in DESIGN.md.
