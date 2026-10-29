# Verbindliche Markierung: A2 · Dezenter Schleier

**Nutzerfreigabe: 29. Oktober 2026.** Teil des fertigen Physim-Designs, ersetzt
A · Sanfte Fläche. [ADR 016](../../decisions/016-selected-marking-a2.md).
Im bestehenden nativen Auftrag mit umsetzen; keine erneute Designentscheidung.

[Freigegebene interaktive Referenz](index.html?approved=1&context=code) ·
[Text dunkel](review/a2/text-a2-dark.png) · [Text hell](review/a2/text-a2-light.png)

## Genaue Farbrollen

| Rolle | Dunkel | Hell |
| --- | --- | --- |
| Auswahlfläche für Text, Zeilen und Szenenobjekte | `rgba(170,184,202,0.15)` | `rgba(90,108,130,0.11)` |
| Fläche eines gezogenen Auswahlrahmens | `rgba(170,184,202,0.06)` | `rgba(90,108,130,0.045)` |
| Zeilenakzent, Häkchen, Objektkontur und Griffe | `#abb9ca` | `#65788d` |
| Aktiver / gehoverter Menüpunkt | Dieselbe Auswahlfläche | Dieselbe Auswahlfläche |
| Schrift in der Auswahl | Ursprüngliche Text-/Syntax-/Statusfarbe, volle Deckkraft | Ursprüngliche Text-/Syntax-/Statusfarbe, volle Deckkraft |

Die Hintergrundflächen bleiben die gemeinsamen Werkbankrollen. Die oben
genannten Alpha-Werte auf die jeweilige Fläche komponieren, dann die Zeichen
darüber zeichnen. Keine zusätzliche Deckkraft auf Text, ganze Zeilen, Elternpanels
oder fertige Glyphen anwenden. Kein deckender weißer/blauer Textwechsel wie in C,
keine Unterstreichung wie in B. Die A2-Rollen aus `selection.css` sind maßgeblich.

## Darstellung nach Inhalt

- **Text / Code / Originaltext:** lückenloser ausgewählter Zeichenbereich,
  einschließlich mehrerer Zeilen. Syntaxfarben erhalten; Umbruch und Kopie
  verwenden denselben Originaltext. Zeilennummern gehören nicht zur Auswahl.
- **Protokoll / Explorer / andere auswählbare Listen:** transparente Fläche,
  3-px-Akzent links und Häkchen. Fehler-/Warnfarben, vollständige Namen und
  Metadaten erhalten. Die gewählte Menge bleibt nach Menüöffnung erkennbar.
- **Szenenobjekte:** dünne Kontur und vier kleine Eckgriffe wie in A2,
  schwache Fläche hinter Objekt und Beschriftung. Beschriftung vollständig
  einschließen. Griffe belegen keine vorhandene Transformationsfunktion;
  solche Werkzeuge brauchen einen eigenen Fachvertrag.
- **Tastaturfokus:** eigene gestrichelte Fokusmarkierung in der gemeinsamen
  Akzentfarbe; Fokus nicht mit ausgewählter Menge gleichsetzen.
- **Hoher Kontrast:** System-/gemeinsame Hochkontrastrollen, erkennbare Konturen
  und Häkchen verwenden. Lesbarkeit hat Vorrang vor transparenter Farbtreue.

## Verbindliche Bedienung

1. Linksklick / Ziehen markiert, ohne schwebende Leiste oder automatisch geöffnetes
   Aktionsmenü. Rechtsklick, Menütaste oder Umschalt+F10 öffnen das Kontextmenü.
2. Rechtsklick auf ausgewählte Elemente erhält die Mehrfachauswahl. Rechtsklick
   auf ein anderes Element wählt zunächst dieses Ziel. Textauswahl beim Öffnen
   nicht verlieren oder durch einen anderen Bereich ersetzen.
3. Strg/⌘ ergänzt einzelne Listenelemente, Umschalt einen Bereich. Szenenrahmen
   und ergänzende Auswahl folgen den Regeln der [Referenz](README.md).
4. Rechtsklick ohne Ziehen und vorhandene rechte Kamera-Ziehbewegung in Simulieren
   unterscheiden. Nach Kamerabewegung kein Kontextmenü öffnen. Bestehende
   Kamera-/Tastatursteuerungen erhalten und diese Eingabekollision nativ prüfen.
5. Menü mit Pfeilen, Pos1/Ende, Enter bedienen. Escape schließt nur das Menü,
   erhält die Auswahl und gibt Fokus zurück. Klick außerhalb, tatsächlicher
   Seiten-/Bereichsscroll, Resize oder Fokusverlust schließen ohne Aktion.
6. Menüziele beim Öffnen festhalten. Bei Projektwechsel, entfernten Objekten oder
   veralteten Daten Menü verwerfen. Abgebrochenes Ziehen stellt die vorige
   Auswahl wieder her. Kein Befehl darf auf ein nachträglich anderes Ziel wirken.
7. Nur tatsächlich verfügbare Aktionen anbieten. Die HTML-Nur-Lese-Beispiele
   sind kein Grund, echte Editorbefehle zu entfernen oder neue Datei-/Modell-
   Funktionen vorzutäuschen. Native Berechtigungen und Fehlerwege weiterverwenden.

Der sichtbare Grafikexport-Zugang und der freigegebene Exportdialog bleiben
erhalten. Ein zusätzlicher Kontextbefehl verwendet denselben Exportablauf.
Variantenkarten, Freigabehinweise und Demonstrationsbuttons gehören nicht in die App.

## Umsetzung und Abnahme

Auswahl-, Text-, Listen- und Szenenrenderer am aktuellen nativen Stand abgleichen.
Die Browservorlage als Gestaltung verwenden, keine JavaScript-/WebView-Integration.
Vorhandene Clipboard-, UTF-8-, Daten-, Rechte- und Accessibility-Verträge erhalten.

Nativ prüfen: beide Themes und hoher Kontrast, lange und mehrzeilige Texte,
verschiedene Syntax-/Statusfarben, Kopie ohne Zusatzzeichen, Mehrfachauswahl,
Rechtsklick während bestehender Auswahl, Tastaturmenü und Fokuskehr,
Szenenpicking vs. Kameradrag, Abbruch, verschwundene Ziele, kleine Fenster und
große Schrift. Plattformen, Programme und tatsächlich ausgeführte Tests angeben.

Die vorhandenen [64 Browser-Layouts](review/a2/validation.json) und A2-Bilder
belegen den gewählten Entwurf. Sie bestätigen keine bereits fertige native
Auswahl- oder Plattformabnahme. Der [Gesamtauftrag](../unified-workspace/NEXT-CHAT.md)
ist für den umsetzenden Chat aktualisiert.
