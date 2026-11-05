# Native Parameterhilfe: erster integrierter Stand

Stand: 5. November 2026, isolierter Quellstand `signature-hud-5`. Die zuvor
geprüfte Aufruflogik ist jetzt mit einer eigenständigen Anzeige im Editor verbunden.
Dies ist ein geprüfter Integrationsschritt, keine Gesamtabnahme von ADR023 oder
der gesamten Oberfläche. Die vollständigen offenen Aufgaben bleiben bestehen.

Die Hilfe zeigt die tatsächliche vollständige Signatur und den aktuellen Parameter
mit denselben Syntaxrollen wie der Editor, echter Semibold-Schrift und A2-Fläche
samt Unterkante. Sie öffnet automatisch und mit Strg+Umschalt+Leertaste. Vorschläge
stehen im Normalfall mit Abstand darunter; ihre Übernahme schließt die Aufrufhilfe
nicht. Escape schließt zunächst die Liste, danach die Hilfe für diesen Aufruf.
Manuelles Wiederöffnen bleibt möglich. Innere Aufrufe, überzählige Argumente,
IME-Zwischenzustände, Auswahl und veraltete Quellstände werden getrennt behandelt.
Ein unterdrückter Aufruf besitzt einen exakten eigenen Callee-Schlüssel.

Die Veröffentlichung prüft den tatsächlichen Editorinhalt, Besitzer, Sprache und
Cursor. Das ist auch dann nötig, wenn ein Speicherfehler das alte gültige Locals-
und Signaturpaar erhält. Der native Fehlerfall verändert die echte Eingabe und
verweigert eine Locals-Allokation: Die alte Anzeige bleibt verborgen, anschließend
erholt sie sich. Projekt-/Dateilebenszeiten räumen den neuen Besitz wieder auf.

## Tatsächlich ausgeführte Prüfungen

Windows Release/MSVC und Linux Debug/GCC mit ASan/UBSan bestehen jeweils sechs
native GUI-Gruppen: neue Parameterhilfe, bestehende Completion, automatische
Vorschläge, Kandidaten-Kurzinfo, lokale Variablen und API-Referenz.
Windows: `run-rg_b7qhf`; Linux/X11/Xvfb1600×1000: `run-46yp8mpv`.
Die GUI-Läufe verwenden wie bisher keine Leak-Erkennung für den Grafiktreiber;
daraus folgt keine vollständige Leak-Abnahme.

Der neue Ablauf bedient C und Physim jeweils im Modell-, Analyse- und Dateieditor:
Dunkel/12px Code, Hell/22px und hoher Kontrast. Er prüft tatsächliche SDL-Ereignisse,
Signaturglyphen mit aktiver Schrift, echte Clipflächen, Abstand, Escape-Reihenfolge,
Komma-Unterdrückung, manuelles Öffnen, Übernahme, innere/überzählige Argumente,
IME-/Auswahlpause sowie den Speicherfehler und seine Erholung. Das sind zwölf
Editorsequenzen über beide Plattformen, keine physischen Tastatur-/IME-Nachweise.
Je Plattform wurden zwei gezielte HUD-Bilder und die drei vorhandenen Bilder
mit gleichzeitigem HUD, Vorschlagsliste und Kurzinfo tatsächlich geöffnet.

Ein echter Randfehler blieb zunächst trotz verbreiterter Fläche auf Linux22px:
`double ps_vlength(ps_vec3 a)` zeigte nur27 von28 Bytes. Der unveränderte ELF im
Debugger lieferte bei gleicher Breite318,593964px am Ursprung28px Höhe und am
tatsächlichen Ort605,593994/220,5 plötzlich56px. Aufsummierte absolute X-Werte
rundeten die Schlussklammer auf eine ungemessene zweite Zeile. Die Korrektur
verwendet relative Stift-/Zeilenkoordinaten und identische Mess-/Zeichenwege.
Die Kontrolle mit exakt derselben alten Breite und Position liefert28/28px und
28 tatsächlich gezeichnete Bytes. Beide Debuggerprozesse wurden gezielt beendet;
die erfolgreichen vollständigen GUI-Läufe sind gesonderte Nachweise.

Die Entwicklung deckte außerdem Fehler im neuen Test auf: eine nicht deklarierte
C-Testfunktion, `strlen` auf einem nicht nullterminierten Editorpuffer und einen
falschen Fixture-Dateinamen. Diese Ursachen sind korrigiert und werden nicht als
Produktfehler umgedeutet. Originalstände und Diagnosen bleiben erhalten.
Befehle, Programm-/Quell-/Archivhashes, ursprüngliche Versuche und GDB-Paar stehen
einmal im [strukturierten Nachweis](signature-hud-2026-11-05.json).

## Nächster Schritt und Grenzen

Zuerst den Umbruch an Wort-/Tokengrenzen statt mitten im Rückgabepfeil verfeinern
und lange16-Parameter-Signaturen bei kleinen Fenstern samt Scrollen und aktivem
Parameter nativ prüfen. Der Exzesshinweis ist ehrlich, verwendet aber noch die
gedämpfte Textrolle statt des freigegebenen Fehlertons. Herausgelöster Workspace,
echte Fenster-/Fokuswechsel, native Accessibility-Verbraucher und Screenreader
benötigen eigene Aufgaben. Vorhandene Accessibility-Textlabels sind keine solche
Abnahme. Physische DPI-/IME-Aufgaben und vollständige Performanceprüfung bleiben offen.

Die noch ungeklärten190 Spezial-/Property-/Dokumentationseinträge, zusätzliche
Intrinsics, Funktionswerte, Import-/Aliasauflösung und ausgelassene Rückgabetypen
bleiben Aufgaben des Datenkerns. Die Regeln des [Modellnachweises](signature-model-2026-11-05.md)
gelten weiter. Sein abgeschlossener [gehosteter Mac-Nachlauf](signature-model-macos-2026-11-05.json)
enthielt noch keinen sichtbaren HUD. Neue HUD-Mac-Ergebnisse werden getrennt
nachgetragen; der private Nutzer-Mac ist ausgeschaltet und wurde nicht angesprochen.
