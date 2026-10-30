# Projektarbeit

- Der lokale Projektstand ist die Grundlage für die weitere Umsetzung.
- Quellcode, Tests, Dokumentation und benötigte Assets vollständig mit Git verfolgen.
  Generierte Builds, lokale Abhängigkeiten, Testausgaben und Laufdaten bleiben ignoriert.
- Abgeschlossene, geprüfte Änderungen committen und in beide Repositories pushen:
  `origin` (Lulus792/Physim) und `upstream` (PhysicSimulator/physim).
  Diese Vorgabe ist vom Nutzer autorisiert; nicht bei jedem Push erneut fragen.
- Bestehende Remote-Historien erhalten. Keine erzwungenen Pushes ohne ausdrücklichen Auftrag.
- Linux- und macOS-Unterstützung anhand tatsächlich ausgeführter Builds und Tests bewerten.
  Eine vorhandene CI-Konfiguration allein beweist keine Funktionsgleichheit.

## Wiedereinstieg und abgeschlossene Arbeit

- Bei nativer UI-Arbeit zuerst [den kurzen Arbeitsstand](docs/weiterarbeit.md)
  lesen, besonders nach Kontextkompaktierung oder Chatwechsel; anschließend
  `git status` und relevante Diffs abgleichen.
- Abgeschlossene Befunde mit vorhandenem Nachweis nicht allein wegen eines
  Kontextwechsels erneut untersuchen, bauen oder testen. Eine Wiederholung
  braucht einen konkreten Anlass: relevante Code-/Abhängigkeitsänderung,
  neue Fehlerbelege oder eine noch offene Prüfdimension. Anlass und Umfang
  vor der Ausführung festhalten. Erforderliche Regressionsprüfungen bleiben bestehen.
- Den kurzen Arbeitsstand beim Abschluss eines Schritts aktualisieren:
  Ergebnis, Commit/Nachweis, offene Punkte und der nächste konkrete Schritt.

## Lokale macOS-Prüfungen

- Der Nutzer hat die Verlegung der bisherigen Datei-/Bearbeiten-/Ansicht-/Build-/
  Hilfe-Menüs in die echte macOS-Systemmenüleiste beauftragt. [ADR 024](docs/decisions/024-native-macos-menubar.md)
  beachten: nur diese Plattformanpassung, vorhandene Aktionen/Zustände erhalten;
  native Umsetzung und macOS-Abnahme sind noch offen.
- Vor Tests auf dem Nutzer-Mac die aktuelle Anleitung
  `D:/NexaCloud/docs/MACOS_TESTING.md` und die aktuellen Mac-Hinweise in
  `D:/NexaUI/AGENTS.md` samt verlinkten Umgebungs-/Prüfberichten lesen.
  Der Nutzer hat ausdrücklich auf diese gemeinsame Umgebung verwiesen.
  Historische Verfügbarkeitsmeldungen nicht als aktuelle Erreichbarkeit auslegen.
- Vor Remote-Zugriffen den dort zuletzt dokumentierten Verfügbarkeitszustand
  beachten; eine dokumentierte Aus-Meldung erlaubt keine SSH-/Erreichbarkeitsprobe.
- Nach Mac-Tests eigene entbehrliche Tools, Downloads, Helfer und ersetzte Ausgaben
  aufräumen. Zuerst Ergebnisse und offene Fehlerbelege nach D: sichern und Hashes
  prüfen; vor dem Löschen aufgelöste Pfade, Eigentum, laufende Nutzung und nächsten
  Bedarf prüfen. Gemeinsame NexaUI-/NexaCloud-Toolchains und fremde Arbeiten erhalten.
  Entfernte Pfade, freigegebene Bytes und verbleibende Wiederverwendung dokumentieren.

## Verbindliche Designfreigabe: Grafikexport

- Der Nutzer hat den Grafikexport am 20. Oktober 2026 als Teil des fertigen Designs
  freigegeben. Bei Arbeiten an Auswerten, Berichtsdiagrammen oder Grafikexporten
  [ADR 010](docs/decisions/010-analysis-graphic-export.md) und die
  [freigegebene Spezifikation](docs/visuals/unified-workspace/EXPORT-EXAMPLE.md) lesen
  und nativ mit umsetzen. Es handelt sich nicht mehr um einen optionalen Vorschlag.
- Der [Gesamtauftrag für den umsetzenden Chat](docs/visuals/unified-workspace/NEXT-CHAT.md)
  enthält diese Freigabe. Vorhandene Browsernachweise sind keine native Abnahme.

## Verbindliche Designauswahl vom 25. Oktober 2026

- Der Nutzer hat ausdrücklich ausgewählt: **Leeransicht bestätigt; Protokoll C;
  Szene & Werte A; Auswertung A; Läufe/Serien A; Neues Projekt B; Datei-Explorer A.**
  Die bisherige Auswahlphase ist abgeschlossen. Diese Teile im bestehenden
  nativen Auftrag mit umsetzen; keine erneute Variantenentscheidung vorschalten.
- [ADR 011](docs/decisions/011-selected-workspace-additions.md),
  [verbindliche Spezifikation](docs/visuals/design-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) lesen.
  Die gemischte Auswahl gilt auch im Unteraufruf: Projektdialog B verwendet
  Explorer A, auch wenn er aus Protokoll C geöffnet wird.
- Die Grafikexport-Freigabe bleibt erhalten. Die Designfreigabe bestätigt keine
  fertige native Umsetzung oder Plattformabnahme; bestehende Funktionen erhalten.

## Verbindliche Designfreigabe: Markierung A2

- Der Nutzer hat am 29. Oktober 2026 **A2 · Dezenter Schleier ausdrücklich als
  finales Markierungsdesign freigegeben**. A2 ersetzt die frühere Grundlage A.
  [ADR 016](docs/decisions/016-selected-marking-a2.md),
  [verbindliche Spezifikation](docs/visuals/selection-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten.
  A2 nativ mit umsetzen; keine erneute Variantenentscheidung vorschalten.
  Linksauswahl zeigt keine automatische Aktionsleiste; Aktionen über Rechtsklick,
  Menütaste oder Umschalt+F10. Bestehende Designfreigaben bleiben erhalten.

## Verbindliche Designfreigabe: Dateisicherheit B2

- Der Nutzer hat am 29. Oktober 2026 **B2 mit einer gemeinsamen Git-Diff-Ansicht
  für Speichern, Dateikonflikte und Wiederherstellung ausdrücklich freigegeben**.
  [ADR 017](docs/decisions/017-selected-file-safety-b2.md),
  [verbindliche Spezifikation](docs/visuals/file-safety-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten.
  Die vorherige Übernahmesperre dieser Runde ist aufgehoben; B2 nativ mit umsetzen,
  keine neue Variantenentscheidung vorschalten. A/B/C bleiben Vergleichsmaterial.
  Eigenständige Aufgabenfenster, Markierung A2, Projekt B, Explorer A und der
  freigegebene Grafikexport bleiben erhalten. Browsernachweise sind keine native Abnahme.

## Verbindliche Designfreigabe: Einstellungen A

- Der Nutzer hat am 29. Oktober 2026 **A · Kategorien als fertiges
  Einstellungsdesign ausdrücklich freigegeben**. Die vorherige Übernahmesperre
  ist aufgehoben; A im bestehenden nativen Auftrag mit umsetzen, keine neue
  Variantenwahl vorschalten. B/C bleiben Vergleichshistorie.
- [ADR 018](docs/decisions/018-selected-settings-a.md),
  [verbindliche Spezifikation](docs/visuals/settings-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten:
  Kategorien links, Optionen mittig, Live-Vorschau rechts; Suche, Änderungsprüfung,
  Übernehmen/Abbrechen/Standardwerte und Fehlerzustände. Eigene Aufgabenfenster
  nach ADR 015, Preferences/Geometrie/Einheiten unabhängig halten.
  Bisherige Freigaben bleiben erhalten; Browserbelege sind keine native Abnahme.

## Verbindliche Designfreigabe: Rückmeldungen A

- Der Nutzer hat am 30. Oktober 2026 **A · Leiser Hinweis für D05 ausdrücklich
  ins fertige Design übernommen**. Die vorherige Übernahmesperre ist aufgehoben;
  A im bestehenden nativen Auftrag mit umsetzen, keine neue Variantenwahl.
  B/C bleiben Vergleichshistorie.
- [ADR 019](docs/decisions/019-selected-feedback-a.md),
  [verbindliche Spezifikation](docs/visuals/feedback-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten:
  höchstens zwei Erfolgskarten, erreichbarer Verlauf, lokale Pfad-/Kopierfehler,
  dauerhafte Veraltet-Kennzeichnung, Gruppierung und Fokus. Mit dem vorhandenen
  Protokoll-C-Ereignisbestand verbinden; echte Ziel-/Versionsidentitäten erhalten.
  Alle bisherigen Freigaben und native Arbeiten bleiben bestehen. Browserbelege
  sind keine native Abnahme. D06/D07 sind inzwischen ebenfalls gewählt.

## Verbindliche Designauswahl: Fortschritt B mit Pop-out

- Der Nutzer hat am 30. Oktober 2026 **B · Aufgabenleiste mit der Möglichkeit
  zum Herauslösen als eigenes Fenster** ausgewählt. Die vorherige Übernahmesperre
  für D06 ist aufgehoben; nativ mit umsetzen, keine neue Variantenwahl.
  A/C bleiben Vergleichshistorie.
- [ADR 020](docs/decisions/020-selected-progress-b-popout.md),
  [verbindliche Spezifikation](docs/visuals/progress-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten:
  kompakte B-Leiste, aufklappbare Details, echtes eigenes OS-Pop-out. Pop-out und
  erneutes Fensteranzeigen ausdrücklich als Icon-only mit Tooltip/zugänglichem Namen. Rückholen
  und Fensterschließen ohne Abbruch/Neustart. Ein gemeinsamer Auftragszustand,
  keine doppelten Worker. Hauptfenster bleibt bedienbar. Browser-Pop-up ist
  keine native Abnahme. Bestehende Freigaben und native Arbeit erhalten;
  D07 ist inzwischen als A gewählt; die vorhandene Suche bleibt, D09 folgt als neue Runde.

## Verbindliche Designauswahl: Kontextmenüs A

- Der Nutzer hat am 30. Oktober 2026 **A · Kompakt für D07 ausgewählt**.
  Die vorherige Übernahmesperre ist aufgehoben; A nativ mit umsetzen,
  keine neue Variantenwahl. B/C bleiben Vergleichshistorie.
- [ADR 021](docs/decisions/021-selected-context-menu-a.md),
  [verbindliche Spezifikation](docs/visuals/context-menu-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten:
  kompakte Liste mit Trennlinien, Tastaturhinweisen, Untermenüs und gesperrten
  Befehlen mit Grund; keine B-Zielköpfe/C-Erklärzeilen. Neun Zielarten,
  Mehrfachauswahl, A2, Rechtsklick/Menütaste/Umschalt+F10, Fokus und Kameradrag
  erhalten. Echte native Verfügbarkeit, Ziel-/Versionsbindung und Ergebnisse
  verwenden. Alle bisherigen Freigaben und native Arbeit bleiben erhalten;
  Browserbelege sind keine native Abnahme. D09 ist die nächste offene Designrunde.

## Bestandsabgleich: Editor-Suche D08

- Die vorhandene Suche/Suchinsel ist bereits gestaltet und nativ vorhanden.
  Nach dem Nutzerhinweis vom 30. Oktober 2026 D08 nicht als neue Layoutwahl führen.
  Die [ergänzende A/B/C-Studie](docs/visuals/search-options/README.md) ist getrennt,
  nicht freigegeben und ersetzt die bestehende Suche nicht. Keine erneute
  Variantenentscheidung vorschalten; A2, Kontextmenüs A und alle Freigaben erhalten.
  „Alle ersetzen“ ist nur eine gekennzeichnete Studie, kein neuer nativer Auftrag.
  Keine projektweite Suche/Regex/Ganzwortmodi ableiten. D09 ist die nächste
  tatsächlich offene Designrunde; dafür liegen inzwischen getrennte Popup-Entwürfe vor.

## Automatische Vervollständigung und neue Popup-Entwürfe D09

- Der Nutzer hat am 30. Oktober 2026 **automatisches Anzeigen beim Tippen
  ausdrücklich beauftragt und die Optik A/B/C verworfen**.
  [ADR 022](docs/decisions/022-automatic-completion.md) und
  [neue Entwürfe D/E/F](docs/visuals/completion-typing-options/README.md) lesen.
  Automatische Anzeige nativ mit ergänzen; manuelles Strg+Leertaste erhalten.
  D ist inzwischen mit Ergänzungen gewählt (ADR 023); E/F bleiben Historie;
  A/B/C nicht erneut zur Wahl stellen. Vorhandenen Katalog/Sprach-/Memberregeln,
  Undo/Zielbindung, API-Karte, F1 H, Suchinsel und A2 erhalten. Automatische Anzeige
  ist keine automatische Einfügung. Vorschläge/Timer bei Eingabe-/Dokumentwechsel
  verwerfen, IME/Kommentare/Strings/Nur lesen berücksichtigen. Kein neuer KI-/
  LSP-Dienst oder Provider-Worker. Referenz-Ladefall bleibt ein Zustandsentwurf.
  D10 vor neuer Gestaltung am bereits vorhandenen Explorer-/API-Bestand prüfen.

## Verbindliche Designauswahl: Vervollständigung D

- Der Nutzer hat am 30. Oktober 2026 **D mit standardmäßig fünf sichtbaren
  Einträgen, schmaler Scrollleiste und Variablenvorschlägen in Funktionsargumenten**
  ausgewählt. Anzahl später in Einstellungen A / Editor einstellbar, Standard 5;
  Ergebnisbestand nicht auf fünf kürzen. [ADR 023](docs/decisions/023-selected-completion-d.md),
  [Spezifikation](docs/visuals/completion-typing-options/APPROVED.md) und
  [Gesamtübergabe](docs/visuals/unified-workspace/NEXT-CHAT.md) beachten.
  Code in Kurzinfo und Editor mit denselben aktiven Syntaxrollen zeichnen.
  Zusätzlich separate Parameterhilfe wie VS Code/Visual Studio 2022: vollständige
  erwartete Namen/Typen/Anzahl, aktiven Parameter hervorheben, automatisch beim
  Aufruf/Komma und manuell per Strg+Umschalt+Leertaste. Liste im Normalfall unten;
  das zusätzliche Oben-Beispiel hat der Nutzer ausdrücklich zurückgenommen.
  Zugängliche Variablen/Parameter aus tatsächlichen sprachabhängigen Scopes
  anbieten; der begrenzte Browser-C-Lexer ist keine vollständige native Lösung.
  Abschließend mit neutraler Kurzinfo freigegeben; Farbfrage ist abgeschlossen.
  Codefarben an VS Code/VS 2022 orientieren: **Funktionen gelb**, dunkel #dcdcaa,
  hell #795e26. Gemeinsame Rollen für Funktionen, Typen, Variablen/Parameter und
  Kontrollfluss in Editor, Liste, Kurzinfo und Aufrufsignatur nativ ergänzen.
  Die alte pauschale ps_-Typfarbe ist ersetzt; genaue Werte in APPROVED.md.
  Keine erneute Farb-/Grundlayoutwahl vorschalten.
  Automatische Anzeige und alle bisherigen Freigaben bleiben bestehen;
  Browserbelege bestätigen keine native oder Plattformabnahme.

## Speicherpflege

- Der Nutzer hat die F:-Nutzung am 25. Oktober 2026 widerrufen: Physim bleibt
  vollständig auf D:. Die Rückführung ist abgeschlossen. F: weder für Builds,
  Tests, Archive, Caches noch temporäre Daten verwenden oder darauf zugreifen.
  Generierte Daten unter `D:\Physim\build` ablegen; Quellen, Testprogramme
  und kompakte Nachweise bleiben versioniert im Repository.
  `tools/with-artifacts.py -- ...` verwendet den lokalen Buildordner automatisch;
  unter WSL entspricht er `/mnt/d/Physim/build`. Explizite Ausgabeoptionen
  ebenfalls auf D: setzen. Alte eingefrorene Nachweise mit F:-Pfaden sind
  historische Daten und keine aktuellen Ausführungsanweisungen. Die Anleitung steht in
  [docs/development-storage.md](docs/development-storage.md).
- Prüfnachweise zuerst als kompakten Text-/JSON-Bericht mit Quellstand,
  Kommandos, Soll/Ist-Ergebnis und offenen Befunden dokumentieren. Bilder
  gezielt für visuelle Eigenschaften und Fehlerbelege aufnehmen; gleiche
  Zustände nicht bei jedem Eingabeschritt erneut als Vollbild speichern.
  Bestehende eingefrorene Nachweise nicht nachträglich verändern.

- Regelmäßiges Aufräumen gehört zur Projektarbeit. Nach abgeschlossener Arbeit
  veraltete, ersetzte und nachweislich nicht mehr benötigte Build-Ausgaben,
  Compiler-/Linker-Caches, temporäre Hilfsdateien und automatisch erzeugte
  Testbilder löschen, damit sich keine unnötigen Datenbestände ansammeln.
- Vor dem Löschen Quellen, Abhängigkeiten und verbleibende Verwendung prüfen.
  Laufende Builds, Tests oder fremde aktive Arbeiten nicht durch Bereinigung stören.
- Quellen, lokale Änderungen, Nutzdaten/Experimente, Git-Historie, Referenzbilder
  für Bildvergleiche, Dokumentationsbilder, benötigte Programme/Debug-Symbole,
  Offline-Abhängigkeiten und VM-/WSL-Datenträger erhalten. Ausdrücklich eingefrorene
  Prüfnachweise und Belege offener Fehler bleiben geschützt.
- Alte Test-Rohbilder nach Abnahme entfernen, sobald sie weder für einen offenen
  Befund noch als Referenz benötigt werden. Alte erfolgreiche Testausgaben ohne
  verbleibenden Zweck nicht unbegrenzt aufbewahren.
- Build- und ignorierte Verzeichnisse nicht pauschal leeren. Größere Bereinigungen
  kurz mit Umfang, Begründung und freigegebenem Speicher dokumentieren.

## Gemeinsame lokale Linux-Entwicklungsumgebung

- Auf diesem Windows-Rechner verwenden Physim, NexaUI und NexaCloud gemeinsam
  die vorhandene WSL-2-Distribution `Alpine-Dev` (Alpine Linux 3.24).
- Start: `wsl -d Alpine-Dev`; direkte Werkzeugaufrufe:
  `wsl -d Alpine-Dev --exec <Programm> <Argumente>`.
- Der Linux-Datenträger liegt außerhalb der Projekte unter
  `D:\WSL\Alpine-Dev\ext4.vhdx`. Er enthält Linux, installierte Werkzeuge,
  Bibliotheken und Linux-Dateien; er ist kein entbehrliches Build-Artefakt.
- Seit 2026-10-16 ersetzt dies `NexaUI-Alpine-3.24` und den früheren Speicherort
  `D:\NexaUI\build-linux-wsl\instance\ext4.vhdx`. Bei Wiederholung alter
  Anleitungen nur den Distributionsnamen anpassen; historische/festgeschriebene
  Prüfnachweise nicht nachträglich umschreiben. Die alte Registrierung existiert
  nicht mehr; keine zweite projektbezogene Distribution dafür importieren.
- Windows-Projektpfade bleiben `D:\Physim`, `D:\NexaUI`, `D:\NexaCloud`,
  unter Linux entsprechend `/mnt/d/Physim`, `/mnt/d/NexaUI`, `/mnt/d/NexaCloud`.
  Gemeinsame Pakete oder WSL-Einstellungen nur mit Blick auf alle drei Projekte
  ändern; diesen Datenträger bei Projektbereinigungen erhalten.
- Lokale Betriebs-/Umzugsnotiz: `D:\WSL\README.md`. Diese Angaben beschreiben
  die lokale Entwicklungsmaschine, keine Voraussetzung für andere Rechner/CI.
