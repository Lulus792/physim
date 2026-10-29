# Gemeinsame Farben für das Protokoll

Nutzerauftrag vom 29. Oktober 2026: Der Protokollbereich soll dieselben Farben
wie die übrige Physim-Oberfläche erhalten. Das konkretisiert die frühere Auswahl
von Navigator C; Anordnung und Bedienung bleiben erhalten.

`protocol_panel` verwendet jetzt `ps_workbench_palette` anstelle einer eigenen,
bläulicheren Palette. Panel, Felder, Details, Konturen, Auswahl und Text verwenden
dieselben semantischen Rollen wie die Werkbank. Das gilt auch im herausgezogenen
Fenster und beim Themewechsel. Fehler-/Warnhinweise behalten zusätzlich Text,
Symbol und ihre unterscheidbare Statusfarbe. Hoher Kontrast bleibt eine eigene
gemeinsame Variante.

Der [kompakte Nachweis](../../verification/explorer-ui-2026-10-29.json) enthält
Programm-/Quellhashes, Kommandos, tatsächliche Pixelwerte und die Originalberichte.
An je drei nativen Windows- und Linux-Aufnahmen stimmen die Protokollflächen
exakt mit den benachbarten Werkbankflächen überein:

| Variante | Workspace und Protokoll | Editor und Protokolldetails |
| --- | --- | --- |
| Dunkel | `#1e1f23` | `#191a1e` |
| Hell | `#f6f7f9` | `#ffffff` |
| Hoher Kontrast | `#000000` | `#000000` |

Die vorhandenen acht Navigatorabläufe einschließlich Herausziehen/Zurückholen
und sechs Typografiezustände bestehen auf beiden Plattformen mit dieser Version.
Windows/Dunkel und Linux/Hell wurden zusätzlich visuell angesehen. Neue
versionierte Bilder oder Desktop-Vollbilder waren nicht nötig.

Die gemeinsame erweiterte Prüfrunde besteht unter Linux mit 5/5 Gruppen. Unter
Windows bestehen 4/5: Der erste UIA-Clientaufruf für ein Explorer-Pfadfeld lief in
einen Timeout. Zwei direkte Wiederholungen mit identischem Programm und Probe,
darunter ein längerer Unicode-Pfad, sowie die Wiederholung über denselben
Testrunner mit dessen VS-Umgebung bestehen. Die ursprüngliche Runde bleibt
als fehlgeschlagen dokumentiert; ihre Timeoutursache ist nicht belegt. Dieser
Befund wird nicht als durch die Farbänderung behoben ausgegeben.

Die Farbenangleichung ersetzt weder die vollständige C-/F1-Gestaltungsabnahme
noch Screenreader-, DPI-, Monitor- oder macOS-Prüfungen. Der separate
[physische Windows-Tastennachweis](PROTOCOL-PHYSICAL-KEYS-NATIVE.md) bezieht sich
auf das Programm vor dieser reinen Farbänderung.
