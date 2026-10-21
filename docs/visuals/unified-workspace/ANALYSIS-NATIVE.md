# Native Auswertung: Umsetzung und offener Referenzabgleich

Stand: 20. Oktober 2026. Dieser Bericht dokumentiert einen Teil der laufenden
Angleichung der gesamten Oberfläche. Er ist keine vollständige Designabnahme.
Der anschließende [Abgleich einzelner Felder und Konturen](ANALYSIS-CONTROLS.md)
ergänzt diesen eingefrorenen Stand mit eigenen Bildern und eigenem Prüfumfang.
Darauf folgt die [kompakte Einheitenwahl mit Textkorrekturen](ANALYSIS-UNITS.md).
Maßgeblich bleiben der [Gesamtauftrag](NEXT-CHAT.md), [Resizing](RESIZING.md),
[Auswertungsvertrag](../analysis-workspace/IMPLEMENTATION.md) und der freigegebene
[Grafikexport](EXPORT-EXAMPLE.md) mit [ADR 010](../../decisions/010-analysis-graphic-export.md).

## Nativ umgesetzter Aufbau

Die bisherige Auswertungsseite verwendet jetzt die gewählte Anordnung: Kopf mit
Analyseaktionen, echte durchsuchbare Messdateien links, Messdaten-/Ergebnistabs
in der Mitte und Kanal, Anzeigeeinheit sowie ausdrücklich benannter aktiver Lauf
rechts. Laufliste und Details lassen sich unabhängig einblenden. Normale
Anfangsbreiten sind 232 und 242 logische Pixel; die Kopfunterkante liegt bei 198.

Die Auswahl bleibt auf acht echte Analyseeingaben begrenzt. Die erste Auswahl
ist die Referenz. Dateiname, Dateigröße, bekannte Punktzahl und Zeitspanne kommen
aus dem tatsächlichen Bestand. Unbekannte Modellparameter werden nicht aus der
Browserdemo ergänzt. Der Projekt-Analysecode ist entsprechend beschriftet und
wird nicht als Herkunftsnachweis eines älteren Berichts ausgegeben. Dessen
vorhandene vollständige Herkunftsansicht bleibt erhalten.

Bestehende Köpfe und Tabs dienen weiter als Ziehflächen für eigene native Fenster.
Es wurden keine sichtbaren Zusatzleisten oder Auskoppeln-Buttons hinzugefügt.
Schließen des Zusatzfensters bringt den Bereich an seinen festen Platz zurück.
Die Daten und Auswahl bleiben erhalten.

Zwischen Messkurve und Statistik liegt eine ein Pixel hohe Grenze mit neun
Pixel großer Trefffläche. Ziehen verändert die Kurvenhöhe, Pfeiltasten ändern
sie um 10 beziehungsweise mit Umschalt um 40 Pixel. Home/End erreichen die
Grenzen; Enter und Doppelklick stellen die automatische Höhe wieder her. Diese
folgt dem Seitenverhältnis der Referenzgrafik (800 × 326). Escape, Fokusverlust
und Bereichswechsel verwerfen eine noch laufende Ziehbewegung. Ein bloßer Klick
ändert weder die automatische Höhe noch einen wegen Platzmangel begrenzten
Breitenwunsch. Bereits abgeschlossene Tastaturschritte bleiben dagegen erhalten.

Bei hoher Kurve bleibt der untere Inhalt einschließlich Statistik und CSV-Aktion
durch Scrollen erreichbar. Das Scrollen verändert weder Zoom noch Messwerte.

Die gewünschte Lauflistenbreite darf bis 480 Pixel reichen. Einstellungen und
benannte Layouts akzeptieren dieselbe obere Grenze; ältere vorhandene Dateien
bleiben lesbar. Ein globales Speichern von Schrift oder Theme bewahrt die
Werkbankgeometrie und übernimmt die bestätigte Bereichsgeometrie in den
zugehörigen Sitzungszustand. Abbrechen stellt keinen fremden Bereich um.

## Noch erforderliche Produktarbeit

Der spätere [Export-Umsetzungsstand](GRAPHIC-EXPORT-NATIVE.md) ergänzt eine
eigene, geprüfte Diagrammkopie und einen gemeinsamen Mehrlauf-Datenbaustein
einschließlich Zeitachsen, Einheiten, Masken und vollständiger Statistik.
Die sichtbare Datenansicht und der Exportdialog sind noch daran anzubinden;
die folgenden Produktpunkte bleiben dadurch offen.

| Anforderung | Aktueller, eingegrenzter Stand |
| --- | --- |
| Mehrere echte Rohdatenkurven | Noch offen. Die vorhandene Analyse kann mehrere Dateien erhalten; die neue Rohdatenansicht zeichnet weiterhin den ausdrücklich aktiven einzelnen Lauf. |
| Vollständige Statistik je Vergleichslauf | Noch offen. Die aktuelle Tabelle enthält die vollständige Kanalstatistik des aktiven Laufs, unabhängig vom Zoom. Sie ist noch keine Vergleichstabelle je Lauf. |
| Unabhängige Zeitvektoren, passende Kanäle und Lücken im Vergleich | Der bestehende Einzelpfad bleibt erhalten; ein begrenzter Mehrlaufcache mit veraltete Antworten verwerfenden Workern ist noch umzusetzen. |
| Freigegebener Grafikexportdialog | Noch offen. Die bisherigen echten Berichts-CSV-/SVG-/PNG-Wege einschließlich Größen und Ausschnitten bleiben erhalten. Sie ersetzen weder den neuen Dialog noch die native Bildkopie. |
| Analysecode in der gewählten Werkbank | Der bestehende Editor ist erreichbar; vollständige gemeinsame Gestaltung und Zuordnung bleiben abzugleichen. |
| Detailherkunft wie in der Demo | Echte vorhandene Angaben werden verwendet. Weitere zuverlässig gespeicherte Modellfelder sind noch zuzuordnen; fehlende Metadaten bleiben unbekannt. |
| Seitengrenzen der Browserdemo | Nativ gelten derzeit mindestens 208 Pixel je Seite und 300 für die Mitte. Die Demo erlaubt 140/180. Das ist eine bestehende Implementierungsgrenze, kein nachgewiesenes technisches Hindernis. |
| Kleine Fenster und große Schrift | Tatsächliche native Varianten und Bilder prüfen. Eine bestandene Zustandsfolge allein beweist keine vollständige visuelle Übereinstimmung. |

Sichtbar abzugleichen sind außerdem Suchfeldrahmen und Einzüge, Kartenabstände,
die aktive Tablinie, die Flächen der Kanalwahl und die Einheitenwahl. Letztere
öffnet derzeit den vorhandenen ausführlichen Einheitendialog; der kompakte
Auswahlablauf der Vorlage ist noch nicht umgesetzt. Große Schrift benötigt
weiteren visuellen Abgleich bei Achsen, Statistik und Metadaten. Diese Punkte
sind offene Umsetzung, keine belegten technischen Unmöglichkeiten. Die eigenen
Bereichsgeometrien gelten derzeit für die Sitzung; ein vollständiger Erhalt
über einen Neustart ist noch nicht umgesetzt und nachgewiesen.

Auch die Werkbank-Standardbreiten sind noch gesondert abzugleichen: Der
Produktstandard verwendet 248/280, während die ältere visuelle Werkbankfixture
218/255 setzt. Diese Fixture beweist daher nicht den passenden Produktstart.
Fünf gemeinsame Menüs, Theme-Aktion und der neue vollständige F1-H-Bildvergleich
bleiben Teil des übergeordneten offenen Auftrags.

## Prüfprovenienz und Grenzen

Der [eingefrorene Nachweis](analysis-native-2026-10-20/validation.json) gehört
zum internen Quellstand 21, keiner Produktversionsnummer. Er erfasst die 24
zugehörigen Quelldateien, zehn unverändert vorgefundene Dateien anderer lokaler
Accessibility-Arbeit, Programmhashes, ausgeführte Befehle und Bildquellen. Die
Builds enthalten diese fremden Änderungen; ein sauberer Checkout wurde damit
nicht geprüft.

| Tatsächliche Ausführung | Umfang pro Plattform |
| --- | --- |
| Windows und Alpine-Dev / Quellstand 21 | Fünf bestandene Auswertungsvarianten: Dunkel, Hell, hoher Kontrast bei 1440 × 1000 sowie Dunkel/Hell bei 1080 × 740 und UI-Größe 22. |
| Windows und Alpine-Dev / derselbe Programmhash | Je 15 bestandene Funktionsgruppen: Einstellungen, Einstellungstastatur, Werkbank, Layouts, Teilfenster, Messlabor, Inspektor, kleine/große Toolbar, Laufdetails bei 16/22, Berichtsdetails, Plotexport, Eingabeisolation und F1 H. Insgesamt je 52 native Programmausführungen einschließlich der Auswertungsmatrix. |
| Unveränderte Codec-Eingaben / Quellstand 12 | Je zwei bestandene Binärformat-Tests für Einstellungen und Layouts einschließlich 480/481-Grenze und Erhalt vorheriger Daten bei ungültigen Eingaben. Diese Ergebnisse werden nicht nachträglich als Quellstand-21-Ausführung bezeichnet. |
| Direkter Vergleich mit Original-PNGs | Vier Vergleiche: Auswerten Hell/Dunkel × Windows/Linux; je elf begrenzte Prüfungen des gemeinsamen Rahmens. Keine vollständige Designbewertung. |

Die Auswertungsmatrix prüft alle vier Seitenzustände, drei echte Zusatzfenster
mit Rückkehr, Einstellungen aus Auswerten und Simulieren sowie tatsächliche
Breiten und Höhen. Sie prüft 10-/40-Pixel-Tastaturschritte, Abbruch einer
Ziehbewegung durch Escape/Fokus-/Bereichswechsel, unveränderte begrenzte Wünsche
bei einem Klick und die Rückkehr zur automatischen Kurvenhöhe. Bei hoher Kurve
scrollt sie zur tatsächlich sichtbaren und fokussierbaren CSV-Aktion; Zoom und
Statistik bleiben bytegleich. Die Fixture erzeugt zwei gültige PSrun-Dateien mit
deterministischen synthetischen Testsignalen. Das ist kein Nachweis eines
physikalisch korrekten Modells und keine Mehrlaufdarstellung der Produktansicht.

Die Ereignisse stammen aus der nativen Testfixture. Auf Windows bewegen diese
Ereignisse den physischen Mauszeiger nicht; SDL-Mausfang wird deshalb gegen den
tatsächlichen Mausfokus geprüft. Ein vollständiger physischer Windows-Nachweis
für Ziehen über die Fenstergrenze wird daraus nicht abgeleitet. Unter Linux
war tatsächlicher Mausfokus samt Fang- und Freigabeflag beobachtbar.

Frühere Fehler bleiben ausdrücklich negative Ergebnisse: unter anderem
abgeschnittene große Bedienelemente, ein nicht gekennzeichnetes und deshalb
gefiltertes Fokusereignis und die fehlende Rückkehr eines erweiterten
Werkbank-Testschritts. Das [negative Bild aus Stand 14](analysis-native-2026-10-20/counterexample-linux-large-controls-v14.png)
bleibt erhalten. Die korrigierten finalen Abläufe ersetzen diese Ergebnisse
nicht rückwirkend.

| Native Aufnahme | Windows | Linux |
| --- | --- | --- |
| Dunkel | [Bild](analysis-native-2026-10-20/windows-analyze-dark-v21.png) | [Bild](analysis-native-2026-10-20/linux-analyze-dark-v21.png) |
| Hell | [Bild](analysis-native-2026-10-20/windows-analyze-light-v21.png) | [Bild](analysis-native-2026-10-20/linux-analyze-light-v21.png) |
| Nur Details | [Bild](analysis-native-2026-10-20/windows-details-only-v21.png) | [Bild](analysis-native-2026-10-20/linux-details-only-v21.png) |
| Nur Mitte | [Bild](analysis-native-2026-10-20/windows-center-only-v21.png) | [Bild](analysis-native-2026-10-20/linux-center-only-v21.png) |
| Nur Laufliste | [Bild](analysis-native-2026-10-20/windows-runs-only-v21.png) | [Bild](analysis-native-2026-10-20/linux-runs-only-v21.png) |
| Mindestfenster, große Schrift, dunkel | [Bild](analysis-native-2026-10-20/windows-minimum-dark-v21.png) | [Bild](analysis-native-2026-10-20/linux-minimum-dark-v21.png) |
| Mindestfenster, große Schrift, hell | [Bild](analysis-native-2026-10-20/windows-minimum-light-v21.png) | [Bild](analysis-native-2026-10-20/linux-minimum-light-v21.png) |
| Hoher Kontrast | [Bild](analysis-native-2026-10-20/windows-contrast-v21.png) | [Bild](analysis-native-2026-10-20/linux-contrast-v21.png) |
| Breite Laufliste | [Bild](analysis-native-2026-10-20/windows-wide-runs-v21.png) | [Bild](analysis-native-2026-10-20/linux-wide-runs-v21.png) |
| Kurve zurückgesetzt | [Bild](analysis-native-2026-10-20/windows-curve-restored-v21.png) | [Bild](analysis-native-2026-10-20/linux-curve-restored-v21.png) |
| Statistik und CSV nach Scrollen | [Bild](analysis-native-2026-10-20/windows-statistics-scrolled-v21.png) | [Bild](analysis-native-2026-10-20/linux-statistics-scrolled-v21.png) |
| Eigenes Lauflistenfenster | [Bild](analysis-native-2026-10-20/windows-external-0-v21.png) | [Bild](analysis-native-2026-10-20/linux-external-0-v21.png) |
| Eigenes Messdatenfenster | [Bild](analysis-native-2026-10-20/windows-external-1-v21.png) | [Bild](analysis-native-2026-10-20/linux-external-1-v21.png) |
| Eigenes Detailfenster | [Bild](analysis-native-2026-10-20/windows-external-2-v21.png) | [Bild](analysis-native-2026-10-20/linux-external-2-v21.png) |

Alle 28 aktuellen PNGs und das negative Bild wurden verlustfrei aus nativen
BMP-Aufnahmen konvertiert. Datei-Hashes und erneut verglichene dekodierte
RGB-Pixel stehen im Nachweis. Keine Retusche, Skalierung oder Umfärbung.

Die geprüften lokalen Programme liegen unter
`build/native/Design-analysis/bin/physim.exe` und
`build/native/Design-analysis-linux/bin/physim`. Vorhandene andere Buildordner
wurden dadurch nicht aktualisiert.

Nach dem Einfrieren wurden 56 eigene ersetzte Zwischenbilder und JSON-Beiblätter
aus diesem neuen Bildordner entfernt (5.894.923 Byte). Die aktuellen 28 Bilder,
das negative Bild, Originalreferenzen und Belege offener Fehler bleiben erhalten.

Die ursprünglichen Referenz-PNGs sowie HTML/CSS sind unverändert. Das Browser-
werkzeug blockierte die lokale file-Adresse; daraus wird keine neue interaktive
Browserabnahme behauptet. Native Textrasterung kann zwischen Plattformen abweichen.
Abstände, Farben, Kontrollformen und erreichbare Inhalte bleiben dennoch
anzugleichen. macOS, manuelle Screenreader und physischer Monitor-DPI-Wechsel
sind in diesem Umsetzungsschritt nicht nachgewiesen.
