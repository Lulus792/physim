# F1-Schriften nach Bedarf und Besitz der Atlanten

Architekturentscheidung vom 15. Oktober 2026. Der zusätzliche Atlasbesitz im
Backend und F1s Größenwahl verwenden den Aufbau nach Bedarf.
Die unten genannten Nachweise gelten jeweils für ihre Revision und Plattform;
die aktuelle Gesamtbewertung steht in [004](004-f1-handbook.md).
Die Entscheidung folgt dem zentralen
`software-architect`-Skill unter `D:/Skills/skills` und den tatsächlich
ausgeführten Messungen in [004](004-f1-handbook.md).

## Problem und unveränderte Anforderungen

Der frühere F1-Pfad erzeugte beim Öffnen 38 primäre Schriftvarianten samt ergänzenden Quellen:
neun normale, neun Titel-, elf fette und neun Codegrößen. In der anfänglichen
Ansicht wird nur ein Teil dieser Größen gebraucht. Im gemessenen ASan-Lauf
braucht allein die CPU-Berechnung des F1-Atlas 6,325 Sekunden; Prüfung und
Upload benötigen zusammen 0,013 Sekunden. Der R8-Atlas spart Speicher, reicht
aber für verlässlich kurze Bereitschaft nicht aus.

Alle bisherigen Codepunkte, echten Schriftquellen, Rollen, Größen bis 32 Pixel,
H2-Größen bis 36 Pixel, Oversampling und Dichten müssen erhalten bleiben.
Ein fehlgeschlagener Aufbau darf weder alte Handles noch lesbaren Inhalt,
Zoom, Auswahl, native Inhaltsversion oder Scrollposition beschädigen.
Die vorhandenen Bereitschafts- und Clientgrenzen bleiben bestehen.

## Verglichene Strukturen

| Struktur | Vorteil | Konkretes Risiko |
| --- | --- | --- |
| Ein Atlas nur für die gerade benötigten Rollen und Größen, bei jedem Zoom vollständig ersetzen | Weniger Eigentümer und weitgehende Wiederverwendung des vorhandenen Ersatzpfads | Jeder Zoom wiederholt bereits geleistete Arbeit; ein Austausch während eines bereits begonnenen Frames kann dessen alte Textbefehle ungültig machen. Erfordert zusätzlich eine sauber verzögerte Größenübernahme. |
| Fehlende Varianten in zusätzlichen Atlanten berechnen und bereits veröffentlichte Atlanten bis zum Dichtewechsel behalten | Einmal berechnete Größen bleiben nutzbar; bestehende Textbefehle und Handles bleiben während Zoom gültig | Mehrere GPU-Ressourcen müssen begrenzt, eindeutig besessen und gemeinsam beim Dichtewechsel oder Schließen freigegeben werden. |

Für die Umsetzung wird die zweite Struktur gewählt. Der Renderer kann bereits
pro Textbefehl verschiedene Fonttexturen verwenden; `nk_sdl` besitzt aktuell
den Atlas, die Fonttextur und das weiße Nulltexel. Das zusätzliche Eigentum
gehört daher ebenfalls in `nk_sdl`, nicht in den Dokumentparser oder die
nativen Accessibility-Brücken. Die Anwendung entscheidet über benötigte Rollen
und Größen; das Backend verantwortet Bake, Validierung, Upload und Freigabe.

## Vertrag und Umsetzungsstand

- Beim Öffnen werden nur die benötigten normalen, Titel-, Code- und fetten
  Varianten einschließlich der größeren H2-Rolle vorbereitet. Jede Variante
  erhält weiterhin den vollständigen bisherigen Zeichenvorrat.
- Ein Ergänzungsauftrag baut ausschließlich fehlende Varianten. Alte Atlanten
  und bereits aufgezeichnete Befehle bleiben gültig. Erst nach vollständigem
  Erfolg werden neue Handles veröffentlicht; ein Fehler verwirft nur den
  Kandidaten. Das Backend nimmt nur unberechnete Kandidaten an; nach einem
  fehlgeschlagenen Upload verbleiben berechnete Daten im Besitz des Aufrufers.
  Dieser muss sie freigeben, bevor er einen neuen Kandidaten vorbereitet.
- Die Zahl der ergänzenden Atlanten ist durch die Zahl der möglichen Varianten
  begrenzt. Es gibt kein unbegrenztes Wachstum pro Zoomereignis oder Fontgröße.
- Bei neuer Rasterdichte wird der aktuell benötigte Satz vollständig vorbereitet.
  Erst anschließend werden Stil und Handles übernommen und alle Atlanten der
  alten Dichte gemeinsam freigegeben. Eine gescheiterte Vorbereitung erhält
  den vollständigen bisherigen Satz.
- Zoom und Anwendungseinstellungen dürfen erst nach erfolgreicher Vorbereitung
  auf Handles einer neuen Größe verweisen. Der GL-Kontext muss dabei explizit
  der Dokumentationsansicht gehören; Einstellungen werden teilweise aus dem
  Hauptfenster heraus angewendet.
- Die Zeicheninventur muss weiterhin sämtliche 38 Varianten tatsächlich
  aufbauen und prüfen können. Sie darf fehlende Größen weder überspringen noch
  durch anders große Handles ersetzen.

## Umsetzung und erforderliche Nachweise

Das Backend besitzt jetzt bis zu 38 zusätzliche Atlanten. Die gemeinsame
Upload-/Validierungsroutine wird auch vom bisherigen Dichteersatz verwendet.
F1 lädt beim Öffnen die fünf benötigten Varianten. Zoom und unabhängige
UI-/Code-Einstellungen ergänzen ausschließlich fehlende Varianten. Die alte
Größenwahl bleibt bis zur erfolgreichen Vorbereitung gültig. Einstellungen
bereiten Fonts vor dem Schreiben der Einstellungsdatei vor; fehlgeschlagener
Fontaufbau verändert diese Datei nicht. Ein erfolgreicher neuer Versuch entfernt
die zugehörige Fontfehlermeldung, ohne andere Dokumentfehler zu löschen.
Die Vorbereitung stellt den zuvor aktiven GL-Kontext wieder her.

Dichteersetzung und vollständige Inventur verwenden denselben Besitzvertrag.
Die Inventur fordert ausdrücklich alle 38 realen Varianten an; normales Öffnen
reduziert ausschließlich die vorab geladenen Größen, nicht deren Zeichenvorrat.

Vor einer Abnahme sind erforderlich: tatsächliche Kaltstart-/F1-Zeiten mit
derselben Schriftabdeckung, alle 38 Varianten mit System- und mitgelieferten
Schriften, wiederholter Zoom einschließlich Zurücksetzen, unabhängige UI-/Code-
Einstellungen, 1/1,25/1,5/2-Dichterundlauf, vorher erzeugte gültige Textbefehle,
gezielte Allokations-/Uploadfehler und vollständige Freigabe. GPU-Bilder und
native Lese-/Feld-/Linkabläufe müssen auf den jeweils ausgeführten Plattformen
weiterhin bestehen. Die bisherigen Backend-Nachweise ersetzen keine Abnahme
des vollständigen Produkts oder einer noch ungeprüften Zielplattform.

## Bisheriger Backend-Nachweis

Der echte GPU-Fall `font_bank_ui` zeichnet Text, ergänzt einen Atlas bei noch
aufgezeichneten Textbefehlen und verlangt anschließend bytegleiche BMP-Ausgabe.
Er zählt tatsächlich helle Textpixel und zeichnet auch mit dem ergänzten Font.
Er prüft die Kapazitätsgrenze, abgewiesene andere Rasterdichte, fehlgeschlagene
Allokation, erhaltenen Bestand bei fehlgeschlagenem Dichteersatz und einen
erfolgreichen Dichtewechsel. Zählende Allokatoren und echte `glIsTexture`-
Abfragen prüfen vollständige Freigabe beim Ersatz sowie beim Schließen.

Der erste Stand besteht unter MSVC ASan zusammen mit dem bisherigen
F1-Dichterundlauf in `run-lx6hmyx9` **2/2**, unter GCC 15.2 ASan/UBSan mit
X11/Openbox zusammen mit dem GPU-Renderer in `run-tanthpk0` **2/2**.
Die Windows-Bilder vor/nach Ergänzung und mit ergänzter Schrift wurden geprüft.
Der Plattform-Katalog samt absichtlichen Fehlergegenproben besteht in
`build/native/font-bank-runner-proof`.

Eine zusätzliche kontrollierte GL-Fehlergegenprobe findet anschließend eine
Retry-Lücke (`run-o8ix6moi`): Ein Kandidat mit bereits berechneten Puffern wurde
erneut angenommen. Da Nuklears Bake diese Puffer nicht vor einem neuen Bake
freigibt, könnte dies den alten Speicherbesitz überschreiben. Die gemeinsame
Vorprüfung weist solche Kandidaten jetzt ohne weiteren Aufbau zurück.
Die Gegenprobe kontrolliert außerdem, dass die beim Grafikfehler neu erzeugte
Textur gelöscht wird und der zuvor installierte Font gültig bleibt. Dies ist
eine kontrollierte GL-Fehlerweitergabe, keine vorgetäuschte echte GPU-OOM-Prüfung.

Die erste Korrektur macht eine zweite Freigabelücke sichtbar (`run-u_k1g7uw`):
`nk_font_atlas_clear` gibt den Pixelpuffer zwischen erfolgreichem Bake und
fehlendem `atlas_end` bisher nicht frei. Der vendorte Clear-Pfad gibt jetzt auch
diesen temporären Puffer frei. Der native Grenztest bricht beide Atlasformate
gezielt vor `atlas_end` ab und verlangt jeweils null verbleibende Allokationen.

Der finale Windows-Stand besteht mit MSVC ASan in `run-w5tcouw4` **3/3**
native Schriftfälle und in `run-fph31f8x` **3/3** GUI-Fälle: Atlasbesitz samt
kontrolliertem Grafikfehler, vollständige F1-Zeicheninventur mit System- und
mitgelieferten Fonts sowie zehnfacher unabhängiger Link-Stresstest.
Die finale Katalog-/Fehlergegenprobe besteht in
`build/native/font-bank-final-runner-proof`.
Der finale GCC-15.2-ASan/UBSan-Stand unter Alpine 3.24.2 besteht native
Schriftfälle in `run-f9mjpamq` **3/3** und unter X11/Openbox Atlasbesitz,
GPU-Renderer sowie unabhängige AT-SPI-Links in `run-8p39b1tp` **3/3**.
Der zentrale `code-reviewer`-Skill wurde auf Eigentumsübergabe, Lebenszeiten,
Abbruch und Ersetzen angewendet. Die beiden dabei konkretisierten Fehlerfälle
sind durch die genannten Gegenproben und finalen Erfolge eingegrenzt.
Eine macOS-Ausführung des neuen Atlasbesitzes steht noch aus; die angepasste
Workflow-Konfiguration ist allein kein Nachweis dafür.

## F1-Integration und tatsächliche Abnahme

Der erste Integrationslauf `run-ch6pbmjz` scheitert **0/4**: Der Standardfont
wurde vor dem Bake aus einem erst beim Bake initialisierten Handle gelesen.
Die Korrektur hält den tatsächlichen `nk_font`-Zeiger aus dem Add-Aufruf fest.
Danach bestehen Windows-Linkclient, Zeicheninventur, Zoom und Dichte in
`run-4zc48xgn` **4/4**. Die Inventur baut sowohl mit Systemschriften als auch
ohne Systemschriften alle 38 Varianten auf. Bei normalem Öffnen meldet derselbe
Lauf fünf Varianten und einen F1-Bake von 1,301519 Sekunden.

Der neue echte App-Fall `documentation_font_demand` ergänzt gezielte
Allokationsfehler nach bereits begonnenem Aufbau. Er bestätigt unveränderte
aktive Handles, Zoom, native Inhaltsversion, GL-Kontext und Einstellungsdatei.
Nach erfolgreichem erneutem Zoom bleiben alte Fontzeiger nutzbar. Ein Wechsel
auf UI-Größe 22 und Codegröße 18 erhöht den Bestand von fünf über zehn auf
13 Varianten. Die Rückkehr auf die Anfangsgrößen verwendet bestehende Handles.
Der zusätzliche 2/1-Dichterundlauf ersetzt den gefüllten Cache jeweils durch
fünf passende Varianten und erhält logische Größe, Lesetext und Editorzustand.

Tatsächlich ausgeführt, jeweils mit den bisherigen Zeitgrenzen:

- MSVC ASan: `run-f8izfahj` **4/4**, Bedarfstest, beide Einstellungsabläufe und
  Navigation. `run-17g6yjzd` **29/29** umfasst alle ausgewählten F1-GUI-Fälle,
  beide Einstellungsabläufe und sämtliche ausgewählten nativen Windows-Clients.
  Enthalten sind verzögerte Tastaturbedienung, der erwartete unerreichbare
  Fokusgegenfall, Schließen/Wiederöffnen, Textfelder, Lesetext und Links samt
  zehnfacher Fokus-/Fensterbewegung.
- Die nachfolgende Erweiterung um den gefüllten Dichtecache besteht in
  `run-scefp_ru` **1/1**. Die neue gemeinsame Prüfung von PNG-CRC, Dimensionen,
  Scanlines und tatsächlichen Textpixeln besteht in `run-trrn7fj7` **2/2** für
  Bedarf und bisherigen Dichtefall. Fehler-, Einstellungs- und Dichtebilder
  wurden unmittelbar geprüft.
- Alpine 3.24.2 / GCC 15.2 ASan/UBSan / X11/Openbox / Software-GL:
  `run-zaz1o0wk` **10/10**, einschließlich Atlasbesitz, vollständiger
  Zeicheninventur, gefülltem Dichtecache, Zoom, Navigation, Seitenleiste und
  unabhängigen AT-SPI-Lese-, Feld- und Linkclients. Die Fehleransicht und das
  tatsächliche 1520 × 1080-Dichtebild wurden geöffnet und geprüft.
- Die gemeinsame PNG-Prüfung besteht zusätzlich für vier Originalbilder aus
  beiden Plattformläufen und weist eine absichtlich beschädigte CRC ab;
  Bericht `build/native/font-demand-pixel-proof.json`.
- Plattform-Katalog und absichtliche Fehlergegenproben bestehen in
  `build/native/font-demand-final-runner-proof`.

Drei serielle unabhängige Windows-Linkläufe ohne parallelen Produkt-Build
bestehen anschließend vollständig in `build/native/font-demand-links-01` bis
`-03`. Bereitschaft: **5,093 / 7,266 / 5,609 Sekunden**; F1-Bake:
**0,555714 / 0,708286 / 0,572180 Sekunden**. Alle drei bauen fünf Anfangsvarianten
auf und behalten die 20-/25-Sekunden-Grenzen. Der vollständige Bericht liegt in
`build/native/font-demand-link-series.json`. Dies sind gemessene Läufe auf der
genannten Umgebung, keine Garantie unter beliebiger Rechnerlast.

Die frühere Windows-Fokussporadik wird durch erfolgreiche Wiederholungen
nicht als behoben erklärt. Die nachfolgende Ausführung auf `188a411` besteht
inzwischen auf macOS Intel und ARM64 jeweils **10/10 native und 13/13 GUI-Fälle**,
einschließlich tatsächlicher Schriftschnittwahl, Bedarf und Dichte.
[Lauf und begrenzte Abnahme](004-f1-handbook.md). Dies ist keine Abnahme der
danach ergänzten Tabellenadapter oder praktischer Screenreader-Aufgaben.

## Wiederholte Quellenanalyse und kontrollierter Vergleich

Der Intel-macOS-Lauf auf `e6773c5` überschritt im ersten Mathematikfall die
unveränderte 45-Sekunden-Grenze. Seine Spur weist für die ersten fünf F1-
Varianten 10,595 Sekunden Quellenvorbereitung, 15,679 Sekunden Bake und
1,619 Sekunden Prüfung/Upload aus. Das Hauptfenster hatte davor weitere
Schriftarbeit. Der nachfolgende Lauf auf `e8042e4` besteht wieder vollständig;
damit ist die Ursache der sporadisch hohen Bereitschaftszeit noch nicht behoben.
Die tatsächlichen Läufe und Grenzen stehen in [004](004-f1-handbook.md).

Der zentrale `performance-engineer` wurde zusätzlich angewendet. Im bisherigen
Fallbackaufbau initialisiert die Abdeckungsfrage eine bereits vorhandene
Schriftquelle für viele einzelne Kandidaten erneut. Die begrenzte Änderung
initialisiert jede vorhandene Quelle einmal pro Ergänzung, entfernt bereits
abgedeckte Kandidaten und bildet aus den übrigen Codepunkten dieselben
Bereichspaare. Der vorhandene temporäre Bereichsspeicher reicht dafür aus;
Allokationszahl, Quelldateien, Schriftschnitte, Glyphenpriorität, Abdeckung,
Oversampling und Atlasbesitz bleiben erhalten. Dies ist eine Optimierung der
wiederholten Analyse, keine Verringerung des Schriftangebots.

`tools/profile_font_prepare.py` verwendet die tatsächliche
`system_font_sampling`-Funktion aus `app/main.c`. Der Workload entspricht den
fünf Anfangsvarianten bei Dichte 2 mit allen bisherigen Bereichen und derselben
Abtastung. Die alte Ergänzungsfunktion aus `e8042e4` und die neue laufen in
einem Prozess abwechselnd je dreimal. Die übrige Nuklear-Implementierung ist
identisch. Das Werkzeug speichert stdout/stderr, Quellhashes, vollständige
R8-Atlasbytes und einen JSON-Bericht; vorhandene fertige Berichte benötigen
ein neues Label und werden nicht überschrieben.

Lokale Paarmessung mit ASan auf Intel Xeon W-2123, vier Kernen/acht Threads:

| Umgebung | Alte Quellenzeiten in Sekunden | Neue Quellenzeiten in Sekunden | Median alt → neu |
| --- | --- | --- | --- |
| Windows 10.0.26200 / MSVC | 0,078277 / 0,074531 / 0,077552 | 0,042635 / 0,041721 / 0,039766 | 0,077552 → 0,041721 (−46,2 %) |
| Alpine 3.24 / WSL / GCC 15.2 ASan/UBSan | 0,321981 / 0,339151 / 0,302665 | 0,310326 / 0,272335 / 0,269662 | 0,321981 → 0,272335 (−15,4 %) |

Während dieser Paarmessungen liefen keine weiteren von diesem Agenten
gestarteten Builds oder GUI-Fälle. Der Dateisystemcache wurde nicht geleert;
dies sind keine Kaltstart- oder macOS-Latenzgarantien. Die vollständigen
Atlasbytes wurden tatsächlich verglichen und sind pro Plattform identisch:
Windows 30 Quellen/13.134 Glypheneinträge, Linux 29/10.805. Eine Bake-
Beschleunigung wird der Änderung nicht zugeschrieben.

Die vorherigen getrennten Prozessmessungen schwankten stark und zeigten unter
Linux auch langsamere Werte. Sie bleiben unter `build/native/font-prepare-*`
erhalten, tragen aber keine allgemeine Geschwindigkeitszusage. Maßgebliche
Paarberichte: `build/native/font-prepare-paired-windows/report.json` und
`build/native/font-prepare-paired-linux/report.json`. Der macOS-Workflow führt
denselben Vergleich erst nach den realen AppKit-/F1-Fällen aus; deren bisherige
Bereitschaftsgrenzen werden nicht verändert. Seine Ergänzung allein belegt
noch keinen erfolgreichen macOS-Lauf dieser Optimierung.

Die finale kompaktierte Kandidatenfassung besteht Windows MSVC ASan **3/3**
native Fälle (`run-aukq9sfp`: Fallback, Quellenbesitz und TTC-Schriftschnitte)
und **3/3** GUI-Fälle (`run-v8r8o3d_`: Mathematik-/Zeicheninventur, Fontbedarf
und zehnfacher unabhängiger UIA-Linkstress). Linux GCC 15.2 ASan/UBSan besteht
dieselben **3/3** nativen Fälle (`run-b7jmfbu6`) und **3/3** GUI-Fälle
(`run-gn2_vxh6`: Mathematik, Bedarf und unabhängige AT-SPI-Links).
Die Mathematikfälle prüfen alle 38 Varianten mit Systemschriften und nochmals
mit ausdrücklich erzwungenen mitgelieferten Primärschriften. Beide tatsächlichen
1520 × 1080-Dichtebilder wurden geöffnet und visuell geprüft; Überschriften,
Hervorhebung und umbrochene Links bleiben lesbar. Der abschließende native
Dokumenttest bestätigt weiterhin 185 Seiten und 214 Abschnittslinks.

Die erste Markierungsfassung bestand zuvor jeweils 6/6 native und 6/6 GUI-
Nachbarn auf Windows und Linux. Ihre getrennten Prozesszeiten und die spätere
Kompaktierung bleiben als Zwischenstände in den lokalen Originalausgaben
erhalten. Die obigen Paarmessungen und 3/3-Verbünde betreffen die finale
Kompaktierung. Keiner dieser lokalen Nachweise erklärt den sporadischen
Intel-macOS-Kaltstart oder die frühere Windows-Fokusstörung für behoben.

## Ausgeführte macOS-Paarprüfung auf 14cfe8d

Der [tatsächliche Lauf](https://github.com/Lulus792/physim/actions/runs/38782921553)
besteht auf Intel **11/11** native Fälle (`run-2xt0o7m8`) und **13/13**
GUI-/AppKit-Fälle (`run-z1rjdrd5`), auf ARM64 **11/11** (`run-glugwpno`) und
**13/13** (`run-dgatnyxz`). Beide vollständigen Joblogs wurden gelesen.
Die anschließende Paarprobe bestätigt auf beiden Architekturen tatsächlich
identische vollständige Atlasbytes mit 33 Quellen und 13.319 Glypheneinträgen.
Mediane der Quellenvorbereitung: Intel **0,121199 → 0,070146 Sekunden**,
ARM64 **0,107589 → 0,074474 Sekunden**. Die drei alten/neuen Paare, Quellhashes
und vollständigen Rasterdateien werden im jeweiligen Workflow-Artefakt bewahrt.

Die Paarprobe lief ausdrücklich nach den realen Kaltstartfällen. Sie misst
deren kalte Bereitschaftszeit nicht nachträglich. Der frühere Intel-Timeout
und die Windows-Fokussporadik bleiben deshalb eigenständige offene Befunde.
