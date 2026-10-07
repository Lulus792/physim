# C-Referenz: Material- und Medieneigenschaften

Eigenschaften sind explizite SI-Daten mit Quellenangabe, Gültigkeitsbereich und konstantem oder tabellarischem Modell. Tabellen werden begrenzt linear/bilinear interpoliert; keine Extrapolation oder automatische Materialauswahl. Das konsumierende Modell prüft die benötigten Eigenschaften.

[Anleitung und Beispiele](../properties.md) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)

Einbinden: `#include "physim/properties.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.

## Konstanten

```c
#define PS_PROPERTY_MAX_AXIS 64u
```

## Typen und Funktionen

### ps_property_model

```c
typedef enum { PS_PROPERTY_CONSTANT, PS_PROPERTY_TABLE } ps_property_model;
```

Caller-owned property data in SI. Strings/arrays/unit symbol must outlive all evaluations. Keep them unchanged while evaluating; synchronize shared mutation. No allocation, implicit catalog or material selection. Domain is closed, Kelvin >= 0, Pascal >= 0. value_unit must have scale 1. Constant uses constant_value_si; table uses 1..64 strictly increasing finite axes and temperature-major values (pressure varies fastest). A one-point axis means independence of that coordinate throughout the domain; its reference coordinate must lie inside the domain. Multi-point axes must cover the declared domain. No extrapolation, phase transitions or uncertainty model is inferred. Negative property values are permitted; each consuming physical model validates the properties it actually needs.

### ps_property

```c
typedef struct {
    ps_property_model model;
    const char *name, *source;
    ps_unit value_unit;
    double minimum_temperature_k, maximum_temperature_k;
    double minimum_pressure_pa, maximum_pressure_pa;
    double constant_value_si;
    const double *temperature_k, *pressure_pa, *values_si;
    size_t temperature_count, pressure_count;
} ps_property;
```

optional, bounded UTF-8; NULL means absent

## ps_property_validate

Prüft SI-Einheit, UTF-8-Metadaten, geschlossenen T/P-Bereich, konstante Werte oder vollständige endliche Tabellen mit höchstens 64 Punkten je Achse.

```c
ps_result ps_property_validate(const ps_property *property);
```

Invalid descriptor -> PS_INVALID, excessive axis size -> PS_LIMIT.

## ps_property_evaluate

Wertet konstante oder begrenzt bilinear interpolierte Materialdaten bei Kelvin/Pa aus und erhält die Ausgabe bei Fehlern. Quellen, Einheit und Gültigkeit bleiben explizite Modelldaten.

```c
ps_result ps_property_evaluate(
    const ps_property *property,double temperature_k,
    double pressure_pa,ps_quantity *out);
```

Bilinear interpolation (linear/constant on one-point axes), no extrapolation. Nonfinite/out-of-domain query -> PS_INVALID; nonfinite arithmetic -> PS_NUMERIC. All failures preserve out. Unit symbol is borrowed from the property.
