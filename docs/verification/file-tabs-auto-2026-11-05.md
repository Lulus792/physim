# Dateireiterprüfung wartet auf automatische Vorschläge

Der Mac-Workspace-Lauf42068991869 zur Quelle8bf467ce scheiterte auf Intel und
ARM an derselben Dateireiterprüfung in Stufe27. Die beiden anderen Gruppen
bestanden; die anschließende68-Projekt-Matrix wurde übersprungen. Mit den bereits
geprüften Windows-/Linux-Binärdateien reproduziert sich derselbe Fehler jeweils
als0/1. Ein vorhandener grüner Nachweis anderer Editoraufgaben deckte diese
konkrete Dateireiterfolge nicht ab.

Die Probe prüfte die Vorschlagsliste einen Frame nach Verarbeitung der Texteingabe.
Die normale automatische Vervollständigung wartet jedoch140ms. Der Test wartet
jetzt höchstens drei Sekunden auf das tatsächliche automatische Öffnen. Danach
prüft er zusätzlich die exakten Quellbytes, den zugehörigen Editor, das Präfix
`ps_vl`, den automatischen Modus und die bestehende Position unter dem Cursor.
Er sendet weiter das echte SDL-Textereignis; kein manueller Completion-Befehl und
kein direkter Aufruf zum Öffnen ersetzen den Ablauf. Die App-Implementierung,
ihr140ms-Timer und das Design bleiben unverändert.

Die erste Beobachtung meldet eine noch wartende Anfrage für den richtigen Editor
und Cursor: Windows123,742ms und Linux106,568ms Restzeit. Anschließend wird die
wirklich geöffnete Liste bestätigt. Windows Release **3/3 (`run-iuqaftzp`)** und
Linux Debug ASan/UBSan **3/3 (`run-gd7xo029`)** bestehen: Dateireiterfolge,
automatische Vervollständigung und deren gesonderte Frist-/Verwerfungsprüfung.
Beide vorhandenen Ansichten der tatsächlichen Vorschlagsliste wurden geöffnet.
Für die Synchronisation entstanden keine zusätzlichen Bilder.

Vor dem Build war die geänderte vorhandene Testdatei ausdrücklich im303-Dateien-
Manifest enthalten und bytegleich zu Root und Isolation. Source-/Runtimearchive,
Kommandos, Programmhashes, ursprüngliche Fehler und Ergebnisse stehen im
[strukturierten Nachweis](file-tabs-auto-2026-11-05.json). Linux verwendet einen
eigenen Xvfb-/Openbox-Desktop. Die korrigierte Dateireiterfolge benötigt noch ihre
neue Mac-Abnahme; der Nutzer-Mac bleibt ausgeschaltet.

Dies korrigiert eine veraltete Testannahme und bestätigt keine vollständige
Gestaltungs-, physische Tastatur- oder Screenreader-Abnahme. Die offene Rückkehr
aus Suchfeldern und anderen Controls bleibt ein eigener nächster Prüfschritt.
