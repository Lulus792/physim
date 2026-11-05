# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
lesen. Gesamtauftrag/Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Vollständige UI und
Projektumsetzung bleiben offen; Teilprüfungen sind keine volle Punktzahl.

## Letzter geprüfter Schritt

[Dateinamen bei großer Schrift](verification/filename-wrap-2026-11-05.md):
Workspace hält passende letzte Namensteile und Erweiterungen zusammen,
zum Beispiel `completion-` / `note.phys`; vollständiger Originalname bleibt
lesbar und öffnet die richtige Datei. Windows Release und Linux ASan/UBSan
jeweils **4/4 Datenprüfungen und3/3 GUI-Gruppen**, zwei gezielte finale Bilder
tatsächlich geöffnet. Testvorbereitung und historische Protokoll-Erwartungen der
Werkbankgruppe korrigiert; normale Protokollimplementierung unverändert.
[Kommandos, Originale und Grenzen](verification/filename-wrap-2026-11-05.json).

Warmstand **filename-wrap-owned**,303 ausdrücklich erfasste Eingaben.
Vor dem Build alle neun eigenen Quell-/Test-/Workflowdateien bytegleich gegen
Arbeitsbaum, Isolation und Manifest gelesen. Eigene Änderungen und Nachweise
gezielt committen; fremdes WIP erhalten. Detailstand und aktueller Mac-Handle:
`build/validation/filename-wrap-2026-11-05/STATE.md`.

## Vorherige Abschlüsse erhalten

- [Zusatzfenster](verification/signature-detached-2026-11-05.md),
  [Quelle9cc44508](verification/signature-detached-commit-2026-11-05.json):
  zwei Fokuskorrekturen; Windows/Linux je7/7. Hauptfenster und echter Zusatzkontext,
  C/Ph je drei Editoren, Scrollgriff über Quelle, Tippen und Rückholen geprüft.
- [Mac-Nachlauf42049114569 zu9cc44508](verification/signature-detached-macos-2026-11-05.json)
  ist terminal: Intel127320087841 **11/11 Daten+10/10 GUI**, Original-ZIP/Digest/
  alle zehn Einträge gelesen. Vier neue Bilder geöffnet, vier bytegleich zu
  zuvor geöffneten Hauptfensterreferenzen. ARM127320087545 vor Teststart
  abgebrochen: **kein ARM-Ergebnis**. Nicht erneut pollen; neue Quelle getrennt
  prüfen. Der aktuelle Workflow umfasst jetzt14 Daten+12 GUI je Architektur.
- [Workspace ohne Häkchen](verification/workspace-no-checks-2026-11-05.md),
  Quelle0ddfe287: keine Checkboxen UND keine reservierte Spalte. Nutzerpräzisierung
  in AGENTS/ADR016/A2 festgehalten; historische Bilder ändern diesen Vertrag nicht.
- Weitere geschlossene Quellen und Plattformläufe stehen im
  [Prüfindex](native-verification-index.md). Alte geschlossene Läufe nicht neu
  starten oder pollen. Protokoll-Trennlinie seit `ef1f3321` behoben; kein neuer
  Nachweis allein wegen Kontextwechsel. Ebenso keine F1-/Settings-Wiederholung.

## Nächste konkrete Aufgaben

1. Aktuellen Mac-Nachlauf verfolgen, Original-ZIPs/Digests lesen und tatsächliche
   UI prüfen. Host-Abbruch und fehlende Architekturabnahme ehrlich offen halten.
2. Capture außerhalb einer OS-Fenstergrenze, externe Fokuswechsel und native
   Accessibility-/Screenreader-Aufgaben. Bisherige SDL-Skripte sind keine
   vollständige physische Eingabe- oder Assistive-Technology-Abnahme.
3. Übrige freigegebene Oberfläche (A2, Kontextmenüs A, B2, Aufgabenfenster,
   Einstellungen/AX, Gesamtgestaltung) und tatsächliche Plattformabnahme.
   Rechnen & Geräte A erst danach (ADR025).
4. Die190 Signaturfälle nach Spezialaufrufen/Overloads/Optionen/Eigenschaften
   unterscheiden. Intrinsics, Funktionswerte, Importaliases und ausgelassene
   Rückgabeannotationen aus echten Verträgen ergänzen. Keine Typinferenz erfinden.
5. Große C-Textänderungen parsen noch vollständig; Cursorcache allein erfüllt
   16,7ms nicht zuverlässig. Inkrementelle Edits brauchen Frischvergleich,
   atomare Fehlerpfade und Messung; kein Provider-Worker oder gekürzter Scope.

## Speicher und Schutz

[Speicheraufnahme](verification/storage-inventory-2026-11-05.json) abgeschlossen:
536100 Projektdateien,83,80GB Dateigrößen/41,33GB tatsächlich zugeordnet;
40,47GB davon in build*-Ordnern. Gemeinsame WSL-VHDX44,98GB separat, kein
vollständiger Physim-Anteil. Lange Testpfade nacherfasst, danach0Fehler.
Keine erneute Inventur allein wegen Kontextwechsel. Bereinigungen und erhaltene
Referenzen: [Wartungsbericht](storage-maintenance-2026-11-05.md).

Nur **D:**; F: weder lesen noch schreiben. Generiertes unter `build`; gemeinsame
WSL **Alpine-Dev**, VM, Offline-Abhängigkeiten, Nutzdaten und offene Fehler erhalten.
**Privater Nutzer-Mac AUS: keine SSH-/Erreichbarkeitsprobe oder Uploads.**
Gehostete GitHub-Runner sind getrennt. Vor späterem Nutzer-Mac-Zugriff die in
AGENTS genannten NexaCloud-/NexaUI-Anleitungen lesen.

Abgelehnte Löschpfade nicht erneut versuchen: `build/validation/documentation-links-2026-10-30/windows-build/obj`
und `build/validation/completion-c-2026-11-05/restored-portable-windows`.
Fremdes AT-SPI-/Orca-/Workflow-/Status-WIP, Referenz-/Tutorial-Zeilenendungen
und `dist/index.html`-Löschung erhalten. Eigene geprüfte Änderungen normal in
**origin und upstream** pushen; keine Force-Pushes. Root-Linkcheck findet einen
bekannten fremden Status-WIP-Link: ohne neue relevante Änderung nicht wiederholen.
