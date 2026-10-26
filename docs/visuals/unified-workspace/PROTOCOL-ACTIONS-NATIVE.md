# Protokoll C: Detailabstand und Aktionsformen

Der native Detailbereich übernimmt jetzt die 20 Pixel Innenabstand aus
`.log-catalog .log-detail`. Quellsprung und Kopieren verwenden die
inhaltsbreiten Aktionen der [freigegebenen CSS-Quelle](../design-options/options.css):
14 Pixel Schrift, 1,55-fache Zeilenhöhe, 11,2/6,72 Pixel Innenabstand,
1 Pixel Rand, 7 Pixel Rundung und 10 Pixel Abstand. Daraus folgt bei
Skalierung 1 eine Höhe von 37,14 Pixeln. Bei Platzmangel wechselt die zweite
Aktion mit demselben Abstand in die nächste Zeile.

Der Quellsprung erscheint als leise Textaktion; Kopieren behält seine
gerahmte Fläche. Ohne tatsächlich verfügbare Quellstelle entfällt die
Sprungaktion. Die produktive Beschriftung lautet „Quellstelle ansehen →“:
Sie öffnet die echte Datei; die Galerie bezeichnet ihre reine Beispielstelle
entsprechend anders. Der übrige Protokoll- und Editoraufbau bleibt erhalten.

## Nachweise und gefundene Ursachen

Die neue native Gegenprobe scheitert am vorherigen Layout: Beide Aktionen
messen 267 × 32 Pixel bei 18 Pixel Innenabstand. Erwartet sind unter Windows
in diesem Fall 160,3121 bzw. 138,0680 Pixel Breite, 37,14 Pixel Höhe und
20 Pixel Abstand zum Panelrand. Der [kompakte Bericht](../../verification/protocol-actions-2026-10-26.json)
verknüpft Quell-/Programmhashes, genaue Befehle, Exitcodes und Rohberichte.

| Prüfung | Ergebnis und Grenze |
| --- | --- |
| Aktionslayout und tatsächliches Kopieren | Je sechs Fälle auf Windows und Linux bestanden: dunkel, hell, hoher Kontrast, große Schrift, schmaler Aufbau und Meldung ohne Quellstelle. Maße, Abstand, Zeilenwechsel, unveränderte Quelle und kompletter Clipboard-Text sind geprüft. |
| Sichtbarer Abschlusspfeil | Seine tatsächliche native Pixelregion enthält Tinte. Der anfängliche Standardbutton verlor den Pfeil, weil Nuklear zusätzlich den Eckenradius vom Textbereich abzog. Die vorhandene native Treffer-/Accessibility-Anbindung zeichnet jetzt Text mit der vollständigen Glyphenhöhe und den CSS-Innenmaßen. |
| Prüfergegenproben | Fehlender Kopierschritt, gestreckte Kopierbreite, geänderter Quellen-CRC und leere Pfeilregion werden ausgehend vom echten nativen Lauf jeweils abgewiesen. Dafür entstehen keine neuen Bilder oder App-Läufe. |
| Originaltext-Regressionsfälle | Unter Windows und Linux je fünf Fälle bestanden. Sie stammen aus dem zweiten Nachlauf mit bytegleichen Produktdateien; danach wurden ausschließlich Testhelfer und eine optionale Fokusvoraussetzung geändert. |
| Navigator | Linux besteht acht Fälle mit denselben Produktdateien. Die neuen Windows-Läufe scheitern am fehlenden tatsächlichen Host-Tastaturfokus; daraus folgt keine neue Windows-Navigatorabnahme. |

Im ersten Prüfhelfer war eine Zeichenpufferabfrage nach dessen Leerung
ungeeignet. Diese Abfrage wurde entfernt; Breiten-, Höhen-, Abstands- und
Inset-Erwartungen blieben bestehen. Beim Großschriftfall musste der Helfer
zunächst den äußeren Scrollbereich bewegen, wenn die Aktion bereits in ihr
eigenes Panel passte. Linux verwendet den vorhandenen strengen PNG-Prüfer
ohne zusätzliche Pillow-Abhängigkeit. Diese Helferkorrekturen und ihre
fehlgeschlagenen Vorläufe bleiben im Bericht erkennbar.

Die ersten Diagnosegruppen bestehen ebenfalls auf beiden Plattformen, gehören
jedoch zum Zwischenstand vor der korrigierten Aktionszeichnung. Sie ersetzen
keine finale Diagnoseabnahme. Für diesen Zwischenstand sind App und eigene
Quellen archiviert; ein neues vollständiges Archiv aller transitiven
SDK-Kindprogramme wurde vor dem Folgebuild nicht angelegt. Der Bericht
behauptet deshalb keine vollständige Laufzeitkonservierung dieser Gruppe.

## Clipboard-Konvention unter Windows

SDL 3.2.30 fügt beim Schreiben von Windows-Klartext vor ein einzelnes LF ein
CR ein; vorhandenes CRLF bleibt erhalten. Das ist im
[versionierten Windows-Backend](https://raw.githubusercontent.com/libsdl-org/SDL/release-3.2.30/src/video/windows/SDL_windowsclipboard.c)
belegt und im nativen Test tatsächlich beobachtet: 115 Originalbytes mit zwei
einzelnen LF werden zu 117 Clipboard-Bytes. Der Test fordert genau diese
Ausgabe; er entfernt weder beliebige CR noch andere Leerzeichen.

Originaltext, Quelldatei und TXT-Export behalten ihre getrennten Byteverträge.
Die vorhandenen CRLF-Originaltextfälle bestehen weiterhin mit exakter Kopie.
Unter Linux wird der Klartext unverändert geprüft. Der Windows-Klartextweg
ist für LF-Eingaben somit **nicht byteidentisch**; ein zusätzliches bytegenaues
Clipboardformat ist hier nicht implementiert. Diese konkrete SDL-Konvention
ist keine allgemeine Freigabe, Originalmeldungen zu verändern.

## Windows-Desktop-Fokus bleibt offen

Der zusätzliche opt-in Prüfmodus `PHYSIM_TEST_WAIT_FOR_HOST_FOCUS=1` beginnt
erst bei echtem SDL-Host-Tastaturfokus mit seinen Eingaben. Er schwächt die
bestehenden Fokus- oder 40-Pixel-Erwartungen nicht ab. Die absichtlich
unfokussierte Gegenprobe bleibt davon ausgenommen.

Drei solche Vorläufe liefen vor dem ersten Eingabeschritt in die
Fokus-Zeitgrenze. Computer Use meldete für ein eindeutig ausgewähltes eigenes
Testfenster eine erfolgreiche Vordergrundaktivierung; der native Host-Fokus
blieb trotzdem aus. Ein späterer Desktop-Klick auf das zuvor beobachtete
Suchfeld scheiterte mit „coordinate input geometry is unavailable“.
Eine bestätigte Klickzustellung liegt nicht vor. Eine UIA-Meldung über das
Suchfeld allein beweist hier keinen Host-Fokus.

Damit bleiben der neue vollständige Windows-Navigatornachweis und die
[direkte Separator-Fokusprobe](PROTOCOL-FOCUS-NATIVE.md) offen. Nächster
Prüfschritt ist eine stabile interaktive Fixture mit beobachteter
Fenster-/Bildgeometrie und tatsächlichem SDL-Fokus, bevor weitere Tasten
gesendet werden. Die fehlgeschlagenen Versuche werden nicht als bestanden
oder als physische Tastaturabnahme gewertet.

## Aufbewahrung und verbleibende Gestaltung

Versioniert sind nur zwei unveränderte native Vergleichsbilder:
[vorher](protocol-actions-2026-10-26/before.png) und
[nachher](protocol-actions-2026-10-26/after.png).
Jeder Aktionsfall hat eine gezielte Endaufnahme; die Eingabefolge und
Gegenproben stehen als Text/JSON im Bericht. Die Bilder zeigen unterschiedliche
Detail-Scrollstände und sind kein Ganzfenster-Pixelgleichheitsbeweis.
Bereits archivierte Programme werden anhand ihrer Prüfsummen wiederverwendet;
neue Laufzeitarchive enthalten nur geänderte Dateien. Alle Ausgaben bleiben
auf D:, F: wurde nicht angesprochen.

Die [natürliche Originaltexthöhe](PROTOCOL-ORIGINAL-FLOW-NATIVE.md) bleibt
erhalten. Offen sind insbesondere die 12-KiB-Textabschnitte, weitere
Detailtypografie und Blockabstände, die vollständige Hover-/Druck-/Fokus-/
Deaktiviert-Matrix, tatsächliches Screenreader-Lesen, macOS, zusätzliche
Pixeldichten und die gesamte C-/H-/Oberflächengleichheit. Große Schrift ist
hier mit UI-Größe 22 gegenüber 16 geprüft, keine neue 200%-Textabnahme.
Die Linux-Fälle verwenden Xvfb/Openbox, Software-OpenGL und ASan/UBSan mit
`detect_leaks=0`. Die lokale interaktive HTML-Vorlage wurde in diesem Nachlauf
nicht neu im Browser geöffnet.

Die tatsächlichen Fonts unterscheiden sich weiterhin: Windows lädt Segoe UI,
die geprüfte Alpine-Umgebung DejaVu Sans. Deshalb sind die inhaltsbreiten
Aktionen unter Linux breiter, obwohl dieselben logischen CSS-Maße verwendet
werden. Dieser vorhandene Schrift-Fallback ist keine plattformübergreifende
Pixelgleichheit und bleibt im Gesamtabgleich zu berücksichtigen.
