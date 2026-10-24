# Native Laufgrenzen, Ereignisse und Aufzeichnung

Fortsetzung der [B-Wertefelder](RUN-FIELDS-NATIVE.md). Maßgeblich sind die
gewählten [B-Karten](../simulation-designs/MODEL-EDITING.md), die tatsächliche
Kaskade aus `inspector.css` / `inspector-variants.css` und die gemeinsame
Farbpalette aus `workspace.css` / `bridge.js`.

## Umsetzung

Laufgrenzen verwenden denselben nativen Wertefeldbaustein wie die Laufparameter:
kleine Beschriftung mit Einheit, 19-px-Zahl, 35-px-Eingabe und 8-px-Rundung.
Speicher bleibt eine ganze Zahl von 0 bis 16384 MiB, Zeit eine endliche Zahl
von 0 bis 86400 s. Null bedeutet weiterhin ohne Grenze; die Zeit zählt Pausen
mit. Komma und Punkt sind zulässig. Ungültiger Text bleibt korrigierbar, statt
still durch den letzten gültigen Wert ersetzt zu werden. Build, Simulation und
Auswertung starten damit nicht. Während eines Prozesses bleiben die Felder
gesperrt. Derselbe Editor wird auch in den weiteren nativen Inspektoransichten
verwendet, damit Text und Validierung beim Ansichtswechsel zusammenbleiben.

Die Ereignisse folgen B mit 7-px-Rundung, 10-px-Innenabstand, 10-px-Metadaten,
12-px-Meldung und 7-px-Abstand zwischen den Einträgen. Sie nutzen den gemeinsamen
Scrollbereich. Das alte kleine Innenfenster und die doppelt/tripelt wiederholte
Warnung entfallen. Harte Zeilenumbrüche, UTF-8 und Tabulatorinhalt bleiben
lesbar. Die native Reihenfolge der neuesten Meldungen zuerst bleibt erhalten;
die Protokolldatei behält auch die älteren, nicht mehr in der Vorschau enthaltenen
Meldungen. „Protokoll öffnen“ bleibt ein tatsächlicher Tastatur-/Mausknopf.

Eine Warnung bleibt außerhalb der geschlossenen Gruppe erhalten. Ist die
Gruppe offen und enthält sie dieselbe Warnung bereits, wird sie nicht nochmals
darüber gezeichnet. Wurde sie aus der begrenzten Vorschau verdrängt, bleibt der
separate Hinweis erhalten. Die Statuszeile nennt ausdrücklich „Warnung“ oder
„Fehler“, auch wenn die betreffende Karte weggescrollt wurde.

Aufzeichnung zeigt den tatsächlichen Dateinamen mit 6-px-Statuspunkt, 9-px-Abstand
und kleiner Beschreibung. Der vollständige Pfad ist per Tooltip verfügbar.
Die Angabe zur Szene folgt weiterhin dem tatsächlichen Aufzeichnungszustand.
Ohne Aufzeichnung erscheint ein eigener Leerzustand ohne Statuspunkt.
Dateinamen und Ereignisse stammen aus dem Programmzustand; die Beispieltexte
der HTML-Vorlage werden nicht in die Oberfläche eingesetzt.

## Prüfumfang und Grenzen

Die native Fixture verwendet ein ausdrücklich nichtphysikalisches Logging-Modell
mit vorgeschriebenem Skalar, echten Runner-Prozessen, mehrzeiliger Unicode-Meldung
und tatsächlich geschriebenen `.psrun`-/Protokolldateien. Ihre zwei Messpunkte
werden unabhängig aus der Laufdatei gelesen. Quelltext und geschlossene Laufdatei
bleiben trotz Darstellungswechseln und Grenzkorrekturen unverändert.

Die UI-Prüfung umfasst sechs Darstellungen, beide Schriftquellen, echte Leerzustände,
gesperrte Felder, ungültige Eingaben, Komma, blockierte Prozessstarts, Tastaturzugriff
zum Protokoll und Korrektur über einen Ansichtswechsel hinweg. Interne SDL-Ereignisse
und native OpenGL-Aufnahmen belegen die automatisierten Abläufe. Praktische
Screenreader-Aufgaben, macOS, physische Monitor-/DPI-Wechsel und vollständige
Browser-Pixelgleichheit bleiben gesondert offen.

Die [eingefrorenen Belege](inspector-tail-2026-10-24/README.md) enthalten die
bestandenen Windows-/Linux-Prüfungen mit 128 Aufnahmen, den verworfenen ersten
Bildleser sowie den ursprünglichen Gegenfall. Die Herkunft der ergänzenden
Logging-, Projekteinstellungs- und Runnerprüfungen ist getrennt angegeben.

Die Messkanal-/Einheitenansicht sowie der vollständige Abgleich von Entwickeln,
Auswerten und F1 bleiben Teil des fortgesetzten Gesamtauftrags.

## Speicherpflege

Nach dem letzten erfolgreichen Nachlauf wurden ausschließlich 240 ersetzte
PNG-Rohbilder aus vier erfolgreichen Zwischenläufen entfernt: 30.888.960 Bytes.
Die [Bereinigungsaufzeichnung](inspector-tail-cleanup-2026-10-24.json.gz) erfasst
Pfade, Größen und Hashes. Fehlgeschlagene Nachweise, ursprüngliche Gegenbilder,
alle 128 letzten Aufnahmen, Laufdaten und SDKs bleiben erhalten.
