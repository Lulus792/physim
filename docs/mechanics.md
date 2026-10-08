# Mechanik und Medien

`physim/mechanics.h` ergänzt starre Körper mit Translation und Rotation, Kräfte,
Impulse, Kugel- und Boxkontakte, Reibung, eine axiale Feder mit Dämpfung sowie auswählbaren
Kugelwiderstand und hydrostatischen Auftrieb. Alle Größen verwenden SI; Richtungen und Geschwindigkeiten liegen
im Weltkoordinatensystem. Die API ist unabhängig von Szene, Runner und GUI.

Der vollständige [Lernpfad zum elastischen und inelastischen Stoß](collision-tutorial.md)
führt durch ein zentrales Vakuummodell mit wählbaren Massen, Geschwindigkeiten
und Restitution sowie vollständigen C-/Physim-Quellen und Analysen.

## Trägheit und Energiebereich

Die homogene Vollkugel verwendet `I = 2*m*r²/5`, die Box mit vollen
Kantenlängen beispielsweise `Ix = m*(y²+z²)/12`. Die Konstruktoren summieren
Produkte der binären Eingaben exakt und runden die rationale Formel einmal
auf den nächsten Binary64-Wert, bei Gleichstand zum geraden Wert.
Zwischenprodukte dürfen den Double-Bereich überschreiten. Nur wenn die positive
Endträgheit zu null rundet oder überläuft, folgt `PS_NUMERIC`; der Ausgabekörper
bleibt unverändert. Masse null erzeugt weiterhin einen ruhenden statischen Körper.

`ps_body_kinetic_energy` summiert Translation und Rotation in den gespeicherten
Hauptachsen. Bei identischer Orientierung werden die binären Produkte exakt
summiert und einmal gerundet. Für gedrehte Körper wird die Winkelgeschwindigkeit
zunächst skaliert und mit dem normalisierten Quaternion in die Hauptachsen
transformiert. Diese Rotation hat weiterhin Double-Rundungsfehler; ihre
anschließenden Energieprodukte werden exakt summiert. So bleibt beispielsweise
Energie bei sehr großer Geschwindigkeit und sehr kleiner Masse darstellbar,
obwohl das unskalierte Geschwindigkeitsquadrat überlaufen würde.

Eine zu null gerundete Energie ist gültig. Ein Energieüberlauf erhält den
Ausgabeparameter. Diese Bereichsgarantien betreffen Konstruktoren und Energie;
Schrittintegration, Drehmomente, Impulse und Kontaktlösung haben eigene
Zwischenwerte und Genauigkeitsgrenzen.

## Stoßexperiment in der App

1. Vorlage **Kugelstoß & Medien** anlegen und mit F5 bauen.
2. Simulieren und nach dem Kontakt stoppen, dann analysieren.
3. In `main.c` `PS_COLLISION_MEDIUM` auf 1 ändern, erneut bauen und laufen lassen.
4. Unter **Läufe & Berichte** beide Läufe auswählen und vergleichen.

Medium 0 ist Vakuum, 1 Luft, 2 Wasser, 3 das editierbare `custom_medium`.
`PS_COLLISION_DRAG` wählt `PS_DRAG_NONE`, `PS_DRAG_STOKES` oder
`PS_DRAG_QUADRATIC`. Im Standardfall stehen zwei gleich schwere Kugeln im Vakuum
ohne Anfangsrotation oder Reibung gegenüber: Beim elastischen Stoß tauschen sie ihre
Geschwindigkeiten. Mit `PS_COLLISION_SPIN=1.0` und `PS_COLLISION_FRICTION=0.3`
wird Rotation durch den Tangentialimpuls zwischen den Kugeln übertragen.

`PS_COLLISION_SPEED` legt den Betrag der beiden entgegengesetzten
Anfangsgeschwindigkeiten fest (Standard 0,6 m/s). Im Vakuum ist
`PS_COLLISION_CCD` standardmäßig aktiv: Die Vorlage bewegt die Kugeln bis zum
ersten Kontakt, löst den Impuls und führt die restliche Schrittzeit mit den neuen
Geschwindigkeiten aus. Für dieses isolierte Paar ohne äußere Kräfte genügt ein
Stoß pro Schritt. Eine allgemeine Mehrkörper-Ereignissteuerung ist das nicht.

Mit `PS_COLLISION_CCD=0` lässt sich der diskrete Ablauf vergleichen. Beispielsweise
laufen die Kugeln bei 100 m/s und `dt=0.05` ohne CCD zwischen den Abtastzeitpunkten
aneinander vorbei; mit CCD bleibt die Reihenfolge nach dem Rückprall korrekt.
Bei Medien 1–3 ist CCD standardmäßig aus. Explizites Aktivieren zusammen mit einem
dieser Medien wird zurückgewiesen, da die Widerstandsbewegung nicht linear ist.
Metadaten speichern Geschwindigkeit und gewähltes Kontaktverfahren.

Die Messung enthält Positionen, X-Geschwindigkeiten, beide Z-Winkelgeschwindigkeiten,
gesamte kinetische Energie einschließlich Rotation, X-/Y-Gesamtimpuls, angewandte
X-Widerstandskraft und Kontaktimpuls dieses Schritts. Die Szene zeigt einen weißen
Orientierungsstrich, Geschwindigkeitspfeile, rosafarbene Kraftpfeile und den letzten
Kontaktpunkt. Pfeilskalen sind in der Szene angegeben. Ein Impuls ist keine Kraft;
der Kontaktmarker speichert den Ort des letzten Kontakts, keine aktuelle Berührung.

Massen, Radien, Restitution, Reibung, Anfangsrotation, Medium, Dichte, Viskosität,
Fluidgeschwindigkeit, Widerstandsmodell und ausgeschlossene Effekte stehen in den
Laufmetadaten. Alte Projektquellen bleiben unverändert: Die neue Vorlage gilt für
neu angelegte Projekte oder kann bewusst in ein bestehendes Projekt übernommen werden.

## Box auf Ebene

Die vierte Projektvorlage lässt eine geneigte Box unter Gravitation auf eine Ebene
fallen und zur Ruhe kommen. Mit F5 bauen, simulieren und anschließend analysieren.
Die blauen Boxflächen zeigen die Orientierung; rosa Punkte markieren die Kontakte.
Die Kollisionsebene ist mathematisch unendlich, ihre gezeichnete Fläche misst 8 × 8 m.

In `main.c` lassen sich Anfangslage, Geschwindigkeit, Kantenlängen, Masse und
Solverparameter ändern. Standard sind 64 Iterationen, Restitution 0,1 und Reibung 0,6.
Die elf Messkanäle umfassen Position, vertikale Geschwindigkeit, Gesamtenergie,
kinetische Energie, Winkelgeschwindigkeit, Kontaktanzahl, vertikalen Kontaktimpuls,
Bodenabstand und normalen Solverfehler. `clearance` ist der kleinste Abstand eines
Boxeckpunkts nach der Positionskorrektur; negative Werte bedeuten Eindringung.
`contact.error` misst die normale Kontaktbedingung vor der Positionskorrektur,
nicht die vollständige Konvergenz der Reibung. Im Ruhezustand gleicht der aufsummierte
vertikale Kontaktimpuls näherungsweise `m*g*dt` aus. F1 öffnet diese Anleitung und
die API auch während der Arbeit direkt in der App.

## Zwei Boxen stoßen zusammen

Die Vorlage **Boxstoß** startet zwei gleich schwere Boxen mit je 0,6 m/s aufeinander
zu. Im Standardfall treffen ihre Flächen nach einer Sekunde aufeinander. Vier
Kontaktpunkte verhindern dabei eine künstliche Drehung: Die Boxen tauschen ihre
Geschwindigkeiten, die gesamte kinetische Energie bleibt bei 0,36 J.
Mit F5 bauen, unter **Simulieren** starten und nach dem Stoß stoppen; anschließend
stehen Messdaten und die normale Analyse in **Auswerten** bereit.

In `main.c` verändert `PS_BOX_RESTITUTION` die Restitution, `PS_BOX_FRICTION`
die Coulomb-Reibung. Mit `PS_BOX_ANGLE=0.35` und `PS_BOX_OFFSET=0.12` trifft Box B
gedreht und versetzt auf Box A. Zusammen mit Restitution 0,6 und Reibung 0,3 entsteht
ein dissipativer Stoß mit Rotation. Die Boxen bewegen sich frei im Vakuum ohne
Gravitation; es gibt weder Boden noch weitere Körper.

Die Szene zeigt die tatsächlichen Boxorientierungen und Geschwindigkeitspfeile.
Rosa Punkte und Pfeile markieren den **letzten** erkannten Kontakt und dessen
Impulse auf Box A. Sie bleiben nach der Trennung als Referenz sichtbar. Ein Impuls
in Ns ist keine Kraft. Die 15 Kanäle enthalten Position und X-Geschwindigkeit von A,
X-Position/-Geschwindigkeit von B, gesamte kinetische Energie einschließlich Rotation,
beide Winkelgeschwindigkeitsbeträge, alle drei Komponenten des Gesamtimpulses,
aktuelle Kontaktpunktzahl, X-Kontaktimpuls auf A und normalen Solverfehler.
Unter **Messdaten** lässt sich der jeweilige Kanal auswählen. Abgeleitete
Geschwindigkeit aus diskreten Positionsdaten glättet den Sprung am Stoß; für den
unmittelbaren Vorher-/Nachher-Vergleich den Kanal `velocity.x` verwenden.

Geometrie, Massen, Anfangszustände, Medium, Integrator, Kontaktverfahren,
Solverparameter und ausgeschlossene Effekte stehen in den Laufmetadaten.
Die Anleitung und `mechanics.h` lassen sich über F1 direkt in der App öffnen.

## Körper und Trägheit

`ps_body_sphere` erzeugt eine homogene Vollkugel mit `I = 2*m*r*r/5`.
`ps_body_box` nimmt volle Kantenlängen und berechnet beispielsweise
`Ix = m*(y*y+z*z)/12`. Die drei gespeicherten Trägheitswerte gehören zu lokalen
Hauptachsen. Die Quaternion rotiert diese ins Weltkoordinatensystem; der Tensor
wirkt dort als `R * diag(Ix,Iy,Iz) * R^T`. Eine beliebige positiv definite Trägheit
kann durch ihre Hauptwerte und Hauptachsen beschrieben werden.

```c
ps_body body;
ps_result r = ps_body_sphere(2.0, 0.2, &body);
if (r == PS_OK) {
    body.position_m = ps_v3(0, 2, 0);
    r = ps_body_step(&body, ps_v3(0, -2.0 * 9.81, 0), ps_v3(0, 0, 0), 0.001);
}
```

Masse null bezeichnet einen statischen Körper: Trägheit und beide Geschwindigkeiten
müssen null sein. Dynamische Körper benötigen positive Hauptträgheitswerte. Eine
Quaternion muss endlich und bis auf 1e-8 normiert sein; der Nullwert ist hier keine
Identität. Konstruktoren setzen die Identität automatisch. Position, Orientierung
und Geschwindigkeiten dürfen vom Experiment direkt gesetzt werden und werden bei
jedem API-Aufruf geprüft.

`ps_body_force_torque` berechnet `(Angriffspunkt - Schwerpunkt) x Kraft`.
`ps_body_apply_impulse` ändert lineare und Winkelgeschwindigkeit unmittelbar.
`ps_body_point_velocity` umfasst Translation und `omega x Hebelarm`.
`ps_body_kinetic_energy` enthält beide Bewegungsanteile.

## Integration und Genauigkeit

`ps_body_step` führt zuerst `v += F*dt/m`, dann `x += v*dt` aus. Die Winkelbeschleunigung
lautet `inverse(I_world) * (torque - omega x (I_world*omega))`. Die neue
Winkelgeschwindigkeit rotiert die Quaternion mit einer Exponentialrotation;
anschließend wird sie normiert. Kräfte und Drehmomente sind während eines Schritts
konstant. Diese Methode hat insgesamt erste Ordnung; bei asymmetrischen freien
Körpern erhält sie Energie und Drehimpuls nicht exakt. Zeitschrittverfeinerung bleibt
Teil des Experiments. Insbesondere schnelle Rotation und steife Federn brauchen
kleinere Schritte. Es gibt keine automatische Stabilitätsgarantie.

## Kontakte und Reibung

`ps_contact_spheres` erkennt auch exakte Berührung. Bei deckungsgleichen Mittelpunkten
liefert es deterministisch die X-Richtung; diese Wahl beschreibt keine eindeutige
physikalische Kontaktnormale. `ps_contact_sphere_plane` verwendet eine normierte
Ebenennormale in Richtung des freien Halbraums. Kontaktpunkt und Normale liegen in
Weltkoordinaten; die Kontaktnormale zeigt von Körper A zu B.

`ps_contact_resolve` bestimmt einen normalen Stoßimpuls mit Restitution 0 bis 1,
danach einen Tangentialimpuls gegen die relative Kontaktgeschwindigkeit. Dessen
Betrag ist durch `mu * normaler Impuls` begrenzt. Rotationsanteile gehen in die
effektive Masse ein. Die Grundgleichungen für Kontaktgeschwindigkeit, effektive
Masse und Coulomb-Impuls sind in [Catto: Sequential Impulses, Folien 18–24](https://box2d.org/files/ErinCatto_SequentialImpulses_GDC2006.pdf)
beschrieben. Diese Funktion behandelt einen einzelnen Kontakt.

Danach wird Überlappung proportional zu den inversen Massen herausgeschoben.
Diese geometrische Korrektur ist keine physikalische Bewegung und kann bei
vorhandener Überlappung Potentialenergie und orbitalen Drehimpuls verändern.
Bei bereits auseinanderlaufenden Körpern erfolgt nur diese Positionskorrektur.
`NULL` als Körper B bedeutet eine unbewegliche Welt, etwa für die Ebene.

`ps_contact_sphere_box` bestimmt den nächsten Punkt einer orientierten Box. Bei
einem Kugelmittelpunkt im Inneren wählt es die nächste Fläche, bei Gleichstand
deterministisch X, Y, Z. Die Richtung einer tiefen Überlappung dient dem
Herausschieben und rekonstruiert keinen vergangenen physikalischen Stoß.
`ps_contacts_box_plane` liefert bis zu acht Boxeckpunkte auf oder hinter der Ebene.
Beide Funktionen verwenden volle Kantenlängen. Ein leeres Manifold ist gültig.

`ps_contacts_resolve` behandelt ein Manifold für genau ein Körperpaar. Es akkumuliert
normale und tangentiale Impulse über eine feste Zahl von Iterationen. Der normale
Impuls bleibt nichtnegativ; die zwei Tangentialkomponenten werden durch projizierte
Gradientenschritte auf die Coulomb-Scheibe mit Radius `mu * Normalimpuls` begrenzt.
Die Schrittweite berücksichtigt den größten Eigenwert der tangentialen effektiven
Massenmatrix. Restitution wird einmal aus der anfänglichen Annäherung berechnet;
unterhalb der Rückprallschwelle entfällt sie, um ruhende Kontakte zu unterstützen.

`PS_CONTACT_SOLVER_DEFAULT` setzt 40 Iterationen, Restitution 0, Reibung 0,5,
Rückprallschwelle 0,5 m/s, tolerierte Eindringung 1e-5 m und Korrekturanteil 0,8.
`PS_OK` bedeutet, dass das Iterationsbudget verarbeitet wurde. Das Ergebnis enthält
die Impulse auf A und den größten normalen Komplementaritätsfehler in m/s;
es verspricht keine Konvergenz für beliebige Kontaktanordnungen. Der Fehler prüft
bei positivem Normalimpuls die Abweichung von der Zielgeschwindigkeit und sonst
nur eine verbleibende Verletzung in Annäherungsrichtung.

Die anschließende Positionskorrektur berücksichtigt bereits angewandte Verschiebungen,
damit vier gleichgerichtete Kontakte dieselbe Eindringung nicht vierfach korrigieren.
Sie verschiebt Schwerpunkte, korrigiert keine Orientierung und ist weiterhin eine
geometrische Näherung. Die Reihenfolge kann das Ergebnis beeinflussen.

`ps_contacts_boxes` erkennt zwei orientierte Boxen. Die Trennachsenprüfung verwendet
die sechs Flächennormalen und neun Kreuzprodukte der Kantenrichtungen. Grundlage
der Achsenwahl ist [Eberly: Dynamic Collision Detection using Oriented Bounding Boxes,
Abschnitt 2](https://www.geometrictools.com/Documentation/DynamicCollisionDetection.pdf).
Für eine Fläche wird die gegenüberliegende Fläche der anderen Box an vier Seiten
abgeschnitten; daraus entstehen bis zu acht Kontakte. Bei einem Kantenpaar liefert
der Mittelpunkt der beiden nächsten Kantenpunkte einen Kontakt. Die Normalen
zeigen von A nach B, Kontaktpunkte liegen mittig zwischen den zugehörigen Oberflächen.

Die Funktion arbeitet relativ zu A, skaliert mit der größten halben Kantenlänge.
Abstände bis `64 * DBL_EPSILON` dieser Länge gelten als Berührung; Kreuzachsen mit
Länge bis `32 * DBL_EPSILON` werden als degeneriert behandelt. Gleiche minimale
Eindringtiefen bevorzugen die Flächen von A, dann B, dann Kanten. Identische Eingaben
liefern dieselbe Reihenfolge. Keine Berührung ergibt `PS_OK` mit `count=0`; Fehler
verändern die Ausgabe nicht. Sehr unterschiedliche Größen und große absolute
Weltkoordinaten unterliegen weiterhin den Auflösungsgrenzen von Double.
Anfangs vollständig ineinander liegende Boxen erhalten eine geometrische
Ausstoßrichtung, keine physikalisch rekonstruierte Stoßhistorie.

Noch offen sind ein Solver für Netze verbundener Körperpaare, Constraints,
Kontaktcache/Warmstart, Broad Phase und kontinuierliche Kollisionserkennung.
Hohe Geschwindigkeit kann Körper zwischen zwei Schritten durcheinanderführen.

## Federn und Widerstand

Die App-Vorlage **Feder–Masse–Dämpfer** verbindet diese Kraftfunktion mit RK4,
einer geführten Masse und einer Bilanz der dissipierten Energie. Anleitung,
Gleichungen und Referenzfälle: [Feder–Masse–Dämpfer](spring.md).

`ps_spring_force` liefert auf A entlang der Verbindung zu B die Kraft
`k*(Abstand-Ruhelänge) + c*relative axiale Geschwindigkeit`. B erhält die Gegenkraft.
Das ist eine axiale Feder mit axialer viskoser Dämpfung. Bei zusammenfallenden
Endpunkten und aktiver Feder/Dämpfung ist die Richtung undefiniert: `PS_SINGULAR`.

`ps_sphere_drag` verwendet die Geschwindigkeit des Körpers relativ zum Fluid:

- Kein Widerstand: null Kraft.
- Stokes: `-6*pi*Viskosität*Radius*relative Geschwindigkeit`, für schleichende Strömung.
- Quadratisch: `-0.5*Dichte*Cd*pi*Radius²*Betrag(v)*v`, mit explizitem Cd.

Die Stokes-Annahme für kleine Reynolds-Zahlen wird in
[MIT: Classical Mechanics III, Abschnitt 6.8](https://ocw.mit.edu/courses/8-09-classical-mechanics-iii-fall-2014/d9bac33f6c60b304dc0398e99b327102_MIT8_09F14_full.pdf)
hergeleitet.

Das Programm wählt nicht automatisch anhand der Reynolds-Zahl. Widerstand und
Auftrieb sind getrennte Kräfte; die Stoßvorlage aktiviert keinen Auftrieb.
Zusatzmasse, Rotationswiderstand und temperaturabhängige Stoffdaten sind nicht
enthalten. Die Wasseroption allein macht das Modell daher noch nicht zu einer
vollständigen Unterwassersimulation. Stoffwerte sind editierbare Daten; der Aufrufer
verantwortet Modell und Gültigkeitsbereich.

## Auftrieb und teilweise eingetauchte Kugeln

Die App-Vorlage **Auftrieb** zeigt eine schwimmende Kugel mit Kräftepfeilen,
zwölf Messkanälen und Energiebilanz. [Anleitung und Modellgrenzen](buoyancy.md).

`ps_buoyancy_force(rho, volume, gravity, &force)` liefert die archimedische Kraft
`-rho * volume * gravity`. Dichte und verdrängtes Volumen müssen endlich und
nichtnegativ sein. Gravitation ist ein beliebig gerichteter endlicher Weltvektor.
Gewicht und Widerstand werden vom Aufrufer zusätzlich addiert. Die Kraft greift
am Schwerpunkt des verdrängten Fluids an; `ps_body_force_torque` liefert daraus
das Drehmoment für einen versetzt liegenden Körperschwerpunkt.

`ps_sphere_submersion(radius, center_height, &submersion)` berechnet das verdrängte
Volumen und den Schwerpunkt einer Kugel unter einer ebenen Fluidoberfläche.
`center_height` ist der vorzeichenbehaftete Abstand des Kugelmittelpunkts zur
Ebene, positiv zur trockenen Seite. Der Fluidraum liegt auf der negativen Seite.
Der Schwerpunktversatz wird entlang dieser Oberflächennormalen angegeben.
Bei trockenem Kontakt sind beide Ausgaben null; eine vollständig eingetauchte
Kugel verdrängt `4*pi*r³/3`, mit Schwerpunkt im Kugelzentrum. Eine halb eingetauchte
Kugel hat den Schwerpunktversatz `-3*r/8`.

Für horizontales Wasser bei `y=0` und eine homogene Kugel mit Radius `radius`
kann eine Schrittfunktion so aussehen (alle Größen in SI):

```c
ps_result water_step(ps_body *body, double radius, double dt) {
    if (!body) return PS_INVALID;
    ps_submersion submerged;
    ps_vec3 gravity = ps_v3(0, -9.81, 0), force, torque;
    ps_result r = ps_sphere_submersion(radius, body->position_m.y, &submerged);
    if (r != PS_OK) return r;
    r = ps_buoyancy_force(1000, submerged.volume_m3, gravity, &force);
    if (r != PS_OK) return r;
    ps_vec3 point = ps_vadd(body->position_m,
                           ps_v3(0, submerged.centroid_offset_m, 0));
    r = ps_body_force_torque(body, force, point, &torque);
    if (r != PS_OK) return r;
    return ps_body_step(body, ps_vadd(force, ps_vscale(gravity, body->mass_kg)),
                        torque, dt);
}
```

Für eine geneigte Ebene wird `center_height = dot(center-plane_point, normal)`
mit Einheitsnormaler verwendet; der Angriffspunkt ist dann
`center + normal * centroid_offset`. Im hydrostatischen Modell muss diese Normale
der Gravitation entgegengerichtet sein. Dichte und Gravitation sind räumlich
konstant; Wellen, Oberflächenspannung, Zusatzmasse und Fluidströmung werden nicht
berechnet. Das Beispiel enthält keine Dämpfung: Eine ausgelenkte schwimmende Kugel
kann daher um ihre Gleichgewichtslage schwingen. Sehr kleine Double-Ergebnisse
können zu null unterlaufen; nichtendliche Ergebnisse ergeben `PS_NUMERIC`.

Der Test `buoyancy` integriert unabhängig Kreisquerschnitte und deren erste
Momente für 101 Eintauchtiefen. Er prüft außerdem komplementäre Volumina,
Schwimmgleichgewicht, rückstellende Kraft, Sinken/Steigen, beliebige
Gravitationsrichtungen, extreme Größenordnungen und unveränderte Ausgaben bei Fehlern.

## Distanzgelenk

`ps_distance_joint_validate` prüft lokale Anker, Soll-Länge und Stabilisierung
ohne Körperzustände. `ps_distance_joint_resolve` prüft diese Konfiguration ebenfalls
und erzwingt eine Geschwindigkeitsbedingung entlang der
Verbindung zweier Anker. Die Anker liegen im lokalen Koordinatensystem ihrer
Körper; bei `B=NULL` ist der zweite Anker ein fester Weltpunkt. Versetzte Anker
erzeugen auch Drehimpulse. Das Gelenk kann ziehen und drücken und benötigt eine
positive Solllänge. Zusammenfallende Anker liefern `PS_SINGULAR`, weil dann keine
eindeutige Verbindungsrichtung existiert.

Die Stabilisierung zwischen 0 und 1 gibt den Anteil des aktuellen Längenfehlers
an, der als zusätzliche Schließgeschwindigkeit pro Zeitschritt angefordert wird:
`target = -stabilization * (length - length_m) / dt_s`. Bei 0 wird ausschließlich
die radiale Relativgeschwindigkeit beseitigt. Die Korrektur kann Energie zuführen;
das Verfahren ist kein energieerhaltender Integrator. Große Zeitschritte oder
starke Anfangsfehler erfordern besondere Aufmerksamkeit und Verfeinerung.

Externe Kräfte werden zuerst in Geschwindigkeiten integriert, danach wird das
Gelenk gelöst und schließlich werden Positionen und Orientierungen fortgeschrieben.
Der Gelenkaufruf selbst verändert ausschließlich Geschwindigkeiten. Wer
`ps_body_step` danach nutzt, darf bereits integrierte Kräfte nicht erneut anwenden;
auch dessen gyroskopische Winkelbeschleunigung muss im eigenen Ablauf berücksichtigt
werden. Eine automatische gemeinsame Integrationssteuerung existiert noch nicht.

Beispiel für einen Körper mit seinem Schwerpunkt an einem festen Aufhängepunkt:

```c
ps_distance_joint joint = {
    .anchor_a_m = {0, 0, 0},
    .anchor_b_m = {0, 2, 0},
    .length_m = 1,
    .stabilization = 0.2
};
ps_distance_joint_solution result;
ps_result error = ps_distance_joint_resolve(&body, NULL, &joint, dt, &result);
```

Das Ergebnis meldet den Impuls auf A, den Längenfehler vor dem Lösen und den
absoluten Restfehler der Geschwindigkeitsbedingung. Zwei statische Körper erhalten
keinen Impuls; ein unerfülltes Stabilisierungsziel bleibt am Restfehler sichtbar.
Alle Ausgaben bleiben bei Fehlern unverändert. Es erfolgen keine Allokationen.
Mehrere Gelenke und Kontakte werden mit `ps_constraints_resolve_graph` gemeinsam
iteriert. Warmstart und weitere Gelenktypen bleiben offen.

Referenzen prüfen Impulserhaltung, die analytische Antwort eines gedrehten
Boxankers, Stabilisierung, Zeitschrittverfeinerung einer Kreisbewegung, statische
Partner und unveränderte Ausgaben bei ungültigen oder numerisch unlösbaren Eingaben.

## Gemeinsame Gelenk- und Kontaktlösung

`ps_constraints_resolve_graph` erweitert die Kontaktgraph-Lösung um bis zu 256
Distanzgelenke (`ps_distance_constraint`). Es gelten weiterhin höchstens 128 Körper
und 512 Kontaktpunkte. Körperindizes und `PS_CONTACT_WORLD` entsprechen den
Kontaktbedingungen. Alle Arbeitspuffer sind begrenzt; es gibt keine Heap-Allokation.

In jeder Geschwindigkeitsiteration werden zuerst sämtliche Kontakte und dann
sämtliche Gelenke in ihrer Eingabereihenfolge bearbeitet. Ein durch ein Gelenk
übertragener Impuls kann so in der nächsten Iteration durch einen Bodenkontakt
abgestützt werden. Restitutionsziele stammen weiterhin aus den ursprünglichen
Kontaktgeschwindigkeiten. Die Gelenkstabilisierung erhält den tatsächlichen
Zeitschritt `dt_s`; die Funktion integriert selbst keine Bewegung.

Das Ergebnis enthält den Kontaktbericht, die akkumulierten Impulse auf A für jedes
Gelenk und den maximalen Gelenk-Geschwindigkeitsrestfehler. Beide Arten von
Geschwindigkeitsrestfehler werden aus dem gemeinsamen Endzustand vor der
Positionskorrektur berechnet. Ein knappes Iterationsbudget oder widersprüchliche
Bedingungen können trotz `PS_OK` erhebliche Restfehler hinterlassen.

Danach folgt die bisherige Translationskorrektur der Kontakte. Eine direkte
Positions-/Orientierungsprojektion der Gelenke findet nicht statt. Diese Korrektur
kann Gelenklängen verändern; `max_joint_length_error_m` meldet deshalb den maximalen
absoluten Längenfehler **nach** der Kontaktprojektion. Für passende Modelle kann
`correction_fraction=0` mit kleinen Schritten und Gelenkstabilisierung verwendet
werden. Das ersetzt keine gemeinsame nichtlineare Positionslösung.

Ein Fehler, auch in einem erst später bearbeiteten Gelenk, lässt alle ursprünglichen
Körper und das Ergebnis unverändert. Leere Arrays dürfen bei Anzahl 0 `NULL` sein;
das Ergebnis ist optional. Ohne Gelenke entspricht die Rechnung dem bisherigen
`ps_contacts_resolve_graph`.

Referenzen prüfen zwei gekoppelte Körper mit Bodenkontakt, die gemeinsamen
Ruhe-/Rückprallgeschwindigkeiten und Kontaktimpulse, unzureichende Iterationen,
widersprüchliche Gelenke, Änderungen der Länge durch Kontaktprojektion,
Fehleratomizität und alle drei gleichzeitig ausgeschöpften Kapazitätsgrenzen.

## Kontaktketten und Stapel

`ps_contacts_resolve_graph` verarbeitet bis zu 128 Körper und 512 Kontaktpunkte
gemeinsam. Jeder `ps_contact_constraint` enthält zwei Indizes in das Körperarray
und einen bereits ermittelten Kontakt. Für eine feste Umgebung steht an Stelle
von B `PS_CONTACT_WORLD`. Ein Manifold mit mehreren Punkten wird in entsprechend
viele Einträge aufgeteilt. Die Normale zeigt weiterhin von A nach B.

Der Solver akkumuliert Normal- und Reibungsimpulse und iteriert in der vorgegebenen
Kontaktreihenfolge über das gesamte Netz. Dadurch kann sich ein Impuls entlang
einer Kette ausbreiten. Restitutionsziele werden einmal aus den anfänglichen
Kontaktgeschwindigkeiten bestimmt. Ein gemeinsamer `ps_contact_solver` legt
Reibung, Restitution, Iterationszahl und Korrektureinstellungen fest; individuelle
Materialparameter pro Kontakt sind noch nicht vorgesehen.

Nach den Geschwindigkeitsiterationen folgen ebenso viele Durchläufe der
Translationskorrektur. Dabei werden bereits durch andere Kontakte verursachte
Verschiebungen berücksichtigt. Orientierung, Kontaktpunkte und Normalen bleiben
während des Lösens fest. Kontakte müssen in jedem Simulationsschritt neu erzeugt
werden. Das Verfahren ist keine automatische Kollisions- oder Ereignissteuerung.

`ps_contact_graph_solution` enthält pro Kontakt den gesamten Impuls auf A und zwei
Restfehler: `max_normal_error_m_s` beschreibt die Normalgeschwindigkeitsbedingung
vor der Positionskorrektur; `max_projection_error_m` die noch nicht erfüllte
angeforderte Translationskorrektur. `PS_OK` bestätigt die abgearbeiteten Iterationen,
nicht die Konvergenz. Nicht korrigierbare statische Überlappungen bleiben deshalb
am zweiten Restfehler erkennbar. Bei identischer Eingabereihenfolge ist der Ablauf
deterministisch; nach Umsortieren können endliche Iterationsbudgets andere
Näherungen liefern.

Der Solver benötigt keine Heap-Allokationen. Arbeitskopien garantieren, dass bei
Fehlern alle Körper und der optionale Bericht unverändert bleiben. Ein-/Ausgaben
müssen getrennte Speicherbereiche sein. Warm-Starting über Zeitschritte,
Gelenkbedingungen und automatische Kontaktverwaltung bleiben offen.

Referenzen prüfen die gemeinsame Endgeschwindigkeit und Energie einer
inelastischen Dreikörperkette, einen gestützten Dreikörperstapel, unzureichende
Iterationsbudgets und beide Größenlimits. Ein Reibungstest mit vier Boxkontakten
liefert bitgleiche Körper und Impulse wie der bisherige Paarsolver, der dieselben
numerischen Rechenschritte verwendet.

## Kontinuierliche Kugelkontakte

`ps_sweep_spheres` und `ps_sweep_sphere_plane` suchen den ersten Kontakt entlang
einer **linearen Verschiebung**. Die Verschiebung wird in Metern übergeben,
beispielsweise `velocity * dt`; gespeicherte Körpergeschwindigkeiten werden nicht
automatisch verwendet. Bei zwei Kugeln zählt ihre relative Verschiebung, bei
einer Ebene bleibt die Ebene fest. Das Ergebnis `ps_sweep_hit` enthält den Anteil
`fraction` zwischen 0 und 1 sowie Kontaktpunkt und Normale. Die Normale folgt
dem bestehenden Vertrag: von Kugel A zu B beziehungsweise in den festen
Halbraum der Ebene. Bei einem neuen Kontakt ist die Eindringtiefe null.

Die Funktionen verändern keine Körper. Der Aufrufer bewegt sie zunächst bis
zum Kontakt, löst dessen Impulsantwort und berechnet anschließend die Bewegung
für die verbleibende Zeit neu. Beschleunigte oder gekrümmte Bahnen erfordern eine
geeignete Unterteilung oder einen anderen Detektor. Boxrotation und eine automatische Ereignissteuerung für mehrere Körper bleiben
offen. Lineare Kugel–Box-Bewegung kann mit einer Box als konvexem Netz über den
unten beschriebenen Kugel–Netz-Sweep geprüft werden.

Bereits berührende oder überlappende Körper melden Anteil 0, auch wenn sie sich
entfernen. Eine Ereignisschleife muss diese Anfangskontakte behandeln und
Zeitfortschritt sicherstellen. Ohne Treffer wird nur `touching=false` gesetzt;
der Trefferwert bleibt unverändert. Fehler lassen beide Ausgaben unverändert.
Extreme Größenverhältnisse begrenzen die Genauigkeit von Kontaktzeit und Ort.

Für die Broad Phase müssen die Hüllboxen den **gesamten Bewegungsweg** abdecken.
`ps_aabb_swept_sphere` erzeugt dazu die Vereinigung der nach außen gerundeten
Anfangs-/Endhüllboxen einer linear bewegten Kugel. Hüllboxen nur am Start- oder
Endzeitpunkt können einen tatsächlichen Kontakt zwischen den Zeitpunkten
aussortieren. Unendliche Ebenen werden weiterhin separat geprüft.

Die Tests prüfen Durchtunneln, Streifkontakte, Anfangsüberlappung, beide bewegten
Partner, Kontakt am Intervallende, Vorbeiflug, unveränderte Ausgaben bei Fehlern
und lange Bahnen mit kleinen Zielkugeln. Eine elastisch an einer Ebene reflektierte
Kugel wird bis zum Ereignis und durch die Restzeit bewegt; Endposition und
kinetische Energie werden gegen die analytische Lösung geprüft.

## Lineare konvexe Sweeps

`ps_sweep_convexes` schneidet die Zeitintervalle aller Flächen- und
Kantenkreuzprodukt-Trennachsen über den geschlossenen Anteil `[0,1]`.
`ps_sweep_convex_plane` verwendet den ersten Vertexkontakt zum festen Halbraum.
`ps_sweep_sphere_convex` ermittelt Eintrittszeiten gegen Dreiecksflächen,
Kantenzylinder und Vertexkugeln; dadurch zählen auch Kanten-/Eckkontakte,
die ein Test nur gegen aufgeweitete Flächenebenen falsch klassifizieren würde.
Das Netz besitzt während der gesamten Anfrage **feste Orientierung**.
Beide Körper können sich entlang ihrer expliziten Verschiebung bewegen.
Gespeicherte lineare und angulare Geschwindigkeiten werden nicht integriert.

Anfangsberührung und Überlappung zählen bei `fraction=0`, auch bei Trennung;
die Eindringtiefe stammt dann aus der diskreten Abfrage. Neue Treffer haben
Eindringtiefe null und einen einzelnen Kontakt. Erfolg ohne Treffer ändert
nur `touching=false`; Fehler erhalten Trefferwert und Flag sowie sämtliche
Körper-/Netzdaten. Invalid-/Limit-/Numeric-Codes entsprechen den diskreten
Geometrieverträgen. Sehr unterschiedliche Größen und lange Bewegungswege
begrenzen Kontaktort und Zeitgenauigkeit; der Zeugenvergleich berücksichtigt
Rundung der Bewegung. Eine unverändert grüne Anfrage ist keine Aussage über
rotierende oder beschleunigte Bahnen.

`ps_aabb_swept_convex` vereinigt gepufferte Anfangs-/Endgrenzen, die bei fester
Orientierung den gesamten linearen Vertexweg enthalten. Diese Hülle gehört
in die Broad Phase. Anfangs-/Endhüllen einzeln können trotz eines Kontakts
zwischen den Zeitpunkten getrennt bleiben. Die Hülle deckt keinen Rotationsweg ab.

Physim verwendet `Sweep.convexes`, `Sweep.convexPlane`, `Sweep.sphereConvex`
und `Aabb.sweptConvex` mit besitzenden Vertex-/Indexarrays. `Sweep.hit`,
`fraction()` und `contacts()` folgen dem bestehenden Kugelvertrag. Ein Modell
bewegt Körper bis zum Ereignis, löst den Impuls und integriert die Restzeit mit
der neuen Bewegung. Die C-/Physim-Tetraederbeispiele tun dies tatsächlich:
Die Kugel startet bei `(10,1,1)` mit `vx=-20` m/s, erreicht den statischen Vertex
bei Anteil `0.425`, prallt elastisch zurück und endet nach einer Sekunde bei
`x=13` m mit unveränderter kinetischer Energie von 200 J. Der zentrale Impuls
induziert hier keine Rotation; es ist ein isoliertes kraftfreies Ereignis.

Allgemeine rotierende Sweeps und eine Ereignissteuerung für mehrere Körper
bleiben offen. Diese Grenzen ersetzen nicht die vollständige CCD-Anforderung
im Projektplan.

## Kandidatenpaare für mehrere Körper

`physim/collision.h` ergänzt eine Broad Phase für bis zu 1024 Hüllboxen. Mit
`ps_aabb_sphere` und `ps_aabb_box` entstehen achsenparallele Hüllboxen in
Weltkoordinaten; die Boxfunktion berücksichtigt die Quaternionorientierung.
Die Grenzen werden nach außen gerundet. Boxen erhalten zusätzlich einen kleinen
größenabhängigen Rundungspuffer. Dadurch sind zusätzliche Kandidaten möglich.
Unendliche Ebenen werden separat gegen die Körper geprüft.

`ps_broad_phase(bounds, count, pairs, capacity, &pair_count)` sortiert zunächst
nach der unteren X-Grenze und prüft die überlappenden Intervalle auf Y und Z.
Berührung zählt als Überlappung. Die Ausgabe enthält Eingabeindizes `a < b`,
lexikografisch sortiert und ohne Duplikate. Für jeden Kandidaten folgt erst die
passende genaue Kontaktprüfung, zum Beispiel `ps_contacts_boxes`.
Eine überlappende Hüllbox allein ist kein Beleg für einen Kontakt.

Die Funktion verwendet feste Stackpuffer und Heap-Sortierung ohne Allokationen.
Im ungünstigsten Fall entstehen `n*(n-1)/2` Paare; einschließlich ihrer Sortierung
beträgt der Aufwand dann O(n² log n). Bei räumlicher Trennung auf X endet die
Paarprüfung früher. `pairs=NULL, capacity=0` dient als Größenabfrage. Reicht die
Kapazität nicht, liefert die Funktion `PS_LIMIT` und die benötigte Anzahl, ohne
das Paararray anzutasten. Andere Fehler lassen beide Ausgaben unverändert; eine
Überschreitung des Körperlimits liefert `PS_LIMIT` ohne neue Anzahl.

Statische Paare und Kollisionsgruppen filtert der Aufrufer. Hüllboxen werden nach
Positions- oder Orientierungsänderungen neu erzeugt. Die Prüfung beschreibt nur
den aktuellen Zeitpunkt; schnelle Bewegungen benötigen weiterhin CCD oder
ausreichend kleine Zeitschritte. Die Kontaktantwort erfolgt anschließend mit dem Paar- oder Graph-Solver.

## Allgemeine konvexe Polyeder

`ps_convex_mesh` leiht bis zu 64 körperlokale Vertices in Metern und 128
nach außen orientierte Dreiecke. Das Netz muss ein geschlossenes, dreidimensionales
konvexes Volumen bilden. Jede Kante tritt zweimal mit entgegengesetzter Richtung
auf; alle Vertices werden benutzt. Koplanare Flächen dürfen trianguliert sein.
Die Prüfung verwirft offene, konkave, doppelte und degenerierte Geometrie.
Sie erzeugt keine konvexe Hülle aus einer beliebigen Punktwolke.

`ps_contact_convexes` prüft Flächennormalen und Kantenkreuzprodukte nach dem
[Trennachsenverfahren von David Eberly](https://www.geometrictools.com/Documentation/MethodOfSeparatingAxes.pdf).
Auch vollständiges Enthaltensein ergibt eine Eindringtiefe. Zur gewählten
minimalen Trennverschiebung werden Oberflächenzeugen bestimmt; ihr gemeinsamer
Mittelpunkt in der ursprünglichen Lage wird als **ein Kontakt** ausgegeben.
Dieser lässt sich mit `ps_contact_resolve` beziehungsweise als einzelne
Graphbedingung lösen. Die Repräsentation bildet kein stabiles Mehrpunktmanifold
für ruhende Flächenkontakte. Für Boxstapel bleibt das Boxmanifold geeignet.

`ps_contact_sphere_convex` ermittelt die nächste Dreiecksoberfläche für äußere
und innere Kugelmittelpunkte. `ps_contact_convex_plane` liefert den tiefsten
Vertexkontakt zum festen Halbraum; die Ebenennormale zeigt in den freien Raum.
`ps_aabb_convex` enthält alle transformierten Vertices mit Rundungspuffer und
kann an die vorhandene Broad Phase übergeben werden. Eine Box ist bei Bedarf
selbst ein konvexes Netz mit acht Vertices; die vorhandenen Boxdetektoren bleiben
für ihre Mehrpunktkontakte verfügbar.

Geometrie wird vor Kreuzprodukten skaliert. Toleranz ist `128*DBL_EPSILON` in
den normierten Koordinaten. Sehr dünne Dreiecke, fast zusammenfallende Vertices
oder nach gemeinsamer Skalierung nicht mehr auflösbare Kanten werden verworfen.
Ungültige Netze liefern `PS_INVALID`, übergroße Netze `PS_LIMIT`, nicht
auflösbare transformierte Geometrie oder Ausgaben `PS_NUMERIC`. Fehler erhalten
Körper und beide Ausgaben; Erfolg ohne Treffer ändert nur `touching`.
Die Reihenfolge der Netzdaten bestimmt Gleichstandsentscheidungen. Die
Kontaktabfrage ist diskret; die oben beschriebenen Sweeps ergänzen lineare Bewegung.
Die persistente Kontaktwelt
verwaltet weiterhin Kugeln, Boxen und Ebenen; eigene konvexe Kontakte können
über die öffentliche Graph-API eingebunden werden.

Masse, Schwerpunkt und Hauptträgheiten gehören zum Modell. Mit
`ps_body_with_inertia` beziehungsweise `Body.withInertia(mass,principalInertia)`
entsteht ein ruhender Körper mit expliziten Masseneigenschaften. Positive Masse
verlangt drei positive endliche Hauptträgheiten; Masse null verlangt Nullträgheit.
Die Netzkoordinaten müssen zum Schwerpunkt und zu den gewählten Hauptachsen
passen. Physim benutzt besitzende `Array<Vec3>`-Vertices und flache
`Array<Int64>`-Dreieckstripel für `Contacts.convexes`, `Contacts.sphereConvex`,
`Contacts.convexPlane` und `Aabb.convex`. Die Arrays bleiben bei jeder Abfrage
unverändert; Fehlercodes bleiben in abfangbaren Runtime-Diagnosen erhalten.

Die vollständigen Beispiele sind
[C](../examples/convex_contacts/main.c) und
[Physim](../examples/language/convex_contacts.phys). Ein homogener regulärer
Tetraeder mit den dortigen Vertices hat Schwerpunkt null und drei
Hauptträgheiten `2*m/5` kg·m²: Der Mittelwert von jedem Koordinatenquadrat ist
`1/5`, also ist beispielsweise `Ix=m*(E[y²]+E[z²])`. Beide Beispiele prüfen
Kugelkontakt, gedrehte Geometrie gegen eine Ebene, konservative Hüllgrenzen und
ein elastisches Kugelereignis gegen den statischen Tetraeder mit Restzeit.

## Fehler und Prüfungen

Die Mechanikfunktionen allokieren keinen Speicher und übernehmen Ausgaben erst nach Erfolg.
Für die Broad Phase gilt die oben beschriebene Ausnahme zur benötigten Paarkapazität.
`PS_INVALID` meldet verletzte Eingabeverträge, `PS_NUMERIC` nicht darstellbare Ergebnisse.
Ein Fehler verändert weder Körper noch Ausgabeparameter. Detektoren mit `touching`
setzen bei Erfolg ohne Kontakt nur `touching=false`; der Kontaktwert bleibt erhalten.
Die Box–Ebene- und Box–Box-Detektoren schreiben stattdessen ein Manifold mit `count=0`.

Referenzen prüfen Kugel-/Boxträgheit, rotierte Trägheitsachsen, Drehmomente,
freie Kugelrotation, Konvergenz asymmetrischer Rotation, Gravitation, elastischen
und inelastischen Stoß, linearen und angularen Impuls bei Berührung, gleitenden und
haftenden Tangentialimpuls sowie linearen und quadratischen Widerstand.
Zusätzlich läuft die echte Stoßvorlage mit Vakuum, Luft, eigenem Medium und Reibung
im Runner; sämtliche 401 Messpunkte werden geprüft.
Kontaktprüfungen umfassen orientierte Boxgeometrie, redundante Kontaktpunkte,
gleitende und haftende Mehrpunktkontakte sowie 2000 Schritte ruhenden Bodenkontakt.
Die echte Boxvorlage wird zusätzlich über zehn Sekunden gegen Energiegrenzen,
Bodenabstand, Kontaktimpulse und ihren abschließenden Ruhezustand geprüft.
Für Box–Box werden 12000 deterministische Paare mit unabhängigen Projektionen aller
Eckpunkte verglichen. Kontaktpunkte werden auf beiden Oberflächen geprüft, außerdem
Vertauschung der Körper, Wiederholbarkeit, Größenextreme und Impuls-/Drehimpulserhaltung.
Die Boxstoßvorlage läuft elastisch, inelastisch und gedreht mit Reibung im Runner;
alle 401 Samples jedes Laufs werden gegen Erhaltungsgrößen und Referenzen geprüft.

## Persistente Kontaktverwaltung

Für automatisch regenerierte Kugel-/Box-/Ebenenkontakte ergänzt `contact_world.h`
einen caller-eigenen Zustand mit stabilen Collider-IDs, lokaler Punktzuordnung,
Lebensdauerzählungen und Warmstart des Graphsolvers. Vorhandene Paar- und
Graphfunktionen bleiben kalt und unverändert. [Vertrag und Stapelbeispiel](contact-world.md).
