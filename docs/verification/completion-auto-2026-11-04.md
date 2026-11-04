# Native automatische Vorschläge · Eingabegrundlage

Der [D-Auftrag](../visuals/unified-workspace/COMPLETION-D-NATIVE.md) bleibt im
vollen Umfang offen, soweit dort benannt. Dieser Schritt korrigiert das native
automatische Eingabeverhalten; er ist keine vollständige D-Layout-, Scope-,
Parameterhilfe-, Preferences- oder Plattformabnahme.

## Tatsächliches Verhalten

Der vorherige Editor öffnete vorhandene API-Vorschläge sofort nach zwei Zeichen
und wählte den ersten Treffer zur Enter-/Tab-Übernahme. Jetzt wartet die Anfrage
140 ms und prüft ihre exakten Quellbytes, Editor und Cursor. Automatische Listen
beginnen ohne Übernahmeauswahl. Enter/Tab behalten dann Zeilenumbruch/Einrückung.
Eine explizite Auswahl per Pfeilen, Strg+N/P oder Klick erlaubt die Übernahme.
Manuelles Strg+Leertaste behält seine bestehende Erstauswahl und Undo-Transaktion.
Eine automatische Auswahl gilt für ihren Präfix: weiteres Tippen verlangt eine
neue Auswahl. Vorgemerkte Übernahmen eines älteren Textstands werden bei neuer
Eingabe verworfen. Backspace unter die Zwei-Zeichen-Schwelle schließt die
automatische Liste; manuelles Öffnen bleibt auch mit leerem Präfix möglich.

Escape unterdrückt die aktuelle Bezeichneranzeige beim weiteren Tippen;
Strg+Leertaste öffnet ausdrücklich erneut. Vorgemerkte Anfragen werden bei
Cursor-/Quelltext-/Ziel-/Fokus-/Auswahlwechsel verworfen. SDL-IME-Komposition
pausiert automatische und manuelle Vorschläge; bestätigte Eingabe darf sie
erneut auslösen. Reine Leerlaufframes scannen den Quelltext nicht zusätzlich.

Die ergänzte Kontextprüfung behebt einen vorhandenen C-Lexerfehler:
Backslash-LF und Backslash-CRLF setzen `//`-Kommentare fort. Physim behält seine
abweichende Zeilenregel. Die Unitprüfung verlangt bei Ablehnung unveränderte
Ausgaben; keine vollständige Präprozessor- oder Typanalyse wird behauptet.

## Gegenproben

- **Unerwünschte Einfügung:** Nur in der isolierten Testkopie wird die Schutz-
  bedingung vor Enter/Tab ohne Auswahl deaktiviert. Die neue echte Editorprüfung
  scheitert unter Windows an Stufe 7: `ps_aabb` wurde eingefügt, erwartet war
  `ps` mit Zeilenumbruch. `run-u54u8brs`, 0/1. Ein C-App-Prozess; Physim wird nach
  dem Fehler nicht mehr gestartet. Kein Linux-Gegenlauf. Original/root bleibt
  unverändert; positive Herkunft und mutierte Eingaben sind getrennt archiviert.
- **Auswahl nach Präfixänderung:** Eine weitere echte native Gegenprobe wählt
  zunächst einen Treffer, tippt anschließend einen anderen Präfix und Enter im
  selben SDL-Ereignispaket. Der bisherige Refresh wählte still den neuen ersten
  Treffer: Windows Stufe 51 enthält `ps_v2` statt `ps_v` mit Zeilenumbruch,
  `run-cvghj8fc`, 0/1. Nach Trennung manueller Herkunft und automatischer Auswahl
  bleibt Enter eine neue Zeile. Dieser Gegenlauf startet nur eine C-App; Physim
  und Linux werden darin nicht ausgeführt. Der endgültige Positivlauf prüft beide.
- **Fortgesetzter C-Kommentar:** Die neue LF-/CRLF-Unitprüfung wird mit dem alten
  `app/completion.c` direkt unter WSL kompiliert. Sie scheitert mit Exit 1;
  beide tatsächlichen Quelldateien, Binärhash, Kommando und Log sind gesichert.
  Die korrigierte Zwischenfassung besteht direkt unter Linux. Abschließend
  bestehen beide vollständigen Plattform-Toolchain-Prüfungen einschließlich
  eines positiven Physim-Falls, der die C-Regel ausdrücklich nicht übernehmen darf.

## Ausgeführte Phasen

| Phase | Windows Release | Linux Debug ASan/UBSan |
| --- | --- | --- |
| `completion-auto-ready` | 2/2, `run-26i43itu` | 2/2, `run-imw0wa35` |
| `completion-auto-counter` | 0/1, `run-u54u8brs`, wirkliche falsche Einfügung | nicht ausgeführt |
| `completion-auto-final` – Idle-Pfad/Safe-Diagnose und positiver Code nach Gegenprobe | 2/2, `run-37jpcs6v` | 2/2, `run-dhvgitib` |
| `completion-auto-context` – einschließlich Lexer-Korrektur | **Unit 1/1 + GUI 2/2**, `run-ipjevj_z` / `run-gn8cn9qm` | **Unit 1/1 + GUI 2/2**, `run-3kbefkwr` / `run-54kpz_no` |
| `completion-auto-prefix-before` – echte veraltete Auswahl nach weiterer Eingabe | 0/1, `run-cvghj8fc` | nicht ausgeführt |
| **`completion-auto-refined` – endgültig mit Präfix-/Schwellenschutz** | **Unit 1/1 + GUI 2/2**, `run-8ge77tbp` / `run-w9x107g3` | **Unit 1/1 + GUI 2/2**, `run-jzzktbs2` / `run-1ae5dg9v` |

Der letzte Aufruf führt zuerst die nichtgrafische Katalog-/Lexerprüfung, danach
die beiden GUI-Gruppen aus. Das Programm ist zwischen diesen beiden Aufrufen
bytegleich. Pro Plattform vier GUI-App-Prozesse: manueller C-/Physim-Ablauf mit
je drei Editorzielen und automatischer C-/Physim-Ablauf. Der manuelle Ablauf
prüft weiterhin UTF-8-Positionen, Bezeichnerersetzung und Undo/Redo.

Die neue automatische Prüfung verwendet echte SDL-Text-/Tastenereignisse und
anfänglichen Pointerfokus. Sie verlangt dreizehn Abschlussmarker je Sprache:
Enter ohne Auswahl, Tab ohne Auswahl, Escape/Weitertippen/manuelles Öffnen/Undo,
Strg+N/P mit Undo, Präfixänderung nach Auswahl, Unterschreiten der Zwei-Zeichen-
Schwelle, Cursorwechsel, gleich lange externe Textänderung, Komposition/
Commit, Kommentare/Strings, Auswahl, Zielwechsel und markierter Fokusverlust.
Fixture-Rücksetzungen nutzen die vorhandene Editor-Fokusrestaurierung. Das
Kompositionsereignis und der Fokusverlust sind synthetisch; kein Nachweis einer
physischen IME, eines echten Betriebssystem-Fokuswechsels oder Screenreaders.

154 eigene Eingaben und 1.110 Compiler-/Testdateien erfasst, alle referenzierten
Archivbytes geprüft. Der letzte Stand `completion-auto-refined` ist eingefroren;
alle lokalen Build-/Prüf-/Freeze-Handles sind beendet. [Originalkommandos,
Quell-/Binär-/Ergebnishashes, Einzelfälle und Umfang](completion-auto-2026-11-04.json).

Ein Vorbereitungsschritt wurde vor dem Ende des vorigen Archivabschlusses
aufgerufen und scheiterte vor jeder Mutation am noch fehlenden Manifest.
Danach wurde derselbe Freeze-Handle bis zum Ende beobachtet; erst dann begann
die Vorbereitung. Das war ein Orchestrierungsfehler, kein App-Testfehler.

Der bestehende Python-Native-Runner-Prüfer meldet separat vollständigen Erfolg;
der Prozess ist beendet. Seine direkte äußere Exit-/Sessionantwort ging durch
eine anschließend fehlerhafte Formatierung leerer GitHub-Jobschritte verloren.
Dieser Metadatenverlust wird nicht als eigener App-Fehler oder zusätzlicher
Exitcode-Nachweis ausgegeben; die bestandene Prüfung wird dafür nicht wiederholt.

## Speicher und verbleibende Abnahme

Die automatische Prüfung erzeugt keine Rasterbilder. Nach den jeweiligen
Abschlüssen werden 24 ersetzte erfolgreiche manuelle BMP-Aufnahmen der Phasen
`ready`/`final` entfernt (76.726.512 Bytes) und nach der Präfixkorrektur weitere
zwölf der Phase `context` (38.363.256 Bytes): **36 Bilder / 115.089.768 Bytes**.
Vorher sämtliche absoluten D:-Pfade,
Besitzer, laufende Windows-/Linux-Nutzung, Reparse-Vorfahren, Länge und SHA-256
geprüft. Zwölf aktuelle manuelle Vergleichsbilder bleiben für die weitere
D-Umsetzung erhalten. Fehler, Originalergebnisse, Quell-/Runtimearchive,
Abhängigkeiten, Referenzbilder der Spezifikation und VM bleiben erhalten.
Plan und Abschluss sind im JSON-Bericht mit Hashen erfasst; nicht erneut löschen.

Ein Workflow-Nachlauf auf gehostetem macOS ist für den neuen Quellstand noch
erforderlich. [Separater Apple-Silicon-Nachweis](settings-keyboard-arm-2026-11-04.json)
bestätigt **16/16** für den früheren Einstellungen-Fix `4adf2699`; sein Intel-Job
wartete zuletzt noch. Das ist keine Mac-Abnahme dieser Completion-Änderung.
Der Nutzer-Mac bleibt ausgeschaltet und unkontaktiert; kein Zugriff auf F:.
