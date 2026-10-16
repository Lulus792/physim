# Direkte Bereichswechsel im nativen H-Handbuch

F6 wechselt zwischen Kopfsuche, Themenliste, Artikel und ausgewähltem Kontexttab.
Shift+F6 kehrt die Richtung um. Geschlossene Seiten und leere Trefferlisten
werden übersprungen. Tab und Shift+Tab erhalten die vollständige Reihenfolge.
Suchtext, Lesestelle, Sprachwahl und rechter Tab bleiben erhalten; die F1-Tasten
werden weiterhin ausschließlich im Dokumentationsfenster verarbeitet.

## Gefundene Ursachen und Korrekturen

Der frühere Inline-Prüfer traversierte für Artikelaktionen die umfangreiche
Themenliste. Auf Intel/macOS wurde dabei das bestehende interne Limit von 60 s
erreicht. Der Prüfer verwendet nun die tatsächlich implementierte F6-Bedienung
und danach Tab innerhalb des Artikels. Die Zeitgrenze bleibt unverändert. Die
Ausgabe zählt echte Tab- und F6-Ereignisse; der Python-Prüfer verlangt für die
drei Artikelaktionen höchstens zwölf Tabs und vier Bereichssprünge.

Die unabhängige normale App ohne Selbsttestschalter weist das fehlende F6 auf
dem alten Binary gezielt nach (`h-panes-before-synchronized/results.json`).
Nach Einführung von F6 legte sie außerdem die blockierte globale Texteingabe
nach einem Tooltip offen (`h-panes-normal-paced/results.json`): der native
Suchfeldwert blieb trotz tatsächlicher Tasten bei `Pendel`. Nuklear behält das
Fenster eines geschlossenen Tooltips; `nk_sdl_update_TextInput` darf deshalb nur
einem **aktiven** Popup die Eingabehoheit geben. Der gespeicherte Popup-Zeiger
allein reicht nicht.

Bei UI 22 erkannte die neue Matrix einen weiteren tatsächlichen Fehler: Der
Themenbutton wurde auf ein ganzzahliges Rechteck gezeichnet, die native
Fokuszuordnung erhielt jedoch das ungerundete Ergebnis von `nk_widget_bounds`.
Beide verwenden jetzt das Ergebnis des tatsächlichen `nk_widget`-Aufrufs.
Der Gegenlauf `run-p_stujlt` scheiterte gezielt bei Fall 4 am Themenfokus.
Ein früherer Prüferversuch hatte zusätzlich fälschlich Fensternamen als native
Elterngruppen angenommen; dieser Prüfaufbau ist korrigiert und kein Produktfehler.

## Ausgeführte Abnahme

Linux: Alpine/X11, SDL 3.2.30, GCC 15.2 mit ASan/UBSan,
`build/native/F1-linux/Debug-sanitized/test-results/run-ztsqqs2x`: **10/10**.
Geprüft sind Tastatur, Verlauf, Zoom, Inlineinhalt, Navigation, Sidebar, H,
die neue Bereichsmatrix sowie externe AT-SPI-Dokument- und Eingabefeldaufgaben.
Die Matrix prüft 24 Zustände: drei Themes, zwei Schriftkonfigurationen und vier
Seitenkombinationen, beide Sprachen und beide Kontexttabs verteilt. Jeder Zustand
durchläuft einen vollständigen Vorwärts- und Rückwärtszyklus, einschließlich
tatsächlicher nativer Control-Fokuszustände und SDL-Texteingabe nach Tooltip.
Quelltexte, Simulationszeit sowie Lauf-/Resetzustände müssen unverändert bleiben.
Nach Ergänzung des begrenzten tatsächlichen Hostfokus-Starts besteht die finale
Bereichsmatrix separat **1/1** (`run-y9d257jh`), weiterhin mit allen 24 Zuständen.

Die normale App besteht anschließend `h-panes-normal-final/results.json` mit
echten X11-Maus-/Tastenereignissen und unabhängigen AT-SPI-Abfragen: beide
Sprachsegmente und Kontexttabs, getrennte Seitenaktionen, Dokumentfokus,
erhaltener Suchtext sowie leere und wiederhergestellte Trefferlisten.
Der Prüfer wartet auf tatsächliche Fensterposition und native Textauswahl.
Binary-SHA-256: `0e096db3fe025ae32f99303606b0d86063054aa33cb565a52843154be21ddf49`.
Der Wiederlauf nach der letzten Testeinrichtungsänderung besteht ebenfalls:
`h-panes-normal-published/results.json`, Binary-SHA-256
`71d613f3bf01d86949c8db98a56ceddae18e21f69e2cbfae124fc1ebcde7cfa9`.
Die Inlineaktionen benötigen in diesem Lauf 2/3/4 Tabs und 3/0/3 F6-Ereignisse;
die letzte Kopieraktion wird nach 5,993 s erreicht. Das ist ein Laufwert,
keine allgemeine Latenzgarantie.

Windows/MSVC/ASan: `run-hipv6vh6` besteht **8/10**; H, Tastatur, Verlauf, Zoom,
Inlineinhalt, Navigation, Sidebar und der externe Dokumenttest bestehen.
Die externe Feldprüfung scheitert bei der Rückkehr des Tastaturfokus, die
Bereichsmatrix beim ersten nativen Control-Fokus. Der gezielte Wiederlauf
`run-pjk_1j1x` besteht **0/2**. Die daraufhin präzisierte Testeinrichtung fordert
das Fenster ausdrücklich an und wartet höchstens eine Sekunde auf tatsächlichen
Hostfokus. `run-4at5jpj3` besteht wiederum **0/2** und grenzt den Startbefund ein:
`document=4 keyboard=0`, bereits vor dem ersten F6-Zyklus. Die Sitzung besitzt
einen zugänglichen Default-Desktop und ein Vordergrundfenster; aus dem fehlenden
Appfokus folgt deshalb keine Behauptung einer gesperrten Sitzung. Die konkrete
Ursache der verweigerten Windows-Fokusübernahme bleibt offen. Die Prüfungen
werden weder übersprungen noch als bestanden gewertet.

Die abschließende Windows-Gruppe für H und allgemeine Tastaturführung besteht
**2/2** (`run-5pk_vgl3`). Der geprüfte Build liegt unter
`build/native/Debug-sanitized/bin/physim.exe`, F1 öffnet die Dokumentation.
SHA-256: `e4c796a99819ab77f98fd1ef4e1bd66a11277a39c6de3e008207fece46354570`.
Die vorherige EXE ist als `build/native/physim-before-pane-navigation.exe`
gesichert; die drei SDL-/ASan-Laufzeit-DLLs besitzen in beiden Startordnern
identische Hashes. Eine laufende Nutzer-App wurde nicht ersetzt oder beendet.
Das aktuelle dunkle H-Fenster `docs-h-00.bmp` ist nach verlustfreier Dekodierung
pixelidentisch mit `native-h-fidelity-dark.png`; die Fokuskorrekturen verändern
dieses visuelle Zielbild nicht (`h-panes-visual-comparison.json`).

Der [tatsächliche Mac-Lauf von c01ac79](https://github.com/Lulus792/physim/actions/runs/38950207861)
belegt die Erweiterung auf ARM: **19/19 portable, 27/27 GUI**, alle Gegenproben
und externe AX; Gesamtjob bestanden. Portable `run-fa7dqkcr`, GUI `run-_jdmfz4u`.
Intel besteht **19/19 portable, 24/27 GUI** und externe AX
(`run-iuyrw0u1`, `run-_xbdyne4`). Typeset, Bereiche und H erreichen die interne
60-s-Grenze bei Stage 5, 4 beziehungsweise 16. Die nachfolgenden Gegenproben
werden dort nicht ausgeführt. Diese tatsächlichen Zeitlimits bleiben offen;
die Grenze wurde nicht angehoben. Geschützte Jobauszüge liegen unter
`build/native/mac-c01-117139390326-excerpt.log` und dem entsprechenden ARM-Auszug.

Der nachfolgende tatsächliche [Lauf von 10e318a](https://github.com/Lulus792/physim/actions/runs/38971626070)
besteht auf beiden Architekturen 19/19 portable und 27/27 GUI-Fälle samt
Gegenproben und externer AX-Prüfung. ARM: `run-xlf391t7` / `run-h7zbrac9`;
Intel: `run-vdzvhwxj` / `run-4ll3_mp_`. Die Auszüge liegen unter
`build/native/mac10e-117211115929-verified.log` beziehungsweise
`mac10e-117211115686-verified.log`. Die anschließende H-Detailrevision `3d89897`
besitzt ebenfalls zwei vollständig erfolgreiche Mac-Jobs; Details in
[H-FIDELITY.md](H-FIDELITY.md). Diese Grünläufe belegen die jeweilige Ausführung;
sie erklären nicht rückwirkend die sporadischen älteren Zeitlimits.

Die anschließende [Windows-Fokuszustandskorrektur](WINDOWS-FOCUS-STATE.md)
hat eigene Gegenproben und neue reale Win32-Nachweise. Sie erklärt nicht
rückwirkend sämtliche älteren Aktivierungsfehler.
Vollständige Screenreader-Aufgaben und physische DPI-Wechsel bleiben
weiter offen; aus diesen begrenzten Prüfungen folgt keine Höchstwertung.

Der abschließende native Markdown-Parserlauf bestätigt alle 185 registrierten
Seiten und 220 internen Abschnittslinks (`h-panes-anchors-final.log`). Auch die
Prüfung des Testkatalogs samt Fehler-/Timeoutberichten besteht.
