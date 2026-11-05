# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag/Designfreigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Das Ziel bleibt die
vollständige Umsetzung/Abnahme; keine volle Punktzahl oder gesamte grüne CI behaupten.

## Aktueller Anschluss

[C-Syntaxwiederverwendung](verification/completion-reuse-2026-11-05.md): ein
unveränderter, eigener Flat-Snapshot für Cursorwechsel/erneutes Öffnen; Scope-/
Aufrufdaten frisch sammeln, alle Bytes/Besitzer/Sprache prüfen. Sichtbare Liste
beim Schließen leer; Cache bei Editor-/Dokument-/Projektwechsel und Ende frei.
Windows/Linux je **Unit 8/8 + GUI 6/6**, danach gezielt C-Unit **1/1** bei identischen
Appbytes. 887 Frischvergleiche, 123/123 Aufbau-, 7/7 Cursor-, 87/87 Edit-Speicherfehler.
Die erste falsche Inhaltsprüfung durchlief den alten Test; verstärkter gleicher
Besitzer/Cursor/Textumfang (`double`→`return`) weist sie und einen ignorierten
Cursor zurück. Finale 1.239 Eingaben gegen Git-Index gelesen, Snapshot
`completion-reuse-strong`; alle lokalen Handles beendet. Details/STATE unter
`build/validation/completion-reuse-2026-11-05/`. 33 bytegleiche BMPs/105.498.954 B
entfernt, eine abweichende Blinkphase behalten. Keine SDK-/VM-Bereinigung.
Cursor-Messung nur 7 Allokationen; größter p95 je Lauf 10,20 bzw.22,95 ms, Textedits
weiter langsam; 16,7-ms-Ziel **nicht zuverlässig erfüllt**. Ein Cache kostet im
größten Beispiel 6.166.734 gehaltene Bytes. Kein neuer Worker oder Designwechsel.
Neue gehostete Mac-Abnahme nach Quellpush noch offen; privater Mac aus/unberührt.

[C-Performance-Baseline](verification/completion-perf-2026-11-05.md) abgeschlossen:
Anwendungscode unverändert `694bb586`, neue CLI-Werkzeuge und kompakter JSON-Beleg.
Windows Release, sechs Quellen/21 Aufrufe pro Modus/Allocator; Cache p95 <0,04 ms,
große frische/Cursor-Auswertungen verfehlen die vorher gewählte 16,7-ms-Prüfmarke.
Im größten Fall Syntaxmedian: Parsen 37,67 / Knotenliste 12,50 / Freigabe 5,06 ms;
maximal 22.330.270 angeforderte Bytes. Keine Optimierung/gesamte UI-Abnahme daraus
ableiten. Werkzeug Windows/Linux je 47 Fälle, Linux ASan/UBSan mit Leakprüfung;
abschließender Python-CLI-Check frisch gebaut. Rohwerte/STATE unter
`build/validation/completion-perf-2026-11-05/`, keine neuen Bilder. Geschlossene
App-/GUI-/Mac-Prüfungen nicht wiederholt; Nutzer-Mac ausdrücklich weiter aus.

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

**Gehosteter Mac-Nachlauf geschlossen:** `41960990299`, Source/Head `694bb586`;
ARM `127027035143` / Intel `127027035424` jeweils
[8/8, beide ZIPs/Digests/alle Originaleinträge gelesen](verification/completion-c-macos-2026-11-05.json),
einschließlich C/Physim über je drei Ziele. Keine gesamte Mac-/physische oder neue
Mac-Allokationsfehler-Abnahme behaupten. Nutzer-Mac aus und unberührt. Diesen Lauf
und die alten Läufe `41947340908`/frühere nicht erneut starten oder pollen.

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

1. C-Locals-Schritt einschließlich gehostetem Mac-Nachlauf abgeschlossen;
   keine unveränderten lokalen/CI-Prüfungen allein wegen Kontextwechsel wiederholen.
2. Unveränderte C-Syntax wird wiederverwendet; echte Einfüge-/Löschfolgen und
   gleich lange Strukturänderung gegen kalte Modelle geprüft. Große Textänderungen
   parsen weiter vollständig. Nächster Ansatz: sichere inkrementelle Edit-Auswertung
   mit unabhängigem vollständigem Ergebnisvergleich und atomaren Fehlerpfaden;
   Cursor-/Edit-Zeit samt gehaltenem Speicher weiter als offene Performance führen.
   Kein neuer Provider-Worker, keine Ergebnis-/Scopeverkürzung oder Budgetschwächung.
   Keine UI-/Linux-/Mac-Performance-Abnahme. Neue gehostete Mac-Quellprüfung offen.
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
