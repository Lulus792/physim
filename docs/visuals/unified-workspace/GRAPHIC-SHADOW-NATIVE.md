# Native Schatten und CSS-Maße des Exportdialogs

Der Berichtsdialog ergänzt die beiden freigegebenen Schatten:
`0 28px 100px #0007` unter dem Dialog und `0 8px 28px #0002` unter dem Papier.
Die Oberfläche verwendet dafür native Shader und keine zusätzlichen UI-Leisten.
Hoher Kontrast und reduzierte Transparenz schalten beide Effekte ab.

Der [CSS-Schattenvertrag](https://drafts.csswg.org/css-backgrounds-3/#shadow-blur)
beschreibt eine Gaußannäherung mit halbem Blur-Radius als Sigma und eine
Aussparung innerhalb des ursprünglichen Rahmens. Der native Pfad integriert
das Rechteck analytisch. Für die kleinen gerundeten Dialogecken korrigiert er
deren Fläche und Schwerpunkt. Dies ist für das tatsächlich verwendete Verhältnis
Radius/Sigma ≤ 0,3 begrenzt; die neue API behauptet keine beliebigen scharfen
Rundschatten. Das rechteckige Papier benötigt keine Eckkorrektur.

Die hergeleitete Schranke für die Eckannäherung liegt bei maximal 0,00463 einer
8-Bit-Farbstufe für Alpha 119. Sie gilt nur für die kontinuierliche Gaußfunktion;
Erf-Approximation, Float-Rundung, Antialiasing und abschließende Quantisierung
sind davon getrennt. Der tatsächliche GPU-Test vergleicht deshalb zusätzlich
510 Farbpunkte gegen eine unabhängige numerische Integration der gerundeten
Maske. Bei den ausgeführten Windows-/Linux-Kernläufen liegt der größte Fehler
unter 1,1 Farbstufen. Aussparung, kombinierte Schatten, spätere gewöhnliche
Geometrie und das Entfernen der Effekte werden ebenfalls geprüft.

## Maße und schmale Ansichten

Die Auswertung der tatsächlichen CSS-Quelle korrigiert den bislang fehlenden
1-px-Außenrahmen im Inhaltsmaß, die Seitenkante der Einstellungen, Zeilenboxen,
Abstände, Kopf-/Fußzeile und die rechtsbündige Größenangabe. Bei 1440 px Breite
und Standardschrift ergeben sich 1080 × 848,642 px Außenmaß sowie ein
737 × 522,042 px großer Papierbereich vor der Nuklear-Rasterung. Die Testfolge
prüft diese unabhängig aus der Referenz abgeleiteten Werte und erfasst Hell/Dunkel
auch bei derselben Fenstergröße 1440 × 1100 wie die freigegebenen Bilder.

Die Vorschaufläche füllt die gesamte linke Spalte und zentriert ihren Inhalt,
wenn die Einstellungen deren Höhe bestimmen. In schmalen Ansichten werden
Formatkarten und Felder gemäß den vorhandenen Breakpoints angeordnet. Bei
Platzmangel wechseln die Abschlussaktionen untereinander; bei der kleinsten
zweispaltigen Aktionsgruppe teilen sie die verfügbare Breite. Lange Dateinamen
bleiben vollständig umbrochen. Der auf 360 logische Pixel begrenzte schmale
Papierrahmen behält die CSS-Breite; die Grafik passt proportional hinein.

Der 480-px-Fall mit doppelter Dialoggeometrie verwendet gültige UI-Fontbänke.
Die Fixture senkt nur ihr eigenes Mindestfenster auf die für ausgehängte Panels
relevante Größe. Das beweist weder eine gewöhnliche Hauptfenster-Mindestbreite
von 480 px noch eine globale 32-px-Schriftoption oder physische 200-%-DPI.

## Aufnahmen und Grenzen

Neue Aufnahmen dieser Dialog-Testfolge entstehen direkt als verlustfreie PNGs.
Ein tatsächlich aus demselben Frame aufgenommenes BMP/PNG-Paar wird über sämtliche
RGB-Pixel verglichen. Die übrigen bestehenden Testabläufe behalten ihre BMP-Pfade.
Das verhindert weiteres unnötiges Rohbildwachstum, ohne alte Belege zu löschen.
Die zuvor abgelehnte Bereinigung wird damit nicht wiederholt oder umgangen.

Die finale Zuordnung von Quellständen, Ausführungen, Gegenbelegen und Bilddateien
steht im [Prüfmanifest](graphic-shadow-2026-10-22/validation.json).

| Tatsächlich ausgeführt | Ergebnis |
| --- | --- |
| Windows MSVC/ASan, Stand 5 | `graphic_dialog` 1/1, 45,312 s |
| Windows Release, Stand 5 | `graphic_dialog` 1/1, 9,547 s |
| Alpine-Dev GCC/ASan+UBSan, Stand 6 | `graphic_dialog` 1/1, 74,021 s |
| Unveränderter GPU-Kern, Stand 2 | `backdrop_ui` auf allen drei Konfigurationen bestanden; maximal 0,935494 bzw. 0,816959 Farbstufen Abweichung |
| Gemeinsamer Capture-/Szenenpfad, Stand 2 | `graphics` unter Windows und Linux jeweils 1/1 |

Die Produktions-C-Quellen von Stand 5/6 sind gleich. Stand 6 gleicht nur den
äußeren Python-Wächter an den bereits vorhandenen nativen 120-s-Auftrag samt
Startphase an (135 s vor Clipboard-Veröffentlichung, bestehende äußere 150-s-Grenze).
Der unveränderte GPU-Kern und die gemeinsamen Capture-Prüfungen behalten ihren
eigenen Quellstand; es wird kein erfundener gemeinsamer Gesamtlauf ausgewiesen.

Alle 24 verglichenen Exportdateien sind weiterhin bytegleich zum eingefrorenen
vorherigen Stand. Die drei gepaarten BMP-/PNG-Aufnahmen besitzen vollständig
gleiche RGB-Pixel. Für 28 übereinstimmende Aufnahme-Namen sinkt die tatsächlich
erzeugte Bildmenge von 138.000.664 BMP-Bytes auf rund 4,2–4,5 MB PNG. Ein BMP von
rund 5,4 MB bleibt zusätzlich als Paarbeleg; sechs neue Größenaufnahmen kommen
hinzu. Alte Rohdateien wurden dafür nicht gelöscht.

[Native Referenzgröße dunkel](graphic-shadow-2026-10-22/windows-release-graphic-dialog-reference-dark.png) ·
[hell](graphic-shadow-2026-10-22/windows-release-graphic-dialog-reference-light.png) ·
[480 px mit gestapelten Aktionen](graphic-shadow-2026-10-22/windows-debug-graphic-dialog-480-footer.png) ·
[begrenzter Papierrahmen](graphic-shadow-2026-10-22/linux-debug-graphic-dialog-680-default.png) ·
[zentrierte Vorschau bei 800 px](graphic-shadow-2026-10-22/linux-debug-graphic-dialog-800-centered.png).

Ein Windows-Debug-Lauf von Stand 2 besaß ab
Start keinen Tastaturfokus (`keyboard=0`) und wurde am unverändert strengen
Clipboard-Zeitlimit beendet. Linux und Windows Release bestanden denselben
Quellstand; auch die unveränderte Debug-Wiederholung bestand. Der fehlgeschlagene
Lauf bleibt als Gegenbeleg erhalten und wird nicht als Erfolg gezählt.

Stand 4 enthielt einen falschen Pixel-Prüfwert `(18,19,22)`. Die originale CSS-
Quelle und die tatsächlich ausgelesenen Referenzpixel zeigen `(20,21,24)`.
Dieser Gegenbeleg bleibt erhalten; der strenge Prüfwert wurde auf die belegte
Referenz korrigiert. Eine echte separate Abweichung waren die zu dunklen hellen
Ränder: Stand 5 übernimmt `(180,188,200)` und zeichnet die geraden Feld-/Kartenränder
als einpixelige gefüllte Konturen. Eine neue native Pixelprobe prüft diese Farbe.
Ein Linux-Lauf von Stand 5 erreichte mit gültigem Tastaturfokus erst bei rund
65 s den Zustand 214; der frühere äußere Wächter beendete ihn vor der Publikation.
Dieser Lauf zählt als fehlgeschlagen. Stand 6 schließt die gesamte Folge mit
allen unveränderten Fokus-, Zustands- und Pixelbedingungen tatsächlich ab.

Die gesamte Oberfläche ist weiterhin nicht pixelidentisch abgenommen.
Der aktuelle Nuklear-UI-Pfad quantisiert mehrere Zeichenkoordinaten vor dem
GPU-Rendern auf ganze Pixel. Beim direkten Vergleich in Referenzgröße sind
dadurch unter anderem vertikale Kanten um einen Pixel versetzt. Das ist eine
konkrete verbleibende Renderingarbeit, keine behauptete Unmöglichkeit nativer UI.
Schriftfamilie/-gewicht, weitere Komponenten und kontinuierliche Scrollzustände
bleiben gegen die Referenzen abzugleichen. Browser- und native Rasterung werden
nicht allein aus den bestandenen Funktionsgruppen gleichgesetzt.

Der folgende [sichtbare Mehrlaufvergleich](ANALYSIS-COMPARISON-NATIVE.md) bindet
den Cache an Kurve, Referenz, Legende, vollständige Statistik und Export.
Externe Screenreader, echte
OS-Dateiauswahlbedienung, physische DPI-/Monitorwechsel, ausgehängter Quellbereich
und macOS bleiben gesonderte offene Abnahmen.
