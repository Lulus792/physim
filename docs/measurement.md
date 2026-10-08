# Messungen & Sensoren

Physim trennt den idealen Modellzustand von der simulierten Messung. Die öffentliche
[Messungsreferenz](reference/measurement.md) bietet konstante, uniforme und normale
Verteilungen sowie einen zustandsbehafteten Sensor ohne Speicherallokation.
Diese Anleitung und die Funktionsreferenz sind offline unter F1 direkt in der App verfügbar.

## Sensor einrichten

```c
#include "physim/measurement.h"

ps_sensor_config config = {0};
config.unit = PS_METRE;
config.rate_hz = 100;
config.resolution = .005;     /* Auflösung: 5 mm */
config.offset = .01;          /* konstanter Bias: 1 cm */
config.drift_per_s = .002;    /* zusätzlich 2 mm pro Sekunde */
config.noise = (ps_distribution){PS_DIST_NORMAL, 0, .02};
config.dropout_probability = .05;
config.uncertainty_absolute = .003;
config.uncertainty_relative = .001;

ps_sensor sensor;
ps_result result = ps_sensor_init(&sensor, &config, 42);
if (result != PS_OK) return result;
ps_measurement sample;
result = ps_sensor_read(&sensor, 0, (ps_quantity){-2, PS_METRE}, &sample);
if (result != PS_OK) return result;
if (sample.state == PS_MEASUREMENT_VALID) {
    /* Erst hier sample.value.value und sample.standard_uncertainty verwenden. */
}
```

Ein Sensor gehört dem Aufrufer. `ps_sensor_reset` setzt Raster und Seed zurück;
`ps_sensor_next_time` liefert den nächsten fälligen Zeitpunkt. Der Einheitentext
ist geliehen und muss so lange wie Sensor und ausgegebene Messungen gültig bleiben.
Eigene Instanzen dürfen parallel laufen; eine einzelne Instanz benötigt bei
gleichzeitigen Zugriffen Synchronisierung. Fehler lassen Zustand und Ausgabe unverändert.

## Messungen typisiert nach SI veröffentlichen

Die Sensor-Ausgabe bleibt in der konfigurierten Einheit, auch die absolute
Standardunsicherheit. Für einen Meterkanal kann ein Zentimetersensor deshalb
über `ps_channel_sample_quantity` beziehungsweise `Channel.sampleQuantity`
veröffentlicht werden. Dimensionen werden geprüft und beide Zahlen nach SI
konvertiert; falsche Dimensionen und nicht darstellbare SI-Werte erhalten den
vorherigen Kanalwert.

```c
if (sample.state == PS_MEASUREMENT_VALID) {
    result = ps_channel_sample_quantity(context, value_channel, sample.value);
    if (result != PS_OK) return result;
    result = ps_channel_sample_quantity(context, uncertainty_channel,
        (ps_quantity){sample.standard_uncertainty, sample.value.unit});
    if (result != PS_OK) return result;
}
```

```physim
if reading.isValid():
    length.sampleQuantity(reading.value)
    uncertainty.sampleQuantity(Quantity(reading.standardUncertainty,reading.value.unit))
```

Der Setter erkennt Quantity-Dimensionen, nicht den Messstatus. Platzhalter für
NOT_DUE/DROPPED bleiben ausdrücklich durch den eigenen Statuskanal markiert;
der Setter macht daraus keine gültigen Messwerte. Die Sensor-Wurfbeispiele
benutzen den typisierten Pfad für Wert und Standardunsicherheit tatsächlich.
Der C-Wurf verwendet dabei einen lokalen Context zum Staging, damit mehrere
Kanalupdates gemeinsam mit dem Modellzustand zurückgenommen werden können.

## Abtastraster und Gültigkeit

Das Raster ist `start_time_s + index / rate_hz`. Modellzeitschritt und Sensorfrequenz
dürfen sich unterscheiden. Bei 200 Modellschritten/s und 100 Sensorwerten/s ist jeder
zweite Schritt nicht fällig. Sehr kleine Rundungsabweichungen an Rastergrenzen werden
toleriert; die genaue Grenze steht im Header. Lange Modellläufe sollten ihre Zeit
aus einem ganzzahligen Schrittindex oder durch kompensierte Addition bestimmen.

| Zustand | Bedeutung | Zahlenwert auswerten? |
| --- | --- | --- |
| `PS_MEASUREMENT_NOT_DUE` / 0 | Noch kein neuer Abtastzeitpunkt | Nein |
| `PS_MEASUREMENT_VALID` / 1 | Neue gültige Messung | Ja |
| `PS_MEASUREMENT_DROPPED` / 2 | Fällige Messung ausgefallen | Nein |

Ein verspäteter Aufruf misst das übergebene Signal zur **tatsächlichen Aufrufzeit**.
`skipped` nennt übersprungene Rasterplätze; vergangene Werte werden nicht erfunden.
`time_s` ist der tatsächliche Zeitpunkt. Ein erneuter Aufruf vor dem nächsten Termin
ändert den Sensor nicht. Aufrufe vor dem Start oder vor der letzten Abtastung sind ungültig.
Nicht darstellbare Zeitraster und übergroße Indizes liefern einen Fehler.

Für nicht gültige Messungen sind Wert und Unsicherheit Null-Platzhalter, keine Messdaten.
Die Laufdatei erhält alle Zeilen. Die Konvention `<Kanal>.status` maskiert `<Kanal>` und
`<Kanal>.u`; `ps_channel_status_index` aus der [Datenreferenz](reference/data.md) findet die
zugehörige dimensionslose Statusspalte. Statuswerte außerhalb 0/1/2 werden abgewiesen.
Die App zeigt für solche Kanäle nur gültige Punkte und berechnet ihre Statistik daraus.
Bei null gültigen Werten bleiben Kennzahlen leer; bei nur einem Wert bleibt die
Stichprobenstandardabweichung leer. Die allgemeine CSV-Zusammenfassung verhält sich ebenso.

**Eigene Series-Auswertungen müssen den Status ausdrücklich beachten.** Series und
der Rohdaten-CSV-Export erhalten alle Zeilen einschließlich Platzhaltern. Sie filtern
nicht automatisch, da sonst die Ausrichtung zu Modell und Zeitachse verloren ginge.
Das Analysebeispiel zeigt blockweises Lesen, explizites Filtern und vollständigen Export.

## Messmodell und Unsicherheit

Der Sensor konvertiert das ideale Signal in seine konfigurierte Einheit und bildet:

`Messung = quantisieren(ideal + offset + drift_per_s * (time - start) + Rauschen)`

Auflösung null schaltet Quantisierung ab. Sonst wird auf das nächste Vielfache der
Auflösung gerundet, bei exakt halbem Abstand zum geraden Vielfachen. Offset und Drift
modellieren unkompensierte systematische Effekte. Sie verändern das physikalische
Modell nicht. Das Rauschen wird additiv in der Sensoreinheit angegeben.

Die ausgegebene **Standardunsicherheit** ist:

`u = sqrt(u_abs² + (abs(ideal) * u_rel)² + noise_stddev² + resolution² / 12)`

Die Komponenten werden als unabhängig angenommen. Die Auflösungskomponente setzt
gleichverteilten Quantisierungsfehler innerhalb eines Quantisierungsintervalls voraus.
Diese Annahme ist insbesondere bei kleinen, konstanten oder korrelierten Signalen
nicht automatisch erfüllt. Bekannte Offset-/Driftwerte werden weder als Unsicherheit
mitgezählt noch korrigiert. Absolute und relative Unsicherheit sind zusätzliche
Standardunsicherheitsangaben; sie erzeugen kein weiteres Zufallsrauschen. Schon
enthaltene Beiträge dürfen nicht nochmals angegeben werden. `u` ist kein 95-%-Intervall.

Die RSS-Regel folgt der [NIST-Beschreibung kombinierter Standardunsicherheit](https://www.nist.gov/pml/nist-technical-note-1297/nist-tn-1297-5-combined-standard-uncertainty).
Für eine Gleichverteilung ist die Standardabweichung die halbe Breite geteilt durch
Wurzel 3; daraus folgt hier `resolution / sqrt(12)` unter der genannten Annahme.
Siehe [NIST: Unsicherheitsfortpflanzung](https://www.nist.gov/pml/nist-technical-note-1297/nist-tn-1297-appendix-law-propagation-uncertainty).
Die API simuliert einen Rohsensor, keine vollständige metrologische Kalibrierung.

## Verteilungen und Wiederholung

`ps_distribution` verwendet zwei Parameter:

| Art | a | b |
| --- | --- | --- |
| `PS_DIST_CONSTANT` | Wert | 0 |
| `PS_DIST_UNIFORM` | Minimum | Maximum |
| `PS_DIST_NORMAL` | Mittelwert | Standardabweichung ≥ 0 |

`ps_distribution_sample` nutzt einen explizit initialisierten `ps_rng`.
Konstante beziehungsweise entartete Verteilungen verbrauchen keine Zufallsziehung.
`ps_distribution_moments` liefert analytischen Mittelwert und Standardabweichung.
Ungültige Parameter und nicht endliche Ergebnisse ändern RNG und Ausgabe nicht.

Ein Sensor leitet Rauschen und Bernoulli-Ausfälle getrennt aus Seed und Rasterindex ab.
Zusätzliche nicht fällige Abfragen verschieben deshalb keine Messfolge. Das Überspringen
von Indizes ändert spätere Ziehungen nicht; eine andere Ausfallwahrscheinlichkeit ändert
die Rauschwerte der gemeinsamen gültigen Indizes nicht. Für verschiedene Sensoren
sind verschiedene Seeds nötig. Bitgleiche Wiederholung gilt innerhalb derselben
Binärdatei und Laufumgebung; mathematische Bibliotheken verschiedener Plattformen
können abweichende Rundungen liefern.

## Wurfvorlage und Bericht

„Wurf mit Unsicherheit“ verwendet zwei Sensoren mit 100 Hz und den obigen Parametern.
Die Anfangsgeschwindigkeiten bleiben unabhängige Modellunsicherheiten. Modell,
Sollbahn, Sensor x/y, ihre Statuswerte, tatsächliche Abtastzeit, Standardunsicherheiten
und übersprungene Rasterplätze stehen in getrennten Kanälen. Die Szene hält den
letzten gültigen x/y-Punkt und beschriftet dessen tatsächlichen Zeitpunkt.
Die Sprachvorlage `examples/language/uncertain_projectile.phys` verwendet
dieselben Modellverteilungen, Sensorparameter, Seed-Ströme und 15 Messkanäle.
Ihr Szenenlabel enthält derzeit keinen formatierten Messzeitpunkt. Zwei Seeds
und die Auswertung beider Laufdateitypen mit C- und Sprachmodul werden geprüft.

Der Einzelbericht zeigt Sollbahn, Modell und gültige Sensor-x-Punkte, zählt Ausfälle
und nicht fällige Zeilen und berechnet Fehler gegenüber dem Modell ausschließlich
aus gültigen Messungen. Der vollständige gefilterte Export heißt `*-sensor.csv`;
er enthält Zeit, Modell, Sollbahn, Sensor, Fehler und Standardunsicherheit. Nur die
Diagrammvorschau wird gegebenenfalls auf 2048 gültige Punkte reduziert. Bei vollständigem
Ausfall bleiben Modellkurven und Statuszählung sichtbar; eine Sensorfehlerstatistik entfällt.

Bei [Monte-Carlo-Endwerten](monte-carlo.md#fehlende-endwerte) werden nur gültige
Sensorendwerte aggregiert. Nicht fällige und ausgefallene Endmessungen bleiben
im Abschlussjournal und Endpunkt-CSV mit Status und leerem Wert erhalten.
Der Bericht zeigt ihre Anzahl und ihren Anteil; eine Serie ohne gültigen Endwert
enthält nur die Messabdeckung. Fehlende Messungen werden nicht auf null gesetzt
oder durch frühere Werte ersetzt. Selektive Ausfälle können die Statistik der
gültigen Teilmenge verzerren und werden nicht automatisch korrigiert.


## Explizite Zufallszustände und Ziehungsreihenfolge

`ps_rng` besitzt zwei öffentliche 64-Bit-Zustandswerte, `state` und `increment`.
`ps_rng_seed` initialisiert den Strom; eine Wertkopie beider Felder ist ein
vollständiger Snapshot. Verschiedene Instanzen besitzen keine gemeinsame
veränderliche Zufallsquelle. Wer dieselbe Instanz in mehreren Threads verwendet,
muss sie extern synchronisieren. Eine Speicherung muss beide Felder erhalten;
für Dateiformate sind Byteordnung und Version ausdrücklich zu definieren,
statt rohe C-Strukturen zu schreiben.

Der ganzzahlige PCG32-Strom ist vollständig bestimmt. `ps_rng_uniform` verbraucht
eine 32-Bit-Ziehung und bildet sie als `(word + 0.5) / 2^32` auf das offene
Intervall `(0,1)` ab. Der direkte C-Helfer `ps_rng_normal` verwendet Box–Muller:
zuerst eine uniforme Ziehung für den Radius, danach eine für den Winkel.
Diese Reihenfolge ist in getrennten C-Anweisungen festgelegt. Es gibt keinen
versteckten Ersatzwert-Cache; der direkte Helfer verbraucht auch bei
Standardabweichung 0 zwei Ziehungen.

Die geprüfte API `ps_distribution_sample` und `Rng.sample` in Physim behandeln
konstante und degenerierte Verteilungen dagegen ausdrücklich ohne Ziehung.
Ungültige Verteilungen und nichtendliche Resultate erhalten Zustand und Ausgabe.
`Rng(seed)` übernimmt das `Int64`-Bitmuster vollständig: `-1` entspricht dem
C-Seed `UINT64_MAX`. Kopien können unabhängig weiterlaufen oder auf denselben
Snapshot zurückgesetzt werden. Die bestehende Bindung an Laufseeds bleibt erhalten.

Ganzzahlige Zustände und uniforme Abbildungen sind exakt prüfbar. Für `log`,
`sqrt` und `cos` hängt die letzte Rundung von der Mathematikbibliothek ab;
die geordnete Ziehung verspricht keine bitgleichen Normalwerte über beliebige
Plattformen oder Toolchains. Gleicher Build, Seed und gleiche Aufrufreihenfolge
reproduzieren die Folge. Experimentmetadaten und archivierter Quell-/Buildstand
gehören deshalb zur Reproduzierbarkeit.

`tests/test_rng_reference.py` berechnet die Integer-Übergänge unabhängig und
prüft C und Physim für die Seeds 0, 42, `2^63` und `2^64-1`, normale und gemischte
Verteilungen sowie deren Ziehungsverbrauch. Die C-/Physim-Probes prüfen zusätzlich
Snapshots, Wertkopien und unbeeinflusste interleavte Ströme. Integer-Zustände
werden exakt verglichen; Normalwerte verwenden eine absolute/relative Toleranz
von `2e-14`. Die bisherige statistische Momentenprüfung bleibt daneben erhalten.
