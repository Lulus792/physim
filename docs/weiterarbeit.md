# Wiedereinstieg in die native UI-Arbeit

Stand: 31. Oktober 2026. Zuerst diese Übergabe, `git status` und relevante Diffs
abgleichen. Designverträge stehen in [AGENTS.md](../AGENTS.md) und der
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Eine Kontextkompaktierung
ist kein Anlass, abgeschlossene Prüfungen oder die Protokoll-Trennlinie erneut
zu beginnen. Detailhistorien stehen in den verlinkten Nachweisen und Git.

## Aktueller Abschluss: Einstellungen A als eigenes Fenster

**Neuester eingefrorener Anschluss: `settings-a-window-visual`, 141 eigene Eingaben.**
Das Kategorienfenster A ist jetzt nativ implementiert: eigenes nichtmodales
OS-Fenster, fünf Kategorien, echte Suche, Theme-/Schriftentwurf in separater
Vorschau, dreispaltige Änderungsprüfung und sichere Übernahme. Der alte
Tab-6-Einstellungsrenderer ist entfernt. Wiederaufruf erhält Entwurf/Suche;
das Hauptfenster bleibt im aktuellen Workspace bedienbar. Alle vierzehn
vorhandenen Einstellungen bleiben wirksam. Keine neue Variantenwahl einholen.
[Umsetzung und verbleibender Umfang](visuals/unified-workspace/SETTINGS-A-NATIVE.md),
[kompakter Prüfnachweis](verification/settings-a-window-2026-10-31.md).

Windows Release/Linux Debug ASan/UBSan: `settings-a-window-ready` je **7/7**,
`run-hxya7sl3` / `run-8qt40qsk`. Geprüft sind tatsächliche Texteingaben,
Suche/Wiederaufruf, paralleler Workspace-Wechsel, nur im Entwurf verändertes Theme,
Änderungsprüfung/Escape/OS-Schließen, ungültige Größe 139 behalten, auf 300
korrigieren, sicher speichern und erneut lesen. Dazu feldweise Transaktionen,
Tastaturfolge, zwölf Typografie-Unterprozesse, sieben Theme-Unterprozesse und
beide F10-Menügrößen. GUI-Leakprüfung unter Linux aus; Sanitizerfehler bleiben fatal.

Die anschließende kleine Bildkorrektur `settings-a-window-visual` betrifft den
einzeiligen Änderungszähler, die transparenten CSS-Auswahlfarben und die normale
Zurück-Schaltfläche. Dafür nur `settings_a_window` mit je zwei tatsächlichen
App-Prozessen nachprüfen; keine erneute Gesamtmatrix behaupten.
Der gezielte Nachlauf besteht je **1/1**: `run-mgnwir2w` / `run-cv0mpuxb`.
Die lokalen Build-/Prüf-/Archivhandles dieses Anschlusses sind beendet.
Alle referenzierten Archivbytes sind zurückgelesen. Details und Originalfehlläufe
bleiben im JSON erhalten. Die alten fehlerhaften Phasen sind keine bestandenen
Abnahmen; Debugger-Gegenproben zählen nur als Ursachenbeleg. 656 ersetzte
erfolgreiche BMPs entfernt (2.093.854.368 Bytes); Fehler und PNG-Referenzen erhalten.

**`479a8954` ist in origin und upstream gepusht.** Alle 1.106 erfassten
Compiler-/Testdateien entsprechen diesem Commit nach direktem Git-Blob-Vergleich
oder beidseitiger Zeilenendennormalisierung. [Commitabgleich und CI-Snapshot](verification/settings-a-window-commit-2026-10-31.json).

Der Nutzer-Mac ist **aus** und wird nicht angesprochen. Neue macOS-Nachweise nur
aus tatsächlich ausgeführten gehosteten Intel-/Apple-Silicon-Läufen werten.
Der neue Workflow umfasst zusätzlich Settings A und Transaktionen; seine
Ausführung ist bis zum dokumentierten Ergebnis offen. Gezielter Lauf
`41170676474`: Apple Silicon `124447773891`, Intel `124447773821`.
Menüregression `41170676487`: ARM `124447774112`, Intel `124447773804`.
Beim letzten gelesenen Snapshot lief ARM-Typografie, die anderen drei Jobs
waren eingereiht. Nicht blind neu starten; diese IDs bis zum terminalen Ergebnis
weiterverfolgen. Ältere erfolgreiche
Menü-/F1-/Typografieläufe gelten nur für ihren jeweiligen Quellstand.

## Nächster konkreter Umsetzungsblock

Einstellungen A ist noch **nicht vollständig abgenommen**:

- Anzeigeeinheiten öffnet bisher noch den alten modalen Einheitenablauf.
  Nach ADR 015/018 eigenes OS-Aufgabenfenster implementieren: echte Kanäle und
  Kataloge, Entwurf je Kanal, Vorschau, unabhängiges Anwenden/Abbrechen,
  vollständige numerische und reale Datenvalidierung erhalten. Allgemeines
  Abbrechen darf angewendete Einheiten oder einen offenen Einheitenentwurf
  nicht zurücksetzen.
- Completion D benötigt echte Backend-/Preferences-Optionen für Auto-Anzeige
  und sichtbare Trefferzahl (Standard fünf). Keine funktionslosen Ersatzfelder
  hinzufügen. Scope-/Signaturhilfe und gemeinsames Syntaxschema bleiben offen.
- Historische `settings_workflow`-/AX-Fixtures von Tab 6, Radios und globalem
  Fokus auf das aktuelle Aufgabenfenster portieren. Dabei Wiederherstellung,
  maximierte/restaurierte Geometrie und Sicherheitsassertionen erhalten.
  Der alte Test scheiterte bereits vor diesem Umbau am 0/0-Protokollgriff.
  Das begründet keinen neuen Test der abgeschlossenen Protokoll-Trennlinie.
- No-results-Zustand: Suchleisten-Löschen und Ergebnis-Rücksetzen teilen derzeit
  benutzerdefinierte Fokus-ID 26. Tastatureigentümer gezielt trennen und prüfen.
- Manuelle Screenreader-/DPI-/Monitor-/physische Eingabe-/macOS-Vollbildabnahme
  sowie weitere visuelle Feinheiten bleiben gesondert offen.

Feldweise Preferences-Zusammenführung aus `fa97b315` erhalten. Ein reiner Theme-
oder Schriftentwurf darf spätere Geometrie nicht überschreiben. Entwickeln-
Ausgangswerte sind von laufenden Simulieren-/Auswerten-Sitzungen getrennt.
Explizite gespeicherte Protokollwerte warten beim parallelen Übernehmen auf
Entwickeln; spätere bewusste Resize-/Sichtbarkeits-/Layoutaktionen haben Vorrang.
Beim Beenden `settings_current` verwenden, sonst überschreibt die aktuelle
andere Sitzung wieder den gespeicherten Ausgangswert.

Weitere native Arbeit: A2 in Szene/Mehrfachauswahl, Kontextmenüs A, B2 und übrige
freigegebene Oberfläche. Kein „volle Punktzahl“ aus Teilprüfungen ableiten.
`docking_workflow`/`inspector_workflow` prüfen bereits `fixed_panels`; alte freie
Docking-Fixtures nicht reaktivieren.

## Sicherer lokaler Anschluss

- `prepare-next.py <neuer-tag> settings-a-window-visual` verwenden. Der Helfer
  prüft den eingefrorenen Quell-/Runtime-/Archivstand. Neue Module ausdrücklich
  zur Eingabeliste hinzufügen, bevor ihre Include-Kette gebaut wird.
- Helfer/Manifeste/Byteindizes: `build/validation/explorer-inline-2026-10-29/`.
  `run.py`/`freeze.py` vor Verwendung auf tatsächliche Argumente prüfen.
- Gemeinsamer isolierter Baum: `build/validation/protocol-wrap-2026-10-26/3-isolated/`
  mit `src`, `windows-build`, `linux-build`. Keine Quell-/Runtimeänderung bei
  lebendem Build-, Test- oder Archivhandle. Nicht aus einem älteren Stand ersetzen.
- Diagnose und kompakte Aufräumnachweise: `build/validation/settings-window-2026-10-31/`.
  GDB-ROM-Gegenprobe ist kein normaler App-Pass. Der neue normale 7/7-Nachlauf
  ist der maßgebliche Eingabenachweis.
- Historischen Settings-A-Fehlerbeleg unverändert erhalten:
  `build/validation/macos-menu-workflows-2026-10-30/ui-22-typography-settings.png`.
  Er zeigt den damaligen Stand; er beschreibt nicht den neuen aktuellen Renderer.
- Nur D:, kein Zugriff auf F:. Alle neuen Build-/Testdaten unter `build/`.
  Erfolgreiche, ersetzte Rohbilder nach dokumentiertem Pfad-/Hashabgleich entfernen;
  aktuelle Referenzen, offene Fehler, Quellen, Archive und benötigte Symbole erhalten.
- Vor jedem Nachlauf konkrete Änderung/offene Frage und kleinsten passenden Test
  benennen. Ergebnisse zuerst als Text/JSON mit Quelle/Kommandos/Soll-Ist/Hash
  dokumentieren. Keine Matrix allein wegen eines Kontextwechsels wiederholen.

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
