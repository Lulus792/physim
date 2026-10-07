# C-Referenz: Einheiten und Größen

ps_unit beschreibt Dimension, positive Skala und Symbol. ps_quantity kombiniert Zahlenwert und Einheit. Addition/Subtraktion konvertieren den rechten Operanden in die Einheit des linken. Produkte und Quotienten kombinieren Dimensionen. Symbole müssen solange wie die Einheit gültig bleiben. Temperatur-Offsets werden nicht unterstützt.

[Anleitung und Beispiele](../numerics.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/units.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.

## Typen und Funktionen

### PS_ACCELERATION

```c
extern const ps_unit PS_ONE, PS_AMPERE, PS_KELVIN, PS_MOLE, PS_CANDELA, PS_NEWTON, PS_PASCAL,
    PS_WATT, PS_HERTZ, PS_ACCELERATION;
```

## ps_unit_valid

Prüft, ob die Einheit den Vertrag für Dimensionen, Skala und Symbol erfüllt.

```c
bool ps_unit_valid(ps_unit unit);
```

## ps_unit_compatible

Prüft gleiche SI-Dimensionen; unterschiedliche Skalen können kompatibel sein.

```c
bool ps_unit_compatible(ps_unit a, ps_unit b);
```

## ps_unit_multiply

Addiert Dimensionsexponenten und multipliziert Skalen; symbol wird geliehen.

```c
ps_result ps_unit_multiply(ps_unit a, ps_unit b, const char *symbol, ps_unit *out);
```

Algebra returns a borrowed symbol pointer (caller supplies its lifetime). Exponent overflow and nonpositive/unrepresentable scales are rejected. All checked operations leave output unchanged on failure. No affine units.

## ps_unit_divide

Subtrahiert Dimensionsexponenten und dividiert Skalen; symbol wird geliehen.

```c
ps_result ps_unit_divide(ps_unit a, ps_unit b, const char *symbol, ps_unit *out);
```

## ps_unit_power

Potenz einer Einheit mit ganzzahligem Exponenten und geprüftem Überlauf.

```c
ps_result ps_unit_power(ps_unit a, int power, const char *symbol, ps_unit *out);
```

### ps_quantity

```c
typedef struct {
    double value;
    ps_unit unit;
} ps_quantity;
```

## ps_quantity_convert

Konvertiert Zahlenwert und Einheit in eine dimensionskompatible Zieleinheit.

```c
ps_result ps_quantity_convert(ps_quantity value, ps_unit target, ps_quantity *out);
```

## ps_quantity_add

Addiert dimensionskompatible Größen in der Einheit von a mit normierter, kompensierter Umrechnung und abschließender Subnormalrundung; Fehler erhalten die Ausgabe.

```c
ps_result ps_quantity_add(ps_quantity a, ps_quantity b, ps_quantity *out);
```

Addition/subtraction return a's unit and combine normalized operands before final scaling, including when b alone cannot be represented in a's unit. Assumes default round-to-nearest/ties-to-even; compensated terms retain conversion/sum residuals and subnormals round once on their final lattice. Exact normalized cancellation may return zero; a nonzero normalized result rounding to zero or a nonfinite final result yields PS_NUMERIC. Double rounding still applies, especially near cancellation; no exact-arithmetic guarantee. Inputs/output may alias. Product/quotient compose dimensions.

## ps_quantity_subtract

Subtrahiert dimensionskompatible Größen in der Einheit von a mit normierter, kompensierter Umrechnung; Rückskalierung erfolgt erst nach der Summe.

```c
ps_result ps_quantity_subtract(ps_quantity a, ps_quantity b, ps_quantity *out);
```

## ps_quantity_multiply

Multipliziert Werte und Einheiten; symbol benennt die Produkteinheit.

```c
ps_result ps_quantity_multiply(
    ps_quantity a,
    ps_quantity b,
    const char *symbol,
    ps_quantity *out);
```

## ps_quantity_divide

Dividiert Werte und Einheiten mit Prüfung auf ungültigen Divisor.

```c
ps_result ps_quantity_divide(
    ps_quantity a,
    ps_quantity b,
    const char *symbol,
    ps_quantity *out);
```

## ps_unit_format_dimension

Schreibt die kanonische SI-Dimensionsdarstellung in einen begrenzten Textpuffer.

```c
ps_result ps_unit_format_dimension(ps_unit unit, char *text, size_t capacity);
```

Canonical SI dimension spelling; ignores unit scale and custom symbol.
