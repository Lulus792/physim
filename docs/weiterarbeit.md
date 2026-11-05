# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Datei, `git status` und relevante Diffs
lesen. Gesamtauftrag/Freigaben: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Vollständige UI und
Projektumsetzung bleiben offen; Teilprüfungen sind keine volle Punktzahl.

## Letzter geprüfter Schritt

[OS-Fokus im Zusatzfenster](verification/signature-focus-2026-11-05.md):
echter Windows-Fehler reproduziert, aktiven Quelleditor beim tatsächlichen
OS-Fokusgewinn wieder zuordnen. Windows Release **2/2 GUI-Gruppen run-8aio6i_b**,
Linux ASan/UBSan **2/2 run-dx4glpkf**, privater Xvfb-/Openbox-Desktop. C/Ph je
Modell-/Analyse-/Dateieditor, tatsächliche Fenster-/Peerprozess-Rückkehr, Textkanal,
Quelle/Cursor/Anker/Scroll und nichtleere Auswahl. Acht finale Bilder bytegleich
zu erhaltenen, bereits geöffneten Referenzen; nur neue Kopien entfernt.
[Originale, Fehlversuche und Grenzen](verification/signature-focus-2026-11-05.json).

Warmstand **signature-focus-accepted**,303 ausdrücklich erfasste Eingaben;
alle vier eigenen Quell-/Testdateien vor Build Root/Isolation/Manifest bytegleich.
Windows07535014… und Linux54f6e72f…; Source-/Runtimearchive vollständig gelesen.
[Quelle8bf467ce in beiden Remotes](verification/signature-focus-commit-2026-11-05.json),
alle1257 Eingaben gegen Index und tatsächlichen Commit gelesen.
Details: `build/validation/signature-focus-2026-11-05/STATE.md`.

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

1. Fokusquelle8bf467ce: gehosteter Editorlauf **42069271439 queued**, bei
   Beobachtung noch keine Jobs zugeteilt; erwartet je14 Daten+12 GUI. Ebenfalls
   queued zu derselben Quelle: Typografie42069010306, Workspace42068991869,
   natives Menü42068975965. Diese Handles verfolgen, nicht neu starten.
   [Source-/Run-Nachweis](verification/signature-focus-commit-2026-11-05.json).
   Geschlossenen Lauf42059129050 und seine22 Originaleinträge nicht erneut prüfen.
   Fehlende neue Architekturabnahme ehrlich offen halten.
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
