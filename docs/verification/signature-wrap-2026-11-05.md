# Parameterhilfe: Umbruch und lange Signaturen

Stand: 5. November 2026, isolierter Quellstand `signature-wrap-final`.
Fortsetzung der [ersten Integration](signature-hud-2026-11-05.md) nach ADR023;
keine Gesamtabnahme der Oberfläche.

Die Signatur nutzt jetzt den vorhandenen Unicode-Wort-/Graphemumbruch.
Zusammengehörige Operatoren wie `->` und `...` bleiben auch beim Notumbruch
zusammen. Originaltext und Parameterbereiche bleiben unverändert. Ein gemeinsamer,
an Text, Breite, aktive Schrift und Parameter gebundener Zeilenplan dient zum
Messen, Zeichnen und Wiederanzeigen des aktiven Parameters. Bei Modelländerung
wird dieser Plan vor der Freigabe des geliehenen Textes verworfen.

Lange Signaturen erhalten einen begrenzten Codebereich mit schmaler Scrollleiste.
Der normale Parameterhinweis bleibt darunter fest sichtbar. Bei einem selbst
überhohen Hinweis bleibt als Rückfall der gesamte Inhalt scrollbar. Mausrad über
der Hilfe sowie Alt+BildAuf/BildAb/Pos1/Ende bewegen deren Inhalt. Die offene
Kandidaten-Kurzinfo behält ihre Tastenkürzelpriorität. Strg+Umschalt+Leertaste
zeigt den aktiven Parameter erneut; Editorinhalt und Cursor bleiben erhalten.
Die Vorschlagsliste berücksichtigt die belegte Fläche auch bei wenig Platz.

Ein leerer echter Nullparameteraufruf zeigt „Keine Parameter erwartet“.
Überzählige Argumente verwenden die gemeinsame Fehlerfarbe und behaupten keinen
gültigen aktiven Parameter. Die Escape-Unterdrückung berücksichtigt auch die Sprache.

## Ausgeführte Prüfungen

Windows Release/MSVC und Linux Debug/GCC mit ASan/UBSan bestehen jeweils die
neue Umbruchprüfung und drei vorhandene Text-/Codeumbruchregressionen (4/4).
Nach der Präzisierung des Gegenbeispiels besteht `signature_wrap` erneut je1/1;
865 Breiten-/Operator-/Unicode-/16-Parameterfälle, vollständige Quellbereiche,
aktive Schriftmetriken und fehlerhafte Anfragen. Der alte allgemeine Umbruch
zerlegt im tatsächlich ausgeführten Gegenbeispiel `aaaaaa->ReturnType` den Pfeil.
Bei Linux-Datentests ist die Leak-Erkennung eingeschaltet.

Die fünf vorhandenen Completion-/Kurzinfo-/Locals-/API-GUI-Gruppen bestehen auf
beiden Plattformen mit unveränderter Produktimplementierung. Der abschließend
korrigierte neue Parameterhilfe-Ablauf besteht gesondert je1/1. Das ist kein
behaupteter gemeinsamer finaler6/6-Lauf. Er bedient C und Physim jeweils in drei
Editorzielen und prüft anschließend bei tatsächlich800×540 Pixeln und22px Schrift
16 eigene Parameter, den letzten aktiven Parameter, festen Hinweis, Mausrad,
Alt+Pos1/Ende, manuelle Rückkehr, erhaltene Editorposition, Zeilenplan-Wiederverwendung,
disjunkte Liste sowie Nullparameter und Fehlerfarbe. Die abgesenkte Fenster-
Mindestgröße gilt ausschließlich für diesen Stresstest; die App behält1080×740.
GUI-Sanitizerläufe lassen wie bisher die Grafiktreiber-Leakprüfung ausgeschaltet.

Die Entwicklung korrigierte drei falsche Annahmen im neuen Test: Ganzzahl-Y-Werte
verfälschten den Vergleich fraktionaler Glyphenpositionen; SDL begrenzte den ersten
Kleinfensterwunsch auf die App-Mindestgröße; im schmalen Windows-Editor belegt der
C-Rückgabetyp eine eigene erste Zeile. Alt+Pos1 stand bereits korrekt bei null.
Diese Testkorrekturen werden nicht als Produktreparaturen ausgegeben. Fehlversuche
und Debuggerdiagnosen bleiben mit ihrem jeweiligen Quellstand erhalten.

Kommandos, Ergebnisdateien, Programm-/Quellhashes, geprüfte Bilder und Grenzen:
[strukturierter Nachweis](signature-wrap-2026-11-05.json).

## Mac und verbleibende Arbeit

Der [erste gehostete HUD-Mac-Nachlauf](signature-hud-macos-first-2026-11-05.json)
zum älteren Quellcommit `caa1faf8` ist beendet: ARM/Intel jeweils10/10 Daten,
aber8/9 GUI. Original-ZIPs, Digests und alle Einträge wurden gelesen. Intel
scheitert am HUD mit null Fläche und fehlendem Fokusnachweis; ARM erreicht den
C-Analyseeditor, scheitert dort an der vollständigen Darstellungskontrolle.
Dieser alte Lauf beweist die neue Umbruch-/Scrollimplementierung nicht. Die neue
CI nimmt gezielt vier HUD-Bilder je Architektur auf; neue Ergebnisse getrennt
verfolgen. Der private Nutzer-Mac bleibt ausgeschaltet und unangetastet.

Scrollleisten-Drag, herausgelöster Workspace, echte Fenster-/Fokuswechsel und
Accessibility-Verbraucher/Screenreader brauchen weitere native Aufgaben.
Physische DPI-/IME-Prüfungen, vollständige Performanceabnahme,190 Spezialverträge
und die übrige freigegebene Oberfläche bleiben offen. Bestehende Labels und
einzelne erfolgreiche Abläufe ersetzen diese Abnahme nicht.
