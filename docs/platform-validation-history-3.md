# Historische Plattformnachweise Band 3

Diese Nachweise wurden am 13. Oktober 2026 ohne Änderung ihrer Aussagen
aus der [Plattformprüfung](platform-validation.md) ausgelagert. Die genannten
Revisionen und Umgebungen bestimmen ihre Gültigkeit.

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
