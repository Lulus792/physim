# Einheiten, SI-Werte und Konvertierung

Eine `ps_unit` beschreibt sieben ganzzahlige Exponenten, eine positive endliche
Skala und ein Symbol. Die Reihenfolge der Exponenten ist immer
**Länge, Masse, Zeit, Strom, Temperatur, Stoffmenge, Lichtstärke**.
`value * unit.scale` ist der Wert in der entsprechenden SI-Dimension.
Symbole sind Beschriftungen; die Bibliothek leitet daraus keine Dimension oder
Skala ab. Zwei Einheiten sind kompatibel, wenn ihre sieben Exponenten gleich sind.

| SI-Basis | C-Konstante | Symbol |
| --- | --- | --- |
| Länge | `PS_METRE` | m |
| Masse | `PS_KILOGRAM` | kg |
| Zeit | `PS_SECOND` | s |
| Strom | `PS_AMPERE` | A |
| Temperatur | `PS_KELVIN` | K |
| Stoffmenge | `PS_MOLE` | mol |
| Lichtstärke | `PS_CANDELA` | cd |

Alle Basiskonstanten haben Skala 1. Benannte abgeleitete Einheiten sind unter
anderem `PS_NEWTON`, `PS_PASCAL`, `PS_JOULE`, `PS_WATT`, `PS_HERTZ`,
`PS_VELOCITY` und `PS_ACCELERATION`. `PS_ONE` und `PS_RADIAN` sind dimensionslos.
Weitere Einheiten entstehen mit `ps_unit_multiply`, `ps_unit_divide` und
`ps_unit_power`; die Exponenten müssen in `int8_t` passen. Ein Symbol allein
macht aus einer Länge keine Zeit. Eine positive Skala mit falscher Dimension
wird bei Konvertierung ebenso zurückgewiesen wie eine nichtpositive Skala.

## Wertdarstellung und gespeicherte Daten

`ps_quantity` und Physim `Quantity` sind Wert-plus-Einheit-Darstellungen. So kann
`Quantity(125,centimetre)` einen Eingabewert in Zentimetern ausdrücken. Für eine
SI-Grenze wird daraus ausdrücklich `Quantity(1.25,metre)` beziehungsweise der
SI-Zahlenwert 1,25. Temporäre Darstellungen dürfen eine andere Skala besitzen.

Experimentkanäle verlangen bei `ps_channel_add` und Physim `Channel` Skala 1.
`context.values` und `channel.sample` müssen bereits SI enthalten. Die Laufdatei
speichert Kanalname, Dimension, Symbol und Beschreibung; die Skala ist im
kanonischen Kanalformat fest 1. Einheiten-Symbole werden nicht als versteckte
Umrechnungsregeln benutzt. [Kanäle und Metadatengrenzen](workspace.md).

`ps_channel_sample_quantity(context,index,quantity)` und Physim
`channel.sampleQuantity(quantity)` übernehmen diese SI-Grenze atomar: Sie prüfen
Dimensionen, konvertieren die Eingabeskala und ändern erst danach den einen
Kanalwert. `PS_INVALID`, `PS_NUMERIC` und ein zu kurzer Context-Prefix
(`PS_VERSION`) erhalten den gesamten C-Kontext. Die Sprachseite prüft zusätzlich
Besitzer, Index und Samplingphase. Der rohe `channel.sample(number)`-Weg erwartet
weiterhin bereits konvertierte SI-Zahlen; ohne Einheit kann er keine Dimension
prüfen.

Analyse-Reihen aus `ps_series_from_values` konvertieren nichtkanonische Eingaben
blockweise in eigenständige SI-Daten mit Skala 1. Reihenoperationen verbinden
Dimensionen und prüfen die zugehörigen Handles. Dimensionen und Anzeigeeinheit
gehören zu den Metadaten; der Anwender wählt Anzeige-/Exportdarstellungen
explizit. [Reihen und ihre Einheiten](series.md).

Die C-Bibliothek kann einem nackten `double` keine physikalische Bedeutung
ansehen. Physik-APIs benutzen deshalb benannte SI-Felder wie `position_m` oder
`mass_kg`, oder ausdrücklich mitgegebene Dimensionsmetadaten. Allgemeine
Mathematik arbeitet in der vom Aufrufer gewählten konsistenten Einheit.

## Konvertierung

```c
#include <physim/units.h>

ps_unit centimetre = PS_METRE;
centimetre.scale = .01;
centimetre.symbol = "cm";
double metres;
ps_result status = ps_convert(125, centimetre, PS_METRE, &metres);
```

`ps_convert` und `ps_quantity_convert` berechnen das exakte binäre Verhältnis
`value * from.scale / to.scale` und runden das Endergebnis einmal zur nächsten
Double-Zahl, bei Gleichstand mit gerader Mantisse. Der normale Floating-
Environment-Modus wird vorausgesetzt. Identische Skalen erhalten jedes Bit des
Eingabewerts; dies gilt auch für signierte Null. SI-Normalisierung und Umrechnung
von SI benutzen, wo möglich, eine einzelne Multiplikation beziehungsweise Division.
Andere Verhältnisse verwenden begrenzte exakte Produkt-/Quotientenhilfen, um
Zwischenüberläufe und Doppelrundung zu vermeiden. Es gibt keine Heapallokation.

Nichtendliche Werte, nichtpositive/nichtendliche Skalen, ungleiche Dimensionen
und fehlende Ausgaben ergeben `PS_INVALID`. Ein nichtendliches Endergebnis oder
ein von null verschiedener Wert, der vollständig auf null rundet, ergibt
`PS_NUMERIC`. Jeder Fehler erhält die C-Ausgabe; Wert-/Ausgabe-Aliasing ist erlaubt.
`ps_quantity_convert` übernimmt bei Erfolg die Zieleinheit. Die Symbolzeiger
gehören weiterhin dem Aufrufer und müssen so lange wie ihre Einheiten leben.

Physim verwendet dieselbe Core-Berechnung:

```physim
let centimetre = Unit(1,0,0,0,0,0,0,0.01,"cm")
let metre = Unit(1,0,0,0,0,0,0,1,"m")
let distance = centimetre.convert(125,metre)
let quantity = Quantity(125,centimetre).converted(metre)
```

Ungültige Dimensionen und numerische Bereichsfehler bleiben verschiedene
Fehlercodes in strukturierten Runtime-Diagnosen. `attempt` kann beide abfangen.
Dasselbe gilt für Unit-Algebra und Quantity-Operationen; die bestehenden
lesbaren Fehlermeldungen bleiben erhalten. Die eigenständige Konvertierung hat
einen stärkeren Rundungsvertrag als [Quantity-Addition und andere Numerik](numerics.md).

## Anzeige und Export

`ps_unit_format_dimension` schreibt eine kanonische Dimensionsangabe wie
`m kg s^-2` für Kraft, unabhängig von Symbol und Skala. Ein zu kleiner Puffer
liefert `PS_LIMIT` und bleibt unverändert. Die App hält Anzeigeeinheiten
getrennt von den gespeicherten SI-Werten. CSV-/Berichtsexport dokumentiert die
jeweiligen Einheiten. [Anzeigeeinheiten](workspace.md), [Berichte](reports.md).

Temperatur-Offsets wie Celsius zu Kelvin sind keine reine Skalierung und werden
von dieser linearen Unit-API nicht unterstützt. Einheitenalgebra und gültige
Skalen ersetzen keine Kalibrierung oder fachliche Modellannahme.

Die unabhängige rationale Gegenprobe prüft Core-Konvertierung, Quantity-
Konvertierung, Identitäten, signierte Null, extreme Exponenten, Bereichsfehler,
Dimensionsfehler, Aliasierung und Fehlererhaltung in C und Physim. Ein separater
Host-Trap-Test prüft ursprüngliche Fehlercodes samt strukturierten Diagnosen
und Quellposition. Diese Nachweise werden nicht auf ungeprüfte Datenpfade oder
vollständige Plattform-/Modellabnahme übertragen.
