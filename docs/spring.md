# Feder–Masse–Dämpfer

## Lernziel und Ablauf

Untersuche, wie Dämpfung eine Schwingung verändert, und trenne physikalischen
Energieverlust vom Integrationsfehler.

1. Unter **Öffnen oder anlegen** die Vorlage **Feder–Masse–Dämpfer** wählen.
2. Mit F5 bauen, unter **Simulieren** starten und mindestens einige Sekunden laufen lassen.
3. Stoppen, **Auswerten**, **Analyse starten**. Im Diagrammmenü **Energiebilanz mit
   Dämpfung** wählen. Die drei Kurven zeigen mechanische Energie, dissipierte
   Energie und deren Summe. Die Ergebnistabelle **Energiebilanz · inklusive
   Dissipation** nennt die maximale Abweichung der Summe von ihrem Anfangswert.
4. In `main.c` `PS_SPRING_DAMPING` ändern, erneut bauen und einen neuen Lauf starten.
   Unter **Läufe & Berichte** beide Läufe auswählen und vergleichen. Der Vergleich
   zeigt Position, Geschwindigkeit und die interpolierte Positionsdifferenz.

Die Vorlage verwendet `physim/experiment.h` für Lebenszyklus, Messkanäle und Szene,
`physim/core.h` über diesen Header für RK4 und Vektoren sowie
`physim/mechanics.h` für die axiale Federkraft mit viskoser Dämpfung.
`analysis.c` ist dieselbe bearbeitbare Analysevorlage wie beim Pendel. Sie erkennt
die zusätzlich vorhandenen Energiekanäle automatisch. Bereits bestehende Projekte
behalten ihren bisherigen Analysecode.

## Modell und Gleichungen

Eine Masse bewegt sich auf einer vorgeschriebenen horizontalen Führung. Eine
masselose lineare Feder und ein linearer viskoser Dämpfer verbinden sie mit einem
festen Punkt. Die Gleichgewichtslage liegt bei X=0, der Anker bei X=-L. Solange die
Masse rechts des Ankers bleibt, ist X zugleich die Federdehnung:

```text
m*x'' + c*x' + k*x = 0
Federkraft = -k*x       Dämpfungskraft = -c*v
E_mechanisch = m*v²/2 + k*x²/2
dE_dissipiert/dt = c*v²
E_Bilanz = E_mechanisch + E_dissipiert ≈ E_mechanisch(0)
```

Die Dämpfungsfälle folgen aus `omega0 = sqrt(k/m)` und `alpha = c/(2*m)`.
Unterkritisch gilt `x = exp(-alpha*t)*(A*cos(w*t)+B*sin(w*t))` mit
`w = sqrt(omega0²-alpha²)`. Kritisch gilt `x = (A+B*t)*exp(-alpha*t)`;
überkritisch ist die Lösung die Summe zweier reeller abklingender Exponentialterme.
Die Konstanten folgen aus Anfangsort und Anfangsgeschwindigkeit. Gleichung,
Dämpfungsfälle und Energierate sind in [MIT: Damped Oscillations, Abschnitte 2–3](https://ocw.mit.edu/courses/res-8-009-introduction-to-oscillations-and-waves-summer-2017/mitres_8_009su17_lec4.pdf)
hergeleitet.

## Parameter und erwartete Ergebnisse

Standard sind m=1 kg, k=16 N/m, c=1,2 N·s/m, L=1 m, x(0)=0,35 m und v(0)=0.
Die anfängliche Energie beträgt 0,98 J. Die Schwingung klingt ab; ihre gedämpfte
Periode beträgt etwa 1,589 s. Alle Parameter stehen im Quellcode und in den
Laufmetadaten.

| Dämpfung c in N·s/m | Verhalten bei diesen Anfangswerten |
| --- | --- |
| 0 | ungedämpfte Schwingung, Periode pi/2 s |
| 1,2 | abklingende Schwingung |
| 8 | kritische Dämpfung, Rückkehr ohne Überschwingen |
| 12 | überkritische, langsamere Rückkehr ohne Überschwingen |

Die elf Kanäle enthalten Position, Geschwindigkeit, kinetische und elastische
Energie, mechanische Energie, dissipierte Energie, Energiebilanz, beide Kräfte,
Gesamtkraft und dissipierte Leistung. Die Bilanz nutzt eine separat integrierte
Arbeitsvariable, die dieselben RK4-Stufen wie die Bewegung verwendet; sie wird
nicht nachträglich auf konstante Energie gesetzt.

Die Szene zeigt eine blaue Masse, die Feder als räumliche Linie, einen schematischen
Dämpfer und eine weiße Gleichgewichtsmarkierung. Grüne Pfeile stellen Geschwindigkeit
dar, gelbe Federkraft und rosa Dämpfungskraft. Die angegebenen Pfeilskalen sind
Darstellungshilfen; Federwindungen und Dämpferkörper tragen keine eigene Dynamik.

## Numerische Grenzen und Prüfungen

RK4 ist explizit und nicht für beliebig steife Federn oder große Dämpfung bei
unverändertem Zeitschritt stabil. Zeitschritt verkleinern und Ergebnisse vergleichen.
Die Vorlage enthält weder Gravitation noch Kontakt, Federanschläge, trockene Reibung,
nichtlineare Kennlinien, Federmasse oder ein Temperaturmodell. Die Führung ist
vorgeschrieben; sie verwendet keinen allgemeinen Constraint-Solver. Das Durchqueren
des Ankers wird als außerhalb des Modells abgewiesen.

Automatische Tests vergleichen vier reale Läufe über je zehn Sekunden mit den
geschlossenen Lösungen. Bei dt=0,005 s gelten absolute Grenzen von 3e-8 m für den
Ort, 1e-7 m/s für die Geschwindigkeit und 1e-7 J für Energie und Bilanz. Die
Halbierung von dt=0,05 auf 0,025 s bei t=1 muss den Zustandsfehler um einen Faktor
zwischen 14 und 18 verkleinern. Diese Grenzen betreffen genau die getesteten
Parameter. Zusätzlich werden alle Punkte der drei Energiekurven, die Bilanzkennzahl
und der im Analysemanifest benannte Messkanal geprüft.

Die Standardanalyse verwendet `energy.balance` für die Energieabweichung, falls
dieser Kanal vorhanden ist; sonst verwendet sie weiterhin `energy`.
`energy_metric_channel` im Analysemanifest macht die Auswahl nachvollziehbar.
Ein abnehmender mechanischer Energieinhalt allein ist bei aktiver Dämpfung kein
Nachweis eines numerischen Fehlers.
