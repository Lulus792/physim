# Wiedereinstieg in die native UI-Arbeit

Stand: 31. Oktober 2026. Zuerst diese Übergabe, `git status` und relevante Diffs
abgleichen. Designverträge stehen in [AGENTS.md](../AGENTS.md) und der
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Kompaktierung ist kein
Anlass, geschlossene Prüfungen oder die Protokoll-Trennlinie erneut zu beginnen.

## Aktueller Abschluss: eigene Anzeigeeinheitenaufgabe

**Neuester eingefrorener Anschluss: `units-window-caller`, 151 eigene Eingaben.**
**`657219a3` ist in beide Repositories gepusht.** Alle 1.109 erfassten
Compiler-/Testdateien entsprechen dem Commit direkt oder nach Zeilenendennormalisierung.
[Commitabgleich und offener CI-Snapshot](verification/units-window-commit-2026-10-31.json).
Die alte globale Einheitenüberlagerung ist entfernt. Eigene OS-/GL-/Nuklear-/
Eingabe-/AX-Oberfläche, reale Kanäle, erhaltene Entwürfe je Kanal, tatsächliche
Symbol-/Faktoreingabe, Vorschau, sichere unabhängige Übernahme und explizite
Auftraggeberzuordnung sind umgesetzt. Hauptfenster und andere Aufgaben bleiben
bedienbar. SI ist zunächst Entwurf; Anwenden erhält wie in der Einheitenreferenz
die Aufgabe und andere Kanalentwürfe. Allgemeines Settings-Abbrechen schließt
die Aufgabe nicht. [Umsetzung](visuals/unified-workspace/UNIT-TASK-NATIVE.md),
[Nachweis](verification/units-window-2026-10-31.md).

Windows Release/Linux Debug ASan/UBSan: `units-window-focus` je **5/5**,
`run-tp2w0fqh` / `run-flt4us81`. Fensterablauf bei UI 16/22, beide Projektsprachen,
Analyse dunkel/hell-groß-schmal und Inspektor mit beiden Fontquellen bestanden.
Tatsächliche Settings-Kategorie/Launcher, Parent-Abbruch, Quellen-/Dateibesitz,
echte Popup-Pfeile/Aktivierung/Tab/Escape, Schreibfehler/Wiederholung,
Datensatzüberlauf außerhalb der Vorschau, OS-Schließen und drei Frames echter
SDL-Tastaturfokus über weiterlaufendem Hauptfenster sind geprüft.
Bei 360×360/UI 22 bleibt der 79,5-Pixel-Ausschnitt für den größeren Fehlerblock
stabil: Windows 101,75 / Linux 126,5 Pixel Nachricht, keine wechselnde automatische
Ausrichtung an beiden Enden. Kompakte Textbelege statt weiterer Vollbilder.

Anschließend wurde die vermeidbar große Anfangshöhe auf die echte 850×473-
Referenz bei Standardschrift korrigiert und mit der Schrift skaliert. Dafür
nur `unit_task_window` nachprüfen; **keinen neuen gesamten Fünfer-/Sechserlauf
mit der letzten Binärdatei behaupten**. `units-window-reference` besteht je 1/1.
Der letzte Anschluss `units-window-caller` erhält den tatsächlichen GL-Kontext
des Aufrufers: Mausaktivierung mitten im Settings-Renderer und Tastaturaufruf
zwischen Frames wurden getrennt ausgeführt. Die Laufzeitspur prüft Fenster-ID
und Kontextidentität vor/nach dem Öffnen. Auch dieser Nachlauf besteht je 1/1.
Der Abschluss mit Lauf-IDs und Hashes steht im obigen JSON.
Alle lokalen Build-/Prüf-/Vergleichs-/Archivhandles sind beendet; alle referenzierten
Archivbytes zurückgelesen. Keine isolierten Bäume aus einem älteren Stand ersetzen.

Ursprüngliche Fehlphasen bleiben erhalten: erster Build mit Typ-/Funktions-
Namenskollision; alter Main-F7-Test löste im absichtlich bedienbaren Hauptfenster
einen weiteren Lauf aus; Windows-Inspektorfixture verlangte ein unmögliches ganzes
Kartenscrollziel. Der archivierte Vorgänger scheitert identisch. Spätere Fixtures
zählten dekorative Kartensichtbarkeit oder forderten ein PNG vor einer Tab-Warte-
schleife mehrfach an. Encoder und Mindestabdeckung wurden nicht gelockert.
Die aktuelle Rasterprüfung zählt reale vollständig sichtbare Labels+Eingaben
und die exponierte Kontur: Windows 12/8, Linux 21/10 Felder/Werte je Fontquelle.
315 entbehrliche Dateien entfernt, 563.430.107 Bytes: 100 erfolgreiche Roh-BMPs
und 215 nur zum Vergleich restaurierte, weiterhin archivierte Runtime-Dateien.
Fehler-/PNG-Referenzen, Quellen, Archive und benötigte Programme/Symbole erhalten.

## Bereits geschlossener Settings-/Mac-Anschluss

Einstellungen A ist seit `479a8954` ein eigenes natives Kategorienfenster,
kein Tab 6-Formular. Fünf Kategorien, echte Suche, nur im Entwurf veränderte
Vorschau, vollständige dreispaltige Prüfung und sichere feldweise Transaktion.
[Umsetzung](visuals/unified-workspace/SETTINGS-A-NATIVE.md),
[lokaler Nachweis](verification/settings-a-window-2026-10-31.md).
`settings-a-window-ready` je 7/7, anschließend gezielte Bildkorrektur je 1/1.
Diese älteren Ergebnisse nicht als neue Gesamtmatrix darstellen.

Nutzer-Mac **aus**, niemals ansprechen. Gehostete Ergebnisse sind abgeschlossen:
Settings-/Typografie `41170676474` auf `479a8954`: ARM `124447773891` und
Intel `124447773821` je 9/9. [Verifizierte Archive/Einzelfälle](verification/settings-a-macos-2026-10-31.json).
Die separate Menüregression `41170676487` blieb zunächst je 4/7: Stufe 12 erwartete
Entwickeln nach Settings aus Auswerten. `140ead03` korrigiert exakt die Fixture.
Nachlauf `41171432257`, ARM `124450544037` / Intel `124450543920` je 7/7, beide Artefakte,
Digests und alle Ergebnisdateien gelesen. [Nachweis](verification/macos-settings-menu-accepted-2026-10-31.json).
Diese Fälle sind geschlossen; nicht allein wegen Kompaktierung neu starten.
Der neue Einheitenfenster-Stand braucht seinen eigenen tatsächlichen gehosteten
macOS-Nachweis nach Push; eine Workflowdatei ist kein Pass. Kein physischer Mac
für diese CI nötig. Der vor diesem Unit-Commit letzte Dokumentationscommit war
1bb07db5, in beide Repositories gepusht. Der neue tatsächliche Lauf auf
`657219a3` ist `41177844214`: ARM `124468995798`, Intel `124468995912`;
Menüregression `41177844185`: ARM `124468995758`, Intel `124468995948`.
Beim Snapshot laufen ARM-Builds, Intel wartet; **kein neues Mac-Pass behaupten**.
Diese bestehenden Handles weiter beobachten, keine Ersatzläufe starten.

## Nächste konkrete Arbeit

- Einstellungen A: obere Suchlöschung und Rücksetzen im leeren Ergebnis teilen
  Fokus-ID 26. Eindeutige IDs und Eigentümer, sichtbare Tabfolge/kein Treffer
  gezielt umsetzen und prüfen. Nicht die gesamte geschlossene Matrix wiederholen.
- Completion D: tatsächliche Backend-/Preferences-Optionen für Auto-Anzeige und
  sichtbare Trefferzahl (Standard fünf), danach Scope-/Signaturhilfe und
  gemeinsames Syntaxschema. Keine funktionslosen Ersatzfelder.
- Historische `settings_workflow`-/AX-Fixtures von Tab 6/Radios/globalem Fokus auf
  das Aufgabenfenster portieren, Wiederherstellung/Maximieren/Geometrie und sichere
  Dateioperationen erhalten. Der alte 0/0-Protokollgriff ist kein neuer Befund
  an der seit langem geschlossenen Trennlinie.
- Einheiten-Symbolprüfung bei Unicode-Randleerzeichen gezielt prüfen: vorhandene
  `channel_units.c:text_valid` prüft äußere ASCII-Leerzeichen. Keine allgemeine
  Unicode-Abnahme aus den bisherigen Symboltests ableiten.
- Übrige native Auswahl: A2 Szene/Mehrfachauswahl, Kontextmenüs A, B2, weitere
  Aufgabenfenster nach ADR 015 sowie freigegebene UI. `docking_workflow` und
  `inspector_workflow` prüfen bereits `fixed_panels`; freie Dock-Fixtures nicht
  reaktivieren. Manuelle Screenreader-/DPI-/Monitor-/physische Eingabe-/Vollbild-
  und vollständige visuelle Abnahme bleiben offen. Keine volle Punktzahl behaupten.

Feldweise Preferences-Zusammenführung aus `fa97b315` erhalten. Theme-/Schriftentwurf
darf spätere Geometrie nicht überschreiben. Entwickeln-Ausgangswerte sind von
laufenden Simulieren-/Auswerten-Sitzungen getrennt. Explizite Protokollwerte
warten beim parallelen Übernehmen auf Entwickeln; spätere bewusste Resize-/
Sichtbarkeits-/Layoutaktionen haben Vorrang. Beim Beenden `settings_current`
verwenden, damit die andere Sitzung nicht den gespeicherten Ausgangswert ersetzt.

## Sicherer lokaler Anschluss

- `prepare-next.py <neuer-tag> units-window-caller` verwenden. Helfer prüft
  eingefrorene Quelle/Runtime/Archive. Neue Module ausdrücklich zur Eingabeliste
  hinzufügen; fremde lokale AT-SPI-/Orca-Arbeit nicht in eigene Builds mischen.
- Helfer/Manifeste/Byteindizes: `build/validation/explorer-inline-2026-10-29/`.
  `run.py`/`freeze.py` vor Verwendung auf ihre tatsächlichen Argumente prüfen.
- Isolierter Baum: `build/validation/protocol-wrap-2026-10-26/3-isolated/`
  mit `src`, `windows-build`, `linux-build`. Keine Quell-/Runtimeänderung bei
  lebendem Build-/Prüf-/Archivhandle. Quellen und neue Testprogramme versionieren.
- Unit-Kampagne: `build/validation/units-window-2026-10-31/`; Baseline-215-Dateien
  wurden nach erfolgreichem Vergleich entfernt und bleiben archiviert. Alte
  Settings-Diagnose: `build/validation/settings-window-2026-10-31/`. Debugger-
  Gegenproben dort sind Ursachenbelege und keine normalen App-Pässe.
- Historischen damaligen Settings-A-Bildbefund unverändert erhalten:
  `build/validation/macos-menu-workflows-2026-10-30/ui-22-typography-settings.png`.
  Er beschreibt nicht den aktuellen Renderer. Im vorigen Abschluss waren bereits
  656 erfolgreiche BMPs/2.093.854.368 Bytes entfernt; nicht doppelt zählen.
- Nur D:, kein Zugriff auf F:. Generierte Ausgaben unter `build/`; VM-/WSL-
  Datenträger, benötigte Offline-Abhängigkeiten, Benutzerexperimente und fremde
  aktive Arbeiten schützen. Keine Buildverzeichnisse pauschal leeren.
- Vor jedem Nachlauf konkrete Änderung/offene Frage und kleinsten passenden
  Test benennen. Text/JSON mit Quelle/Kommando/Soll-Ist/Hash zuerst, Bilder nur für
  visuelle Fragen. Bereits geschlossene Matrix nicht wegen Kontextwechsel wiederholen.

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
