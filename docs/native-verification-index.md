# Index abgeschlossener nativer Teilprüfungen

Stand: 5. November 2026. Diese historischen Nachweise behalten ihren angegebenen
Quellstand und Umfang. Sie beweisen keine vollständige aktuelle UI-, Plattform-
oder physische Abnahme. Eine Wiederholung benötigt eine relevante Änderung,
einen neuen Fehlerbeleg oder eine bislang offene Prüfdimension. Nicht allein
wegen Kontextwechsel erneut anfangen; die ursprünglichen Nachweise unverändert lassen.
Statusaussagen in älteren Einträgen beziehen sich auf ihren Belegstand. Den
heutigen nächsten Schritt nennt ausschließlich der [kurze Arbeitsstand](weiterarbeit.md);
spätere Nachläufe ändern die ursprünglichen Originalberichte nicht.

## Neuere Editorprüfungen

- [Dateireiter und automatische Vorschläge](verification/file-tabs-auto-2026-11-05.md):
  dieselbe Stufe27 auf Mac Intel/ARM und lokalen unveränderten Windows-/Linux-
  Programmen reproduziert. Probe wartet begrenzt auf echtes automatisches Öffnen,
  prüft Quelle/Editor/Präfix/Geometrie. App und140ms-Timer unverändert.
  Lokal Windows Release/Linux ASan/UBSan je3/3; neue Mac-Abnahme noch offen.
- [OS-Fokus und Texteingabe im Zusatzfenster](verification/signature-focus-2026-11-05.md):
  echter Windows-Gegenfall, Quelleditor bei realem Fokusgewinn wieder zuordnen;
  Windows Release/Linux ASan/UBSan jeweils2/2 GUI-Gruppen. Je C/Ph drei Editoren,
  eigenes Peerprozess-Fenster und nichtleere Auswahl erhalten. Acht Bilder
  vollständig bytegleich zu erhaltenen Referenzen; nur neue Kopien entfernt.
  Physische Eingabe, Controls/Screenreader und neue Mac-Abnahme bleiben offen.
- [Mac-Abschluss der Dateinamenquelle0d3f4a65](verification/filename-wrap-macos-2026-11-05.json):
  Lauf42059129050 Intel/ARM jeweils14/14 Daten+12/12 GUI, beide Jobs terminal/success.
  Original-ZIPs/Digests und alle22 Einträge gelesen; vier neue Bildinhalte geöffnet,
  weitere14 bytegleich zu erhaltenen Referenzen. Keine Wiederholung oder weiteres
  Pollen dieses Laufs; keine Abnahme der späteren Fokuskorrektur daraus ableiten.
- [Dateinamen bei großer Schrift](verification/filename-wrap-2026-11-05.md):
  Windows Release/Linux ASan/UBSan jeweils4/4 Daten und3/3 GUI-Gruppen.
  Ganze Erweiterung/letzter Namensteil, exakte Spannen und echter22px-Draw/Clip
  samt Dateiöffnung; zwei gezielte Bilder geöffnet. Scope-Audit verhindert
  übergangene vorhandene Testdateien. [Originale](verification/filename-wrap-2026-11-05.json).
- [Mac-Nachlauf zum Zusatzfenster](verification/signature-detached-macos-2026-11-05.json),
  Quelle9cc44508, Lauf42049114569 terminal: Intel11/11 Daten+10/10 GUI,
  Original-ZIP/Digest/alle zehn Einträge gelesen. Vier neue Bilder geöffnet,
  vier bytegleich zu zuvor geöffneten Referenzen. ARM vor Start abgebrochen;
  keine ARM-Abnahme. Nicht weiter pollen; aktuelle neue Quelle separat prüfen.
- [Parameterhilfe im nativen Zusatzfenster](verification/signature-detached-2026-11-05.md):
  zwei Fokuskorrekturen; Windows Release und Linux ASan/UBSan jeweils7/7.
  C/Ph je drei Editoren, echter Zusatzkontext, Überlappung/Scrollgriff/Tippen
  und Rückkehr samt Quelle/Cursor/Auswahl/Anordnung; acht finale Bilder geöffnet.
  Test-Selektions- und Resize-Sequenz anhand ursprünglicher Befunde präzisiert.
  [Befehle, Quellen und Grenzen](verification/signature-detached-2026-11-05.json).
- [Gehosteter Mac-Nachlauf der Kartenüberschreitung](verification/signature-outside-macos-2026-11-05.json),
  Quelle27ffb2f6, Lauf42037853611: Intel/ARM jeweils11/11 Daten und9/9 GUI.
  Beide Original-ZIPs samt veröffentlichtem Digest und allen sechs Einträgen
  je Archiv gelesen; acht native Bilder tatsächlich geöffnet, Workspace ohne
  Häkchen. Hauptfenster-Drag und anschließende Eingabe belegt. Beide Jobs
  geschlossen; keine Abnahme des herausgelösten Workspaces daraus ableiten.
- [Parameterhilfe: über den Kartenrand ziehen](verification/signature-outside-2026-11-05.md):
  Hauptfenster, C/Ph: Griff halten, über den Quelleditor ziehen, dort loslassen,
  Erhaltung von Cursor/Auswahl/Scroll und anschließendes Tippen/Löschen geprüft.
  Windows direkter Nachlauf derselben Binärdatei nach echter Fokusaktivierung;
  Linux ASan/UBSan registriert1/1. Acht aktuelle Bilder geöffnet, Workspace ohne
  Häkchen. Nur Testabdeckung und korrekte Gesamtfrist geändert, keine neue
  normale Appfunktion. [Befehle und Grenzen](verification/signature-outside-2026-11-05.json).
- [Gehosteter Mac-Nachlauf der Scrollgriff-/Eingabekorrektur](verification/signature-thumb-macos-2026-11-05.json),
  Quelle05e43dc0, Lauf42031236181: **Intel/ARM jeweils11/11 Daten und9/9 GUI**,
  Original-ZIPs/Digests/alle12Einträge gelesen und acht native HUD-Bilder geöffnet.
  Beide Jobs geschlossen. Die spätere Workspace-Häkchenentfernung und neue
  Kartenüberschreitungsprüfung gehören nicht zu diesem historischen Quellstand.
- [Workspace ohne Häkchen](verification/workspace-no-checks-2026-11-05.md):
  Nutzerpräzisierung vom5.11. nativ umgesetzt; Checkboxzeichnung und reservierte
  Spalte entfernt, aktive Zeile/Symbole/Hierarchie erhalten. Windows Release und
  Linux ASan/UBSan je2/2 vorhandene Dateibaum-/Projektbaumabläufe; vier gezielte
  Bilder geöffnet. [Quellen und Grenzen](verification/workspace-no-checks-2026-11-05.json).
- [Scrollgriff und Editor-Eingabe](verification/signature-thumb-2026-11-05.md):
  Windows Release und Linux ASan/UBSan jeweils **6/6 GUI** nach zwei Produktkorrekturen.
  Echter alter Drag-Fehler abgewiesen; automatische Vorschlagsabfrage erhält den
  Editorbesitzer. Native Text-/ROM-Diagnose und Hardware-Watchpoint belegen die
  zweite Ursache. C/Ph je drei Ziele, Griff-Drücken/Bewegen/Loslassen und Erhaltung.
  Acht Bilder bytegleich zu geöffneten Referenzen; ein gezieltes Fehlerbild erhalten.
  [Quellen, Phasen und Grenzen](verification/signature-thumb-2026-11-05.json).
  [Quellcommit05e43dc0 und gehosteter Nachlauf42031236181](verification/signature-thumb-commit-2026-11-05.json).
  [Mac-Lauf42026549588](verification/signature-wrap-macos-first-2026-11-05.json)
  zum vorherigen Quellstand beendet: beide11/11 Daten,8/9 GUI; tatsächlicher
  OS-Fokus0 bei gültiger Quelle. Original-ZIPs gelesen; keine HUD-Abnahme.
- [Parameterhilfe: Umbruch und lange Signaturen](verification/signature-wrap-2026-11-05.md):
  Windows/Linux je4/4 Daten, präzisierte neue Unit je1/1; fünf bestehende GUI-Gruppen
  bestanden und abschließend neuer HUD-Ablauf gesondert je1/1. Unicode/Operatorgrenzen,
  tatsächliche800×540/22px,16Parameter, fester Hinweis, Mausrad/Alt-Navigation,
  unveränderter Editor, Nullparameter und Fehlerfarbe. Acht gezielte Bilder geöffnet.
  [Quellen, Phasen und Grenzen](verification/signature-wrap-2026-11-05.json).
  [Quellcommit ba95641f und neuer gehosteter Mac-Lauf42026549588](verification/signature-wrap-commit-2026-11-05.json).
  [Älterer erster HUD-Mac-Lauf](verification/signature-hud-macos-first-2026-11-05.json)
  beendet: ARM/Intel10/10 Daten,8/9 GUI; Fokus-/Darstellungskontrolle offen,
  Original-ZIPs gelesen. Kein Beweis für den neuen Umbruchstand.
- [Erste sichtbare native Parameterhilfe](verification/signature-hud-2026-11-05.md):
  Windows Release/Linux ASan-UBSan jeweils6/6 GUI; C/Ph je Modell/Analyse/Datei,
  reale SDL-Eingaben, aktive Semibold-Syntaxglyphen/Clips, Auto/manuell/Escape,
  innere/überzählige Argumente, unabhängige Übernahme und alter Locals-/Modell-
  Zustand bei OOM. Echter nativer Float-/Umbruch-Gegenbeleg28/56px, korrigiert28/28.
  Zehn gezielte Bilder geprüft. Wort-/Tokengrenzen, lange/kleine Layouts, Detached,
  Fensterfokus, AX/physische Abnahme und weitere Verträge offen; keine volle UI-Abnahme.
  [Quellen/Programme/Prüfstufen](verification/signature-hud-2026-11-05.json).
  [Quellcommit und gehosteter Mac-Nachlauf](verification/signature-hud-commit-2026-11-05.json),
  [sichere Entfernung bytegleicher Bildduplikate](verification/signature-hud-cleanup-2026-11-05.json).
- [Gemeinsamer Datenkern der Parameterhilfe](verification/signature-model-2026-11-05.md):
  C-/Physim-Quellbesitz, tatsächliche Ziele,370C-/396formalePh-API-Aufrufe sowie
  eigene Funktionen/Methoden. Windows/Linux App+je10/10 Daten, final neuer
  unveränderter-Aufruf-Cache je1/1;38 Fälle,16Parameter,72OOM-Pfade, fünf
  gezielte Ablehnungen. Keine Bilder/neue GUI.190Spezial-/Property-/Dokueinträge,
  weitere Intrinsics/Funktionswerte und sichtbarer HUD noch offen. [Mac-Nachlauf
  ARM/Intel je10/10 Daten +8/8 GUI, Originalarchive geprüft](verification/signature-model-macos-2026-11-05.json).
  [Stufen/Hashes](verification/signature-model-2026-11-05.json).
- [Tatsächliches C-Aufrufziel und Namensüberdeckung](verification/c-lookup-2026-11-05.md):
  getrennte Abfrage auf dem vorhandenen C-Snapshot, eigene unfertige Initialisierer,
  Typedefs/Enum/Parameter, vollständige direkte Funktions-/Zeigerköpfe. Windows/Linux
  zunächst je8/8 Daten, endgültige neue Abfrage52 Fälle samt zwei belegten
  Bereichskorrekturen, zwei Scratch-Fehlern und vier sauberen Negativprogrammen.
  Lokal keine neue GUI/Bildserie; [gehosteter Quellnachlauf ARM/Intel je9/9 Daten
  +8/8 GUI, Originalarchive geprüft](verification/c-lookup-macos-2026-11-05.json).
  Gemeinsamer Resolver und sichtbarer HUD offen. [Prüfstufen/Hashes](verification/c-lookup-2026-11-05.json).
- [Physim-Modulköpfe während unvollständiger Eingabe](verification/module-signatures-2026-11-05.md):
  echte Parser-/Lexer-Wiederaufnahme für spätere Modul-/Methodenköpfe, lokale
  Funktionen weiter über Cursor-Präfix. Windows/Linux erster Stand je17/17 Daten
  und6/6 GUI, endgültige mode-only-Sicherung je5/5 Daten.13 Fälle/70 Präfixe,
  drei beschädigte Namespace-Köpfe, vier gezielte Ablehnungen und alter begrenzter
  Timeout. Strikte AST-/Präfixverträge erhalten. [Gehosteter Mac-Quellnachlauf:
  ARM/Intel je8/8 Daten +8/8 GUI, Originalarchive geprüft](verification/module-signatures-macos-2026-11-05.json).
  **Gemeinsamer Resolver, HUD und Gesamt-/physische Abnahme offen.**
- [Geschriebene C-Funktionssignaturen](verification/c-signatures-2026-11-05.md):
  tatsächliche Deklaratoren/Prototypen/einfache Zeiger/Definitionsparameter und
  alte geschriebene Typen aus dem unveränderten C-Snapshot. Endgültig Windows/Linux
  je Unit 12/12 + GUI 6/6; 33 Fälle, 205 Präfixe, 775 Bereichsvergleiche, drei
  Scratch-Fehler und vier gezielte Negativprogramme. Drei belegte Randfehler
  behoben; zweite Matrizen aufgrund neuer Produktionskorrektur. **Gemeinsame
  Auflösung, sichtbarer HUD und Gesamt-/physische Abnahme offen.** Gehosteter
  Quellnachlauf `f3ed4068` [ARM/Intel je Unit 7/7 + GUI 8/8, Originalarchive
  geprüft](verification/c-signatures-macos-2026-11-05.json); spätere Parserarbeit ausgeschlossen.
- [Geschriebene Physim-Funktionssignaturen](verification/signature-declarations-2026-11-05.md):
  echte Parserproduktionen für Parameter/Typen/Rückgabe, Methoden/Besitzer und
  lokale Bereiche. Windows/Linux je 15 Unit-Gruppen + GUI 6/6; 294 AST-/Präfix-
  Vergleiche, drei beschädigte Varianten erkannt. Padding-Testfehler korrigiert,
  Appbytes über Phasen gleich; Kurzinfo-Prüfer weist alte Frist am tatsächlichen
  Modell ab. Gehosteter Quellnachlauf `7a3e7950` [ARM/Intel je Unit 6/6 + GUI 8/8,
  beide Originalarchive geprüft](verification/signature-declarations-macos-2026-11-05.json).
  **C-Projektion, Auflösung, sichtbarer HUD, Gesamt-/physische und Performance-Abnahme offen.**
- [C-/Physim-Aufrufziele](verification/call-targets-2026-11-05.md): Windows/Linux
  je Unit 9/9 + GUI 6/6; 36 exakte Namens-/Empfängerfälle und beide vorigen
  Sprachpfade konkret abgewiesen. 887 Cachevergleiche umfassen neue Metadaten.
  **Nur Datengrundlage: Signatur-/Deklarationsauflösung, sichtbarer Parameter-HUD
  noch offen.** Gehosteter Nachlauf `c9b9c00f`: [ARM Unit 4/4 + GUI 8/8,
  Intel Unit 4/4 + GUI 7/8](verification/call-targets-macos-2026-11-05.json).
  Intel-Kurzinfo-Prüfer scheitert bei der Frame-Abtastung einer kurzen Frist;
  Korrektur/Nachlauf separat verfolgen, keine grüne gesamte Mac-Abnahme.
- [C-Syntaxwiederverwendung](verification/completion-reuse-2026-11-05.md): Windows/Linux
  je Unit 8/8 + GUI 6/6, danach verstärkter C-Test je 1/1 ohne Appänderung.
  887 Frischvergleiche, 7/7 Cursor-/87/87 Edit-Speicherfehler, zwei bewusst falsche
  Cacheidentitäten abgewiesen. Liste ausblenden/Cache erneut öffnen nativ C/Physim
  über je drei Ziele belegt. Sieben Cursor-Allokationen; große Edits und zuverlässige
  16,7-ms-Marke weiter offen. Gehosteter Quellnachlauf `9292b67e`,
  [Intel/ARM je 8/8 mit Originalarchiven](verification/completion-reuse-macos-2026-11-05.json).
  **Gesamte/physische/Performance- und neue Mac-Allokationsfehler-Abnahme fehlt.**
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
