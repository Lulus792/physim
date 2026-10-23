# Native Kamera-Belege

Dieser Ordner gehört zum [begrenzten Kamera-Abgleich](../CAMERA-CONTROLS-NATIVE.md).
Er ist keine Fertigmeldung für die gesamte Oberfläche.

`source*.json.gz` hält die tatsächlich verwendeten Quell- und Fontgraphen fest.
Die öffentlichen Quell-Deltas enthalten vollständigen UTF-8-/LF-Text der
Änderungen gegenüber der angegebenen Basisrevision. Zwölf fremde Arbeitsdateien
bleiben separat erfasst und werden nicht übernommen. Drei davon gehören zum
lokalen Linux-Kompiliergraphen; die Apps sind deshalb kein Clean-Checkout-Beweis.

Stand 1 besteht die sieben vorhandenen Kamera-/Anzeige-/Baumgruppen und den
MSVC-Release-Build. Die Bildkontrolle findet zusätzlich den falschen linken
Zeilenursprung. Stand 2 korrigiert ihn und ergänzt den eigenständigen nativen
Kamera-Prüfweg. Ergebnisse, vollständige Prozessspuren, Quelldeltas und
strukturierte Geometrie werden den tatsächlich ausgeführten Ständen zugeordnet.

`before-captures.json` dokumentiert die beiden RGB-identischen BMP-zu-PNG-
Umwandlungen. `before-presets.png` stammt aus der tatsächlichen Kamera-Probe
von Baum-Stand 3 vor diesem Abgleich und zeigt die alten Preset-Buttons.
`source1-inset.png` stammt aus Kamera-Stand 1 und zeigt den falschen Einzug.
Die Versionen und Appzustände werden nicht als identisch ausgegeben. Die
zugehörige Kamera-Probe verwendet eine statische Szene; der neue Bildprüfer
arbeitet dagegen mit einem echten pausierten Pendel-Runner. Diese Bilder sind
begrenzte Gegenbelege, kein Vergleich identischer gesamter Fensterzustände.

Der neue Live-Prüfer ist `tests/check_camera_design.py`. Er verbindet echte
Eingaben und Kamerawerte mit nativen Textbefehlen, Geometrie und Bildpixeln.
System- und mitgelieferte Schriften, schmale/weite Spalten, Hell/Dunkel und
Hochkontrast gehören zum Prüfweg. Native Fokusereignisse ersetzen keine
praktische Screenreader-Aufgabe oder einen physischen Monitorwechsel.

Stand 2 endet mit 7/8 Gruppen: `camera_design` scheitert nativ in Stufe 20
an der fehlenden Ende-Behandlung. Die Datei mit `zoom-max` im Namen hält
diesen fehlgeschlagenen Versuch fest und zeigt tatsächlich noch 6 m.
`source2-controlled-zoom-max.png` ist davon getrennt: GDB setzt dort einmalig
12 m, damit die alte Griffgeometrie am rechten Ende sichtbar wird. Danach
scheitert die App erwartungsgemäß an Pos1. `controlled-range.json` und die
GDB-/Shell-/Logarchive grenzen diesen reinen Geometriebeleg ausdrücklich ab.

Stand 3 besteht erneut die sieben vorhandenen Regressionen. Die neue Probe
belegt jetzt echte Zoom-Endpunkte und Menü-Abbruch, scheitert anschließend
aber am Klick in die Orbitspur: Nuklear startet lediglich vom vorhandenen
Griff aus einen Drag. Die drei `source3-system-dark-*`-Aufnahmen gehören zu
diesem unvollständigen Lauf. Stand 4 ergänzt den HTML-üblichen Sprung auf
die Schrittposition des tatsächlichen Spur-Klicks. Die folgende Probe prüft
zusätzlich das Ziehen vom linken Orbitende. Der Abschluss führt die
Stand-3-Regressionen und den eigenen Stand-4-Lauf getrennt auf.

Stand 4 erreicht mit Systemschrift Dunkel, Hell, 18 und 20 px einschließlich
Spurklick und Griff-Drag. Danach endet er mit dem unveränderten 180-s-Limit
in der kombinierten Kopf-/Feld-Scrollprobe bei 22 px. Stand 5 betrachtet
den tatsächlichen Gesamthöhenbedarf: Passt die Kombination nicht in den
Viewport, verlangt er stattdessen das vollständig sichtbare Auswahlfeld.
Die Geometrie des übergroßen Kombinationsziels wird protokolliert.
`source4-clipped-focus.png` hält zusätzlich die noch abgeschnittene äußere
Fokuslinie fest. Stand 5 erweitert ausschließlich deren horizontalen
Zeichenclip in den vorhandenen Innenabstand, mit unveränderter vertikaler
Sichtgrenze. Beide negativen Pixelproben bleiben im eingefrorenen Prüfer.

Stand 5 besteht den Baumtest und den nativen Kameradurchlauf mit Systemschrift
in allen sechs Darstellungen. Der Python-Bildprüfer verwirft anschließend
die um 0,3627 px über den Ausschnitt ragende untere Reglergrenze nach dem
umgebrochenen freien Blickfeld. Stand 6 ergänzt dafür ausschließlich drei
Scrollaufrufe im Kamera-Prüfer. Die vollständige Sichtbarkeitsanforderung
bleibt erhalten. Gegenüber Stand 5 ist der Produktcode identisch; der aktuelle
Baumnachweis bleibt korrekt Stand 5 zugeordnet.

Stand 6 besteht anschließend alle sieben ausgeführten Kamera-/Speicher-/
Sichtbarkeitsgruppen, darunter 60 Kamerabilder pro Schriftquelle. Die weitere
Gestenprüfung findet den echten Layoutunterschied zwischen freier und
kanonischer Ansicht bei großer Schrift: Das Feld wechselt zwischen 73,8375
und 44 px Höhe. Die beiden `source6-variable-field-*`-Bilder und die
strukturierten Daten belegen diesen Höhenwechsel. Ein fortgesetzter alter
Zwei-Bewegungs-Drag wird damit nicht als ausgeführt behauptet. Nuklears
`nk_slider_behavior` bindet die Fortsetzung an die ursprüngliche
Klickposition innerhalb des aktuellen Griffs; ein bewegter Regler gefährdet
diese Bindung.

Stand 7 reserviert für alle Blickrichtungsbeschriftungen die erforderliche
Zeilenzahl bei der tatsächlichen Breite. Bei normaler Breite bleibt es bei
32 px. In der schmalen Spalte mit 22 px Schrift bleibt das höhere Feld auch
bei „Oben“ erhalten, damit der Wechsel zu „Eigene Ansicht“ den laufenden Drag
nicht verschiebt. Die Probe bewegt den Zeiger zweimal bei gleichbleibendem
ursprünglichem Y und verlangt den zweiten tatsächlichen Orbitwert. Eine
weitere Aktion prüft Spurklick und anschließenden Griff-Drag getrennt.

`cleanup-deferred.json` dokumentiert einen nicht ausgeführten
Bereinigungsversuch: 111.607.339 Byte ersetzte erfolgreiche Stand-1-Rohdaten
waren vorab gegen identische archivierte Ergebnisse und pixelidentische
Referenzdaten geprüft. Die Ausführung wurde vor Prozessstart automatisch
mit „blocked by policy“ abgelehnt. Der Rohordner bleibt erhalten; ein
alternativer Löschweg wurde nicht versucht.

Stand 7 besteht die sieben bisherigen Regressionen einschließlich Baum. Die
neue Zwei-Bewegungs-Probe liest zunächst in Stufe 56 noch vor dem Zeichenlauf
den alten Orbitwert; `workspace_test_frame` läuft vor `draw_ui`. Stand 8
ändert ausschließlich die Reihenfolge im Kamera-Prüfer: Nach jeder neu
eingespeisten Bewegung muss ein tatsächlicher Zeichenzyklus vergangen sein,
bevor der daraus entstandene Wert geprüft wird. Die Anforderungen an erste
und zweite Bewegung sowie an den weiteren Drag bleiben unverändert.
Der Produktcode von Stand 7 ist damit identisch; dessen sieben erfolgreichen
Regressionsgruppen bleiben diesem Stand zugeordnet.

Der Abschlussstand 8 besteht die Kameraprobe mit beiden Schriftquellen:
120 native Aufnahmen und tatsächliche Eingabefolgen. Sieben weitere
Regressionen sind bei identischem Produktcode Stand 7 zugeordnet. Der
Windows-Release-Build ist erfolgreich; eine aktuelle Windows-GUI-Abnahme ist
nicht enthalten. `validation.json` begrenzt diese Nachweise und lässt den
Gesamtauftrag offen. `check_frozen_camera.py` überprüft die Artefakthashes
sowie die Gegenproben für abgeschnittene Griffe, abgeschnittenen Fokus und
wechselnde Feldhöhe. Normale breite Felder müssen weiterhin 32 px hoch sein.

`references.json` verwendet für Textreferenzen die LF-normalisierte Git-Fassung
und für PNGs rohe Bytes. `reference-file-bytes.json` hält zusätzlich den
Rohhash der tatsächlich gelesenen Datei fest, einschließlich der lokal mit
CRLF vorliegenden Nuklear-Header. Die Indexprüfung muss gegen die passende
Git-Fassung erfolgen; Binärdaten werden nicht normalisiert.
