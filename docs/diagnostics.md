# Strukturierte Diagnosen

Ein Fehler besitzt einen Code und getrennte Felder für Operation, Argument,
Nachricht und ursprüngliche Quellstelle. `ps_diagnostic` ist ein eigener Wert;
Kopien teilen keinen Speicher. Es gibt keinen globalen Last-error-Zustand.
Die bisherige Textausgabe bleibt für ältere Module und Clients erhalten.

```c
ps_diagnostic error;
ps_diagnostic_set(&error, PS_SINGULAR, "linearSolve", "matrix",
                  __FILE__, __LINE__, 1, "Matrix ist singulär");
return ps_experiment_fail(context, &error);
```

`ps_experiment_fail` speichert den Wert und einen begrenzten kompatiblen Text
in `context->error`. Die Funktion liefert den gespeicherten Fehlercode; der
Callback muss ihn zurückgeben, um den Lauf abzubrechen. Ältere ABI-3-Contexts
bekommen weiterhin den Text. `ps_experiment_diagnostic` liefert eine Kopie,
beziehungsweise `PS_VERSION`, wenn der optionale Diagnose-Tail fehlt.

```text
let failure = Diagnostic.here(8, "linearSolve", "matrix", "Matrix ist singulär")
failure.raise()
```

Physim 0.175.0 besitzt den Werttyp `Diagnostic`. `Diagnostic.here` erfasst den
Quellpfad, die Zeile und die Spalte dieser Expression. Die vollständige Factory
ist `Diagnostic(code, operation, argument, source, line, column, message)`.
`Diagnostic.empty()` besitzt Code 0. Methoden `code()`, `operation()`, `argument()`,
`source()`, `line()`, `column()` und `message()` lesen die Felder; `formatted()`
liefert einen eigenen String. `isValid()` prüft den Wert. `encoded()` liefert
`[Int64]` mit Bytes 0–255; `Diagnostic.decode(bytes)` prüft den vollständigen
Payload. `save(path)` legt exklusiv eine Datei an, `Diagnostic.load(path)` liest
sie. Vorhandene Dateien bleiben erhalten. `.raise()` löst den gespeicherten
Fehler aus; außerhalb eines abgefangenen Wertausdrucks endet der Callback.
`currentDiagnostic()` liest den aktuellen Experimentcontext.

`attempt` fängt weiterhin Fehler innerhalb eines Wertausdrucks ab. Ein dort
abgefangener Fehler verändert weder Hostdiagnose noch Legacy-Fehlertext.
Eine `.raise()`-Anweisung lässt sich dazu in eine Funktion mit Ergebniswert
einbetten; `attempt(function())` liefert bei Fehler `nil`. Besitzer und
Aufräumregeln bleiben erhalten. Nicht abgefangene Sprachfehler tragen ihre
ursprüngliche Expression-Position. Core-Fehlercodes aus Analyseoperationen bleiben
erhalten; gewöhnliche Rechen-/Assert-Fehler verwenden `PS_NUMERIC`.

## Fehlercodes und Grenzen

| Code | C-Wert | Bedeutung |
| --- | --- | --- |
| 0 | PS_OK | leere Diagnose, kein Fehler |
| 1 | PS_INVALID | ungültiges Argument |
| 2 | PS_IO | Datei-/Prozess-I/O |
| 3 | PS_MEMORY | Speicher erschöpft |
| 4 | PS_VERSION | nicht unterstützte Version |
| 5 | PS_CORRUPT | beschädigte Daten |
| 8 | PS_SINGULAR | singuläres/numerisch schlecht konditioniertes System |
| 9 | PS_LIMIT | Iterations-, Schritt- oder Ressourcengrenze |
| 10 | PS_NUMERIC | nichtendliche numerische Berechnung oder Sprachfehler |

`PS_EOF=6` und `PS_RECOVERED=7` sind keine Fehlerdiagnosen. Die C-Konstruktion
liefert `PS_OK`, wenn sie einen Fehlerwert erfolgreich erstellt hat; dieser
Konstruktionsstatus unterscheidet sich vom gespeicherten Fehlercode.

Operation und Argument haben je höchstens 64 UTF-8-Bytes, Quelle und Nachricht
je höchstens 1024 Bytes, jeweils ohne mitgezählten NUL-Abschluss. C-Eingaben sind
NUL-terminiert. Nur Nachrichten erlauben LF/CR/tab. Koordinaten sind 1-basiert,
0 bedeutet unbekannt; eine Spalte benötigt eine Zeile, Koordinaten eine Quelle.
Ungültige Eingaben erhalten den Ausgabewert. `ps_diagnostic_format` begrenzt Text
an einer UTF-8-Grenze und meldet Kürzung mit `PS_LIMIT`.

## Runner, App und Aufzeichnung

Fehlgeschlagene Experimente speichern `<output.psrun>.psdiag`, Analysen
`<output-prefix>.psdiag`, sofern das neue exklusive Anlegen möglich ist. Eine
fehlende Diagnose-Datei verändert den ursprünglichen Fehlercode nicht. Die Datei
ist separat von den Messdaten und enthält exakte Felder mit Version und CRC.
Sie ist kein vollständiges Ausführungsjournal und garantiert keine
Stromausfall-Durabilität.

`--interactive --diagnostics` aktiviert `PS_MSG_DIAGNOSTIC=12` im IPC-5-Strom.
Ohne Opt-in bleibt `PS_MSG_ERROR=7` mit bisherigem Text. Die App fordert die
strukturierten Daten an und benötigt keinen Regex für deren Quellposition.
Sie zeigt den Fehler an und öffnet per Klick den passenden Projekteditor oder
zusätzlichen Quelltext. Einträge ohne bekannte Zeile springen nicht an eine
beliebige Codeposition. Compilertexte werden weiterhin separat geparst.

Analyse-Module erhalten die neue optionale ABI-3-Funktion
`run_diagnostic(inputs, count, prefix, out)`. Sie arbeitet synchron mit 0–8
Eingaben. Ältere Deskriptoren mit `run`/`run_many` bleiben gültig und bekommen
bei Fehler eine generische Runnerdiagnose. Die alte `run_many`-Feldgrenze wird
unabhängig von der neuen Deskriptorgröße geprüft. Neue Physim-Analysemodule liefern
die vollständigen Fehlerdaten über diesen Callback.

Die [C-Referenz](reference/diagnostic.md) beschreibt Core-Funktionen und
Fehlerverhalten. Ausführbare Beispiele liegen unter
[C-Experiment](../examples/diagnostics/main.c),
[C-Analyse](../examples/diagnostics/analysis.c),
[Physim-Experiment](../examples/language/diagnostic_experiment.phys),
[Physim-Analyse](../examples/language/diagnostic_analysis.phys) und
[Wert-/Dateirundlauf](../examples/language/diagnostic_values.phys).
Die Fehlerbeispiele brechen absichtlich ab.
