# F1 H: unabhängige Eingabeprüfungen, 26. Oktober 2026

Die unabhängigen Suchfeldprüfungen bestehen wieder: Windows-UIA prüft beide
Suchfelder und den Textscrollzugriff; Linux-AT-SPI prüft Werte, Unicode-Auswahl,
Textersetzung, native Fokuszustände und die Tastaturrückkehr. Der Produktcode
und das ausgewählte H-Design wurden dafür nicht verändert.

Die [kompakte Ergebnisliste](../../verification/f1-edit-h-2026-10-26.json)
enthält genaue Kommandos, Exitcodes, Quell-/Programmhashes und Verweise auf
die vollständigen lokalen Zustandsberichte. Frühere Fehlversuche im
[Originaltext-Nachlauf](../unified-workspace/PROTOCOL-ORIGINAL-NATIVE.md)
bleiben unverändert erhalten.

## Ursache und Korrektur

Der bisherige Prüfaufbau öffnete F1 und einen Artikel und meldete sofort
`EDIT READY SEARCH`. Das H-Design veröffentlicht die Artikelsuche erst nach
dem ausdrücklichen Öffnen ihrer Suchleiste. Das Feld war deshalb beim Zugriff
des unabhängigen Clients noch nicht vorhanden. Derselbe Fehler trat im
vorherigen Vergleichsprogramm auf; er war kein belegter Fehler des neuen
Originaltext-Umbruchs.

Die beiden Remote-Prüfmodi lösen jetzt zuerst Strg+F als SDL-Tastaturereignis
aus, lassen vollständige Frames durchlaufen und prüfen eine geöffnete Leiste
mit positiver Feldgeometrie. Erst dann melden sie Bereitschaft. Die native
Bearbeitung, Auswahl und Fokusprüfung bleibt vollständig beim unabhängigen
UIA- beziehungsweise AT-SPI-Client. Der Prüfer setzt diese Ergebnisse nicht
direkt im Anwendungsmodell.

| Tatsächlich ausgeführte Prüfung | Ergebnis |
| --- | --- |
| Windows `accessibility_edit_windows` | Bestanden: beide Feldwerte, Unicode-Auswahl und Ersetzung, globale Fokusidentität, Rückkehr zur Artikelsuche, vorheriger/nächster Treffer und Tab-Ausstieg. |
| Windows `accessibility_edit_scroll_windows` | Bestanden: nativer Zugriff auf Anfang und Ende des langen Suchtextes ohne Übernahme des bisherigen Tastaturfokus. |
| Linux `accessibility_edit_atspi` | Der native Ablauf bestand bereits im ersten Nachlauf; der Python-Prüfer scheiterte noch an drei erwarteten BMP-Dateien. Nach Anpassung dieser Bildpflicht besteht der Prüfer mit unverändertem Programm und 32 aufgezeichneten nativen Ereignissen. |

Der Linux-Prüfer kontrolliert weiterhin Entry-Rolle, Editable-/Focused-Zustand,
Wertübernahme, Unicode-Caret und Auswahl, Textersetzung, Tabrückkehr sowie
Textänderungs- und Auswahlereignisse. Diese Anforderungen wurden nicht gelockert.
Der entfernte Standardzwang betraf ausschließlich die Existenz dreier Bilder.

## Nachweise und Grenzen

Die Prüfungen benötigen keine visuellen Vergleichsbilder. Ohne
`PHYSIM_TEST_ALL_CAPTURES` erzeugen diese beiden Remote-Modi daher keine
Aufnahmen; der Linux-Prüfer verlangt dann ausdrücklich deren Abwesenheit.
Der explizite Aufnahmemodus bleibt erhalten, wurde hier aber nicht ausgeführt.
Alle neuen Nachweise dieses Nachlaufs kommen ohne Bildschirmdateien aus.

Vorhandene Programme und Nachweise wurden erhalten. Die neuen Laufzeitarchive
enthalten nur geänderte Dateien gegenüber dem verlinkten vorherigen Archiv;
identische Programme und Bibliotheken werden darin nicht erneut gespeichert.
Beide Archivteile wurden nachgelesen und per SHA-256 geprüft. Das vollständige
lokale Dateimanifest liegt unter
`D:/Physim/build/validation/protocol-wrap-2026-10-26/6-f1-edit`.

Im älteren Fehlversuch ist der unabhängige Windows-Probehelfer nur durch Quelle,
Kommando, rohe Ausgabe und Binärhash belegt; ein eigenes eingefrorenes Archiv
dieser damaligen Hilfsdatei fehlt. Das damalige App-Archiv bleibt erhalten.
Der aktuelle positive Probehelfer ist im neuen Laufzeitstand enthalten.

Windows verwendet MSVC Release, Linux `Alpine-Dev`, GCC Debug mit ASan/UBSan,
Xvfb/Openbox und Software-OpenGL. Leak-Erkennung war bei der GUI-Prüfung aus.
macOS, physische Tastaturbedienung, tatsächliche Screenreader-Leseaufgaben,
weitere Lifecycle-Prüfungen und die vollständige H-/Gesamtoberflächenabnahme
bleiben offen. Die aufgeführten nativen Clientprüfungen schließen nur diese
konkreten Eingabefälle.
