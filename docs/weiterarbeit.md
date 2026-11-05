# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag/Designfreigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Das Ziel bleibt die
vollständige Umsetzung/Abnahme; keine volle Punktzahl oder gesamte grüne CI behaupten.

## Aktueller Anschluss

Die Physim-Vervollständigung ließ `Ve` innerhalb eines offenen mehrzeiligen Strings
mit innerem Anführungszeichen zu. Reguläre Regression am alten Code Exit 1;
nach Anbindung an den echten Sprachlexer elf Literal-Kontexte im strengen
C17-ASan/UBSan-Test bestanden. Windows/Linux danach jeweils **Unit 5/5 + GUI 6/6**,
einschließlich automatischer/manueller Unterdrückung und Wiederaufnahme außerhalb.
Keine Optik-/Friständerung, kein neuer Dienst; C-Verhalten erhalten. Kein nativer
Vorherlauf behauptet. [Umfang/Kommandos/Grenzen](verification/completion-literals-2026-11-05.md).
Quellcommit **`7f2cb2fc`**, alle **1.136** Eingaben gegen tatsächliche Git-Blobs
gelesen, nur CRLF/LF normalisiert. [Commitabschluss](verification/completion-literals-commit-2026-11-05.json).

Die vorherige [native Locals-Integration](verification/completion-locals-2026-11-05.md)
ist unter Windows/Linux geprüft. Gehosteter Nachlauf **`41945223855`** zu `b64cc8ca`
nach Undo-Testkorrektur `e27fc144`: ARM **`126975654119`** / Intel **`126975654382`**
je **8/8**, Originalarchive/Digests/alle Einträge gelesen. [Mac-Abschluss](verification/completion-locals-macos-after-2026-11-05.json).
Vorheriger [7/8-Befund](verification/completion-locals-macos-before-2026-11-05.json)
bleibt erhalten. Diese Mac-Läufe enthalten die anschließende Stringkorrektur nicht.
In diesen Schritten 47 ersetzte BMPs / **150.256.086 Bytes** entfernt; Originalfehler,
Archive und benötigte Referenzen erhalten. Einzelbelege in den verlinkten Berichten.

Eingefrorener lokaler Anschluss: `completion-literals-first`, 178 eigene Eingaben /
1.128 Compiler-/Testdateien, 248 Windows-/135 Linux-Laufzeitdateien; alle Quell-/
Archivbytes gelesen, lokale Handles beendet. Detaillierte aktuelle Notiz:
`build/validation/completion-literals-2026-11-05/STATE.md`. Vorbereitung/Frieren über
die Helfer unter `build/validation/explorer-inline-2026-10-29/`; Argumente lesen.
Shared Baum: `build/validation/protocol-wrap-2026-10-26/3-isolated/` mit `src`,
`windows-build`, `linux-build`. Niemals bei laufendem lokalem Build/Test mutieren.

## Nächste Arbeit und verbleibender Umfang

1. Nach Push der geprüften Stringkorrektur tatsächlichen neuen gehosteten
   Editorlauf beobachten und Originalergebnisse lesen; keine Wiederholung unveränderter lokaler Prüfungen.
2. Die beiden obigen Mac-Läufe sind beendet; nicht neu starten oder pollen.
   Die neue Stringkorrektur benötigt nach lokalem Abschluss ihren eigenen Nachlauf.
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
