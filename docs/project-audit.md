# Abgleich mit dem vollständigen Projektplan

Stand: 7. Oktober 2026, Ausgangsrevision `4841ee2`. Das Ziel bleibt die Umsetzung
des [vollständigen Projektplans](../Physim_Projektplan.md). Ein grüner Teiltest,
ein vorhandener Header oder eine konfigurierte CI beweist keine Produktabnahme.

Die maschinenlesbare Erfassung in `docs/project-audit.json` bewahrt 531 Planblöcke,
darunter alle 475 Aufzählungspunkte, mit Originaltext, Zeilenbereich, Abschnitt
und SHA-256 des Plans. Auch langfristige Ziele, Nicht-Ziele und illustrative
Entwürfe bleiben enthalten. `implemented` bedeutet vorhandene Umsetzung mit
begrenzten Nachweisen; es bedeutet nicht `verified` für den gesamten Umfang.
`unverified` bezeichnet fehlende Einzelprüfung, `incomplete` eine konkrete Lücke.
Der Großteil des Plans ist noch nicht einzeln abgenommen. Spätere Domänen sind
keine erledigten Anforderungen; Nicht-Ziele werden nicht als zusätzliche
Implementierungsaufträge behandelt. Die konkrete Umsetzung darf den ursprünglichen
Umfang nicht ersetzen.

## Erste belegte Befunde

| Planbereich | Aktuelle Grundlage | Noch erforderlicher Nachweis oder Arbeit |
| --- | --- | --- |
| Plattformen und Pakete (§2, Phase 10) | Ausgeführte Intel-macOS-/Debian-Builds, verschobene SDKs und historische CI-Nachweise in [Plattformprüfung](platform-validation.md) | Aktuelle vollständige Windows-/Apple-Silicon-/Intel-/Linux-Matrix; frische Systeme. Signierung/Notarisierung bleibt an Entwicklerzugänge gebunden. |
| Direkte Projektbuilds (§2) | `app/build_main.c`, `tools/build.py`, `tests/test_bootstrap_build.py`; echte C-/Physim-Analyseprojekte und verschobene SDKs | Gegen sämtliche aktuellen Plattformen prüfen. Alte CMake-Skizzen sind durch die ausdrückliche Ergänzung zu direkten Builds abgelöst. |
| Kern und Numerik (§7) | Öffentliche GUI-unabhängige Header, Referenz-, Konvergenz- und Fehlertests | Jede explizite Algorithmusforderung und ihr dokumentiertes Fehler-/Einheitenverhalten einzeln zuordnen; keine Ableitung aus bloßer Funktionszahl. |
| Sprache und Bindungen (§10, LANG-001..007) | Version 0.180.0, Lexer/Parser/Checker/C17-Backend, typisierte Werte, Module, Generics, Runner- und Analysebindungen | Vollständigen semantischen Vertrag und C-/Physim-Funktionsparität prüfen. `LANG-005` besitzt jetzt explizite Batch-Hostdienste und besitzende Sprachwerte mit Start, Pause, Wiederaufnahme, Archivabfragen, Statistik, Diagrammen und Export. Die vollständige Sprach-/Produktabnahme und neue Remote-Matrix bleiben offen. |
| Barrierefreiheit (Phase 10, PP-0710) | Themes, Code-Schriftgröße, Tastenkürzel; `app/preferences.h`, `app/settings_ui.inc` | UI-weite Schriftvergrößerung, vollständige Tastaturführung und grundlegender Screenreader-Zugang. |
| Dokumentation (§16, LANG-007) | Acht gekoppelte Lernpfade mit vollständigen Quellen, Modellannahmen und automatisierten Prüfungen; `tests/tutorial_sources.json` | Die beiden separat navigierbaren Wege sind implementiert; aktuelle gesamte Bindungs-/Plattformabnahme und alle Vorlagen bleiben gesondert offen. |
| Weitere Domänen (§7.7, Phase 12+) | Mechanik-, Material-/Medien-, Mess- und Analysebasis | Thermodynamik besitzt nun ein SI-Modul und vollständige C-/Physim-Beispiele für ideales Gas und Wärmefluss; reale Gase bleiben offen. Elektromagnetismus besitzt nun einen geprüften SI-Einstieg für Ladungen, Felder und RC-Schaltungen. Wellen/Optik und Strömung fehlen weiterhin als eigenständige Domänen. Die empfohlene Reihenfolge bleibt erhalten. |

## Aktuelle CI-Fehler zuerst schließen

Die [C17-CI für `4841ee2`](https://github.com/PhysicSimulator/physim/actions/runs/37546883578)
enthält fehlgeschlagene Linux-/macOS-Jobs. Die abgelegten Artefakte wurden per
SHA-256 gegen die im GitHub-Lauf veröffentlichten Digests geprüft. Ihre vollständigen
Testberichte zeigen 563 bestandene Fälle unter Linux/GCC und macOS/Apple Silicon;
Linux/Clang scheitert am Build von `logging`. Die Fehler der absichtlich fehlschlagenden
Test-Runner-Selbstprüfung sind Testdaten und keine Produktfehler.

Drei konkrete Ursachen wurden lokal nachvollzogen:

1. **Unvollständiges Prüfkit:** Die bisherige Linux-Paket-CI archiviert nur sechs
   Dateien. `verify-native-sdk.py` benötigt inzwischen zusätzliche unabhängige
   Prüfer und Header, zunächst `tools/sdk_series_probe.c`. Das identisch erzeugte
   Kit scheitert vor dem eigentlichen SDK-Test mit `FileNotFoundError`.
2. **Fehlende Beispieleingaben:** `test_language_examples.py` startet
   `run_index_values`, ohne `index α.psrun` und seine beiden Varianten anzulegen.
   Das Programm meldet I/O-Fehler. Die SDK-Prüfung hatte diese Dateien vorbereitet;
   deshalb blieben die fehlenden Eingaben bei den bisherigen Teilnachweisen verborgen.
3. **Header-Reihenfolge bei Linux/Clang:** Der Sprachsupport verwendet die GNU-
   Erweiterung `strtod_l`, deren Deklaration nach bereits eingebundenen libc-Headern
   fehlt. Ein strenger C17-Compile von `test_logging.c` reproduziert dies unabhängig
   vom restlichen Build. Die Linux-Implementierung verwendet nun `strtod` mit
   vorübergehender POSIX-Thread-Locale und stellt die aufrufende Locale wieder her.

Der neue `tools/package-verification-kit.py` erfasst die expliziten
Repository-Dateiabhängigkeiten des SDK-Prüfers und lokale Testheader, ergänzt die
Dialogprüfer und erzeugt ein ausschließliches Archiv mit SHA-256-Manifest.
Fehlende oder dynamisch berechnete Repository-Pfade brechen die Paketierung ab.
Die Physim-Implementierung und SDK-Header bleiben außerhalb des Kits; sie müssen
vom installierten SDK kommen. `tests/test_verification_kit.py` prüft Integrität,
Grenzen, unveränderte vorhandene Archive und Bereinigung bei Schreibfehlern.
Die Prüfung eines aus dem Kit entpackten SDK-Prüfers läuft ohne Repositoryquellen.

## Beweisumfang festhalten

Am unveränderten Ausgangsstand `4841ee2` bestehen die vollständigen lokalen
Release-Suiten mit **563/563** auf dem Intel-Mac unter
`build/contact-world-language-release-mac/test-results/run-7kin6e77` und Debian/GCC
unter `build/contact-world-language-release-linux/test-results/run-dbd70ucn`.
Das ist ein Nachweis für diesen Stand und diese Umgebungen. Die anschließenden
Änderungen an Laufzeitsupport und Prüfern benötigen eigene Nachweise; eine
nachträgliche Änderung der Zahl macht den alten Lauf nicht zu einem aktuellen
Gesamtlauf. Der aktuelle Katalog enthält mit den zusätzlichen Lernwegprüfungen
589 Fälle ohne Fenster, 574 ohne SDL und weiterhin 66 Fensterfälle.

Die Abnahme bleibt offen, bis sämtliche konkreten Anforderungen passende
aktuelle Implementierungs-, Laufzeit- und Plattformnachweise besitzen.

## Erneute Prüfung der Änderungen

Die vollständigen Release-Läufe mit der Localekorrektur und der zusätzlichen
Kitprüfung bestehen mit **564/564**: Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-ty_ic4b4`, Debian/GCC
unter `build/contact-world-language-release-linux/test-results/run-1_8_4_wp`.
Linux/Clang Debug besteht die vier gezielten Prüffälle unter
`build/project-audit-clang-linux/test-results/run-daitiwoc`; die drei
Laufzeitprüfungen bestehen mit ASan/UBSan unter
`build/spring-tutorial-asan-linux/test-results/run-cg8grlij`.
Das belegt diese Änderungen auf diesen Systemen. Neue vollständige
Windows-/Apple-Silicon-/Intel-CI-Ergebnisse bleiben davon unabhängig erforderlich.

Die frischen SDKs mit allen Änderungen bestehen die vollständige isolierte
Kitprüfung auf macOS und Linux, unter Linux auch sämtliche neun grafischen
Projektabläufe. Ihre Pfade und Manifestprüfungen stehen in der
[Plattformprüfung](platform-validation.md). Die nächste Remote-CI muss die
Korrekturen zusätzlich in ihrer tatsächlichen Matrix bestätigen.

## Batch-Bindung und Controller

Der aktuelle Abgleich enthält dreizehn implementierte Blöcke, sechs konkrete
unvollständige Blöcke und 512 weiterhin ungeprüfte Blöcke. Die acht Lernpfade
bleiben erhalten; hinzu kommen LANG-005, der Batch-/Parametercontroller und die beiden Dokumentationsteile. Der Elektromagnetismus-Einstieg ergänzt die erste weitere Domäne.
Die Originaltexte aller 531 Blöcke und 475 Aufzählungspunkte bleiben unverändert.
Die Sprachbindung und die beiden Lernwege besitzen ausdrücklich begrenzte Implementierungs- und
Laufzeitnachweise in [Batch](batch-language.md) und der [Plattformprüfung](platform-validation.md).
Dies ersetzt keine vollständige Plattform-, Dokumentations- oder Produktabnahme.
