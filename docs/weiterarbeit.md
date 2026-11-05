# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, Git-Status und relevante Diffs lesen.
Maßgeblich bleiben [AGENTS.md](../AGENTS.md) und der
[Gesamtauftrag](visuals/unified-workspace/NEXT-CHAT.md).
**Vollständige UI-/Projektumsetzung und volle Punktzahl sind weiterhin offen.**

## Aktueller geprüfter Schritt

[F1-Linkfehler außerhalb der Ansicht](verification/f1-error-semantics-2026-11-05.md):
vorhandenes Status-/Fehlerlabel mit echten Offscreen-Zuständen veröffentlichen,
ohne Zeichen-/Layoutänderung. Windows UIA/Tastatur/Metadaten3/3; Linux Tastatur/
Metadaten und finaler unabhängiger AT-SPI-Client bestanden. Derselbe endgültige
Client erkennt den fehlenden Hinweis am archivierten alten Programm.
Ein nativer Fokuszustand als vollständige BMP bytegleich. Native Aktivierung
fokussiert/revealt ihre Quelle; die exakte Endposition erst nach dem eingereihten
Folgeframe messen. Zwischenfehler und Geometriespur sind dokumentiert.

[F1-Fontprobe](verification/font-settings-fault-2026-11-05.md): tatsächlich separates
Settings-Fenster öffnen, Allokationsfehler erreichen und Rollback/Entwurf prüfen.
Vorher beide Stufe3 negativ, nachher Windows/Linux je3/3. Produktfonts unverändert.

Warmstand **f1-error-semantics-final**, **307 eigene Eingaben**. Alle berührten
Eingaben vor Build/Prüfung in Root/Isolation/Manifest abgeglichen; Font- und
Link-Quell-/Runtimearchive vollständig zurückgelesen. Alle lokalen Sessions
terminal, inklusive finalem Client52151, Gegenprobe17087 und Freeze33045.
**Nicht allein aus Kontext wiederholen.** Detailstand:
`build/validation/f1-error-semantics-2026-11-05/STATE.md`.

## Weitere aktuelle Abschlüsse

- [Berichtsexport](verification/report-export-workflow-2026-11-05.md):
  Windows Release/Alpine/X11 ASan/UBSan je5/5,24 Grafikdateien geprüft.
  Quelle01d2c0d0 und beide Remotes samt1257Eingaben nachgewiesen.
  Linux-UI42090594921 inzwischen terminal: GCC/Clang je26/26 Daten+46/48 UI,
  nur die jetzt lokal korrigierten Font-/Linkprüfungen fehlgeschlagen; beide
  Originaljoblogs vollständig gelesen, keine großen ZIPs zusätzlich geladen.
  POSIX-Sicherheitsprüfung erhalten, eigene Testabbilder nach Archivprüfung
  [aufgeräumt](verification/report-export-filesystem-cleanup-2026-11-05.json).
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

1. Die neuen tatsächlichen Plattformläufe zum gepushten F1-Quellcommit verfolgen,
   insbesondere glibc-Font-/Linkprüfung und korrigiertes SDK-Prüfkit auf frischen
   Debian-/Ubuntu-Verbrauchern. Handles aus `f1-error-semantics-2026-11-05/new-runs.json`
   im lokalen Validationordner verwenden; kein Neustart wegen Beobachtungsfrist.
2. Frühere Quelle01d2c0d0 zuletzt: MacWorkspace42090594627 Intel/ARM in der echten
   34-Projekt-Matrix, Editor42090594861 Intel aktiv/ARMqueued; Paket42090594644
   jobs[], GeneralCI42090594659 mehrere Compilerjobs aktiv. NativeMenüs42090594773
   und Typografie42090594756 beide Architekturen laut Job-API success, deren
   Originalartefakte noch nicht gelesen. Nicht als vollständige neue Abnahme führen.
   Linux-UI42090594921 ist vollständig terminal; nicht erneut pollen. Der ältere
   GCC-grabFocus-Fehler tritt in dessen neuen beiden Jobs nicht auf: nicht auf
   Verdacht reparieren. Neue F1-Korrekturen brauchen ihre eigene glibc-Abnahme.
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
- Die sieben konkreten Dateien im [neuen abgelehnten Bereinigungsplan](verification/f1-error-cleanup-2026-11-05.json).
  Ein wiederhergestelltes temporäres Programm und sechs bytegleiche Diagnosebilder;
  automatische Ablehnung „blocked by policy“, alle erhalten,0Bytes freigegeben.

Letztere wurden automatisch mit „blocked by policy“ abgelehnt; diese Ablehnung
ist kein Auftrag für einen anderen Löschweg.
