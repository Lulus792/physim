# Native Belege: Szenenbaum und linke Darstellung

Dieser Ordner gehört zum [begrenzten Baumabgleich](../SCENE-TREE-NATIVE.md).
Er enthält tatsächliche native Readbacks und ausgeführte Prüfungen. Die gesamte
Oberfläche ist damit nicht fertig. Maßgebliche CSS-/HTML-/Resizequellen werden
mit ihren Hashes separat festgehalten.

`source*.json.gz` hält die tatsächlich verwendeten Quell-/Fontgraphen fest.
Der lokale Graph enthält drei fremde AT-SPI-Dateien; zwölf fremde Arbeitsdateien
bleiben insgesamt unverändert und werden nicht übernommen. Deshalb sind die
lokalen Linux-Apps kein Clean-Checkout-Nachweis des späteren Commits.
Die öffentlichen Quell-Deltas der Vorstufen enthalten den vollständigen
UTF-8-/LF-Text der Änderungen gegenüber der angegebenen Basisrevision und
machen auch fehlgeschlagene Zwischenstände nachvollziehbar.

Stand 1 besteht 5/10 Linux-Gruppen. Der neue Fokusprüfer verwendet einen falschen
semantischen Gruppennamen; Kamera-/Klickproben setzen alte Abschnittszustände
beziehungsweise äußere Panelmaße voraus. Der zusätzliche Größenbefund ist ein
Produktfehler: Bei anfänglich kleinem Fenster wird vor Maximieren die falsche
Wiederherstellungsgröße gespeichert. `window-small-before.json.gz` enthält den
tatsächlichen Fehler und den erfolgreichen Gegenlauf mit einem älteren lokalen
Build. `window-normal-before.json.gz` zeigt den Unterschied ohne kleinen Start.
Die Apphashes, Befehle, Größenpaare, Ausgaben und Exitcodes sind darin enthalten.

Stand 2 synchronisiert die angeforderte Startgröße vor dem Maximieren. Der
gezielte Gegenlauf und die vollständige Einstellungsgruppe bestehen mit den
ursprünglichen Sollwerten und Zeitgrenzen. Kamera-/Anzeigegruppen bestehen mit
Schrift 16/22. Eine zu strenge zusätzliche obere Marge im Klickhelfer verhindert
aber die Bedienung des ersten Baum-Eintrags direkt am tatsächlichen Inhaltsclip.
Die fehlgeschlagenen Hierarchie-/Sichtbarkeitsläufe und das unveränderte Bild
bleiben als Ursachenbeleg erhalten. Die endgültigen Gruppen und Belegdateien
werden im zugehörigen Manifest getrennt zugeordnet.

Die Resizeprobe von Stand 2 bleibt unabhängig davon bei tatsächlichem Hostfokus
ohne Separatorfokus. `tree-source2-focus.log.gz` enthält den unveränderten
Debug-Build an dieser Stelle: Beide vertikalen Separatoren sind deaktiviert;
die beiden horizontalen Messlabor-Separatoren überlappen ihre Eingabeflächen.
Der GDB-Lauf liefert die Zustandsdaten, scheitert danach am nicht erkannten
booleschen Literal im Diagnosekommando und endet mit Exitcode 1. Er wird
ausdrücklich nicht als bestandener App-Test gewertet. Der Prozess und sein
Runner sind anschließend beendet. Stand 3 korrigiert die gegenseitige
Separatorverdeckung und ergänzt eine echte Menü-Sperr-/Escape-Gegenprobe.

Stand 3 besteht 9/10 Gruppen. Die Resizeassertionen bestehen jetzt, danach
scheitert F10 aus dem fokussierten nativen Trenner: Die allgemeine
Fokusnachbehandlung schließt das neue Menü wieder. Stand 4 ändert ausschließlich
die Übergabe in `toolbar_ui.inc` gegenüber Stand 3 und prüft die drei vorhandenen
Menügruppen sowie den neuen Baumablauf. Die bisherigen neun Regressionen und
die aktuellen gezielten Ergebnisse werden im Manifest getrennt ausgewiesen.

Der endgültige Stand 4 besteht die vier gezielten Gruppen mit GCC und
ASan/UBSan unter Alpine-Dev, Xvfb und Openbox. Der Baumprüfer besteht beide
Schriftpfade in 133,459 Sekunden: 72 Bilddatensätze und zwei Menüaufnahmen.
Die neun zuvor bestandenen Funktionsgruppen bleiben dem tatsächlich geprüften
Stand 3 zugeordnet. Zwischen 3 und 4 ändert sich ausschließlich die F10-Übergabe.
Der Windows-Release-Build von Stand 4 ist erfolgreich; dessen Baum-GUI wurde
unter Windows noch nicht ausgeführt. Der separate bestandene Windows-Nachlauf
für Inspektor-Stand 5 ist kein Ersatz dafür.

`validation.json` benennt Quellstand, Grenzen, Gruppen und ausgewählte Bilder;
`artifacts.json` pinnt die Belegdateien. `verify-negative.py` prüft die tatsächliche
alte Aufnahme ohne unmittelbar sichtbare Hierarchie/Grundfilter gegen die
neuen sichtbaren Glyphen und die ausgeführten Baumaktionen. Er vergibt keinen
vollständigen UI-Ähnlichkeitswert.

`before-scene-tree.png` ist die RGB-identische Umwandlung des alten nativen BMP;
`before-capture.json` nennt Quelle und Pixelnachweis. Die übrigen PNGs werden
unverändert aus dem nativen Readback übernommen. Keine Retusche, Skalierung
oder Browseraufnahme dient als native Abnahme.

Der neue Live-Prüfer ist `tests/check_tree_design.py` mit dem nativen
`tree-design`-Ablauf. Er prüft beide Schriftquellen, sechs Theme-/Schriftzustände,
vollständige sichtbare Beschriftungen, Tastatur-Reveal, getrennte Auswahl und
Sichtbarkeit sowie gemeinsamen linken/rechten Kamerazustand. Quellen,
pausierter Snapshot, Zeit und Messdatei müssen unverändert bleiben.
Native Eingaben ersetzen keine praktische Screenreader-Aufgabe und keinen
physischen DPI-/Monitorwechsel. Ein macOS-Lauf ist nicht vorhanden.
