# Native Umsetzung von F1-Design H

Prüfstand: `34ac138faeb438cdb3e855d12829b4e66a5b1fd6` auf `codex/f1-design-h`.
Die Implementierung geht von `0581915` aus.
`ec88cf7` ergänzt gemessene native Fensterrechtecke, verfügbare Displaygröße und
tatsächliche Pixelmaße. Die abschließende Nachprüfung umfasst außerdem den
schmalen Integralsatz und die übereinstimmende Formelbreite bei Messung/Zeichnung.

## Ergebnis

F1 verwendet weiterhin den vorhandenen C-/SDL-/Nuklear-Renderer. Links stehen
der vollständige Themenkatalog und direkte Abschnittssprünge gemeinsam. Rechts
stehen unabhängig scrollbar Modell und Beispiel. Beide Seiten besitzen ihren
eigenen kleinen Schalter im gemeinsamen Kopf; `Strg+Umschalt+B` und
`Strg+Umschalt+R` schalten sie per Tastatur. Beim Schließen bleibt der Fokus auf
dem sichtbaren Schalter. Die gegenüberliegende Seite behält Position und Breite.

C/Physim gilt global für Lernwege, Artikelfassungen, Suche und Quellcode.
Gemeinsame Physikerklärungen bleiben gemeinsame Inhalte. Modellkontexte stammen
aus den echten Pendel-, Wurf-, Wurf-mit-Widerstand- und Federbeispielen. Andere
Artikel zeigen einen ausdrücklichen Leerzustand. Codekopieren übernimmt die
vollständige Originalquelle der gewählten Sprache. Themenwechsel, Wiederöffnen
und Themewechsel erhalten Sprache, Seitenzustände und rechten Tab.

Artikelbreite und Kontextrechtecke werden unabhängig berechnet. Die schmale
Artikelwerkzeugleiste darf umbrechen; Leseschrift und kompakte Bedienoberfläche
besitzen eigene vorbereitete Schriftgrößen. Helle, dunkle und kontrastreiche
Darstellung verwenden die bestehenden semantischen Design-Tokens. Formelsatz,
Inline-Formeln, Tabellen, Links, Medien und Verlauf bleiben im bestehenden
Dokumentationsrenderer. Die gesetzten Formeln werden nicht durch Klartext ersetzt.

Native Auswahlgruppen heißen „Dokumentationssprache“ und „Kontextansicht“.
Auswahlzustände werden als native Radiobuttons veröffentlicht. Verborgene
Kontextkontrollen verschwinden aus dem veröffentlichten Accessibility-Modell.
Beide Suchfelder behalten getrennte Werte und Fokusidentitäten. Ein ausstehendes
letztes Unicode-Zeichen wird vor dem Schließen eines Bereichs übernommen.

## Native Fensterbilder

Die folgenden Zustandsbilder sind verlustfreie Konvertierungen tatsächlicher BMP-Aufnahmen
aus dem abschließenden Windows-Lauf `run-_2nt1jci`, ohne Layoutnachbearbeitung.

| Zustand | Dunkel | Hell |
| --- | --- | --- |
| Beide geöffnet | [Fensterbild](native-h-dark.png) | [Fensterbild](native-h-light.png) |
| Links geschlossen | [Fensterbild](native-h-dark-left-closed.png) | [Fensterbild](native-h-light-left-closed.png) |
| Beide geschlossen | [Fensterbild](native-h-dark-both-closed.png) | [Fensterbild](native-h-light-both-closed.png) |
| Rechts geschlossen | [Fensterbild](native-h-dark-right-closed.png) | [Fensterbild](native-h-light-right-closed.png) |

Weitere echte Aufnahmen: [760 × 540](native-h-minimum.png),
[760 × 540 mit UI 22 px und Leseschrift 32 px](native-h-large.png),
[Physim-Beispiel](native-h-physim-example.png),
[geöffneter Suchtreffer](native-h-pointer-search.png).

[Gesetzte Inline-Formeln](native-h-typeset-formulas.png) zeigt zusätzlich die
native Mathematik-Testseite mit Brüchen, Matrix, Indizes, Integral und Wurzel.
[Großes Integral im Mindestfenster](native-h-integral-large.png) zeigt den
zusätzlichen 32-px-Fall mit beiden geöffneten Seiten. Beide Aufnahmen stammen
ebenfalls aus `run-_2nt1jci`.

## Ausgeführte automatisierte Abnahme

| Plattform / Stand | Ausgeführt | Ergebnis / lokale Evidenz |
| --- | --- | --- |
| Windows, MSVC mit Sanitizern, `0581915` | F1-Fenster, Tastatur, externe UIA-Edit-/Link-/Dokument-Probes, Fontbedarf, Navigation, Sidebar, Inline-Mathematik, H | **10/10 bestanden**, `build/native/h-win-final-verified.log`, `run-2avugpy7` |
| Windows, gleicher Buildmodus, `ec88cf7` | H mit tatsächlich abgefragten nativen Paneelrechten und Pixelmaßen | **1/1 bestanden**, `build/native/h-win-window-proof.log`, `run-x41t5kit` |
| Windows, abschließende Formelkorrektur | H, Displayformelsatz und Inline-Mathematik mit zusätzlicher Prüfung auf Einhaltung der Formelzeile | **3/3 bestanden**, `build/native/h-win-formula-row.log`, `run-_2nt1jci` |
| Windows, `34ac138` | Lernweg-/Physik-/Modell-/Mechanik-/Grundlagenformeln, schmale Varianten und Metadatenzeile | **10/10 bestanden**, `build/native/h-win-formula-catalog-final.log`, `run-y2td9hh0` |
| Linux, WSL Alpine, GCC mit Sanitizern, Xvfb/Openbox, `ec88cf7` | F1-Fenster, Tastatur, externe AT-SPI-Probes, Verlauf, Navigation, Edit-/Link-AT-SPI, Sidebar, Inline-Mathematik, H | **10/10 bestanden**, `build/native/h-linux-window-proof.log`, `run-f7rkj_av` |
| Linux, gleicher Buildmodus, `34ac138` | H, Displayformelsatz und Inline-Mathematik mit zusätzlicher Prüfung auf Einhaltung der Formelzeile | **3/3 bestanden**, `build/native/h-linux-formula-row.log`, `run-0_12bm32` |
| Linux, `34ac138` | Lernweg-/Physik-/Modell-/Mechanik-/Grundlagenformeln, schmale Varianten und Metadatenzeile | **10/10 bestanden**, `build/native/h-linux-formula-catalog-final.log`, `run-gvl0aljy` |
| Linux, `ec88cf7`, isoliertes absichtlich verschlechtertes H-Layout | Abschnittshöhe auf eine Zeile begrenzen | Gegenbeispiel korrekt abgelehnt: `precise_rejection=true`, `build/native/F1-linux/sidebar-counterexample/report.json` |

`documentation_h` erfasst **56 native Layoutzustände**: Dunkel/Hell/hoher Kontrast,
1080/760 px, Leseschrift 16/32 px und alle vier Seitenkombinationen; zusätzlich
UI 22 px bei Leseschrift 32 px im Mindestfenster. Es verlangt tatsächliche
Paneelpositionen, unveränderte gegenüberliegende Anker, Artikelbreite mindestens
320 px sowie einen sichtbaren Lesekörper mit mehr als 220 px Breite und mindestens
120 px Höhe. 14 Größenkonfigurationen protokollieren gewünschte, verfügbare und
wirkliche Fenstergröße. Kleinere native Displays werden anhand ihrer nutzbaren
Fläche berücksichtigt; das Mindestfenster bleibt verpflichtend.

Weitere H-Aufgaben prüfen globale Sprachfilter, gemeinsame Inhalte, genaue
C-Quellcodekopie, vollständigen Physim-Code, Modell/Beispiel, erfolglose Suche,
fehlenden Kontext, Zustandserhalt, native Auswahlzustände und verborgene
Kontrollen. Ein echter SDL-Zeichen-/Tastaturablauf erhält das letzte `θ` beim
Schließen. Ein SDL-Zeigerablauf öffnet mit einem einzelnen Down/Up-Paar den
Pendel-Suchtreffer bei zuvor aktivem Suchfeld. Editorquellen und Simulationszeit
werden dabei auf Unverändertheit geprüft.

Die externe Windows-Abnahme prüft neun Fokus-/Edit-Szenarien einschließlich
Unicode-Auswahl und beider Suchfelder, Tabellen-/Linkbereiche und vollständiges
Lesen einer 512-KB-Quelle. Die Inline-Mathematik-Abnahme erfasst 48 Fensterbilder
und 162 echte Formelzeichnungen einschließlich mehrerer gleichzeitig lebender
Formeltexturen. Lernweg-/Quellkatalog und Offline-Ziele bestehen zusätzlich mit
`tests/test_documentation_tracks.py`.

Die abschließende Bildprüfung fand eine zusätzliche Überlagerung: Die Formel-
Höhenmessung benutzte die Clipbreite mit einem Sicherheitsabzug, die Zeichnung
die tatsächliche Widgetbreite. Nahe einer Umbruchgrenze konnten beide dadurch
verschiedene Varianten wählen. Die Höhenmessung verwendet jetzt dieselbe Breite
wie das dynamische Nuklear-Widget. Der Zeichenprüfer verlangt zusätzlich, dass
das gesamte Formelbild innerhalb seiner eigenen zugewiesenen Zeile liegt.
Beim elliptischen Integral stehen die Grenzen mit TeX `\limits` über/unter dem
Integralzeichen. Dadurch passt es mit unveränderter 32-px-Schrift in die schmale
Spalte; die mathematische Aussage und lineare Lesealternative bleiben gleich.
Alle 193 Ausdrücke, 1351 Displayvarianten, 193 Inlinevarianten und 197
veröffentlichten Displayblöcke bestehen `tools/math/check.py`.

Reproduktion: `tools/build.py --compiler cl --sdl third_party/SDL3-3.2.30
--sanitizers --test-display --test-filter documentation_h`; unter Linux analog
mit GCC, installierter SDL3 und einer echten X11-/AT-SPI-Sitzung. Die übrigen
oben genannten Manifestfälle lassen sich als weitere `--test-filter` angeben.

Binär-SHA256 des Prüfstands:

- Windows: `f88317b94f360846efce3ecff5057ca7b45aae684b8a1b59c2fb3ce7fd514165`
- Linux: `f20f11feca2924da0ef67e8d61a5c3b8aa7c4e2f1ccbd1053dab551f7fad04eb`

## Plattform- und Geräteumfang

Die normale Windows-App wurde am Stand `ec88cf7`, vor der abschließenden
Formelkorrektur, zusätzlich mit leerem Workspace geöffnet. F1,
globale Suche nach „Pendel“, Tastaturauswahl des Treffers und unabhängiges
Schließen/Wiederöffnen des Modellbereichs wurden tatsächlich bedient. Der
Sprachverlauf des vorhandenen Windows Narrator bestätigt „Modellbereich
ausblenden (geöffnet), button“ und „Pendel & Integratoren, button“. Dies belegt
diese konkreten Ansagen; vollständige Screenreader-Aufgaben für mathematisches
Lesen, Tabellen und alle Navigationselemente sind damit nicht abgenommen.

Die lokale Windows-Aufnahme verwendet Pixeldichte 1. Große Schrift und vorhandene
synthetische Dichtefälle ersetzen keinen physischen Wechsel zwischen Monitoren
mit unterschiedlicher DPI. Dieser Gerätewechsel bleibt offen.

Der erste neue macOS-Lauf auf ARM und Intel in
[Focused macOS F1 accessibility](https://github.com/Lulus792/physim/actions/runs/38915985885)
bestand 19/19 portable Verträge und die externe AX-Prüfung auf beiden Geräten.
Die GUI-Abnahme bestand 24/26 auf ARM und 25/26 auf Intel. Beide fanden den
inzwischen korrigierten Integralfall; ARM zusätzlich einen unmittelbaren
Fenstergrößenvergleich nach Resize. Der H-Test bestand auf Intel. Die korrigierte
Abnahme vergleicht tatsächliche Zielgröße und Paneele erst nach den vorgesehenen
Renderframes und protokolliert eine abgelaufene synchrone Resize-Wartezeit.
Eine vollständige erfolgreiche macOS-Abnahme des Endstands ist daraus noch
nicht abzuleiten.

Die Nachprüfung von `34ac138` wird separat ausgeführt:
[macOS ARM/Intel](https://github.com/Lulus792/physim/actions/runs/38919952426) und
[Linux GCC/Clang](https://github.com/Lulus792/physim/actions/runs/38919952359).
Ein noch laufender oder wartender CI-Lauf zählt nicht als bestandene Abnahme.

Die breitere Linux-CI ergänzt die bestandenen lokalen Tests:
[Linux UI sanitizer regressions](https://github.com/Lulus792/physim/actions/runs/38915985847).
Ihr GCC-Lauf bestand 46/47 GUI-Fälle; auch hier war der Integralfall die einzige
fehlgeschlagene Prüfung vor der Korrektur.

Die native UI wurde mit den lokalen Fachskills `ui-ux-designer`,
`design-system-engineer`, `ui-implementation`, `accessibility-engineer`,
`test-engineer` und `documentation-engineer` aus `D:/Skills/skills` umgesetzt und
geprüft. Die externe Desktop-Sichtprüfung nutzt den Computer-Use-Skill.
