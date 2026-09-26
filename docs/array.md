# Dynamische Arrays

`physim/array.h` bietet zusammenhängende Arrays für per Byte kopierbare C-Werte.
Ein Array besitzt seinen Speicher und einen ausdrücklich übergebenen Allocator.
Es gibt weder versteckte globale Zustände noch eine Abhängigkeit zur GUI.

`ps_array_init(allocator, sizeof(T), maximum_count, &array)` initialisiert ohne
Allokation. Das Elementlimit ist verbindlich; null wählt die größte ohne
Byteüberlauf darstellbare Anzahl. Die tatsächliche Speicherverfügbarkeit bleibt
davon unabhängig. Der Typ darf keine größere Ausrichtung als
`PS_MEMORY_ALIGNMENT` benötigen. Arrays führen keine Konstruktoren, Destruktoren
oder tiefen Kopien aus: Enthaltene Zeiger und die dahinterliegenden Ressourcen
bleiben in der Verantwortung des Aufrufers.

```c
#include "physim/array.h"

/* out must not already own storage. Caller destroys the successful result. */
ps_result make_positions(ps_array *out) {
    ps_result r = ps_array_init(ps_allocator_default(), sizeof(ps_vec3), 4096, out);
    if (r != PS_OK) return r;
    for (unsigned i = 0; i < 100; i++) {
        ps_vec3 point = ps_v3(i * .01, 0, 0);
        r = ps_array_append(out, &point, 1);
        if (r != PS_OK) {
            ps_array_destroy(out);
            return r;
        }
    }
    return PS_OK;
}
```

Danach sind `((ps_vec3 *)array.data)[0..array.count)` gültige Werte. Die Felder
`count`, `capacity`, `element_size`, `maximum_count` und `allocator` dürfen nicht
direkt verändert werden. Einen besitzenden Array-Descriptor nicht kopieren; vor
erneuter Initialisierung zuerst `ps_array_destroy` aufrufen. Der Allocator und
sein Benutzerzustand müssen bis zur Zerstörung gültig bleiben.

## Operationen und Lebensdauer

| Funktion | Verhalten |
| --- | --- |
| `ps_array_reserve` | Stellt mindestens die gewünschte Kapazität bereit; Anzahl bleibt gleich |
| `ps_array_resize` | Ändert die Anzahl; neu sichtbare Bytes werden nullgesetzt |
| `ps_array_insert` | Fügt vor einem Index ein und verschiebt den Rest |
| `ps_array_append` | Hängt einen Bereich an |
| `ps_array_erase` | Entfernt einen Bereich und schließt die Lücke |
| `ps_array_clear` | Setzt die Anzahl auf null, hält den Speicher |
| `ps_array_shrink` | Verkleinert die Kapazität auf die Anzahl |
| `ps_array_destroy` | Gibt Speicher frei und setzt den Descriptor auf null |

Nullgesetzte Bytes sind keine typabhängige Initialisierung; insbesondere ersetzt
dies keine Konstruktion komplexer Werte. Entfernen oder Verkleinern ruft keine
Freigabe für enthaltene Zeiger auf. Ein leerer Bereich darf einen NULL-Quellzeiger
verwenden. Die Elemente müssen bei nichtleeren Operationen vollständig lesbar sein.

Wachstum und erfolgreiche Kapazitätsverkleinerung können alle Elementzeiger
ungültig machen. Einfügen und Löschen verschieben Elemente: Positionsreferenzen
ab dem betroffenen Index müssen entsprechend angepasst werden. `clear` behält
zwar den Speicher, macht aber alle bisherigen Elemente logisch ungültig.

Selbsteinfügen ist erlaubt, etwa das Verdoppeln durch
`ps_array_append(&array, array.data, array.count)`. Der Quellbereich muss im
aktuellen Inhalt liegen und an einer Elementgrenze beginnen. Einfügen aus dem
eigenen Inhalt vor dem Ende benötigt bei ausreichender Kapazität einen temporären
Speicherblock, damit die Verschiebung die Quelle nicht überschreibt.

## Grenzen und Speicherfehler

Die Kapazität wächst geometrisch, beginnend mit höchstens acht Elementen und
begrenzt durch `maximum_count`. Bei Wachstum liegen alter und neuer Speicher
kurzzeitig gleichzeitig vor. Das Elementlimit ist deshalb kein Limit für den
Spitzenverbrauch. Für ein Bytebudget einen entsprechend begrenzten Allocator
verwenden. Auch Selbsteinfügen und `shrink` können Speicher anfordern und scheitern.

`PS_LIMIT` meldet ein überschrittenes Elementlimit oder einen Byteüberlauf,
`PS_MEMORY` einen abgewiesenen Speicherwunsch und `PS_INVALID` einen ungültigen
Aufruf. Auf Fehler bleiben Descriptor, lebende Elemente und bisherige Zeiger
unverändert. Der Allocator kann dennoch einen fehlgeschlagenen Versuch zählen;
seine externen Nebenwirkungen werden nicht zurückgenommen.

Mit einem Arena-Allocator gibt es keinen Heap-Rückfall. Wachstum benötigt einen
neuen Arenablock; Freigaben gewinnen dort keinen Speicher zurück. Erst alle
Array-Benutzer zerstören, dann die Arena zurücksetzen. Für vorher bekannte Mengen
einmalig reservieren, um wiederholtes Wachstum zu vermeiden.

Der Test `array` vergleicht 5.000 deterministische Operationen mit einem festen
Referenzarray. Er prüft Selbstkopien mit und ohne Wachstum, Nullinitialisierung,
Ausrichtung, temporäre Speicherfehler, fehlgeschlagenes Wachstum/Schrumpfen,
Größenlimits, exakte Freigabegrößen, Bytebudgets und Arena-Verhalten.
