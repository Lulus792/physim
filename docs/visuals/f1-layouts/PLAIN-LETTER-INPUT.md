# Normale Buchstaben und echte Tastenkürzel

Stand: 17. Oktober 2026. Ergänzt den Detailabgleich in
[H-FIDELITY.md](H-FIDELITY.md) und die [Bereichsnavigation](PANE-NAVIGATION.md).

## Reproduktion und Ursache

Die normale Linux-App verlor bei der lokalen F1-Suche den bisherigen Text:
Nach `theta` blieb `a` stehen. Beide unabhängigen X11-/AT-SPI-Läufe
`build/native/h-detail-normal-pointer` und `h-detail-normal-pointer-paced`
bestanden bereits die Mausaufgaben für Sprache, Kontextreiter und Seitenflächen,
scheiterten aber an der anschließenden Texteingabe.

Die Ereignisspur in `build/native/f1-input-route-baseline/physim.stderr` zeigt
alle erwarteten SDL-Zeichen, den richtigen nativen Feldfokus und aktive
SDL-Texteingabe. Der gespeicherte Tooltip war hier nicht die Ursache.
Ein stärker instrumentierter Lauf bestand zufällig; zusätzliche Logausgaben
veränderten die Ereignisverteilung über Frames. Das war keine Korrektur.

Der Adapter bildete bisher jedes Drücken und Loslassen von A/C/V/X/B/E auf
einen Nuklear-Befehl ab, auch ohne Befehlsmodifikator. Er übergab dann zweimal
`false`. Der verwendete Nuklear-Snapshot zählt auch redundante Freigaben als
Klicks. Zwei solche Meldungen im selben Frame gelten als Tastendruck.
Deshalb konnte ein normales `a` den Text auswählen und anschließend ersetzen,
`c` kopieren, `v` einfügen, `x` ausschneiden sowie `b`/`e` den Cursor versetzen.
Die veränderte Frameverteilung erklärt das sporadische Verhalten.

## Korrektur und gezielte Gegenprobe

Die sechs Kürzel nutzen jetzt denselben geschützten Übersetzungsweg wie die
vorhandenen Wort-/Zeilen- und Undo-/Redo-Befehle in `app/ui_sdl.c`.
Ein Befehl wird beim tatsächlichen Drücken ausgelöst. Eine Freigabe wird nur
gemeldet, wenn dieser Befehl zuvor gehalten war. Wiederholte echte Drücke und
das Loslassen des Modifikators vor dem Buchstaben bleiben zulässig.

Der bestehende Renderer-Eingabetest enthält sechs normale Buchstabenfälle mit
festen erwarteten Texten sowie sechs echte Kürzel. Er sendet jeweils echte
SDL-Ereignisstrukturen durch den vollständigen SDL-Adapter und wertet danach
den tatsächlich bearbeiteten Nuklear-Text aus. Kopieren und Einfügen verwenden
eine private Testzwischenablage mit Aufrufzählern; der Test verändert keine
Nutzerzwischenablage. Die Kürzel müssen genau einmal wirken und im folgenden
Frame inaktiv bleiben. Unter macOS wird dabei Cmd, unter Windows/Linux Strg
verwendet. Die vorhandenen PC-/Mac-Navigationstests bleiben zusätzlich erhalten.

Die isolierte Gegenprobe verbindet den ursprünglichen Adapter aus `3d89897`
mit exakt diesen aktuellen Tests. Sie verwirft alle sechs normalen Buchstaben,
während die sechs echten Kürzel bestehen:
`build/native/f1-plain-letter-counterexample-native-final/report.json`,
`precise_rejection=true`, Programm-SHA256
`c3a33dc6ee0789095dc679d211a6739463249a5def073d3acb9af36b2805325a`.
Der vorherige Gegenprüfer hatte die verschobene private Hilfsfunktion direkt
angesprochen. Er zählt nicht als gezielte Gegenprobe; die abschließende Variante
benutzt für normale Zeichen und echte Kürzel denselben vollständigen SDL-Weg.

## Abnahme und Grenzen

Der erste korrigierte normale Linux-Lauf `build/native/f1-input-normal-fixed`
besteht alle vier tatsächlichen Maus-/Tastaturaufgaben, einschließlich lokaler
Suche, F6/Shift+F6, globaler Suche und Überspringen leerer Suchergebnisse.
Er verwendet den ursprünglichen 40-ms-Burst ohne zusätzliche Wartezeiten,
ohne `--acknowledge-text` und ohne Diagnoselogging. Der dortige Binary-Hash ist
`8741bb5a271c67579e2cbd364986c2e11f925de2e91523508fd7b2ae38da2bcc`.
Danach wurde der Kürzelteil des nativen Tests auf den vollständigen SDL-Weg
umgestellt; der produktive Fix blieb unverändert.

Der abschließende Linux-Lauf `run-6ue5q3oy` besteht 4/4: Renderer einschließlich
aller zwölf neuen Ereignisfälle, F1-Tastatur, Inline-Formeln und die 24
Bereichszustände. Umgebung: Alpine 3.24.2, GCC 15.2, ASan/UBSan,
SDL 3.2.30, X11/Xvfb/Openbox und llvmpipe.

Windows bestätigt denselben finalen Quellstand mit MSVC/ASan 5/5
(`run-9oqk0tfx`): Renderer mit den zwölf neuen Ereignisfällen, F1-Tastatur,
unabhängige externe UIA-Suchfeldprüfung, Inline-Formeln und Bereichsnavigation.
Windows-Binary-SHA256:
`9de92e39840a449a492d605031ab8889919ea7014fae9eff779f1cdc9ea46f94`.
Linux-Binary-SHA256 des abschließenden Quellstands:
`c30e62e47caf637ecef742923f347720c62e31229d038fe162306c59dd366e87`.
Auch der abschließende normale Linux-Lauf mit genau diesem Binary besteht alle
vier Aufgaben (`build/native/f1-input-normal-final/results.json`), unverändert
ohne Diagnosemodus und bei 40 ms Eingabeverzögerung.
Das geprüfte Windows-EXE-/PDB-Paar liegt zusätzlich am gewohnten Startpfad
`build/native/Debug-sanitized/bin`; SDL und beide ASan-DLLs stimmen per Hash
überein. Das vorherige Paar liegt unter `build/native/physim-before-plain-letter-input`.

Die Anwendung enthält kein dauerhaftes neues Texteingabelogging. Die normalen
und gezielt getakteten Prüfungen bleiben getrennt; ein erfolgreicher getakteter
Lauf ersetzt keinen erfolgreichen ursprünglichen Ablauf.

Dieser Befund ist eine konkrete Eingabekorrektur, keine vollständige
Screenreader- oder Gerätefreigabe. Der Inhaltsabgleich aller 185 Seiten,
vollständige Assistive-Aufgaben, physische DPI-/Monitorwechsel und die übrige
Projektumsetzung bleiben eigene Abnahmepunkte.

Die eingegrenzte Ursache samt eingefrorenen Gegen-/Positivbelegen ist im lokalen
Skill `ui-implementation` unter `knowledge/nuklear-plain-letter-shortcuts.md`
fachlich geprüft aufgenommen; eine erneute zentrale Suche findet die Notiz.
Der Quellreview verfolgt Befehlsmodifikatoren und die unveränderte Weitergabe
wiederholter echter Drücke. Die neuen ausgeführten Tests belegen Freigabe nach
Modifier-Up, normalen Text und Clipboard-Nebenwirkungen.

Nach der Abnahme wurden ausschließlich 85 ersetzte BMPs aus dem älteren
erfolgreichen Windows-Lauf `run-52linhe9` entfernt: 189.659.730 Byte.
Die aktuelle Eingabe-/Bereichsabnahme, der spätere echte Captionnachweis,
versionierte PNGs und sämtliche Ergebnis-/Logdateien bleiben erhalten.
Einzelpfade und Begründung stehen in `build/native/f1-input-cleanup.json`.
