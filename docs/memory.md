# Speicherverwaltung und Arenen

`physim/memory.h` bietet explizite Allocatoren, geprüfte Speicheroperationen und
eine Arena mit festem Speicherbereich. Es gibt keinen global umschaltbaren
Allocator und keine GUI-Abhängigkeit.

[Dynamische Arrays](array.md) verwenden denselben Allocator-Vertrag und unterstützen
Größenlimits sowie transaktionales Verhalten bei Speicherfehlern.

## Allocator-Vertrag

`ps_allocator` besteht aus `user`, `allocate(user, bytes)` und
`deallocate(user, pointer, bytes)`. Der Descriptor wird als Wert übergeben und
von Berichten beziehungsweise Analysekontexten kopiert. Der Zustand hinter `user`
und der Callback-Code müssen sämtliche zugehörigen Allokationen überleben. Dazu
gehört auch, dass ein Modul mit diesen Callbacks nicht vorzeitig entladen wird.

Der Allokationscallback erhält eine positive Byteanzahl und liefert entweder
NULL oder einen eigenen, beschreibbaren Speicherblock. Dessen Adresse muss ein
Vielfaches von `PS_MEMORY_ALIGNMENT` sein. Das genügt den fundamentalen C-Typen
der unterstützten Plattformen, aber nicht beliebigen überausgerichteten Typen.
Unter Standard-C wird `max_align_t` verwendet; für MSVC/Clang-Cl mit den Windows-
Headern stellt Physim den entsprechenden Typ `ps_memory_alignment` bereit.

Der Freigabecallback erhält den ursprünglichen Pointer und exakt die ursprünglich
angeforderte Byteanzahl. Speicher darf nur an denselben Allocator zurückgegeben
werden. NULL wird nicht an den Freigabecallback weitergereicht. Callbacks dürfen
nicht per `longjmp` abbrechen oder dasselbe Objekt während seiner Konstruktion
oder Zerstörung erneut aufrufen. Gemeinsamer veränderlicher Allocatorzustand
benötigt Synchronisation durch den Aufrufer.

`ps_allocator_default()` liefert einen Descriptor um `malloc` und `free`.
Bestehende Konstruktoren verwenden ihn weiterhin.

## Geprüfte Speicheroperationen

- `ps_memory_allocate(allocator, bytes, &storage)`: nicht initialisierte Bytes.
- `ps_memory_zero(allocator, count, size, &storage)`: prüft zuerst `count * size`
  auf Überlauf und setzt alle angeforderten Bytes auf null.
- `ps_memory_resize(allocator, old, old_bytes, new_bytes, &storage)`: reserviert
  einen neuen Block, kopiert den gemeinsamen Präfix und gibt erst danach den
  alten Block frei. Zusätzliche Bytes bleiben uninitialisiert. Gleiche Größe
  behält den alten Block. Größe null gibt ihn frei und liefert NULL.
- `ps_memory_free(allocator, pointer, bytes)`: gibt einen gültigen Block frei;
  NULL ist erlaubt. Pointer, Größe und Allocator müssen zusammenpassen.

Eine Anforderung von null Bytes ist erfolgreich, liefert NULL und ruft keinen
Allokationscallback auf. Bei Fehlern bleibt der Ausgabezeiger unverändert;
bei fehlgeschlagenem Vergrößern bleiben zusätzlich der alte Speicher und dessen
Inhalt erhalten. Der Ausgabezeiger selbst darf nicht innerhalb des umzuziehenden
Blocks liegen. `old_bytes` muss der alten Allokation entsprechen.

Fehlercodes: `PS_INVALID` für ungültige Parameter/Callbacks, `PS_LIMIT` für
Multiplikationsüberlauf, `PS_MEMORY` bei abgelehnter Speicheranforderung. Ein
Bytebudget oder absichtlich ausgelöster Testfehler wird über den Callback ebenfalls
als `PS_MEMORY` gemeldet. Der Aufrufer besitzt erfolgreich zurückgegebenen Speicher.

## Arena

`ps_arena_init(&arena, buffer, capacity)` übernimmt einen vom Aufrufer bereitgestellten
Bereich ohne ihn zu besitzen oder zu löschen. Über `ps_arena_allocator(&arena)` kann
dieser Bereich als Allocator verwendet werden. Die Arena richtet jede Allokation aus,
auch wenn der Anfang des Puffers nicht ausgerichtet ist. Es gibt keine automatische
Vergrößerung und keinen Rückfall auf den Heap. Erschöpfung lässt `used` unverändert.

`arena.used` und `arena.capacity` zeigen belegte beziehungsweise verfügbare Gesamtgröße;
Padding zur Ausrichtung zählt als belegt. Diese Felder sind für den Nutzer nur lesbar.
Einzelne Freigaben geben keinen Arenaspeicher zurück. Auch Resize belegt einen neuen
Block; der alte Bereich bleibt bis zum Reset verbraucht. `ps_arena_reset` setzt nur den
Belegungszähler zurück und löscht keine Daten. Er invalidiert alle bisherigen Pointer.
Zuerst alle Besitzer zerstören, damit beispielsweise Dateihandles geschlossen werden.

Für langlebige Berichte mit vielen wiederholten Änderungen oder Exporten eignet sich
ein freigebender Allocator besser. Eine Arena passt zu einer zusammenhängenden
Berechnungsphase, nach der alle Objekte gemeinsam verschwinden. Das Fehlschlagen
einer Objektoperation rollt bereits verbrauchte Arenablöcke nicht automatisch zurück.

```c
#include <physim/report.h>

ps_allocator heap = ps_allocator_default();
void *storage = NULL;
size_t bytes = 1024 * 1024;
ps_result result = ps_memory_allocate(heap, bytes, &storage);
if (result == PS_OK) {
    ps_arena arena;
    ps_arena_init(&arena, storage, bytes);
    ps_report *report = NULL;
    result = ps_report_create_with_allocator(
        "Auswertung", "", ps_arena_allocator(&arena), &report);
    /* Bei PS_OK Diagramme oder Tabellen aufbauen. */
    ps_report_destroy(report);
    ps_arena_reset(&arena);
    ps_memory_free(heap, storage, bytes);
}
```

Das Beispiel reserviert den Arena-Puffer einmal über den Standardallocator.
Eigener, passend dimensionierter Speicher kann ebenso bereitgestellt werden;
seine Lebensdauer und tatsächlich zugängliche Größe liegen beim Aufrufer.

## Berichte und Analysekontexte

`ps_report_create_with_allocator`, `ps_report_load_with_allocator` und
`ps_analysis_create_with_allocator` ergänzen die bisherigen APIs. Die alten
Funktionen bleiben verfügbar und verwenden den Standardallocator. Opaque Besitzer
merken sich ihren Allocator bis zur Zerstörung; die öffentlichen Modulstrukturen
und das Berichtsdateiformat ändern sich nicht.

Beim Bericht gehören das Objekt, Kurven, Tabellen und die temporären Puffer beim
Laden, Speichern und SVG-/Plot-CSV-Export zur gewählten Speicherdomäne. Der
Tabellen-CSV-Export benötigt keinen eigenen Heap-Puffer. Die Serialisierung reserviert
derzeit vorübergehend bis zu 8 MiB zusätzlich zum lebenden Bericht. Ein kleineres
Budget kann deshalb den Aufbau erlauben und später das Speichern ablehnen.

Der Analysekontext besitzt einen festen, über diesen Allocator reservierten
Speicherblock. Datenreihen und deren Transformationen verwenden begrenzte
Arbeitspuffer und temporäre Dateien. Ihr vorhandenes `scratch_byte_limit` begrenzt
Dateinutzdaten und ist **kein** Heap-Limit.

C-stdio, Betriebssystem, Stack, Batchcontroller und GUI bleiben außerhalb dieser
Speicherdomänen. Das ist kein Prozess-Speicherlimit und keine Sandbox.

## Tests

`tests/test_allocator.h` enthält einen Allocator mit gezieltem Fehler bei der
n-ten Allokation, Bytebudget, vergifteten neuen Blöcken und Erfassung aller lebenden
Pointer und ihrer Größen. Er erkennt falsche Freigabegrößen, fremde Pointer und
doppelte Freigaben. Dieser Testhelfer gehört nicht zur öffentlichen Produkt-API.

`memory` prüft Überlauf, Fehlererhaltung beim Resize, Ausrichtung, Arena-Grenzen
und Wiederverwendung. `memory_owners` unterbricht jede Allokation eines vollständigen
Berichtablaufs und eines Ladevorgangs, prüft Aufräumen nach jedem abgeschnittenen
Dateipräfix, Erhaltung bestehender Berichte und Freigaben nach Exportfehlern.
Ein echter Datensatz mit abgeleiteter Datenreihe und ein kompletter Bericht werden
zusätzlich über Arenen verarbeitet. Die App-Regressionsprüfungen decken den
bestehenden Standardallocator-Pfad ab.
