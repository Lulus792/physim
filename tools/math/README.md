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
User Guide, Pendel-, Wurf-, Widerstands-, Material-, Feder- und Stoßlernpfad
verwenden diesen Formelsatz. `documentation_formula_catalog` zeichnet jeden
veröffentlichten Displayblock dieser Seiten und prüft Formelbild und native
Lesereihenfolge. Thermodynamik, Elektromagnetismus, Wellen/Optik und Strömung
werden zusätzlich durch `documentation_formula_physics` geprüft.
Reale Gase, Messunsicherheit und der Monte-Carlo-Lernweg verwenden ebenfalls
registrierte Displayformeln. `documentation_formula_models` prüft alle ihre
Formelblöcke bei 16 px; `documentation_formula_models_narrow` verlangt dieselben
Blöcke vollständig sichtbar bei 32 px im 760 × 540-Fenster. Mechanik, Auftrieb
und Numerik werden entsprechend durch `documentation_formula_mechanics` und
`documentation_formula_mechanics_narrow` geprüft. Der Katalog umfasst
171 Ausdrücke in 177 veröffentlichten Blöcken auf 17 Seiten.
`documentation_formula_narrow` verlangt vollständig sichtbare Gitter-/
Energieformeln im 760 × 540-Fenster bei 32 px. Die diskrete Saitenenergie
ist dafür in gleichwertige, einzeln erklärte Terme aufgeteilt. Weitere
mathematische Fachseiten müssen noch einzeln übertragen werden.

`documentation_math` inventarisiert sowohl registrierte Quelldokumente als auch
die linearen Lesealternativen des Formelkatalogs. TeX-Quellzeichen allein decken
die tatsächlich gezeichneten nativen Lesetexte nicht ab. Ein sichtbarer y-Index
bleibt im TeX gesetzt; die lineare Fassung verwendet bei Bedarf `_y`, anstatt
ein ähnlich aussehendes griechisches tiefgestelltes Gamma als y auszugeben.

Der Generator lädt die TeX-Pakete `base`, `ams` und `boldsymbol`; damit sind
auch fette griechische Vektoren möglich. SVGs werden mit `serializeXML` erzeugt,
damit legale TeX-Vergleiche wie `<` in Metadaten XML-konform maskiert sind.
`check.py` prüft zusätzlich die XML-Struktur aller SVG-Originale.

Quellen: [MathJax Node-Schnittstelle](https://docs.mathjax.org/en/v4.1/server/direct.html),
[Herkunft und Lizenzen](../../docs/formulas/NOTICE.txt).
