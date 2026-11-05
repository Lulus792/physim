# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
lesen. Gesamtauftrag/Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Vollständige UI und
Projektumsetzung bleiben offen; Teilprüfungen sind keine volle Punktzahl.

## Letzter geprüfter Schritt

[Linux-Endianschnittstellen im C-Parser](verification/glibc-endian-2026-11-05.md):
Ubuntu/Clang scheiterte beim Kompilieren an versteckten le16toh/be16toh;
Debian/GCC später beim Linken. Beide Original-Joblogs zur Quelle13cc4422 gelesen
und gesichert; keine nativen Ergebnisarchive entstanden. Jetzt _DEFAULT_SOURCE
im projektseitigen Linux-Adapter vor Systemheadern anfordern. Upstream-Dateien,
Compiler-/POSIX-Optionen und UI unverändert.
Windows Release8/8Daten+2/2GUI (run-ms_cod0i/run-fw347tfd), Alpine ASan/UBSan
8/8+2/2 (run-07qrdp6f/run-syykpp1q), Datenprüfungen mit detect_leaks=1.
**Alpine verwendet musl; neue tatsächliche glibc-Abnahme bleibt offen.**
[Originale, Befehle und Grenzen](verification/glibc-endian-2026-11-05.json).

Warmstand **glibc-endian**,303 Eingaben, eine geänderte Adapterdatei vor Build
Root/Isolation/Manifest bytegleich, zwei Upstream-Header roh gegen Git geprüft.
Source-/Runtimearchive vollständig gelesen. Detailstand:
`build/validation/glibc-endian-2026-11-05/STATE.md`.

Zuvor [Dateireiter-Auto-Completion](verification/file-tabs-auto-2026-11-05.md),
[Quelle13cc4422 in beiden Remotes](verification/file-tabs-auto-commit-2026-11-05.json):
Windows/Linux je3/3 nach begrenztem Warten auf tatsächlich automatische Liste,
App/Design/140ms-Timer unverändert. Beide Listenansichten geöffnet. Alle1257Inputs
gegen echten Commit gelesen; nicht erneut wegen Kontext prüfen.

Zuvor [OS-Fokus im Zusatzfenster](verification/signature-focus-2026-11-05.md):
Quelle8bf467ce, Windows Release/Linux ASan/UBSan je2/2. Eigener Peerprozess,
Quelle/Cursor/Anker/Scroll und nichtleere Auswahl über echte OS-Fokuswechsel.
[Alle1257Quellen gegen tatsächlichen Commit gelesen, beide Remotes](verification/signature-focus-commit-2026-11-05.json).
Nicht wegen Kontext erneut prüfen; der neue Dateireitertest ändert diesen
Productfix nicht. Vollständige physische/assistive Abnahme bleibt offen.

[Mac-Nachlauf42059129050 zu0d3f4a65 abgeschlossen](verification/filename-wrap-macos-2026-11-05.json):
Intel127355571409 und ARM127355571940 **terminal/success**, jeweils14/14 Daten+
12/12 GUI. Beide Original-ZIPs/Digests und alle22 Einträge gelesen, vier neue
Bildinhalte geöffnet,14 weitere bytegleich zu erhaltenen Referenzen. Nicht weiter
pollen oder neu starten. Diese alte Quelle belegt nicht die spätere Fokuskorrektur.

## Vorherige Abschlüsse erhalten

- [Dateinamen bei großer Schrift](verification/filename-wrap-2026-11-05.md),
  [Quelle0d3f4a65 in beiden Remotes](verification/filename-wrap-commit-2026-11-05.json):
  Windows/Linux jeweils4/4 Daten+3/3 GUI, vollständige Namen/Erweiterungen,
  echter22px-Draw/Clip und richtige Dateiöffnung. Alle1257 Eingaben gegen Commit
  gelesen. Werkbank-Testmaße korrigiert; Produkt-Protokoll unverändert.
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

1. Dateireiterquelle13cc4422: Mac-Workspace **42073119561** weiterverfolgen.
   Intel127406074781 zuletzt in der68-Projekt-Matrix; erster3er-Schritt erfolgreich,
   ARM127406075215 inzwischen vor dem Teststart cancelled. Keine ARM-Abnahme;
   keine vollständige neue Mac-Abnahme behaupten. Intelhandle weiter beobachten.
   Weitere bekannte CI-Handles derselben Quelle:42073119608 (CI),42073119770
   (Linux-UI-Sanitizer),42073119600 (Linux-Paket). Paketjob127406074505 terminal/
   failure und Clang-UI127406075360 terminal/failure durch oben dokumentierte
   glibc-Header; keine Ergebnisarchive suchen. GCC-UI127406075709 zuletzt noch
   in_progress bei Abhängigkeiten. Neue korrigierte glibc-Quelle separat prüfen;
   bereits gelesene Fehlerlogs nicht wegen Kontext erneut herunterladen.
   [Quell-/Run-Nachweis](verification/file-tabs-auto-commit-2026-11-05.json).
   Fokusquelle8bf467ce ist jetzt [vollständig nachgelesen](verification/macos-8bf-2026-11-05.json):
   Editor beide14/14Daten+12/12GUI, Typografie beide16/16, Menü beide7/7;
   alter Workspace beide2/3 durch Stufe27, nächste Matrix übersprungen.
   Alle vier8bf-Läufe terminal;8ZIPs/Digests/34Einträge gelesen.18Bildinhalte
   bytegleich zu behaltenen Referenzen, keine Extraktionen. Nicht weiter pollen
   oder erneut lesen. Ebenso geschlossene Quelle0d3f4a65/42059129050 erhalten.
2. Fokuswechsel aus Suchinsel/anderen Controls, Capture außerhalb einer
   OS-Fenstergrenze und native Accessibility-/Screenreader-Aufgaben. Die neue
   Peerprobe verwendet einen eigenen Prozess derselben Binärdatei und geskriptete
   SDL-Tasten; keine vollständige physische/OS-Tastatur- oder assistive Abnahme.
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
Zusätzlich [acht lokale Bildkopien](verification/signature-focus-images-cleanup-2026-11-05.json)
und [14 Mac-Bildkopien](verification/filename-wrap-macos-cleanup-2026-11-05.json)
entfernt: zusammen29.123.036 Bytes Dateigrößen; tatsächliche NTFS-Zuordnung dafür
nicht gemessen. Kanonbilder und Mac-Original-ZIPs erhalten.
[Vier weitere identische Mac-Extraktionsbilder entfernt](verification/signature-detached-macos-cleanup-2026-11-05.json):
9.850.152Bytes, Original-ZIP und kanonische Bildinhalte erhalten.

Nur **D:**; F: weder lesen noch schreiben. Generiertes unter `build`; gemeinsame
WSL **Alpine-Dev**, VM, Offline-Abhängigkeiten, Nutzdaten und offene Fehler erhalten.
**Privater Nutzer-Mac AUS: keine SSH-/Erreichbarkeitsprobe oder Uploads.**
Gehostete GitHub-Runner sind getrennt. Vor späterem Nutzer-Mac-Zugriff die in
AGENTS genannten NexaCloud-/NexaUI-Anleitungen lesen.

Abgelehnte Löschpfade nicht erneut versuchen: `build/validation/documentation-links-2026-10-30/windows-build/obj`
und `build/validation/completion-c-2026-11-05/restored-portable-windows`.
Zusätzlich wurde die Löschung von `build/validation/signature-focus-2026-11-05/xauth-verified`
und `build/validation/signature-focus-2026-11-05/xauth-accepted` automatisch mit
„blocked by policy“ abgelehnt. Diese Ziele ebenfalls nicht erneut versuchen oder
über andere Werkzeuge löschen. Die22 Bildduplikate wurden zuvor separat entfernt.
Fremdes AT-SPI-/Orca-/Workflow-/Status-WIP, Referenz-/Tutorial-Zeilenendungen
und `dist/index.html`-Löschung erhalten. Eigene geprüfte Änderungen normal in
**origin und upstream** pushen; keine Force-Pushes. Root-Linkcheck findet einen
bekannten fremden Status-WIP-Link: ohne neue relevante Änderung nicht wiederholen.
