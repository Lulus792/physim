# Explorer A: Dateizeilen, Kacheln und Metadaten

Stand: 30. Oktober 2026. Der Teilstand gleicht die Listen-/Kachelgeometrie und
mehrere Schriftrollen mit der [freigegebenen A-Vorlage](../design-options/APPROVED.md)
ab. Die [unabhängigen Aufgabenfenster](TASK-WINDOWS-NATIVE.md#datei-ordnerwahl-a)
bleiben erhalten. Dies ist keine vollständige Abnahme des Explorers oder der
gesamten Oberfläche.

## Übernommene Maße

Dateinamen behalten die ausdrücklich festgelegte Zeilenhöhe 1,6. Kleine
Spaltentexte und der Tabellenkopf erben dagegen 1,55. Die normale Zeile ergibt
sich aus dem größeren Wert von 18-px-Symbol und vollständigem Dateinamen,
Innenabstand, Kontur und 2-px-Außenabstand: bei einem einzeiligen Namen 44 px
Zeilenabstand. Der zusätzliche seitliche 8-px-Einzug entfällt. Die schmale Zeile
verwendet die eigene Symbolspalte, den 9-px-Zeilenabstand und die darunterliegende
Artangabe gemäß CSS.

Der Tabellenkopf bleibt auch vor dem Kachelraster sichtbar; erst der kompakte
Umbruch blendet ihn aus. Kacheln verwenden 12 px Rasterrand und 9 px Abstand,
kompakt 8 und 6 px. Kontur, Innenabstand, 35-px-Symbol mit 8 px Unterabstand,
beide Inhaltsabstände, sämtliche Dateinamenzeilen und die Artangabe bestimmen
die Höhe. Die Mindesthöhe beträgt 150 px; der bisherige Zuschlag „Dateiname +
85 px“ entfällt. Alle Karten einer Rasterzeile berücksichtigen deren längsten
Namen. Dateiname und Art werden zeilenweise zentriert.

Beschriftung und Wert der Vorschau-Eigenschaften verwenden beide 0,8 rem und
Zeilenhöhe 1,55. Der Browserfuß verwendet 0,73 rem einschließlich Checkbox und
Ordneraktion; diese erben auch die gedämpfte Textfarbe. Der Ort im Dialogfuß
verwendet 0,75 rem mit 1,55. Messung und Zeichnen verwenden jeweils dieselbe
Rolle, damit lange Orte weder Aktionen überlagern noch einen falschen Scrollweg
erzeugen. Die nichtmonospazierten API-Texte übernehmen ihre jeweiligen Rollen
und 1,55; die Signatur behält unverändert 0,72 rem mit 1,85.

## Überschriften und Vorschauhinweise

Der folgende [Typografienachweis](../../verification/explorer-typography-2026-10-30.json)
verwendet für den Explorerkopf und „ORTE“ getrennte persistente Schriftrollen.
Beide übernehmen 0,7 rem und den vorhandenen kräftigen nativen Schriftschnitt;
der Kopf erhält 1,7 px zusätzliche Laufweite, „ORTE“ 1 px. Breitenmessung und
Glyphenausgabe verwenden dieselben Werte, einschließlich Präferenzskalierung.
Die Typ-Badges behalten ihre normale, ungesperrte Schrift. Im kompakten Layout
bleibt die Ortsüberschrift gemäß Vorlage ausgeblendet.

Die Hinweise vor einer Auswahl, bei fehlender oder nicht darstellbarer
Textvorschau und bei gekürzten Dateien verwenden 0,78 rem mit der geerbten
Zeilenhöhe 1,55. Der Kürzungshinweis erhält seine vollständig umbrochene Höhe
statt eines festen Zuschlags. Dadurch berechnen Vorschau und nachfolgender
API-Bereich denselben Platz auch bei schmaler Darstellung und großer Schrift.

Der lokale Windows-Adapter lädt Segoe UI Semibold für den kräftigen Schnitt.
Der Linux-Adapter verwendet seine vorhandene System-/Offline-Schriftpolitik;
daraus folgt keine Zusage identischer plattformübergreifender Glyphenpixel.
Rasterung und installierter Schriftschnitt bleiben getrennt vom geprüften
CSS-Abgleich der Größen und Abstände. Ein aktueller macOS-Nachweis fehlt.

## Aussagekraft der Prüfungen

Der [kompakte Nachweis](../../verification/explorer-layout-2026-10-30.json)
ordnet Quellstände, reale Kommandos, Plattformen, Fehlversuche und Gegenproben
zu. Die vorhandenen Bediengruppen behalten Dateiöffnung, getrennte Fenster,
Schriftwechsel, freie Dateienansicht, Vorschau, Signaturkopie und tatsächlichen
F1-Sprung. Die neue Prüfung liest die gezeichneten Textbefehle vor deren Freigabe
und leitet ihre Erwartungen aus den freigegebenen CSS-Regeln ab. Sie prüft
auch vollständige lange Namen, gemessene Schriftrollen, Kartenhöhen, Vorschau
und Fußtexte. Die Vorschau wird mit echten eingereihten Tab-Ereignissen erst
sichtbar gemacht; außerhalb des Ausschnitts liegender Text wird nicht als
angeblich sichtbarer Inhalt gewertet.

Gezielte native Rasterbilder ergänzen die Text-/Geometrieprüfung. Die Pixelprüfung
verwendet tatsächlich sichtbare Dateinamenzeilen. Eine feste Auswahl nach Wörtern
wie „assets“ wäre falsch, wenn diese bei anderer Schrift oder längerem Pfad
außerhalb des Bildes liegen. Die Gegenvariante zeichnet ausschließlich die
Kacheltexte wieder linksbündig; derselbe Rasterprüfer muss diese echte native
Ausgabe ablehnen. Bilder werden nicht für jeden Tastenschritt neu gespeichert.

Die Prüflogik und Fixtures liegen versioniert unter
`tests/check_explorer_layout.py` und `app/file_browser_layout_tests.inc`.
Mit eingerichteter Windows-C17-Toolchain und der lokalen SDL-Installation lässt
sich die Gruppe aus einem sauberen Checkout beispielsweise so reproduzieren:

```powershell
python -X utf8 -B tools/with-artifacts.py -- python -X utf8 -B tools/build.py --config Release --build-dir build/explorer-layout-release --sdl third_party/SDL3-3.2.30 --test-display --test-filter explorer_a_layout
```

Die tatsächlich ausgeführten isolierten Windows-/Linux-Kommandos und Quellhashes
sind im Nachweis getrennt aufgeführt. Der Linux-Lauf verwendet Xvfb/Openbox und
ASan/UBSan; GUI-Leakprüfung war deaktiviert. Eine frische Ausführung darf andere
Pfadumbrüche und Bildhashes erzeugen und muss dieselben CSS-/Inhaltsprüfungen bestehen.

## Verbleibende Grenzen

Der Dateizeilen-/Kachelabgleich ersetzt weder A2 noch die vollständige visuelle
Matrix. Die Kopf-/Orte-Laufweiten und Vorschauhinweise sind im folgenden
Typografienachweis geprüft; weitere Leer-/Fehlerzustände und die vollständige
Schrift-/Plattformmatrix bleiben abzugleichen. Der genaue vertikale Abstand
zwischen Ortsüberschrift und Navigation sowie weitere Toolbarrollen gehören
zum folgenden Geometrieabgleich. Der funktionale API-Nachweis und die betrachteten API-Bilder
beweisen noch keinen vollständigen unabhängigen Prosa-/Tabellentypografieabgleich.
Messlauf-/Bildvorschau, sehr große Listen und praktische Gesamtaufgaben bleiben
ebenfalls im bestehenden Auftrag.

Die native Betriebssystem-Titelleiste ist gemäß ADR 015 der zusätzliche äußere
Rahmen. Rasterbilder zeigen die Clientfläche und den jeweiligen Scrollzustand.
Die Prüfungen verwenden injizierte SDL-Ereignisse; physische Eingabe, weitere
Monitor-/DPI-Zustände, echte Screenreader und macOS benötigen eigene Nachweise.
