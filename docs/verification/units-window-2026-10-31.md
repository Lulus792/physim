# Anzeigeeinheiten: eigenes natives Aufgabenfenster

31. Oktober 2026. **Geprüfter Teilstand, keine gesamte UI-/Plattformabnahme.**
[Umsetzung und Zustandsvertrag](../visuals/unified-workspace/UNIT-TASK-NATIVE.md),
[Kommandos, Hashes, eingefrorene Quellen/Runtimes und ursprüngliche Ergebnisse](units-window-2026-10-31.json).
Der Nutzer-Mac blieb ausgeschaltet und wurde nicht angesprochen. Frühere
Mac-Nachweise zu Einstellungen/Menüs gelten für ihre jeweiligen Quellstände,
nicht automatisch für diese neue Einheitenaufgabe.

## Tatsächlich bestanden

Fünf-Gruppen-Abschluss `units-window-focus`, **151 eigene erfasste Eingaben**. Windows Release
mit MSVC und Linux Debug mit GCC/ASan/UBSan, X11/Xvfb, Openbox und Software-GL.
GUI-Leakprüfung aus, Sanitizerfehler fatal. Fremde AT-SPI-/Orca-Arbeit ist nicht
in den isolierten Compilerstand übernommen. 215 Windows-/113 Linux-Runtimedateien
und alle referenzierten Archivbytes tatsächlich zurückgelesen.

| Prüfung auf dem gemeinsamen Fünf-Gruppen-Stand | Windows | Linux |
| --- | --- | --- |
| Fünf registrierte Gruppen | **5/5**, `run-tp2w0fqh` | **5/5**, `run-flt4us81` |
| Eigene Aufgabe: echte zwei Kanaldatei, UI 16/dunkel und 22/hell, Fehler/Entwürfe/Popup/OS-Schließen | bestanden, zwei App-Prozesse | bestanden, zwei App-Prozesse |
| Bestehender C-/Physim-Einheitenablauf, Neulesen, kaputter Katalog, bewusstes Zurücksetzen | bestanden, beide Projektsprachen | bestanden, beide Projektsprachen |
| Analyse: kompakte Auswahl plus eigener Bearbeitungseinstieg | dunkel und hell/groß/schmal bestanden | beide bestanden |
| Inspektor: tatsächlicher pausierter Pendellauf, andere Betrachtungszeit, Einheiten, sechs Darstellungen, echte Quellen-/Messdatei-CRC | beide Fontquellen bestanden | beide Fontquellen bestanden |

Die registrierten Gruppen sind `unit_task_window`, `channel_units_workflow`,
`analysis_units_dark`, `analysis_units_light_large_narrow` und `inspector_channel`.
Der unveränderte allgemeine Settings-A-Fensterablauf behält seinen vorherigen
bestandenen Nachweis; keinen neuen vollständigen Sechserlauf mit der letzten
Binärdatei behaupten. Auch die gesamte GUI-Suite ist damit nicht abgenommen.

Danach besteht `units-window-reference` je **1/1** (`run-klag3aoo` / `run-5n2u1huw`):
Die Anfangshöhe folgt jetzt der freigegebenen Referenz 850 × 473 bei UI 16 und
skaliert mit der Schrift. Der gezielte letzte Anschluss `units-window-caller`
prüft zusätzlich Mausaktivierung mitten im Settings-Renderer und Tastaturaufruf
zwischen Frames. Vor und nach dem Öffnen werden die tatsächliche Fenster-ID
und GL-Kontextidentität verglichen. Lauf-IDs, Ergebnisse und Hashes stehen im
JSON. Die fünf Gruppen wurden nach diesen beiden Korrekturen nicht erneut als
Gesamtgruppe ausgeführt; keine neue Gesamtmatrix mit der letzten Binärdatei behaupten.

Die neue Aufgabe wird über die tatsächliche Settings-Kategorie Anzeigeeinheiten
und deren sichtbare Maus-/Tastaturaktion geöffnet. Der Test prüft verschiedene
OS-Fenster-/UI-Identitäten und die ausdrückliche Zuordnung zum Einstellungsfenster.
Der Hauptworkspace wechselt bei geöffneter Aufgabe weiter. Tatsächliche
Textereignisse ändern Symbol und Faktor; Kanalwechsel erhält auch den anderen
Kanalentwurf. Allgemeines Abbrechen schließt nur Einstellungen. Ungültiger Faktor
null bleibt mit sichtbarem Fehler erhalten, Korrektur speichert bewusst.

SI-Wiederherstellen bleibt zunächst Entwurf. Schließen verwirft ihn, ohne eine
angewendete Einheit zurückzusetzen. Explizites Anwenden entfernt später den
Override. Ein echter Überlauf in einem Datensatzwert außerhalb des Vorschauwerts
wird abgelehnt; danach wird der ursprüngliche Testwert wiederhergestellt.
Ein real fehlgeschlagener Dateiersatz bewahrt Katalog und Entwurf, Wiederholung
funktioniert. OS-Schließen und erneutes Öffnen lesen den angewendeten Stand.
Popup-Pfeile wählen tatsächlich ein Menüelement; erneutes Öffnen fokussiert den
gewählten Entwurf. Tab und Escape verlassen zuerst die Liste, dann die Aufgabe.

Bei Analyse- und Inspektoreinstiegen bleibt der tatsächliche SDL-Tastaturfokus
über drei Hauptfenster-Renderzyklen im Aufgabenfenster. Eine alte unmittelbare
Menürückgabe ist für das Öffnen der Aufgabe entfernt. Beim Schließen wird nur
ein noch vorhandener sichtbarer Auftraggeber angesprochen, wenn die Aufgabe
selbst Fokus hatte; keine späteren Wiederholungsversuche gegen andere Arbeit.

## Kleine Fenster, Fonts und Bildumfang

Bei 22 px und 360 × 360 Clientgröße beträgt der Inhaltsausschnitt 79,5 logische
Pixel. Der reale Fehlerblock ist unter Windows 101,75, unter Linux 126,5 Pixel
hoch. Sein Anfang wird einmal gezeigt, der Scrollwert bleibt danach stabil:
Windows 746, Linux 790, obere Abweichung durch ganze Scrollpixel jeweils −0,75.
Kein ständiges Wechseln zwischen Anfang und Ende eines nicht vollständig in den
Ausschnitt passenden Blocks. Nach Vergrößern bleiben Korrektur und Aktionen nutzbar.
Diese Prüfung braucht kein weiteres Vollbild.

Inspektor je Fontquelle: 14 protokollierte native Zustände; Windows zwölf vollständig
sichtbare Beschriftungen/Eingaben und acht Werte, Linux 21/10. Die Pixelprüfung
kontrolliert Glyphen und die tatsächlich sichtbare äußere Kartenkontur. Ihre
Mindestzahlen zwölf/acht wurden nicht gesenkt. Ein angeschnittener dekorativer
Kartenrand zählt nicht als verlorene vollständig sichtbare Eingabe.

Pro Betriebssystem bleiben drei gezielte Einheiten-PNGs erhalten: breite dunkle
Ansicht, tatsächlicher Eingabefehler und schmale helle Ansicht mit großer Schrift.
Die übrigen Zustands-/Datei-/Fokusbelege stehen zuerst als Text/JSON vor.
Keine pixelidentische Gesamtoberfläche, physische DPI- oder Screenreader-Abnahme
aus diesen Bildern ableiten.

## Ursprüngliche Fehlläufe erhalten

`units-window-first` scheitert beim Build auf beiden Systemen: neuer Funktionsname
kollidiert mit vorhandenem `channel_unit_preset`-Typ. Umbenennung behebt diese
Ursache. Eine eventuell noch vorhandene Vorgänger-Binärdatei wurde nicht als
Ausführung des fehlgeschlagenen Quellstands gezählt.

`units-window-build`: Windows 4/6, Linux 5/6. Die neue Fenstergruppe besteht.
Der alte Lauf schickt F7 ans jetzt bewusst weiter bedienbare Hauptfenster und
startet damit zu Recht einen weiteren Messlauf. F7-Rauschen wird für die
Eingabetrennung jetzt an die Einheitentask geschickt; die Ein-Lauf-/Quellen- und
Rohdatenprüfungen bleiben erhalten. Separat scheitert die alte Windows-
Inspektorfixture bei großer Schrift am unmöglichen vollständigen Kartenscrollziel.
Die hashgeprüfte archivierte Vorgänger-App scheitert identisch in Stufe 21 nach
180,006 Sekunden. Der Fensterumbau verursacht diesen alten Befund nicht.

`units-window-controls`: 4/6 und 5/6. Ein vor einer wiederholten Tab-Wartephase
angefordertes PNG wird mehrfach geschrieben; der vorhandene exklusive PNG-
Schreiber lehnt das korrekt ab. Die neue Diagnosezeile beginnt außerdem wie ein
echter Feldrekord und irritiert den Windows-Bildprüfer. Beide Ursachen wurden in
den Fixtures korrigiert, ohne Dateiüberschreiben im Encoder zu erlauben.

`units-window-keys`: Windows 0/2, Linux 1/2. UI 16 mit echter Popup-Bedienung ist
fertig, UI 22 prüft aber die anfängliche Fehlersichtbarkeit erneut, während Tab
bereits bewusst zur Korrektureingabe scrollt. Eigene Fixture-Stufen trennen diese
Handlungen. Der Windows-Inspektorlauf erreicht alle 14 App-Zustände; der Prüfer
zählt noch vollständige Karten statt sichtbarer Beschriftung plus Eingabe.
Die tatsächlichen vorhandenen Raster liefern mit der korrigierten Bedingung
zwölf Felder/acht Werte. `units-window-proof` besteht danach je 2/2 einschließlich
beider Fontquellen. Erst der letzte Fünfernachlauf nimmt zusätzlich die danach
verbesserte Auftraggeberzuordnung und minimale Fehleransicht ab.

## Speicherpflege und Restabnahme

Nach Ende aller Build-/Test-/Archiv- und Vergleichsprozesse wurden **315 Dateien,
563.430.107 Bytes**, entfernt: 100 redundante erfolgreiche BMPs und 215 ausschließlich
zum Vorgängervergleich wiederhergestellte Runtime-Dateien. Jeder Zielpfad,
Dateiumfang und SHA-256 wurde vorab geprüft; sämtliche Vorgänger-Archivbytes vor
Löschung erneut zurückgelesen. Die Runtime bleibt aus dem Archivindex rekonstruierbar.
Fehlerbilder, PNG-Referenzen, kompakte Nachweise, Quellen, Archive, benötigte aktuelle
Programme/Symbole und Abhängigkeiten bleiben erhalten. Genauer Plan und Ergebnis:
`build/validation/units-window-2026-10-31/cleanup-{plan,result}.json`.

Neue tatsächliche macOS-Einheitenläufe stehen nach Commit/Push noch aus. Manuelle
Screenreader-, physische Eingabe-, Monitor-/DPI-/Vollbild- und gesamte native
Designabnahme bleiben offen. Completion-D-Preferences, die allgemeine historische
Settings-/AX-Fixture-Portierung und die übrige freigegebene Oberfläche bleiben
eigene Aufgaben; keine „volle Punktzahl“ aus dieser Teilprüfung ableiten.
