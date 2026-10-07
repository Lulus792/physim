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
| Direkte Projektbuilds (§2) | `app/build_main.c`, `tools/build.py`, `tests/test_bootstrap_build.py`; einheitlich geprüfte 27 Core-Module, zwölf native Domänenprojekte mit unabhängigen Lernorakeln und verschobene SDKs | Gegen sämtliche aktuellen Plattformen prüfen. Alte CMake-Skizzen sind durch die ausdrückliche Ergänzung zu direkten Builds abgelöst. |
| Kern und Numerik (§7) | Öffentliche GUI-unabhängige Header, Referenz-, Konvergenz- und Fehlertests | Jede explizite Algorithmusforderung und ihr dokumentiertes Fehler-/Einheitenverhalten einzeln zuordnen; keine Ableitung aus bloßer Funktionszahl. |
| Sprache und Bindungen (§10, LANG-001..007) | Version 0.182.0, Lexer/Parser/Checker/C17-Backend, typisierte Werte, Module, Generics, Runner- und Analysebindungen | Vollständigen semantischen Vertrag und C-/Physim-Funktionsparität prüfen. `LANG-005` besitzt jetzt explizite Batch-Hostdienste und besitzende Sprachwerte mit Start, Pause, Wiederaufnahme, Archivabfragen, Statistik, Diagrammen und Export. Die vollständige Sprach-/Produktabnahme und neue Remote-Matrix bleiben offen. |
| Barrierefreiheit (Phase 10, PP-0710) | Themes, Code-Schriftgröße, Tastenkürzel und vollständige Hauptmenüführung per F10/Pfeilen/Enter sowie Einstellungsgruppen, Handbuch und Projekterstellung per Tab/Pfeilen/Enter mit sichtbarem, automatisch gescrolltem Fokus; `app/preferences.h`, `app/settings_ui.inc`, `app/toolbar_ui.inc` | UI-Schriftgröße ist unabhängig einstellbar; vollständige Tastaturführung und grundlegender Screenreader-Zugang bleiben offen. |
| Dokumentation (§16, LANG-007) | Acht gekoppelte Lernpfade mit vollständigen Quellen, Modellannahmen und automatisierten Prüfungen; `tests/tutorial_sources.json` | Die beiden separat navigierbaren Wege sind implementiert; aktuelle gesamte Bindungs-/Plattformabnahme und alle Vorlagen bleiben gesondert offen. |
| Weitere Domänen (§7.7, Phase 12+) | Mechanik-, Material-/Medien-, Mess- und Analysebasis | Thermodynamik besitzt nun ein SI-Modul und vollständige C-/Physim-Beispiele für ideales Gas und Wärmefluss; ein homogenes Van-der-Waals-Modell ergänzt reale Gase; Phasengleichgewichte bleiben offen. Elektromagnetismus besitzt nun einen geprüften SI-Einstieg für Ladungen, Felder und RC-Schaltungen. Wellen/Optik besitzt nun Oszillator, 1D-Gitterausbreitung und geometrische Strahlen-/Linsenfunktionen. Strömung besitzt jetzt laminare Rohre, passive Drucknetze und periodischen Tracertransport; spätere ernsthafte Fluidmodelle bleiben offen. Die empfohlene Reihenfolge bleibt erhalten. |

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
621 Fälle ohne Fenster, 601 ohne SDL und 77 Fensterfälle.

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

Der aktuelle Abgleich enthält fünfundzwanzig implementierte Blöcke, fünf konkrete
unvollständige Blöcke und 501 weiterhin ungeprüfte Blöcke. Die acht Lernpfade
bleiben erhalten; hinzu kommen LANG-005, der Batch-/Parametercontroller und die beiden Dokumentationsteile. Elektromagnetismus und Wellen/Optik besitzen zusätzliche begrenzte Domänennachweise.
Die Originaltexte aller 531 Blöcke und 475 Aufzählungspunkte bleiben unverändert.
Die Sprachbindung und die beiden Lernwege besitzen ausdrücklich begrenzte Implementierungs- und
Laufzeitnachweise in [Batch](batch-language.md) und der [Plattformprüfung](platform-validation.md).
Dies ersetzt keine vollständige Plattform-, Dokumentations- oder Produktabnahme.


## Numerische Integration und Differentiation (§7.2)

PP-0363 ist jetzt einzeln der Array- und Series-API zugeordnet. Sekanten und
Trapezintegration besitzen Referenz- und Konvergenzprüfungen, explizite Einheiten,
Messlücken und transaktionale Fehlerpfade. Die gemeinsame skalierte Rechnung
schließt eine Restlücke zu CR-007 auch in den C-/Physim-Series-Operationen.
[Methoden und Grenzen](numerics.md) dokumentieren insbesondere ungleiche
Messabstände und Rauschverstärkung. Dieser Implementierungsnachweis ersetzt
keine aktuelle Gesamtplattformabnahme oder eine allgemeine Fehlerschranke.


## Einzelabgleich der Basis (§7.1)

PP-0348/0349/0350/0351/0353 sind jetzt konkreten APIs, Quellen, Fehlerverträgen und
Referenzprüfungen zugeordnet: feste Integer-/Ergebniswerte, explizite Allocatoren,
alle vier Container, explizite Logger und RNG-Zustände. Die unabhängigen RNG-Probes
prüfen vier vollständige 64-Bit-Seeds in C und Physim, geordnete Normalziehungen,
verbrauchsfreie degenerierte Verteilungen, Snapshots und unabhängige Wertkopien.
[Mess- und RNG-Vertrag](measurement.md) unterscheidet exakt prüfbare Integerzustände
von plattformabhängiger libm-Rundung.

PP-0352 bleibt ungeprüft für seinen gesamten Umfang. Series/Dataset und
Reportelemente besitzen geprüfte generationale Handles; Reader/Writer und weitere
Deskriptoren enthalten weiterhin öffentliche Zustandsfelder. Einzelne sichere
Handle-APIs beweisen nicht automatisch die allgemeine Anforderung. Auch die
vollständige Allocator-Anbindung sämtlicher Subsysteme und die aktuelle gesamte
Plattform-/Produktabnahme bleiben offen. Die Plantexte aller 531 Blöcke bleiben
erhalten.


## Material- und Medieneigenschaften (§7.5)

PP-0391/0392/0394/0396 besitzen jetzt einen konkreten Eigenschaftsbaustein mit
SI-Einheiten, Name/Quelle, geschlossenem Gültigkeitsbereich und konstanten oder
begrenzt bilinear interpolierten Tabellen. C leiht diese Daten; eigene
Physim-Wertstrukturen besitzen kopierbare Arrays und Texte. Beide realen
Experimentsprachen speichern vollständige Modelldaten und erzeugen mit beiden
Analysesprachen geprüfte Diagramme. [Verträge und vollständige Quellen](properties.md)
unterscheiden synthetische Referenzdaten von gemessenen Stoffdaten.

PP-0393 bleibt für die gesamte Liste der physikalischen Größen und Stoffmodelle
ungeprüft. Ein allgemeiner Quantity-Datentyp allein ersetzt deren fachliche
Einzelabnahme nicht. Neue API-/Laufzeitnachweise ersetzen weiterhin weder die
Gesamtplattformabnahme noch die übrigen Plananforderungen.


## Homogene reale Gaszustände (§7.7)

PP-0412 bleibt unvollständig, besitzt aber zusätzlich ein explizit begrenztes
Van-der-Waals-Modell. C und Physim berechnen Druck, Druckableitung, Energie und
Entropiedifferenz mit denselben molaren SI-Koeffizienten. Ein synthetisches
isothermes Vergleichsexperiment speichert sieben Kanäle und seine Koeffizienten;
alle vier gemischten Analysen werden unabhängig geprüft.
[Modell, Quellen und Grenzen](real-gas.md) schließen Phasenauswahl,
Maxwell-Konstruktion und experimentelle Stoffkalibrierung aus. Der ideale
Grenzfall und mechanisch instabile homogene Algebra sind ausdrücklich getestet.
Die Zahl implementierter Planblöcke bleibt unverändert; diese Ergänzung ist
keine gesamte thermodynamische oder Plattformabnahme.


## Gemeinsame SI-Grenze für Messkanäle (§7.3)

Der Einzelabgleich von PP-0372 deckt eine Lücke der C-Kanaldeklaration auf:
Nichtkanonische Einheitenskalen wurden angenommen und beim Speichern des
Schemas verworfen. C und Physim verwenden jetzt denselben Validator mit Skala 1,
begrenzten UTF-8-Texten, eindeutigen Namen und atomaren Fehlern. Ein unabhängiger
Laufdatei-/CSV-Prüfer bestätigt die ausdrückliche Umrechnung 125 cm → 1,25 m
in beiden Sprachen. [Kanalvertrag](numerics.md) beschreibt Herkunft, Grenzen
und die Trennung von SI-Werten und Anzeigeeinheiten.

PP-0372 und die weiteren Einheitenblöcke bleiben für ihre gesamte Forderung
ungeprüft. Ein grüner Kanaltest beweist weder jede Analyseoperation noch die
korrekte physikalische Bedeutung nackter Zahlen. Historische Rohschemata
behalten ihren Dateiformatvertrag. Die Anzahl implementierter Blöcke bleibt
unverändert.


## Quantity-Summen und numerischer Bereich (§7.3)

Der weitere Abgleich belegt eine numerische Bereichsgrenze: Eine separat
überlaufende Konvertierung verhinderte eine endliche Quantity-Summe.
Normierte und kompensierte C-Summen erhalten jetzt auch sehr kleine Beiträge;
subnormale Ergebnisse werden erst auf ihrem endgültigen Gitter gerundet.
Physim benutzt dieselben Funktionen. Unabhängige exakte Fraction-Referenzen
prüfen Bereich, Werte, linke Einheit und unveränderte Fehlerausgaben.
[Methoden und Grenzen](numerics.md) unterscheiden die Erweiterung vom weiterhin
begrenzten Double-Vertrag. PP-0372 bleibt für sämtliche Grenzen ungeprüft;
dieser Nachweis ersetzt die gesamte Einheitenabnahme nicht.


PP-0370 ist durch einen tatsächlichen Series-Lauf jetzt als unvollständig belegt:
Die API speichert 50 cm roh mit Skala 0,01 und addiert diese Werte zu 1 m als
51 m. Die korrekte Summe wäre 1,5 m. Dieser Gegenbeleg wird als nächster
Analyse-/SI-Speicherfehler bearbeitet. Er bleibt ausdrücklich offen; ein grüner
Quantity-Teiltest macht die Series-Operation nicht korrekt.


## Kanonische SI-Series (§7.3)

Der oben belegte Series-Gegenbefund ist nun korrigiert. Eigene/ausgerichtete
Reihen übernehmen deklarierte Eingabeeinheiten und speichern blockweise SI mit
Skala 1. Addition und abgeleitete Analysen verwenden dieselbe Grundlage.
C und Physim erzeugen unabhängig geprüfte CSV-Spalten und Berichte; Range-
und Quota-Fehler erhalten Daten und Handles. [Vertrag und Umstellung](series.md)
erklären insbesondere die geänderte Bedeutung von value()/unitScale().

PP-0370 kehrt nach Schließen dieses konkreten Gegenbefunds zu `unverified`
zurück: Alle übrigen internen Speicherschichten sind für den gesamten Planumfang
noch nicht einzeln abgenommen. Die vollständige Einheiten-/Plattformabnahme
bleibt offen. Aktuell 25 implementierte, fünf unvollständige und 501 ungeprüfte
Blöcke; frühere Gegenbefunde und tatsächliche Nachweise bleiben erhalten.


## ODE-Methoden und gewichtete Zustände (§7.2)

PP-0362 ist jetzt den fünf tatsächlich implementierten C-/Physim-Methoden,
Konventionen und Referenz-/Konvergenz-/Fehlerprüfungen zugeordnet. Ein neuer
Gegenbeleg zeigt unnötigen RK4-Zwischenüberlauf bei einer endlichen konstanten
Lösung. Gemeinsame skalierte Arithmetik korrigiert diese Produkt-/Summengrenze
auch für Euler, symplektischen Euler, Verlet und RK45-Stufen/Fehlerschätzungen.
244 unabhängige konstante Lösungen ergänzen die bestehenden nichtkonstanten
Oszillator- und Pendelprüfungen. [Einheiten und Grenzen](numerics.md) unterscheiden
lokale Fehlerschätzung von globaler Genauigkeit und nichtsteife von steifen
Problemen. Dieser Implementierungsnachweis ersetzt keine ganze Plattformabnahme.

Aktuell 25 implementierte, fünf unvollständige und 501 ungeprüfte Blöcke.
Weitere Mathematikforderungen bleiben einzeln zu prüfen; sie werden nicht allein
wegen dieses ODE-Nachweises als erfüllt markiert.
