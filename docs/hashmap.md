# Hashmap mit eigenen Schlüsseln

`physim/hashmap.h` ordnet Byte-Schlüsseln Werte einer festen Größe zu. Schlüssel
und Werte werden beim Einfügen kopiert und gehören anschließend der Map. Leere
Schlüssel und eingebettete Nullbytes sind gültig; UTF-8 wird wie bei String-Views
byteweise verglichen. Es gibt keine automatische Normalisierung oder Änderung
der Groß-/Kleinschreibung.

Der Konstruktor erhält einen expliziten Allocator, die Wertgröße, die maximale
Eintragsanzahl und die maximale Schlüssellänge. Wertgröße und Eintragslimit müssen
positiv sein. Ein Schlüssellimit von null erlaubt ausschließlich den leeren
Schlüssel. Die Grenzen sind verbindlich und haben keine impliziten Standardwerte.

```c
#include "physim/hashmap.h"

ps_result store_parameters(ps_hashmap **out) {
    if (!out) return PS_INVALID;
    ps_hashmap *map;
    ps_result r = ps_hashmap_create(ps_allocator_default(), sizeof(double), 64, 48, &map);
    if (r != PS_OK) return r;
    double mass = 2.5;
    r = ps_hashmap_set(map, (ps_string_view){"mass_kg", 7}, &mass);
    if (r != PS_OK) {
        ps_hashmap_destroy(map);
        return r;
    }
    *out = map;
    return PS_OK;
}
```

Der Aufrufer zerstört die erfolgreiche Map mit `ps_hashmap_destroy`. Der Allocator
und sein Benutzerzustand müssen bis dahin verfügbar bleiben. Die Map führt keine
Destruktoren oder tiefen Kopien aus; Zeiger innerhalb eines Wertes behalten ihre
ursprünglichen Eigentumsregeln. Werte sind bis `PS_MEMORY_ALIGNMENT` ausgerichtet.

## Operationen

`ps_hashmap_set` fügt einen neuen Schlüssel ein oder ersetzt dessen Wert. Ersetzen
benötigt keine neue Allokation und funktioniert auch bei erreichtem Eintragslimit.
`ps_hashmap_get` kopiert einen Wert in einen ausreichend großen, vom Aufrufer
besessenen Puffer. Dieser darf keinen Speicher innerhalb der Map überlagern.
`ps_hashmap_erase` entfernt einen Eintrag. Fehlt ein Schlüssel, geben `get` und
`erase` `PS_EOF` zurück; `get` verändert den Ausgabepuffer dabei nicht.

`ps_hashmap_reserve` reserviert die Bucket-Tabelle für eine Mindestanzahl von
Einträgen. Einzelne Schlüssel-/Wertknoten werden weiterhin separat allokiert.
Die Tabelle wächst in Zweierpotenzen, beginnend mit acht Buckets. Pro Bucket
können mehrere verkettete Einträge liegen. `clear` gibt alle Einträge frei und
behält die Bucket-Tabelle; `destroy` gibt auch diese und das Map-Objekt frei.

Der Hash ist deterministisches 64-Bit-FNV-1a. Kollisionen werden durch Hash-,
Längen- und Bytevergleich unterschieden. Dies ist keine kryptografische Map:
Gezielt kollidierende Eingaben können eine lange Kette und lineare Suche bewirken.
Für nicht vertrauenswürdige große Eingaben sind die Größenlimits allein kein
Rechenzeitlimit. Die Map übernimmt keine OS-Sandbox- oder Scheduling-Aufgabe.

## Traversierung und Zeiger

`ps_hashmap_visit` ruft einen Callback einmal pro Eintrag auf. Die Reihenfolge ist
nicht festgelegt und darf sich nach einer Mutation ändern. Wissenschaftliche
Auswertungen, die eine feste Summationsreihenfolge benötigen, müssen die Daten
selbst explizit ordnen.

Der Callback erhält schreibgeschützte, geliehene Schlüssel- und Wertzeiger.
Andere Einfügungen und `reserve` verändern ihre Adressen nicht. Entfernen des
Eintrags, `clear` oder `destroy` machen sie ungültig; Ersetzen ändert den Wert
am bestehenden Ort. Für unabhängige Lebensdauer eigene Kopien anlegen.

Während der Traversierung sind Leseoperationen zulässig. Mutation und rekursive
Traversierung ergeben `PS_INVALID`; `destroy` führt währenddessen nichts aus.
Den Callback nicht per `longjmp` verlassen. Jeder Callback-Status außer `PS_OK`
beendet die Traversierung und wird zurückgegeben, etwa `PS_EOF` für einen
beabsichtigten frühen Abbruch. Bereits ausgeführte Callback-Nebenwirkungen werden
nicht rückgängig gemacht.

## Speicherfehler und Grenzen

`PS_MEMORY` meldet einen fehlgeschlagenen Speicherwunsch, `PS_LIMIT` überschrittene
Schlüssel-/Eintragslimits oder nicht darstellbare Allokationsgrößen und
`PS_INVALID` ungültige Argumente. Bei fehlgeschlagenem Einfügen oder Reservieren
bleiben vorhandene Einträge und ihre Traversierungsreihenfolge erhalten.

Beim Wachstum existieren alte und neue Bucket-Tabelle kurzzeitig gleichzeitig.
Eintragslimits begrenzen deshalb nicht unmittelbar den Spitzenverbrauch. Ein
Allocator mit Bytebudget kann diesen zusätzlich beschränken. Arena-Allocatoren
gewinnen durch einzelne Freigaben keinen Speicher zurück; erst alle Benutzer
zerstören und anschließend die Arena zurücksetzen.

Der Test `hashmap` prüft 5.000 deterministische Operationen gegen ein einfaches
Referenzmodell, 64 Schlüssel im selben Bucket, Löschen aus Kollisionsketten,
Schlüssel-/Wertkopien, Wertausrichtung, Traversierungsschutz, frühe Abbrüche,
Größenlimits und Fehler bei Objekt-, Eintrags- und Bucket-Allokationen.
