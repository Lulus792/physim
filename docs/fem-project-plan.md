# FEM Teilprojekt für elastische Körper

**Releaseziel seit 13. Oktober 2026: 1.0.0.** Alle Stufen dieses Teilplans,
einschließlich höherer Elementordnungen, 3D, Dynamik, thermischer Dehnung,
Kontakt, großer Verformung, Plastizität und Bruch, gehören vor 1.0 zur Abnahme.
„Nach dem Einstieg“ bezeichnet die Entwicklungsreihenfolge innerhalb von 1.0.
Der verbindliche Gesamtumfang steht in
[Abschnitt 23 des Projektplans](../Physim_Projektplan.md#23-gesamte-physik-als-verbindlicher-umfang-von-100).

Das FEM-Teilprojekt soll Verformung und Spannungen kontinuierlicher Körper in
Physim berechnen. Es erhält einen eigenen Modellvertrag, Netz- und Feldformate,
Solver, C-/Physim-Zugang und Abnahme. Starre Körper in [Mechanik](mechanics.md)
behalten ihren bestehenden Vertrag. Die folgenden Stufen sind geplante Arbeit;
sie bezeichnen keine bereits ausgelieferte FEM-Funktion.

## Modell und Ausbau

Der Einstieg ist quasistatische lineare Elastizität mit kleinen Verformungen,
isotropem Material und einem zweidimensionalen Dreiecksnetz. Ebener
Spannungszustand und ebener Verzerrungszustand sind getrennte, ausdrücklich
gewählte Modelle. Elastizitätsmodul E in Pa, Poissonzahl nu, Schichtdicke in m,
Knotenkoordinaten und Verschiebungen in m sowie Kräfte in N gehören zu den
Eingabedaten. Für das erste isotrope Modell gilt E>0 und -1<nu<0.5.

Lineare Analyse setzt kleine Verschiebungen und Verzerrungen sowie lineares
Materialverhalten voraus. Diese Voraussetzungen beschreibt die
[MIT-Einführung in lineare Finite-Elemente-Analyse](https://ocw.mit.edu/courses/res-2-002-finite-element-procedures-for-solids-and-structures-spring-2010/video_galleries/linear/).
Große Rotation, plastische Verformung oder fast inkompressibles Material
werden nicht durch stilles Fortsetzen dieses ersten Modells dargestellt.

Nach dem linearen Einstieg folgen höhere Elementordnungen und 3D-Elemente,
transiente Elastodynamik, thermische Dehnung und Materialtabellen. Kontakt,
geometrische Nichtlinearität, Plastizität und Bruch erhalten jeweils separate
Formulierungen, Zustandsgrößen und Referenzen. Insbesondere ist ein Kontaktimpuls
zwischen starren Körpern kein fertiges Kontaktmodell für deformierbare Netze.

## Netz und Ressourcen

`physim-fem` wird als optionale C17-Bibliothek mit Header `physim/fem.h` geplant.
Sie hängt von Basis-, Einheiten-, Material- und Mathematikfunktionen ab; ihre
numerischen Routinen besitzen keine GUI-Abhängigkeit. Native Builds und SDKs
verzeichnen die verfügbaren Element- und Solverversionen. Eine fehlende Domäne
wird vor dem Projektbuild verständlich gemeldet; CMake bleibt für
Nutzerprojekte entbehrlich.

Ein Kontext übernimmt Netzdaten über einen expliziten Allocator. Öffentliche
Netze, Felder und Ergebnisse verwenden Besitzer, Slot und Generation.
Elementkonnektivität ist indexbasiert. Doppelte beziehungsweise fehlende Knoten,
nicht endliche Koordinaten, degenerierte Elemente, ungültige Orientierung,
Indexüberlauf und inkonsistente Randzuordnung sind definierte Eingabefehler.
Die erste Importpolitik verlangt positive Dreiecksorientierung; eine
Umorientierung ist eine explizite Importoperation mit eigenem Bericht.

Anzahl von Knoten, Elementen, Freiheitsgraden, Nichtnullwerten und benötigten
Bytes wird vor der Assemblierung geprüft. Speicher- und Iterationsbudgets
sind konfiguriert und in den Metadaten sichtbar. Ein verworfener Aufbau oder
Solve erhält das bisher akzeptierte Ergebnis und gibt seine Kandidatressourcen
frei. Ein Ergebnis darf nach einer Netzänderung nicht weiter als Ergebnis
dieses Netzes verwendet werden.

## Elemente und Gleichungslösung

Die erste Elementfamilie verwendet lineare Dreiecke mit zwei
Verschiebungsfreiheitsgraden je Knoten. Der Elementvertrag nennt Formfunktionen,
Ableitungen, Jacobian, Flächen-/Dickenfaktoren und die Konvention für
Schubverzerrung. Materialmatrix, Steifigkeit und Lastintegrale werden zunächst
unabhängig am Einzelelement geprüft. Der globale Solver assembliert eine
symmetrische dünn besetzte Matrix mit nachvollziehbarer Knotenreihenfolge.

Vorgegebene Verschiebungen werden so eliminiert, dass Symmetrie erhalten bleibt;
Reaktionskräfte werden aus dem ursprünglichen Gleichungssystem rekonstruiert.
Die erste iterative Lösung arbeitet nur auf entsprechend positiv definiten
Systemen. Ein Körper mit freien Starrkörpermoden meldet eine Singularität.
Ein erreichtes Iterationsbudget liefert Residuum und Fehlerstatus, aber kein
scheinbar konvergiertes Ergebnis. Vorconditioner, Skalierung und
Abbruchkriterien gehören zum Methodenbericht.

Äußere Flächenlasten, Volumenlasten und Knotenkräfte sind getrennte Größen.
Die Schichtdicke wird bei Steifigkeit, Masse und integrierten Lasten konsistent
angewandt. Zur Prüfung der physikalischen Bilanz dienen gesamte Last,
Reaktionskräfte und elastische Energie. Spannung und Verzerrung werden
elementweise gespeichert; die App kennzeichnet geglättete Knotendarstellungen
als abgeleitete Vorschau.

## Referenzen und Abnahmen

Patchtests prüfen, ob die Elementfamilie vorgeschriebene einfache
Verschiebungsfelder und zugehörige Verzerrungen reproduziert. Sie ergänzen
Konvergenztests und ersetzen sie nicht; beide Themen behandelt die
[MIT-Unterlage zu linearer Elastizität und Patchtests](https://ocw.mit.edu/courses/16-225-computational-mechanics-of-materials-fall-2003/resources/lecture__8/).
Die folgenden Grenzen sind eigene Abnahmeanforderungen für Physim.

| Fall | Verbindlicher Vergleich |
| --- | --- |
| Starrkörpertranslation und kleine Starrkörperrotation | Keine Verzerrung und keine elastische Energie innerhalb der skalierten Rundungstoleranz. |
| Affines Verschiebungsfeld auf einem unregelmäßigen Patch | Konstante unabhängige Referenzverzerrung; richtige Spannung für beide ebenen Modelle. |
| Rechteck unter gleichmäßiger Zuglast | Verschiebung, seitliche Kontraktion, Spannung, Reaktionssumme und Energie; vollständige SI-Daten. |
| Elastischer Stab und thermische Dehnung | Unabhängige eindimensionale Referenz für den passenden Modellbereich; feste und freie Lagerung getrennt. |
| Glatte hergestellte Lösung | Vorab hergeleitete Volumen-/Randlasten; Verschiebungsfehler und Energienorm bei Netzverfeinerung. |
| Ungenügende Lagerung und degenerierte Elemente | Eindeutiger Fehler; keine erfundene regularisierte Lösung und keine veränderte Ausgabe. |
| Besitzer und Allokation | Veraltete Handles, Netzwiederverwendung, jeder provozierte Allokationsfehler und Sprachabbruch ohne Leck. |

Für die glatte hergestellte Lösung werden mindestens drei reguläre Netzgrößen
verwendet. Der geplante P1-Einstieg verlangt auf den feinsten zwei
Verfeinerungen mindestens Ordnung 1.8 in der L2-Verschiebungsnorm und 0.9 in
der Energienorm. Singularitäten an Last-/Geometriesprüngen werden getrennt
behandelt; sie dürfen diese glatte Referenz nicht verdecken. Iterationsfehler
müssen mindestens zehnmal kleiner als der ausgewertete Diskretisierungsfehler
sein. Starre und affine Fälle verwenden vorab festgelegte absolute und
relative Toleranzen mit Einheiten und Konditionsbereich.

## Felddaten und Lernablauf

Netz, Konnektivität, Materialzuordnung, Lagerung und Lastfall sind Bestandteile
des gespeicherten Modells. Große Verschiebungs-, Spannungs- und Reaktionsfelder
werden blockweise in einem eigenen versionierten Feldarchiv gespeichert.
Der Vertrag umfasst Einheiten, Komponentenreihenfolge, Knoten-/Elementzuordnung,
CRC, begrenzte Längen und Wiederherstellung vollständig geschriebener Blöcke.
Ein Feldarchiv darf kein frei interpretierter externer Dateipfad in der
Laufprovenienz sein. Import, Projektkopie und Export bewahren alle benötigten
Netz- und Felddateien.

Die geplante App zeigt Ausgangsnetz, verformtes Netz, Spannungsfelder,
Verschiebungen und Reaktionskräfte. Ein Verformungsmaßstab ist sichtbar und
ändert keine Ergebniswerte. Auswahl und Legenden unterscheiden Knoten- von
Elementdaten und nennen SI-Einheiten. Die C- und Physim-Lernprojekte verwenden
dasselbe Netz, Material und denselben Lastfall. Eine Analyse vergleicht
Konvergenz, Energie und Reaktionsbilanz und exportiert vollständige Tabellen
und Felder. Der Ablauf umfasst Build, Runner, Speichern, Stoppen, Wiederöffnen,
Analyse und gemischte Sprachen.

## Meilensteine und Freigabe

| Stufe | Lieferumfang | Voraussetzung für die nächste Stufe |
| --- | --- | --- |
| Modellvertrag | Materialgleichung, Ebenenmodell, Einheiten, Netz-/Randregeln und Referenzen | Fachprüfung; unabhängige Referenzdaten und Fehlermaße. |
| Einzelelement | P1-Dreieck, Lasten und Materialmatrix | Starrkörper-, Affin-, Symmetrie- und Energieprüfungen einschließlich Orientierung. |
| Globales System | Assemblierung, Lagerung, Residuen und Reaktionen | Patchtests, Singularitätserkennung, Zugreferenz und globale Kraftbilanz. |
| Sparse Solver | Speicherbudget, Skalierung und Iterationsdiagnose | Vorab definierte Konvergenz, Konditionsfälle und atomare Limit-/Allocatorfehler. |
| C und Physim | Besitzen von Netzen und Ergebnissen, blockweiser Zugriff, Analyse | Identische Quellenfälle und Fehlerverträge; vollständige Besitzer-/Abbruchprüfung. |
| App und Paket | Netze, Felder, Legenden, vollständiger Lernpfad und Exporte | Tatsächliche C-/Physim- und gemischte Appketten im verschobenen SDK. |
| 3D und Dynamik | Neue Elemente, Massenmatrix und Zeitverfahren | Eigene Moden-/Wellenreferenzen, getrennte Raum-/Zeitverfeinerung und Energiebilanz. |
| Nichtlineare Modelle | Zustandsgrößen, Lastschritte und spezifizierte Tangenten | Eigene Fachvalidierung, Pfadabhängigkeit, Konvergenz- und Recovery-Abnahmen. |
| Höhere Elementordnungen | Weitere Formfunktionen, Integrationsregeln und konsistente Last-/Massenmatrizen | Eigene Patchtests, Konditionsgrenzen und gemessene Konvergenz jeder Elementfamilie. |
| Thermische Dehnung und Materialtabellen | Temperaturfelder, Stoffbereiche und Thermomechanik | Freie/feste Ausdehnung, dokumentierte Materialreferenzen und Energieaustausch. |
| Kontakt und große Verformung | Netzbasierter Kontakt/Reibung und geometrisch nichtlineare Kinematik | Kontakt-/Kraftbilanz, objektive Rotation, Lastpfade und Konvergenz. |
| Plastizität und Bruch | Explizite Materialhistorie, Fließ-/Verfestigungs- und Bruchmodelle | Unabhängige Last-/Entlastungsfälle, Dissipation, Netzabhängigkeit und Referenzdaten. |

Jede zusätzliche Modellfamilie erhält vor Implementierung ein vollständiges
Fachpaket. Keine Stufe wird durch die lineare 2D-Abnahme als erledigt behandelt;
alle genannten Stufen gehören zum verbindlichen Ziel von 1.0.0.

Jede Solverfreigabe benötigt tatsächlich ausgeführte Windows-, Linux- und
macOS-Prüfungen sowie Sanitizer, Datenmutationen und verschobene SDKs.
Nichtnullwerte, Peak-RAM, Assemblierungszeit, Iterationen und Auswertezeit
werden an reproduzierbaren Netzen gemessen. Eine bestandene Prüfung linearer
Elastizität wird nicht als Nachweis für Kontakt, Plastizität oder Bruch verwendet.

[CFD Teilprojekt](cfd-project-plan.md) · [Vollständiger Projektplan](../Physim_Projektplan.md)
