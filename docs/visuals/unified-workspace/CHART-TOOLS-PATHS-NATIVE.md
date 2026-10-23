# Diagrammwerkzeuge und vollständige Workspace-Pfade

Stand: 23. Oktober 2026, auf Basis `56c2dca`. Dieser Teilabgleich bearbeitet die
[Nutzeraufnahmen](USER-REVIEW-2026-10-21.md) zu Zoomaktionen und Pfad-Tooltips.
Die [Gesamtabnahme](NATIVE-FIDELITY.md) bleibt offen.

## Diagrammwerkzeuge

Messdaten und Berichtsdiagramme verwenden nun dieselbe native Komponente.
Die Vorgaben stammen unverändert aus `analysis-workspace/analysis.css`,
`graphic-export.css`, dem Original-SVG und der gemeinsamen Palette in `bridge.js`.

| Vorgabe | Native Umsetzung |
| --- | --- |
| 12-px-Schrift, mindestens 28 px Höhe, Radius 7 px | Vorhandene entsprechende Schriftrolle; Maße wachsen mit der UI-Schriftgröße. |
| Transparente Fläche, Kontur, Hoverfläche | Hintergrund des Analysebereichs, normale Kontur und Hoverfarbe der gemeinsamen Palette. |
| 4 px Abstand; horizontale Innenabstände 9 bzw. 7 px | Gemessene Beschriftungsbreiten und dieselben Breitenumschaltungen. |
| Export mit zusätzlichem Abstand, 16-px-SVG und 7-px-Textabstand | Originalpfad mit sechs Linien, runden Enden und Verbindungen. |
| Äußerer Fokusring: 2 px, Abstand 3 px | Eigene Komponentenzeichnung; gemeinsamer nativer Fokus-/Aktivierungsvertrag bleibt erhalten. |
| Umbruch bei Platzmangel | Mehrere rechtsbündige Werkzeugzeilen; vollständiger UTF-8-Text bleibt erhalten. |

Zoom, Zurücksetzen und Grafikexport bedienen weiterhin die echten Ansichten.
Ein leeres Berichtsdiagramm deaktiviert seine Aktionen. Die normale
Pointer-Zuordnung der Dockbereiche bedeutet dagegen keinen deaktivierten Zustand:
Eine gültige Aktion bleibt auch bei einem Zeiger außerhalb des Bereichs normal
gezeichnet. Zwei Beschriftungen verwenden jetzt bei einem Lauf den Singular.

Die neue gemeinsame Textmessung erhält auch beim Umbruch alle Bytes, einschließlich
Leerzeichen und Pfadtrennern. Sie misst vollständige Präfixe mit der tatsächlichen
Schriftfunktion, damit etwa Kerning zwischen `A` und `V` berücksichtigt bleibt.

## Pfad-Tooltips

Ein Pfad stammt weiterhin aus dem echten Workspace-Baum. Nur der sichtbare Teil
einer Zeile darf den Tooltip auslösen. Der vollständige Pfad wird kopiert, bevor
eine Baumaktion Einträge ersetzen kann. Die Darstellung bricht den Text um und
bleibt mit acht logischen Pixeln Rand im jeweiligen nativen Fenster. Normale
Pfade behalten das vorhandene Popup-Material.

Ein außergewöhnlich langer Pfad bekommt eine begrenzte Scrollfläche. Der Zeiger
kann von der Zeile in dieses Popup wechseln; Radbewegungen darin erreichen nur
den Pfad. Der restliche Frame erhält währenddessen keine Pointer-Eingabe.
Tastatureingaben, Fokusverlust und ein Klick außerhalb schließen den Tooltip.
Beim Zurückholen eines zugehörigen Teilfensters wird dessen Tooltip verworfen.

Der Zustand umfasst höchstens 4095 Textbytes und passende UTF-8-Zeilenanfänge.
Er wird nicht in Benutzereinstellungen gespeichert. Die Zeilenmessung wird nur
bei geändertem Pfad, Schriftmaß oder Fenstermaß erneuert. Lange zugängliche
Textzeilen werden an UTF-8-Grenzen in vollständige Fragmente aufgeteilt. Das
belegt noch keine externe Screenreader-Abnahme.

## Native Nachweise

Die [Prüfdaten](component-details-2026-10-23/validation.json) trennen die internen
Quellstände, Programmhashes, Befehle und tatsächlichen Ergebnisse. Die Nummern
bezeichnen Prüfstände dieses Arbeitsschritts, keine Produktversionen.

| Ausführung | Tatsächliches Ergebnis |
| --- | --- |
| Windows Debug, Stand 5 | 2/2 neue Komponentengruppen: Dunkel und Hell mit großer Schrift im schmalen Client. |
| Windows Release, Stand 5 | 2/2 neue Komponentengruppen: Hell und hoher Kontrast; zusätzlich 1/1 Text-/Fokus-Unit-Test. |
| Windows Release, Stand 4 | 7/7: damalige Hell-/Kontrast-Komponenten, Dateibaum, Werkbank, Grafikexport, großer schmaler Mehrlaufvergleich und nativer Accessibility-UI-Test. |
| Alpine-Dev, Debug ASan/UBSan, Stand 5 | 6/6: neue Dunkel-/große Hell-Komponenten, Dateibaum, Werkbank, Grafikexport und großer schmaler Mehrlaufvergleich. |
| Windows Debug und Alpine-Dev, Stand 4 | Je 1/1 Text-/Fokus-Unit-Test. Die getestete gemeinsame API ist in Stand 5 unverändert. |

Der reine `accessibility_ui`-Katalogfall ist unter Linux nicht registriert und
wird dort nicht als ausgeführt gezählt. Die eigene neue native Fokusaufgabe
bleibt vom fehlenden externen Screenreader-Nachweis getrennt. Die acht
bestehenden Referenzexporte sind je Windows Release / Stand 4 und Linux /
Stand 5 bytegleich zum vorherigen plattformspezifischen Abnahmestand.

Die neuen nativen Aufgaben prüfen tatsächlichen Host-/Kontrollfokus und Enter-Zoom,
Maus-Zurücksetzen, Berichtszoom ohne Änderung der Rohdatenansicht, deaktivierten
Export sowie vollständige gezeichnete Werkzeugbeschriftungen bei 180 px
Komponentenbreite. Diese 180-px-Probe ist ein Komponenten-Stresstest und keine
Behauptung über die Mindestbreite eines ausgegliederten Arbeitsbereichs.

Die Pfadaufgabe beginnt mit einer tatsächlich angelegten Projektdatei. Danach
folgt eine ausdrücklich synthetische Layoutprobe mit 4095 Bytes, griechischem
Text und einem vorhandenen Emoji-Glyphen: 62 Zeilen im 1080×740-Client bei
UI-Schriftgröße 22. Die Probe erreicht das sichtbare Ende `Ende 😀.phys`, kehrt
zum Anfang zurück und schließt mit Escape. Editorinhalt, Auswahl, Caret und beide
Scrollwerte bleiben dabei erhalten. Unter Windows wurden 624 px nach der ersten
Radbewegung und 894 px am sichtbaren Ende gemessen.

Die Bildprüfungen messen Kontur, normale Fläche, Hover, äußeren Fokus und die
Pfeilspitze des ursprünglichen Export-SVG. An der antialiasierten einpixeligen
Kontur wird maximal ein RGB-Byte Kanalabweichung zugelassen; normale und
Hoverflächen sowie der voll gedeckte Fokusring müssen die Palette genau treffen.
Die maximale Tooltip-Probe prüft zusätzlich die deckende Popup-Fläche.

| Native Aufnahme | Bild |
| --- | --- |
| Windows, normale Messdatenwerkzeuge | [Bild](component-details-2026-10-23/windows-debug-dark-tools-normal.png) |
| Windows, Berichtsdiagramm mit denselben Werkzeugen | [Bild](component-details-2026-10-23/windows-debug-dark-tools-report.png) |
| Windows, große Schrift und äußerer Fokus | [Bild](component-details-2026-10-23/windows-debug-light-large-narrow-tools-focus.png) |
| Windows, vollständiger echter Projektpfad | [Bild](component-details-2026-10-23/windows-debug-dark-tip-real.png) |
| Windows, sichtbares Ende der maximalen Layoutprobe | [Bild](component-details-2026-10-23/windows-debug-dark-tip-bottom.png) |
| Linux, normale Werkzeuge | [Bild](component-details-2026-10-23/linux-debug-dark-tools-normal.png) |
| Linux, großes schmales Pfadende | [Bild](component-details-2026-10-23/linux-debug-light-large-narrow-tip-bottom.png) |

Alle abgelegten Bilder sind ursprüngliche native PNG-Dateien. Datei-Hashes und
vollständige RGB-Gleichheit mit den jeweiligen Testaufnahmen wurden geprüft.
Keine Aufnahme wurde umgefärbt, retuschiert oder skaliert. Das Archiv bewahrt
gezielt die Kontrollzustände und Pfadgrenzen; redundante Zwischenaufnahmen und
ersetzte Stand-4-Bilder werden nicht zusätzlich kopiert. Deren Testschritte und
die drei relevanten negativen Bilder bleiben erhalten.

## Erhaltene Gegenfälle und Korrekturen

- Stand 1: Der neue isolierte Unit-Test verwendete einen nur in der Haupt-App
  vorhandenen Disabled-Wrapper. Er wurde auf die eigentliche Bibliotheks-API
  umgestellt; dieser Fehler war kein bestandener Produktnachweis.
- Stand 2: Der Windows-Charttest erhielt keinen Hostfokus. Unter Linux erreichte
  er danach versehentlich den Analysecode-Editor statt des Berichts. Diese
  Fehlerfälle bleiben erhalten. Die Navigation bleibt nun im Auswertungsbereich.
  Das native Bild zeigte außerdem tatsächlich zu blasse gültige Aktionen: Die
  Dock-Pointer-Zuordnung war fälschlich als semantisch deaktiviert gezeichnet.
- Stand 3: Der Tooltip blieb nach einer echten Radbewegung bei Scrollwert null.
  GDB zeigte vor `nk_popup_begin` einen gespeicherten Wert von 624 und keinen
  vorherigen Popup-Zeiger; das neu angelegte Popup hatte wieder den Wert null.
  Der Offset wird nun nach dem erfolgreichen Begin und vor der Textanordnung
  auf das tatsächliche neue Popup übertragen.
- Stand 4 bestand die Verhaltens- und damaligen Pixelprüfungen auf beiden
  Plattformen. Beim zusätzlichen Sichtabgleich fiel jedoch das transparente
  Material des synthetischen Testcontainers auf. Stand 5 gibt dessen Popup
  ausdrücklich das normale Werkbank-Material und prüft dessen sichtbare Fläche.
  Die Produktimplementierung ist zwischen Stand 4 und 5 unverändert; geändert
  wurden nur die neue native Testfixture und ihre Bildprüfung.

## Verbleibende Grenzen

Die Umbruchprobe ist kein Nachweis, dass Windows eine reale Datei mit einem
4095-Byte-Pfad öffnen kann. Dateisystem-/API-Grenzen und die vorhandene
Windows-Manifestkonfiguration wurden durch diesen UI-Patch nicht geändert.

Die Aufnahmen verwenden tatsächlich 1×-Clientpixel. UI-Schriftgröße 22 und ein
schmales Fenster ersetzen keine physische DPI-/Monitorwechselprüfung.
macOS, externe Assistive-Technik, der direkte Tooltip-Abgleich in einem
ausgegliederten Workspace und sämtliche anderen Oberflächenzustände bleiben
gesonderte Abnahmen. Unterschiedliche Schriftfamilien und Glyphenrasterung
bleiben sichtbar zu prüfen; die bestandenen kleinen Farbproben erklären keine
vollständige Pixelgleichheit der Oberfläche.

Die lokale HTML-Datei wurde in diesem Schritt nicht neu interaktiv geöffnet.
Die ursprünglichen HTML-/CSS-/SVG-Quellen und Referenzbilder wurden verwendet,
ohne die bekannte Browser-Sperre zu umgehen oder Referenzen zu verändern.
