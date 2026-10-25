# Native Belege: Auswertungsseiten, 25. Oktober 2026

Umfang und Grenzen stehen in [ANALYSIS-SIDES-NATIVE.md](../ANALYSIS-SIDES-NATIVE.md).
Die beiden Manifeste verknüpfen unveränderte Build-/Prüfausgaben, Programmhashes,
30 echte native PNGs und deren dekodierte RGB-Hashes:

- [Windows](windows-manifest.json), [9 Gruppen](windows-results.json), [2 Offline-Proben](windows-bundled-results.json).
- [Linux](linux-manifest.json), [9 Gruppen](linux-results.json), [2 Offline-Proben](linux-bundled-results.json).
- [311 aktuelle Eingabehashes](source10-inputs.json); die übrigen `sourceN`-Dateien
  benennen frühere Zwischenstände, keine ebenfalls bestandenen Endstände.
- `source-app-*.gz` enthalten die acht zugehörigen aktuellen Quelldateien.
  `harness-*.gz` erhalten die tatsächlich verwendeten lokalen Hilfsskripte.
  Die normalen Regressionen bleiben im Projekt unter `app/` und `tests/`.
- `*-steps.json.gz` sind originale App-Abläufe, `*-checker.log.gz` originale
  Prüfausgaben. `counterexample-*` sind ausdrücklich keine zusätzlichen Pässe.
- [Bereinigung alter erfolgreicher Rohbilder](cleanup.json).
- [Abschlussaudit](audit.json) mit unabhängiger RGB-Prüfung aller 30 Originalbilder;
  das zugehörige [Prüfskript](audit.py.gz) bleibt ebenfalls erhalten.

Die Aufrufe verwenden `tools/build.py` für den bestehenden Windows-Release-SDK
beziehungsweise Linux-Debug-SDK mit ASan/UBSan. Anschließend läuft
`tests/check_display_workflow.py <Fall> --app <Programm> --work <frischer Ordner>
--root <Projekt>`. Die genauen Fallnamen, Pfade, Laufzeiten und Exitcodes stehen
in den Ergebnisdateien. Linux verwendet `xvfb-run`, Openbox und ext4-Arbeitsdaten;
`ASAN_OPTIONS=detect_leaks=0`, `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`
und `LIBGL_ALWAYS_SOFTWARE=1` sind in den erhaltenen Skripten dokumentiert.

Die negativen Debuggerläufe stoppen absichtlich am beobachteten Zustandswechsel
beziehungsweise vor der Testauswertung. Ihr Debugger-Exitcode ist kein App-Pass.
Der ursprüngliche 208-px-Gegenlauf besitzt keine separat gesicherte Programmdatei;
seine Originalausgabe und Bilder sind erhalten. Für die finalen Läufe sind die
Programm- und Eingabehashes dagegen vollständig festgehalten.

Vorlagen wurden nicht angepasst. PNGs sind RGB-Konvertierungen der angegebenen
nativen BMPs ohne Retusche, Skalierung oder Farbänderung. Der strenge Vergleich
der normalen Linux-Dunkelansicht liefert null abweichende Pixel; auch das ist
eine begrenzte native Erhaltungsprüfung und keine vollständige Designabnahme.
