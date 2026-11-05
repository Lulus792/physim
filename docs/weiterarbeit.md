# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag und Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Vollständige Umsetzung und Abnahme bleiben das Ziel; Teiltests sind keine volle Punktzahl.

## Aktueller Abschluss

[Gemeinsamer Datenkern der Parameterhilfe](verification/signature-model-2026-11-05.md):
eigener Quell-/Header-/Displaybesitz, C-Zielabfrage am Callee-Namensort, echte Ph-
Modul-/Präfixdaten. 370 C- und 396 formale Ph-API-Aufrufe, eigene Funktionen/Methoden;
190 Spezial-/Property-/Dokueinträge ausdrücklich ungeklärt. **Noch kein sichtbarer HUD.**

Windows/Linux App+zunächst je **10/10 Daten**; danach neuer Cache desselben
unveränderten Aufrufs, final je **1/1 Modellgruppe**. 38 Fälle, 16 Parameter je Sprache,
60 C-/8 Ph-/4 geliehene Cache-OOM-Pfade, fünf saubere Gegenprogramme; Cursorwechsel
im selben Aufruf ohne weitere Modellallokation. Nested-/Invalid-Rückkehr geprüft.
Direkt strikt C17/Werror/ASan/UBSan/LSan; [Stufen/Hashes](verification/signature-model-2026-11-05.json).

Vorbereitung künftig auf **`signature-model-closed`** aufbauen: 294 eigene /1.233
Compiler-Testeingaben, alle 1.252 mit Git abgeglichen; Archive geprüft. Keine neuen
Bilder; 10 ersetzte temporäre Programme/Objekte entfernt, 124.082.744 logische Bytes.
Lokale Handles beendet. Details: `build/validation/signature-model-2026-11-05/STATE.md`.
Neuer Mac-Quellnachlauf noch offen; nach Push echte Kennung anhand Quellcommit
erheben (erwartet 10 Daten +8 GUI je Architektur). Alte C-/Modulnachläufe abgeschlossen.

## Nächste konkrete Schritte

1. Den vorhandenen Modellkern in den unabhängigen nativen Parameter-HUD nach
   ADR 023/APPROVED einbinden. Locals/Modell gegen echte aktuelle Eingabe prüfen;
   Speicherfehler erhalten alte Daten, dürfen aber keinen veralteten HUD zeigen.
   volle Signatur, gemeinsame Syntaxrollen, aktiver Parameter A2/fett/Unterkante,
   innere/überzählige Argumente, automatisch/manuell, normale Liste darunter,
   getrenntes Annehmen/Schließen, Escape, Fokus/IME/Revision und Accessibility.
2. 190 Einträge nach echten Spezialaufrufen/Overloads/Optionen und Eigenschaften
   unterscheiden; Compilerintrinsics außerhalb Katalog (print/assert/Konvertierung)
   aus echten Verträgen ergänzen. Funktionswert-Annotationen/Import-/Aliasfälle
   und ausgelassene Rückgabeannotationen klären. Keine Beispieltypen oder
   Empfängertypen aus Initialisierern erfinden; vollständiger Resolver bleibt offen.
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

- Frühere C-/Ph-Nachläufe sind im Prüfindex geschlossen; Originale geprüft.
  Alten Intel-Fristfehler seit `7a3e7950` nachgeprüft. Keine neue Beobachtung.
- Modulabfrage `ec7824ca`, Lauf41986344544: [ARM/Intel je8 Daten +8 GUI](verification/module-signatures-macos-2026-11-05.json),
  beide Original-ZIPs/Digests/Einträge gelesen. Abgeschlossen, nicht erneut pollen.
- C-Lookup `d75da866`, Lauf41992949589: [ARM/Intel je9 Daten +8 GUI](verification/c-lookup-macos-2026-11-05.json),
  beide Original-ZIPs/Digests/Einträge gelesen. Abgeschlossen, nicht erneut pollen.

Diese Mac-Belege bestätigen den Modellkern/HUD noch nicht. Root-Linkcheck findet
vorhandenen fremden Status-WIP-Link zur internen Wartungsnotiz: gleicher Fehler
vor/nach Generatorpatch belegt, fremde Dateien erhalten. Ohne neue Änderung nicht wiederholen.

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
