# Berichtsexport über das tatsächliche Aufgabenfenster prüfen

Die alten Auftriebs- und Federabläufe klickten eine nicht mehr gezeichnete
SVG-Schaltfläche an und erwarteten sofort eine Datei. Der freigegebene Export
führt inzwischen über **Grafik exportieren…** in ein eigenes OS-Fenster. Beide
alten Abläufe scheitern unter Windows und in den gesicherten glibc-CI-Läufen
präzise an Stufe 34. Die Oberfläche wurde für diese Korrektur nicht verändert.

Die nativen Testschritte öffnen jetzt das tatsächliche Exportfenster, wählen
SVG beziehungsweise PNG, erreichen die Speicheraktion, warten auf die echte
asynchrone Datei und schließen das Fenster mit OS-Fokus-Rückgabe. Reportobjekt,
Diagrammindex und Achsenausschnitt müssen erhalten bleiben. Nur die bereits
vorhandene Testauflösung des Dateiauswahldialogs wird verwendet; der Renderer
und der Dateischreiber werden tatsächlich ausgeführt.

Zwei anschließend sichtbar gewordene alte Prüfannahmen sind ebenfalls angepasst:

- Der Wechsel zur Werkbank stellt die zuletzt geöffnete Datei wieder her.
  Der Test prüft nun deren vorher festgehaltenen Schlüssel, die sichtbare
  Werkbank und das tatsächliche Suchziel statt pauschal den internen Tabwert 0.
- Rohdatenexport wird über **Messdaten** erreicht. Der Test scrollt den Inhalt
  am linken Rand und klickt die tatsächlich sichtbare CSV-Aktion. Das Diagramm
  übernimmt das Mausrad über seiner eigenen Fläche; deshalb wird dort nicht
  versucht, den Seiteninhalt zu scrollen. Eine begrenzte Suche liefert bei
  einem Fehler die tatsächlichen Button-, Paneel- und Diagrammmaße.

Windows Release und Alpine Linux mit X11, ASan und UBSan bestehen jeweils **5/5**:
Auftrieb, Feder, allgemeiner Plot-Ablauf, Pendel-Energie und Pendel mit Medium.
Die beiden Pendelgruppen prüfen C und Ph. Der unabhängige Vergleich von
CSV-Ausgaben mit den vollständigen ursprünglichen Laufreihen bleibt erhalten.
Insgesamt wurden **24 tatsächlich gespeicherte Grafiken** zusätzlich gelesen:
SVG als XML mit Beschriftung und Geometrie; PNG mit vollständigen Chunks,
Prüfsummen, Pixelstrom und den erwarteten 2400 × 1700 Pixeln.

Zwei repräsentative PNGs wurden geöffnet: Titel, Legende, Achsen, Einheiten und
Kurve sind vollständig lesbar. Die Systemschrift unterscheidet sich zwischen
Windows und Linux; eine plattformübergreifende Pixelgleichheit wird daraus
nicht abgeleitet. Es wurden keine zusätzlichen UI-Screenshotzustände eingeführt.

## Lokale Linux-Dateisystemvoraussetzung

Ein früher Linux-Nachlauf auf der Windows-Einbindung scheiterte bereits an der
vorhandenen Sicherheitsprüfung für atomisches Speichern. Die Kontrollprobe
liefert dort nach `chmod(0700)` weiterhin `0777`; im privaten ext4-Abbild sind es
korrekt `0700`. Symlinks funktionieren in beiden Kontrollen. Die Sicherheits-
assertions wurden erhalten und der Nachlauf in einer privaten Mount-Namespace
mit einem ausschließlich unter `D:/Physim/build` gespeicherten Testabbild
ausgeführt. Gemeinsame WSL-Mounts, Einstellungen und der VM-Datenträger blieben
unverändert.

Alle 758 Dateien des erfolgreichen POSIX-Nachlaufs wurden komprimiert und
vollständig zurückgelesen. Der frühere Fehlernachlauf mit 662 Dateien ist
ebenfalls archiviert. Danach wurden nur die zwei eigenen, nicht mehr benutzten
Testabbilder entfernt: [Bereinigungsnachweis](report-export-filesystem-cleanup-2026-11-05.json).
Gemessen wurden **191.393.792 zugeordnete Dateibytes**; die logischen Abbildgrößen
von zusammen 4 GiB sind eine andere Größe.

## Belege und Grenzen

Der [strukturierte Bericht](report-export-workflow-2026-11-05.json) bindet die
Originalbefehle, Zwischenfehler, Programme, Ergebnisdateien, Grafiken und
Dateisystemkontrollen an Prüfsummen. Alle sechs berührten Eingaben waren vor
dem Build ausdrücklich im 306-Dateien-Manifest enthalten und bytegleich zu
Projekt und Isolation. Sämtliche Zustände und Runtimearchive sind eingefroren.

Dieser Nachlauf bestätigt den beschriebenen Windows-/Alpine-Prüfumfang. Neue
glibc- und macOS-Exportnachweise, physische Eingabe, Screenreader und die
vollständige Designabnahme bleiben offen. Die übrigen bestätigten CI-Fehler
werden durch die erfolgreiche Exportprüfung nicht als behoben erklärt.
