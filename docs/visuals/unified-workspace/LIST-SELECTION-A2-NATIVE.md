# A2-Markierung für Protokoll und Explorer

Stand: 30. Oktober 2026. Der native Renderer übernimmt die
[freigegebene Markierung A2](../selection-options/APPROVED.md) für Meldungszeilen,
Explorer-Dateizeilen und Explorer-Kacheln. Die vorhandene Einzelauswahl bleibt
die Grundlage dieses Schritts. Mehrfachauswahl und Kontextmenüs sind weitere
verbindliche Teile des Gesamtauftrags, keine durch diesen Nachweis erledigten
Funktionen.

## Gestaltung

Die gemeinsame Auswahlfläche verwendet dieselben A2-Werte wie die Textauswahl:
Dunkel `rgba(170,184,202,.15)`, Hell `rgba(90,108,130,.11)`. Die 3-px-Kante und
Häkchen erhalten die eigene Akzentrolle `#abb9ca` / `#65788d`. Hoher Kontrast
verwendet die gemeinsamen deckenden Auswahl- und gelben Konturrollen.
Normale Text-, Metadaten-, Datei- und Statusfarben bleiben erhalten.

Der Renderer komponiert die Fläche über den vorhandenen Zeilen-/Kartenhintergrund
und zeichnet die Originalinhalte darüber. In Datei- und Meldungszeilen erhält
eine eigene 20-px-Auswahlspalte mit 9 px Abstand zusätzlich die bereits
freigegebenen Datei- und Fehler-/Warnsymbole. Breitenmessung und Umbruch
berücksichtigen diese Spalte. Kacheln tragen den Indikator links oben.
Die Layoutprüfung berechnet den zusätzlichen Platz aus der A2-CSS-Referenz;
sie übernimmt ihn nicht aus dem nativen Quellcode.

Tastaturfokus erhält eine getrennte gestrichelte 2-px-Markierung in der gemeinsamen
Fokusfarbe. Die Auswahlfläche und der linke Akzent bleiben davon unabhängig.
Die lokale Explorer-Akzentfarbe darf die gemeinsame Fokusrolle nicht ersetzen.
Auch die Kästchenumrandung erhält die gemeinsame A2-Farbrolle statt einer
lokal überschriebenen Metadatenfarbe.
Navigation, Schalter und die übrigen Hervorhebungen behalten ihre eigenen Rollen.

## Nachweise

Die [kompakte Prüfung](../../verification/list-selection-a2-2026-10-30.json)
nennt tatsächliche Programme, Quellstände, Befehle, Ergebnisse und Grenzen.
Sie prüft native Bilder und Zeichenbefehle für Protokoll, Dateiliste und Kacheln
in Hell, Dunkel, hohem Kontrast und großer Schrift. Echte eingereihte
SDL-Mausereignisse ändern die Auswahl. Eingereihte Tab-Tasten führen im Explorer
zum Fokusziel; das ist kein Nachweis physischer Windows-Eingabe.

Vor/nach der Auswahl müssen Text, Farben und Geometrie identisch bleiben.
Die Pixelprüfung kontrolliert die A2-Komposition, die linke Kante, das Häkchen
und unveränderte Schrift-/Warnfarben einschließlich Kantenglättung. Beim
zusätzlichen Tastaturdurchlauf darf der vorhandene Außenviewport scrollen;
relative Textgeometrie, Originalfarben und Größe bleiben geprüft. Die tatsächliche
Verschiebung wird erfasst. Fokusfarbe und Strichstärke werden an Zeichenbefehlen,
sichtbare Striche und Lücken getrennt am Rasterbild geprüft.

Die zusätzliche Explorer-Layoutprüfung behält 320 × 1200 für große schmale
Ansichten. Sie bringt den langen Dateinamen vor der vollständigen Glyphenprüfung
über die echte Tastaturführung in den sichtbaren Bereich. Die native
Fokus-/Scrollführung und der vollständige Namensvergleich bleiben erhalten.

Die normalen Windows-/Linux-Läufe bestehen jeweils drei Gruppen: zwölf
Listenzustände, sechs Explorer-Layouts und zwölf Textauswahlzustände.
Sechs separat erzeugte native Fehlerbilder (Deckkraft, fehlende Kante,
fehlendes Häkchen, geänderte Status-/Textfarbe und durchgehender Fokus)
müssen an derselben Prüfung scheitern. Das unveränderte normale Programm
besteht danach dieselben zwölf Listenfälle; seine EXE-/PDB-Bytes bleiben geprüft.

## Verbleibender Umfang

Dieser Schritt schließt A2 nicht insgesamt ab. Workspace-Dateibaum und Szenenbaum
folgen im [separaten nativen Baumabgleich](TREE-SELECTION-A2-NATIVE.md).
Szenenobjekte und weitere auswählbare Inhalte sind noch abzugleichen. Ebenso
offen bleiben Strg/⌘-/Umschalt-Mehrfachauswahl, Auswahlrahmen, die neun nativen
Kontextmenüziele aus ADR 021, Auswahl bei Menüöffnung und sichere Zielbindung.
Die bestehenden Aufgabenfenster, B2, Einstellungen, Rückmeldungen, Fortschritt
und Grafikexport bleiben verbindlich. Die separat beauftragte automatische
Vorschlagsanzeige und Vervollständigung werden durch diesen Auswahlnachweis
nicht erledigt.
Vollständige F1-/Oberflächenabnahme, physische Eingabe, echte Screenreader,
Monitor/DPI und aktuelle macOS-Ausführung
werden durch diese Teilprüfung nicht ersetzt.
