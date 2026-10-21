# Werkbank: API-Karte und Protokollkopf

Nachfolgender Stand: [Kontextmaße, Schriftlaufweite und Scrollzugang](WORKBENCH-CONTEXT-FLOW.md).
Die nachstehenden Bilder und Prüfdaten bleiben der historische Stand 47.

Fortsetzung des nativen Referenzabgleichs vom 21. Oktober 2026, auf Grundlage
von `6d07783`. Dies ist ein weiterer Teilstand, keine vollständige Designabnahme.
Maßgeblich bleiben die unveränderten `develop-light.png`, `develop-dark.png`
und `develop-designs/workbench.css`.

## Änderung

Die gefüllte API-Karte verwendet den vorgesehenen Aufbau: kleine Kategorie,
monospaced Name, Beschreibung, Code, zwei Argumenthinweise und die untere
Aktion. Rahmen, Fläche, Radius, Innenabstand und Schriftrollen folgen der
Werkbankvorlage. Lange Inhalte bleiben in der Karte scrollbar; die Aktion
bleibt unten. Die gesamte Kontextspalte behält ihren eigenen Scrollzugang.

Die Inhalte stammen aus dem tatsächlich geladenen Referenzdokument. Nur ein
Codeblock unmittelbar nach dem ausdrücklichen Absatz „Beispiel für den Editor:“
ist einfügbar. Signaturen und Überladungen werden nicht als Beispiele behandelt.
`ps_channel_add` und Physims `Channel` enthalten solche dokumentierten Beispiele.
Einfügen ersetzt ausschließlich die aktuelle Auswahl im passenden Editor,
erzeugt einen Undo-Schritt und stellt den Editorfokus wieder her. Die Aktion
speichert und kompiliert nicht selbst; Kontextvariable, Bezeichner und Kanalname
müssen gegebenenfalls zum eigenen Modell angepasst werden.

Die Kategorie öffnet den vollständigen Abschnitt im F1-Handbuch. Bei Symbolen
ohne markiertes Beispiel lautet die untere Aktion „Im Handbuch öffnen“.
„Hilfe → API-Referenz schließen“ erhält das bisherige ausdrückliche Schließen,
ohne die eigene alte Buttonzeile in der Karte weiterzuzeigen.

Der Protokollkopf zeigt das tatsächliche Profil rechts mit dem Vorlageneinzug.
Das zusätzliche Schließenkreuz entfällt; die vorhandene Titelaktion klappt den
Inhalt ein und aus. Titel und Diagnosezahl werden mit den tatsächlichen
Fontbreiten angeordnet, damit sie bei großer Schrift nicht überlappen.
Die bestehenden Menüs und das Herausziehen an der Überschrift bleiben erhalten.

## Gezielt behobene Fehler

- Fontmetriken werden vor der Atlasbereinigung aus dem echten `nk_font`
  gespeichert. Der erste neue GUI-Versuch übergab fälschlich einen
  `nk_user_font`; der Windows-Sanitizer wies den Zugriff zurück.
- `nk_layout_space_bounds` liefert hier den Clipbereich und nicht die nächste
  Widgetposition. Die Karte verwendet die tatsächlichen Widgetkoordinaten;
  das [negative Bild](workbench-context-2026-10-21/counterexample-position-v43.png)
  zeigt die zuvor getrennten Rahmen und Inhalte.
- Große Schrift benötigte eine echte Umgehung der globalen Mindestzeilenhöhe.
  Klammern um einen objektartigen C-Makronamen unterdrücken dessen Ersetzung
  nicht. Explizite direkte Layoutfunktionen vor den Aliasdefinitionen verhindern
  die zweite Vergrößerung. Die native Probe prüft auch die letzte Argumentzeile.
  Der strengere Windows-Gegenlauf von Stand 46 verwirft den alten Zustand:
  335,881 px gezeichneter Inhalt bei 292,631 px Platz. Die
  [Aufnahme vor der Korrektur](workbench-context-2026-10-21/counterexample-windows-large-arguments-v45.png)
  und ihr
  [Linux-Gegenstück](workbench-context-2026-10-21/counterexample-linux-large-arguments-v45.png)
  bleiben erhalten. Frühere erfolgreiche Aktivierungsprüfungen werden dadurch
  nicht nachträglich zu einer vollständigen Sichtbarkeitsprüfung erklärt.

## Verbleibender unmittelbarer Designabgleich

Die Karte ist im üblichen Fenster noch sieben Pixel schmaler als die Vorlage:
Nuklear reserviert in der äußeren Kontextspalte Scrollbarbreite, auch wenn die
Leiste gerade nicht sichtbar ist. Außerdem bleiben die genaue Kategorienlaufweite,
Gliederungsabstände und die vertikale Kartenposition abzugleichen. Das sind offene
Umsetzungsaufgaben; eine technische Unmöglichkeit ist dafür nicht belegt.

Native Schriftmetriken unterscheiden sich zwischen den geprüften Plattformen;
deshalb können dieselben echten Beispiele unterschiedliche Umbrüche und Höhen
benötigen. Die Browseraufnahme verwendet außerdem andere illustrative Namen und
Quellen. Die native Modellnotiz zeigt weiterhin ihren tatsächlichen Leerzustand,
solange keine solche Dokumentinformation vorliegt. Das Protokoll nennt die
tatsächlich vorhandenen Diagnosen; deren Parser besitzt noch keine getrennten
Fehler-/Warnungszahlen. Diese Unterschiede werden nicht durch Demoausgaben ersetzt.

F1 Design H, Simulieren, Auswerten und der freigegebene Grafikexport bleiben
Bestandteil des [Gesamtauftrags](NATIVE-FIDELITY.md).

## Ausgeführte Prüfung

[Quellstände, Binär-/Bildhashes, Originalkommandos und Ergebnisse](workbench-context-2026-10-21/validation.json)
trennen die früheren negativen Versuche von den erfolgreichen Prüfungen.

Windows 11 Pro 26200 mit MSVC-ASan und Alpine-Dev 3.24.2 mit GCC-ASan/UBSan
bestehen die vier fachlichen Gruppen: Werkbank, Gliederung, API-Referenz und
API-Beispielkarte. Das sind 18 unterschiedliche native App-Abläufe je Plattform.
Werkbank, Gliederung und der erste Referenzlauf verwenden Stand 45; die neue
Kartenprobe mit dem vollständigen Argumentbereich verwendet Stand 47.
Die reguläre API-Referenz wurde anschließend auf Stand 47 erneut ausgeführt.
Die direkten Layoutfunktionen ändern ausschließlich die neue API-Karte.

Die Kartenprobe prüft C und Physim, Hell/Dunkel, 1440 × 1000 und 1080 × 740,
UI-Schrift 16/22, echten Tastaturfokus und Herausziehen/Schließen des Kontextfensters.
Einfügen verändert nur die gewählte Quelle, erhält die Analyse und die noch
ungespeicherte Datei auf dem Datenträger, lässt sich mit einem Undo zurücknehmen
und mit Redo wiederherstellen. Erst F5 speichert und baut. Die anschließenden
echten Läufe enthalten einen Kanal `angle` in `rad`; alle aufgezeichneten Werte
entsprechen den Zeiten der kontrollierten Testmodelle. In der korrigierten großen
Karte stimmen berechnete und tatsächlich belegte Inhaltshöhe überein: Windows
292,631 px und Linux 289,805 px. Die letzte Argumentzeile liegt vollständig vor
der Aktionsfläche.

Der API-Unit-Test wurde auf beiden Plattformen erneut ausgeführt: alle 1.264
Symbole lösen sich auf, einschließlich 15 dokumentierter Typführer-Fallbacks.
Markierte Beispiele, normale Signaturen, Tabellen und unveränderte Ausgaben bei
ungültigen Bereichen sind geprüft. Die relevanten Unit-/Parser-/Dokumentationsquellen
sind gegenüber den erfolgreichen Unit-Builds unverändert.

Vier unmittelbare Rahmenvergleiche gegen die Original-PNGs – beide Plattformen
in Hell/Dunkel – bestehen weiterhin je 13 begrenzte Prüfungen. Diese prüfen
Flächen und Grenzen, nicht die vollständige Karte oder gesamte Oberfläche.

| Tatsächliche Aufnahme | Windows | Linux |
| --- | --- | --- |
| C, dunkel | [Bild](workbench-context-2026-10-21/windows-c-dark-v47.png) | [Bild](workbench-context-2026-10-21/linux-c-dark-v47.png) |
| Physim, hell | [Bild](workbench-context-2026-10-21/windows-phys-light-v47.png) | [Bild](workbench-context-2026-10-21/linux-phys-light-v47.png) |
| Große Schrift, Mindestfenster | [Bild](workbench-context-2026-10-21/windows-c-light-large-narrow-v47.png) | [Bild](workbench-context-2026-10-21/linux-c-light-large-narrow-v47.png) |
| Separates Kontextfenster | [Bild](workbench-context-2026-10-21/windows-c-light-detached-v47.png) | [Bild](workbench-context-2026-10-21/linux-c-light-detached-v47.png) |
| Zurück im Hauptfenster, eingefügter Text | [Bild](workbench-context-2026-10-21/windows-c-light-returned-v47.png) | [Bild](workbench-context-2026-10-21/linux-c-light-returned-v47.png) |

Alle PNGs sind verlustfreie Konvertierungen tatsächlicher BMPs mit identischen
dekodierten RGB-Pixeln. Die Aufnahmen wurden unmittelbar visuell geprüft;
Referenzen und Ausgaben sind nicht retuschiert. Die Bedienproben injizieren
SDL-Ereignisse und verwenden den realen nativen Fokuspfad. Physische Zeigerbedienung,
neue externe Screenreader, gemischte Monitor-DPI und macOS sind damit nicht geprüft.
Die Builds enthalten die separat erfasste lokale Accessibility-Arbeit; ein sauberer
Checkout ist nicht nachgewiesen.

Zehn ersetzte lokale PNG-Kopien wurden nach Sicherung der finalen Bilder entfernt:
1.233.800 Byte. Roh-BMPs, Originalreferenzen, aktuelle Bilder und negative
Gegenproben bleiben erhalten; Einzelpfade und Hashes stehen im Manifest.

Geprüfter Windows-Build: `D:/Physim/build/native/Design-analysis/bin/physim.exe`.
Der übliche Debug-Startpfad und ein portables Releasepaket wurden hier nicht
aktualisiert oder neu abgenommen.
