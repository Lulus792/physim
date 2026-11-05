# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, Git-Status und relevante Diffs lesen.
Maßgeblich bleiben [AGENTS.md](../AGENTS.md) und der
[Gesamtauftrag](visuals/unified-workspace/NEXT-CHAT.md).
**Vollständige UI-/Projektumsetzung und volle Punktzahl sind weiterhin offen.**

## Aktueller geprüfter Schritt

[Berichtsexport über die heutigen Aufgabenfenster](verification/report-export-workflow-2026-11-05.md):
Windows Release und Alpine/X11 ASan/UBSan jeweils **5/5**. Auftrieb, Feder,
Plot, Pendel-Energie sowie Pendel mit Medium; beide Pendelsprachen enthalten.
24 gespeicherte Grafiken zusätzlich gelesen, SVG-XML/Geometrie und vollständige
PNG-Daten geprüft; zwei repräsentative PNGs geöffnet.
Nur Testabläufe/Erwartungen angepasst: tatsächliches Exportfenster, letzte
Quelldatei, Rohdatenaktion aus Messdaten. Renderer und Design erhalten.
[Originale und Grenzen](verification/report-export-workflow-2026-11-05.json).

Warmstand **report-export-accepted**, **306 eigene Eingaben**. Alle sechs seit
dem vorherigen Warmstand berührten Eingaben vor Build zwischen Root, Isolation
und Manifest bytegleich. Sämtliche Zwischenstände und Runtimearchive eingefroren.
Windows-Run `run-kbdk8vph`, Linux `run-oal6k0k9`; alle lokalen Test-/Freeze-
Sessions terminal. Nicht aus Kontext erneut starten, vorbereiten oder einfrieren.
Detailstand: `build/validation/report-export-workflow-2026-11-05/STATE.md`.

Die POSIX-Sicherheitsprüfung blieb aktiv: DrvFS liefert nach chmod0700 weiterhin
0777, die private ext4-Fixture korrekt0700. Die zwei eigenen temporären Abbilder
wurden nach vollständiger Archivierung und Prüfung auf offene Nutzung entfernt:
[191.393.792 zugeordnete Dateibytes](verification/report-export-filesystem-cleanup-2026-11-05.json).
Originalarchive und Referenzen bleiben erhalten; keine gemeinsame WSL-Änderung.

## Weitere aktuelle Abschlüsse

- [SDK-Prüfbaukasten](verification/verification-kit-import-2026-11-05.md),
  [Quelle7bbc5f13 und beide Remotes geprüft](verification/verification-kit-import-commit-2026-11-05.json):
  fehlendes lokales artifact_paths.py aufgenommen; isolierter Import auf Windows/
  Linux vor Fix negativ, danach positiv. Alle212Verbraucherdateien gegen Git gelesen.
  Originaler Debian-Release-Build zur Quelle df9927c6 tatsächlich789/789, Appstart
  ohne Compiler/Python auf Debian und Ubuntu erfolgreich. Die spätere unabhängige
  Verbraucherprüfung scheiterte am inzwischen korrigierten Prüfarchiv.
- [Mac-Suchfokus zur Quelle7c1870c4](verification/macos-search-focus-2026-11-05.md):
  Editorlauf42081349550 vollständig terminal/success. Intel127434902003 und
  ARM127434901591 jeweils14/14Daten+12/12UI mit je zwölf neuen Suchfokussequenzen.
  Beide Original-ZIPs/Digests/alle22Einträge gelesen;18Bildinhalte identisch zu
  geprüften Referenzen, keine Extraktion. **Nicht erneut pollen oder lesen.**
- [Getrennter Mac-Workspace](verification/macos-workspace-7c-intel-2026-11-05.json):
  Lauf42081349500 vollständig terminal. Intel127434902628 erfolgreich4/4 samt
  68gebauten/aufgezeichneten/ausgewerteten Projekten bei16/22px. ARM127434901974
  vor dem ersten Schritt cancelled; keine ARM-Workspace-Abnahme. Beide Original-
  Einträge gelesen. Nicht weiter pollen; bekannten Retry403 nicht wiederholen.
- Frühere belegte Abschlüsse im [Prüfindex](native-verification-index.md) erhalten.
  Dazu gehören Dateireiter-Auto-Completion, Zusatzfensterfokus, Dateinamen,
  Workspace ohne Häkchen/ohne reservierte Spalte sowie F1-/Settings-Teilprüfungen.
  Protokoll-Trennlinie seit ef1f3321 behoben. Kein neuer Test allein wegen Kontext.

## Nächste konkrete Aufgaben

1. Aktuelle eigene Testkorrekturen samt Nachweisen prüfen, committen und beide
   Remotes normal pushen; tatsächlichen Quellcommit gegen alle verwendeten Eingaben
   lesen und neue CI-Handles festhalten. Kit-Quelle7bbc5f13 hat bisher
   Linux-Paket42082675664 und CI42082675610, zuletzt pending. Keine Wiederholung
   wegen Beobachtungsfrist; auf dem tatsächlich neuen Quellstand weiterarbeiten.
2. Übrige bestätigte glibc-CI-Fehler anhand gesicherter Originale bearbeiten:
   font_demand Stufe3, accessibility_links_atspi fehlende Fehlermeldung und
   GCC zusätzlich accessibility_atspi.py:200, reader.grabFocus() liefert false.
   Alte Auftriebs-/Federfehler Stufe34 sind lokal behoben; neue glibc-Abnahme
   ist noch nicht bewiesen. Quelle df9927c6 / Linux-UI42076708745 ist vollständig
   terminal (Clang44/48, GCC43/48, jeweils26/26Daten).
   Clang-Archiv im [Nachweis](verification/glibc-endian-clang-2026-11-05.md),
   vollständiges GCC-Joblog unter `verification-kit-import-2026-11-05`.
3. Native Accessibility-/Screenreader-Aufgaben, physische Eingabe und Capture
   außerhalb einer OS-Fenstergrenze; gesamte freigegebene Gestaltung A2/B2,
   Kontextmenüs A, Einstellungen, Aufgabenfenster und F1 weiter abgleichen.
   Die geskripteten SDL-Ereignisse und engen Teilprüfungen ersetzen diese Abnahme nicht.
4. Die190Signaturverträge weiter auf Intrinsics, Funktionswerte, Importaliases und
   ausgelassene Rückgabetypen prüfen. Große C-Änderungen parsen noch vollständig;
   16,7ms erfordern inkrementelle Edits mit Frischvergleich und atomaren Fehlerpfaden.
   Rechnen & Geräte A folgt nach der UI (ADR025).

## Speicher und Schutz

Nur **D:**, F: weder lesen noch schreiben. Generiertes unter `build`.
Gemeinsame WSL **Alpine-Dev**, VHDX, Offline-Werkzeuge, Nutzdaten und Referenzen
erhalten. [Speicheraufnahme](verification/storage-inventory-2026-11-05.json)
abgeschlossen; keine neue vollständige Inventur nur wegen Kontext.
**Privater Nutzer-Mac AUS: keine SSH-/Erreichbarkeitsprobe oder Uploads.**
Gehostete Runner sind getrennt; für spätere Nutzer-Mac-Arbeit zuerst die in
AGENTS genannten NexaCloud-/NexaUI-Hinweise lesen.

Fremdes AT-SPI-/Orca-/Workflow-/Status-WIP, Referenz-/Tutorial-Zeilenendungen und
`dist/index.html`-Löschung erhalten. Keine fremden Diffs in die Isolation übernehmen.
Eigene geprüfte Änderungen normal in origin UND upstream pushen; kein Force-Push.
Der Root-Linkcheck hat einen bekannten fremden Status-WIP-Link; ohne neue relevante
Änderung nicht wiederholen.

Abgelehnte Löschziele nicht erneut versuchen oder mit anderem Werkzeug umgehen:
- `build/validation/documentation-links-2026-10-30/windows-build/obj`
- `build/validation/completion-c-2026-11-05/restored-portable-windows`
- `build/validation/signature-focus-2026-11-05/xauth-verified`
- `build/validation/signature-focus-2026-11-05/xauth-accepted`

Letztere wurden automatisch mit „blocked by policy“ abgelehnt; diese Ablehnung
ist kein Auftrag für einen anderen Löschweg.
