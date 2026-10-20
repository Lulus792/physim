# Auswerten: Felder, Laufliste und aktive Tabgrenze

Dieser Schritt führt den [nativen Auswertungsstand](ANALYSIS-NATIVE.md) näher an
die unveränderten Referenzen heran. Er ersetzt weder deren offene Fachfunktionen
noch den vollständigen Abgleich aller Arbeitsbereiche.
Die anschließende [kompakte Einheitenwahl und Textkorrekturen](ANALYSIS-UNITS.md)
führen diesen eingefrorenen Quellstand weiter.

## Sichtbare Änderungen

Das Suchfeld verwendet den Einzug von zwei Pixeln, Höhe 34, Radius 6 und den
ein Pixel starken Rahmen der Vorlage. Die geraden Kanten werden als gefüllte
Kontur gezeichnet: Der vorher verwendete Nuklear-Linienstrich verteilte die
Rahmenfarbe auf benachbarte Pixel. Die Bearbeitung bleibt ein echtes natives
Textfeld und erhält den zugänglichen Namen „Messläufe durchsuchen“.

Die Suchfeld- und Kanaltexte verwenden die 12-Pixel-Rolle. Die Kanalwahl besitzt
die Eingabefläche der Referenz und einen integrierten Pfeil ohne zusätzliches
graues Rechteck. Native Auswahl und bestehende Kanaldaten bleiben erhalten.
Die aktive Tablinie liegt auf der unteren Bereichsgrenze. Lauflistenabstände
werden aus der wirklichen Kartenhöhe und dem Abstand berechnet; ungewählte
Karten erhalten die vorgesehene Hoverfläche. Auswahl und Referenz behalten ihre
bisherige Bedeutung.

## Eingegrenzte Prüfung

Die abschließende Prüfung gehört zum internen Quellstand 24. Der
[Nachweis](analysis-controls-2026-10-20/validation.json) hält Quell-/Programmhashes,
Befehle, Rückgabewerte und verlustfrei konvertierte native Aufnahmen fest. Die
zehn separat erfassten Dateien vorgefundener Accessibility-Arbeit bleiben
unverändert. Ein sauberer Checkout und manuelle Assistive-Bedienung sind damit
nicht nachgewiesen.

Auf Windows und Alpine-Dev besteht jeweils die bestehende Auswertungsmatrix mit
fünf tatsächlich ausgeführten Varianten: Hell/Dunkel, Kontrast sowie Hell/Dunkel
bei 1080 × 740 und UI-Größe 22. Die Matrix durchläuft Seitenzustände, Rückkehr aus echten
Teilfenstern, Einstellungen, Trenner und den Scrollzugang zur Statistik/CSV.
Die 15 breiten Funktionsgruppen aus dem vorherigen Bericht sind weiterhin
Quellstand-21-Nachweise; sie werden nicht als neue Ausführung dieses Schritts
ausgegeben.

Der direkte Farbvergleich umfasst sechs ausdrücklich benannte Pixelpositionen
in Hell/Dunkel auf beiden Plattformen: Suchfeldrahmen und -fläche, Tabunterkante,
Kanalfläche und Hintergrund neben dem Pfeil. Alle 24 Stichproben bestehen. Die vorherigen unveränderten
Stand-21-Bilder verfehlen mehrere dieser Kriterien. Diese 24 Stichproben sind
keine Vollbewertung von Geometrie, Schrift oder Bedienzuständen.

| Aktuelle native Aufnahme | Windows | Linux |
| --- | --- | --- |
| Dunkel | [Bild](analysis-controls-2026-10-20/windows-dark-v24.png) | [Bild](analysis-controls-2026-10-20/linux-dark-v24.png) |
| Hell | [Bild](analysis-controls-2026-10-20/windows-light-v24.png) | [Bild](analysis-controls-2026-10-20/linux-light-v24.png) |
| Hoher Kontrast | [Bild](analysis-controls-2026-10-20/windows-contrast-v24.png) | [Bild](analysis-controls-2026-10-20/linux-contrast-v24.png) |
| Kleine Fläche, große Schrift, dunkel | [Bild](analysis-controls-2026-10-20/windows-dark-large-narrow-v24.png) | [Bild](analysis-controls-2026-10-20/linux-dark-large-narrow-v24.png) |
| Kleine Fläche, große Schrift, hell | [Bild](analysis-controls-2026-10-20/windows-light-large-narrow-v24.png) | [Bild](analysis-controls-2026-10-20/linux-light-large-narrow-v24.png) |

## Weiter offen

Die Einheitenwahl öffnet noch den ausführlichen vorhandenen Einheitendialog.
Ein kompakter Auswahlablauf, Mehrlaufkurven und -statistik, echte zusätzliche
Metadaten, der freigegebene Grafikexport und der gemeinsame Analyseeditor sind
weiter umzusetzen. Auch Kartenrahmen, weitere Einzüge, komplette Typografie,
große Schrift und alle Hover-/Fokuszustände benötigen weiteren Referenzabgleich.
Diese Unterschiede sind offene Arbeit, keine nachgewiesenen technischen Grenzen.

Die Linux-Aufnahme mit 1080 × 740 und UI-Größe 22 zeigt konkret gekürzte
Suchplatzhalter, Laufmetadaten, die Qualitätszeile und einen Achsenwert nahe
null. Die normalen Seitenbreiten und einzeiligen Text-/Achsenplätze wachsen
hier nicht ausreichend mit der Schrift. Das ist eine zu korrigierende
Layoutgrenze; die bestandene Ablaufprüfung erklärt diese Darstellung nicht für
fertig. Der numerisch sehr kleine Achsenwert benötigt außerdem eine passendere
Zahlenformatierung.

Die geprüften lokalen Programme bleiben
`build/native/Design-analysis/bin/physim.exe` und
`build/native/Design-analysis-linux/bin/physim`. Andere vorhandene Buildordner
werden dadurch nicht aktualisiert. Physische Windows-Mausbewegung, tatsächliche
Screenreader, Monitor-DPI-Wechsel und macOS bleiben gesondert nachzuweisen.
