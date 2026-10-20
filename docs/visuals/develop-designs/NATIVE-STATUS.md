# Native Werkbank – Prüfbericht

**Überarbeitung seit 20. Oktober 2026:** Die Rückmeldung des Nutzers bestätigt,
dass die bisherige Gestaltung die gewählte Werkbank nicht ausreichend trifft.
Die folgenden Nachweise vom 19. Oktober belegen Funktionen und damalige Bilder;
sie sind keine Abnahme der visuellen Übereinstimmung. Der engere native Abgleich
steht mit aktuellen Bildern, konkreten Korrekturen und verbleibenden Unterschieden
in [FIDELITY.md](FIDELITY.md). Die folgenden Ergebnisse bleiben historisch.

Stand: 19. Oktober 2026, Ausgangsrevision `e5c3144`, Zweig `codex/f1-design-h`.
Die native Umsetzung ist implementiert und unter Windows und Linux lokal geprüft.
Auch die vollständige aktuelle Windows-Fokusmatrix ist bestanden. Die unten
genannten Plattform-/Assistive-Nachweise bleiben offen.

## Umsetzung

Die bestehende C17-/SDL3-/Nuklear-/OpenGL-Oberfläche erhält kompakte Dateitabs,
Workspace-Pfadlinien, Panelköpfe und die gemeinsame Suchinsel. Projektbanner und
Suchzeile vor dem Text entfallen. Die Codefläche beginnt direkt unter den Tabs;
ihr oberer Textinnenabstand beträgt sechs logische Pixel. Suchinsel und Textfeld
verwenden getrennte Rechtecke: vier Pixel unter der Codefläche, acht Pixel vom
rechten Rand der gesamten Codefläche, höchstens 540 Pixel bzw. Paneelbreite minus 16.

Die Suche arbeitet auf dem tatsächlichen aktiven Experiment-, Analyse- oder
Texteditor. Literal, case-sensitiv, nicht überlappend, mit Vor-/Rückwärtssuche,
Umbruch und Trefferzahl. Bytepositionen werden für Nuklears UTF-8-Auswahl in
Zeichenpositionen umgerechnet. Ersetzen verwendet den vorhandenen Undo-, Dirty-,
Save- und Recovery-Pfad. Ctrl/Command+F öffnet und fokussiert; Escape schließt und
fordert den Fokus am Suchicon an. Die Insel belegt keinen Platz im Dokumentfluss.

Das Glas verwendet zwei begrenzte GPU-Filterpässe: Framebuffer-Blit des bereits
gezeichneten Inselhintergrunds mit Filterrand, horizontaler Filter auf separater
Textur, vertikaler Filter beim gerundeten Zusammensetzen. Sättigung 1,15, kühle
24-Prozent-Tönung und Lichtgradient entsprechen der Referenz. 16/9 Pixel Radien,
Vektoricons mit 1,5-Pixel-Strichen und runden Endpunkten, gemeinsame Navigation,
getrennte Hinweisbadges und stärkere Eingabeflächen sind nativ gezeichnet.
Kein CPU-Readback im Materialpfad; keine Filterpässe bei geschlossener Suche.
Temporäre Filtertexturen werden beim Schließen bzw. deckendem Fallback freigegeben.

Der Kontextschalter besitzt ein gemeinsames, stabiles Rechteck ganz rechts;
Workspace-Schalter, Suche und Speichern bleiben getrennte Aktionen. Die vorhandene
Dockingstruktur einschließlich Größen, Tabgruppen, schwebender und ausgeblendeter
Panels bleibt die einzige Layoutquelle. Das Wiederöffnen am Rand umschließt den
bestehenden Dockbaum, statt ein beliebiges erstes Blatt zu teilen. Die Werkbank
reserviert mindestens 300 Pixel für den Editor. Die native Breitenklemmen bleiben
208–360 Pixel für den Workspace und 208–480 für den Kontext; gespeicherte Werte
werden beim bloßen Fensterresize nicht überschrieben.

Separatoren besitzen Namen, Größenwert und Orientierung: Pfeile 10 Pixel,
Umschalt 40, Home/End, Enter/Doppelklick zum Reset und Escape zum Abbrechen.
UIA erhält Separator/RangeValue, AT-SPI Separator/Value, AppKit AXSplitter.
Profil und tatsächlicher Buildzustand stehen unten; Datei, Zeile/Spalte und UTF-8
kommen aus dem aktiven Editor. Build-/Abbruch-, Diagnose-, Completion-, API-,
Dateidialog- und Wiederherstellungswege bleiben vorhanden. Der Kontext zeigt
vorhandene API-Daten oder einen verständlichen Leerzustand.

## Ausgeführte Prüfungen

- Windows/MSVC mit AddressSanitizer: **13/13 bestanden**, `run-5u54fb7n`.
  Linux/Alpine-Dev GCC 15.2 mit ASan/UBSan: **13/13 bestanden**, `run-81zm3lsd`.
  Umfang: Suchkern, neun Accessibility-Modelle/Indizes, Docking, UI-Geometrie
  und persistente Einstellungen.
- Der Suchkern wird gegen einen unabhängigen erschöpfenden Scanner verglichen;
  zusätzlich UTF-8, Zeilenumbrüche, leere/fehlende Treffer und ein adversarialer
  großer Text. Docking prüft unter anderem alle Panels/Ränder und Wire-Roundtrips.
- Reale GPU-Pixelprüfungen bestanden unter Windows/NVIDIA RTX 2080 Ti,
  OpenGL 3.3, Treiber 610.88, und Linux/Mesa 26.1.6 llvmpipe, OpenGL 4.6.
  Nachgewiesen: Filterwirkung, deckende exakte Fallbackfarbe, nachfolgende normale
  Zeichenbefehle, Schließen/Wiederöffnen sowie PNG-Export mit zweifacher und
  dreifacher Pixeldichte. Dies ersetzt keinen physischen Monitorwechsel.
- Der unabhängig beobachtete Win32-Fokuslauf vom 19. Oktober bestand:
  `build/native/develop-windows-host-coordinated/host-audit/results.json`.
  GetForegroundWindow/GetGUIThreadInfo bestätigten Vordergrund-, Aktiv- und
  Tastaturfokus. Der native Ablauf prüfte die Separator-Tastaturfolge und Maus-
  Resize mit Escape. Der Prüfer aktiviert selbst keine Fenster.
- Aktuelle Windows-GPU-, Editor-Replace- und Completion-Prüfungen: **3/3**,
  `run-5ltoaw5c`. C und Physim werden jeweils für Experiment, Analyse und eine
  zusätzliche UTF-8-Textdatei geprüft: Ersetzen, leere Ersetzung, Undo/Redo,
  Dirty, echtes Speichern und genaue Dateiinhalte. Source-Undo funktioniert
  auch, wenn die Maus danach über der Suchinsel steht.
- Die abschließende aktuelle Windows-Werkbankmatrix bestand **6/6 App-Läufe**
  im koordinierten Vordergrund-Zeitfenster: dunkel/hell mit allen vier
  Seitenzuständen und vollständigem Maus-/Tastaturablauf, schmales Fenster,
  große Schrift, deckender Fallback und hoher Kontrast. Alle sechs bestätigen
  die tatsächliche Fokus-Rückgabe; die beiden vollständigen Läufe zusätzlich
  Separatorfokus, Resize/Abbruch/Reset, Mausrad außerhalb der Suchinsel und
  Vorrang des Layoutdialogs. [Nachweis](native/windows-matrix.json), geprüfte
  Programm-SHA-256 `7c9790494268ac29c8543573491a98d04213ffdfe51675e02b277557bb516e65`.
- Linux: **5/5** native Gruppen in `run-fmcga_dl`, danach **3/3** mit den
  zusätzlichen nichtmodalen Fällen in `run-8klzpoj1`. Abschließende vollständige
  Werkbankmatrix **1/1** in `run-1r7b0qf6`: beide Erscheinungen, alle vier
  Seitenzustände, 1080×740, 1600×1000 mit UI-/Codeschrift 22, deckender Fallback,
  hoher Kontrast, Fokuskehr, Mausbewegung ohne Fokusverlust, Mausrad im Code,
  Resize-Pfeile/Shift/Home/End/Enter/Escape, Maus-Abbruch und Doppelklick-Reset.
  Ein echter Layoutdialog erhält Escape vor der noch offenen Dokumentsuche.
- Vorhandene Regressionen bestanden zusätzlich für Datei-Tabs, Workspace-Baum,
  Themes, Recovery, gespeicherte/schwebende/gruppierte Docklayouts, API-Referenz,
  erfolgreichen/abgebrochenen Build und Compiler-/Laufzeitdiagnosesprung. Die
  jeweiligen ausgeführten Stände und Einzelgruppen stehen im
  [Prüfmanifest](native-validation.json); die ältere breite Suite war wegen der
  dort inzwischen korrigierten Befunde ausdrücklich kein Gesamterfolg.
- Das aktuelle Windows-Programm wurde einschließlich passendem PDB in
  `build/native/Debug-sanitized/bin` übernommen. SDL-/ASan-Laufzeitdateien wurden
  vorher per SHA-256 abgeglichen; der anschließende `physim.exe --smoke` bestand.
  Frühere separate F1-/Orca-Arbeitsänderungen wurden bewahrt und nicht gemeinsam
  mit dieser Werkbankänderung committet. Geprüft wurde der gemeinsame lokale Stand.

Die automatisierte Windows-Werkbankmatrix verlangt echten veröffentlichten
Hostfokus. Frühere Läufe ohne koordinierten Vordergrund konnten den Fokus am
Suchicon nicht bestätigen; Auswahl und Scrollstand blieben dabei unverändert.
Die abschließende koordinierte Matrix bestand mit unveränderter Fokusprüfung.
Der separate frühere Win32-Beobachterlauf bleibt unter seiner eigenen
Binär-Prüfsumme dokumentiert; die beiden Nachweise werden nicht gleichgesetzt.

Reproduzierbare Einstiegskommandos (PowerShell, vom Projektverzeichnis):

```powershell
python tools/build.py --build-dir build/native/F1-reading-focus/Debug-sanitized --sanitizers --jobs 4 --test --test-filter "accessibility*" --test-filter docking --test-filter editor_search --test-filter ui_geometry --test-filter preferences
python tools/build.py --build-dir build/native/F1-reading-focus/Debug-sanitized --sanitizers --jobs 4 --test-display --test-filter backdrop_ui --test-filter editor_replace_workflow --test-filter completion_workflow
python tests/check_documentation_windows_host.py --mode workbench-dark --force-raise --deadline 20 --app build/native/F1-reading-focus/Debug-sanitized/bin/physim.exe --work build/native/workbench-focus-new
python tests/check_display_workflow.py workbench_workflow --app build/native/F1-reading-focus/Debug-sanitized/bin/physim.exe --work build/native/workbench-matrix-new --root D:/Physim
```

Linux wurde tatsächlich in `Alpine-Dev` mit GCC, SDL3 3.2.30, Xvfb/Openbox und
DBus ausgeführt. Buildparameter: `--build-dir build/native/F1-linux/Debug-sanitized
--sdl build/native/F1-linux/sdl-install --compiler gcc --sanitizers --jobs 4`.
Der Display-Lauf verwendet `ASAN_OPTIONS=detect_leaks=0` für SDL/Mesa;
separate Modellläufe besitzen diese Ausnahme nicht. Danach etwa:

```sh
xvfb-run -a -s '-screen 0 1600x1100x24' dbus-run-session -- sh -c 'openbox >/tmp/physim-workbench-wm.log 2>&1 & python3 tools/wait-window-manager.py && ASAN_OPTIONS=detect_leaks=0 python3 tools/build.py --build-dir build/native/F1-linux/Debug-sanitized --sdl build/native/F1-linux/sdl-install --compiler gcc --sanitizers --jobs 4 --test-display --test-filter workbench_workflow'
```

## Native Bilder

Echte Linux-Clientaufnahmen, PNG-Konvertierung ohne Bildretusche, logische und
Geräteskalierung 1×. Normal 1204×866 bei UI-/Codeschrift 16; große Ansicht mit
22. Schatten und Codehintergrund stammen aus dem nativen GPU-Renderer.

| Fall | Aufnahme |
| --- | --- |
| Normal dunkel/hell | [Dunkel](native/linux-dark.png), [Hell](native/linux-light.png) |
| Ersetzung erweitert | [Dunkel](native/linux-dark-expanded.png), [Hell](native/linux-light-expanded.png) |
| Rechter Kontext verborgen | [Dunkel](native/linux-dark-right-hidden.png), [Hell](native/linux-light-right-hidden.png) |
| Linker Workspace verborgen | [Dunkel](native/linux-dark-left-hidden.png), [Hell](native/linux-light-left-hidden.png) |
| Beide Seiten verborgen | [Dunkel](native/linux-dark-both-hidden.png), [Hell](native/linux-light-both-hidden.png) |
| Schmales Fenster 1080×740 | [Suche](native/linux-dark-narrow.png), [Ersetzung](native/linux-dark-narrow-expanded.png) |
| Groß 1600×1000, Schrift 22 | [Suche](native/linux-light-large.png), [Ersetzung](native/linux-light-large-expanded.png) |
| Transparenz reduziert | [Suche](native/linux-dark-opaque.png), [Ersetzung](native/linux-dark-opaque-expanded.png) |
| Hoher Kontrast | [Suche](native/linux-high-contrast.png), [Ersetzung](native/linux-high-contrast-expanded.png) |

[Unabhängige Windows-Fokusbeobachtung](native/windows-focus-observation.json).

Die abschließende Windows-Matrix enthält dieselben 18 nativen Aufnahmezustände
mit gleicher Fenster-/Schriftkonfiguration und ohne Bildretusche:

| Fall | Windows-Aufnahme |
| --- | --- |
| Normal dunkel/hell | [Dunkel](native/windows-dark.png), [Hell](native/windows-light.png) |
| Ersetzung erweitert | [Dunkel](native/windows-dark-expanded.png), [Hell](native/windows-light-expanded.png) |
| Rechter Kontext verborgen | [Dunkel](native/windows-dark-right-hidden.png), [Hell](native/windows-light-right-hidden.png) |
| Linker Workspace verborgen | [Dunkel](native/windows-dark-left-hidden.png), [Hell](native/windows-light-left-hidden.png) |
| Beide Seiten verborgen | [Dunkel](native/windows-dark-both-hidden.png), [Hell](native/windows-light-both-hidden.png) |
| Schmales Fenster 1080×740 | [Suche](native/windows-dark-narrow.png), [Ersetzung](native/windows-dark-narrow-expanded.png) |
| Groß 1600×1000, Schrift 22 | [Suche](native/windows-light-large.png), [Ersetzung](native/windows-light-large-expanded.png) |
| Transparenz reduziert | [Suche](native/windows-dark-opaque.png), [Ersetzung](native/windows-dark-opaque-expanded.png) |
| Hoher Kontrast | [Suche](native/windows-high-contrast.png), [Ersetzung](native/windows-high-contrast-expanded.png) |

Der Bildvergleich bestätigte die gewählte kühle Suchinsel, klare Trennung von
normalem/expandiertem Zustand, feste Schalter und durchgehende Codegeometrie.
Die oben begrenzte Plattformabnahme bleibt davon getrennt.

## Gefundene und korrigierte Ursachen

Nuklear bündelte zunächst die weiße Materialfläche mit gewöhnlichen Formen.
Explizite Konvertierungsgrenzen vor/nach dem Material erhalten dessen Shader-
Zuordnung. Ein nach Hide freigewordener Dockknoten wurde noch als Split behandelt;
leere Knoten werden ausgeschlossen. Das Wiederöffnen des Workspace teilte zuvor
den falschen Ast; der Rand-Reveal erhält jetzt die übrige Aufteilung.

Eine stehengebliebene Doppelklick-Eingabe konnte den ersten Resize-Klick als Reset
behandeln. Der Backendzustand wird bei jeder linken Maustastenaktion aktualisiert;
Reset verlangt aufeinanderfolgende Klicks am selben Trenner. Der neue native
Ablauf prüft sowohl Ziehen/Abbrechen als auch einen ausdrücklichen Doppelklick.

Die neue Statusleiste konnte die aktive Nuklear-Editorfläche übernehmen, wenn der
Pointer über der Completion lag. Die Statusleiste routet ihren eigenen begrenzten
Eingabebereich und erhält den bisherigen Tastaturbesitzer. Die vorhandene
Completion-Folge prüft Texteingabe und Enter im selben Frame erneut. Ein leerer
Nuklear-String kann trotz zurückgehaltenem Speicher keinen Textzeiger liefern;
die Cursorumrechnung behandelt leer, Anfang und Ende vor der Zeigerarithmetik.
Der Sanitizer-Leerdokumentfall besteht danach.

Mausbewegung allein darf den Tastaturbesitzer nicht wechseln. Die Suchinsel
behält ihren expliziten Besitzer bis zu Klick/Fokusübergabe; reine Hover-Flächen
nehmen keine Quelltastaturereignisse an. Mausrad außerhalb der Insel aktiviert
nur deren native Code-Scrollbar mit maskierter Tastatur und erhält die Auswahl.
Die neuen nativen Folgen prüfen beide Richtungen dieser nichtmodalen Bedienung.

Tests wählen Dokumente über die tatsächlichen Dateitabs, berücksichtigen den
zusätzlichen Build-Einstellungsmenüpunkt und scrollen verdeckte Diagnosezeilen
vor dem Anklicken sichtbar. Strukturierte Laufzeitdiagnosen werden unabhängig
von zusätzlichen echten Compilerwarnungen geprüft. Erwartungen an Dateiinhalte,
Auswahl, Undo/Redo, Fehlerdaten und Sprungziel bleiben bestehen.

## Begründete Unterschiede zur Browserreferenz

Die native App verwendet ihre echten Dateinamen, Quellen, Compiler und Builddaten.
Demo-Outline, Modellnotiz und fingierte Buildumgebung werden nicht nachgebaut.
Die vorhandene UI-/Codeschrift und gespeicherte Panelgrößen haben Vorrang vor den
kleinen Präsentationsschriften und den 218/255-Pixel-Demowerten. Die Prüfaufnahmen
setzen diese Referenzbreiten ausdrücklich nur in ihrer isolierten Testkonfiguration.

Nuklear/OpenGL und Browser/CSS besitzen unterschiedliche Text- und Kantenglättung.
Der Schatten ist durch abgestufte native Konturen angenähert; der Blur filtert
hingegen tatsächlich den Codehintergrund. Pixelidentität zwischen diesen
Renderern wird nicht behauptet. Die native Mindestfenstergröße und frei gespeicherte
Dockingaufteilung ersetzen die CSS-Stapelregeln für 760/500-Pixel-Browseransichten.
Simulieren und die Ergebnisansicht von Auswerten erhalten keine neue Gestaltung.
Die Analysecodeansicht erhält dieselbe funktionale Suchinsel.

## Transparenz und offene Plattformnachweise

Hoher App-Kontrast, fehlende Filterressourcen und reduzierte Transparenz verwenden
bei gleichem Grundriss deckende semantische Flächen. Portabler expliziter Schalter:
`PHYSIM_REDUCE_TRANSPARENCY=1` vor dem Start. Windows prüft den öffentlichen
High-Contrast-Parameter und zusätzlich Best-Effort `EnableTransparency`. AppKit
fragt Reduce Transparency/Increase Contrast ab. Für Linux wird keine universelle
Systempräferenz behauptet.

Primärquellen: [Microsoft High Contrast](https://learn.microsoft.com/en-us/windows/win32/winauto/high-contrast-parameter),
[AppKit Reduce Transparency](https://developer.apple.com/documentation/appkit/nsworkspace/accessibilitydisplayshouldreducetransparency).
Die Registry-Abfrage ist kein zugesicherter öffentlicher API-Vertrag.

Offen bleiben eine neue macOS-Ausführung dieses Patches, tatsächliche Screenreader-
Bedienung der neuen Werkbankziele und physische DPI-/Monitorwechsel einschließlich
Windows-Systemtextskalierung. Exportskalierung und große App-Schrift sind dafür
getrennte, begrenzte Nachweise. Bestehende F1-/Orca-Arbeiten bleiben separat erhalten.

## Bereinigung

Nach Abnahme wurden 388 ersetzte Roh-/Prüfbilder mit zusammen 1.184.742.051 Bytes
(rund 1,10 GiB Dateivolumen) entfernt. Quellen, Experimente, Abhängigkeiten,
Git-Historie, Referenzen, finale native PNGs, aktuelle Prüfläufe, JSON-/Logbelege
und Bilder negativer/offener Fokusfälle bleiben erhalten. Keine pauschale
Build-/Cacheleerung; keine WSL-Datenträger wurden verändert.
