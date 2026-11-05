# C-Syntaxgrundlage — 5. November 2026

Die C-Vervollständigung benötigt echte Deklaratoren und Scopes. Als Voraussetzung
liegt jetzt ein eigener, begrenzter Syntaxadapter mit unverändert eingebundener
Tree-sitter-Laufzeit 0.27.1 und C-Grammatik 0.24.2 vor. Er gibt originale Bytebereiche,
Felder und Elternbeziehungen zurück; keine Bibliotheksobjekte verlassen den Aufruf.
[Entscheidung und Grenzen](../decisions/026-c-editor-grammar.md).

**Noch keine Änderung der C-Vorschlagsliste:** Das Modul ist bislang ausschließlich
im registrierten Test eingebunden. Scope-Sammlung, GUI-Anbindung, C-Typdarstellung
und separate Parameterhilfe bleiben offen. Es ersetzt keinen Präprozessor oder
Compiler und liest keine Includes. Das freigegebene Design D bleibt erhalten.

## Ausgeführte Prüfungen

[Originalkommandos, Ergebnisse, Quell-/Artefakthashes und Grenzen](c-syntax-foundation-2026-11-05.json).
Windows Release/MSVC und Linux Debug/GCC ASan/UBSan: jeweils **1/1 `c_syntax`**,
`--no-app --test --test-filter c_syntax`, Linux zusätzlich `--sanitizers` mit
Leak-Prüfung. Tatsächliche Laufverzeichnisse: Windows `run-xsmeb1mw`, Linux
`run-rwgb14le`. Beide beendet, Exit 0, leeres Test-stderr.

Ein zusätzlicher direkter C17-GCC-Build mit `-Wall -Wextra -Werror`, ASan/UBSan
und `detect_leaks=1` ist ohne Warnungen bestanden. Geprüft wurden:

- echte C-Grammatik, UTF-8-Quellbereiche und nach Freigabe gültige Grammatiknamen;
- offene Aufrufe mit tatsächlichen ERROR-Knoten;
- alle **94/94** Allokationsfehler des Prüffalls: genaue Freigabe, kein lebender
  Speicher, anschließender Wiederholungsaufruf erfolgreich;
- Quell-/NUL-/Ausgabe-/Heapgrenzen und Abbruch durch das Fortschrittsbudget;
- zwei echte OS-Threads mit je 40 unabhängigen erfolgreichen bzw. fehlschlagenden
  Aufrufen sowie **1.024** deterministische fehlerhafte Byteeingaben.

Die private Allokationsdomäne verwendet die vorhandenen Compilezeit-Hooks und
eine C-interne Fehlergrenze; kein globaler Allocatorwechsel und kein Prozessabbruch
bei Speichermangel. ELF-Prüfung: Runtime-Funktionen lokal, Allocator-Symbol versteckt,
keine dynamischen Exporte; Windows-PE-Exportverzeichnis leer. Das belegt diese
Testprogramme, keine bereits ausgelieferte GUI-Binärdatei.

MSVC meldet weiterhin Warnungen in unveränderten Runtime-Quellen, darunter
Verengungen, Vorzeichenvergleich und C4701; der eigene ausgerichtete Trackingblock
erzeugt C4324 für Padding. Im regulären Linux-Build bestehen außerdem unveränderte
Beispiel-Initialisierungswarnungen. Kein warnungsfreier Windows-Gesamtbuild behauptet.

## Herkunft und Befunde

Alle **91** eingebundenen Dateien stimmen bytegenau mit den festgehaltenen
Upstream-Git-Blobs und SHA-256-Werten überein. Drei Lizenztexte sind erhalten:
Runtime-MIT, Grammatik-MIT und die Lizenz der eingebetteten ICU-Header.
[Manifest](../../third_party/c-editor-grammar.json). Vor einer GUI-Verknüpfung
müssen diese Lizenzen auch im Distributionspaket mitgeführt werden.

Die tatsächliche Parserprobe zeigte: Bei `void step(double dt){ double velocity=1;
f(` kann die normale Funktions-/Blockhierarchie in einen ERROR-Baum übergehen.
Parameter und Deklarationsknoten bleiben vorhanden. Der kommende Scope-Besucher
muss dies berücksichtigen und fremde/geschlossene/zukünftige Scopes ausschließen;
ein Besucher nur über `function_definition` genügt nicht.

Aufbau- und Prüferfehler sind getrennt erhalten: Archivlayout, POSIX-/ICU-Includes,
falsche Annahme eines immer normalen Wurzelknotens, ein verworfener Sichtbarkeits-
Pragmaansatz mit Linkfehler sowie die zu enge erste Symbolprüfung. Die positive
Phase verwendet unveränderte Upstream-Bytes und Sichtbarkeit pro Übersetzungseinheit.
Kein nativer GUI-Vorherfehler aus diesen Aufbaufehlern abgeleitet.

Snapshot `c-syntax-first`: **275** eigene und **1.214** Compiler-/Testeingaben;
alle Quell- und referenzierten Archivbytes gelesen. Laufzeitinventar 251 Windows-/
137 Linux-Dateien enthält auch unveränderte frühere Programme; es bedeutet keine
erneute Ausführung dieser Tests. Keine neuen Bilder und keine Wiederholung
geschlossener F1-/Protokoll-/Settings-Prüfungen. Neue macOS-Abnahme dieses Moduls,
GUI-Integration und physische Eingabe/IME/AT/DPI/Gesamtabnahme bleiben offen.
Der Nutzer-Mac ist ausgeschaltet und wurde nicht kontaktiert.

Quellcommit **`a25683ef`** ist in beiden Repositories gepusht. Alle **1.233**
eindeutigen erfassten Eingabedateien wurden gegen die tatsächlichen Commit-Blobs
gelesen, Vendor bytegenau, sonst nur CRLF/LF normalisiert.
[Commitabschluss](c-syntax-foundation-commit-2026-11-05.json).
Drei ersetzte direkte Testprogramme entfernt: **17.129.816 Bytes**; Ergebnisse,
aktuelle Binaries, benötigte Grammatikprobe und Quellen erhalten.
[Bereinigungsbeleg](c-syntax-foundation-cleanup-2026-11-05.json).
