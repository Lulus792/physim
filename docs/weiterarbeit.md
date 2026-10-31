# Wiedereinstieg in die native UI-Arbeit

Stand: 31. Oktober 2026. Diese kurze Übergabe vor neuen Untersuchungen lesen und
mit `git status` sowie den relevanten Diffs abgleichen. Designverträge stehen in
[AGENTS.md](../AGENTS.md) und der [Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Bei Widersprüchen zuerst Quellstand, Datum und betroffenen Umfang abgleichen.

## Letzter abgeschlossener Abschnitt

Die Workspace-Menüportierung ist abgeschlossen: Dateitab-Menüs über AppKit,
Projektmanager per nativer Menü-Tastatureingabe und Einstellungen über Cmd+Komma.
Lauf `41147957905` auf `7a95c8ae` besteht auf Intel und Apple Silicon je **4/4**.
Jede Architektur hat alle 34 Projekt-/Sprachkombinationen bei 16 und 22 px
tatsächlich gebaut, aufgezeichnet/importiert und analysiert: je 68 Projekte.
[Kompakter Nachweis](verification/macos-workspace-menu-2026-10-31.json).
Die frühere Editorregression `41147445304` besteht je 4/4 und die Menüregression
`41147445297` je 7/7; ihre tatsächlichen Ergebnisarchive sind hashgeprüft gesichert.

Auch Windows Release und Linux Debug ASan/UBSan bestehen die Matrizen und die
Fehlerfälle. Die ersten lokalen Gesamtläufe `run-ch7e30mf`/`run-_qma9a32` bleiben
korrekt **3/4 fehlgeschlagen**, weil der Python-Prüfer nach sieben Umschaltungen
noch Flags 32 statt der tatsächlichen 96 erwartete. Seit Protokoll C startet der
Standard geschlossen mit 31; 31 xor 127 ergibt 96. Beide gespeicherten Dateien
sind 312 Bytes groß und haben SHA256
`4195990619af103cce0329845a7ca8603c406bcf4a6dec79be6340e6571d6ab4`.
`cc267a6d` korrigiert nur die Erwartung. Der Nachtest besteht auf beiden Plattformen
mit identischer App-Binärdatei, inklusive Schreiben/Lesen/Abbrechen; die langen
Matrizen wurden dafür nicht wiederholt. Der verifizierte Python-Overlay-Helfer
veränderte den gemeinsamen Quellbaum nicht. Ein erster Helferaufruf scheiterte
vor App-Start am fehlenden Arbeitsordner; dieser separate Fehler ist erhalten,
der korrigierte Helfer lief als `corrected-settings-r2` erfolgreich.

**Alle lokalen Sessions/Builds/Apps/Archivläufe dieses Abschnitts sind beendet.**
`mac-workspace-menu-first` sichert den ursprünglichen Stand `fe54e7a0`.
Neuester eingefrorener Anschluss ist **`mac-workspace-menu-accepted` mit 119
eigenen Eingaben, 1098 gegen `7a95c8ae` abgeglichenen Compiler-/Testdateien,
200 Windows-/103 Linux-Runtimedateien und zurückgelesenen Archivbytes**.
Nur die geprüfte Python-Erwartung änderte sich zwischen diesen Quellkopien;
die App-Runtimes blieben gleich. Diesen Anschluss vor weiteren Änderungen nutzen.
Die überholten Workspace-CI-Läufe `41147445351`/`41147732814` sind bestätigt
abgebrochen, keine positiven Nachweise.

40 entbehrliche Rohbilder wurden nach Abschluss entfernt (116.741.520 Bytes).
Logs, echte Einstellungen, erzeugte Quellen/Laufdaten und Runtimes bleiben
erhalten; offene F1-Fehlerarchive wurden nicht bereinigt. Keine Matrix oder
Protokoll-Trennlinie allein wegen eines Kontextwechsels erneut prüfen.

## Nächster konkreter Schritt

**Gehosteten F1-Lauf `41153551967` auf `de658612` verfolgen; danach übrige aktive
Menütests portieren.** Die fünf Ursachen sind korrigiert und in beide Repositories
gepusht. Windows Release und Linux Debug ASan/UBSan bestehen je **5/5 GUI-Fälle**
(`font_bank_ui`, `documentation_keyboard`, `documentation_math`,
`documentation_panes`, `documentation_h`) sowie separat **1/1 `font_sources`**.
Die beiden absichtlich fehlerhaften Grafikvarianten werden auf beiden Plattformen
gezielt abgelehnt. [Kompakter Nachweis](verification/f1-repairs-2026-10-31.json).
`font_sources` gehört zu `--test`, nicht zu `--test-display`; das gemeinsame
Filterargument im ersten GUI-Aufruf führte deshalb korrekt nur fünf Fälle aus.
Der separate portable Nachlauf ist tatsächlich bestanden, keine angenommene sechste
GUI-Prüfung. Diese lokalen Fälle nicht wegen eines Kontextwechsels neu starten.

- U+2318 (⌘) ist jetzt in allen fünf UI-/Code-/Fallback-Bereichen enthalten.
  Der schon vorher mit positiver und negativer Rasterprobe bestätigte Testinput
  `tests/test_font_sources.c` ist ausdrücklich im neuen Quellsnapshot enthalten.
- Der Fontbank-Test prüft nach injiziertem GL-Fehler die vorherige lebende
  Nichtnull-Textur und Unpack-Ausrichtung 8. Der Renderer blieb unverändert.
  `tools/check_font_bank_state_counterexample.py` baut isoliert je eine falsche
  Bindungs-/Unpack-Wiederherstellung; beide treffen unter Windows/Linux exakt
  ihre eigene Assertion mit Exitcode 1. Das ist kein Nachweis für GPU-Erschöpfung
  oder Leckfreiheit der absichtlich früh beendeten Negativprozesse.
- F1-Tastaturbefehl, native Suche und Aktionspopover verwenden dieselbe
  Öffnungstransaktion einschließlich Suchinsel und Tastaturfokus.
  Die Mac-Feldfixture öffnet sie jetzt mit echtem AppKit-Cmd+F und behält alle
  Unicode-/Auswahl-/Caret-/Reveal-/Hidden-Prüfungen. Die neue gehostete Gegenprobe
  setzt ausschließlich den alten nativen Handler in einer isolierten Include-Kette
  zurück und verlangt genau den Cmd+F-Öffnungsfehler; ihre Ausführung ist noch offen.
- H nutzt für Kontextsteuerung und Suchinsel-Schließen echte F6-Wechsel und
  kurze Tab-/Umschalt+Tab-Wege. Keine direkte Fokuszuweisung; der interne
  H-Watchdog wurde nicht erhöht. Alle 56 Geometriezustände bleiben erhalten.
- Die Bereichsprüfung behält alle 24 Zustände; nur ihr interner Watchdog liegt
  jetzt bei 70 Sekunden innerhalb des bisherigen äußeren 75-Sekunden-Budgets.

Lokale GUI-Ergebnisse: Windows `run-wwb7ojfa`, Linux `run-wtzsvybh`;
portable Ergebnisse: Windows `run-gtln9on5`, Linux `run-i1exa7dx`.
Alle vier lokalen Build-/Test-Sessions sind beendet. Der neue eingefrorene
Anschluss heißt **`f1-repairs-accepted-local`**; er ersetzt für neue Änderungen
`mac-workspace-menu-accepted`, ohne dessen Belege zu verändern.
1100 Compiler-/Testdateien entsprechen nach beidseitiger Zeilenendennormalisierung
`de658612`; 212 Windows-/111 Linux-Runtimedateien und alle referenzierten Archivbytes
sind zurückgelesen. Der Git-Blob des libunibreak-Manifests enthält selbst CRLF;
dieser Metadatenunterschied ist im Nachweis ausdrücklich eingegrenzt.
228 entbehrliche BMP-Rohbilder der beiden erfolgreichen GUI-Läufe wurden entfernt
(491.327.304 Bytes). Das tatsächlich betrachtete 32-px-Formelbild bleibt als ein
PNG unter dem Kampagnenordner; Logs, App-Schritte, Quellen, Runtimes und die alten
offenen Mac-Fehlerarchive sind erhalten. Diese enge Sichtprüfung ersetzt keine
gesamte Designabnahme.

Der gezielte Mac-Lauf war zuletzt eingereiht, Jobs **`124395257321` (ARM)** und
**`124395257512` (Intel)**. Vorhandene Handles verwenden, keine zweite Kampagne
wegen Wartezeit starten. Erst der tatsächliche Abschluss bestätigt die native
Cmd+F-Gegenprobe und die reparierte AX-Feldprüfung. Der Nutzer-Mac ist weiterhin
aus und bleibt unangetastet. Automatisch ausgelöste breitere CI-Läufe getrennt
bewerten; aus den lokalen Teilprüfungen kein Gesamtgrün ableiten.

Die unveränderte Mac-Baseline `41146212249` (`c35fc39d`: Intel 22/27, ARM 23/27,
je 19/19 portable Fälle) und ihre ZIPs/Logs bleiben erhalten.
[Ursprüngliche Diagnose](verification/macos-f1-open-findings-2026-10-31.json).
Die dort unabhängig bestandene externe Lesetextprüfung bleibt von der reparierten
in-process Feldfixture getrennt. Die drei dort übersprungenen späteren
Renderer-Gegenproben weiterhin nicht als ausgeführt zählen.

Dokument-Neuladen, Vervollständigung und API-Referenz sind nun ebenfalls
abgeschlossen: `c35fc39d`, Mac-Lauf `41146212373`, Intel/Apple Silicon je **4/4**
registrierte Fälle mit je sechs App-Unterprozessen. Windows Release und Linux
Debug ASan/UBSan bestehen dieselben vier Fälle. Beide Sprachen, alle drei
Editorkontexte, Text-/Undo-/Dateierhaltung und tatsächliche native Menübefehle
sind belegt. [Nachweis](verification/macos-editor-menu-2026-10-31.json).
Der geänderte gemeinsame Testhelfer löste zusätzlich Lauf `41146212246` aus;
die vorhandene Menüregression besteht auf demselben Stand je **7/7**.
Auch diese Fälle nicht bei bloßem Kontextwechsel wiederholen.

Andere automatisch ausgelöste CI-Läufe auf `c35fc39d` sind separat:
F1 `41146212249` ist mit den oben getrennt dokumentierten Befunden beendet.
C17 `41146212393` und Linux-Paket `41146212247` sind bestätigt abgebrochen;
Linux-UI `41146212248` und der überholte F1-Lauf `41147445310` sind ebenfalls
bestätigt abgebrochen. Diese Läufe sind keine positiven Nachweise.
Diese vorhandenen Handles bei Bedarf prüfen, nicht wegen einer Beobachtungspause
neu starten. Die grünen gezielten Menüläufe bestätigen keine gesamte CI.

Die fünf Toolbar-/Tastaturfälle sind jetzt abgeschlossen: Auf `8d75cee7`
besteht Lauf `41144579429` auf Intel und Apple Silicon jeweils **7/7**
einschließlich Menüadapter und App-Start.
[Kompakter Nachweis](verification/macos-menu-workflows-2026-10-31.json).
Getestet sind echte AppKit-Eingaben, Menüauswahl/Abbruch, Fokus und Speichern,
unabhängiges Projektfenster sowie die echten Fenstersteuerungsknöpfe.
Die Produktionsmenülogik wurde dabei nicht verändert. Der Test wartet nach
Größenwechseln auf tatsächlich gezeichnete Kopfgeometrie; frühe Prozessenden
zählen ohne Abschlussmarkierung nicht mehr als bestanden. Windows/Linux
bestehen je zwei klassische Menüabläufe; die vorhandene Dateien-Aktion ist
in den bisherigen Tastaturerwartungen jetzt berücksichtigt.
Diese abgeschlossene Teilmenge nicht wegen eines Kontextwechsels wiederholen.

Aktive Batch-/Serien-/Build-/Workspace-Katalog- und Typografie-/Bereichsfokusabläufe
verwenden teilweise weiterhin die alten
In-Window-Menüflächen oder erwarten das eigene F10-Menü. Diese Erwartungen
passen nach der ausdrücklich gewünschten Verschiebung nicht mehr zu macOS;
keinen alten Menü-Fallback ausschließlich zum Bestehen dieser Tests aktivieren.
Das vorher fehlerhafte Dokumentationsanker-Gate ist inzwischen gezielt behoben
und unten als eigener Teilabschluss belegt.

**Vor weiteren Portierungen den registrierten Einstieg prüfen:**
`docking_workflow` und `inspector_workflow` rufen bereits `fixed_panels` auf.
Die alten freien Topologieübungen in `docking_tests.inc`/`inspector_tests.inc`
sind historische Fixtures, keine aktuelle Anforderung. Freies internes Docking
nicht wiederherstellen und diese alten Flächen nicht als offene Mac-Portierung
zählen. ADR 009 verlangt feste Plätze mit echten Zusatzfenstern.

Die Systemmenüleiste selbst nach [ADR 024](decisions/024-native-macos-menubar.md)
ist implementiert und gezielt nativ geprüft: Datei/Bearbeiten/Ansicht/Build/Hilfe
über AppKit, Einstellungen im App-Menü, gemeinsame Befehle und Zustandsprüfung.
Auf `517021f0` bestehen Intel und Apple Silicon je zwei Fälle: echter
AppKit-Menüadapter und gestartete App mit entfernten In-Window-Menüknöpfen.
Windows Release und Linux Debug ASan/UBSan bestehen je vier Menü-/Headerfälle
plus App-Start; der Testkatalog ist geprüft. Diese grünen Kampagnen bei bloßem
Kontextwechsel nicht wiederholen. Manuelle Bedienung/VoiceOver/Vollbild sowie die
gesamte Mac-GUI-Suite bleiben gesondert offen. Der Nutzer hat am 31.10.
ausdrücklich gemeldet: **Der Mac ist aus.** Ohne neue Online-Meldung nicht
ansprechen; der abgeschlossene Lauf verwendete ausschließlich GitHub-Runner.
ARM `toolbar_large` startete dort tatsächlich mit 1080×740; nur Intel erreichte
1440×940. Die beiden ARM-Varianten belegen keine getrennte Großbildabnahme.

Weitere verbindliche Arbeit umfasst A2 in der Szene, Mehrfachauswahl,
Kontextmenüs A, Completion D mit Parameterhilfe und die übrige native Oberfläche.
Freigabe und Implementierungs-/Plattformnachweis getrennt halten.

Physim verlinkt AppKit bereits; die bestehende Accessibility-Brücke nutzt
typisierte Objective-C-Runtime-Aufrufe aus C17. Offizielle Apple-Quellen und der
enge Nutzerumfang sind in ADR 024 festgehalten. Vollbild-/Menüleistenverhalten
macOS verwalten lassen. [Kompakter Nachweis](verification/macos-menubar-2026-10-30.json)
nennt tatsächliche lokale/gehostete Prüfungen und verbleibende Prüflücken.

**Dokumentationsanker-Gate behoben auf `ed3866ed`:** Die ursprünglichen Läufe
`41126070397`/`41126070358` scheiterten an neun nicht erreichbaren Themenzielen.
Die saubere Git-Gegenprobe bestätigt genau diese neun Fehler. Acht Entwickler-
und Designverweise sind jetzt ausdrücklich externe, revisionsfeste Repository-
Quellen; F1 kopiert ihre Adresse. libunibreak-Herkunft und beide Lizenztexte sind
als native Offline-Themen registriert. Der SDK-Installer erhält ihre Pfade auch
außerhalb von `docs/` und überschreibt seine angepasste Startseite nicht.
Der Prüfer bleibt strikt; fehlende Dateien liefern nun gezielte Fehler statt
Python-Tracebacks. Windows/Linux bestehen 188 Themen und 234 Abschnittslinks,
Paket-Gegenprobe/Umzug und je zwei echte F1-Fälle (16/22 px). Der gehostete
Mac-Lauf `41136506871` besteht auf Intel und Apple Silicon je **4/4**.
[Nachweis und Grenzen](verification/documentation-links-2026-10-30.json).
Diese gezielten Prüfungen ohne relevante Änderung nicht erneut beginnen.

Die uncommittete fremde Ergänzung in `docs/status.md` besitzt separat einen
relativen Link zum noch nicht registrierten Bereinigungsbericht vom 22.10.
Sie bleibt unverändert. Deshalb wurde der Git-Quellstand mit ausschließlich
eigenen Änderungen isoliert geprüft; kein pauschales Grün für fremdes WIP oder
die gesamte CI ableiten. Die alten Fehlerprotokolle unter
`build/validation/tree-selection-a2-2026-10-30/ci-*.log` bleiben erhalten.

## Abgeschlossen – nicht aus Kontextverlust wiederholen

| Teil | Ergebnis und Nachweis | Verbleibende Grenze |
| --- | --- | --- |
| Mac-Dateitabs/Projektmanager/Einstellungen | `7a95c8ae`: Intel/Apple Silicon je 4/4 mit je 68 tatsächlich gebauten/analysierten Projekten; Windows/Linux gleiche Abdeckung nach separat bestandenem Python-Prüfernachlauf mit unveränderter App. [Nachweis](verification/macos-workspace-menu-2026-10-31.json). | Breitere F1-Befunde, übrige aktive Menütests und gesamte GUI-/physische Abnahme bleiben offen. |
| Mac-Dokument-/Completion-/API-Menüeingaben | `c35fc39d`: Intel/Apple Silicon, Windows Release und Linux Debug ASan/UBSan je 4/4 registrierte Fälle / sechs App-Unterprozesse. Gleicher Mac-Stand zusätzlich je 7/7 Menüregression. [Nachweis](verification/macos-editor-menu-2026-10-31.json). | Gesamte GUI-/Design-/VoiceOver-/physische DPI-Abnahme und andere aktive historische Menütests offen. |
| Historische Mac-Toolbar-/Tastaturabläufe | `8d75cee7`: Intel/Apple Silicon je 7/7 einschließlich Menüadapter und App-Start; Windows Release/Linux Debug ASan/UBSan je 2/2. [Nachweis](verification/macos-menu-workflows-2026-10-31.json). | Übrige historische Workflows, manuelle VoiceOver-/Vollbildprüfung und physische Großbild-/DPI-Abnahme offen; ARM-Großvariante hatte 1080×740. |
| F1-Link-Gate und Offline-Lizenzen im SDK | `ed3866ed`: 188 Themen/234 Abschnittslinks unter Windows/Linux; Herkunft und beide Lizenzen tatsächlich per F1-Tastatur geöffnet, Quellenadresse kopiert; SDK-Umzug mit fehlenden Dateien als Gegenprobe. Intel/Apple Silicon je 4/4. [Nachweis](verification/documentation-links-2026-10-30.json). | Gesamte GUI-/Inhalts-/Screenreader-Abnahme bleibt offen; fremde Status-Ergänzung nicht mit geprüft. |
| macOS-Systemmenüleiste | `d43dcd6c` + `517021f0`; je 2/2 native Fälle Intel/Apple Silicon, je 4/4 Menü-/Headerfälle plus Start unter Windows/Linux. [Kompakter Nachweis](verification/macos-menubar-2026-10-30.json). | Manuelle Vollbild-/VoiceOver-Prüfung und übrige historische Mac-GUI-Workflows offen; Toolbar/Tastatur siehe eigene Zeile. |
| Trennlinie oberhalb des Protokolls | **Behoben seit `ef1f3321`.** Dauerhaft 1 px, gemeinsame Konturfarbe; Akzent bei Hover/Fokus. [Ursprüngliche Korrektur und negative Kontrolle](verification/protocol-separator-2026-10-29.json). [Nachlauf 30.10.](verification/protocol-boundary-workspaces-2026-10-30.json): Windows Release und Linux Debug ASan/UBSan, je 12/12 Zustände in Entwickeln, Simulieren, Auswerten. | Große Schrift im Nachlauf: 22 px; keine neue macOS-/DPI-/Screenreader-Abnahme. |
| A2 für Textauswahl | Gemeinsamer Textfeldrenderer, Commit `c12e2a10`. [Umfang und native Belege](visuals/unified-workspace/TEXT-SELECTION-A2-NATIVE.md). | Belegt keine vollständige A2-Umsetzung aller Objektarten. |
| A2 für Protokoll- und Explorerlisten/-kacheln | Commit `d228b16e`. [Native Umsetzung](visuals/unified-workspace/LIST-SELECTION-A2-NATIVE.md), [Windows-/Linux-Prüfungen und sechs native Gegenproben](verification/list-selection-a2-2026-10-30.json). | Mehrfachauswahl bleibt offen. |
| A2 für Workspace- und Szenenbaum | [Umsetzung](visuals/unified-workspace/TREE-SELECTION-A2-NATIVE.md), [Nachweis 30.10.](verification/tree-selection-a2-2026-10-30.json): je zehn tatsächliche Zustände Windows/Linux, sechs native Gegenproben, zusätzlich 72 Linux-Pendellaufansichten. | Einzelauswahl; 32 px sind eine gezielte Rendererbelastung, keine neue Einstellungsoption oder reale 200%-DPI-Abnahme. |
| F1: ungültige synthetische Umschalttastenereignisse unter Linux | Commit `8840b9c9`: SDL-Ereignisse ausdrücklich initialisiert; sieben Linux-Kontextfälle bestanden. | Eigenständige Windows-Fokusbefunde bleiben offen; nicht als gleicher Fehler behandeln. |

Der Trennlinien-Nachlauf war hier eine unnötige Wiederholung eines bereits
behobenen Befunds. Er brachte zusätzliche Bereichsabdeckung, aber keine neue
Darstellungskorrektur. **Nicht noch einmal als neue Reparatur beginnen.**

## Wann erneut prüfen?

- Relevante Änderungen am Renderer, an Palette/Layout oder am betroffenen
  Eingabeweg: passende Regression ausführen und den konkreten Anlass nennen.
- Neuer konkreter Fehlerbeleg: zuerst betroffene Version und Zustand mit dem
  vorhandenen Nachweis vergleichen; nur die offene Abweichung reproduzieren.
- Eine bisher unbelegte Plattform/Eingabe-/Darstellungsdimension: gezielt diese
  Dimension ergänzen und bestehende Ergebnisse ihrer Revision zugeordnet lassen.

Ein neuer Chat oder eine Kontextkompaktierung genügt als Anlass nicht. Vor jedem
neuen Lauf kurz festhalten: **Was hat sich geändert, welche Frage ist offen und
welcher kleinste Test beantwortet sie?**

## Lokaler technischer Anschluss

Der letzte A2-Stand heißt `tree-a2-final`. Alle zugehörigen Builds,
nativen Tests, Gegenproben und Archivprüfungen sind beendet. Die abschließenden
Baumtests `tree-a2-fifth` bestehen auf Windows/Linux, die sechs Gegenproben in
`tree-a2-counter-r3` ebenfalls. Die finalen Quellen unterscheiden sich davon nur
durch die geprüfte Python-Gegenproben-Harnesskorrektur, nicht durch App-Code.

Generierte Manifeste, Archive und Kommandoprotokolle:
`build/validation/explorer-inline-2026-10-29/tree-a2-final-*` und
`build/validation/tree-selection-a2-2026-10-30/`. Der gemeinsame isolierte
Compilerbaum liegt unter `build/validation/protocol-wrap-2026-10-26/3-isolated/`.
Für dessen nächste Änderung das neue Manifest verwenden, nicht den älteren
`list-a2-final`- oder `protocol-boundary-final`-Lauf. Der letzte Eingabebestand
umfasst 101 Dateien. Der neue Menüstand `mac-menu-shell` erfasst 105 Eingaben
einschließlich Adapter-/Testdateien. Seine Windows-/Linux-Läufe sind beendet;
Manifest-/Archivnachweise liegen im gleichen Kampagnenordner.
Der frühere Anschluss `doc-links-final` hat 112 Eingaben und 1097 gegen Git
abgeglichenen Compiler-/Testdateien, aufgebaut auf `mac-menu-shell`.
Die F1-Läufe `doc-links-first` sind Windows/Linux je 2/2 bestanden; danach änderte
sich nur der Mac-Menüabsatz in `design-direction.md`, kein Compilerinput oder
App-Runtime. Alle lokalen Builds, Textprüfungen und Archivläufe sind beendet.
Der vorherige Anschluss `mac-menu-workflows-accepted` hat 114 Eingaben, 1098
gegen Git-Quellstand `8d75cee7` abgeglichene Compiler-/Testdateien, 200
Windows-/103 Linux-Runtimedateien; sämtliche Archivbytes zurückgelesen.
Die lokalen Läufe `mac-menu-workflows-second` sind je 2/2 bestanden.
Danach änderte sich nur das komplett mit `__APPLE__` geschützte Testmodul;
echte MSVC-/GCC-Präprozessorläufe bestätigen dessen Ausschluss unter
Windows/Linux. Die Runtimes bleiben unverändert. Alle lokalen Builds,
Präprozessor- und Archivläufe sind beendet.
Vorheriger Anschluss: `mac-editor-menu-first`, 117 eigene Eingaben, 1098 gegen
`c35fc39d` abgeglichene Compiler-/Testdateien, 200 Windows-/103 Linux-Runtimes.
Windows `run-0_069fqq` und Linux `run-5xfqg2vv` bestehen je 4/4. Alle lokalen
Builds, App-Prozesse, Tests und Archivläufe sind beendet. Die sechs geänderten
Compiler-/Testquellen und alle referenzierten Runtime-Archivbytes wurden
zurückgelesen. Neueren Anschluss `mac-workspace-menu-accepted` oben verwenden.
A2-/Protokolltests ohne relevante Änderung weiterhin nicht wiederholen.

Nach dieser Editorprüfrunde wurden 40 erfolgreiche Rohbilder entfernt
(143.181.840 Bytes). Ergebnisberichte, Referenzen, Quellen und Runtimes bleiben
erhalten; vier zusätzliche Mac-ZIPs sind gegen GitHub-Artefaktdigests geprüft.
Sie liegen zusammen mit dem Textbericht unter
`build/validation/macos-menu-workflows-2026-10-30/`.

Die neue Menüprüfrunde entfernte 32 erfolgreiche Rohbilder (116.589.376 Bytes).
Fehlerbelege und bestehende Referenzen bleiben erhalten; Mac-Ergebnisse bestehen
aus kompakten JSON-Berichten. Der Workflow bewahrt sie 14 Tage auf; lokale,
hashgeprüfte ZIP-Kopien liegen unter `build/validation/macos-menu-workflows-2026-10-30/`.
SDL 3.2.30 wird in CI je Runner-Image/Architektur zwischengespeichert; beide
abschließenden Jobs haben diesen Cache tatsächlich wiederhergestellt.

Die 16 neuen erfolgreichen F1-Rohbilder wurden nach bestandener Zustandsprüfung
entfernt (26.267.808 Bytes); Textberichte und Originalreferenzen bleiben erhalten.
Eine zusätzliche Bereinigung von Compilerobjekten wurde automatisch mit
`blocked by policy` abgelehnt, auch der engere Einzeldateiversuch. Es wurden
keine Objekte entfernt: 64 `.obj`-Dateien / 6.103.831 Bytes sind bestätigt erhalten.
Die Löschung nicht über eine andere Shell oder Kodierung umgehen.

Parallele lokale AT-SPI-/Orca-, Workflow- und Statusänderungen getrennt erhalten;
nur eigene geprüfte Dateien committen. Alles bleibt auf D:, F: nicht berühren.

Für spätere Mac-Tests verweist der Nutzer ausdrücklich auf NexaCloud und NexaUI:
`D:/NexaCloud/docs/MACOS_TESTING.md` sowie die aktuellen Mac-Hinweise in
`D:/NexaUI/AGENTS.md` zuerst lesen. Eigene Remote-Reste nach gesicherter,
hashgeprüfter Belegkopie gezielt aufräumen; gemeinsame Werkzeuge erhalten.
Der Nutzer hat am 31.10. direkt bestätigt, dass sein Mac jetzt aus ist.
Deshalb keine SSH-/Probe auf dem Nutzer-Mac ohne neue Verfügbarkeitsmeldung.
Die hier belegten neuen Mac-Tests liefen auf gehosteten GitHub-Runnern.
