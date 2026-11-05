# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag/Designfreigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Das Ziel bleibt die
vollständige Umsetzung/Abnahme; keine volle Punktzahl oder gesamte grüne CI behaupten.

## Aktueller Anschluss

Lokale Physim-Variablen/Parameter sind nativ angebunden: eigener Source-Snapshot,
nächste Bindung vor API-Namen, geschriebener Typ/Zeile, neutrale Karte ohne falsche
API-Referenz, Unicode-Präfix, leerer Aufruf/Komma, exakter Ersatz und ein Undo.
Quellcommit `072c06f5`, Nachweisabschluss `ae331255`, beide Repositories gepusht.
[Umfang und tatsächliche Phasen](verification/completion-locals-2026-11-05.md):
Windows/Linux je Unit 10/10 + GUI 6/6, danach gezielt GUI 2/2. Kein einzelner
neuer Vollaufruf mit letzter Binärdatei behauptet. Sechs finale Ansichten erhalten;
22.378.566 Bytes ersetzte Rohbilder nachvollziehbar entfernt.

Gehosteter Editorlauf `41943245186` zu `ae331255` ist beendet: ARM `126969009301`
und Intel `126969009667` jeweils **7/8**. Nur die neue Locals-Fixture scheitert
an Stufe 7: Sie sendete auch auf Apple Ctrl+Z statt Command+Z. Beide ZIPs/Digests/
Originaleinträge gelesen. [Originalbefund](verification/completion-locals-macos-before-2026-11-05.json).
Jetzt ausschließlich die zwei Undo-Teststellen auf `PS_UI_COMMAND_MOD` korrigiert;
Produktcode, Assertions und Fristen unverändert. Windows/Linux gezielt je **GUI 1/1**,
`run-s422uc0n` / `run-iqxgva07`. [Prüfnachweis](verification/completion-locals-command-2026-11-05.json).
Ein neuer gehosteter Lauf mit dieser Fassung steht noch aus; keine neue Mac-Abnahme.

Eingefrorener lokaler Anschluss: `completion-locals-command`, 178 eigene Eingaben /
1.128 Compiler-/Testdateien, 248 Windows-/135 Linux-Laufzeitdateien; alle Quell-/
Archivbytes gelesen, lokale Handles beendet. Detaillierte aktuelle Notiz:
`build/validation/completion-locals-2026-11-05/STATE.md`. Vorbereitung/Frieren über
die Helfer unter `build/validation/explorer-inline-2026-10-29/`; Argumente lesen.
Shared Baum: `build/validation/protocol-wrap-2026-10-26/3-isolated/` mit `src`,
`windows-build`, `linux-build`. Niemals bei laufendem lokalem Build/Test mutieren.

## Nächste Arbeit und verbleibender Umfang

1. Geprüfte Testkorrektur committen/pushen, tatsächlichen neuen Mac-Lauf verfolgen;
   ursprünglichen beendeten Lauf nicht erneut starten. Ergebnisse/Archive lesen.
2. Neuer konkreter Fehler: `completion_prepare` erlaubt `Ve` innerhalb eines
   mehrzeiligen Physim-Strings mit enthaltenem Anführungszeichen; echter Lexer
   erkennt den offenen String. Strenger direkter ASan/UBSan-Probe Exit 1,
   Soll `PS_INVALID`, Ist `PS_OK`. Quelldatei/Kommandos/Hashes in
   `build/validation/completion-locals-2026-11-05/literal-probe.json`.
   Vor Korrektur reguläre Regression ergänzen, Stringerkennung an den echten
   Sprachlexer anbinden; C-Verhalten erhalten. Noch kein nativer Vorherlauf behauptet.
3. Echte C-Scopes und separate Aufruf-/Parameterhilfe einschließlich Benutzerfunktionen
   ergänzen; weitere Codekontexte und semantische Rollen vervollständigen. Keine
   Typinferenz/Includes/Werte vortäuschen, kein Browser-Lexer/Dienst/Provider-Worker.
4. Übrige freigegebene UI: A2 Szene/Mehrfachauswahl, Kontextmenüs A, B2, weitere
   Aufgabenfenster sowie historische Settings-/AX-Fixtures und Gesamtgestaltung.
   Physische Eingabe/IME, Screenreader, DPI/Monitor/Vollbild und unabhängige
   Windows-Fokusbefunde bleiben offen. Linux-Baum bei 22: Dateierweiterungen
   umbrechen; finale Analyseaufnahme erhalten, vor Layoutkorrektur Explorer A lesen.
5. Getrennte offene Befunde: Unicode-Randleerzeichen im Einheitensymbol und
   historische CI-Fälle `run_events`/`nul.psrun`, Floating/Spring, Font-Demand, AX.
   Keine erfolgreiche Gesamtabnahme aus anderen Teiltests ableiten.

[Abgeschlossene Prüfungen samt Grenzen](native-verification-index.md) werden nicht
wegen Kompaktierung wiederholt. Insbesondere die Protokoll-Trennlinie ist seit
`ef1f3321` behoben; alter 0/0-Griff ist kein neuer Fehler. F1-/Protokoll-/Settings-
Matrizen benötigen vor Wiederholung einen festgehaltenen konkreten Anlass.
Rechnen & Geräte **A** ist gewählt, kommt nach der aktuellen UI-Arbeit; keine
erneute Variantenwahl ([ADR 025](decisions/025-selected-compute-devices-a.md)).

## Umgebung und Schutz bestehender Arbeit

**Nutzer-Mac aus: keine SSH-/Erreichbarkeitsprobe, Uploads oder lokalen Mac-Aktionen.**
Bisherige neue Mac-Läufe verwenden ausschließlich gehostete GitHub-Runner.
Vor späterem Nutzer-Mac-Zugriff aktuelle NexaCloud-/NexaUI-Anleitungen laut AGENTS lesen.
Nur D:, F: weder lesen noch schreiben. Generiertes unter `build`; WSL `Alpine-Dev`,
VM-Datenträger, Offline-Abhängigkeiten, Nutzdaten und benötigte Referenzen erhalten.
Die früher automatisch abgelehnte Löschung von 64 `.obj` unter
`build/validation/documentation-links-2026-10-30/windows-build/obj` nicht nachholen
oder umgehen. [Speicherregeln](development-storage.md).
Fremdes AT-SPI-/Orca-/Workflow-/Status-WIP und `dist/index.html`-Löschung erhalten;
nicht pauschal stagen/zurücksetzen oder als mit geprüft erklären. Eigene geprüfte
Änderungen normal committen und in `origin`/`upstream` pushen, keine Force-Pushes.
