# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag und Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Vollständige Umsetzung und Abnahme bleiben das Ziel; Teiltests sind keine volle Punktzahl.

## Aktueller Abschluss

[Sichtbare native Parameterhilfe](verification/signature-hud-2026-11-05.md):
erster integrierter Stand nach ADR023, unabhängig von Liste/Kurzinfo. Volle getestete
Signaturen, aktive A2-/Semibold-Syntax, Auto/Strg+Umschalt+Leertaste, Escape,
Übernahme, innere/überzählige Argumente, IME/Auswahl und OOM-Veröffentlichung geprüft.
Windows Release **6/6 GUI run-rg_b7qhf**, Linux ASan/UBSan **6/6 run-46yp8mpv**;
je C/Ph in drei Editorzielen. Zehn gezielte Bilder geöffnet, übrige Ergebnisse als JSON.
GDB-Negativbeleg: gleicher Text/alteBreite am Ursprung28px, Bildschirmort56px;
relative Stift-/Zeilenrechnung liefert nun28/28 und28 sichtbare Bytes. Kein neuer
Test allein wegen Kontextwechsel; [Phasen und Grenzen](verification/signature-hud-2026-11-05.json).

Warmstand künftig **`signature-hud-5`**,296 eigene Eingaben; Source-/Runtime-
Deltas aller Stufen gelesen. Lokale Builds/Tests/GDB-Prozesse beendet.
[Modellkern](verification/signature-model-2026-11-05.md) und dessen
[Mac-Nachlauf42005020387](verification/signature-model-macos-2026-11-05.json) geschlossen:
ARM/Intel10/10 Daten +8/8 GUI, Originale geprüft. Kein sichtbarer HUD im alten Mac-Stand.

## Nächste konkrete Schritte

1. HUD-Feinschliff: Wort-/Tokengrenzen statt Auftrennung von `->` (Linux22px-Bild),
   ehrlichen Exzesshinweis mit freigegebenem Fehlerton zeichnen. Lange16Parameter,
   kleine Fenster/Scroll und aktiven Parameter nativ prüfen; dann herausgelöster
   Workspace, echte Fokuswechsel und Accessibility-/Screenreader-Aufgaben.
   Detailstand unter `build/validation/signature-hud-2026-11-05/STATE.md`; den
   neuen gehosteten HUD-Mac-Nachlauf nach Quellpush unter seiner Kennung verfolgen.
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

Der neueste Mac-Beleg bestätigt den Datenkern, keinen sichtbaren HUD. Root-Linkcheck findet
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
