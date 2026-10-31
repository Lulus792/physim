# Einstellungen A: getrennte Suchaktionen und sichtbarer Fokus

31. Oktober 2026. Geprüfter Teilstand; keine gesamte UI-/Plattformabnahme.
[Kommandos, Quell-/Runtimearchive, ursprüngliche Ergebnisse und Hashes](settings-search-2026-10-31.json).

## Ergebnis und tatsächliche Prüfung

„Suche löschen“ im Suchkopf und „Suche zurücksetzen“ bei fehlenden Treffern haben
eigene Namen, Fokus-IDs und Besitzer. Der Inhaltsbutton gehört zur sichtbaren
Tabfolge; Umschalt+Tab führt zurück zur letzten Kategorie. Beide Aktionen geben
Tastatur und Texteingabe ans Suchfeld zurück. Der Einstellungsentwurf bleibt erhalten.

Die Breitenreservierung und Sichtbarkeit des Löschsymbols verwenden denselben
Suchzustand. Beim ersten Zeichen entsteht damit kein außerhalb des schmalen
Fensters gezeichneter Button. Abgescrollte, verdeckte Controls dürfen keinen
Pointerfokus übernehmen: `settings_focus_draw` prüft nun den realen Pane-Clip.
Layout, Kategorien und Theme folgen weiterhin der ausgewählten Referenz A.

Abschluss `settings-search-clipped`: 151 eigene Eingaben, Windows Release/MSVC
und Linux Debug/GCC/ASan/UBSan unter X11/Xvfb/Openbox/Software-GL. GUI-Leakprüfung
aus, Sanitizerfehler fatal. **Je 2/2** Gruppen: `settings_search_keyboard` und
`settings_a_window`, Windows `run-n2nnzihq`, Linux `run-z7knodp0`.
Je Betriebssystem vier tatsächliche App-Prozesse: UI 16/dunkel und UI 22/hell
für beide Gruppen. Texte, Tab/Shift+Tab, Leertaste/Enter, beide Pointeraktionen,
erneute Texteingabe, Entwurfserhaltung und der bestehende Fensterablauf bestehen.

Die Großschriftprobe scrollt Kategorie 21 bewusst hinter den festen Suchkopf.
Die Laufzeitspur bestätigt den ignorierten verdeckten Treffer; danach wird
„Sicherung“ tatsächlich ins Suchfeld eingegeben. Native SDL-Ereignisse und
publizierter Control-Fokus sind damit geprüft, keine physische Tasten-/Screenreader-Abnahme.
Vier gezielte finale PNGs zeigen den Rücksetzfokus; weitere Zustände zuerst als Text/JSON.

`b20d550e` ist in beide Repositories gepusht. Alle 1.109 erfassten Compiler-/
Testdateien stimmen direkt oder nach Zeilenendennormalisierung mit dem Commit
überein. [Commitabgleich und gestartete gehostete Nachläufe](settings-search-commit-2026-10-31.json).

Der getrennte Anschluss `settings-search-ready` besteht insgesamt je **2/3**:
allgemeines Einstellungen-A-Fenster und Inspektor bestehen, der damalige neue
Suchprüfer scheitert noch bei großer Schrift. Die Inspektorprüfung mit beiden
Fontquellen behält ihren tatsächlichen Pass; nach den späteren Suchkorrekturen
wurde sie nicht erneut ausgeführt. Keinen neuen gesamten Dreierlauf behaupten.

## Mac-Ergebnisse und Zeichnungsaufnahme

Auf `657219a3` sind die gehosteten Jobs beider Architekturen beendet:
Einstellungen/Einheiten `41177844214` je **12/13** (Header 3/3, Typografie 1/1,
kombinierte Gruppe 8/9). Nur `inspector_channel` scheitert; der anschließende
portable Preferences-Schritt wurde übersprungen. Die getrennte Menüregression
`41177844185` besteht je **7/7**. Alle vier ZIPs tatsächlich heruntergeladen,
GitHub-Digests und sämtliche Ergebnisdateien geprüft; Details stehen im JSON.

Der erste cm-Nachweis protokollierte noch „m“ im Selektor, während Katalog und
Wert schon cm verwendeten. Der Menüauftrag kann nach dem Selektorzeichnen im
selben Frame greifen. Die Fixture wartet jetzt höchstens drei Sekunden auf die
tatsächlich aktualisierte Selektorzeichnung. Daten-/Rasterkonsistenz und die
Mindestzahlen zwölf Felder/acht Werte bleiben erhalten. Diese Korrektur besteht
lokal im Inspektoranschluss; der neue tatsächliche Mac-Nachlauf bleibt offen.
Bei erneutem CI-Fehler werden gezielt App-Schritte und die erste cm-Aufnahme
gesichert; erfolgreiche Läufe erzeugen kein zusätzliches Diagnose-Bildarchiv.

## Ursprüngliche Fehler und Grenzen

Die ersten Testversuche enthielten fehlende Python-/CLI-Registrierung und ein
fehlendes Arbeitsverzeichnis. Ein weiterer Prüfer verwechselte den Gruppen-
Parameter des Accessibility-Getters mit dem Panenamen. Diese Fehlläufe sind
keine Produktabnahme. Später klickte die Probe vor abgeschlossenem Scrollen.
Alle ursprünglichen Kommandos/Ergebnisse und die konkrete Korrektur stehen im JSON.

Der anschließende Breitenfix legte einen weiteren echten Produktfehler offen:
Die Suche wurde gelöscht, aber die abgescrollte Kategorie übernahm denselben
Klick und setzte Fokus 21. Erst der Clip-Guard schließt diesen Gegenfall.
Keine fehlgeschlagenen Zwischenstände nachträglich als bestanden umdeuten.

Fremde AT-SPI-/Orca-Arbeit blieb außerhalb der isolierten Quelle. Nur D:;
der ausgeschaltete Nutzer-Mac wurde nie angesprochen. In dieser Suchkampagne
wurde keine weitere Bereinigung behauptet. Completion D mit tatsächlichen
Preferences/Backend, historische Settings-/AX-Fixtures, übrige freigegebene UI
und manuelle DPI-/Monitor-/Vollbild-/Screenreader-Aufgaben bleiben offen.

## Nachtrag: gehosteter Mac-Abschluss

Der nachfolgende Stand `b20d550e` besteht auf Intel und Apple Silicon je **15/15**,
getrennte Menüregression je **7/7**. Alle vier tatsächlichen Archive, Digests und
Ergebnisdateien sind geprüft. [Abschlussbeleg](settings-search-macos-2026-10-31.json).
Die ursprünglichen Fehl- und Wartesnapshots oben bleiben als Historie erhalten;
der spätere Einheiten-Fokusstand ist damit noch nicht abgenommen.
