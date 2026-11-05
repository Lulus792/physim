# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag und Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Vollständige Umsetzung und Abnahme bleiben das Ziel; Teiltests sind keine volle Punktzahl.

## Aktueller Abschluss

[Workspace ohne Häkchen](verification/workspace-no-checks-2026-11-05.md):
ausdrücklicher Nutzerwunsch vom5.11. Checkboxzeichnung UND reservierte Spalte
entfernt; aktive Zeile, Symbole und Hierarchie erhalten. Ausnahme in AGENTS,
ADR016 und A2-Spezifikation festgehalten; Häkchen dort nicht wieder einbauen.
Windows Release **2/2 run-wepdvezc**, Linux ASan/UBSan **2/2 run-2n0icoeu**;
vorhandene Datei-/Projektbaumabläufe und vier gezielte Bilder geprüft.
[Befehle, Programme und Grenzen](verification/workspace-no-checks-2026-11-05.json).
Warmstand **`workspace-no-checks`**, 300 eigene Eingaben. Source-/Runtime-
Archive eingefroren und gegen Git gelesen; lokale Jobs beendet.
[Quelle0ddfe287, beide Remotes](verification/workspace-no-checks-commit-2026-11-05.json).

[Scrollgriff und Editor-Eingabe](verification/signature-thumb-2026-11-05.md):
vorherige zwei Produktkorrekturen, Windows **6/6 run-9euexj2y**, Linux **6/6 run-h3atr8no**.
Echter Griff-Drag und Editorbesitzer über Auto-Refresh; GDB/Watchpoint belegt
zuvor sechs Textbytes bei Workspace-ROM und gelöschtem Besitzer.
[Quelle05e43dc0, beide Remotes](verification/signature-thumb-commit-2026-11-05.json);
alle1.256 Eingaben gelesen. Acht Bilder bytegleich zu geöffneten Referenzen.
Behoben durch Erhalt des Editorbesitzers beim Auffrischen; Warten allein half nicht.

Gehosteter Editor-Mac-Nachlauf **42031236181 zu05e43dc0**: Intel127257917635/
ARM127257917828 zuletzt tatsächlichqueued geprüft;11Daten+9GUI und gezielteBMPs.
Er enthält die anschließende Workspace-Häkchenentfernung noch nicht.
[Vorgänger42026549588](verification/signature-wrap-macos-first-2026-11-05.json)
geschlossen: beide11/11Daten,8/9GUI, echte focus0-Diagnose; Original-ZIPs gelesen.
Alte Läufe nicht pollen; neuer Fokuswunsch wartet real bis30s, kein Guard-Bypass.

## Nächste konkrete Schritte

1. HUD: Drag außerhalb der Karte/Fenstergrenze, herausgelöster Workspace, Fokuswechsel und
   Accessibility-/Screenreader-Aufgaben. Detailstand und aktuelle Mac-Handles:
   `build/validation/signature-wrap-2026-11-05/STATE.md` zuerst lesen.
   Neuer Mac-Nachlauf muss11 Daten+9 GUI und gezielte HUD-Bilder liefern;
   Original-ZIPs/Digests lesen. Alte geschlossene Läufe nicht neu starten.
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

- Frühere C-/Ph-Nachläufe und alter Intel-Fristfehler seit `7a3e7950` geschlossen; Originale geprüft.
- Modulabfrage `ec7824ca`/41986344544 und C-Lookup `d75da866`/41992949589 abgeschlossen.
- Original-ZIPs/Digests/Einträge beider Architekturen im Prüfindex belegt; nicht erneut pollen.

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
Zusätzlich [60 bytegleich doppelte HUD-Testbilder entfernt](verification/signature-hud-cleanup-2026-11-05.json):
191.816.280 logische Bytes; alle57 Bildinhalte/40 geschützten Aufnahmen und beide Apps erhalten.

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
