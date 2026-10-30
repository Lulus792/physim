# Physim-Website

`index.html` ist der versionierte Quellstand der öffentlich bereitgestellten
statischen Website. Die Seite benötigt keinen Build. Lokal lässt sie sich z. B.
mit `python -m http.server 8000 --directory website` ansehen.

## Versionen und Veröffentlichungen

Am 30. Oktober 2026 existiert im GitHub-Repository kein veröffentlichtes Release
und kein Git-Tag. Die Galerie und der Einstieg beschreiben daher ausdrücklich den
Entwicklungsstand; die Pendelvorschau ist eine analytische Browserberechnung.
Sie ist kein Lauf der Physim-App. Links auf Dokumentation, Quellen und Downloads
dürfen erst auf einen tatsächlich veröffentlichten stabilen Release-Tag zeigen.
Nach einer Veröffentlichung alle vier Verweise (Download, User Guide, Beispiele,
API) auf denselben Release-Stand ergänzen und Inhalte gegen diesen Stand prüfen.
Der Link zu GitHub-Releases zeigt den tatsächlichen Releasebestand; er wird
nicht automatisch als Nachweis für ein vorhandenes stabiles Release gewertet.

Bei Änderungen `website/index.html` und den Inhalt von `dist/index.html` im
zugehörigen Sites-Quellrepository identisch halten. Anschließend die Sites-Version
bauen, veröffentlichen und die öffentliche Erreichbarkeit prüfen. Zugangsdaten
und generierte Archive gehören nicht ins Physim-Repository.

## Recherche und Herkunft

Die Trennung von Einstieg, Referenz und Version orientiert sich an den
[NumPy User Guide](https://numpy.org/doc/stable/user/) und der
[SciPy-Dokumentation](https://docs.scipy.org/doc/scipy/index.html); die
frageorientierte Beispielauswahl ist von der
[Matplotlib-Galerie](https://matplotlib.org/stable/gallery/index.html) angeregt.
Die Physim-Inhalte wurden mit `docs/examples.md`, `docs/user-guide.md` und dem
Projektstand abgeglichen. Die Grafiken sind eigene schematische SVG-Darstellungen.
