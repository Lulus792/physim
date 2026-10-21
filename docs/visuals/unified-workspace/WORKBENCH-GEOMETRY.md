# Werkbank: Referenzmaße und freie API-Aktion

Stand: 21. Oktober 2026. Fortsetzung des [gemeinsamen Rahmens](SHELL-NATIVE.md).
Die vollständige Designabnahme aller vier Arbeitsbereiche bleibt offen.

## Maße und persönliche Einstellungen

Neue Einstellungen verwenden 218 px Workspace-Breite und 255 px Kontextbreite.
Das Protokoll ist zunächst geöffnet. Der direkte Bildvergleich korrigiert auch
die Editorstatuszeile auf 24 px: Mit 42 px Protokollkopf und 86 px Inhalt beträgt
die gespeicherte Gesamthöhe 152 px. Die zunächst angesetzten 154 px übernahmen
noch die frühere native 26-px-Statuszeile und liegen zwei Pixel neben der Vorlage.

Die Workspace-Grenze erlaubt 140–480 px, die Kontextgrenze 180–480 px; die
Editorspalte behält mindestens 300 px. Das Protokoll erlaubt 60–300 px Inhalt,
also 126–366 px einschließlich seiner festen Kopf-/Statusflächen. Die aktuelle
Fensterhöhe begrenzt das Maximum zusätzlich, damit mindestens 280 px Editorraum
bleiben. Zehn-/Vierzig-Pixel-Schritte, Home/End, Enter, Doppelklick und
Pointerabbruch verwenden dieselben Grenzen und Standardwerte.

Bestehende persönliche Einstellungen werden nicht ersetzt. Alle zuvor gültigen
Größen bleiben gültig. Die vorhandenen Dateiformate bleiben erhalten; neue
Grenzwerte bestehen ebenfalls die Speicherprüfung. Die Formate 1–3 enthalten
keine Kontextbreite und behalten deshalb ausdrücklich ihren historischen
Fallback von 280 px. Der neue Standard 255 px wird nicht rückwirkend in diese
alten Formate hineininterpretiert.

## Linien, Suchschatten und Kontextkarte

Die Trennlinien unter Dateitabs, Protokollkopf und über dem Editorstatus sind
ein Pixel starke Flächen. Das ersetzt die weich gerasterten Strichbefehle.
Der Schatten der Suchinsel bleibt auf den Code-Viewport begrenzt. Er verändert
damit weder die Kontexttrennlinie noch Tabs oder Protokoll außerhalb dieses
Bereichs. Die eigentliche Glasdarstellung und ihr deckender Fallback bleiben
erhalten.

Die leere API-Karte misst Hinweis und Haupttext mit den tatsächlich verwendeten
Schriften und Zeilenumbrüchen. Ihre Mindesthöhe wächst bei Bedarf. Hinweis und
Aktion besitzen getrennte Flächen mit Abstand; bei großer Schrift überlagern
sie sich nicht mehr. Die echte gefüllte API-Referenz bleibt erhalten.

## Prüfstand

Der [eingefrorene Nachweis](workbench-geometry-2026-10-21/validation.json) enthält
35 eigene Quellhashes, die zehn unverändert vorgefundenen Dateien anderer lokaler
Accessibility-Arbeit, Programmhashes und tatsächliche Befehle. Die nativen
Anwendungen gehören zum internen Stand 40 auf Basis von `510963f`. Anschließend
ändert Stand 41 ausschließlich das Gesamtbudget der erweiterten Werkbankgruppe
von 120 auf 180 Sekunden: Der Linux-Lauf mit neun Programmstarts benötigte rund
139 Sekunden. Der Katalog lädt den neuen Wert auf Windows und Linux; die
45-Sekunden-Einzelgrenze und der native 60-Sekunden-Abbruch bleiben erhalten.

Windows und Alpine-Dev bestehen jeweils sieben Gruppen mit **38 tatsächlichen
nativen Programmausführungen**:

| Gruppe | Geprüfter Inhalt |
| --- | --- |
| Werkbank | Neun Läufe: Hell/Dunkel, Seitenzustände, Suche und Ersetzung ohne Editorverschiebung, Mindestfenster, große Schrift auch bei 1080 × 740, Kontrast, deckender Glasfallback sowie zwei 1440 × 1000-Referenzaufnahmen. Alle drei Trenner erhalten Grenzen, 10-/40-px-Schritte und Standardwerte; Workspace zusätzlich Pointerabbruch und Doppelklick. |
| Einstellungen | Speichern, Wiederöffnen, Reset, beschädigte Datei, maximiertes Fenster sowie vollständig per Tastatur erreichbare Einstellungen einschließlich Abbruch ohne Dateiänderung. Der Test verwendet den neuen Bearbeiten-Menüweg. |
| Layoutvorlagen | Echte C-/Physim-Projekte, laufende Simulation, Erhalt ungespeicherter Quelle und Darstellung, Speichern/Ersetzen/Anwenden/Wiederöffnen, beschädigte Datei und ausdrücklich ausgelöstes Zurücksetzen. |
| Simulieren | Startzustand, persönliche Aufteilung und Wiederöffnen, Hell/Dunkel/Kontrast, große Schrift und beide Projektsprachen; acht Seiten-/Kurvenkombinationen je Darstellungsfall. |
| Native Teilfenster | Alle sechs Bereiche als eigenständige Fenster außerhalb des Elternfensters, Messkurven, Zeitleiste, Erhalt der Quelle und Rückkehr an die festen Plätze. |
| Auswerten | Bestehende Layoutaufgabe in Hell mit großer Schrift am Mindestfenster. Kein neuer vollständiger Auswertungsabgleich. |

Zusätzlich bestehen je Plattform die beiden Einstellungs-/Layoutkatalogtests
mit Grenzwerten, älteren Formaten, CRC/Mutationen, atomarem Speichern und
Fehlererhalt. Ihre Quellen bleiben seit Stand 39 unverändert. Unter Linux
überschritt der Katalogtest auf dem gemounteten Windows-Dateisystem die
unveränderte 60-Sekunden-Grenze. Derselbe Testbinary besteht auf nativem Linux
unter `/tmp` in 2,16 Sekunden, der Einstellungstest in 0,44 Sekunden. Der
Timeout bleibt negativ dokumentiert und wird nicht als bestanden umgedeutet.

Vier direkte Bildfälle – Windows/Linux × Hell/Dunkel – bestehen jeweils
13 begrenzte Prüfungen mit `tests/check_unified_chrome.py --workbench`.
Bei 1440 × 1000 stimmen die vertikalen Regeln bei x=217/1185 und die
Protokoll-/Statusregeln bei y=820/861/948 mit den Original-PNGs überein.
Die unveränderten Referenzen bestehen die Selbstprobe. Die alte native Aufnahme
von Stand 37 verfehlt die beiden neuen Grenzprüfungen; sie bleibt als
[Gegenbild](workbench-geometry-2026-10-21/counterexample-boundaries-v37.png) erhalten.
Pillow läuft für diese Vergleiche unter Windows über die unveränderten Aufnahmen
beider Plattformen. Die native Linux-Bedienfolge benötigt dadurch kein Pillow.

| Aufnahme | Windows | Linux |
| --- | --- | --- |
| Hell | [Bild](workbench-geometry-2026-10-21/windows-light-v40.png) | [Bild](workbench-geometry-2026-10-21/linux-light-v40.png) |
| Dunkel | [Bild](workbench-geometry-2026-10-21/windows-dark-v40.png) | [Bild](workbench-geometry-2026-10-21/linux-dark-v40.png) |
| Große Schrift am Mindestfenster | [Bild](workbench-geometry-2026-10-21/windows-light-large-narrow-v40.png) | [Bild](workbench-geometry-2026-10-21/linux-light-large-narrow-v40.png) |
| Gezielt zurückgesetzte Grenzen | [Bild](workbench-geometry-2026-10-21/windows-reset-v40.png) | [Bild](workbench-geometry-2026-10-21/linux-reset-v40.png) |

Die PNGs sind verlustfreie Konvertierungen tatsächlicher BMP-Aufnahmen mit
identischen dekodierten RGB-Pixeln. Weder Referenzen noch Nachweise wurden
retuschiert. Die negativen Fokus-/Test-/Umgebungsversuche der Stände 35–39
bleiben im Manifest erfasst. Der neue Trennerablauf wartet nach dem Schließen
von Suche und Layoutdialog auf das wirklich veröffentlichte Fokusziel.

Die Builds enthalten die separat erfasste lokale Accessibility-Arbeit; ein
sauberer Checkout wurde nicht geprüft. Neue externe UIA-/AT-SPI-Clients,
manuelle Screenreader, physische Zeigerbedienung, gemischte Monitor-DPI und
macOS wurden in diesem Schritt nicht ausgeführt. Diese Funktions- und
Pixelteilprüfungen sind keine vollständige Abnahme der gesamten Oberfläche.

Nach Sicherung der finalen Bilder wurden drei überflüssige lokale
Review-PNG-Kopien entfernt: 575.719 Byte. Native BMPs, Originalreferenzen,
finale PNGs und negative Prüfnachweise bleiben erhalten; die Einzelpfade und
Hashes stehen im Manifest.

## Weiterer Referenzabgleich

Die gefüllte API-Karte, Gliederungsabstände, Modellnotiz und Protokollaktionen
müssen weiterhin unmittelbar mit gleichwertigen echten Inhalten verglichen
werden. Insbesondere zeigt das native Protokoll zusätzlich ein Schließenkreuz;
der Referenzkopf zeigt dort nur das Profil. Dies ist ein noch offener
Gestaltungsabgleich und keine belegte technische Unmöglichkeit. Quellen,
Projektname, Verzeichnisse und Buildstatus bleiben echte Daten des Testprojekts;
die illustrativen Pendeldaten aus dem Browser werden nicht ergänzt.

Nachfolgender Teilstand: [API-Karte und Protokollkopf](WORKBENCH-CONTEXT.md).
Er entfernt das hier für Stand 40 beschriebene zusätzliche Schließenkreuz,
gleicht die gefüllte Karte weiter an und prüft dokumentierte Einfügung,
vollständige Argumentzeilen bei großer Schrift und das separate Kontextfenster.
Die oben eingefrorenen Prüfungen und ihre begrenzte Aussage bleiben unverändert.
