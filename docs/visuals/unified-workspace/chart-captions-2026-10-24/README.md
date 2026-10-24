# Ruhige native Diagrammköpfe

Fortsetzung von `bdd1ac2e22748b0f466db8623bae7aee3638ce05`. Die Referenz `simulation-designs/index.html` zeigt in `chart-label` zwei Textspannen; `simulation.css` bestimmt Name links, gedämpfte Einheit rechts und 9 px Abstand. Es gibt dort weder Pfeil noch Buttonfläche. Der native Diagrammkopf übernimmt diesen Ruhezustand jetzt auch bei Hover. Das bisher für den Pfeil reservierte Stück der langen Namenszeile entfällt.

Die Kanalauswahl bleibt derselbe native Button-/Popup-Weg. Ihre Namen, Beschriftung, Tabposition und Aktivierung sind unverändert. Der sichtbare Tastaturfokus bleibt erhalten. Das separate Auswahlfeld im Inspektor behält Chevron und Hoverfläche. Damit ist eine im vorherigen [Kurvenbericht](../CURVES-NATIVE.md) noch ausdrücklich offene gestalterische Anpassung umgesetzt; es bestand keine technische Unmöglichkeit.

## Prüfgegenstand

Der kontrollierte native Kurvenprüfweg ergänzt einen achten Fall: Mausbewegung per SDL in den Kopf. Er prüft den tatsächlichen Hoverzustand und pixelgenau den unveränderten Hintergrund. Die vorigen Leer-/Einzel-/Fehlwert-/Lücken-/Offset-/Langtext-/Umrechnungsfälle bleiben erhalten. Zusammen sind dies 48 Zustände je Schriftquelle, 96 PNGs. Diese kontrollierten Werte werden oberhalb eines unveränderten echten pausierten C-Pendels gezeichnet. Sie belegen keine im Runner erzeugten Ausfälle.

Die eigenständige Messbereichsgruppe prüft weiterhin 60 native Bilder, echte Tastaturwahl des Kanals, Anzeigeumrechnung, vollständige fokussierte Beschriftung und unveränderte Quelle, Daten, Szene, Zeit und Messdatei. Die zusätzliche ursprüngliche Inspektorgruppe prüft normale/große externe Teilfenster und Rückkehr. Praktische Mausaktivierung des neuen Diagrammkopfes ist daraus nicht zusätzlich nachgewiesen; der unveränderte gemeinsame Aktivierungsweg ist im Quellcode erhalten.

`before.json` verweist auf eine unveränderte native Vorgängeraufnahme mit drei tatsächlich gerasterten Chevronpixeln im isolierten Zwischenraum. Derselbe lokale Pixelcheck verwirft dieses Original und akzeptiert die neuen Ruhe-/Hoverbilder. Er ist kein Vollbildvergleich zwischen unterschiedlichen Pendelphasen.

Quellgraph, Fontdateien, Protokolle, Schritte, Ergebnisse und alle PNG-Hashes sind beigefügt; acht ausgewählte PNGs sind direkt gespeichert. Drei separat erhaltene fremde AT-SPI-Dateien liegen im lokalen Compile-Graphen. Die zwölf fremden Änderungen bleiben erhalten und sind nicht Teil dieses Commits. Der Nachweis ist deshalb kein Clean-Checkout-Nachweis.

Windows wird hier durch den Release-Build belegt. Aktuelle praktische Windows-Bedienung, macOS, Screenreader, physische DPI-/Monitorwechsel und die Gesamtabnahme bleiben offen. Die übrigen Designanforderungen und der Langzeit-Lückenerhalt sind weiterhin gesondert zu bearbeiten.

## Eingefrorenen Nachweis wiederholen

Aus dem Repository mit Python 3:

```text
python docs/visuals/unified-workspace/chart-captions-2026-10-24/check_frozen_captions.py
```

Nur Standardbibliothek und vorhandene Prüfmodule sind nötig. Der Prüfer startet keine Anwendung. Die endgültigen Gruppen-/Laufdaten stehen in `validation.json`, die ausführlichen Ergebnisse in `results.json.gz`.

## Ergebnis und weiterer Sichtbefund

Der Lauf `run-7_9voktn` besteht alle drei Gruppen. Die Hashkarte enthält 162
PNG-Dateien: 60 Messbereichbilder, 96 kontrollierte Kurvenbilder und sechs
zusätzliche Aufnahmen der Inspektorgruppe. Der Windows-Releasebuild ist erfolgreich.
Die eingefrorenen aktuellen und vorherigen Kurvennachweise bestehen ebenfalls.

Die tatsächlich geöffnete große helle Aufnahme
[`curve-system-large-long.png`](curve-system-large-long.png) zeigt außerhalb
der kontrollierten Tafel noch getrennte native Mängel: Inspektortitel wie
„Modellwerte“ und „Laufparameter“ werden mitten im Wort geteilt; die Konturen
von Schalter und Zeitleiste wirken verformt. Diese sind keine akzeptierten
Abweichungen oder nachgewiesenen technischen Grenzen. Ihre Ursache ist als
nächster Schritt zu prüfen. Die Komponententafel verdeckt einen Teil der Szene
und wird ausdrücklich nicht als normale Produktoberfläche ausgegeben.
