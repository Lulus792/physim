# C-Scopes aus der tatsächlichen Grammatik — 5. November 2026

`app/c_bindings.c` ermittelt geschriebene Objektdeklaratoren und Parameter in den
am Cursor aktiven C-Bereichen. Die gesamte originale Quelle wird unverändert mit
der [vorher geprüften Grammatik](c-syntax-foundation-2026-11-05.md) gelesen.
Auch im abgeflachten ERROR-Baum bleiben echte Klammer-/Deklaratorknoten die
Grundlage; keine zusätzliche Browsergrammatik, synthetischen Namen oder Quellumschreibung.

Der Sammler erhält originale Byte-, Zeilen- und Spaltenpositionen. Er unterscheidet
Funktionsdeklarationen, Funktionszeiger, Arrays und direkt verfügbare Parameter.
Geschlossene/fremde Bereiche, zukünftige Deklaratoren, die eigene offene
Initialisierung und innere Callback-Parameter bleiben ausgeschlossen. Nach einem
echten Komma kann der vorherige vollständige Deklarator bereits angeboten werden.
`for`-Variablen enden mit ihrem Schleifenbereich; typisierte ältere Parameterformen
gehören zum Funktionsrumpf und gelangen nicht in den globalen Bereich.

Die Typdarstellung setzt originale Specifier-/Deklaratorbereiche ohne den
Bezeichner zusammen: etwa `const Velocity *`, `int (*)(double x)` oder `int [10]`.
Sie zeigt die geschriebene Form, ohne implizite C-Typanpassungen, Präprozessor-
oder Include-Auflösung, Typinferenz oder Laufzeitwerte. Äußere überdeckte Namen
bleiben in Deklarationsreihenfolge für die spätere Zusammenführung erhalten.

**Noch nicht an Completion-Modell oder GUI angebunden.** C-Aufrufkontext und leere
Argumente, Besitzer-/Revisionsbindung, Quell-/Typkarte, Cache, Einfügen/Undo und
separate Signaturhilfe müssen anschließend integriert und nativ geprüft werden.
Die GUI-unabhängige Core-Bibliothek bleibt unabhängig. Das freigegebene Design D
wird durch diesen Schritt nicht verändert.

## Ausgeführte Nachweise

[Originalkommandos, Ergebnisse, Quell-/Archivhashes und Grenzen](c-bindings-2026-11-05.json).
Zuerst Windows Release/MSVC `run-vjokuzca` und Linux Debug/GCC ASan/UBSan
`run-tf8s4ihe` je **1/1 `c_bindings`**, damals 32 Codefälle. Nach Erweiterung und
Korrektur der Rückgabe-Deklaratorformen abschließend Windows `run-0a_i9bxg` und
Linux `run-tdymmf7x` wiederum je **1/1**, jetzt mit:

- **35** Originalcode-Fällen einschließlich voller Dokumente und offener Aufrufe;
- **182** Cursorpositionen, jeweils mit vollständigem Dokument und Präfix, gegen
  ausdrücklich festgelegte Sichtbarkeitsintervalle geprüft;
- **65/65** Allokationsfehlern: vollständige Freigabe und erfolgreicher Neuversuch;
- **1.024** deterministischen fehlerhaften Byteeingaben, Quell-/NUL-/UTF-8-Cursor-/
  Ausgabegrenzen und Größenabfrage/Begrenzung der Typausgabe.

Zusätzlicher direkter C17-GCC-Build mit `-Wall -Wextra -Werror`, ASan/UBSan und
`detect_leaks=1`: bestanden, keine Warnungen. Die letzten inkrementellen nativen
Builds kompilierten nur Test und Sammler neu und waren ebenfalls ohne Warnungen;
die initialen Builds enthalten weiterhin die früher dokumentierten Runtime-/
Alignment-/Beispielwarnungen. Keine warnungsfreie Gesamt-Toolchain behauptet.

Zwei echte Entwicklungsregressionen sind erhalten: Präfixposition 140 verlor
zunächst die Laufvariable nach dem fertigen Schleifenkopf; die erweiterte Probe
für Zeiger-Rückgaben verlor Parameter `dt` (Fall 32). Beide vorher Exit 1, nach
Korrektur im abschließenden Stand bestanden. Eine zusätzliche isolierte Gegenprobe
mit absichtlich aufgehobener Bereichsgrenze wird im finalen Test korrekt abgelehnt:
Fall 2 meldet die unerlaubten Namen `foreign`/`hidden`. Die Originalquelle wurde
für diese Gegenprobe nicht verändert. Keine native GUI-Vorherprüfung daraus ableiten.

Snapshots `c-bindings-first`/`c-bindings-final`: je **278** eigene und **1.217**
Compiler-/Testeingaben, 254 Windows-/139 Linux-Laufzeitdateien; alle referenzierten
Archivbytes gelesen. Das Laufzeitinventar enthält unveränderte ältere Programme
und ist keine erneute Testausführung dieser Fälle. Keine neuen Bilder, keine
Wiederholung abgeschlossener F1-/Protokoll-/Settings-Prüfungen. Der Nutzer-Mac blieb
aus und unberührt; neue macOS-, GUI-, Leistungs- und physische Abnahme sind offen.
