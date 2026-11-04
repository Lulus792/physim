# Einstellungen: vollständiger Tastaturablauf bei langsamen Frames

## Befund und Grenze der Zuordnung

Der gehostete Intel-Lauf `41182399084` auf `7064a38c` beendet den
Einstellungen-Tastaturablauf nach Fortschritt bis Stufe 33, ohne einen lokalen
Feld-/Fokusprüffehler zu melden. Der damalige Hauptloop besitzt für sämtliche
Settings-Prüfungen eine globale Grenze von 20 Sekunden. Genau dieser Zweig
setzt den Fehler, ohne die Stufe weiterzuschalten. Die fehlende Zeitdiagnose
wird ergänzt. Der ursprüngliche Mac-Log bleibt unverändert; ein neuer Mac-Lauf
des korrigierten Quellstands steht bei Erstellung dieses Berichts noch aus.

Eine kontrollierte native Windows-Gegenprobe verwendet denselben Ablauf mit
250 ms zusätzlicher Frame-Pause. Sie schreitet bis Stufe 10 fort und läuft bei
**20,158 s** in die ausdrücklich gemeldete 20-s-Grenze, während der letzte Tab
von Fokus 13 zu 14 weitergegangen ist. Kein Feld-/Speichercheck meldet einen
Fehler. Das belegt die unpassende Gesamtgrenze für einen fortschreitenden Ablauf;
es ist kein neuer Nachweis eines Fokusfehlers auf dem Nutzer-Mac.

## Änderung und erhaltene Prüfungen

Nur die Testfamilie `--settings-test … keyboard*` erhält 60 s Gesamtzeit;
andere Settings-Modi behalten 20 s. Der Python-Prozesswächter hat 75 s,
die einzelnen Tab-Traversierungen weiterhin höchstens 28 Versuche. Keine
Produktinteraktion, Wartebedingung oder fachliche Assertion wird übersprungen.

`settings_keyboard_paced` nutzt denselben Prüfer wie `settings_keyboard_workflow`:
Kategorien/Felder, sieben Sichtbarkeitsänderungen, Änderungsprüfung, Übernehmen,
Schließen/Neustart, Entwurf verwerfen und Standardwerte ohne gespeicherte Änderung.
Python prüft weiterhin die genaue PSPREF05-Dateilänge und Werte an ihren Offsets
sowie den unveränderten Dateifingerprint nach Abbrechen. Die langsame Variante
muss Stufe 36 nach mehr als 20 s erreichen und ihren Pacing-Modus nachweisen.
Sie erzeugt keine zusätzliche PNG-Aufnahme. Die normale Fokusaufnahme bleibt.

CLI-Freigabeliste, native Registrierung und Mac-Workflow enthalten den neuen
Prüfmodus ausdrücklich. Neue Zeit-/Fokusdiagnosen erleichtern die künftige
Unterscheidung zwischen Timeout, Feldfehler und tatsächlichem Stillstand.

## Tatsächliche Ausführung

| Phase | Windows Release | Linux Debug mit ASan/UBSan |
| --- | --- | --- |
| Erster neuer Prüfmodus | 0/1, App lehnt noch fehlende CLI-Registrierung mit Exit 2 ab | derselbe Harnessfehler, 0/1 |
| Korrigierte Gegenprobe, C-Grenze 20 s | 0/1, `run-hutk9rc9`, tatsächlicher Timeout bei 20,158 s | nicht ausgeführt |
| Erster Abschlussaufruf | kein Test ausgewählt: falscher Kurzfilter `settings_keyboard` | derselbe Auswahlfehler |
| Exakte Filter, unveränderte fertige Binärdateien | **2/2**, `run-amponrq5`, vier App-Prozesse | **2/2**, `run-uexcwcq1`, vier App-Prozesse |

Die normalen Schreib-/Leseabläufe erreichen ihre Schlussprüfung unter Windows
nach 2,131/0,577 s, unter Linux nach 7,694/2,169 s. Die langsamen Varianten
erreichen dieselben Schlussprüfungen nach **27,988/5,846 s** bzw.
**34,929/7,711 s**. Beide finalen Binärdateien sind bytegleich mit den schon vor
der Filterkorrektur erzeugten Dateien; kein neuer Build wegen der Prüfauswahl.

Der bestehende Python-Prüfer `tests/test_native_test_runner.py` besteht separat
unter Windows, einschließlich Plattformkatalog, Fehlerbehandlung, Timeouts und
vollständigen Berichten. Alle vorherigen Registry-Einträge sind inhaltlich
unverändert; endgültig wird genau ein neuer 30-Zeilen-Eintrag ergänzt.

153 eigene Eingaben je isolierter Phase erfasst; alle Quell-/Runtime- und
referenzierten Archivbytes geprüft. `settings-keyboard-verified` ist eingefroren,
alle lokalen Handles sind beendet. [Kommandos, Hashes, Einzelergebnisse und
Zwischenfehler](settings-keyboard-2026-11-04.json). Die Nachweise unterscheiden
Harness-/Auswahlfehler ausdrücklich vom tatsächlich reproduzierten Timeout.

## Wiederholung und verbleibende Abnahme

Die normalen Prüfeinstiege sind `tools/with-artifacts.py -- python tools/build.py`
mit `--test-display --test-filter settings_keyboard_workflow --test-filter
settings_keyboard_paced` und einer zur Plattform passenden SDL-/Buildkonfiguration.
Die tatsächlich ausgeführten vollständigen Kommandos stehen im JSON-Bericht.
Die Namen sind exakte Filter, keine impliziten Teilstrings.

Gehostetes macOS muss den neuen Quellstand noch bestätigen; frühere bestandene
Mac-Fälle werden nicht als Abnahme dieses Patches ausgegeben. Manuelle
Screenreader-, physische Eingabe-, DPI-, Monitor- und Vollbildabnahme sowie die
gesamte native Gestaltung bleiben offen. Der Nutzer-Mac bleibt ausgeschaltet
und unkontaktiert. Alle neuen Ausgaben liegen auf D:, kein Zugriff auf F:.
