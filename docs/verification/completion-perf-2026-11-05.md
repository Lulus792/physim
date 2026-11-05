# C-Vervollständigung: Latenz und Speicherbedarf

Stand: 5. November 2026. **Messung abgeschlossen, Performance-Ziel für große
Dateien noch nicht erfüllt.** Der Anwendungscode bleibt unverändert auf
`694bb586`; die acht beteiligten C-Dateien wurden gegen dessen Git-Blobs gelesen
(ausschließlich CRLF/LF normalisiert). Kein Optimierungs-/Beschleunigungsnachweis
und keine neue GUI-, Linux-/macOS-Performance- oder physische Abnahme.
Der private Nutzer-Mac bleibt aus; keine Zugriffe oder Erreichbarkeitsproben.

[Kompakter Nachweis mit Quell-/Programm-/Originalberichtshashes](completion-perf-2026-11-05.json).
Originale, Messquellen und vollständige Stichproben bleiben unter
`build/validation/completion-perf-2026-11-05/` auf D:. Keine neuen Bilder.

## Befund

Windows 11, MSVC Release `/O2`, acht logische CPUs. Die vor der Baseline
festgelegte technische Prüfmarke beträgt p95 ≤ 1 ms für einen identischen
Snapshot und ≤ 16,7 ms für eine neue Auswertung/Cursoränderung. Das ist eine
Backend-Prüfmarke, kein Vertrag über die Bildrate der Anwendung.

Der abschließende vollständige Werkzeuglauf misst jeweils 21 Aufrufe mit dem
normalen Allocator; Zeiten in Millisekunden, p95 nach nächstem Rang:

| Quelle | Bytes | Treffer | Neues Modell p95 | Cursorwechsel p95 | Cache p95 |
| --- | ---: | ---: | ---: | ---: | ---: |
| Echtes Pendelbeispiel | 13.767 | 18 | 5,96 | 6,11 | 0,0017 |
| 64 lokale Variablen | 1.692 | 65 | 1,21 | 0,87 | 0,0003 |
| 512 lokale Variablen | 13.752 | 513 | 7,68 | 8,08 | 0,0031 |
| 2.048 lokale Variablen | 56.272 | 2.049 | 45,36 | 33,41 | 0,0118 |
| 4.096 lokale Variablen | 113.616 | 4.097 | 47,07 | 95,69 | 0,0252 |
| 6.144 lokale Variablen | 170.960 | 6.145 | 75,97 | 120,84 | 0,0208 |

Alle drei vollständigen Phasen verwenden dieselben sechs Quelldateibytes.
Die erste instrumentierte Baseline lag im größten Fall bei etwa 70/71 ms p95
(frisch/Cursor); separate spätere Prozesse zeigen merkliche Streuung. Daraus
keinen Vorher-/Nachhergewinn oder genauen Allocator-Aufschlag berechnen.
Die Verletzung der Frisch-/Cursor-Prüfmarke bleibt in allen Phasen bestehen.

Eine **temporär instrumentierte Kopie** der Syntaxfunktion trennt innerhalb
derselben Aufrufe Aufbau, Parsen, Knotenübertragung und Freigabe. Beim größten
Fall ergeben sich Mediane von **0,011 / 37,67 / 12,50 / 5,06 ms**. Damit ist die
vollständige Syntaxauswertung als nächster Optimierungsbereich belegt; die
unabhängigen Medianwerte sind keine exakte Zerlegung eines GUI-Frames.
Die Produktion enthält diese zusätzlichen Uhren/Ausgaben nicht.

Mit explizitem Mess-Allocator liegen im größten Fall maximal **22.330.270 Bytes**
gleichzeitig angefordert vor (etwa 21,30 MiB, einschließlich altem Snapshot beim
atomaren Wechsel). Dauerhaft bleiben 1.801.397 Bytes. Ein frischer Durchlauf hat
61.513 Allokationsversuche; ein identischer Cacheaufruf null. Kein Versuch wurde
vom 64-MiB-Messlimit abgelehnt. Dies zählt angeforderte Größen einschließlich
der internen Parserverwaltung, ohne die äußeren Messheader; es ist **kein RSS**.
Normale Allocator-Zeiten und instrumentierte Speicherwerte werden getrennt geführt.

## Messvertrag und Grenzen

- `syntax`: echte C-Grammatik einschließlich flacher Knotenliste;
  `bindings`: zusätzlich Scope-/Aufrufsammlung; deren Ausgabepuffer werden vor
  dem Timer angelegt, bleiben aber in den instrumentierten Speicherwerten enthalten.
- `cold`: jedes Mal neues Vorschlagsmodell, vorheriges Modell vor dem Timer
  zerstört. Prozess/Dateicache bleiben warm; kein Rechnerkaltstart.
- `cursor`: gleiche Quelle, abwechselnde benachbarte UTF-8-Grenzen, zuvor am
  anderen Cursor vorbereitet. Alle 21 Stichproben parsen neu. Nur die ursprüngliche
  Baseline enthält noch einen ersten Cachetreffer; sie wurde nicht nachträglich verändert.
- `cached`: identischer Besitzer, Quelle und Cursor, zuvor vorbereitet.
  Messauflösung und Timerkosten begrenzen die Aussage über sehr kleine Werte.
- Das echte Pendelbeispiel wird unverändert einschließlich Makros/Includes
  übergeben. Der Grammatikstatus ist dort `PREFIX`, nicht Compilerakzeptanz.
  Die fünf künstlichen vollständigen Quellen haben Grammatikstatus `OK`.
  Kein Präprozessor, Include-Auflösen oder Ausführen der Quellen.
- Noch kein Tippen mit geänderten Quelldateibytes und keine rasche Revisionsfolge
  gemessen. Vor einer inkrementellen Lösung passende Editierfolgen und einen
  unabhängigen Vergleich sämtlicher Vorschläge/Scopes/Fehlerpfade ergänzen.
  Keine Trefferbegrenzung, Scope-Verkürzung, abgeschwächten Ressourcenverträge
  oder neuen Provider-Worker als vermeintliche Performancekorrektur einführen.

## Wiederholen bei begründetem Anlass

Das neue Werkzeug baut nur seine neun C-Übersetzungseinheiten mit dem vorhandenen
Builder, ohne SDL, App-Fenster oder fremdes AX-/Orca-WIP. Neues Ausgabeverzeichnis
verwenden; vorhandene Ergebnisse werden nicht überschrieben:

```powershell
python -B tools/with-artifacts.py -- python -B tools/profile_completion.py --work build/validation/completion-perf-neu
```

Für eine eigene Quelle zusätzlich `--input pfad/zur/datei.c --cursor BYTEOFFSET`.
`--iterations` ist standardmäßig 21. Ein schneller Werkzeugcheck darf z. B. drei
Aufrufe verwenden, begründet aber keinen stabilen p95. Compiler-/Programmaufrufe,
Quellhashes, Rohwerte und Erwartungsstatus stehen im Arbeitsverzeichnis; ein
abschließendes `report.json` entsteht nur nach dem vollständigen Lauf.
`commands.jsonl` und `measurements.jsonl` erhalten abgeschlossene Teilaufrufe
auch bei einem späteren Fehler. `Debug --sanitizers` ist eine andere Messkonfiguration.

Das Werkzeug selbst wurde mit **47 Fällen je Windows/Linux** geprüft: fünf Modi,
beide Allocatoren, Unicode-Grenzen, EOF, leere Eingaben und erwartete Fehlercodes
für falsche Zahlen/Modi, NUL und Übergröße. Linux tatsächlich mit GCC,
`-Wall -Wextra -Werror`, ASan/UBSan und `detect_leaks=1`; keine Sanitizerdiagnostik.
Die C-Profildatei besteht zusätzlich MSVC `/W4 /WX`. Beim vollständigen nativen
Build bleiben die bereits vorhandenen Warnungen aus Parser-/Adaptercode sichtbar.
Der abschließende Python-Werkzeugcheck baut frisch und führt zehn Kombinationen
mit je drei Aufrufen auf einer Unicode-Quelle bei EOF aus.
Geschlossene GUI-/F1-/Protokollprüfungen wurden dafür nicht wiederholt.

Alle aufbewahrten Ausgaben liegen auf D:. Die ersten manuellen Windows-Aufrufe
erbten noch das Windows-`TEMP` auf C:; dortige temporäre Werkzeugdateien wurden
nicht geprüft. Das fertige Werkzeug setzt Compiler-`TMPDIR`/`TMP`/`TEMP` selbst
auf sein Arbeitsverzeichnis unter D:, auch ohne Wrapper; im letzten Lauf belegt.
F: wurde nicht verwendet oder geprüft. Keine geschützten SDK-/Referenzbestände
gelöscht. Kleine Messquellen und Originalberichte bleiben als nächste Baseline
erhalten; sie benötigen keine Bildserien.
