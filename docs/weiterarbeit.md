# Wiedereinstieg in die native UI-Arbeit

Stand: 31. Oktober 2026. Zuerst diese Übergabe, `git status` und relevante Diffs
abgleichen. Designverträge stehen in [AGENTS.md](../AGENTS.md) und der
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Eine Kontextkompaktierung
ist kein Anlass, abgeschlossene Prüfungen oder die Protokoll-Trennlinie erneut
zu beginnen. Detailhistorien stehen in den verlinkten Nachweisen und Git.

## Aktueller Abschluss

`68ab5f71` ist in `origin` und `upstream` gepusht. Typografie-/Headerlauf
**`41158796808` (`Lulus792/physim`)**: Apple Silicon `124411442189` und Intel
`124411442496` sind terminal erfolgreich, je **4/4**. Die Archive wurden gegen
GitHub-Digests und ihre einzelnen Ergebnisdateien geprüft: drei Theme-/Fokusfälle,
zwölf Schriftgrößen-/Lese-/Abbruch-/Standardwert-Prozesse und zwölf tatsächliche
AppKit-Menüabbrüche unter 1,5 s je Architektur. Die 2-s-Notfallstornierung zählt
nicht. [Vollständiger kompakter Nachweis](verification/macos-typography-menu-2026-10-31.json).

Windows Release/Linux Debug ASan/UBSan: erste Aufrufe je **3/4 bestanden,
insgesamt fehlgeschlagen** (`run-hhh5fjki`/`run-36885fz9`). Alle drei Theme-Fälle
bestanden. Die alte Typografie-Fixture verlangte F1-Titel UI+7 statt H-Rollen.
Der gesonderte H-Rollen-Nachlauf blieb je **0/1** (`run-yikz9w1_`/`run-bjbddlso`):
erst danach griff die zweite alte Erwartung „Editor 16“, tatsächlicher Standard 12.
Beide Ursachen sind korrigiert: Body/Titel/Chrome/Controls werden über tatsächliche
H-Handlehöhen und behaltene em-Metriken geprüft; die vorherige Editorgröße bleibt
erhalten, nur Standardwerte verwenden `PS_PREFERENCES_DEFAULT.editor_size`.
Ohne eigenen Lesezoom folgt F1 der UI-Größe. Der letzte Typografie-Aufruf besteht
je **1/1** mit allen zwölf Unterprozessen: Windows `run-vt2p0lgi`, Linux
`run-gdzm0sqt`. Die unveränderten Theme-Fälle behalten ihren ersten Nachweis;
keinen einzelnen neuen lokalen Vollaufruf mit einer Binärdatei behaupten.

**Alle zugehörigen lokalen Build-/App-/Prüf-/Archivsessions und beide gezielten
Mac-Jobs sind beendet.** 104 entbehrliche erfolgreiche BMPs wurden entfernt
(376.257.552 Bytes). Originale Fehlerbilder, Quellen, Runtimes, App-Schritte,
reale Preferences und genau ein PNG des offenen Designbefunds bleiben erhalten.
Weitere automatisch ausgelöste breite CI ist davon getrennt zu bewerten.

## Nächster konkreter Umsetzungsblock: Einstellungen A

Der tatsächliche 22-px-Bildabgleich zeigt weiterhin das alte Formular im
Hauptfenster. `open_settings` in `app/settings_ui.inc:134` setzt nur Tab 6;
`settings_view` ab Zeile 257 besitzt noch keine Kategorien/Suche/Live-Vorschau
oder Änderungsprüfung. Das ist eine **offene native Umsetzungslücke**, keine
belegte technische Unmöglichkeit. Die bestandenen Schrift-/Menütests nehmen das
Design nicht ab. [Befund und geschützter einzelner Bildbeleg](verification/settings-a-native-gap-2026-10-31.md).

Zuerst [ADR 018](decisions/018-selected-settings-a.md),
[verbindliche Spezifikation](visuals/settings-options/APPROVED.md) vollständig,
[Referenz](visuals/settings-options/index.html?approved=1&group=appearance) und
[ADR 015](decisions/015-independent-task-windows.md) lesen. **A ist bereits
freigegeben; keine neue Variantenentscheidung einholen.** Danach passende
zentrale Skills nach `D:/Skills/skills/AGENTS.md` laden: UI-Umsetzung/Designsystem,
C17/Lebenszeiten und Accessibility. Die erste deterministische Auswahl schlug
Accessibility und C vor; UI-Umsetzung/Designsystem anhand des tatsächlichen
Designauftrags ergänzen, nicht auf die Triggerauswahl beschränken.

Eigenes nichtmodales OS-Aufgabenfenster mit eigener Eingabe-/Nuklear-/AX-/GL-
Oberfläche; keine zweite App-Instanz und kein WebView. Vorhandene Fenster-
Lebenszeiten/Font-Ownership aus `panel_windows.inc`, `graphic_export_ui.inc`,
`project_manager_ui.inc` und `documentation_window.inc` anhand tatsächlicher
Aufrufstellen prüfen. Fünf Kategorien, Suche, Optionen, separate Entwurfsvorschau,
vollständiger Änderungsvergleich und wirksame/gesperrte Fußaktionen wie freigegeben
umsetzen. Wiederaufruf erhält den Entwurf; Hauptfenster bleibt bedienbar.

Bestehende sichere Preferences-/Font-Transaktionen erhalten. Theme-/Schriftentwurf
darf parallel geänderte Arbeitsflächengeometrie nicht zurückschreiben; Defaults
bleiben Entwurf. Unabhängig angewendete Anzeigeeinheiten samt eigener offener
Einheitenaufgabe beim allgemeinen Abbrechen erhalten. Reale Kanäle/Kataloge,
vollständige numerische Validierung und Fehlerwiederholung verwenden.
Die Ergänzungen der gewählten Completion D (Auto-Anzeige, fünf sichtbare Zeilen,
restliche Treffer scrollbar) in „Editor & Sicherung“ berücksichtigen; tatsächlichen
Backend-/Preferences-Bestand vor Feld-/Schemaänderungen prüfen, keine funktionslose
Galerie-Nachbildung. Keine alten Tab-6-/No-op-Übernehmen-Erwartungen nur für Tests
beibehalten. Benutzerfunktionen und genaue Designfreigaben haben Vorrang.

Weitere offene native Arbeit bleibt bestehen: Completion D mit echter Scope-/
Signaturhilfe und gemeinsamem Syntaxschema, A2 in der Szene/Mehrfachauswahl,
Kontextmenüs A, B2 und übrige freigegebene Oberfläche. Andere aktive historische
F10-/Bereichsfokusfälle bei relevanter Änderung am tatsächlichen registrierten
Einstieg prüfen; alte freie Docking-/Inspector-Fixtures nicht reaktivieren.
`docking_workflow` und `inspector_workflow` rufen bereits `fixed_panels` auf.

## Sicherer lokaler Anschluss

**Neuester eingefrorener Anschluss: `mac-typography-menu-accepted-local`.**
132 eigene Eingaben, 1101 Compiler-/Testdateien entsprechen nach beidseitiger
Zeilenendennormalisierung `68ab5f71`; 212 Windows-/111 Linux-Runtimedateien und
alle referenzierten Archivbytes sind zurückgelesen. Der Git-Blob des
libunibreak-Manifests enthält selbst CRLF; diese Ausnahme ist eingegrenzt.
Der akzeptierte Anschluss übernimmt dieselben App-Bytes wie der tatsächlich
ausgeführte Nachlauf `mac-typography-menu-editor` und fügt keine Quelländerung hinzu.
`mac-typography-menu-first` und `mac-typography-menu-h-roles` bleiben unverändert.

- Gemeinsamer isolierter Baum: `build/validation/protocol-wrap-2026-10-26/3-isolated/`
  mit `src`, `windows-build`, `linux-build`. Nicht aus einem älteren A2-/Menüstand
  überschreiben. Keine Quell-/Runtimeänderung bei lebendem Build-/Test-/Archivhandle.
- Eingaben/Manifeste/Byteindizes: `build/validation/explorer-inline-2026-10-29/`.
  `prepare-next.py <neuer-tag> mac-typography-menu-accepted-local` prüft den alten
  Stand und übernimmt den eigenen Eingabebestand. **Neue Dateien/Module ausdrücklich
  zur Eingabeliste hinzufügen**, bevor ihre Include-Kette gebaut wird.
  `run.py` und `freeze.py` liegen dort; vorher ihre tatsächlichen Argumente lesen.
- Kampagnenberichte, Originalfehlerlogs und hashgeprüfte Mac-ZIPs:
  `build/validation/macos-menu-workflows-2026-10-30/`. Der einzelne geschützte
  Settings-A-Beleg heißt `ui-22-typography-settings.png` (146.649 Bytes); Hash im Befund.
- Vor jedem neuen Lauf: relevante Änderung, offene Frage und kleinsten passenden
  Test benennen. Quellen/Kommandos/Soll-Ist/Exitcode/Hash/Restbefunde zuerst als
  Text/JSON sichern; Bilder nur für tatsächliche visuelle Fragen. Keine abgeschlossene
  Matrix, Trennlinie oder A2-Auswahl allein wegen eines Kontextwechsels wiederholen.

## Abgeschlossen – Umfang und Grenzen erhalten

| Teil | Ergebnis und Nachweis | Verbleibende Grenze |
| --- | --- | --- |
| Typografie und nativer Header | `68ab5f71`, `41158796808`: Intel/Apple Silicon je 4/4, zwölf UI-Größen-/Lese-/Abbruch-/Default-Prozesse und zwölf echte Menüabbrüche. Lokal je drei unveränderte Theme-Fälle plus gesondert bestandener Typografienachlauf. [Nachweis](verification/macos-typography-menu-2026-10-31.json). | Editorerhaltung mit realem Standard 12 px, keine vollständige Sechs-Editorgrößenmatrix; Einstellungen A ist dadurch nicht umgesetzt. |
| Mac-Studien-/Projektmenüs | `725dd84e`, `41156020989`: Intel/Apple Silicon je 5/5 mit je neun echten AppKit-Menübefehlen; lokale erste Läufe Windows 4/5 und Linux 5/5, danach reparierte Batch-Fixture je 1/1. [Nachweis](verification/macos-study-menu-2026-10-31.json). | Kein einzelner neuer lokaler Vollaufruf mit einer Binärdatei behauptet; Weitere Bereichsfokusabläufe und gesamte native UI-Abnahme bleiben offen. |
| F1-Reparaturen auf Mac | `b648b0e7`, `41155920224`: Intel/Apple Silicon je 5/5 GUI, 1/1 portable Fontquelle, 3/3 Metriktests und drei gezielt abgelehnte Gegenbeispiele; je 56 H-Geometrien/24 Bereichszustände. [Nachweis](verification/macos-f1-repairs-2026-10-31.json). | Gezielte Teilabnahme; keine manuelle VoiceOver-/Vollbild-/physische Menüleistenbedienung oder gesamte GUI-Suite. |
| Mac-Dateitabs/Projektmanager/Einstellungen | `7a95c8ae`: Intel/Apple Silicon je 4/4 mit je 68 tatsächlich gebauten/analysierten Projekten; Windows/Linux gleiche Abdeckung nach separat bestandenem Python-Prüfernachlauf mit unveränderter App. [Nachweis](verification/macos-workspace-menu-2026-10-31.json). | Übrige aktive Menütests und gesamte GUI-/physische Abnahme bleiben offen. |
| Mac-Dokument-/Completion-/API-Menüeingaben | `c35fc39d`: Intel/Apple Silicon, Windows Release und Linux Debug ASan/UBSan je 4/4 registrierte Fälle / sechs App-Unterprozesse. Gleicher Mac-Stand zusätzlich je 7/7 Menüregression. [Nachweis](verification/macos-editor-menu-2026-10-31.json). | Gesamte GUI-/Design-/VoiceOver-/physische DPI-Abnahme und andere aktive historische Menütests offen. |
| Historische Mac-Toolbar-/Tastaturabläufe | `8d75cee7`: Intel/Apple Silicon je 7/7 einschließlich Menüadapter und App-Start; Windows Release/Linux Debug ASan/UBSan je 2/2. [Nachweis](verification/macos-menu-workflows-2026-10-31.json). | Übrige historische Workflows, manuelle VoiceOver-/Vollbildprüfung und physische Großbild-/DPI-Abnahme offen; ARM-Großvariante hatte 1080×740. |
| F1-Link-Gate und Offline-Lizenzen im SDK | `ed3866ed`: 188 Themen/234 Abschnittslinks unter Windows/Linux; Herkunft und beide Lizenzen tatsächlich per F1-Tastatur geöffnet, Quellenadresse kopiert; SDK-Umzug mit fehlenden Dateien als Gegenprobe. Intel/Apple Silicon je 4/4. [Nachweis](verification/documentation-links-2026-10-30.json). | Gesamte GUI-/Inhalts-/Screenreader-Abnahme bleibt offen; fremde Status-Ergänzung nicht mit geprüft. |
| macOS-Systemmenüleiste | `d43dcd6c` + `517021f0`; je 2/2 native Fälle Intel/Apple Silicon, je 4/4 Menü-/Headerfälle plus Start unter Windows/Linux. [Kompakter Nachweis](verification/macos-menubar-2026-10-30.json). | Manuelle Vollbild-/VoiceOver-Prüfung und übrige historische Mac-GUI-Workflows offen; Toolbar/Tastatur siehe eigene Zeile. |
| Trennlinie oberhalb des Protokolls | **Behoben seit `ef1f3321`.** Dauerhaft 1 px, gemeinsame Konturfarbe; Akzent bei Hover/Fokus. [Ursprüngliche Korrektur und negative Kontrolle](verification/protocol-separator-2026-10-29.json). [Nachlauf 30.10.](verification/protocol-boundary-workspaces-2026-10-30.json): Windows Release und Linux Debug ASan/UBSan, je 12/12 Zustände in Entwickeln, Simulieren, Auswerten. | Große Schrift im Nachlauf: 22 px; keine neue macOS-/DPI-/Screenreader-Abnahme. |
| A2 für Textauswahl | Gemeinsamer Textfeldrenderer, Commit `c12e2a10`. [Umfang und native Belege](visuals/unified-workspace/TEXT-SELECTION-A2-NATIVE.md). | Belegt keine vollständige A2-Umsetzung aller Objektarten. |
| A2 für Protokoll- und Explorerlisten/-kacheln | Commit `d228b16e`. [Native Umsetzung](visuals/unified-workspace/LIST-SELECTION-A2-NATIVE.md), [Windows-/Linux-Prüfungen und sechs native Gegenproben](verification/list-selection-a2-2026-10-30.json). | Mehrfachauswahl bleibt offen. |
| A2 für Workspace- und Szenenbaum | [Umsetzung](visuals/unified-workspace/TREE-SELECTION-A2-NATIVE.md), [Nachweis 30.10.](verification/tree-selection-a2-2026-10-30.json): je zehn tatsächliche Zustände Windows/Linux, sechs native Gegenproben, zusätzlich 72 Linux-Pendellaufansichten. | Einzelauswahl; 32 px sind eine gezielte Rendererbelastung, keine neue Einstellungsoption oder reale 200%-DPI-Abnahme. |
| F1: ungültige synthetische Umschalttastenereignisse unter Linux | Commit `8840b9c9`: SDL-Ereignisse ausdrücklich initialisiert; sieben Linux-Kontextfälle bestanden. | Eigenständige Windows-Fokusbefunde bleiben offen; nicht als gleicher Fehler behandeln. |


## Umgebung und fremde Arbeit

Der Nutzer-Mac ist seit seiner Meldung vom 31.10. **aus**. Keine SSH-/Probe,
Uploads oder lokale Mac-Aktionen ohne neue Verfügbarkeitsmeldung. Die neuen
Mac-Nachweise stammen ausschließlich von gehosteten GitHub-Runnern. Vor späterer
Nutzung des Nutzer-Mac `D:/NexaCloud/docs/MACOS_TESTING.md` und aktuelle Mac-Hinweise
in `D:/NexaUI/AGENTS.md` lesen; eigene Reste gezielt aufräumen, gemeinsame Werkzeuge
erhalten. Manuelle VoiceOver-/Vollbild-/physische Menüleisten-/DPI-Abnahme und
gesamte GUI-/Designabnahme bleiben offen; Teiltests beweisen keine volle Punktzahl.

Nur D: für Quellen, Builds, Tests, Archive und temporäre Daten; **F: weder lesen
noch schreiben**. WSL ausschließlich `Alpine-Dev`; `D:/WSL/Alpine-Dev/ext4.vhdx`
und Offline-Abhängigkeiten/Referenzen erhalten. Details in
[Speicheranleitung](development-storage.md). Die automatisch abgelehnte Löschung
von 64 `.obj` / 6.103.831 Bytes unter `build/validation/documentation-links-2026-10-30/windows-build/obj`
wurde nicht ausgeführt. Nicht über andere Shell/Kodierung umgehen.

Paralleles WIP in AT-SPI-/Orca-/Workflow-/Statusdateien sowie die vorhandene
Löschung von `dist/index.html` getrennt erhalten. Nicht pauschal stagen, zurücksetzen
oder als mit geprüft erklären. Die isolierten Nachweise betreffen den angegebenen
Git-Stand mit ausschließlich eigenen Änderungen. Die fremde Status-Ergänzung
verweist auf einen noch nicht registrierten Bereinigungsbericht; deshalb keine
pauschale Link-/Gesamt-CI-Abnahme. Eigene abgeschlossene Änderungen normal committen
und in `origin` und `upstream` pushen; keine erzwungenen Pushes.
