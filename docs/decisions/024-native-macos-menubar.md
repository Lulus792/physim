# ADR 024: Physim-Menüs in der macOS-Systemmenüleiste

Status: Nutzerauftrag vom 30. Oktober 2026, Adapter implementiert;
gezielte native Builds/Tests auf Intel und Apple Silicon bestanden.
Manuelle Vollbild-/VoiceOver-Abnahme bleibt offen.

## Entscheidung

Unter macOS erscheinen Datei, Bearbeiten, Ansicht, Build und Hilfe in der echten
Menüleiste am oberen Bildschirmrand. Im Physim-Fenster entfällt ihre bisherige
Menüzeile; die übrige Gestaltung und die bestehenden Funktionen bleiben erhalten.
Windows und Linux behalten ihre bisherige Menüplatzierung.

Die macOS-Menüs verwenden dieselben Aktionen und Verfügbarkeitsregeln wie die
vorhandenen Menüs. Tastenkürzel und gesperrte Befehle müssen konsistent bleiben;
ein nativer Menübefehl darf nicht zusätzlich als zweites Tastaturereignis ausgeführt
werden. Hauptfenster, F1 und herausgelöste Aufgaben-/Bereichsfenster berücksichtigen.
Das Schließen eines Fensters darf keine veralteten Menüziele hinterlassen.

App-Menü, Fenster-/Hilfe-Integration und das Anzeigen/Ausblenden der Systemleiste
folgen AppKit/macOS. Die App zeichnet keine zweite Systemmenüleiste nach und
erzwingt keine eigene Vollbild-Ausblendregel. Bestehende Einstellungen werden über
ihre vorhandene Aktion angebunden; kein allgemeines neues App-Redesign ableiten.

## Technischer Anschluss und Abnahme

AppKit stellt die Menüleiste über
[`NSApplication.mainMenu`](https://developer.apple.com/documentation/appkit/nsapplication/mainmenu)
bereit. Apples [Menüleisten-Leitlinie](https://developer.apple.com/design/human-interface-guidelines/the-menu-bar)
beschreibt Reihenfolge, deaktivierte statt versteckte Befehle, Kürzel und
systemverwaltetes Vollbildverhalten. Die offiziellen Markdown-Fassungen wurden
am 30.10. gelesen; lokale Quellenhashes liegen unter
`build/validation/macos-menubar-2026-10-30/source-references.json`.

Der C17-/SDL-Bestand kann einen separaten AppKit-Adapter verwenden; die vorhandene
native Accessibility-Brücke nutzt bereits typisierte Objective-C-Runtime-Aufrufe
aus C und die App verlinkt AppKit. Menüdefinition, Zustandsprüfung und Ausführung
sollen einen gemeinsamen fachlichen Ursprung behalten.

Der gelesene [SDL-3.2.30-Cocoa-Adapter](https://github.com/libsdl-org/SDL/blob/release-3.2.30/src/video/cocoa/SDL_cocoaevents.m)
erstellt bereits App-/Fenstermenüs samt einem noch ungebundenen Preferences-
Eintrag. Diese OS-Anbindung erhalten und anbinden. Sein `sendEvent:` ruft den
SDL-Tastaturhandler vor AppKits `super sendEvent:` auf: Bei nativen Kürzeln deshalb
eine einzelne Befehlsausführung und die Trennung von Menü-Navigation und
Editor-/Szeneneingabe gezielt nativ prüfen. Noch kein beobachteter Doppelaufruf
in Physim zum Zeitpunkt der Quellenprüfung. Beide gepinnten
Cocoa-Quelldateien und ihre Hashes liegen im genannten lokalen Prüfverzeichnis.

Erforderlich sind ein tatsächlicher macOS-Build und native Prüfungen für
Menüeinträge, Verfügbarkeit, einzelne Befehlsausführung, Kürzel, Fensterwechsel und
geordneten Shutdown. Praktische Menü-/Vollbild-/Assistive-Bedienung bleibt als
eigene Dimension kenntlich. Windows-/Linux-Prüfungen beweisen die AppKit-Seite nicht.
Vor Nutzung des lokalen Mac die in `AGENTS.md` verlinkten aktuellen NexaCloud-/
NexaUI-Hinweise einschließlich Verfügbarkeit und Bereinigung lesen.

## Implementierung

`app/native_menu.c` bindet AppKit aus C17 an. Die fünf Hauptmenüs stammen aus
denselben `toolbar_entries` wie bisher; Aktionen laufen nach erneuter Prüfung
über `toolbar_dispatch`. `native_menu_ui.inc` veröffentlicht Beschriftungen,
Verfügbarkeit und das aktuelle SDL-Fenster. Einstellungen belegt den vorhandenen
SDL-Appmenüeintrag; App-/Fenstermenü sowie Services, Ausblenden und Beenden bleiben
in Systemhand. Im Mac-Fenster werden nur die bisherigen Menüknöpfe ausgelassen.

Native Kürzel besitzen genau ihr bereits eingereihtes SDL-KeyDown; dieser wird
vor allen Fenster-/Editorhandlern abgefangen. Menü-Tracking verwendet die
offiziellen [Beginn-](https://developer.apple.com/documentation/appkit/nsmenu/didbegintrackingnotification)
und [Ende-Benachrichtigungen](https://developer.apple.com/documentation/appkit/nsmenu/didendtrackingnotification)
und eine SDL-Warteschlangengrenze, damit Menü-Navigation nicht den Editor bedient.
Frühere Eingaben und eingereihte Appbefehle bleiben in ihrer Reihenfolge erhalten.
Beim Shutdown werden gehaltene Menüziele entwaffnet und bestehende SDL-Einträge
wiederhergestellt. Windows/Linux verwenden einen wirkungslosen Plattformadapter.

`tests/test_native_menu.c` prüft auf macOS tatsächliche NSMenu-Objekte, Zustände,
Aktionszustellung, ein über AppKit eingereihtes ⌘S bei aktiver Texteingabe,
Fensterwechsel, die Tracking-Grenze und gehaltene Objekte nach Shutdown.
Die Benachrichtigungs-Gegenprobe ist kein manueller Navigations-/Vollbildnachweis.
Der eigene Workflow `macos-native-menu.yml` trennt diesen Plattformtest vom bereits
vorher fehlerhaften Dokumentationsanker-Gate; dieses Gate wird nicht abgeschwächt.

Der ergänzte `window_smoke`-Pfad prüft in der tatsächlich gestarteten Mac-App,
dass AppKit aktiv ist, die alten fünf Menüknöpfe keine Fläche mehr besitzen und
Bereichswechsel sowie Fensterbedienung weiterhin gezeichnet werden. Der native
Adapter ist auf Intel und Apple Silicon im ersten Stand `d43dcd6c` bestanden;
der abschließende [App-Shell-Nachweis auf `517021f0`](../verification/macos-menubar-2026-10-30.json)
besteht auf beiden Architekturen je zwei Fälle. Die heruntergeladenen kompakten
Ergebnisarchive wurden gegen die GitHub-SHA256-Digests geprüft. Windows Release
und Linux Debug ASan/UBSan bestehen je vier Menü-/Headerfälle plus App-Start.

Die vollständige bisherige Mac-GUI-Suite ist damit nicht abgenommen. Mehrere
historische Toolbar-/Tastatur- und Dokumenttests klicken ausdrücklich auf die
alten In-Window-Menüflächen beziehungsweise erwarten F10 im eigenen Menü.
Diese Erwartungen müssen auf den neuen macOS-Eingabeweg portiert werden;
unveränderte Windows-/Linux-Nachweise bleiben gültig. Keine künstliche Aktivierung
der alten Mac-Menüflächen ausschließlich zum Bestehen dieser Tests verwenden.
