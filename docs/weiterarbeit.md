# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag und Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Vollständige Umsetzung und Abnahme bleiben das Ziel; Teiltests sind keine volle Punktzahl.

## Aktueller Abschluss

[Physim-Köpfe trotz unvollständiger Eingabe](verification/module-signatures-2026-11-05.md):
getrennte Dokumentabfrage am echten Parser, vollständige Modul-/Methodenköpfe
auch hinter einer bearbeiteten Aufrufstelle. Keine beliebigen partiellen ASTs
als gültig behandeln. Lokale Funktionen bleiben in der Cursor-Präfixabfrage.
Compiler/alte API behalten ihre Verträge; noch kein gemeinsamer Resolver/HUD.

Erster Stand Windows/Linux je **Daten 17/17 + GUI 6/6**. Zusätzliche Sicherung nur
der neuen Dokumentabfrage gegen erneutes Beginnen beschädigter Namespace-Köpfe:
endgültig je **5/5 gezielte Datenprüfungen**, keine erneute unveränderte GUI-Matrix.
13 Fälle, 70 Präfix-/Kopfvergleiche, drei beschädigte Köpfe; vier Negativprogramme
und begrenzter alter Timeout geprüft. [Umfang/Hashes](verification/module-signatures-2026-11-05.json).

Finale Vorbereitung künftig auf **`module-signatures-final`** aufbauen.
288 eigene / 1.227 Compiler-/Testeingaben; alle 1.246 Eingaben gegen Git-Index,
alle Archive geprüft. Alle lokalen Handles beendet. 34 bytegleiche BMPs /
108.695.892 Bytes entfernt, Referenzen/Fehler erhalten.
Details: `build/validation/module-signatures-2026-11-05/STATE.md`.
Quellcommit **`ec7824ca`**, in beide Repositories gepusht;
[Commitabgleich](verification/module-signatures-commit-2026-11-05.json).
Aktueller gehosteter Lauf **`41986344544`**: ARM `127107852822` läuft in den
Daten-Units; Intel `127107853216` wartet. Genau diesen Lauf mit acht Daten- und
acht GUI-Gruppen samt Originalartefakten weiter prüfen, nicht neu starten.

## Nächste konkrete Schritte

1. **Nutzerauftrag vom 5. November:** Nach diesem Abschluss die Dokumentation
   verbessern und ihre Pflege künftig vereinfachen; ersetzte, nachweislich nicht
   mehr benötigte Bilder, Builds und temporäre Dateien auf D: prüfen/aufräumen.
   Quellen, aktive Arbeit, Referenzen, Fehlerbelege, eingefrorene Nachweise,
   benötigte Programme/Symbole, Offline-Abhängigkeiten und VM schützen.
2. Gemeinsamen tatsächlichen C-/Physim-Signaturresolver anbinden: API und eigene
   Funktionen, Scopes/Überdeckung, keine erfundenen Typen/Überladungen. C-eigene
   unvollständige Initialisierer sowie Typ-/Enum-Namen brauchen echte Zielmasken;
   die Vorschlagsliste allein ist dafür kein vollständiger Scope-Nachweis.
3. Unabhängigen nativen Parameter-HUD vollständig nach ADR 023/APPROVED:
   volle Signatur, gemeinsame Syntaxrollen, aktiver Parameter A2/fett/Unterkante,
   innere/überzählige Argumente, automatisch/manuell, normale Liste darunter,
   getrenntes Annehmen/Schließen, Escape, Fokus/IME/Revision und Accessibility.
4. Übrige freigegebene UI (A2, Kontextmenüs A, B2, Aufgabenfenster, Settings/AX,
   Gesamtgestaltung) und tatsächliche Plattform-/physische Abnahme fortsetzen.
   Linux-Explorer bei22: Dateierweiterungen umbrechen; Analyseaufnahme erhalten.
   Vor Layoutkorrektur Explorer A lesen. Rechnen & Geräte A erst danach (ADR 025).
5. Große C-Textänderungen parsen weiterhin vollständig. Cursorcache allein
   erfüllt die16,7-ms-Marke nicht zuverlässig. Inkrementelle Edits nur mit
   unabhängigem Frischvergleich/atomaren Fehlerpfaden und Messung; kein Provider-
   Worker oder gekürzter Ergebnis-/Scopebestand. Weitere offene Befunde stehen
   im [Prüfindex](native-verification-index.md), nicht aus anderen Tests schließen.

## Abgeschlossene Grundlagen und Mac

[Prüfindex mit Quellständen und Grenzen](native-verification-index.md) statt
alte Detailberichte hier erneut anzuhängen. Geschlossene Prüfungen nur bei
konkreter Code-/Abhängigkeitsänderung, neuem Fehler oder offener Prüfdimension
wiederholen; Anlass/Umfang vor dem Lauf nennen. Insbesondere die Protokoll-
Trennlinie ist seit `ef1f3321` behoben: kein neuer Test allein wegen Kontextwechsel.

- C-Signaturgrundlage `f3ed4068`: [ARM/Intel je Daten7/7 + GUI8/8](verification/c-signatures-macos-2026-11-05.json),
  Lauf41980836062, beide ZIPs/Digests/Originaleinträge gelesen. Nicht erneut pollen.
- Physim-Signaturgrundlage `7a3e7950`: [ARM/Intel je6/6 +8/8](verification/signature-declarations-macos-2026-11-05.json),
  Lauf41976275068 geschlossen. C-/Physim-Aufrufdaten und Cachevorläufer über Index.
- Alter Intel-Fehler41972622898 bleibt belegt; Fristprüfer in7a3e7950 korrigiert
  und erfolgreich nachgeprüft. Kein weiterer Nachlauf dieses alten Standes.

Diese Belege bestätigen die neue Dokumentabfrage/den Resolver/HUD nicht.
Mac-Nachlauf der aktuellen Quelle läuft unter obiger Kennung; Originalartefakte
noch nicht ausgewertet.

## Umgebung und Schutz

**Nutzer-Mac aus: keine SSH-/Erreichbarkeitsprobe, Uploads oder lokalen Aktionen.**
Gehostete GitHub-Runner sind davon getrennt. Vor späterem Nutzer-Mac-Zugriff
NexaCloud-/NexaUI-Anleitungen laut AGENTS lesen.

Nur **D:**; F: weder lesen noch schreiben. Generiertes unter `build`;
WSL `Alpine-Dev`, VM, Toolchains, Offline-Abhängigkeiten und Nutzdaten erhalten.
[Speicherregeln](development-storage.md). Gemeinsamer isolierter Baum:
`build/validation/protocol-wrap-2026-10-26/3-isolated/`;
Vorbereitungs-/Freeze-Helfer unter `build/validation/explorer-inline-2026-10-29/`.
Bei laufenden lokalen Builds/Tests keinen gemeinsamen Snapshot verändern.

Abgelehnte Löschungen nicht nachholen/umgehen: historische64obj unter
`build/validation/documentation-links-2026-10-30/windows-build/obj` sowie
`build/validation/completion-c-2026-11-05/restored-portable-windows`
([Beleg](verification/completion-c-cleanup-2026-11-05.json)).

Fremdes AT-SPI-/Orca-/Workflow-/Status-WIP und `dist/index.html`-Löschung erhalten.
Nicht pauschal stagen/zurücksetzen/als geprüft erklären. Eigene geprüfte Änderungen
normal committen und in `origin`/`upstream` pushen; keine Force-Pushes.
