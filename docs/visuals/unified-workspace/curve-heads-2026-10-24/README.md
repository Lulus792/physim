# Native Kanalüberschriften und Kurven – 24. Oktober 2026

Der [Umsetzungsbericht](../CURVES-NATIVE.md) beschreibt Änderungen, Referenzstellen und verbleibende Abweichungen. Diese Nachweise betreffen den Messbereich in Simulieren. Sie sind keine Abnahme der gesamten Oberfläche.

## Durchläufe

| Quellenstand | Lauf | Ergebnis und Aussage |
|---|---|---|
| 1 | run-p0zkj_7q | 0/2 angenommen: native Systemfont-Sequenzen liefen durch; Python beanstandete eine teilweise weggeblätterte Überschrift und das verdeckte kontrollierte Prüffenster. |
| 2 | run-o17yb7jy | 1/2: Messbereich mit 60 Bildern bestanden; die kontrollierte Systemfont-Sequenz mit 42 Bildern verwendete fälschlich die allgemeine App-Palette und wurde vom Pixelvergleich abgelehnt. |
| 3 | run-rmkzsc1f | Kontrollierte Kurvenprüfung mit 84 Bildern bestanden. |
| 4 | run-k_e8ymox | Sieben Gruppen bestanden; vor Entfernen unnötiger Ganzzahlrundung. |
| 5 | run-crpxs20y | Finale Messbereichs- und Kurvenprüfungen bestanden, 60 + 84 native Bilder. Tatsächliche NK-Kommandos belegen 2 / 2,25 / 2,5 / 2,75 Pixel Linienbreite. |
| 6 | run-nckh7met | Docking, Inspektor und Labor bestanden. |
| 7 | run-7n6swyhd | Anzeigeeinheiten, Datenmasken und Szenenrahmen bestanden. |

Die finalen Quellen- und Fontkarten 5/6/7 sind identisch. Alle acht finalen Gruppen verwendeten dieselbe nach SHA-256 geprüfte Linux-Anwendung. Regressionen 6/7 nutzten bereits gebaute Bibliotheken; die Binärkarten vor und nach Ausführung sind identisch. Windows wurde erfolgreich gebaut; ein aktueller Windows-Bediennachweis liegt für diese Änderungen nicht vor.

## Umfang der Belege

Die Messbereichsprüfung verwendet das echte C-Pendel und prüft native Tastatureingaben, Kanalwechsel und Einheitenwechsel. Quelltext, Messdaten, Szene, Zeit und psrun-Prüfsumme bleiben erhalten. Die zusätzliche Kurvenprüfung zeichnet ausdrücklich kontrollierte Werte oberhalb desselben unveränderten pausierten Projekts: leer, Einzelwert, fehlende Werte, Lücken, große Offsets, lange Metadaten und nicht darstellbare Einheiten. Sie belegt die Darstellung dieser Werte, keine im Runner erzeugten Ausfälle und keine Langzeit-Dezimierung.

`all-frame-hashes.json` enthält die Hashes aller 144 finalen Bilder; 20 ausgewählte native PNGs liegen hier. Zwölf weitere PNGs aus Docking-/Inspektorprüfungen bewahren die RGB-Pixel der ursprünglichen BMP-Dateien unverändert. Der OS-Fensterrahmen gehört nicht zum SDL-Framebuffer. Die Quelldaten und Laufprotokolle erlauben die Zuordnung zu den jeweiligen Prüfungen.

`before-color.png`, `source1-hidden-probe.png` und `source2-wrong-palette.png` sind tatsächliche ältere native Aufnahmen, die drei lokale Pixelbedingungen verletzen. Der eingefrorene Prüfer akzeptiert die entsprechenden finalen Pixel. Dies ist kein identischer Vollbildvergleich: Pendelphase und Messzeit können zwischen Läufen abweichen.

Drei fremde AT-SPI-Dateien gehören zum lokalen Compile-Graphen. Die zwölf fremden Änderungen blieben unverändert und werden nicht mit diesem Arbeitsstand eingecheckt. Deshalb ist dies kein Nachweis eines sauberen Checkouts.

## Verbleibende Abnahme

Der interaktive Kanalpfeil unterscheidet sich von den statischen Referenzbeschriftungen; seine Gestaltung kann weiter angenähert werden. Lange tatsächliche Metadaten werden lesbar umgebrochen. Windows-Bedienung, macOS-Ausführung, praktische Screenreader-Aufgaben, physische DPI- und Monitorwechsel sowie weitere Oberflächenbereiche bleiben offen. Die bereits aktivierte Float-Erweiterung des nativen Renderers erlaubt präzise Linien; hier besteht keine technische Ganzzahlbeschränkung.

246 ersetzte Rohbilder aus dem älteren erfolgreichen Lauf run-egt4z71n wurden nach Sicherung der benötigten Nachweise entfernt: 561.180.705 Bytes. Referenzen, offene Fehlerbelege, Quelltexte, Projekte, Messdateien und Protokolle blieben erhalten. Der ältere eingefrorene Messbereichsnachweis wurde danach erneut geprüft.

## Gesicherte Prüfung wiederholen

Aus dem Repository mit Python 3:

```text
python docs/visuals/unified-workspace/curve-heads-2026-10-24/check_frozen_curves.py
```

Der Prüfer nutzt nur die Standardbibliothek und vorhandene Prüfmodule; er startet weder Anwendung noch Browser. Er prüft Hashes, Gruppen, identische Quellenkarten, tatsächliche Linienbreiten und die lokalen positiven/negativen Pixelbelege. `artifacts.json` umfasst alle Dateien dieses Ordners außer sich selbst.
