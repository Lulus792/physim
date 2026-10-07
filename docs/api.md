# Öffentliche API, Version 3

Alle Header sind einzeln includierbar. Eigene Funktionen und Typen verwenden `ps_`.
API und Modul-ABI sind getrennt versioniert, zurzeit jeweils 3. Kompatibilität wird
über Versions- und Größenfelder geprüft. Diese Vorabversion garantiert noch keine
Kompatibilität zu späteren Major-Versionen.

Vorhandene Experiment- und Analysemodule müssen mit dem aktuellen SDK neu gebaut
werden. ABI-1-Module werden vor ihrem ersten Callback abgewiesen. `.psrun` bleibt
Dateiformat 1; bestehende Messdaten müssen nicht konvertiert werden.

Der App-Editor verwendet UTF-8 und begrenzt einzelne Quelldateien auf 256 KiB.
Ungültige Bytefolgen und binäre Steuerzeichen werden beim Import abgewiesen.

## Kern

`physim/memory.h`: explizite Allocatoren, geprüfte Allokation/Resize und eine feste
Arena. Berichte und Analysekontexte besitzen optionale Konstruktoren mit eigenem
Allocator; die bisherigen APIs verwenden weiter den Standardallocator.
Verträge und Beispiel: [Speicherverwaltung](memory.md).

`physim/core.h`: dreidimensionale Vektoren, Matrixprodukt (spaltenweise Speicherung),
Quaternion-Rotation, Einheitenkonvertierung, PCG32-Zufallsstrom, Box-Muller-Normalverteilung,
Euler/RK4, symplektischer Euler, Medien, quadratischer Widerstand und Kugelstoß.

`physim/math.h` ergänzt Vec2/Vec4, Mat3, Matrixinversion, Quaternion-Produkte und
Rotationsinterpolation sowie Transformationen für Punkte, Richtungen und Normalen.
Verträge, Rechenbeispiele und Fehlergrenzen: [Vektoren und Transformationen](math.md).

`ps_ode_step` arbeitet allokationsfrei mit bis zu 32 Zustandsvariablen. Der Callback
liefert die Ableitung aller Zustände. RK4 besitzt globale Ordnung 4 für hinreichend
glatte Probleme. Expliziter Euler eignet sich zum Methodenvergleich, aber nicht für
langfristige energieerhaltende Oszillationen. `ps_symplectic_step` gilt für separable
Systeme; beim Pendel mit geschwindigkeitsabhängigem Widerstand ist die dort verwendete
Aktualisierung eine explizite Näherung. Adaptive Integration und Velocity Verlet sind zusätzlich in `physim/numerics.h` verfügbar.

`physim/numerics.h` enthält außerdem lineare Gleichungslöser, Bisektion und
Golden-Section-Minimierung. `physim/units.h` ergänzt zusammengesetzte Einheiten und
physikalische Größen. Verträge und Grenzen: [Numerik](numerics.md).

Einheiten speichern sieben SI-Exponenten plus Skalierung. Konvertierung zwischen
unterschiedlichen Dimensionen scheitert mit `PS_INVALID`; Temperatur-Offsets sind
noch nicht unterstützt. Experimentkanäle speichern SI-Dimensionen und Anzeigesymbole.

## Mechanik

`physim/collision.h`: Hüllboxen für Kugeln/orientierte Boxen und deterministische
Kandidatenpaare für die Kollisionsprüfung; Einzelheiten unter [Mechanik](mechanics.md).

`physim/mechanics.h`: starre Körper mit Hauptträgheit und Quaternionorientierung,
Kräfte/Drehmomente, Impulse, Kugel-/Ebenenkontakte mit Coulomb-Reibung und Restitution,
axiale Feder/Dämpfung sowie Stokes- und quadratischer Kugelwiderstand. Körpergrößen
sind explizit in SI benannt. Die Funktionen prüfen Eingaben und lassen Ausgaben bei
Fehlern unverändert. Konstruktoren für homogene Kugeln und Boxen sind vorhanden;
Kugel–Box, Box–Ebene und Box–Box berücksichtigen die Orientierung der Boxen.
`ps_contacts_boxes` erzeugt Flächen- und Kantenkontakte für zwei Boxen. Ein iterativer
Solver löst bis zu acht Kontakte eines Körperpaars mit akkumulierten Impulsen und
zweidimensionaler Coulomb-Reibung. Kontaktgraphen und gemischte Kontakt-/Gelenkgraphen
lösen mehrere verbundene Körperpaare gemeinsam. Kontinuierliche Kugel–Kugel- und
Kugel–Ebene-Abfragen sowie bewegte Kugelhüllboxen sind vorhanden; sie übernehmen
nicht die Integration der restlichen Schrittzeit. Beispiele stehen in der Mechanikanleitung.

Verträge, Integrationsgenauigkeit und Anleitung zur Stoßvorlage:
[Mechanik und Medien](mechanics.md).

## Experiment

`physim/experiment.h`: ein Modul exportiert `ps_get_experiment()` mit statisch lebendem
`ps_experiment_api`. Alle Pflichtcallbacks müssen vorhanden sein. Der Host besitzt
den Context und die Szene, das Modul besitzt `context->user` und gibt diesen Speicher
in `destroy` frei – auch wenn `create` teilweise fehlgeschlagen ist.

`ps_context` enthält als optionale ABI-3-Erweiterung bis zu 16 Parameter. Der Host
setzt vor `create` mit `ps_parameter_override` eindeutige Werte. Das Modul ruft
in `create` für jeden Parameter einmal `ps_parameter_define` mit Name,
Beschreibung, endlichem Standardwert und inklusiven Grenzen auf. Die Funktion
liefert den wirksamen Wert über den Ausgabezeiger; ein Wert außerhalb der Grenzen
ist ein Fehler. Danach prüft der Host mit `ps_parameter_finalize`, ob alle
Overrides eine Definition besitzen. Namen sind höchstens 47 ASCII-Bytes lang,
beginnen mit einem Buchstaben oder `_` und enthalten danach zusätzlich Ziffern,
Punkte oder Bindestriche. Beschreibungen enthalten 1 bis 95 Bytes ohne
Steuerzeichen. Vor Zugriff auf die Erweiterung muss `struct_size` geprüft werden.

- `create`: Zustand anlegen, Kanäle registrieren, Modellmetadaten beschreiben.
- `reset`: Ausgangszustand wiederherstellen; keine erneute Kanalregistrierung.
- `step`: genau `dt_s` fortschreiten; Messwerte in `context->values` setzen.
- `build_scene`: ausschließlich Daten liefern, keine Grafik-API aufrufen.
- `destroy`: modulseitige Ressourcen freigeben.

`context->time_s` ist beim Eintritt in `step` die bisherige Simulationszeit. Erst
nach erfolgreichem Schritt setzt der Host die neue Zeit auf `Schrittzahl * dt` und
schreibt die Messung. Die Anfangsmessung bei t=0 stammt aus `create`.

Grenzen dieser ABI: 16 skalare Float64-Kanäle, 32 Szenenobjekte und 96 Polyline-Punkte
je Snapshot, Kanalnamen
47 UTF-8-Bytes, Einheiten 15 Bytes, Beschreibungen 95 Bytes. Ein Experiment hält seine
Kanäle nach `create` stabil. Nicht-endliche Messwerte beenden den Lauf mit Fehler.

Szenenobjekte werden in einem rechtshändigen 3D-Raum mit Y nach oben gerendert.
Alle Längen sind Meter. Boxen und Ebenen besitzen eine Quaternion `orientation`
für die Rotation vom lokalen in den Weltraum. Sie wird beim Rendern normiert;
die Nullquaternion wird als Identität behandelt.

| Form | `a` | `b` | `radius` |
| --- | --- | --- | --- |
| `PS_SPHERE` | Mittelpunkt | unbenutzt | Kugelradius |
| `PS_BOX` | Mittelpunkt | volle positive Ausdehnung in X/Y/Z | bei nichtpositiver Ausdehnung: halbe Kantenlänge eines Würfels |
| `PS_LINE` | Startpunkt | Endpunkt | Zylinderradius, bei 0: 0,009 m |
| `PS_ARROW` | Startpunkt | Spitze | Schaftradius, bei 0: 0,009 m |
| `PS_POINT` | Mittelpunkt | unbenutzt | Markerradius, bei 0: 0,025 m |
| `PS_PLANE` | Mittelpunkt | volle Seitenlängen in lokalem X/Z; Y unbenutzt | unbenutzt |
| `PS_POLYLINE` | unbenutzt | unbenutzt | Linienradius, bei 0: 0,009 m |
| `PS_LABEL` | Textanker | unbenutzt | unbenutzt |

`ps_scene_push` fügt ein vollständig beschriebenes Objekt geprüft hinzu.
`ps_scene_polyline` kopiert mindestens zwei Weltkoordinaten in den szeneneigenen
Punktpuffer. `ps_scene_label` kopiert bis zu 63 UTF-8-Bytes ohne Steuerzeichen.
Die drei Funktionen liefern `PS_INVALID`, wenn Daten oder Kapazität ungültig sind,
und verändern die Szene in diesem Fall nicht. Das ältere `ps_scene_add` ist ein
einfacher Wrapper ohne Fehler-Rückgabe. Der Host initialisiert die Szene vor jedem
`build_scene` vollständig mit null; Module behalten keine Zeiger auf diese Szene.

Linien, Pfeile und Polylinien sind räumliche Meshes. Farben speichern `0xRRGGBBAA`;
Alpha 255 zeichnet opake Geometrie, Alpha 1–254 mischt Dreiecke von hinten nach
vorn über der opaken Szene. Sich durchdringende transparente Flächen können
Sortierartefakte zeigen. Alpha 0 blendet Geometrie und Labels vollständig aus,
einschließlich Mausklickauswahl. Teiltransparente Geometrie bleibt auswählbar.
Labels werden als lesbare Bildschirmannotationen
über der Szene gezeichnet und können auch vor verdeckenden Körpern stehen. Anker
außerhalb des Kameravolumens werden nicht beschriftet. Überlappende Labels werden
vertikal versetzt und erhalten eine Verbindung zum Anker; wenn kein freier Platz
im Viewport bleibt, wird das betreffende Label ausgelassen. Pfade, Punkte und Labels sind
in der App getrennt ausblendbar.

Nicht-endliche Zahlen, ungültiges UTF-8 und ungültige Punktbereiche verhindern die
Übertragung eines Snapshots. Endliche, extrem große Koordinaten ab 1e12 m sowie
Radien über 1e10 m werden beim Rendern übersprungen. Kamera-Clipping: 0,02 bis 1000 m.
Die Wurfvorlage speichert die letzten 64 Positionen im Abstand von zehn Physikschritten
als Flugbahn. Deren Inhalt hängt nicht von der Häufigkeit der Scene-Snapshots ab.

## Datensätze und Analyse

`physim/snapshot.h` beschreibt unveränderliche Zustände mit Zeit, Kanalwerten,
Pausestatus und vollständiger Szene. `ps_snapshot_encode`/`ps_snapshot_decode`
verwenden das explizite little-endian Format; der Encoder benötigt einen Puffer
mit `PS_SNAPSHOT_MAX` Bytes. Fehler des Decoders lassen alle Ausgaben unverändert.

`physim/data.h` bietet zusätzlich `ps_run_append_snapshot` und
`ps_run_snapshot_next` für optionale Szenenblöcke. Verwende für das Lesen von
Messwerten und Szenen jeweils einen eigenen `ps_run_reader`. Szenenblöcke ändern
den Messpunktzähler nicht; vorhandene Reader und CSV-Exporte lesen weiterhin alle
Messpunkte. Eine alte Datei ohne Szenen endet beim Snapshotlesen mit `PS_EOF`.
`PS_RECOVERED` bezeichnet einen gültigen Präfix mit unvollständigem/CRC-fehlerhaftem
Ende. Einzelheiten und Versionsregeln stehen im [Dateiformat](data-format.md).

`physim/series.h` stellt Datensätze und versionierte Reihen-Handles bereit:
blockweises Lesen, Ausschnitte, gemeinsame Statusauswahl, Einheitenprüfung, Reihenarithmetik, lineares Resampling, Ableitung,
Trapezintegration, gleitendes Mittel, Statistik und CSV-Export. Bis zu acht Läufe
können gleichzeitig geöffnet werden. Beispiele und Lebensdauerregeln stehen in
[Datenreihen und eigene Analysen](series.md). Die App unterstützt die
[Auswahl und den Vergleich von bis zu acht Läufen](runs.md).

`physim/report.h` erzeugt eigene Ergebnisdiagramme und Tabellen. Die App lädt
`<output_prefix>.psreport` nach Abschluss der Analyse. Linien, Punkte, Histogramme,
mehrere Kurven, Einheitenskalen sowie CSV-/SVG-Export sind in
[Diagramme und Tabellen](reports.md) beschrieben.

`physim/data.h`: Writer und Reader gehören dem Aufrufer und müssen geschlossen werden.
`ps_run_next` liest einen Messpunkt, ohne den gesamten Lauf im Speicher zu halten.
`PS_EOF` bedeutet: Abschlussmarker mit korrekter Punktzahl gelesen.
`PS_RECOVERED` bedeutet: Dateiende oder beschädigter letzter Block, vorherige Werte
sind verwendbar. Strukturell ungültige Daten liefern `PS_CORRUPT`.

`physim/analysis.h`: Welford-Mittelwert und Stichprobenstandardabweichung, zentrale
Sekantenableitung mit einseitigen Rändern, Trapezintegration. `ps_analyze_run` erstellt
kanalweise Statistik, einen SVG-Plot, Energieabweichung und Pendelperioden aus linear
interpolierten aufsteigenden Nulldurchgängen. Ohne genügend Nulldurchgänge lautet
die Periode im Manifest `nan`, niemals eine erfundene Schätzung.

Analysemodul: `ps_get_analysis()` liefert einen `ps_analysis_api`. Der Callback erhält
Eingabepfad und Ausgabeprefix. Alle Dateien entstehen im Analyseprozess. Die GUI zeigt
zusätzlich eine eigene, auf 2048 Punkte begrenzte Kanalvorschau; CSV enthält alle Werte.

Der optionale Callback `run_many` empfängt mehrere Eingabepfade in Auswahlreihenfolge.
Er ist eine optionale Strukturerweiterung; der Runner prüft `struct_size` vor dem Zugriff.
Bestehende ABI-3-Analysemodule mit der kürzeren Struktur bleiben für Einlaufaufträge
gültig. Neue Module ohne Mehrlaufanalyse setzen das Feld auf NULL. Einzelheiten und
Kommandozeilenaufruf stehen unter [Läufe und Berichte](runs.md).

## Szenenobjekt-IDs (ABI 3)

`ps_object.id` ist eine optionale, vom Experiment vergebene `uint32_t`-ID.
Null bezeichnet einen anonymen Listeneintrag. Andere IDs müssen innerhalb eines
Snapshots eindeutig sein und sollen über den Lauf dasselbe logische Objekt
bezeichnen. `ps_scene_push`, Szenenvalidierung und Snapshot-Decoder lehnen
doppelte nichtnull IDs ab. IDs werden unabhängig von der Listenposition übertragen.
Beispiel: `ps_object body = {.shape = PS_SPHERE, .radius = .2, .color = 0xff4444ff, .id = 17};`
und anschließend `ps_scene_push(scene, &body)`.

Für einfache Primitive gibt es `ps_scene_add_id`, `ps_scene_polyline_id` und
`ps_scene_label_id`: direkt nach dem Szenenargument steht die ID, anschließend
folgen dieselben Argumente wie bei der jeweiligen Funktion ohne `_id`.
Alle drei liefern `ps_result`; bei einer doppelten nichtnull ID bleibt die Szene
einschließlich ihres Punktpuffers unverändert. Die bisherigen Konstruktoren
erzeugen weiterhin anonyme Einträge.

Die acht Vorlagen verwenden feste IDs nach Bedeutung. Körper, Vektoren, Pfade
und Beschriftungen behalten ihre Zuordnung, auch wenn Sensorpunkte oder
Kontaktinformationen hinzukommen. Kurzlebige Kontaktpunkte und deren einzelne
Impulse bleiben anonym, da der Kontaktlöser keine persistente Kontakt-ID liefert.

API/ABI 3 und IPC 3 erfordern den Neubau vorhandener Experiment- und Analysemodule.
ABI-2-Binärmodule werden abgewiesen. Die Dateiformate `.psrun` und `.psreport`
bleiben unverändert. Ein Analysemodul darf weiterhin den optionalen `run_many`-Teil
seiner Struktur weglassen, muss aber die aktuelle ABI-Version angeben.

## Messungen und Sensoren

`physim/measurement.h` bietet geprüfte konstante/uniforme/normale Verteilungen und
Sensoren mit Einheiten, Zeitraster, Auflösung, Offset, Drift, Rauschen, Ausfällen und
Standardunsicherheit. Messwert und Gültigkeit sind getrennt. App-Vorschau und
`ps_analyze_run` beachten zugehörige `.status`-Kanäle; Rohdaten und Series behalten
alle Zeilen. Beispiel und genaue Annahmen: [Messungen & Sensoren](measurement.md).

## Reproduzierbarkeit

Seed, dt, Modellparameter, Integratorname und Modul-FNV-1a-Fingerabdruck werden in der
Laufdatei gespeichert. Die App legt den Experimentquellcode neben dem Lauf ab und den
Analysequellcode neben dem Bericht. FNV ist ein Identifikator, kein Sicherheitsnachweis.
Reproduzierbarkeit gilt für gleichen Code, Compiler, Plattform und Parameter; es gibt
keine Zusage bitidentischer Mathematik zwischen unterschiedlichen Plattformen.

Die App und `physim-batch` unterstützen außerdem Laufserien mit bis zu acht
gleichzeitigen Runnern und einem expliziten Seed je Runner-Prozess,
Modul-/Quellsnapshot, Endwert-CSV und
aggregiertem Bericht. Der Controller ist vorerst ein interner Anwendungsdienst.
Einrichtung, Statistik und Beispielcode: [Monte Carlo und Unsicherheit](monte-carlo.md).

## Lokale Szenenkoordinaten

`ps_scene_frame`, `ps_scene_transforms` und `ps_scene_world_point` ergänzen
explizite hierarchische TRS, Weltpositionen und dieselbe Geometrie für
Darstellung und Picking. Gruppen und Körper als Eltern liefern selbst keine
Koordinatentransformation. [Vertrag und Beispiele](scene-frames.md).

## Thermodynamik

`physim/thermodynamics.h` liefert ideale Gaszustände, Energie und Entropiedifferenzen,
Wärmekapazität, Wärmefluss und exakte thermische Relaxation ohne versteckte Zustände.
[Vertrag und vollständige C-/Physim-Beispiele](thermodynamics.md).
