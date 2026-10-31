# Einheitenfenster: Fokus nur im sichtbaren Bereich

31. Oktober 2026. Geprüfter Teilstand; keine gesamte UI-/Plattformabnahme.
[Kommandos, originale Ergebnisse, Quell-/Runtimearchive und Hashes](unit-focus-2026-10-31.json).

Die feste Fußleiste konnte Klicks an ein dahinter abgeschnittenes Eingabefeld
weitergeben. Die Windows-Gegenprobe `unit-focus-before`, `run-5ywcdn5y`, scheitert
in Stufe 53: Der freie Fußleistenpunkt 253,75/269,50 liegt über dem verdeckten
Symbolfeld. Der logische Fokus springt von „SI wiederherstellen“ (4) auf Symbol
(2), wodurch der folgende Tab die falsche Aktion erreicht. Linux wurde für
diesen Gegenfall nicht ausgeführt; keine dortige Gegenprobe behaupten.

`unit_focus` berücksichtigt jetzt den tatsächlichen sichtbaren Ausschnitt.
Die unveränderte Gegenprobe behält danach den SI-Fokus; Tab erreicht die
publizierte Schaltfläche „Schließen“, Enter schließt die Aufgabe. Bei 360 × 360 wird
je nach Schrift ein verdecktes Symbol- oder Kanalfeld ignoriert. Katalog,
Messwerte und Entwürfe bleiben erhalten. Das festgelegte Layout bleibt bestehen.
Die neue Fokusprüfung braucht keine zusätzliche Bildaufnahme.

## Tatsächlich ausgeführte Prüfungen

- `unit-focus-clipped`: Windows Release/MSVC **2/2**, `run-9i0uur2k`;
  Linux Debug/GCC/ASan/UBSan **2/2**, `run-ip6fpifj`, X11/Xvfb/Openbox/Software-GL.
  GUI-Leakprüfung aus, Sanitizerfehler fatal. `unit_task_window` prüft beide
  Schriftgrößen 16/dunkel und 22/hell samt altem Entwurfs-/Fehler-/Popup-/
  GL-Kontextablauf und dem neuen Fußleistenfall. `channel_units_workflow` umfasst
  acht tatsächliche App-Prozesse für C und Physim: Lauf, Neulesen, kaputten
  Katalog erhalten, ausdrücklich zurücksetzen sowie Quellen-/Rohdatenhashes.
- Anschließend `unit-focus-compact`: Nur der nachfolgend veränderte ältere
  Einheitenprüfer erneut ausgeführt, Windows **1/1**, `run-dqqgwoof`, Linux
  **1/1**, `run-4wghyt1d`. Je acht App-Prozesse; kein neuer gesamter Zweierlauf
  mit dieser letzten, nur in Testausgaben veränderten Binärdatei behaupten.

Die bisher nur auf Dateiexistenz geprüften Vollbilder des älteren Prüfers sind
durch Katalog-, Livewert- und Statistikzeilen ersetzt. Numerische Umrechnung,
Dateifehler, Quell- und Rohdatenintegrität bleiben geprüft; kein neues
`units-*.bmp` wird erzeugt. Der nur für die letzte ungenutzte Aufnahme bestimmte
Scrollschritt ist entfernt. Die gezielten Bilder des eigentlichen Einheiten-
fensters sowie die Rasterprüfungen für Analyse und Inspektor bleiben erhalten.

Nach Ende der Builds, Tests und Archivprüfung wurden genau **20 ersetzte
erfolgreiche Rohbilder, 51.229.960 Bytes**, entfernt. Alle absoluten D:-Ziele,
Vorfahren ohne Umleitung, Dateigrößen und SHA-256 wurden vor der ersten Löschung
geprüft. Die beiden alten Laufverzeichnisse und fünf erlaubten Dateinamen waren
ausdrücklich begrenzt; keine rekursive Bereinigung. Quellen, Laufdaten, Archive,
Ergebnisberichte, Fehlerbelege, PNG-Referenzen, Abhängigkeiten und VM bleiben erhalten.
Plan/Ergebnis: `build/validation/unit-focus-2026-10-31/cleanup-{plan-v2,result}.json`.

Der vorige gehostete Stand `b20d550e` besteht inzwischen auf Intel und Apple
Silicon je **15/15**, getrennte Menüregression je **7/7**. Alle vier Archive,
Digests und Ergebnisdateien sind gelesen. [Abgeschlossener Mac-Nachweis](settings-search-macos-2026-10-31.json).
Diese Ergebnisse belegen den vorherigen Such-/Inspektorstand; der neue
Einheiten-Fokusstand benötigt seinen eigenen tatsächlichen Mac-Nachlauf.

Nur D:, kein Zugriff auf F:. Der ausgeschaltete Nutzer-Mac wurde nicht
angesprochen. Native SDL-Ereignisse und publizierter Fokus ersetzen keine
physische Eingabe-/Screenreader-/DPI-/Monitor-/Vollbild- oder ganze Designabnahme.
Completion D mit tatsächlichen Preferences, Scope- und Parameterhilfe sowie
die übrige freigegebene UI bleiben offen; keine volle Punktzahl aus Teilprüfungen ableiten.
