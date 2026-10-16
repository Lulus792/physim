# Entwürfe für den Arbeitsbereich Simulieren

Stand 16. Oktober 2026. Vorschläge zur Auswahl, keine neue verbindliche
Produktentscheidung. Verantwortlich für diesen Entwurf: UI/UX-Design.

## Gemeinsamer Rahmen

Die kompakte gemeinsame Menü-/Fensterleiste und die drei gleich breiten Tabs
**Entwickeln**, **Simulieren**, **Auswerten** folgen der bestehenden
[Designrichtung](../../design-direction.md). Simulationsaktionen liegen im
Arbeitsbereich. Der native Leerstart ohne Projekt bleibt maßgeblich; die
Galerie startet zum Designvergleich bewusst mit einem Beispiel.

Farben verwenden semantische Rollen auf Grundlage von `app/design_tokens.h`:
dunkles Window #1e1f23, Canvas #141518, Text #edeef2, Muted #a6a9b1;
helles Window #f6f7fa, Canvas #e8ebf1, Text #191e26, Muted #4e5764.
Eigene ergänzende Rollen modellieren Raster, Pendelmaterial und Auswahlfläche.
Blau kennzeichnet Auswahl und Auslenkung, Orange den Geschwindigkeitsvektor
und dessen Kurve; Beschriftung und Einheiten ergänzen jede Farbbedeutung.
Geschlossene Flächen benötigen weder Blur noch Betriebssystemeffekte.

Das Abstandsraster verwendet überwiegend 4/8/12/16 px, kleine Bedienradien 6–8 px
und 12 px am Fenster. Die kleine UI-Schrift der Desktopstudie liegt bei 11–14 px,
Messwerte bei 23–25 px. Für die native Umsetzung sind diese Referenzmaße keine
Obergrenze: vorhandene Text-/DPI-Einstellungen berücksichtigen, messen und
gegebenenfalls umbrechen oder scrollen. Insbesondere sekundäre 9–10-px-Labels
der verdichteten Vorschau benötigen in der nativen Umsetzung eine Prüfung
bei den tatsächlich gewählten Schriftgrößen.

## Die drei Varianten

| Variante | Aufbau bei 1440 px Browserbreite | Zweck und Abwägung |
| --- | --- | --- |
| 01 Studio | Szenenbaum 204 px, flexible Szene, Inspektor 238 px; drei Messwerte und Zeitleiste unten | Allgemeiner Arbeitsraum. Direkter Objektzugriff und Eigenschaften; Signalverläufe bleiben der Auswertung bzw. Messlabor vorbehalten. |
| 02 Messlabor | Baum 178 px, kompaktere Szene, Inspektor 238 px; Messwerte und zwei Signalverläufe unter der Szene | Zusammenhänge von Bewegung und Signalen beobachten. Weniger Höhe für die Szene. |
| 03 Fokus | Beide Seiten zunächst geschlossen; große Szene, Auswahlkontext oben links; kompakte Messwerte unten | Bewegung und räumliche Orientierung priorisieren. Struktur und Details benötigen bei Bedarf einen Klick. |

Alle Varianten zeigen dasselbe Beispiel und dieselbe Zeitbasis. Die separaten
Steuerungen für Szenenbaum und Inspektor bleiben immer an den beiden Enden der
Bereichsleiste erreichbar. Ein geschlossener Bereich gibt seine Breite an die
Szene zurück. Die Seiten verwenden feste Grid-Spalten; das Schließen links darf
rechts weder einklappen noch die Szene in eine Nullspalte verschieben.

Die Beispiele bleiben inhaltlich am vorhandenen Funktionsumfang: Szenenhierarchie,
Darstellungsfilter, Kameravorgaben, Start/Pause/Schritt/Stop/Reset, Tempo,
Zeitleiste und Live-Rückkehr, Seed und Zeitschritt. Das Beispiel zeigt die
Laufeinstellungen lesend. Eine spätere native Umsetzung muss vorhandene
Eingabemöglichkeiten und den tatsächlichen Runner-Vertrag erhalten. Die
Messlabor-Zusammenstellung aus Szene und zwei Kurven ist ein Layoutvorschlag;
ihre native gemeinsame Darstellung ist hier nicht implementiert.

## Interaktions- und Zustandsvertrag

| Zustand / Aktion | Sichtbare Rückmeldung und Verhalten |
| --- | --- |
| Pausiert | Beschrifteter Status; Fortsetzen, Schritt, Stop und Zurücksetzen erreichbar. |
| Läuft | Pause ersetzt Fortsetzen; Einzelschritt deaktiviert. Simulationszeit und Messwerte folgen dem Lauf. |
| Bereit / nach Stop | Neuer Lauf beginnt bei t = 0. Der letzte Zustand bleibt bis zum Start sichtbar. |
| Zurücksetzen | Initialzustand pausiert. Nativ neue Messdatei anlegen und vorige Läufe erhalten, wie im vorhandenen Reset-Vertrag. |
| Zeit verschieben | Aufzeichnung ist erkennbar, Live-Auswahl aufgehoben. Die laufende Beispielzeit schreitet unabhängig weiter. |
| Live | Aktuellen Zustand wieder zeigen; Aufzeichnungswiedergabe beenden. |
| Abspielen | Aufzeichnung wiedergeben; andere Live-Laufzeit nicht verändern. |
| Leerer Workspace | Zentrale nächste Aktion, keine aktiven Laufaktionen; kein angeblich geladenes Szenenmodell. Die Demoaktion heißt bewusst Beispiel anzeigen. Nativ Ordner öffnen verwenden. |
| Lauf fehlerhaft | Verständlicher Fehler mit Hinweis auf erhaltene Daten und Protokoll. Demoaktion setzt nur das Beispiel zurück. Nativ Protokoll und Wiederanlauf anbieten. |
| Objektwahl | Baum und Inspektor zeigen dieselbe Auswahl; Größen mit Einheiten. |
| Seitenbereich schließen | Nur eigene Seite verändern; andere Seite, Zeit und Auswahl erhalten. |
| Themewechsel | Nur Farbrollen ändern; Paneelzustand und Zeitleistenposition erhalten. |
| Schmale Ansicht | Unter 650 px Seiteninhalte unter die Szene setzen; Zugänge erhalten. Dies ist eine Browservergleichsansicht, keine neue native Mindestfenstergröße. |

Die globale Navigation bleibt im nativen Produkt vollständig erreichbar. Die
Galerie bildet nur ihren Ort ab. Auch F5/F7 und vorhandene Shortcuts werden
hier nicht neu definiert. Keine globalen Browser-Tastenkürzel abfangen.

## Prüfung und Grenzen

Ausgeführt auf Windows mit Edge 155.0.4283.45, Node/Playwright/Sharp, lokaler
`file:`-Vorschau; Referenzbilder bei 1440 px Browserbreite und Geräteskalierung 1.
Kommando: `node tools/check_simulation_designs.cjs --export`, Exitcode 0.

- 24 Layoutfälle: drei Varianten × zwei Themes × 1440/1080/760/390 px.
  Keine horizontale Seitenüberschreitung, kollabierte Szene oder horizontal
  überlaufende zentrale Bedienbereiche in diesen Fällen.
- Sechs Interaktionsdurchläufe: unabhängige Seiten, Enter/Leertaste am Inspektor,
  Scrub/Live, Run/Pause, Step/Reset, neuer Lauf nach Stop, Leer-/Fehlerzustand,
  Themeerhalt, Auswahl, Vektorsichtbarkeit und Kamera-Vorgabe/Reset bestanden.
- Zwölf berechnete Text-/Hintergrundkontraste: 4,73:1 bis 15,62:1.
- Browserfall mit 200% vergrößerten Bedienbeschriftungen sowie 2×-Rasterisierung
  mit reduzierter Bewegung ausgeführt. Kein vollständiger nativer Text-/DPI-Test.
- Keine JavaScript-Laufzeitfehler. Die sechs Referenzbilder und die Galerie
  wurden visuell auf Hierarchie, Abstände und Beschriftungsüberlagerung geprüft.

Die Browserprüfung belegt diese Designstudie. Die tatsächliche native
Fensterintegration, Screenreader-Nutzung, Hochkontrastabnahme, alle
Tastaturaufgaben der nativen App, echte Monitorwechsel und Linux/macOS bleiben
offen. Keine Plattformgleichheit aus einer CI-Datei oder dieser Galerie ableiten.
Die Animation wird nur nach explizitem Start aktiviert; sie zeigt den Gegenstand
der Vorschau. Es gibt keine dekorative Daueranimation oder Übergangssperre.

## Übergabe nach Auswahl

Die gewählte Variante zuerst als Produktentscheidung festhalten. Anschließend
UI-Umsetzung und Designsystem mit Accessibility- und Plattformprüfung verbinden:
vorhandene Layout-/Toolbar-/Szenenbaum-/Zeitleistenfunktionen in `app/` verwenden,
keine Browserdemo als Runner übernehmen. Fenster, Plattformmenüs, native
Dateidialoge und Accessibility-Verträge der bestehenden App erhalten.
Native Zustände und echte Laufdateien separat prüfen; Windows, Linux und macOS
nur nach tatsächlich ausgeführten Tests als abgenommen bezeichnen.
