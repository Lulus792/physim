# Laufparameter und Laufeinstellungen als native B-Wertefelder

Fortsetzung des [Rundungs-/Titelabgleichs](ROUNDED-SECTIONS-NATIVE.md).
Maßgeblich bleiben die ausgewählten [B-Wertefelder](../simulation-designs/MODEL-EDITING.md)
und `inspector-variants.css` / `inspector.css` im selben Referenzordner.
Die gemeinsame Farbpalette stammt aus der freigegebenen Gesamtoberfläche:
`workspace.css` und die durch `bridge.js` übernommenen Farbrollen übersteuern
die ursprünglichen Farben der einzelnen Simulationsvorschau.

## Ursache und Änderung

Die Laufparameter verwendeten noch generische Beschriftungszeilen und große
Standardknöpfe. Die neuen Karten übernehmen die kleine Beschriftung mit Einheit,
19-px-Wertschrift, 35-px-Eingabe und dezente 8-px-Rundung der Vorlage. Die
CSS-Abstände von 11 px oben/unten und 12 px seitlich ergeben einschließlich
des Rahmens 12 beziehungsweise 13 px bis zum Inhalt. „Standard“ bleibt direkt
der jeweiligen echten Eingabe zugeordnet; seine zusätzliche Abschwächung bei
gesperrten Feldern entspricht der zusammengesetzten Deckkraft der Vorlage.
Beschreibungen, Grenzen und Standardwerte stammen weiterhin aus dem kompilierten
Modell. Es werden keine Beispielwerte in die native Oberfläche eingesetzt.

Zeitschritt, adaptive Grenzen, Zufallsseed und Haltepunkte verwenden denselben
Baustein. Fehlertexte bleiben im gemeinsamen Scrollbereich. Der alte Abschluss
der benutzerdefinierten Gruppe rief irrtümlich `nk_tree_pop` auf, obwohl diese
Gruppe keinen Nuklear-Baum öffnet. Der native Debug-Gegenlauf bricht deshalb
beim Öffnen der Einstellungen mit `layout->row.tree_depth` ab. Der korrigierte
Lab-Abschluss verwendet die zugehörige eigene Abschnittsfunktion.

Komma und Punkt sind für numerische Benutzereingaben zulässig. Das kanonische
Runner-/Projektformat bleibt unverändert. Inaktive gültige Zahlen erhalten eine
kurze Darstellung, die wieder exakt denselben Gleitkommawert ergibt; während
des Editierens wird der Text nicht umformatiert. Quelltextgebundene Modellwerte
und Laufparameter behalten ihre unterschiedlichen Verträge. Die Sperre bei
aktivem oder pausiertem Lauf sowie der Standardwert und die SI-Umrechnung
bleiben angebunden.

Feldnamen, Beschreibungen und Werte sind modellabhängig und können deshalb von
den Beispieltexten der HTML-Vorlage abweichen. Die Gestaltung wird übernommen,
ohne die tatsächlichen deklarierten Parameter umzubenennen.

## Abnahmeumfang

Der native Bildablauf verwendet das echte C-Pendel mit acht deklarierten
Parametern und einer tatsächlich erzeugten Laufdatei. Er vergleicht sechs
Darstellungen mit beiden Schriftquellen, gesperrte Felder sowie feste,
adaptive und ausdrücklich ungültige Einstellungen. Anfang und Ende des
Inhalts werden durch tatsächliche Scrollereignisse erreicht. Geometrie und
sichtbare Wertschrift werden separat aus Zeichenkommandos und PNGs geprüft.
Der Bedienablauf prüft Komma/Punkt, Null-Zeitschritt und Seed-Überlauf; bestehende
Integrationsfälle prüfen Standardwerte, Speichern/Öffnen und neue Laufdaten.
Diese Abläufe erzeugen interne SDL-Ereignisse und prüfen die tatsächlichen
nativen Fenster, Zeichenkommandos, OpenGL-Aufnahmen und gespeicherten Dateien.
Sie ersetzen keine vollständige manuelle Desktop- oder Screenreader-Abnahme.

Die [eingefrorenen Belege](run-fields-2026-10-24/README.md) enthalten 302 native
Bildhashes und die erfolgreichen Windows-/Linux-Ergebnisse. Acht unterschiedliche
Linux-Gruppen und vier Windows-Gruppen bestehen; die Testrunnerprüfung besteht
zusätzlich auf beiden Systemen. Vier Bindungsgruppen sind SDK-Quelle 6
zugeordnet, der abschließende Studienlauf und die letzten Feld-/Kopfprüfungen
SDK-Quelle 7. Die genaue Zuordnung und verworfene Zwischenstände sind im
Belegordner erhalten. Fremde AT-SPI-Änderungen im lokalen SDK werden ausdrücklich
benannt; ein sauberer Checkout wurde hiermit nicht nachgewiesen.

Die vollständige Gestaltung von Laufgrenzen, Ereignissen, Aufzeichnung,
weiteren Arbeitsbereichen und F1 bleibt offen. Sehr lange eingegebene Zahlen
können im nativen einzeiligen Editor horizontal gescrollt werden. Schmale
Karten umbrechen Einheiten und Aktionen, ohne Schrift zu verkleinern. Eine
vollständige Browser-Pixelgleichheit, praktische Screenreader-Aufgaben,
macOS und physische Monitor-/DPI-Wechsel werden hiermit nicht behauptet.

## Speicherpflege

Nach dem erfolgreichen letzten Nachlauf wurden ausschließlich 300 ersetzte
PNG-Rohbilder aus drei früheren erfolgreichen Feldprüfungen entfernt:
36.621.422 Bytes. Die [Bereinigungsaufzeichnung](run-fields-cleanup-2026-10-24.json.gz)
enthält Pfade, Größen und Hashes. Fehlgeschlagene Läufe, Original-Gegenfälle,
alle 302 letzten Aufnahmen, Laufdaten, SDKs und Debug-Symbole bleiben erhalten.

## Nachfolgender Inspektorabgleich

Der anschließende [Abgleich von Laufgrenzen, Ereignissen und Aufzeichnung](INSPECTOR-TAIL-NATIVE.md)
setzt die in diesem Bericht noch offenen drei Bereiche mit gesonderten nativen
Belegen fort. Die vollständige Oberfläche einschließlich F1 bleibt weiterhin offen.
