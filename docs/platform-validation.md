# Plattformprüfung

Stand: 12. Oktober 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

Die [historischen Plattformnachweise](platform-validation-history.md) enthalten
die früheren lokalen/CI-/Paketprüfungen einschließlich der ersten Linux-
AT-SPI- und skalaren Suchnachweise. Ihr vollständiger Text und die Belegpfade bleiben erhalten.
Die folgenden Abschnitte dokumentieren die neueren tatsächlich ausgeführten
Prüfungen. Alle drei Seiten sind im Offline-Handbuch erreichbar; ältere Belege
stehen auch in [Archivband 2](platform-validation-history-2.md).

## Linux-AT-SPI und asynchrone UI-Aktionen (PP-0710)

Der vollständige historische Nachweis mit allen Testpfaden bleibt im
[Plattformarchiv](platform-validation-history.md#linux-at-spi-und-asynchrone-ui-aktionen-pp-0710) erhalten.

## Skalare Suche, exakte Toleranzen und Modulblock-Closures (PP-0361)

Der vollständige historische Nachweis mit allen Testpfaden bleibt im
[Plattformarchiv](platform-validation-history.md#skalare-suche-exakte-toleranzen-und-modulblock-closures-pp-0361) erhalten.

## Transformationsbereiche und Mathematikzuordnung (PP-0357)

Der vollständige historische Nachweis mit allen Testpfaden bleibt im
[Plattformarchiv](platform-validation-history.md#transformationsbereiche-und-mathematikzuordnung-pp-0357) erhalten.

## Opake Run-Streams am 8. Oktober 2026

Der vollständige historische Nachweis mit allen Testpfaden bleibt im
[Plattformarchiv](platform-validation-history.md#opake-run-streams-am-8-oktober-2026) erhalten.

## Exakte Toleranzvergleiche am 8. Oktober 2026

Der vollständige historische Nachweis mit allen Testpfaden bleibt im
[Plattformarchiv](platform-validation-history.md#exakte-toleranzvergleiche-am-8-oktober-2026) erhalten.

## Kurvenauswertung am 8. Oktober 2026

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#kurvenauswertung-am-8-oktober-2026) erhalten.

## Einheitenkonvertierung am 8. Oktober 2026

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#einheitenkonvertierung-am-8-oktober-2026) erhalten.

## Quantity-Werte an der SI-Kanalgrenze am 8. Oktober 2026

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#quantity-werte-an-der-si-kanalgrenze-am-8-oktober-2026) erhalten.

## Starrkörperträgheit und Energiebereich am 8. Oktober 2026

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md) erhalten.

## Konvexe Narrow Phase am 8. Oktober 2026

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#konvexe-narrow-phase-am-8-oktober-2026) erhalten.

## Lineare konvexe CCD am 8. Oktober 2026

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#lineare-konvexe-ccd-am-8-oktober-2026) erhalten.

## Konservative rotierende und quadratische CCD am 8. Oktober 2026

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#konservative-rotierende-und-quadratische-ccd-am-8-oktober-2026) erhalten.

## Kontinuierlicher Mehrkörperschritt und erneute Review-Regressionen

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#kontinuierlicher-mehrkörperschritt-und-erneute-review-regressionen) erhalten.

## Native Checkboxen und tatsächlicher Einstellungsentwurf

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#native-checkboxen-und-tatsächlicher-einstellungsentwurf) erhalten.

## Native benannte Optionsgruppen

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#native-benannte-optionsgruppen) erhalten.

## Nativer Tastaturfokus für einfache Controls

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#nativer-tastaturfokus-für-einfache-controls) erhalten.

## Projektmigration der vorhandenen Formate 1 → 2

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#projektmigration-der-vorhandenen-formate-1--2) erhalten.

## Ressourcenmessung für native Daten- und UI-Referenzlasten

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#ressourcenmessung-für-native-daten--und-ui-referenzlasten) erhalten.

## Produktiver Szenenpfad und optionale OpenGL-Serverzeit

Der vollständige historische Nachweis bleibt im
[Plattformarchiv](platform-validation-history.md#produktiver-szenenpfad-und-optionale-opengl-serverzeit) erhalten.

## Opt-in-Aufzeichnung tatsächlicher App-Abläufe

Der vollständige Nachweis bleibt in
[Archivband 2](platform-validation-history-2.md#opt-in-aufzeichnung-tatsächlicher-app-abläufe) erhalten.

## Ressourcen abgeschlossener eigener Runner-/Jobprozesse

Der private Prozessdienst cached Benutzer-/System-CPU und Peakbytes beim
Reaping beziehungsweise vor dem Handleclose. POSIX verwendet pid-spezifisches
`wait4`, Windows Prozesszeiten/Working-Set-Abrechnung. Keine spätere PID-Suche
oder kumulative App-`RUSAGE_CHILDREN` wird verwendet. Getter erhalten Outputs
bei laufenden/fehlenden/ungültigen Daten; Close erhält den Cache und Neustart
invalidiert ihn. Kill und shutdownbedingtes Close erfassen ebenfalls Abschlüsse.
Die POSIX-/Windows-Scopes bleiben ausdrücklich verschieden und Peaks sind keine
gleichzeitige Baumsumme. [Vertrag](app-profiling.md#ressourcen-abgeschlossener-eigener-prozesse).

Schema 2 schreibt Frame- und Prozessrecords über dieselbe atomare Queue in
getrennte CSVs, mit getrennten geschriebenen/ausgelassenen Zählern. Ein neuer
Lebenszyklus setzt die App-Markierung zurück, sodass normales Polling und späterer
Shutdown keine doppelten Records erzeugen. Sequenzen unterscheiden spätere
PID-Wiederverwendung. Fehlende Ressourcen bleiben leer/null. Schema 1 bleibt
mit ausschließlich seinen Framewerten lesbar. Die Ressourcen stammen aus
abgeschlossenen eigenen Experiment-, Build-, Parameterbeschreibungs-, Analyse-
und Exportprozessen; intern verwaltete Batchworker sind nicht vollständig erfasst.

Die native Gegenprobe lässt zwei eigene Kinder 32/64 MiB berühren und CPU
verbrauchen. Selbstauskünfte werden gegen Finalwerte verglichen. Ein zunächst
blockiertes zweites Kind endet erst nach dem ersten; dessen Cache muss unverändert
bleiben. Wiederholte Getter, Getter nach Close, fehlgeschlagener Neustart, Kill
und Close eines laufenden Kindes sind geprüft. Queuegegenproben vergleichen
verfügbare und ausdrücklich fehlende Prozessfelder im tatsächlichen Writer.

Intel macOS 14.6.1/Apple Clang 16 besteht drei Ressourcen-/Kind-/Queuefälle 3/3
(`run-lv294fub`), Debian 12/GCC 12.2 dieselben 3/3 (`run-hphxtiy1`). Die initiale
macOS-Gegenprobe mit bestehender Parent-Watch-Prüfung besteht 3/3 (`run-hzs5uuwn`).
Die App-Abläufe für C und vollständiges Physim mit echter Kompilierung, Runnern,
Analyse und Bibliothek protokollieren verfügbare Experiment-/Builder-/Analysewerte
sowie den erwarteten Compilerfehler. Sie bestehen zunächst auf beiden Systemen
1/1 (`run-jm8d9a02`, `run-6eytro6m`). Die erweiterte macOS-Gegenprobe besteht
1/1 (`run-v7j_1bra`): NaN-Prozesszeiten, abgeschnittene Prozess-CSV und erfundene
Werte bei Nichtverfügbarkeit werden abgewiesen; tatsächliche Nichtverfügbarkeit
und Schema 1 bleiben korrekt auswertbar.

Linux/Clang 14 mit ASan/UBSan besteht alle vier Ressourcen-/Kind-/Queue-/
Parent-Watch-Fälle 4/4 (`run-rsvz7fng`) und den erweiterten App-Prüfer 1/1
(`run-f9beqjjt`). `detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen bleiben
aktiv. Diese Läufe sind keine ThreadSanitizer-Abnahme. macOS-Sanitizer bleiben
wegen fehlendem `ld64.lld` ungeprüft; Windows/Apple Silicon wurden nicht ausgeführt.

Zur Freigabe des Linux-Prüfraums wurden 504 Dateien der bereits bestandenen
älteren Läufe `run-rmvt1uw_` und `run-9_tqrbcn` in
`build/child-profiling-previous-results.tar.gz` archiviert. Alle Datei-/Symlink-
mitglieder wurden gegen Originale und alle Originalhashes vor Entfernen erneut
geprüft. `build/child-profiling-previous-results.json` enthält den vollständigen
Wiederherstellungsnachweis. Es wurden nur diese archivierten Testoriginale entfernt.

PP-0711 bleibt für Live-/gleichzeitige Prozessbaum-/Mehrworkerressourcen,
GPU-Auslastung, OS-Start vor `main` und echte Eingabe-bis-Anzeige-Latenz
unvollständig. Die 531 Originalplanblöcke und das gesamte Ziel bleiben erhalten.

Die abschließenden sieben Ressourcen-/Kind-/Queue-/Parent-Watch-/Handbuch-/
Referenz-/Prüfpaketfälle bestehen auf macOS 7/7 (`run-uhxr6m61`) und Linux
7/7 (`run-hxwfq40o`). Der erweiterte Linux-App-Prüfer besteht außerdem
1/1 (`run-ha6zki2c`). Die getestete Implementierung und Testeingaben bleiben
bei diesem Dokumentationsnachtrag unverändert;
`build/child-profiling-final-code-freeze.json` erfasst ihren Quellstand.

## CI-Testkatalog, Compilerkodierung und Linux-Prüfabhängigkeit

Die tatsächlichen Ergebnisse von [C17-Lauf 37773257771](https://github.com/PhysicSimulator/physim/actions/runs/37773257771)
am Stand `0678f6057f6b7f1d68f98585595f4a2b1f36331b` zeigen 675/675 bestandene
native Fälle unter Linux/Clang und macOS/Apple Silicon. Der anschließende
Test-Runner-Selbsttest scheiterte am veralteten erwarteten Displaykatalog; dieser
Fehler wurde auf dem lokalen Intel-Mac reproduziert. Der Katalog erfasst nun
auch Migration, App-Profiling und die plattformspezifischen Checkbox-/Options-/
Fokusabläufe und prüft weiterhin exakte Fallmengen, Skripte und benötigte Argumente.

Windows/MSVC Debug bestand 674/675 Fälle. Der Migrationstest scheiterte beim
UTF-8-Decodieren der weitergereichten OEM-Compilerausgabe. Der Prüfer erhält
jetzt Originalbytes und prüft ASCII-Marker als Bytes; die bestehende Kontrolle
von Quellen, Backup, Cache und Messwerten bleibt erhalten. Der Integrationsfall
mischt zusätzlich absichtlich ungültige UTF-8-Diagnosebytes in echte
Builderausgaben. Dies prüft die Kodierungsgrenze auch auf macOS und Linux.

Die drei heruntergeladenen Artefakte wurden vor Auswertung gegen die SHA-256-
Digests der offiziellen GitHub-Artefaktmetadaten geprüft:

- `sdk-windows-v143-Debug`: `d3623863966e4b3901cca3e578cc0302f4f8793512dbe158d52a4c89e061eaad`
- `linux-ui-clang`: `65cdada87d8e0b02dd2f22797034840c6c5da7ffd4a145c8471232b65ae442b6`
- `macos-macos-15`: `30a967415fac215d408659be2076dfa6b0b9098cc01af06397854d14b597720c`

Im [Linux-Paketlauf 37762643835](https://github.com/PhysicSimulator/physim/actions/runs/37762643835)
erreichte das Debian-12-Prüfpaket alle neun GUI-Projektabläufe sowie Handbuch,
Migration und Einstellungen, scheiterte dann aber beim Import von `dbus` im
nativen Checkboxprüfer. Das Debian-Artefakt wurde gegen den offiziellen Digest
`d6a8a980863bf206519c365dcd014da14141391deb588b8a0ab87708161f8360` geprüft.
Beide Linux-Workflows installieren `python3-dbus` jetzt ausdrücklich. Im
Paketworkflow geschieht dies weiterhin erst nach der Startprüfung ohne Python.

Intel macOS 14.6.1/Apple Clang 16 und Debian 12/GCC 12.2 bestehen jeweils den
Runner-Selbsttest und die direkten Bootstrap-Build-Gegenproben. Migration mit
eingemischten OEM-Bytes und Prüfpaket bestehen jeweils 2/2 (`run-j9l4p4oh`,
`run-ocqcofxc`). Auf Debian bestehen außerdem die drei tatsächlichen nativen
Checkbox-/Options-/Fokusabläufe mit dem bereits vorhandenen verlagerten SDK
und unabhängigen Prüfpaket (`build/ci-dbus-sdk-checks`). Dieser letzte Lauf
prüft die Abhängigkeit am bestehenden SDK, kein neu erzeugtes Gesamtpaket.

Die Korrekturen sind damit lokal auf beiden Systemen geprüft. Eine vollständige
erneute CI-Abnahme einschließlich Windows, Apple Silicon und frisch installierter
Debian-/Ubuntu-Pakete ist noch nicht belegt. Der gesamte Projektplan bleibt offen.

## Kraftvektoren des vertikalen Pendelablaufs

Die C-Pendelvorlage, alle fünf Sprachvarianten und beide Lernpfadexperimente
zeigen Gewicht und radiale Stangenkraft mit identischen Farben und einem
beschrifteten Maßstab von 0,05 m/N. Das C-Modell ergänzt bei eingeschaltetem
Luftwiderstand dessen tangentiale Kraft. Stabile IDs und Elternbeziehungen
erhalten Auswahl und Sichtbarkeit. Die Kräfte verändern weder Modellzustand
noch Messkanäle; der vorhandene Geschwindigkeitspfeil behält seinen Maßstab.
[Modellgleichungen](pendulum-tutorial.md#verfahren-auswählen).

`pendulum_forces` lädt neun tatsächliche C-/Physim-Module, einschließlich eines
C-Moduls mit Luftdichte 1,225 kg/m³. Drei Längen/Anfangswinkel, je 401 Szenen
und Reset prüfen Kraftsumme gegen eine unabhängige Newton-Gegenrechnung,
Gewichtsrichtung, dissipativen Widerstand, Pfeilursprung, Einheitenbeschriftung,
Farben und Hierarchie. Die bestehende Integratorprüfung vergleicht weiterhin
alle Szenenfelder und Messdaten; sie umfasst jetzt alle zwölf Lernpfadobjekte.

Intel macOS 14.6.1/Apple Clang 16 besteht zunächst Kraft-/Sprach-/ID-Prüfung
3/3 (`run-987n24li`), den vollständigen Pendellernpfad 1/1 (`run-glzge7id`)
und abschließend Kraft-/Quellen-/Prüfpaketfälle 3/3 (`run-9qil7a7b`).
Handbuch-/Quellengleichheit bestehen 2/2 (`run-mmp19n79`); der Runner-Selbsttest
besteht ebenfalls. Beide C-/Physim-Lernpfadfenster bestehen gemeinsam 1/1
(`run-r9zx8x6x`), die tatsächliche Sprachvorlage mit Build, Runner und Auswertung
1/1 (`run-giqtz_jk`). Ihre Lernpfadbilder wurden nebeneinander visuell geprüft.

Debian 12/GCC 12.2 besteht alle fünf Kraft-/Sprach-/ID-/Lernpfad-/Quellenfälle
5/5 (`run-c_ebe3wq`) und beide Fensterabläufe 2/2 (`run-rzh8hhwe`).
Linux/Clang 14 mit ASan/UBSan besteht die neun Module umfassende Kraftprüfung
1/1 (`run-4i8nz9ms`), mit `detect_leaks=0` und aktiver Address-/UB-Prüfung.
Windows und Apple Silicon wurden für diese Kraftänderung noch nicht ausgeführt.

Der erste Linux-Build scheiterte an vollem Datenträger und zählt nicht als
Nachweis. Zum Freigeben wurden 13.638 Mitglieder des älteren, vollständig
bestandenen 78-Fälle-Laufs `run-38r48_zv` auf dem Mac archiviert. Alle
Archivmitglieder und anschließend alle Originale wurden per SHA-256 verglichen,
erst dann wurde ausschließlich dieser Linux-Testordner entfernt. Die kanonische
Kopie liegt auf dem Mac unter `build/pendulum-forces-archived-linux-proofs.tar.gz`,
SHA-256 `96aad9d1b4fbcf8db3c32e7eed36ae9bbdbfb134a11797dae48fa5d655eae346`;
`build/pendulum-forces-archived-linux-receipt.json` enthält Pfade und Einzelhashes.
Projektquellen und Benutzerdaten wurden nicht entfernt.

PP-0776 besitzt damit einen konkreten begrenzten Nachweis für Pendel,
Kraftvektoren und Live-Werte. Die übrigen Anforderungen des vollständigen
vertikalen Anwendungsfalls und des Gesamtplans bleiben separat abzunehmen.

## CPU-Prüflast unter Windows/MSVC

Der [C17-Lauf 37777889619](https://github.com/PhysicSimulator/physim/actions/runs/37777889619)
am Stand `c229e359c94f70082e56b028448f24d39db7f1b7` liefert unter MSVC Debug
675/676 bestandene native Fälle. Die ergänzten Diagnosewerte des verbleibenden
`process_usage`-Fehlers zeigen jeweils 0 s Benutzer-/System-CPU vor und nach
der Last, aber einen korrekt gestiegenen Peak von 3.162.112 auf 36.728.832 Bytes
bei 33.554.432 berührten Bytes. Das heruntergeladene Artefakt wurde gegen den
offiziellen SHA-256-Digest
`791bb51edb33e5e694109ae94b21616a93dcc82a147114aab61bb80ad45505ba` geprüft.

Der Test beendete die Last nach 40 ms Wandzeit, ohne dass die OS-Abrechnung
CPU-Zeit veröffentlicht hatte. Er arbeitet jetzt bis mindestens 40 ms
zusätzliche Prozess-CPU erfasst sind, höchstens 20 s Wandzeit. Endliche,
monotone Benutzer-/Systemwerte werden bei jeder Abfrage geprüft. Die
Speicheruntergrenze, Lebenszeit-Peak-Erhaltung und Prüfung nach Freigabe bleiben
erhalten. Die API-Implementierung wird durch diese Testkorrektur nicht geändert.
[Prozesszeiten](https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-getprocesstimes)
beschreiben CPU-Abrechnung; sie sind keine Wandzeituhr.

Die Korrektur besteht auf Intel macOS/Apple Clang 16 1/1 (`run-zhos1g1r`)
und Debian 12/GCC 12.2 1/1 (`run-zed_431v`). Die erneute tatsächliche
Windows-Abnahme dieser Korrektur steht noch aus.

## Pendelanalyse als Physim-Projektvorlage

Neue Pendelprojekte mit Physim-Auswertung kopieren die bereits gepflegte
`examples/documentation/pendulum_analysis.phys`. Das gilt für C- und Physim-
Experimente. Die Analyse lädt die gespeicherten Läufe, zeigt Winkel und
Energieabweichung, berechnet positive Nulldurchgangsperioden und exportiert die
vollständigen CSV-Spalten. Kurze oder ruhende Läufe erhalten keinen erfundenen
Periodenwert. Vorhandene Projekte werden nicht umgeschrieben. Andere Modelle
und unabhängige Analyseprojekte behalten ihre eigene Vorlagenauswahl.

Die tatsächlichen `language_full_workflow`-/`language_mixed_workflow`-Fenster
prüfen kopierte Quellen, Build, Runner, gespeicherte Quelldateien, Bericht,
Energie-Kurvenlänge, Stichprobenzahl und Periodentabelle. Die Tastaturgegenprobe
baut, startet und analysiert alle 32 Vorlagen-/Sprachpaare und beide unabhängigen
Analysen bei 16 und 22 px, insgesamt 68 Projekte je Host. Alle drei Fälle
bestehen auf Intel macOS 14.6.1/Apple Clang 16 3/3 (`run-dgrqglsx`) und
Debian 12/GCC 12.2 3/3 (`run-ybh1ocsn`). Das tatsächlich geöffnete
Physim-Analysefenster wurde anhand seiner Aufnahme visuell geprüft.

`pendulum_template_analysis` prüft außerdem die echten Standardexperimente:
4000 Schritte des C-Pendels und des Physim-Pendels sowie zehn Schritte eines
ruhenden Modells. Die Physim-Auswertung und ein unabhängiger C-Berichtsprüfer
vergleichen die ursprünglichen Läufe mit Kurven, Energieabweichung, SI-Einheiten
und Periodentabelle. Zusammen mit Quellengleichheit und Prüfpaket bestehen
die drei nativen Fälle 3/3 auf macOS (`run-xl8ful2s`) und Linux
(`run-sn4h2119`). Linux/Clang 14 besteht den neuen Integrationsfall mit
ASan/UBSan 1/1 (`run-l6a9ae5i`, `detect_leaks=0`). Ein zuvor parallel
gestarteter macOS-Build wurde vom Buildlock abgewiesen und zählt nicht als
erfolgreicher Test; der erfolgreiche native Lauf verwendet einen eigenen Buildordner.

Die aktualisierte `check_modules`-Funktion des SDK-Prüfers wurde mit den
tatsächlichen Runner-/Analysemodulen des bereits vorhandenen installierten
macOS-SDK ausgeführt (`build/pendulum-analysis-sdk-branch-mac/results.json`).
Dies belegt den geänderten Zweig, keine neue vollständige SDK-Abnahme. Die
Prüfpaket-Gegenproben bestehen auf beiden Hosts mit unverändert 140 manifestierten
Eingaben. Windows/Apple Silicon und frisch erzeugte Gesamtpakete dieser
Vorlagenänderung bleiben separat zu prüfen. Der Gesamtplan bleibt offen.

## Bestätigte Windows-Ressourcenprüfung

Der [C17-Lauf 37780045308](https://github.com/PhysicSimulator/physim/actions/runs/37780045308)
am Stand `9e85dfde8af9f99a811b296fb5c17a733c735c22` besteht den vollständigen
MSVC-Debug-Job. Das Ergebnisartefakt enthält 676/676 bestandene native Fälle
(`run-ntop68h6`) und 10/10 gezielte AddressSanitizer-Fälle (`run-cdjpb81e`).
Der zuvor fehlgeschlagene Prozessressourcentest meldet jetzt 0,046875 s Benutzer-
CPU und einen Peak von 36.712.448 Bytes bei 33.554.432 berührten Bytes. Das
Artefakt wurde gegen den offiziellen SHA-256-Digest
`009a6f884d2c8a6a2e460ee235ec583e088cae51e88e36a3570031c1edd3cbf4` geprüft.
Die absichtlich fehlschlagenden Runner-Selbsttestfixtures bleiben Testdaten.
Diese Abnahme gilt für diesen Stand; sie beweist keine spätere Änderung oder
vollständige Gleichheit der noch laufenden gesamten Plattformmatrix.

## Energieplot und vollständiger CSV im C-Pendelablauf

Die C-Standardanalyse ergänzt für Läufe mit Winkel und Energie ein Diagramm
`E - E(0)` mit Zeit in Sekunden und Energieänderung in Joule. Die bestehende
Bilanzdarstellung für Modelle mit `energy.balance` bleibt erhalten. Ein
gesonderter `-energy.csv` enthält alle ursprünglichen Zeit-/Energiewerte und
die Änderung, unabhängig von der reduzierten Berichtsvorschau. Im konservativen
Pendel dient die Änderung als Energiefehler; im Widerstandsmodell enthält sie
auch physikalische Dissipation. Es entsteht keine Dämpfungsabschätzung aus
diesem Plot allein.

Der erweiterte `pendulum_template_analysis` prüft Standard-C- und Standard-
Physim-Läufe durch beide Analysemodule sowie einen ruhenden Lauf. Ein
unabhängiger C-Prüfer vergleicht den neuen Plot, SI-Achsen und sämtliche
Energie-/Periodenmetriken mit den ursprünglichen Daten. Ein unabhängiger
PSRUN-/CRC-/CSV-Decoder vergleicht jede exportierte Zeile, darunter sämtliche
6001 Werte des längeren C-Laufs. Die abschließenden nativen Fälle bestehen
auf Intel macOS/Apple Clang 16 1/1 (`run-oghr8d4n`) und Debian 12/GCC 12.2
1/1 (`run-mdlhi87f`). Linux/Clang 14 mit ASan/UBSan besteht die Gegenprobe
1/1 (`run-31o70csc`, `detect_leaks=0`); anschließend wurde nur das sichtbare
Minuszeichen der Beschriftung geändert, keine Rechnung oder Speicherlogik.

`pendulum_c_energy_workflow` erstellt tatsächliche C- und Physim-Pendelprojekte
mit C-Auswertung, führt je mindestens 20 s simulierte Zeit aus und prüft Build,
Runner, Energietabelle, bekannte nichtlineare Referenzperiode des C-Modells,
Plot, vollständigen CSV, gespeicherten Bericht und Wiederöffnen aus der
Bibliothek. SVG- und PNG-Exporte werden über echte Buttons gestartet. Der
Prüfer kontrolliert PNG-CRC, Kodierung, Abmessungen und sämtliche Scanlines
sowie den SVG-Titel. Beide Abläufe bestehen gemeinsam auf macOS 1/1
(`run-ehmtvfaw`) und Linux 1/1 (`run-8me4chhy`). App-Aufnahme und PNG wurden
visuell geprüft. Ein fehlender Test-Elternordner wurde korrigiert; die davor
gescheiterten Läufe sind keine erfolgreichen Nachweise. Die Bildprüfung führte
zur lesbaren ASCII-Beschriftung `E - E(0)`; die finalen Fensterläufe prüfen
diese Fassung. Der native Runner-Selbsttest besteht mit dem neuen Displayfall.

PP-0667, PP-0779 und PP-0780 besitzen damit konkrete begrenzte Nachweise für
gespeicherte Pendeldaten, Energie-/Periodenanalyse, Plot/Tabelle und Exporte.
Eine vollständige Dämpfungsanalyse und die übrigen Anforderungen des vertikalen
Anwendungsfalls bleiben getrennt offen. Windows/Apple Silicon dieser neuen
C-Energieplotänderung wurden noch nicht ausgeführt; der zuvor bestätigte
Windows-Job ist kein Nachweis für den späteren Quellstand.

## Einstellbares Medium und Winkelsensorrauschen des Pendels

Die allgemeine C-Pendelvorlage und alle fünf Physim-Varianten besitzen dieselben
sieben SI-Parameter: Länge, Anfangswinkel, Masse, Mediumdichte, konstanter
Widerstandskoeffizient, Querschnitt und Rauschstandardabweichung. Instanzwerte
ersetzen ausschließlich die bisherigen festen Defaults; Vakuum und ausgeschaltetes
Rauschen bleiben Standard. Die gemeinsame `ps_medium`-/`Medium`-Drag-API wirkt
entgegen der Tangentialgeschwindigkeit. Kraftpfeile berücksichtigen die ausgewählte
Masse und zeigen aktiven Widerstand. Positive Dichte, Koeffizient und Querschnitt
werden einzeln geprüft; ein unterlaufendes Produkt kann Velocity Verlet keinen
geschwindigkeitsabhängigen Term vortäuschen. Ein exakt ausgeschalteter Term ist
auch bei positiver Dichte zulässig.

Der wahre Winkel bleibt von `sensor.angle` getrennt. Gaußrauschen wird nur bei
positiver Standardabweichung gemessen, aus dem Laufseed wiederholbar zurückgesetzt
und beeinflusst keine Integration. Die C-Vorlage übernimmt Zustand, Messwerte
und Zufallszustand erst nach erfolgreichen endlichen Berechnungen. Die gepflegten
Pendelanalysen nennen die mechanische Änderung nun `Energy change`; tatsächliche
Dissipation wird damit nicht als ausschließlich numerischer Drift bezeichnet.
[Parameter und Modellannahmen](workspace.md).

`pendulum_medium` vergleicht neun tatsächliche Module in fünf C-/Physim-Paaren,
je 5001 Messungen mit ausgewählter Masse, Medium und Rauschen. Die Gegenprobe
prüft sieben Dimensionen, Kraftsummen gegen Newtons tangentiale/radiale Rechnung,
Energieverlust, Rauschmittelwert/-standardabweichung, Reset, getrennte Instanzen,
ungültige Masse und Verlet-Grenzen. Tiny-positive Eingaben werden bei Verlet
abgewiesen; exakt null Widerstandskoeffizient bleibt erlaubt. NaN und sehr große
Schritte erhalten Messwerte und Zufallszustand bei Fehlern.

Intel macOS 14.6.1/Apple Clang 16 und Debian 12/GCC 12.2 bestehen alle sechs
Medium-/Sprach-/Kraft-/Standardanalyse-/Lernpfad-/Quellenfälle 6/6
(`run-1ul01yjy`, `run-f14lhjrd`). Die erweiterten Fehlergrenzen bestehen danach
auf macOS 1/1 (`run-zn_wzl4q`) und Linux/Clang 14 mit ASan/UBSan 1/1
(`run-hqir8mmd`, `detect_leaks=0`). Die vorherige Sanitizergegenprobe besteht
ebenfalls 1/1 (`run-nq90ooq9`). Der Runner-Selbsttest besteht mit dem neuen Fensterfall.

Die tatsächlichen Fensterabläufe bauen C-/Physim-Projekte, wählen Masse 2 kg,
Dichte 1,225 kg/m3, Koeffizient 0,8, Querschnitt 0,08 m2 und Rauschen 0,02 rad,
führen mindestens 6 s aus und prüfen gespeicherte Parameter/Einheiten, Kräfte,
Energieabnahme und Analyseberichte. Zusammen mit den bestehenden Parameterstudien
(einschließlich cm-Eingabe) und der Standard-Sprachvorlage bestehen sie auf
macOS 3/3 (`run-f2d7tdvv`) und Linux 3/3 (`run-sfh67nn3`). Die tatsächliche
Physim-Szene mit Widerstandskraft wurde visuell geprüft. Eine fehlende Test-
Headerdeklaration und ein ungültiger doppelter Override im Prüfer wurden korrigiert;
die zuvor fehlgeschlagenen Läufe zählen nicht als erfolgreiche Nachweise.

PP-0765/0766 besitzen jetzt begrenzte Nachweise für das Mediumkonzept und
Winkelsensorrauschen. PP-0756 bleibt als gesamtes erstes Experiment unvollständig:
insbesondere eine vollständige Dämpfungsabschätzung und die übrige vertikale
Abnahme bleiben offen. Diese Änderung wurde noch nicht unter Windows oder
Apple Silicon ausgeführt. Alle 531 Originalplanblöcke bleiben erhalten.

## Beobachtete Amplitudenabnahme gespeicherter Pendelläufe

`ps_series_positive_peaks` und `Series.positivePeaks` liefern gemeinsam
ausgerichtete Zeit-/Amplituden-/Segmentreihen. Ein Anstieg mit folgendem Abfall
erkennt eine positive Spitze; flache Spitzen erscheinen einmal in ihrer Zeitmitte.
Randwerte und konstante Segmente bleiben ausgeschlossen. Fehlende Signal-/Zeit-
zeilen trennen Segmente. Die Zeit muss über gültige Zeilen streng steigen.
Blockweise zwei Durchläufe benötigen 24 Scratchbytes je Spitze; Fehler erhalten
Ausgaben und Quota. Die Ausgaben besitzen eine neue Samplezuordnung und können
die Eingabehandles des Aufrufers überschreiben. API-/Bibliotheksreferenzen sind
regeneriert und geprüft.

Die C-Standardauswertung und beide Lernpfadanalysemodule berechnen aus zwei
geeigneten Spitzen δ = ln(A₁) − ln(A₂) und r = δ/(t₂−t₁). Nur Spitzen desselben
Segments werden verbunden. Eine Tabelle pro Bericht fasst Intervallzahl,
mittleres Dekrement und Mittelwert/Spannweite der Raten zusammen. Volle Spitzen-
und Intervall-CSVs bleiben unabhängig von Tabellen- und Plotgrenzen erhalten.
Wachstum besitzt negative Raten; unzureichende Spitzen liefern keine Zeile.
Es werden beobachtete Werte ohne Spitzeninterpolation, Rauschfilter oder
Anpassung eines konstanten viskosen Dämpfungsmodells ausgewiesen.
[Herleitung und Grenzen](pendulum-tutorial.md#beobachtete-amplitudenabnahme).

Die native Gegenprobe prüft flache Spitzen, Blockgrenzen, fehlende Segmente,
strenge Zeit, leere Ergebnisse, Eingabe-/Ausgabealias und Quota. Die tatsächliche
Physim-Analysebindung besitzt dieselben bekannten Spitzen- und Maskenwerte.
Sechs synthetische Dateien erzeugen bekannte exponentielle Amplitudenhüllen,
Wachstum, unregelmäßige Zeiten, flache Spitzen, Ruhe und nur eine Spitze. Drei
reale Analysemodule erzeugen daraus 18 unabhängig geprüfte Berichte. Je 400
Spitzen und 399 Intervalle übersteigen die 256 Tabellenzeilen; alle CSV-Zeiten,
Amplituden und signierten Raten werden trotzdem vollständig verglichen.
Diese Dateien sind ausdrücklich synthetische Signale, keine physikalischen Pendel.

Intel macOS 14.6.1/Apple Clang 16 besteht die abschließenden sechs Spitzen-/
Bindungs-/Referenz-/Standardanalyse-/Lernpfad-/Quellenfälle 6/6 (`run-kwnuc3yy`),
Debian 12/GCC 12.2 dieselben 6/6 (`run-si18cisp`). Linux/Clang 14 mit ASan/UBSan
besteht Spitzen, Bindung und die 18 Referenzberichte 3/3 (`run-xb4hqrcy`,
`detect_leaks=0`). Ein zunächst veralteter Handbuch-Codeblock wurde mit der
getesteten Quelle abgeglichen; der davor fehlgeschlagene Quellenfall gilt nicht
als erfolgreicher Nachweis. Der native Runner-Selbsttest besteht ebenfalls.

Die tatsächlichen Standard-Sprach-, Vakuumlernpfad- und Mediumfenster bestehen
auf macOS 3/3 (`run-1zs7qsy_`) und Linux 3/3 (`run-6cgec3wr`). C- und Physim-
Mediumprojekte zeigen nach mindestens sechs Sekunden positive beobachtete
Abnahmeraten in ihrer dritten Tabelle; die tatsächlich geöffnete Physim-Tabelle
wurde visuell geprüft. Fehlende Dämpfung in Ruhe-/Kurzläufen bleibt ausdrücklich
von einer gemessenen Rate null verschieden. Windows/Apple Silicon und neue
Gesamtpakete dieser API-/Analyseänderung wurden noch nicht ausgeführt.

PP-0767 besitzt damit einen begrenzten Nachweis für Periodendauer, beobachtete
Amplitudenabnahme und Energieabweichung. Dies ist keine vollständige Abnahme
des gesamten vertikalen Anwendungsfalls oder des Gesamtplans.

## Direkte lineare Pendelgeschwindigkeit und Berichtsgegenprobe

Die allgemeine C-Vorlage, alle fünf Physim-Pendelvorlagen sowie beide
Lernpfadmodelle speichern nun `velocity.x`, `velocity.y` und `speed` als
kanonische SI-Kanäle in `m/s`. Die ursprünglichen sechs Kanalindizes bleiben
erhalten; neun Kanäle werden ohne Format-/ABI-Änderung geschrieben. Komponenten
folgen der Ableitung der Kreisbahn, der Betrag ist `L |ω|`. Messwerte stammen
vom akzeptierten Modellzustand, nicht von einer Ableitung des gespeicherten
Sensorsignals. C prüft sämtliche neun Werte vor dem Commit; fehlgeschlagene
Schritte erhalten auch die neuen Kanäle und den RNG-Zustand.

`pendulum_medium` vergleicht neun Module in fünf C-/Physim-Paaren über je 5001
Zustände. Kanalnamen und SI-Dimensionen, Vektorbetrag, Orthogonalität zur Stange
und Energie aus der linearen Geschwindigkeit werden unabhängig geprüft,
zusammen mit Mediumkräften, Rauschen, Reset und Fehlergrenzen. Der Lernpfadtest
prüft die neuen Werte in allen gespeicherten Zeilen, einschließlich fünf
Integratoren, geänderter Länge/Amplitude, Ruhe und adaptiver Zeiten. Seine
PSRUN-/Szenen-Gegenprobe bleibt unabhängig vom Bibliotheksreader. Der
Sprachintegrationstest prüft zusätzliche echte Runnerdateien.

Intel macOS 14.6.1/Apple Clang 16 besteht fünf native Fälle 5/5
(`run-c624o33a`); Debian 12/GCC 12.2 dieselben 5/5 (`run-7kzt1qd5`).
Linux/Clang 14 mit ASan/UBSan besteht Lernpfad und Medium 2/2
(`run-wjjohn89`, `detect_leaks=0`). Die tatsächlichen Standard-Sprach-, Lernpfad-
und Mediumfenster bestehen macOS 3/3 (`run-cabirlw8`) und Linux 3/3
(`run-5s3oovhd`). Alle 14 geänderten Implementierungs-/Testeingaben wurden
zwischen beiden Testsystemen per SHA-256 abgeglichen.

Der offizielle Linux-Paketlauf `37790113179` am vorherigen Stand `4ac9028`
endete mit 677/678 bestandenen nativen Fällen. Das Artefakt
`linux-release-build-tests` wurde vor dem Lesen gegen den offiziellen Digest
`e0cdfa246f5b5a2d2851ceb001af94bd8e80aa8625fb83167d4e6331bd76ce50`
geprüft. `derived_reference` erwartete noch drei Plots/zwei Tabellen, obwohl
die Energieänderung und die neue Amplitudenabnahme den Bericht auf vier
Plots/drei Tabellen erweitert haben. Die Gegenprobe prüft jetzt zusätzlich
Energieeinheit, vollständige Kurvenquellen und kleine Vakuumenergieabweichung
sowie Titel, Intervalle, Rate und Einheit der Abnahmetabelle. Sie besteht
macOS 1/1 (`run-y7_q8fd7`) und Linux 1/1 (`run-cxzh78ag`). Ein zwischenzeitlicher
zweiter Linux-Build wurde von der aktiven Buildsperre abgewiesen und erst nach
Abschluss des ersten Builds erneut erfolgreich ausgeführt; dies ist kein
zusätzlicher erfolgreicher Testlauf.

PP-0763 ist damit im beschriebenen Umfang nachgewiesen. PP-0756 und die
vollständige Plattform-/Paketabnahme bleiben offen; Windows und Apple Silicon
wurden für diese Änderung nicht ausgeführt. Der neue Gesamtpaketlauf ist noch
kein bestätigter Nachweis.

## Laufparameter für allgemeine Pendel-Integratoren

Die C-Pendelvorlage und alle fünf Physim-Pendelvorlagen besitzen jetzt einen
zusätzlichen dimensionslosen Parameter `integrator` mit ganzzahligem Bereich
0..4: Euler, symplektischer Euler, RK4, Velocity Verlet, Dormand–Prince 5(4).
Die bisherigen Standardverfahren bleiben erhalten; `PS_PENDULUM_METHOD`
bestimmt weiterhin den C-Standard. Die gewählte Methode gehört zur jeweiligen
Instanz und erscheint in deren Laufmetadaten. Nichtganzzahlige Werte und
Werte außerhalb des Bereichs werden abgewiesen. Verlet mit positiven Werten
für Dichte, Widerstandskoeffizient und Fläche bleibt auch bei unterlaufendem
Produkt unzulässig. Die adaptive Laufsteuerung verwendet weiterhin getrennt
Dormand–Prince; feste Integratorwahl und adaptive Methode sind beide dokumentiert.

Die Medium-Gegenprobe prüft jede der fünf Physim-Vorlagen gegen dasselbe
C-Modell mit allen fünf ausgewählten Verfahren, Vakuum und zulässigem Medium
über 500 Schritte je Konfiguration. Euler und symplektischer Euler besitzen
zusätzlich eine unabhängige erste Schrittformel. Namen der gewählten Methode,
alle neun Messwerte, reproduzierbares Rauschen und Reset werden verglichen.
Gleichzeitig lebende Instanzen verwenden verschiedene Integratoren und
Parameter; ein Euler-Schritt der zweiten Instanz erhält die erste unverändert.
Negative, zu große und nichtganzzahlige Auswahlen sowie die Verletgrenzen
werden für jede Vorlage provoziert.

Intel macOS 14.6.1/Apple Clang 16 besteht die abschließenden vier Fälle
`language_experiment`, `derived_reference`, `pendulum_template_analysis` und
`pendulum_medium` 4/4 (`run-0w_kykim`). Debian 12/GCC 12.2 besteht dieselben
4/4 (`run-4mgo3rcq`). Linux/Clang 14 ASan/UBSan besteht die vollständige
Medium-/Methodengegenprobe 1/1 (`run-1nmkd17f`, `detect_leaks=0`).
Eine zunächst fest gebliebene RK45-Metadatenangabe wurde vom neuen Test
entdeckt und korrigiert; davor fehlgeschlagene Läufe gelten nicht als Nachweis.
Die neun geänderten Implementierungs-/Testeingaben stimmen zwischen macOS
und Linux per SHA-256 überein.

Die tatsächlichen App-Workflows bestehen macOS 2/2 (`run-5aoagl1r`) und
Linux 2/2 (`run-djqqn6dr`). Das C-/Physim-Mediumfenster wählt `integrator=2`
im Parameterformular, speichert die Auswahl und prüft sie zusammen mit der
RK4-Metadatenangabe, Messkanälen, Kräften und Auswertung. Die adaptive
Parameterstudie bleibt für beide Sprachen einschließlich cm-Eingaben und
Wiederöffnung erfolgreich. Windows/Apple Silicon und neue vollständige
SDK-/App-Pakete wurden für diese Änderung nicht ausgeführt.

PP-0764 und PP-0773 besitzen damit begrenzte konkrete Nachweise. PP-0756,
aktuelle vollständige Plattform-/Paketabnahme und Gesamtplan bleiben offen.

## Verschobene aktuelle SDK-Pendelketten

Am Implementierungsstand `a0592d5` wurden neue Release-SDKs ohne Oberfläche
mit allen kompilierten C-/Physim-Beispielen tatsächlich gebaut: Intel macOS
14.6.1/Apple Clang 16 und Debian 12/GCC 12.2. Die aktuelle Pendelgegenprobe
liegt im unabhängigen Kit und wird nun auch vom vollständigen SDK-Prüfer
aufgerufen. `--pendulum-only` führt denselben Teil isoliert aus. Der alte
adaptive SDK-Probe erwartete sechs Kanäle; er prüft jetzt neun Kanäle,
Geschwindigkeitsnamen/-einheiten und Kinematik. Allgemeine Sprachberichte
behalten ihre eigenen zwei Plots ohne Tabellen; sie werden ausdrücklich
nicht mit den gesonderten Pendelberichten verwechselt.

Das Kit umfasst 146 explizite Prüfeingaben, kein Physim-Core und keine
öffentlichen SDK-Header. Seine vollständigen Bytes wurden beim Entpacken
gegen das enthaltene SHA-256-Manifest geprüft. Der Kit-Selbsttest besteht
mit Grenzen, fehlenden/dynamischen Pfaden und exklusiver Veröffentlichung.
Die Ausführung erfolgte ausschließlich mit dem aus dem Kit entpackten
Prüfer; Compilerquellen und SDK-Header stammen aus dem verschobenen Paket.
Das Paketmanifest wird vor dem ersten Compile vollständig geprüft.

Je System werden installierte und aus Paketquellen neu gebaute Bibliothek
getrennt geprüft. Sechs allgemeine Modelle werden mit fünf ausgewählten
Verfahren, Vakuum und zulässigem Medium ausgeführt. Ein unabhängiger
PSRUN-Decoder prüft CRCs, Footer, Metadaten, neun Kanalnamen, SI-Dimensionen,
Kinematik, Tangentialbedingung und Energie. Euler/symplektischer Euler
besitzen eigene erste Schrittformeln. Unzulässige Auswahlwerte und Verlet
mit Luftwiderstand scheitern erwartungsgemäß. Je Bibliothek werden außerdem
zwölf adaptive C-/Physim-Berichte gegen den aktualisierten SDK-Probe geprüft.

Beide Lernpfadmodelle durchlaufen je Bibliothek die vollständige bestehende
Gegenprobe: 40010 primäre Messzeilen, fünf Integratoren, Szenen, unabhängige
nichtlineare Perioden-/Taylorreferenzen, Verfeinerung, adaptive Zeiten und
beide gemischten Analysesprachen. Drei Analysemodule erzeugen jeweils 18
Referenzberichte mit 400 Spitzen, vollständigen CSVs, Wachstum, ungleichen
Zeiten, Plateaus, Ruhe und nur einer Spitze. Positive-Spitzen-API und reale
Sprachbindung werden ebenfalls gegen jede Bibliothek ausgeführt.

Die abschließenden verschobenen SDK-Prüfungen bestehen vollständig:

- macOS: `build/pendulum-sdk-kit-final-proof-mac/Native SDK ä u1wizah6`.
- Linux: `build/pendulum-sdk-kit-final-proof-linux/Native SDK ä wr6n9hd4`.

Beide besitzen `PASSED.txt` und vollständige `verification.log`. Die getrennten
C-Lernpfadquellen im Consumer wurden zusätzlich gegen die Paketmanifest-Hashes
geprüft. Ein vorheriger macOS-Verifierentwurf verwendete kollidierende
Dateinamen für C- und generierte Physim-Lernpfadquellen; dessen Lauf gilt daher
nicht als C-/Physim-SDK-Nachweis. Die abschließenden Läufe verwenden getrennte
Dateien und unabhängige echte C-/Physim-Module.

Die Pakete sind Headless-SDKs; sie enthalten keine App oder Projektbuilder.
Dies ist keine vollständige neue SDK-/GUI- oder Plattformmatrixabnahme.
Windows und Apple Silicon wurden für diese Gegenprobe nicht ausgeführt.
Der ältere offizielle Linux-Paketlauf `37780798990` am Stand `66ea2bf` besitzt
zusätzlich tatsächlich erfolgreiche Paket-, Debian-12- und Ubuntu-24.04-Jobs;
er ist ausdrücklich kein Nachweis für die späteren Pendeländerungen.
PP-0756 bleibt unvollständig, und die unveränderten 531 Originalplanblöcke
bleiben Grundlage der weiteren Umsetzung.

## Tatsächliche Medium-Pendelexporte aus der App

Der vorhandene C-/Physim-Mediumworkflow endet jetzt nach den tatsächlich
betätigten Exportknöpfen für Abnahmetabelle als CSV, Energiediagramm als SVG
und PNG sowie sämtliche Rohmessdaten als CSV. Die App behält den bisherigen
Exportpfad; lediglich die Bounds des Rohdatenknopfes werden für die tatsächliche
Eingabegegenprobe erfasst. Berichtansicht, Projektbuild, Parameterwahl, isolierter
Runner, Simulation, finalisierte Daten und beide Analysesprachen liegen in
jeweils derselben tatsächlichen Projektkette.

`tests/check_pendulum_exports.py` dekodiert die Originaldatei unabhängig vom
Physim-Reader. Er prüft CRCs, Footer, alle neun Kanalnamen/Dimensionen, Lauf-
und Parameterdaten, Kinematik, Energie und Sensorrauschstatistik. Die über den
Appknopf exportierte Rohdaten-CSV stimmt in jeder Originalzeile und Spalte
exakt überein. Vollständige Energie-/Änderungs-CSVs, lokale positive Spitzen,
alle Dekremente/Raten und sämtliche Werte der exportierten Zusammenfassung
werden ebenfalls unabhängig verglichen. Die gespeicherte Datei bleibt während
der Gegenprobe per SHA-256 unverändert.

Die manuell über die App angeforderten SVGs werden als XML gelesen und auf
Energietitel/Einheit geprüft. Die PNGs in der tatsächlichen Standardgröße
2400 × 1700 besitzen geprüfte Chunk-CRCs, IEND, vollständige dekodierte RGB-
Scanlines und Bildinhalt. Ein tatsächliches C-Energie-PNG wurde visuell geprüft:
Titel, Zeit-/Jouleachsen, abnehmende Energie und Quellenzeilenzahl sind lesbar.
Die Darstellung bezeichnet ihre reduzierten Berichtsdaten ausdrücklich;
volle Originaldaten stehen in den getrennten CSVs.

Intel macOS 14.6.1/Apple Clang 16 besteht `pendulum_medium_workflow` 1/1
(`run-1apm3hbv`) mit tatsächlichem C- und Physim-Projekt. Debian 12/GCC 12.2
besteht denselben Workflow 1/1 (`run-41feacy4`). Die abschließende unabhängige
Gegenprobe einschließlich der danach ergänzten vollständigen Energie-CSV-
Vergleiche wurde gegen alle vier tatsächlichen Projekte ausgeführt:

- macOS C: 3070 originale Rohdaten-/Energiezeilen.
- macOS Physim: 3091 originale Rohdaten-/Energiezeilen.
- Linux C: 3157 originale Rohdaten-/Energiezeilen.
- Linux Physim: 3035 originale Rohdaten-/Energiezeilen.

Alle vier besitzen zwei echte positive Spitzen und ein positives beobachtetes
Abnahmeintervall. Unterschiedliche Zeilenzahlen stammen vom interaktiv
angeforderten Stop nach mindestens sechs Sekunden; die Prüfung vergleicht
jedes Projekt gegen seine vollständige eigene Zeitachse. Die fünf geänderten
Implementierungs-/Prüfeingaben stimmen zwischen beiden Systemen per SHA-256
überein. Zunächst zählte der neue Prüfer auch ein automatisch von der C-Analyse
geschriebenes SVG mit; die Gegenprobe wurde auf die tatsächlichen `-diagramm`
Buttonexporte korrigiert. Die zunächst fehlgeschlagenen Gesamtläufe werden
nicht als erfolgreiche Nachweise gezählt.

PP-0777 ist damit konkret nachgewiesen; PP-0779/0780 besitzen zusätzliche
Medium-/Exportnachweise. Windows/Apple Silicon und neue gesamte Pakete wurden
für diese Änderung nicht ausgeführt. Die gesamte vertikale Plattformabnahme
und der Gesamtprojektplan bleiben offen.

## SDK-Studiengate und aktuelle Plattformnachweise

Der offizielle C17-Lauf `37800124412` am Stand `7fea5f2` besteht unter
Linux/GCC und Linux/Clang jeweils alle 681 nativen Fälle (`run-fy9xavxm`,
`run-frebudc9`). Beide nachfolgenden SDK-Prüfungen scheitern konkret am
unveränderten `tools/sdk_series_probe.c`: Die tatsächlich erzeugte adaptive
Pendelstudie besitzt neun Kanäle; der Probe erwartet noch sechs. Die Archive
`linux-ui-gcc` und `linux-ui-clang` wurden vor dem Lesen gegen die offiziellen
SHA-256-Digests geprüft:

- GCC: `ea9b8847ad07bc902591ce8b4e8c9022ed0738ae5e8b4c2fea84c4f5c6297578`.
- Clang: `9cb55d724356106927041ea911cafef248c87bba985c0b8ded1cc80806655301`.

Die Studiengegenprobe erwartet jetzt neun Kanäle und prüft sämtliche Namen,
Geschwindigkeitseinheiten/-dimensionen und Komponenten/Betrag an jeder
Originalzeile. Die bisherigen PCHIP-, Sweep-, variablen Schrittzahl-, exakten
Endzeit-, Energie-, Studienkurven- und SVG-Prüfungen bleiben bestehen.
Die unabhängige Pendel-SDK-Prüfung führt zusätzlich denselben tatsächlichen
Batchpfad mit sechs Modellen, je drei Längen und drei Workern aus, gegen
installierte und aus Paketquellen neu gebaute Bibliothek.

Die aus dem bytegeprüften unabhängigen Kit entpackte Gegenprobe besteht:

- Intel macOS/Apple Clang 16:
  `build/target-pendulum-proof-mac/Native SDK ä bh2do0yk`.
- Debian 12/GCC 12.2:
  `build/target-pendulum-proof-linux/Native SDK ä mpym0irx`.

Je System bestehen zwölf komplette Studien mit insgesamt 36 Läufen, alle
bisherigen Pendel-SDK-Referenzen und beide Bibliotheken. Das Kit besitzt
weiterhin 146 unabhängige Eingaben; sein Selbsttest besteht. Dies belegt die
konkrete Korrektur des fehlgeschlagenen Gates, nicht den gesamten danach
folgenden SDK-/GUI-Lauf.

Derselbe offizielle Stand `7fea5f2` besteht unter Windows/MSVC Debug und
Clang-Cl Debug jeweils 681/681 native Fälle (`run-7wyphq5w`, `run-dx0a1n89`)
sowie jeweils 10/10 gezielte Sanitizerfälle (`run-w7j5mk58`, `run-mu2544k5`).
Die Artefaktdigests wurden vor der Auswertung geprüft:

- MSVC Debug: `f3f09fdb33c9db9cad236ab333b4554307f2a462bf2b163f2ccb0df029e7a4b7`.
- Clang-Cl Debug: `a90e2ff57a77f041a1dc2fb8bed876253a0a5c900329dd3ae745ac853651f7d9`.

Absichtlich scheiternde Nested-Runnerfälle bleiben Teil der Selbstprüfung,
keine Produktfehler. Windows/Clang-Cl Release scheitert laut offiziellem
Jobstatus ebenfalls im Schritt „Native SDK package and relocation“; sein
konkretes Diagnoseartefakt war noch nicht lesbar, daher wird keine identische
Ursache behauptet. Die macOS-Matrixjobs waren beim letzten Abruf weiterhin
queued. Eine vollständige neue Matrix-/Release-Paketabnahme ist nicht bewiesen.

Am Stand `35793ee` wurden beide lokalen vollständigen nativen Release-Läufe
mit SDL gestartet. Intel macOS/Apple Clang 16 besteht alle **681/681** Fälle
in `run-q5rzlvo1` (`build/current-complete-native-mac.log`). Linux ist noch
nicht als erfolgreicher Gesamtlauf abgenommen. Alle 1295 Git-Dateien wurden
zuvor zwischen Mac und VM per SHA-256 abgeglichen. Die während der laufenden
Nativsuite korrigierten SDK-Harnessdateien sind keine Compiler-Eingaben
jener Nativfälle und werden separat wie oben geprüft.
Beim Linux-Lauf übertrug das macOS-Tar zusätzlich 735 AppleDouble-Begleitdateien;
dadurch scheiterte die Headerinventur des Dokumentationschecks. Diese
Transferdateien wurden nach Magic-/Gegenstückprüfung bytegeprüft unter
`build/source-transfer-metadata.tar.gz` gesichert und ausschließlich daraus
bereinigt. Der direkte Referenzcheck besteht danach mit 31 Seiten. Der bereits
fehlgeschlagene Gesamtlauf wird nicht als bestanden gezählt oder verschwiegen.

## Wiederherstellbare ältere Linux-SDK-Nachweise

Die älteren Prüfverzeichnisse `build/project-manager-keyboard-sdk-proof-linux`,
`build/saved-run-tutorial-sdk-proof-linux` und
`build/pendulum-tutorial-sdk-final-linux` sind vollständig unter
`/Users/lulus/Projects/physim/build/historical-linux-sdk-proofs.tar.gz`
auf dem Mac erhalten. Das Archiv ist 457044705 Bytes groß, umfasst 17060
reguläre Dateien mit ursprünglich 1692191898 Bytes und besitzt SHA-256
`cdf5854385d6ab93b411f602c8de5e8d318dcca1df685ada3c94d8c8f8661886`.
Die nebenliegende `historical-linux-sdk-proofs-receipt.json` enthält jede
Originaldatei mit Größe/Prüfsumme und den Wiederherstellungspfad. Die Receipt
liegt auch in der VM. Alle Archivmitglieder wurden vollständig gegen die
Originale geprüft; vor der Bereinigung wurden die Originaldateien nochmals
vollständig abgeglichen. Nur danach wurden diese drei VM-Verzeichnisse entfernt.
Die Dateien können mit ihren ursprünglichen projektbezogenen Archivpfaden
wiederhergestellt werden. Die einzige Archivkopie liegt auf dem Mac; das
Archiv bleibt deshalb erhalten. Linux hatte danach wieder rund 2 GB Platz.

Die zusätzliche Platzbereinigung auf dem Mac bewahrt vollständig die älteren
Verzeichnisse `build/Quantity sums SDK ä mac cv7rk3x8` und
`build/Linear systems SDK ä mac izmu2853` unter
`build/historical-mac-sdk-proofs.tar.gz` auf dem Mac.
Das Archiv enthält 29148 reguläre Dateien mit ursprünglich 1495530695 Bytes,
ist 431791069 Bytes groß und besitzt SHA-256
`0381599e5db1fe5b9a02f03958820b3c21cc26bf92835cce6e8c0e344a5d07dd`.
Die Receipt liegt auf beiden Systemen. Alle Archivmitglieder und anschließend
sämtliche Originale wurden byteweise gegen ihre Prüfsummen abgeglichen;
erst danach wurden ausschließlich diese zwei Mac-Verzeichnisse entfernt.
Die Archivkopie wurde vollständig per SHA-256 zurück auf den Mac geprüft,
bevor die VM-Kopie entfernt wurde. Die einzige Archivkopie liegt jetzt auf dem
Mac und bleibt deshalb erhalten.
Die nebenliegenden Receipts und `build/app-profiling-proof-locations.json`
dokumentieren beide Archive und die Wiederherstellungspfade.

Ein neues vollständiges macOS-App-SDK wurde tatsächlich gebaut unter
`build/current-complete-sdk-mac`. Seine erste komplette Prüfung in
`build/current-complete-sdk-proof-mac/Native SDK ä ljx1_xif` scheiterte beim
Anlegen der Datei `phys-3.psrun`, während die verfügbare Mac-Kapazität zuvor
auf ungefähr 124 MB gefallen war. Der Runner meldete nur den allgemeinen
Run-Create-Fehler; Speicherplatz als genaue Ursache ist damit nicht bewiesen.
Der gleiche Runner-/Modul-/Verletfall besteht direkt mit 4001 Samples unter
einem neuen Dateinamen. Der Fehlversuch bleibt erhalten und zählt nicht als
Gesamtpaketnachweis. Nach der verifizierten Archivierung hatte der Mac rund
4,3 GB frei; ein neuer vollständiger SDK-Lauf wurde in einem neuen, getrennten
Prüfverzeichnis gestartet. Dieser bleibt bis zu seinem tatsächlichen Ende offen.

Der erste lokale Linux-Release-Gesamtlauf endet mit **680/681** in
`run-8axb5g8f`. Ausschließlich `documentation_reference` scheitert wegen der
oben belegten AppleDouble-Transferdateien; alle nativen Compiler-/Laufzeitfälle
bestehen. Der direkte Dokumentationscheck besteht nach deren Bereinigung.
Ein neuer vollständiger Linux-Lauf auf dem bereinigten Baum mit den korrigierten
SDK-Prüfern wurde gestartet und bleibt bis zu seinem Ende offen. Die 680/681
werden ausdrücklich nicht zu einem bestandenen Gesamtlauf umetikettiert.


## Ausgeführte Verfahren und störfeste Migrationstests

Die unverkürzten lokalen Release-Nativsuiten des Ausgangsstands bestehen
auf Intel macOS 14.6.1/Apple Clang 16 **681/681** in
`build/contact-world-language-release-mac/test-results/run-q5rzlvo1` und auf
Debian 12/GCC 12.2 **681/681** in
`build/contact-world-language-release-linux/test-results/run-r9u97k77`.
Der zweite Linux-Lauf verwendet den bereinigten Transferbaum; der frühere
680/681-Versuch bleibt als Fehlversuch erhalten.

Ein tatsächlicher allgemeiner C-Lauf mit `integrator=0 --adaptive` speicherte
korrekt Dormand–Prince als adaptive Methode, wurde im Lernpfadbericht jedoch
als Euler beschriftet (`build/pendulum-label-repro/oldreport.psreport`).
C- und Physim-Auswertung wählen jetzt bei `step_mode=adaptive` die ausgeführte
adaptive Methode und kennzeichnen sie als adaptiv. Ältere feste Läufe bleiben
lesbar; ältere adaptive Lernpfadläufe werden nur beim belegten RK45-Verfahren
akzeptiert. Unbekannte oder fehlende adaptive Methoden anderer Verfahren
werden abgewiesen, ohne einen fertigen Bericht zu veröffentlichen.

`pendulum_report_labels` prüft acht tatsächlich gerechnete gemischte C-/Physim-
Läufe gegen unabhängig vorgegebene Legenden in beiden Plots und allen drei
Tabellen. Hinzu kommen alte feste Metadaten, der ältere adaptive Lernpfad und
vier abgewiesene Berichte. Die vier betroffenen nativen Pendelfälle bestehen
auf macOS **4/4** in `build/pendulum-template-native-mac/test-results/run-c923sd_m`
und Linux **4/4** in `build/pendulum-template-native-linux/test-results/run-66ap9b1a`.
Der tatsächliche Lernpfad-Appfall besteht jeweils **1/1** in
`run-_79kpcta` (macOS) und `run-1p5xz0wj` (Linux), jeweils unter dem
`contact-world-language-release-*`-Testverzeichnis. Linux/Clang 14 Debug mit
ASan/UBSan besteht die neue Legendenprüfung **1/1** in
`build/atspi-asan-linux/test-results/run-zvu17tpk`;
`ASAN_OPTIONS=detect_leaks=0` bleibt auf SDL/Mesa-Leaks begrenzt.

Der zweite vollständige alte macOS-SDK-Versuch in
`build/current-complete-sdk-proof-mac/Native SDK ä 0qyzod7z` besteht die
vorangehenden Library-/Modell- und neun Appabläufe sowie Dokumentations-,
Typografie- und Einstellungstests, scheitert aber beim normalen Migrationstest
in Stufe 3. Seine Ergebnisse sind kein vollständiger SDK-Nachweis. Der
Migrationstest fehlte im vorhandenen Filter für fremde Eingabeereignisse.
Gezielt eingestreute native Mausbewegungs-/Fokusereignisse reproduzieren den
Stufe-3-Fehler vor der Korrektur (`build/migration-noise-before-mac.log`).
Die Filterergänzung gilt für den ausdrücklich gestarteten Testmodus.
Der reguläre Produkt-Eingabepfad bleibt unverändert. Eine Aufzeichnung des
ursprünglichen externen Ereignisses liegt nicht vor.

Die dauerhafte Gegenprobe führt zwölf tatsächliche Migrationen durch:
zwei Fenstergrößen, mit/ohne fremde Ereignisse, jeweils normal, extern
geändert oder mit ungespeicherten Einstellungen. Sie prüft Backup und
Manifest byteweise sowie erhaltene Quellen/Laufdaten und beide Schutzfälle.
Der native Katalogfall besteht auf macOS **1/1** in `run-c_zd5i8i` und Linux
**1/1** in `run-znf7v7vt`, jeweils im `contact-world-language-release-*`-
Testverzeichnis. Die separat ausgeführten zwölf Fälle bestehen ebenfalls
auf beiden Systemen (`build/migration-integrated-proof-{mac,linux}.log`).

Die unabhängige SDK-Prüfmappe enthält jetzt 148 verifizierte Eingaben und
prüft die Verfahrenslegenden gegen installierte und neu gebaute Bibliotheken.
Ein neues App-SDK ist unter `build/report-method-migration-sdk-mac` gebaut.
Seine vollständige Prüfung in
`build/report-method-migration-sdk-proof-mac/Native SDK ä u9ms4eew` endet mit
Fehler im Projektil-Appfall, Stufe 62 beim Wiederöffnen eines gespeicherten
Berichts. Zuvor bestehen die Library-/Modellprüfungen einschließlich der
neuen Legendenkontrolle gegen beide Bibliotheken, adaptive Studien, alle neun
neu gebauten Beispielprojekte und der Pendel-Appablauf. Das SDK besitzt kein
`PASSED.txt` und zählt nicht als vollständiger Paketnachweis. Der Fehler und
`verification.log` bleiben erhalten; die ursprüngliche Ereignisursache ist
noch offen.


## Frische Linux-Pakete und Windows-SDK am Studiengate

Die Paket-CI am Commit `40712ce` besteht auf frischen Ubuntu-24.04- und
Debian-12-Systemen. Die offiziellen Artefakte wurden vor dem Lesen anhand
ihrer API-SHA-256-Digests geprüft:

- `build/sdk-gate-installed-linux-ubuntu-24.04.zip`:
  `839f76f46b930dd7e7464c827b23a29d63fe91d866142fb8a7fff34111ca6b77`;
  vollständiger SDK-Nachweis in `Native SDK ä 4egw6igf/PASSED.txt`.
- `build/sdk-gate-installed-linux-debian-12.zip`:
  `bef76388b2b63c57a3cc2a149a9cdefcf5c11f22a59ff98c012507d30914eef3`;
  vollständiger SDK-Nachweis in `Native SDK ä eueq3f7v/PASSED.txt`.

Beide Archive enthalten tatsächlich ausgeführte App-, Tastatur-,
Projektmanager- und alte Migrationstests sowie eigene bestandene Zenity-
Dateidialogprüfungen mit Unicodepfaden, Abbruch und fehlendem Backend.
[CI-Paketlauf](https://github.com/PhysicSimulator/physim/actions/runs/37809527308).
Die zwölf neuen störbehafteten Migrationen und Verfahrenslegenden gehören
zum späteren Commit `005339a` und werden diesen älteren Paketen nicht zugerechnet.

Auch Windows/MSVC Release besitzt am Commit `40712ce` eine bestandene
vollständige SDK-Prüfung ohne App-Displaytests in
`native/Native SDK ä avl5n_zi/PASSED.txt`. Das zugehörige offizielle Archiv
`build/report-label-windows-v143-release.zip` ist gegen SHA-256
`f1dc4065340ee61614388cdb3c15c747e000f47b3997c47e5a13ae5a62883840`
geprüft. Der [gesamte Windows-Job](https://github.com/PhysicSimulator/physim/actions/runs/37809527284/job/113422452838)
scheitert erst im separaten Schritt `Record verified Release workloads`.
Die konkrete Benchmarkursache ist nicht belegt; der Joblogzugriff liefert
HTTP 403 und es liegt kein Benchmarkartefakt vor. Dieser Job wird trotz
bestandenem SDK ausdrücklich nicht als vollständig erfolgreich bewertet.


## Fremde Ereignisse beim Wiederöffnen im allgemeinen Appskript

Der fehlgeschlagene SDK-Projektilfall oben besitzt eine kontrollierte
Gegenprobe: `build/workflow-noise-before-absolute-mac.log` reproduziert exakt
Stufe 62 mit leerem Bericht nach fremden Mausbewegungs-, Freigabe-, Rad- und
Fokusereignissen unmittelbar nach dem tatsächlichen Öffnen-Button.
Der allgemeine `--self-test`-Ablauf fehlte ebenfalls im vorhandenen Filter.
Er ist jetzt ausdrücklich eingeschlossen; reale Nutzerabläufe und die
bestehenden absichtlich ungefilterten Dialog-/Dokumenttests ändern sich nicht.
Der ursprüngliche native Ereignisstrom des SDK-Fehlversuchs wurde nicht
aufgezeichnet; die kontrollierte Gegenprobe beweist diese Fehlerklasse.

Der neue Katalogfall `workflow_pointer_isolation` öffnet jeweils den
ursprünglichen Bericht und Lauf mit genau diesen Ereignissen erneut, bei
1440×940 und 1080×740. Er prüft die vollständigen bestehenden Appablauf-
Assertions sowie beide tatsächlichen Ereignisinjektionen, Screenshotdateien
und erhaltene zusammengehörige Lauf-/Berichtsartefakte. Intel macOS besteht **1/1** in
`build/contact-world-language-release-mac/test-results/run-fn2v62mb` und
Debian/GCC **1/1** in
`build/contact-world-language-release-linux/test-results/run-x6jkhejg`.
Die Logs liegen unter `build/workflow-input-isolation-{mac,linux}-proof.log`.
Ein früherer Harnessversuch ohne angelegten Elternordner und ein Linux-
Aufruf mit falschem SDL-Prefix starteten die Gegenprobe nicht erfolgreich;
sie werden nicht als Produktnachweise gezählt.

Der vollständige SDK-Prüfer übernimmt dieselbe Gegenprobe. Das neue App-SDK
`build/workflow-input-sdk-mac` ist tatsächlich gebaut; die vorherige
SDK-Gesamtprüfung bleibt fehlgeschlagen. Eine nachfolgende vollständige
Prüfung wird erst nach ihrem tatsächlichen Ende bewertet.


Die neue unabhängige Prüfmappe besteht ihren Paketgrenzen-Selbsttest mit
**149** exakten Eingaben, SHA-256, fehlenden/dynamischen Pfaden und exklusiver
Veröffentlichung (`build/workflow-input-kit-selftest.log`). Die extrahierten
149 Dateien wurden vollständig gegen ihr Manifest abgeglichen. Die neue
vollständige macOS-App-SDK-Prüfung läuft aus dieser Mappe unter
`build/workflow-input-sdk-proof-mac`; ihr Log ist
`build/workflow-input-sdk-full-mac.log`. Solange sie nicht erfolgreich endet,
bleibt dieser vollständige Paketnachweis offen.


## Explizite Windows-Linkabhängigkeit des Szenenbenchmarks

Der direkte MSVC-/Clang-Cl-Builder verknüpfte `physim-scene-benchmark` mit
`app/ui_sdl.c`, gab diesem Ziel aber im Gegensatz zur App und zum UI-Benchmark
keine `dwmapi.lib`. `window_chrome` ruft dort unmittelbar
`DwmSetWindowAttribute` auf; [Microsoft nennt Dwmapi.lib als erforderliche Importbibliothek](https://learn.microsoft.com/en-us/windows/win32/api/dwmapi/nf-dwmapi-dwmsetwindowattribute).
Die explizite Linkliste enthält nun alle drei Ziele. Es werden keine
Bibliotheken in der GUI-freien Physim Library ergänzt.

Alle drei tatsächlichen Release-Benchmarkziele bauen mit Apple Clang 16
auf Intel macOS und GCC 12.2 auf Debian. Die vorhandenen Szenenprüfungen
mit/ohne GPU-Zeitabfrage bestehen auf beiden Systemen **2/2**:
`build/contact-world-language-release-mac/test-results/run-_7hwhkyq` und
`build/contact-world-language-release-linux/test-results/run-rp_ogf66`.
Die echten Referenzlasten und der unabhängige Benchmarktreiber bestehen je
**2/2** in `run-55nywkeq` (macOS) und `run-vfgmtp54` (Linux), jeweils unter
demselben Release-Testverzeichnis. Build- und Testlogs sind
`build/scene-benchmark-link-{build,tests}-{mac,linux}.log` und
`build/scene-benchmark-workloads-{mac,linux}.log`.
Diese tatsächlich ausgeführten Prüfungen belegen keinen Windows-Build.
Die fehlende direkte Importbibliothek ist durch die Quellen belegt; die
konkrete Ursache des oben fehlgeschlagenen Windows-CI-Jobs bleibt ohne Joblog
offen. Der aktuelle MSVC-Link-/Benchmarknachweis bleibt bis zu ausgeführter
CI offen.


## Wiederaufbau und Schritt-Haltepunkte am 11. Oktober 2026

Bei der erneuten Bestandsaufnahme enthält `build/` nur noch Finder-Metadaten.
Die zuvor ignorierten lokalen SDKs, Prüflogs, Bitmaparchive und die SSH-/Image-
Konfiguration der Debian-VM sind nicht vorhanden. Der Quellcode bleibt erhalten.
Die oben dokumentierten Läufe und Prüfsummen bleiben historische Aussagen;
ihre lokalen Belegpfade und die als alleinige Kopien bezeichneten Archive
sind aktuell nicht verfügbar. Es wurde keine neue Linux-VM-Prüfung daraus
abgeleitet. Die aktuelle Prüfung wird in neuen Verzeichnissen aufgebaut.

SDL 3.2.30 wurde erneut aus dem offiziellen `release-3.2.30`-Quellstand unter
`build/deps/` gebaut und installiert. Intel macOS 14.6.1/Apple Clang 16
besteht die neuen Runner-/Haltepunkt-/Bitmapfälle **3/3** in
`build/current-headless-mac/test-results/run-0pp11wvc`, Projektdatei und
Migration **2/2** in `build/current-release-mac/test-results/run-8mc1lryw`
sowie Haltepunkt-Appkette und verzögerten Dokumentationstest **2/2** in
`build/current-release-mac/test-results/run-bu7fets6`. Die vier bestehenden
Runner-Steuerungsfälle bestehen **4/4** in `run-xqkfm_nw` desselben Release-
Testverzeichnisses. Details: `build/current-{breakpoint-native,project-tests,window-tests,runner-regression}-mac.log`.

Die Haltepunktliste enthält bis zu 32 verschiedene akzeptierte Schrittzahlen
von 0 bis 1000000000. Tests rechnen tatsächlich C- und Physim-Modelle mit
festen und adaptiven Schritten, halten bei 0/3/7, prüfen Einzelschritt und
Fortsetzen, vergleichen alle gespeicherten Messzeilen exakt mit einem Lauf
ohne Haltepunkte und weisen zwölf ungültige CLI-Konfigurationen vor jeder
Laufdateianlage ab. Die Appprüfung bedient die echte Eingabe sowie Start-,
Einzelschritt-, Fortsetzen-, Reset- und Stop-Buttons für beide Sprachen bei
zwei Fenstergrößen und prüft gespeichert erneut geöffnete Einstellungen.

Das neu gebaute App-SDK `build/current-sdk-mac` besteht außerdem die
verschobene Gegenprobe aus einer **151 Eingaben** umfassenden, vollständig
SHA-256-geprüften unabhängigen Mappe ohne Physim-Implementierung. Der Compiler
wird dabei automatisch erkannt; der Aufruf benötigt kein `--compiler`.
Nachweis: `build/current-breakpoint-sdk-proof-mac/Native SDK ä y6g_pzoq/PASSED.txt`
und `verification.log`; Log: `build/current-breakpoint-sdk-proof-mac.log`.
Dies ist eine fokussierte SDK-Abnahme, keine gesamte Plattform-/Produktabnahme.

`--archive-bitmaps` komprimiert nur bereits beendeter Prüfkommandos erzeugte
BMPs. SDK-Eingaben bleiben geschützt. Komprimierte und wiederhergestellte
Bytes werden vor dem Entfernen des Originals gegen SHA-256 geprüft; eine
atomar veröffentlichte `bitmap-archive.json` erhält Originalpfad, Größe,
Prüfsumme, Archivpfad und Prüfphase. Die 16 echten SDK-Screenshots behalten
alle **275560608 Originalbytes** in **8023502 komprimierten Bytes**; jedes
Archiv wurde zusätzlich nach dem SDK-Lauf vollständig nachgeprüft. Die
Bilder lassen sich mit `python3 tools/archive_bitmaps.py PRÜFVERZEICHNIS --restore`
bytegetreu zurückholen; fremde vorhandene Dateien werden nicht ersetzt.
Paketgrenzen-Selbsttest und korruptions-/Unterbrechungs-/Pfad-/Eigentümer-
Gegenproben bestehen. Die CI archiviert auch Receipts und komprimierte BMPs.

## Spätere CI am Commit e8774bd

Der [C17-Lauf](https://github.com/PhysicSimulator/physim/actions/runs/37821695246)
besteht Linux/GCC, Linux/Clang, Apple-Silicon-macOS sowie MSVC/Clang-Cl Debug
und Release. Windows/MSVC Release besteht jetzt auch den separaten Benchmark-
Schritt. Das offizielle Artefakt `build/e877-windows-release-benchmark.zip`
ist gegen SHA-256
`2dfc60e0ba491898016ca5b773927679147bc5c1e44433b2e2a38244ea536742`
geprüft und enthält alle acht Referenzlasten in `summary.json`.
Die API-Job-/Artefaktinformationen liegen unter `build/e877-ci-*.json`.
Diese Ergebnisse betreffen `e8774bd`, noch nicht die neue Haltepunktfunktion.

Nur der Intel-macOS-Job endet mit Fehler: `docs-keyboard-delayed` überschreitet
mit absichtlich 80 ms Verzögerung je Bild und Apple Software Renderer die
60 Sekunden nahe dem letzten Schritt 30. Der authentifiziert gelesene
[Joblog](https://github.com/PhysicSimulator/physim/actions/runs/37821695246/job/113464056502)
belegt diese Stelle; der Auszug liegt in `build/e877-intel-sdk-failure-excerpt.log`.
Das Budget ist ausschließlich für diesen verzögerten Test auf 120 Sekunden
mit 135 Sekunden äußerem Prozesslimit angehoben. Alle Fokus-, Eingabe-,
Clipboard-, Navigations- und Fehlerprüfungen bleiben erhalten; die normalen
Dokumentationsmodi behalten 60 Sekunden. Eine neue tatsächlich ausgeführte
Intel-CI-/Gesamtpaketprüfung bleibt bis zu ihrem Ergebnis offen.


## Aktuelle Library-/Repository-Grenzen

`build/current-base-requirement-receipt.json` ordnet die Bestandsprüfung dem
Commit `bfb7967a9e142400a744a60d26967a2325c4dedb` zu. Alle 52 öffentlichen
Header und 28 Core-Quellmodule enthalten keine direkten SDL-/Nuklear-/OpenGL-/
AppKit-Includes. Die Imports des tatsächlich ohne SDL/App gebauten
`build/current-headless-mac/lib/libphysim-core.a` enthalten keine entsprechenden
GUI-Symbole; dessen SHA-256 ist
`c8a9918448ff15361a0f2f266c8a2da6587bf390b10e433f9b87d2df8b2c5144`.
Die SDK-Library-Probes linken ohne GUI gegen installierte und neu gebaute
Core-Archive. Damit ist die Library-Grenze PP-0056 konkret nachgewiesen.

Die NUL-separierte Git-Inventur belegt 636 verfolgte Beispiel-/Testdateien
und alle 635 erforderlichen Code-, Header-, Physim-, Python-, Konfigurations-,
Dokument- und Assetdateien dieses Baums. Auch `Größe.phys` wird bytegetreu
zugeordnet; Shell-/Git-Darstellungsquotierung gilt nicht als fehlende Datei.
Damit ist PP-0067 konkret nachgewiesen. Diese begrenzten Einzelabnahmen
behaupten keine bestandene vollständige Produkt-/Plattformprüfung.

Die neue unverkürzte lokale Headless-Release-Suite besteht **660/660** in
`build/current-headless-mac/test-results/run-ur_wxs0o`, Log
`build/current-complete-native-mac.log`. Dieser Aufruf baut ohne SDL/App;
die ausgelassenen App-/Displayfälle sind darin nicht als bestanden gezählt.

Die vollständige SDK-Prüfung aus der finalen unabhängigen Mappe endet mit
**PASSED** in `build/current-full-sdk-proof-mac/Native SDK ä i6gbzrb2`.
`PASSED.txt`, `verification.log` und `build/current-full-sdk-proof-mac.log`
belegen die tatsächlich ausgeführte Prüfung mit `--app-tests --archive-bitmaps`:
installierte und aus Paketquellen neu gebaute Bibliotheken, beide vollständigen
Lernpfade, reale C-/Physim-Runner, neun GUI-Experimente, Migration, native
Barrierefreiheitsaktionen, Haltepunkte, Dokumentation sowie alle 32
Vorlagen-/Sprachpaare und zwei unabhängige Analysen bei 16 und 22 px.
Das zuvor eingefrorene SDK prüft den Stand vor der folgenden Fokus-/
Isolationstest-Erweiterung; es belegt diese neuen Änderungen nicht.
Alle **365** BMP-Archive wurden anschließend erneut vollständig gestreamt und
gegen Originalgröße, Original-SHA-256 und komprimierten SHA-256 geprüft:
**6418763970 Originalbytes**, **194853871 komprimierte Bytes**.

## Windows-UTF-8 und nachgewiesener Linux-Eingabefokus

Die Debug-Jobs des [C17-Laufs am Stand bfb7967](https://github.com/PhysicSimulator/physim/actions/runs/38124281696)
bestehen die neuen nativen Haltepunktprüfungen, scheitern aber beim
Bitmaparchiv-Selbsttest. Der authentifiziert gelesene Joblog und das gegen
SHA-256 `6f408511383115fe79232ffe522b1f7f4c6fb2693868d901b73bfb0eb2d2414d`
geprüfte offizielle Artefakt `build/bfb-windows-debug-evidence.zip` belegen:
Die UTF-8-Receipt wurde mit dem lokalen cp1252-Standard gelesen; Unicode-
Dateipfade waren dadurch beschädigt. Die Receipt-Lesewege geben jetzt UTF-8
explizit an. Eine Gegenprobe erzwingt cp1252 als Standard und besteht.
Der Python-Testtreiber schreibt seine Diagnoseausgabe ebenfalls explizit
als UTF-8; zuvor verdeckte ein zusätzlicher UnicodeEncodeError die Ursache.
Der Treiber-Selbsttest besteht mit kontrollierten inneren Fehlerfällen.
Die tatsächliche aktuelle Windows-Gesamtprüfung bleibt bis zum CI-Ende offen.

Der Linux-/Clang-SDK-Job desselben Laufs erreicht den kleinen C-Haltepunkt-
Appfall, nimmt dort aber keine Texteingabe entgegen. Das offizielle Artefakt
`build/bfb-linux-clang-evidence.zip` ist gegen SHA-256
`d694b0e37bd9955c415f2e7f72d57bbb105f417d2a224ea9024c1bcd40b47d39` geprüft.
Der tatsächliche Screenshot zeigt ein leeres Feld ohne Caret; die Auswertung
scheitert in Stufe 4. Der Harness wartet nun vor der Texteingabe auf den
wirklichen Nuklear-Editorfokus und wiederholt den Mausklick höchstens 16-mal.
Texteingabe-, Speicher-, Lauf- und Messdatenassertions bleiben unverändert.
Der Hinweis erhält zwei einzelne Labelzeilen, da der vorherige Zeilenumbruch
als Fragezeichen gerendert wurde. Die neue Linux-Gegenprobe bleibt offen.

Am nachfolgenden [Stand 35fea55](https://github.com/PhysicSimulator/physim/actions/runs/38125861907)
enden Windows/MSVC Debug und Windows/Clang-Cl Debug erfolgreich. Das ist ein
ausgeführter Nachweis der UTF-8-Korrektur, einschließlich der nativen Suite.
Linux/GCC erreicht im [Job 114429971568](https://github.com/PhysicSimulator/physim/actions/runs/38125861907/job/114429971568)
wieder denselben kleinen C-Haltepunkt-Appfall und scheitert in Stufe 4.
Der authentifiziert gelesene Log belegt den Fehler vor der Fokuskorrektur;
er ist kein Nachweis gegen die anschließend geänderte Harnessfassung.

## Runner-Isolation und Wiederaufnahme in der tatsächlichen App

Die neue Release-Appprüfung besteht **2/2** in
`build/current-release-mac/test-results/run-qwbgjr0b`, Log
`build/current-focus-isolation-mac.log`: die Haltepunkt-Appkette mit bestätigtem
Editorfokus und `runner_isolation_workflow` mit **acht** tatsächlich getrennt
gestarteten Fehler-/Wiederaufnahmefällen auf Intel macOS.
C und Physim werden jeweils beim Erzeugen und während des Schritts geprüft,
mit Absturz und Endlosschleife. C verwendet einen echten `abort()`-Prozessabbruch;
Physim löst einen kontrollierten Laufzeitfehler mit `assert(false, ...)` aus.
Die Physim-Fälle sind kein Nachweis eines nativen Speicherzugriffsfehlers.
Die hängenden Runner werden über den echten Stop-Button und dessen Timeout
beendet. Danach öffnen und schließen die Tests das Handbuch per Tastatur
und starten im selben App-Prozess einen gesunden Lauf bis Schritt 7.

Fehler beim Erzeugen hinterlassen keine scheinbar gültige Messdatei.
Schrittfehler erhalten lesbare, streng zeitlich geordnete vollständige
Messblöcke; deren Dateiprüfsumme bleibt nach dem neuen Lauf unverändert.
Der erfolgreiche neue Lauf enthält exakt acht Messpunkte einschließlich des
Anfangszustands. Alle acht Fälle bewahren beide unveränderten Quell-Snapshots
und eigene Laufnamen; Wiederaufnahme ersetzt keinen vorherigen Lauf.
Der erste Harnessversuch verwendete die Katalog-Restore-API nach erfolgter
Parametererkennung und wurde korrekt abgewiesen. Die Gegenprobe wählt nun
den vorhandenen validierten Parameter; dieser Fehlversuch zählt nicht als
Produktfehler oder bestandener Nachweis. Linux-/Windows-Isolation bleibt
bis zur tatsächlichen Ausführung offen.

Die frische Gegenprobe zu den priorisierten Review-Korrekturen besteht
`analysis_extremes` und `material_tutorial` **2/2** in
`build/current-headless-mac/test-results/run-x9thngvs`, Log
`build/current-review-regressions-mac.log`.
Die beiden schreibenden Review-Gegenproben `autosave` und `text_document`
bestehen ebenfalls frisch **2/2** in
`build/current-release-mac/test-results/run-v_t569c2`, Log
`build/current-review-persistence-mac.log`. Die sieben ursprünglichen
Review-Reparaturen aus `903135e` bleiben erhalten.

## Fehlender Datensatz nach frühem Experimentabbruch

Die stärkere Gegenprobe am Stand `7b08c04` weist einen irreführenden Abschluss
nach: Ein echter C-Abbruch beim Erzeugen hinterlässt keine Messdatei. Die App
startete dennoch einen Datensatz-Worker und ersetzte den Runner-Hinweis durch
„Datensatz konnte nicht vollständig gelesen werden“. Der tatsächliche Versuch
endet in Stufe 6 mit Exit 1; Log:
`build/current-no-measurement-before-absolute-mac.log`. Ein zuvor mit relativem
Projektpfad gestarteter Harnessversuch scheiterte bereits am Projektbuild und
ist kein Nachweis dieser Fehlerursache.

Die App prüft nun nach dem Prozessende, ob eine Messdatei vorhanden ist.
Ohne Datei bleibt „keine Messdatei angelegt“ mit Exit-Code beziehungsweise
Zeitlimit und Verweis auf das Protokoll erhalten; ein Datensatz-Worker wird
nicht gestartet. Vorhandene Dateien werden weiterhin geladen und bei Bedarf
rekonstruiert. Fehlgeschlagene Reset-Läufe behalten ihren bestehenden eigenen
Pfad zur Wiederherstellung des vorherigen Zustands. Quell-Snapshots und
Laufgrenzen werden nicht gelöscht. Die create-Fälle des Isolationstests
prüfen den erhaltenen Hinweis zusätzlich zu fehlender Messdatei, beendetem
Runner, bedienbarem Handbuch und erfolgreichem neuen Lauf.

Die korrigierte Release-App besteht **2/2** Katalogfälle in
`build/current-release-mac/test-results/run-dbnlt5_d`, Log
`build/current-no-measurement-recovery-mac.log`: alle **acht** realen
C-/Physim-Isolationsfälle und **vier** Resetvarianten. Die vier frühen
Fehlerfälle enthalten den richtigen Hinweis und keinen nachfolgenden
generischen Datensatzfehler; alle zwölf App-Prozesse enden erfolgreich.
Der Screenshot `isolation-recovered.bmp` des C-create-Abbruchs wurde zusätzlich
als PNG-Vorschau dekodiert und visuell geprüft: vollständiger Fehlerhinweis,
weiter verfügbare Steuerung und keine erfundenen Messwerte.
Dies belegt Intel macOS; die aktuelle andere Plattformmatrix bleibt offen.

## Runner-Isolation aus der unabhängigen SDK-Prüfmappe

Ein neues Release-App-SDK unter `build/current-isolation-sdk-mac` wurde mit
Apple Clang auf Intel macOS gebaut und installiert. Alle **471** Manifestdateien
wurden vollständig gegen SHA-256 geprüft. Der unabhängige Kit-Selbsttest
besteht mit **151** Eingaben (`build/current-isolation-kit-selftest.log`);
alle entpackten Dateien wurden erneut vollständig gegen ihr Kitmanifest
verglichen. Der entpackte Prüfer ist bytegleich mit der neuen Quellfassung.

Der tatsächlich ausgeführte Aufruf aus `build/current-isolation-verification-kit`
mit `--isolation-only --archive-bitmaps` und automatischer Compilererkennung
endet mit **PASSED** in
`build/current-isolation-sdk-proof-mac/Native SDK ä f58alae6`.
`PASSED.txt`, `verification.log`, `Runner isolation app/app-steps.json` und
`build/current-isolation-sdk-proof-mac.log` belegen alle **acht** realen
C-/Physim-create/step-Abbruch-/Endlosschleifenfälle, frühe Fehlerhinweise,
Handbuchbedienung, gesunden Neustart und unveränderte Lauf-/Quellartefakte.
Die Projekte werden innerhalb der verschobenen SDK-Kopie mit dem ausgelieferten
Builder und Compiler neu gebaut; SDK-Quellen und Header kommen aus dem Paket.
Es ist keine Wiederholung gegen Repository-Binärdateien.

Alle **20** BMP-Archive sind nach Abschluss erneut gestreamt und gegen
Originalgröße sowie Original-/Archiv-SHA-256 geprüft:
**255746760 Originalbytes**, **7751387 komprimierte Bytes**. Die neue
fokussierte Prüfung benennt ihre Grenze ausdrücklich; sie ersetzt keine
vollständige SDK- oder Plattformabnahme. Die vollständige Prüfung mit
`--app-tests` ruft dieselbe Isolationsfunktion auf. Unzulässige Kombinationen
mit anderen fokussierten Modi oder `--app-tests` werden vor jedem Kopieren
abgewiesen. Der gesamte neue SDK-Prüfer wurde nach dieser Ergänzung noch
nicht vollständig durchlaufen.

PP-0061 besitzt damit einen konkreten Architekturnachweis: `build_project`
in `app/main.c` startet `physim-build` als getrennten Prozess; dessen
Compileraufrufe in `app/build_main.c` starten ihrerseits getrennte Prozesse.
`start_run_mode` startet ausschließlich den externen `physim-runner`.
`src/platform.c` verwendet dafür `CreateProcessW` beziehungsweise `fork` und
`exec`; Experimentmodule werden in `runners/experiment.c` geladen.
Die echte App baut beide Sprachen aus dem verschobenen SDK, übersteht den
nativen C-Prozessabbruch und bedient im selben App-Prozess danach das Handbuch
und einen gesunden Lauf. Dieser Nachweis beschränkt sich auf die Prozessgrenze;
aktuelle gesamte Linux-/Windows-/macOS-Funktionsgleichheit bleibt offen.

## Strukturierte Material-/Mediumseinheiten im Lernpfad

Die neue unabhängige Metadaten-Gegenprobe scheitert am bisherigen Stand
`2f0f9a4`: Der tatsächliche C-Lauf speichert Material-/Mediumswerte, aber keine
`parameter_unit.materialDensity`-Deklaration. Log:
`build/current-material-units-before-mac.log`. Einheiten in Beschreibungstexten
ersetzen keine strukturierten Dimensionen. Die C- und Physim-Materialquellen
deklarieren jetzt Radius in `m`, beide Dichten in `kg/m^3` und dynamische
Viskosität in `Pa s` mit SI-Skalierung 1. Parameterwerte, Grenzen und Modell-
Rechnung bleiben gleich; Dateiformat und ABI erhalten keine neue Version.

Die native Release-Prüfung besteht **1/1** in
`build/current-headless-mac/test-results/run-70_3wxc7`, Log
`build/current-material-units-native-mac.log`. Ihr unabhängiger Binärparser
prüft alle CRCs und Schemaeinheiten sowie für **14** vollständige C-/Physim-
Läufe in sieben Konfigurationen: gewählte Werte, Standardwerte, Grenzen,
Einheitensymbol, Skalierung, alle sieben SI-Dimensionen, Modell-Eigenschaftswerte,
Material-/Mediumsnamen, tatsächlichen Zeitschritt und verlustfreien Seed.
Das kleinere-Kugel-Paar verwendet den maximalen UInt64-Seed
`18446744073709551615`; andere Fälle verwenden 42. Analytische Referenzen,
1001 Messzeilen je Lauf, C-/Physim-Parität, alle vier gemischten Auswertungen,
18 hochpräzise Review-Gegenproben und sechs abgewiesene Modellbereiche bleiben
vollständig geprüft. Die bytegleichen vier Quellblöcke des Handbuchs bestehen
`documentation_material_source` nach Aktualisierung ebenfalls.

Die tatsächliche Release-Appkette besteht ebenfalls **1/1** in
`build/current-release-mac/test-results/run-ct1qe27h`, Log
`build/current-material-units-app-mac.log`. Beide separat gebauten C-/Physim-
Projekte erkennen genau vier Parameter mit den erwarteten Einheitensymbolen,
SI-Skalierungen und Dimensionen, rechnen 40 tatsächliche Einzelschritte,
lesen den aufgezeichneten Lauf wieder ein und öffnen den Analysebericht.
Der erhöhte Prüfvertrag wird auch durch den bestehenden vollständigen
SDK-Prüfer gegen installierte und neu gebaute Core-Archive ausgeführt; ein
neuer gesamter SDK-/Linux-/Windows-Lauf nach dieser Ergänzung liegt noch
nicht vor. PP-0066 ist damit für diesen konkreten Modellweg nachgewiesen,
bleibt für alle Vorlagen und die aktuelle Plattformmatrix unvollständig.

## Änderbarer Wurfwiderstand mit korrekter Modellbeschreibung

Die Gegenprobe am Stand `239adf1` belegt zwei zusammenhängende Lücken in
`build/current-projectile-provenance-before-mac/observed.json`: C weist
`dragCoefficient=0` als unbekannten Parameter mit Exit 5 ab. Physim akzeptiert
den Wert und speichert `parameter.dragCoefficient=0`, meldet aber irreführend
`Cd=0.47` in der Modellbeschreibung. Ältere entsprechende Physim-Dateien
werden nicht umgeschrieben; ihr gespeicherter Parameterwert beschreibt die
tatsächlich verwendete Einstellung, die alte konstante Cd-Angabe nicht.

Beide Experimentquellen deklarieren jetzt `dragCoefficient` als instanzlokalen,
explizit dimensionslosen SI-Parameter mit Standard `.47` und Grenzen `0..2`.
C übergibt den gewählten Wert in jeder RK4-Stufe an die Kraftfunktion; beide
Modellbeschreibungen speichern den tatsächlichen Wert. Reset erhält ihn,
mehrere gleichzeitig lebende Instanzen bleiben unabhängig. ABI, Projekt-
und Messdateiformat ändern sich nicht. Die bisherigen Standard-Referenzwerte
bleiben in der Gegenprobe erhalten.

Die finale native Release-Gegenprobe besteht **1/1** in
`build/current-headless-mac/test-results/run-b4mp02oc`, Log
`build/current-projectile-provenance-mixed-final-mac.log`. Sie prüft **acht**
wirkliche C-/Physim-Runnerläufe mit `Cd=0/.47/.94/2`, jeweils alle 201 Zeilen,
Modell-/Parametergleichheit, Standard/Grenzen, kanonische Einheit und Dimension,
Energiedissipation und die unabhängige analytische Vakuumparabel. Zwei gleichzeitig
lebende Instanzen pro Sprache werden vor und nach Reset unabhängig berechnet.
Vier ungültige Bereichswahlen werden vor Messdateianlage abgewiesen. Alle
**vier** gemischten Experiment-/Analysewege prüfen sämtliche Reportkurvenpunkte
gegen die Laufdaten und die Erzeugung der jeweiligen CSV-Datei. Der CSV-Inhalt
wird in diesem Test nicht separat vollständig eingelesen. Frühere Zwischenläufe ohne diese finalen Zusatzassertions
bleiben auf ihren jeweiligen Prüfumfang beschränkt.

Die Release-App besteht **2/2** Katalogfälle in
`build/current-release-mac/test-results/run-fsnpa919`, Log
`build/current-projectile-provenance-app-mac.log`: Physim-Projekt inklusive
Parameterstudie und gespeichertem erneut geöffnetem Parameter sowie C-Projekt
bei beiden Fenstergrößen mit tatsächlicher Wiederöffnung von Lauf/Bericht
unter fremden Eingabeereignissen. Beide Sprachen wählen `.6`; das neue App-
Testassert prüft die erkannte Einheit und die genaue Übereinstimmung von
gewähltem Parameter und gespeichertem `Cd`. Die drei realen Appprozesse
enden erfolgreich. Beide vollständigen Handbuchquellen bestehen
`documentation_projectile_drag_source`; die Erklärung nennt nun die tatsächlich
verwendete flächenbasierte `Medium.dragForce`-Schnittstelle.

Dies belegt Intel macOS. Die neue ganze SDK-/Linux-/Windows-Abnahme dieses
Wurfparameters steht noch aus; PP-0066 bleibt unvollständig. Am vorherigen
[Stand 7b08c04](https://github.com/PhysicSimulator/physim/actions/runs/38126895020)
endet Windows/Clang-Cl Release inzwischen erfolgreich; dieser Lauf prüft
noch nicht die anschließende Wurf-/Material-Erweiterung.

## Boxkollision: gleiche Laufparameter und SI-Metadaten

Am Stand `0531eb8` weist der vorhandene C-Runner `restitution=0.5` als
unbekannten Parameter mit Exit 5 ab. Die tatsächliche Gegenprobe liegt in
`build/current-box-parameters-before-mac/observed.json`. Physim besaß bereits
vier Laufparameter; C verlangte für diese Werte einen Neubau mit Makros.
Die C-Vorlage deklariert nun ebenfalls `angle`, `offset`, `restitution` und
`friction`. Beide Fassungen speichern explizite SI-Deklarationen (`rad`, `m`,
`1`, `1`) und verwenden die gewählten Werte in Initialzustand, Solver und
Modellbeschreibung. C-Makros bleiben Startwerte innerhalb der gemeinsamen
Grenzen. Reset leert Kontaktzustände, erhält aber die gewählten Parameter.

Die finale native Release-Paritätsprüfung besteht **1/1** in
`build/current-headless-mac/test-results/run-_02x7k7a`, Log
`build/current-box-parameters-model-metadata-final-mac.log`. Zehn wirkliche
Runnerläufe vergleichen jeweils alle **401** Messzeilen und **15** Kanäle für
Standard, unelastischen und schrägen Stoß: die bisherigen kompilierten C-
Makrovarianten und zusätzliche Laufparameteränderungen der unverändert
gebauten C-Basis werden gegen Physim geprüft. Alle gewählten Werte stimmen
mit ihren Modellbeschreibungen überein; Einheitensymbole, SI-Skalierungen und
alle Dimensionsexponenten werden direkt aus den Dateien gelesen. Zwei
gleichzeitig lebende Instanzen pro Sprache bleiben in Pose und Einstellungen
auch über zwei Resetzyklen unabhängig. Alle **16** ungültigen Bereichswahlen
enden mit Exit 5 vor Messdateianlage. Die bestehenden Szenen-, Report- und
CSV-Dateianlageprüfungen bleiben erhalten; sie sind keine zusätzliche
vollständige CSV-Inhaltsprüfung.

Die unabhängige physikalische Boxreferenz und die vorherige Paritätsfassung
bestehen zusammen **2/2** in `run-d0c4tqi6` desselben Headless-Verzeichnisses,
Log `build/current-box-parameters-native-complete-mac.log`.
Der erste Ausbau des Paritätsharness scheiterte am doppelten lokalen Namen
`values`; nach Korrektur besteht die oben genannte finale Fassung. Dieser
Harness-Compilefehler wird nicht als Produktfehler oder bestandener Test gezählt.

Die tatsächlichen Release-Appketten bestehen mit gewählter Restitution `.5`:
Physim **1/1** in `build/current-release-mac/test-results/run-ptei2imp`, Log
`build/current-box-parameters-app-mac.log`, sowie C **1/1** in `run-oky2vnou`
desselben Verzeichnisses, Log `build/current-box-parameters-c-app-final-mac.log`.
Beide bauen ihre Projektquellen, simulieren, lesen den Lauf, prüfen sämtliche
vier Einheiteneinträge und den tatsächlichen Rückprallwert in den Metadaten
und öffnen die Auswertung. Der neue C-Katalogfall prüft zusätzlich den
gespeicherten Wert in `physim.project`; der Physim-Fall öffnet das Projekt mit
dem Wert erneut. Die C- und Physim-Appfälle sind separat ausgeführt; der erste
kombinierte Aufruf hatte den gerade erst hinzugefügten C-Katalogfall noch
nicht ausgewählt und wird ausdrücklich nur als Physim **1/1** gezählt.

Diese Nachweise gelten für Intel macOS. Aktuelle Linux-/Windows- und vollständige
SDK-Abnahme dieser Erweiterung bleiben offen. PP-0066 bleibt unvollständig;
alle 531 ursprünglichen Planblöcke behalten ihren Umfang.

## Frische Linux-Gesamtpakete am Stand 35fea55

Der [Paketlauf 38125861913](https://github.com/PhysicSimulator/physim/actions/runs/38125861913)
endet für Bau, Debian 12 und Ubuntu 24.04 erfolgreich. Die authentifiziert
gelesenen Logs der [Debian-Prüfung](https://github.com/PhysicSimulator/physim/actions/runs/38125861913/job/114434334933)
und [Ubuntu-Prüfung](https://github.com/PhysicSimulator/physim/actions/runs/38125861913/job/114434334936)
belegen tatsächlich ausgeführte SDK-/GUI-/Dialogprüfungen. Die offiziellen
Artefaktinformationen ordnen sie exakt
`35fea551bc73d38e8c7835d61efc692951bd0d96` zu; sie enthalten noch nicht die
später ergänzten Isolations-, Material-, Wurf- und Boxparameterprüfungen.

Die heruntergeladenen ZIPs wurden vor der Inhaltsprüfung gegen die im Lauf
veröffentlichten SHA-256-Werte geprüft:

| Frisches System | Lokales offizielles Artefakt | SHA-256 |
| --- | --- | --- |
| Debian 12 / Mesa 22.3.6 | `build/35fea-installed-linux-debian-12.zip` | `b5809d0f79a3d0c0cc5f34bbab9ae6c90bda9e448ee9ab815948c8967caa3cd0` |
| Ubuntu 24.04 / Mesa 25.2.8 | `build/35fea-installed-linux-ubuntu-24.04.zip` | `98d5b71dce054e58f2b5355c7672bec2815d7cf68308c4e7efc7f1930faeb4f5` |

Beide Systeminventuren und App-Bibliothekslisten sind gelesen; fehlende
Laufzeitbibliotheken liegen nicht vor. Die App startet vor Installation von
Compiler und Python. Danach bestehen installierte und neu gebaute Core-
Verbraucher, beide Lernpfade, neun GUI-Projektabläufe, Haltepunkte bei beiden
Sprachen und Fenstergrößen, Migration, native Barrierefreiheit, Dokumentation,
Tastaturbedienung und alle 32 Vorlagen-/Sprachpaare bei 16/22 px.
Die echten Zenity-Dialogprüfungen bestehen Unicode-Pfade, Auswahl, Abbruch
und nicht verfügbares Backend. Die Archive enthalten beide Erfolgsmarker:
`Native SDK ä w_nr8d2t/PASSED.txt` und `Native dialogs ä jzydeotz/PASSED.txt`
(Debian), `Native SDK ä ii0dl2zc/PASSED.txt` und
`Native dialogs ä ju9rzcqe/PASSED.txt` (Ubuntu).

Alle **365** komprimierten BMPs je System wurden anschließend direkt aus den
ZIPs gestreamt und gegen Originalgröße sowie Original-/Archiv-SHA-256 geprüft.
Je System sind **1604728770** Originalbytes vollständig erhalten, in
**50213393** (Debian) beziehungsweise **50190175** (Ubuntu) gzip-Bytes.
`build/35fea-fresh-linux-proof-receipt.json` erhält die genaue Zuordnung;
`build/35fea-packages-{jobs-current,artifacts}.json` erhält die API-Metadaten.
Dies sind neue tatsächlich inspizierte Belege nach dem lokalen Wiederaufbau;
es wurden keine fehlenden historischen VM-Artefakte dafür vorausgesetzt.

## CI-Warteschlange mit erhaltenen laufenden Prüfungen

Die API-Bestandsaufnahme `build/current-ci-queue.json` zeigt zahlreiche
vollständige, noch wartende Läufe aufeinanderfolgender Hauptbranch-Commits.
Beide Workflows gruppieren künftig nach Workflowname, Ereignis und Git-Ref.
Mit `cancel-in-progress: false` bleibt ein bereits laufender Gruppentest
erhalten; ein neuer Commit ersetzt den älteren noch wartenden Lauf derselben
Gruppe. Push-, Pull-Request- und manuell gestartete Prüfungen sowie verschiedene
Branches und Workflows teilen keine Gruppe. Das entspricht der
[GitHub-Konkurrenzsteuerung](https://docs.github.com/en/actions/how-tos/write-workflows/choose-when-workflows-run/control-workflow-concurrency).
Bereits vor dieser Konfigurationsänderung ausgelöste Runs werden dadurch nicht
nachträglich umgruppiert oder abgebrochen; es wurden keine laufenden Jobs
manuell gestoppt. Die neuen Workflows werden erst nach ihrem tatsächlichen
Remote-Ende als neue Plattformnachweise bewertet.

Beide YAML-Dateien sind geparst, und die Workflow-/Ausdrucksprüfung mit dem
offiziellen **actionlint 1.7.12** endet ohne Befund, Log
`build/current-ci-workflow-lint.log`. Das macOS-Tool wurde lokal unter
`build/deps/actionlint/` aus der offiziellen Release-Datei eingerichtet;
deren SHA-256 stimmt mit der veröffentlichten Prüfsummenliste überein:
`5b44c3bc2255115c9b69e30efc0fecdf498fdb63c5d58e17084fd5f16324c644`.
Optionale ShellCheck-/Pyflakes-Prüfungen wurden dabei nicht ausgeführt.
Dieser statische Nachweis ist keine tatsächlich ausgeführte neue gesamte CI.

## Kugelstoß: C-/Physim-Laufparameter für Medien, Rotation und CCD

Der C-Runner am Stand `375eca4` weist `mediumMode=1` noch mit Exit 5 als
unbekannten Experimentparameter ab; die tatsächlich ausgeführte Gegenprobe
ist `build/current-sphere-parameters-before-mac/observed.json`.
Beide Vorlagen deklarieren jetzt dieselben **acht** Laufparameter mit SI-
Einheiten: Geschwindigkeit, Medium-/Widerstands-/CCD-Modus, Coulomb-Reibung,
Anfangsrotation und Dichte/Viskosität des eigenen Mediums. Die Mode-Felder
verlangen Ganzzahlen. Kontinuierliches CCD ist ausschließlich im Vakuum
zulässig; automatische Wahl verwendet dort den linearen Sweep und in Medien
diskrete Kontakte. C-Makros bleiben Startwerte innerhalb der gemeinsamen
Grenzen. Ohne explizites CCD-Makro ist der C-Startwert jetzt −1; die tatsächlich
ausgeführte Standardphysik im Vakuum beziehungsweise Medium bleibt erhalten.
Reset erhält die Parameter und leert nur Körper-/Kontaktzustände.

Die finale native Release-Prüfung besteht **3/3** in
`build/current-headless-mac/test-results/run-4xe9tw7t`, Log
`build/current-sphere-parameters-native-final-mac.log`: erweiterte Sprach-
Parität, unabhängige Kugelstoßreferenz und unabhängige grobe/feine CCD-
Referenz einschließlich absichtlichem diskreten Tunneln. **20** wirkliche
C-/Physim-Runnerläufe vergleichen jeweils alle **401** Zeitpunkte und **elf**
Kanäle: Standardvakuum, Luft, eigenes Medium, Reibung/Spin, entsprechende
C-Laufparameter statt Makro-Neubau, Wasser ohne Widerstand, eigenes Stokes-
Medium mit geänderter Dichte/Viskosität und diskreter Stoß mit anderer
Geschwindigkeit. Die Gegenprobe liest gewählte Parameter, SI-Deklarationen,
alle Dimensionen und tatsächliche Modell-Eigenschaften direkt aus den Dateien.

Zwei gleichzeitig lebende Instanzen je Sprache behalten unterschiedliche
Geschwindigkeit, Medium, Widerstandsmodell und Spin über zwei Resetzyklen;
die Gegenprobe prüft beide Anfangszustände nach jedem Reset sowie tatsächliche
Schritte und Kontakte. Alle **40** ungültigen Einstellungen werden mit Exit 5
vor Messdateianlage abgewiesen: Bereichsüberschreitungen, gebrochene Mode-Werte
und erzwungenes kontinuierliches CCD im Medium. Bestehende Szenen-, Report-
und CSV-Dateianlageprüfungen bleiben erhalten; letzteres ist keine zusätzliche
vollständige CSV-Inhaltsprüfung.

Die tatsächlichen Release-Appketten bestehen **2/2** in
`build/current-release-mac/test-results/run-r1ycks7m`, Log
`build/current-sphere-parameters-app-mac.log`. C und Physim wählen Reibung `.3`
und Spin `1`, bauen ihre Projektquellen, simulieren, lesen den aufgezeichneten
Lauf, prüfen Einheiten für Geschwindigkeit/Rotation/eigene Mediumswerte und
öffnen die Auswertung. Der neue C-Fall prüft zusätzlich beide gespeicherten
Werte in `physim.project`; der bestehende Physim-Fall öffnet das Projekt mit
den Einstellungen erneut. Die Änderung benötigt keine neue ABI-, Projekt-
oder Messdateiversion. Dies belegt Intel macOS; ganze aktuelle SDK-/Linux-/
Windows-Abnahme und die übrigen Vorlagen bleiben offen. PP-0066 bleibt
unvollständig. Der ältere C17-Lauf `7b08c04` hat inzwischen alle vier Windows-
Jobs einschließlich MSVC Release erfolgreich beendet; er prüft diese neue
Kugelstoß-Erweiterung noch nicht.

## Grundanforderungen der drei Arbeitsbereiche

Die aktuelle Release-App am Quellstand `06a30c9` besteht zwei zusätzliche
wirkliche Gesamtwege ohne Quelländerung: `pendulum` (C-Experiment und C-
Analyse) sowie `language_full` (Physim-Experiment und Physim-Analyse).
Die Prüfprojekte sind `build/current-workspace-core-proof-{c,phys}`;
Logs sind `build/current-workspace-core-proof-{c,phys}.log`, Ergebnissammlung
`build/current-workspace-core-proof-results.json`. Beide Prozesse enden mit
`APP SELF-TEST: PASSED`. Die Wege erzeugen absichtlich Compilerdiagnosen,
stellen die Quellen wieder her, bauen, simulieren, pausieren, führen einen
Einzelschritt aus, stoppen, laden ihre Datensätze, berechnen Berichte und
öffnen Dateien beziehungsweise Ergebnisse erneut. Der C-Weg zeigt zusätzlich
seine tatsächliche Berichtstabelle und exportiert ihre Daten.

Die konkrete Zuordnung zum Grundumfang ist:

| Planblock | Inspizierte Umsetzung und ausgeführter Nachweis |
| --- | --- |
| PP-0057: drei Hauptarbeitsbereiche | `app/toolbar_ui.inc` bietet genau Entwickeln, Simulieren, Auswerten; `app/design_ui.inc` wählt deren tatsächliche Inhalte. Aktuelle Bilder und beide Gesamtwege zeigen die Auswahl. |
| PP-0058: Experimenteditor für beide Sprachen | `document_language` in `app/document_actions.inc` unterscheidet Experiment-/Analyseeditor und Dateiendung. `syntax_text`/`editor` in `app/main.c` färben C beziehungsweise Physim und erhalten Cursor, Zeilennummern und Änderungen. C- und Physim-Quellen werden tatsächlich gebaut. |
| PP-0059: Simulation, 3D, Steuerung, Livewerte | Die aktuellen Kugelstoßbilder enthalten perspektivische Szene, Körper, Vektoren, Pause/Step/Fortsetzen/Stop und gemessene Position. Beide Gesamtwege prüfen tatsächlich fortschreitende und pausierte Zeit, Szeneninhalte, Einzelschritt und gespeicherte Werte. |
| PP-0060: eigener Analysecode, Daten, Tabellen, Plots | Aktueller C-/Physim-Code erzeugt und lädt Berichte. `app/report_ui.inc` liest tatsächliche Plot-/Tabellenobjekte; die neue C-Aufnahme zeigt SI-beschriftete Tabellenwerte. Der aktuelle Physim-Bericht wird zusätzlich unabhängig gegen seine ursprüngliche Laufdatei geprüft. |
| PP-0064: 2D-Modell in 3D-Szene | Der Kugelstoß bewegt Körper in XY bei Z=0; `viewport` und `app/graphics.c` verwenden dieselben 3D-Objekte, Tiefentest, Kamera und Projektionsmatrix. Aktuelle C-/Physim-Szenenbilder zeigen diesen echten Renderweg. |
| PP-0065: Start/Pause/Step/Fortsetzen/Stop | Gemeinsame IPC-/Buttonpfade sind inspiziert. Aktuelle Gesamtwege prüfen Start/Pause/Step/Stop; die dokumentierte Haltepunkt-Appkette prüft den echten Fortsetzen-Button und Reset für beide Sprachen/Fenstergrößen. |

Die zusätzliche tatsächliche Physim-Experimenteditor-Vorschau aus der aktuellen
`examples/language/collision.phys` endet erfolgreich:
`build/current-phys-experiment-preview.log` und `unicode-editor.bmp` im zugehörigen
Prüfordner. Sie zeigt `main.phys` mit sprachgerechter Syntaxfarbe und Zeilennummern.
Der frühere Screenshot `editor.bmp` aus dem Physim-Gesamtweg zeigte tatsächlich
den Analyseeditor; er wird nicht fälschlich als Experimenteditor-Nachweis verwendet.
Die sechs Kugelstoßaufnahmen und die neue Vorschau werden zur visuellen Prüfung
als PNG dekodiert unter `build/current-three-workspaces-preview/`.
`build/current-three-workspaces-receipt.json` erhält die Hashes der tatsächlich
gebauten Quellen, Szenenaufnahmen und Berichte am Stand `06a30c9`.

Der unabhängige öffentliche-API-Prüfer
`integration-pendulum-tutorial-report-probe` liest zusätzlich den gerade von der
Physim-App erzeugten Bericht und seine eigene Messdatei vollständig: zwei Plots,
drei Tabellen, SI-Achsen, vollständige Kurven, Energieabweichung, Messpunktzahlen
und Periodenwerte stimmen mit der ursprünglichen Messreihe überein.
Log: `build/current-workspace-phys-report-proof.log`. Der Physim-Appablauf erzeugt
kein eigenes Tabellen-Screenshot; seine Tabellen sind durch den realen Bericht
und diesen Datenvergleich belegt. Der C-Appablauf zeigt und exportiert die Tabelle.
Eine bytegetreu restaurierte ältere SDK-Tabellenaufnahme wurde ebenfalls angesehen;
sie wird vom aktuellen C-Bild getrennt gehalten.

Diese Zuordnung belegt die sechs knappen Grundanforderungen als vorhandene
Funktionen. Sie ersetzt weder sämtliche detaillierten Editor-/Analyseforderungen
in Abschnitt 6 noch eine vollständige UX-, Sprach-, Plattform- oder Produktabnahme.
Die neue unverkürzte Headless-Suite läuft separat unter
`build/current-headless-mac`, Log `build/current-parameters-complete-native-mac.log`;
ein Gesamtergebnis wird erst nach ihrem tatsächlichen Ende behauptet.

## Vollständige Headless-Abnahme nach den Parameteränderungen

Die neue unverkürzte Intel-macOS-Release-Suite ist tatsächlich beendet:
**660/660** in `build/current-headless-mac/test-results/run-oggi4py0`, Log
`build/current-parameters-complete-native-mac.log`. Alle 660 Ergebnisobjekte
haben Status `passed`. Sie erfasst die neuen C-/Physim-Material-, Wurf-, Box-
und Kugelstoßparameter sowie die bestehenden Core-/Sprach-/Runner-/Orakel-
und Dokumentationsfälle. Quellgrundlage ist `06a30c9`; danach änderten sich
zunächst nur die Auditdokumente. Die folgende App-Tastaturerweiterung ist
keine Eingabe der ohne SDL/App ausgeführten Suite und wird daraus nicht
als GUI-Abnahme abgeleitet.

## F8/F9 für tatsächliche Simulationssteuerung

Im Hauptfenster unter Simulieren führt **F8** genau einen Schritt eines
pausierten, fertig initialisierten Runners aus. **F9** verwendet denselben
Stop-/Timeoutpfad wie der Button und funktioniert auch vor HELLO bei hängender
Initialisierung. Gemeinsame Bereitschaftsprüfung verhindert einen Step bei
nicht verfügbarer Steuerung; wiederholte oder modifizierte Tasten werden
ignoriert. Editor, Handbuch und andere Hauptarbeitsbereiche lösen damit keine
Simulationsaktion aus. Beide Button-Tooltips und das Handbuch nennen die Kürzel.

Die tatsächliche Release-App besteht **2/2** Katalogfälle in
`build/current-release-mac/test-results/run-ivh7y06y`, Log
`build/current-runner-keyboard-mac.log`. Der bestehende Maus-Haltepunktfall
besteht für beide Sprachen und Fenstergrößen mit seinen vollständigen alten
Messdaten-/Speicher-/Reopen-Assertions. Der neue Tastaturfall führt **zehn**
erfolgreiche App-Prozesse aus: vier C-/Physim-/Fenstergrößenfälle, vier
gespeicherte Wiederöffnungen und zwei tatsächliche hängende create-Callbacks
mit F9-Stop und gesundem Neustart im selben App-Prozess.

Die vier Tastaturketten betätigen F6/F8/F6/F7/F6/F9 über wirkliche SDL-
Tastaturereignisse. Sie prüfen alle gespeicherten Messzeilen bei 3, 4 und 7
akzeptierten Schritten, gehaltene Zeit und erhaltene Quellen/Einstellungen.
Zusätzlich bleiben Runner und Messzeit unter wiederholtem F8/F9, dem macOS-
Command-Modifikator, F8/F9 im echten Hilfefenster sowie F8/F9 nach Cmd+1 im
Editor unverändert. Nach Cmd+2 arbeiten die Kürzel wieder. Die hängenden
C-/Physim-Fälle besitzen vor F9 tatsächlich noch kein HELLO; der Stop-Timeout
beendet den Runner, und der neue gesunde Lauf enthält exakt acht Messpunkte.
Dies ist eine konkrete zusätzliche Tastaturabnahme auf Intel macOS, keine
vollständige Barrierefreiheits-, SDK- oder gesamte Plattformabnahme.

## Linux/Clang am Stand 7b08c04 vollständig erfolgreich

Der [Linux-/Clang-Job 114433017259](https://github.com/PhysicSimulator/physim/actions/runs/38126895020/job/114433017259)
ist erfolgreich abgeschlossen. Sein authentifiziert gelesener Log belegt
**685/685** native Fälle und **90/90** Displayfälle, nochmals **685/685** mit
Sanitizern sowie **90/90** Sanitizer-Displayfälle. Der komplette verschobene
App-SDK-Lauf endet ebenfalls erfolgreich in `Native SDK ä xeqtytld`.
Beide Display-Suiten enthalten den tatsächlich bestandenen
`runner_isolation_workflow`; auch der zuvor gestörte Haltepunkt-Appfall bei
beiden Sprachen/Fenstergrößen besteht. Der genaue Auszug ist
`build/7b08-linux-clang-proof-excerpt.log`.
Die absichtlich fehlgeschlagenen inneren 1/3-Fälle des Testtreiber-Selbsttests
sind kontrollierte Prüfdaten und werden nicht als Produktfehler umgedeutet.
Dieser Linux-Nachweis betrifft `7b08c04`, nicht die spätere Parameter- oder
F8/F9-Erweiterung. Die aktuelle gesamte Plattformmatrix bleibt offen.

## Tastatursteuerung im verschobenen aktuellen SDK

Ein neues Release-App-SDK wurde tatsächlich installiert unter
`build/current-controls-sdk-mac`. Alle **471** Manifestdateien stimmen mit ihren
SHA-256-Werten überein. Die unabhängige Mappe besteht den Paketgrenzen-Selbsttest
mit **151** Eingaben (`build/current-controls-kit-selftest.log`); jede entpackte
Datei wurde erneut gegen ihr Manifest geprüft. Ihr SDK-Prüfer ist bytegleich
mit der neuen Quellfassung. Die Physim-Implementierung kommt aus dem Paket.

Der tatsächlich ausgeführte Prüfer aus `build/current-controls-verification-kit`
endet mit **PASSED** in
`build/current-controls-sdk-proof-mac/Native SDK ä nj6teowq`.
Der Aufruf verwendet `--isolation-only --archive-bitmaps` und erkennt den
Compiler automatisch. `PASSED.txt`, `verification.log`, beide `app-steps.json`
und `build/current-controls-sdk-proof-mac.log` belegen **18** erfolgreiche
App-Prozesse: acht create/step-Abbruch-/Endlosschleifenfälle sowie vier
C-/Physim-/Fenstergrößen-Tastaturketten, vier gespeicherte Wiederöffnungen
und zwei weitere hängende Initialisierungen mit Tastaturstop vor HELLO und
gesundem Neustart. Die F6/F7/F8/F9-Aktionen und ignorierten Repeat-/Command-/
Editor-/Hilfefenster-Eingaben laufen gegen die tatsächlich verschobene App.
Projektbuilder, Compiler, Beispiele und Core-Dateien stammen aus dem SDK.

Alle **42** BMP-Archive wurden danach vollständig gestreamt und gegen
Originalgröße sowie Original-/Archiv-SHA-256 nachgeprüft:
**608031396 Originalbytes**, **18033713 komprimierte Bytes**. SDK-Eingaben
bleiben geschützt. Der fokussierte Marker nennt seinen begrenzten Umfang;
er ist keine ganze Produkt- oder Plattformabnahme.

Die gemeinsame Isolationsfunktion des SDK-Prüfers führt nun auch die
Tastaturketten aus, sowohl unter `--isolation-only` als auch in der ganzen
`--app-tests`-Prüfung. Die neue unverkürzte Gesamtprüfung aus derselben
unabhängigen Mappe und demselben eingefrorenen Paket ist erfolgreich beendet:
`build/current-controls-full-sdk-proof-mac/Native SDK ä eq6mmrib/PASSED.txt`, Log
`build/current-controls-full-sdk-proof-mac.log`, vollständiges `verification.log`.
Die Prüfung umfasst installierten und aus SDK-Quellen neu gebauten Core,
unabhängige Fachorakel, tatsächliche native C-/Physim-Projektbuilds, neun
Appgesamtwege, Dokumentation, Typografie, Migration, Breakpoints, Runner-Isolation,
F6/F7/F8/F9 sowie sämtliche 32 Vorlagen-/Sprachpaare und unabhängige Analysen
bei 16/22 px. Weitere GUI-Prüfungen begannen erst nach dem terminalen Exit 0.

Alle **407** Screenshotarchive sind danach unabhängig vollständig gestreamt
und gegen Originalgröße, Original-SHA-256 und Archiv-SHA-256 geprüft:
**7026795366 Originalbytes**, **213113174 komprimierte Bytes**. Der Nachweis
steht in `build/current-controls-full-sdk-proof-archive-receipt.json`.
Jeweils alle **471** Manifestdateien im ursprünglichen und verschobenen SDK
sowie alle **151** Eingaben der unabhängigen Prüfmappe stimmen weiterhin mit
ihren SHA-256 überein; Eingangsbeleg
`build/current-controls-full-sdk-input-receipt.json`.

Dieser vollständige Lauf prüft seine zuvor eingefrorenen Eingaben. Die späteren
CCD-/Kontaktstapel-/Build-Herkunfts-/ABI-Prüfungs-/Metadaten- und Compileranzeigen-
Änderungen sind dadurch nicht als SDK-geprüft bezeichnet. Die aktuelle andere
Plattformmatrix und vollständige Barrierefreiheit bleiben weiterhin offen.


## CCD-Beispiel: typisierte Startgeschwindigkeit und vollständige Laufherkunft

Am 11. Oktober 2026 fehlten im C-/Physim-CCD-Beispiel die Geschwindigkeitseinheit
am Laufparameter und der tatsächlich gewählte Startwert in den Modellmetadaten.
Der erweiterte Test scheiterte mit den bisherigen Modulen an
`initial_speed_m_s`; das Reproduktionslog liegt unter
`build/current-ccd-provenance-before-mac.log`.

Beide Beispiele deklarieren `speed` jetzt als m/s mit Standard 10 und Grenzen
0..100. Die Modellbeschreibung speichert `initial_speed_m_s` aus dem tatsächlichen
Laufparameter. Geometrie, Solver und Standardphysik bleiben unverändert.

Der tatsächlich ausgeführte AppleClang-Release-Build ohne App besteht
`ccd_event_workflow`: **1/1 Katalogfall**, Ergebnisse
`build/current-headless-mac/test-results/run-f5stc8ip`, Log
`build/current-ccd-provenance-after-mac.log`. Der Fall führt **zwölf** vollständige
C-/Physim-Runnerläufe mit 0, 2, 4, 10, 20 und 100 m/s aus. Ein unabhängiges
1D-Orakel prüft freie Bewegung sowie erste und zweite elastische Impulsübertragung
bei den Kontaktzeiten 3/u und 6/u, Endlagen, Geschwindigkeiten, Ereignis-/Kontaktzahl,
Impuls und Energie. Beide Sprachwege stimmen überein. Der Parser prüft CRC,
Schema, zwei vollständige Samples, Abschlusszähler, dt, Seed und tatsächliche
Parameterwerte samt Standard, Grenzen, Einheit, Skala und Dimensionsvektor.
Vier Einstellungen außerhalb des Bereichs scheitern vor dem Anlegen einer Laufdatei.

Diese Prüfung gilt für Intel macOS. Die weiterhin laufende Gesamt-SDK-Prüfung
verwendet zuvor eingefrorene Eingaben und beweist diese spätere Beispieländerung
nicht. Eine aktuelle SDK-/Linux-/Windows-Gesamtmatrix bleibt offen.


## Kontaktstapel: tatsächliche Warmstart- und Solver-Einstellungen

Am 11. Oktober 2026 reproduzierte der erweiterte Parser fehlende Modellmetadaten
mit `KeyError: warm_start_fraction` gegen die bisherigen C-/Physim-Module;
Log `build/current-contact-provenance-before-mac.log`. Beide Beispiele verwenden
nun dimensionslose SI-Parameter mit Einheit `1`. Die Modellbeschreibung speichert
Warmstartanteil, Iterationsbudget, Boxmasse/-größe sowie Restitution, Reibung,
Bounce-Schwelle, Penetrationstoleranz und Projektionsanteil. Der C-Weg liest
seine tatsächlichen Solverwerte; Physim beschreibt die Werte seines Konstruktors.

AppleClang Release ohne App besteht **2/2** Katalogfälle:
`runner_contact_world` und `contact_world_language_parity`, Ergebnisse
`build/current-headless-mac/test-results/run-1fdkdxxd`, Log
`build/current-contact-provenance-after-mac.log`. Die bestehende unabhängige
Warm-/Kaltprüfung bewahrt den Bodenkontakt, validiert Kontaktlebensdauer und
aufgezeichnete Geometrie und verlangt geringere Warmstartreste. Der erweiterte
Sprachvergleich führt acht vollständige Läufe über je 1001 Samples aus:
kalt/warm mit vier Iterationen und halber Warmstart mit einer beziehungsweise
16 Iterationen. Alle Messwerte und Szenenfelder stimmen überein; CRC, Schema,
Abschluss, dt, Seed, Werte/Standards/Grenzen und SI-Dimensionen werden geprüft.
Zehn ungültige Bereichs-/Ganzzahleinstellungen erzeugen keine Laufdatei.
Die zuvor eingefrorene Gesamt-SDK-Prüfung enthält diese spätere Änderung nicht;
aktuelle andere Plattformen bleiben für diesen Änderungsumfang offen.

## Ausgeführte Linux-GCC-Gesamtprüfung am Stand 7b08c04

Der authentifiziert gelesene Actions-Job `114433017302` im Lauf `38126895020`
ist erfolgreich abgeschlossen. Der tatsächliche Log belegt **685/685** native
und **90/90** Fenstertests, jeweils auch unter Sanitizern. Runner-Isolation und
Schritt-Breakpoints bestehen in beiden Fenstersuiten. Die verschobene SDK-Prüfung
endet mit `Native SDK verified` unter `Native SDK ä q820fiwp`; sie umfasst die
32 Vorlagen-/Sprachpaare und reale Appketten. Der gefilterte lokale Nachweis liegt
unter `build/7b08-linux-gcc-proof-excerpt.log`.

Dies ist eine tatsächliche Linux-GCC-Ausführung am genannten Commit. Spätere
Parameter- und Tastaturänderungen sowie die beiden noch laufenden macOS-Jobs
werden damit nicht als vollständig geprüft bezeichnet.


## Build-Herkunft: Compiler, Profil und ausgewählte SDK-Versionen

Am 11. Oktober 2026 erweitert der native Projektbuilder seine erfolgreiche
`build.config` um `api_version` und `abi_version`. Die Werte werden aus dem
Core-Header des tatsächlich gewählten SDK gelesen. Der gemeinsame Leser
`ps_project_build_info_read` akzeptiert frühere Konfigurationen ohne diese
Felder, prüft Pflichtfelder, Duplikate, leere/ungültige Compilerpfade und
Versionsgrenzen und erhält Ausgabe und Quelldatei bei Fehlern.

AppleClang Release besteht **2/2** native Katalogfälle `project_file` und
`native_project_build`; Ergebnisse
`build/current-release-mac/test-results/run-tgoypdr_`, Log
`build/current-build-info-complete-native-mac.log`. Der echte Builder baut
C-/Physim-Projekte und unabhängige Analysen in Debug/Release, prüft
Headeränderungen, unveränderte Cache-Builds, Fehlerschutz, Locks und Verschieben.
Jeder erfolgreiche Aufruf vergleicht gespeicherten Compiler/Profil mit dem
tatsächlichen Aufruf und SDK-Versionen mit den Headern. Ein zusätzliches
kopiertes SDK mit API **4** und ABI **5** baut erfolgreich eine Analyse und
zeichnet genau diese Versionen auf. Der Parser prüft daneben CRLF, Unicode,
Windows-Pfade, frühere Konfigurationen, zehn fehlerhafte Konfigurationen sowie
fehlende Dateien und Argumente.

Dies ist ein nativer Intel-macOS-Nachweis der Build-Herkunft. Die anschließende
App-Anzeige ist nun separat geprüft (siehe unten). Die zuvor eingefrorene
SDK-Gesamtprüfung enthält diese spätere Änderung nicht.


## Einzelabnahme des versionierten Experiment-Lebenszyklus

Am 11. Oktober 2026 besteht AppleClang Release ohne App den neuen
`runner_experiment_abi`-Katalogfall: **1/1**, Ergebnisse
`build/current-headless-mac/test-results/run-27gn52kx`, Log
`build/current-experiment-abi-mac.log`. 17 tatsächlich kompilierte und vom
Runner geladene Module prüfen den aktuellen Grundvertrag. Vier gültige
Beschreibungen (aktuelle Größe, unbekanntes Capability-Bit, tatsächlich längere
Beschreibung und eingefrorene Grundgröße) erzeugen vollständige Laufdateien mit
exakten Zeiten/Werten, Schema, CRC und Abschluss. Alle Create-/Reset-/Step-/
Scene-/Destroy-Aufrufe werden im Modul selbst protokolliert.

Zwölf ungültige Varianten werden mit Exit 4 vor jedem Callback und ohne
Laufdatei abgewiesen: zu kurze Grundgröße, falsche ABI, fehlender Name,
fehlende Pflichtcallbacks, Scene Frames ohne Hierarchie, fehlender adaptiver
Callback, zu kurze adaptive Erweiterung und null Beschreibung. Ein gültiger
Vertrag mit fehlgeschlagenem `create` liefert Exit 5 und ruft genau einmal
`destroy` auf, ohne eine Laufdatei anzulegen.

Zusätzlich bestehen **3/3** aktuelle Fälle `language_experiment`,
`runner_hierarchy` und `runner_adaptive`; Ergebnisse
`build/current-headless-mac/test-results/run-qcaoxf8d`, Log
`build/current-experiment-abi-language-mac.log`. Sie prüfen tatsächliche
C-/Physim-Lebenszyklen, Create-/Step-/Scene-Fehler, Messwerte, Szene und Besitz,
die alte ABI-3-Szenenbeschreibung sowie feste/adaptive Läufe mit zu kurzer
adaptiver Beschreibung. Die neuen Module ändern keine Produktions-ABI.

PP-0262/0264/0276 sind damit für den grundlegenden, versionierten Callbackvertrag
konkret zugeordnet. Der Plan-Codeblock ist ein Entwurf; die endgültige Schnittstelle
verwendet `ps_result`, `ps_scene`, Größenfelder, Capability-Bits und eine optionale
adaptive Erweiterung. Die Originaltexte im Audit bleiben erhalten. Diese
Einzelabnahme ersetzt keine ganze Plattform- oder Produktabnahme.


## Feste Läufe: Herkunftsangaben werden nicht stillschweigend abgeschnitten

Am 11. Oktober 2026 erzeugte der bisherige Runner mit `fault=8` aus dem
vorhandenen Metadatengrenzfall einen festen Lauf mit Exit 0. Der unabhängig
CRC-geprüfte Metadatenchunk umfasste 2206 Bytes; `module_fnv1a64` und
`runner_build` fehlten. Die tatsächliche Reproduktion ist dokumentiert in
`build/current-fixed-provenance-before-mac.json`, der Lauf liegt unter
`build/current-fixed-provenance-before-mac/truncated.psrun`.

Die bereits vorhandene Kapazitätsprüfung für die ergänzten Herkunftsangaben
gilt nun ohne Sonderbedingung. Feste und interaktive feste Läufe brechen bei
zu langen Modellmetadaten vor dem Writer mit Exit 5 ab und geben den Modulzustand
frei. Die vorhandenen Prüfungen für adaptive Läufe, Zielzeit und Breakpoints
bleiben erhalten. Dateiformat, Modul-ABI und normale Metadaten ändern sich nicht.

AppleClang Release ohne App besteht **3/3** aktuelle Katalogfälle:
`runner_adaptive`, `runner_target_time` und `runner_experiment_abi`; Ergebnisse
`build/current-headless-mac/test-results/run-gakt_3dj`, Log
`build/current-fixed-provenance-after-mac.log`. Zwei neue Fälle verlangen Exit 5
und das Fehlen einer Laufdatei bei festen beziehungsweise interaktiven festen
Läufen mit maximaler Modellbeschreibung. Die bestehenden C-/Physim-Prüfungen
validieren adaptive Intervalle, Pause/Einzelschritt, Zielzeiten, gespeicherte
Daten und ABI-Grenzen. Der Nachweis gilt für Intel macOS; die laufende eingefrorene
SDK-Suite und andere Plattformen enthalten diese spätere Änderung noch nicht.


## Sichtbare Compiler- und API-Angaben in den Build-Einstellungen

Am 11. Oktober 2026 zeigt die App in den Build-Einstellungen ihre API/ABI und
nach einem erfolgreichen Build dessen tatsächlich gespeicherten Compilerpfad,
Profil und SDK-API/ABI. Vor dem ersten Build fehlen keine scheinbar gültigen
Compilerangaben: Die UI fordert ausdrücklich einen erfolgreichen Build an.
Projektwechsel leeren die Anzeige; ein Profilwechsel erhält die als **Letzter
Build** bezeichneten Angaben und verlangt erneut einen Build.

AppleClang Release besteht den tatsächlich ausgeführten
`project_settings_workflow`: **1/1** Katalogfall, drei App-Prozesse mit Schreiben,
Neustart/Build/Runner und erneutem Öffnen des geänderten Profils. Ergebnisse
`build/current-release-mac/test-results/run-h5qj6l6z`, Log
`build/current-build-info-ui-final-mac.log`. Das Modul wird wirklich gebaut,
seine Laufdatei mit exaktem Zeitschritt/UInt64-Seed gelesen und die tatsächlichen
Build-Angaben vor sowie nach dem Profilwechsel geprüft. Der Python-Nachprüfer
vergleicht die gespeicherten API/ABI-Felder und den vorhandenen Compilerpfad.

Die erste Darstellung schnitt die ABI ab und legte die Build-Version unterhalb
des sichtbaren Bereichs ab. Kürzere Beschriftungen und eine zusammenhängende
Anordnung beheben dies. Der abschließend tatsächlich gerenderte Screenshot
`build-info.bmp` des genannten Falls zeigt App-API/ABI, Build-API/ABI, Release
und `/usr/bin/clang` vollständig im kleinen 1080×740-Fenster. Die daraus erstellte
PNG-Vorschau `build/current-build-info-preview/build-info-final.png` wurde visuell
geprüft. Lange Compilerpfade besitzen zusätzlich einen vollständigen Tooltip.

PP-0250 ist damit für die geforderte Anzeige konkret belegt. Es handelt sich
um eine Intel-macOS-Einzelprüfung; neue andere Plattformen, jede Schrift-/Pfad-
Variante und die gesamte neue SDK-Matrix werden damit nicht pauschal abgenommen.


## Build-Fehler in Headern und Hilfsdateien tatsächlich anklicken

Am 11. Oktober 2026 suchte die App bei textuellen Diagnosen ausschließlich nach
der Haupt- oder Analysequelldatei. Dadurch fehlten Compilerfehler aus eingebundenen
Headern/Hilfsquellen in der anklickbaren Liste. Der neue eigenständige Parser
liest GCC-/Clang- und MSVC-Datei-/Zeilen-/Spaltenformate sowie frühere
`runtime error`-Zeilen. Die App erhält den vollständigen Quellpfad bis 4095 Bytes,
ordnet ihn anhand der tatsächlichen Datei der Hauptquelle, Analyse oder einem
zusätzlichen Dokument zu und öffnet fremde Quellen beim Klick. Strukturierte
Diagnosen ersetzen nur Meldungen derselben Datei; gleiche Basenamen genügen nicht.

Die native AppleClang-Release-Prüfung `compiler_diagnostic` besteht **1/1**;
Ergebnisse `build/current-headless-mac/test-results/run-eo5js2qx`, Log
`build/current-compiler-diagnostic-parser-final-mac.log`. Zehn Formatfälle prüfen
relative/absolute Pfade, Leerzeichen, Unicode, Windows-Laufwerkspfade,
Klammernamen, fehlende Spalten und maximale positive int-Koordinaten. Sechzehn
ungültige Zeilen, ein zu langer Pfad und null Argumente werden atomar abgewiesen.
MSVC-/GCC-Formatfälle sind hier Parserdaten, keine Ausführung dieser Compiler.

Die endgültige App-Prüfung `diagnostic_workflow` besteht **1/1** Katalogfall mit
**sechs** tatsächlich gestarteten App-Prozessen; Ergebnisse
`build/current-release-mac/test-results/run-kzfsw535`, Log
`build/current-compiler-diagnostic-identity-mac.log`, `app-steps.json`.
Ein echter Clang-Build scheitert an `include/helper α.h`, Zeile 2. F6 startet
weder Runner noch Laufdatei. Ein tatsächlicher Mausklick auf die Diagnose öffnet
den richtigen Header und wählt seine Fehlerzeile. Nach der Reparatur speichert
F5 das Dokument und baut das Projekt; F6/F9 starten und stoppen einen gesunden
Pendel-Lauf mit vollständigem Footer und mindestens drei Samples. Die Nachprüfung
findet genau eine Laufdatei und den gespeicherten reparierten Header.

Der fremde Quellenfall prüft zusätzlich `nested/main.c` und die tatsächliche
Hauptdatei `main.c`: Ihre gleichnamigen Diagnosen bleiben beide erhalten.
Die vier vorhandenen C-/Physim-Experiment-/Analysefälle bestehen mit strukturierten
Fehlern, Integritätsprüfung der Sidecars und Navigation zur richtigen Quelle.
Der Screenshot `compiler-helper-jump.bmp` zeigt den geöffneten Header, Zeile 2
und den anklickbaren tatsächlichen Compilerfehler. Die Darstellung wurde anhand
der PNG-Vorschau visuell geprüft.

PP-0248 und PP-0251 sind damit für Build-Fehlernavigation und Startfreigabe nach
erfolgreichem Build konkret belegt. Dies ist ein Intel-macOS-Nachweis. Aktuelle
Linux-/Windows-Appketten, die neue Gesamt-SDK-Matrix und alle denkbaren
Compiler-Ausgabeformate werden dadurch nicht pauschal abgenommen.


## Build-Protokoll neben anklickbaren Diagnosen lesen

Am 11. Oktober 2026 verdeckte die Diagnoseansicht die sonstige gesammelte
Textausgabe, sobald mindestens eine anklickbare Meldung vorhanden war. Das
Protokoll bietet jetzt **Ausgabe**/**Diagnosen** zum Wechsel. Die Ausgabe nutzt
denselben bestehenden schreibgeschützten, scrollbar kopierbaren 64-KiB-Puffer;
die Diagnoseeinträge bleiben beim Wechsel erhalten. Ein Projektwechsel stellt
die anfängliche Diagnoseansicht wieder her. Dies ist keine unbeschränkte
Archivierung des gesamten Prozessoutputs.

AppleClang Release besteht `diagnostic_workflow`: **1/1** Katalogfall mit
**sechs** echten App-Prozessen, Ergebnisse
`build/current-release-mac/test-results/run-jetkmn04`, Log
`build/current-build-output-ui-mac.log`. Der echte Headerfehler wird angeklickt,
dann per tatsächlicher Maustaste die Textausgabe geöffnet. Der Fall prüft
den enthaltenen Compilerfehler und weiterhin vorhandene Diagnosen, zeichnet
`compiler-helper-output.bmp` auf, wechselt zurück und führt Reparatur, Neubuild
und gesunden Lauf vollständig aus. Die PNG-Vorschau
`build/current-build-output-preview/compiler-output.png` wurde visuell geprüft.
Die vier C-/Physim-Runtimewege und die fremde Quelle bestehen ebenfalls.
Andere Plattform-/SDK-Gesamtprüfungen für diese spätere UI-Ergänzung bleiben offen.

## Tatsächlich ausgeführte Apple-Silicon-Gesamtprüfung am Stand 7b08c04

Der authentifiziert gelesene Actions-Job `114433017309` im Lauf `38126895020`
ist erfolgreich beendet. Sein Log nennt ausdrücklich `Image: macos-15-arm64`
und belegt **685/685** native Debug-Prüfungen, **91/91** Fenstertests sowie
**660/660** Prüfungen im Build `Debug-sanitized`. Die Sanitizerprüfung dieses
Jobs ist ohne App; sie ist keine Sanitizer-GUI-Abnahme.

Die gesamte verschobene SDK-Prüfung endet mit `Native SDK verified` unter
`Native SDK ä 3_pkglyh` und umfasst die 32 Vorlagen-/Sprachpaare mit tatsächlichen
Builds, Läufen und unabhängigen Analysen bei 16/22 px. LaunchServices startet
die gepackte App tatsächlich für Pendel- und vollständige Physim-Abläufe;
`LaunchServices verification complete` nennt `LaunchServices ä c4xd47kd`.
Schritt-Breakpoints und Runner-Isolation bestehen in den Fenstertests. Der
gefilterte lokale Logbeleg liegt in `build/7b08-macos-arm-proof-excerpt.log`.

Dies ist ein tatsächlicher ARM64-macOS-Nachweis am genannten früheren Commit.
Die späteren Parameter-, Tastatur-, Metadaten-, ABI-Prüfungs-, Compileranzeigen-
und Diagnoseänderungen sowie die ganze aktuelle Plattformmatrix bleiben gesondert
zu prüfen. Der Intel-macOS-Job desselben Laufs war beim letzten Abruf noch aktiv.


## Editor: ausgewählter Treffer und zusammenhängende Undo-/Redo-Schritte

Am 11. Oktober 2026 ersetzt die bisherige Suche nach einem tatsächlichen Klick
auf **Suchen**/**Ersetzen** den späteren Treffer im Kommentar statt des markierten
Treffers im Code. Die echte Reproduktion liegt in
`build/current-editor-replace-before3-mac.log` und
`build/current-release-mac/test-results/run-m6db4gm1`. Nach der Korrektur der
Trefferwahl bestätigt ein weiterer tatsächlicher Durchlauf das fehlerhafte Undo:
Aus `ä😀LONG` wird `ALTä😀LONG`, statt den Originaltext wiederherzustellen;
Log `build/current-editor-replace-atomic-before2-mac.log`, Ergebnisse `run-p3v6uwcl`.
Die Testeingaben wurden zuvor um die notwendigen Renderframes für widgetbasierte
Tastaturaktionen ergänzt; frühere Testinitialisierungs-/Timingfehler sind keine
Produktgegenbelege.

Ersetzen verwendet nun eine passende aktuelle Auswahl und die bereits atomare
UTF-8-Pasteoperation; leere Ersetzungen löschen die Auswahl in einem Undo-Schritt.
Ein fehlgeschlagenes Reservieren erhält den Quelltext. Die optionale lokale
Nuklear-Flagge `NK_EDIT_PRESERVE_UNDO` bewahrt die eingebettete Historie beim
Reaktivieren caller-eigener Code-Editoren. Kurzlebige `nk_edit_string`-Felder
behalten ihr bisheriges Resetverhalten. Neue Editorinitialisierung, Quellneuladen
und die vorhandenen Historiengrößen werden nicht geändert; der Patch ist in
`third_party/README.md` beschrieben.

AppleClang Release besteht **3/3** aktuelle GUI-Fälle `documents_small`,
`documents_large` und `editor_replace_workflow`, Ergebnisse
`build/current-release-mac/test-results/run-hj1q4pvl`, Log
`build/current-editor-replace-complete-mac.log`. Die abschließende Prüfung mit
expliziter sauberer Draftinitialisierung besteht **1/1** unter
`build/current-release-mac/test-results/run-or_jij56`, Log
`build/current-editor-replace-ready-mac.log`. Ein tatsächlicher App-Prozess
prüft Experiment-, Analyse- und zusätzlichen Editor: Cmd+F, echte Such-/
Ersetzen-Klicks, Rückkehr per Maus, Cmd+Z/Cmd+Shift+Z, exakten UTF-8-Text,
leeren Ersatz, Suchumlauf sowie alle Undo-/Redo-Endzustände. Die bestehenden
Dokumentfälle prüfen weiter Tab, Undo/Redo und Dokumentbedienung in zwei Größen.

Zwei native Fälle `editor_clipboard` und `ui_text_tail` bestehen ebenfalls:
`build/current-release-mac/test-results/run-t1vgxm7j`, Log
`build/current-editor-replace-native-mac.log`. Fixed-Buffer-Erschöpfung,
dynamische Allokationsfehler, UTF-8 und atomare Auswahlersetzung bleiben geprüft.
Drei tatsächliche Screenshots zeigen Suchleiste, Zeilennummern und die
C-Syntaxfarben für Kommentare, Schlüsselwörter und Stringliterale; die PNG-
Vorschauen unter `build/current-editor-replace-preview` wurden visuell geprüft.
Die Schrift zeigt für nicht enthaltene Emoji-Glyphen einen Platzhalter; die
UTF-8-Bytes bleiben im Test exakt erhalten.

PP-0244 ist damit für seinen grundlegenden Editorumfang konkret belegt.
Andere Plattformen, jede Syntax-/Schriftvariante und die neue Gesamt-SDK-Matrix
werden durch diese Intel-macOS-Einzelprüfung nicht pauschal abgenommen.


## API-Browser: Module, Datentypen, Funktionen, Einheiten und Beispiele

Am 11. Oktober 2026 scheitert der neue tatsächliche Handbuchtest vor der
Korrektur: `ps_vec3` ist auf der Referenzseite als Überschrift der Ebene 3
vorhanden, fehlt aber in der Inhaltsnavigation. `documentation_test_seek`
meldet am Schritt 4 das unerreichbare Ziel 20015. Log
`build/current-api-browser-before-mac.log`, Ergebnisse
`build/current-release-mac/test-results/run-dgzitkqu`. Die vorhandene
Modulauswahl war zuvor erfolgreich; dies ist ein konkreter Navigationsfehler.

Referenzseiten unter `docs/reference/` nehmen nun Überschriften bis Ebene 3
in sichtbare Inhaltsbuttons und dieselbe Tastatur-Fokusliste auf. Andere
Handbuchseiten behalten ihre bisherige Tiefe. Die vorhandenen 31 generierten
C-/Physim-Referenzen enthalten Modulbezeichnungen, öffentliche Typen,
Funktionssignaturen und verlinkte Anleitungen/Beispiele; der Generator prüft
sie gegen Header, registrierte Themen, Offline-Verweise und Dokumentgrenzen.

AppleClang Release auf Intel macOS besteht abschließend **5/5** echte
Fensterfälle: `api_browser_workflow`, `documentation_keyboard`,
`documentation_keyboard_22`, `documentation_keyboard_delayed` und die
beabsichtigte Unerreichbarkeitsprüfung `documentation_keyboard_unreachable`.
Ergebnisse: `build/current-release-mac/test-results/run-foi5iuan`; Log:
`build/current-api-browser-complete-mac.log`. Der neue Fall öffnet F1, wählt
per Tab/Return die Referenzgruppe und das Core-Modul, springt zu `ps_vec3`
und `ps_vadd`, wechselt zur Einheitenreferenz und `ps_quantity` und folgt
zur Einheitenanleitung. Dort wird ein tatsächlicher Codeblock angesteuert
und per Leertaste kopiert; der vollständige Zwischenablagetext muss exakt
mit dem Dokumentcode übereinstimmen. Vier Screenshots aus dem echten
Handbuchfenster wurden visuell geprüft, PNG-Vorschauen liegen unter
`build/current-api-browser-preview`. Das abschließende Beispielbild zeigt
den Konvertierungscode und den fokussierten Kopierbutton.

Die nativen Fälle `documentation` und `documentation_reference` bestehen
**2/2**, Ergebnisse `build/current-headless-mac/test-results/run-sefkiz3m`,
Log `build/current-api-browser-native-mac.log`. Der separate Generatorlauf
meldet ebenfalls `31 reference documents verified`. PP-0246 ist damit für
den grundlegenden Offline-API-Browser belegt. Autovervollständigung, sämtliche
Symbol-/Darstellungsvarianten und die aktuelle Linux-/Windows-/ARM-Matrix
werden durch diese Einzelprüfung nicht abgenommen.


## Tatsächlich beendete Intel-macOS-CI am Stand 7b08c04

Der authentifiziert gelesene [Intel-Job 114433017247](https://github.com/PhysicSimulator/physim/actions/runs/38126895020/job/114433017247)
ist inzwischen erfolgreich beendet. Sein tatsächlicher Log belegt **685/685**
native Debug-Fälle, **91/91** Fensterfälle und **660/660** Fälle im App-freien
`Debug-sanitized`-Build. Der Paket-/Relokationslauf endet erfolgreich unter
`Native SDK ä vealxjl_`, LaunchServices unter `LaunchServices ä j66_loab`.
Projektvorlagen per Tastatur, Schritt-Breakpoints und Runner-Isolation sind
im Fensterlog ausdrücklich erfolgreich. Der lokale Auszug liegt unter
`build/7b08-macos-intel-proof-excerpt.log`.

Die Jobs-Abfrage bestätigt damit alle acht Jobs dieses älteren Laufs als
erfolgreich abgeschlossen: Linux Clang/GCC, Intel-/ARM-macOS und die vier
Windows-Compiler-/Profilkombinationen. Die oben einzeln dokumentierten
Testzahlen stammen aus tatsächlichen Logs. Diese Ergebnisse bleiben dem
Quellstand `7b08c04` zugeordnet; später gepushte Änderungen benötigen ihre
eigenen Nachweise. Eine vorhandene oder grüne Konfiguration ist weiterhin
kein Beweis für den ganzen aktuellen Projektplan.


## Experimentvorlagen tatsächlich anlegen, bauen und auswerten

Am 11. Oktober 2026 besteht auf Intel macOS mit AppleClang Release der
aktuelle Fall `project_manager_keyboard_workflow`: Ergebnisse
`build/current-release-mac/test-results/run-fyuan1ee`, Log
`build/current-experiment-templates-mac.log`. Zwei echte App-Prozesse legen
bei **16/22 px** jeweils **32** Kombinationen aus acht Modellen, zwei
Experimentsprachen und zwei Auswertungssprachen an. Dazu kommen je zwei
eigenständige Analyseprojekte, insgesamt **68** Projekte. Alle werden
über F10, Pfeile, Tab und Enter im Projektmanager erstellt, mit F5 gebaut,
als Experiment aufgezeichnet beziehungsweise mit Messdaten versorgt und
an einem tatsächlich erzeugten Bericht mit mindestens einem Plot geprüft.
Der Python-Harness vergleicht Experiment- und Analysequellen bytegenau mit
der gewählten Vorlage und prüft Manifest, Sprache und Buildordner.

Die vollständigen Fortschrittsdateien nennen beide Male exakt die Projekte
00–33. Die Eingabequittung `build/current-experiment-templates-input-receipt.json`
ordnet die Prüfung dem App-Stand `678003a` und seinem während der Prüfung
unveränderten Binary-Hash zu. Diese Projektprüfung ist keine neue vollständige
fachliche Abnahme jedes physikalischen Modells.

Bei der Bildkontrolle war **Zurück zum Workspace** im 22-px-Formular
abgeschnitten. Die Beschriftung lautet nun **Abbrechen**; die Wirkung bleibt
das Schließen des Formulars. Der vorhandene Fehlerfall wurde um 22 px und
eine Aufnahme des fokussierten Abbrechen-Buttons ergänzt. Die abschließende
Prüfung `project_manager_keyboard_errors` besteht mit zwei echten App-Prozessen
bei **16/22 px**: Ergebnisse
`build/current-release-mac/test-results/run-9dw60_z8`, Log
`build/current-experiment-template-errors-final-mac.log`. Ungültiger Name,
Datei statt Zielordner und vorhandene Projektquelle ergeben einen Fehler;
der vollständige Originalinhalt der vorhandenen Quelle bleibt unverändert.
Escape sowie Tab/Shift+Tab/Enter schließen das Formular tatsächlich. Beide
Abbrechen-Aufnahmen wurden geprüft; die Beschriftung ist nun vollständig
sichtbar. PNG-Vorschauen: `build/current-experiment-templates-preview`.
Die vorherige unveränderte Fehlerprüfung bestand ebenfalls unter `run-afsfsaqw`.

Die Bedienungsanleitung beschreibt jetzt die getrennte Experiment-/
Auswertungssprache und den aktuellen Abbrechen-Button. PP-0245 ist für seinen
grundlegenden Vorlagenumfang belegt. Der einzelne kurze Beschriftungswechsel
erfordert keine erneuten 68 unveränderten Modellbuilds; dafür ist die aktuelle
Formularprüfung ausgeführt. Andere Plattformen und umfassende UI-/Modellabnahmen
bleiben ihren getrennten Anforderungen zugeordnet.


## Projektbaum: Quellen, Assets und Messläufe bedienen

Am 11. Oktober 2026 reproduziert `project_tree_workflow` einen konkreten
Fehler: Quellen und Textasset sind per Baum-Klick erreichbar, ein gültiger
Messlauf wird aber als Textdatei behandelt und mit **Kein gültiger UTF-8-Text**
abgewiesen. Die App bleibt beim Asset-Editor und hat keine Messpunkte.
Ergebnisse: `build/current-release-mac/test-results/run-cmg6zqbb`; Log:
`build/current-project-tree-before-mac.log`. Das Testprojekt enthält
`main.c`, `analysis.c`, `assets/note α.txt` sowie einen über die echte
Core-Schreibschnittstelle erzeugten Lauf mit drei Zeilen und gültigem Footer.

Die Dateinavigation erkennt nun `.psrun` unabhängig von Groß-/Kleinschreibung
und öffnet den Lauf über denselben Weg wie **Läufe & Berichte**. Der gemeinsame
`open_run_path` prüft geöffnetes Projekt, laufende Jobs, Pfadlänge und den
begrenzten Dateiheader vor dem Wechsel. Die bestehenden Hintergrundworker
lesen Messpunkte, Szenen und CRCs weiter. Ein ungültiger Header verändert
vorhandene Messdaten, Editorinhalte und Laufpfad nicht. Der Pfad wird vor
Zustandswechseln kopiert. Die geöffneten Dateien bleiben an ihrem Speicherort;
der Öffnungsvorgang ist kein Messdatenimport.

AppleClang Release auf Intel macOS besteht **3/3** Fälle
`project_tree_workflow`, `workspace_tree_workflow` und `timeline_workflow`,
Ergebnisse `build/current-release-mac/test-results/run-y1k3m3hi`, Log
`build/current-project-tree-complete-mac.log`. Der vorhandene Zeitleistenfall
führt je Sprache fünf echte App-Prozesse aus: neuer Lauf, Wiederöffnung,
Legacy ohne Szenenchunks, negative Legacy-Zeit und rekonstruierbares Ende.
Originaldateien bleiben nach deren Hashprüfung erhalten. Der allgemeine
Baumfall prüft verschachtelte Ordner und hinzugefügte Wurzeln.

Die abschließende Projektbaumprüfung mit zusätzlichem bytegenauen
Quell-/Assetvergleich besteht **1/1** unter
`build/current-release-mac/test-results/run-x9xx3xou`, Log
`build/current-project-tree-final-mac.log`. Acht tatsächliche Mausklicks
öffnen Experimenteditor, Analyseeditor, Assetordner, Textasset, schließen
dessen Ordner, öffnen den Laufordner und wählen gültigen beziehungsweise
headerungültigen Lauf. Die Projektquellen erzeugen keine zusätzlichen
Dokumente. Der gültige Lauf mit der Endung `.PSRUN` zeigt exakt Werte 4/5/6,
Zeiten 0/0,01/0,02 s, den Kanal `position.x`, Meterdimension und erfolgreich
gelesenen Footer. Der ungültige Header erhält alle drei Messpunkte und die
noch ungespeicherte Assetänderung. Der Harness prüft anschließend beide
Projektquellen gegen ihre Vorlagen und die unveränderte Assetdatei.

Drei echte Fensteraufnahmen sind visuell geprüft; die abschließenden PNGs
unter `build/current-project-tree-preview` zeigen Baum, Textasset und die
Messdatenkurve. Vor der Aufnahme wird der Mauszeiger aus dem Dateibaum
bewegt, damit dessen langer Pfadtooltip die Darstellung nicht verdeckt.
PP-0243 ist für den grundlegenden Projektbaum konkret belegt. Dies ist keine
Abnahme aller binären Asset-Vorschauen, der vollständigen Tastatur-/UI-
Bedienung oder anderer aktueller Plattformen. Die frühe Headerprüfung
verspricht auch keine Erhaltung früherer Messdaten bei Fehlern, die erst
während der vollständigen Hintergrunddekodierung entdeckt werden.


## Build-Schaltfläche, F5 und Debug/Release tatsächlich bedienen

Am 11. Oktober 2026 besteht auf Intel macOS mit AppleClang Release der neue
Fall `build_controls_workflow`: Ergebnisse
`build/current-release-mac/test-results/run-7b5p5bo6`, Log
`build/current-build-controls-mac.log`. Zwei echte App-Prozesse prüfen den
Mindestumfang PP-0247/0249 über tatsächliche Eingaben. Der erste baut ein
C-Pendelprojekt mit unmodifiziertem F5 in Debug, öffnet die Profilauswahl
per Maus und wählt Release. Die Auswahl invalidiert den vorhandenen Build;
F6 startet vor dem Neubuild keinen Runner und erzeugt keine Laufdatei. Ein
anschließender echter Klick auf **Build** erzeugt den erfolgreichen Release-
Build. Der Profilwechsel zurück zu Debug wird durch normales SDL-Quit mit
der bestehenden Projekt-Speicherlogik übernommen.

Der zweite Prozess öffnet dasselbe Projekt mit wiederhergestelltem Debug
und baut es per F5 erneut. Alle drei erfolgreichen Builds besitzen API/ABI,
tatsächlichen Compilerpfad, passende Moduldatei, passenden Profilwert und
übereinstimmende Quell-/Buildrevision. Der Python-Harness prüft beide
`build/Debug/build.config` und `build/Release/build.config`, die gespeicherte
Debug-Auswahl in `physim.project`, den existierenden Compiler und die
bytegenau unveränderten Experiment-/Analysequellen. Beide Buildkonfigurationen
nennen `/usr/bin/clang` und API 3/ABI 3.

Die UI nutzt für das Profilfeld jetzt dieselben Nuklear-Comboelemente
explizit, damit die tatsächlichen Rechtecke für Mausklicks erfasst werden
können. Zeilenhöhe, Begrenzung des Popovers und Profilauswahl bleiben nach
dem bisherigen Nuklear-Comboablauf erhalten. Auch der tatsächliche
Editor-Buildbutton wird für diesen Ablauf erfasst; der Test ruft seine
Buildaktion und die Profilauswahl nicht direkt auf. Die bestehende
Parameter-/Build-Speicherlogik ist unverändert.

Die bisherige Regression `project_settings_workflow` besteht ebenfalls
**1/1** mit drei echten App-Prozessen: Ergebnisse
`build/current-release-mac/test-results/run-ftzavw7d`, Log
`build/current-build-controls-regression-mac.log`. Sie prüft weiterhin
Release-Build, Profilwechsel, Persistenz, maximale Seedwerte, fehlerhafte
Seedablehnung, Zeitschritt, Parameter und Erhaltung eigener Manifestfelder
und Quellen. Drei aktuelle Screenshots zeigen Debug, Release und nach
Neustart wiederhergestelltes Debug samt App-/Build-API, Compiler und
Build-Schaltfläche; PNGs unter `build/current-build-controls-preview` wurden
visuell geprüft.

PP-0247/0249 besitzen damit begrenzte Einzelbelege. Dies beweist weder alle
Tastatur-/Schriftvarianten noch andere aktuelle Plattformen oder sämtliche
Compileroptimierungen. Die authentifizierte Actions-Abfrage für den
vorherigen Projektbaumstand `5a0a893` meldet dessen Läufe `38143321676`
(C17) und `38143321671` (Linux-Pakete) beim Abruf als pending. Der ältere
Paketlauf `38140783654` am Stand `1624d75` besitzt bereits einen erfolgreichen
Paketjob, seine Ubuntu-/Debian-SDK-/Fensterprüfungen liefen beim Abruf noch.
Diese Zustände sind keine abgeschlossenen Plattformnachweise.


## API-Autovervollständigung aus generierter Symboltabelle

Am 11. Oktober 2026 besitzt die App erstmals API-Vorschläge aus einer
generierten, mit Git verfolgten Tabelle. `tools/generate-completions.py`
verwendet die 30 dokumentierten C-Header-Module des Referenzgenerators,
registrierte Physim-Aufrufe und ihre Referenzsignaturen sowie die echten
Builtin-Typnamen aus dem Checker. Das Ergebnis enthält **621 C-Symbole**
(Funktionen, Typen, Konstanten) und **546 Physim-Aufrufe/Typen**. Registrierte
statische Fabriken werden von Instanzmethoden unterschieden; die Tabelle
enthält keine aus generischen Signatur-Platzhaltern erfundenen Typen.
`--check` erkennt veraltete Tabellen; normale App-Builds führen diesen
Abgleich vor dem Build aus. Zum Aktualisieren dient
`python3 tools/generate-completions.py`.

Eine unabhängige Kopie unter `build/current-completion-generation-h1z_cr47`
besteht zunächst `--check`. Eine neue C-API-Deklaration in ihrer kopierten
`core.h` macht die alte Tabelle ungültig; sowohl Generator-Abgleich als auch
normaler App-Build enden mit Fehler, bevor ein Buildverzeichnis entsteht.
Nach Regenerierung enthält die Kopie den neuen Namen und besteht wieder.
Die Originalheader und Originaltabelle wurden dabei nicht geändert;
`receipt.json` zeichnet alle fünf ausgeführten Schritte auf.

Die allokationsfreie Präfixsuche arbeitet mit Bytepositionen und prüft
UTF-8-Grenzen, Bezeichnergrenzen, Kommentare, Literale, begrenzte Präfixe
und Empfängerschreibweise. Ein Fehler erhält die Ausgabequery. Vorschläge
werden nach C/Physim und nach Namespace-Fabrik beziehungsweise Instanzaufruf
gefiltert. Lokale Variablen werden hierbei nicht semantisch typisiert: Bei
einem unbekannten Empfänger werden passende Instanzmethoden mit ihrem
zugehörigen API-Typ angezeigt. C-Strukturfelder und lokale Projektsymbole
sind nicht Teil dieser generierten Bibliothekstabelle.

Die sichtbare Liste lässt sich per **Ctrl+Leertaste** oder Button öffnen,
mit Pfeilen durchlaufen und per Enter, Tab oder echtem Klick übernehmen.
Bei offenen Vorschlägen werden neue SDL-Texteingaben vor der Übernahme
verarbeitet. Wortgrenzen reichen bis zum Ende des bestehenden Bezeichners;
UTF-8-Präfixe und vorhandene Empfänger bleiben erhalten. Übernahme benutzt
die atomare Nuklear-Pasteoperation und einen Undo-Schritt. Unveränderte
Übernahmen erzeugen keinen wirkungslosen Undo-Eintrag. Nach Mausübernahme
kehrt der Editorfokus zurück; Kontextwechsel und externe Klicks schließen
die Liste. Textdateien ohne C-/Physim-Modus bleiben bei ihrer Textbedienung.

Die abschließenden nativen Fälle `completion` und `completion_reference`
bestehen **2/2**, Ergebnisse
`build/current-headless-mac/test-results/run-9uwiy9jl`, Log
`build/current-completion-native-verified-mac.log`. Sie prüfen unter anderem
C-Funktion/Konstante/Typ, echte Sprachtypen, Instanzmethoden, statische
Fabriken, ein Array als Empfängervariable, begrenzte Ergebnisausgaben,
UTF-8-Positionen, Kommentare, verschachtelte Sprachkommentare, escapte
Literale und Fehlererhaltung. Beide Generator-Abgleiche bestehen ebenfalls.

Die GUI-Gesamtregression besteht **4/4**: `completion_workflow`,
`editor_replace_workflow`, `documents_small`, `documents_large`, Ergebnisse
`build/current-release-mac/test-results/run-r2lh9bs1`, Log
`build/current-completion-final-regression-mac.log`. Nach dem letzten
Empfängerabgleich besteht der Vorschlagsfall nochmals unter `run-qp6qok60`;
die abschließende echte Button-Öffnung besteht **1/1** unter
`build/current-release-mac/test-results/run-cn3rh8su`, Log
`build/current-completion-ui-button-mac.log`. Die abschließende zusätzliche
Leereditor-/Esc-Prüfung besteht **1/1** unter `run-wc13j6iy`, Log
`build/current-completion-ui-empty-mac.log`; die zuletzt geänderte Behandlung
leerer Buffer ist damit ebenfalls tatsächlich ausgeführt. Zwei reale App-Prozesse
prüfen je Experiment-, Analyse- und zusätzlichen Editor in C/Physim: echte
Ctrl+Leertaste- und Button-Öffnung, Pfeile, Enter/Tab/Maus, Übernahme mitten
im Wort, exakte UTF-8-Endzustände, Dirty-Zustände sowie einzelne Undo-/
Redo-Schritte. Texteingabe und Enter innerhalb desselben Ereignisframes,
aktualisierte Vorschläge, unveränderte Übernahme und Literalsperre sind
ebenfalls tatsächlich bedient. Sechs echte Fensterbilder unter
`build/current-completion-preview` wurden visuell geprüft.

Frühere Teststart-/Renderframefehler sind keine Produktgegenbelege: Der
Projektstart musste zuerst die Dateiliste abwarten, Mausübernahme wird erst
nach Release und nachfolgendem Renderframe geprüft. Danach wurde ein
tatsächliches Fokusproblem bei Undo nach Mausübernahme sichtbar
(`run-jrxxncd0`) und durch Rückgabe des Editorfokus behoben. Ein vollständiger
sprachsemantischer Editor, alle Darstellungsvarianten und die aktuelle
Linux-/Windows-/ARM-SDK-Matrix sind mit dieser Intel-macOS-Einzelprüfung
nicht abgenommen. PP-0255 ist für die geforderte Bibliotheks-Symboltabelle
mit Autovervollständigung konkret belegt.


## API-Dokumentation neben dem Code

Am 11. Oktober 2026 zeigt der Inspektor API-Dokumentation direkt neben
dem aktiven C-/Physim-Editor. Shift+F1 beziehungsweise **API-Referenz**
ermittelt den Namen an der Cursorposition. Mehrdeutige Instanzmethoden
werden über die bestehende Vorschlagsliste gewählt; **Referenz** öffnet
den ausgewählten Eintrag ohne Übernahme in den Code. Die API-Tabelle und
die vorhandenen Referenzdateien liefern Namen, Signaturen und Beschreibungen.
Die Seitenreferenz besitzt einen eigenen Parserbuffer und Scrollzustand.
**Handbuch** öffnet denselben Abschnitt im unabhängigen Handbuchfenster;
**Schließen** gibt den Inspektor wieder frei. Ladefehler veröffentlichen
keinen halbfertigen Zustand und erhalten die vorherige Referenz.

Die nativen Fälle `api_reference` und `documentation` bestehen **2/2**,
Ergebnisse `build/current-headless-mac/test-results/run-4u5yooy1`, Log
`build/current-api-reference-all-symbols-final-mac.log`. Neben exakten
Bezeichnergrenzen und Ausgabeerhaltung prüfen sie gemeinsame C-Konstanten,
PS_EXPORT als Textvertrag, benachbarte Sprachüberladungen und Typfamilien.
Alle **1.167** generierten API-Einträge lassen sich gegen die tatsächlich
gebündelten Seiten auflösen; **12** Sprachtypen benutzen den Abschnitt
„Statische Prüfung 0.1“ des Sprachhandbuchs.

Die abschließenden GUI-Fälle `api_reference_workflow`, `completion_workflow`
und `documentation_keyboard` bestehen **3/3**, Ergebnisse
`build/current-release-mac/test-results/run-6bf3k1rm`, Log
`build/current-api-reference-verified-mac.log`. Zwei echte App-Prozesse
prüfen die Seitenreferenz in Experiment-, Analyse- und Zusatzeditor jeweils
in C/Physim. Shift+F1, Editorbutton und echter Maus-Klick auf **Referenz**,
Handbuchsprung und Schließen sind tatsächlich bedient. Editorbytes und
Dirty-Zustände bleiben durch Referenzaktionen erhalten; eine zusätzliche
Texteingabe und ihr einzelner Undo-Schritt erhalten die angeheftete Referenz.
Sechs reale Fensterbilder aus `run-_uca9ujs` wurden unter
`build/current-api-reference-preview` visuell geprüft.

Ein echter Fehler wurde bei vier Vorschlägen sichtbar: Die neue Fußzeile
ragte aus dem bisherigen 250-Pixel-Listenbereich; der Referenz-Klick schloss
dadurch die Liste statt die Dokumentation zu öffnen (`run-29e57dsb`).
Der 280-Pixel-Bereich umfasst nun auch diese Fußzeile; der betreffende
Physim-Instanzmethoden-Klick besteht samt Vorschlagsregression. Frühere
Prüfungen vor dem ersten Renderframe waren Fixturefehler; mehrere simulierte
Zeicheneingaben benötigen mehrere Undo-Schritte und wurden für die gezielte
Einzel-Undo-Prüfung durch eine Eingabe ersetzt.

Diese Prüfung belegt PP-0258 auf Intel macOS mit AppleClang und SDL3.
Aktuelle Linux-/Windows-/ARM-Läufe, vollständige IDE-Sprachsemantik und
alle Dock-/Schriftgrößenvarianten sind damit nicht abgenommen.


## Parameterformular aus Experimentmetadaten

Am 11. Oktober 2026 ist **Experimentparameter** als eigener Abschnitt
oben im Inspektor erreichbar. Das bereits vorhandene Formular lag zuvor
innerhalb der aufgeklappten Laufeinstellungen. Name, Beschreibung,
Standard und inklusive Grenzen stammen weiterhin aus der realen
`--describe`-Abfrage des gebauten Moduls, einschließlich Anzeigeeinheiten.
Namen und Beschreibungen besitzen jetzt zur Breite passende umbrochene
Zeilen; ungültige Werte erhalten einen umbrochenen farbigen Hinweis.
Eingaben und Standardbutton gelten vor dem nächsten Lauf. Während des
Laufs und eines ausstehenden Resets bleiben die Bedienelemente gesperrt.

`parameter_catalog` und `language_parameters` bestehen **2/2**, Ergebnisse
`build/current-headless-mac/test-results/run-lghbit5t`, Log
`build/current-parameter-form-native-mac.log`. Die nativen Fälle prüfen
Atomizität fehlerhafter Metadaten, gültige und ungültige Auswahl, neue
Grenzen nach Rebuild, Dimensions-/Anzeigeeinheitenwechsel, genaue SI-
Standard-/Grenzwerte, Vorzeichen von Null, Unterlauf und Sprach-Runner-
Parameter. Die Filteranforderung `parameters` wählte keinen eigenen Fall;
die genannten zwei Fälle sind die tatsächlich ausgeführten Prüfungen.

Die abschließende GUI-Regressionsgruppe besteht **4/4**:
`parameter_form_workflow`, `project_settings_workflow`, `inspector_workflow`
und `api_reference_workflow`, Ergebnisse
`build/current-release-mac/test-results/run-k44cmzjy`, Log
`build/current-parameter-form-regression-mac.log`. Das neue Formular wird
in vier realen App-Prozessen bedient: C und Physim, jeweils Erstöffnung
und Neustart. F5 baut tatsächliche Module und liefert zwei deklarierte
Parameter. Ein echter Klick öffnet das Formular; Mausfokus, Cmd+A und
SDL-Texteingaben bearbeiten die Felder. `9 cm` liegt unter der inklusiven
Grenze `10 cm` und F6 startet keinen Lauf. **Standard** stellt `50 cm`
wieder her. `75 cm` und ein unskalierter Wert `3.5` werden ausgewählt,
per F6 gestartet und als **0.75 SI** beziehungsweise **3.5** in jedem
gespeicherten Sample samt Parametermetadaten geprüft. Ein tatsächlicher
Standardbutton-Klick während des Laufs bleibt wirkungslos. F9 beendet
den Lauf; normale Beendigung speichert die Auswahl. Der Neustart und F5
stellen `75`/`3.5` in den Metadatenfeldern wieder her. Genau ein Lauf
entsteht je Sprache, die Quelldateien bleiben bytegenau erhalten und die
Neustartprüfung ändert den gespeicherten Lauf nicht.

Default-, Fehler-, Auswahl- und Wiederöffnungsansichten in C/Physim wurden
visuell geprüft; acht finale Fensterbilder liegen unter
`build/current-parameter-form-preview`. Ein zunächst falscher Test-Klickbereich
vor dem automatisch gerenderten Tree-Header wurde korrigiert
(`run-rclj5a5h`). Der erste Physim-Fixture fehlte der verpflichtende
create-Callback; der Compiler wies sie korrekt ab (`run-p_bd41q5`).
Beide Fehler waren in Testansteuerung/Testquelle, nicht Parameterberechnung.
Der vollständige C-/Physim-Workflow bestand zunächst unter `run-50ncs71e`;
der finale Lauf prüft auch den farbigen, dynamisch umbrochenen Fehlerhinweis.

PP-0256 ist für das Metadatenformular auf Intel macOS 14.6.1 mit
AppleClang 16 und SDL3 konkret belegt. Die aktuelle Linux-/Windows-/ARM-
Ausführung dieser Änderung sowie alle Schrift- und Dockvarianten sind
noch offen. Die laufende Linux-Paketprüfung des vorherigen Commits
`80972a1` und dessen noch ausstehende C17-Matrix ersetzen diese Prüfung
nicht. Der Gesamtplan bleibt offen.


## Simulationslayout, Live-Werte und Laufereignisse

Am 11. Oktober 2026 folgt der Simulationsbereich dem Planlayout: Szenenbaum,
Darstellung und Kameraoptionen links, 3D-Viewport in der Mitte, Parameter
sowie Live-Messwert, Anzeigeeinheit, Kurve und Laufereignisse rechts.
Zeitleiste und Laufsteuerung liegen unter dem Viewport; der Protokollbereich
bleibt unten. Individuelle Dockanordnungen sind weiterhin möglich. Die
Live-Anzeige benutzt denselben Kanal und dieselbe Einheitenkonvertierung
wie zuvor, die Messdatei bleibt SI. Die Viewporthöhe berücksichtigt die
tatsächlich verfügbare Panehöhe und große Schrift. Der Parameterabschnitt
heißt jetzt kurz „Parameter“; Wiedergabe und Protokollbutton vermeiden
die in echten 22-Punkt-Ansichten beobachteten ungünstigen Wortumbrüche.

Die App besitzt eine Vorschau der neuesten acht validierten Experiment-
meldungen. Eine zusätzliche letzte Warnung/Fehlermeldung bleibt bestehen,
auch wenn Debugmeldungen die Acht-Eintrag-Liste verdrängen. Schweregrad
und Simulationszeit werden angezeigt. Der Textrenderer berücksichtigt
UTF-8, Zeilenumbrüche, CRLF und als Leerraum dargestellte Tabulatoren.
Die archivierte .pslog-Datei und Loggerbudget/Unterdrückungszusammenfassung
werden durch diese Vorschau nicht geändert. Ein erfolgreich gestarteter
neuer Runner und Projektwechsel leeren die Vorschau; Reset erzeugt wie
bisher einen neuen Lauf.

Die erste Regression besteht **5/5** unter
`build/current-release-mac/test-results/run-i1wte4hg`, Log
`build/current-simulation-layout-actions-mac.log`: Hierarchie, Inspektor,
Einheitenwechsel, Logging und Parameterformular. Die anschließend erweiterte
Gesamtgruppe besteht **8/8**, Ergebnisse
`build/current-release-mac/test-results/run-pjt375za`, Log
`build/current-simulation-layout-final-mac.log`. Ihre **40 echten App-
Prozesse** prüfen `reset_workflow`, `speed_workflow`, `hierarchy_workflow`,
`adaptive_workflow`, `logging_workflow`, `scene_frames_workflow`,
`runner_keyboard_workflow` und `live_inspector_22_workflow`. Tatsächliche
Widgets/Shortcuts bedienen Start, Pause/Fortsetzen, Schritt, Stop und Reset
in C/Physim. Feste/adaptive Daten, Parameter-/Seed-Erhaltung, hängende
Initialisierung und fehlgeschlagene Resetstarts, kleine/große Fenster,
Zeitleistenwiedergabe und Koordinatenrahmen sind enthalten. Die neuen
Geometrieprüfungen lokalisieren den Szenenbaum links, Live-Werte rechts
und Steuerung unter dem Viewport. Baumklicks erhalten den Snapshot und
die archivierte Datei. Der gemeinsame Scroll-Klickhelper verwendet nun
den tatsächlich betroffenen Bereich, einschließlich unverändert rechts
verbliebener Lauf- und Haltepunkteinstellungen.

Nach den abschließenden Typografieanpassungen bestehen **4/4** weitere
Fälle mit **18 App-Prozessen**: Geschwindigkeit, Logging, Parameterformular
und große Live-Ansicht, Ergebnisse
`build/current-release-mac/test-results/run-uvq9urtx`, Log
`build/current-simulation-layout-typography-mac.log`. C/Physim und eine
5000-Meldungen-Probe zeigen dieselben zwei gespeicherten Samples
(0 s/142, 0.25 s/142.25). Mehr als 4000 aus der UI-Vorschau verdrängte
Meldungen entfernen deren Warnung nicht; die Protokolldatei enthält weiterhin
die budgetierten 4096 Meldungen samt Unterdrückungszusammenfassung.
Die 22-Punkt-Fälle setzen danach tatsächlich zurück, zeigen 142 bei 0 s
ohne alte Warnung und ohne alte Verdrängungszahl und erhalten die vorherige
Messdatei per CRC. Reale Fensterbilder unter
`build/current-simulation-layout-preview` wurden auf normale/große Schrift,
mehrzeilige Warnung, Steuerung und linken Szenenbaum visuell geprüft.

Die native Taktung besteht **1/1** unter
`build/current-headless-mac/test-results/run-63gg7dn_`, Log
`build/current-simulation-layout-native-mac.log`. Runner-Taktung und
Logging bestehen **5/5** unter
`build/current-headless-mac/test-results/run-sybmsbba`, Log
`build/current-simulation-layout-runner-mac.log`. C-/Physim-Runner vergleichen
alle ersten **201** Zeitpunkte und Kanalwerte mit dem Offline-Referenzlauf
bei 0.5×, 4×, Offline und verzögertem Leser. Pause, Live-Geschwindigkeits-
wechsel, einzelner Schritt, begrenztes Scheduling-Aufholen, große Schritte
und ungültige Geschwindigkeiten sind ebenfalls geprüft.

PP-0282/0283/0284/0285, PP-0289/0290/0291/0293 und PP-0311 besitzen damit
begrenzte konkrete Intel-macOS-Nachweise. PP-0292 bleibt **incomplete**:
`send_message` verwendet fwrite/fflush synchron; ein blockierter Vorschau-
Empfänger kann die Runner-Schleife bremsen. Identische Zahlenfolgen und
separate Taktung beweisen keine vollständige Unabhängigkeit des Durchsatzes
vom Rendering. Eine asynchrone/begrenzt ausdünnbare Vorschauübertragung
mit zuverlässigen Steuerungs-/Lognachrichten und vollständig erhaltenem
Messstrom ist als nächster konkreter Teil noch offen. Aktuelle Linux-/
Windows-/ARM-Ausführung dieser Änderung und alle benutzerdefinierten
Dockvarianten sind nicht abgenommen.


## Asynchrone Runnerausgabe und Vorschau-Backpressure

Der vollständige geprüfte Nachweis bleibt in
[Archivband 2](platform-validation-history-2.md#asynchrone-runnerausgabe-und-vorschau-backpressure) erhalten.

## Analyseprojekte und Laufauswahl

Am 2026-10-11 auf Intel macOS 14.6.1 / AppleClang 16 tatsächlich gebaut
und ausgeführt: `library_selection_workflow` und `workflow_pointer_isolation`
bestehen **2/2** unter `build/current-release-mac/test-results/run-w0g5v3vw`,
Log `build/current-library-selection-final-mac.log`. Die erste Gruppe startet
vier Apps: C-/Physim-Analyse mit UI-Schrift 16 und 22 im Fenster 1080×740.
Neun gültige Laufdateien enthalten je fünf analytisch bekannte Positionen
mit unterschiedlichen Zeitrastern und Geschwindigkeiten. Echte Kästchen
wählen acht Läufe; der neunte bleibt mit explizitem Hinweis abgewiesen.
**Als Referenz** verschiebt den gewählten Lauf nach vorn und erhält die
übrige Reihenfolge. Entfernen der Referenz, Nachrücken und Nachauswahl
werden ebenfalls über die GUI geprüft. Die letzte Reihenfolge lautet
9, 1, 2, 3, 4, 5, 7, 8. Die echte Analyseaktion übergibt diese Reihenfolge
an den separaten Runner; sämtliche acht abgeleiteten Geschwindigkeitskurven
haben die unabhängigen Sollwerte 9, 1, 2, 3, 4, 5, 7, 8. Nach **Auswahl
leeren** folgt eine echte Einlaufanalyse mit Geschwindigkeit 3.

Python prüft beide Eingabemanifeste unabhängig: einbasierte Indizes,
Pfadreihenfolge, Dateigrößen und neu berechnete FNV-1a-64-Fingerabdrücke
von Modul und Eingängen sowie identische archivierte Analysequellen und
vorhandene Berichte. Die App prüft die gemessene Textbreite der Referenz-
und Analyseaktion. Fensterbilder sind ausgewertet; die Auswahlanzahl hat
nun eine eigene Zeile, die kurzen Aktionen passen bei beiden Schriftgrößen.
Die zweite Gruppe öffnet gespeicherte Läufe/Berichte in zwei Fenstergrößen
und provoziert fremde Fenstereingaben; Auswahl und bestehende Ergebnisse
bleiben unabhängig.

Der vorausgehende Stand besteht **3/3** unter `run-by6ntkxk`, Log
`build/current-library-selection-verified-mac.log`: Laufauswahl,
Fenstereingabeisolation und `analysis_projects_workflow`. Letzteres startet
vier Apps für unabhängige C-/Physim-Analyseprojekte: Erstellung, eigener
Build ohne Experimentmodul, Import unveränderter Rohdaten, echter Bericht
und erneutes Öffnen. Die erste Auswahlprobe `run-todk41yi` scheiterte in
Physim, weil synthetische Positionsdaten ohne Winkel/Energie irrtümlich
mit der Pendelvorlage analysiert wurden. Das Fixture verwendet jetzt
explizit die allgemeine Positionsanalyse; die Ablehnung wird nicht als
Produktfehler umgedeutet.

Die nativen Regressionen bestehen **4/4** unter
`build/current-headless-mac/test-results/run-ytgc0p5n`, Log
`build/current-library-analysis-regression-mac.log`: `language_analysis`,
`analysis_isolation`, `run_comparison`, `documentation_reference`.
Die Prüfung der generierten API-Referenz besteht zusätzlich **1/1** unter
`run-ef5vccdt`, Log `build/current-library-selection-docs-final-mac.log`.
PP-0319/0320 besitzen damit begrenzte lokale Nachweise. Aktuelle Linux-,
Windows- und Apple-Silicon-Ausführung dieser Änderung, allgemeine
Barrierefreiheit und vollständige Planabnahme bleiben offen.


## Blockweise Quantile und Analyse

Am 2026-10-11 auf Intel macOS 14.6.1 / AppleClang 16 tatsächlich gebaut:
Die alte Quantilfunktion lud alle Werte in einen sortierten RAM-Puffer.
Die neue Gegenprobe scheiterte deshalb mit einem temporären 8-KiB-Budget
bei 65.537 Werten, `build/current-headless-mac/test-results/run-d47j5uce`,
Log `build/current-quantile-before-mac.log`. Der Projektplan verlangt
blockweise Operationen auch für große Reihen. Kleine Eingänge bis 256
Werte behalten die einblockige Sortierung; große Eingänge verwenden nun
zwei feste Ranghistogramme und höchstens neun blockweise Durchläufe.
Die Typ-7-Definition bleibt erhalten: exakte benachbarte Ordnungsstatistiken,
anschließend dieselbe lineare Interpolation. Maskierte Beobachtungen zählen
nicht; signierte Null ist bei großer Auswahl numerisch dieselbe Null.
Es entstehen keine Scratch-Dateien, neuen Handles oder Quotaänderungen.

Die abschließenden C-/Physim-Proben bestehen **2/2** unter
`build/current-headless-mac/test-results/run-0xxmmwcw`, Log
`build/current-quantile-budget-mac.log`. Unabhängig per qsort sortierte
Referenzen umfassen 1, 255, 256, 257, 513 und 65.537 Werte, volle/teilweise
Masken, neun Wahrscheinlichkeiten, Duplikate, negative Werte, extreme
Exponenten, DBL_MAX, Subnormale und beide Nullzeichen. Konstantwerte,
vollständig fehlende Beobachtungen und ein einziger gültiger Wert in
einer großen Reihe sind geprüft. **6.400 Byte** temporäres Budget reichen;
**6.399 Byte** und injizierte Allokationsfehler liefern PS_MEMORY bei
unverändertem Ergebnis und Scratch-Verbrauch. Freigaben sind vollständig.

Ein gesonderter Lauf enthält **1.048.577** Samples mit analytischen
Positionswerten und Beobachtungsmaske. Die C-Analyse prüft kopierte UTF-8-
Metadaten/Kanaldefinitionen, SI-Exponenten, blockübergreifende und verkürzte
Endlesevorgänge, Quantile, Ableitung, Integral, gleitenden Mittelwert,
affine Transformation und Statistik. Der gemessene Allokationsbereich
steigt nur von **121.080** auf **127.480 Byte** und fällt nach Abschluss
vollständig zurück. Dieses Budget erfasst die kontrollierten Core-
Allokationen; C-stdio-/OS-Puffer, Stack und Prozess-RSS sind kein Bestandteil
dieser Zahl. Der kompilierte Physim-Analyseprozess liest dieselbe große
Datei, prüft Metadata/Schema/Blöcke und exakte beziehungsweise tolerierte
Quantile und erzeugt ein Histogramm mit **524.289** gültigen Beobachtungen.

Die vorausgehende Gruppe besteht **6/6** unter `run-q2y4pmsp`, Log
`build/current-quantile-verified-mac.log`: Basisreihen, Masken, neue
Streamingprobe, Sprach-Analyse, Sprach-Streamingprobe und API-Dokumentation.
Die erweiterte Gruppe besteht **5/5** unter `run-qzno2xmd`, Log
`build/current-quantile-final-mac.log`: neue C-/Physim-Proben, numerische
Extremfälle, SI-Reihen und Masken. Generierte Referenzen und alle 1167
Completion-Signaturen sind auf Aktualität geprüft. Die beiden ältesten
Abschnitte der aktuellen Plattformchronik wurden mit unverändertem
Nachweistext ins Archiv verschoben; ihre ursprünglichen Anker verweisen
weiterhin dorthin. Beide Seiten bleiben unter der Handbuchgrenze.

Die abschließende Prüfung der generierten API-Referenz besteht **1/1**
unter `run-8ly8e53a`,
Log `build/current-quantile-docs-mac.log`.

PP-0321/0322/0340 besitzen damit begrenzte lokale Funktionsnachweise.
Aktuelle Linux-/Windows-/Apple-Silicon-Ausführung dieser Änderung sowie
vollständige Plattform-, Sprach- und Planabnahme bleiben offen.


## Berichtdetails, Buildkopien und Exportnachweise

Am 2026-10-11 auf Intel macOS 14.6.1 / AppleClang 16 tatsächlich gebaut:
**Details** beziehungsweise **Herkunft & Quellen** zeigt vollständige,
scrollbare Herkunftsangaben und die zugehörige Quellkopie, Eingabemanifest
und Buildkopie. Leere Berichte besitzen dieselbe Ansicht. Begleitpfade
werden ausschließlich vom geöffneten Berichtsnamen abgeleitet; Pfade im
Herkunftstext werden nicht interpretiert. Neue Analysen archivieren
`build.config` exklusiv als `<präfix>.build.txt`, zusätzlich zu Quellkopie,
Eingabemanifest und vorhandenen Laufgrenzen. Ältere Begleitdateien bleiben
optional. Anzeige und Kopieren führen keine Quellen aus und verändern
keine Projekteditoren oder Archive. Der Textzugriff bleibt auf 256 KiB
und gültiges UTF-8 begrenzt; CRLF/CR wird nur für die Anzeige vereinheitlicht.

Die Abschlussgruppe besteht **3/3** unter
`build/current-release-mac/test-results/run-k1y770v1`, Log
`build/current-report-details-confirmed-mac.log`: `report_details_workflow`,
`analysis_projects_workflow`, `editor_replace_workflow`. Die Detailgruppe
startet vier Apps für C-/Physim-Quellkopien bei UI-Schrift 16/22 im Fenster
1080×740. Sie kopiert alle langen Herkunftsangaben und sämtliche originalen
Quell-/Manifest-/Buildbytes, bestätigt echten Textfokus, versucht Eingabe
und Löschen und scrollt zum sichtbaren letzten Quellmarker. Unsaved-
Projekttexte, Dirty-Status und archivierte Dateien bleiben erhalten.
Ein ungültiger Bericht erhält bisherigen Bericht und Cache; ein gültiger
leerer Bericht ersetzt beide. Fehlende Begleitdateien zeigen eine Erklärung
und deaktivieren Kopieren, ohne das Ergebnis zu verwerfen. Eine gefälschte
Quellpfadangabe im Herkunftstext wird ignoriert. Fensterbilder sind geprüft.

Der echte unabhängige Analyseworkflow startet vier weitere Apps in C/Physim:
Producer, Import, eigener Analysebuild, Bericht, Anzeige der Quellkopie,
Eingänge und Buildkopie sowie erneutes Öffnen. Archivierte Quellen und
Builddateien stimmen bytegenau mit den tatsächlichen Build-Eingängen und
Konfigurationen überein. Python prüft unveränderte Fingerabdrücke aller
Begleitdateien, Rohdaten und Berichte nach Neustart. Der unabhängige Editor-
Workflow prüft zusätzlich atomare Änderung, Fokuswechsel und Undo/Redo.

Die erste sichtbare Detailgruppe und Plotgruppe bestanden **2/2** unter
`run-3l384yly`; eine strengere Fokusprobe deckte anschließend auf, dass
Nuklear bei readonly Feldern alle Eingaben einschließlich Navigation
unterdrückte (`run-kjfh_5y9`/`run-d6kfuhaa`). Der vendorte Patch filtert
nun gezielt Mutationen und erhält Auswahl, Kopieren und Navigation.
Ein weiterer echter Durchlauf `run-9hae5h6r` zeigte, dass Scroll-End sowohl
Textfeld als auch äußeres Panel bewegte; aktive Felder konsumieren jetzt
die Scrolltasten. Die Layoutberechnung wächst beim äußeren Scrollen nicht.
Der falsche Release-Sollwert im unabhängigen Projektfixture wurde auf das
tatsächlich gewählte Debug-/Release-Profil korrigiert. Diese Fehlversuche
werden nicht als bestandene aktuelle Produktprüfungen gezählt.

`editor_readonly` und `editor_clipboard` bestehen **2/2** unter
`build/current-release-mac/test-results/run-iujqd4bl`, Log
`build/current-editor-readonly-tested-mac.log`. Das native Widgetfixture
prüft echte Aktivierung, Auswahl/Kopieren, unterdrückte Texteingabe,
Delete/Backspace, Cut/Paste, Undo/Redo, Insert-/Replace-Modus, Enter/Tab,
direktes Scrollen zur Zielzeile, konsumierte Scrolltasten und inaktive
Disabled-Felder. Quellen und Undo-Historie bleiben erhalten.

Native Report-/Sprach-Report-/Plot-View-/Handbuchparser-Regressionen bestehen
**4/4** unter `build/current-headless-mac/test-results/run-__qb8_17`, Log
`build/current-report-regression-mac.log`. Die GUI-Gruppe `run-9hae5h6r`
besitzt **3** bestandene Fälle (bei insgesamt zwei weiteren fehlgeschlagenen
Detailfixtures): Plotworkflow, Logging und Editorwechsel. Der Plotworkflow
prüft reale Linien-/Punkt-/Histogramm-Anzeige, Zoom/Pan, wieder geladene
Berichte, CSV-/SVG-/PNG-Exporte, Bildgrößen und Ausschnitte; PNG-Dateien
werden unabhängig auf Pixel, Kompression und CRC geprüft. Logging deckt
die ebenfalls schreibgeschützte Protokollansicht ab. Die Referenzgenerator-
Prüfungen früherer Abschnitte wurden in ihrer Benennung korrigiert: sie
waren keine Parserläufe. Die tatsächliche Prüfung beider großen Handbuch-
seiten besteht separat in `run-s5mo0ln_`, Log
`build/current-report-doc-parser-mac.log` und der genannten Vierergruppe.

Die abschließende Parser- und Referenzprüfung besteht **2/2** in
`build/current-headless-mac/test-results/run-na6sakbf`, Log
`build/current-report-details-docs-mac.log`.

PP-0323/0324/0325/0326 besitzen damit konkrete lokale Nachweise.
Aktuelle Linux-/Windows-/Apple-Silicon-Ausführung dieser Änderungen,
vollständige Barrierefreiheit und gesamte Projektabnahme bleiben offen.


## Linux-Sanitizerfehler in UTF-8 und Editorfixtures

Der tatsächlich beendete [C17-Lauf 38152656546](https://github.com/PhysicSimulator/physim/actions/runs/38152656546)
gehört zu **bb1a369e41175b41b49e297b3c27d77ea8d2bb9f**, nicht zum aktuellen
HEAD. Beide macOS-Jobs und alle vier Windows-Jobs bestehen. Unter Linux
bestehen mit GCC/Clang jeweils **690/690** normale native Fälle,
**100/100** normale Grafikfälle und **690/690** native Sanitizerfälle.
Die Sanitizer-Grafikgruppen enden jedoch jeweils mit **97/100**:
`project_tree_workflow`, `completion_workflow`, `api_reference_workflow`.
GCC-Job **114517919387**: normale native/Grafik- sowie Sanitizer-native/Grafik-
Resultate `run-omf7yb2x`, `run-0uf3ucin`, `run-z3d01fq6`, `run-mmleuqvj`.
Clang-Job **114517919564**: `run-0oeesjjj`, `run-x7i762gl`,
`run-2g9mv5jz`, `run-_nz5v88b`. Die vollständigen Joblogs wurden gelesen;
absichtlich fehlgeschlagene Selbstprüfungen des Test-Runners werden nicht
als Produktfehler gezählt.

Der API-Referenzfall deckt einen echten Heap-Überlauf beim Undo auf:
`nk_textedit_undo → nk_str_delete_runes → nk_str_at_rune → nk_utf_decode`.
Beim Löschen verschob Nuklear den Bytebufferzeiger zum Anfang des Bereichs,
behielt aber die ursprüngliche Byteanzahl. Die neue Gegenprobe liest daher
hinter einem exakt dimensionierten 15-Byte-Buffer. Auf Intel macOS mit
AppleClang 16 ist derselbe Überlauf tatsächlich mit ASan reproduziert,
Log `build/current-utf8-string-before-asan.log`; die Grenze und Stackfolge
entsprechen den Linux-Logs. Beide Endpunkte werden jetzt gegen den
ursprünglichen Buffer aufgelöst. Suffixentfernung verwendet eine exakte
Byteanzahl, und vollständiges Entfernen setzt Byte- und Runeanzahl zurück.

Die beiden übrigen CI-Fehler liegen in den Prüfungen: ein Vergleich rief
`memcmp(NULL, ..., 0)` auf; ein anderer verwendete `strcmp` auf einem
längenbasierten, nicht nullterminierten Editorbuffer. Beide verwenden
jetzt dieselbe strikte Textgleichheit mit expliziter Länge und behandeln
leere Texte ohne ungültigen Pointeraufruf. Ein entsprechender leerer
Vergleich im Editorwechsel-Fixture ist ebenfalls korrigiert.

Lokale tatsächliche Nachweise auf Intel macOS 14.6.1:

- Exakte UTF-8-Bufferranges und Suffixentfernung mit ASan/UBSan, Log
  `build/current-utf8-string-verified-asan.log`.
- Clipboard/Unicode/Undo/Redo/Ersetzung und readonly Fokus/Kopie/
  Mutationsabwehr ebenfalls tatsächlich mit ASan/UBSan, Logs
  `build/current-editor-clipboard-asan.log` und
  `build/current-editor-readonly-asan.log`. Diese isolierten Compilerläufe
  verwenden AppleClang und benötigen keinen ld64.lld. LeakSanitizer ist
  auf diesem Host nicht unterstützt, daher `detect_leaks=0`; das ist kein
  Leck- oder vollständiger App-Sanitizernachweis.
- Die drei nativen Regressionen bestehen **3/3** unter
  `build/current-release-mac/test-results/run-nw1q08gx`, Log
  `build/current-utf8-native-regression-mac.log`.
- Die drei betroffenen echten GUI-Workflows plus Editorwechsel bestehen
  **4/4** unter `run-mame86d7`, Log
  `build/current-linux-failure-regression-mac.log`, einschließlich C/Physim,
  leerem Completion-Editor und zusätzlichem Textdokument.

Die tatsächliche Handbuchparser-/Referenzgruppe besteht **2/2** in
`build/current-headless-mac/test-results/run-tkhbp63t`, Log
`build/current-linux-fixes-docs-mac.log`.

Für die tatsächliche Linux-Nachprüfung ergänzt ein eigenständiger Workflow
GCC/Clang mit drei Sanitizer-Primitivefällen und den drei betroffenen
Sanitizer-GUI-Gruppen. Er benutzt dieselbe gepinnte SDL-Version, Xvfb/Mesa
und Openbox; die vollständige C17-Matrix bleibt erhalten. Die YAML-Struktur
und Compilermatrix sind lokal geprüft. Ein aktuelles Linux-Ergebnis der
Korrektur wird erst nach tatsächlicher Ausführung als bestanden gewertet.

Der ebenfalls beendete [Linux-Paketlauf 38156557421](https://github.com/PhysicSimulator/physim/actions/runs/38156557421)
gehört zu **162f5e5b974ca693106878b0faa7a4fde23aa0a3**. Paketbau und
beide Prüfjobs auf frischem Debian 12 und Ubuntu 24.04 bestehen. Dies ist
ein positiver historischer Paket-/Relokationsnachweis, kein aktueller
Sanitizer- oder gesamter Plattformnachweis der jetzigen Änderung.


## Linux-Sanitizernachweis und Kanalzugriff per Index

Der [fokussierte Linux-Lauf 38167610199](https://github.com/PhysicSimulator/physim/actions/runs/38167610199)
ist für **12c24626c4f7fde43d906661b32a61a46920ba19** tatsächlich beendet.
Clang-Job **114553322481** besteht **3/3** Sanitizer-Primitivefälle in
`run-i6duowk2` und **3/3** Sanitizer-GUI-Gruppen in `run-cqibikmd`.
GCC-Job **114553322669** besteht dieselben Gruppen in `run-d9jxugop`
und `run-ghd6c9t9`. Joblogs sind gelesen: UTF-8-Ranges, Clipboard,
Readonly-Editor, Projektbaum, Completion und API-Referenz bestehen ohne
ASan-/UBSan-Fehler. Grafik verwendet Xvfb/Mesa/Openbox und deaktiviert
nur dort LeakSanitizer. Dies bestätigt die konkrete UI-Korrektur unter
beiden Linux-Compilern, nicht die gesamte aktuelle Matrix.

Bei der folgenden Kanalprüfung zeigte sich eine andere Inkonsistenz:
Experimente dürfen einen Messkanal `time` deklarieren, die Analyseöffnung
wies solche gültigen Laufdateien aber wegen ihrer reservierten Zeitachse
ab. Die tatsächliche Gegenprobe `run-i3warwhz`, Log
`build/current-dataset-clock-before-mac.log`, erzeugt einen solchen Lauf
und scheitert beim Öffnen. Statt bestehende Kanalnamen zu verbieten,
öffnet die Analyse diese Dateien nun und bietet
`ps_dataset_channel_series` beziehungsweise `Dataset.channelSeries(index)`.
Der nullbasierte Index adressiert ausschließlich aufgezeichnete Kanäle;
`series("time")` bleibt die synthetische Laufzeitachse. Doppelte
aufgezeichnete Namen bleiben ungültig. Metadaten, Einheit, Alignment und
Generation/Lebensdauer bleiben erhalten; API-/ABI-/Dateiformate bleiben
unverändert, die Erweiterung ist additiv.

Auf Intel macOS 14.6.1 / AppleClang 16 besteht die Abschlussgruppe **6/6**
in `build/current-headless-mac/test-results/run-h4bsfjcr`, Log
`build/current-dataset-channel-index-verified-mac.log`: neue C-/Physim-
Indexerfälle, Completion-/Referenzkataloge, C-/Physim-Kanaldeklarationen
und SI-Reihenworkflow. Eine vorherige Gruppe besteht **7/7** in
`run-y9m5128n`, Log `build/current-dataset-channel-index-mac.log`:
Indexerfälle, Basisreihen, SI-Reihen, Kanaldeklaration, Quantity-Kanäle
und kompilierte Sprachanalyse. Die Indexerprobe unterscheidet für 513
Samples Laufzeit `0.125*i` von Messgerätezeit `1000+5*i`. Derselbe
Positionsverlauf hat daher Geschwindigkeit **16 m/s** über der Laufzeit
und **0.4 m/s** über der Messgerätezeit. Reportdaten werden unabhängig
gelesen und gegen alle Messpunkte geprüft. Blockgrenzen, SI-Dimensionen,
fremde/geschlossene/neu belegte Datasets, ungültige Indizes und volle
128 Reihenplätze erhalten Ausgaben und Scratch-Quota.

Die abschließende Handbuchparser-/Referenzgruppe besteht **2/2** in
`run-3oef9bhn`, Log `build/current-channel-index-docs-mac.log`.

Die SI-/Quantity-/Parametereinheiten-Orakel bestehen **3/3** in
`run-0x3aqf5o`, Log `build/current-si-boundaries-oracles-mac.log`.
Die GUI-Gruppe besteht **3/3** in
`build/current-release-mac/test-results/run-vga08yn2`, Log
`build/current-dataset-channel-index-gui-mac.log`: Anzeigeeinheiten mit
C/Physim und Neustart, Completion und API-Referenz. Die generierten
Kataloge enthalten jetzt **622 C-** und **547 Physim-Symbole** und sind
auf Aktualität geprüft. PP-0371/0372 besitzen begrenzte lokale
Metadaten-/Einheitengrenzen-Nachweise. Der neue Indexer hat noch keine
aktuelle gesamte Linux-/Windows-/Apple-Silicon-Abnahme; der positive
Linux-Lauf oben gehört ausdrücklich zur vorausgehenden UI-Korrektur.

## Lineare und gesamte Drehimpulsabfragen (PP-0381/0382)

Ausgangspunkt am 11. Oktober 2026 ist
`5283d5e930dd230a8c4065c863533127fb3fb8d2`. Zwei additive öffentliche
C-Abfragen und Physim-Methoden liefern Weltimpuls sowie Bahn- und Eigendrehimpuls
um einen beliebigen endlichen Weltpunkt. Körperlayout, ABI und Dateiformate
bleiben unverändert. Die Drehimpulssumme verwendet exakt akkumulierte binäre
Produkte nach einer normalisierten Double-Rotation; deren Rundungsfehler bleiben.
Fehler erhalten die C-Ausgabe. Es entsteht kein Heap-Speicherbedarf.

Tatsächlich ausgeführt auf Intel macOS 14.6.1 mit AppleClang 16, Release:

- `build/current-body-momentum-verified-mac.log`, Ergebnisse
  `build/current-headless-mac/test-results/run-a8x2gq36`: **7/7 bestanden**.
  Neue analytische C-Prüfung und kompilierte Physim-Ausführung; bestehende
  Mechanik-, Kontaktgraph-, Zwangsgraph-, Kontakt- und Körperbereichsreferenzen.
  Analytische Werte, gedrehte anisotrope Trägheit, Verschiebung des Ursprungs,
  außermittiger Impuls sowie Gesamtimpuls/Drehimpuls bei elastischem,
  inelastischem und reibendem Kontakt ohne Positionskorrektur sind geprüft.
- `build/current-body-momentum-oracle-mac.log`, Ergebnisse
  `build/current-headless-mac/test-results/run-mys4gcdy`: **1/1 bestanden**.
  `tests/test_body_momentum_oracle.py` vergleicht **1012** Fälle mit unabhängigen
  exakten Python-Fraction-Referenzen für Identitätsorientierung. Zufallswerte
  über den Binary64-Bereich, überlaufende Einzelterme mit endlichem Rest,
  kleinste Unterlaufwerte und atomare echte Überlauffehler sind enthalten.
- `build/current-body-momentum-asan.log`: direkter AppleClang-Build der
  Mechanik/Core-Quellen und des neuen C-Tests mit AddressSanitizer und
  UndefinedBehaviorSanitizer **bestanden**. LeakSanitizer ist auf diesem
  macOS-Host nicht unterstützt und war deaktiviert.
- `build/current-body-momentum-docs-mac.log`, Ergebnisse
  `build/current-release-mac/test-results/run-386336hk`: **4/4 bestanden**.
  App gebaut; Completion-, API-Referenz-, tatsächlicher Handbuchparser- und
  Referenzgenerator-Test erfolgreich. 624 C- und 549 Physim-Symbole.

Diese Änderung besitzt noch keinen ausgeführten Linux-/Windows-Nachweis.
Die umfassende Mechanikabnahme und der Gesamtplan bleiben offen. Der frühere
Transformationsnachweis wurde vollständig in das Plattformarchiv verschoben,
um beide Handbuchseiten unter der Parsergrößengrenze zu halten.

## Pendel: Verlustenergie und numerische Bilanz (PP-0756/0767/0779)

Ausgangspunkt am 11. Oktober 2026 ist
`50cb4bf3aafec29a41b64627998b31980545996d`. Allgemeine C-/Physim-Pendelvorlagen
integrieren neben Winkel und Winkelgeschwindigkeit den Widerstandsverlust D:
`D'=0.5*rho*Cd*A*|L*omega|^3`, `D(0)=0`. Zwei zusätzliche SI-Kanäle speichern
`energy.dissipated` und `energy.balance=energy+D`. Die ursprünglichen neun
Kanäle behalten Reihenfolge und Bedeutung; aktuelle Vorlagen besitzen elf.
Neue Projekte erhalten die Quellen; bereits vorhandene Projektquellen werden
weiterhin erhalten. ABI und Dateiformate bleiben unverändert.

Euler/RK4/RK45 integrieren den dritten Zustand mit ihren Bewegungsstufen.
Symplektisches Euler verwendet für den Verlust die alte Geschwindigkeit wie
bei der expliziten Widerstandsbeschleunigung. Verlet bleibt auf widerstandsfreie
Bewegung beschränkt. Adaptive RK45-Fehlerkontrolle umfasst drei Komponenten;
absolute Toleranz gilt numerisch auch in Joule. Die Bilanz ist ein physikalisches
Prüfkriterium, keine Zusage exakter Bewegung oder exakter Verlustintegration.

C-Standardanalyse und beide Lernanalysen zeigen den numerischen Bilanzfehler
getrennt von mechanischer Energieänderung und Amplitudenabnahme. Vollständige
Bilanz-CSVs behalten jedes gespeicherte Zeitraster. Ältere Läufe ohne diese
Kanäle und gemischte aktuelle/ältere Eingaben funktionieren weiter; der
zusätzliche Bilanzbericht enthält nur entsprechend ausgestattete Läufe.

Tatsächlich ausgeführt auf Intel macOS 14.6.1, AppleClang 16, Release:

- `build/current-pendulum-ledger-confirmed-native-mac.log`, Ergebnisse
  `build/current-headless-mac/test-results/run-yyrxbxki`: **14/14 bestanden**.
  Ganze native Pendelgruppe und Sprach-Experimentregression: analytische
  Perioden/Energie, adaptive und Verlet-Referenzen, Lernpfad, Instanzen, Kräfte,
  Vorlagenanalyse, Medium/Rauschen/Reset, Abnahme/Legacyberichte und Codeblöcke.
- Neue `pendulum_energy_balance`-Prüfung: **30** C-/Physim-Integrationen,
  fünf Verfahren bei dt=0.08/0.04/0.02 s über vier Sekunden; monotone
  Verlustgröße, SI-Dimensionen, unabhängige Modellbilanz, Parität und Reset.
  RK4-Maximalfehler der Bilanz: 0.000276831278 / 0.0000069609172 /
  0.000000135272161 J. Verlet im Vakuum zeigt zweite Ordnung; adaptive RK45
  bleibt bei feinster Auswahl unter 2e-7 J. Die Energiebilanz kann schneller
  konvergieren als der Zustand; keine allgemeine Ordnungsgarantie wird abgeleitet.
- Neue `pendulum_balance_reports`-Prüfung: zwei tatsächlich gespeicherte
  C-/Physim-Runs mit je **2001** Samples, sechs gemischte Berichte in C und
  Physim, **12006** vollständige exportierte Bilanzzeilen. Unabhängiger
  PSRUN-/CRC-/Footerdecoder vergleicht jeden Energie-, Verlust-, Bilanz- und
  Fehlerwert; native Berichtsprüfer kontrollieren Kurven, Einheiten und Tabellen.
- Neue `pendulum_parameter_runs` führt den bestehenden SDK-Pendelprüfer
  zusätzlich im Repository aus: sechs Modelle, alle fünf Verfahren, Vakuum/
  Widerstand, **54** gültige feste Runs, **18** abgewiesene Einstellungen,
  sechs adaptive Runs und zwölf adaptive Berichte. Dies ist keine neue
  verschobene SDK-/Paketabnahme.
- `build/current-pendulum-ledger-gui-final-mac.log`, Ergebnisse
  `build/current-release-mac/test-results/run-3pwe1d50`: **2/2 bestanden**.
  Tatsächliche C-/Physim-Appketten für C-Energieauswertung sowie beide
  Widerstands-/Rauschvorlagen. Build, Parameter, Runner, Szene, Speicherung,
  Analyse, vollständige rohe/abgeleitete CSVs, Amplitudenintervalle, SVG und
  vollständig dekodierte PNGs geprüft. Bildschirminhalt und Energieexport
  wurden zusätzlich visuell geprüft.

Für diese Änderung liegt noch kein aktueller Linux-/Windows-/Apple-Silicon-
Nachweis vor. Gesamtplan und vollständige vertikale Plattform-/Paketabnahme
bleiben offen. Der Lernpfad dokumentiert Gleichungen, Quellen, Toleranzumfang
und die Trennung von physikalischer Dissipation und numerischem Bilanzfehler.

## Rückstauprüfung mit langsamen Schritten auf CI-Hosts

Ausgangspunkt am 11. Oktober 2026 ist
`103207b75d91f911b7bc87f313b6a7c9d019e872`. Die volle CI am vorherigen
`50cb4bf3aafec29a41b64627998b31980545996d` liefert einen konkreten Fehler:
[Run 38173524632](https://github.com/PhysicSimulator/physim/actions/runs/38173524632),
Apple-Silicon-Job 114571612317 und Windows/ClangCL-Debug-Job 114571612576,
jeweils **699/700** native Tests. Ausschließlich `runner_backpressure` scheitert
an der festen Annahme, nach vier Sekunden 1200 Schritte erreicht zu haben:
Checkpoint 352 beziehungsweise 256. Die vollständigen Joblogs wurden gelesen.

Das Modell ruft pro Schritt `ps_sleep(1)` auf. Diese Mindestwartezeit und
Scheduling/Dateiarbeit dürfen auf geteilten Hosts länger dauern. Ein expliziter
15-ms-Testschritt reproduziert den Fehler auf Intel macOS: Checkpoint 224 nach
vier Sekunden, obwohl der Prozess gesund ist. Die korrigierte Prüfung lässt
stdout weiterhin ungelesen, bestätigt den tatsächlich angehaltenen Leser und
wartet höchstens 45 Sekunden pro Modus auf denselben Schrittcheckpoint UND den
vollständigen PSRUN-Sample. Truncate-/Write-Rennen der Checkpointdatei werden
berücksichtigt. Die Gesamt-Testgrenze beträgt 240 Sekunden. Alle bisherigen
Erwartungen an Samplezahlen, zwei unabhängige Kanäle, Frame-Sequenzen, Szenen,
zuverlässige Pause/Einzelschritt/Stop/Logs/BYE, Wiederaufnahme und Fehler beim
permanent blockierten/geschlossenen Leser bleiben erhalten. Der produktive
Runner und seine zweisekündige Shutdown-Grenze wurden nicht verändert.

Tatsächlich ausgeführt auf Intel macOS 14.6.1 mit AppleClang 16:

- `build/current-runner-slow-advance-before-mac.log`,
  `build/current-headless-mac/test-results/run-w5_86iea`: alter Vier-Sekunden-Test
  scheitert an Checkpoint **224**, tatsächliche lokale Reproduktion.
- `build/current-runner-backpressure-progress-verified-mac.log`,
  `build/current-headless-mac/test-results/run-wbcpymeu`: Release **2/2 bestanden**.
  Normaler und 15-ms-Fortschritt; im langsamen Fall etwa 20.4/20.2 Sekunden bis
  1201 vollständige Live-Samples bei blockiertem stdout, danach korrekte
  Wiederaufnahme/Endzustände. Der 0.25×-Fall benötigt etwa 6.8 Sekunden.
- `build/current-runner-backpressure-progress-debug-mac.log`,
  `build/current-backpressure-debug-mac/test-results/run-a22s1tfe`:
  Debug **2/2 bestanden**, gleiche normalen/langsamen und sechs Transportmodi.
- `build/current-runner-backpressure-negative-control-mac.log`: historischer
  synchroner Runner aus `770dd5d^:runners/experiment.c`, gebaut gegen aktuellen
  Core/Platform, bleibt tatsächlich bei Checkpoint **96** stehen. Die neue
  45-Sekunden-Prüfung weist ihn weiterhin ausdrücklich ab. Mehr Zeit lässt
  einen echten blockierenden stdout-Fehler damit nicht bestehen.

Die zusätzliche Workflowdatei `runner-backpressure.yml` führt die zwei Fälle
gezielt in Debug auf Linux/GCC/Clang, macOS/Apple Silicon/Intel und Windows/
ClangCL/MSVC aus. Ihre YAML und sechs Matrixeinträge sind lokal geprüft;
Remote-Ausführung dieses neuen Workflows ist noch kein Nachweis. Die übrige
vollständige Plattform-/Paketabnahme bleibt offen. Der historische Run-Stream-
Abschnitt wurde vollständig ins Plattformarchiv verschoben; seine Anker bleiben.

## CI-Rückstau: Gesamtzeit und Stillstand getrennt

Die gezielte [CI 38175226720](https://github.com/PhysicSimulator/physim/actions/runs/38175226720)
am exakt belegten `e0f8f2728dd3faeafea6364cba6d3d0dee79d929` liefert erstmals
vier Plattform-/Compiler-Gegenprüfungen: Linux/Clang (Job 114576103747,
`run-_t_086pi`), Linux/GCC (114576103919, `run-98oekrjz`), Windows/ClangCL
(114576104025, `run-qcp97_5u`) und Windows/MSVC (114576104055,
`run-8rpatrb6`) bestehen jeweils **2/2** Debug-Fälle. Die vollständigen Joblogs
sind gelesen; beide normalen/langsamen Fälle wurden tatsächlich ausgeführt.
Dies ist weiterhin keine vollständige Windows-/Linux-Abnahme.

macOS/Intel (114576103977, `run-useoa1bw`) und Apple Silicon
(114576104099, `run-dxn3adym`) bestehen den normalen Fall. Der künstliche
15-ms-Fall erreicht nach 45 Sekunden aber erst Checkpoint 736 beziehungsweise
576. Damit ist auch der frühere Gesamtzeitrahmen auf diesen Hosts zu knapp.
Die neue Prüfung trennt nun einen Gesamtdeckel von **180 Sekunden pro Modus**
von einer **15-Sekunden-Stillstandsgrenze** zwischen tatsächlichen
Schrittcheckpoints. Unverändert werden derselbe Zielschritt UND alle vollständigen
PSRUN-Samples bei ungelesenem stdout verlangt. Jeder neue Checkpoint setzt nur
die Stillstandsgrenze zurück; die Gesamtgrenze bleibt fest. Gesamt-Testgrenze:
720 Sekunden; gezielter Workflow: 30 Minuten. Fehlende Fortschritte werden
somit früh erkannt, auch wenn der gesamte gesunde Lauf länger dauern darf.

Lokale Intel-macOS-/AppleClang-16-Gegenprüfungen:

- `build/current-runner-progress-idle-verified-mac.log`, Debug-Ergebnisse
  `build/current-backpressure-debug-mac/test-results/run-k0iyx_h9`:
  **2/2 bestanden**, normale und 15-ms-Schritte, alle sechs Transportmodi.
- `build/current-runner-progress-idle-negative-mac.log`: historischer synchroner
  Runner aus `770dd5d^` steht bei Checkpoint 96 und wird nach **15.47 Sekunden**
  durch die neue Stillstandsprüfung abgewiesen. Die große Gesamtgrenze lässt
  diesen echten blockierenden Ausgabepfad weiterhin nicht bestehen.

Die zusätzliche 60-ms-Gegenprobe `build/current-runner-progress-sixty-ms-mac.log`
besteht alle sechs Transportmodi: vollständige Live-Daten nach 75.840/75.338 s,
0.25×-Fall nach 25.171 s. Sie überschreitet tatsächlich die alten 45 Sekunden.

Der Produktcode ist unverändert. Die macOS-CI-Gegenprüfung der neuen Grenzen
und die vollständige Plattform-/SDK-Abnahme bleiben noch offen.


## Gezielte Rückstauprüfung auf sechs Plattformen und Compilern

Die [erneute CI 38175743829](https://github.com/PhysicSimulator/physim/actions/runs/38175743829)
am exakt belegten `ac5e162154fd5437992fee7eb015b789fd886fbe` ist tatsächlich
auf allen sechs Matrixeinträgen erfolgreich abgeschlossen. Vollständige Joblogs
wurden gelesen; normale und künstlich langsame Rückstaufälle bestehen jeweils
**2/2** mit Debug-Builds:

| Host und Compiler | Job | Native Ergebnisse |
| --- | --- | --- |
| macOS Intel, AppleClang | 114577640961 | `run-1em4bgwj` |
| macOS Apple Silicon, AppleClang | 114577641124 | `run-azoc8y5h` |
| Linux GCC | 114577641064 | `run-orn4ykgo` |
| Linux Clang | 114577641181 | `run-y3ktl5br` |
| Windows MSVC | 114577641202 | `run-9_ir9cv1` |
| Windows ClangCL | 114577641259 | `run-xy9mam1z` |

Dieser Nachweis schließt den gezielten Rückstaufehler einschließlich der beiden
macOS-CI-Hosts. Er ersetzt keine vollständige C17-/GUI-/SDK-/Paketmatrix.

## Eigenständige CFD und FEM Planung

PP-0417 besitzt jetzt zwei getrennte Teilprojektpläne mit optionalen
C17-Bibliotheken, Modell-/Einheitenverträgen, Ressourcen/Handles, Feldarchiven,
C-/Physim-Parität, analytischen Referenzen, Konvergenzmaßen, Meilensteinen und
Plattform-/Paketabnahmen. Die bestehenden Lehrmodelle bleiben als solche benannt;
eine Planungsanforderung wird nicht als bereits implementierter Solver gewertet.
Beide Seiten sind in den Projektunterlagen des Offline-Handbuchs registriert.
Der SDK-Packager verwendet hierfür seine bestehende Dokumentkopie.

`build/current-domain-plans-docs-mac.log`, Release-Ergebnisse
`build/current-release-mac/test-results/run-gtp2e4lp`: **2/2 bestanden**.
Der App-Build ist tatsächlich ausgeführt; der Handbuchparser liest auch beide
neuen Seiten. Die Referenzgeneratorprüfung besteht. Dies ist keine tatsächliche
CFD-/FEM-Solverprüfung und keine neue verschobene SDK-Abnahme. Der vollständige
lokale Build und SDK-Prüfer am eingefrorenen e0f8f27 laufen noch unabhängig weiter.


## RunStore und RunReader in Physim

Der opake Run-Stream-Lesezugang besitzt typisierte Sprachbindungen. Kopien
teilen Besitzer und Cursor; explizites Schließen invalidiert alle Aliase.
Zeilen und Szenen bleiben unabhängige Werte. Besitzerbereinigung, acht
Reader-Slots, Generationen, Allokationsfehler und terminale Zustände sind
ausgeführt; Writer bleiben C-only. Die allgemeine Handle-Migration und die
vollständige C-/Physim-Parität bleiben offen.

Intel macOS 14.6.1 / AppleClang 16, tatsächlich ausgeführt:

- `build/current-language-run-stream-complete-mac.log`, native Release-Ergebnisse
  `build/current-headless-mac/test-results/run-bgk_viql`: **5/5 bestanden**.
  Neue Sprachwerte werden auch im Experiment- und Analyse-Runner verwendet;
  ein unabhängiger C-Prüfer kontrolliert Samples, Zeiten, Einheiten und Bericht.
  Unabhängig beschädigte CRCs prüfen Recovery und den letzten gültigen Wert.
- Der generierte Standalone-Sprachtest prüft zusätzlich die vollständige
  Freigabe der erfassten Speicherblöcke am Programmende.
- `build/current-language-run-stream-asan.log`: der direkte C-Lebensdauertest
  besteht mit AddressSanitizer und UBSan. LeakSanitizer ist auf diesem Host
  deaktiviert; dies ist kein LeakSanitizer-Nachweis.
- `build/current-language-run-stream-app-docs-mac.log`, App-Release-Ergebnisse
  `build/current-release-mac/test-results/run-ip18smgi`: **4/4 bestanden**,
  Completion, API-Referenz, Handbuchparser und Referenzgenerator.

Die eingefrorene Gesamtprüfung an e0f8f27 ist inzwischen mit **701/704**
beendet. Drei Berichtstests erwarten noch die Struktur vor der zusätzlichen
Energiebilanzkurve. Die nachgeschaltete SDK-Prüfung wurde nicht gestartet.
Dieser Teilnachweis ersetzt keine vollständige aktuelle Plattformabnahme.


## Berichtregressionen nach der Energiebilanz-Erweiterung

Die vollständige Intel-macOS-Prüfung am eingefrorenen e0f8f27 endet mit
**701/704** (`build/current-full-build-e0f8f27-mac.log`, Ergebnisse
`build/current-full-build-e0f8f27-mac/test-results/run-utr4rylx`).
Die neuen Diagramme sind korrekt erzeugt; floating_reference, spring_reference
und derived_reference erwarten noch die alte Berichtstruktur.
Die SDK-Prüfung wurde wegen dieser Fehler nicht begonnen.

Die drei Referenzprüfer verlangen jetzt genau fünf Diagramme und prüfen die
zusätzliche Joule-Fehlerkurve einschließlich Zeitwerten und exakter Differenz
zur Anfangsbilanz. Bestehende analytische Bewegungs-, Energie-, RK4-, CSV-,
Histogramm-, Perioden- und Abklingprüfungen bleiben erhalten.
`build/current-energy-report-regressions-final-mac.log`, Release-Ergebnisse
`build/current-headless-mac/test-results/run-4xjim5ar`: **3/3 bestanden**.

Auch die frischen Linux-Paketläufe
[38174646112](https://github.com/PhysicSimulator/physim/actions/runs/38174646112)
an 103207b (**700/703**) und
[38176617548](https://github.com/PhysicSimulator/physim/actions/runs/38176617548)
an a7917ae (**701/704**) enden mit drei nativen Fehlern vor der Installation.
Ihre vollständigen Logs sind gelesen; ein erfolgreicher frischer Paketnachweis
kann daraus nicht abgeleitet werden. Eine erneute vollständige Prüfung und
SDK-Abnahme des korrigierten Standes bleiben erforderlich.


## Verschobenes SDK mit RunStore und RunReader am 12. Oktober 2026

Intel macOS 14.6.1 / AppleClang 16: der Release-SDK-Build und eine tatsächliche
Verschiebung des SDKs in einen UTF-8-/Leerzeichenpfad sind ausgeführt.
`build/current-language-reader-sdk-install-mac.log` und
`build/current-language-reader-sdk-verify-mac.log` enden erfolgreich.
Der Consumer-Nachweis liegt in
`build/current-language-reader-sdk-consumer-mac/Native SDK ä 3pwmazgb`;
alle **17** protokollierten Prüfkommandos in `verification.log` enden mit 0.

Der Prüfer emittiert Standalone-, Experiment- und Analyse-C mit dem
installierten Physim-Compiler und kompiliert jeweils gegen die installierte
Core-Bibliothek und einen aus den SDK-Quellen neu gebauten Core. Öffentliche
Header kommen ausschließlich aus dem SDK. C-Allokationsfehler, geteilte
Cursor, Besitzerbereinigung, ungültige Aliase, acht Slots, Szenen, beschädigte
CRC und Recovery sowie unabhängige Zeilen sind geprüft. Die generierte
Standalone-Physim-Ausgabe prüft erfasste Restallokationen am Programmende.
Tatsächliche Runner erzeugen anschließend einen Replay-Lauf und einen
Analysebericht; ein unabhängiger C-Prüfer kontrolliert alle Samples, Zeiten
und SI-Spalten. Das neue Sprachruntime-Header ist im verschobenen SDK vorhanden.

`build/current-language-reader-verification-kit-mac.log` besteht mit **158**
exakt erfassten Dateien, SHA-256 und den Paketgrenzen. Neue Reader-Fixtures
sind im unabhängigen Verifikationspaket enthalten; Implementierung und
öffentliche Header werden weiterhin aus dem SDK bezogen. Der fokussierte
Nachweis ersetzt keine ganze GUI-, Linux-, Windows- oder Produktabnahme.
Die Gesamtprüfung und anschließende SDK-/App-Prüfung am eingefrorenen
`1a1a42d65227f8ed05f936f9902e281bfc599d59` laufen noch separat.


## RunWriter und explizites SI-Schreibschema am 12. Oktober 2026

RunStore besitzt eine atomare Konfiguration für Zeitschritt, Seed, UTF-8-Metadaten
und kanonische SI-Kanäle. RunWriter erfasst dieses Schema bei Erstellung, öffnet
exklusiv und bindet vollständige Messzeilen, Szenenkopien, Zähler, Finale und
Abort. Kopien teilen den Writer; Besitzer, begrenzte Slots und Generationen
verhindern eine Wiederbelebung alter Kopien. Neue Store-Schemata gelten nur
für künftige Dateien. Explizites close meldet Finalisierungsfehler; automatische
Bereinigung gibt Ressourcen frei und kann keinen Fehler zurückliefern.

Intel macOS 14.6.1 / AppleClang 16, tatsächlich ausgeführt:

- `build/current-language-run-writer-native-final-mac.log`, Release-Ergebnisse
  `build/current-headless-mac/test-results/run-oy59c4vz`: **11/11 bestanden**.
  Writer/Reader, RunIndex, Slot-Retirement sowie bestehende Array-Append-/
  Remove-Fälle bestehen. Ein unabhängiger öffentlicher C-Writer, direkte
  Sprachruntime-Aufrufe und ein besitzendes Physim-Programm erzeugen
  **bytegleiche Archive**, einschließlich Metadaten, Schema, drei Zeilen,
  drei Szenen, Index und Footer.
- Ungültige Konfiguration, doppelte/nicht-SI-Kanäle, Allokationsfehler,
  bestehende Dateien, falsche Kanalzahlen, NaN-Zeiten/-Werte, ungültige Szenen,
  Aliase, Slot-Wiederverwendung, Abort/Recovery, letzte Besitzer und
  künftige Schemaänderungen werden geprüft. Der Physim-Test prüft zudem
  erfasste Restallokationen bei Programmende.
- Tatsächliche Experiment- und Analyse-Runner verwenden jetzt auch Writer.
  Ein unabhängiger C-Prüfer kontrolliert ihre gesamten Kopieläufe mit jeweils
  drei Samples, Zeiten, Metadaten, SI-Schemata und gültigem Abschluss.
- `build/current-language-run-writer-asan-boundaries.log`: direkte Reader- und
  Writer-Lebensdauerprüfungen bestehen mit AddressSanitizer und UBSan.
  LeakSanitizer bleibt auf diesem Host deaktiviert.
- `build/current-language-run-writer-kit-mac.log`: unabhängiges
  Verifikationspaket mit **161** exakt erfassten Dateien bestanden.

Die ganze allgemeine Ressourcenmigration, C-/Physim-Funktionsparität und aktuelle
Plattformabnahme bleiben offen. Der eingefrorene Gesamtbuild an 1a1a42d prüft
weiterhin seinen eigenen Stand vor der Writer-Ergänzung.


Der vollständige Writer-SDK-Build und die erneute Verschiebungsprüfung
`build/current-language-writer-sdk-install-mac.log` /
`build/current-language-writer-sdk-verify-final-mac.log` bestehen. Der Consumer
`build/current-language-writer-sdk-consumer-final-mac/Native SDK ä yu14vs29`
protokolliert **24/24 erfolgreiche Kommandos**. Der installierte Compiler,
öffentliche SDK-Header, installiertes und aus SDK-Quellen neu gebautes Archiv
bestehen dieselben Reader-/Writer- und tatsächlichen Hostprüfungen.
Auch 16 Kanäle, eine atomar abgewiesene 17. Deklaration, 2047 Metadatenbytes
und der Int64-Seed als vollständiges uint64-Bitmuster sind ausgeführt.
Der App-Build mit Completion, API-Referenz, beiden Plattform-Handbuchseiten
und generierter Referenz besteht **4/4**:
`build/current-language-run-writer-app-docs-mac.log`,
`build/current-release-mac/test-results/run-f86hxmxo`.
Diese lokalen Nachweise ersetzen keine aktuelle ganze Plattformabnahme.


## Messkanalvertrag mit Einheit, Abtastrate und Zeitstempel (PP-0400)

Der ursprüngliche Planblock ist einzeln gegen SensorConfig/Measurement, die
C-/Physim-Bindungen, Dokumentation und tatsächliche Daten geprüft. Konfigurierte
Rate und Startzeit sind vom Modellschritt getrennt; Messungen tragen reale
Aufrufzeit, Index und übersprungene Rasterplätze. Das Sensor-Wurfbeispiel
archiviert die Rate ausdrücklich als Modellmetadaten und den Zeitstempel
als Sekundenkanal; Channel selbst startet keine automatische Abtastung.

`build/current-measurement-rate-timestamp-mac.log`, native Release-Ergebnisse
`build/current-headless-mac/test-results/run-m557ek2a`: **7/7 bestanden** auf
Intel macOS 14.6.1 / AppleClang 16. Ausgeführt sind Kernmessungen, fünf
Physim-Wert-/Fehlerfälle und C-/Physim-Sensorparität samt echtem Runnerarchiv.
Die zusätzliche Archivprüfung verlangt `sensor_rate_hz=100`, Meter- und
Sekundenschema und vergleicht sämtliche 201 Zeilen mit dem C-Sensormodell.
Zu frühe/verspätete Aufrufe, falsche Dimensionen, ungültige Rate, nicht
darstellbare Raster, Seed-/Reset-/Kopieverhalten und Zustandserhaltung sind
ebenfalls ausgeführt. Dies ist ein begrenzter Bibliotheks-/Persistenznachweis;
die ganze aktuelle Plattformabnahme bleibt offen.


## Gesamtbuild und Zielzeit-Serien nach der Energiebilanz-Erweiterung

Der eingefrorene Intel-macOS-/AppleClang-16-Release-Build am
`1a1a42d65227f8ed05f936f9902e281bfc599d59` endet tatsächlich mit
**705/705 bestandenen nativen Fällen**. Ergebnisse:
`build/current-full-build-1a1a42d-mac/test-results/run-7yavie3l/results.json`;
Log: `build/current-full-build-1a1a42d-mac.log`. Alle 705 Ergebniszustände sind
einzeln als bestanden gelesen. Dieser Stand liegt vor RunWriter und dem
zusätzlichen Messkanal-Persistenznachweis. Das SDK wurde installiert.

Die anschließende ganze SDK-Prüfung
`build/current-full-sdk-1a1a42d-mac.log` endet dagegen mit Fehler: der
Zielzeit-Serienprüfer verlangt noch neun Kanäle statt der elf aktuellen
Pendelkanäle. Sie erreicht die vollständige App-Prüfung nicht. Der eingefrorene
Stand und seine Fehlerbelege bleiben erhalten; diese Prüfung wurde nicht
stillschweigend mit veränderten Eingaben fortgesetzt.

Der korrigierte Prüfer verlangt genau elf Namen, die SI-Geschwindigkeits-
und Joule-Schemata, endliche Werte, Vakuumverlust exakt null und Bilanz
exakt gleich mechanischer Energie. Vorhandene PCHIP-Referenzwerte,
Parameter-/Adaptive-Metadaten, strikt steigende variable Zeiten, exakter
gemeinsamer Endpunkt, Energiedrift, kinematische Identitäten und Studienbericht
bleiben geprüft. Ein neuer Repositoryfall führt diese Zielzeit-Serien mit
allen sechs C-/Physim-Pendelmodellen aus: 18 tatsächliche Läufe.

`build/current-pendulum-target-ledger-native-mac.log`, Release-Ergebnisse
`build/current-headless-mac/test-results/run-ye471cw9`: **2/2 bestanden**,
Parameterläufe und neuer Zielzeit-Serienfall.
`build/current-pendulum-series-ledger-sdk-mac.log`, Consumer
`build/current-pendulum-series-ledger-sdk-mac/Native SDK ä zyl4ip7m`:
**47/47 protokollierte Kommandos bestanden** gegen installiertes und aus
SDK-Quellen neu gebautes Core-Archiv. Dies umfasst 36 Zielzeit-Läufe,
C-/Physim-Verfahrens-/Medienparität, gemischte Berichte, den Integrator-Lernpfad,
Abklingauswertung und beide Peak-Bindungen. Die ganze aktuelle SDK-/App-/
Plattformabnahme bleibt offen.


## Student-t-Mittelwertintervalle in C und Physim (PP-0405)

Der bisher fehlende Intervallpfad ist ergänzt: explizite Stichprobenkennzahlen
oder gültige Series-Werte liefern ein zweiseitiges Student-t-Intervall mit
Konfidenzniveau und n-1 Freiheitsgraden. MeanInterval ist ein kopierbarer
Sprachwert; tatsächliche Analysen können die Grenzen mit SI-Spalten ausgeben.
Unabhängige normalverteilte Beobachtungen, Statusfilterung, Varianz-/
Darstellbarkeitsgrenzen und Fehleratomizität sind ausdrücklich dokumentiert.
Der Algorithmus interpretiert keine korrelierte Zeitserie automatisch als IID.

Intel macOS 14.6.1 / AppleClang 16, tatsächlich ausgeführt:

- `build/current-mean-interval-confirmed-native-mac.log`, Release-Ergebnisse
  `build/current-headless-mac/test-results/run-70zqphsv`: **6/6 bestanden**.
  Messungen, Series, numerische Grenzfälle, Sensorparität, Intervallwerte und
  der unabhängige Oracle-Fall sind ausgeführt. **66 C-/Physim-Fälle** vergleichen
  kritische Werte über direkte Dichtequadratur, exakte ganzzahlige
  Binomialnormalisierung bis 100000 Freiheitsgraden und eigene Wallis-
  Normierungsentwicklung darüber; kein Core-Betakettenbruch wird wiederverwendet.
  Extreme Konfidenz, große Freiheitsgrade und ungültige Eingaben sind enthalten.
- Der eigentliche Analyse-Runner schreibt einen Bericht; ein unabhängiger
  C-Prüfer kontrolliert seine Intervallgrenzen und SI-Spalten. Masken,
  konstante Werte, zu wenige Beobachtungen, veraltete Handles, Varianzunterlauf,
  positive zusammengefallene Grenzen und unveränderte Fehlerausgaben sind geprüft.
- `build/current-mean-interval-existing-report-mac.log`, Ergebnisse
  `build/current-headless-mac/test-results/run-9efteyhc`: **1/1 bestanden**,
  bestehende Berichte, Histogrammzählung, konstante Daten und Exporte.
- `build/current-mean-interval-asan.log`: direkte C-Intervall-/Series-Prüfung
  mit AddressSanitizer und UBSan bestanden; LeakSanitizer deaktiviert.
- `build/current-mean-interval-app-docs-mac.log`, App-Ergebnisse
  `build/current-release-mac/test-results/run-t2xjuj1g`: **4/4 bestanden**,
  Completion, API-Referenz, Handbuchparser und Referenzgenerator.
- `build/current-mean-interval-sdk-install-mac.log` und
  `build/current-mean-interval-sdk-verify-mac.log` bestehen. Consumer:
  `build/current-mean-interval-sdk-consumer-mac/Native SDK ä qsjbzpgi`,
  **13/13 protokollierte Kommandos bestanden**. Der installierte Compiler,
  öffentliche SDK-Header und installiertes/neues Core-Archiv prüfen dieselben
  unabhängigen Referenzen und tatsächlichen Analyseberichte. Das benötigte
  private numerische Header wird im SDK mit den Core-Quellen ausgeliefert.
- `build/current-mean-interval-kit-mac.log`: unabhängiges Verifikationspaket
  mit **168** exakt erfassten Dateien und kontrollierten Paketgrenzen bestanden.

Die vorhandenen Type-7-Quantile und Histogramme besitzen separate Series-/
Reportnachweise. PP-0405 ist damit als begrenzte Bibliotheksumsetzung belegt;
die ganze aktuelle Plattform- und Produktabnahme bleibt offen. Die eingefrorene
Gesamtprüfung an 03a58b3 läuft unabhängig auf ihrem Stand vor MeanInterval.


## App-Berichtserwartungen nach der Pendelbilanz-Erweiterung

Die C17-CI am genau belegten 03a58b3 besteht unter Linux/Clang und Linux/GCC
jeweils **706/706 native Fälle**, scheitert anschließend jedoch im tatsächlichen
App-Selftest language_full bei Stufe 7. Vollständige Logs sind gelesen:
[Run 38180960909](https://github.com/PhysicSimulator/physim/actions/runs/38180960909),
Jobs 114596665718 (Clang, `run-i9accbke`) und 114596665624
(GCC, `run-inyer4gm`). Die beiden Jobs sind fehlgeschlagen; ihre nativen
Erfolge sind keine ganze Plattformabnahme.

Die App-Funktion erzeugt korrekt den zusätzlichen numerischen Bilanzplot
und die Verlust-/Fehlertabelle. Der alte Selbsttest verlangt noch zwei Plots
und drei Tabellen. Er ist auf Intel macOS für beide Sprachkombinationen
lokal reproduziert: `build/current-language-balance-workflow-before-mac.log`,
Ergebnisse `build/current-release-mac/test-results/run-dlh9388_`: **0/2**.
Die vorherige erfolgreiche Gesamtnative-Suite enthielt diese Display-Fälle nicht.

Die korrigierte Erwartung verlangt genau drei Plots und vier Tabellen und
behält die mechanische Energie-/Periodenprüfung bei. Eine neue unabhängige
Archivprüfung liest alle Messzeilen: elf Kanäle, Joule-Schemata,
E + Verlust = Bilanz, nichtnegative Verluste, sämtliche gezeichneten
Fehlerpunkte und die vollständigen Endverlust-/Maximalfehlerwerte.
Der zusätzliche vollständige Bilanz-CSV muss vorhanden sein.

`build/current-language-balance-workflow-after-mac.log`, Release-Ergebnisse
`build/current-release-mac/test-results/run-1i1w3nmq`: **2/2 bestanden**.
Auch ein tatsächlich installiertes SDK besteht dieselben realen
App-/Projektbuild-/Runner-/Berichtsabläufe mit ausschließlich seinen
Quellen, Headern und Programmen: `build/current-language-balance-sdk-install-mac.log`,
`build/current-language-balance-sdk-full-mac.log` und
`build/current-language-balance-sdk-mixed-mac.log`. Beide app-steps.json
protokollieren jeweils einen Appprozess mit Exit 0. Der fokussierte
Linux-Sanitizerworkflow registriert die beiden Fälle zusätzlich; seine
Konfiguration gilt noch nicht als ausgeführter Nachweis der Korrektur.


## Unsicherheit, Sensorparameter und getrennte Modelle (PP-0401/0402/0406)

Die ursprünglichen Planblöcke sind einzeln gegen die vorhandenen
Standardunsicherheits-/Sensor-/Messdatenpfade abgeglichen. Neue negative
Konfigurationsfälle prüfen absolute/relative Unsicherheit, Auflösung,
Dropout-Wahrscheinlichkeit, Offset und Drift samt vollständiger
Zustandserhaltung. Ein einzelner absoluter Beitrag von **1e308** wird
korrekt ausgegeben, obwohl sein Quadrat nicht als Double darstellbar wäre.
Bestehende unabhängige C-/Physim-Sensor-/Bewegungsreferenzen bleiben erhalten.

Intel macOS 14.6.1 / AppleClang 16, tatsächlich ausgeführt:

- `build/current-measurement-uncertainty-model-mac.log`, Release-Ergebnisse
  `build/current-headless-mac/test-results/run-_tgsqax9`: **7/7 bestanden**.
  Einheitenskalierung, RSS-Beiträge, Quantisierungsgleichstände, Drift/Offset,
  indexbasiertes Rauschen/Dropout, Sparse-/Vollraster, vier vollständige
  64-Bit-Seeds, Reset/Kopien, Bereichsfehler und Ausgabeerhaltung sind geprüft.
- `build/current-measurement-uncertainty-model-asan.log`: direkter
  C-Messungstest mit AddressSanitizer/UBSan bestanden; LeakSanitizer deaktiviert.
- `build/current-measurement-model-separation-reports-mac.log`, Ergebnisse
  `build/current-headless-mac/test-results/run-83v_4dvz`: **2/2 bestanden**,
  tatsächliche C-/Physim-Wurfläufe samt Analysen und eigenständige
  Status-/Messfehlerberichte. Ideale Kurven und gültige Messpunkte bleiben
  getrennt; Histogramme/Statistik behandeln leere und einzelne gültige Werte.
- `build/current-measurement-model-separation-analysis-mac.log`, Ergebnisse
  `build/current-headless-mac/test-results/run-xy8zjsj0`: **1/1 bestanden**,
  bisheriger Sprach-Analysepfad zusätzlich ausgeführt.

Diese Belege schließen begrenzte Bibliotheksanforderungen; Sensor-
Standardunsicherheit ist weiterhin kein Konfidenzintervall oder vollständiges
Kalibrierungsmodell. Die gesamte aktuelle Plattform-/Produktabnahme bleibt offen.


## Feder- und Auftriebs-Appketten mit numerischer Bilanzkurve

Die gesamte SDK-Prüfung am eingefrorenen 03a58b3 endet tatsächlich mit Fehler
im App-Selftest spring: Stufe 7 verlangt vier Diagramme, obwohl die Analyse
korrekt fünf erzeugt. `build/current-full-sdk-03a58b3-mac.log` und Consumer
`build/current-full-sdk-03a58b3-consumer-mac/Native SDK ä _2xoj5hq` bewahren
den Fehler. **562 von 563** protokollierten Kommandos enden mit 0; der
letzte endet mit 1, kein PASSED-Marker ist vorhanden. Die vier vorherigen
GUI-Ketten pendulum/projectile/collision/box_floor bestehen. Der Stand ist
damit ausdrücklich keine gesamte SDK-/App-Abnahme.

Feder und Auftrieb sind lokal mit denselben veralteten Erwartungen
nachvollzogen: `build/current-other-balance-workflows-before-mac.log`,
Ergebnisse `build/current-release-mac/test-results/run-ddeehtor`: **0/2**.
Die Korrektur verlangt genau fünf Diagramme und prüft die numerische
Joule-Bilanzkurve unabhängig gegen sämtliche gespeicherten Messzeilen und
den vollständigen Maximalfehler. Der ganze Bilanz-CSV muss vorhanden sein.
Der neue spring_workflow ist jetzt auch im Repository-Displaykatalog
registriert; beide Fälle kommen zusätzlich in die fokussierte Linux-CI.

`build/current-other-balance-workflows-after-mac.log`, Release-Ergebnisse
`build/current-release-mac/test-results/run-16f_ncqh`: **4/4 bestanden** auf
Intel macOS 14.6.1 / AppleClang 16: Feder, Auftrieb und beide Pendel-
Sprachkombinationen. Ein neu installiertes SDK besteht Feder und Auftrieb
ebenfalls mit jeweils einem vollständigen Appprozess (Exit 0):
`build/current-general-balance-sdk-install-mac.log`,
`build/current-general-balance-sdk-spring-mac.log` und
`build/current-general-balance-sdk-floating-mac.log`. Aktuelle ganze
Plattform-/SDK-Abnahme bleibt offen; die erneute CI ist ein gesonderter Gate.

## Monte-Carlo mit explizitem Seed (PP-0404)

Der Originalblock ist gegen Controller, C-/Physim-Modelle, archivierte Seeds
und unabhängige numerische Referenzen geprüft.
`build/current-monte-carlo-seed-plan-mac.log`, Ergebnisse
`build/current-headless-mac/test-results/run-3ujom15v`: **1/1 bestanden**
auf Intel macOS / AppleClang 16. **1536 archivierte Läufe** werden mit einem
eigenen PCG-/Box-Muller- und analytischen Ballistik-Orakel geprüft.
Ein und vier Worker erzeugen identische Journale; beide Sprachen liefern
gleichwertige Trajektorien. Hochbit-/maximale Seeds, konstante Populationen,
acht gemischte Analysen, Szenen sowie abgewiesene unvollständige oder
inkonsistente Ensembles sind enthalten.
`build/current-monte-carlo-seed-batch-controls-mac.log`, Ergebnisse
`build/current-headless-mac/test-results/run-mkgya14g`: **2/2 bestanden**,
Batch-Referenzen und parallele Worker; Seedüberlauf wird vor Start abgewiesen.
Dies belegt die begrenzte Bibliotheks-/Controlleranforderung; die gesamte
aktuelle Plattform- und Produktabnahme bleibt offen.


## Linux-Sanitizernachweis für die Pendel-Sprachanalyse

Die [fokussierte CI 38183381868](https://github.com/PhysicSimulator/physim/actions/runs/38183381868)
am exakt belegten `2f4a46af10a5a65320037dd89e9988f72c4081f9` ist unter
GCC und Clang erfolgreich beendet. Vollständige Joblogs sind gelesen.
Beide führen **3/3 native** und **5/5 grafische** Sanitizerfälle tatsächlich
aus, einschließlich language_full_workflow und language_mixed_workflow:

| Compiler | Job | Native | Grafisch |
| --- | --- | --- | --- |
| Clang | 114600480800 | `run-ihoop7oq` | `run-ihf3rn7c` |
| GCC | 114600480941 | `run-cpyd43g7` | `run-ej5zlx95` |

Damit besitzt die lokale Stufe-7-Korrektur einen tatsächlichen Linux-
ASan-/UBSan-Appnachweis. Die später ergänzten Feder-/Auftriebsfälle sind
in diesem Stand noch nicht enthalten. Die gesamte neue C17-/SDK-/GUI-
Matrix bleibt eine getrennte Abnahme.


## Positive Verteilungen in C und Physim (PP-0403)

Exponential- und Lognormalverteilung ergänzen konstant/uniform/normal in
`measurement.h` und den Physim-Konstruktoren `Distribution.exponential` /
`Distribution.logNormal`. Validierung, Momente und Ziehungen verwenden denselben
Core. Eine Exponentialziehung verbraucht einen, eine nichtdegenerierte
Lognormalziehung zwei offene Uniformwerte; eine Lognormalverteilung mit
Log-Standardabweichung null verbraucht keinen RNG-Schritt. Bereichsfehler
bewahren RNG und Ausgaben. Sensorrauschen erhält seine indizierte Folge;
der positive Erwartungswert wird nicht automatisch als Bias entfernt.

Tatsächlich ausgeführte Intel-macOS-Nachweise:

- Native **4/4**, `build/current-positive-distributions-final-native-mac.log`,
  `run-dfwmdb6d`: neue Verteilungen, Messmodell, RNG und Mittelwertintervalle.
  Das unabhängige Orakel prüft **60** C-/Physim-Fälle über vier vollständige
  Seeds, ganzzahlige PCG-Übergänge, Decimal-Momente und Transformationen.
  Kleine Log-Streuung, darstellbare Momente trotz Zwischenüberlauf, ungültige
  Parameter und Über-/Unterlauf sind enthalten.
- Direkter AppleClang-ASan/UBSan-Lauf besteht unter
  `build/current-positive-distributions-asan.log`; LeakSanitizer ist auf diesem
  macOS-Lauf deaktiviert.
- App-Build und Referenz-/Dokumentationsprüfungen **4/4**, `run-a8t84ap7`,
  `build/current-positive-distributions-app-docs-mac.log`.
- Unabhängiges Verification Kit prüft **173** Quellen exakt unter
  `build/current-positive-distributions-kit-mac.log`.
- Verschobenes SDK und separat neu gebauter Core bestehen **8/8** Verbraucherbefehle
  und denselben
  Verteilungs-/Sensor-/Orakelvertrag. Log:
  `build/current-positive-distributions-sdk-verify-mac.log`; Verbraucher:
  `build/current-positive-distributions-sdk-consumer-mac/Native SDK ä nqdlus0u`.

Die Parameter-/Einheitenkonvention und wissenschaftlichen Quellen stehen in
[Messung und Unsicherheit](measurement.md#positive-verteilungen).
Dies belegt den konkreten Bibliotheksblock, keine vollständige aktuelle
Linux-/macOS-/Windows-Abnahme. Plattformübergreifende bitgleiche `libm`-Werte
werden nicht behauptet.

Die anschließenden Handbuch-/Katalog-/Kit-Prüfungen nach der Aufteilung des
Plattformarchivs bestehen **3/3** (`run-_xemcusc`); alle drei Seiten werden
mit dem echten Offline-Parser unterhalb 256 KiB eingelesen. Referenz und
Completion bestehen zusätzlich **2/2** (`run-434xwlk8`). Historische Abschnitts-
überschriften bleiben als Weiterverweise erhalten, verschobene Belegtexte
sind unverändert.


## Vollständiger nativer Lauf und Linux-GUI am Stand 03ca6a2

Der eingefrorene Quellstand `03ca6a252bd0816b5a91ae25f5732bf9f3eb7dfc`
besteht auf Intel macOS **708/708** native Release-Tests. Alle 708 JSON-
Einzelergebnisse haben Status `passed`; Log und Bericht liegen unter
`build/current-full-build-03ca6a2-mac.log` und
`build/current-full-build-03ca6a2-mac/test-results/run-gm64me3l/results.json`.
Die 1366 Quelldateien des eingefrorenen Eingangs bleiben getrennt von späteren
Änderungen; seine vollständige verschobene SDK-/GUI-Prüfung ist ebenfalls erfolgreich abgeschlossen.

[Linux-UI-Sanitizer-CI](https://github.com/PhysicSimulator/physim/actions/runs/38184378499)
führt auf genau demselben Commit tatsächlich aus:

- GCC, Job 114603469259: native **3/3** `run-x1mchnl2`, GUI **7/7** `run-9848kjoc`.
- Clang, Job 114603469387: native **3/3** `run-h_xytns4`, GUI **7/7** `run-qhak6xsv`.

Die sieben GUI-Fälle enthalten Projektbaum, Completion, API-Referenz, beide
Physim-Auswertungsabläufe sowie Feder und Auftrieb. Die vollständigen Joblogs
sind gelesen; keine Sanitizerdiagnose belegt einen Fehler. Dies bestätigt die
konkreten Abläufe auf Ubuntu 24.04 mit beiden Compilern. Exponential-/Lognormal-
verteilungen und die spätere Ereignisaufzeichnung gehören nicht zu diesem
Stand. Die ganze neue Plattform-/Paketabnahme bleibt offen.


## Ereignisse und Warnungen in psrun (PP-0452)

Optionaler Chunk 8 enthält Version 1, Level 1–4, endliche logische Sekundenzeit
und begrenzten UTF-8-Text, mit CRC über den vollständigen Payload. C-Low-Level-
und versionierte Handle-APIs sowie Physim-Writer/Reader verwenden denselben
Core. Ungültige Records schreiben keine Bytes; Lesefehler bewahren die Ausgabe.
Mess- und Szenenzähler bleiben getrennt. Alte Format-1-Reader überspringen den
neuen optionalen Typ; Datei-/API-/ABI-/IPC-Versionen bleiben erhalten.

Der Runner archiviert auch Meldungen vor der Schemakonfiguration über eine
begrenzte temporäre Aufzeichnung sowie Abschluss-/Budgetwarnungen vor dem Footer.
Ein vorhandener fremder Sidecar wird nicht überschrieben und unterdrückt die
Laufarchiv-Aufzeichnung nicht. Nach einem abrupten Prozessabbruch gibt es keinen
Abschluss; vollständige CRC-Blöcke bleiben als recoverbarer Präfix lesbar.

Tatsächlich ausgeführte Intel-macOS-Nachweise:

- **5/5** neue/bestehende native Gruppen, `run-8pfwye_p`,
  `build/current-run-events-confirmed-mac.log`: Ereignisse, beide tatsächlichen
  C-/Physim-Logger, Handle-Vertrag und Run-Index. Der unabhängige Decoder prüft
  bytegleiche C-/Physim-Dateien einschließlich Header, Metadaten, Schema,
  Ereignissen, Index und Footer. CRC, Länge, UTF-8, Level, NaN-Zeit, NUL,
  unbekannte Ereignisversion, unbekannter Chunk und fehlender Footer sind
  konkrete Gegenfälle. Tatsächliche kontrollierte Fehler und `abort` bewahren
  die geschriebenen Ereignisse mit `PS_RECOVERED`; über 5000 Logaufrufe werden
  begrenzt und mit einer archivierten Warning zusammengefasst.
- Bestehende Sprach-Store-/Reader-/Writer-Besitz- und Runnerdiagnosen: **2/2**,
  `run-01_vos7d`, `build/current-run-events-owner-diagnostics-mac.log`.
- Direkter AppleClang-ASan/UBSan-Lauf von Ereignis-Writer/Reader/Store besteht
  unter `build/current-run-events-asan.log`; LeakSanitizer ist lokal deaktiviert.
- Ein tatsächlich gegen den unveränderten früheren Core `03ca6a2` gelinkter
  Reader liest die neue Datei mit einer Messzeile und gültigem Footer:
  `build/current-run-events-legacy-mac.log`. Der eingefrorene Eingang bleibt
  unverändert; das Probeprogramm ist als Quelle versioniert.
- App-Build, Offline-Parser, Referenz, Completion, Core-Katalog und unabhängiges
  Verification Kit bestehen **5/5**, `run-cf75_6ea`,
  `build/current-run-events-final-sdk-install-mac.log`. Das Kit prüft dort
  179 exakte Eingänge; die ergänzte Legacy-Probe erhält eine eigene Nachprüfung.
- Die abschließende Ereignisgruppe einschließlich Legacy-API-Probe besteht
  **1/1**, `run-lhq88pod`, `build/current-run-events-final-native-mac.log`.
- Nach Ergänzung aller Probequellen bestehen Dokumentation/Katalog/Kit/Completion
  **5/5**, `run-hhu506d7`, `build/current-run-events-final-docs-mac.log`.
  Das unabhängige Kit prüft jetzt **180** exakte Eingänge.
- Das verschobene SDK und separat neu gebauter Core bestehen abschließend
  **16/16** Verbraucherbefehle einschließlich Legacy-API-Lesern, echten
  C-/Physim-Runnern und Fehlerpräfixen:
  `build/current-run-events-final-sdk-verify-mac.log`, Verbraucher
  `build/current-run-events-final-sdk-consumer-mac/Native SDK ä usuq0564`.

Dies belegt den konkreten Laufdateiblock auf Intel macOS, keine vollständige
aktuelle Plattform-/Produktabnahme. Vor erfolgreicher Laufdateierstellung gibt
es nur die Begleitdatei. Ein Stromausfall kann Daten seit dem letzten OS-Sync
verlieren. Ereignisse besitzen einen sequentiellen Cursor; ihre Anzeige beim
Offline-Import und ein eigener Ereignisindex sind daraus nicht abgeleitet.


## Dauerhafte Projekt- und Experiment-IDs (PP-0446)

Neue App-Projekte speichern `project.id` und Experimentprojekte zusätzlich
`experiment.id` einmal in `physim.project`. IDs bestehen aus 32 kleinen
Hexadezimalzeichen ohne Pfadbestandteile. Save/Build erhalten sie; Kopien
des Projektordners behalten die Identität. Der UI-Generator ist getrennt
vom Simulations-RNG. IDs dienen der Herkunftszuordnung, nicht Authentifizierung.

Der Runner erhält `--project-id` / `--experiment-id` und speichert genau diese
Werte im CRC-geschützten Metadatenchunk. Ohne Herkunft stehen beide Felder auf
`unavailable`. Ungültige/doppelte Optionen erzeugen keine Datei. Modelle dürfen
die Hostfelder nicht deklarieren; direkte oder über CR eingeschobene Kollisionen
werden vor dem Anlegen der Laufdatei abgewiesen.

Ausgeführte Intel-macOS-Nachweise:

- Parser/Save, unveränderte Formatmigration, tatsächliche C-/Physim-Logger und
  ID-CLI **5/5**, `run-3mh51hba`,
  `build/current-project-identity-final-install-mac.log`. Ungültige Länge,
  Großbuchstaben, Pfade, Tab, Unicode und doppelte IDs sind konkrete Gegenfälle.
  Parsefehler bewahren die vollständigen vorherigen Settings.
- App-Gesamtketten für C-Feder und Physim-Pendel **2/2**, `run-mfa15p_e`,
  `build/current-project-identity-workflows-mac.log`. Der echte Archivreader
  bestätigt beide Manifest-IDs in den Metadaten der ausgeführten Läufe.
  Bestehende Bilanz-/Berichtsprüfungen bleiben enthalten.

Ältere Projekte werden beim Öffnen nicht umgeschrieben. Eine zusätzliche
explizite ID-Vergabe für Bestandsprojekte und typisierte Herkunftsangaben für
beliebige Core-/Physim-Writer bleiben offen; PP-0446 bleibt unvollständig.
Das verschobene SDK mit installierter und separat neu gebauter Bibliothek
besteht **3/3** Verbraucherbefehle: `build/current-project-identity-sdk-verify-mac.log`,
Verbraucher `build/current-project-identity-sdk-consumer-mac/Native SDK ä ckvbf5oa`.
Hier werden die tatsächlichen C-/Physim-Runner mit denselben ID-/Manipulations-
gegenfällen ausgeführt. Die ganzen aktuellen Linux-/macOS-/Windows-/SDK-Gates
sind damit nicht abgenommen.

Die anschließenden Parser-/Offline-Referenz-/Kit-/Completion-Prüfungen bestehen
**5/5**, `run-anay7i_c`, `build/current-project-identity-docs-mac.log`.
Das unabhängige Verification Kit umfasst **182** exakte Quellen einschließlich
aller neuen Manipulationsproben. Die private gemeinsame Lexikprüfung ist im
SDK-Include-Verzeichnis enthalten.


## Linux-GUI und sechs Rückstau-Hosts am Stand 6648ee9

Die vollständigen Logs der folgenden CI-Jobs sind gelesen. Sie gelten für
`6648ee9615f322ed6f2681bb8745faaf768d7e58`, einschließlich Ereignisaufzeichnung,
vor der späteren Projekt-ID-Erweiterung.

[Linux-UI-Sanitizer](https://github.com/PhysicSimulator/physim/actions/runs/38186529506):
GCC Job 114609929200 besteht **3/3** native (`run-lfg0rdlb`) und **7/7** GUI
(`run-8yg_2vl2`); Clang Job 114609929346 besteht **3/3** native (`run-zk3ldkop`)
und **7/7** GUI (`run-k74jmfs8`). Die sieben Abläufe enthalten Projektbaum,
Completion, API-Referenz, beide Physim-Auswertungen, Feder und Auftrieb.

[Rückstau-CI](https://github.com/PhysicSimulator/physim/actions/runs/38186529531)
führt `runner_backpressure` und `runner_backpressure_slow_advance` in Debug
tatsächlich auf allen sechs Host-/Compilerkombinationen erfolgreich aus:

| Umgebung | Job | Ergebnisverzeichnis | Fälle |
| --- | --- | --- | --- |
| Linux/GCC | 114609929534 | run-c0gnqk0t | 2/2 |
| Linux/Clang | 114609929553 | run-r5jphijd | 2/2 |
| macOS/Apple Silicon | 114609929370 | run-lew2uolc | 2/2 |
| macOS/Intel | 114609929706 | run-ymj3504v | 2/2 |
| Windows/ClangCL | 114609929628 | run-bh6ofqoj | 2/2 |
| Windows/MSVC | 114609929677 | run-z5tqonu1 | 2/2 |

Dies belegt die benannten Abläufe. Die komplette neue C17-/Paket-/SDK-Matrix
ist damit nicht abgenommen. Die spätere Projekt-ID-Erweiterung benötigt eigene
Remote-Nachweise.


## Explizite ID-Vergabe bei Bestandsprojekten (PP-0446)

`physim-build --assign-project-ids --project PATH` und die App-Aktion
„Projekt-IDs vergeben“ ergänzen fehlende IDs. Vorhandene IDs und sämtliche
vorherigen Manifestbytes bleiben erhalten; die Formatversion wird nicht
umgestellt. Analyseprojekte erhalten nur eine Projekt-ID. Die atomare
Dokumentspeicherung sichert das alte Manifest und erkennt externe Änderungen.
Wiederholung ist ein verifizierter No-op ohne Backupersetzung.

Ausgeführte Intel-macOS-Nachweise:

- Native Vertrags-/Parser-/Migrationsgruppen **3/3**, `run-gb74rv1u`,
  `build/current-legacy-ids-first-mac.log`. LF, CRLF, fehlender letzter
  Zeilenumbruch, teilweise vorhandene IDs, Analyseprojekte, Dateirechte,
  veraltete Snapshots, Größenlimit und Backupfehler sind konkrete Gegenfälle.
  Fehler bewahren Manifest, Snapshot und Ergebnis.
- Tatsächliche C-/Physim-CLI-Projekte **1/1**, `run-457b0ffm`,
  `build/current-legacy-ids-workflow-mac.log`: Quellen, vorhandene Archive
  und Build-Dateien sind vor/nach Vergabe byteweise unverändert. Anschließend
  bauen beide Projekte und zeichnen mit genau den vergebenen IDs auf.
- Direkter AppleClang-ASan/UBSan-Lauf von Vergabe/Parser/Dokumentersetzung
  besteht unter `build/current-legacy-ids-asan.log`; LeakSanitizer ist deaktiviert.
- Verschobenes SDK mit installierter und separat neu gebauter Bibliothek:
  **4/4** Verbraucherbefehle unter `build/current-legacy-ids-sdk-verify-mac.log`.
  Verbraucher: `build/current-legacy-ids-sdk-consumer-mac/Native SDK ä w6wo7d47`.
  Darunter laufen die echte CLI-Vergabe und folgende C-/Physim-Projektbuilds
  aus dem installierten Paket.

Die installierte SDK-App besteht dieselben zwölf ID-Buttonfälle unter
`build/current-legacy-ids-sdk-app-mac.log`; Marker
`Actual app project identity assignment: twelve quiet/noisy cases ... passed`.
Der unabhängige Verification-Kit-Lauf umfasst **183** genaue Eingänge,
`build/current-legacy-ids-kit-mac.log`; Referenzgenerator **31/31** Dokumente.

Die neue Display-Suite besteht **1/1**, `run-h1cxrul7`, unter
`build/current-legacy-ids-app-confirmed-mac.log`: zwölf tatsächliche ruhige/
gestörte App-Prozesse bei zwei Fenstergrößen prüfen den sichtbaren Button,
vergebene IDs, unveränderte Quellen/Archive sowie Stale-/Dirty-Guards.
Die bisherige Formatmigrations-Appgruppe besteht ebenfalls unter
`run-f4z766mb`; nur die falsche Artefaktpfad-Deklaration der neuen Gruppe wurde
anschließend korrigiert. Typisierte Herkunft frei programmierter Core-/Physim-
Writer bleibt offen; PP-0446 ist weiterhin unvollständig.


## Abgeschlossener vollständiger Intel-macOS-SDK-Nachweis für 03ca6a2

Der exakt eingefrorene Stand `03ca6a252bd0816b5a91ae25f5732bf9f3eb7dfc`
besteht nach **708/708** nativen Release-Tests auch die vollständige verschobene
SDK-Prüfung mit App: **608/608** Verbraucherbefehle haben Exit 0.
`PASSED.txt` ist vorhanden. SDK-Log: `build/current-full-sdk-03ca6a2-mac.log`;
Verbraucher: `build/current-full-sdk-03ca6a2-consumer-mac/Native SDK ä nkyc0ydr`.
Alle **1366** Quellhashes und die ursprüngliche Tar-Prüfsumme sind danach
unverändert; die Eingänge wurden während der Prüfung nicht repariert.

Der Lauf enthält installierte und aus SDK-Quellen neu gebaute Cores, alle
C-/Physim-Lernorakel, Mittelwertintervalle, Reader/Writer-Besitz, zwölf kalte
native Domänenprojekte und die tatsächlichen App-Abläufe. Die abschließende
Tastatur-Projektmatrix baut/zeichnet/analysiert alle **32** Template-/Sprachpaare
und zwei unabhängige Analysen bei 16/22 px. Menü-/Handbuch-/Einstellungs-/
Schrittbreakpoint-/Runner-Isolations-/Accessibility-Abläufe sind ausgeführt.
Die früheren allgemeinen Feder-/Auftriebs-Berichtsfehler treten dabei nicht auf.

**408** genaue Bitmaparchive umfassen **7 048 453 104** unkomprimierte Bytes.
Alle Gzip-Dateihashes sowie die rückgewonnenen Bytezahlen und SHA-256-Werte
bestehen gegen `bitmap-archive.json`; Nachweis:
`build/current-full-sdk-03ca6a2-bitmap-integrity.log`.

Dies ist die volle lokale Intel-macOS-Abnahme dieser Suite und dieses
Quellstands. Die späteren positiven Verteilungen, Ereignisblöcke und Projekt-
IDs gehören zu anderen, gezielt geprüften Ständen. Aktuelle gesamte Linux-/
Windows-/Apple-Silicon-Pakete sowie die ganze Projektplan-Abnahme bleiben offen.


## Typisierte Laufidentität in C und Physim (PP-0446)

`ps_run_identity` / `RunIdentity` speichern zwei validierte Hex32-IDs als
unabhängigen Wert ohne RNG oder Allokation. C-Low-Level-/Handle-Writer und
Physim `RunStore.setIdentity` archivieren die typisierten Hostfelder. Vor
Dateierstellung werden ungültige Identitäten und kollidierende Modellmetadaten
abgewiesen. Decoder/Reader unterscheiden vollständige Identität, fehlende/
unavailable Herkunft und beschädigte/doppelte Felder. Eingebettetes NUL wird
nicht als verstecktes Metadatenende akzeptiert. Der Physim-Konstruktor und
Metadaten-Decoder übergeben ausdrücklich die ganzen String-Bytebereiche.

Auf Intel macOS tatsächlich ausgeführt:

- Bestehende Streams, Sprachbesitz, Ereignisse und Dateimutationen zusammen
  mit neuen typisierten IDs **4/4**, `run-wmim9dp9`,
  `build/current-typed-identity-confirmed-mac.log`.
- Neue Sprachwerte einschließlich Arrays, Records, Optionals, Getter-Kopien,
  freiem Metadaten-Decoder und zukünftiger Store-Konfiguration bestehen
  `run-gias0zfi`, `build/current-typed-identity-span-native-mac.log`.
  Ein unabhängiger Python-/zlib-Decoder bestätigt bytegleiche C-/Handle-/
  Physim-Dateien einschließlich Schema, IDs, Daten, Index und Footer.
  CRC-korrekt doppelte, ungültige, fehlende, unavailable und NUL-Metadaten
  sowie Großschreibung sind konkrete Gegenfälle mit erhaltener Ausgabe.
- Direkter AppleClang-ASan/UBSan-Lauf von C-Werten/Writer/Reader/Store
  besteht unter `build/current-typed-identity-asan.log`; LeakSanitizer ist
  lokal deaktiviert.
- Tatsächliche C-Feder-/Physim-Pendel-App-Gesamtketten **2/2**, `run-pioeflw1`,
  `build/current-typed-identity-workflows-mac.log`; die gespeicherten Manifest-
  IDs und vorhandenen Bilanz-/Berichtsverträge bleiben geprüft.

Die neuen APIs verändern Datei-/API-/ABI-/IPC-Versionen nicht. Alte Writer-
Aufrufe und Dateien ohne vollständige Herkunft bleiben verfügbar; fehlende
Identität wird nicht erfunden. Der ID-Block ist damit implementiert, die ganze
aktuelle Linux-/Windows-/Apple-Silicon-/Produktabnahme bleibt offen.

Die abschließende verschobene SDK-Prüfung besteht **11/11** Verbraucherbefehle
mit installierter und neu gebauter Bibliothek, expliziter Legacy-Vergabe und
typisierten C-/Handle-/Physim-Archiven: `build/current-typed-identity-final-sdk-verify-mac.log`;
Verbraucher `build/current-typed-identity-final-sdk-consumer-mac/Native SDK ä h2h3trsh`.
App-/Referenz-/Kit-/Completion-/Katalogprüfungen bestehen **4/4**, `run-8xxyj3dl`;
Offline-Parser, Bestandsprojekt-Vergabe und tatsächliche Runner-IDs bestehen
**3/3**, `run-vepyebiv`, `build/current-typed-identity-final-docs-mac.log`.
Das unabhängige Kit prüft **187** genaue Eingänge; **31** Referenzdokumente und
**640 C / 607 Physim** Completion-Einträge sind abgeglichen.


## Compiler- und Buildherkunft im Archiv (PP-0447)

Core-Writer ergänzen `core_compiler` / `core_build`; der Runner ergänzt
`module_compiler`, `module_build`, `runner_compiler`, `runner_build` sowie den
bisherigen Modulhash. C-Vorlagen und Physim-Module deklarieren Kompilierungs-
herkunft mit `PS_EXPERIMENT_BUILD_INFO` und dem optionalen ABI-3-Tail.
Compilerkennungen und Buildliterale stammen aus der jeweiligen Übersetzung.
Alte Module ohne Capability tragen `unavailable`; ihre fremden längeren
Tails werden nicht als neue Zeigerfelder gelesen. Die adaptive Mindestgröße
bleibt am ursprünglichen Callback-Ende. API-/ABI-/Dateiversionen bleiben gleich.

Ausgeführte Intel-macOS-Nachweise:

- Identität, typisierte Writer und beide realen Logger **4/4**, `run-54gcidic`,
  `build/current-build-origin-second-mac.log`.
- Neue Herkunfts-/ABI-Gruppen **2/2**, `run-g87t7klp`,
  `build/current-build-origin-capability-mac.log`. Tatsächliche C-/Physim-
  Module archivieren Core-/Runner-/Modulversionen und Buildtexte. Ein
  wirklich kürzerer alter adaptiver API-Struct und ein längerer fremder
  Tail mit absichtlich ungültigen Zeigern laufen erfolgreich. Ungültige
  Modultexte sowie gefälschte Core-/Modulmetadaten erzeugen keine Laufdatei.
  Wiederholte Läufe derselben Artefakte bleiben bytegleich.
- Instrumentierter alter adaptiver ABI-Struct: AppleClang-ASan/UBSan besteht
  unter `build/current-build-origin-asan.log`; instrumentierter fremder
  Tail unter `build/current-build-origin-asan-future.log`. LeakSanitizer
  ist lokal deaktiviert.
- SDK-Installation und Referenz-/Katalog-/Kit-/Completion-Prüfungen **4/4**,
  `run-a4cu4nmh`, `build/current-build-origin-final-sdk-install-mac.log`.

Builddatei-Literale sind keine Laufstartzeit und keine Signatur. Die getrennte
Anforderung PP-0448 ist für die tatsächliche Laufstartzeit noch unvollständig.
Diese Nachweise ersetzen keine ganze aktuelle Plattform-/Produktabnahme.

Die abschließenden Herkunfts-/ABI-/typisierten Writer-Gruppen bestehen
**3/3**, `run-nv2g8nvx`, `build/current-build-origin-verified-native-mac.log`.
Experimentnamen sind begrenztes, nichtleeres einzeiliges UTF-8; weder ein
Nullname noch über den Namen eingeschobene Metadaten erzeugen eine Datei.
Das verhindert zusätzliche Hostfelder außerhalb der Modellmetadatenprüfung.
SDK-Installation mit Referenz-/Katalog-/Kit-/Completion-Prüfungen besteht
**4/4**, `run-ffvx33xx`, `build/current-build-origin-verified-sdk-install-mac.log`.

Das endgültige SDK besteht **3/3** Verbraucherbefehle mit installierter und
neu gebauter Bibliothek, realen C-/Physim-Modulen, altem adaptivem ABI, fremdem
Tail und fünf konkreten Herkunfts-/Namens-Gegenfällen:
`build/current-build-origin-verified-sdk-verify-mac.log`; Verbraucher
`build/current-build-origin-verified-sdk-consumer-mac/Native SDK ä 6pcchh_e`.
Die finalen C-Feder-/Physim-Pendel-Appketten bestehen **2/2**, `run-hp687osr`,
`build/current-build-origin-verified-workflows-mac.log`. Das unabhängige Kit
umfasst **191** exakte Eingänge; **31** Referenzen und **643 C / 607 Physim**
Completion-Einträge sind abgeglichen.
Der nochmals mit der endgültigen Namensprüfung instrumentierte Runner besteht
mit kurzem altem ABI, fremdem Tail und Nullnamen: `build/current-build-origin-asan-verified.log`.


## Tatsächliche archivierte Startzeit (PP-0448)

Neue Core-Writer speichern die unmittelbar vor dem exklusiven Anlegen gelesene
System-Wanduhr als ganzzahlige Unixsekunden/Nanosekunden im Metadatenchunk.
C-/Handle-/Physim-Werte bleiben nach Reader-/Store-Close gültig. Der Decoder
prüft Breite, Bereich, Duplikate, Teilfelder und eingebettete NUL-Bytes; Fehler
erhalten die Ausgabe. Alte Dateien ohne Startzeit bleiben lesbar. Die feste
Breite erhält Indexpositionen; Uhrwerte sind keine Simulationszeit und kein
Versprechen über Genauigkeit oder Auflösung der Systemuhr.

Ausgeführte Intel-macOS-Nachweise:

- Native Herkunfts-/Identitäts-/Event-/Stream-/Batch-Gruppen **6/6**,
  `run-0_p6o4ju`, `build/current-start-time-confirmed-native-mac.log`.
- Endgültiger Startzeittest mit Abweisung gefälschter Hostfelder ohne Datei
  **1/1**, `run-or03uqof`, `build/current-start-time-final-native-mac.log`.
  Der unabhängige Pythondecoder prüft reale UTC-Grenzen, CRC-korrekte Legacy-/
  beschädigte Varianten und INT64_MIN. Wiederholungsparität normalisiert nur
  beide validierten Uhrwerte samt Metadaten-CRC; eine geänderte Messzahl wird
  weiterhin erkannt, Index und Footer bleiben vollständig verglichen.
- Verschobenes SDK mit installierter und aus SDK-Quellen neu gebauter
  Bibliothek **7/7** Verbraucherbefehle:
  `build/current-start-time-sdk-verify-mac.log`; Verbraucher
  `build/current-start-time-sdk-consumer-mac/Native SDK ä zs0n2tc0`.
- Direkter AppleClang-ASan/UBSan-Lauf besteht:
  `build/current-start-time-asan.log`; LeakSanitizer ist lokal deaktiviert.
- Tatsächliche C-Feder-/Physim-Pendel-Appketten **2/2**, `run-_kmqrrid`,
  `build/current-start-time-workflows-mac.log`.
- SDK-Installation mit Referenz-/Kit-/Completion-/Katalogabgleich **4/4**,
  `run-av2526u_`, `build/current-start-time-sdk-install-mac.log`.
  **31** Referenzdokumente, **649 C / 615 Physim** Completion-Einträge.

API-/ABI-/Dateiversionen bleiben gleich. Diese fokussierten Nachweise ersetzen
keine ganze aktuelle Linux-/Windows-/Apple-Silicon-/Produktabnahme.

Die abschließenden Offline-Parser-/Referenz-/Kit-/Completion-/Katalogprüfungen
bestehen **5/5**, `run-_fvbcdo_`, `build/current-start-time-final-docs-mac.log`.
Das unabhängige Prüfkit enthält **196** genaue manifestierte Eingänge ohne
Physim-Implementierung. Ausschließlich aus diesem entpackten Kit besteht die
verschobene SDK-Gegenprobe erneut **7/7** Befehle:
`build/current-start-time-independent-sdk-verify-mac.log`; Verbraucher
`build/current-start-time-independent-sdk-consumer-mac/Native SDK ä 7am6_vcu`.


## Einzelprüfung der Archivverträge (§8.2)

Vorhandene Implementierung am Stand `2ce2f12f4fb0f11df388aa8ca7b9930a2dd1e67e`:
Header mit Magic/Version/Endianness, append-only CRC-Chunks, exklusives Anlegen,
optional übersprungene unbekannte Chunks und rekonstruierbarer Abschlussindex.
Die gezielte Prüfung verändert keine Produktimplementierung.

Intel macOS/AppleClang Release besteht CRC, RunStream, Szenen, deterministische
Mutationen, C-Index und Physim-Index **6/6**, `run-cwjylwew`,
`build/current-archive-contracts-mac.log`. Der Mutationstest hat **10.130** Fälle,
**3.312** erfolgreiche Opens und **7.617** gültige gelesene Zeilen geprüft.
Der unabhängige Pythondecoder prüft exakte Headerbytes, CRCs, paginierte
Indexeinträge, absolute Offsets und Footer für **1.000.000** Messzeilen.
Fehlender Index/Footer, CRC-falsche und CRC-korrekt semantisch falsche Seiten
sowie fremde Indexversionen erzwingen validierte Rekonstruktion.
Allocatorfehler erhalten Ausgaben und hinterlassen keine eigenen Allokationen.

Core und Physim-Stream-Integration bestehen zusätzlich **2/2**, `run-j3rpjscz`,
`build/current-archive-contracts-final-mac.log`: exklusive Erstellung vorhandener
Ziele, CSV-Quoting, sämtliche Dateitrunkierungen, beschädigte Messpayload und
gültiger unbekannter Typ 999 mit unveränderten Messwerten/Abschluss. C-Index
prüft Typ 99. Beschädigte Chunks werden nicht als gültige unbekannte Daten
übersprungen. Die Rekonstruktion erfindet keinen erfolgreichen Abschluss;
unvollständige Dateien melden `PS_RECOVERED`.

Die Nachweise gelten für diesen lokalen Intel-macOS-Stand. Sie ersetzen keine
ganze Plattformmatrix und keine zugesicherte transaktionale Speicherung bei
Stromausfall oder Veränderungen durch fremde Prozesse.


## Gezielte CI und instrumentierte Archive am Stand 2ce2f12

Die vollständige Logauswertung der Linux-UI-Sanitizer-CI
[38194127484](https://github.com/PhysicSimulator/physim/actions/runs/38194127484)
belegt unter Clang **3/3** native Fälle (`run-bkvpm7mx`) und **7/7** tatsächliche
Appfälle (`run-6dp5z8kj`), unter GCC **3/3** (`run-kjrvbadh`) und **7/7**
(`run-anfu4uok`). Job-IDs: 114632955791 / 114632956032. Die Displayfälle umfassen
Floating, C-/Physim-Gesamt-/Mischpfad, Projektbaum, Completion, API-Referenz und
Feder. Die einzelnen Passed-Marker und Abschlusszahlen wurden gelesen.

Die Rückstau-CI [38194127463](https://github.com/PhysicSimulator/physim/actions/runs/38194127463)
belegt jeweils **2/2** Debug-Fälle, normal und künstlich langsamer Vorlauf:

| Host/Compiler | Job | Ergebnisverzeichnis |
| --- | --- | --- |
| Linux/GCC | 114632955952 | run-csittzif |
| Linux/Clang | 114632955976 | run-jtnkw_pt |
| macOS/Apple Silicon | 114632955801 | run-_55llue6 |
| macOS/Intel | 114632955973 | run-ky8qqonb |
| Windows/ClangCL | 114632955962 | run-fviav4hv |
| Windows/MSVC | 114632956059 | run-v_chz19z |

Der lokale direkte AppleClang-ASan/UBSan-Build instrumentiert alle **28**
Core-Dateien. Die identische Archivkampagne besteht mit **10.130** Fällen,
**3.312** erfolgreichen Opens und **7.617** gültigen Zeilen:
`build/current-archive-contracts-asan.log`; LeakSanitizer bleibt deaktiviert.

Die großen Remote-C17-/Linux-Paketläufe für diesen Stand wurden abgebrochen;
sie zählen nicht als erfolgreicher Nachweis. Die gezielten Ergebnisse ersetzen
keine vollständige Plattform-/SDK-Abnahme.


## Vollständiger 2ce2f12-Lauf und Herkunftsintegration

Der vollständige historische Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#vollständiger-2ce2f12-lauf-und-herkunftsintegration) erhalten.

## Vollständiger nativer Intel-macOS-Lauf für c497a26

Der unveränderte eingefrorene Stand `c497a266987ff054f54f9f9aac0ad351365e73eb`
besteht **716/716** native Release-Tests und installiert das SDK erfolgreich.
Alle 716 JSON-Statuswerte sind `passed`; alle **1.398** Quellhashes bleiben
unverändert. Ergebnisse: `build/current-full-build-c497a26-mac/test-results/run-83xrnz23/results.json`;
Log: `build/current-full-build-c497a26-mac.log`. Die vollständige verschobene
SDK-/App-Prüfung läuft anschließend unter `build/current-full-sdk-c497a26-mac.log`.
Dies beweist noch keinen Abschluss dieser SDK-Prüfung. Die späteren
Laufdetails-/Modellschichtänderungen gehören zu einem anderen Stand.

## Archivierte Modellschichten und App-Laufdetails (PP-0482)

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#archivierte-modellschichten-und-app-laufdetails-pp-0482) erhalten.

## Modellschichten aller acht Standardvorlagen

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#modellschichten-aller-acht-standardvorlagen) erhalten.

## Abgeschlossene vollständige Intel-macOS-SDK-Prüfung für c497a26

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#abgeschlossene-vollständige-intel-macos-sdk-prüfung-für-c497a26) erhalten.

## Archivierte äußere Serienherkunft

Der vollständige Nachweis bleibt im [Plattformarchiv](platform-validation-history.md#archivierte-äußere-serienherkunft) erhalten.

## Modellschichten der weiteren ausgelieferten Experimente

Die restlichen **27 Physim-/17 C-Module** deklarieren nun die sieben Schichten.
Zusammen mit den acht Standardpaaren sind alle 35 vom SDK gebauten Physim-
Experimentmodule und 25 C-Beispielexperimente erfasst. Solver, Materialien,
Messung und Domänenbeispiele nennen ihre konkreten Grenzen: Stabilisierung kann
Energie verändern, diskrete Kontakte besitzen keine CCD, der schnelle Kugelfall
hat eine Ereignisgrenze, Stokes benötigt Gültigkeitsgrenzen, die Saite eine
CFL-Bedingung, synthetische Stofftabellen sind keine Kalibrierung und das
Rohr-/Tracerbeispiel ist keine CFD. Logging, Darstellung und absichtlich
fehlschlagende Diagnosen werden ausdrücklich so bezeichnet.

`scene_shapes` hat keine Messkanäle. Der Runner lehnt die Archivierung mit
Exit 6 ab und legt keine Datei an. Ein unabhängiger C-Verbraucher lädt das
Modul, prüft Create/Reset, acht gültige Szenenobjekte und veränderte Geometrie
nach Step; er liest die tatsächliche Deklaration. Es wurde kein künstlicher
Messkanal ergänzt. Diagnosemodule werden mit null Schritten als gültige
Initialarchive geprüft; ihr absichtlicher Fehler bleibt erhalten.

Ausgeführte Intel-macOS-/AppleClang-Release-Nachweise:

- **20/20** Tutorial-/Kit-/Beispielgruppen, `run-9g6z78ku`,
  `build/current-extra-layers-final-mac.log`; alle 18 Tutorialquellgruppen stimmen
  mit ihren ausgeführten Quellen überein.
- **49** vollständige CRC-geprüfte Archive: 43 normale/initiale Fälle und sechs
  Parameterwechsel für Pendel-Drag/Rauschen und inelastische Stöße. Deklarationen
  gleicher C-/Physim-Profile stimmen exakt überein.
- Gegen Modellquellen aus dem unveränderten `4c90a35`-Snapshot bestehen alle
  **49** bytegleichen Kanalschemas und Messdatenblöcke mit demselben aktuellen
  Runner/Core: `build/current-extra-layers-baseline-compare-mac.log`.
- Das unabhängige **204**-Dateien-Kit besteht **29/29** SDK-Befehle mit
  installierter und ausschließlich aus dem Paket neu gebauter Bibliothek,
  **98** Archivfällen und zwei tatsächlichen kanalfreien Modulprüfungen:
  `build/current-extra-layers-sdk-verify-mac.log`, Verbraucher
  `build/current-extra-layers-sdk-consumer-mac/Native SDK ä w5w59uzy`.
  `--examples-only` ist eine fokussierte Gegenprobe, keine Gesamtplattformabnahme.

Die aktuelle vollständige Linux-/Windows-/macOS-Abnahme bleibt offen.
PP-0482 bleibt deshalb im Gesamtplan unvollständig. Der parallel laufende
vollständige eingefrorene `4c90a35`-Lauf umfasst diese späteren Deklarationen nicht.

Die abschließende Dokumentations-/Kit-/Archivprüfung bestand zunächst 3/4,
`run-vvkbtdc3`: die generierte Experimentreferenz enthielt noch die alte
uniform-motion-Quelle. Nach Neugenerierung besteht die betreffende Referenz-
prüfung **1/1**, `run-gix0im_m`,
`build/current-extra-layers-reference-verified-mac.log`. Offline-Seitengrenzen,
alle 531 Originaltexte und die ursprüngliche Planprüfsumme sind erneut geprüft.


## Einzelabnahme von Zeitsteuerung und Messdaten (PP-0303..0310)

Am `c0fdb33`-Stand mit ergänztem unabhängigem Schemaorakel bestehen auf
Intel macOS/AppleClang Release **6/6**, `run-x43qmpw5`,
`build/current-live-data-audit-mac.log`: `pacing`, beide echten C-/Physim-
Runnerzeitsteuerungen, verzögerter Offline-Beobachter, Sensorreferenz und
C-/Physim-Kanaldeklaration. Gefrorene Zeit prüft Akkumulator und begrenzte
Wallclock-Schulden nach Suspension; reale Runner prüfen Pause, Einzelschritt,
Live-Speedwechsel und vollständige Messwertgleichheit trotz langsamer Leser.
Offline entfernt Scheduling-Warten; Disk-/IPC-/Observerlatenz besitzt keine
absolute Laufzeitgarantie. Vorschau-Snapshots bleiben ausgewählt, jeder
akzeptierte Physikschritt wird persistiert.

Das ergänzte unabhängige Orakel prüft alle drei archivierten Beschreibungen,
Namen, SI-Einheiten/Dimensionen und die tatsächliche binary64-Zeit/Wert-Darstellung
in CRC-Chunks sowie CSV-Parität. Drei Sensorvarianten besitzen jeweils 6001
vollständige Zeilen mit Unsicherheit/Gültigkeit, ohne Einfluss auf die idealen
Modellwerte. Der Datentyp ist für Laufdateiformat 1 global Float64; Unsicherheit
ist ein optionaler eigener SI-Kanal, keine automatische Schätzung des Writers.

PP-0303/0304/0305/0309/0310 sind damit einzeln zugeordnet. Das ist keine neue
Gesamtplattformabnahme. Die 531 Originaltexte/Planprüfsumme bleiben erhalten:
**116 implemented**, **14 incomplete**, **401 unverified**.

Die anschließenden Dokumentations-/Referenzprüfungen bestehen **2/2**,
`run-4ai91kcy`, `build/current-live-data-docs-mac.log`.


## Einzelabnahme der grundlegenden Mechanik (PP-0375/0381..0386)

Am `e2e7ed6`-Stand bestehen auf Intel macOS/AppleClang Release **7/7**,
`run-t05kxf_v`, `build/current-mechanics-audit-mac.log`: `mechanics`,
`distance_joint`, `broad_phase`, `language_joint`, `language_collision_parity`,
`spring_reference` und `collision_reference`. Die Quellenprüfung ordnet sechs
Planblöcke einzeln zu.

Punktmodelle verwenden den translationsreinen Body-Pfad; Kräfte am Schwerpunkt
und fehlende Rotation lassen die Hauptträgheiten aus dieser Bewegung heraus.
Analytische Kraft-/Impuls-/Energievergleiche, Kugel-/Boxträgheit, gedrehte Anker,
freie Rotation, gyroskopische Beschleunigung, Energie-Konvergenz sowie
Feder-/Gelenk-/Reibungs-/Restitutionsreferenzen bestehen. Die unabhängige
Broad-Phase-Referenz vergleicht 80 vollständige Paarmengen und den maximalen
1024-Boxen-Fall einschließlich Grenzberührung und Fehleratomizität.

SI-Feldnamen und Quantity-/Kanal-Dimensionsmetadaten sind explizit; ein double
trägt keine implizite statische Einheit. Die tatsächliche SI-Kanalgrenze ist
zusätzlich in `run-x43qmpw5` geprüft. Diese Einzelabnahme schließt weder die
Mehrkörper-CCD-Gesamtabnahme noch eine neue Gesamtplattformmatrix ab.
Aktuell: **122 implemented**, **14 incomplete**, **395 unverified** bei
531 unveränderten Originalblöcken und ursprünglicher Planprüfsumme.

Die Dokumentations-/Referenzprüfung besteht **2/2**, `run-9cbs_s9b`,
`build/current-mechanics-audit-docs-mac.log`.


## Vollständiger nativer 4c90a35-Lauf: veraltetes Diagnoseorakel

Der eingefrorene Stand `4c90a35591a66db0c655b804bbd5da7fcde249d4` beendet
seinen Release-/AppleClang-Lauf mit **720/721**, `run-e7226be2`,
`build/current-full-build-4c90a35-mac.log`. `runner_identity` erwartete den alten
Wortlaut „host-owned identity fields“; der Runner nennt inzwischen Identität,
Build und Serienfelder gemeinsam. Gefälschte IDs werden weiter mit Exit 5
abgewiesen, ohne Laufdatei. Die SDK-Folgeprüfung ist wegen dieses Testfehlers
nicht gestartet; dieser Lauf ist kein vollständiger grüner Paketnachweis.

Das Orakel prüft nun Rückgabecode, Host-/Identitätskategorie und das Ausbleiben
von Lauf-/Logdateien. Es besteht mit den unveränderten eingefrorenen Binärdateien
unter `build/current-frozen-identity-recheck`. Aktuelle Identitäts-/Serien-
Prüfgruppen bestehen **2/2**, `run-zb3subru`,
`build/current-identity-regression-mac.log`. Alle **1409** eingefrorenen Dateien
und die TAR-Prüfsumme sind unverändert bestätigt. Die ursprüngliche Fehlprüfung
wurde im eingefrorenen Verzeichnis nicht nachträglich umgeschrieben.


## Expliziter 2D-Ebenenmodus (PP-0295)

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#expliziter-2d-ebenenmodus-pp-0295) erhalten.

## Neun ausgelieferte Materialeigenschaften (PP-0393)

Der vollständige lokale/SDK-/Linux-Nachweis bleibt in
[Archivband 2](platform-validation-history-2.md#neun-ausgelieferte-materialeigenschaften-pp-0393) erhalten.

## Vollständiger nativer Intel-macOS-Lauf für 0cab455

Der vollständige Nachweis mit **722/722** nativen Tests, **704/704** SDK-Befehlen
und allen **424** prüfsummenbestätigten Bildarchiven bleibt in
[Archivband 2](platform-validation-history-2.md#vollständiger-nativer-intel-macos-lauf-für-0cab455) erhalten.

## Projekt-Anzeigevorgaben aus der Oberfläche

Der vollständige lokale/SDK-/Linux-Nachweis bleibt in
[Archivband 2](platform-validation-history-2.md#projekt-anzeigevorgaben-aus-der-oberfläche) erhalten.

## Standardkamera und freie 3D-Bedienung (PP-0294)

Der vollständige lokale/SDK-/Linux-Nachweis bleibt in
[Archivband 2](platform-validation-history-2.md#standardkamera-und-freie-3d-bedienung-pp-0294) erhalten.

## Stabile Module und Parameterdefinitionen in der Projektdatei

Der vollständige lokale/SDK-/Linux-Nachweis bleibt in
[Archivband 2](platform-validation-history-2.md#stabile-module-und-parameterdefinitionen-in-der-projektdatei) erhalten.

## Einzelne und gemeinsame Szenensichtbarkeit (PP-0296)

Rendering und Auswahl verwenden jetzt denselben Filter für individuelle
Einträge, vererbte Gruppensichtbarkeit und gemeinsame Vektor-/Pfad-/Punkt-/
Beschriftungsschalter. Der Filter prüft transparente und leere Label-Einträge
und verändert die Szene nicht. Bestehende stabile IDs erhalten die Sichtbarkeit
bei Umordnungen; anonyme Einträge behalten ihre bisherige Strukturpolitik.

Intel macOS/AppleClang Release: **2/2** echte GUI-Fälle bei 16/22 Pixeln,
`run-c5t0nj6s`, `build/current-scene-visibility-ui-mac.log`. Jeder Fall bedient
alle vier Schalter, die individuelle Objektcheckbox, **Alle einblenden** und
Tastatur-Auswahl/Ausblenden. Tatsächlich eingereichte Geometrie, fehlende
Treffer ausgeblendeter Objekte und sichtbare Label-Bounds sind geprüft.
Ein verstecktes Objekt behält nach einer gezielten Fixture-Umordnung seine ID-
bezogene Sichtbarkeit; Szene, Messwerte und Simulationszeit bleiben während
aller Bedienhandlungen unverändert. Die 22-Pixel-Ansicht wurde visuell geprüft.

Die abschließende Installation besteht **6/6** native Sichtbarkeits-, Hierarchie-,
Koordinatenrahmen-, Dokumentations-, Referenz- und Kitfälle, `run-vjvcc0ok`,
`build/current-scene-visibility-install-mac.log`. Native Sichtbarkeitsproben
umfassen alle **16** globalen Schalterkombinationen, einzelne/vererbte
Sichtbarkeit, Grenzslots und ID-Erhalt. Direkte ASan-/UBSan-Instrumentierung
des View-Filters besteht unter `build/current-scene-visibility-asan-mac.log`;
die Core-Bibliothek ist dabei uninstrumentiert, LeakSanitizer deaktiviert.

Das frische unabhängige **208**-Dateien-Kit besteht mit **2/2** Befehlen gegen
das verschobene SDK: `build/current-scene-visibility-sdk-verify-mac.log`, Consumer
`build/current-scene-visibility-sdk-consumer-mac/Native SDK ä 0oq1lw0w`.
Die Linux-Sanitizer-CI enthält jetzt beide zusätzlichen Sichtbarkeitsfälle;
ihr Ergebnis für diese Änderung ist noch offen. Der eingefrorene vollständige
c60cced-Lauf gehört zum vorherigen Stand und enthält diese Erweiterung nicht.
Die gesamte Plattform- und Produktabnahme bleibt offen.

## SecondBrain Projektgedächtnis und Quellen

`brains/physim` verwendet das tatsächliche Schema 1/Vorlage 1 von SecondBrain
mit elf Markdown-Notizen, kleinem JSON und relativem `project_root`. Die Originale
bleiben in docs/examples; der Quellenindex verweist darauf. Ein lokal gebauter
SecondBrain-C-Kern 0.9.74, Revision 70266ae85c62, erkennt Physim, erzeugt den
Kontext und findet die Modulanforderungsnotiz, ohne Notizen umzuschreiben.
`tools/check-secondbrain.py` prüft außerdem alle relativen Originalquellenlinks.

Die vollständige Installation besteht **3/3** Dokumentations-/Referenz-/Kitfälle,
`run-yzhoimn7`, `build/current-secondbrain-complete-install-mac.log`. Sie enthält
auch die Brain-Dateien, Projektanweisungen und den Quellenkatalog. Die Kopie
unter `build/SecondBrain complete SDK ü/Relocated SDK ä` besteht denselben
CLI-/Linktest; alle **498** SDK-Dateien bleiben SHA-256-validiert.
Die Markdownseiten und Metadaten sind auch in der Offline-Navigation registriert.
Die graphische SecondBrain-Leseansicht wurde hierbei nicht erneut abgenommen.

## Vollständiger nativer Intel-macOS-Lauf für c60cced

Der eingefrorene Stand c60cced3888c3d305ff12b759407efb8dc77d753 besteht
AppleClang Release **724/724**, `run-dx2sis73`,
`build/current-full-build-c60cced-mac.log`. Alle Ergebnisstatus, **1417**
Quellprüfsummen und die TAR-Prüfsumme sind unabhängig bestätigt.
Die zugehörige vollständige verschobene SDK-/App-Prüfung ist ebenfalls erfolgreich:
**714/714** protokollierte Befehle, kein fehlgeschlagener Ergebnisstatus,
`build/current-full-sdk-c60cced-mac.log`. Die Prüfung umfasst die tatsächliche
Oberfläche einschließlich aller 32 Vorlagen-/Sprachpaare und beider Analysearten
bei 16/22 px. Der Verbraucher liegt unter
`build/current-full-sdk-c60cced-consumer-mac/Native SDK ä q3j4a2m9`.
Alle **438** BMP-Archivbelege wurden unabhängig dekomprimiert: **7435401244**
Originalbytes, identische Original- und Archivprüfsummen. Die **1417**
eingefrorenen Quellprüfsummen und die TAR-Prüfsumme sind erneut bestätigt;
`build/current-full-c60cced-independent-proof.json` enthält die Zählung.
Spätere Sichtbarkeits- und Dokumentationserweiterungen gehören nicht zu diesem
Stand. Der gesamte Projektplan bleibt offen.

## Native Dokumentation und SecondBrain-Lernwege

Am 12. Oktober 2026 wurden auf Intel macOS 14.6.1 mit AppleClang 16 die
überarbeiteten User-Guide-/Example-Seiten und zehn ergänzte Tutorials geprüft.
Alle veröffentlichten Codeblöcke der zehn Tutorials sind gegenüber dem vorherigen
Stand unverändert. Beide Lernwege behalten sieben Kapitel und acht gemeinsame
C-/Physim-Modellgruppen. Referenz, Dokumentationsparser, Lernwege und unabhängiges
Prüfungspaket bestehen **4/4**, `run-hokakdqk`,
`build/current-documentation-native-sdk-mac.log`.

Die native F1-Hilfe zeichnet die gebündelten Vektordiagramme und eine echte
GPU-Pendelszene mit eigenem Modell-/Kamerazustand. Regler, Kameratasten,
Projektion und Szene sind per Tastatur bedienbar; Experimentzustand und Zeit
bleiben erhalten. Die Medien-/Scroll-Gegenproben bestehen bei 16/22 px
zusammen mit den bisherigen Tastatur-Lernwegen **4/4**, `run-x4804ujz`.
Die Bildschirmaufnahmen wurden visuell geprüft; ein Überlappen nach der
Schriftgrößenanpassung wurde korrigiert. Der Browserentwurf ist entfernt;
Quellenlinks kopieren ausschließlich ihre Adresse.

Das installierte SDK wurde nach `build/Documentation SDK ü/Relocated SDK ä`
verschoben. Die vier tatsächlichen App-Fälle `docs-keyboard-media`,
`docs-keyboard-media-22`, `docs-keyboard`, `docs-keyboard-22` bestehen dort.
Der echte SecondBrain-C-Kern prüft Erkennung, Kontext und Suche; relative
Originalquellenlinks bleiben gültig. Alle **506** SDK-Dateien bleiben
SHA-256-identisch. Die letzten redaktionellen Nachweistexte und SVG-Titel
ändern dieses vorher installierte Paket nicht nachträglich.

Die neuen Medienfälle sind in der Linux-Sanitizer-CI und SDK-Gesamtprüfung
registriert. Für diese Erweiterung wird hier ausschließlich die tatsächlich
lokal ausgeführte macOS-Prüfung behauptet. Der vollständige Projektplan und
seine 531 Originaldatensätze bleiben offen und unverändert.

Der abschließende redaktionelle Stand besteht erneut **4/4**, `run-7p1u2nwf`,
`build/current-documentation-native-final-sdk-mac.log`. Alle 506 Prüfsummen des
abschließenden SDK sind gültig; seine App ist bytegleich zur verschobenen
GUI-Gegenprobe. Der Dokumentationsparser besteht zusätzlich lokale ASan-/UBSan-
Prüfungen. Die eigenständige Runnerprüfung besteht nach Ergänzung der bereits
vorhandenen Projektidentitäts-GUI-Gegenprobe in ihrem Erwartungskatalog;
`build/current-documentation-runner-check-final.log`.

## Linux-Dokumentationsmedien am Stand 9c93312

Die tatsächlichen Linux-Sanitizerläufe 38233906692 bestehen mit Clang und GCC
jeweils **9/9** native und **18/18** GUI-Fälle. Die GUI-Fälle umfassen
`documentation_media_16/22` und `scene_visibility_16/22`; ASan/UBSan sind
aktiviert. Die vollständigen Joblogs wurden gelesen und lokal gespeichert:
Clang 114759764403 (`run-98iaibao`, `run-cfll_v0w`) und
GCC 114759764521 (`run-g_qndpts`, `run-j4hiho1b`). Die Belege liegen unter
`build/current-ci-9c93312-job-<Job-ID>.log`. Diese Nachweise gelten für den
gepushten Dokumentationsstand, nicht für spätere Reglererweiterungen.

## Native skalare Accessibility-Regler

Das portable Modell ergänzt benannte endliche Wertebereiche mit aktuellem Wert,
Minimum, Maximum und Schrittweite. Queued Änderungen erreichen nur den nächsten
lebenden, aktiven Controlbesuch; spätes Queueing, begrenzte Werte, Bereichswechsel,
deaktivierte/entfernte IDs und getrennte Fenster bleiben geschützt.
Der Modellfall besteht lokal unter ASan/UBSan. AppKit veröffentlicht AXSlider,
numerische Werte/Grenzen, Increment/Decrement und Value-Setter mit tatsächlichen
nativen Gegenproben einschließlich zurückbehaltener ungültiger Referenzen.

Auf Intel macOS bestehen native AppKit-/gezeichnete UI-, Dokumentations-, Kamera-
und Laufpositionsfälle **7/7**, `run-xdy5fed8`,
`build/current-accessibility-range-all-sliders-mac.log`. Beide Mediengrößen
bestehen nach der zusätzlichen exakten Oben-Ansicht **2/2**, `run-vkccwltz`.
Die Parameter bleiben unabhängig von Experiment und gespeicherten Messdaten.
Das aktuelle SDK besteht Modell, UI-Geometrie, Dokumentation, Referenz und
Prüfungspaket **5/5**, `run-dkt020sj`,
`build/current-accessibility-range-final-sdk-mac.log`.

Die Linux-Anbindung ergänzt Slider/Value, den eigentlichen skalaren Setter und
Wertänderungsereignisse. Ein unabhängiger PyAT-SPI-Prüfer und ein echter gezeichneter
Zweifensterfall sind registriert. Die Ausführung dieser neuen Anbindung bleibt
bis zum aktuellen Linux-CI-Ergebnis unbewiesen. Windows/UIA und praktische
Screenreader-Abnahme bleiben offen; PP-0710 wird weiterhin nicht abgeschlossen.

Die abschließende Richtungskonsistenz besteht **4/4**, `run-acr_u_0h`,
`build/current-accessibility-range-direction-mac.log`. Die verschobene App unter
`build/Accessibility range SDK ü/Relocated SDK ä` besteht beide Medienfälle
einschließlich der exakten Oben-Ansicht. Alle **506** SDK-Prüfsummen bleiben
unverändert. Spätere redaktionelle Nachweise verändern das geprüfte Paket nicht.

Das abschließend gebaute SDK unter `build/current-accessibility-range-reviewed-sdk-mac`
besteht erneut **5/5**, `run-yvywb2gl`,
`build/current-accessibility-range-reviewed-sdk-mac.log`; es enthält auch die
einheitlichen Tastaturrichtungen. Die vollständige Gegenprobe für 9c93312 ist
inzwischen nativ **724/724** erfolgreich, `run-nen1n6ic`; ihr eingefrorenes SDK
läuft weiter und enthält diese spätere Reglerarbeit ausdrücklich nicht.
