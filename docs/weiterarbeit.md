# Wiedereinstieg in die native UI-Arbeit

Stand: 31. Oktober 2026. Zuerst diese Übergabe, `git status` und relevante Diffs
abgleichen. Verbindliche Designs: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Keine geschlossene
Prüfung oder Protokoll-Trennlinie allein wegen Kompaktierung erneut beginnen.

## Aktueller Anschluss: Suchaktionen und Pointerfokus

**Eingefroren: `settings-search-clipped`, 151 eigene Eingaben.**
**`b20d550e` ist in beide Repositories gepusht; 1.109 Compiler-/Testdateien
stimmen mit dem Commit überein.** [Commitabgleich und offener CI-Snapshot](verification/settings-search-commit-2026-10-31.json).
Suchkopf „Suche löschen“ und Inhaltsaktion „Suche zurücksetzen“ sind getrennt;
sichtbare Tabfolge, Rückkehr zu Suche/Texteingabe und Entwurfserhaltung umgesetzt.
Das erste Suchzeichen zeichnet kein Löschsymbol mit alter Breitenreservierung.
Verdeckte, abgescrollte Kategorien dürfen keinen Headerklick/Fokus übernehmen.
[Umfang, ursprüngliche Fehler und Nachweise](verification/settings-search-2026-10-31.md).

Windows Release/MSVC und Linux Debug/GCC ASan/UBSan: Abschluss je **2/2**
(`settings_search_keyboard`, `settings_a_window`), `run-n2nnzihq` /
`run-z7knodp0`. Je vier App-Prozesse, UI 16/dunkel und 22/hell; echte
SDL-Texte, Tab/Shift+Tab, Aktivierung und beide Pointeraktionen. UI 22 stellt
Kategorie 21 hinter den Suchkopf und bestätigt den ignorierten verdeckten Treffer.

Inspektor mit beiden Fontquellen bereits auf `settings-search-ready` bestanden,
Windows `run-_8a5pztq` / Linux `run-m17aq13q`. Gesamt dort je **2/3**, weil der
damalige neue Suchprüfer noch scheiterte. Spätere Änderungen betreffen Suche;
kein neuer gesamter Dreierlauf mit der letzten Binärdatei behauptet.
Alle lokalen Build-/Prüf-/Archivhandles beendet; referenzierte Archivbytes gelesen.
Fehlerphasen behalten: frühe Python-/CLI-Registrierung, falscher nativer Gruppen-
Parameter, verfrühte Pointerkoordinaten, danach echte verdeckte Fokusübernahme.
Nicht pauschal als Produktfehler oder bestandene Abnahme umdeuten.

## Einheitenfenster und Mac-Nachweise

Eigenes Einheitenfenster seit **`657219a3`**, Commit in origin/upstream;
1.109 erfasste Compiler-/Testdateien abgeglichen. [Umsetzung](visuals/unified-workspace/UNIT-TASK-NATIVE.md),
[lokale fünf Gruppen und gezielte Nachläufe](verification/units-window-2026-10-31.md).
Einstellungen A bleibt seit `479a8954` ein eigenes Kategorienfenster;
[Verträge und offener Umfang](visuals/unified-workspace/SETTINGS-A-NATIVE.md).
Gemeinsamen Speicher-/Geometriebesitz aus `fa97b315` erhalten; bei Beenden
`settings_current` verwenden. Allgemeines Abbrechen schließt die Einheitenaufgabe nicht.

Nutzer-Mac **aus**, niemals SSH/Erreichbarkeit/Upload/Test anstoßen.
Auf `657219a3` sind beide gehosteten Settings-/Einheitenjobs `41177844214`
beendet, **je 12/13**: Header 3/3, Typografie 1/1, kombinierte Gruppe 8/9;
portabler Preferences-Schritt übersprungen. Nur erster cm-Inspektornachweis
scheitert an Selektor „m“ bei schon aktualisiertem Katalog/Wert „cm“.
Fixture wartet nun begrenzt auf die wirkliche neue Zeichnung; Daten-/Raster-
Konsistenz und Mindestzahlen nicht gelockert. Lokal beide Fontquellen bestanden.
Mac-Nachlauf auf korrigiertem neuen Quellcommit bleibt eigens nachzuweisen.
Neue Handles auf `b20d550e`: Settings/Suche/Einheiten `41180437523`,
ARM `124476553111`, Intel `124476552980`; Menü `41180437463`,
ARM `124476553001`, Intel `124476552881`. Beim Snapshot warten die Settings-
Jobs; Intel-Menü startet. Diese Handles weiter beobachten, keine Ersatzläufe.
Separate Menüregression `41177844185` je **7/7**; alle vier ZIP-Digests und
Ergebnisdateien tatsächlich gelesen. Details im neuen Suchnachweis.
Ältere Settings-/Mac-Menümatrix (9/9 auf `479a8954`, 7/7 auf `140ead03`) ist
ebenfalls geschlossen; nicht als Abnahme des späteren Quellstands ausgeben.

## Nächste konkrete Arbeit

- Neuen gehosteten Such-/Inspektorlauf nach Push des Quellcommits über seine tatsächlichen
  Handles weiter beobachten und Artefakte/Digests/Einzelfälle lesen. Bestehende
  Läufe nicht durch Ersatzläufe überdecken. Nutzer-Mac wird dafür nicht benötigt.
- Completion D nach ADR 022/023 tatsächlich implementieren. Bestandsaudit:
  `app/completion_ui.inc` zeigt noch vier Zeilen und Vierergruppen, manuelles
  Strg+Leertaste; `ps_preferences`/Format PSPREF05 enthält noch keine Completion-
  Optionen. Auto-Anzeige, sichtbare Trefferzahl (Standard fünf), echte Scope- und
  Parameterhilfe sowie gemeinsames Syntaxschema fehlen. Kein Ersatz-Backend,
  kein neuer Dienst; Einstellungen A, Transaktion und Altdateien erhalten.
- Historische Settings-/AX-Fixtures von Tab 6/Radios/globalem Fokus portieren.
  Wiederherstellung, Maximieren, Geometrie und Dateisicherheit erhalten. Der alte
  0/0-Protokollgriff ist kein neuer Befund an der behobenen Trennlinie.
- Gleichen Clip-Vertrag gezielt beim eigenen `unit_focus` prüfen: auch dort
  existiert ein roher Bounds-Hovertest. Noch keine reproduzierte Einheiten-
  Fehlwirkung behaupten. Unicode-Randleerzeichen im Einheitensymbol separat prüfen.
- Übrige freigegebene UI: A2 Szene/Mehrfachauswahl, Kontextmenüs A, B2, weitere
  Aufgabenfenster. Manuelle Screenreader-/physische Eingabe-/DPI-/Monitor-/
  Vollbildabnahme und Gesamtgestaltung bleiben offen. Keine volle Punktzahl behaupten.

## Sicherer lokaler Anschluss

- `prepare-next.py <neuer-tag> settings-search-clipped` prüft vorher Quelle,
  Runtime und Archive. Neue Module ausdrücklich in die Eingabeliste aufnehmen.
  Eigene Quellen vollständig versionieren, fremde AT-SPI-/Orca-/Status-/dist-
  Änderungen erhalten und nicht in eigene isolierte Quellen übernehmen.
- Helfer/Indizes: `build/validation/explorer-inline-2026-10-29/`; aktueller
  Arbeitsnachweis: `build/validation/settings-search-2026-10-31/`.
  `run.py`/`freeze.py` vor Verwendung auf tatsächliche Argumente prüfen.
- Isolierter Baum `build/validation/protocol-wrap-2026-10-26/3-isolated/`
  mit `src`, `windows-build`, `linux-build`; keine Mutation bei lebendem Handle.
- Nur D:, kein Zugriff auf F:. Generiertes unter `build/`; WSL-Datenträger,
  benötigte Offline-Abhängigkeiten, Nutzerexperimente und fremde Arbeit erhalten.
  Vorige abgeschlossene Bereinigungen: Einheiten 315 Dateien/563.430.107 Bytes,
  davor Settings 656 BMPs/2.093.854.368 Bytes. Nicht doppelt zählen. Neue Such-
  Kampagne behauptet keine weitere Löschung. Fehler und benötigte Referenzen schützen.
- Text/JSON mit Quelle/Kommando/Soll-Ist zuerst; Bilder gezielt. Vier finale
  Such-PNGs für Fokusdarstellung reichen; nicht jeden Eingabeschritt fotografieren.

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
