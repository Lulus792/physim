# Codekontext: Tooltip nur für sichtbare Gliederungszeilen

Die Gliederung zeigt keine Hinweise mehr über anderen Panels, wenn die
zugehörige Zeile außerhalb ihres sichtbaren Bereichs liegt. Der bisher im
[Originaltext-Nachlauf](PROTOCOL-ORIGINAL-NATIVE.md) dokumentierte Hinweis aus
dem Codekontext über dem Protokoll ist damit für diesen Pfad behoben.

Die Ursache war eine reine Rechteckprüfung: Auch unsichtbare Zeilen behalten
ihre Layoutposition für Navigation und Scrollen. Der Hoverpfad behandelte diese
Position trotzdem als Treffer, ohne den Gruppenclip zu prüfen. Ein Tooltip
zeichnet anschließend außerhalb dieses Clips und wurde über dem Protokoll
sichtbar. Die bestehende Klickbehandlung hatte bereits einen Cliptest.

Die Korrektur verwendet für Zeilenhervorhebung und Tooltip denselben sichtbaren
Treffer: Das Widget darf nicht vollständig ausgeclippt sein, und der Zeiger
muss sowohl innerhalb der Zeile als auch innerhalb des aktuellen Clips liegen.
Beschriftung, Anordnung, Größen, Navigation und Farben bleiben dabei erhalten.

## Vorher-/Nachher-Nachweis vom 26. Oktober 2026

Der neue native Prüfmodus erzeugt ausdrücklich 24 Testdeklarationen. Er prüft
zuerst den zusätzlichen Tooltip-Zeichenbefehl an einer sichtbaren Zeile und
bewegt den Zeiger dann auf die Layoutposition einer unsichtbaren Zeile über dem
Protokoll. Die Quellprüfsumme muss in allen Schritten gleich bleiben.

| Prüfung | Beobachtetes Ergebnis |
| --- | --- |
| Unkorrigierter Windows-Stand | Sichtbare Zeile: ein Beschriftungsbefehl, beim Hover zwei. Verdeckte Zeile: zunächst null, über dem Protokoll fälschlich ein Tooltip-Befehl. Der Prüfer weist diesen Stand zurück. |
| Korrigierter Windows- und Linux-Stand | Je Dunkel, Hell und große Schrift: sichtbarer Tooltip bleibt erhalten, verdeckte Zeile erzeugt keinen Tooltip; Quelle unverändert. |
| Bestehende Gliederungsregression | Auf beiden Plattformen bestehen C/Dunkel, Physim/Hell und C/Hell/große Schrift einschließlich Quellsprung, Auswahl, Texteingabe, Undo und gespeicherter Quelldatei. |
| Eng begrenztes Windows-Bildpaar | 1204 × 866 Pixel: genau 3.193 veränderte Pixel im Rechteck `(1082, 707)–(1181, 740)` des unerwünschten Tooltips. Alle übrigen Pixel stimmen überein. |

[Fehlerzustand](outline-hover-native-2026-10-26/before.png) ·
[korrigierter Zustand](outline-hover-native-2026-10-26/after.png) ·
[Kommandos, Quell-/Programmhashes und Ergebnisse](../../verification/outline-hover-2026-10-26.json).
Das Bildpaar belegt ausschließlich diese Zustandsänderung, keine vollständige
Designgleichheit der Oberfläche.

## Aufbewahrung und Grenzen

Der neue Hoverfall erzeugt pro Plattform nur ein finales Bild sowie bei Fehlern
einen gezielten Beleg; Eingaben und Zeichenbefehle sind JSON. Das Windows-Paar
ist versioniert. Die 18 BMPs der bestehenden erfolgreichen Gliederungsprüfung
wurden vollständig komprimiert und nachgelesen, bevor ihre losen Kopien entfernt
wurden: 88.450.356 Byte Rohdateien, 2.545.914 Byte Archiv. Das ist eine logische
Dateigrößenbilanz, keine Messung physisch freigegebener NTFS-Blöcke. Die lokale
Quittung enthält ursprüngliche Pfade, Größen und SHA-256-Werte zur gezielten
Wiederherstellung. Frühere eingefrorene Nachweise bleiben unverändert.

Neue Daten liegen unter
`D:/Physim/build/validation/protocol-wrap-2026-10-26/7-outline-hover`.
Ein Hilfsprogramm wurde nach einer ineffizienten Pixelzählung beendet; die
bereits geschriebenen Archive wurden beim korrigierten Nachlauf nachgelesen
und wiederverwendet. App-Tests und ihr Ergebnis waren davor abgeschlossen.

Windows verwendet MSVC Release und NVIDIA/OpenGL, Linux `Alpine-Dev`, GCC Debug
mit ASan/UBSan sowie Xvfb/Openbox/Software-OpenGL ohne Leak-Erkennung.
Teilweise abgeschnittene Zeilen verwenden denselben Zeiger-Cliptest, wurden hier
aber nicht als eigener nativer Größenfall variiert. macOS, physische DPI-/
Monitorwechsel, Screenreader und die vollständige UI-Abnahme bleiben offen.
