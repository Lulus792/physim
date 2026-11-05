# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag/Designfreigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Das Ziel bleibt die
vollständige Umsetzung/Abnahme; keine volle Punktzahl oder gesamte grüne CI behaupten.

## Aktueller Anschluss

[Aufrufziele für die Parameterhilfe](verification/call-targets-2026-11-05.md):
beide echten Parser liefern ursprüngliche Ziel-/Namens-/Empfängerspannen und
Member-Syntax; indirekte Ziele bleiben ohne geratenen Namen. Noch keine
Deklarations-/Signaturauflösung oder sichtbare Parameterhilfe. Windows/Linux
je **Unit 9/9 + GUI 6/6**, 36 neue Fälle, vorige C-/Physim-Logik konkret abgewiesen.
887 Frischvergleiche samt neuen Metadaten, 123/7/87 Speicherfehler weiter bestanden.
Finaler Snapshot `call-targets-ci`, 282 eigene / 1.221 Compiler-/Testeingaben;
alle 1.240 Eingaben gegen Git-Index gelesen. Details/STATE unter
`build/validation/call-targets-2026-11-05/`. Alle lokalen Handles beendet.
34 bytegleiche BMPs/108.695.892 Bytes entfernt, sämtliche Referenzen erhalten.
Neuer Mac-Workflow enthält vier Daten-Units vor acht GUI-Gruppen; Ausführung am
neuen Quellstand noch offen. Kein neues Performance-/physisches Abnahmeversprechen.

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
Quellcommit **`9292b67e`**, beide Repositories; [Commitabschluss](verification/completion-reuse-commit-2026-11-05.json).
Gehosteter Lauf **`41968549203`** zu genau diesem Quellstand geschlossen:
Intel `127051147984` und ARM `127051148217` jeweils
[8/8; beide ZIPs/Digests/alle Originaleinträge gelesen](verification/completion-reuse-macos-2026-11-05.json),
einschließlich C/Physim drei Ziele und Schließen/Cache-Wiederöffnen. Diesen und
ältere geschlossene Läufe nicht wiederholen/pollen. Keine ganze Mac-/physische/
neue Mac-Allokationsfehler- oder Performance-Abnahme. Privater Mac aus/unberührt.
Die nächste Parameterhilfe-Datenarbeit gehört nicht zu dieser Mac-Abnahme.

Frühere abgeschlossene Grundlagen bleiben über ihre Belege erreichbar:
[C-Performance-Baseline](verification/completion-perf-2026-11-05.md) mit tatsächlichem
Syntaxengpass und [native C-Locals](verification/completion-c-2026-11-05.md),
`694bb586`, inklusive Paket/Lizenzen und [ARM/Intel je 8/8](verification/completion-c-macos-2026-11-05.json).
Die früheren Läufe `41960990299`, `41947340908` und ältere nicht erneut pollen oder
aus Kontextgründen wiederholen. Aktueller gemeinsamer isolierter Baum bleibt
`build/validation/protocol-wrap-2026-10-26/3-isolated/` (`src`, `windows-build`,
`linux-build`); Helfer unter `build/validation/explorer-inline-2026-10-29/`.
Nächste Vorbereitung auf **`call-targets-ci`** aufbauen, niemals bei laufenden
lokalen Jobs mutieren. Die frühere C-Verfügbarkeitstextkorrektur in
`docs/workspace.md` ist inzwischen übernommen.

## Nächste Arbeit und verbleibender Umfang

1. C-Locals-Schritt einschließlich gehostetem Mac-Nachlauf abgeschlossen;
   keine unveränderten lokalen/CI-Prüfungen allein wegen Kontextwechsel wiederholen.
2. Unveränderte C-Syntax wird wiederverwendet; echte Einfüge-/Löschfolgen und
   gleich lange Strukturänderung gegen kalte Modelle geprüft. Große Textänderungen
   parsen weiter vollständig. Nächster Ansatz: sichere inkrementelle Edit-Auswertung
   mit unabhängigem vollständigem Ergebnisvergleich und atomaren Fehlerpfaden;
   Cursor-/Edit-Zeit samt gehaltenem Speicher weiter als offene Performance führen.
   Kein neuer Provider-Worker, keine Ergebnis-/Scopeverkürzung oder Budgetschwächung.
   Keine UI-/Linux-/Mac-Performance-Abnahme. Mac-UI-Nachlauf zu `9292b67e` geschlossen.
3. Aufrufziel-Daten fertig; tatsächliche API-/Benutzerfunktionsdeklarationen und
   geschriebene Parameter/Typen für C/Physim anbinden, anschließend unabhängigen
   Signatur-HUD vollständig nach ADR 023/APPROVED umsetzen (automatisch/manuell,
   aktive A2-/Fett-/Unterkantenmarkierung, zu viele/innere Argumente, Escape,
   Fokus/IME/Revision/Accessibility, normale Liste darunter). Keine Typinferenz/
   Includes/Werte vortäuschen, kein Browser-Lexer/Dienst/Provider-Worker.
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
