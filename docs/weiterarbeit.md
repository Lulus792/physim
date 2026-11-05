# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag/Designfreigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Das Ziel bleibt die
vollständige Umsetzung/Abnahme; keine volle Punktzahl oder gesamte grüne CI behaupten.

## Aktueller Anschluss

Neue [C-Scope-Sammlung](verification/c-bindings-2026-11-05.md) auf der echten
Grammatik: Objektdeklaratoren und Parameter, geschriebene Typformen, aktive
Bereiche einschließlich ERROR-Erholung, Schleifen, Arrays/Funktionszeiger und
vollständige vorherige Komma-Deklaratoren. Windows Release/Linux Debug ASan/UBSan
final je **Unit 1/1**; 35 Codefälle, 182 Cursor-/Dokumentpaare, 65/65
Speicherfehler und 1.024 fehlerhafte Eingaben. Strenger direkter C17-Build bestanden;
Schleifenkopf-/Zeiger-Rückgabe-Regressionen und Bereichs-Gegenprobe tatsächlich
abgelehnt, korrigierter Stand bestanden. **Noch nicht in Completion-Modell/GUI
verknüpft.** Keine C-Aufruf-/Parameterhilfe oder neue Mac-Abnahme daraus ableiten.
[ADR 026](decisions/026-c-editor-grammar.md). Quellcommit **`37cb80dd`**, beide
Repositories gepusht, alle **1.236** erfassten Eingaben gegen Git-Blobs gelesen.
[Commitabschluss](verification/c-bindings-commit-2026-11-05.json).
Drei ersetzte erfolgreiche Programme/**17.734.624 Bytes** entfernt; Ergebnisse,
negative Originalfälle, aktuelle Binaries und Quellen erhalten.

Die [Syntaxgrundlage](verification/c-syntax-foundation-2026-11-05.md) ist unter
`a25683ef`/`9827831b` geschlossen, beide Repositories; 91 unveränderte Upstream-
Dateien, private Allokationsdomäne, Herkunft/Lizenzen/Symbole geprüft. Nicht erneut
prüfen ohne relevante Änderung. Drei ersetzte Programme/17.129.816 Bytes entfernt.
Die [Physim-Stringkorrektur](verification/completion-literals-2026-11-05.md)
`7f2cb2fc` und [native Physim-Locals](verification/completion-locals-2026-11-05.md)
sind separat geschlossen. Gehosteter String-Nachlauf `41947340908` zu `5c3b6da4`:
ARM/Intel je [8/8, Originalarchive/Digests gelesen](verification/completion-literals-macos-2026-11-05.json).
Alle früheren Mac-Läufe sind beendet; keinen davon weiter pollen. Nutzer-Mac aus.

Eingefrorener lokaler Anschluss: `c-bindings-final`, 278 eigene Eingaben /
1.217 Compiler-/Testdateien, 254 Windows-/139 Linux-Laufzeitdateien; alle Quell-/
Archivbytes gelesen, lokale Handles beendet. Detaillierte Notiz:
`build/validation/c-bindings-2026-11-05/STATE.md`. Vorbereitung/Frieren über Helfer
unter `build/validation/explorer-inline-2026-10-29/`; Argumente lesen.
Shared Baum: `build/validation/protocol-wrap-2026-10-26/3-isolated/` mit `src`,
`windows-build`, `linux-build`. Niemals bei laufendem lokalem Build/Test mutieren.

## Nächste Arbeit und verbleibender Umfang

1. C-Scope-Sammlung abgeschlossen. Keine erneute Prüfung unveränderter Syntax-/
   GUI-Fälle; keine sichtbare C-Vervollständigung daraus ableiten. Anschluss: Punkt 3.
2. Alle obigen Mac-Läufe sind beendet; nicht neu starten oder pollen.
3. C-Scope-Daten ins Completion-Modell und die GUI integrieren: Besitzer/Revision,
   Cache und Speicherfehler, vollständige lokale Treffer, geschriebene Typ-/Quellkarte,
   tatsächlicher C-Aufrufkontext/leere Argumente und Einfügen/Undo. Drei Lizenzen
   vor GUI-Linkage ins portable Paket aufnehmen; Core unabhängig halten. Danach
   separate Aufruf-/Parameterhilfe einschließlich Benutzerfunktionen
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
