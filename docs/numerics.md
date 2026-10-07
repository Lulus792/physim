# Numerische Verfahren und Einheiten

`physim/numerics.h` und `physim/units.h` ergänzen die öffentliche C-API.
Die Verfahren arbeiten ohne Heap-Allokation mit höchstens 32 Komponenten.
`PS_SINGULAR`, `PS_LIMIT` und `PS_NUMERIC` ergänzen die bisherigen Fehlercodes.
Zustands- und Ergebnisarrays bleiben bei Fehlern unverändert. Callback-seitige
Nebenwirkungen können dagegen nicht zurückgenommen werden.

## Integration

- `ps_ode_step`: Euler und klassisches RK4 mit festem positivem Zeitschritt.
- `ps_verlet_step`: Velocity Verlet, Ordnung 2, für `q'' = a(t,q)`. Position und
  Geschwindigkeit dürfen sich nicht überlappen. Geschwindigkeitsabhängige Kräfte
  wie Luftwiderstand sind für diese Schnittstelle nicht zulässig.
- `ps_ode_integrate`: explizites Dormand–Prince 5(4), vorwärts oder rückwärts bis
  zum gewünschten Endzeitpunkt. Geeignet für nichtsteife ODEs. Keine Ereignissuche
  und keine dichte Ausgabe zwischen Stützstellen.

Für jeden RK45-Versuch werden sieben Ableitungen berechnet. Akzeptiert wird ein
Schritt, wenn die größte komponentenweise geschätzte Abweichung, geteilt durch
`atol[i] + rtol * max(abs(y_alt[i]), abs(y_neu[i]))`, höchstens 1 beträgt.
Optional ersetzen komponentenweise absolute Toleranzen den gemeinsamen Wert.
Die Kontrolle betrifft den lokalen Fehler; sie garantiert keinen globalen Fehler.
Callbacks müssen alle Ableitungen setzen, deterministisch sein und dürfen keine
Messungen, Zufallsziehungen oder andere sichtbare Nebenwirkungen auslösen.

`ps_ode_options_default()` liefert atol=1e-9, rtol=1e-7, Anfangsschritt 0,001,
Minimal-/Maximalschritt 1e-14/1 und höchstens 100000 Versuche. Schrittweiten sind
positive Beträge; die Richtung folgt aus Start und Ende. Ein letzter Schritt darf
kürzer als das Minimum sein. Bei erschöpftem Budget, nicht weiter reduzierbarem
Schritt oder fehlendem Zeitfortschritt folgt `PS_LIMIT`. Nichtendliche Stufen oder
fehlende Ableitungskomponenten ergeben `PS_NUMERIC`. Der optionale Bericht enthält
auch bei solchen Abbrüchen den intern erreichten Zeitpunkt; der übergebene Zustand
wird erst bei vollständig erfolgreicher Integration geändert.

### Abbruchursachen untersuchen

`ps_ode_integrate_diagnosed` bietet dieselbe Rechnung mit einem zusätzlichen,
optionalen `ps_ode_diagnostic` des Aufrufers. Die bisherige Funktion bleibt
unverändert verwendbar. Es gibt keinen globalen Fehlerzustand und keine
zusätzliche Speicherallokation.

```c
ps_ode_diagnostic diagnostic;
ps_ode_report report;
ps_result result = ps_ode_integrate_diagnosed(
    derivative, user, start, end, state, n, &options, &report, &diagnostic);
if (result != PS_OK) {
    fprintf(stderr, "%s at t=%.17g\n",
            ps_ode_diagnostic_string(diagnostic.reason), diagnostic.time);
}
```

Die Ursache unterscheidet ungültige Argumente, nichtendliche Anfangszustände,
ungültige Komponententoleranzen, verbrauchtes Schrittbudget, fehlenden
Gleitkomma-Zeitfortschritt, nichtendliche Zwischenzustände, fehlende/nichtendliche
Ableitungen, nichtendliche Fehlerschätzung und eine trotz minimaler Schrittweite
verfehlte Toleranz. `component` und `stage` sind nullbasiert; ohne entsprechenden
Bezug enthalten sie `SIZE_MAX` beziehungsweise `UINT_MAX`.

Bei Stufenfehlern bezeichnet `time` den Auswertungszeitpunkt, bei Fehlern der
Fehlerschätzung das versuchte Schrittende. Bei Limits und Erfolg ist es der
intern erreichte Zeitpunkt. Dieser bedeutet bei einem Fehler **nicht**, dass der
Zustand des Aufrufers bis dahin aktualisiert wurde. Bei ungültigen Argumenten
enthält die Diagnose den übergebenen Startwert, der auch nichtendlich sein kann;
der Fortschrittsbericht bleibt dann unverändert. Bei jedem Aufruf wird die
Diagnose neu geschrieben, bei Erfolg mit `PS_ODE_DIAG_NONE`. Diagnose, Bericht
und Zustand müssen getrennte Speicherbereiche sein. Die Textfunktion liefert
statische englische Beschreibungen; Anwendungen können anhand des Enums eigene
Übersetzungen anbieten.

Die Pendelvorlage unterstützt `PS_RK45` und `PS_VERLET` zusätzlich zu den bisherigen
Methoden. `PS_PENDULUM_METHOD` wählt das Verfahren; standardmäßig bleibt es RK4.
RK45 nimmt adaptive interne Schritte, während die Aufzeichnung weiter exakt den
vorgegebenen Ausgabeabstand `dt` nutzt. Verfahren und RK45-Toleranzen stehen in den
Modellmetadaten. Verlet wird bei eingeschaltetem Luftwiderstand abgewiesen.

Tableau und Methodeneinordnung: [SciPy RK45-Dokumentation](https://docs.scipy.org/doc/scipy/reference/generated/scipy.integrate.RK45.html),
mit Verweis auf Dormand & Prince (1980). Die C-Implementierung ist eigenständig;
es besteht keine Laufzeitabhängigkeit zu SciPy.

## Lineare Systeme, Nullstellen und Minimum

`ps_linear_solve` löst eine dichte, zeilenweise gespeicherte Matrix durch
zeilenskalierte partielle Pivotisierung. A und b bleiben erhalten; x darf b
überlagern. Toleranz 0 wählt `n * DBL_EPSILON`. Zu kleine Pivots ergeben
`PS_SINGULAR`; es wird keine Konditionszahl oder Fehlerschranke versprochen.

`ps_root_bisect` benötigt eine stetige Funktion mit Vorzeichenwechsel oder einer
exakten Nullstelle am Rand. `ps_minimize_golden` benötigt ein unimodales Intervall.
Beide verwenden eine absolute plus relative x-Toleranz und ein Iterationslimit.
Berichte auf `PS_LIMIT` sind Näherungen, keine Konvergenzbestätigung. Absolute
Toleranzen müssen positiv, relative Toleranzen endlich und im Bereich [0,1) sein.

## Einheitenalgebra

Einheiten enthalten sieben SI-Exponenten und eine positive endliche Skalierung.
Produkte, Quotienten und ganzzahlige Potenzen prüfen Exponentenüberläufe und nicht
darstellbare Skalierungen. `ps_quantity_add/subtract` konvertieren den zweiten
Operanden in die Einheit des ersten; unverträgliche Dimensionen sind Fehler.
Produkte und Quotienten kombinieren Zahlenwerte und Dimensionen.

Addition und Subtraktion erhalten die Einheit des linken Operanden. Beide
Werte und die Skalen sind endlich, Skalen positiv und Dimensionen gleich.
Die Umrechnung des rechten Operanden und die Summe werden in normierter
Binärform kombiniert; die Rückskalierung folgt erst am Ende. Beispielsweise
liefert `-2^1023 m + 2^1023 (Skala 2 m)` den endlichen Wert `2^1023 m`, obwohl
jede separat ausgegebene Umrechnung des zweiten Operanden überlaufen würde.
Auch ein Beitrag unterhalb der kleinsten Subnormalzahl kann noch die Summe
beeinflussen: `minDouble + minDouble (Skala 0,5)` rundet auf `2*minDouble`.

Die Rechnung hält mit FMA den Produktrundungsrest und einen Quotientenrest
sowie mit TwoSum einen Summenrest fest. TwoSum und FMA-Produktzerlegung folgen
[Ogita, Rump und Oishi (2005), Algorithmen 3.1/3.5](https://www.tuhh.de/ti3/paper/rump/OgRuOi05.pdf).
Subnormale Ergebnisse werden auf ihrem endgültigen Gitter gerundet, damit ein
kleiner Rest die Entscheidung an einer halben Gitterweite beeinflussen kann.
Die Implementierung verwendet ausschließlich Double und setzt die normale
Rundung zur nächsten Zahl mit gerader Mantisse bei Gleichstand voraus.
Sie ist keine beliebig genaue Arithmetik und verspricht keine allgemeine
exakte Rundung jeder nichtbinären Skalenteilung. Nahe Auslöschung können
relative Fehler weiterhin groß werden.

Ein nichtendliches Endergebnis oder ein nichtnuller kompensierter Rest, der
auf null unterläuft, liefert `PS_NUMERIC`; kompensierte Auslöschung kann null
liefern. Ungültige Eingaben liefern `PS_INVALID`. Fehler erhalten die gesamte
C-Ausgabe; Input/Output-Aliasing ist erlaubt. Physim-Operatoren und benannte
Quantity-Funktionen verwenden dieselbe C-Rechnung; Fehler sind mit `attempt`
abfangbar. Die eigenständige Konvertierung `ps_convert` behält ihren bisherigen
Bereichsvertrag. Auch Produkt-/Quotientenoperationen behalten ihren Vertrag.

`ps_convert` vermeidet vermeidbare Zwischenüberläufe durch Mantissen-/Exponenten-
Zerlegung. Überlauf oder vollständiger Unterlauf des Ergebnisses wird gemeldet.
Eigene Einheitensymbole werden nicht kopiert; ihre Lebensdauer liegt beim Aufrufer.
`ps_unit_format_dimension` erzeugt eine kanonische SI-Dimensionsangabe. Affine
Temperaturskalen und die automatische Dimensionsprüfung kompletter Analyseskripte
sind weiterhin offen.

### SI-Grenze für Experimentkanäle

`ps_channel_add` und Physim `Channel` akzeptieren nur `unit.scale == 1`.
Die Laufdatei speichert Dimensionen und Symbol, aber keine Kanalskala: Werte
in `context.values` oder `channel.sample` müssen bereits SI sein.
Beispiel: erst `ps_convert(125, cm, PS_METRE, &value)` beziehungsweise
`centimetre.convert(125,metre)` ausführen, dann 1,25 im Meterkanal speichern.
Die Bibliothek prüft die Metadaten; eine nackte Zahl kann sie nicht auf eine
physikalisch richtige Einheit prüfen. Anzeigeeinheiten sind ein gesonderter
Teil der App und ändern gespeicherte Werte nicht.

Die Deklaration kopiert UTF-8 ohne Kürzung: Name 1–47 Byte, Symbol 0–15 Byte,
Beschreibung 0–95 Byte. Name/Symbol enthalten keine Steuerzeichen; eine
Beschreibung darf Zeilenumbrüche und Tabs enthalten. Namen sind eindeutig,
höchstens 16 Kanäle. Fehler liefern in C -1 und erhalten den gesamten Kontext;
in Physim sind sie mit `attempt` abfangbar. Die Rückgabe ist der Index,
fehlgeschlagene Deklarationen verbrauchen keinen Slot. Eigene bereits angelegte
Rohschemata und historische Laufdateien behalten ihren Formatvertrag.

## Referenzprüfungen

Tests vergleichen RK45 mit Exponentialfunktion, harmonischem Oszillator und einer
zeitabhängigen analytischen Lösung, einschließlich Rückwärtsintegration,
verschieden skalierter Komponenten und Fehlerbudgets. Verlet wird auf Ordnung 2,
beschränkten Energiefehler und Zeitumkehr geprüft. Separate Pendelläufe vergleichen
RK4, RK45 und Verlet mit der elliptischen Referenzperiode. Lineare Systeme prüfen
Pivotisierung, Singularität, Skalierung und Aliasfälle; Einheitenprüfungen behandeln
Dimensionsfehler und Ergebnisintegrität.

## Integration und Differentiation von Messreihen

`ps_derivative` und `ps_series_derivative` berechnen zentrale Sekanten,
an Segmenträndern einseitige Sekanten. Das Verfahren ist bei gleichmäßigen
Abständen im Inneren von zweiter Ordnung, an Rändern von erster Ordnung.
Bei ungleichen Abständen ist die zentrale Sekante **keine** quadratische
Interpolation: für `x={0,1,3}`, `y=x²` liefert sie am mittleren Punkt 3 statt
der analytischen Ableitung 2. Ableitungen verstärken Messrauschen; weder
Glättung noch eine globale Fehlerschranke sind implizit enthalten.

`ps_trapezoid` und `ps_series_integral` integrieren den stückweise linearen
Verlauf zwischen benachbarten Messpunkten. Die Series-Variante beginnt mit
einem expliziten Anfangswert und verwendet kompensierte Summation. Einheiten
werden beim Series-Aufruf zu `y/x` beziehungsweise `y*x` kombiniert; die
alten Array-Helfer verwenden die vom Aufrufer festgelegten Einheiten.

Beide API-Wege verwenden dieselbe skalierte Intervallrechnung. Beispielsweise
ist die Sekante von `x=y={-1e308,1e308}` genau 1, obwohl beide Differenzen
allein den Double-Bereich übersteigen. Ein konstantes Signal `1e-308` über
dieser Achse hat ein darstellbares Integral von ungefähr 2. Auch kleinste
subnormale Signalwerte werden vor der Mittelwertbildung skaliert. Echte
Überläufe bleiben Fehler; eine Prüfung der darstellbaren Intervallflächen
ersetzt keine allgemeine Analyse der Kondition oder Rundungsfehler.

Messlücken werden nicht überbrückt. Die Ableitung verwendet nur direkt
benachbarte gültige Werte; ein isolierter gültiger Punkt bleibt unbekannt.
Beim kumulativen Integral sind die erste fehlende Messung und alle folgenden
Werte unbekannt. Explizite Auswahl gültiger Zeilen kann diese Semantik ändern,
weil sie ein neues, verdichtetes Raster erzeugt.

`tests/test_series_numeric_extremes.c` prüft extreme Differenzen, subnormale
Flächen, Vorzeichenauslöschung, Streaming-Blockgrenzen, Messmasken und echte
Überläufe. Fehler veröffentlichen keinen neuen Handle und erhöhen den
Scratch-Verbrauch nicht. `analysis_reference.phys` führt dieselben
extremen Rechnungen im echten Analyseprozess über die Sprachbindung aus.
