# Native Szenenrahmen-Belege

Zum [begrenzten Szenenrahmen-Abgleich](../SCENE-FRAME-NATIVE.md). Der Gesamtauftrag
bleibt offen; historische und neue Läufe werden nicht miteinander gleichgesetzt.

`scene-source0.json.gz` beschreibt den vorherigen Quellgraphen. Stand 1 scheitert
beim Kompilieren. `source1-public-delta.json.gz` ist aus dem eingefrorenen Stand 2
mit den beiden bekannten Korrekturen zurückgerechnet und für jede Datei gegen
die tatsächlich vor dem Build erfassten Stand-1-Hashes geprüft. Es ist kein
behaupteter erfolgreicher Lauf dieses Stands.

Die späteren `source*.json.gz`, öffentlichen Deltas, Prozessspuren, Bildmetriken
und Compilerlogs gehören jeweils zu ihrem angegebenen Stand. Zwölf fremde
Arbeitsdateien werden nicht übernommen. Drei davon sind im lokalen Linux-
Kompiliergraphen enthalten; die lokalen Apps beweisen deshalb keinen Clean-Checkout.

`before-no-scene-controls.png` ist eine unveränderte native Vorgängeraufnahme.
Sie belegt das Fehlen der neuen Elemente in deren lokalen Rechtecken. Unterschiedliche
pausierte Samples schließen einen pixelidentischen Vergleich des ganzen Fensters aus.

Der vorgezogene Systemschrift-Lauf von Stand 2 verwendet denselben Binärstand
auf einem eigenen X-Display. Seine native Bedienfolge besteht; die Python-
Auswertung scheitert am zusätzlichen Text des überlagernden Menüs im Rechteck
der unteren Aktion. Die vollständige Spur bleibt als `early2-*` erhalten.

Die praktische Screenreader-/Monitor-/Windows-/macOS-Abnahme ist damit nicht
abgeschlossen. Eine vorhandene Testdatei oder ein Build belegt keine Bedienaufgabe.

Stand 3 besteht die Szenenprobe (108 Datensätze mit beiden Schriftquellen).
Stand 4 hat denselben Quellgraphen und besteht elf Gruppen. Die zusätzlichen
`old4-*`-Aufnahmen zeigen native Fenster vor der Korrektur der Designvariante;
sie sind keine vollflächig identischen Vorher-/Nachher-Szenen.

Stand 5 besteht drei Gruppen: Szenenrahmen, normales Herausziehen/Rückkehr und
Herausziehen/Rückkehr bei großer Schrift. Die vier Fensterabläufe prüfen die
relativen Szenenmaße bei gleicher Inhaltsbreite ausdrücklich am jeweiligen
externen Fenster. Stand 6 verwendet den identischen Quellgraphen und die bereits
gebauten Programme. Der eingefrorene Helfer ruft den regulären getrackten Runner
mit unverändertem Katalog auf; Kompilieraufrufe sind gesperrt, alle Binärhashes
werden vorher/nachher verglichen. Die Ergebnisdateien weisen den Abschluss aus.

Abschluss: Stand 5 besteht 3/3 Gruppen (`run-j6td0ed_`), Stand 6 besteht 10/10
(`run-xi5w_ssb`), zusammen 13 unterschiedliche Gruppen. `validation.json` bindet
den Windows-Build an Stand 5 und weist seine fehlende aktuelle GUI-Abnahme aus.
`check_frozen_scene.py` prüft Hashes, identische Quellgraphen, lokale alte/neue
Fußzeilenpixel, zwölf Beschriftungsgegenbeispiele und vier Fensterabläufe erneut.
Aufruf vom Repository: `python docs/visuals/unified-workspace/scene-frame-2026-10-24/check_frozen_scene.py`.

Die ausgewählten Szenen-PNGs stammen aus Stand 5. Die externen und wieder
eingebetteten Panelaufnahmen sind RGB-identische BMP-zu-PNG-Konvertierungen;
`panel-captures.json` enthält beide Dateihashes und den Pixelhash. Native
OS-Rahmen werden vom App-Framebuffer nicht mit aufgenommen.
