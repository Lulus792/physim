# Sichtbarer nativer Messlaufvergleich

Die Messdatenansicht zeichnet jetzt bis zu acht ausgewählte gespeicherte Läufe.
Kurven, Legende, Referenz, Statistik und der freigegebene Grafikexport verwenden
denselben unveränderlichen Datenstand. Der bisherige aktive Lauf bleibt als
eigene Quelle für die rechten Laufdetails und den vollständigen Rohdatenexport
ausdrücklich benannt. Dies ist ein weiterer Umsetzungsschritt; die gesamte
Oberfläche ist damit weiterhin nicht pixelidentisch abgenommen.

## Auswahl und Daten

- Die Auswahlreihenfolge bleibt erhalten; eine explizite Referenzaktion rückt
  den betreffenden ausgewählten Lauf nach vorn. Die Suche ändert keine Auswahl.
- Jeder Lauf besitzt seinen eigenen Zeitvektor. Kanalname, Dimension und
  Winkel-/Skalarrolle bestimmen die Zuordnung, statt eines gemeinsamen
  Spalten- oder Sampleindex. Ein Wechsel des aktiven Laufs erhält den gewählten
  Kanal auch bei anders sortiertem Schema, sofern er dort vorhanden ist.
- Statuswerte und Lücken folgen dem vorhandenen Snapshot-/Series-Vertrag.
  Die begrenzte Vorschau erhält Spitzen und Segmentgrenzen. Eine reduzierte
  Vorschau wird benannt; die Statistik verwendet sämtliche gültigen Quellwerte
  vor der Reduktion. Mittelwert, Stichprobenstandardabweichung, Minimum und
  Maximum werden in derselben Anzeigeeinheit wie die Kurve dargestellt.
- Fehlende, inkompatible oder beschädigte Eingaben erhalten eine benannte
  Fehlermeldung und keine erfundene Nullkurve. Gültige andere Läufe bleiben
  sichtbar. Ist die gewählte Referenz nicht verfügbar, bekommt keine andere
  Kurve still deren Referenzkennzeichnung. Ressourcenfehler und Abbruch
  veröffentlichen weiterhin keinen halben Datenstand.
- Ohne Auswahl erscheint der Leerzustand. Ohne darstellbare gemeinsame Kurve
  gibt es eine Fehlerrückmeldung und einen erneuten Ladezugang. Beim Entfernen
  des aktiven Laufs wird ein verbleibender Lauf als nächster aktiver Lauf geladen.

Die bisherige strenge Konstruktion `ps_graphic_snapshot_from_runs` bleibt
erhalten. Die zusätzliche Vergleichskonstruktion hält pro ausgewählter Eingabe
Resultat und Kurvenzuordnung fest. Sie verwendet den strengen Datenpfad jeweils
für einen Lauf und übernimmt nur dessen bereits geprüfte eigene Kopie.

## Besitz und Hintergrundarbeit

`app/analysis_comparison.c` besitzt höchstens einen Worker und eine aktuelle
wartende Auswahl. Der Auftrag enthält eigene Kopien aller Pfade, Projektkontext,
Revision, Kanal und Anzeigeeinheit; er enthält weder App- noch UI-/GL-Zeiger.
Auswahl- oder Einheitenwechsel verwerfen den alten sichtbaren Cache sofort und
fordern kooperativen Abbruch an. Ein fertiger Auftrag darf nur bei passender
Anforderungsversion veröffentlicht werden. Auch eine zwischenzeitliche Rückkehr
zu einem früheren Schlüssel erlaubt keinen alten Abschluss.

Der Hauptthread holt nur einen bereits als fertig gemeldeten Auftrag ab. Der
Shutdown fordert Abbruch an und verbindet sich vor Freigabe mit dem einen
besessenen Thread. Ein laufender Kernaufruf ist weiterhin nicht mitten in seiner
Snapshot-/Reduktionsoperation unterbrechbar. Das ist kein Nachweis eines
maximalen interaktiven Abbruch- oder Framezeitbudgets.

Temporäre Series-Dateien liegen neben dem vorhandenen persönlichen bzw.
testbezogenen Einstellungsstand. Ein noch nicht gebautes oder schreibgeschütztes
Quellprojekt benötigt dafür keinen eigenen Buildordner. Der bestehende
Scratch-Vertrag des Kerns gilt weiter; es entsteht kein unbegrenzter Samplecache
im UI-Thread.

## Darstellung und Grafikexport

Die Kurven und ihre umbrechende Legende verwenden dieselben Farb- und
Linienrollen wie die gemeinsame Exportfigur. Die Statistik besitzt eine Zeile
je ausgewähltem Lauf; eine fehlende Statistik bleibt als nicht verfügbar markiert.
Große Schrift und schmale Spalten umbrechen Bedienelemente und Vorschauhinweise.
Die bestehende feste Panelanordnung und die vorhandenen Trenner bleiben erhalten.

**Grafik exportieren…** steht jetzt auch an der Messkurve neben den Zoomaktionen.
Der gemeinsame Dialog kopiert den aktuell angezeigten Cache samt tatsächlicher
X-/Y-Ausschnittsgrenzen, Referenz, Kanal und Einheit. Er öffnet die Quelldateien
nicht erneut. Spätere Auswahl-, Lade- oder Einheitenwechsel verändern den offenen
Export nicht. Der bestehende CSV-Weg bleibt als **gesamter aktiver Lauf** benannt.

## Nachweise und Gegenproben

Die tatsächlichen Quellstände und Ausführungen stehen im
[Prüfmanifest](analysis-comparison-2026-10-22/validation.json).
Getrennte Kern-, UI-, Release- und historische Fehlläufe werden dort nicht zu
einem erfundenen gemeinsamen Gesamtlauf zusammengefasst.

| Tatsächlich ausgeführt | Ergebnis |
| --- | --- |
| Windows MSVC/ASan, Stand 7 | Berichtsdialog und beide Vergleichsaufgaben: 3/3 |
| Alpine-Dev GCC/ASan+UBSan, X11/Mesa, Stand 7 | Dieselben drei Gruppen: 3/3 |
| Windows Release, Stand 7 | Berichtsdialog sowie Vergleich dunkel, hell, hell/groß/schmal und hoher Kontrast: 5/5 |
| Unveränderter Cache-/Snapshotkern | Windows Debug Stand 1, Linux Stand 5 und Release Stand 6 jeweils 2/2 |
| Bisherige Layout-/Einheitenaufgaben, Stand 5 | Windows und Linux jeweils 4/4 einschließlich der beiden neuen Vergleichsfälle |

Die Produktions-C-Quellen bleiben von Stand 5 bis 7 gleich. Die späteren Stände
korrigieren die benannten Fixture-/Aufnahmephasen. Alle 24 erneut verglichenen
Berichts-Exportdateien sind bytegleich zum zuvor eingefrorenen Stand.

[Windows dunkel](analysis-comparison-2026-10-22/windows-release-comparison_dark-comparison-eight.png) ·
[Windows hell](analysis-comparison-2026-10-22/windows-release-comparison_light-comparison-eight.png) ·
[Linux mit großer Schrift](analysis-comparison-2026-10-22/linux-debug-comparison_light_large_narrow-comparison-eight.png) ·
[benannte unverfügbare Eingabe](analysis-comparison-2026-10-22/windows-debug-comparison_dark-comparison-partial.png).


Die neuen nativen Aufgaben erzeugen acht echte Dateien mit verschiedenen
Startzeiten, Schrittweiten und Kanalreihenfolgen, Statuslücken sowie 6.001
ursprünglichen Samples mit erhaltenen positiven/negativen Spitzen. Sie wechseln
die Referenz per Tastatur, zoomen und verschieben tatsächlich und öffnen den
Grafikexport. Währenddessen wechselt der sichtbare Cache auf einen anderen Lauf
und von m nach cm; der offene Export behält acht Läufe und m. Eine seiner
Testquelldateien wird vor der Ausgabe entfernt. Vier PNG-Größen, Gesamt-PNG,
Gesamt-/Ausschnitt-SVG und die unveränderte Referenz werden geprüft.

Ein unabhängiger PNG-Prüfer sucht alle acht Kurvenfarben innerhalb der tatsächlichen
Plotfläche, einschließlich der erwarteten antialiasierten RGB-Mischungen. Eine
absichtlich auf eine Kurve begrenzte native Anwendung besitzt weiterhin acht
Cache-/Exportkurven, wird aber für die fehlende zweite sichtbare Kurve abgewiesen.
Eine zweite negative Variante akzeptiert überholte Workerabschlüsse und scheitert
an der verlangten aktuellen Veröffentlichung. Diese Gegenläufe sind keine
bestandenen Produktläufe.

Die Kernprüfungen erhalten außerdem alle bisherigen Snapshotfälle und ergänzen
unverfügbare Eingaben/Referenzen, Kopien und 18 Allokationsfehlerstellen der
Vergleichskonstruktion. Die Cacheprüfung prüft eigene Auftragsschlüssel,
überholte Antworten, acht Zeitachsen, Einheiten, Leeren und Shutdown.

Frühe UI-Gegenläufe enthielten falsche Testannahmen: ein nicht sichtbarer
Referenzlauf in der nach Datum sortierten Liste, ein noch nicht gewährter
Fokusauftrag, ein nicht sichtbarer Speicherbutton sowie Proben vor dem ersten
tatsächlichen Rendern. Der RGB-Prüfer musste antialiasierte statt ausschließlich
deckender Linienpixel prüfen. Die finale Folge wartet auf die jeweiligen realen
Zustände und verwendet die vorhandene Such-/Tabinteraktion. Spätere Aufnahmen
halten den geprüften Zustand vor den nächsten Fixtureaktionen fest. Der alte
Berichtstest wählt seinen echten Rohdatenlauf jetzt ausdrücklich aus; sein
früherer Null-Auswahlzustand kann keinen sichtbaren Rohdatenplot mehr voraussetzen.

## Verbleibende Abnahme

Diese Aufgaben ersetzen weder den vollständigen Referenzabgleich aller vier
Bereiche noch die gesamte Desktop-Matrix. Insbesondere Schriftfamilie/-gewicht,
frühe Integer-Rasterung, weitere Komponenten und kontinuierliche Größenwechsel
bleiben abzugleichen. Externe Screenreader, physische DPI-/Monitorwechsel,
ausgehängte Vergleichsquellen, tatsächliche OS-Dateiauswahlbedienung und macOS
besitzen weiterhin keine vollständige aktuelle Abnahme. Die bisherigen lokalen
Accessibility-Änderungen sind erhalten; die App-Builds sind keine isolierten
sauberen Checkouts nur dieses Patches.
