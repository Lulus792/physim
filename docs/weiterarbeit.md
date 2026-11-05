# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag/Designfreigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Das Ziel bleibt die
vollständige Umsetzung/Abnahme; keine volle Punktzahl oder gesamte grüne CI behaupten.

## Aktueller Anschluss

[Native C-Locals](verification/completion-c-2026-11-05.md) sind jetzt in Modell und
GUI angeschlossen: geschriebene Typ-/Deklarations-/Quellkarte, Besitzer-/Sprach-/
Revisionscache, lokale Namen vor API, volle Trefferzahl, UTF-8/UCN, leere Argumente,
Ersetzen/Undo und Schutz vor falscher API-Referenz. Core unabhängig; keine separate
Aufrufsignaturhilfe oder semantische C-/Include-/Präprozessoranalyse daraus ableiten.
Windows Release/Linux Debug ASan/UBSan jeweils **Unit 8/8 + GUI 6/6**, gleiche
Appbytes pro Plattform. Sechs C-Ansichten gesehen; 121/121 C-Modell-Speicherfehler
mit Snapshot-Erhaltung, 38 Aufrufkontexte. Paket/6.422 Dateien und drei Lizenzen
geprüft, keine dynamischen Parser-Exporte. Quellcommit **`694bb586`**, beide Repos;
alle **1.239** erfassten Eingaben gegen Git-Blobs gelesen. [Commitabschluss](verification/completion-c-commit-2026-11-05.json).

**Offener gehosteter Mac-Nachlauf:** `41960990299`, Source/Head `694bb586`;
ARM-Job `127027035143` zuletzt tatsächlich queued. Intel `127027035424` beendet:
[8/8, ZIP/Digest/alle Originaleinträge gelesen](verification/completion-c-macos-intel-2026-11-05.json),
einschließlich C/Physim über je drei Ziele. Nur den ARM-Anschluss weiterverfolgen;
keine gesamte Mac-/physische Abnahme behaupten. Nutzer-Mac aus und unberührt.
Die alten Läufe `41947340908` und frühere sind geschlossen; nicht erneut pollen.

Eingefrorener lokaler Anschluss: `completion-c-restored`, 281 eigene / 1.220
Compiler-/Testeingaben, 623 Windows-/143 Linux-Laufzeitdateien; Archive gelesen,
alle lokalen Handles beendet. Die spätere reine C-Verfügbarkeitstextkorrektur in
`docs/workspace.md` ist separat dokumentiert, in diesem Snapshot noch nicht drin;
beim nächsten vorbereiteten Schritt über die vorhandene Own-Input-Liste übernehmen.
Details: `build/validation/completion-c-2026-11-05/STATE.md`; Helfer unter
`build/validation/explorer-inline-2026-10-29/`. Shared Baum bleibt
`build/validation/protocol-wrap-2026-10-26/3-isolated/` (`src`, `windows-build`,
`linux-build`); niemals bei laufendem Build/Test mutieren. 28 bytegleiche BMPs/
89.514.264 Bytes entfernt, sechs C-Referenzen und alle Originalergebnisse erhalten.

## Nächste Arbeit und verbleibender Umfang

1. Neuen gehosteten Mac-Editorlauf oben bis zu tatsächlichen Originalergebnissen
   verfolgen; keine unveränderten lokalen Prüfungen allein wegen Kontextwechsel wiederholen.
2. Native C-Integration lokal abgeschlossen; großes Quellprogramm auf UI-Latenz/
   Speicherbedarf prüfen, bevor eine vollständige Performance-Abnahme behauptet wird.
3. Separate Aufruf-/Parameterhilfe einschließlich Benutzerfunktionen
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
Auch die neue kombinierte rekursive SDK-/Bildbereinigung wurde automatisch mit
„blocked by policy“ abgelehnt. Nur separate bytegleiche BMPs wurden entfernt;
`build/validation/completion-c-2026-11-05/restored-portable-windows` bleibt erhalten.
Diese SDK-Löschung nicht auf anderem Weg umgehen. [Beleg](verification/completion-c-cleanup-2026-11-05.json).
Fremdes AT-SPI-/Orca-/Workflow-/Status-WIP und `dist/index.html`-Löschung erhalten;
nicht pauschal stagen/zurücksetzen oder als mit geprüft erklären. Eigene geprüfte
Änderungen normal committen und in `origin`/`upstream` pushen, keine Force-Pushes.
