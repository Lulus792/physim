# ADR 001: Grundlage des ersten vertikalen Durchstichs

Datum: 2026-09-16. Status: implementierte Ausgangsentscheidung.

- Lizenz: MIT, vom Projekteigentümer ausdrücklich gewählt.
- Eigener Produktcode: C17. CMake und CI-Dateien sind Buildkonfiguration.
- Windows-Basis: Windows 10 ab 1903 wegen UTF-8-Prozessmanifest, MSVC 2022.
- Linux-Ziel: aktuelle glibc-Distributionen, GCC und Clang. Validierung über CI;
  eine lokale Linux-Laufzeit steht auf dem Entwicklungsrechner nicht bereit.
- UI: SDL3 3.2.30 und vendored Nuklear. Keine GUI-Abhängigkeit der Kernbibliothek.
- Renderer: Der ursprüngliche SDL-Renderer mit CPU-Projektion wurde durch
  OpenGL 3.3 Core ersetzt. UI und Szene teilen einen Kontext; Details, Tests
  und aktuelle Grenzen stehen in ADR 002.
- Compiler: CMake erkennt MSVC/GCC/Clang; Experimentbuild läuft als separater
  Hintergrundprozess mit Argumentliste. Die erste Distribution enthält keinen Compiler.
- Isolation: getrennte Prozesse, versionierte lokale Pipe-Kommunikation. Kein OS-Sandboxing.
- Daten: little-endian, IEEE-754 Float64, append-only Chunks mit CRC32. Die Vorschau
  darf ausdünnen, der Runner schreibt jeden erfolgreichen Physikschritt.
- Start-Zielgröße: blockweises Lesen beliebig langer Dateien, bis 16 skalare Kanäle,
  dt standardmäßig 5 ms, Vorschau maximal 60 Hz. Durchsatzgrenzen sind zu messen.
- Genauigkeit: referenzgeprüfte Numerik statt plattformübergreifendem Bitgleichheitsversprechen.

Die Entscheidung priorisiert den im Projektplan geforderten Code-bis-Diagramm-Weg.
Fortgeschrittene Editorfunktionen und ein generischer Rigid-Body-Solver werden
nicht durch diese ersten Implementierungen vorweggenommen.
