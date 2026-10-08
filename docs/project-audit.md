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
| Direkte Projektbuilds (§2) | `app/build_main.c`, `tools/build.py`, `tests/test_bootstrap_build.py`; einheitlich geprüfte 28 Core-Module, zwölf native Domänenprojekte mit unabhängigen Lernorakeln und verschobene SDKs | Gegen sämtliche aktuellen Plattformen prüfen. Alte CMake-Skizzen sind durch die ausdrückliche Ergänzung zu direkten Builds abgelöst. |
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
Gesamtlauf. Der am 8. Oktober auf macOS gezählte Katalog enthält inzwischen
669 Fälle ohne Fenster, 647 ohne SDL und 81 Fensterfälle. Diese aktuelle
Katalogzählung ist kein neuer Gesamtlauf; die oben genannten alten Läufe behalten
ihren damaligen Prüfumfang.

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

Der aktuelle Abgleich enthält sechsundzwanzig implementierte Blöcke, fünf konkrete
unvollständige Blöcke und 500 weiterhin ungeprüfte Blöcke. Die acht Lernpfade
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
Reportelemente besitzen append-only Besitzer-/Index-Handles; Reader/Writer und weitere
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


## Lineare Gleichungssysteme (§7.2)

PP-0360 ist jetzt konkret der dichten C-/Physim-Lösung mit 1–32 Unbekannten
zugeordnet. Zwei gewöhnliche Zwischenüberläufe bei endlichen Lösungen sind
geschlossen; unabhängige rationale Elimination und Residuen prüfen 53 Systeme
in beiden Sprachen. [Methoden und Grenzen](numerics.md) enthalten Einheiten,
Pivottoleranz, atomare Fehler, Ausgabealias und die verbleibenden Double-Grenzen.
Die übrigen Mathematikblöcke und die gesamte Plattform-/Produktabnahme bleiben
einzeln offen. Die Originaltexte aller 531 Planblöcke sind erhalten.


## Nativer Zugang zu macOS-Controls (PP-0710)

Die belegte Screenreader-Lücke wird jetzt durch ein privates, kopierendes
Accessibility-Modell und einen tatsächlichen AppKit-Einstieg bearbeitet.
Sichtbare einfache Buttons und statische Texte besitzen native Rollen, Labels,
Rahmen und sichere Press-Aktionen. [Umfang und Grenzen](accessibility.md)
unterscheiden die ausgeführten Modell-/Native-/UI-Prüfungen von einer
praktischen Screenreader-Abnahme. Textfelder, Menüs, Regler, Editoren, Fokus
und Windows/UI Automation bleiben offen. Linux besitzt inzwischen denselben
begrenzten Einstieg über AT-SPI; ein externer PyAT-SPI-Client prüft zwei
gezeichnete Fenster, echte Buttonaktionen, Cache-/Zustandsänderungen und
ungültige Anfragen. Wayland und praktische Orca-Bedienung bleiben offen. PP-0710 bleibt unvollständig;
die Zahl implementierter Planblöcke und der vollständige Planumfang ändern sich
durch diesen Einstieg nicht.


## Nullstellensuche und Optimierungsgrundlagen (§7.2 / PP-0361)

Bisection und Golden Section sind über die öffentlichen C-/Physim-Funktionen
geprüft. Ein belegter falscher Konvergenzerfolg bei subnormalen und relativen
Rundungsgrenzen ist durch exakte binäre Toleranzvergleiche geschlossen. Die
Fraction-Gegenprobe prüft anfängliche/finale Entscheidungen, Callback-Bereiche,
Zähler und gemeinsame Berichte. [Verträge und Grenzen](numerics.md) dokumentieren
Einheiten, Stetigkeit/Unimodalität, Stagnation und Ausgabeerhaltung bei Fehlern.
Der dabei belegte Capture-Fehler für Modulblock-Closures ist ebenfalls geschlossen;
retained Schleifen-/Blockwerte werden als besitzende Snapshots geprüft.
Aktuell: 27 implementierte, fünf unvollständige und 499 ungeprüfte Planblöcke.
Der vollständige Projektplan und die Plattform-/Produktabnahme bleiben offen.


## Vektoren, Matrizen und Transformationen (§7.2 / PP-0357)

Vec2/3/4, Mat3/4, Quaternionen und Transformationen sind konkret den C-/Physim-
APIs und Referenzprüfungen zugeordnet. Die bestehende Rodrigues-, Slerp-,
Inversions- und Senkrechtstellungsprüfung wird um rationale und Decimal-
Grenzreferenzen ergänzt. Belegte Zwischenüberläufe, verlorene Auslöschungsreste
und Rundungsfehler nahe Null sind geschlossen; Normalen benötigen keine
vollständig darstellbare Inverse. [Verträge und Grenzen](math.md) halten
Konventionen, Einheiten und Konditions-/Pivotgrenzen fest.
Aktuell: 28 implementierte, fünf unvollständige und 498 ungeprüfte Planblöcke.
Die übrigen Mathematikforderungen und der gesamte Projektplan bleiben offen.


## Opake Run-Streams (§7.1 / PP-0352)

Die konkrete Lücke in öffentlichen Reader-/Writer-Zuständen wird durch opake
Store-Objekte und owner/slot/generation-Kennungen bearbeitet. Runner und Import
nutzen sie tatsächlich; alte Kopien, Fremdbesitzer, begrenzte Slots, Allocator-
Fehler und atomare Streaming-Ausgaben werden geprüft. [Vertrag und Umstieg](run-streams.md)
halten die verbleibenden Legacy-Zustände und Besitzerlebensdauer ausdrücklich fest.
PP-0352 ist nun als unvollständig erfasst: 28 implementierte, sechs unvollständige
und 497 ungeprüfte Planblöcke. Der vollständige Plan ist weiterhin offen.


## Exakte Zahlenvergleiche (§7.2 / PP-0358)

`ps_close` und Physim `isClose` prüfen nun dieselbe explizite Bedingung
`abs(a-b) <= absolute + relative*max(abs(a),abs(b))` für exakte binäre
Eingabewerte. Beide Toleranzen null verlangen exakte Gleichheit. Nichtendliche
Werte und negative/nichtendliche Toleranzen ergeben false. Die absolute
Toleranz besitzt die Einheit der Zahlen, die relative ist dimensionslos.
Ein konservativer Fehlerfilter entscheidet gewöhnliche Fälle; feste Integerarrays
bewahren Entscheidungen unmittelbar an der Grenze, auch unterhalb des kleinsten
Subnormalwertes und bei überlaufender Differenz oder relativem Produkt.

Die unabhängige Fraction-Gegenprobe findet im bisherigen Vergleich 1751 falsche
Grenzentscheidungen bei 9176 Fällen. Derselbe C-Probe prüft nun beide
Vergleichsrichtungen; 263 exakt rekonstruierbare Physim-Eingaben prüfen die
Sprachbindung. Weitere Solver- und ODE-Toleranzverträge sind separat dokumentiert.
PP-0358 ist mit diesen begrenzten Nachweisen implementiert: aktuell 29
implementierte, sechs unvollständige und 496 ungeprüfte Planblöcke.
Die Originaltexte aller 531 Blöcke bleiben erhalten; PP-0365 und die vollständige
Algorithmus-/Plattform-/Produktabnahme bleiben offen.


## Interpolation und Kurven (§7.2 / PP-0359)

Lineares/nearest/previous Resampling, monotones kubisches PCHIP, Quaternion-Slerp
und räumliche kubische Bézierkurven besitzen konkrete Referenz-/Grenzprüfungen.
Die neue rationale Kurvenprobe findet in der bisherigen Implementierung 627
abweichende Fälle bei 956 Eingaben: Zwischenrundung kann große benachbarte
Kontrollpunkte zu einer Nulltangente machen oder bei Subnormalwerten deren
Vorzeichen umkehren. Position, analytische Tangente und Unterteilungskontrollpunkte
werden nun als exakte binäre Polynome ausgewertet und einmal abschließend gerundet.
77 Physim-Fälle prüfen denselben Vertrag und kopierte Kontrollpunkte mit gültigen
sowie ungültigen Indizes. Unterteilung aliasiert weiterhin ihre Eingabe und kann
auch bei nichtdarstellbarer Tangente gelingen; Fehler erhalten Ausgaben.

PP-0359 ist mit diesen begrenzten Nachweisen implementiert: aktuell 30
implementierte, sechs unvollständige und 495 ungeprüfte Planblöcke. Einheiten,
Parameterbereich, Extrapolationsgrenzen und Fehlererhaltung sind dokumentiert;
automatische Bogenlängenparametrisierung ist nicht enthalten. Die Originaltexte
aller 531 Planblöcke bleiben erhalten; PP-0365 und die gesamte Algorithmus-,
Modell-, Plattform- und Produktabnahme bleiben offen.


## SI-Definitionen und Konvertierung (§7.3 / PP-0369, PP-0373)

Die sieben SI-Basiskonstanten, benannten abgeleiteten Einheiten und allgemeine
Dimensions-/Skalenalgebra sind konkret gegen Basisvektoren, Symbole und
abgeleitete Dimensionen geprüft. `ps_convert` konnte bisher selbst beim
Umrechnen in dieselbe Einheit Werte um ein Bit verändern: 220 von 2000
Identitätsfällen; die umfassendere rationale Gegenprobe findet 540 abweichende
Fälle bei 8214 Eingaben. Die Konvertierung rundet nun das exakte binäre Verhältnis
einmal abschließend und erhält Identitäten sowie signierte Null. 264 Physim-Fälle
prüfen denselben Core. Fehler erhalten Ausgaben; ursprüngliche Invalid-/Numeric-
Codes gelangen in die strukturierten Runtime-Diagnosen.

Dimensionsformatierung, persistente Anzeigeeinheiten, SI-Reihen, Berichts-/CSV-
Export und Parameter-Einheiten sind konkret geprüft. PP-0369 und PP-0373 sind mit
diesen begrenzten Nachweisen implementiert: aktuell 32 implementierte, sechs
unvollständige und 493 ungeprüfte Planblöcke. PP-0370/0371/0372/0375 bleiben für
ihren gesamten Umfang ungeprüft; temporäre Quantity-Darstellungen und fest
kanonische Run-Kanäle müssen von allen übrigen Datenpfaden unterschieden werden.
Die Originaltexte aller 531 Blöcke bleiben erhalten. Die vollständige
Plattform-/Modell-/Produktabnahme wird aus diesen Einzelnachweisen nicht abgeleitet.


## Typisierte SI-Kanalgrenze (§7.3 / PP-0370..0375)

`ps_channel_sample_quantity` und Physim `Channel.sampleQuantity` ergänzen einen
geprüften Pfad von externen Quantity-Darstellungen in kanonische SI-Kanäle.
Dimensionen, Kontextprefix, Index und Schema werden vor der Konvertierung
geprüft; Fehler erhalten den gesamten Kontext. Die Sprachseite prüft Besitzer
und Callbackphase und bewahrt Invalid-/Numeric-/Version-Codes.

Die echten C-/Physim-Runner-Fälle lesen einen Zentimetersensor samt absoluter
Standardunsicherheit und persistieren 1,25 m sowie 0,01 m. Unabhängige Run-/CRC-/
Schema-/CSV-Prüfung zeigt dieselben Werte auf beiden Sprachwegen; abgefangene
Dimensions-/Bereichsfehler ersetzen keinen früheren Wert. Die Sensor-Wurfbeispiele
benutzen den neuen Pfad für Wert und Unsicherheit tatsächlich. Der C-Wurf staged
Kanalupdates zusammen mit dem Modellzustand, der Physim-Wurf hält Statusmasken
und konvertierte Szenenpositionen ausdrücklich fest.

PP-0370/0371/0372/0375 bleiben für ihren gesamten Umfang ungeprüft. Externe
Quantity-/Measurement-Darstellungen, eigene Reportachsen und bereits kanonische
Mess-/Analysewerte sind nicht gleichzusetzen; rohe Zahlen bleiben ohne
prüfbare Dimension. Aktuell unverändert: 32 implementierte, sechs unvollständige
und 493 ungeprüfte Planblöcke. Die Originaltexte aller 531 Blöcke bleiben erhalten.


## Starrkörper: begrenzter Bereichsnachweis und konkrete Geometrielücke

PP-0382 bleibt für den gesamten Umfang unverified. Kugel-/Boxkonstruktoren
und Energie besitzen jetzt unabhängige rationale sowie Decimal-Referenzen,
ohne daraus eine vollständige Abnahme der Integration und Kontaktlösung
abzuleiten. Die [Mechanikverträge](mechanics.md) trennen diese Grenzen.

Die zunächst für PP-0385 erfasste Lücke ist inzwischen implementiert:
Geprüfte konvexe Dreiecksnetze besitzen Paar-, Kugel- und Ebenenkontakte,
konservative Hüllgrenzen, C-/Physim-Bindungen und explizite Masseneigenschaften.
Unabhängige SAT-/Oberflächenzeugen prüfen den diskreten Vertrag. Es handelt
sich um einzelne repräsentative Kontakte; konvexe Ruhemanifolds und persistente
konvexe Kontaktverwaltung bleiben gesondert offen. Das Audit enthält
33 implementierte, 7 unvollständige und 491 ungeprüfte Planblöcke.


PP-0387 ist jetzt konkret incomplete: Lineare konvexe Paar-/Kugel-/Ebenen-
Sweeps und Bewegungshüllen ergänzen die Kugelbasis. Unabhängige Zeitintervalle,
Distanznullstellen und Oberflächenzeugen sowie echte C-/Physim-Ereignisse mit
Restzeit sind geprüft. Konstante Weltachsenrotation und quadratische Translation sind inzwischen
implementiert. Ein atomischer Kick-Drift-Controller integriert inzwischen mehrere
Ereignisse und gleichzeitige Kontaktgruppen für Kugel/Box/Ebene/Convex in C und
Physim. Rationale 1D-Referenzen und tatsächliche Runner-Läufe prüfen ihn. Allgemeine
zeitabhängige Rotation/Kräfte fehlen weiterhin; der Gesamtumfang bleibt unvollständig.


Der begrenzte native Barrierefreiheitseinstieg besitzt jetzt zusätzlich Checkboxen:
Rollen, boolesche Werte und native Umschaltaktionen sind durch tatsächliche
AppKit-/AT-SPI- und App-Einstellungsabläufe geprüft. Die Bibliotheksauswahl besitzt
native Laufnamen trotz optisch leerer Checkboxbeschriftung. Bestehende Dock-
Aktivierung bleibt erforderlich; Text-/Editor-/Fokusdienste und praktische
Screenreader-Abnahme bleiben offen. PP-0710 bleibt ausdrücklich unvollständig.


Native Optionsgruppen ergänzen PP-0710 um Darstellung und beide Schriftgrößen.
Echte Eltern-/Kindbeziehungen, Auswahlwerte und Aktionen sind durch portable,
AppKit-/AT-SPI- und tatsächliche App-Einstellungsprüfungen belegt. Die Gruppen
trennen gleichlautende Werte, behalten eine Auswahl bei wiederholter Aktivierung
und berücksichtigen native sowie Pointer-Ereignisse. Dropdowns, Text-/Editor-
und Fokusdienste sowie praktische Screenreader-Abnahme bleiben offen; der Block
bleibt unvollständig.


Ein nativer Fokusdienst ergänzt einfache Controls um tatsächliche Tastatur-
eigentümerschaft und Aktivierung in zuvor inaktiven Dock-Bereichen. Native Zustände,
Fokusrahmen, Enter/Space/Tab/Pfeile sowie Sperren und verdeckende Fenster besitzen
gezielte Modell-/AppKit-/AT-SPI-/App-Nachweise. Text-/Editor-/Menü-/Dropdownfokus
und praktische Screenreader-Abnahme bleiben gesondert offen; PP-0710 bleibt
unvollständig.


PP-0708 besitzt nun die ausdrückliche Migration zwischen den vorhandenen
Projektformaten 1 und 2, bedienbar in App und Builder. Exakte Manifest-/Backupbytes,
Dateirechte, Wiederholung, unbekannte Versionen, Größen-/Schreibfehler und
Konflikte mit dem geöffneten Snapshot sind geprüft. Tatsächliche C-/Physim-
Builds und App-Aktionen erhalten Quellen, Archive, Cache und Messwerte.
[Plattformnachweise](platform-validation.md) nennen die ausgeführten Umgebungen
und verschobenen SDKs. Künftige Formatverträge werden damit nicht vorweggenommen.
Aktuell: 34 implementierte, 7 unvollständige und 490 ungeprüfte Planblöcke.
Alle 531 Originaltexte und der Plan-Hash bleiben erhalten; das Gesamtziel bleibt offen.


PP-0711 ist durch die ergänzte Ressourcenmessung jetzt konkret als unvollständig
erfasst. Native Referenzlasten besitzen CPU-, Lebenszeit-Peak-RAM- und logische
Dateidurchsatzmessung; der UI-Benchmark trennt Aufbau, Renderaufruf und Swap.
Vollständige App-/Runner-/Mehrworker-, Szenen-/GPU-, Startzeit- und reale
Interaktionsmessung bleiben offen. [Messvertrag](performance.md) und
[Plattformnachweise](platform-validation.md) begrenzen diese Fortschritte.
Aktuell: 34 implementierte, 8 unvollständige und 489 ungeprüfte Planblöcke.
Alle 531 Originaltexte und der vollständige Projektumfang bleiben erhalten.


PP-0711 ergänzt jetzt den produktiven 3D-Szenenpfad um Vorbereitung, Tessellierung
und Submission. Vier Referenzlasten besitzen unabhängig erwartete Geometriemengen
und wiederholte Bildvergleiche. Ein optionales OpenGL-Serverintervall trennt
Ergebnissammlung und CPU-Intervalle; deaktivierte/nicht verfügbare Messwerte sind
explizit fehlend. Vollständige App-/Runner-/Mehrworker-, Startzeit-, reale
Interaktions- und GPU-Auslastungsmessung bleiben offen. Der Block und die Zahlen
bleiben unverändert: 34 implementiert, 8 unvollständig, 489 ungeprüft.
[Vertrag](scene-rendering.md) und [Nachweise](platform-validation.md).


PP-0711 besitzt nun auch eine opt-in-Aufzeichnung tatsächlicher App-Frames mit
Phasen, Startbereitschaft ab `main`, App-CPU/Peak-RAM und empfangenen Pipebytes.
Ein asynchroner Writer mit begrenzter atomarer Queue schützt den UI-Pfad; Drops
und Schreib-/Abschlussfehler bleiben ausdrücklich unvollständig. Die realen
C-/Physim-Workflows und Reportergegenproben erweitern den Nachweis. Kindprozess-/
Mehrworkerressourcen, GPU-Auslastung, OS-Start vor `main` und tatsächliche
Eingabe-bis-Anzeige-Latenz bleiben offen. [Vertrag](app-profiling.md).
Die Gesamtzahlen bleiben unverändert: 34 implementiert, 8 unvollständig, 489 ungeprüft.


Schema 2 der App-Aufzeichnung ergänzt PP-0711 um abgeschlossene eigene Runner-/
Jobressourcen. Der Plattformdienst erfasst sie bei Reaping/Handleabschluss,
erhält sie nach Close und invalidiert sie bei Neustart. Eigene Lebenszyklus-
markierungen verhindern doppelte Records; OS-Scope und Nichtverfügbarkeit sind
explizit. Live-/gleichzeitige Baum-/Mehrworkerressourcen, GPU-Auslastung und echte
Latenz bleiben offen. [Messvertrag](app-profiling.md). Status und Zahlen bleiben
unverändert: 34 implementiert, 8 unvollständig, 489 ungeprüft.
