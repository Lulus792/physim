# CFD Teilprojekt für inkompressible Strömung

**Releaseziel seit 13. Oktober 2026: 1.0.0.** Alle Stufen dieses Teilplans
einschließlich 3D, thermischer Kopplung und der zusätzlichen Modellfamilien
unter PHYS-003 sind vor 1.0 abzunehmen. „Später“ bezeichnet hier die Reihenfolge
innerhalb der Entwicklung zu 1.0. Der verbindliche Gesamtumfang steht in
[Abschnitt 23 des Projektplans](../Physim_Projektplan.md#23-gesamte-physik-als-verbindlicher-umfang-von-100).

Das CFD-Teilprojekt soll räumlich und zeitlich aufgelöste Fluidbewegung in Physim
ermöglichen. Es erhält eine eigene Bibliothek, Datenbeschreibung, Lernbeispiele
und Abnahme. Die vorhandenen [Rohrnetze und Tracermodelle](fluid.md) bleiben
eigenständige Lehrmodelle. Die folgenden Meilensteine beschreiben geplante
Umsetzung; sie sind keine Freigabe eines bereits vorhandenen CFD-Solvers.

## Modell und Ausbau

Der erste Solver behandelt zweidimensionale, inkompressible Newtonsche Strömung
mit konstanter positiver Dichte und Viskosität auf einem rechtwinkligen Gitter.
Eine explizite Schichtdicke h in Metern verbindet die ebene Rechnung mit
Volumen, Masse, Energie und Fluss in SI. Zunächst sind beide Richtungen
periodisch; danach folgen undurchlässige ruhende und bewegte Wände sowie
definierte Ein- und Auslässe.

Geschwindigkeit liegt in m/s, Druck in Pa, Dichte in kg/m³ und kinematische
Viskosität in m²/s vor. Die Gleichungen sind `div(u)=0` und
`du/dt + (u·grad)u = -grad(p)/rho + nu*laplace(u) + a`.
Die Beschleunigung a wird als Modelldatum in m/s² vorgegeben. Die erste Stufe
verwendet konstante Koeffizienten; später zeitabhängige Daten erhalten eigene
Verträge und Referenzen. Druck ist bei periodischen Grenzen nur bis auf eine
Konstante bestimmt; sein räumlicher Mittelwert wird auf null festgelegt.

Weitere Meilensteine erweitern das Modell um dreidimensionale Gitter,
Temperaturtransport, Auftrieb und gekoppelte Stoffeigenschaften. Kompressible
Strömung, freie Oberflächen, bewegte Geometrien und Turbulenz benötigen jeweils
zusätzliche Fachanforderungen und Abnahmen. Eine bestandene laminare Prüfung
erlaubt keine Übertragung ihrer Genauigkeit auf diese Modelle.

## Eigenständige Architektur

`physim-cfd` wird als optionale C17-Bibliothek mit öffentlichem Header
`physim/cfd.h` geplant. Die Bibliothek benötigt keine GUI, keinen Runner und
keine externe Solverbibliothek. Der native Builder und das SDK verzeichnen
ihre Verfügbarkeit ausdrücklich; Nutzerprojekte benötigen weiterhin kein
CMake. Die Kopplung an Mechanik erfolgt über explizite Kräfte, Randbewegungen
und Zeitschritte, nicht über gemeinsam veränderbare interne Körperzustände.

Ein CFD-Kontext besitzt Gitter und Arbeitsfelder über den expliziten Allocator.
Öffentliche Ressourcen verwenden Besitzer, Slot und Generation. Ein Schritt
berechnet zunächst einen Kandidaten; ein ungültiger Rand, überschrittenes
Arbeitsbudget, Allokationsfehler oder nicht darstellbares Ergebnis erhält den
akzeptierten Zustand. Größenmultiplikationen, Indexbereiche und Aliasing werden
vor Zugriffen geprüft. Maximalwerte für Zellen, Arbeitsspeicher, Iterationen
und Laufzeit gehören zum Kontextvertrag und in die Metadaten.

Als erster diskreter Ansatz ist ein versetztes Gitter vorgesehen: Druck in
Zellmitten, Geschwindigkeiten auf Flächen. Divergenz und Druckgradient verwenden
konsistente Operatoren. Ein Projektionsschritt korrigiert das vorläufige
Geschwindigkeitsfeld über eine Druck-Poisson-Gleichung. Advektion, Diffusion,
Zeitintegration und Drucklösung werden getrennt geprüft. Der Bericht nennt
Zeitschrittgrenzen, Poisson-Residual, Divergenz, Iterationszahl und akzeptierte
Modellzeit. Ein erreichtes Iterationslimit darf keinen erfolgreichen,
unaufgelösten Schritt veröffentlichen.

Für den Einstieg werden feste Zeitschritte, Adams-Bashforth zweiter Ordnung
für die Advektion und Crank-Nicolson für Diffusion mit inkrementeller
Druckkorrektur geplant. Der erste Schritt benötigt ein gesondert geprüftes
Verfahren zweiter Ordnung. Die Drucklösung behandelt den periodischen
Nullraum ausdrücklich. Variable Zeitschritte sind eine spätere Erweiterung,
weil sie auch die historischen Ableitungen und Zeitkoeffizienten ändern.

## Felddaten und Darstellung

Ein Zellfeld wird nicht in die begrenzte skalare Kanalzahl oder die aktuelle
Szenenpunktzahl gezwängt. Ein eigener versionierter Feldarchivvertrag muss
Gitterlage, Schichtdicke, Komponenten, SI-Einheiten, Zeit, Masken und
blockweise Daten mit Längenprüfung und CRC erhalten. Unvollständige Endblöcke
werden wie bei Laufdateien als Recovery behandelt; bereits akzeptierte Blöcke
bleiben lesbar. Bestehende PSRUN-Dateien behalten ihren Vertrag. Eine optionale
Runner-Fähigkeit muss unbekannte Feldversionen ausdrücklich zurückweisen.

Skalare Laufkanäle enthalten Diagnose und integrale Größen. Der Lauf verweist
auf seine mitgespeicherten Feldarchive und deren Herkunft; Export, Projektkopie
und Wiederöffnung müssen beide Datenarten zusammen erhalten. C- und
Physim-Analysen lesen identische Blöcke über geprüfte Handles. Die Physim-Werte
besitzen ihren Speicher und unterstützen dieselben Fehlerfälle wie C.

Die App benötigt Feldkarten, Geschwindigkeitsvektoren und später Schnitte durch
3D-Felder. Vorschauauflösung, Maßstab und dargestellte Zeit sind sichtbar.
Reduzierte Darstellung verändert weder Integrale noch Rohdatenexport. Legenden
zeigen die Einheit und den tatsächlichen Wertebereich; ein Druckoffset wird
als Darstellungsentscheidung gespeichert.

## Referenzfälle und Fehlermaße

Codeprüfung und physikalische Validierung erhalten getrennte Nachweise, wie im
[NASA-Leitfaden zur CFD-Verifikation und Validierung](https://www.grc.nasa.gov/WWW/wind/valid/tutorial/overview.html)
beschrieben. Die folgenden Zahlen und Grenzen sind projektspezifische
Abnahmeanforderungen, keine von dieser Quelle übernommenen Genauigkeitszusagen.

Für den ersten analytischen Fall gilt auf `[0,2*pi*L]²`:
`u=U*sin(x/L)*cos(y/L)*exp(-2*nu*t/L²)` und
`v=-U*cos(x/L)*sin(y/L)*exp(-2*nu*t/L²)`.
Der Druck mit Mittelwert null ist
`p=rho*U²/4*(cos(2*x/L)+cos(2*y/L))*exp(-4*nu*t/L²)`.
Mit L=1 m, U=1 m/s, rho=1 kg/m³, nu=0.01 m²/s und h=1 m sind Eingaben,
Einheiten und Randbedingungen vollständig bestimmt. Der Energievergleich
verwendet zusätzlich `E=rho*A*h*U²/4*exp(-4*nu*t/L²)` mit Fläche A.
Die Referenz wird unabhängig vom diskreten Solver ausgewertet.

| Fall | Verbindliche Prüfung |
| --- | --- |
| Konstantes periodisches Feld | Geschwindigkeit bleibt konstant; Projektion erzeugt keinen Gradienten. |
| Diskreter Sinus für den Poisson-Operator | Vergleich mit eigenständig berechnetem Eigenwert; Drucknullraum und Residual separat prüfen. |
| Abklingender zweidimensionaler Wirbel | Normierter L2- und Maximalfehler von u, v und Druck; Energieabnahme und Divergenz kontrollieren. |
| Laminarer Kanal und Scherströmung | Parabolisches beziehungsweise lineares Profil, Wandbedingung, Volumenfluss und Druck-/Leistungsbilanz. |
| Skalierung von Länge, Geschwindigkeit und Dichte | Dimensionslose Lösungen bleiben vergleichbar; kein verdeckter Wechsel zwischen Pa und Druck pro Dichte. |
| Fehler und Limits | Ungültige Gitter, inkompatible Ränder, NaN, Überläufe, veraltete Handles und jeder provozierte Allokationsfehler erhalten Zustand und Ausgaben. |

Für glatte Fälle werden mindestens drei Gitter und drei Zeitschritte getrennt
verfeinert. Geplant sind räumliche und zeitliche Ordnung zwei; eine beobachtete
Ordnung unter 1.8 auf den feinsten zwei Verfeinerungen verhindert diese Freigabe.
Eine andere nachgewiesene Ordnung wird als solche dokumentiert und erhält einen
eigenen Methodeneintrag. Iterationsfehler müssen mindestens zehnmal kleiner als
die ausgewerteten Diskretisierungsfehler sein; ein Test darf die Toleranz nicht
nachträglich an sein Ergebnis anpassen. Druckvergleiche entfernen ausschließlich
die vereinbarte konstante Eichung.

## Meilensteine und Freigabe

| Stufe | Lieferumfang | Voraussetzung für die nächste Stufe |
| --- | --- | --- |
| Modellvertrag | Gleichungen, Grenzen, Einheiten, Randbedingungen, Speicherbudget und unabhängige Referenzen | Fachprüfung und ausführbare Referenzdaten mit Herkunft. |
| Diskrete Operatoren | Gitter, Divergenz, Gradient, Diffusion und Drucklösung | Operatoridentitäten, Nullraum, Fehlerfälle, symmetrische Systeme und gemessene Residuen. |
| Periodischer 2D-Solver | Zeitschritt, Transport, Projektion und Diagnose | Wirbelreferenz, getrennte Verfeinerungen, Erhaltung und atomare verworfene Schritte. |
| Wände und offene Ränder | Randgeometrie und wohldefinierter Ein-/Ausfluss | Kanal-/Scherreferenz, Massenbilanz sowie getrennte Randkonvergenz. |
| C und Physim | Kontext, Feldzugriff, Simulation, Analyse und Besitzerprüfung | Dieselben archivierten Felder und Fehlercodes; Speicher bleibt bei Abbruch und `attempt` frei. |
| App und Paket | Feldarchiv, Karten, Berichte, Rohdatenexport und vollständiger Lernpfad | Neu anlegen, bauen, simulieren, stoppen, öffnen und auswerten in beiden Sprachen und gemischten Abläufen. |
| 3D und Kopplung | Dreidimensionale Operatoren, Temperatur und explizite Kopplungsverträge | Eigene analytische Fälle, Verfeinerung und experimentelle Validierung für jeden neuen Modellbereich. |
| Kompressible Strömung | Konservative Masse-/Impuls-/Energiegleichungen, Zustandsgleichung und Stoßbehandlung | Stoßrohr, glatte Referenzen, Positivität und Erhaltungsbilanz mit Raum-/Zeitverfeinerung. |
| Freie Oberflächen und Mehrphasenströmung | Explizite Phasen-/Grenzflächenmodelle, Oberflächenspannung und Stoffaustausch | Ruhende Tropfen, hydrostatische Oberflächen, Referenzwellen, Phasenmassen und Energie. |
| Bewegte Geometrie und Fluid-Struktur-Kopplung | Geometrie-/Randbewegung, Kraftübertragung und gemeinsames Zeitverfahren | Geometrische Erhaltung, unabhängige Bewegungsfälle sowie Impuls-/Energieaustausch. |
| Turbulenzmodelle | Benannte und begrenzte Modellansätze mit eigenen Koeffizienten und Auflösungsanforderungen | Unabhängige Referenzdaten, Modell-/Gitterunsicherheit und Gültigkeitsbereich; laminare Tests allein reichen nicht. |

Die zusätzlichen Stufen benötigen vor ihrer Implementierung jeweils ein eigenes
vollständiges Fachpaket. Alle sind verbindliche 1.0-Anforderungen. Der Titel und
der erste Solververtrag benennen weiterhin den inkompressiblen Einstieg;
dieser Einstieg allein schließt das CFD-Teilprojekt nicht ab.

Jede Freigabe benötigt tatsächlich ausgeführte Windows-, Linux- und macOS-
Builds, Sanitizerprüfungen, verschobene SDKs und die dokumentierten
App-Abläufe. Peak-RAM, Zellen pro Sekunde, Solveriterationen und Feldschreibrate
werden für reproduzierbare Lasten gemessen. Die Solverabnahme und eine spätere
physikalische Validierung bleiben getrennte Entscheidungen; ein plausibles
Bild oder ein grüner Build ersetzt beide nicht.

[FEM Teilprojekt](fem-project-plan.md) · [Vollständiger Projektplan](../Physim_Projektplan.md)
