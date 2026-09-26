# Datenreihen und eigene Analysen

Die API in `physim/series.h` öffnet Läufe als Datensätze und stellt Kanäle als
versionierte Series-Handles bereit. Eine Analyse kann bis zu acht Läufe gleichzeitig
öffnen. Der Context verwaltet bis zu 128 lebende Reihen. Die Werte werden blockweise
verarbeitet; es entsteht keine vollständige Kopie im Arbeitsspeicher.

## Position ableiten und exportieren

```c
#include "physim/series.h"
#include <stdio.h>

static ps_result velocity_report(const char *input, const char *prefix) {
    ps_analysis_context *ctx = NULL;
    ps_result result = ps_analysis_create(prefix, 0, &ctx);
    if (result != PS_OK) return result;
    ps_dataset run = {0};
    ps_series time = {0}, position = {0}, velocity = {0};
    result = ps_analysis_open_run(ctx, input, &run);
    bool recovered = result == PS_RECOVERED;
    if (result == PS_OK || recovered) {
        result = ps_dataset_series(ctx, run, "time", &time);
        if (result == PS_OK)
            result = ps_dataset_series(ctx, run, "position.x", &position);
        if (result == PS_OK)
            result = ps_series_derivative(ctx, position, time, &velocity);
        if (result == PS_OK) {
            char path[4096];
            int n = snprintf(path, sizeof path, "%s-velocity.csv", prefix);
            if (n < 0 || (size_t)n >= sizeof path) result = PS_LIMIT;
            else {
                ps_series columns[] = {time, position, velocity};
                result = ps_series_export_csv(ctx, columns, 3, path);
            }
        }
    }
    ps_analysis_destroy(ctx);
    return result == PS_OK && recovered ? PS_RECOVERED : result;
}
```

Die Routine kann aus dem `run`-Callback eines Analysemoduls aufgerufen werden.
Sie läuft im Analyseprozess. Die CSV enthält alle Werte, keine ausgedünnte Vorschau.
Der Ausgabeordner muss existieren; vorhandene Dateien werden nicht überschrieben.
Der Context wird auch nach einem Fehler zerstört.

## Datenzugriff und Lebensdauer

`ps_analysis_open_run` validiert die Quelldatei und erstellt einen unveränderlichen
temporären Datensnapshot. Zeitwerte müssen streng steigen. Metadaten und
Kanaldefinitionen liefert `ps_dataset_describe` als Kopie. Ein fehlender oder
doppeldeutiger Kanalname ist ein Fehler. `time` ist für die Zeitachse reserviert.

`ps_series_read` kopiert einen beliebigen Ausschnitt ab einem Sampleindex in einen
Puffer des Aufrufers. Eine Anfrage am Ende liefert erfolgreich null Werte; hinter
dem Ende ist der Index ungültig. `ps_series_describe` liefert Name, Anzahl,
SI-Exponenten, Skalierung und Anzeigesymbol als Kopien.

Handles gehören zu genau einem lebenden Context. `ps_series_release` gibt eine
Reihe frei; daraus bereits erzeugte Reihen bleiben erhalten. `ps_dataset_close`
invalidiert dagegen alle Quell- und Ergebnisreihen dieses Datensatzes.
Wiederverwendete Slots akzeptieren keine alten Handles. Nach Zerstörung eines
Contexts dürfen dessen Handles nicht mehr benutzt werden.

## Operationen und Einheiten

- `ps_series_slice`: zusammenhängender Samplebereich, zum Beispiel ein Zeitfenster.
- `ps_series_select`: gemeinsame Zeilenauswahl mehrerer Reihen anhand eines Statuskanals.
- `ps_series_affine`: dimensionsloser Faktor plus Offset mit passender Einheit.
- `ps_series_combine`: Addition, Subtraktion, Produkt oder Quotient zweier Reihen.
- `ps_series_derivative`: zentrale Sekanten; an den Rändern einseitige Sekanten.
- `ps_series_integral`: kumulative Trapezintegration mit Anfangswert und Einheit.
- `ps_series_moving_average`: kausaler Mittelwert über höchstens 4096 Samples.
- `ps_series_resample_linear`: explizite lineare Interpolation auf die x-Reihe eines
  anderen Datensatzes, ohne Extrapolation.
- `ps_series_statistics`: Anzahl, Mittelwert, Stichprobenstreuung, Minimum, Maximum.

Ableitung und Integral prüfen eine streng steigende x-Reihe. Ihre Ergebnisdimension
ist y/x beziehungsweise y*x. Beim gleitenden Mittelwert werden am Anfang nur die
bereits vorhandenen Samples verwendet. Nichtendliche Ergebnisse werden gemeldet.

Gepaarte Reihen müssen aus demselben Datensatz und demselben Samplebereich stammen.
Gleich lange Reihen verschiedener Läufe werden nicht stillschweigend ausgerichtet.
Für den Laufvergleich können mehrere Datensätze getrennt verarbeitet werden.
Die App bietet [Mehrfachauswahl, überlagerte Vergleichsplots und Differenzkurven](runs.md).

## Gültige Messungen auswählen

`ps_series_select(ctx, columns, count, selector, accepted, out)` übernimmt nur
Zeilen, deren dimensionsloser Selektorkanal exakt dem angegebenen Zahlenwert
entspricht. Der Wert ist in der gespeicherten Einheit des Selektors angegeben;
für Sensorstatus bedeutet `1` gültig, `0` nicht fällig und `2` ausgefallen.
Statuskanäle werden nicht automatisch erkannt oder auf andere Reihen angewandt.

Zeitachse und Messwerte werden gemeinsam ausgewählt, zum Beispiel nach dem
Öffnen der drei Quellreihen `sensor_time`, `sensor_x` und `sensor_status`:

```c
ps_series columns[] = {sensor_time, sensor_x};
ps_series selected[2];
ps_result r = ps_series_select(ctx, columns, 2, sensor_status, 1, selected);
if (r == PS_OK) {
    r = ps_series_export_csv(ctx, selected, 2, "valid-measurements.csv");
    ps_series_release(ctx, selected[0]);
    ps_series_release(ctx, selected[1]);
}
```

Ein Aufruf akzeptiert 1–32 miteinander und mit dem Selektor ausgerichtete Reihen.
Die Ausgabe behält Reihenfolge, Namen und Einheiten und besitzt eigene temporäre
Dateien. Alle Ergebnisse eines Aufrufs teilen eine neue Samplezuordnung.
Separate Aufrufe sind auch bei identischen Zeilen nicht automatisch ausgerichtet;
deshalb zusammengehörige Kanäle immer gemeinsam filtern. Nachfolgende Ausschnitte
und Transformationen erhalten diese Zuordnung, Resampling übernimmt die des Ziels.
Das Schließen des Ursprungsdatensatzes invalidiert auch seine Auswahlreihen.

Keine passende Zeile ergibt erfolgreich leere Reihen. Mindestanzahlen der
nachfolgenden Operationen gelten weiterhin. Ableitung und Integral verbinden
die behaltenen Punkte über Messlücken hinweg; die Funktion interpoliert keine
fehlenden Messungen. Für eine Darstellung ohne verbindende Linien eignen sich
Punktdiagramme. Die Standardanalyse verwendet diese API für ihre Sensorkurve.

Die Verarbeitung benötigt nur begrenzte Blockpuffer. Alle gewählten Werte zählen
gegen das Scratch-Limit; bei Fehlern bleiben Ausgabehandles und belegte
Scratch-Bytes unverändert. Ein- und Ausgabearray dürfen identisch sein.

## Unterschiedliche Zeitraster vergleichen

`ps_series_resample_linear(ctx, y, x, target_x, &out)` interpoliert stückweise linear
zwischen benachbarten Quellpunkten. `x` und `y` müssen dieselbe Samplezuordnung haben.
`target_x` darf zu einem anderen Datensatz desselben Contexts gehören. Beide Achsen
müssen nichtleer und streng steigend sein; alle Quellkoordinaten werden geprüft,
auch außerhalb des angefragten Teilbereichs. Kompatible Achseneinheiten werden vor
dem Vergleich umgerechnet. Auch danach müssen Zielkoordinaten streng steigen.

Alle Zielwerte müssen im geschlossenen Bereich der Quelle liegen. Außerhalb liefert
die Funktion `PS_INVALID`; sie extrapoliert und verschiebt Zeitachsen nicht. Eine
Quelle mit einem einzigen Sample erlaubt nur genau diese Zielkoordinate. Die
Ergebnisreihe besitzt Einheit und Skalierung von `y`, aber Samplezuordnung und
Lebensdauer des Zieldatensatzes. Der Quelldatensatz darf danach geschlossen werden.
Beim Schließen des Zieldatensatzes wird auch das Interpolationsergebnis ungültig.

`ps_series_resample(ctx, y, x, target_x, method, &out)` bietet drei Verfahren mit
demselben Vertrag für Einheiten, Zeitraster und Lebensdauer:

| Verfahren | Ergebnis zwischen zwei Quellpunkten |
| --- | --- |
| `PS_RESAMPLE_LINEAR` | Lineare Verbindung der Werte; entspricht `ps_series_resample_linear`. |
| `PS_RESAMPLE_NEAREST` | Wert des näheren Quellpunkts; bei gleichem Abstand der frühere. |
| `PS_RESAMPLE_PREVIOUS` | Letzter Wert bei oder vor der Zielkoordinate, etwa für gehaltene Stellgrößen. |

An exakten Quellkoordinaten liefern alle Verfahren den dortigen Wert. Auch das
Halten des letzten Werts endet am letzten Quellpunkt; eine Fortschreibung über
den aufgezeichneten Bereich hinaus ist nicht erlaubt. Die Verfahrenswahl muss
zum Signal passen: Nearest oder Previous erzeugen keine Zwischenwerte für
diskrete Zustände, können aber keine fehlende Messinformation rekonstruieren.

```c
ps_series reference_on_target = {0}, difference = {0};
ps_result r = ps_series_resample_linear(ctx, reference_position, reference_time,
                                       target_time, &reference_on_target);
if (r == PS_OK)
    r = ps_series_combine(ctx, PS_SERIES_SUBTRACT, target_position,
                          reference_on_target, &difference);
```

Hier sind `target_time` und `target_position` bereits auf den gemeinsamen Bereich
zugeschnitten, etwa mit `ps_series_slice`. Die mitgelieferte Mehrlaufanalyse zeigt
die vollständige Auswahl des Bereichs, Fehlerbehandlung und Freigabe. Das Verfahren
benötigt O(n + m) Arbeit für n Quell- und m Zielpunkte, festen Blockspeicher und
8m Bytes temporären Plattenspeicher für das Ergebnis.

Interpolation ist ein Modell zwischen Messpunkten. Sie rekonstruiert keine
unbeobachteten Sprünge oder hochfrequenten Signale und ist kein Antialiasfilter für
Downsampling. Bei zweimal differenzierbaren Funktionen mit `|f''| <= M` beträgt der
lokale Interpolationsfehler höchstens `M*h*h/8` für den jeweiligen Quellabstand h,
zusätzlich zum Mess- und Rundungsfehler. Der Referenztest mit `f(x)=x*x` prüft diese
Grenze an Intervallmitten. Eine lineare Funktion wird bis auf Rundung exakt getroffen.

Das bisherige `.psrun`-Format enthält SI-Exponenten und Symbole, aber keine
Skalierungsfaktoren. Die Series-API interpretiert die gespeicherten Zahlen daher
entsprechend dem SDK-Vertrag als SI-Werte mit Skalierung 1.

## Speicher und Fehler

Rechenoperationen nutzen feste Blöcke mit 256 Samples. Datensnapshots und abgeleitete
Reihen belegen temporären Plattenspeicher. Das Standardlimit pro Context ist 1 GiB;
`ps_analysis_create` nimmt alternativ ein explizites Bytebudget entgegen.
`ps_analysis_scratch_bytes` meldet den belegten Nutzdatenumfang. Dateisystem- und
Bibliothekspuffer kommen hinzu. Das Limit ist keine Betriebssystem-Sandbox.

Scratchdateien werden exklusiv neben dem angegebenen Arbeitspräfix angelegt. Unter
Windows löscht das Betriebssystem sie beim Schließen der letzten Dateireferenz;
unter POSIX werden sie unmittelbar nach dem Öffnen entlinkt. Auch ein Prozessende
lässt keine benannten Scratchdateien zurück. Der Dateiname wird nie als vorhandene
Nutzdatei wiederverwendet.

Scheitert eine Transformation, bleiben Eingabereihen und Ausgabehandle unverändert;
ihr temporäres Zwischenergebnis wird verworfen. `PS_LIMIT` bezeichnet erschöpfte
Slots oder das Plattenbudget, `PS_NUMERIC` nichtendliche Rechenergebnisse, `PS_IO`
Dateifehler. `PS_RECOVERED` beim Öffnen ist ein gültiger, unvollständiger Datensatz.
Ein Context ist nicht gleichzeitig aus mehreren Threads verwendbar.

## Weiterführend

[API-Überblick](api.md)

[Numerische Verfahren und Einheiten](numerics.md)

[Dateiformat und Recovery](data-format.md)
