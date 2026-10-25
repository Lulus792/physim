# Belege zum nativen Werkbankkopf

Zum [Umsetzungsbericht](../WORKBENCH-HEADER-NATIVE.md). Die 61 in
`manifest.json` erfassten Binärdateien sind unverändert eingefroren. JSON liegt
platzsparend als UTF-8 mit Gzip vor; PNGs sind vollständige native Aufnahmen.

- `source1` bis `source5`: normalisierte Quellhashes, Basisrevision, rohe
  Font-/Fremddateihashes. `delta1` bis `delta5` enthalten die zugehörigen eigenen
  Quelltexte gegenüber `e7e9022`. Die erfassten fremden Eingaben bleiben lokale
  Voraussetzungen; die Karten behaupten keinen sauberen reproduzierten Checkout.
- `failed1-*`: native Zeitüberschreitung nach Öffnen der Suche. Der Öffnungsklick
  verliert die logische Tastaturzuordnung; keine bestandenen Ergebnisse.
- `failed2-*`: die logische Zuordnung bleibt, aber die echte Unicode-Eingabe
  scheitert am deaktivierten Feld. Die separate Debuggerprobe bestätigt
  `keyboard=1`, `edit_active=0` und leeren Suchtext.
- `failed3-*`: die native Bedienfolge besteht. Der nachgeschaltete Bildprüfer
  verwirft die zwischen zwei Pixelspalten liegende Linie bei großer Schrift.
  `counterexample-fractional-rule.png` ist das unveränderte Gegenbild.
- `failed4-*`: die korrigierte Linie und native Bedienfolge bestehen. Der
  Bildprüfer findet einen Aufnahmefehler der Fixture: Das als „vorher“ bezeichnete
  Bild wurde erst nach der Sichtbarkeitsänderung durch Herauslösen gezeichnet.
  Quelle 5 ändert ausschließlich diesen Aufnahmezeitpunkt; Produktcode bleibt
  gegenüber Quelle 4 unverändert.
- `diagnostic-*`: GDB-Dateien und ursprüngliche Diagnoseausgaben. Der zweite
  Eingabe-Diagnoseaufruf endet nach der absichtlichen GDB-Beendigung zusätzlich
  mit Shell-Exit 127 wegen einer CR-Zeile. Er ist eine Diagnose, kein grüner Test.
- `windows-native` / `linux-native` und `*-validation`: aktuelle vollständige
  Dateitab-/Kopfgruppe einschließlich beider Schriftquellen. Die vier weiteren
  `*-workflow`-/`*-analysis_editor`-Dateien je Plattform enthalten die aktuellen
  Regressionsgruppen mit Befehlen und Programmbindung.
- `capture-hashes`: 188 aktuelle Kopf-/Tabbilder; `regression-image-hashes`:
  weitere 204 aktuelle Bilder. Die ursprünglichen Rohaufnahmen bleiben unter
  den in den Ergebnissen genannten ignorierten Testpfaden erhalten.
- `native-header-parity`: vier strenge RGB-Vergleiche gleich großer nativer
  Kopfrechtecke. Alle vier melden ausdrücklich `exact_rgb_equal=false`, mit
  höchstens einer RGB-Stufe Unterschied. Das Ergebnis wird nicht durch eine
  neue Passtoleranz in exakte Gleichheit umgedeutet.
- `counterexample`: die unveränderte Originalreferenz besitzt 36 von 36
  erwarteten Trennlinienpixeln bei x=1113, y=120–155. Die frühere native Aufnahme
  aus dem vorangegangenen Analyseeditor-Abgleich besitzt dort null. Beide
  Originalbilder sind bereits versioniert; Pfade und Hashes sind angegeben.
- `references`: Basis- und aktuell beobachtete Referenzhashes. Die während der
  Arbeit parallel ergänzte Designauswahl in `NEXT-CHAT.md` gehört nicht zu diesem
  Patch und wird nicht als Freigabe neuer Varianten behandelt.

Ausführung: Windows/MSVC Release `run-a__niv7e`, Linux/GCC mit ASan/UBSan
`run-is4cqw8n`, jeweils 5/5. Die Linux-Hilfe startet Xvfb/Openbox und setzt
`ASAN_OPTIONS=detect_leaks=0`, `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`
sowie `LIBGL_ALWAYS_SOFTWARE=1`.

Die native Testgruppe verwendet interne SDL-Eingaben und semantische
Fokusanforderungen. Vollständige Browser-/Design-/macOS-/physische Eingabe-/DPI-
und externe Assistive-Nachweise bleiben offen. Diese Belege sind kein voller
Punktescore der Oberfläche.
