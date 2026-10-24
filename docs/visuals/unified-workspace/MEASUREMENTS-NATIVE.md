# Nativer Messbereich

Fortsetzung nach `8d483ab63fa8ece4e203a5c5f6b5efa46a4039f8`. Der gesamte
Oberflächenabgleich einschließlich Entwickeln, Auswerten und F1 bleibt offen.

## Gestaltung und echte Inhalte

Maßgeblich sind `measure-heading`, `metric-grid`, `metric-line`, Schalter und
Diagramme in `simulation-designs/index.html`/`simulation.css`, die gemeinsame
Palette aus `bridge.js` und der Größenvertrag aus `RESIZING.md`. Eine erneute
Browserausführung ist damit nicht behauptet.

Die bisher einheitlich große Zahl-/Einheitenzeile wird in einen großen Wert
und eine kleine gedämpfte Einheit aufgeteilt. Kanalnamen verwenden die kleine
Metadatenschrift. Außenabstand, Kartenabstand, Unterstrich und Schalter folgen
den jeweiligen CSS-Rollen; große App-Schrift skaliert sie mit. Schmale Bereiche
brechen Karten und lange Inhalte um, statt die gewählte Schrift kleinzurechnen.

Gewöhnliche Messwerte erhalten wie die Vorlage zwei Nachkommastellen und ein
Dezimalkomma. Kleine nichtnullige Werte und sehr große Beträge verwenden eine
kompakte wissenschaftliche Schreibweise, damit die Anzeige weder null behauptet
noch überläuft. Tooltip und zugänglicher Kartentext behalten den bisherigen
genaueren Wert. Dies verändert weder Rohdaten noch gespeicherte Messungen.

Die native Implementierung zeigt die beiden tatsächlich gewählten Kanäle.
Ihre Namen, Einheiten und Anzahl werden nicht durch illustrative Beispielwerte
wie „Höhe h“ ersetzt. Die native Übergabe fordert höchstens zwei gleichzeitige
Verläufe und Zugriff auf alle echten Kanäle. Die kleine Kanalauswahl bleibt
deshalb als echte Bedienaktion erhalten, während die Demo nur Beschriftungen
zeigt. Sie verwendet eine flache Textzeile mit Chevron und den bestehenden
nativen Menü-/Tastaturweg.

Die Kurvenfläche verwendet horizontale Gitterlinien, kleine Achsenwerte und
einen gedämpften gestrichelten Zeiger an der betrachteten Zeit. Berechnung,
Anzeigeeinheiten und Statusmasken verwenden die vorhandenen echten Zeitreihen.
Ungültige Werte werden nicht miteinander verbunden. Leer-/Fehlertexte bleiben
erhalten. Die begrenzte Historie ist keine vollständige Messdatei.
Sie wird ausdrücklich mit Punktzahl und gegebenenfalls „ausgedünnt“ beschriftet.
Diagrammaußenabstand und Zwischenraum folgen derselben CSS-Geometrie wie die
Karten; die neun Pixel breite Trefffläche sitzt im Zwischenraum. Tastaturschritte
werden auf die tatsächliche verfügbare Kurvenbreite umgerechnet.

## Prüfung und Grenzen

Die neue Probe baut das echte C-Pendel, pausiert den Runner und hält Quelltext,
Samples, Szene, Zeit und Messdatei fest. Sie prüft die tatsächliche native
Schalteraktivierung per Tastatur, Kanalwechsel im Menü, Umrechnung desselben
Samples und Rückkehr. Beide Schriftquellen und sechs Schrift-/Theme-/Breitenfälle
werden getrennt aufgezeichnet. Die Probe vergrößert ihre Kartenfläche ausdrücklich
auf 240 px und die Kurvenfläche auf 420 px; sie beweist keine feste Standardhöhe.
Die bestehenden Messlabor-/Fensterfälle prüfen daneben ihre eigenen normalen
und kleinen Größen einschließlich Scrollen und Herausziehen.

Ausgeführte Ergebnisse werden nach Abschluss mit Quell- und Binärhashes ergänzt.
Ein erfolgreicher Build ist kein Bediennachweis. Praktische Screenreader-Aufgaben,
physische DPI-/Monitorwechsel, aktuelle Windows-GUI und macOS bleiben offen.
Drei separat erhaltene fremde AT-SPI-Dateien liegen im lokalen Kompiliergraphen;
der lokale Linux-Lauf ist deshalb kein Clean-Checkout-Nachweis.

Stand 1 besteht Windows-Build und vier von sechs Linux-Gruppen. Der neue native
Prüfer verlangt nach dem Wechsel zur Kanalauswahl noch den vorherigen Schalterfokus;
dieser Reihenfolgefehler wird korrigiert. Die große Messlaborprobe erwartet den
Kurventrenner noch ohne den durch größere Typografie nötigen Scrollweg. Stand 2
ergänzt diesen Weg über echte Mausradeingabe, ohne die Größen-, Drag- oder
Zustandsbedingungen zu entfernen. Der neue native Szenenablauf von Stand 1
bleibt separat bestanden.

Eine echte externe Fensteraufnahme von Stand 1 zeigt außerdem einen 6–10 px
breiten Keil statt des angeforderten 2-px-Unterstrichs. Der lokale Nuklear-Zeichner
begrenzt Rundungen nur auf die volle Rechteckhöhe; CSS begrenzt sie auf die halbe.
Stand 2 korrigiert ausschließlich den Radius dieses flachen Elements. Die neue
Pixelprüfung misst die tatsächliche Dicke an mehreren inneren Stellen.

Stand 2 besteht die native Bedienfolge mit sechs Darstellungen und
Formatter-Grenzfällen. Seine anschließende Systemschrift-Bildprüfung verwirft
jedoch das Umrechnungsbild: Der Prüfer hatte Einheiten/Kanäle unmittelbar nach
der Aufnahmeanforderung bereits zurückgesetzt. Die Datei zeigt dadurch den
zurückgesetzten Zustand, das Protokoll noch den umgerechneten. Stand 3 trennt
Aufnahme und anschließende Mutation in unterschiedliche Zeichenzyklen. Die
kleinen Einheiten erhalten zudem anhand der tatsächlichen Glyphenmetriken
dieselbe Grundlinie wie die Zahl.

Stand 3 besteht die neue Messbereichgruppe mit beiden Schriftquellen: 60
Datensätze mit echter nativer Bedienfolge und geprüften Bildpixeln. Die
[eingefrorenen Nachweise](measurements-2026-10-24/README.md) enthalten auch die
verworfenen vorherigen Stände. Weitere Leer-/Fehler-/Masken-/Langtextfälle,
die genaue restliche Kanalkopf-Anordnung und die gesamte Oberfläche bleiben
weiter im Abgleich; dies ist kein pauschaler Messlabor- oder Plattformabschluss.
Die Kurven-/Unterstrichfarben verwenden noch die native Kanalpalette. Der nächste
Abgleich muss die Akzent-/Vektorfarbrollen der gemeinsamen Referenz ausdrücklich
prüfen und den Kanalkopf mit links/rechts verteiltem Text weiter angleichen.

Diese beiden offenen Punkte werden im anschließenden
[Kanalkopf-/Kurvenabgleich](CURVES-NATIVE.md) bearbeitet. Dessen finaler Stand
belegt zusätzliche Randfälle und erhält Subpixel-Strichstärken; die oben
beschriebenen historischen Stände bleiben unverändert eingegrenzt.

Stand 4 besteht alle fünf übrigen Gruppen am unveränderten Stand-3-Programm:
normale und große externe Fenster, Anzeigeeinheiten, Messlabor und Szenenrahmen.
Zusammen mit der neuen Messbereichgruppe sind dies sechs aktuelle Gruppen.
Die große Messlaborprobe macht den Kurventrenner mit vier tatsächlichen
Mausradereignissen sichtbar und besteht danach weiterhin Drag, 10/40-Schritte,
Grenzen und die übrigen Zustandsbedingungen. Page Down bewegt den Messbereich
im beobachteten Fall von Offset 72 auf 138. Szene, Zeit und Kamera bleiben dabei
unverändert. Windows ist für diesen Stand erfolgreich gebaut; die aktuelle
Windows-Bedienfolge ist weiterhin offen.

## Speicherpflege

Aus den abgeschlossenen, durch den letzten Szenenstand ersetzten erfolgreichen
Läufen `run-hn9m655z` und `run-vjl13vbv` wurden ausschließlich 440 automatisch
erzeugte PNG-/BMP-Rohbilder entfernt: 601.808.746 Byte, rund 574 MiB. Ihre
eingefrorenen Quellstände, Logs, Ergebnisdateien und ausgewählten Belegbilder
bleiben im Szenenbericht erhalten. Quellen, Testprojekte, Messdateien, Belege
offener Fehler und sämtliche Referenzbilder wurden erhalten. Die neuen laufenden
Messbereichprüfungen liegen in einem anderen Verzeichnis.
