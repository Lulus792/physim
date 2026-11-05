# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
abgleichen. Auftrag und Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md).
Vollständige Umsetzung und Abnahme bleiben das Ziel; Teiltests sind keine volle Punktzahl.

## Aktueller Abschluss

[Parameterhilfe im herausgelösten Workspace](verification/signature-detached-2026-11-05.md):
zwei reproduzierte Fokusfehler korrigiert, keine neue sichtbare UI.
Windows Release7/7 run-69j1crw4, Linux ASan/UBSan7/7 run-xh60v0jy;
acht finale Zusatzfensterbilder tatsächlich geöffnet. C/Ph jeweils drei Ziele,
exakte Quelle/Cursor/Auswahl/Anordnung, überlappender Scrollgriff und Tippen nach
Drag sowie Rückkehr geprüft. [Originale und Grenzen](verification/signature-detached-2026-11-05.json).
Warmstand **signature-detached-checked**,301 eigene Eingaben. Detailstand und
späterer Mac-Handle: `build/validation/signature-detached-2026-11-05/STATE.md`.
[Quelle9cc44508, beide Remotes](verification/signature-detached-commit-2026-11-05.json):
alle1.257 Eingaben gegen Index und tatsächlichen Commit gelesen. Neuer gehosteter
Mac-Lauf42049114569: ARM127320087545 und Intel127320087841 zuletzt queued;
erwartet je11 Daten+10 GUI. Diesen Stand getrennt verfolgen, noch keine Abnahme.

[Kartenüberschreitung im Hauptfenster](verification/signature-outside-2026-11-05.md):
C/Ph samt Tippen nach Release, Windows gleicher Build direkt nach Fokusaktivierung,
Linux1/1; acht aktuelle Bilder ohne Workspace-Häkchen. Test-Gesamtfrist korrigiert,
normales Appverhalten unverändert. [Originalversuche und Grenzen](verification/signature-outside-2026-11-05.json).
[Quelle27ffb2f6, beide Remotes](verification/signature-outside-commit-2026-11-05.json).
[Mac-Lauf42037853611](verification/signature-outside-macos-2026-11-05.json)
zu27ffb2f6 geschlossen: ARM127280562388 und Intel127280563043 jeweils11/11 Daten
und9/9 GUI. Original-ZIPs/Digests/alle sechs Einträge je Archiv gelesen, acht
tatsächliche HUD-Bilder ohne Workspace-Häkchen geöffnet. Nicht erneut pollen.

[Workspace ohne Häkchen](verification/workspace-no-checks-2026-11-05.md):
ausdrücklicher Nutzerwunsch vom5.11. Checkboxzeichnung UND reservierte Spalte
entfernt; aktive Zeile, Symbole und Hierarchie erhalten. Ausnahme in AGENTS,
ADR016 und A2-Spezifikation festgehalten; Häkchen dort nicht wieder einbauen.
Windows Release **2/2 run-wepdvezc**, Linux ASan/UBSan **2/2 run-2n0icoeu**;
vorhandene Datei-/Projektbaumabläufe und vier gezielte Bilder geprüft.
[Befehle, Programme und Grenzen](verification/workspace-no-checks-2026-11-05.json).
Dieser abgeschlossene Workspace-Stand umfasste300 eigene Eingaben; Source-/Runtime-
Archive eingefroren und gegen Git gelesen. Aktuelle Proben: siehe STATE oben.
[Quelle0ddfe287, beide Remotes](verification/workspace-no-checks-commit-2026-11-05.json).

[Scrollgriff und Editor-Eingabe](verification/signature-thumb-2026-11-05.md):
vorherige zwei Produktkorrekturen, Windows **6/6 run-9euexj2y**, Linux **6/6 run-h3atr8no**:
Griff-Drag und Editorbesitzer über Auto-Refresh, echte GDB-/Watchpoint-Belege.
[Quelle05e43dc0, beide Remotes](verification/signature-thumb-commit-2026-11-05.json);
alle1.256 Eingaben gelesen. Acht Bilder bytegleich zu geöffneten Referenzen.
Behoben durch Erhalt des Editorbesitzers beim Auffrischen; Warten allein half nicht.

[Gehosteter Editor-Mac-Nachlauf42031236181 zu05e43dc0](verification/signature-thumb-macos-2026-11-05.json)
geschlossen: Intel/ARM je11/11Daten+9/9GUI, Original-ZIPs/alle12Einträge gelesen,
acht tatsächliche HUD-Bilder geöffnet. Keine erneute Abfrage dieses alten Laufs.
Er enthält die anschließende Workspace-Häkchenentfernung noch nicht.
Auch [Vorgänger42026549588](verification/signature-wrap-macos-first-2026-11-05.json) ist geschlossen; nicht pollen.

## Nächste konkrete Schritte

1. Jetzt Capture außerhalb einer OS-Fenstergrenze, externe Fokuswechsel und
   Accessibility-/Screenreader-Aufgaben. Detailstand und aktuelle Mac-Handles:
   `build/validation/signature-detached-2026-11-05/STATE.md` zuerst lesen.
   Mac-Nachlauf des jeweils neuen Quellstands getrennt verfolgen;
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

- Frühere C-/Ph-/Modul-/C-Lookup-/Intel-Fristnachläufe samt Original-ZIPs sind im Prüfindex geschlossen; nicht erneut pollen.

Der neue Mac-Beleg enthält jetzt auch den sichtbaren HUD zu05e43dc0. Root-Linkcheck findet
vorhandenen fremden Status-WIP-Link zur internen Wartungsnotiz: gleicher Fehler
vor/nach Generatorpatch belegt, fremde Dateien erhalten. Ohne neue Änderung nicht wiederholen.

## Dokumentation und Speicherpflege abgeschlossen

[Entwicklungsleitfaden](development-workflow.md): kurze ersetzte Übergaben und Text-/JSON zuerst.
[Bereinigung vom5.11.](storage-maintenance-2026-11-05.md): 248 Cachedateien/105.475.284 Bytes;
99 Programme/Bibliotheken hashgleich. [60 doppelte HUD-Bilder entfernt](verification/signature-hud-cleanup-2026-11-05.json):
191.816.280 Bytes, alle57 Bildinhalte/40 geschützten Aufnahmen erhalten. Quittungen gelten; nicht wiederholen.

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
