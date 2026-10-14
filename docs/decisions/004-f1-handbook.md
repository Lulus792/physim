# F1 Handbuch und Abnahme

Entscheidung vom 13. Oktober 2026: F1 soll sowohl den ersten abgeschlossenen
Versuch ermöglichen als auch beim Arbeiten präzise Antworten liefern. Die
Abnahme betrachtet Inhalt, Bedienung, Darstellung und zugängliches Lesen.
Ein grüner Parsertest allein ist keine vollständige Abnahme.

## Aktuelle Bewertung vom 14. Oktober 2026

Bewertungsgrundlage sind die zentralen Skills unter `D:/Skills/skills`:
`documentation-engineer`, `ui-ux-designer`, `accessibility-engineer`,
`ui-implementation` und `test-engineer`. Volle Punktzahl erfordert die acht
unten beschriebenen Abnahmekriterien; fehlende Prüfungen gelten als offen.

| Bereich | Belegter Stand | Rest zur vollständigen Abnahme |
| --- | --- | --- |
| Dokumentationszugang | 183 registrierte Seiten und 214 interne Abschnittslinks geprüft; Navigation, Verlauf und Suche mit realen F1-Abläufen geprüft | Vollständige Plattformabnahme derselben Revision |
| Inhalt und Aufgabe | Lernwege, Beispiele, Referenz und Fehlerbehebung vorhanden; ausgewählte Quellen-/CLI-Abläufe ausgeführt | Gesamtabgleich sämtlicher Anleitungen mit dem Projektstand |
| Lesbarkeit und Bedienung | 16/32-Pixel-Schrift, kleines Fenster, drei Themes, Codekopieren, Links und Tastaturführung durch Laufzeitbilder und Verhalten belegt | Tatsächliche DPI-/Monitorwechsel und vollständige Zustandsmatrix |
| Barrierefreiheit | Windows UIA liest vollständigen Text, bedient Suchfelder und veröffentlicht interne Links mit Textbereichen, Fokus und Aktivierung; unabhängige Clientprüfungen bestehen | Native Text-/Feld-/Linkbereiche auf macOS/Linux, Tabellensemantik und echte Screenreader-Aufgaben |

Die Hilfe ist in den geprüften Windows-Abläufen gut nutzbar. Eine vollständige
Bewertung mit Höchstpunktzahl ist wegen dieser offenen Abnahmefälle noch nicht
begründet. Die folgenden Nachweise nennen jeweils ihre Revision und Umgebung.

### Erneuter Skill-Review des lokalen Stands

Geprüft am 14. Oktober 2026 auf Windows 10.0.26200 mit dem Debug-Build unter
`build/native/F1-review`, Ausgangsrevision `38efa9d`. Die Auswahl erfolgte über
`D:/Skills/skills/scripts/skill_system.py select`; gelesen und angewendet wurden
`documentation-engineer`, `ui-ux-designer`, `accessibility-engineer` und
`test-engineer` aus diesem zentralen Bestand einschließlich ihrer Einstiegsdateien
und der Desktop-Abnahmematrix. Die Wertung ist ein fachliches Urteil über die
vorliegenden Belege, keine normierte Messung oder Produktzertifizierung.
10/10 setzt vollständige Erfüllung und Prüfung der relevanten Abnahmekriterien
voraus; fehlende Umgebungen und Prüfungen erhalten keine Erfolgspunkte.

| Gegenstand | Bewertung | Begründung und Grenze |
| --- | --- | --- |
| UI und Nutzerführung | 8/10 | Getrennte Lernwege, lokale/globale Suche, Verlauf, Codekopieren und Tastaturführung funktionieren in den geprüften Windows-Aufgaben. 32-Pixel-Text bleibt bei 760 × 540 bedienbar; Dunkel, Hell und hoher Kontrast besitzen geprüfte Bilder. Die kompakte Ansicht lässt allerdings wenig Lesefläche. Vollständige Schriftabdeckung, tatsächliche DPI-/Monitorwechsel und sämtliche Plattformzustände fehlen. |
| Aufgabe als Dokumentation | 8/10 | Einstieg führt zu einem gespeicherten Versuch und einer prüfbaren Erwartung. Sieben Kapitel je Lernweg, acht gemeinsame Modellaufgaben, vollständige Beispielquellen, API-Verträge und symptomorientierte Fehlerhilfe unterstützen Lernen und Nachschlagen. 183 Seiten und 214 Abschnittslinks sind erreichbar. Der Gesamtabgleich aller Texte mit der Umsetzung ist offen; der Review findet konkrete veraltete Aussagen. |
| Barrierefreiheit | 5/10 | Externe Windows-UIA-Clients lesen Dokumenttext, bedienen Suchfelder und Links und prüfen den Fensterlebenszyklus. Das ist ein belastbarer Teilnachweis. Echte Narrator-/NVDA-/VoiceOver-/Orca-Aufgaben, native Tabellensemantik und vollständige Text-/Feld-/Linkanbindung auf macOS/Linux bleiben offen. |

Für die Inhaltsaufgabe wurden User Guide, beide Lernwege, C-Experimenttutorial,
API-Einstieg und Fehlerbehebung gegen ihre Quellen und vorhandenen Prüfungen
betrachtet. Der User Guide nennt Voraussetzungen, konkrete Bedienaktionen,
erwartete Dateien/Kurven und eine anschließende kontrollierte Änderung. Die
Referenz ergänzt Signaturen, Einheiten, Lebensdauer und Fehlerverträge.
Planung und implementierter Stand werden im Handbuchstart ausdrücklich getrennt.
Der Quellenvergleich bestätigt identische veröffentlichte Beispiele; er bedeutet
nicht, dass jedes Modell in diesem Review vollständig ausgeführt wurde.

Aktuell tatsächlich ausgeführte Belege, jeweils Exitcode 0:

- `run-o7656q5r`: 27/27 native Inhalts-/Referenzfälle. Darunter 1.264 aufgelöste
  API-Symbole, 31 Referenzdokumente, beide Lernwege, Beispielquellenabgleich,
  ausgeführte C-/Physim-Bewegungsbeispiele sowie 183 Seiten und 214 Abschnittslinks.
- `run-8eh1g5p7`: 5/5 GUI-Fälle für Tastatur, Verlauf, Zoom, Inline-Inhalte und
  den echten Windows-F1-Fensterlebenszyklus. Die neuen Bilder des kleinen
  32-Pixel-Lesers, der hellen Suche und der Wiederöffnung wurden visuell geprüft.
- `run-u5xoqydr`: 3/3 externe Windows-UIA-Appfälle für Suchfelder,
  interne Links und vollständigen Dokumenttext.

Die erweiterte Zeichen-Gegenprobe `run-m3m9o5rx` scheitert mit Exitcode 1.
Sie findet U+1D40, neun Rahmenzeichen, U+2603 sowie U+1F30D und U+1F600 in
allen 38 Fontvarianten ohne verwendbare Glyphen. Die geladenen Bereiche
schließen diese Codepunkte aus; das Wiederöffnungsbild zeigt `θ?` statt `θ🌍`.
Die experimentelle Probe meldet außerdem U+0020, weil sie für jedes druckbare
Zeichen sichtbare Tinte verlangt. Dieser Leerzeichenbefund ist ein Prüferfehler
und zählt nicht als fehlende Schriftglyph. Die bestehende mathematische
43-Zeichen-Prüfung belegt folglich keine vollständige Zeichendarstellung.

Zwei konkrete Inhaltskorrekturen folgen aus dem Review: `docs/accessibility.md`
behauptete noch, Windows/UIA sei nicht angebunden, und führte es nochmals pauschal
als offene Aufgabe. Der Text beschreibt jetzt den tatsächlich geprüften Umfang
und dessen Grenzen. Der Einstieg von `docs/physim-guide.md` nennt nun den
Entwicklungsstand `0.1.0-dev`, passend zu README und Sprach-SDK, statt einer
historischen Versionsangabe. Weitere historische Nachweise bleiben als solche
erhalten. Priorität haben anschließend Schriftfallback, praktische assistive
Nutzeraufgaben, Tabellensemantik und die native Desktop-Abnahmematrix.
Nach den Inhaltskorrekturen bestehen Parser, Lernwege, Offline-Abschnittslinks
und API-Referenz erneut 4/4 mit Exitcode 0 in `run-9e2tghw0`.

### Offline-Schriftquellen und vollständige Zeicheninventur

Die nächste Korrektur erweitert die Gegenprobe auf alle **167 druckbaren Zeichen
der 183 registrierten Originalseiten**. Die vorherige Beschränkung auf 43
mathematische Zeichen genügte nicht. Leerzeichen benötigen einen vorhandenen
Quellglyph und positiven Vorschub, aber keine sichtbare Tinte. U+0378 bleibt
ein negativer Fall: Die rohen Atlas-Metadaten kennen seinen Codepunkt, der
Quellfont besitzt jedoch keinen Glyphenindex dafür.

Fünf unveränderte Noto-Schriften werden offline mit ihren SIL-OFL-1.1-Lizenzen
ausgeliefert. Quelle ist Google Fonts, Revision
`66928797f271fed5ee428544e7229685e2d90127`; Herkunft, Bytegrößen und SHA-256
stehen in `third_party/fonts/manifest.json`. Noto Sans, Sans Mono, Sans Math,
Sans Symbols 2 und Emoji ergänzen nur Zeichen, die in vorherigen deklarierten
Quellbereichen fehlen. Die gebackenen Glyphen speichern ihre tatsächliche
Quellpräsenz; der Leser überspringt `.notdef` auch bei überlappenden Bereichen.
Diese Information bleibt nach Freigabe der TTF-Quelldaten gültig. Eigene
Fallback-Bereiche bleiben bis zur Atlasfreigabe erhalten.

Die neue native Gegenprobe entdeckt zunächst einen Nuklear-Fehler:
Zusatzschriften wurden an den ersten statt den zuletzt angelegten Handle
gehängt (`run-yzktc5r5`). Nach dieser Korrektur findet sie den weiterhin
verwendeten fehlenden Originalglyph (`run-wx8p5re3`). Die Quellpräsenz und
Lookup-Korrektur beheben beide Ursachen. Fehlgeschlagene Adds verändern keine
Listen oder Besitzverträge; die Tests prüfen Allokationsausfälle, Retry,
Rastertinte und vollständige Freigabe. Der erfolgreiche Merge liefert den
tatsächlich erweiterten letzten Handle zurück.

Die erste komplette GUI-Gegenprobe findet noch die neun Rahmenzeichen
(`run-plv4gkwn`); die Monospace-Quelle ergänzt sie. Die Bildprüfung entdeckt
anschließend verschiedene Vorschübe für Rahmen und Leerzeichen. Rahmen gehören
jetzt auch zum deklarierten Bereich der primären Codeschrift; alle neun
Codevarianten müssen gleiche Zellbreiten liefern. Ist die Systemcodeschrift
nicht verfügbar, wird Noto Sans Mono als primäre Codeschrift geladen.
Die zusätzlichen Bilder prüfen Globus, Gesicht, Schneemann, Modifikator,
tiefgestellten Buchstaben und Rahmencode bei 16/32 Pixeln.

Ein zweiter echter App-Lauf setzt `PHYSIM_TEST_BUNDLED_FONTS=1` und umgeht alle
Systemschriften. Er bestätigt 50 tatsächlich geladene primäre Handles im
Hauptfenster und in F1 sowie erneut alle 167 Zeichen in 38 F1-Varianten.
Diese strengere Gegenprobe findet das fehlende Integralzeichen U+222B
(`run-6am85uut`); Noto Sans Math behebt es. Der finale ASan-Lauf besteht
beide Modi. Dies beweist den geprüften Zeichenbestand ohne Systemschrift;
weitere Schriftsysteme oder komplexe Textformung folgen daraus nicht.

Abschließende tatsächlich ausgeführte Windows-Belege, jeweils Exitcode 0:

- `run-efjeijrr`: 8/8 native Font-, Parser-, Lernweg-, API- und Abschnittsfälle.
- `run-28qo4u86` unter MSVC ASan: 4/4 Fontfälle mit Quellfonts, Rastertinte,
  Fehlereigentum und Cleanup. Die Quelle ohne Integral wurde zusätzlich durch
  die reale App-Gegenprobe erkannt.
- `run-2ciy1267` unter MSVC ASan: 5/5 GUI-/UIA-Fälle für Zeichen einschließlich
  rein gebündelter primärer Fonts, Zoom, Suchfelder, Links und Lebenszyklus.
  Die neuen 32-Pixel-Bilder mit Integral und Rahmen wurden visuell geprüft.
- `build/native/F1-review/font-installed-proof`: der echte Release-SDK wurde
  nach `build/native/font installed ä/Physim` verschoben und von einem anderen
  Arbeitsverzeichnis ausgeführt. Beide primären Fontmodi bestehen dieselbe
  App-Zeichenprobe. Alle 536 Paketdateien und zehn Font-/Lizenzdateien stimmen
  mit ihren Manifest-Hashes überein; die primären Fontpfade liegen im SDK.

Damit sind die belegten fehlenden Quellzeichen und falschen Merge-/Besitzpfade
behoben. Eine universelle Schriftabdeckung oder vollständige UI-Abnahme wird
nicht erteilt. Bei rein gebündelten primären Fonts bleiben Hervorhebungsgewichte
und die vertikalen Abstände einzelner Rahmendarstellungen optischer Feinschliff.
Die tatsächliche DPI-/Monitor-, assistive und native Plattformabnahme bleibt
ebenfalls offen. Die Linux-Workflowauswahl enthält jetzt beide neuen nativen
Fontfälle und die erweiterte Zwei-Modi-Zeichenprobe.

Der vorherige Linux-Stand `38efa9d` ist inzwischen tatsächlich erfolgreich:
Clang besteht 20/20 native und 25/25 GUI-Fälle (`run-ri29r7wd`, `run-uyccp8ik`),
GCC ebenso (`run-1r6uzz3_`, `run-16qahise`), Workflow `38497490053`.
Dies ist ein Nachweis für den vorherigen Stand und ersetzt keine Ausführung
der neuen Schriftkorrektur unter Linux oder macOS.
Nach den Dokumentationskorrekturen bestehen API, Parser, Lernwege und
Abschnittslinks erneut 4/4 in `run-2b5685yy`. Der Testläufer erkennt seine
gezielten Fehlermuster in `build/native/F1-review/font-runner-proof`;
die Gesamtprüfung dieses Läufers endet mit Exitcode 0. `.gitattributes` erhält
die gepinnten Font-/Lizenzbytes auch bei Windows-Checkouts unverändert.

### Hervorhebung, Rahmenzellen und tatsächlich sichtbarer Leserabschluss

Die Gegenprobe `run-d8kr410o` bestätigt identische normale und starke Schrift
in allen neun rein gebündelten Leseschriftgrößen. Die normale Standardachse
eines variablen Fonts ersetzt keinen Bold-Schnitt. Eine unveränderte statische
Noto-Sans-Bold-Datei aus dem offiziellen archivierten Noto-Repository,
Revision `ffebf8c1ee449e544955a7e813c54f9b73848eac`, wird jetzt für starke Rollen
geladen. Der Font und seine SIL-OFL-1.1-Lizenz besitzen eigene gepinnte Quelle
und Hashes. Der Fontbestand umfasst damit sechs Dateien plus sechs Lizenzen.

Die native Rasterprüfung misst für A–Z bei 32 Pixeln eine Alpha-Summe von
526290 für Regular und 821164 für Bold. Die Strichfläche ist tatsächlich
verschieden. Die reale App fordert außerdem unterscheidbare AaBb-Metriken
in allen neun Lesegrößen und 24 tatsächlich geladene Bold-Handles innerhalb
ihrer 50 primären Hauptfenster-/F1-Handles. Normal und stark stehen im neuen
Prüftext nebeneinander; beide Schriftgrößen und Fontmodi werden gezeichnet.

`run-94p1ztw2` erkennt eine Codezeilenhöhe von 38 gegenüber einer vertikalen
Quellzelle von 39. Für Codeblöcke mit Rahmenzeichen wird die natürliche Zellhöhe
einmal pro Layout berechnet und gecacht. Messung, Zeichnung und native
Zeichenrahmen verwenden dieselbe Zellverschiebung. Gewöhnlicher Code und
Absätze behalten ihre bisherige Zeilenhöhe; Fontatlas-Glyphen werden dafür
nicht global verändert.

Die strengere Schlusslinienprobe findet zusätzlich abgeschnittene Abschlüsse.
Ein direkt gesetzter Fokuswert und der echte Tab-/Ende-Ablauf beseitigen die
Ursache nicht (`run-ltd3cj00`, `run-8jhuq64b`). Die Diagnose `run-l3uvjvhd`
zeigt, dass ein großer Scrollauftrag vom Scrollbalken anschließend auf 78
zurückgesetzt wird. Eine ungenutzte Layoutzeile zwischen Messung und Anlage
des Lesergruppenrahmens verschob dessen Ursprung, während seine Höhe noch
von der früheren Position aus berechnet wurde. Ihr Entfernen richtet
Gruppenrahmen und tatsächliche Clipfläche aus. Der echte Tab-/Ende-Ablauf
zeigt danach den vollständigen Diagrammabschluss.

Die unabhängige BMP-Prüfung liest echte Pixel aller drei vertikalen Linien
und der Schlusslinie. Sie berücksichtigt das Verhältnis zwischen 760 × 540
logischen Koordinaten und den Bildpixeln, verlangt durchgehende Striche und
lehnt eine injizierte Lücke ab. Sie besteht mit Systemfonts und rein gebündelten
Fonts. Die neuen Bilder mit sichtbarer Hervorhebung und geschlossenem Rahmen
wurden visuell geprüft.

Abschließende Windows-Belege mit Exitcode 0:

- `run-hxqhkbmu`: 9/9 GUI-/UIA-Fälle für Tastatur, Verlauf, Inline-Inhalt,
  Schrift/Pixel, Zoom, Suchfelder, vollständiges Lesen, Links und Lebenszyklus.
- `run-kbrw86jp` unter MSVC ASan: 6/6 native Font-, Parser- und Abschnittsfälle.
- `run-bdgenm68` unter MSVC ASan: 4/4 GUI-/UIA-Fälle einschließlich vollständigem
  Lesen, Links, Zoom und beiden Fontmodi mit Rastergegenprobe.
- `build/native/F1-review/typography-installed-proof`: beide echten App-Fontmodi
  bestehen aus dem verschobenen Release-SDK unter
  `build/native/typography installed ä/Physim`, von einem anderen Arbeitsordner.
  Alle 538 Paketdateien und zwölf Font-/Lizenzdateien entsprechen ihren Hashes.
  Die tatsächlichen Bold-Handles und geschlossenen Rahmen werden mitgeprüft.

Der vorige Stand `2d88eb3` ist auch unter Linux tatsächlich erfolgreich:
Workflow `38504376167`, Clang 22/22 native (`run-w9fa1arb`) und 25/25 GUI
(`run-zg7hwyie`), GCC 22/22 native (`run-mjoimclz`) und 25/25 GUI
(`run-ium9fqh7`). Die neuen Typografie-/Leserhöhenänderungen benötigen eigene
Linux- und macOS-Nachweise; die vorherigen Läufe ersetzen sie nicht.

Die beiden optischen Font-Restpunkte sind damit in den geprüften Windows-Modi
behoben. Tatsächliche DPI-/Monitorwechsel, praktische Screenreader-Aufgaben,
native Tabellen- und vollständige Text-/Feld-/Linkanbindung auf macOS/Linux
und weitere F1-Abnahmekriterien bleiben offen. Eine vollständige Produktfreigabe
wird weiterhin nicht behauptet.
Nach der Dokumentation dieser Korrektur bestehen Parser, API-Referenz, Lernwege
und Abschnittslinks erneut 4/4 mit Exitcode 0 in `run-rt8xha9i`.

### Pixeldichte, logische Fontgrößen und atomarer Atlaswechsel

Der nächste Schritt trennt die Rasterauflösung eines Fonts von seiner logischen
Höhe. Nach dem Bake werden die Handles und Query-Metriken auf die gewünschte
logische Größe abgebildet. Die native Probe bestätigt die Rasterdichten
1/1,25/1,5/2 mit unveränderten Textbreiten; erneute Abbildung ist idempotent,
ungültige Werte verändern den vorhandenen Stand nicht. Rahmenzellen lesen
ebenfalls die logischen Query-Metriken statt unskalierter Atlasrechtecke.

F1 fragt bei Erstellung und vor dem Zeichnen `SDL_GetWindowPixelDensity` ab.
Seine 38 Größen-/Stilvarianten werden mit entsprechend höherer Rasterauflösung
angelegt. Ein Ersatzatlas wird getrennt aufgebaut, vollständig gebacken und
hochgeladen. Erst bei Erfolg werden Textur, Nulltextur und Font-Handles
übernommen; der neue Defaultfont wird vor Freigabe des alten Atlasses installiert.
Die Dokumentposition wird anhand der veröffentlichten Leseposition erhalten
und der Layoutcache invalidiert. Fehlgeschlagene Versuche behalten die lebenden
Fonts; dieselbe fehlgeschlagene Dichte wird nicht in jedem Frame neu gebacken.

Die wirkliche App-Probe `run-bhggy_1m` wechselt die Rasterdichte
1→1,25→1,5→2→1. Bei 16 logischen Pixeln liegen 20/24/32/16 Rasterpixel vor;
die gemessene AaBb-Textbreite bleibt 57,081. Textversion und Editorinhalt bleiben
gleich. Die anschließende Fehler-Gegenprobe `run-5610x6n2` deckt einen Debug-Assert
bei absichtlich fehlender Bake-Allokation auf. Nur der betreffende Prüfprozess
wurde beendet. Temporäre, Glyph-, Pixel- und RGBA-Allokationsfehler kehren jetzt
auch im Debug-Build durch die bereits vorhandenen Freigabepfade zurück. Die
reale Probe bestätigt den erhaltenen Defaultfont und vollständige Freigabe des
fehlgeschlagenen Kandidaten.

Eine weitere Gegenprobe erkennt eine zu frühe Umschaltung im Prüfer
(`run-unzsj6g6`): Der Zahlenbeleg stammte noch von Dichte 2, die PNG-Aufnahme
entstand nach dem Wechsel auf Dichte 1. Der folgende Wechsel erfolgt jetzt
erst im nächsten Frame. Der finale Beleg `run-4qsmr6yd` zeichnet den echten
1520 × 1080-Framebuffer aus 760 × 540 logischen Koordinaten mit dem tatsächlich
32 Pixel hoch gerasterten 16-Pixel-Font. PNG-CRC, Dimensionen, Scanlines und
lesbare Textpixel werden unabhängig geprüft; das Bild wurde visuell angesehen.

Abschließende Windows-Belege, jeweils Exitcode 0:

- `run-au995z_a`: 7/7 native Font-, Parser- und Abschnittsfälle mit MSVC ASan.
- `run-vylai6qc`: 6/6 reale F1-/UIA-Fälle unter MSVC ASan für Rasterwechsel,
  Zeichen, Zoom, vollständiges Lesen, Links und Fensterlebenszyklus.
- `build/native/F1-review/density-installed-proof`: der verschobene Release-SDK
  unter `build/native/density installed ä/Physim` besteht Raster-Rundlauf,
  Fehlererhalt und doppelte Framebuffer-Ausgabe von einem anderen Arbeitsordner.
  Alle 538 Paketdateien und zwölf Font-/Lizenzdateien entsprechen ihren Hashes.

Diese Rasterprüfungen sind keine echte Windows-DPI- oder Monitorwechselabnahme.
Der verfügbare Windows-Monitor liefert 96 DPI. SDL unterscheidet Pixeldichte
und Inhaltsskalierung: Windows/X11 verwenden typischerweise physische
Fensterkoordinaten mit Inhaltsskalierung, macOS/Wayland zusätzlich Fenster- zu
Pixelverhältnisse. Grundlage sind die offiziellen
[SDL-HiDPI-Verträge](https://wiki.libsdl.org/SDL3/README-highdpi) und
[DisplayScale](https://wiki.libsdl.org/SDL3/SDL_GetWindowDisplayScale).
Der nun geprüfte Font-Rasterpfad behandelt die Pixeldichte; Windows-Inhaltsskalierung,
die Hauptfenster-DPI-Integration, reale Monitorwechsel und native Plattformbilder
bleiben weitere Aufgaben. Die Diagnose überschreibt in der Probe die Rasterdichte
und erzeugt tatsächliche Renderziele; sie täuscht kein anderes Betriebssystem-DPI vor.

Der vorherige Typografie-Stand `9e172e4` ist unter Linux inzwischen ausgeführt:
Workflow `38508599005`, Clang 22/22 native (`run-ewmhvafp`) und 25/25 GUI
(`run-srggasan`), GCC 22/22 native (`run-1qg4r9pk`) und 25/25 GUI
(`run-olowylup`). Der neue Rasterwechsel benötigt einen eigenen Linux-/macOS-Nachweis.
Die abschließenden Parser-/API-/Abschnitts-/Dichtefälle bestehen nach der
Dokumentation erneut 4/4 mit Exitcode 0 in `run-2qbh1dw6`. Der Testläufer
bestätigt seine gezielten Fehlermuster in `build/native/F1-review/density-runner-proof`.

## Nutzeraufgaben

- Eine Vorlage anlegen, bauen, aufzeichnen und auswerten, mit sichtbaren
  Zwischenergebnissen und einer Prüfung des gespeicherten Ergebnisses.
- Ein Beispiel nach einer physikalischen Frage auswählen und verändern.
- C oder Physim lernen, ohne unbeabsichtigt den Sprachweg zu wechseln.
- Eine Funktion einschließlich Signatur, Einheiten, Lebensdauer und Fehlern finden.
- Ein beobachtetes Problem anhand seiner Symptome beheben.

## Navigation und Darstellung

Der Einstieg priorisiert Bedienung, Beispiele, Lernwege, Referenz und
Fehlerbehebung. Projekttechnik und historische Nachweise bleiben erreichbar.
Seitennavigation erhält Verlauf und Leseposition. Eine Suche zeigt ihren Umfang,
Treffer und den aktuellen Treffer eindeutig. Fehlende Dokumente und fehlende
Treffer geben eine verständliche Rückmeldung und einen erreichbaren Rückweg.

Der Text bleibt die Hauptfläche. Überschriften zeigen Hierarchie; Hervorhebung,
Inline-Code, vollständige kopierbare Codeblöcke, Tabellen und mathematische
Zeichen müssen ihren Inhalt erhalten. Links stehen erkennbar beim zugehörigen
Text. Lange Texte und deutsche Beschriftungen dürfen keine Aktion verdecken.
Die räumlichen Erklärungen besitzen Zahlen, Einheiten und Textalternativen.

## Abnahmekriterien

1. Alle registrierten Seiten und internen Abschnittslinks sind offline erreichbar.
   Historische Texte werden ohne Inhaltsverlust auf lesbare Archivbände verteilt.
2. Codekopieren, Suche, Abschnittssprünge und vollständiger Navigationsverlauf
   funktionieren mit Maus und Tastatur. F1-Eingaben verändern keinen Editor.
3. Beispiele entsprechen den veröffentlichten Quellen und der tatsächlichen CLI.
   Implementierung, ausgeführte Prüfung und Planung bleiben unterscheidbar.
4. Standardtext und mindestens doppelte Textgröße bleiben bei Mindestgröße und
   gewöhnlicher Fenstergröße lesbar und bedienbar. Die Schrift enthält alle
   in den gebündelten Seiten verwendeten mathematischen Zeichen.
5. Dunkel, Hell und hoher Kontrast besitzen geprüfte Zustände und sichtbaren Fokus.
   Text-, Such- und Fehlerzustände sind auch ohne alleinige Farbkodierung erkennbar.
6. Native Accessibility veröffentlicht Inhalt und Bedienelemente mit sinnvoller
   Lesefolge, Namen, Rollen, Werten und Zuständen. Echte Screenreader-Aufgaben
   prüfen Suche, Lesen, Linkaktivierung und Rückkehr zum Arbeitsbereich.
7. Fenstergrößen, tatsächliche Pixeldichten, Theme-Wechsel und Eingabeisolation
   werden mit Laufzeitbildern und Verhalten geprüft. Simulation und Erklärung
   bleiben getrennt; reduzierte Effekte erhalten sämtliche Funktionen.
8. Windows, Linux und macOS erhalten eigene Build-, Funktions- und UI-Nachweise
   für die geprüfte Revision. Nicht ausgeführte Fälle bleiben offen.

## Prüfstand

Die Umsetzung dieser Entscheidung ist in Arbeit. Nach der ersten Reparaturgruppe
bestehen unter Windows 11 Pro 10.0.26200 mit MSVC und SDL 3.2.30 die 27 ausgewählten
Dokumentations-/API-Tests (`run-tyh8qq7j`) und elf GUI-Fälle (`run-comr4nko`).
Die übergroße Plattformseite wurde ohne Textverlust aufgeteilt; alle 183
registrierten Seiten und 214 Abschnittslinks sind erreichbar. Der Fontatlas
enthält jetzt mathematische Zeichen, Pfeile und tiefgestellte Ziffern.

Der zunächst fehlgeschlagene Kopiertest war ein Fehler des Prüfers: SDL ergänzt
unter Windows CR vor LF für CF_UNICODETEXT. Die echte Aktion liefert den gesamten
Code. Der Vergleich akzeptiert ausschließlich diese Zeilenendendarstellung;
Gegenbeispiele mit fehlenden Zeilen, verändertem Code und Einrückung bleiben Fehler.
Ergebnisse liegen unter `build/native/F1-review/test-results` im ignorierten
Buildverzeichnis. Dies ist eine lokale Prüfung, keine vollständige Produktabnahme.

Der Verlauf enthält jetzt bis zu 64 Seiten und Abschnittsziele mit Leseposition,
Suchbegriff, ausgewähltem Suchtreffer, Inhaltsansicht und Sprachweg. Die
globale Suche berücksichtigt registrierte Quellseiten, ordnet Titeltreffer zuerst
und verwendet dieselbe Reihenfolge für Anzeige und Tastatur. Bedienung und
Beispiele stehen am Anfang der Themenliste.

Die Hilfe besitzt eine eigene Leseschrift bis 32 Pixel. Bei doppelter
Standardschrift werden Navigation und Suche kompakter; mehrzeilige Themen
bleiben lesbar. Die 13 GUI-Fälle dieser Erweiterung bestehen in `run-9w1tddq_`;
die abschließende Gegenprobe für Verlauf, Header-Suche und Schrift besteht
3/3 (`run-lf_ojqxq`). Die 30 ausgewählten nativen Fälle bestehen in `run-1ay2gb5q`;
nach Erhaltung des ausgewählten Suchtreffers bestehen die sechs betroffenen
Modell-/Parser-/Referenz-/Geometrie-/Einstellungs-/Linkfälle erneut in `run-lrv_uo0z`.
Die Bilder des kleinen 32-Pixel-Lesers und des wiederhergestellten Verlaufs
wurden visuell geprüft. Das bestätigt noch keine HiDPI- oder Screenreader-Abnahme.

Die abschließende 32-Pixel-Prüfung besteht zusätzlich unter Dunkel, Hell und
hohem Kontrast, bei 760 × 540 und 1080 × 780 sowie nach Schließen und
Wiederöffnung (`run-fohiafzg`). Die unabhängige Prüfung der tatsächlichen
Bitmapfarben besteht in `build/native/F1-review/theme-proof`. Sie erkennt
das ursprüngliche Gegenbeispiel, bei dem der Themenwechsel vor der Aufnahme
stattfand und die Bildnamen nicht zur dargestellten Palette passten. Die
korrigierten Theme-Bilder wurden visuell geprüft.

Inline-Texte behalten jetzt Hervorhebungen, Codezeichen und Links in ihrem
Lesefluss. Überschriften besitzen drei sichtbare Größen; Tabellen behalten die
Formatierung ihrer Zellen. Links lassen sich direkt anklicken oder mit Tab und
Enter/Leertaste aktivieren. Mehrzeilige Links erhalten auch dann stabilen Fokus,
wenn der Link höher als der kleine Lesebereich ist. Suchtreffer zählen keine
zusätzlichen, unsichtbaren Link-Aktionsblöcke.

Die Bildprüfung entdeckte einen Fehler der erweiterten Schriftpalette:
Nuklear meldete einen nur teilweise gepackten Atlas als erfolgreich. Bei
1024 × 32768 fehlten unter anderem die ASCII-Glyphen einer 30-Pixel-Schrift.
Die F1-Schriften verwenden jetzt zwei horizontale und einen vertikalen
Rasterungsschritt; das Backend prüft sichtbare ASCII-Glyphen aller geladenen
Schriften vor der Verwendung. Die ursprüngliche leere Codedarstellung
(`run-duh14r10`) wird durch die unabhängige Pixelprüfung abgelehnt.

Die abschließenden 14 Windows-GUI-Fälle bestehen in `run-1cpt5r4u`, einschließlich
F1-Eingabeisolation, 16/22/32-Pixel-Schrift, Links in Absätzen und Tabellen,
Verlauf, Codekopieren und drei Themes. Die gezielten Laufzeitbilder wurden
visuell geprüft; die Codefläche enthält tatsächlich lesbare Glyphen.
Die 29 ausgewählten nativen Dokumentations-, Referenz-, Beispiel- und
Accessibility-/Geometriefälle bestehen in `run-mji_rnhn`.

Windows besitzt jetzt einen UI-Automation-Provider für die veröffentlichten
Bedienelemente. Die C17-Brücke veröffentlicht Namen, Rollen, Fokus und
Button-/Checkbox-/Auswahl-/Slider-Aktionen über den nativen Fensteranschluss.
Ein separater UIA-Clientprozess bedient die tatsächlichen Einstellungen;
die drei Checkbox-, Auswahlgruppen- und Fokusfälle bestehen in `run-k8519wd_`.
Der Client identifiziert gleichnamige Gruppen und Textüberschriften anhand
ihrer Rolle. Entfernte oder zerstörte Elemente können keine Aktion mehr auslösen.

Mit aktiver Windows-Anbindung bestehen 16 Accessibility- und F1-GUI-Fälle
(`run-sezoviel`). Die beiden nativen Provider-/Rendererfälle bestehen zusätzlich
mit MSVC AddressSanitizer (`build/native/F1-uia-sanitized/test-results/run-ycfu0o49`).
Die Windows-Brücke und der unabhängige Client sind reguläre Testziele; ihre
Laufprotokolle verbleiben im ignorierten Buildverzeichnis.
Nach dem abschließenden Review bestehen alle fünf Windows-Provider- und
Clientfälle erneut (`run-nh1gv2ti`); die fünf Modell-/Parser-/Abschnittsfälle
bestehen in `run-shcvhfxl`. Der sichtbare Fokus des extern bedienten Buttons
wurde im Laufzeitbild geprüft. Die native ID beschreibt die aktuelle
Laufzeitidentität eines Elements; eine dauerhaft sprachunabhängige Automation-ID
wird nicht vorgetäuscht.

Die F1-Ansicht veröffentlicht jetzt zusätzlich den vollständigen Text in
Lesereihenfolge und die tatsächlich gezeichneten Zeichenpositionen. Der
portable Entwurf und der veröffentlichte Stand bleiben getrennt; Seitenwechsel
verwerfen alte Textbereiche und Scrollanfragen. Code behält seine Originalbytes;
zusätzliche Absatztrennungen liegen außerhalb der kopierbaren Codeblöcke.

Windows stellt diese Daten über den nativen TextPattern bereit. Ein separater
UIA-Client liest die echte Quellseite `docs/project-audit.json` einschließlich
aller **498605 UTF-8-Bytes** des Lesetexts. Der Vergleich mit der Originaldatei
plus den beiden Absatztrennungen ist bytegenau. Derselbe Client klont und
vergleicht Textbereiche, prüft sichtbare Geometrie und fordert einen Sprung zum
Dokumentende an. Unicode-Suche, Surrogatpaare, begrenzte Zeichenbewegung und
ungültige alte Textbereiche sind zusätzlich am nativen Provider geprüft.

Die Bildabnahme erkannte zunächst einen leeren Zielbereich trotz großer
Scrollposition. Ursache war eine abweichende Umbruchbreite zwischen Messung
und Zeichnung. Beide verwenden jetzt dieselbe Breite; die Prüfung verlangt,
dass das letzte inhaltliche Zeichen tatsächlich in den veröffentlichten
Glyphenpositionen sichtbar ist. Windows-CRLF wird als eine visuelle Zeilengrenze
behandelt und bleibt im gelesenen Quelltext erhalten. Die korrigierte Aufnahme
zeigt den echten Dokumentabschluss (`run-f6yat_2_`).

Die 19 betroffenen GUI-Fälle bestehen in `run-23izk2ps`; nach der visuellen
Korrektur bestehen Dokumentende, Absatz-/Codeformatierung und Abschnittssprung
erneut 3/3 (`run-f6yat_2_`). Die abschließenden zwei Provider-/Clientfälle bestehen
in `run-3hc3s5t2`. Zwei Provider-/App-Lesefälle bestehen mit MSVC AddressSanitizer
in `build/native/F1-uia-sanitized/test-results/run-89tndz7k`. Die portable
Dokument-/Modell-/Parsergruppe besteht 5/5 (`run-hcb0ypz3`); auch der Testkatalog
und dessen erwartete Fehlerfälle bestehen. Diese Nachweise bestätigen den
beschriebenen Leseweg, keine vollständige Screenreader-Abnahme.
Der abschließende Stand besteht zusätzlich 4/4 in `run-oxz08pfb` (Provider,
externer Leser, 22-Pixel-Tastatur und Verlauf) sowie 6/6 Modell-/Parser-/Linkfälle
in `run-u3ns2d1p`. Obere und untere Scrollausrichtung werden als unterschiedliche
native Anfragen übertragen; der gezeigte externe Leseweg prüft den Dokumentabschluss.

Die beiden F1-Suchfelder besitzen unter Windows jetzt eine Edit-Rolle, feste
Namen, ihren aktuellen UTF-8-Wert und den nativen ValuePattern. Schreibanfragen
werden ohne Kürzung validiert und genau einmal im passenden UI-Frame übernommen.
Ungültiges Unicode, Zeilenumbrüche, zu lange Werte und entfernte oder deaktivierte
Felder verändern den bisherigen Text nicht. Die native Schnittstelle meldet
Wertänderungen; Feldnamen und Laufzeit-IDs bleiben dabei stabil.

Ein unabhängiger Client bedient **Aktuelles Dokument durchsuchen** und
**Alle Inhalte durchsuchen**. Er setzt `ps_rng_seed` und den Fokus. Die App prüft
den tatsächlichen Suchzustand, zusätzliche Tastatureingabe und den Rückweg per
Tab. Haupteditoren und Simulationszeit bleiben dabei unverändert. Die Bilder
der beiden Suchansichten wurden geprüft. Eine Gegenprobe zeigte zunächst ein
fälschlich deaktiviertes Feld im inaktiven Nachbarpanel; die native Berechtigung
berücksichtigt jetzt sichtbare, nicht überdeckte Panels.

Die 20 betroffenen GUI-Fälle bestehen in `run-l9swswr9`; die externe Feldprüfung
besteht zusätzlich in `run-0cs6jmql`. Die sieben Modell-/Parser-/Linkfälle bestehen
in `run-oqc3g_rw`, einschließlich Unicode, Leerwert, verspäteter Schreibanfrage,
Kapazitätswechsel und veralteter Ziele. Provider und echter Feldablauf bestehen
mit MSVC AddressSanitizer in
`build/native/F1-uia-sanitized/test-results/run-nqs4wt1q`. Auch die Erweiterung
des Testkatalogs besteht mit den erwarteten Fehlerfällen.

Die Windows-Suchfelder veröffentlichen jetzt zusätzlich TextPattern und
TextPattern2 mit Unicode-Textbereichen, aktueller Auswahl und Cursorposition.
Die Geometrie verwendet die Zeichenbreiten, den Textursprung und den horizontalen
Versatz des echten Felds. Nur tatsächlich sichtbare Zeichen werden gemeldet.
Die Cursorposition bleibt auch bei rückwärts gerichteter Auswahl getrennt von
den sortierten Auswahlgrenzen. Text- und Auswahländerungen lösen native Ereignisse aus.

Auswahlanfragen binden sich an Feld-ID und Inhaltsversion, übernehmen UTF-8-
Skalargrenzen einmalig im UI-Frame und verfallen bei geändertem Inhalt. Der
Windows-Textcache unterscheidet Eigentümer und Version; ein nativer Gegenfall
prüft gleichzeitig Feld und Dokument mit identischer Versionsnummer.
Ein externer Client wählt `θ🌍` im echten Suchfeld aus, liest die Auswahl zurück,
prüft die aktive Cursorposition und ihre sichtbare Geometrie. Anschließende
Tastatureingabe ersetzt genau diesen Bereich. Die Laufzeitaufnahme zeigt die
Auswahl; ein nicht vorhandenes Emoji wird vom bestehenden Font als Ersatzglyph
gezeichnet, während der native Text unverändert bleibt.

Die 15 GUI-Regressionen bestehen in `run-l15ud31b`; weitere fünf Feld-,
Fokus-, Zoom- und Absatzfälle bestehen in `run-ys7t4zmb`. Die vier portablen
Dokument-/Edit-/Fokus-/Auswahlmodelle bestehen in `run-bvhbk87r`; Grundmodell
und Editor-Clipboard bestehen in `run-7fwpnj0c`. Provider und echter Unicode-
Feldablauf bestehen zusätzlich mit MSVC AddressSanitizer in
`build/native/F1-uia-sanitized/test-results/run-b3n46l3_`.
GPU-/SDL-Eingabeprüfung und externer Feldablauf bestehen abschließend 2/2 in
`run-evxuvinw`. Die Eingabeprüfung bestätigt, dass mehrzeiliges oder tabhaltiges
Einfügen eine bestehende einzeilige Auswahl unverändert lässt; der mehrzeilige
Editor behält seine Navigation sowie Undo/Redo.

Der Linux-Sanitizerlauf für `7ea0c66` ist tatsächlich fehlgeschlagen:
[Clang-GUI-Lauf](https://github.com/Lulus792/physim/actions/runs/38474528096/job/115561838320).
Die Protokolle melden `Font initialization: Invalid texture size`; 20 von 21
GUI-Fällen scheitern, darunter F1-Medien- und Abschnittsfälle. Dieser Befund
bestätigt keine Linux-Funktionsgleichheit.

Die Atlas-Packung berücksichtigt jetzt die tatsächliche GPU-Texturgrenze beider
Achsen. Bei Platzmangel wird mit breiterer Fläche erneut gepackt; jeder Glyphen-
und Cursorbereich muss Platz finden. Rasterisierungsfehler dürfen keinen
unvollständigen Atlas liefern. Die Schriftauswahl und ihre Abtastung bleiben erhalten.
Eine eingebettete Schriftprobe erzeugt einen vollständigen 2048 × 2048-Atlas,
prüft Pixel sämtlicher sichtbarer ASCII-Glyphen, kleine und nichtpotente Grenzen,
acht Allokationsfehler sowie RGBA-Konvertierungsfehler, Freigabe und erneutes Backen.

Die beiden Fontfälle bestehen unter Windows in `run-i9w2bshn` und mit MSVC ASan
in `build/native/F1-uia-sanitized/test-results/run-353uk564`. Zwölf tatsächliche
GPU-/F1-Fälle bestehen in `run-aahao1yx`, darunter 16/22/32-Pixel-Schrift,
Absätze/Code, Themes, Verlauf, native Feldauswahl und vollständiges natives Lesen.
Die Codeaufnahme wurde visuell geprüft. Testkatalog und erwartete Fehlerfälle
bestehen in `build/native/F1-review/font-limit-runner-proof`.
Der korrigierte Stand `3ab6a1b` besteht unter Ubuntu 24.04, X11/Xvfb mit GCC
und AddressSanitizer/UBSan: **14/14 Modell-/Editor-/Fontfälle** und **21/21
GUI-Fälle**, einschließlich F1-Medien/Abschnitten und der bestehenden AT-SPI-
Bedienprobe. [Ausgeführter GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38478702052/job/115575701645).
Clang besteht ebenfalls 14/14 native Fälle und die vier F1-Medien-/Abschnittsfälle.
Seine GUI-Gruppe besteht 18/21; Completion und beide Kamera-Ebenenfälle melden
denselben globalen Pufferüberlauf in der Editor-Cursorbreitenmessung.
[Ausgeführter Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38478702052/job/115575701905).
Dies ist ein begrenzter X11-Nachweis, keine Wayland- oder macOS-Abnahme.

Die Cursorursache ist ein leerer Editor mit behaltenem Pufferspeicher:
`nk_str_get_const` liefert null und wird durch `""` ersetzt, während die
Cursorabfrage noch auf den ursprünglichen Puffer zeigt. Die Subtraktion von
Zeigern unterschiedlicher Objekte erzeugt eine falsche Messlänge. Ein leerer
Editor verwendet jetzt durchgehend denselben leeren String als Text und
Cursorbasis. Unter Windows bestehen Completion, beide Kamera-Ebenenfälle und
das native F1-Feld mit MSVC ASan 4/4 in `run-9o8cfvas`. Der Stand `fce0249`
besteht anschließend unter Ubuntu 24.04/X11/Xvfb mit beiden Compilern:
Clang **14/14 native und 21/21 GUI-Fälle** (`run-4hjvqzva`, `run-cas15ah_`),
GCC ebenfalls **14/14 und 21/21** (`run-tt2lzkxk`, `run-geqdyjui`).
[Clang-Nachweis](https://github.com/Lulus792/physim/actions/runs/38479929845/job/115579741517),
[GCC-Nachweis](https://github.com/Lulus792/physim/actions/runs/38479929845/job/115579741720).
Die Linux-Artefakte des vorherigen Fontstands zeigen tatsächlich Mesa 25.2.8/
llvmpipe mit 16384 Pixel GPU-Grenze, einen 2048 × 16384-Hauptatlas und einen
4096 × 16384-F1-Atlas. Die Bilder der räumlichen F1-Erklärung wurden geprüft.

Eine Zeicheninventur der 183 registrierten Seiten findet 43 mathematische,
griechische oder hoch-/tiefgestellte Zeichen. U+1D63 (`ᵣ`) im Pendel-Tutorial
fehlte im geladenen Bereich. Die Windows-Fonts Segoe UI, Segoe UI Semibold und
Consolas enthalten laut direkter GDI-Glyphenprüfung sämtliche 43 Zeichen;
das ersetzt keine Prüfung ihrer tatsächlich gebackenen und gezeichneten Glyphen.
U+1D63 wird jetzt für Text und Code geladen. Der neue Test `documentation_math`
liest die registrierten Originalseiten erneut und prüft sämtliche so gefundenen
Zeichen in allen **38 F1-Schriftvarianten**. Er verlangt einen tatsächlichen
Codepunkt im gebackenen Atlas, einen echten Glyphenindex im Quellfont und positive
Glyphengeometrie. Die Negativprobe U+0378 besitzt zwar einen Eintrag im deklarierten
griechischen Bereich, wird aber als fehlender Quellglyph erkannt. Der vorherige
Stand scheitert für U+1D63 in allen 38 Varianten (`run-pvxb2xzn`).

Der Test öffnet das echte Pendel-Tutorial, sucht `eᵣ` und prüft seine sichtbare
Glyphenposition bei 16 und 32 Pixeln. Die Bildprüfung entdeckte zunächst einen
falschen 32-Pixel-Ausschnitt: Der Test las noch die 16-Pixel-Geometrie, während
die Ansicht nach neuem Umbruch den Treffer verlor. Die strengere Gegenprobe
prüft nach abgeschlossenen Layout-Frames und scheitert am vorherigen Zoom
(`run-sq0lwnpr`). Zoom richtet einen vorhandenen Suchtreffer jetzt nach dem
neuen Umbruch erneut aus. Beide korrigierten Laufzeitbilder zeigen `eᵣ` in der
Formel, einschließlich 32-Pixel-Leseschrift im kleinen Fenster.

Die Mathematik-, Verlauf- und Zoomfälle bestehen 3/3 in `run-edtojs15`; die
abschließenden fünf Mathematik-/Inline-/Tastatur-/nativen Feld-/Lesefälle bestehen
in `run-bqpq0966`. Mathematik und Zoom bestehen 2/2 mit MSVC ASan in
`build/native/F1-uia-sanitized/test-results/run-ob6vmb6j`. Testkatalog und erwartete
Fehlerfälle bestehen in `build/native/F1-review/math-runner-proof`.
Die abschließende Mathematikprobe einschließlich U+0378 besteht zusätzlich mit
MSVC ASan in `build/native/F1-uia-sanitized/test-results/run-7pbhh6m_`.
Der Stand `72ff73f` besteht unter Ubuntu 24.04/X11/Xvfb mit ASan/UBSan:
Clang **14/14 native und 22/22 GUI-Fälle** (`run-1im2mdks`, `run-l1tjzep9`),
GCC ebenfalls **14/14 und 22/22** (`run-aoj8jg81`, `run-jd0b_5m4`).
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38482055800/job/115586716595),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38482055800/job/115586716800).
Beide bestehen die neue Zeichendeckung. Die Linux-Artefakte bestätigen 43
Zeichen in 38 Fontvarianten; die 16-/32-Pixel-Bilder der Formel wurden visuell
geprüft. Die mathematische Zeichendeckung unter macOS bleibt offen.

Windows kann jetzt auch verdeckte Textbereiche eines F1-Suchfelds per
`ScrollIntoView` sichtbar machen. Die Anfrage bindet sich an Feld-ID und
Inhaltsversion, validiert UTF-8-Grenzen und verfällt bei geänderten, entfernten
oder deaktivierten Zielen. Sie verändert weder Text noch Auswahl oder Fokus.
Der horizontale Versatz gehört zum jeweiligen Feld und bleibt beim Zeichnen
eines unfokussierten Felds erhalten. Tastatureingabe kann den Cursor weiterhin
normal sichtbar halten. Verborgene oder minimierte Fenster erhalten keine
Scrollaktion über den Windows-Provider.

Ein unabhängiger UIA-Prozess setzt einen langen Unicode-Suchbegriff und verlässt
das Feld per Tab. Er bestätigt zunächst, dass der angeforderte Bereich außerhalb
der sichtbaren Textbereiche liegt, fordert das Ende `Ziel ᵣ` und danach den Anfang
`Anfang θ` an und verlangt deren vollständige Zugehörigkeit zu sichtbaren
Textbereichen. Wert, beide Auswahlendpunkte, Cursorposition, aktiver Caret-Zustand
und Tastaturfokus bleiben gleich. Die App bestätigt zusätzlich den erhaltenen
Fokus auf „Vorheriger“. Beide Laufzeitbilder wurden geprüft.

Die vier GPU-/nativen Feld-/Lesefälle bestehen in `run-zetj65o2`; die neun
betroffenen Provider-/UI-/F1-Fälle bestehen in `run-zpr20ya4`. Fünf Modell-/Editorfälle
bestehen in `run-1xc1a_c6`, einschließlich Unicode-Grenzen, Inhaltswechsel,
einmaliger Auslieferung und dem tatsächlichen Zeichenversatz eines unfokussierten
Felds bei unverändertem Fokus/Auswahl des anderen Felds. Provider und echter
Feldscroll bestehen mit MSVC ASan 2/2 in `run-li5put2z`; Modell und Editorprimitive
bestehen dort ebenfalls 2/2 in `run-mptcb7p_`. Beide ASan-Läufe liegen unter
`build/native/F1-uia-sanitized/test-results`. Testkatalog und erwartete Fehlerfälle
bestehen in `build/native/F1-review/edit-scroll-runner-proof`.
Die abschließende Modellprobe für deaktivierte, entfernte und nach dem Zeichnen
queued Ziele besteht mit MSVC ASan in `run-t96ly68b`.

Der Stand `5ba0638` besteht inzwischen auch unter Ubuntu 24.04/X11/Xvfb mit
ASan/UBSan: Clang **15/15 native und 22/22 GUI-Fälle** (`run-349e8nv_`,
`run-otthh0xf`), GCC ebenfalls **15/15 und 22/22** (`run-o1y6jb2l`,
`run-wgjwn5yp`).
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38485919616/job/115599421224),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38485919616/job/115599421568).
Der neue Editor-Scrolltest gehört zu den nativen Fällen. Diese Läufe prüfen
keinen Windows-UIA-Provider unter Linux.

Zoom ohne Suchbegriff bewahrt jetzt eine Textstelle aus dem veröffentlichten
sichtbaren Lesebereich. Vor dem Umbruch wird deren UTF-8-Position gebunden an die
Inhaltsversion vorgemerkt; das neue Layout richtet diese Textstelle aus.
Mehrere Zoomtasten vor dem nächsten Bild ersetzen den ursprünglichen Anker
nicht. Eine bereits vorgemerkte native Scrollaktion behält Vorrang. Ohne
sichtbare Textgeometrie bleibt die bisherige Pixel-Skalierung als Fallback.
Dies verspricht keine identische Pixelposition oder Anker für rein grafische
Inhalte.

Die Gegenprobe `run-vpdclvrf` verliert ohne Suchbegriff die Formel nach dem
Zoom auf 32 Pixel. Die Korrektur besteht die erweiterte Mathematikprobe in
`run-t_vcy24o`: Nach abgeschlossenen Layout-Frames ist die Formel mit und ohne
Suche sichtbar, einschließlich Rückkehr zu 16 Pixeln. Alle drei zusätzlichen
Lesebilder wurden visuell geprüft. Der Modellfall prüft Versionsbindung,
fehlende/verborgene Geometrie, Unicode-Grenzen, unabhängige Boxreihenfolge und
den Vorrang bestehender Scrollanfragen (`run-z87vr1aw`). Fünf betroffene
Tastatur-/Verlauf-/Zoom-/native Feld-/Lesefälle bestehen in `run-kww751ua`.
Die MSVC-ASan-Läufe bestehen **1/1 Modellfall** (`run-5xw41dxo`) und **2/2
Mathematik-/Zoomfälle** (`run-1_js51y5`) unter
`build/native/F1-uia-sanitized/test-results`. Die Prüfung des Testläufers mit
erwarteten Gegenfehlern besteht in
`build/native/F1-review/reading-anchor-runner-proof`.
Der Stand `e676b92` besteht inzwischen unter Ubuntu 24.04/X11/Xvfb mit
ASan/UBSan: Clang **15/15 native und 22/22 GUI-Fälle** (`run-sygycws1`,
`run-h3lzuif4`), GCC ebenfalls **15/15 und 22/22** (`run-m5mr0erm`,
`run-yhfoibhp`).
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38487336229/job/115604045409),
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38487336229/job/115604045648).
Die neue Lesepositionsprobe gehört zu beiden GUI-Läufen. macOS bleibt offen.

Interne F1-Links besitzen jetzt eine eigene native Rolle und einen versionierten
Bereich im vollständigen Lesetext. Ihr Name entspricht dem Linktext. Die
Publikation enthält auch Links außerhalb des sichtbaren Ausschnitts; ihre
Identität bleibt beim Scrollen erhalten. Native Fokusanforderungen machen die
zugehörige Textstelle sichtbar. Ein anderes Dokument erhält neue Linkidentitäten.
Verdeckte oder deaktivierte Ziele nehmen keine native Fokus-/Aktivierungsaktion
an. Die Aktion „Quellenadresse kopieren“ bleibt als Button veröffentlicht.

Windows `TextPattern` liefert die eingebetteten Links über `GetChildren` in
Lesereihenfolge, auch bei teilweise überlappenden Bereichen. `RangeFromChild`
liefert ihre exakten UTF-16-Grenzen; `GetEnclosingElement` erkennt den Link auch
innerhalb seines Textbereichs. Ein Linkbereich enthält keine weiteren Kinder.
Fremde Controls, nicht zugeordnete Elemente und veraltete Bereiche werden
abgewiesen. Diese Zuordnung folgt den Microsoft-Verträgen für
[Hyperlink](https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-supporthyperlinkcontroltype),
[RangeFromChild](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextprovider-rangefromchild)
und [GetChildren](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextrangeprovider-getchildren).

Die Inventur parst alle **183 registrierten Originalseiten** und veröffentlicht
ihre **1.058 internen Links**, höchstens **119 pro Seite**, in einem separaten
Modell. Linkname und Linkbereich entsprechen exakt dem Text der Leserfassung; das Modell
behält mindestens 64 Plätze für andere Bedienelemente. Die Inventur läuft auch
im plattformübergreifenden Mathematikfall. AXLink und AT-SPI-Linkrolle sind
zugeordnet; native Textbereichszuordnung auf diesen Plattformen bleibt offen.

Der unabhängige Windows-UIA-Prozess prüft im echten F1-Fenster zwei Links in
Absatz und Tabelle, ihre Rolle, Reihenfolge, exakten Textbereiche und
umschließenden Elemente. Er bestätigt zunächst den verdeckten Tabellenlink,
fokussiert ihn, verlangt sichtbare Glyphen und öffnet darüber den C-Lernweg.
Danach ist der alte Textbereich ungültig. Die App bestätigt den Zielartikel,
Rückkehr zum Arbeitsbereich sowie unveränderte Editorinhalte und Simulationszeit.
Die Bildprüfung korrigierte einen zunächst zu engen Fokusrahmen um „C“; das
abschließende Bild zeigt das Zeichen lesbar im eigenen Rahmen.

Modell und Parser bestehen 2/2 in `run-horbig68`; die fünf Provider-/externen
Link-/nativen Lese-/Mathematik-/Inline-Fälle bestehen in `run-guf9qwrb`.
Vier betroffene Modellfälle bestehen nach der abschließenden Identitätsprüfung
in `run-3mgmcufa`. MSVC ASan besteht **2/2 Modell-/Parserfälle**
(`run-6fjr53xr`), **3/3 Provider-/Render-/echte Linkfälle** (`run-e54z9n9s`),
die zusätzliche reale Renderprüfung verdeckter offscreen Links
(`run-x44glzgj`) und die abschließende vollständige Inventur mit Mathematik/Zoom
(`run-xe4xlwwk`). Diese ASan-Läufe liegen unter
`build/native/F1-uia-sanitized/test-results`. Testkatalog und erwartete
Fehlerfälle bestehen in `build/native/F1-review/link-runner-proof`.
Die abschließende Provider-/Clientprüfung einschließlich verschiedener nichtleerer
Automation-IDs besteht unter MSVC ASan zusätzlich 2/2 (`run-5zghefgs`).
Die strengere Inventur einschließlich vollständiger Linknamen besteht unter
MSVC ASan in `run-uvy0j5b0`.
Die erweiterte Linux-Prüfung enthält die neuen Modelle sowie Inline-Links,
Verlauf und Zoom; eine Workflowdefinition bestätigt deren Ausführung noch nicht.

Der Stand `38dde0b` wurde unter Ubuntu 24.04/X11/Xvfb tatsächlich mit ASan/UBSan
ausgeführt: GCC besteht **19/19 native und 24/25 GUI-Fälle** (`run-63n96l08`,
`run-vvzfn_n2`); Clang ebenfalls **19/19 und 24/25** (`run-hcesvfom`,
`run-k0fed1p5`).
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38489455686/job/115610946220),
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38489455686/job/115610946434).
Der zusätzliche Zoomfall prüfte unmittelbar nach der asynchronen Größenänderung
noch die Leseflächenhöhe des vorigen Bildes (250 Pixel). Das archivierte
1080 × 780-Pixel-Bild zeigt bereits das korrekt vergrößerte Layout. Die kleine
Zoomansicht und die Formel ohne Suchbegriff wurden ebenfalls visuell geprüft.
Die Korrektur synchronisiert beide Größenanforderungen mit `SDL_SyncWindow`
und wartet abgeschlossene Layout-Frames ab. Sie senkt keinen Geometrie-Grenzwert.
Jedes Zoom-Bild wird jetzt gegen die protokollierte tatsächliche logische Größe,
Drawable-Pixelgröße, Schriftgröße und Leseflächenhöhe geprüft. Das entspricht
dem [SDL-Vertrag für Größenänderungen](https://wiki.libsdl.org/SDL3/SDL_SetWindowSize).
Die korrigierte Linux-Ausführung stand an diesem Zwischenschritt noch aus.

Der korrigierte Stand `fa3e8ac` besteht inzwischen unter Ubuntu 24.04/X11/Xvfb
mit ASan/UBSan: GCC **19/19 native und 25/25 GUI-Fälle** (`run-ex_l712d`,
`run-r0gu5ywa`), Clang ebenfalls **19/19 und 25/25** (`run-0bqee4bo`,
`run-hrzfzoz5`).
[GCC-Lauf](https://github.com/Lulus792/physim/actions/runs/38493455260/job/115623771433),
[Clang-Lauf](https://github.com/Lulus792/physim/actions/runs/38493455260/job/115623771688).
Beide bestehen den strengeren Zoomfall einschließlich tatsächlicher Bildgrößen.

Windows meldet jetzt Änderungen der Linkgeometrie, Sichtbarkeit und Bedienbarkeit
mit typisierten UIA-Property-Ereignissen. Die Geometrie berücksichtigt auch eine
Verschiebung des nativen Fensters bei unverändertem Dokumentlayout. Ein nativ
deaktiviertes Fenster sperrt Fokus, Aktivierung und die Übernahme von
Widget-Aufträgen. Die Ereigniserzeugung hält Providerreferenzen, gibt den
Modellmutex vor nativen Aufrufen frei und meldet keine unveränderten Werte.

Maus, Tastatur und native Linkaktivierung verwenden denselben Abschlussweg.
`Invoked` wird nach erfolgreicher Navigation gesendet, während der veröffentlichte
Quelllink noch verfügbar ist. Annahme eines Auftrags, deaktivierte Ziele und
fehlgeschlagene Navigation erzeugen keine erfolgreiche Aktivierungsmeldung.
Der Client registriert seine Ereignisse mit gecachter Automation-ID, sodass er
den nach der Navigation verschwindenden Quelllink zuordnen kann. Dieser Ablauf
folgt den [Microsoft-Invoke-Konventionen](https://learn.microsoft.com/en-us/windows/win32/winauto/uiauto-implementinginvoke).

Die unabhängige Ereignisprobe scheitert am vorherigen Stand (`run-r9fl1j38`):
Der Link wird sichtbar, aber Geometrie- und Sichtbarkeitsereignisse fehlen.
Die Korrektur besteht zunächst in `run-pyylvgh5`. Die erweiterte Probe deaktiviert
und reaktiviert das tatsächliche Hostfenster, verlangt beide Zustandsereignisse,
weist eine Aktivierung während der Deaktivierung ab und verschiebt das Fenster
um 13/7 Pixel. Native Geometrie und Ereignis bestätigen die Verschiebung;
unveränderte Folgeframes erzeugen keine zusätzlichen Meldungen (`run-8xmxztfz`).

Ein zusätzlicher ungültiger Link zeigt den Fehler jetzt im F1-Fenster selbst.
Der Client wartet auf diesen nativen Fehlertext und bestätigt einen weiterhin
gültigen Quellbereich ohne Invoked-Ereignis; die App bestätigt unveränderte
Seite und Inhaltsversion. Die anschließende erfolgreiche Navigation meldet
genau eine Aktivierung und invalidiert den alten Bereich. Die Fehleransicht
wurde visuell geprüft. Ereignisse, strenger Zoom und Inline-Links bestehen
3/3 in `run-59gvjw_h`; die fünf betroffenen nativen Feld-/Scroll-/Lese-/Verlauf-/
Mathematikfälle bestehen in `run-7eu3zujr`. Die vier Provider-/Render-/externen
Ereignis-/Zoomfälle bestehen mit MSVC ASan in
`build/native/F1-uia-sanitized/test-results/run-1b5w42va`.
Der Testläufer samt erwarteten Gegenfehlern besteht in
`build/native/F1-review/link-events-runner-proof`.

Beim Verbergen oder Minimieren bleibt der veröffentlichte Lesetext erhalten,
aber das Modell setzt native Aufträge und Fokus aus. Wartende Text-, Wert-,
Auswahl-, Scroll-, Fokus- und Aktivierungsaufträge werden verworfen. Erst eine
vollständig gezeichnete neue Publikation gibt die Aktionen wieder frei.
Inhalte, Inhaltsversionen und Elementidentitäten bleiben über diese Pause
erhalten. Das Fenster benötigt für diese Zustandsmeldung keinen neuen GPU-Frame.

Windows liefert in diesem Zustand keine sichtbare Glyphengeometrie und nur
einen leeren, degenerierten sichtbaren Textbereich. Fokus und aktiver Caret
sind aus; Fokus- und Aktivierungsanforderungen werden abgewiesen. Dies folgt
den Verträgen für [GetVisibleRanges](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextprovider-getvisibleranges)
und [GetBoundingRectangles](https://learn.microsoft.com/en-us/windows/win32/api/uiautomationcore/nf-uiautomationcore-itextrangeprovider-getboundingrectangles).
Das Lesen des erhaltenen Gesamttexts ist weiterhin möglich.

Der unabhängige Client hält dieselben Dokument-, Feld-, Link- und Textbereichs-
objekte über Escape/Schließen, Wiederöffnen, echtes Win32-Minimieren und
Wiederherstellen. Die Gegenprobe `run-v0zb0uxy` findet nach Schließen noch
aktiven Feldfokus; `run-hh37coiv` zeigt fehlende Sichtbarkeitsereignisse.
Die erste Korrektur besteht in `run-gztif0t0`. Die strengere Prüfung der
Wiederöffnung entdeckt anschließend einen alten Nuklear-Editierzustand
(`run-60i5pyfg`). Dessen ausdrückliche Rücksetzung verhindert erneuten Fokus
aus dem vorherigen Fensterzustand.

Die finale Probe bestätigt zwei Hidden- und zwei Shown-Ereignisse, keinen
Invoked-Event, unveränderten Gesamttext, Unicode-Feldwert und Inhaltsversion,
leere verborgene Textgeometrie, abgewiesene verborgene Aktionen und erhaltene
Elementidentitäten. Editor und Simulation bleiben gleich. Die Bildprüfung
entdeckte zunächst eine zu frühe Minimierung vor der Wiederöffnungsaufnahme;
der Client wartet nun auf die bestätigte Aufnahme. Beide finalen Bilder wurden
geprüft. Der ergänzte Modellfall prüft rückgenommene Aufträge, Aktionen während
der Pause, erhaltene Werte/IDs und Wiederfreigabe erst nach der Publikation.

Lebenszyklus und Linkereignisse bestehen 2/2 in `run-ysg64uez`; fünf betroffene
Modellfälle bestehen in `run-cgltr8ud`. Die drei Modell-/Feld-/Lesefälle bestehen
mit MSVC ASan in `run-v_fnovl6`; die abschließenden vier Provider-/Render-/
Lebenszyklus-/Linkfälle bestehen dort in `run-h32gapm8`. Die ASan-Ergebnisse
liegen unter `build/native/F1-uia-sanitized/test-results`. Vier weitere
Tastatur-/native Feld-/Lese-/Zoomfälle bestehen in `run-b824u53x`. Testkatalog
und erwartete Gegenfehler bestehen in `build/native/F1-review/lifecycle-runner-proof`.
Ein vorheriger ASan-Lauf verlor nach dem externen Fensterverschieben den
Linkfokus (`run-ke7exiiw`); der Client bestätigt den Fokus jetzt auch am Ende
seiner Bewegungs-/Idle-Prüfung. Der abschließende vollständige Fallverbund
und das Paar `run-tlbbydis` bestehen. Dies ersetzt keine vollständige
Fenster-/Monitor-/DPI-Matrix oder echte Screenreader-Abnahme.

Die Lebenszyklusbilder zeigen zusätzlich ein Ersatzzeichen für `🌍` im
Suchfeld, obwohl der native UTF-8-/UTF-16-Wert erhalten bleibt. Der Codepunkt
U+1F30D gehört nicht zu den aktuell geladenen Fontbereichen. Die mathematische
Inventur bleibt gültig, bestätigt aber keine solche Zeichenabdeckung.
Schriftfallback und eine umfassendere visuelle Zeichenprüfung bleiben offen.

Offen sind vollständige assistive Feldbedienung und ihre native
macOS-/Linux-Anbindung, native Linktextbereiche auf macOS/Linux, native
vollständige Fenster-/Monitor-Ereignismatrix auf allen Zielplattformen,
Schriftfallback für weitere Unicode-Zeichen, Tabellensemantik, vollständiges natives
Lesen auf macOS/Linux, praktische Screenreader-Abnahme
und die Abnahmematrix für Dichte, Themes sowie Linux und macOS. Diese Liste
beschreibt Restarbeit und erteilt keine Produktfreigabe.
