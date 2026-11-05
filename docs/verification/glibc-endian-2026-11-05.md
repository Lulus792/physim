# Endian-Schnittstellen im Linux-C-Parser

Die frischen Linux-Läufe zur Quelle13cc4422 scheiterten vor ihren eigentlichen
Prüfungen: Ubuntu24.04/Clang meldete fehlende Deklarationen für `le16toh` und
`be16toh`; Debian12/GCC kompilierte mit Warnungen und scheiterte beim Linken an
diesen Symbolen. Dadurch entstanden keine nativen Ergebnisarchive. Die Logs
sind nach Quellstand und Job gebunden im [strukturierten Bericht](glibc-endian-2026-11-05.json).

Der Physim-Adapter bindet die gepinnte Tree-sitter-Laufzeit ein. Deren unveränderte
Portabilitätsschicht wählt unter Linux `<endian.h>`. Die strikten C17-/POSIX-
Optionen reichen unter glibc für diese Schnittstellen nicht aus. Benötigt wird
`_DEFAULT_SOURCE` vor den Systemheadern: [Endian-Dokumentation](https://man7.org/linux/man-pages/man3/endian.3.html),
[GNU-glibc2.36-Handbuch](https://sourceware.org/glibc/manual/2.36/html_node/Feature-Test-Macros.html).

`app/c_syntax.c` fordert diese Schnittstellen jetzt am Dateianfang ausschließlich
für Linux an, sofern das Makro nicht schon definiert ist. Die bestehenden
POSIX-/Compileroptionen und die eingebundenen Bibliotheksdateien bleiben erhalten.
Windows und macOS durchlaufen diesen zusätzlichen Präprozessorzweig nicht.

Lokal bestehen Windows Release **8/8 Datenprüfungen und2/2 GUI-Gruppen**
(`run-ms_cod0i`, `run-fw347tfd`) sowie Alpine Linux Debug mit ASan/UBSan
**8/8 und2/2** (`run-07qrdp6f`, `run-syykpp1q`). Die Datenprüfungen verwenden
`detect_leaks=1` und prüfen Parser, Bindungen, Aufrufkontext, Signaturen, Lookup,
lokale Variablen und Aufrufziele. Die GUI-Gruppen prüfen vorhandene Completion-
und Parameterhilfeabläufe. Für die reine Integrationskorrektur wurden keine
zusätzlichen Bilder eingeführt.

Vor dem Build war die eine geänderte Adapterdatei ausdrücklich im303-Dateien-
Manifest enthalten und bytegleich zu Root und Isolation. Beide relevanten
Upstream-Header wurden roh gegen Git geprüft. Source-/Runtimearchive sind
eingefroren und vollständig gelesen. Die lokale Linux-Umgebung verwendet musl;
sie ersetzt **keinen erfolgreichen neuen Ubuntu-/Debian-glibc-Lauf**. Dessen
Abnahme bleibt bis zu tatsächlich ausgeführten neuen CI-Ergebnissen offen.
Ebenso bleiben die volle UI-, physische Eingabe- und Screenreader-Abnahme offen.
