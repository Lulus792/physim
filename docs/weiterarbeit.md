# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag/Designfreigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Das Ziel bleibt die
vollständige Umsetzung/Abnahme; keine volle Punktzahl oder gesamte grüne CI behaupten.

## Aktueller Anschluss

[Geschriebene C-Signaturen](verification/c-signatures-2026-11-05.md): tatsächliche
vollständige Deklaratoren aus vorhandenem unveränderlichem Grammatik-Snapshot,
kein erneutes Parsen/Worker. Funktionen/Prototypen, einfache Funktionszeiger und
Definitions-Callbackparameter, eigene Parameterliste bei Zeiger-Rückgaben,
void/unspezifiziert/Ellipsis, geschriebene alte Typen und echte Bereiche/Reihenfolge.
Keine API-/Benutzerauflösung oder sichtbarer unabhängiger Parameter-HUD.
Windows/Linux endgültig je **Unit 12/12 + GUI 6/6**; 33 Deklarationsfälle,
205 Bytepräfixe, 775 Cursor-/Namensvergleiche und 3 Scratch-Fehler. Vier defekte
Varianten gezielt abgewiesen. Drei tatsächliche Implementierungsfehler behoben:
Cursor vor `}`, falscher Kommalisten-Basisbereich und globale Callback-Annotation
bei beschädigtem altem Kopf. Letzter Befund war konkreter Anlass für zweite
12+6-Matrizen; kein Wiederholen wegen Kontextwechsel.
Finaler Snapshot **`c-signatures-final`**, 287 eigene / 1.226 Compiler-/Testeingaben;
alle 1.245 Eingaben gegen Git-Index gelesen, alle Archive geprüft.
Windows `run-bx1hn8_m` / `run-caqjhctl`, Linux `run-g7rdvvr5` / `run-r90u_b6w`;
[JSON](verification/c-signatures-2026-11-05.json) und
`build/validation/c-signatures-2026-11-05/STATE.md` enthalten erste/finale Phasen.
Alle lokalen Handles beendet; 68 bytegleiche BMPs/217.391.784 Bytes entfernt,
Referenzen und Originalberichte erhalten. Neuer Mac-Nachlauf offen, Workflow
sieben Daten-Units + acht GUI-Gruppen. Keine gesamte UI-/Performance-/physische Abnahme.
Quellcommit **`f3ed4068`**, normal in beide Repositories gepusht;
[Commitabgleich und Mac-Anschluss](verification/c-signatures-commit-2026-11-05.json).
Gehosteter Lauf **`41980836062`** zu genau diesem Quellstand geschlossen: ARM
`127090312595` und Intel `127090312320` je **Unit 7/7 + GUI 8/8**.
[Beide ZIPs/Digests/alle Originaleinträge gelesen](verification/c-signatures-macos-2026-11-05.json),
C-Fälle 33/205/775/3, Physim 294 und Cache-/Fristmarker bestätigt. Diesen Lauf
nicht erneut pollen oder wegen Kontextwechsel wiederholen. Keine gemeinsame
Signaturauflösung, neue Modul-Recovery-, HUD-/Gesamt-/physische oder Performance-Abnahme.

[Geschriebene Physim-Signaturen](verification/signature-declarations-2026-11-05.md):
echter Parser veröffentlicht vollständige Köpfe mit Parametern/Typen/Rückgabe,
Generics, Methoden/Besitzer und lokalen Block-/Sichtbarkeitsspannen. Noch keine
C-Deklarationsprojektion, Auflösung oder unabhängiger sichtbarer Parameter-HUD.
Windows/Linux je **15 Unit-Gruppen + GUI 6/6**; 294 AST-/Präfix-Bytevergleiche,
drei defekte Datenvarianten abgewiesen. Erster Windows-Test verglich Padding;
feldweiser Nachtest je 1/1 bei gleichen Appbytes, Windows 1.410 Padding-Abweichungen.
Kurzinfo-Fristprüfer testet reale alte Frist am wartenden Modell statt ein
100-ms-Abtastfenster zu verlangen; sichtbare Prüfungen bleiben. Der gehostete
sechs-Daten-/acht-GUI-Nachlauf ist abgeschlossen (siehe unten).
Finaler Snapshot **`signature-declarations-fields`**, 284 eigene / 1.223
Compiler-/Testeingaben; alle 1.242 Eingaben gegen Git-Index gelesen, alle Archive
geprüft. Windows `run-nxyvdfhz` / `run-ptblb524`, Linux `run-q9gskm57` /
`run-ttk794d8`; erste 15er-Gruppen/Fehler und Details im
[JSON](verification/signature-declarations-2026-11-05.json) und
`build/validation/signature-declarations-2026-11-05/STATE.md`.
Alle lokalen Handles beendet. 34 bytegleiche BMPs/108.695.892 Bytes entfernt,
Referenzen und Originalberichte erhalten. Keine SDK-/VM-/Fremddatenbereinigung.
Quellcommit **`7a3e7950`**, normal in beide Repositories gepusht;
[Commitabgleich und Mac-Anschluss](verification/signature-declarations-commit-2026-11-05.json).
Gehosteter Lauf **`41976275068`** zu genau diesem Quellstand geschlossen: Intel
`127075753047` und ARM `127075753405` je **Unit 6/6 + GUI 8/8**.
[Beide ZIPs/Digests/Originaleinträge gelesen](verification/signature-declarations-macos-2026-11-05.json),
294 AST-Präfixfälle, neue Frist-Units und sechs tatsächliche Altfrist-Prüfmarker
pro Architektur, C/Physim je drei Cache-/Wiederöffnungsziele bestätigt. Diesen
Lauf nicht weiter pollen oder wegen Kontextwechsel wiederholen. Keine neue
C-Signatur-/HUD-/Gesamt-/physische oder Performance-Abnahme. Nutzer-Mac aus.

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
Mac-Nachlauf mit vier Daten-Units vor acht GUI-Gruppen ist mit dem unten belegten
Intel-GUI-Fehler beendet. Kein neues Performance-/physisches Abnahmeversprechen.
Quellcommit **`c9b9c00f`**, beide Repositories;
[Commitabgleich und Mac-Anschluss](verification/call-targets-commit-2026-11-05.json).
Gehosteter Lauf **`41972622898`** ist beendet und seine beiden ZIPs/Digests/
Originaleinträge sind gelesen: ARM `127064120558` **Unit 4/4 + GUI 8/8**,
Intel `127064120877` **Unit 4/4 + GUI 7/8**. [Nachweis und Fehler](verification/call-targets-macos-2026-11-05.json).
Intel scheitert in `completion_info_workflow`, Physim-Ziel 2, Stufe 15: der alte
Prüfer verlangt einen gerenderten Frame zwischen zwei kurzen Fristen. Die
Zeitprüfung ist in `7a3e7950` am realen wartenden Modell von der Frame-Abtastung
getrennt und auf beiden Architekturen erfolgreich nachgeprüft. Diesen alten Lauf nicht
erneut pollen/starten. Kein neuer Zugriff auf den Nutzer-Mac.

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
Nächste Vorbereitung auf **`c-signatures-final`** aufbauen, niemals bei laufenden
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
3. Aufrufziel-Daten und geschriebene C-/Physim-Köpfe fertig; gemeinsame tatsächliche
   API-/Benutzerfunktionsauflösung für beide Sprachen mit Scopes, Überdeckung und
   ehrlichen unbekannten/mehrdeutigen Ergebnissen anbinden und unabhängigen
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
