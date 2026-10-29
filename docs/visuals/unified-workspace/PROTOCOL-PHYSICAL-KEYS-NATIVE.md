# Protokollhöhe: physische Windows-Tasten

Am 29. Oktober 2026 hat der Nutzer die zuvor vertagte Prüfung wieder aufgenommen.
Der [kompakte Nachweis](../../verification/protocol-physical-keys-2026-10-29.json)
bindet die Originalprotokolle an Programmhash, Compilerquellen, Laufzeitarchivindex
und Startkommando. Die interaktive Fixture erzeugt Prüfmeldungen, injiziert aber
keine Maus- oder Tastaturereignisse. Es wurden keine Bilder angelegt.

Der Nutzer hat die Trennleiste selbst mit der Maus betätigt und die Ausführung
bestätigt. Physim meldet nativen Windows-Tastaturfokus, die fokussierte
„Protokoll-Höhe“ und echte Pfeiltasten: Aufwärts ist SDL-Schlüssel 1073741906,
Scancode 82, Windows-Rohcode 57416. Das unterscheidet diesen Nachweis vom früher
durch Computer Use gelieferten Nummernblock-8-Ereignis.

Alle sechs protokollierten Umschalt-Pfeil-Aktionen ändern die Höhe um genau
40 logische Pixel in der richtigen Richtung. Dazu gehören 283 → 323 und die
Rückkehr 336 → 296 bei Umschalt + Abwärts. Die vorherige Mausbewegung änderte die
Ausgangshöhe; deshalb wird jeweils der tatsächlich protokollierte Vorherwert
verwendet. Nach dem Schließen endet das Programm mit Exitcode 0 und unverändertem
Binärhash.

Computer Use konnte trotz Wiederherstellung keine Fensterliste abrufen und hat
bei diesem Lauf keine App-Eingaben ausgeführt. Die erfolgreiche Prüfung beruht
auf der bestätigten menschlichen Bedienung und den empfangenen nativen Ereignissen.

Damit ist der konkrete offene Windows-Nachweis für die Umschalt-Pfeilbedienung
der Protokollhöhe erbracht. Andere Trenner, Pos1/Ende, DPI-/Monitorwechsel,
Screenreader-Lesen und die vollständige Oberflächenabnahme sind dadurch nicht
geprüft. Die anschließend gewünschte Farbangleichung gehört ebenfalls nicht
zu diesem bereits beendeten Lauf. Ältere eingefrorene Nachweise bleiben erhalten.
