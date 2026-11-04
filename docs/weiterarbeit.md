# Wiedereinstieg in die native UI-Arbeit

Stand: 5. November 2026. Zuerst diese Übergabe, `git status` und relevante Diffs
abgleichen. Verbindliche Designs: [AGENTS.md](../AGENTS.md) und
[Gesamtübergabe](visuals/unified-workspace/NEXT-CHAT.md). Keine geschlossene
Prüfung oder Protokoll-Trennlinie allein wegen Kompaktierung erneut beginnen.

## Aktueller Anschluss: Automatikprüfer gegen Fristrennen gesichert

**Nutzer-Mac aus; keine Zugriffe/Erreichbarkeitsproben.**
**Eingefroren: `auto-info-deadline-final`, 174 eigene Eingaben / 1.124 erfasste
Compiler-/Testdateien. Alle lokalen Handles beendet. Quell-/Laufzeit-/Archivbytes
gelesen. Quellcommit `0c525422` in origin/upstream; 1.132 eindeutige Eingaben
gegen tatsächliche Git-Blobs abgeglichen, nur CRLF/LF normalisiert.**
[Commitabschluss](verification/auto-info-deadline-commit-2026-11-05.json).
Belegtes Rennen im Prüfer: Vor Zeichnung unsichtbare Pending-Karte → geplanter
Strg+Leertaste-Befehl → zwischenzeitlich gezeichnete Karte → Taste blendet sie
aus. Auswahl/Cursor/Quelle und Scrollposition bleiben gleich. Kontrollierter
Windows-C-Vorherfall **0/1**, `run-06uwoald`, Stufe 71; keine Linux-/Physim-
Vorherausführung behaupten. Original-ARM-Fehler hatte keine Mode-/Fristdaten;
den konkreten damaligen Ereignisablauf nicht als nachträglich beobachtet ausgeben.

Prüfer wartet nun innerhalb derselben 3-s-Stufengrenze auf `mode != PENDING`,
bevor er gegebenenfalls umschaltet. Keine Produktionsoptik/Controlleränderung,
keine gelockerten Daten-/Undo-/Geometrieassertionen, kein neuer Schlaf/Watchdog.
Test-only Fristvariante und sparsame Vor-/Nachzeichnungsdiagnose ergänzt.
Windows Release/Linux Debug ASan/UBSan zunächst je **GUI 3/3**, danach nur
Testregistrierungsformat bereinigt, endgültige Eingaben erneut je **GUI 3/3**:
`run-5wx3me0m` / `run-s96zafnf`. Appbytes je Plattform zwischen beiden positiven
Phasen gleich. C/Physim, normaler/erzwungener Automatikablauf und Info-Gruppe;
keine neuen Bilder. Register-/Harness-Selbstprüfung Exit 0; Mockfehler sind keine
Produktfehler. [Phasen, Originalbefunde und Grenzen](verification/auto-info-deadline-2026-11-05.md).

**Nächster konkreter Schritt:** gehosteten Editorlauf **`41935266700`** mit sieben
Gruppen anhand der Originalartefakte bewerten: ARM **`126942020658`**, Intel
**`126942021220`**, zuletzt beide queued. Keine aktuelle Mac-Abnahme behaupten,
keinen Ersatzlauf starten. Nutzer-Mac bleibt
unberührt. Danach native lokale Namen/Quellstellen-Karte/Einfügen/Undo/leere
Argumentauslöser, echte C-Scopes und separate Parameterhilfe fortsetzen.
Keine volle D-/UI-Punktzahl oder neue manuelle/assistive/DPI-Abnahme. Keine
geschlossenen F1-/Protokoll-/Settings-Matrizen wegen Kontextwechsel wiederholen.
Notiz: `build/validation/auto-info-deadline-2026-11-05/STATE.md`.

## Abgeschlossen: Physim-Scopes als Parsergrundlage

**Nutzer-Mac aus; keine Zugriffe/Erreichbarkeitsproben.**
**Eingefroren: `language-scopes-affinity`, 174 eigene Eingaben / 1.124 erfasste
Compiler-/Testdateien. Quellcommit `5d67db6e` in origin/upstream; 1.132 eindeutige
Compiler-/Test-/eigene Eingaben gegen tatsächliche Git-Blobs abgeglichen,
nur CRLF/LF normalisiert. Alle lokalen Handles beendet.**
[Commitabschluss](verification/language-scopes-commit-2026-11-05.json).
Echter Physim-Parser liefert aktive Deklarationsstapel samt Quell-/geschriebenen
Typbereichen, Parameter, Block-/Funktions-/Lambda-Grenzen, Iteration, bedingte
Bindungen, Guard, Switch-Nutzlast und nominales `self`. Rückwärtssuche wählt den
nächsten Namen; keine fertige eindeutige UI-Liste. Keine eigenen Namen in noch
offener Initialisierung, keine späteren/fremden Bindungen. Leerzeilen-Lookahead
liest ausschließlich Layout, nicht spätere Deklarationen. Strenger Parser bleibt
streng; kein AST aus einem unvollständigen Präfix zur Übersetzung verwenden.

Abschluss Windows Release/MSVC und Linux Debug/GCC ASan/UBSan je **Unit 5/5**,
`run-efq_y_cx` / `run-nurnwodj`: Parser, Checker, neue Bindings, Source-Outline,
Syntax. 36 konkrete Präfixe plus 1.024 deterministische Eingaben; jeder AST-
Einzelfeldwert mit strengem Einstieg verglichen. Direkter strenger C17-Test mit
`detect_leaks=1` bestanden. Drei echte Vorherfehler (Lambda-Variablenname,
exakte Puffergrenze, Leerzeilenzugehörigkeit) erkannt/behoben; isolierter Scope-
Rollback-Mutant abgelehnt. Alle Quell-/Laufzeit-/Archivbytes gelesen, keine Bilder.
**`--no-app`: keine neue App-/GUI-Abnahme, ältere App-Dateien bleiben archiviert.**
[Phasen, Originalkommandos und Grenzen](verification/language-scopes-2026-11-05.md).

**Nicht erledigt:** native Zusammenführung/Quellstellen-Karte/Einfügen/Undo/leere
Argumentauslöser, echte C-Scopes, semantisch nachgewiesene Typinferenz und separate
Aufrufhilfe einschließlich Benutzerfunktionen. Physische Eingabe/IME, AT,
DPI/Monitor/Vollbild/aktuelle Mac-Abnahme und volle D-/UI-Punktzahl bleiben offen.

Neue gehostete Evidenz zum **vorherigen** `f01f25c1`: Editor-Intel **6/6**, ARM
**5/6**; nur `completion_automatic_workflow` scheitert auf ARM an Stufe 71,
Ursache offen. Originalarchiv/Digest/alle Einträge gelesen; neuer Info-Ablauf
selbst bestanden. Intel-F1 **6/6** aktuell, frühere Timeouts damit nicht erklärt.
Settings `41928194543` und ARM-F1 `126918062826` zuletzt queued.
[Getrennter Mac-Bericht](verification/completion-info-macos-2026-11-05.json).
Keine Mac-Abnahme der neuen Parserquellen.
Neue gehostete Läufe zu `5d67db6e`: C17 **`41932070515`** pending,
Linux-UI **`41932070573`** in_progress, Mac-F1-Accessibility **`41932070548`**
pending. Kein neuer Editor-/Settings-/F1-Regressionslauf im API-Snapshot;
keine Jobnummer oder erfolgreiche Mac-Abnahme ableiten. Kein Ersatzlauf.

**Nächster konkreter Schritt:** zuerst ARM-Automatik-Stufe 71 gezielt
untersuchen, danach lokalen Scopebestand nativ integrieren und C ergänzen.
Hypothese noch unbelegt: Sichtbar-Zustand vor tatsächlicher Geometrie-/Viewport-
Stabilisierung beim Theme-/Schriftwechsel. Vor Korrektur konkrete Gegenprobe
oder stärkere Diagnose ergänzen; keine blinde Fristverlängerung oder gelockerte
Datenassertion. Keine geschlossenen F1-/Protokoll-/Settings-Matrizen wegen Kontext
wiederholen. Notiz: `build/validation/language-scopes-2026-11-04/STATE.md`.

## Abgeschlossen: Kurzinfo-Bedienung Windows/Linux

**Nutzer-Mac aus; keinerlei Zugriff oder Erreichbarkeitsprobe.**
**Eingefroren: `completion-info-tab`, 170 eigene Eingaben / 1.122 Compiler-
und Testdateien. Quellcommit `f01f25c1` in origin/upstream; alle 1.130 eindeutigen
Compiler-/Test-/eigenen Eingaben mit tatsächlichen Git-Blobs abgeglichen,
nur CRLF/LF normalisiert.** [Commitabschluss](verification/completion-info-commit-2026-11-04.json).
Manuelle Liste zunächst ohne Info, Auswahl-/Mausvorschau nach 280 ms,
`i`/Strg+Leertaste sofort und ohne neue Übernahmeauswahl. Exakte vollständige
Quell-/Editor-/Kandidaten-/Cursor-/Viewportbindung; kein alter Hinweis nach
Ausblenden. Alt+BildAuf/BildAb/Pos1/Ende scrollt die sichtbare Karte. Umschalt+Tab
verlässt das Codefeld ohne die vorher tatsächlich eingefügten vier Leerzeichen.

Abschluss Windows Release/Linux Debug ASan/UBSan je **Unit 1/1 + GUI 4/4**,
Windows `run-ot7qjiy2` / `run-zzc2kapy`, Linux `run-3ypvbd2x` / `run-7fh8kh0v`.
Jeweils identische App-Bytes zwischen Unit-/GUI-Aufruf. Neuer Ablauf C/Physim
in drei Editor-/Theme-Kontexten, echte Zeichnungs-/Datenbeobachtung, keine Bilder.
Vorherlauf Win 0/1; erste Integration beidseits 2/4 mit belegtem Shift+Tab-Fehler
und Messung vor Zeichnung. Keine Undo-/Daten-/Referenzassertionen entfernt.
Separater abschließender strenger C17-ASan/UBSan-Test mit `detect_leaks=1`
bestanden; GUI weiterhin `detect_leaks=0`. Alle lokalen Handles beendet,
Quell-/Laufzeit-/Archivbytes gelesen. Drei aktuelle Windows-Ansichten betrachtet.
Zwölf ersetzte erfolgreiche Rohbilder entfernt: **38.363.256 Bytes**;
Fehlerbilder/aktuelle Referenzen erhalten, eigener Bereinigungsbeleg verlinkt.
[Phasen, Originalkommandos, konkrete Fehler und Grenzen](verification/completion-info-2026-11-04.md).

Gehosteter **vorheriger** Quellstand `25dd37dc` nun beendet: Intel-Editor **5/5**,
Einstellungen **16/16**, F1 **5/6**; ARM jeweils cancelled, kein offener Job
dieser drei Läufe. Drei ZIPs/Digests/alle Einträge gelesen. F1 `docs-keyboard-h`
Timeout 60,089 s an Stufe 26, Ursache offen; getrennt vom früheren Stufe-39-Fall.
[Mac-Bericht](verification/completion-help-macos-2026-11-04.json).
Keine Mac-Abnahme der neuen Kurzinfo-Bedienung. Nutzer-Mac bleibt unberührt.
Neue gehostete Handles zu `f01f25c1`: Editor **`41928194536`**, Einstellungen
**`41928194543`**, F1-Regressionsgruppe **`41928194478`**, zuletzt queued;
C17 **`41928194403`** pending. Keine neue Mac-Abnahme oder gesamte grüne CI
behaupten. Konkrete Handles weiter beobachten, keinen Ersatzlauf starten.

**Nächster konkreter Schritt:** echte lokale C-/Physim-Scopes und separate
Aufrufhilfe einschließlich Benutzerfunktionen. Weitere Codekontexte,
physische Eingabe/IME, Screenreader, DPI/Monitor/Vollbild und aktuelle Mac-Abnahme
offen. Keine volle D-/UI-Punktzahl; geschlossene F1-/Protokoll-/Settings-Matrix
nicht wegen Kontextwechsels wiederholen. Aktuelle lokale Notiz:
`build/validation/completion-info-2026-11-04/STATE.md`.

## Abgeschlossen: Kandidateninhalt und Dokumentbesitz

**Nutzer-Mac aus; keine Zugriffe oder Erreichbarkeitsproben.**
**Eingefroren: `completion-help-restored`, 166 eigene Eingaben / 1.118 Compiler-
und Testdateien. Quellcommit `25dd37dc` in beiden Repositories; sämtliche
Compiler-/Test- und eigenen Eingaben mit tatsächlichen Git-Blobs abgeglichen,
nur CRLF/LF normalisiert.** [Commitabschluss](verification/completion-help-commit-2026-11-04.json).
Eigener Referenzseiten-Cache, echte Beschreibung, vollständige
vorhandene Argumenttabelle samt Originalüberschrift, Fehler ohne alten Inhalt,
gesperrter Referenzknopf und neuer Versuch nach Listenschließen. Neutrale Karte
mit 6-Einheiten-Vertikalleiste, keine unnötige Leiste bei kurzem Inhalt. Input
erreicht keinen darunterliegenden Bereich; Tastaturrückkehr erhält Undo.
[Umfang, Originalbefunde und Nachweise](verification/completion-help-2026-11-04.md).

Windows Release/Linux Debug ASan/UBSan: Cache-Stand `completion-help-routing`
je **Unit 3/3**, nach Fokuskorrektur `completion-help-focus` je **GUI 3/3**.
Anschließend nur Kartengeometrie und gezielte Prüfer geändert; Abschluss
`completion-help-bottom` je **GUI 2/2**, `run-l1ud0_3b` / `run-xbzg1kzk`.
C und Physim in drei Editoren/Themes, echte Beschreibungen, Referenzzugang,
Source/Undo, zwei echte Argumentzeilen und separate synthetische dritte
Renderer-Prüfzeile, Fehler/gesperrter Klick/Retry tatsächlich geprüft.
Aktuelle Cache-Unit separat mit ASan/UBSan und `detect_leaks=1` bestanden.
Keinen vollständigen neuen 3+3-Lauf mit letzter Binärdatei behaupten.

Isolierter Windows-Gegenfall `run-cgtbhjhm` **0/1**: nur Beschreibungszeichnung
unterdrückt; `summary=0`, Dokument/Referenz und alle Syntaxmerkmale weiterhin 1.
Root unverändert. Danach alle Quell- und 239 Windows-/129 Linux-Laufzeitdateien
exakt aus bestandenem Stand wiederhergestellt, sämtliche referenzierten ZIP-
Bytes geprüft; kein zusätzlicher identischer Testlauf erfunden. Alle lokalen
Handles beendet. Vier aktuelle native Bilder gesehen. 27 ersetzte Erfolgs-BMPs
entfernt (**86.317.326 Bytes**); Fehler/Gegenprobe/aktuelle Ansichten und eine
ursprüngliche visuelle Vergleichsaufnahme erhalten. Keine Objektordner löschen:
deren Namen sind Define-/Sprachvarianten, keine veralteten Quellstand-Hashes.

Kurzinfo-Verzögerung, Sofortzugang und Tastatur-Scrollen sind inzwischen oben
ergänzt. Danach echte lokale Scopes und separate Aufrufhilfe; keine volle D-/UI-Punktzahl.
Physische Eingabe/IME, Screenreader, DPI/Monitor/Vollbild und aktuelle Mac-
Completion-Abnahme offen. Keine geschlossene F1-/Protokoll-/Settings-Matrix allein
wegen Kontextwechsels wiederholen.

Gehosteter vorheriger Syntaxstand `f97344fb`: Einstellungen Intel/ARM **je 16/16**,
F1 ARM **6/6**, Intel **5/6**. Vier ZIPs/Digests/Originalergebnisse gelesen.
Intel `documentation_h`, Job `126856133610`, Zeitlimit 60,026 s an Stufe 39:
Ursache offen; dieser konkrete neue Befund erlaubt einen gezielten Nachlauf.
[Getrennte Mac-Evidenz](verification/syntax-roles-macos-2026-11-04.json).
Kein Nachweis der neuen Kandidaten-Kurzinfo; Nutzer-Mac bleibt unberührt.
Neue gehostete Läufe zu `25dd37dc`: Editor **`41919823783`**, Einstellungen
**`41919823552`**, F1 **`41919823808`** sind inzwischen beendet, siehe aktuellen
Mac-Bericht oben. Historischen Wartestatus nicht als aktuellen Handle verwenden.
Keine aktuelle Mac-Kurzinfo-Bedienungsabnahme oder gesamte grüne CI behaupten.

## Abgeschlossen: gemeinsame Syntaxrollen

**Nutzer-Mac weiterhin aus; keine Zugriffe oder Erreichbarkeitsproben.**
**Eingefroren: `syntax-roles-head-proof`, 163 eigene Eingaben / 1.115 Compiler-
und Testdateien. Quellcommit `f97344fb` in beiden Repositories; alle 1.115
Compiler-/Testdateien und 163 eigenen Eingaben mit tatsächlichen Git-Blobs
abgeglichen, nur CRLF/LF normalisiert.** [Commitabschluss](verification/syntax-roles-commit-2026-11-04.json).
Gemeinsame zehn freigegebene Rollen in Editor, Vorschlagsliste,
neutraler Karte und API-Codeblöcken, Funktionen gelb, Typen/Variablen getrennt,
echtes Semibold-Präfix. Echter Physim-Lexer/Typregistry, exakte Katalogsuche und
revisionseigener vorhandener Dokumentumriss; keine vollständigen lokalen Scopes.

`syntax-roles-final`: Windows Release/Linux Debug ASan/UBSan je **8/8 Unit +
5/5 GUI**. Danach echter Ph-Bereichsoperatorfehler mit scheiterndem Vorherlauf
behoben; ausschließlich Scanner + Syntax-Unit geändert. `syntax-roles-ranges`
je **1/1 Unit + 5/5 GUI**. Zuletzt nur Titel-Nachweis räumlich verschärft,
`syntax-roles-head-proof` je **1/1** (`run-u1wb6920` / `run-ryf85y0v`).
Produktionsbytes seit ranges unverändert; alle übrigen Eingaben seit final per
Hash gleich. Keinen neuen vollständigen 8+5-Lauf mit letzter Binärdatei behaupten.
[Originalkommandos, Phasen, Fehler/Gegenproben, Grenzen](verification/syntax-roles-2026-11-04.md).

Alle lokalen Handles beendet; Quell-/Runtime-/Archivbytes gelesen. Dunkel,
hell/große Schrift und Kontrast nativ betrachtet. 36 ersetzte erfolgreiche eigene
BMPs entfernt (**115.089.768 Bytes**), aktuelle zwölf Completion-Bilder und
Fehler/Referenzen erhalten; getrennte neue Pfade, frühere Einsparungen nicht addieren.
Kandidateninhalt/Dokumentbesitz inzwischen oben ergänzt. Offen: echte Scopes, separate Aufrufhilfe,
übrige Codekontexte und physische/Assistive-/DPI-/aktuelle Mac-Abnahme. D bleibt
unvollständig. Der aktuelle nächste Schritt steht im Kurzinfo-Anschluss oben.

Vorheriger Stand `913d60ed`: gehostete Intel-Editorfälle **5/5**, Settings **16/16**
tatsächlich abgeschlossen, beide Archive/Digests/Originalergebnisse gelesen.
ARM-Jobs `126806005373` / `126811425903` inzwischen **cancelled**, keine offene
Warteschlange oder bestandene ARM-Abnahme dieses Stands behaupten.
[Getrennter Beleg](verification/completion-d-macos-intel-2026-11-04.json).
Dies ist keine Mac-Abnahme der neuen Syntaxquellen. Nutzer-Mac bleibt aus.
Gehosteter Settings-Lauf `41910405173` zu `f97344fb` inzwischen Intel/ARM je 16/16;
Editorlauf im API-Snapshot noch nicht aufgeführt. Keine neue Mac-Abnahme oder
laufende Editor-Jobnummer ableiten, keinen Ersatzlauf ohne Anlass starten.

## Abgeschlossen: D-Viewport und wirksame Preferences

**Nutzer-Mac aus; keine Zugriffe oder Erreichbarkeitsproben.**
**Quellstand `913d60ed` in beide Repositories gepusht; alle 1.110 Compiler-/
Testdateien stimmen mit den tatsächlichen Git-Blobs überein.**
**Eingefroren: `completion-d-refined`, 155 eigene Eingaben / 1.110 Compiler-
und Testdateien.** D bleibt insgesamt unvollständig; keine volle Punktzahl.

Ergänzt: flache 400-Einheiten-Liste mit standardmäßig fünf ganzen Zeilen,
unabhängigem Scrolloffset und ziehbarem 6-Einheiten-Thumb. Alle Treffer erhalten,
Namen einzeilig, Typ rechts, Gesamtzähler und getrennte neutrale Kandidatenkarte
mit tatsächlicher Katalogsignatur/Modul. Referenzlink vollständig sichtbar.
Einstellungen A / Editor: automatische Vorschläge und sichtbare Zeilen 1–20,
Standard 1/5. PSPREF06 speichert/migriert 01–05; Übernehmen, Abbrechen,
Standardwerte, Suche und Änderungsprüfung sind verbunden. Manueller Zugang bleibt
bei ausgeschalteter Automatik wirksam. Frühere 140-ms-Eingabe-/IME-/Escape-/Undo-
Verträge bleiben erhalten. [Umfang und weiterhin offenes D](visuals/unified-workspace/COMPLETION-D-NATIVE.md).

`completion-d-final` Windows Release / Linux Debug ASan/UBSan je **Unit 2/2 +
GUI 5/5**, `run-ko9q17o5` / `run-b9elxr1a` und `run-2pr6qzoa` / `run-7qms13gs`.
Danach tatsächlicher Reflow-Gegenfall: gewählt 648, erste sichtbare Position 640,
eine Zeile. Korrektur bringt Auswahl bei veränderter Zeilenzahl ins Sichtfeld.
Gezielter Abschluss `completion-d-refined` je **Unit 1/1 + GUI 2/2**,
`run-wzwnoqh0` / `run-3m9ciqz6` und `run-vrm9pyps` / `run-qghdxkv9`.
Preferences-/Settings-Quellen einschließlich main.c und Design-Tokens seit der
vollständigen Gruppe unverändert; Nachweis über tatsächliche Hashes. Nicht einen
neuen vollständigen 2+5-Lauf mit der letzten Binärdatei behaupten.

Erste Funktionstests hatten den abgeschnittenen Referenzknopf übersehen. Native
Bildprüfung fand ihn, neue Bounds-Prüfung lehnt isolierten Höhenmutant wirklich ab;
Korrektur visuell betrachtet. Beide Gegenfälle bleiben erhalten. Große UI-/Code-
Schrift 22 mit Hell/Kontrast, Editorraumbegrenzung, ein/neun Zeilen, Rad ohne
Übernahme, Thumb-Drag und langsam getaktete Settings tatsächlich geprüft.
Alle lokalen Handles beendet, Quell-/Runtime-/Archivbytes gelesen. 23 ersetzte
BMPs entfernt (73.529.574 Bytes); Originalfehler, zwölf aktuelle manuelle Bilder
und ausdrücklich benötigte Referenzen behalten. Automatische Prüfung ohne Bilder.
[Originalkommandos, Phasen, Gegenfälle und Grenzen](verification/completion-d-2026-11-04.md).

**Weiter offen (Syntaxanschluss inzwischen oben ergänzt):** vollständige
Kandidatenkurzinfo, echte C-/Physim-Scopes und
separate Aufrufhilfe mit Benutzerfunktionen und Revisionen. Referenz-Pointerzugang
bei Ergänzung der Kurzinfo ausdrücklich mitprüfen. Physische Eingabe/IME,
Screenreader, DPI/Monitor/Vollbild und aktuelle Mac-Abnahme stehen aus.
Interaktiver `file:`-Browseraufruf durch URL-Richtlinie blockiert; kein Umweg.
Versionierte Freigabe/CSS und Referenz-PNG gelesen; keine Browserbedienung behaupten.

## Abgeschlossener Einstellungen-Tastaturanschluss

`4adf2699` ist in beide Repositories gepusht, 1.109 Compiler-/Testdateien
abgeglichen. Die fortschreitende Tastaturprüfung lief bei 250-ms-Frames in die
alte Gesamtgrenze von 20 s; nur `keyboard*` erhält 60 s, andere Modi bleiben bei
20 s und einzelne Tab-Traversierungen bei 28 Versuchen. Alle Speicher-/Default-/
Abbrechen-Assertions erhalten. Lokal je **2/2** mit vier App-Prozessen.
[Nachweise einschließlich früherer Harness-/Filterfehler](verification/settings-keyboard-2026-11-04.md).

Gehosteter Lauf `41871954744`: Apple Silicon Job `126721139251` tatsächlich
**16/16**, ZIP/Digest und vier Originalergebnisdateien gelesen.
[ARM-Beleg](verification/settings-keyboard-arm-2026-11-04.json).
Intel Job `126721139641` ist inzwischen **abgebrochen (`cancelled`)**; keine
ausgeführte Intel-Abnahme dieses Stands. Nicht als bestanden oder weiter laufend
behandeln. Ursache des Abbruchs nicht aus dem Status ableiten.
Dies prüft `4adf2699`, nicht den späteren Completion-Quellstand.
Vorheriger Mac-Stand `7064a38c`: Apple Silicon 15/15, Intel 13/14; [historischer
CI-Snapshot](verification/unit-focus-macos-2026-11-04.json).
Referenzgenerator-Korrektur **`d0d529de`** ist in beiden Repositories: vorhandene
Channel-Beispiele reproduzierbar, 31 Dokumente unter Windows/Linux geprüft;
WIP-Status-Link bleibt getrennt offen. [Abschluss](verification/continuation-2026-11-04.md).

## Abgeschlossener Einheitenfokus und kompakte Nachweise

**Eingefroren: `unit-focus-compact`, 151 eigene Eingaben.**
Der Einheitenfall ist tatsächlich reproduziert: Ein freier Fußleistenklick
übernahm den Fokus des verdeckten Symbolfelds. `unit_focus` prüft jetzt den
wirklichen sichtbaren Ausschnitt. Der neue reale Pointerfall bei 360 × 360
erhält SI-Fokus; Tab erreicht Schließen und Enter schließt die Aufgabe.
[Gegenprobe, Originalergebnisse und Umfang](verification/unit-focus-2026-10-31.md).

`unit-focus-clipped` besteht Windows Release/Linux Debug ASan/UBSan je **2/2**,
`run-9i0uur2k` / `run-ip6fpifj`: Fenster bei UI 16/22 plus alter C-/Physim-
Einheitenablauf (zusammen zehn App-Prozesse je Plattform). Danach ersetzt
`unit-focus-compact` nur ungenutzte BMP-Ausgaben durch Katalog-/Live-/Statistiktext,
je **1/1**, `run-dqqgwoof` / `run-4wghyt1d`, acht App-Prozesse. Kein neuer
Zweierlauf mit der letzten, nur in Testausgaben veränderten Binärdatei behauptet.
Alle lokalen Handles beendet, Quell-/Runtime-/Archivbytes geprüft.
20 ersetzte erfolgreiche Rohbilder (51.229.960 Bytes) nach genauer Pfad-/Hash-
Prüfung entfernt; erforderliche PNG-Referenzen, Fehler, Daten und Archive erhalten.

## Abgeschlossener Suchanschluss

**Eingefroren: `settings-search-clipped`, 151 eigene Eingaben.**
**`b20d550e` ist in beide Repositories gepusht; 1.109 Compiler-/Testdateien
stimmen mit dem Commit überein.** [Commitabgleich und offener CI-Snapshot](verification/settings-search-commit-2026-10-31.json).
Suchkopf „Suche löschen“ und Inhaltsaktion „Suche zurücksetzen“ sind getrennt;
sichtbare Tabfolge, Rückkehr zu Suche/Texteingabe und Entwurfserhaltung umgesetzt.
Das erste Suchzeichen zeichnet kein Löschsymbol mit alter Breitenreservierung.
Verdeckte, abgescrollte Kategorien dürfen keinen Headerklick/Fokus übernehmen.
[Umfang, ursprüngliche Fehler und Nachweise](verification/settings-search-2026-10-31.md).

Windows Release/MSVC und Linux Debug/GCC ASan/UBSan: Abschluss je **2/2**
(`settings_search_keyboard`, `settings_a_window`), `run-n2nnzihq` /
`run-z7knodp0`. Je vier App-Prozesse, UI 16/dunkel und 22/hell; echte
SDL-Texte, Tab/Shift+Tab, Aktivierung und beide Pointeraktionen. UI 22 stellt
Kategorie 21 hinter den Suchkopf und bestätigt den ignorierten verdeckten Treffer.

Inspektor mit beiden Fontquellen bereits auf `settings-search-ready` bestanden,
Windows `run-_8a5pztq` / Linux `run-m17aq13q`. Gesamt dort je **2/3**, weil der
damalige neue Suchprüfer noch scheiterte. Spätere Änderungen betreffen Suche;
kein neuer gesamter Dreierlauf mit der letzten Binärdatei behauptet.
Alle lokalen Build-/Prüf-/Archivhandles beendet; referenzierte Archivbytes gelesen.
Fehlerphasen behalten: frühe Python-/CLI-Registrierung, falscher nativer Gruppen-
Parameter, verfrühte Pointerkoordinaten, danach echte verdeckte Fokusübernahme.
Nicht pauschal als Produktfehler oder bestandene Abnahme umdeuten.

## Einheitenfenster und Mac-Nachweise

Eigenes Einheitenfenster seit **`657219a3`**, Commit in origin/upstream;
1.109 erfasste Compiler-/Testdateien abgeglichen. [Umsetzung](visuals/unified-workspace/UNIT-TASK-NATIVE.md),
[lokale fünf Gruppen und gezielte Nachläufe](verification/units-window-2026-10-31.md).
Einstellungen A bleibt seit `479a8954` ein eigenes Kategorienfenster;
[Verträge und offener Umfang](visuals/unified-workspace/SETTINGS-A-NATIVE.md).
Gemeinsamen Speicher-/Geometriebesitz aus `fa97b315` erhalten; bei Beenden
`settings_current` verwenden. Allgemeines Abbrechen schließt die Einheitenaufgabe nicht.

Nutzer-Mac **aus**, niemals SSH/Erreichbarkeit/Upload/Test anstoßen.
Auf `657219a3` sind beide gehosteten Settings-/Einheitenjobs `41177844214`
beendet, **je 12/13**: Header 3/3, Typografie 1/1, kombinierte Gruppe 8/9;
portabler Preferences-Schritt übersprungen. Nur erster cm-Inspektornachweis
scheitert an Selektor „m“ bei schon aktualisiertem Katalog/Wert „cm“.
Fixture wartet nun begrenzt auf die wirkliche neue Zeichnung; Daten-/Raster-
Konsistenz und Mindestzahlen nicht gelockert. Lokal beide Fontquellen bestanden.
Der korrigierte Stand **`b20d550e` ist inzwischen auf beiden Architekturen
abgeschlossen**: Settings/Suche/Einheiten `41180437523` je **15/15**;
separate Menüregression `41180437463` je **7/7**. Alle vier Archive, Digests und
Einzelergebnisse tatsächlich gelesen. [Abschlussbeleg](verification/settings-search-macos-2026-10-31.json).
Diese Matrix nicht wegen Kompaktierung wiederholen. Der eigene Nachlauf der
Einheiten-Fokusänderung `7064a38c` ist oben mit seinem offenen Intel-Fall erfasst.
Ältere Settings-/Mac-Menümatrix (9/9 auf `479a8954`, 7/7 auf `140ead03`) ist
ebenfalls geschlossen; nicht als Abnahme des späteren Quellstands ausgeben.

## Nächste konkrete Arbeit

- Läufe zu `913d60ed`: Intel Editor `41895571962` **5/5**, Settings `41895572088`
  **16/16** geschlossen; ARM jeweils cancelled. Keine Wiederholung wegen Kompaktierung.
  Vorheriger Editor `25662d6f`: Intel `126774542390` jetzt beendet **4/5**;
  C-Completion-App nach letzter Stufe 2→3 / keyboard=0 in 75-s-Timeout, Ursache offen.
  ZIP/Digest und alle Originalergebnisse gelesen, automatische Gruppe/API bestanden.
  ARM `126774543066` zuletzt wartend. Dies nicht als neue D-Abnahme ausgeben.
  Intel-Settings `4adf2699` wurde abgebrochen; ARM-Settings 16/16 geschlossen, nicht neu prüfen.
  Nutzer-Mac bleibt aus.
- D weiterführen: echte lokale Scopes, separate Aufrufhilfe und Assistive-Bedienung;
  aktueller Anschluss `build/validation/completion-info-2026-11-04/STATE.md`.
  Kurzinfo-Verzögerung/i/manueller Zugang, versteckte Trefferfläche und
  Tastatur-Scrollen oben geprüft; nicht allein wegen Kontextwechsel wiederholen.
  Referenz-Pointerzugang und Karten-Reflow dabei tatsächlich prüfen. Viewport
  und Preferences sind oben geprüft; nicht wegen Kompaktierung neu beginnen.
- Danach/verknüpft gemeinsame Syntaxrollen in weiteren Codekontexten sowie echte C-/Physim-Scopes und
  getrennte Aufrufsignatur einschließlich Benutzerfunktionen vervollständigen.
  Leere Argumente, Shadowing, unvollständige Initialisierung, verschachtelte
  Aufrufe und Revisionen gehören weiterhin zum vollen Auftrag. Kein begrenzter
  Browser-Lexer als nativer Ersatz, kein neuer Dienst/Provider-Worker.
- Historische Settings-/AX-Fixtures von Tab 6/Radios/globalem Fokus portieren.
  Wiederherstellung, Maximieren, Geometrie und Dateisicherheit erhalten. Der alte
  0/0-Protokollgriff ist kein neuer Befund an der behobenen Trennlinie.
- Unicode-Randleerzeichen im Einheitensymbol separat prüfen. Der konkrete
  `unit_focus`-Clipfehler ist jetzt mit Vorher-/Nachherlauf geschlossen.
- Weitere CI-Befunde bleiben getrennt offen: historischer `run_events`-Windows-
  Pfad `nul.psrun`, Floating/Spring-Workflow, Font-Demand und AX. Fremde laufende
  AX-/Orca-Arbeit nicht überschreiben und keine vollständige grüne CI behaupten.
- Übrige freigegebene UI: A2 Szene/Mehrfachauswahl, Kontextmenüs A, B2, weitere
  Aufgabenfenster. Manuelle Screenreader-/physische Eingabe-/DPI-/Monitor-/
  Vollbildabnahme und Gesamtgestaltung bleiben offen. Keine volle Punktzahl behaupten.

## Sicherer lokaler Anschluss

- `prepare-next.py <neuer-tag> auto-info-deadline-final` prüft vorher Quelle,
  Runtime und Archive. Neue Module ausdrücklich in die Eingabeliste aufnehmen.
  Eigene Quellen vollständig versionieren, fremde AT-SPI-/Orca-/Status-/dist-
  Änderungen erhalten und nicht in eigene isolierte Quellen übernehmen.
- Helfer/Indizes: `build/validation/explorer-inline-2026-10-29/`; aktueller
  Arbeitsnachweis: `build/validation/auto-info-deadline-2026-11-05/`; Scopes unter
  `build/validation/language-scopes-2026-11-04/`; Kurzinfo unter
  `build/validation/completion-info-2026-11-04/`; vorherige Inhalte
  unter `build/validation/completion-help-2026-11-04/`; Syntax unter
  `build/validation/syntax-roles-2026-11-04/`; frühere Completion
  unter `build/validation/completion-auto-2026-11-04/`; Settings unter
  `build/validation/settings-keyboard-2026-11-04/`; Einheitenfokus
  unter `build/validation/unit-focus-2026-10-31/`; älterer Suchstand
  unter `build/validation/settings-search-2026-10-31/`.
  `run.py`/`freeze.py` vor Verwendung auf tatsächliche Argumente prüfen.
- Isolierter Baum `build/validation/protocol-wrap-2026-10-26/3-isolated/`
  mit `src`, `windows-build`, `linux-build`; keine Mutation bei lebendem Handle.
- Nur D:, kein Zugriff auf F:. Generiertes unter `build/`; WSL-Datenträger,
  benötigte Offline-Abhängigkeiten, Nutzerexperimente und fremde Arbeit erhalten.
  Vorige abgeschlossene Bereinigungen: Einheiten 315 Dateien/563.430.107 Bytes,
  davor Settings 656 BMPs/2.093.854.368 Bytes; danach Einheitenfokus 20 BMPs/
  51.229.960 Bytes und Completion 36 BMPs/115.089.768 Bytes. Nicht doppelt zählen. Fehler und benötigte Referenzen schützen.
- Text/JSON mit Quelle/Kommando/Soll-Ist zuerst; Bilder gezielt. Vier finale
  Such-PNGs für Fokusdarstellung reichen; nicht jeden Eingabeschritt fotografieren.

## Abgeschlossen – Umfang und Grenzen erhalten

| Teil | Ergebnis und Nachweis | Verbleibende Grenze |
| --- | --- | --- |
| Typografie und nativer Header | `68ab5f71`, `41158796808`: Intel/Apple Silicon je 4/4, zwölf UI-Größen-/Lese-/Abbruch-/Default-Prozesse und zwölf echte Menüabbrüche. Lokal je drei unveränderte Theme-Fälle plus gesondert bestandener Typografienachlauf. [Nachweis](verification/macos-typography-menu-2026-10-31.json). | Editorerhaltung mit realem Standard 12 px, keine vollständige Sechs-Editorgrößenmatrix; Einstellungen A ist dadurch nicht umgesetzt. |
| Mac-Studien-/Projektmenüs | `725dd84e`, `41156020989`: Intel/Apple Silicon je 5/5 mit je neun echten AppKit-Menübefehlen; lokale erste Läufe Windows 4/5 und Linux 5/5, danach reparierte Batch-Fixture je 1/1. [Nachweis](verification/macos-study-menu-2026-10-31.json). | Kein einzelner neuer lokaler Vollaufruf mit einer Binärdatei behauptet; Weitere Bereichsfokusabläufe und gesamte native UI-Abnahme bleiben offen. |
| F1-Reparaturen auf Mac | `b648b0e7`, `41155920224`: Intel/Apple Silicon je 5/5 GUI, 1/1 portable Fontquelle, 3/3 Metriktests und drei gezielt abgelehnte Gegenbeispiele; je 56 H-Geometrien/24 Bereichszustände. [Nachweis](verification/macos-f1-repairs-2026-10-31.json). | Gezielte Teilabnahme; keine manuelle VoiceOver-/Vollbild-/physische Menüleistenbedienung oder gesamte GUI-Suite. |
| Mac-Dateitabs/Projektmanager/Einstellungen | `7a95c8ae`: Intel/Apple Silicon je 4/4 mit je 68 tatsächlich gebauten/analysierten Projekten; Windows/Linux gleiche Abdeckung nach separat bestandenem Python-Prüfernachlauf mit unveränderter App. [Nachweis](verification/macos-workspace-menu-2026-10-31.json). | Übrige aktive Menütests und gesamte GUI-/physische Abnahme bleiben offen. |
| Mac-Dokument-/Completion-/API-Menüeingaben | `c35fc39d`: Intel/Apple Silicon, Windows Release und Linux Debug ASan/UBSan je 4/4 registrierte Fälle / sechs App-Unterprozesse. Gleicher Mac-Stand zusätzlich je 7/7 Menüregression. [Nachweis](verification/macos-editor-menu-2026-10-31.json). | Gesamte GUI-/Design-/VoiceOver-/physische DPI-Abnahme und andere aktive historische Menütests offen. |
| Historische Mac-Toolbar-/Tastaturabläufe | `8d75cee7`: Intel/Apple Silicon je 7/7 einschließlich Menüadapter und App-Start; Windows Release/Linux Debug ASan/UBSan je 2/2. [Nachweis](verification/macos-menu-workflows-2026-10-31.json). | Übrige historische Workflows, manuelle VoiceOver-/Vollbildprüfung und physische Großbild-/DPI-Abnahme offen; ARM-Großvariante hatte 1080×740. |
| F1-Link-Gate und Offline-Lizenzen im SDK | `ed3866ed`: 188 Themen/234 Abschnittslinks unter Windows/Linux; Herkunft und beide Lizenzen tatsächlich per F1-Tastatur geöffnet, Quellenadresse kopiert; SDK-Umzug mit fehlenden Dateien als Gegenprobe. Intel/Apple Silicon je 4/4. [Nachweis](verification/documentation-links-2026-10-30.json). | Gesamte GUI-/Inhalts-/Screenreader-Abnahme bleibt offen; fremde Status-Ergänzung nicht mit geprüft. |
| macOS-Systemmenüleiste | `d43dcd6c` + `517021f0`; je 2/2 native Fälle Intel/Apple Silicon, je 4/4 Menü-/Headerfälle plus Start unter Windows/Linux. [Kompakter Nachweis](verification/macos-menubar-2026-10-30.json). | Manuelle Vollbild-/VoiceOver-Prüfung und übrige historische Mac-GUI-Workflows offen; Toolbar/Tastatur siehe eigene Zeile. |
| Trennlinie oberhalb des Protokolls | **Behoben seit `ef1f3321`.** Dauerhaft 1 px, gemeinsame Konturfarbe; Akzent bei Hover/Fokus. [Ursprüngliche Korrektur und negative Kontrolle](verification/protocol-separator-2026-10-29.json). [Nachlauf 30.10.](verification/protocol-boundary-workspaces-2026-10-30.json): Windows Release und Linux Debug ASan/UBSan, je 12/12 Zustände in Entwickeln, Simulieren, Auswerten. | Große Schrift im Nachlauf: 22 px; keine neue macOS-/DPI-/Screenreader-Abnahme. |
| A2 für Textauswahl | Gemeinsamer Textfeldrenderer, Commit `c12e2a10`. [Umfang und native Belege](visuals/unified-workspace/TEXT-SELECTION-A2-NATIVE.md). | Belegt keine vollständige A2-Umsetzung aller Objektarten. |
| A2 für Protokoll- und Explorerlisten/-kacheln | Commit `d228b16e`. [Native Umsetzung](visuals/unified-workspace/LIST-SELECTION-A2-NATIVE.md), [Windows-/Linux-Prüfungen und sechs native Gegenproben](verification/list-selection-a2-2026-10-30.json). | Mehrfachauswahl bleibt offen. |
| A2 für Workspace- und Szenenbaum | [Umsetzung](visuals/unified-workspace/TREE-SELECTION-A2-NATIVE.md), [Nachweis 30.10.](verification/tree-selection-a2-2026-10-30.json): je zehn tatsächliche Zustände Windows/Linux, sechs native Gegenproben, zusätzlich 72 Linux-Pendellaufansichten. | Einzelauswahl; 32 px sind eine gezielte Rendererbelastung, keine neue Einstellungsoption oder reale 200%-DPI-Abnahme. |
| F1: ungültige synthetische Umschalttastenereignisse unter Linux | Commit `8840b9c9`: SDL-Ereignisse ausdrücklich initialisiert; sieben Linux-Kontextfälle bestanden. | Eigenständige Windows-Fokusbefunde bleiben offen; nicht als gleicher Fehler behandeln. |


## Umgebung und fremde Arbeit

Der Nutzer-Mac ist seit seiner Meldung vom 31.10. **aus**. Keine SSH-/Probe,
Uploads oder lokale Mac-Aktionen ohne neue Verfügbarkeitsmeldung. Die neuen
Mac-Nachweise stammen ausschließlich von gehosteten GitHub-Runnern. Vor späterer
Nutzung des Nutzer-Mac `D:/NexaCloud/docs/MACOS_TESTING.md` und aktuelle Mac-Hinweise
in `D:/NexaUI/AGENTS.md` lesen; eigene Reste gezielt aufräumen, gemeinsame Werkzeuge
erhalten. Manuelle VoiceOver-/Vollbild-/physische Menüleisten-/DPI-Abnahme und
gesamte GUI-/Designabnahme bleiben offen; Teiltests beweisen keine volle Punktzahl.

Nur D: für Quellen, Builds, Tests, Archive und temporäre Daten; **F: weder lesen
noch schreiben**. WSL ausschließlich `Alpine-Dev`; `D:/WSL/Alpine-Dev/ext4.vhdx`
und Offline-Abhängigkeiten/Referenzen erhalten. Details in
[Speicheranleitung](development-storage.md). Die automatisch abgelehnte Löschung
von 64 `.obj` / 6.103.831 Bytes unter `build/validation/documentation-links-2026-10-30/windows-build/obj`
wurde nicht ausgeführt. Nicht über andere Shell/Kodierung umgehen.

Paralleles WIP in AT-SPI-/Orca-/Workflow-/Statusdateien sowie die vorhandene
Löschung von `dist/index.html` getrennt erhalten. Nicht pauschal stagen, zurücksetzen
oder als mit geprüft erklären. Die isolierten Nachweise betreffen den angegebenen
Git-Stand mit ausschließlich eigenen Änderungen. Die fremde Status-Ergänzung
verweist auf einen noch nicht registrierten Bereinigungsbericht; deshalb keine
pauschale Link-/Gesamt-CI-Abnahme. Eigene abgeschlossene Änderungen normal committen
und in `origin` und `upstream` pushen; keine erzwungenen Pushes.

## Konzept für verbundene Rechengeräte · 4. November 2026

[Konzept und UI-Skizzen](remote-computing-concept.md) halten den Nutzerwunsch
für entfernte Einzelläufe, verteilte Serien, explizite Geräteverknüpfung und
vergleichbaren Leistungstest fest. Konzeptstand, keine Designfreigabe oder
Implementierung; offene Architektur-, Sicherheits- und Plattformfragen stehen dort.
Nächster Schritt: Bedienideen mit dem Nutzer besprechen und offene Entscheidungen
klären. Die oben genannten nativen Aufgaben und offenen Nachweise bleiben bestehen.

## Späterer Ausbau: Rechnen & Geräte · 4. November 2026

Nutzerpriorität präzisiert: [entferntes Rechnen](remote-computing-concept.md) ist
als späterer Plan aufgenommen; aktuelle UI-/Oberflächenarbeit hat Vorrang.
[Interaktive A/B/C-Entwürfe](visuals/compute-device-options/README.md) zeigen
Gerätekopplung und Leistungstest. Auswahl offen; keine Freigabe oder native
Implementierung. Nächster Schritt dieser Runde: Entwürfe mit dem Nutzer besprechen.

## Gewählt für später: Rechnen & Geräte A · 4. November 2026

Der Nutzer hat **A · Geräteliste** zur späteren Übernahme festgelegt.
[ADR 025](decisions/025-selected-compute-devices-a.md) und
[Spezifikation](visuals/compute-device-options/APPROVED.md) sind maßgeblich;
A/B/C-Auswahl abgeschlossen, B/C nur Historie. Frühere Offen-Vermerke oben
sind damit überholt. Aktuelle native UI-Arbeit bleibt vorrangig; anschließend
die offenen technischen Konzeptfragen klären und A ohne neue Layoutwahl umsetzen.
Keine neue Implementierung oder Plattformprüfung durch diese Dokumentation.
