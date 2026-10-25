# Auswerten: schmale Seiten und stabile Bedienung

Der Abgleich vom 25. Oktober 2026 setzt die Mindestbreiten aus
[RESIZING.md](RESIZING.md) um. Die Laufliste endet bei UI-Größe 16 jetzt bei
140 px, die Details bei 180 px; zuvor endeten beide bei 208 px. Bei UI-Größe 22
werden diese Grenzen auf 193/248 px skaliert, damit die gewählte größere Schrift
Platz behält. Die Standardwünsche bleiben 232/242 px, die Mitte mindestens
300 px. Eine vorübergehende Begrenzung überschreibt keinen gespeicherten Wunsch.
Das ist eine bewusste Textskalierung, keine behauptete technische Unmöglichkeit
der Referenzmaße. Die neuen, noch nicht ausgewählten Designkandidaten werden
hiermit nicht übernommen.

Schmale Import-, Simulations- und Detailbuttons umbrechen ihre vollständige
Beschriftung. Der untere Bereich der Laufliste reserviert dafür die tatsächlich
benötigte Höhe. Breite einzeilige Buttons behalten ihre bisherigen 30/32 px.
Die Textprobe prüft sämtliche 24 UTF-8-Bytes der Importbeschriftung und ihre
gezeichneten Rechtecke innerhalb des sichtbaren Buttons.

Bei Tastaturänderungen bleibt der Diagrammtrenner mit seiner vollen 9-px-
Trefffläche sichtbar. Ein bereits begonnener Drag behält seine Eingabe auch
außerhalb der Panelkante; Escape und echter Fensterfokusverlust stellen den
vorherigen Höhenwunsch wieder her. Der Eingabebesitz gilt nur für die aktive
Auswertung und blockiert während dieses Drags andere Panels und deren Scrollen.

Ein weiterer tatsächlich gefundener Fehler betraf das Herausziehen: In der
Linux-Ansicht lag die Mitte des vorhandenen Kopfes auf dem anderen Reiter.
Schon der erste Tastendruck der Maus wechselte dadurch von Messdaten zu
Analyseergebnis. Der [Hardware-Watchpoint](analysis-sides-2026-10-25/counterexample-drag-watch.log.gz)
zeigt `show_report: false -> true`, Mausposition 532/225 und den getroffenen
Reiter 374,5/198/159,5/54 bei aktivem Panelzug. Reiterklicks werden jetzt erst
beim Loslassen übernommen; eine Bewegung über die bestehende Ziehschwelle
verwirft den ausstehenden Klick. Tastatur und native Assistive-Aktivierung
behalten ihren eigenen Weg. Geometrie und sichtbare Gestaltung ändern sich
dadurch nicht. Die Regression zieht ausdrücklich am inaktiven Reiter und
verlangt den Erhalt der Ansicht im externen Fenster und nach der Rückkehr.

## Ausgeführte Prüfung

Die [eingefrorenen Belege](analysis-sides-2026-10-25/README.md) gehören zum
internen Quellstand 10 auf Basis von `5717ba7`. Das ist keine Produktversion.
311 Eingabedateien einschließlich der vorhandenen fremden Accessibility-Arbeit
werden vor und nach den Abläufen gegen ihre Rohhashes geprüft. Diese fremden
Änderungen werden mit diesem Abgleich nicht eingecheckt. Ein sauberer Checkout
wurde damit nicht als Laufzeitumgebung behauptet.

| Tatsächliche Umgebung | Ergebnis |
| --- | --- |
| Windows, MSVC 19.38.33145, Release, SDL 3.2.30, NVIDIA OpenGL | 9/9 Gruppen mit Systemschrift, zusätzlich 2/2 mit Offline-Schriften |
| Alpine-Dev, GCC 15.2.0, Debug ASan/UBSan, SDL 3.2.30, Xvfb/Openbox, Mesa/llvmpipe | 9/9 Gruppen mit Systemschrift, zusätzlich 2/2 mit Offline-Schriften |

Die neun Gruppen sind fünf Auswertungslayouts (Dunkel, Hell, hoher Kontrast,
Dunkel/Hell mit UI 22 und 1080 × 740), zwei Einheitenabläufe, Werkbank und
native Teilfenster. Die Offline-Proben wiederholen Dunkel normal und Hell groß/
schmal mit denselben Programmdateien. Die Auswertungsprobe prüft alle vier
Seitenzustände, drei externe Bereiche samt Rückkehr, Wunschgrößen, Einstellungen,
10/40-px-Schritte, Mausabbruch, Statistik-Scrollen, Pos1-Minima und normale
Klick-/Tastaturwechsel zwischen Messdaten und Analyseergebnis. Auswahl, 301
Messpunkte und vollständige Statistik bleiben erhalten.

Der strenge Vergleich der vollständigen normalen Linux-Dunkelansicht bei
1440 × 1000 mit dem vorherigen nativen Bild ergibt **0 abweichende RGB-Pixel**.
Er belegt den Erhalt dieser Standardansicht, keine Pixelgleichheit mit HTML.
24 aktuelle und sechs zusätzliche native Kontroll-/Gegenbilder sind verlustfrei
als PNG erhalten; ihre Original- und RGB-Hashes stehen in den Manifesten.

[Windows: große Schrift, kleinste Seiten](analysis-sides-2026-10-25/windows-analysis_layout_dark_large_narrow-analysis-minimum-sides.png)
· [Linux: derselbe Zustand](analysis-sides-2026-10-25/linux-analysis_layout_dark_large_narrow-analysis-minimum-sides.png)
· [Windows: normale helle Ansicht](analysis-sides-2026-10-25/windows-analysis_layout_light-analysis-both.png)

## Gegenproben und Grenzen

Der unveränderte Produktcode wird in der neuen Grenzprobe gezielt bei 208 statt
140 px verworfen. Zwischenstände mit zu hohen breiten Buttons, einem unsichtbaren
Trenner und verlorener Drag-Eingabe sind als Gegenbilder beziehungsweise
Originalabläufe erhalten. Die zunächst als Scrollproblem betrachtete Linux-
Ansicht war tatsächlich auf den anderen Reiter gewechselt; Bild und Watchpoint
begrenzen die korrigierte Diagnose ausdrücklich.

Fehler des Prüfaufbaus werden nicht als Produktfehler oder bestandene Abnahme
gezählt: fehlendes Arbeitsverzeichnis, zwei Fokusaufträge im selben Frame,
alte 242-px-Annahme bei großer Schrift, Auslesen vor der Zeichenphase und ein
Klick auf einen weggescrollten Reiter. Für die unabhängige Klickprobe wird die
obere Leseposition ausdrücklich als Fixture vorbereitet; der vorherige
Statistikablauf prüft weiterhin tatsächliches Mausrad-Scrollen. Ein früher
Linux-Shellstart mit falschen Zeilenenden erreichte die App nicht.

Die Eingaben stammen aus nativen SDL-Regressionen. Dies ist kein neuer physischer
Monitor-/DPI-, macOS- oder externer Screenreader-Nachweis. Die vollständige
Oberfläche und F1 bleiben im [Gesamtauftrag](NATIVE-FIDELITY.md) offen.
Schnelle gebündelte Eingabeereignisse benötigen einen eigenen Folgeabgleich;
die hier ausgeführten Klicks verwenden getrennte native Down-/Up-Frames.

155 ersetzte BMPs früherer erfolgreicher eigener Windows-Läufe wurden nach
Pfad-, Nutzungs- und Hashprüfung entfernt: **753.659.790 Byte** (rund 719 MiB).
Fehlerfälle, aktuelle Rohbilder, Referenzen und das benötigte breite Gegenbild
bleiben erhalten. Das [Protokoll](analysis-sides-2026-10-25/cleanup.json) nennt
jede Datei. Neue Windows-Testdaten liegen im ausdrücklich angelegten Ordner
`F:\Physim\validation\analysis-sides-2026-10-25`; Linux-Arbeitsdaten bleiben auf
ext4 in der vorhandenen Distribution.
