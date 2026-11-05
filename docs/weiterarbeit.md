# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag und Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Vollständige Umsetzung und Abnahme bleiben das Ziel; Teiltests sind keine volle Punktzahl.

## Aktueller Abschluss

[Tatsächliches C-Aufrufziel](verification/c-lookup-2026-11-05.md): getrennte
Namensabfrage im vorhandenen C-Snapshot. Vollständige Deklaratoren verdecken API-
Namen bereits in ihrer unfertigen Initialisierung; Typedef/Enum/Parameter ebenso.
Direkt geschriebene vollständige Funktions-/Zeigerköpfe nutzen die gemeinsame
Signaturprojektion. Alte Vorschlags-/Signatur-API unverändert; kein Resolver/HUD.

Windows/Linux zunächst je **8/8 Daten**; danach zwei nur die neue Abfrage
betreffende Bereichslücken belegt/korrigiert. Endgültig **52 Fälle**, zwei Scratch-
Fehler, vier saubere gezielte Ablehnungen; finale Testdatei auf beiden Plattformen
und direkt strikt C17/ASan/UBSan/LSan bestanden. Testfehler-Bereinigung separat
nachgewiesen; keine erneute unveränderte GUI. [Stufen/Hashes](verification/c-lookup-2026-11-05.json).

Vorbereitung künftig auf **`c-lookup-closed`** aufbauen: 290 eigene /1.229 Compiler-
Testeingaben, alle1.248 mit Git abgeglichen; Archive geprüft. Lokale Handles beendet.
Keine neuen Bilder. Details: `build/validation/c-lookup-2026-11-05/STATE.md`.
Quellcommit **`d75da866`**, beide Remotes; [Commitabgleich](verification/c-lookup-commit-2026-11-05.json).
Mac-Nachlauf **41992949589 abgeschlossen**: Intel/ARM je **9/9 Daten +8/8 GUI**.
Beide Original-ZIPs/GitHub-Digests/alle Einträge gelesen: [Mac-Nachweis](verification/c-lookup-macos-2026-11-05.json).
Nicht erneut pollen/starten. Aktuell gemeinsames Modell in Arbeit, noch ungeprüft.

## Nächste konkrete Schritte

1. Gemeinsamen tatsächlichen C-/Physim-Signaturresolver anbinden: API und eigene
   Funktionen, Scopes/Überdeckung, keine erfundenen Typen/Überladungen. C-Lookup
   am ursprünglichen Callee-Namensort verwenden, nicht am Editorcursor in einem
   inneren Argumentblock. Gefundene gewöhnliche Namen verhindern API-Rückfall.
2. Unabhängigen nativen Parameter-HUD vollständig nach ADR 023/APPROVED:
   volle Signatur, gemeinsame Syntaxrollen, aktiver Parameter A2/fett/Unterkante,
   innere/überzählige Argumente, automatisch/manuell, normale Liste darunter,
   getrenntes Annehmen/Schließen, Escape, Fokus/IME/Revision und Accessibility.
3. Übrige freigegebene UI (A2, Kontextmenüs A, B2, Aufgabenfenster, Settings/AX,
   Gesamtgestaltung) und tatsächliche Plattform-/physische Abnahme fortsetzen.
   Linux-Explorer bei22: Dateierweiterungen umbrechen; Analyseaufnahme erhalten.
   Vor Layoutkorrektur Explorer A lesen. Rechnen & Geräte A erst danach (ADR 025).
4. Große C-Textänderungen parsen weiterhin vollständig. Cursorcache allein
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

- Frühere C-/Physim-Mac-Nachläufe41980836062/41976275068 geschlossen, Originale
  geprüft; siehe Prüfindex. Alten Intel-Fristfehler41972622898 seit7a3e7950
  erfolgreich nachgeprüft. Diese Läufe nicht erneut pollen/starten.
- Modulabfrage `ec7824ca`, Lauf41986344544: [ARM/Intel je8 Daten +8 GUI](verification/module-signatures-macos-2026-11-05.json),
  beide Original-ZIPs/Digests/Einträge gelesen. Abgeschlossen, nicht erneut pollen.

Diese Belege bestätigen den C-Lookup, noch keinen gemeinsamen Resolver/HUD
oder die vollständige UI-Abnahme. Alter Modulnachweis bleibt im Prüfindex.

## Dokumentation und Speicherpflege abgeschlossen

[Entwicklungsleitfaden](development-workflow.md), AGENTS, Speicherregeln und
Prüfvorlage führen jetzt kurze ersetzte Übergaben, klaren Prüfanlass und einmalige
Text-/JSON-Nachweise. [Bereinigung vom 5. November](storage-maintenance-2026-11-05.md):
248 ersetzte Compiler-/Cachedateien, 105.475.284 logische Bytes entfernt;
99 Programm-/Bibliotheksdateien vorher/nachher hashgleich. Zusätzliche Bildprüfung
ohne weitere sichere Duplikate; Referenzen/Fehler geschützt. Kein App-Neubau
für diese Dokumentationskorrektur. Details/Quittungen im verlinkten Bericht.

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
