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

`ps_convert` vermeidet vermeidbare Zwischenüberläufe durch Mantissen-/Exponenten-
Zerlegung. Überlauf oder vollständiger Unterlauf des Ergebnisses wird gemeldet.
Eigene Einheitensymbole werden nicht kopiert; ihre Lebensdauer liegt beim Aufrufer.
`ps_unit_format_dimension` erzeugt eine kanonische SI-Dimensionsangabe. Affine
Temperaturskalen und die automatische Dimensionsprüfung kompletter Analyseskripte
sind weiterhin offen.

## Referenzprüfungen

Tests vergleichen RK45 mit Exponentialfunktion, harmonischem Oszillator und einer
zeitabhängigen analytischen Lösung, einschließlich Rückwärtsintegration,
verschieden skalierter Komponenten und Fehlerbudgets. Verlet wird auf Ordnung 2,
beschränkten Energiefehler und Zeitumkehr geprüft. Separate Pendelläufe vergleichen
RK4, RK45 und Verlet mit der elliptischen Referenzperiode. Lineare Systeme prüfen
Pivotisierung, Singularität, Skalierung und Aliasfälle; Einheitenprüfungen behandeln
Dimensionsfehler und Ergebnisintegrität.
