# Simulationsleiste: Originalmaße, Tempo und native Menübedienung

Stand: 23. Oktober 2026, auf Basis `cdec662a57149fb80eac34c8a821e4ad63dcc32a`.
Dieser Teilabgleich bearbeitet insbesondere die Tempoaufnahme aus den
[Nutzerbefunden](USER-REVIEW-2026-10-21.md) und die im
[vorherigen Mindesthöhenabgleich](WORKBENCH-TABS-NATIVE.md) sichtbaren großen
Bedienelemente. Die gesamte Oberfläche ist weiterhin nicht vollständig abgenommen.

Die Laufsteuerung übernimmt die ursprüngliche `simulation.css`: 77 px hohe
Leiste, 18 px Außenrand, natürliche einzeilige Beschriftungen mit 11 CSS-px,
32 px hohe Aktionen, 6 px Zwischenraum und 6 px Rundung bei Standardschrift.
Bei schmalen Fenstern gelten die ursprünglichen 31 px, 4 px Zwischenraum und
8 px horizontalen Innenabstand. Stop und Zurücksetzen trennt eine eigene
20 px hohe Linie. Die Projektspalte startet mit 190 px; Tempo hat eine getrennte
Beschriftung und ein mindestens 70 px breites Feld. Die Statuskapsel ist 26 px hoch.
Die festgelegte gemeinsame Farbpalette stammt weiterhin aus `bridge.js`.

Der generische umbrechende Buttonpfad erhöhte bisher bei „Zurücksetzen“ die
geteilte Zeilenhöhe. Dadurch wuchsen auch Tempoauswahl und deren Stock-Pfeil.
Die neue Leiste verwendet explizite Flächen und volle einzeilige Schriftbounds.
Das Tempo zeichnet den 8×8-px-Chevron mit 1,25 px Strich selbst; sein gewöhnliches
Nuklear-Menü und seine native Semantik bleiben erhalten. Beschriftungen behalten
`▶`, `Ⅱ`, `▸│`, `■` und `↺`; native Aktionsnamen enthalten die Wörter ohne Schmuck.
Dezimaltempo verwendet das deutsche Komma. Die bestehende zusätzliche 8×-Option
bleibt neben den übrigen Geschwindigkeiten erhalten.

Normale Aktionen verwenden die originale Oberfläche und Auswahlfläche bei Hover.
Der primäre Hover multipliziert die sRGB-Farbkanäle mit 1,12 entsprechend der
ursprünglichen CSS-Regel. Die kanalweise Auslegung folgt den
[W3C-Filterfunktionen](https://www.w3.org/TR/filter-effects-1/#FilterFunctions)
und der [brightness-Definition](https://www.w3.org/TR/filter-effects-1/#brightnessEquivalent).
Im vorhandenen Kontrasttheme bleiben dessen gelbe/weiße Zustände erhalten.
Fokus liegt als äußerer Ring um das bestehende Control; neue Auslagerungschrome
wurde dafür nicht eingeführt.

## Symbole und Offline-Schriften

Segoe UI und die bisher mitgelieferten Schriften decken nicht dieselben
Transportzeichen ab. Insbesondere fehlte in der ersten Windows-Probe `Ⅱ`.
Die vier regulären Hauptfenster-Schriftbanken ergänzen deshalb begrenzte
Symbolbereiche: unter Windows zuerst die installierte Segoe UI Symbol,
anschließend den eingecheckten Noto Sans Symbols aus dem festgehaltenen
Quellstand. Der erzwungene Offline-Modus benutzt ausschließlich die vorhandenen
mitgelieferten Schriften. Build und App laden nichts aus dem Netz.

Die neue TTF ist 258.156 Bytes groß. Herkunft, SHA-256 und bereits vorhandene
bytegleiche OFL-Lizenz stehen im [Fontreview](transport-2026-10-23/font-review.json)
und im zentralen Fontmanifest. Der vorläufige Pillow-Zeichenvergleich ist keine
native Abnahme: Der finale Prüfer verlangt tatsächlich sieben Glyphen und
sichtbare Rasterkonturen in beiden Schriftpfaden. F1-, Export-, Code- und
Diagrammschriftrollen behalten ihre bisherigen Bereiche und Fallback-Reihenfolgen.
Die normalen Hauptfenster-Schriftrollen erhalten die zusätzlichen Symbole.

## Gefundener Menüfehler

Die fertige native Veröffentlichung behandelte ein internes Kombomenü bisher
wie dessen normales Besitzerfenster. Später gezeichnete Workspace-Fenster wurden
so als Abdeckung seiner Einträge gewertet. Das Menü war sichtbar und tatsächlich
offen; nur die erste Zeile blieb nativ fokussierbar. Windows und Linux liefen
mit unveränderten 180 Sekunden am Tastaturwechsel zu „1× Echtzeit“ in Stufe 28 aus.

Drei isolierte GDB-Proben trennen Öffnen, Aufnahme und Veröffentlichung: Popup
offen; „1× Echtzeit“ bei Aufnahme enabled; anschließend sieben von acht Zeilen
im veröffentlichten Modell disabled. `nk_build` hängt sämtliche Popups erst nach
den normalen Fenstern an. Der SDL-Adapter folgt jetzt dieser wirklichen
Zeichenreihenfolge. Abdeckung durch spätere gewöhnliche Fenster und spätere
Popups wird weiterhin geprüft. Die generische Abdeckungsprüfung wurde nicht
entfernt. Nach Auswahl und Escape kehrt der tatsächliche Fokus ins Tempofeld
zurück; der optionale Fokusgruppenparameter bezeichnet dabei eine semantische
Gruppe und keinen Fensternamen.

Die GDB-Läufe enden absichtlich durch den Debugger; Leakprüfung war nur dort
unter ptrace abgeschaltet. Sie sind Ursachenbelege, keine bestandenen
Sanitizer-App-Prüfungen. Die regulären Linux-Nachweise verwenden ASan/UBSan. Die vorhandene native
Windows-UI-Prüfung weist echte überdeckte, deaktivierte und entfernte Controls
weiter ab (1/1). Der gemeinsame Textfluss-Rücklauf besteht auf beiden Plattformen
(je 1/1).

## Tatsächlich ausgeführte Prüfungen

Die [Validierung](transport-2026-10-23/validation.json) hält auch die fehlgeschlagenen
Vorstufen, Quellgraphen, Binärhashes und Grenzen fest.

| Umgebung | Aktueller Nachweis |
| --- | --- |
| Windows MSVC Release, Quelle 4 | 5/6 Gruppen: Transport, Tempo, Zurücksetzen, F1-Formeln und Grafikdialog bestanden; separater bestehender F1-Lifecycle-Test scheitert vor seinen eigentlichen Aufgaben. |
| Alpine-Dev GCC Debug mit ASan/UBSan, Quelle 4 | 4/4 Gruppen: Transport, Tempo, Zurücksetzen, Grafikdialog bestanden. |
| Derselbe Linux-Build, F1-Arbeitsdaten auf ext4 | F1-Formeln bestanden in 30,94 s bei ursprünglichen 45 s App-/75 s Außengrenzen. Nur Arbeitsdaten liegen auf ext4, App und Repository bleiben auf `/mnt/d`. |

Transport startet pro Plattform zwei echte App-Prozesse: Systemschriften und
mitgelieferte Schriften. Jeder bedient sechs Zustände: Dunkel/16, Hell/16,
Dunkel/18, Hell/20, Hell/22, Kontrast/22; große Zustände im 1080×740-Client.
Fortsetzen/Pause per echter Enter-Taste, Einzelschritt um genau `dt`, Offline per
Maus, 1× per Tastatur, Escape und automatische Fokusrückkehr bestehen in allen
24 Varianten-/Schrift-/Plattformkombinationen. Gespeicherter Quelltext bleibt
bytegleich zum echten Pendelbeispiel; der Ablauf erzeugt genau einen Messlauf.
Tempo-/Reset-Rückläufe prüfen zusätzlich C/Phys-Projekte und Fehler-/Blockadefälle.

Unabhängige PNG-Auswertung prüft volle Beschriftungen, sichtbare Symbolkonturen,
Chevrongröße, normale Primärfarbe, ursprünglichen Hover und äußeren Fokus.
Die erste Windows-Bereit-Aktionsfläche liegt direkt gegen das unveränderte
Original-PNG innerhalb eines Pixels: native Fläche `(232,141)..(320,171)`,
Original `(232,140)..(321,171)`. Das ist ein begrenzter Rechteckvergleich und
keine vollständige Bildgleichheit. Quelle 3 bestand alle nativen Windows-Aufgaben,
aber der Prüfer verlangte zwei fast deckende Pixel im kleinen Reset-Pfeil.
Er hat bei Segoe nur einen solchen und sieben bis acht über halber Deckung.
Quelle 4 korrigiert ausschließlich diese Antialias-Annahme; die native App ist
identisch. Eine blanke Vergleichsfläche wird weiterhin abgewiesen.

Die [Gegenproben](transport-2026-10-23/verify-negative.py) spielen echte negative
Menüspuren und die korrigierte sichtbare Glyphenprüfung gegen die eingefrorenen
Dateien durch. Die [16 kanonischen PNG-/SVG-Exporte](transport-2026-10-23/exports.json)
sind gegenüber den vorherigen plattformspezifischen Belegen bytegleich.

## Getrennter alter F1-Lifecycle-Befund

Der zusätzliche unabhängige Windows-UIA-Test fragt schon beim Start nach
„Aktuelles Dokument durchsuchen“, öffnet aber die H-Suchleiste vorher nicht.
Die [diagnostische Query](transport-2026-10-23/lifecycle-existing-failure.json)
findet das Feld weder im aktuellen Release noch im erhaltenen älteren Debug-Build:
`role=50004 found=0 hr=S_OK`. Beide scheitern vor Hide/Reopen/Minimize.
Das H-Design startet absichtlich mit geschlossener Suchleiste; vorhandene
H-Bedienprüfungen öffnen sie ausdrücklich. Der alte Remote-Lifecycle-Ablauf muss
an diesen Vertrag angepasst werden. Er bleibt offen und wird nicht durch eine
dauerhaft sichtbare Produktsuchleiste oder eine erfolgreiche Formelprobe ersetzt.

## Verbleibende Unterschiede

Plattformschriften besitzen andere Umrisse und Laufweiten. Die Offline-Schrift
ist Noto Sans; sie kann deshalb andere natürliche Buttonbreiten als Segoe liefern.
Bei schmalem Fenster und großer Schrift verwendet die Projektspalte erforderlichenfalls
den verbleibenden Platz bis mindestens `96 × Schriftfaktor` px. Vollständige
Aktionsnamen bleiben erhalten. Andere Glyphenrasterung erklärt einzelne Pixel,
aber keine anderen Abstände oder Kontrollformen.

Szenenleiste, Szenenkopf, Objektfelder, Kurvenzustände und die komplette Zeitleiste
weichen weiterhin sichtbar ab. Das sind korrigierbare offene Arbeiten. Andere
Projekt-/Messnamen und zwei statt drei echte Messkanäle sind Datenunterschiede;
sie rechtfertigen keine Gestaltungsausnahmen. Eine vollständige Designabnahme,
macOS, physische DPI-/Mehrmonitorbedienung und externe Screenreader-Aufgaben
werden aus diesen Teilprüfungen nicht abgeleitet. Ein neuer vollständiger SDK-
Installationslauf wurde hier nicht ausgeführt; der vorherige SDK4 ist nicht dieser Build.

Die getesteten Binärdateien enthalten zwölf unveränderte vorbestehende lokale
Accessibility-/Dokumentationsdateien. Deren Rohhashes sind eingefroren, ihre
Änderungen sind vom Transport-Commit ausgenommen. Die Quellgraphen unterscheiden
normalisierte Textdateien und rohe Fontdateien.

## Folgearbeit: Zeitleiste

Der [anschließende Zeitleistenabgleich](SIMULATION-TIMELINE-NATIVE.md) verwendet
dieselben vorhandenen Schriftbanken und Symbolbereiche. Er ergänzt die
ursprüngliche Zeitanzeige und Skala sowie denselben Inhalt im nativen Zusatzfenster.
Die hier eingefrorenen historischen Transportnachweise bleiben unverändert.
