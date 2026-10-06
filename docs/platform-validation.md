# Plattformprüfung

Stand: 6. Oktober 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

## Messkanal-Anzeigeeinheiten am 6. Oktober 2026

Der neue private Katalog besteht auf macOS Debug:
`build/channel-units-debug-mac/test-results/run-baav6ek6`.
Geprüft sind dimensionsabhängige Schlüssel, lineare Konvertierung, signierte Null,
subnormale Faktoren, Über-/Unterlauf ohne Änderung der Ausgabe, 64 Einträge,
Ersetzen und Entfernen, UTF-8, Rundlauf, alle Abschneidepositionen und byteweise
Beschädigungen eines Zwei-Eintrag-Katalogs, CRC-gültige Duplikate, unbekannte
Versionen und fehlgeschlagene Dateiersetzung mit Erhalt des Originals.

Der gezielte Fensterablauf besteht auf macOS Debug:
`build/channel-units-debug-mac/test-results/run-kg4b4qqc`.
C- und Physim-Projekte bauen und laufen in der App. Echte Texteingaben und
Mausereignisse prüfen eigene Symbole/Faktoren, Ablehnung von null,
Live-Konvertierung, Statistik, SI-Rücksetzung und Abbrechen. F7 löst im Dialog
keinen Reset aus; der Runner läuft während der Auswahl weiter. Sensorlücken
bleiben Lücken. Ein Faktor mit Überlauf vorhandener Werte wird abgelehnt;
der ausgewählte Diagrammausschnitt bleibt bei SI-Rücksetzung erhalten.
Weitere Prozesse laden die Auswahl und prüfen die Erhaltung beschädigter Dateien
bis zum ausdrücklichen Reset. Hashes von Quellen und Laufdateien bleiben erhalten.

Die vollständigen Release-Läufe ohne Fenster bestehen auf beiden Systemen 515/515:
macOS `build/channel-units-mac/test-results/run-vkw6s26a`, Debian
`build/channel-units-linux/test-results/run-gb6eh__b` innerhalb der VM.
Die Umgebungen bleiben Intel macOS 14.6.1/AppleClang 16 und Debian 12/GCC 12.2/
SDL 3.2.30. API/ABI 3, Sprache 0.171.0, Pipe-Version 4 und Messdatei-Format 1
bleiben unverändert. Der neue persönliche Katalog verwendet Format 1.

Die finale Katalogprüfung nach Ergänzung unveränderter Schema-Namen (auch mit
Rand-Leerzeichen oder leerem Namen) besteht jeweils 1/1: macOS
`build/channel-units-mac/test-results/run-j3bl9vjk`, Debian
`build/channel-units-linux/test-results/run-bmmz2o54`.

Der erste vollständige macOS-Fensterlauf besteht 50/51:
`build/channel-units-mac/test-results/run-t7zmqdg3`. Der bisherige
Autosave-Verwerfentest beobachtet die Oberfläche zu früh; er lässt jetzt einen
vollständigen Renderframe zwischen Mausereignis und Prüfung. Unveränderte
Wiederherstellungs-/Verwerfungsassertionen und der Einheitenablauf bestehen danach
2/2: `build/channel-units-mac/test-results/run-tk8u6egn`.
Die frühe Statistik-Detailaufnahme hatte zudem eine falsche Nuklear-Signatur im
Testcode; der erkannte Buildfehler ist korrigiert.

Unter Debian verliert die VM zunächst SSH; ihre Konsole dokumentiert Kernel-RCU-Stalls.
Nach Neustart bestehen 46 Fensterfälle in
`build/channel-units-linux/test-results/run-sgmfabef`, bevor der VM-Speicherplatz
erschöpft ist. Ältere generierte Arbeitsdateien unter `build/native` werden
entfernt, die strukturierten Ergebnisberichte bleiben erhalten. Die fünf noch
nicht belegten Fälle und der korrigierte Autosave-Ablauf bestehen anschließend
6/6: `build/channel-units-linux/test-results/run-386kps2s`.
Diese Teilnachweise sind kein ununterbrochener 51/51-Komplettlauf. Über die
abgeschlossenen Läufe sind alle 51 Fensterfälle auf beiden Systemen erfolgreich
belegt; `build/channel-units-proof-mac/coverage.json` und innerhalb der VM
`build/channel-units-proof-linux/coverage.json` prüfen jeden Fall gegen den Katalog.
Frühere Fehlschläge beziehungsweise unvollständige Datenträgerberichte bleiben vermerkt.

Visuelle Prüfung bei 1080 × 740 zeigt zu enge Statistikspalten. Die Tabelle erhält
horizontalen/vertikalen Bildlauf, ausreichend breite Spalten und explizite
Abkürzungen mit vollständigem Tooltip. Die Vorschau verwendet ein lesbares
Gleichheitszeichen. Finale Einheitenansicht: macOS 1/1
`build/channel-units-mac/test-results/run-x7q06_28`; Plot-/Eingabeprüfung macOS
2/2 `build/channel-units-mac/test-results/run-e4_1g9ik`. Debian besteht die drei
Fälle 3/3: `build/channel-units-linux/test-results/run-m6hwh4os`.
Dialog und Statistik wurden auf beiden Systemen visuell geprüft.

Die installierte Release-App baut C-/Physim-Projekte auch nach dem Verschieben
in SDK-Pfade mit Leerzeichen und Umlaut und besteht denselben Einheitenablauf
mit Wiederöffnung, Dateischutz und ausdrücklichem Reset: macOS
`build/channel-units-sdk-proof-mac-final/app-steps.json`, Debian
`build/channel-units-sdk-proof-linux/app-steps.json` innerhalb der VM.
Die SDK-Prüfung betrifft hier die installierte App und deren Projektbuild;
der öffentliche Kern ist seit dem PCHIP-Nachweis unverändert. Die 19 erzeugten
Referenzdokumente bestehen weiterhin den Abgleich.

## Monotone kubische Interpolation am 5. Oktober 2026

Resampling, PCHIP-Modell und ausgeführte Physim-Analysemodule bestehen auf
macOS Debug 3/3: `build/pchip-debug-mac/test-results/run-ehr4gt2h`.
Die erste Debian-Release-Prüfung besteht ebenfalls 3/3:
`build/pchip-linux/test-results/run-fms8h68d` innerhalb der VM.
Analytische Referenzen prüfen gleichmäßige und unregelmäßige Raster,
Einheitenumrechnung, exakte Stützstellen, erste Ableitungen an inneren Knoten,
Monotonie, Plateaus, Extrema und mehrere Blockgrenzen. Endliche Extremwerte,
subnormale Achsenabstände, zwei/ein Quellpunkt, Scratch-Quota und Fehler nach
mehreren Zielblöcken sind geprüft. Ein Fehler hinter dem verwendeten Quellpräfix
wird erkannt. Ergebnisreihen bleiben nach dem Schließen der Quelle lesbar und
werden durch das Schließen des Ziel-Dataset invalidiert.

Das neue Verfahren verwendet lokale Fritsch–Butland-Steigungen mit
begrenzten einseitigen Endsteigungen. Mantissen-/Exponentenarithmetik vermeidet
die Voraussetzung eines breiteren `long double`; Bézier-Kontrollen und
de-Casteljau-Auswertung halten Ergebnisse im jeweiligen Werteintervall.
Methodische Grundlage ist die [PCHIP-Dokumentation von SciPy](https://docs.scipy.org/doc/scipy/reference/generated/scipy.interpolate.PchipInterpolator.html).
Die Implementierung ist in C geschrieben und benötigt SciPy nicht zur Laufzeit.

Der neue Fensterablauf besteht auf macOS Debug:
`build/pchip-debug-mac/test-results/run-iiiwjoeb`.
Ein C- und ein Physim-Projekt bauen ihre Analyse in der App, führen das Modell
aus und erzeugen je einen Bericht mit 129 PCHIP- und linearen Werten.
Numerische Zwischenwerte, Monotonie, Einheiten und Endpunkte sind geprüft.
Die SVGs beider Sprachen sind bytegleich; PNG misst jeweils 1200 × 850 Pixel.
Weitere Prozesse öffnen dieselben Berichte; deren SVG bleibt identisch und
Hashes von Quellen, Bericht und Laufdaten bleiben erhalten. Die Kurvenansicht
wurde bei 1080 × 740 logischen Pixeln auf dem Intel-Mac visuell geprüft.
Frühe Fixtures verwenden zunächst nicht unterstützte freie/static Series-Aufrufe
und falsche App-Hilfsfunktionssignaturen; sie verwenden jetzt die vorhandenen
Methoden und Berichtslader. Ihre Zeitgrenzen bleiben unverändert.

Sprache 0.171.0 ergänzt `Series.resampledPchip`; API und ABI bleiben auf 3.
Die privaten numerischen Helfer werden mit den SDK-Quellen ausgeliefert.
Ein unabhängiger öffentlicher SDK-Verbraucher prüft PCHIP gegen das installierte
Archiv und den aus SDK-Quellen neu gebauten Kern.
Die vollständigen Release-Läufe ohne Fenster bestehen jeweils 514/514:
macOS `build/pchip-final-mac/test-results/run-_wqs83qb`, Debian
`build/pchip-linux/test-results/run-27oiqc36` innerhalb der VM. Die Umgebungen
bleiben Intel macOS 14.6.1/AppleClang 16 und Debian 12/GCC 12.2/SDL 3.2.30.
Die vollständigen Release-Fensterläufe bestehen jeweils 50/50: macOS
`build/pchip-final-mac/test-results/run-h2yktdf9`, Debian
`build/pchip-linux/test-results/run-oo9993th` unter X11/Xvfb/Openbox/Mesa.
Auch die Linux-Kurvenansicht wurde bei 1080 × 740 visuell geprüft.
Die verschobenen Release-SDKs bestehen mit unabhängigen Headern und Verbrauchern,
installiertem und aus SDK-Quellen neu gebautem Kern, C-/Physim-Modulen,
adaptiven Läufen, Zielzeitstudien und neun neu gebauten Projekten:
macOS `build/pchip-sdk-proof-mac/Native SDK ä uftgb797`, Debian
`build/pchip-sdk-proof-linux/Native SDK ä knmhmus1` innerhalb der VM.
Der öffentliche Reihenverbraucher prüft PCHIP-Werte und Zielzuordnung mit beiden
Archiven; dadurch ist auch die Auslieferung des privaten numerischen Headers geprüft.

## Benannte Workspaces am 5. Oktober 2026

Workspace-Zustand und neuer Katalog bestehen auf macOS Debug 2/2:
`build/workspaces-debug-mac/test-results/run-a_g1mn_j`. Der Katalog prüft UTF-8,
Ersetzen gleicher Namen, acht Einträge, Löschen, doppelte Namen, verschachtelte
Zustände, alle Abschneidepositionen und byteweisen Beschädigungen, unbekannte
Versionen und fehlgeschlagene Dateiersetzung. Acht Zustände mit jeweils
32 zusätzlichen Pfaden und 16 Dokumenten an der Pfadlängengrenze bestehen im
Roundtrip. Zustände und Transaktionskopien liegen auf dem Heap. Die gemeinsame
Kodierung erhält die vorhandenen Workspace-Formate 1 und 2.

Neuer Katalogablauf, bisherige Workspace-Wiederöffnung und kleines Dokumentfenster
bestehen unter macOS Debug 3/3:
`build/workspaces-debug-mac/test-results/run-fgj6sp7s`; unter Debian Release
ebenfalls 3/3: `build/workspaces-linux/test-results/run-285x2ixz` innerhalb der VM.
Ein C-Projekt speichert seinen Workspace während einer laufenden Simulation;
der Runner und der ungespeicherte Quelltext bleiben erhalten. Öffnen und F7
sind dabei gesperrt. Ein Physim-Projekt wird anschließend als zweiter Workspace
gespeichert. Eine extern geänderte Dokumentdatei verhindert den Wechsel;
eine fehlende Analysequelle im Zielprojekt erhält den bestehenden Workspace.
Nach Auflösen des Konflikts stellt der erste Eintrag Dokumente und Editoransichten
wieder her, ohne Build oder Runner zu starten. Ersetzen und Löschen halten die
erwartete Eintragszahl. Neue Prozesse prüfen bewusste Wiederöffnung, gekürzten
Unicode-Text, Erhalt beschädigter Kataloge und ausdrückliches Zurücksetzen.
Hashes von C-/Physim-Quellen und Laufdatei bleiben unverändert.
Die erweiterten Grenzmodelle bestehen unter Debian Release 2/2:
`build/workspaces-linux/test-results/run-cpighmxl`.

Der erste macOS-Fenstergesamtlauf besteht 48/49:
`build/workspaces-final-mac/test-results/run-pe0vn8sk`. Der bestehende
`workspace_workflow` versucht einen Ordnerwechsel unmittelbar nach dem Projektladen,
während der Bibliotheksleser noch arbeitet. Die App blockiert diesen Wechsel korrekt.
Der Prüfer wartet jetzt auf den Bibliotheksleser; seine Zeitgrenze und alle
Zustandsprüfungen bleiben erhalten. Korrigierter Workspace-Ablauf und neuer
Katalogablauf bestehen unter macOS Debug 2/2:
`build/workspaces-debug-mac/test-results/run-cy86lc6v`.

Frühe Builds und Prüfungen erkennen eine Namenskollision mit dem Dateibaumtyp,
nicht bereinigte Pfadreste im Grenztest und den
Neustartklick vor dem ersten Renderframe. Diese Fehler sind korrigiert; die Zeitgrenzen bleiben
erhalten. Die Sichtprüfung bei 1080 × 740 erkennt eine zu lange Beschreibung
und abgeschnittene Pfade. Die Beschreibung ist gekürzt; Pfade zeigen einen
passenden UTF-8-Suffix samt vollständigem Pfad als Hinweis.

Die vollständigen Release-Läufe ohne Fenster bestehen jeweils 512/512:
macOS `build/workspaces-final-mac/test-results/run-d6055ur1`, Debian
`build/workspaces-linux/test-results/run-ckleqt__` innerhalb der VM.
Der wiederholte macOS-Fenstergesamtlauf besteht 49/49:
`build/workspaces-final-mac/test-results/run-t5wk63xn`. Debian besteht ebenfalls
49/49: `build/workspaces-linux/test-results/run-k_dkl_m3` unter
X11/Xvfb/Openbox/Mesa. Die finale Verwaltungsansicht mit Pfadsuffix und
gesperrter Öffnen-Aktion wurde auf beiden Systemen bei 1080 × 740 visuell geprüft.
Die neu installierten, verschobenen Release-SDKs bestehen mit unabhängigen
Headern und Verbrauchern, installiertem und neu gebautem Kern, C-/Physim-Modulen,
adaptiven Läufen, Zielzeitstudien und neun neu gebauten Projekten:
macOS `build/workspaces-sdk-proof-mac/Native SDK ä fhqa41w_`, Debian
`build/workspaces-sdk-proof-linux/Native SDK ä 4fpkhmcf` innerhalb der VM.
Die Umgebungen bleiben Intel macOS 14.6.1/AppleClang 16 und Debian 12/GCC 12.2
mit SDL 3.2.30. Sprache 0.170.0, öffentliche API und ABI 3 bleiben unverändert;
der persönliche Katalog verwendet Format 1.

## Benannte Panelanordnungen am 5. Oktober 2026

Katalog, Docking und Einstellungen bestehen auf macOS Debug 3/3:
`build/layouts-debug-mac/test-results/run-xrp18z64`. Die neue Verwaltung besteht
mit aktiven UI-Assertions:
`build/layouts-debug-mac/test-results/run-607y3f86`.
Unter Debian Release bestehen die drei Modelle ebenfalls:
`build/layouts-linux/test-results/run-9g3bs89x` innerhalb der VM; der erste
Verwaltungsablauf besteht unter
`build/layouts-linux/test-results/run-zjet3zqb`.

Die Katalogprüfung deckt leere und volle Kataloge, UTF-8-Namen, byteweise
Beschädigungen, alle Abschneidepositionen, zusätzliche Bytes, unbekannte
Versionen, ungültige Namen, doppelte Namen, Panelgraphen, aktive Tabs,
Größengrenzen und fehlgeschlagene Dateiersetzung ab. Lese- und Modellfehler
erhalten den bisherigen Zustand. Die gemeinsame Dockingkodierung erhält
das bisherige Einstellungsformat und die Migration der Formate 1–3.

C und Physim laufen in eigenen Projekten. Maus- und Texteingabeereignisse
speichern zwei Anordnungen, wenden sie an, ersetzen einen vorhandenen Namen
und löschen einen Eintrag. Teilungen, aktive Tabs, versteckte Panels, freier
Inspektor, Größen und Protokollansicht kehren zurück. Theme und Schriftgröße
bleiben bestehen; die Simulation schreitet fort, und der CRC des ungespeicherten
Editors bleibt gleich. Ein vollständig verborgener Panelbaum lässt sich weiterhin
über die Menüzeile wiederherstellen. Weitere App-Prozesse prüfen Neustart,
Erhalt einer beschädigten Datei und ausdrückliches Zurücksetzen. Die Hashes
von Quellen und Laufdaten bleiben dabei unverändert. Der finale Debugablauf
prüft zusätzlich das Anwenden aus den Einstellungen: Geometrie wird synchronisiert,
der ungespeicherte Theme-/Schriftentwurf bleibt erhalten.

Frühe Verwaltungsprüfungen lesen Texteingaben vor dem folgenden Renderframe;
sie warten jetzt auf dessen Verarbeitung. Dabei entdeckt der Prüfer, dass
die ausgewählte Zeile einen neu eingegebenen Namen pro Frame zurücksetzt.
Die Auswahl kopiert den Namen nun ausschließlich bei einem Klick.
Die bisherigen Zeitgrenzen sind unverändert. Dunkle und helle Ansichten,
UTF-8-Namen, Fehler und Zurücksetzen wurden bei 1080 × 740 logischen Pixeln
auf dem Intel-Mac visuell geprüft. Die neue Kopfzeile verwendet keinen Abstand
zwischen den Fenstersteuerungen; alle drei Buttons bleiben vollständig sichtbar.

Die vollständigen Release-Läufe ohne Fenster bestehen jeweils 511/511:
macOS `build/layouts-final-mac/test-results/run-r6lupogg`, Debian
`build/layouts-linux/test-results/run-s6mgc2eh` innerhalb der VM.
Die vollständigen Release-Fensterläufe bestehen jeweils 48/48: macOS
`build/layouts-final-mac/test-results/run-0_2371a7`, Debian
`build/layouts-linux/test-results/run-b4nqdu99` unter X11/Xvfb/Openbox/Mesa.
Auch die Linux-Ausgabe der neuen Verwaltung wurde bei 1080 × 740 visuell geprüft.
Die verschobenen Release-SDKs bestehen mit unabhängigen Headern und Verbrauchern,
installiertem und aus SDK-Quellen neu gebautem Kern, C-/Physim-Modulen,
adaptiven Läufen, Zielzeitstudien und neun neu gebauten Projekten:
macOS `build/layouts-sdk-proof-mac/Native SDK ä e1zcew_4`, Debian
`build/layouts-sdk-proof-linux/Native SDK ä h7_g5aj5` innerhalb der VM.
Die Umgebungen sind weiterhin Intel macOS 14.6.1/AppleClang 16 und
Debian 12/GCC 12.2/SDL 3.2.30. Die Sprache bleibt auf 0.170.0, öffentliche
API und ABI bleiben auf 3; der private Katalog verwendet Format 1.

## Eigenständiger Inspektor am 5. Oktober 2026

Dockingmodell und Einstellungenmigration bestehen auf macOS Release 2/2:
`build/inspector-mac/test-results/run-r0bg29vm`, und Debian Release 2/2:
`build/inspector-linux/test-results/run-2t56qv2n` innerhalb der VM.
Vier Panels, sieben Knoten, Tab-/Teilungs-/Float-/Hidden-Wechsel und transaktionale
Fehler sind geprüft. Eine echte fünfknotige Format-3-Datei erhält ihre Knoten,
Tabs und Rechtecke; der neue Inspektor bleibt zunächst verborgen. Formate 1/2,
CRC, Abschneiden, ungültige Graphen und fehlgeschlagene Dateiersetzung sind geprüft.

Die ersten vier macOS-Fensterprüfungen bestehen:
`build/inspector-mac/test-results/run-oxkweq7i` (Einstellungen, Hierarchie,
adaptive Schritte und Zielzeitserien). Docking, kleines Menü, Projekteinstellungen
und Parametereinheiten bestehen zusätzlich 4/4:
`build/inspector-mac/test-results/run-6a9mkk3c`.

Der neue Inspektorablauf besteht auf macOS:
`build/inspector-mac/test-results/run-c47n901_`.
C und Physim laufen in separaten Projekten; echte Mausereignisse lösen den
Inspektor aus der Anordnung, bedienen Objektsichtbarkeit, schließen und öffnen
ihn über das Menü, bilden eine Tabgruppe mit dem Dateibaum und ändern sein
freies Rechteck auf 320 × 480. Die Simulation läuft dabei weiter. Ein CRC des
ungespeicherten Editors bleibt identisch; es entsteht je Projekt nur ein Run.
Weitere App-Prozesse öffnen die gespeicherte Anordnung ohne Build/Runner;
Hashes von Quelle und Lauf bleiben unverändert.

Die frühe Sichtprüfung bei 1080 × 740 erkennt zu schmale Simulations- und
Zeitleistenbuttons. Bei schmalem Arbeitsbereich verwenden die Steuerungen nun
zwei Spalten, und der Timeline-Schieber hat eine eigene Reihe. Die aktuelle
Anordnung, beschriftete Steuerung und Inspektor wurden bei 2160 × 1480 physischen
Pixeln auf dem Intel-Mac visuell geprüft.
Die ersten zwei Inspektorprüfungen lesen Float-/Tabwechsel vor dem nächsten
Renderframe; sie warten jetzt wie der bestehende Dockingprüfer auf dessen
Verarbeitung. Die bisherigen Zeitgrenzen sind unverändert.

Die vollständigen Release-Läufe ohne Fenster bestehen 510/510: macOS
`build/inspector-final-mac/test-results/run-d6xhnjy0`, Debian
`build/inspector-linux/test-results/run-u18jcah3` innerhalb der VM.
Docking und Migration bestehen zusätzlich im macOS-Debug-Build 2/2:
`build/inspector-debug-mac/test-results/run-4xy3qbmt`.

Der erste macOS-Fenstergesamtlauf besteht 44/47:
`build/inspector-mac/test-results/run-_udplaoz`.
Eine unbeabsichtigte Erweiterung des Standardwerte-Verhaltens bewahrte alte
Panelbreiten und Protokollhöhe. Das bisherige Zurücksetzen ist wiederhergestellt;
die Einstellungsprüfung besteht mit demselben Prüfer:
`build/inspector-settings-final-mac/test-results/run-dbx1usxt`.
Die beiden Plotfälle erkennen eine feste, insgesamt 700 Pixel breite Exportreihe,
deren letzte Schaltfläche unter dem neuen Inspektor lag. Die Ausschnittbuttons
verwenden in schmalen Arbeitsbereichen nun eine eigene Zeile; die Größenwahl
behält ihre Funktion. Der Prüfer verlangt weiterhin tatsächliche SVG-/PNG-Dateien
und ihre Inhalte, ohne die Prüfung abzuschwächen.

Das verschobene macOS-SDK mit installierter und aus SDK-Quellen neu gebauter
Bibliothek besteht: `build/inspector-sdk-proof-mac/Native SDK ä fj4_k76s`.

Die finalen Plot-/Eingabeprüfungen bestehen auf macOS 2/2:
`build/inspector-plot-final-mac/test-results/run-pt1_ldlp`. Beide Ausschnittformate
werden über echte Schaltflächen erzeugt; die bestehenden Bild- und Bereichsprüfungen
bleiben erhalten. Der neue Inspektorablauf besteht auch mit aktiven UI-Assertions
im macOS-Debug-Build: `build/inspector-debug-mac/test-results/run-f4t25yji`.

Der korrigierte vollständige macOS-Fensterlauf besteht 47/47:
`build/inspector-plot-final-mac/test-results/run-jvfs2qf6`. Das finale verschobene
macOS-SDK einschließlich der korrigierten Exportanordnung besteht:
`build/inspector-sdk-final-proof-mac/Native SDK ä kdd9dg3j`.
Der erste Debian-Fensterlauf besteht 45/47:
`build/inspector-linux/test-results/run-vz7089wn`; ausschließlich die beiden
Plot-/SVG-Ausschnittfälle scheitern am selben zu breiten Exportlayout. Inspektor,
Einstellungen und sämtliche übrigen Abläufe bestehen bereits in diesem Lauf.

Der korrigierte vollständige Debian-Fensterlauf besteht 47/47:
`build/inspector-linux/test-results/run-m6rg6hyd` innerhalb der VM. Er verwendet
X11, Xvfb/Openbox und Mesa 22.3.6. Beide zuvor fehlgeschlagenen Plotfälle,
Inspektor, Docking, Einstellungen und sämtliche übrigen Abläufe bestehen mit
den bisherigen Zeitgrenzen. Es wurden keine zusätzlichen Plattformen oder
Wayland aus dieser Abnahme abgeleitet.

Das finale verschobene Debian-SDK mit installierter und unabhängig aus seinen
Quellen neu gebauter Bibliothek besteht ebenfalls:
`build/inspector-sdk-proof-linux/Native SDK ä 1x1xy_0j` innerhalb der VM.
Damit liegen für diese Änderung auf beiden ausgeführten Systemen vollständige
Release-, Fenster- und SDK-Nachweise vor; die UI- und Datenformate wurden nur
in den beschriebenen Umgebungen ausgeführt.

## Explizite Parametereinheiten am 5. Oktober 2026

Die Änderung ergänzt eigene Symbol-/Skalen-/Dimensionsdaten im optionalen
ABI-3-Kontext-Tail, SI-Persistenz, Runner-Abfrageformat 2 und Anzeigeumrechnung.
Die bestehenden Kontext- und Parameterfelder bleiben an ihrer bisherigen Position.
Der Katalog enthält 510 Prüfungen ohne Fenster (499 ohne SDL) und 46 Fensterfälle.

Die gezielte macOS-Release-Prüfung besteht 8/8:
`build/parameter-units-model-final-mac/test-results/run-edmj8xj3`.
Ein analytisches Modell in C und Physim deklariert `velocity` in `cm/s` mit Skala
0,01; sämtliche numerischen Messungen bleiben SI und werden exakt verglichen.
Der Studienbericht enthält 20/160/300 cm/s zu SI-Overrides 0,2/1,6/3 m/s.
Katalogformat 1 bleibt lesbar; Format 2 transportiert bekannte und unbekannte
Einheiten. Ein eigener CRC-gültiger Runner mit unvollständigen oder zwischen
Läufen widersprüchlichen Einheiten wird ohne Gesamtbericht abgewiesen.
Ein Physim-Modul mit zu langem Symbol erhält eine Quelldiagnose und Startfehler.

Der abschließende gezielte macOS-Release-Lauf besteht 5/5:
`build/parameter-units-final-target-mac/test-results/run-o5i6o17q`.
Er prüft zusätzlich Über-/Unterlauf der Anzeige, exakte Erhaltung von Standards
und bestehenden SI-Auswahlen sowie Speichern/Wiederherstellen von 50 cm als
0,5 m in der Projektdatei. Bekannte Dimensionswechsel setzen die Auswahl zurück.
Ungültige Deklarationen und Metadaten erhalten die bisherigen Ausgaben; alte
Kontextgrößen bleiben für untypisierte Parameter nutzbar.

Die zwei vollständigen Meter-/Zentimeter-Bedienfälle bestehen auf macOS:
`build/parameter-units-mac/test-results/run-vvjxc49n`.
Jeweils C- und Physim-Pendel verwenden echte Text-/Mausereignisse für Start- und
Endwert, erzeugen SI-Laufdaten, 1200 × 850 Pixel große PNGs und SVGs und öffnen
den archivierten Bericht ohne Build/Runner erneut. Sämtliche Zeitpunkte und
Kanalwerte stimmen zwischen beiden Sprachen exakt überein. Die PNG-Achse mit
50/150/250 cm und `length [cm]` wurde visuell geprüft. Wiederöffnung bewahrt
die Datenhashes.

Der erste frühe Modelllauf scheitert an noch nicht deklarierten Einheitenbezeichnern
in der Physim-Pendelvorlage; die konkreten Unit-Werte stehen jetzt vor der
Parameterdefinition. Der erste Zentimeter-Bedienlauf besteht nur für C: Das
Physim-Testmodell änderte irrtümlich auch die Messkanäle auf Skala 0,01, die
kanonische Kanalregistrierung lehnt das korrekt ab. Der korrigierte Prüfer ändert
nur die Parameterdeklaration. Diese frühen Läufe gelten nicht als finale Abnahme.

Die vollständigen Release-Läufe ohne Fenster bestehen 510/510: macOS
`build/parameter-units-model-final-mac/test-results/run-sr6cg0g0`, Debian
`build/parameter-units-linux/test-results/run-q0i1kyh5` innerhalb der VM.
Die vollständigen Fensterläufe bestehen 46/46: macOS
`build/parameter-units-mac/test-results/run-hdgug8rh`, Debian
`build/parameter-units-linux/test-results/run-s6dy7qiv`.

Die zusätzliche rundungskritische Bedienprüfung mit Standard 0,29 m und
Zentimeteranzeige besteht auf macOS 2/2:
`build/parameter-units-mac/test-results/run-gzp_j26s`. Ihr erster Aufruf verlangte
irrtümlich das Schreiben einer unveränderten Einstellung; normales Speichern
ist dann ohne Wirkung. Der Prüfer fordert nun wie das Parameterformular die
Übernahme der Auswahl an und prüft den exakt archivierten SI-Wert. Nach der
abschließenden Parseränderung bestehen beide Bedienfälle nochmals 2/2:
`build/parameter-units-mac/test-results/run-1ru5roi5`.

Die abschließende Zahlenprüfung besteht 7/7 auf macOS:
`build/parameter-units-last-model-mac/test-results/run-q3onl7t4`, und 7/7 unter
Debian: `build/parameter-units-final-linux/test-results/run-qwakwdf2`.
Sie erhält nichtnullige subnormale Skalen und Parameterwerte, einschließlich
`offset=1e-310` in C- und Physim-Runnern sowie im Batch-CLI. Ein unabhängiger
Messkanal enthält den Wert in allen gelesenen Samples exakt. `1e-999` bleibt
ungültig; Vorzeichen der Null bleiben erhalten. Eine bei einer neuen Skala zu
Null gerundete vorhandene Auswahl führt zu atomarer Ablehnung statt zum Verlust
ihres SI-Werts. Bericht, Projektdatei, Default 0,29 m und CRC-gültige Einheitenfehler
bleiben Teil dieser Prüfung. Die finalen sechs Modellfälle bestehen zusätzlich
im macOS-Debug-Build:
`build/parameter-units-debug-mac/test-results/run-ijlnvipu`.

Die SDK-Prüfung besteht mit den finalen öffentlichen und privaten Headern,
installierter und aus SDK-Quellen unabhängig neu gebauter Bibliothek, C-/Physim-
Modulen, Parameterstudien, Metadaten, Diagrammen und SVG-Beschriftungen: macOS
`build/parameter-units-sdk-final-proof-mac/Native SDK ä ppueo3yp`, Debian
`build/parameter-units-sdk-final-proof-linux/Native SDK ä 25v20ne2`.
Beide SDKs sind vor der Prüfung verschoben; keine Bibliothek aus dem Checkout
wird als Ersatz benutzt.

Mit dem finalen Zahlenparser bestehen die Meter-/Zentimeter-Bedienfälle unter
Debian 2/2: `build/parameter-units-final-linux/test-results/run-rdp6q5ef`.
Die vollständige Zentimeterprüfung mit UI-Assertions besteht auch im finalen
macOS-Debug-Build: `build/parameter-units-debug-mac/test-results/run-fj1bmvhw`.
Diese zusätzlichen Läufe prüfen die abschließenden Änderungen an Zahleneingabe
und SI-Erhaltung; die vollständigen Durchläufe oben decken die übrigen Abläufe ab.

## Laufserien mit gemeinsamer Zielzeit am 5. Oktober 2026

Der Zielzeitmodus wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Release) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64,
GCC 12.2, SDL 3.2.30, X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Der Katalog enthält 509 Prüfungen ohne Fenster (498 ohne SDL) und 45 Fensterabläufe.

Die gezielte Runnerprüfung besteht auf macOS:
`build/series-target-mac/test-results/run-ihh0h_f7`. Ein analytisches Translationsmodell
in C und Physim vergleicht alle Messungen exakt. Ziele vor dem ersten Rasterpunkt,
zwischen Punkten und nach mehreren Punkten sind geprüft; ein 0,05-s-Endintervall
endet bei 0,25 s trotz konfiguriertem 0,08-s-Minimum. Ein zu kurzes Budget erhält
Anfang und akzeptierten Punkt als lesbaren Präfix ohne gültigen Footer.
Ungültige Ziele und Verwendung im interaktiven Modus werden vor dem Start abgewiesen.

Die Serienprüfung besteht auf macOS:
`build/series-target-mac/test-results/run-8bhsi5w5`. Seedabhängige Anfangswerte,
seriale/parallelisierte Ausführung, C-/Physim-Daten, Rohendwerte und Berichtsdaten
werden verglichen. Velocity-Sweep, abgeschnittenes Endintervall, fehlender adaptiver
Callback, Budgetfehler und Abbruch mit Prozessbereinigung sind geprüft. Ein eigener
Runner schreibt CRC-gültige Dateien mit falschem Ziel, doppelten Zeiten, zu vielen
Samples, falscher Konfiguration, verletzten Schrittgrenzen, falschem Start oder
fehlendem Footer. Der Controller weist diese Dateien ab und schreibt keinen Gesamtbericht.

Die unveränderten Runner-/Parallelprüfungen bestehen auf macOS 4/4:
`build/series-timing-mac/test-results/run-muc2qsb9`; die bisherigen festen
Pendel-/Integratorreferenzen bestehen zusätzlich:
`build/series-timing-mac/test-results/run-3n83g_ih`.
Die bisherigen Serien-/Adaptive-Bedienabläufe bestehen 2/2:
`build/series-timing-mac/test-results/run-rtxdyaqw`.

Der neue Fensterablauf besteht auf macOS:
`build/series-timing-mac/test-results/run-yyf2512r`. C- und Physim-Pendelprojekte
aktivieren adaptive Serien über echte Widgets, geben zunächst das ungültige Ziel
0 und danach `7e-1` über SDL-Text-/Tastaturereignisse ein und vergleichen die Längen
0,5/1,5/2,5 m. Alle überlappenden Zeiten und Kanalwerte stimmen exakt überein;
die Punktzahlen unterscheiden sich nach Pendellänge. Zwei weitere Prozesse öffnen
den archivierten Bericht ohne Build/Runner. Dateihashes bestätigen unveränderte Daten.
Der erste Wiederöffnungstest wartet irrtümlich in der Messlaufansicht auf den Bericht;
die Prüfung wählt nun vor dem Öffnen die Berichtansicht.

Der erste Debian-Build stoppt wegen eines vollen 16-GiB-Datenträgers. Alte generierte
Arbeitsordner früherer Prüfungen unter `build/native/Release/test-results` werden
entfernt; die zwei neuesten vollständigen Durchläufe und sämtliche strukturierten
Ergebnisberichte bleiben erhalten. Dieser Buildabbruch wird nicht als bestandener
Testdurchlauf gezählt.

Die abschließende gezielte macOS-Release-Prüfung besteht 4/4:
`build/series-final-mac/test-results/run-ankpqg54` (Runner, Serien, Bericht und
unveränderter mathematischer Sprachfehlerfall). Die neuen Modellprüfungen bestehen
auch im Debug-Build 2/2: `build/series-debug-mac/test-results/run-obl8dd0u`;
der adaptive Serien-Fensterablauf mit aktiven Assertions besteht zusätzlich:
`build/series-debug-mac/test-results/run-pve22knr`.

Der finale Export-/Bedienvergleich besteht auf macOS 2/2:
`build/series-display-final-mac/test-results/run-fi5w1rhl`. PNG-Dateien mit
1200 × 850 Pixeln und SVG-Dateien werden für beide Modelle erzeugt und geprüft.
Die PNG-Darstellung wurde außerdem visuell kontrolliert. Parameter besitzen in
der aktuellen API keine formale Einheit; die X-Achse zeigt deshalb `length`
ohne erfundenes `[1]`. Die Kanalachse behält `angle [rad]`. Dieselbe Regel gilt
für die App und beide Exportformate.

Der erste macOS-Gesamtlauf ohne Fenster besteht 508/509; der unveränderte
Sprachfehlerfall `language_native_mathpowdomain` überschreitet seine Zehn-Sekunden-Grenze.
Die gezielte Wiederholung besteht mit derselben Grenze. Der erste vollständige
Mac-Fensterlauf besteht 43/45. Der Workspace-Prüfer wechselte das Projekt,
bevor sein Bibliotheksworker beendet war; er wartet nun wie die anderen
Bedienprüfer auf den tatsächlichen Abschluss. Der gezielte Fall besteht mit
seiner bisherigen Grenze: `build/series-display-final-mac/test-results/run-omhe2m67`.
Der zweite Fehler entstand, weil der laufende Prüfer bereits neue Exportdateien
verlangte, sein zuvor gebautes Testprogramm sie aber noch nicht erzeugte.
Der abschließende Exportvergleich verwendet das aktualisierte Programm.

Die erste SDK-Prüfung mit SVG-Export erkennt im Prüfer eine doppelte Ausgabe:
installierter und neu gebauter Verbraucher schrieben denselben Dateinamen.
Der Exporter lehnt bestehende Dateien korrekt ab. Beide Verbraucher verwenden
jetzt getrennte Ausgaben. Das finale verschobene macOS-SDK besteht vollständig:
`build/series-sdk-corrected-proof-mac/Native SDK ä npnkp6da`. Vorgebauter Pendel,
neu gebaute C-Quellen und Physim-Quellen erzeugen je drei adaptive Läufe mit
exakter gemeinsamer Endzeit. Beide Bibliotheksverbraucher prüfen variable
Punktzahlen, Energie, Rohendwerte, Berichtskurven und SVG-Beschriftung.

Das finale verschobene Debian-SDK besteht ebenfalls vollständig:
`build/series-sdk-final-proof-linux/Native SDK ä goh054ga` innerhalb der VM.
Die Zielzeitstudien laufen für alle drei Modulwege durch beide Verbraucher;
Header, Beispielmodule, native Projekte und Sprachmodule werden unabhängig
gegen die installierte und die aus SDK-Quellen neu gebaute Bibliothek geprüft.

Der vollständige finale macOS-Fensterlauf besteht 45/45:
`build/series-display-final-mac/test-results/run-2pue_3pu`.
Der vollständige Debian-Lauf ohne Fenster besteht 509/509:
`build/series-target/test-results/run-oj_64_2b` innerhalb der VM.
Nach den abschließenden Änderungen an Einheitenanzeige und Exportprüfern bestehen
Runner, Serien und Bericht unter Debian nochmals 3/3:
`build/series-target/test-results/run-xo552x2v`.

Der erste vollständige Debian-Fensterlauf besteht 44/45:
`build/native/Release/test-results/run-xqni5_ig`. Der neue Zielzeitablauf besteht;
der unveränderte Geschwindigkeitstest verfehlt bei parallelen SDK-/Buildarbeiten
seinen Mindestfortschritt (1,36 statt mehr als 1,4 simulierte Sekunden).
Die Folgeprüfung läuft nach Abschluss dieser Zusatzarbeiten mit denselben
Zeit- und Fortschrittsgrenzen.

Ein anschließender Prüfaufruf ohne Xvfb scheitert bei allen 45 Fensterfällen
bereits am SDL-Start (`No available video device`):
`build/native/Release/test-results/run-ioa037qk`. Dieser Aufruffehler liefert
keinen Nachweis zum App-Verhalten. Die finale Wiederholung verwendet wieder
Xvfb, Openbox, X11 und Mesa.

Der abschließende vollständige macOS-Release-Lauf ohne Fenster besteht 509/509:
`build/series-final-mac/test-results/run-6kqmayn0`. Er enthält auch die zuvor
zeitüberschrittene Sprachprüfung mit ihrer unveränderten Grenze und beide
neuen Zielzeitprüfungen.

Der finale vollständige Debian-Fensterlauf mit Xvfb/Openbox besteht 45/45:
`build/native/Release/test-results/run-xx5p4wra` innerhalb der VM. Der zuvor
knapp fehlgeschlagene Geschwindigkeitstest und der vollständige neue
C-/Physim-Zielzeit-/Export-/Wiederöffnungsablauf bestehen mit unveränderten
Prüfgrenzen. Damit liegen für diese Änderung auf beiden ausgeführten Systemen
509/509 Prüfungen ohne Fenster, 45/45 Fensterabläufe und die verschobenen
SDK-Verbraucher vor. Das belegt keine zusätzlichen Plattformen oder Wayland.

## Adaptive Simulationsschritte am 5. Oktober 2026

Die adaptiven Schritte wurden auf dem lokalen Intel-Mac (macOS 14.6.1,
Apple Clang 16, SDL 3.2.30, Release) und in der Debian-12-VM (Linux
6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, X11/Xvfb/Openbox, Mesa 22.3.6)
gebaut und ausgeführt. Der Katalog enthält 507 Prüfungen ohne Fenster,
davon 496 ohne SDL, und 44 Grafik-/Fensterabläufe.

Die Numerikprüfung bestätigt den ersten akzeptierten Dormand–Prince-Schritt,
verworfene Versuche, analytische Exponentialreferenzen, Rückwärtsintegration,
Versuchs-/Mindestschrittgrenzen und unveränderten Zustand bei Fehlern.
Der Runnerprüfer verwendet denselben analytischen Oszillator in C und Physim.
Alle 201 Offline-Referenzmessungen stimmen in sämtlichen Kanälen exakt überein;
variable Intervalle und Verwerfungen sind ausdrücklich erforderlich. Jeder
Zeitpunkt wird außerdem gegen Sinus/Cosinus geprüft. Interaktive Läufe mit
0,5×, 4×, Offline und verzögertem Leser werden kanalweise mit unabhängigen
Offline-CLI-Läufen gleicher Länge verglichen. Pause, Einzelschritt und Live-
Geschwindigkeitswechsel erhalten dieses Raster. Ungültige Dauer, Vorschlag,
veränderte Hostzeit und fehlende ABI-Tailgröße beenden den Lauf, ohne einen
weiteren Messpunkt oder gültigen Footer zu schreiben.

Die unveränderten festen Referenzabläufe und das mit eingefrorenem ABI-3-Header
gebaute Modul bestehen zusätzlich auf macOS 4/4:
`build/adaptive-mac/test-results/run-lrdo8mlx`.
Die erste neue Numerikprüfung besteht; der erste Runnerdurchlauf erkennt einen
zu kurzen Referenzlauf für Offline-Geschwindigkeit. Der Prüfer erzeugt nun eine
unabhängige CLI-Referenz mit der tatsächlichen gespeicherten Punktzahl und vergleicht
weiterhin sämtliche Punkte exakt. Er besteht:
`build/adaptive-mac/test-results/run-f_qfh42l`.
Projektdatei und Zeitkonto bestehen 2/2:
`build/adaptive-mac/test-results/run-u60c_w6m`.
Neue Sprachwerte, Callbackdiagnosen und generierte Referenz bestehen 3/3:
`build/adaptive-mac/test-results/run-8d9tm97z`.

`adaptive_workflow` bedient Auswahl, Einzelschritt, Pause, Reset und Stop in
C- und Physim-Pendelprojekten. Reset reproduziert Zeit und alle Werte des ersten
akzeptierten Schritts. Gespeicherte Grenzen und variable Zeiten werden geprüft;
alle überlappenden C-/Physim-Messungen stimmen exakt überein. Zwei weitere Prozesse
öffnen die archivierten Szenen über die Laufbibliothek und bedienen Rückblick,
Schritt und Wiedergabe ohne Modellstart. Hashes bestätigen unveränderte Dateien.

Der korrigierte vollständige Debian-Release-Lauf besteht 507/507 ohne Fenster:
`build/adaptive-final/test-results/run-bhf2hhku` innerhalb der VM.

Der korrigierte vollständige macOS-Release-Lauf besteht 507/507 ohne Fenster:
`build/adaptive-final-mac/test-results/run-5akn2oys`. Der vollständige
Fensterdurchlauf mit geprüften Texteingaben besteht 44/44:
`build/adaptive-ui-final-mac/test-results/run-76aj8pe8`.
Nach Ergänzung derselben Texteingabe für die Startdauer erkennt der adaptive
Bedienfall eine beim Umschalten versehentlich zusätzlich angelegte Layoutzeile.
Die doppelte Zeile ist entfernt; der gesamte adaptive Ablauf besteht mit dem
finalen Editor auf macOS erneut:
`build/adaptive-editor-final-mac/test-results/run-rn_c1kdu`.

Die abschließenden vollständigen Fensterläufe mit dem finalen Start- und
Grenzeditor bestehen je 44/44: macOS
`build/adaptive-editor-final-mac/test-results/run-s75ualf3`, Debian
`build/native/Release/test-results/run-5kcxf9rh` innerhalb der VM.
Die Startdauer, wissenschaftliche Mindestdauer und Höchstdauer sind bei
1080 × 740 logischen Pixeln auf macOS visuell geprüft (2160 × 1480 physische Pixel).

Die erste Sichtprüfung bei 1080 × 740 logischen Pixeln zeigt einen abgeschnittenen
Erklärungstext und als 0.0000 gerundete Mindestschritte. Wissenschaftliche
Texteingaben ersetzen die Zahlenwidgets für Startdauer und beide Grenzen. Der erweiterte Ablauf
schreibt über echte SDL-Tastatur-/Text-Ereignisse zunächst den unvollständigen
Wert `1e-`, verlangt verhinderten Start und schreibt danach `1e-6`.
Der erste erweiterte Prüflauf liest den Wert zu früh vor Verarbeitung im Widget;
ein Frame Abstand, wie bei den bisherigen Dokumenttests, prüft den tatsächlich
verarbeiteten Zustand. Frühere Prüfläufe werden nicht als finale Abnahme gezählt.


Der erste macOS-Gesamtlauf besteht 504/507, der erste Debian-Gesamtlauf 505/507.
Zwei Tutorialprüfungen erkennen in beiden Läufen den fehlenden `NULL`-Tail in den
abgedruckten C-Descriptoren; die Blöcke entsprechen jetzt wieder exakt ihren
getesteten Quelldateien. Der nachträglich ergänzte macOS-Metadatenfall zeigt am
frühen Runner tatsächlich eine Datei ohne Adaptive-Modus und Modulidentität,
wenn das Modell den Textpuffer füllt. Der Runner prüft jetzt Platz für beide
Provenienzblöcke und weist den Start vor dem Anlegen der Datei ab; die neue
Prüfung verlangt den Fehlercode und eine nicht vorhandene Datei.

Der Debug-Modelllauf besteht 4/4 auf macOS:
`build/adaptive-debug-mac/test-results/run-tp7w9y7v`; der vollständige adaptive
Fensterablauf mit Eingabe und aktiven UI-Assertions besteht zusätzlich:
`build/adaptive-debug-mac/test-results/run-yae_4cau`.

Eine weitere Compilerprüfung zeigt, dass ein generisch deklarierter
`adaptiveStep<T>` im frühen Emitter stillschweigend nicht als Callback exportiert
wird. Der Compiler weist diese Deklaration jetzt ausdrücklich ab. Der abschließende
Sprach-/Runnervergleich besteht auf macOS 3/3:
`build/adaptive-callback-final-mac/test-results/run-t2wni63b`.


Der abschließende Sprach-/Runnervergleich besteht auch unter Debian 3/3:
`build/adaptive-callback-final/test-results/run-h_3b3l8m` innerhalb der VM.
Die finalen verschobenen SDKs bestehen auf beiden Plattformen: unabhängige Header,
installierte und aus SDK-Quellen neu gebaute Bibliothek, C- und Physim-Module und
native Projekte. Zusätzlich laufen je 501 adaptive Pendelmessungen aus einem
vorgebauten C-Modul, neu gebauten C-Quellen und Physim-Quellen; tatsächliche variable
Zeiten und Energie werden geprüft, anschließend Analysen in beiden Sprachen
und erneut die Verbraucherprobe gegen die neu gebaute Bibliothek. macOS:
`build/adaptive-sdk-final-proof-mac/Native SDK ä yp9sjtu8`; Debian:
`build/adaptive-sdk-final-proof-linux/Native SDK ä ysgopwad` innerhalb der VM.
Die SDK-Prüfung verwendet hier keine GUI-Abläufe; deren Nachweise stehen separat.

## Szenenhierarchie und Szenenversionen am 5. Oktober 2026

Die Szenenhierarchie wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Release) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64,
GCC 12.2, SDL 3.2.30, Release, X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Der Katalog enthält 503 Prüfungen ohne Fenster, davon 492 ohne SDL, sowie
43 Grafik-/Fensterabläufe.

Die vollständigen Release-Läufe ohne Fenster bestehen je 503/503:
macOS `build/hierarchy-final-mac/test-results/run-kzr13_49`,
Debian `build/hierarchy-final/test-results/run-bz0zd13y` innerhalb der VM.

Die vollständigen Grafik-/Fensterläufe bestehen je 43/43:
macOS `build/hierarchy-mac/test-results/run-kjhsbh2v`,
Debian `build/native/Release/test-results/run-p0pebaqx` innerhalb der VM.

Die gezielten Modellprüfungen bestehen auf macOS 3/3:
`build/hierarchy-mac/test-results/run-b92wlh8h`. Gruppen, fehlende Eltern,
Selbstbeziehungen, Zyklen, atomare Fehler, Umsortierung, Reparenting und geerbte
Sichtbarkeit sind geprüft. Der maximale neue Snapshot mit 32 Einträgen,
96 Punkten und 16 Kanälen hat 8088 Bytes. Eine handgebaute versionierte alte
Szenendatei wird gelesen; gekürzte aktuelle Payloads werden nicht umgedeutet.

`runner_hierarchy` kompiliert ein C-Modul, dasselbe Modell in Physim und ein
Legacy-Modul mit dem eingefrorenen ABI-3-Header aus `8d69e4e`. Letzteres belegt
seine bisherigen Objekt-Padding-Bytes absichtlich mit 0xa5. Der aktuelle Runner
ignoriert diese Bytes ohne Hierarchie-Capability. Alle 21 Messungen stimmen
zwischen den drei Modulen genau überein; kanonische Snapshot-CRCs bestätigen
dieselben C-/Physim-Hierarchien und nach Entfernen der Gruppen dieselbe alte Geometrie.
Der Fehlerprüfer des direkten Testläufers ist erneut ausgeführt:
`build/hierarchy-harness-final/test runner ä ho03opvy` auf macOS.

`hierarchy_workflow` bedient die echten Widgets mit C- und Physim-Pendelprojekten.
Gruppen einklappen/ausblenden, Untergruppe ausblenden und Auswahl mit erneutem
Aufklappen werden gegen Snapshot-CRC und unveränderte pausierte Zeit geprüft.
Nach Stop liest die App die gespeicherte Hierarchie. Zwei weitere Prozesse öffnen
handkonvertierte Version-1-Szenen derselben Versuche über die Laufbibliothek,
zeigen sie als flache Szenen und bedienen Rückblick, Schritt und Wiedergabe.
Dateihashes bestätigen unveränderte Originale und Legacy-Dateien; Modellcode
läuft beim Wiederöffnen nicht.

Die verschobenen SDKs bestehen auf beiden Plattformen. Die Verbraucherprobe
baut Gruppen und Elternbeziehungen, weist einen Zyklus ab und liest den neuen
Snapshot wieder. Sie läuft gegen die installierte Bibliothek und erneut gegen
die aus SDK-Quellen gebaute Bibliothek. Öffentliche Header werden einzeln
kompiliert; gebündelte und neu gebaute C-Vorlagen sowie Physim-Experimente und
Analysen werden ausgeführt. macOS:
`build/hierarchy-sdk-final-proof-mac/Native SDK ä g9yaqnzw`; Debian:
`build/hierarchy-sdk-final-proof-linux/Native SDK ä g0x_bsmp` innerhalb der VM.

Die ersten vollständigen Läufe ohne Fenster erkennen auf beiden Systemen sechs
veraltete Testannahmen: fünf erwarten Sprachversion 0.167.0, eine erwartet die
Pendelgeometrie ohne die zwei neuen Gruppen. Die korrigierten Prüfungen verlangen
exakt 0.168.0 und prüfen zusätzlich die Beziehungen; die bisherigen numerischen
Referenzvergleiche bleiben erhalten. Alle sechs Fälle und die Referenzprüfung
bestehen anschließend je 7/7: macOS `run-8uflbb9y`, Debian `run-pzqgvpxp`.

Der erste Fensterdurchlauf besteht auf macOS 38/42 und auf Debian 41/42.
Beide erkennen dieselbe veraltete Pendel-Szenenanzahl im vollständigen Sprachablauf.
Auf macOS schlagen außerdem drei Dokument-/Workspace-Abläufe fehl. Der zusätzliche
Fall `documents_input_isolation` reproduziert einen verfehlten Dateiklick durch
native Maus-/Fokusereignisse gezielt ohne Eingabeisolation und bestätigt den
vollständigen Dokumentablauf mit Isolation in beiden Fenstergrößen. Er prüft
auch die tatsächlich gespeicherten Unicode-Texte, Sicherung und externe Änderung.
Die Isolation gilt nur für geskriptete Prüfungen; native Dialogtests behalten ihre
Desktop-Eingabe. Die fünf betroffenen macOS-Abläufe bestehen danach 5/5
(`run-vpp8_ufy`), der zusätzliche Störtest 1/1 (`run-wz1ix52a`). Die Zwischenläufe
werden nicht als bestandene Gesamtabnahme gezählt.

Die auf- und zugeklappte Hierarchie sowie ausgeblendete Nachfahren wurden bei
1080 × 740 logischen Pixeln auf macOS visuell geprüft (2160 × 1480 physische Pixel).
Die organisatorischen Beziehungen belegen keine vererbten Koordinatentransformationen.

Der vollständige Hierarchie-Fensterablauf besteht zusätzlich im macOS-Debug-Build
mit aktiven Nuklear-Assertions: `build/workspace-check/test-results/run-4532_de3`.

## Docking und gespeicherte Panelanordnung am 5. Oktober 2026

Das Docking-Grundsystem wurde auf dem lokalen Intel-Mac (macOS 14.6.1,
Apple Clang 16, SDL 3.2.30, Release) und in der Debian-12-VM
(Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, Release,
X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Die vollständigen Tests ohne Fenster bestehen mit 501/501:
macOS `build/timeline-release-mac/test-results/run-ty4lwbk4`,
Debian `build/native/Release/test-results/run-sndxecqo` innerhalb der VM.
Der gemeinsame Katalog enthält jetzt 501 Prüfungen ohne Fenster, davon
490 ohne SDL, und 41 Grafik-/Fensterabläufe.

Der vollständige Grafik-/Fensterdurchlauf besteht auf beiden Systemen mit 41/41:
macOS `build/docking-ui-final/test-results/run-p75wze89`,
Debian `build/native/Release/test-results/run-viu8navp` innerhalb der VM.
Das umfasst Editor, Autosaves, Wiederöffnung, beide Fenstergrößen, Menübedienung,
Diagramme, Serien, Reset, Geschwindigkeit, Zeitleiste sowie alle vollständigen
Physim-Projekte mit Analyse und Export. Die Anordnung bei 1080 × 740 wurde
visuell geprüft; der Mac liefert dabei 2160 × 1480 physische Pixel, Xvfb 1080 × 740.

Das Modell prüft Split-/Tab-/Float-/Hide-Übergänge, einen vollständig leeren
Panelbaum, Wiederanzeigen sowie tausend weitere gültige oder transaktional
abgewiesene Anordnungsänderungen. Einstellungen prüfen zusätzlich Format-1-
und Format-2-Migration, den vollständigen Baum, freie Rechtecke, Tabs,
240 Kürzungen und Byteänderungen sowie einen ungültigen Baum mit gültiger CRC.
Fehler erhalten sowohl die bestehende Datei als auch bereits geladene Einstellungen.

Die abschließenden Modell-/Einstellungsprüfungen bestehen je 2/2:
macOS `build/docking-ui-final/test-results/run-8it06dng`,
Debian `build/native/Release/test-results/run-pbtygbx4` innerhalb der VM.
Die Fehlerbehandlung des nativen Prüfers ist ebenfalls erneut ausgeführt:
`build/docking-harness-final/test runner ä bqu5b3m6` auf macOS.

`docking_workflow` bedient echte Titelzeilen, Ziele, Tabs, Größenänderungsgriffe,
Schließen, Menüaktionen, Escape und die Mindesthöhen einer verschachtelten
Teilung bei stark vergrößerter Nachbarfläche. Zwei Prozesse bauen C- und Physim-Versuche,
ordnen die Panels während weiterlaufender Simulation um und prüfen erhaltenen
ungespeicherten Quelltext über CRC, Zeitfortschritt und die danach vollständig
lesbaren Messdaten. Der Einstellungsweg stellt das ausgeblendete Protokoll wieder
her und prüft die unmittelbar gespeicherte Anordnung. Drei weitere Prozesse
prüfen Speicherung, Neustart/Wiederherstellung, erneutes Andocken, Zurücksetzen
und den Erhalt einer beschädigten Einstellungsdatei. Frei platzierte Panels
bleiben im Hauptfenster; diese Prüfung belegt keine separaten Betriebssystemfenster.

Die drei abschließenden macOS-Debug-Prüfungen von Diagrammen, Serien und
Docking bestehen mit aktivierten Nuklear-Assertions: `build/workspace-check/test-results/run-55oi60e7`.

Nach dem lesbaren Schließen-Knopf und der Begrenzung verschachtelter Teilungen
besteht der komplette Docking-Ablauf erneut auf beiden Systemen: macOS Debug
`build/workspace-check/test-results/run-ru22fbmp`, Debian Release
`build/native/Release/test-results/run-z6s33n7k` innerhalb der VM.

Zwischenläufe deckten eine verdeckte freie Panelfläche, die erst im nächsten
Frame aufgebaute Menüansicht und verbrauchte Mausrad-/Ziehereignisse auf.
Die korrigierte Reihenfolge und Eingabezuordnung bestehen die unveränderten
Diagramm-, Serien- und Zeitleistenprüfungen im vollständigen Enddurchlauf.
Die Zwischenläufe werden nicht als bestandene Gesamtabnahme gezählt.

## Zeitleiste und Szenenaufzeichnung am 5. Oktober 2026

Die Zeitleiste wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Debug und Release) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64,
GCC 12.2, SDL 3.2.30, Release, X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Die vollständigen Release-Läufe bestehen mit 500/500:
macOS `build/timeline-release-mac/test-results/run-97k6w7xp` und
Debian `build/native/Release/test-results/run-qg1essmn` innerhalb der VM.

Zwölf gezielte Prüfungen von Kern, Snapshotcodec, Streaming, Mutationen, Datenreihen,
Berichten, Taktung, Szenenaufzeichnung und echtem Projektbuild bestehen zuvor auf
beiden Plattformen: macOS `run-3nrdim5a`, Debian `run-o7fqbagl`.
Die C- und Physim-Runner liefern mit und ohne `--record-scenes` dieselben 201
Messungen aller Referenzkanäle. Die optionalen Blöcke enthalten die zugehörigen
Werte und komplette Geometrie; Anfang/Ende, Footerzählung, Legacy-Lesen,
ungültige Snapshotversionen/Szenen und CRC-/Tail-Recovery sind geprüft.
Über 10.000 Vorschauzustände bestätigen die Speichergrenze und erhaltene Endpunkte.

Der gemeinsame Fensterdurchlauf besteht je 9/9 mit Zeitleiste, Geschwindigkeit,
Reset, Themen, Projekteinstellungen, beiden Menügrößen und vollständigen
C-/Physim-App-Abläufen einschließlich Analyse, Export und Wiederöffnung:
macOS `run-44gk6k1o`, Debian `run-pf_6612g`.
Die abschließenden Prüfungen von Zeitleiste, Themen und vollständigem Physim-Ablauf
bestehen je 3/3: macOS `run-iyoym0pp`, Debian `run-uety5xa_`.
Nach der abschließenden Leseränderung, die für beide Durchgänge dieselbe geöffnete
Datei hält, bestehen Zeitleiste, Reset und vollständiger Physim-Ablauf erneut
je 3/3: macOS `run-fuvn_qum`, Debian `run-9hjimzg5`.
Der Zeitleistenfall startet zehn Prozesse für C-/Physim-Projekte sowie archivierte,
alte, alte mit negativen Zeitstempeln und rekonstruierte Dateien. Reale Widgets
und Tastaturereignisse prüfen Rückblick bei weiterlaufendem Versuch, Klick/Ziehen,
Vor/Zurück, Wiedergabe/Leertaste, Live und Reset. Kanonische Snapshot-CRCs und
Dateihashes bestätigen unveränderte Zustände und Originaldateien. Die Darstellung
bei 1080 × 740 ist visuell geprüft.

Die verschobenen SDKs sind auf beiden Systemen verifiziert: öffentliche Header
einschließlich `snapshot.h` einzeln kompilieren, gegen die installierte und aus
den gelieferten Quellen neu gebaute Kernbibliothek linken sowie gebündelte und
neu gebaute C-Vorlagen und Physim-Experimente/Analysen ausführen. macOS:
`build/timeline-sdk-proof-mac/Native SDK ä qn7i69nr`; Debian:
`build/timeline-sdk-proof-linux/Native SDK ä x_vuw3n3` innerhalb der VM.

Der erste Debug-Gesamtlauf auf macOS besteht 498/500. Neben dem bereits bekannten
Signal-9-Abbruch von `language_function_values_runtime` erkennt die Referenzprüfung
den noch nicht eingetragenen neuen öffentlichen Header. Modulbeschreibung,
Funktionsbeschreibungen und Offline-Navigation sind ergänzt; die korrigierte
Referenzprüfung besteht. Der vollständige Release-Lauf enthält beide Prüfungen
und besteht. Die lokale Debug-Systempolitik bleibt eine Einschränkung.

Die [C17-CI zu `a714459`](https://github.com/PhysicSimulator/physim/actions/runs/37281091082)
und [Paket-CI](https://github.com/PhysicSimulator/physim/actions/runs/37281091099)
sind fehlgeschlagen. Der alte Isolationstest verlangt nach RUN sofort Fortschritt;
dieser Fehler ist lokal in Debian reproduziert. Der aktualisierte Test trennt
Zustandsbestätigung und echten Fortschritt und prüft anschließend eine zeitstabile
Pause. Er besteht auf beiden Plattformen, einschließlich der Release-Gesamtläufe.
Ein neuer CI-Nachweis für diesen Stand steht bis zum Push aus.

## Simulationsgeschwindigkeit am 5. Oktober 2026

Die Geschwindigkeitswahl und das feste Echtzeitkonto wurden auf dem lokalen
Intel-Mac (macOS 14.6.1, Apple Clang 16, SDL 3.2.30, Debug) und in der Debian-12-VM
(Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, Release, X11/Xvfb/Openbox,
Mesa 22.3.6) gebaut und ausgeführt.

Die sechs gezielten Prüfungen bestehen auf beiden Plattformen: `pacing`,
`project_file`, `scene_protocol`, `parameters_api`, `runner_pacing_c` und
`runner_pacing_phys`. macOS: `build/workspace-check/test-results/run-al6_mguy`;
Debian: `build/native/Release/test-results/run-1kw0782u` innerhalb der VM.
Die C- und Physim-Zufallsmodelle vergleichen jeweils 201 Messpunkte aller Kanäle
mit dem unverändert offline ausgeführten CLI-Referenzlauf. 0,5×, 4×, Offline und
ein absichtlich verzögerter Pipe-Leser ergeben dieselben Zeiten und Werte.
Die tatsächlichen Laufzeiten werden verglichen; der feste `dt` und der Seed
bleiben erhalten. Geschwindigkeit während Betrieb/Pause, Einzelschritt,
ungültige CLI-/Wire-Werte und ein fehlender Handshake sind geprüft.
Bei `dt=1`, Faktor 0,1 meldet RUN sofort den unpausierten Zustand bei Zeit 0;
der folgende pausierte Einzelschritt erreicht unmittelbar Zeit 1.
Das künstliche Zeitkonto prüft zusätzlich lange Verzögerungen und begrenztes Aufholen.

Der gemeinsame App-Durchlauf besteht jeweils 8/8: Geschwindigkeit, Reset,
Projekteinstellungen, Themen, beide Menügrößen und vollständiger C-/Physim-Ablauf
mit Analyse, Export und Wiederöffnung. macOS: `run-wwivzhlo`; Debian: `run-78d35ayu`.
`speed_workflow` bedient die tatsächlichen Widgets in beiden Sprachen bei
1080 × 740 und prüft 0,25×, 4×, Wechsel auf 1× im laufenden Betrieb, Offline,
Pause, Einzelschritt, Reset und Wiederöffnung mit gespeicherter Geschwindigkeit.
Die Darstellung ist visuell geprüft; Messdateiformat und Modul-ABI bleiben gleich.
Nach der sofortigen RUN-Rückmeldung besteht der abschließende Durchlauf mit
Geschwindigkeit, Reset und beiden vollständigen App-Abläufen auf beiden Plattformen
(je 4/4): macOS `run-1j7s4k_5`, Debian `run-a7ock72l`.
Ein macOS-Zwischenlauf endete im internen Testfristpfad; ein zusätzlicher
Debian-Durchlauf wurde durch die nicht mehr auf SSH-Befehle antwortende VM unterbrochen.
Der erneute gemeinsame macOS-Lauf und der Debian-Lauf nach VM-Wiederherstellung
sind die abschließenden Nachweise. Die unterbrochenen Läufe werden nicht als bestanden gezählt.

Die [C17-CI zu `dec629f`](https://github.com/PhysicSimulator/physim/actions/runs/37271795266)
und die [Linux-Paket-CI](https://github.com/PhysicSimulator/physim/actions/runs/37271795260)
sind inzwischen erfolgreich abgeschlossen. Diese Nachweise gelten für den
vorherigen Reset-Stand und enthalten die neue Geschwindigkeitswahl noch nicht.

## Simulationsreset am 4. Oktober 2026

Die Reset-Steuerung wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Debug) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64, GCC 12.2,
SDL 3.2.30, Release) gebaut und ausgeführt. Linux verwendet X11/Xvfb/Openbox und
Mesa 22.3.6. Das ist kein neuer Windows-, Apple-Silicon-, Wayland- oder GPU-Nachweis.

`reset_workflow` startet vier Prozesse mit eigenen Projekten bei 1080 × 740:
C- und Physim-Unsicherheitsmodelle, Physim-Stoßmodell mit Reibung 0,25 und ein
C-Hängefall. Der Seed 18446744073709551615 und dt 0,125 bleiben erhalten.
Reale SDL-Bedienereignisse prüfen Reset während des Laufs und der Pause,
Einzelschritt, Fortsetzen, Stoppen und F7 nach Laufende. Anfangswerte und erster
Zeitschritt stimmen mit den gespeicherten Referenzkanälen überein. Pro Projekt
bleiben vier erfolgreiche Läufe erhalten; CRCs bestätigen die alten Dateien,
Quellcode-Snapshots und Grenzdateien sind zusätzlich geprüft.

Fehlende Quellen und ein vorübergehend entferntes Modul melden Fehler und erhalten
Laufverweis, Messwerte, Live-Verlauf und Analyse. Der C-Prüffall hängt tatsächlich
in `step` und später in `create`; die App beendet beide nach der Stop-Frist.
Vollständige Messblöcke des Hängefalls werden mit `PS_RECOVERED` geladen.
Die erneute Initialisierung gelingt nach Beseitigen der Fehlerursache. Geänderte
Editorquellen verhindern einen Reset. Der pausierte Anfangszustand ist visuell
geprüft; Kamera und kleine Fenster bleiben bedienbar.

Der gemeinsame Durchlauf besteht jeweils 8/8: Reset, Projekteinstellungen, Themen,
Plots, beide Menügrößen sowie vollständiger C- und Physim-App-Ablauf einschließlich
Analyse, Export und Wiederöffnung. macOS: `build/workspace-check/test-results/run-iu0zd7di`;
Debian: `build/native/Release/test-results/run-ohc6zzby` innerhalb der VM.
Die abschließende Prüfung der verzögerten Übernahme von Kanalnamen und Statusschema
besteht zusätzlich mit Reset sowie beiden vollständigen App-Abläufen (3/3):
macOS `run-37nsr8sl`, Debian `run-d9ylbk38`.

Die [Linux-Paket-CI zu `2afef89`](https://github.com/PhysicSimulator/physim/actions/runs/37236029010)
besteht inzwischen auf Debian 12 und Ubuntu 24.04. Dieser Paketnachweis bezieht
sich auf den vorherigen Stand mit Themen und enthält noch keinen Reset.

## Darstellungswechsel am 4. Oktober 2026

Die neue helle und kontrastreiche Darstellung wurde tatsächlich auf dem lokalen
Intel-Mac (macOS 14.6.1, Apple Clang 16, SDL 3.2.30, Debug) und in der Debian-12-VM
(Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, Release) gebaut und ausgeführt.
Debian verwendet X11/Xvfb/Openbox und Mesa 22.3.6. Das belegt keine Wayland- oder
physische GPU-Abnahme.

- `preferences` prüft die drei Paletten, Format-1-Migration, Grenzen, Prüfsumme,
  alle Kürzungen und Erhaltung bei Fehlern. Die neuen Text-, Syntax- und
  Kurvenlegendenfarben erreichen rechnerisch mindestens 4,5:1 auf Hell und 7:1
  auf Hoher Kontrast.
- `themes_workflow` startet je sieben App-Prozesse, bedient die echten Optionen
  und prüft Übernehmen, Abbrechen, Standardwerte und gespeicherte Neustarts.
  Der Wechsel erreicht das vorher geöffnete Dokumentationsfenster und bewahrt
  dessen Fonts. BMP-Ausgaben prüfen die Hintergrundfarbe in Editor, Einstellungen,
  Diagrammen mit drei Kurven und geladener Dokumentation bei 1080 × 740.
- Die bisherigen Abläufe für Einstellungen, Dokumentation, Diagramme, beide
  Menügrößen und kleine Dokumentansicht bestehen zusätzlich: macOS 8/8 mit
  UI-Rendererprüfung (`build/workspace-check/test-results/run-egjd2dld`),
  Debian 7/7 (`build/native/Release/test-results/run-soyj8nw8` innerhalb der VM).
  Die abschließende erweiterte Themenprüfung besteht separat auf macOS
  (`run-ba1lmh_4`) und Debian (`run-g_gz4yq3`). Die abschließende Menü- und
  Themenprüfung des fertigen Quellstands besteht jeweils 2/2:
  macOS `run-q5yc2y_x`, Debian `run-cek89xm5`. Der abschließende Kontrast-/Format-Test
  besteht auf macOS (`run-rflngm6f`) und Debian (`run-6b2meyq1`).
  macOS-Bilder wurden visuell geprüft.

Die Linux-Paket-CI des vorherigen Commits `fa311dc` besteht mittlerweile vollständig
auf [Debian 12 und Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/37234502965),
einschließlich der echten GTK-Dateidialoge. Diese CI enthält noch keine neuen Themen.

## Lokaler Intel-Mac am 4. Oktober 2026

macOS 14.6.1 (23G93), Intel x86_64, Apple Clang 16.0.0 und lokal aus
`release-3.2.30` gebautes SDL prüfen jetzt zusätzlich die Workspace-Dokumentansichten.
Der Debug-Modelltest besteht mit Format-1-Kompatibilität, maximalen Pfad-/Dokumentgrenzen,
abgeschnittenen und manipulierten Dateien sowie Erhaltung des bisherigen Zustands
bei Fehlern (`build/workspace-check/test-results/run-80qn96q7`). Die Fensterprüfung
startet getrennte App-Prozesse für Projekt-/Ordner-Wiederöffnung, aktive Dokumente,
Analyse-/Simulationsansicht, fehlende und extern gekürzte Unicode-Dateien sowie
vorhandene Autosaves. Dateibaum, beide Dokumentfenstergrößen, Buildinvalidierung
und Dokument-Autosaves sind zusätzlich geprüft. Diese lokalen Prüfungen sind kein
neuer Linux- oder Apple-Silicon-Nachweis. Alle sechs Fensterfälle bestehen unter
`build/workspace-check/test-results/run-sg5hhnt6`.

Der SDL-freie Gesamtlauf des Ausgangsstands `42d1f9e` besteht 480 von 482 Fällen
(`build/native/Debug/test-results/run-vsag2ez2`).
`language_function_values_runtime` wird vor der Programmausführung mit Signal 9
beendet; das macOS-Systemprotokoll nennt `AppleSystemPolicy` und
`Security policy would not allow process`. Eine separat ad hoc signierte Kopie
besitzt eine gültige Signatur, wird aber ebenfalls blockiert. Eine Ursache im
Sprachprogramm ist damit nicht nachgewiesen. `batch_reference` scheitert an der
Prüfung der abgeschlossenen Läufe bzw. der Fünf-Sekunden-Grenze; die Fehlerjournale
nennen Runner-Zeitüberschreitungen. Beide gezielten Wiederholungen scheitern erneut
(`run-6x4lxg2j`).

Beide Menüfenstergrößen scheitern in einem gemeinsamen lokalen Lauf; die jeweils
isolierten Wiederholungen bestehen (`run-t9h7c5ep`, `run-zij55d1b`). Eine separat
aus `42d1f9e` exportierte und gebaute App scheitert ebenfalls im kleinen Menütest
(`build/baseline-app/test-results/run-mozmzzn3`). Die wechselnden Fehlerstellen
und eine mögliche Beeinflussung durch native Fokus-/Mausereignisse bleiben offen.
Die bisherigen macOS-15-CI-Ergebnisse belegen keine Fehlerfreiheit auf macOS 14.6.1.

Die anschließende Diagnose zeigt beim Batchtest für alle drei Fehlerfälle
Zeitüberschreitungen nach ungefähr 0,25 Sekunden, einschließlich des normalen
Schemafixtures. Die Fehlerprüfungen lassen Absturz und Schemaprüfung nun fünf
Sekunden zum Starten, verlangen ausdrücklich `PS_IO` bzw. `PS_INVALID` und behalten
für den Hängefall 0,25 Sekunden und `PS_LIMIT`. Die bisherige Gesamtdauergrenze
von fünf Sekunden sowie die Abschlusszahlen bleiben geprüft. Alle drei Fehlerarten
bestehen (`build/native/Debug/test-results/run-bjamthli`); die parallele Batchreferenz
besteht separat (`run-mkysw5d1`).

Natürliche Maus-/Fokusereignisse sind im Menütest nach dem Hilfefenster protokolliert.
Absichtlich eingeschobene Ereignisse reproduzieren den Prüffehler unter
`build/toolbar-noise-before.log`. Die Teststeuerung isoliert jetzt ihre Mauskennung
auch in den Menüfällen. Mit zusätzlichen Maus-, Mausrad- und Fokusereignissen
bestehen die vollständigen kleinen und großen Menüabläufe, die normalen Menütests,
Plot-Eingabeisolation und Fensterstart (5/5 Fälle unter
`build/workspace-check/test-results/run-23gu5c67`). Der SDL-Eingabetest prüft weiterhin
die normale Freigabe gehaltener Tasten/Maustasten bei Fokusverlust.

Der anschließende vollständige lokale Debug-Build besteht 492 von 493 Tests ohne
Fenster (`build/workspace-check/test-results/run-7288kqxh`), einschließlich
Batchtests, nativen Projektbuilds und Benchmarks. Ausschließlich
`language_function_values_runtime` bleibt durch macOS-Systempolitik blockiert.
Dasselbe Sprachprogramm besteht lokal im Release-Build
(`build/native/Release/test-results/run-203vuguu`).

Die Linux-Paketprüfung zu `1b098d7` baut das Paket und besteht dessen Start sowie
SDK und alle neun GUI-Projektabläufe auf Debian 12 und Ubuntu 24.04. Beide Jobs
scheitern weiterhin am nativen Datei-/Ordnerdialog:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/37223668792/job/111500303931),
[Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/37223668792/job/111500303988).
Die Jobannotationen zu `68bb8a4` bestätigen auf beiden Distributionen, dass der
erste Ordnerdialog gefunden und bedient wird, aber vor der nächsten Dateianfrage
offen bleibt:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/37225049564/job/111504289877),
[Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/37225049564/job/111504289907).
Dieser CI-Lauf bestätigt die Dialogabnahme noch nicht; der folgende lokale
Nachweis prüft die anschließende Korrektur.

Der [C17-Lauf zu `68bb8a4`](https://github.com/PhysicSimulator/physim/actions/runs/37225049628)
besteht alle Build-, Test-, Sanitizer-, Fuzzer- und SDK-Schritte in den acht
Plattformkombinationen, einschließlich der jetzt 36 Grafik-/Fensterfälle.
Im Intel-macOS-Job scheitern anschließend beide Artefakt-Uploads mit
`ArtifactService/CreateArtifact`-Zeitüberschreitung. Der gesamte Job ist dadurch
fehlgeschlagen; seine Prüfungen sind bestanden, die Ergebnisarchive fehlen.

## Lokale Linux-Dialogprüfung am 4. Oktober 2026

Eine lokale QEMU-10.1.0-VM mit HVF auf dem Intel-Mac läuft mit Debian 12,
Linux `6.1.0-53-cloud-amd64` und GCC 12.2.0. SDL 3.2.30 und Physim sind dort
aus Quellen als Release gebaut. Der bisherige Dialogprüfer reproduziert den
CI-Hänger (`build/linux-vm/dialog-baseline.log`). Nach funktionierender Auswahl
zeigt die echte Abbruchprüfung einen App-Fehler: SDLs Zenity-Backend liefert
einen leeren ersten Pfadstring, den Physim als Ordnerpfad öffnet. Die App
behandelt diesen Rückgabewert nun wie eine leere Dateiliste als Abbruch;
Fehler mit einer NULL-Dateiliste bleiben als Fehler gemeldet.

Der korrigierte Prüfer verwendet einen privaten Home-/Konfigurationsbereich,
eine D-Bus-Sitzung und die tatsächlichen AT-SPI-Namen und Fensterkoordinaten der
GTK-Dialogelemente. XTest klickt diese Elemente; es werden keine SDL-Ergebnisse
oder Callback-Pfade eingespeist. Eine direkte Pfadeingabe mit schnellen
Unicode-Tastaturzuordnungen und die anfängliche Recent-Ansicht entfallen.

Das verschobene Release-SDK in `Installed package ä/Physim` besteht die tatsächliche
Ordnerauswahl, externe Datei, Zusatzordner, Abbruch mit unverändertem Workspace und
Status sowie den getrennten Test eines nicht vorhandenen Dialogtreibers:

| Umgebung | Dialogbibliotheken | Protokoll |
| --- | --- | --- |
| Debian 12 in der VM | Zenity 3.44.0, GTK 3.24.38, Mesa 22.3.6 | `build/linux-vm/debian-final-dialogs.log` |
| Ubuntu 24.04 im Container innerhalb derselben VM | Zenity 4.0.1, GTK 4.14.5, Mesa 25.2.8 | `build/linux-vm/ubuntu-final-dialogs.log` |

Dialogbilder, `actions.json`, das abschließende Workspace-Bild und
`unavailable.log` liegen unter `build/linux-vm/dialog-evidence`. Die App-Korrektur
baut zusätzlich lokal mit Apple Clang 16.0.0; Workspace-Dateibaum und
Workspace-Neustart bestehen auf macOS 14.6.1 (2/2 Fälle unter
`build/workspace-check/test-results/run-l9i4rk48`).

Diese Linux-Prüfungen verwenden X11/Xvfb und Mesa llvmpipe. Sie sind ein tatsächlich
ausgeführter Dialognachweis für die genannten GTK-Versionen, aber kein Test von
Portal-/Wayland-Dialogen oder realer Grafikhardware. Der erneute Paket-CI-Lauf
mit dem korrigierten Prüfer steht noch aus. Dessen zusätzliche AT-SPI-/Python-
Testabhängigkeiten werden erst nach dem Paketstart ohne Compiler/Python installiert.

## Windows-ClangCL-Sanitizer

Aktuell offener Fehler: Im
[Windows-ClangCL-Debug-Job zu `0321c85`](https://github.com/PhysicSimulator/physim/actions/runs/36504185474/job/109201756506)
bestehen alle 493 normalen Tests einschließlich der neuen Projektcache-Reparatur.
Die Sanitizer-Probe besteht ebenfalls; danach endet der instrumentierte Core-Test
mit Windows-Zugriffsverletzung `0xC0000005` ohne Diagnoseausgabe. Die übrigen neun
Sanitizer-Tests bestehen. Die Ursache dieses Absturzes ist noch ungeklärt;
der gesamte Job ist damit fehlgeschlagen. Das lokale Protokoll liegt unter
`build/ci-cache-clang-debug.log`. Der parallele
[Job desselben Commits im zweiten Repository](https://github.com/Lulus792/physim/actions/runs/36504182060/job/109201744452)
besteht alle zehn Sanitizer-Tests mit derselben Windows-Image-Version
`20260920.314.1`. Der Fehler tritt damit nicht in jedem Lauf auf; die Ursache
bleibt offen. Das Vergleichsprotokoll ist `build/ci-cache-clang-debug-origin.log`.

Lokal scheitert der Core-Test mit ClangCL 19.1.5 ebenfalls mit `0xC0000005`;
hier meldet die Laufzeit zusätzlich eine unbekannte Interceptor-Instruktion.
Der mit [Microsoft ProcDump](https://learn.microsoft.com/en-us/sysinternals/downloads/procdump)
erfasste Minidump zeigt die Fehleradresse in `clang_rt.asan_dynamic-x86_64.dll`
bei Offset `0x4859b`, innerhalb von `__asan_region_is_poisoned`.
Das ist noch kein Nachweis derselben Ursache wie im CI-Lauf ohne Diagnoseausgabe.
Protokolle: `build/clang-asan-core.log`, `build/clang-asan-core-verbose.log`;
Speicherabbild: `build/clang-asan-dumps/physim-test-core.exe_260929_030111.dmp`.

Die Windows-Debug-Jobs konfigurieren nun
[WER-Minidumps pro Testprogramm](https://learn.microsoft.com/en-us/windows/win32/wer/collecting-user-mode-dumps)
für `physim-test-core.exe` und einen eigenen Absturzprüfer. Der Prüfer verlangt
eine echte Zugriffsverletzung und denselben Fehlercode im erzeugten Minidump.
Das Artefakt `windows-crash-diagnostics-ClangCL` beziehungsweise
`windows-crash-diagnostics-v143` enthält Dumps, Testprogramm, PDB und Laufzeit-DLLs.
Die Aufnahme erfolgt während des ursprünglichen Testlaufs; fehlerhafte Tests
werden nicht automatisch wiederholt. Lokal sind der Prüfer kompiliert, sein
Absturz mit ProcDump erfasst und die Dump-Auswertung geprüft
(`build/crash capture check 1tq7fixm`). Im Lauf zu `620246b` bestehen die echte
WER-Aufzeichnung und alle zehn Sanitizer-Tests sowohl mit
[ClangCL](https://github.com/PhysicSimulator/physim/actions/runs/36506432739/job/109208791289)
als auch mit [MSVC](https://github.com/PhysicSimulator/physim/actions/runs/36506432739/job/109208791471).
Die Protokolle sind `build/ci-wer-clang.log` und `build/ci-wer-msvc.log`.
Der ursprüngliche sporadische Absturz gilt damit noch nicht als behoben.
Lokal wurden keine administrativen WER-Einstellungen verändert.

## Linux

**Ubuntu 24.04 mit GCC und Clang ist für die geprüften Arbeitsabläufe bestätigt.**
Die erfolgreichen Jobs für
[GCC](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553336)
und [Clang](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054552812)
prüfen den Quellstand `0a27cef` mit AddressSanitizer und UndefinedBehaviorSanitizer:

- 276 Tests ohne Fenster, einschließlich Sprachcompiler, Physik, Daten,
  Runner, nativen Projektbuilds und Fehlerbehandlung.
- 35 Grafik- und Fensterabläufe mit X11, Xvfb, Openbox und Mesa: darunter
  Menüwechsel, Tabs, Fensterknöpfe, Workspace, Dokumente, Wiederherstellung,
  Projekteinstellungen und C-/Physim-Projekte.
- Verschobenes installiertes SDK: Vorlagen und Sprachbeispiele neu bauen,
  Experimente und Analysen ausführen, Messdateien und Berichte prüfen.
- Acht vollständige C-App-Abläufe: Pendel, Wurf, Stoß, Bodenkontakt, Feder,
  unsicherer Wurf, Boxstoß und Auftrieb. Die Projektpfade enthalten Leerzeichen
  und Unicode. Der Ablauf umfasst Buildfehler, korrigierten Build, Simulation,
  Pause/Einzelschritt, Analyse, Exporte und Wiederöffnung gespeicherter Ergebnisse.

Nutzerprojekte werden direkt mit `physim-build` gebaut und benötigen keine
`CMakeLists.txt`. Quellen und `physim.project` bleiben im Projektordner;
Buildprodukte liegen in `build/Debug` beziehungsweise `build/Release`.

Das Release-Paket besteht inzwischen auch die unten beschriebene Prüfung auf
frischen Debian-12- und Ubuntu-24.04-Systemen. Wayland, weitere Distributionen,
reale Linux-Grafiktreiber und die Integration in einen echten Desktop bleiben offen.
Äußere Fensterecken hängen unter Linux vom Desktop ab; die Windows-DWM-Rundung
ist kein plattformübergreifender Nachweis.

### Release-Paket auf frischen Linux-Systemen

Der zusätzliche Workflow `linux-package.yml` baut ein Release-SDK mit GCC unter
Debian 12 (`debian:bookworm`) und prüft dort alle Tests ohne Fenster. Das Archiv
`Physim-linux-x86_64.tar.gz` wird anschließend in zwei neue Container entpackt:
Debian 12 und Ubuntu 24.04. Diese Prüfungen haben keinen Repository-Checkout,
keine SDL-Entwicklungsdateien und kein CMake. Zunächst werden nur Desktopbibliotheken
installiert und die Oberfläche ohne Compiler oder Python gestartet. Anschließend
kommen ein C17-Compiler für Nutzerprojekte und Python für die Teststeuerung hinzu.

Ein separates Prüfpaket enthält nur den SDK-Prüfer, dessen Compilerhelfer und
kleine Prüfquellen. Der Prüfer verwendet die Header, Bibliotheken, Vorlagen und
Kernquellen aus dem verschobenen SDK. Er baut alle C-/Physim-Beispiele und Projekte
und führt alle acht C-Vorlagen sowie den vollständigen Physim-Sprachablauf in der
Oberfläche aus. Die Systempakete, aufgelösten App-Bibliotheken, Laufprotokolle und
Screenshots werden als `installed-linux-debian-12` und `installed-linux-ubuntu-24.04`
archiviert. Das Release-Archiv heißt als CI-Artefakt `physim-linux-release-x86_64`.

Der [Debian-12-Release-Build zu `0dbe24f`](https://github.com/PhysicSimulator/physim/actions/runs/36505336425/job/109205387725)
besteht alle 493 Tests ohne Fenster und erzeugt das Paket. Die anschließenden
beiden Installationsprüfungen stoppen vor dem App-Start, weil Openbox Python
als Abhängigkeit installiert hatte. Openbox wird jetzt erst nach dem Starttest
installiert; die Bedingung, dass Python beim ersten Start fehlt, bleibt erhalten.
Im Lauf zu `620246b` besteht der App-Start ohne Compiler, Python und CMake in
beiden frischen Systemen:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/36506432788/job/109210313653)
und [Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/36506432788/job/109210313628).
Auch die vollständige SDK-/Projektprüfung besteht auf beiden Systemen: verschobenes
SDK, unabhängige Header und Kernbibliothek, 15 Sprachprogramme, 27 Sprachmodule,
neun neu gebaute Projekte sowie alle acht C-Vorlagen und der vollständige
Physim-Sprachablauf in der Oberfläche. Die Protokolle liegen unter
`build/ci-installed-debian-620246b.log` und `build/ci-installed-ubuntu-620246b.log`.
Das heruntergeladene Paket zu `0dbe24f` hat außerdem die Prüfung der ZIP-/TAR-Struktur,
Ausführungsrechte der App und aller 224 Dateiprüfsummen bestanden.

Die Container verwenden X11, Xvfb und Mesa; sie prüfen die Paketabhängigkeiten
und Anwendungsabläufe, aber keine echte Grafikhardware oder Wayland-Sitzung.
Die native Datei-/Ordnerauswahl ist noch separat zu prüfen. Für Desktops ohne
XDG-Portal enthalten Installationsanleitung und folgende Paketläufe nun Zenity;
[SDL 3.2.30](https://github.com/libsdl-org/SDL/blob/release-3.2.30/src/dialog/unix/SDL_unixdialog.c)
verwendet Portal oder Zenity für diese Dialoge.

Auch der [Paketlauf zu `ae31aee`](https://github.com/PhysicSimulator/physim/actions/runs/36507609163)
besteht in beiden Distributionen mit Zenity als installierter Laufzeitabhängigkeit.
Der neue separate Dialogtest steuert echte Zenity-Fenster mit `xdotool`: Hauptordner
öffnen, externe Datei und zusätzlichen Ordner hinzufügen, anschließend abbrechen.
Alle ausgewählten Pfade enthalten Leerzeichen und Umlaute. Die App prüft die
übernommenen Pfade und den unveränderten Workspace nach Abbruch. Ein zweiter Lauf
erzwingt einen nicht vorhandenen Dialogtreiber und prüft die Fehlermeldung, ohne
den Workspace zu verändern. Protokolle, ausgewählte Pfade und ein abschließendes
Bild liegen im Artefakt unter `Native dialogs*`. Der Linux-Ausführungsnachweis
für diesen neuen Dialogtest steht bei diesem Commit noch aus. Der aktuelle lokale
Nachweis für GTK 3 und 4 steht oben unter „Lokale Linux-Dialogprüfung“.

Im [Dialoglauf zu `bd359c7`](https://github.com/PhysicSimulator/physim/actions/runs/36509517062)
bestehen auf beiden Systemen alle neun SDK-Oberflächenabläufe. Der Dialogprüfer
findet das erste Zenity-Fenster, bleibt aber nach der Ordnerpfadeingabe bis zum
Zeitlimit darin. Er bestätigt jetzt ausdrücklich mit Zenitys OK-Schaltfläche
(`Alt+O`), statt Enter im GTK-Pfadfeld zu verwenden, und archiviert Bilder vor
und nach der Eingabe sowie bei Fehlern. Die erneute Ausführung bleibt offen.
Protokolle: `build/native-linux-dialog-109220128459.log` und
`build/native-linux-dialog-109220128487.log`.

Der gleiche interaktive App-Test besteht lokal unter Windows mit MSVC Debug und
den tatsächlichen Systemdialogen (`build/native-dialog-windows-3.log`,
`build/native-dialog-windows-3/native-dialogs.bmp`). Der Pfadvergleich berücksichtigt
Windows-Pfadtrenner. Die macOS-Systemdialoge und Linux-Portaldialoge sind damit
noch nicht geprüft.

## Windows

Die [CI-Matrix für `0a27cef`](https://github.com/PhysicSimulator/physim/actions/runs/36459604565)
bestand in allen vier Windows-Jobs: MSVC und ClangCL jeweils in Debug und Release,
einschließlich Tests und installiertem SDK. Fenster- und Grafiktests werden
zusätzlich lokal unter Windows ausgeführt; die GitHub-Windows-Worker garantieren
keinen OpenGL-3.3-Treiber.

## macOS

macOS auf Apple Silicon und Intel ist ein verbindliches Plattformziel.
Die CI verwendet `macos-15` und `macos-15-intel` mit Apple Clang und SDL 3.2.30.
Für `0a27cef` haben der
[Apple-Silicon-Job](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553102)
und der [Intel-Job](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553310)
alle 276 Tests ohne Fenster, 35 Grafik-/Fenstertests, das verschobene SDK,
und die acht C-App-Abläufe bestanden. Die Paket-Signierreihenfolge wurde anschließend
für Intel korrigiert. Der separate
[Paketlauf für `2de20b6`](https://github.com/PhysicSimulator/physim/actions/runs/36462932066)
bestand auf beiden Architekturen: neu gebaut, signiert, Signatur geprüft,
in einen Pfad mit Leerzeichen und Umlaut verschoben und daraus vollständige
C- und Physim-Sprachprojekte gebaut, simuliert und ausgewertet.

Die Implementierung berücksichtigt Darwins Programmpfaderkennung, Mach-O-Module,
lokalisierte Zahlenkonvertierung, macOS-Systemschriften und OpenGL 4.1 Core.
`tools/package-macos.py` erzeugt ein verschiebbares `.app`-Paket mit SDL und SDK.
Die lokale Ad-hoc-Signatur wird geprüft; Developer-ID-Signierung und Notarisierung
für eine öffentliche Verteilung sind noch offen. Die CI prüft vollständige C- und
Physim-Sprachprojekte aus dem verschobenen Paket.
Die geprüften ZIP-Dateien stehen im damaligen Paketlauf als `physim-app-macos-15`
(Apple Silicon) und `physim-app-macos-15-intel` bereit. Neue direkte Builds verwenden
`physim-native-macos-15` beziehungsweise `physim-native-macos-15-intel`.
Die CI-Pakete sind Debug-Entwicklungsstände. Weitere macOS-Versionen, echte
Mac-Grafikhardware und Installation auf einem frischen Mac bleiben separate
Abnahmen; die gehosteten Grafiktests verwenden Apples Software Renderer.

Eine zusätzliche Paketprüfung startet jetzt die verschobene `.app` über
`/usr/bin/open -W -n -a`, also über macOS LaunchServices. Sie verwendet ein leeres
Arbeitsverzeichnis, den Systempfad `/usr/bin:/bin:/usr/sbin:/sbin` und entfernt
Entwickler-Vorgaben für Compiler, SDK und Bibliotheken aus der Startumgebung.
Die bisherigen Paketprüfungen starteten direkt `Contents/MacOS/physim`.
Der neue Prüfer verlangt für C und Physim einen vollständigen Oberflächenablauf,
einen erfolgreichen Abschluss nach dem Aufräumen der App, gebaute Module,
gespeicherte Messdaten/Berichte und Screenshots. Abschließend muss die Signatur
des Pakets weiterhin gültig sein. Protokolle und Bilder werden unter
`LaunchServices*` archiviert. Der tatsächliche macOS-Nachweis dieser neuen
Startvariante steht noch aus; native macOS-Dateidialoge bleiben separat offen.

## Direkter Build von Physim

Der Quellstand `7f05668` ergänzt `tools/build.py`. Im
[CI-Lauf](https://github.com/PhysicSimulator/physim/actions/runs/36467398347)
hat der Schritt **Direct compiler build without CMake for Physim** auf allen acht
Kombinationen bestanden: Windows mit MSVC/ClangCL jeweils Debug/Release,
Ubuntu 24.04 mit GCC/Clang sowie macOS 15 auf Apple Silicon/Intel.
Dies belegt diesen abgeschlossenen Schritt; die übrigen Schritte des Laufs
waren beim Erfassen dieses Nachweises noch aktiv.

Die Prüfung baut Bibliotheken, Sprachcompiler, Runner, Projektbuilder und App
mit direkten Compiler-/Archiviereraufrufen. Drei Referenzprogramme prüfen Core,
Numerik und Mechanik. Ein separater Test mit echten Compilerprozessen prüft
Unicodepfade, Headeränderungen, unveränderte Ausgaben, Fehlerwiederaufnahme,
beschädigte Ausgabedateien und die Buildsperre. Linux und beide Macs führen
zusätzlich vollständige Pendel- und Physim-Sprachprojekte in der direkt gebauten
App aus. Windows-Grafikabläufe werden lokal geprüft: beide vollständigen
Projektabläufe mit MSVC Debug sowie Renderer und Eingaberegression mit Clang.
Der lokale Projektbuilder-Test besteht mit den direkt gebauten Werkzeugen für
C- und Physim-Projekte in Debug/Release, einschließlich Projektverschiebung.

Die vollständige Testsuite und SDK-/App-Paketierung sind weiterhin CMake-gestützt.
SDL wird für diese CI-Prüfung zuvor mit seinem eigenen CMake-Buildsystem gebaut.
Der direkte Physim-Build ist daher noch keine vollständige Abnahme des Ziels,
CMake aus der gesamten Entwicklung und Auslieferung zu entfernen.

## Direkte SDK- und App-Pakete

Für `d9f920e` hat der Schritt **Native SDK package and relocation** im
[Paketprüflauf](https://github.com/PhysicSimulator/physim/actions/runs/36470030478)
auf allen vorgesehenen Plattformen bestanden:

| Umgebung | Nachweis |
| --- | --- |
| Windows MSVC Release | [Job 109089656019](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089656019) |
| Windows ClangCL Release | [Job 109089656123](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089656123) |
| Ubuntu 24.04 GCC | [Job 109089655982](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655982) |
| Ubuntu 24.04 Clang | [Job 109089655923](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655923) |
| macOS 15 Apple Silicon | [Job 109089655991](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655991) |
| macOS 15 Intel | [Job 109089655751](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655751) |

Der direkte Installer verwendet kein CMake. Die SDK-Prüfung verschiebt das Paket,
prüft seine Inhaltsprüfsummen, bindet alle öffentlichen Header einzeln ein und
kompiliert einen unabhängigen Prüfer gegen die installierte Kernbibliothek.
Alle acht mitgelieferten C-Beispielmodule und 15 neu übersetzte Sprachprogramme
werden ausgeführt. Anschließend baut `physim-build` alle acht C-Vorlagen und ein
Physim-Projekt aus den installierten Quellen neu. Der Prüfer kontrolliert die
Messdateien mit jeweils 201 Zeilen und die erzeugten Berichte.

Linux und beide Macs bestehen zusätzlich vollständige C-/Physim-App-Abläufe aus
dem verschobenen SDK. Beide Mac-Jobs erzeugen daraus eine `.app`, prüfen deren
Ad-hoc-Signatur und führen die App-Abläufe erneut aus diesem Paket aus.
Windows besteht die Grafikabläufe lokal auf der RTX 2080 Ti; die gehosteten
Windows-Paketprüfungen verwenden die Kommandozeilenprogramme.

Die heruntergeladenen Linux-/Mac-Archive wurden zusätzlich auf Dateiintegrität,
ausführbare Dateirechte, Architektur und SDK-Inhalt geprüft. Mac-Bundles enthalten
die relative SDK-Verknüpfung und SDL. Artefakte im Lauf:
`physim-native-windows-v143`, `physim-native-windows-ClangCL`,
`physim-native-linux-gcc`, `physim-native-linux-clang`,
`physim-native-macos-15`, `physim-native-macos-15-intel`.
Die Mac-/Linux-CI-Pakete sind Debug-Builds; Windows verwendet Release.
Weitere Betriebssystemversionen, Clean-Machine-Abnahme und öffentliche
Mac-Notarisierung bleiben offen.

## Direkter Testläufer

Der direkte Katalog umfasst **493 Tests ohne Fenster**, davon 482 ohne SDL,
und zusätzlich **35 Fenster- und Grafiktests** unter `--test-display`.
Sieben zuvor übertragene Prüfungen
für Tutorialquellen, API-Referenz, PNG-Dekodierung und Projektbuild bestehen lokal
mit MSVC Debug, Clang Release und über CTest. Zum Stand `a051ea0` besteht auch der
direkte Build- und Testschritt in allen acht CI-Kombinationen.
Die vorherige Erweiterung übernimmt 21 Runner-, Physik- und Dokumentationsabläufe.
Alle 21 bestehen lokal mit
MSVC Debug und Clang Release; alle 47 gemeinsamen CTest-Integrationsprüfungen
bestehen ebenfalls. Für den CTest-Vergleich wurden die vorhandenen Runner neu
gebaut, nachdem zwei Crash-Tests mit veralteten Programmen fehlgeschlagen waren.
Die 419 direkten Sprachtests bestanden bereits zum vorherigen Stand `b95e119` mit
beiden Compilern. Zum Stand `7e38cf1` bestehen alle 484 damaligen Tests in allen
acht CI-Kombinationen. Der anschließende Build ohne Tests scheitert dort an der
Argumentprüfung für `--test-filter`; der direkte CI-Schritt ist daher fehlgeschlagen.
Die Korrektur besteht lokal für normale Builds mit MSVC und Clang. Ein zusätzlicher
Regressionstest prüft normale Builds, SDK-Installation und Testfilter über den
Kommandozeileneinstieg. Die Korrektur ist mit dem direkten CI-Schritt zu `a051ea0`
auf allen acht Kombinationen bestätigt.

Alle 34 neuen Grafikabläufe sind lokal mit MSVC Debug auf der RTX 2080 Ti geprüft.
Der erste Lauf bestand 30 Fälle; nach Korrektur der UTF-8-Ausgabe und der Vorbereitung
zweier Testordner bestanden die vier betroffenen Fälle ebenfalls. Die Prüfungen
behalten Menü-, Dokument-, Wiederherstellungs-, Projekteinstellungs- und Sprachabläufe
bei. Beide Diagrammtests prüfen zusätzlich alle PNG-Exporte mit unabhängigen
Pixelvergleichen. Acht ausgewählte Abläufe bestehen außerdem mit der Clang-Release-App:
Auftrieb, Diagramme, Stapelläufe, Dokumentbuild, Projekteinstellungen,
Dokumentwiederherstellung, Autosave und der vollständige Sprachablauf.
Die Projekt-Buildprotokolle bestätigen dabei Clang als Compiler.
Drei zusätzliche CTest-Vergleiche für `toolbar_small`, `plot_workflow` und
`language_mixed_workflow` bestehen mit den neu gebauten MSVC-Debug-Programmen.
Im [CI-Lauf zu `a621b5b`](https://github.com/PhysicSimulator/physim/actions/runs/36488141869)
besteht der direkte Grafikschritt auf macOS Apple Silicon. Linux GCC und Clang
bestehen jeweils 33 von 34 Fällen; der erste Auftriebstest endet nach 150 Sekunden
ohne App-Ausgabe im Timeout. Die Ursache ist noch offen. Der Linux-Aufruf wartet
nun vor dem App-Start auf den Fenstermanager; zusätzliche App-Testprotokolle halten
Initialisierung und Phasenwechsel auch bei Zeitüberschreitung fest. Die erneute
Linux-CI und der Grafiknachweis für macOS Intel standen bei diesem Stand aus.

Im [Folgelauf zu `37eef01`](https://github.com/PhysicSimulator/physim/actions/runs/36490044371)
besteht der direkte Grafikschritt mit allen 35 Fällen unter Linux GCC und Clang
sowie macOS Apple Silicon und Intel. Damit besteht dort auch der neue UI-Benchmark.
Der erste Linux-Auftriebstest läuft mit der Fenstermanager-Wartebedingung durch;
ein eindeutiger Ursachennachweis für den vorherigen Timeout folgt daraus nicht.
Auch der direkte Build- und Testschritt mit 493 Tests besteht auf allen acht
Plattformkombinationen dieses Stands.

Die drei ergänzten Benchmark-Prüfungen bestehen lokal mit MSVC Debug, Clang Release
und über CTest. Der SDL-freie MSVC-Release-Build besteht beide Prüfungen ohne Fenster.
Die Referenzbilder für leere Ansicht, viele Bedienelemente und acht Kurven sind
zwischen den beiden Compilern bytegleich. In allen drei Messfällen entstehen nach
dem Aufwärmen keine weiteren Zeichenpuffer-Allokationen. Die bisherigen Prüfungen
für Export, Größenwechsel, Referenzwerte und Vergleichsfehler bleiben erhalten.
Dies sind Funktionsprüfungen; die parallel zu Builds gemessenen Zeiten sind keine
Performance-Baseline. Der direkte Grafikschritt des Folgelaufs bestätigt den
UI-Benchmark inzwischen auf den oben genannten vier CI-Kombinationen.

Der [Linux-GCC-Lauf zu `c92dbd3`](https://github.com/PhysicSimulator/physim/actions/runs/36491788018)
besteht erneut alle 34 App-Abläufe, scheitert aber im UI-Benchmark am Größenwechsel.
Das archivierte `empty-restored.bmp` hat noch 640 × 480 statt 1080 × 740 Pixel.
Der Benchmark wartet nun mit `SDL_SyncWindow` auf die Größenänderung und prüft
die tatsächliche Fenstergröße, bevor er zeichnet. Lokal besteht die korrigierte
Prüfung mit MSVC Debug und Clang Release. Der erneute Linux-Nachweis steht noch aus.

Der direkte Builder unterstützt zusätzlich `--sanitizers`: ASan unter Windows,
ASan und UBSan unter Linux/macOS. Lokal bestehen mit MSVC die positive
Instrumentierungsprobe (fehlerfreier Lauf und erkannter Heap-Pufferüberlauf) sowie
zehn Tests für Core, Speicherbesitz, Berichte, Protokoll-/Messdateimutationen und
die vier Sprachspeicherprüfungen. Die Ergebnisdatei liegt unter
`build/native/Debug-sanitized/test-results/run-1t_t37re/results.json`.
ClangCL 19.1.5 scheitert lokal schon beim Start des sicheren ASan-Probeprogramms
in seiner Interception-Laufzeit. Deshalb ist dafür keine lokale Abnahme belegt.
Im [CI-Lauf zu `c92dbd3`](https://github.com/PhysicSimulator/physim/actions/runs/36491788018)
bestehen die Sanitizer-Probe und alle zehn ausgewählten Tests unter Windows
mit MSVC und ClangCL. Die Linux-/macOS-Sanitizer-Abnahme steht noch aus.

Der [CI-Lauf zu `a9accf5`](https://github.com/PhysicSimulator/physim/actions/runs/36492484066)
bestätigt außerdem den direkt gebauten IPC-libFuzzer unter Windows ClangCL und
Linux Clang. Beide Prüfungen erzeugen gültige Eingaben, spielen sie erneut ab
und bestehen 10.000 Durchläufe mit zusätzlicher Codeabdeckung ohne Fehlerfund.
Der Apple-Silicon-Job scheitert mit Apple Clang aus Xcode 16.4 am Linken, weil
`libclang_rt.fuzzer_osx.a` in der Toolchain fehlt. Für die Fuzzer-Prüfung wird nun
Homebrew LLVM 20 auf beiden Mac-Architekturen verwendet. Normale App-Builds
verwenden weiter Apple Clang.

Im [Lauf zu `0d10940`](https://github.com/PhysicSimulator/physim/actions/runs/36493259775)
besteht die Fuzzer-Kampagne mit LLVM 20 auf macOS Intel. Auf
Apple Silicon meldet Apples Linker `invalid r_symbolnum=1` für instrumentierte
Objekte. Der direkte Fuzzer-Build verwendet nun LLVMs Mach-O-Linker `ld64.lld`;
im [Lauf zu `f6cedbf`](https://github.com/PhysicSimulator/physim/actions/runs/36494602802)
bestehen damit beide Mac-Fuzzer-Kampagnen einschließlich 10.000 Durchläufen.

Die direkte Linux-Clang-Sanitizer-Suite zu `c92dbd3` besteht 492 von 493 Tests.
UBSan findet im Clipboard-Test einen `memcmp`-Aufruf mit Nullzeiger bei Länge null.
Der Vergleich behandelt leere Texte nun vor dem Speichervergleich; lokal bestehen
die Clipboard-Prüfungen mit MSVC Debug und Clang Release. Im Lauf zu `f6cedbf`
bestehen alle 493 Sanitizer-Tests ohne Fenster unter Linux GCC und Clang.
Der Mac-ARM-Lauf zu `c92dbd3`
Stands besteht 479 von 482 Sanitizer-Tests. Beim erneuten Laden von Modulen melden
zwei Fälle doppelt registrierte ASan-Globals; `template_ids` bricht in der
ASan-Registrierung ab. Die folgende Minimalprüfung grenzt die Ursache ein.
Eine zusätzliche Minimalprüfung lädt zwei instrumentierte Module je 32-mal,
kontrolliert das Zurücksetzen veränderter Globals und verlangt anschließend
einen erkannten globalen Pufferüberlauf. Lokal besteht sie mit MSVC unter
`build/native/sanitizer probe ä edntxgm2`. Im
[Lauf zu `4339411`](https://github.com/PhysicSimulator/physim/actions/runs/36495445390)
besteht sie auch mit Apple Clang auf Intel, reproduziert aber auf Apple Silicon
die doppelte Registrierung von `probe_globals`. Die archivierte Assemblerdatei
zeigt dort `__mod_term_func` für den ASan-Destruktor. Die LLVM-Umstellung auf
[`__cxa_atexit`](https://reviews.llvm.org/D121327) vermeidet diese veraltete
Darstellung. Die macOS-Sanitizer-CI verwendet nun LLVM 20 und LLD wie der Fuzzer;
im [Lauf zu `69ddae7`](https://github.com/PhysicSimulator/physim/actions/runs/36495859541)
besteht die vollständige Minimalprüfung auf Apple Silicon und Intel, einschließlich
der 64 Modulöffnungen und des erkannten globalen Pufferüberlaufs nach erneutem
Laden. Auf beiden Architekturen bestehen außerdem alle 482 SDL-freien
Sanitizer-Tests sowie die direkten Build-, SDK- und Grafikschritte.

Die korrigierte UI-Größenprüfung besteht im direkten Grafikschritt unter Linux
GCC zu `0d10940`. Im vorherigen GCC-Lauf zu `49efcbb` besteht der UI-Benchmark,
aber der erste App-Ablauf hängt erneut vor der Meldung „window and OpenGL ready“.
Zusätzliche Tracepunkte unterscheiden nun SDL-Initialisierung, Fenstererzeugung
und GL-Kontext. Der Wartepunkt auf den Fenstermanager allein behebt diesen
sporadischen Startfehler damit nicht vollständig.
Zu `f6cedbf` bestehen unter Linux GCC und Clang zusätzlich alle 35 direkt gebauten
Fenster-/Grafiktests mit ASan und UBSan. Das belegt diesen Lauf, schließt den
zuvor beobachteten sporadischen Startfehler aber nicht aus.
Die Linux-CI ergänzt deshalb für `floating_workflow` eine strace-Aufzeichnung
einschließlich Prozess-, Socket- und Warteaufrufen. Der bestehende Timeout bleibt
aktiv; `--kill-on-exit` beendet beim Abbruch auch die verfolgten Prozesse.
Eine zusätzliche Linux-Läuferprüfung kontrolliert echte Traceausgabe und das
Ende eines gestarteten Kindprozesses. Der gemeinsame Läufertest besteht lokal
unter Windows (`build/native-trace-runner-windows.log`). Im
[Lauf zu `1790d87`](https://github.com/PhysicSimulator/physim/actions/runs/36497491668)
bestehen die neuen Linux-Zweige mit GCC und Clang sowie alle 35 normalen
Grafiktests mit aktivierter Aufzeichnung des ersten Ablaufs. Mit Clang bestehen
auch alle 35 instrumentierten Grafiktests. Diese erfolgreichen Läufe allein
belegen keine Behebung des sporadischen Startfehlers.

Die direkte SDK-Prüfung umfasst jetzt auch alle zuvor nur im CMake-SDK-Vergleich
gebauten Sprachmodule: 27 Module, neun Sprachexperimente mit beiden allgemeinen
Analysen, spezielle Sensoranalyse und sechs C-/Physim-Kombinationen.
Die installierten Core-Quellen und alle acht C-Vorlagen samt Analyse werden
unabhängig von den ausgelieferten Bibliotheken neu gebaut. Ein SDL-freies
MSVC-Release-SDK besteht die vollständige erweiterte Prüfung unter
`build/native/Native SDK ä cnycd4y3`. Das Clang-Release-SDK besteht zusätzlich
neun Projektbuilds und beide grafischen Abläufe unter
`build/native/Native SDK ä elwayubq`. Im Lauf zu `f6cedbf` besteht der erweiterte
direkte SDK-Schritt unter Linux GCC/Clang, macOS Apple Silicon/Intel und
Windows MSVC/ClangCL Release.

Die Pendelreferenzen prüfen jeweils 4001 Messpunkte, Energiedrift und Periodendauer
gegen eine analytische Referenz. MSVC und Clang liefern für RK4 eine Periodendauer
von 2,48880586925 s bei einer Referenz von 2,48880587159 s. Die Prüfung abgeleiteter
Daten vergleicht vollständiges CSV und Bericht; der maximale Geschwindigkeitsfehler
beträgt dabei rund 5,59e-5 m/s. Crash-, Hang- und Batch-Prüfer kontrollieren zusätzlich
Prozessende, Datenwiederherstellung und das Aufräumen paralleler Kindprozesse.

Die Analyseprüfungen vergleichen den vollständigen CSV-Inhalt und verlangen,
dass ungültige Exporte keine CSV-Datei anlegen. Der Läufertest prüft erwartete
Exitcodes und Diagnosen, Zeitüberschreitungen, fehlende Programme, Emissions-/
Buildfehler, fehlende Ausgaben, falsche Inhalte und ungültiges UTF-8. Fehlerhafte
Schritte verhindern die Ausführung davon abhängiger Programme. Die Ergebnisberichte
enthalten für erwartete Dateien außerdem Größe und SHA-256-Prüfsumme.
Die Integrationsprüfungen erfassen zusätzlich die Prüfsummen aller verwendeten
Programme und Module. Ihre C-Prüfer kontrollieren Messwerte, Metadaten, Szenen,
Berichte und das Verhalten bei ungültigen Parametern. Tests der Ablaufsteuerung
verhindern Erfolge durch veraltete Prüfer nach C-/Physim-Buildfehlern oder fehlende
Ausgabedateien.

Die folgende Tabelle dokumentiert abgeschlossene **direkte Build- und Testschritte**.
Sie nimmt spätere SDK-, Grafik- oder CTest-Schritte desselben CI-Laufs nicht vorweg.
Die acht Kombinationen sind Windows MSVC/ClangCL jeweils Debug/Release, Linux
GCC/Clang sowie macOS auf Apple Silicon/Intel.

| Stand | Direkte Tests | Ausgeführter CI-Nachweis |
| --- | --- | --- |
| `a55d5e0` | 51 C-Tests | Alle acht Kombinationen: [Lauf 36471876255](https://github.com/PhysicSimulator/physim/actions/runs/36471876255) |
| `199a8f7` | 73 Tests | Alle acht Kombinationen: [Lauf 36473305112](https://github.com/PhysicSimulator/physim/actions/runs/36473305112) |
| `430df15` | 134 Tests | Alle acht Kombinationen: [Lauf 36474491263](https://github.com/PhysicSimulator/physim/actions/runs/36474491263) |
| `0a937f8` | 152 Tests | Alle acht Kombinationen: [Lauf 36475496512](https://github.com/PhysicSimulator/physim/actions/runs/36475496512) |
| `d7fb0b0` | 200 Tests | Alle acht Kombinationen: [Lauf 36476324664](https://github.com/PhysicSimulator/physim/actions/runs/36476324664) |
| `0923b6e` | 222 Tests | Alle acht Kombinationen: [Lauf 36477404252](https://github.com/PhysicSimulator/physim/actions/runs/36477404252) |
| `8516187` | 228 Tests | Alle acht Kombinationen: [Lauf 36478529900](https://github.com/PhysicSimulator/physim/actions/runs/36478529900) |
| `2b3b6e7` | 437 Tests | Alle acht Kombinationen: [Lauf 36480859045](https://github.com/PhysicSimulator/physim/actions/runs/36480859045) |
| `b95e119` | 463 Tests | Alle acht Kombinationen: [Lauf 36482481623](https://github.com/PhysicSimulator/physim/actions/runs/36482481623) |
| `a051ea0` | 491 Tests, Build ohne Testfilter korrigiert | Alle acht Kombinationen: [Lauf 36485408557](https://github.com/PhysicSimulator/physim/actions/runs/36485408557) |
| `87c8ae8` | 493 Tests, zusätzlich 15 Sprachprogramme und 27 Module mit Prüfung inkrementeller Builds und Fehlerkorrektur | Alle acht Kombinationen: [Lauf 36499326457](https://github.com/PhysicSimulator/physim/actions/runs/36499326457) |

Die ursprünglichen 51 C-Tests bestanden auch lokal mit MSVC Debug; die 41 SDL-freien
Fälle zusätzlich mit Clang Release. Der vollständige Satz mit 134 Tests bestand
lokal mit MSVC Debug.

Die CI verwendet nach diesen Vergleichen vollständig den direkten Physim-Build.
Im [Lauf zu `558c2ee`](https://github.com/PhysicSimulator/physim/actions/runs/36500503825)
bestehen alle acht Plattformjobs, einschließlich der neuen SDK- und Paketabläufe.
Die 528 Tests, Sanitizer, SDK-Prüfung, Fuzzer und Benchmarks bleiben
enthalten. Die SDK-Grafikprüfung übernimmt zusätzlich alle acht C-Vorlagen;
Linux prüft sie auch mit der instrumentierten App. Mac-Pakete entstehen nur aus
dem nativen SDK und werden nach dem Signieren verschoben und erneut geprüft.
Neue SDKs enthalten keine CMake-Builddateien. Die alten Repository-Builddateien
und CMake-Testskripte sind entfernt; alle 528 Fälle bleiben im direkten Katalog.
Auch das bereinigte Repository besteht alle acht Plattformjobs im
[Lauf zu `7d39aa7`](https://github.com/PhysicSimulator/physim/actions/runs/36502055743).
Lokal bestehen nach dem Entfernen der 90 Physim-CMake-Dateien alle 493 Prüfungen
ohne Fenster mit MSVC Debug (`build/native/Debug/test-results/run-9wu8gqqs`) und
alle 35 Grafikprüfungen mit MSVC Release (`build/native/Release/test-results/run-hxhqdkcm`).
SDL wird in der CI weiterhin mit seinem CMake-Build gebaut.

## Weitere Änderungen prüfen

Jeder Push startet die [CI](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Die jeweiligen Jobs und Artefakte zeigen den geprüften Commit. Linux-Screenshots
und direkte Ergebnisberichte werden als `linux-ui-gcc` beziehungsweise
`linux-ui-clang` archiviert. Diese Artefakte enthalten auch SDK-Protokolle.
