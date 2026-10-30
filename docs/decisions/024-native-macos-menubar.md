# ADR 024: Physim-Menüs in der macOS-Systemmenüleiste

Status: Nutzerauftrag vom 30. Oktober 2026, native Umsetzung und Abnahme offen.

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

Erforderlich sind ein tatsächlicher macOS-Build und native Prüfungen für
Menüeinträge, Verfügbarkeit, einzelne Befehlsausführung, Kürzel, Fensterwechsel und
geordneten Shutdown. Praktische Menü-/Vollbild-/Assistive-Bedienung bleibt als
eigene Dimension kenntlich. Windows-/Linux-Prüfungen beweisen die AppKit-Seite nicht.
Vor Nutzung des lokalen Mac die in `AGENTS.md` verlinkten aktuellen NexaCloud-/
NexaUI-Hinweise einschließlich Verfügbarkeit und Bereinigung lesen.
