# F1-Formelsatz

Die F1-Hilfe zeigt registrierte Displayformeln als gesetzte Mathematik. TeX steht
zwischen zwei eigenen `$$`-Zeilen. Der Inhalt muss exakt einem `tex`-Eintrag in
`docs/formulas/sources.json` entsprechen. `reading` enthält die lineare Unicode-
Alternative für Suche, Kopieren und die native Dokumentlesereihenfolge.

Formeln hinzufügen und erzeugen:

```powershell
cd tools/math
npm ci
npm run generate
cd ../..
python tools/math/check.py
```

Benötigt Node ab 22.15. Die Versionen sind im Lockfile festgelegt. Der Generator
verwendet MathJax und dessen NewCM-Zeichendaten; das laufende Programm benötigt
keinen Browser, Netzwerkzugriff, Node oder eine TeX-Installation.

Für sieben Containerbreiten entstehen SVG-Originale und komprimierte Alphadaten
mit 128 Pixeln pro em. Identische Raster werden anhand ihres Inhalts geteilt.
Der Viewer wählt eine passende Umbruchvariante und reduziert die Flächendeckung
für Schriftgröße und Pixeldichte. Die Textfarbe folgt dem aktuellen Theme.
Der Cache gehört zum Grafik-Kontext des F1-Fensters und wird dort freigegeben.

Quellkatalog, Manifest, SVGs, `.math`-Dateien und den erzeugten nativen Katalog
gemeinsam versionieren. `check.py` prüft Quell-/Asset-/SVG-Hashes, zlib-Abschluss,
Abmessungen, CRC, sichtbare Deckung, den nativen Katalog und alle veröffentlichten
Displayblöcke. Die GUI-Prüfung `documentation_typeset` kontrolliert echte Bilder,
Clipping und native Lesebereiche bei 16/32 px und drei Themes.

Unbekanntes TeX wird als Quelltext gezeigt. Ein nicht geschlossenes Display wird
als ungültiges Dokument abgewiesen. Codeblöcke bleiben wörtlich erhalten.
Inline-TeX ist noch nicht implementiert. Die lineare Lesealternative ersetzt
keine strukturierte mathematische Screenreader-Navigation; diese bleibt offen.
Die Umsetzung wird zunächst am User Guide und Pendel-Lernpfad geprüft. Weitere
mathematische Dokumentationsseiten müssen noch einzeln übertragen werden.

Quellen: [MathJax Node-Schnittstelle](https://docs.mathjax.org/en/v4.1/server/direct.html),
[Herkunft und Lizenzen](../../docs/formulas/NOTICE.txt).
