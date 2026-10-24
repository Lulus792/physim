# Native Rundungen und Inspektorüberschriften – 24. Oktober 2026

Der [Umsetzungsbericht](../ROUNDED-SECTIONS-NATIVE.md) beschreibt Ursache,
Referenzen, Änderungen und verbleibende Abnahme. Diese Nachweise belegen keine
vollständige Gleichheit der gesamten Oberfläche mit allen Referenzen.

## Originale Gegenfälle und Reparatur

- `negative-path-results.json.gz`: Der alte Renderer besteht 5/30 Fälle;
  konvexe Kapseln, enge Kanten und eine unabhängige Kreisdistanz werden verletzt.
- `negative-rounded-gpu.png`: unveränderte tatsächliche GL-Aufnahme der alten
  Zacken. Der damalige Prüfer beendet sich am ersten falschen Außenpixel; seine
  anschließenden LeakSanitizer-Meldungen stammen aus dem frühen Testabbruch.
  Der finale Prüfer sammelt Fehler und gibt seine Ressourcen vollständig frei.
- `negative-large-titles.png` und `negative-titles-results.json.gz`: native
  alte Überschriften. Die neue Wortprüfung scheitert bereits bei UI 18 an
  „Modellwer“ / „te“. Der native Bedienablauf lief bis zum Ende durch.
- `partial-linux3` / `partial-windows3`: erster Reparaturstand mit 20/30
  Rundungsfällen und 2/3 C-Gruppen. Annäherungen der Bogenenden lassen noch kleine
  rückwärts laufende Kanten stehen. Diese Ergebnisse sind keine finalen Erfolge.

Der finale Stand 4 hält die Kommandokoordinaten gebrochen, begrenzt Radien auf
halbe Kanten, verfeinert Viertelkreise und verwendet exakte Übergangspunkte.
Die geometrische Gegenprobe läuft unverändert gegen den alten und den finalen
Renderer. Innen-/Außenpixel werden analytisch geprüft; Antialiasing unmittelbar
an der Kante ist damit nicht vollständig mit einem Browser gleichgesetzt.

## Finale Ergebnisse

| Umgebung | Bestandene unterschiedliche Gruppen |
|---|---|
| Alpine-Dev, GCC Debug, ASan/UBSan | ui_rounded, ui_geometry, ui_subpixel, ui_subpixel_gpu, timeline_design, inspector_design, camera_design, measurement_design, workbench_workflow, graphic_dialog, documentation_h, docking_workflow, inspector_workflow |
| Windows, MSVC Release | ui_rounded, ui_geometry, ui_subpixel, ui_subpixel_gpu |

Die vollständigen Läufe stehen in `validation.json`, Einzelbefunde und originale
Ausgaben in den komprimierten JSON-/Logdateien. Windows-GPU arbeitet mit einem
versteckten nativen GL-Fenster und tatsächlichem Readback; die C- und GPU-Prüfungen
belegen keine aktuelle praktische Windows-Fensterbedienung. 1×/2× sind explizite
Offscreen-Zielgrößen, keine physischen Monitorwechsel.

Die Inspektorprüfung erreicht durch native Scrollereignisse alle neun Titel
bei sechs Schrift-/Themevarianten in beiden Fontmodi. Sichtbare vollständige
Zeichenkommandos werden auf Wortgrenzen geprüft. Der Wunsch 208 px bleibt
unverändert; UI 22 benötigt tatsächlich mindestens 245 px mit Systemschrift
bzw. 226 px mit Noto. Die unverkleinerte Schrift bleibt erhalten. Die anderen
Funktionsgruppen prüfen weiterhin Quelle, Daten, Kamera und Laufzustand sowie
Maus-/Tastaturwege und externe Fenster in ihrem jeweiligen bisherigen Umfang.

## Breite Nachläufe und F1

`broad4` besteht Werkbank und Grafikdialog, scheitert aber bei F1 nach den
56 Layoutzuständen an der 60-s-Appgrenze. Es ist weiterhin ein 2/3-Lauf.
`h-ext4` besteht dieselbe ursprüngliche F1-Gruppe vollständig mit ext4-Arbeitsdaten;
SDK, Erwartungen und Zeitgrenzen sind unverändert. Diese Wiederholung wird
getrennt zugeordnet. Die Vorher-/Nachher-Binärkarten der schreibgeschützten
Nachläufe sind identisch. PNG-Konvertierungen aus H-BMPs erhalten die RGB-Pixel
verlustfrei; `rgb-conversions.json` nennt Original- und RGB-Prüfsummen.

Die Katalogprüfung fand außerdem eine alte unvollständige Zuordnung der GPU-
und spezialisierten Designprüfer. Ihre explizite Liste ist korrigiert; unbekannte
Integrationen werden weiter verworfen. `catalog-windows.log.gz` und
`catalog-linux.log.gz` belegen die gesamte Runnerprüfung. Darin enthaltene
absichtliche Fehler-/Timeout-Fixtures sind Teil der erfolgreichen Runnerabnahme.
`catalog-before.log.gz` erhält den tatsächlichen vorherigen Zuordnungsfehler.

## Quellen, Umfang und Wiederholung

`source4.json.gz`, `public-source.json` und `public-overrides4.json.gz` binden
den aktuellen SDK-Quellstand an Basis `a46f301fe8b45a06b427741fda02870c000457dc`.
Die öffentliche Karte enthält 263 Quellen, die Fontkarte 18 Dateien. Drei fremde
AT-SPI-Dateien gehören weiter zum lokalen Kompiliergraphen. Alle zwölf fremden
Änderungen sind unverändert erhalten und vom Commit ausgeschlossen. Ein sauberer
Checkout wird dadurch nicht belegt. Die separat geänderte Runner-Testdatei
ist zusätzlich über `references.json` erfasst und gehört nicht zum SDK-Graphen.

`all-frame-hashes.json` erfasst die nativen PNG-/BMP-Ausgaben; ausgewählte
unveränderte native PNGs und verlustfreie BMP-Konvertierungen liegen direkt hier.
Die Hashkarte ist kein Vollbildvergleich verschiedener Simulationsphasen.
Weitere Schrift-/Rasterunterschiede, macOS, praktische Screenreader-Aufgaben,
physische DPI-/Monitorwechsel und der gesamte Gestaltungsauftrag bleiben offen.

Mit Python 3 aus dem Repository, ohne Start einer Anwendung:

```text
python docs/visuals/unified-workspace/rounding-headings-2026-10-24/check_frozen_rounding.py
```

Der lokale Prüfer nutzt die Standardbibliothek und vorhandene Prüfmodule. Er
prüft Artefakte, Ergebnisse, tatsächliche Pfadfehler, Wortgrenzen und die analytische
Innen-/Außenbedingung an originalen negativen und positiven GPU-Pixeln.

## Speicherpflege

Nach vollständigem Byte-/Hashvergleich wurden 72 Dateien der doppelten privaten
ext4-Arbeitskopie entfernt: 176.448.473 Byte (rund 168 MiB). Das vollständige
identische Archiv bleibt unter `build/native/rounded-sections-h-ext4` erhalten,
ebenso alle ausgewählten und eingefrorenen Nachweise. Der WSL-Datenträger bleibt
erhalten; er wurde nicht kompaktiert. Die Zahl beschreibt entfernte
Dateibytes im Linux-Dateisystem, keine behauptete Verkleinerung der VHDX unter Windows.
