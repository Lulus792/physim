# Index abgeschlossener nativer Teilprüfungen

Stand: 5. November 2026. Diese historischen Nachweise behalten ihren angegebenen
Quellstand und Umfang. Sie beweisen keine vollständige aktuelle UI-, Plattform-
oder physische Abnahme. Eine Wiederholung benötigt eine relevante Änderung,
einen neuen Fehlerbeleg oder eine bislang offene Prüfdimension. Nicht allein
wegen Kontextwechsel erneut anfangen; die ursprünglichen Nachweise unverändert lassen.

## Neuere Editorprüfungen

- [C-Backend-Performance-Baseline](verification/completion-perf-2026-11-05.md):
  Anwendungscode `694bb586` unverändert, Windows Release sechs Quellen und getrennte
  Syntax-/Binding-/Modell-/Cache-Messungen. Große frische/Cursor-Fälle verfehlen
  die technische 16,7-ms-Prüfmarke; **keine erfolgreiche Performance-Abnahme**.
  Neue Mess-CLI Windows/Linux je 47 Fälle geprüft, Linux ASan/UBSan/Leakprüfung.
  Keine neuen GUI-/Mac-Prüfungen, keine Bilder oder Beschleunigung behauptet.
- [Native C-Locals](verification/completion-c-2026-11-05.md), `694bb586`: Windows/Linux je
  Unit 8/8 + GUI 6/6, C/Physim über drei Editorziele, 121/121 C-Modellfehler erhalten
  den Snapshot; Typ-/Quellkarte, Unicode, lokale API-Abschirmung, Ersetzen/Undo und
  leere Argumente geprüft. Drei Paketlizenzen/6.422 Dateihashes gelesen. **Neuer
  separate Signaturhilfe und physische Abnahme noch offen.** Gehosteter
  ARM-/Intel-Nachlauf [je 8/8 mit beiden Original-ZIPs/Digests](verification/completion-c-macos-2026-11-05.json).
- [C-Scope-Sammlung](verification/c-bindings-2026-11-05.md), `37cb80dd`: Windows/Linux je Unit
  1/1, final 35 Codefälle, 182 Cursor-/Dokumentpaare, 65/65 Allokationsfehler und
  1.024 fehlerhafte Eingaben; echte Schleifenkopf-/Zeiger-Rückgabe-Regressionen und
  gezielt verworfene Bereichs-Gegenprobe. **Completion-Modell/GUI und Mac noch offen.**
- [C-Syntaxgrundlage](verification/c-syntax-foundation-2026-11-05.md): echte
  Grammatik, Quellcommit `a25683ef`, Windows/Linux je Unit 1/1, zusätzlicher strenger C17-ASan/UBSan-Build;
  94/94 Speicherfehler und 1.024 fehlerhafte Eingaben geprüft. **Noch keine
  C-Scope-/GUI-Integration oder neue Mac-Abnahme.**
- [Physim-Stringkontext](verification/completion-literals-2026-11-05.md): reguläre
  Vorherregression abgelehnt, elf Literal-Kontexte und Windows/Linux Unit 5/5 + GUI 6/6
  bestanden. Eigener gehosteter Nachlauf `41947340908` zu `5c3b6da4`: ARM/Intel je
  [8/8, Originalarchive und Digests gelesen](verification/completion-literals-macos-2026-11-05.json).
  Dieser Nachlauf enthält die anschließende C-Syntaxgrundlage nicht.
- [Native lokale Physim-Namen](verification/completion-locals-2026-11-05.md):
  Quellcommit `072c06f5`, Windows/Linux zuerst Unit 10/10 + GUI 6/6, danach GUI 2/2;
  Signatur-Gegenprobe abgelehnt. Nach Testkorrektur `e27fc144` gehosteter Mac-Lauf
  [ARM/Intel je 8/8](verification/completion-locals-macos-after-2026-11-05.json).
  Keine Mac-Gesamtabnahme oder Abnahme der anschließenden Stringkorrektur daraus ableiten.
- [Automatik-Prüfer gegen Fristrennen](verification/auto-info-deadline-2026-11-05.md):
  Quellcommit `0c525422`, lokale GUI je 3/3 und ursprüngliche gehostete Editorgruppen
  ARM/Intel je 7/7. Kein Nachweis der erst danach integrierten lokalen Vorschläge.
- [Physim-Parser-Scopes](verification/language-scopes-2026-11-05.md),
  [Kurzinfo-Bedienung](verification/completion-info-2026-11-04.md) und
  [Kurzinfo-Inhalte](verification/completion-help-2026-11-04.md) beschreiben getrennte
  Quellen/Phasen. Die jeweiligen Grenzen bleiben bestehen.

## Frühere Plattform- und Bereichsprüfungen

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

Der frühere ausführliche Arbeitsstand mit weiteren Syntax-/Viewport-/Preferences-/
Such-/Einheitenbelegen bleibt in Git erhalten: `git show ae331255:docs/weiterarbeit.md`.
Die einzelnen Originalberichte unter `docs/verification` bleiben versioniert.
Aktuelle Aufträge und tatsächliche noch laufende Handles stehen ausschließlich in
[weiterarbeit.md](weiterarbeit.md).
