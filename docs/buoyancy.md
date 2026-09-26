# Auftrieb: schwimmende Kugel

1. In der Seitenleiste **Auftrieb** auswählen und einen neuen Projektordner anlegen.
2. Mit F5 bauen, dann simulieren. Die gelbe Kugel schwingt gedämpft um die Wasserlinie.
3. Stoppen und auswerten: Bewegung, Phasenraum, Geschwindigkeitsverteilung und
   Energiebilanz erscheinen als vier Diagramme. Die Tabellen lassen sich als CSV,
   Diagramme als SVG oder PNG exportieren.

Das blaue Gitter markiert die ebene Wasseroberfläche bei `y=0`. Grün zeigt Auftrieb,
Rosa das Gewicht und Violett die Dämpfung; die Pfeilskala beträgt 0,006 m pro Newton.
Der grüne Punkt zeigt die Höhe des Auftriebsmittelpunkts, zur Sichtbarkeit auf die
Vorderseite der Kugel versetzt. Das Gitter ist keine Wand und kein Kollisionsobjekt.

## Modell und Parameter

Eine homogene Kugel mit Radius 0,1 m bewegt sich auf einer vorgegebenen vertikalen
Führung. Das Fluid ist ein unendlich großes, ruhendes Wasserreservoir mit konstanter
Dichte 1000 kg/m³. Die Kugeldichte beträgt zunächst 500 kg/m³, ihre Masse rund
2,0944 kg. Damit liegt die Gleichgewichtslage bei halb eingetauchter Kugel.
Der Mittelpunkt startet 0,04 m oberhalb der Oberfläche, ohne Anfangsgeschwindigkeit.

`ps_sphere_submersion` liefert das tatsächlich eingetauchte Volumen und dessen
Schwerpunkt; `ps_buoyancy_force` berechnet `rho*V*g`. Gewicht und die ausdrücklich
empirische Dämpfung `-c*Eintauchanteil*v` werden addiert. Der Standardwert ist
`c=1,2 N s/m`. Dieses Dämpfungsmodell ist keine Stokes- oder quadratische
Widerstandsnäherung. Bewegung und dissipierte Arbeit werden mit denselben RK4-Stufen
integriert. Die Referenzprüfung verwendet `dt=0,005 s`; bei Parameteränderungen
die Schrittweite prüfen und gegebenenfalls verkleinern.

In `main.c` lassen sich folgende Vorgaben ändern und neu bauen:

| Vorgabe | Beispiel | Wirkung |
| --- | --- | --- |
| `PS_FLOAT_HEIGHT` | `0` | Start in der Gleichgewichtslage bei halber Wasserdichte |
| `PS_FLOAT_DENSITY` | `1000` | Vollständig eingetaucht schwebend, bei ruhigem Start |
| `PS_FLOAT_DENSITY` | `1500` | Schwerer als Wasser, sinkt |
| `PS_FLOAT_HEIGHT` | `-0.5` | Start vollständig unter Wasser |
| `PS_FLOAT_DAMPING` | `0` | Keine Dämpfung, anhaltende Schwingung |

Es gibt keinen Boden: Eine schwere Kugel kann aus dem Kameraausschnitt sinken.
Wellen, Oberflächenspannung, Zusatzmasse, Strömung, Rotation und Wandkontakte sind
nicht modelliert. Die Darstellung begrenzt weder Simulationsraum noch Rohdaten.

## Messwerte und Energiebilanz

Die zwölf Kanäle enthalten Höhe, Geschwindigkeit, verdrängtes Volumen, Eintauchanteil,
Auftrieb, Gewicht, Dämpfung, Gesamtkraft, mechanische Energie, dissipierte Arbeit,
Gesamtbilanz und Höhe des Auftriebsmittelpunkts relativ zum Kugelzentrum.

`energy` umfasst kinetische Energie und das effektive Potential von Körper und
idealem hydrostatischem Reservoir, mit `U(0)=0`. Es ist nicht allein die Energie
der Kugel. Das Potential erfüllt `dU/dy = m*g - F_A`; außerhalb des teilweise
eingetauchten Bereichs wird es stetig mit der jeweiligen konstanten Kraft fortgesetzt.
`energy.dissipated` integriert `c*Eintauchanteil*v²` und ist nichtnegativ.
`energy.balance` addiert beide Größen. Die Analyse bewertet dessen numerische
Abweichung, statt physikalische Dämpfung als Energiefehler auszugeben.

Die gemeinsame Analyse bevorzugt `position.x`, dann `a.position`, schließlich
`position.y`. Für diese Vorlage wird ausschließlich die vertikale Position verwendet.
Laufvergleiche sind fachlich sinnvoll zwischen Varianten desselben Modells und
derselben Achse; die Auswahl ist keine automatische Modellvergleichsprüfung.

## Referenzprüfung

`floating_reference` startet das echte Modul im Runner: 2001 Werte der gedämpften
Schwingung sowie je 21 Werte für Gleichgewicht, neutrales Schweben, Sinken und
Steigen. Die vollständig eingetauchten Fälle werden mit der analytischen Bewegung
bei konstanter Beschleunigung verglichen. Eine unabhängige Polynomgleichung mit
20-fach feinerer Integration prüft die Schwingung. Weitere 4001 Werte mit halbierter
Schrittweite prüfen die Konvergenz vierter Ordnung. Auch Szenenobjekte, Reset,
Fehler ohne Zustandsänderung und der gespeicherte Analysebericht werden geprüft.

Lokal sank der maximale Geschwindigkeitsfehler beim Halbieren der Schrittweite
von 1,5911e-6 auf 1,0069e-7 m/s (Faktor 15,80). Die Energiebilanz wird über zehn
Sekunden mit einer absoluten Fehlerschranke von 2e-7 J geprüft. Diese Grenzen gelten
für die dokumentierten Parameter und sind keine allgemeine Stabilitätsgarantie.

Die zugrunde liegende API und ihre Geometrieprüfung stehen in
[Mechanik & Medien](mechanics.md).
