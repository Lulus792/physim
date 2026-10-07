# Plattformprüfung

Stand: 7. Oktober 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

## Getrennte C-/Physim-Dokumentation am 7. Oktober 2026

[Teil I: C](c-guide.md) und [Teil II: Physim](physim-guide.md) enthalten jeweils
sieben geordnete Kapitel, eigene ausführbare Spracheinstiege, vollständige
Experiment-/Analyseabläufe, Referenzen und dieselben acht Modelllernwege.
Das Hilfefenster wählt beide Teile separat aus und erhält den Startpunkt beim
Besuch gemeinsamer Themen. Die Prüfung lädt alle registrierten Themen,
kontrolliert Quelltext und Links und bedient die Auswahl und Startknöpfe über
SDL-Mausereignisse, auch im verkleinerten Fenster.

Die sechs gezielten Prüfungen bestehen auf Intel macOS (Apple Clang 16) unter
`build/contact-world-language-release-mac/test-results/run-c_azenr8` und
`run-8qrrmi9x` (je drei Fälle), unter Debian 12/GCC 12.2 unter
`build/contact-world-language-release-linux/test-results/run-hbhstge9` (sechs Fälle).
Die endgültige grafische Navigation besteht auf macOS unter `run-p4g3hwpq`
und Linux unter `run-mltm2ijg` in den jeweiligen Release-Testordnern.

Beide ausschließlich aus dem 43-Dateien-Prüfkit gestarteten SDK-Prüfungen
bestehen vollständig, einschließlich aller neun grafischen Projektabläufe und
der neuen Hilfefenster-Navigation:

- macOS: `build/documentation-tracks-sdk-proof-mac/Native SDK ä 00qmysrj`.
- Linux: `build/documentation-tracks-sdk-proof-linux/Native SDK ä 2s2tu214`.

Beide Prüfungen bauen die Spracheinstiege aus den verschobenen SDK-Quellen,
prüfen die veröffentlichten Modellquellen und führen die bisherigen fachlichen
Prüfungen gegen installierten und neu aufgebauten Core aus. Die endgültigen
Pakete `build/Documentation routes clean SDK ä mac` und
`build/Documentation routes clean SDK ä linux` enthalten 374 Manifestdateien
und weiterhin 63 kompilierte Sprachprodukte. Alle Code-/Beispieldateien werden
byteweise gegen die geprüften SDK-Kopien verglichen, die Apps gegen die
geprüften Release-Binaries. Zum Abschluss werden nur Dokumentation und Manifest
aktualisiert. Der Katalog umfasst jetzt 577 Fälle ohne Fenster und 562 ohne SDL;
die vorherige vollständige 572-Fälle-Abnahme wird dadurch nicht umgedeutet.
Die aktuelle Windows-/ARM-/Clang-Matrix bleibt ein eigener Abnahmenachweis.

## Batch-Hostdienste und Physim 0.178.0 am 7. Oktober 2026

Analysen erhalten einen optionalen, größen- und versionsgeprüften `run_host`-Tail
innerhalb ABI 3. Ein ausdrücklicher Host liefert den vorhandenen Controller;
der Core bleibt unabhängig von Prozess- und GUI-Code. Besitzende `Batch`-Werte
kopieren ihre Konfiguration und Ergebnisse, einschließlich Arrays, optionaler
Werte, Strukturfelder und Closures. Sie starten feste, zeitgesteuerte, adaptive
und Parameterstudien und können nach journalierter Teilerledigung pausieren.
Die Fortsetzung validiert die alte Serie erneut und schreibt ausschließlich in
einen neuen Ordner. [Benutzung und vollständige Beispiele](batch-language.md).

Die gemischte Prüfung führt 32 C-/Physim-Analysekonfigurationen und acht CLI-
Referenzen aus. Sie vergleicht alle gespeicherten Zeitpunkte und Werte,
Controllerzähler, Messstatus, Quellen, Histogramme, exklusiven Ausgabezugriff
und unveränderte frühere Archive. Die vollständigen C-/Physim-Monte-Carlo-
Beispiele werden in allen vier Kombinationen geprüft: 1024 rohe Trajektorien,
unabhängig berechnete Mittelwerte, Streuungen, Type-7-Quantile und Histogramme.
Die gezielte macOS-Prüfung besteht mit 10/10 unter
`build/contact-world-language-release-mac/test-results/run-hkaxnwf8`.
Die sieben gezielten Linux-Clang-14-ASan/UBSan-Prüfungen bestehen unter
`build/batch-language-clang-asan/test-results/run-uzbcfl58`.
Fehlgeschlagene Allokationen erzeugen keine Serien; verkürzte oder unbekannte
Hostdeskriptoren werden vor dem Callback abgewiesen. Ein gespeichertes Unit-
Symbol bleibt nach der Freigabe aller Batch-Kopien gültig.

Der [vorherige CI-Stand](https://github.com/PhysicSimulator/physim/actions/runs/37554714527)
bestätigt Linux/GCC, Windows/v143 Debug und Release sowie Windows/ClangCL Debug.
Die per veröffentlichtem SHA-256-Digest geprüften ClangCL-Release-, Linux-Clang-,
ARM- und Intel-Artefakte enthalten weitere Testfehler: erneute Floating-Point-
Berechnung statt Prüfung der geschriebenen Snapshotbits, ein abweichender
Rückgabetyp des alten `ps_get_analysis`-Fixturesymbols, Timing vor bestätigter
Fortsetzung beziehungsweise unabhängig von tatsächlicher verstrichener Zeit
und bitweise C-/Physim-Gleichheit adaptiver Messwerte. Die Tests bewahren jetzt
die geschriebenen Bytes, verwenden die korrekte Entry-Signatur, messen nach
Rückmeldung und prüfen numerische Parität mit einer 100-mal strengeren relativen
Grenze als der konfigurierte RK45-Vertrag. Anzahl, Endzeit und Struktur bleiben
gesondert geprüft. Clang 14 reproduziert den alten Entry-Ausfall nicht; die
erneute tatsächliche aktuellen Clang-/Windows-/ARM-CI-Abnahme bleibt erforderlich.

Auf dem Intel-Mac blockierten neue unsignierte Testprogramme sowie der
Homebrew-Git-Client im Loader. Eine lokal ad hoc signierte, aus denselben
Quellen gebaute Probe bestand. Der Builder signiert deshalb ausschließlich
frisch gelinkte temporäre Mac-Artefakte vor Veröffentlichung und Cacheprüfung;
Code-Signierfehler erhalten die vorherige Ausgabe. Der systemeigene Apple-Git-
Client ist verwendbar. Bereits blockierte Prozessinstanzen bleiben als
Betriebssystemproblem gesondert zu behandeln; ihre Läufe sind keine Testnachweise.

Ältere Linux-Prüfverzeichnisse unter `build/project-audit` sind vor der Freigabe
des VM-Speichers vollständig auf dem Mac unter
`build/project-audit-linux-evidence.tar.gz` gesichert. Alle 19115 regulären
Dateien wurden gegen ihre ursprünglichen SHA-256-Werte geprüft. Archiv-SHA-256:
`e09978e09f6aa49459134acb0eeb73b444b901f91ef3d1da820f8440f7650f14`.

Die vollständigen erneuten Release-Suiten bestehen mit **572/572** auf Intel
macOS unter `build/contact-world-language-release-mac/test-results/run-vliklgjg`
und Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-nr1abvnk`.
Die letzte Korrektur der Timingmessung und die eigenständige Einbindung des neuen
Host-Headers erhalten zusätzliche Nachweise: vier Mac-App-Prüfungen unter
`run-9o8ahm9d`, vier Linux-App-Prüfungen unter `run-ehaq83ko` und drei Linux-
Batch-Prüfungen unter `run-2z5ei6pe`, jeweils in den genannten Release-Testordnern.

Beide endgültigen, ausschließlich aus dem 41-Dateien-Prüfkit gestarteten SDK-
Prüfungen bestehen unter `build/batch-language-sdk-proof-mac/Native SDK ä kx911ojx`
und `build/batch-language-final-sdk-proof-linux/Native SDK ä pji_aiq0`.
Sie bauen alle öffentlichen Header separat nach stdio-/Locale-Headern,
prüfen die neuen Dienste sowie 2048 neue Batch-Beispielarchive je Plattform
gegen installierten und neu aufgebauten Core und führen die bisherigen
Lernpfade weiter aus. Linux besteht zusätzlich alle neun grafischen SDK-Abläufe.
Die endgültigen Pakete `build/Batch language clean SDK ä mac` und
`build/Batch language clean SDK ä linux` enthalten 368 Manifestdateien und
63 kompilierte Sprachprodukte. Code und Beispiele werden byteweise gegen die
verifizierten Kopien geprüft; die jeweiligen Apps müssen ebenfalls mit den
geprüften Release-Binaries übereinstimmen. Nur Dokumentation und ihr Manifest
werden zum Abschluss aktualisiert. Die neue Remote-Matrix bleibt gesondert offen.

## Elternprozess-Abbruch und Paketierungsfehler am 7. Oktober 2026

Ein unabhängiger Versuch mit dem unveränderten SDK von `222d4bb` beendet nur
die PID des Seriencontrollers. Der getrennte Runner schreibt danach weiter in
seine Heartbeat-Datei; die Fixture wurde anschließend gezielt beendet.
Die neue Offline-Pipeüberwachung endet auch innerhalb eines hängenden Create-
oder Step-Callbacks mit Code 125. Ein- und Vierworker-Serien prüfen den harten
Controllerabbruch, unveränderte abgeschlossene Archive, erhaltene Journalzeilen
und das Fehlen eines erfundenen Gesamtberichts.

Die sechs gezielten Runner-/Serienprüfungen bestehen auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-w2zuyyw1` und Debian
unter `build/contact-world-language-release-linux/test-results/run-nhql7sdy`.
Die neue Abbruchfixture besteht zusätzlich unter Linux mit ASan/UBSan unter
`build/spring-tutorial-asan-linux/test-results/run-um7q1_rf`.
Der grafische Endzeit-Ablauf besteht nach einer Korrektur der Scroll-/Klickfolge
im Test auf macOS unter `run-fj81o4wf` und Linux unter `run-2fkhvsx1` in den
jeweiligen Release-Testordnern. Die Prüfung ersetzt die Texteingabe nicht durch
direktes Setzen des App-Zustands.

Der [CI-Lauf von `222d4bb`](https://github.com/PhysicSimulator/physim/actions/runs/37550644122)
zeigt weitere Fehler im getrennten SDK-Schritt und in Windows-Tests.
Die heruntergeladenen GCC-/ARM-Artefakte scheitern am installierten
`language-contact_stack`-Modul: `--install` hatte ohne `--examples` keine
kompilierten Sprachbeispiele erzeugt. Installation baut jetzt den vollständigen
Katalog automatisch; der SDK-Prüfer verlangt alle Produkte und Manifesteinträge.
Die Windows-Testartefakte zeigen einen CRLF/LF-Vergleichsfehler in der
Extremwertprüfung und `select()` auf einer Pipe im Logging-Test. Der Textleser
verwendet den CRT-Textmodus; ein begrenztes Queue-/Thread-Leseverfahren erhält
die Fristprüfung auch für Windows-Pipes. Aktuelle Windows-Abnahme benötigt
einen erneut tatsächlich ausgeführten Workflow.

Die drei ausgewerteten Archive (`linux-ui-gcc`, `macos-macos-15` und
`sdk-windows-v143-Debug`) stimmen byteweise per SHA-256 mit den über die
GitHub-Artefakt-API veröffentlichten Digests überein. Ihre Hauptsuiten bestehen
mit 564/564 auf GCC und ARM sowie 561/564 unter Windows/v143 Debug. Die drei
Windows-Ausfälle sind ausschließlich `analysis_extremes`, `runner_logging_c`
und `runner_logging_phys`; dieser alte Stand bestätigt noch keine Korrektur.

Diese Arbeiten sichern bestehende Serien und Release-Prüfungen. Die im
Anforderungsabgleich fehlende Physim-Sprachbindung für Batch ist weiterhin offen.

Die vollständigen Release-Suiten mit der Abbruchprüfung und den CI-Testkorrekturen
bestehen mit **565/565** auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-n9fesl5k` und Debian/GCC
unter `build/contact-world-language-release-linux/test-results/run-vl2t4muf`.
Beide SDKs wurden mit `--install` ohne `--examples` erzeugt und enthalten den
vollständigen Katalog mit 61 kompilierten Sprachprodukten.

Beide ausschließlich aus dem unabhängigen Kit gestarteten SDK-Prüfungen bestehen:
macOS unter `build/parent-watch-sdk-proof-mac/Native SDK ä cb0fysqc`, Linux unter
`build/parent-watch-sdk-proof-linux/Native SDK ä m7a3cycd`. Linux führt zusätzlich
alle neun grafischen SDK-Projektabläufe erfolgreich aus. Die endgültigen Pakete
unter `build/Parent watch clean SDK ä mac` beziehungsweise `build/Parent watch clean SDK ä linux`
erhalten die aktuelle Dokumentation mit erneuertem SHA-256-Manifest. Alle Code-
und Beispieldateien werden gegen die verifizierten Kopien geprüft; die jeweilige
App muss zudem bytegleich zum geprüften Release-Binary bleiben.

## Gesamtabgleich und CI-Prüfpfade am 7. Oktober 2026

Der [Abgleich mit dem vollständigen Plan](project-audit.md) bewahrt 531 Quellblöcke
mit Originaltext und Zeilenbereichen, darunter alle 475 Aufzählungspunkte.
Ein SHA-256-Wert bindet die Erfassung an die unveränderte Plandatei.
Acht Lernpfade besitzen begrenzte Implementierungsnachweise; zehn konkrete
Lücken sind erfasst. 513 weitere Blöcke bleiben ohne Einzelabnahme. Diese Zahlen
sind kein Fertigstellungsprozentsatz: Entwürfe, Nicht-Ziele und spätere Domänen
bleiben mit ihrem ursprünglichen Kontext enthalten. Das Gesamtziel ist offen.

Am Ausgangsstand `4841ee2` bestehen die vollständigen Release-Suiten auf
Intel macOS unter `build/contact-world-language-release-mac/test-results/run-7kin6e77`
und Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-dbd70ucn`
mit jeweils 563/563 Fällen. Die zugehörigen CI-Artefakte für Linux/GCC und
Apple Silicon bestätigen ebenfalls 563 bestandene Fälle; ihre Archivbytes
stimmen mit den auf GitHub veröffentlichten SHA-256-Digests überein. Das sind
standbezogene Nachweise und keine Abnahme nachträglicher Änderungen.

Die getrennten CI-Schritte enthielten drei reproduzierbare Fehler: Ein mit nur
sechs Dateien gepacktes Linux-Prüfkit fehlte unter anderem `sdk_series_probe.c`;
der separate Sprachbeispielprüfer hatte die Dateien für `run_index_values`
nicht erzeugt; Linux/Clang wies den Logger-Test wegen einer nicht deklarierten
GNU-Funktion `strtod_l` nach vorangegangenen libc-Headern zurück.

Das Prüfkit wird nun aus den expliziten Repository-Dateiabhängigkeiten des
SDK-Prüfers und lokalen Testheadern erzeugt. Es enthält 33 unabhängige Harness-
Dateien plus SHA-256-Manifest, keine Core-Implementierung oder SDK-Header.
Fehlende, dynamische oder unerlaubte Pfade verhindern die Veröffentlichung.
Bestehende Archive bleiben erhalten; ein Schreibfehler entfernt nur die neu
erzeugte Ausgabe. Der separate Beispielprüfer baut zuerst die vorhandene
öffentliche Indexfixture und erzeugt damit finalisierte, alte und rekonstruierbare
Eingaben. Anschließend bestehen die 18 Programme, alle 43 vorhandenen Module
und die Tests für inkrementelle Übersetzung sowie Fehlererhaltung auf macOS
und Linux/GCC. Test-Runner- und Bootstrap-Selbstprüfungen bestehen separat auf
macOS; ihre absichtlich fehlschlagenden Fälle sind Teil der Prüfung.

Die Linux-Sprachlaufzeit verwendet im Dezimalkomma-Fallback `strtod` mit einer
vorübergehenden POSIX-Thread-Locale und stellt danach die aufrufende Locale
wieder her. Die Windows- und macOS-Implementierungen bleiben erhalten.
Ein strenger Clang-C17-Compile mit `-Werror=implicit-function-declaration`
reproduziert den vorherigen Fehler und besteht mit der Korrektur.
Linux/Clang 14 Debug besteht Logger, Locale, Sprachchecker und Kitprüfung unter
`build/project-audit-clang-linux/test-results/run-daitiwoc`; die Localeprüfung
lief tatsächlich mit installiertem `de_DE.utf8` und kontrolliert auch den
Erhalt eines eigenen aufrufenden Thread-Locale-Handles. Die drei Laufzeit-
Prüfungen bestehen mit AddressSanitizer/UndefinedBehaviorSanitizer unter
`build/spring-tutorial-asan-linux/test-results/run-cg8grlij`.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.

Das nur aus dem Kit entpackte SDK-Prüfprogramm besteht auf macOS unter
`build/project-audit/kit-sdk-proof-mac/Native SDK ä 4gzy3db9` und Linux unter
`build/project-audit/kit-sdk-proof-linux/Native SDK ä 0iuzgm9e`. Linux führt dabei
zusätzlich alle neun grafischen C-/Physim-Projektabläufe aus. Die geprüften SDKs
sind die unveränderten Installationen des vorherigen Lernpfads; aktuelle Pakete
mit der Localekorrektur erhalten gesonderte Nachweise.

Die erneuten vollständigen Release-Suiten mit allen aktuellen Änderungen bestehen
mit 564/564 auf dem Intel-Mac unter
`build/contact-world-language-release-mac/test-results/run-ty_ic4b4` und Debian/GCC
unter `build/contact-world-language-release-linux/test-results/run-1_8_4_wp`.
Dieser Nachweis ersetzt keine noch fehlenden aktuellen Windows-/Apple-Silicon-
CI-Ergebnisse und keine vollständige Produktabnahme.

Die neue Offline-Abnahmeseite und die gesamte Dokumentnavigation bestehen
unter macOS mit `build/contact-world-language-release-mac/test-results/run-p4cryhht`
und unter Linux mit `build/contact-world-language-release-linux/test-results/run-9gtpbm_o`.
Das neu installierte macOS-SDK mit der Localekorrektur besteht den vollständigen,
ausschließlich aus dem Kit gestarteten Prüfer unter
`build/project-audit/final-kit-sdk-proof-mac/Native SDK ä 9pkl2d81`.
Alle öffentlichen Header werden dabei nach bereits eingebundenen stdio-/Locale-
Headern kompiliert, um die problematische Einbindungsreihenfolge ausdrücklich zu prüfen.

Auch das neue Linux-SDK mit der Localekorrektur besteht den vollständigen
isolierten Prüfer und alle neun grafischen Projektabläufe unter
`build/project-audit/final-kit-sdk-proof-linux/Native SDK ä x_rvzwxt`.
Die endgültigen Pakete erhalten die aktuellen Nachweise mit erneuerten
Manifestwerten. Je Paket bleiben alle 141 Code-/Binärdateien und 110 Beispieldateien
bytegleich zur verifizierten Installation; die App bleibt bytegleich zur
geprüften Release-App. Alle 357 Manifestdateien werden per SHA-256 geprüft.
Die tatsächlichen Umgebungen sind Intel macOS 14.6.1/Apple Clang 16 und Debian 12/
GCC 12.2 beziehungsweise die genannten gezielten Clang-14-Prüfungen, jeweils
SDL 3.2.30. Linux-Grafik lief unter X11/Xvfb/Openbox/Mesa. Die Behebung der
remote CI bleibt durch den nächsten tatsächlichen Workflowlauf zu bestätigen.

## Gespeicherten Lauf unabhängig auswerten am 7. Oktober 2026

Der neue Lernpfad verwendet die bestehenden gleichförmigen C-/Physim-Experimente
und zwei vollständige neue Archivanalysen. Ein eigenständiges Analyseprojekt
importiert einen gespeicherten Lauf, leitet `position.x` über zentrale Sekanten
ab und rekonstruiert die Position mit Trapezen ab dem archivierten Anfangswert.
Drei Plots zeigen Position/Rekonstruktion, Geschwindigkeit und Residuum; zwei
Tabellen speichern Laufumfang, Wiederherstellungsstatus, Anfangs-/Endposition,
Verschiebung, Sekantengeschwindigkeit und maximale Rekonstruktionsabweichung.
Der CSV-Export enthält sämtliche fünf Reihen. Das Verfahren benötigt genau
einen Lauf, mindestens zwei Messpunkte, eine streng steigende Zeitachse und
Längenwerte. Ein Nulloffset prüft Dimensionskompatibilität; die gemeinsame
Positionsachse verwendet die kanonischen Meter der rekonstruierten Reihe,
auch wenn das Archiv ein anderes Anzeigeeinheitensymbol enthält.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.
[Lernziel, Modell, Gleichungen und vollständige Quellen](saved-run-tutorial.md).

Der unabhängige Pythonprüfer erzeugt neben beiden echten Modellarchiven sechs
weitere Archive direkt mit eigener PSRUN-Kodierung und CRCs: unregelmäßige
quadratische Positionen, eine spätere Startzeit, zwei rückwärts gerichtete
Messpunkte, ein lesbares Archiv ohne Abschlussfooter, ein abweichendes
Anzeigeeinheitensymbol und 2.049 Messpunkte. Beide Analysesprachen verarbeiten
alle acht Fälle. Ein separater C-Prüfer liest die Originalarchive, berechnet
Sekanten und Trapeze selbst und kontrolliert alle Berichtskurven, Vorschau-
Endpunkte, SI-Achsendimensionen/-maßstäbe/-symbole sowie beide Tabellen.
Python vergleicht jeden Wert der vollständigen CSV und die beiden Analysen
untereinander. Alle 16 gemischten Berichte bestehen; bei der großen Reihe
bleiben sämtliche 2.049 CSV-Zeilen erhalten, während die Vorschau auf höchstens
2.048 Punkte begrenzt ist. Datei-SHA-256-Werte bleiben nach jeder Analyse gleich.

Leere, zu kurze, mehrfach ausgewählte oder fehlende Eingänge, doppelte Zeiten,
fehlende Positionskanäle und falsche Dimensionen werden ohne fertigen Bericht
zurückgewiesen. Wiederhergestellte gültige Präfixe tragen ausdrücklich den
Tabellenwert `Recovered prefix=1`; fehlende spätere Messpunkte werden nicht
hinzugefügt. Die dokumentierten Grenzen unterscheiden Datenverarbeitung von
physikalischer Validierung und erklären Rauschen sowie grobe/unregelmäßige
Abtastung. Vier vollständige Dokumentationsblöcke entsprechen exakt den gebauten
Experiment-/Analysequellen.

macOS Release besteht drei ausgewählte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-fnegvx10`, Linux GCC
Release unter `build/contact-world-language-release-linux/test-results/run-l1y8qzpf`:
neuer Lernpfad, Quellcodegleichheit und bisheriges C-Dokumentationsbeispiel.
Linux Debug besteht den neuen Lernpfad mit AddressSanitizer und
UndefinedBehaviorSanitizer unter
`build/spring-tutorial-asan-linux/test-results/run-0vhqs921`.

Beide Plattformen bestehen drei ausgewählte Fensterabläufe: macOS unter
`build/contact-world-language-release-mac/test-results/run-l9s6h_uq`, Linux unter
`build/contact-world-language-release-linux/test-results/run-6ho99fgb`.
Der neue Ablauf erzeugt je ein echtes Archiv mit 32 angehaltenen Schritten
zu 0,0625 s und importiert es in ein Analyseprojekt der jeweils anderen Sprache.
Die Projekte enthalten keine Experimentquelle und kein Experimentmodul;
Simulation wird abgewiesen. Der Import erfolgt über die tatsächliche
Schaltfläche mit injiziertem Testpfad, nicht über eine automatisierte native
Dateiauswahl. Der Test kontrolliert 33 Messpunkte, drei Plots, zwei Tabellen,
Position 3 m und Geschwindigkeit 1,5 m/s und öffnet das Projekt samt Bericht
anschließend ohne Build erneut. Python vergleicht Original und importierte
Datei per SHA-256. Zusätzlich bestehen die bisherigen eigenständigen
Analyseprojekte und die Offline-Dokumentnavigation einschließlich des neuen
Lernpfads. Die geprüften Intel-macOS-/Debian-12-Umgebungen und Compiler-/SDL-
Versionen entsprechen den folgenden Nachweisen. Position, Kennzahlentabelle
und wiedergeöffnete Geschwindigkeit wurden auf macOS bei 1280 × 900 logischen
Pixeln visuell geprüft, Position und Geschwindigkeit unter Linux bei 1280 × 900.

Beide neu installierten SDKs bestehen den vollständigen Relokationsprüfer:
macOS unter `build/saved-run-tutorial-sdk-proof-mac/Native SDK ä tt9nk6ee`,
Linux unter `build/saved-run-tutorial-sdk-proof-linux/Native SDK ä eo571jes`.
Der Prüfer kontrolliert alle Manifestdateien und öffentlichen Header, baut Core
und alle 43 Sprachmodule aus der mitgelieferten Quelldistribution neu und führt
die bisherigen Index-, Kontakt-, Diagnose-, Serien- und Beispielprüfungen aus.
Der neue Lernpfad läuft mit installiertem und neu gebautem Core sowie über die
dokumentierten nativen Builds von Erzeuger und eigenständigem Analyseprojekt.
Letzteres darf kein Experimentmodul erstellen. Alle acht Archivszenarien und
16 gemischten Berichte bestehen in jedem Durchgang. Die endgültigen Pakete
enthalten die aktuellen Nachweise mit erneuertem Manifest: jeweils 355 Dateien
mit SHA-256-Prüfung. Alle 141 Code-/Binärdateien und 110 Beispieldateien bleiben
bytegleich zur verifizierten Installation; die App bleibt bytegleich zur
geprüften Release-App.

Der aktuelle Katalog umfasst 563 Prüfungen ohne Fenster (548 ohne SDL),
66 Fensterfälle, 18 eigenständige Sprachprogramme und 43 Experiment-/Analysemodule.
Ein Gesamtlauf aller aktuellen Fälle wird hier nicht behauptet; der frühere
Gesamtlauf aller 555 Fälle bleibt weiter unten belegt. Der Lernpfad schließt
keine anderen noch offenen Anforderungen des Projektplans ab.

## Monte Carlo mit unsicheren Anfangswerten am 7. Oktober 2026

Vier vollständige Quellen liefern in C und Physim denselben Vakuumwurf mit
normalverteilten Anfangsgeschwindigkeiten und eine Auswertung seiner archivierten
Serie. Die vier typisierten Geschwindigkeitsparameter gehören zur Modellinstanz.
Die Bewegung ist analytisch, ohne Sensorrauschen oder Integrationsfehler.
Neun Kanäle speichern Ist-/Sollposition, Geschwindigkeit, Populationsstreuung und
Energie; die Szene zeigt beide Bahnen und ein Kreuz von einer Standardabweichung.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.
[Lernziel, Modell, statistische Annahmen und vollständige Quellen](monte-carlo-tutorial.md).

Der unabhängige Pythonprüfer berechnet PCG32-Ziehungen und Ballistik selbst.
Er kontrolliert 1.536 archivierte Läufe mit jeweils 33 Messzeilen: vier Serien
mit 256 unterschiedlichen Seeds in beiden Sprachen und mit einem beziehungsweise
vier Workern sowie zwei konstante Serien mit beiden Standardabweichungen null.
Innerhalb derselben Sprache sind die Endwert-CSV-Dateien bei anderer Parallelität
bytegleich; beide Sprachen stimmen innerhalb der numerischen Vergleichsschranken
überein. Der Prüfer liest PSRUN-Magic, CRCs, Footer, sämtliche neun Kanäle,
SI-Dimensionen und Seedprovenienz unabhängig. Szenen für Seeds 0, 42, einen Wert
oberhalb 2^63 und UINT64_MAX stimmen in sämtlichen Objekt- und Punktfeldern
überein. Fehlende beziehungsweise gekürzte Mitglieder, verschiedene Endzeiten
und geänderte Populationsparameter werden von beiden Analysen abgewiesen.
Die Instanzprobe prüft unabhängige Parameter,
Seed-Wiederholung, Seed-Wechsel, Nullstreuung, Energie und unveränderte Messwerte
nach abgewiesenen Schritten.

Beide Analysesprachen lesen beide Laufsprachen. Vier Plots zeigen alle Endwerte
und die beiden Histogramme; zwei Tabellen vergleichen Mittelwert,
Stichprobenstreuung und Typ-7-Quantile mit der Modellreferenz und zeigen ein
95%-Intervall des Mittelwerts bei bekannter Normalstreuung. Ein separater
C-Prüfer kontrolliert sämtliche Kurven, Klassenhäufigkeiten und Tabellenwerte
gegen die Originalarchive. Acht gemischte Analysen bestehen, einschließlich
konstanter Populationen mit einer Histogrammklasse und Intervallbreite null.
Der CSV-Export enthält alle 256 Endwertpaare. Diese Beispielanalyse erwartet
explizit 256 archivierte Dateien; die allgemeine Serienauswertung bleibt für
andere Laufzahlen verfügbar. Vier dokumentierte Codeblöcke entsprechen exakt
den tatsächlich gebauten Quellen.

macOS Release besteht die drei ausgewählten Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-bnxl1mtn`, Linux GCC
Release unter `build/contact-world-language-release-linux/test-results/run-8exxinth`:
neuer Lernpfad, Quellcodegleichheit und bisherige Monte-Carlo-Serienreferenz.
Linux Debug besteht den neuen Lernpfad mit AddressSanitizer und
UndefinedBehaviorSanitizer einschließlich beider Experimente, Analysen und
Instanzprobe unter `build/spring-tutorial-asan-linux/test-results/run-n7_ec93l`.

Die neue Schaltfläche **Ersten Serienlauf auswerten** startet die gebaute Analyse
mit dem ursprünglichen ersten Archivpfad. Sie ist ohne abgeschlossene erfolgreiche
Serie beziehungsweise bei ungebauter oder ungespeicherter Analyse gesperrt.
Die Monte-Carlo-Beispielanalyse liest dadurch die 256 Nachbardateien, ohne den
Einzellaufimport oder dessen Kopiervertrag zu ändern. Gewöhnliche Analysen
verarbeiten über dieselbe Schaltfläche nur den ersten Lauf.

macOS besteht drei ausgewählte Fensterabläufe unter
`build/contact-world-language-release-mac/test-results/run-qzxygk3g`: der neue
Lernpfad, die bestehende Serienbedienung und die Offline-Dokumentnavigation.
Der neue Ablauf baut beide Sprachprojekte, führt 32 angehaltene Schritte aus,
öffnet 33 archivierte Messzeilen mit Szene, startet je 256 Läufe mit vier
Workern und betätigt die neue Schaltfläche tatsächlich per Mausereignis.
Er prüft anschließend den Originalpfad, vier Plots, zwei Tabellen und die
Modell-/Stichprobenwerte. Linux besteht dieselben drei Fensterabläufe unter
`build/contact-world-language-release-linux/test-results/run-8upn0u0o`.
macOS lief auf dem nativen Intel-Desktop; Linux auf Debian 12/GCC 12.2 unter
X11/Xvfb/Openbox/Mesa, jeweils mit SDL 3.2.30. Szene, Histogramm, Statistik und
Mittelwertintervall wurden auf macOS bei 1440 × 960 logischen Pixeln visuell geprüft.
Auch Szene, Histogramm und beide Tabellen wurden unter Linux bei 1440 × 960 geprüft. Breite
Statistiktabellen benötigen horizontales Scrollen; dies ist im Ablauf beschrieben.

Beide SDKs bestehen den vollständigen Relokationsprüfer: macOS unter
`build/monte-carlo-tutorial-sdk-proof-mac/Native SDK ä t7lngpyy`, Linux unter
`build/monte-carlo-tutorial-sdk-proof-linux/Native SDK ä 9_kllbwq`. Der Prüfer
kontrolliert alle Manifestdateien und öffentlichen Header, baut Core und alle
42 Sprachmodule aus der mitgelieferten Quelldistribution neu und führt die
bisherigen Index-, Kontakt-, Diagnose-, Serien- und Beispielprüfungen aus.
Der Monte-Carlo-Lernpfad läuft mit installiertem und neu gebautem Core sowie
über den dokumentierten nativen C-Projektbuild, jeweils mit den reproduzierbaren
Serien und allen acht gemischten Analysen. Die endgültigen Pakete enthalten
die aktuellen Nachweise mit erneuertem Manifest. Alle 140 Code-/Binärdateien
und 108 Beispieldateien bleiben bytegleich zur jeweils verifizierten Installation;
die App bleibt bytegleich zur geprüften Release-App. Insgesamt enthält jedes
Paket 351 per SHA-256 geprüfte Dateien.

Der aktuelle Katalog umfasst 561 Prüfungen ohne Fenster (546 ohne SDL),
65 Fensterfälle, 18 eigenständige Sprachprogramme und 42 Experiment-/Analysemodule.
Ein Gesamtlauf aller aktuellen Fälle wird hier nicht behauptet; der frühere Lauf
aller 555 Fälle ist weiter unten belegt. Die neue Analyse setzt unabhängige
Normalziehungen und bekannte Modellstreuung voraus und beweist keine allgemeine
Unabhängigkeit aufeinanderfolgender Seeds. Andere offene Anforderungen des
Projektplans bleiben offen.

## Elastischer und inelastischer Stoß am 7. Oktober 2026

Vier neue vollständige Quellen liefern ein zentrales Stoßexperiment im Vakuum
und dessen Auswertung in C und Physim. Massen, Anfangsgeschwindigkeiten und
Restitution sind typisierte Parameter je Modellinstanz. Die kontinuierliche
Kugelabfrage bestimmt den Kontakt innerhalb eines Zeitschritts; Impulsantwort
und Integration der Restzeit erhalten die Nachstoßbewegung. Das Experiment
modelliert genau diesen einen zentralen Stoß ohne äußere Kräfte, Reibung oder
Rotation. Bei vollkommen inelastischem Stoß bleiben zwei co-bewegte Kugeln
bestehen; ein gemeinsamer Körper oder eine Klebeverbindung wird nicht erzeugt.
Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate bleiben unverändert.

Ein unabhängiger Pythonprüfer kontrolliert 7.250 primäre Messzeilen aus elf
Szenarien in beiden Sprachen: elastisch, teilweise und vollkommen inelastisch,
ungleiche Massen, einseitig bewegte, auseinanderlaufende, ruhende und co-bewegte
Kugeln sowie grobe Aufzeichnung und Kontakt auf einer Schrittgrenze. Er berechnet
Stoßzeit, Impuls, Nachstoßgeschwindigkeiten, dissipierte Energie und sämtliche
Positionen aus geschlossenen eindimensionalen Formeln. Er prüft alle elf
Kanalnamen mit SI-Dimensionen, Parameterprovenienz und sämtliche Felder der
Szenen mit acht Einträgen vor beziehungsweise zehn nach dem Kontakt. Ein
zusätzlicher einzelner großer Schritt muss den Stoß und die restliche Bewegung
vollständig erfassen. Ungültige Massen/Restitution werden zurückgewiesen.
Die Instanzprobe führt verschiedene Massen und Restitutionen gleichzeitig aus,
prüft Einheiten, Reset und unveränderte Messwerte bei abgewiesenen Schritten.

Die beiden Analysesprachen lesen C- und Physim-Läufe. Vier Plots zeigen
Positionen, Geschwindigkeiten, Gesamtimpuls und K/D/K+D; zwei Tabellen liefern
Erhaltungskontrollen und tatsächliche Ereignisse. Ausbleibende Kontakte erzeugen
keine erfundene Ereigniszeile. Ein separater C-Prüfer vergleicht acht vollständige
Berichtskurven, SI-Plotachsen und alle Tabellenwerte mit den Originaldaten.
Der unabhängige Parser kontrolliert jedes Feld des vollständigen CSV-Exports.
Alle 44 Kombinationen aus elf Modellen und beiden Experiment-/Analysesprachen
bestehen. Leere oder mehrfache Auswahl wird kontrolliert abgewiesen; diese
Beispielanalyse verarbeitet ausdrücklich einen Lauf. Vier dokumentierte
Codeblöcke entsprechen exakt den gebauten Quellen.
[Lernziel, Modell, Gleichungen und Ablauf](collision-tutorial.md).

macOS Release besteht fünf ausgewählte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-4tqyn295`, Linux GCC
Release dieselben fünf unter
`build/contact-world-language-release-linux/test-results/run-i_fspv_j`:
neuer Lernpfad, Quellcode- und Referenzdokumentation sowie bisherige Kugelstoß-
Referenz und C-/Physim-Kugelparität. Linux Debug mit AddressSanitizer und
UndefinedBehaviorSanitizer besteht den gesamten neuen Lernpfad einschließlich
beider Module, Analysen und Instanzprobe unter
`build/spring-tutorial-asan-linux/test-results/run-bwnqmvya`.

macOS besteht die drei ausgewählten Fensterabläufe unter
`build/contact-world-language-release-mac/test-results/run-s5t1jz8z`, Linux unter
`build/contact-world-language-release-linux/test-results/run-a8u4tf5i`.
Die neue Prüfung übernimmt die getesteten Quellen in beide App-Projektsprachen,
baut, wählt mA=2 kg/e=0, führt 100 angehaltene Schritte zu 0,02 s aus und öffnet
101 gespeicherte Messzeilen mit Szene und Analyse erneut. Die gemeinsame
Nachstoßgeschwindigkeit beträgt 0,2 m/s; K=0,06 J, D=0,48 J und K+D=0,54 J.
Impuls 0,6 kg m/s, Stoßzeit 4/3 s und Impuls auf A −0,8 kg m/s werden geprüft.
Zusätzlich bestehen der bestehende Physim-Kugelablauf und die komplette
Offline-Dokumentnavigation einschließlich des neuen Lernpfads. macOS lief im
nativen Desktop, Linux unter X11/Xvfb/Openbox/Mesa. Compiler-, SDL- und OS-
Versionen entsprechen den folgenden Intel-/Debian-12-Nachweisen. Screenshots
von Szene und Energieabrechnung wurden visuell geprüft. Auf kleiner Fläche
bleiben untere Diagrammteile über Scrollen erreichbar; die Darstellung verbindet
Messpunkte und berechnet keinen endlichen Stoßkraftverlauf.

Beide neu installierten SDKs wurden an einen anderen absoluten Pfad mit
Leerzeichen/Umlauten kopiert und vollständig verifiziert: macOS unter
`build/collision-tutorial-sdk-proof-mac/Native SDK ä ol9_d8s1`, Linux unter
`build/collision-tutorial-sdk-proof-linux/Native SDK ä 4sht9ghs`. Der Gesamtprüfer
kontrolliert Manifest und öffentliche Header, baut Core und alle Sprachmodule
aus der mitgelieferten Quelldistribution neu und führt die bisherigen Archiv-,
Kontakt-, Diagnose-, Serien- und Beispielprüfungen aus. Der neue Stoß-Lernpfad
läuft jeweils mit installiertem und neu gebautem Core sowie ein drittes Mal
über genau den dokumentierten nativen C-Projektbuild. Elf analytische Szenarien
und alle 44 Experiment-/Analysepfade werden bei jedem Durchgang geprüft.
Die endgültigen SDKs übernehmen die aktuellen Nachweise und erneuern deren
Manifestwerte; Code-, Beispiel- und App-Binärdateien bleiben bytegleich zur
verifizierten Installation und zur geprüften Release-App.

Der Katalog umfasst 559 Prüfungen ohne Fenster (544 ohne SDL), 64 Fensterfälle
und 18 eigenständige Sprachprogramme plus 40 Experiment-/Analysemodule. Beide
Release-Builds mit `--examples` erstellen sämtliche Programme und Module.
Ein Gesamtlauf aller aktuellen 559/64 Fälle wird für diese Sitzung nicht
behauptet; der frühere Lauf aller 555 Fälle ist weiter unten belegt.
Dieser Lernpfad erfüllt keine anderen offenen Anforderungen des Projektplans
und wird nicht als vollständige Produktabnahme ausgegeben.

## Pendel-Lernpfad in beiden Sprachen am 7. Oktober 2026

Vier neue vollständige Quellen liefern dasselbe konservative nichtlineare
Pendel und eine gemeinsame Mehrlaufanalyse in C und Physim. Die typisierten
Parameter `length`, `initialAngle` und `integrator` gehören zur Modellinstanz.
Fünf Verfahren stehen zur Wahl: explizites Euler, symplektisches Euler, RK4,
Velocity Verlet und Dormand–Prince 5(4). Tatsächlich adaptive Aufzeichnung
verlangt ausdrücklich Verfahren 4. Modell, SI-Kanäle, Grenzen und Auswahl werden
im Laufmanifest gespeichert. Core-API/ABI 3, Sprachvertrag 0.177.0 und Dateiformate
bleiben unverändert.

Der unabhängige Prüfer kontrolliert 40.010 primäre Messzeilen der fünf Verfahren
in beiden Sprachen samt allen sechs Kanalnamen und SI-Dimensionen sowie sämtliche
Felder der Szenen mit sechs geometrischen Einträgen und zwei Gruppen. Die
nichtlineare Periode wird durch Simpsonquadratur des elliptischen Integrals
berechnet. Eine rekursive Taylorentwicklung der Bewegung bei t=0,5 s prüft die
Ordnungen 1, 1, 4 und 2 beim Halbieren der Schrittweite; sie verwendet keine
RK-Stufen aus dem Produkt. Veränderte Längen, positive/negative Auslenkung,
Ruhezustand und zwei adaptive Läufe werden ebenfalls geprüft. Der
Instanzprüfer kontrolliert unabhängige Parameter/Verfahren, Einheiten, Reset,
fehlgeschlagene Schritte und dass eine fehlerhafte Sprachinstanz andere Instanzen
nicht beeinflusst. Beim Sprachadapter beendet ein Laufzeitfehler die betroffene
Instanz bis Reset; der Host beendet diesen Lauf. Messwerte werden vorher geprüft
und erst bei erfolgreichem Abschluss veröffentlicht.

Die Analysen verbinden einen bis acht gespeicherte Läufe auf deren eigenen
Zeitachsen. Sie erzeugen zwei Plots mit Winkel und Energieabweichung, zwei
Tabellen mit Laufkennzahlen und tatsächlich gemessenen Perioden sowie
vollständiges CSV pro Eingabe. Perioden beruhen auf linear interpolierten
positiven Nulldurchgängen; kurze oder ruhende Läufe erhalten keine erfundenen
Periodenwerte. Ein separater C-Prüfer vergleicht acht gemischte Mehrlaufberichte
und zwei Einzellaufberichte mit den Originaldaten, einschließlich Plotmetadaten,
Quellenzahl, vollständigen Kurven unterhalb der Vorschaugrenze und aller
Tabellenwerte. Der Pythonprüfer prüft sämtliche CSV-Zeilen, auch bei reduziertem
Berichtsplot. Die vier dokumentierten Codeblöcke entsprechen exakt den gebauten
Quellen. [Lernziel, Modell und vollständiger Ablauf](pendulum-tutorial.md).

macOS Release besteht sechs ausgewählte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-f_cdfqro`, Linux GCC
Release dieselben sechs unter
`build/contact-world-language-release-linux/test-results/run-5jvxw5yz`:
neuer Lernpfad, Quellcode-/Referenzdokumentation sowie die bisherigen drei
Pendelreferenzen für RK4, RK45 und Verlet. Linux Debug mit AddressSanitizer und
UndefinedBehaviorSanitizer besteht den gesamten neuen Lernpfad einschließlich
beider Module und Instanzprobe unter
`build/spring-tutorial-asan-linux/test-results/run-0h7jbom1`.

Die zusätzlichen Grenzprüfungen mit genau acht ausgewählten Läufen und einer
abgewiesenen leeren Auswahl bestehen auf macOS zusammen mit Quellcode- und
Referenzprüfung 3/3 unter
`build/contact-world-language-release-mac/test-results/run-wp5pe_w2` und unter
Linux ASan+UBSan im erweiterten Lernpfad unter
`build/spring-tutorial-asan-linux/test-results/run-w8c9qcv3`.

macOS besteht die drei ausgewählten Fensterabläufe unter
`build/contact-world-language-release-mac/test-results/run-hp6wgi1q`, Linux unter
`build/contact-world-language-release-linux/test-results/run-vx1_cjf7`.
Die neue Prüfung erzeugt beide App-Projektsprachen aus Vorlagen, übernimmt die
getesteten Tutorialquellen, baut, wählt Verlet, führt 40 angehaltene Einzelschritte
aus und öffnet die 41 gespeicherten Messzeilen mit Szene und Bericht erneut.
Zusätzlich bestehen der bestehende adaptive Fensterablauf und die gesamte
Offline-Dokumentnavigation einschließlich des neuen Lernpfads. Die macOS-Fenster
wurden nativ ausgeführt; Linux nutzte X11, Xvfb, Openbox und Mesa. Die Plattform-
und Compiler-Versionen entsprechen den folgenden Intel-/Debian-12-Nachweisen.
Screenshots von Pendel, Beschriftungen und Energieanalyse wurden visuell geprüft.
Der kleine Bildschirm erfordert weiterhin Scrollen zu den unteren Diagrammteilen.

Beide SDKs wurden neu installiert, in Ordner mit Leerzeichen/Umlauten kopiert
und unter einem anderen absoluten Pfad vollständig geprüft. macOS-Nachweis:
`build/pendulum-tutorial-sdk-final-mac/Native SDK ä it4e0vz5`; Linux-Nachweis:
`build/pendulum-tutorial-sdk-final-linux/Native SDK ä 5lzt4zzx`. Der Gesamtprüfer
kontrolliert alle Manifestdateien und eigenständigen öffentlichen Header, baut
Core und sämtliche Sprachmodule aus den mitgelieferten Quellen neu und prüft
die vorhandenen Archiv-, Kontakt-, Diagnose-, Serien- und Beispielproben.
Für den neuen Lernpfad laufen fünf Integratoren und acht gemischte
Mehrlaufanalysen jeweils gegen installiertes und neu gebautes Core. Außerdem
führt der Prüfer genau den dokumentierten nativen C-Projektbuild mit
`physim-build` aus und prüft dessen Experiment-/Analysemodul erneut mit denselben
unabhängigen Oracles. Die App wurde aus derselben Release-Binärdatei geprüft;
Code-/Beispieldateien der endgültigen Pakete werden bytegleich zur verifizierten
Installation erhalten. Nur die aktuellen Dokumentationsnachweise und ihre
SHA-256-Manifestwerte werden danach aufgefrischt.

Der Katalog umfasst jetzt 557 Prüfungen ohne Fenster (542 ohne SDL), 63
Fensterfälle und 18 eigenständige Sprachprogramme plus 38 Experiment-/
Analysemodule. Beide Release-Builds mit `--examples` erstellen diese Programme
und Module. Diese Sitzung behauptet keinen Gesamtlauf aller 557/63 Fälle;
der vorherige Gesamtlauf aller 555 Fälle ist im folgenden Abschnitt belegt.
Der Projektplan bleibt offen; dieser Lernpfad schließt keine anderen
Roadmap-Anforderungen automatisch ab.

## Korrekturen des Prüfberichts am 6. Oktober 2026

Die Befunde CR-001 bis CR-007 aus dem externen Prüfbericht wurden am lokalen
Stand nachvollzogen und korrigiert. Die archivierte alte Bibliothek scheitert
an der neuen Sekantenregression. Der konkrete Materialfall mit Dichte 10000,
Fluiddichte `1e-20`, Radius 1 und Viskosität `1e-8` ergibt bei t=0,001 s in beiden
korrigierten Sprachen `reference.y=0.499995096675` und
`reference.velocity=-0.009806649999999978`. Die alte Physim-Quelle ergibt dort
`25141055.324947417` m; die alte C-Quelle `0.49999480194206297` m.

| Befund | Korrektur und Nachweis |
| --- | --- |
| CR-001 | Editor und Autosave erzeugen eindeutige Geschwisterdateien exklusiv. Vorhandene `.tmp`-Dateien, Verzeichnisse, Symlinks und Hardlinks bleiben erhalten. Fehlgeschlagene Veröffentlichung entfernt nur eigene Zwischenfiles. Der App-Selbsttest ruft den tatsächlichen Hauptquellen-Speicherpfad auf und prüft das fremde Symlinkziel. |
| CR-002 | Auf POSIX starten Zwischenfiles mit `0600`; Quelle und Backup übernehmen gewöhnliche rwx-Bits. Tests erhalten `0600`, `0640`, `0700`, `0755`, einschließlich Ausführbarkeit. ACLs, xattrs und besondere Modusbits werden nicht übernommen; Windows-Rechte wurden nicht geprüft. |
| CR-003 | Zwei Taylorfaktoren bis Ordnung acht vermeiden Auslöschung für `λt≤0,1`. 18 vollständige C-/Physim-Läufe mit logarithmischen Viskositäten, Nullfall und Übergangsbereich werden gegen ein unabhängiges Decimal-Oracle mit 80 Stellen geprüft. |
| CR-004 | Konstante Kurven liegen in der Mitte der Zeichenfläche; skalierte Brüche vermeiden überlaufende Achsenspannen. Sieben Konstanten einschließlich `±DBL_MAX` exportieren endliche SVG-Punkte. |
| CR-005 | Energie startet beim ersten gültigen Messwert; nur gültige Messungen tragen zur Drift bei. Lücken unterbrechen Periodenintervalle. Tests decken Status 0/2 am Anfang, in der Mitte und am Ende sowie Nulldurchgänge ab. Fehlende Kennzahlen sind im Manifest `nan`. |
| CR-006 | Nichtendliche Statistikakkumulatoren führen vor dem Export zu `PS_NUMERIC`. Die bestehende `ps_statistics`-ABI bleibt unverändert; ihr unskaliertes `m2` begrenzt den Wertebereich. `±1` und `±1e150` stimmen mit der unabhängigen Streuung überein, `±1e200` wird kontrolliert abgewiesen. |
| CR-007 | Skalierte Differenzen und Produkte liefern Sekante 1 für `x=y={-1e308,1e308}` und Trapezintegral `1e108`. Nichtdarstellbare Ableitungen lassen die ganze Ausgabe unverändert; Eingabe-/Ausgabealiase werden unterstützt. |

Beide Release-Gesamtläufe bestehen den vollständigen aktuellen Katalog ohne
Fenster 555/555: macOS unter
`build/contact-world-language-release-mac/test-results/run-y6el5jvy`, Linux unter
`build/contact-world-language-release-linux/test-results/run-7e671h5x`.
Die C-/Physim-Module, Prozessabläufe, nativen Projektbuilds, Messarchive und
vorhandenen Regressionen sind damit vollständig im aktuellen Katalog geprüft.
Die 62 Fensterfälle wurden hier nur im beschriebenen Ausschnitt ausgeführt.

macOS Release besteht sieben gezielte Prüfungen unter
`build/contact-world-language-release-mac/test-results/run-wramilpq`:
Core, neue Analysegrenzen, Autosave, Textdokumente, Projektdateien,
Materialtutorial und dessen dokumentierte Quellen. Die App besteht die beiden
Abläufe Autosave und Physim-Feder einschließlich der neuen Prüfung des echten
Editor-Speicherpfads unter
`build/contact-world-language-release-mac/test-results/run-fgh4df4a`.

Zusätzlich bestehen auf macOS Dokumentwiederherstellung und Dokumentneubau
unter `build/review-fixes-gui-mac/documents_recovery` beziehungsweise
`build/review-fixes-gui-mac/documents_build`, jeweils mit `app-steps.json`.
Unter Linux bestehen alle vier App-Abläufe (Autosave, Physim-Feder,
Dokumentwiederherstellung und Dokumentneubau) unter
`build/review-fixes-gui-linux/<Fall>`; der Sammellog ist
`build/review-fixes-gui-linux.log`. Die Linux-Fenster liefen tatsächlich unter
X11, Xvfb und Openbox mit Mesa; die macOS-Fenster im nativen Desktop.

Die abschließenden zusätzlichen Kollisionszusicherungen prüfen auch die
Identität beziehungsweise den Inhalt des vorbestehenden temporären Pfades.
Autosave, Textdokumente und Analysegrenzen bestehen erneut 3/3 auf macOS unter
`build/contact-world-language-release-mac/test-results/run-8vcz30cv` und Linux
unter `build/contact-world-language-release-linux/test-results/run-3wpn5bu8`.
Der tatsächliche Editorpfad mit erhaltener Symlinkidentität besteht erneut im
Physim-Federablauf unter `build/review-final-gui-mac` und
`build/review-final-gui-linux`, jeweils mit `app-steps.json`. Eine getrennte
macOS-Probe gegen den aktuellen Core prüft die vollständige Ausgabeerhaltung
auch nach bereits berechneten gültigen Ableitungen unter
`build/review-final-analysis-mac`.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
Core, Analysegrenzen, Messmasken und Materialtutorial 4/4 unter
`build/spring-tutorial-asan-linux/test-results/run-n_2va9lb`.
Die Compiler-, Betriebssystem- und SDL-Versionen entsprechen den folgenden
macOS-Intel- und Debian-12-Nachweisen. Core-API/ABI, Sprachvertrag und Dateiformate
bleiben unverändert. Diese Nachprüfung ist keine Fehlerfreiheitszusage für den
übrigen Code und keine vollständige Abnahme des Projektplans.

## Feder-Lernpfad in beiden Sprachen am 6. Oktober 2026

C- und Physim-Federvorlagen definieren die Dämpfung jetzt als typisierten
Experimentparameter `damping` in N s/m, Bereich 0..64. Wert und Metadaten
gehören zum Modellkontext; mehrere Instanzen teilen keinen veränderlichen
Parameter. Der C-Makrostandard bleibt für vorhandene Referenzvarianten erhalten.
Physim prüft positive Schrittweiten ausdrücklich. Die C-Punktmarkierung speichert
nun denselben zweiten Punktwert wie die Sprachbindung. Modell, RK4-Verfahren,
Kanäle und Standards sind erhalten; öffentliche Core-API, ABI 3, Sprachvertrag
0.177.0 und alle Dateiformate bleiben unverändert.

Der vollständige Lernpfad enthält beide Experiment- und Analysequellen sowie
Ablauf, Gleichungen, vier Dämpfungsfälle, Einheiten, Ergebnisse und Modellgrenzen.
Die Analysen erzeugen drei Plots mit fünf vollständigen Kurven, eine Tabelle
mit Zeilenzahl und maximaler absoluter Bilanzabweichung und vollständiges CSV.
Ein unabhängiger Pythonprüfer vergleicht 16.008 Messzeilen, alle elf Kanäle mit
SI-Dimensionen, komplette Szenen mit 13 Objekten und 65 Helixpunkten sowie
Parameterprovenienz. Er prüft die geschlossenen Lösungen für Dämpfung 0, 1,2, 8
und 12 und eine RK4-Verfeinerung zwischen Faktor 14 und 18. Sechzehn gemischte
Analysewege werden durch einen getrennten C-Prüfer kanalweise samt Einheiten und
exakter Tabellenstatistik mit den Rohdaten verglichen. Die Instanzprobe löst zwei
verschiedene Dämpfungen im selben geladenen Modul, prüft Reset und Fehlererhaltung.
Alle vier dokumentierten Codeblöcke entsprechen den tatsächlich gebauten Quellen.

macOS Release besteht neue Tutorial-, Quellcode- und Referenzprüfung sowie die
bisherigen Federreferenzen und den Sprachlebenszyklus 5/5 unter
`build/contact-world-language-release-mac/test-results/run-b1ltqy4u`.
Linux Release besteht dieselben fünf Prüfungen unter
`build/contact-world-language-release-linux/test-results/run-gl65_h83`.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
Federreferenz, Sprachlebenszyklus und vollständigen neuen Tutorialablauf 3/3 unter
`build/spring-tutorial-asan-linux/test-results/run-otgywszl`.
macOS Release besteht neues C-/Physim-Tutorial, bisherige Sprach-Federvorlage
und Handbuchfenster 3/3 unter
`build/contact-world-language-release-mac/test-results/run-_ow7csno`.
Die neuen Projekte verwenden tatsächlich Parameterwert 8, 40 kontrollierte
Schritte, 41 gespeicherte Messungen/Szenen und eine in der App wieder geöffnete
Energieanalyse. Die Physim-Energieansicht wurde auf macOS und Linux visuell geprüft.

Linux Release besteht dieselben drei Fensterprüfungen unter
`build/contact-world-language-release-linux/test-results/run-uz1gqxof` mit
X11/Mesa, Xvfb und Openbox. Die Paket-Apps sind bytegleich mit den zuvor
geprüften GUI-Apps. Beide ersten SDK-Manifeste enthalten 330 geprüfte Dateien,
darunter 134 Code-/Binärdateien.

Die vollständige verschobene SDK-Prüfung besteht auf macOS unter
`build/spring-tutorial-sdk-proof-mac/Native SDK ä uro52_vz`. Sie kompiliert alle
öffentlichen Header und 21 Core-Einheiten, prüft 18 eigenständige Sprachprogramme
und 36 Module und führt den neuen Federlernweg gegen ausgelieferte und neu
gebaute Bibliotheken aus. Zusätzlich bestehen die bisherigen Spezialproben,
Material-/Medium-Lernpfade, adaptive Studien und neun eigenständige Projektneubauten.
Diese SDK-Prüfung enthält keine zusätzliche GUI-Abnahme.

Linux besteht die vollständige SDK-Prüfung mit demselben Umfang unter
`build/spring-tutorial-sdk-proof-linux/Native SDK ä mjxnz2wi`.
Die finalen Pakete `build/Spring tutorial clean SDK ä mac` und
`build/Spring tutorial clean SDK ä linux` übernehmen die geprüften SDKs und
aktualisieren die Dokumentation. Ihre SHA-256-Manifeste wurden für alle 330
Dateien geprüft. Alle 134 Dateien unter `include`, `src`, `lib` und `bin` sowie
sämtliche Beispielquellen stimmen mit dem jeweiligen vollständig geprüften
verschobenen SDK überein. Die Apps entsprechen bytegenau den geprüften GUI-Apps.
Seit den erfolgreichen Rechen-, Fenster- und SDK-Prüfungen wurde kein
Produktionscode verändert.

Der Katalog umfasst 554 Prüfungen ohne Fenster (539 ohne SDL) und 62 Fensterfälle.
Der Beispiel-/SDK-Katalog umfasst 18 eigenständige Sprachprogramme und 36 Module.
Diese Änderung verwendet gezielte Regressionen und SDK-Prüfungen; ein neuer
Gesamtlauf aller 554 beziehungsweise 62 Fälle ist nicht behauptet. Die frühere
550/550-Abnahme bleibt für ihren benannten Stand erhalten. Dieser Lernpfad
schließt weiterhin weder steife/nichtlineare Federn noch allgemeine Führungs-
Constraints oder weitere offene Produktziele ab.

## Lernpfad eigenes Material und Medium am 6. Oktober 2026

Der achte geforderte Lernpfad liefert vollständige C-/Physim-Experimente und
Auswertungen. Ein benutzerdefiniertes Material bestimmt die Kugelmasse über
Dichte und Volumen; benutzerdefinierte Fluiddichte und Viskosität bestimmen
Auftrieb und Stokes-Widerstand. RK4 integriert Ort, Geschwindigkeit und
Dissipation unabhängig von den analytischen Kontrollkanälen. Eigene,
konservative Grenzen Re<=0,1 und lambda*dt<=0,25 werden durchgesetzt.
Quelle, Parameter, Kräfte, Integrator und ausgeschlossene hydrodynamische
Effekte sind dokumentiert und im Laufmanifest gespeichert. ABI, öffentliche
Core-API, Sprachvertrag 0.177.0 und alle Dateiformate bleiben unverändert.

Der unabhängige Pythonprüfer validiert CRC, alle 1001 Messzeilen und sämtliche
Szenenfelder für sieben Konfigurationen in beiden Sprachen: Standard, Aufstieg,
Dichtegleichheit, Viskosität, Radius, Vakuum und halbierter Zeitschritt.
Masse, Kräfte, analytische Lösung und Energiebilanz werden unabhängig gerechnet.
Die Halbierung verlangt mindestens Faktor 14 weniger Geschwindigkeitsfehler.
Alle vier Experiment-/Analysesprache-Kombinationen erzeugen drei Plots mit
sieben vollständigen Kurven und CSV; ein separater C-Prüfer vergleicht jeden
Kurvenwert und seine SI-Dimension mit den ursprünglichen Messungen.
Ungültige Reynolds-Zahl, Viskosität und Schrittweite werden in beiden Sprachen
abgewiesen. Die Codeblöcke des Handbuchs werden gegen alle vier Quellen geprüft.

macOS Release besteht zunächst den neuen Rechenablauf 1/1 unter
`build/material-tutorial-release-mac/test-results/run-afa49mvr`.
Die abschließende Paketprüfung besteht Materiallernweg, Quellcodeabgleich,
C-Dokumentmodell und generierte Funktionsreferenz 4/4 unter
`build/contact-world-language-release-mac/test-results/run-cd4q13l8`.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
den vollständigen neuen Referenzablauf 1/1 unter
`build/material-tutorial-asan-linux/test-results/run-yiurqtb1`.
macOS Release besteht Handbuchfenster und neuen C-/Physim-Fensterablauf 2/2
unter `build/material-tutorial-release-mac/test-results/run-gpwxchvj`.
Linux Release besteht dieselben Fensterprüfungen 2/2 mit X11/Mesa, Xvfb und
Openbox unter `build/material-tutorial-release-linux/test-results/run-het1psio`.
Beide Sprachprojekte werden tatsächlich angelegt, gebaut, 40 Schritte ausgeführt,
41 Messzeilen und Szenen wieder geöffnet sowie der gespeicherte Analysebericht
in der App geladen. Die Physim-Auswertung wurde auf macOS und Linux visuell geprüft.
Der erste Fensteraufbau markierte die ersetzte Analysequelle nicht als geändert
und baute deshalb die Vorlagenanalyse; die korrigierte Probe markiert beide
Editoren und überprüft das tatsächliche Tutorialergebnis.

Linux Release besteht die abschließende gezielte Paketprüfung 4/4 unter
`build/contact-world-language-release-linux/test-results/run-4y9miqrn`.
Die ersten SDK-Manifeste enthalten jeweils 327 Dateien und 133 Code-/Binärdateien.
Da Apps aus unterschiedlichen Buildordnern verschiedene Binärhashes hatten,
wurden die ausgelieferten Programme direkt geprüft: jeweils vollständiger
C-/Physim-Materialablauf unter `build/material-tutorial-installed-gui-mac`
beziehungsweise `build/material-tutorial-installed-gui-linux`. Beide bestehen.

Die erste erweiterte SDK-Prüfung bestand den neuen Materialablauf, scheiterte
aber danach an einem überschriebenen lokalen Prüferpfad: die Tutorialprobe
ersetzte den allgemeinen Analyseprüfer. Ein eigener Variablenname erhält
nun beide Prüfer; der SDK-Gesamtaufruf wurde vollständig wiederholt.
macOS besteht diese Wiederholung unter
`build/material-tutorial-sdk-final-proof-mac/Native SDK ä rj4l769l`.
Sie prüft alle Header, 21 Core-Einheiten, 18 Sprachprogramme, 35 Module,
installierte und neu gebaute C-/Physim-Materialversuche, alle vier Analysesprachen-
Kombinationen sowie die bisherigen Spezialproben, adaptiven Studien und neun
unabhängigen Projektneubauten. Die SDK-Prüfung selbst enthält keine GUI-Abnahme.

Linux besteht den vollständig wiederholten SDK-Aufruf unter
`build/material-tutorial-sdk-final-proof-linux/Native SDK ä s6j__27_` mit
identischem Umfang. Der abschließend verschärfte Messdateiprüfer verlangt
Namen und SI-Dimensionen aller 13 Kanäle; macOS besteht damit erneut 4/4 unter
`build/contact-world-language-release-mac/test-results/run-jvgg_51u`.

Linux besteht dieselben vier abschließenden Prüfungen unter
`build/contact-world-language-release-linux/test-results/run-epardf8l`.
Die finalen Pakete `build/Material tutorial clean SDK ä mac` und
`build/Material tutorial clean SDK ä linux` übernehmen die geprüften SDKs
und aktualisieren die Dokumentation. Ihre Manifeste enthalten jeweils 327
SHA-256-geprüfte Dateien. Alle 133 Dateien unter `include`, `src`, `lib` und
`bin` sowie alle Experiment-/Analysequellen stimmen bytegenau mit dem jeweiligen
vollständig geprüften verschobenen SDK überein. Die App entspricht der direkt
geprüften installierten App; der Produktionscode blieb unverändert.

Der Katalog wächst auf 552 Fälle ohne Fenster (537 ohne SDL) und 61 Fensterfälle.
Diese Änderung nutzt gezielte Prüfungen und die vollständigen SDK-Prüfungen;
ein neuer Gesamtaufruf aller 552 beziehungsweise 61 Fälle ist nicht behauptet.
Die vorausgehenden 550/550-Gesamtprüfungen bleiben für ihren benannten Stand
erhalten. Materialkennwerte sind Demonstrationsdaten, keine gemessene
Stoffdatenbank; der Lernweg übernimmt keine vollständige Flüssigkeitsdynamik.

## Physim-Kontaktzustände am 6. Oktober 2026

Sprachvertrag 0.177.0 bindet die öffentliche persistente Kontaktwelt und den
manuellen Warm-Kontaktgraphen an denselben C-Core. Collider sind kleine Werte;
Kontaktzustände besitzen unveränderliche Snapshots im gemeinsamen 64-MiB-Budget.
`solve()` und `reset()` erzeugen neue Werte und ändern weder Receiver noch
Eingabekörper. Arrays, optionale Werte, Strukturfelder, Rückgaben und Closures
behalten unabhängig gespeicherte Zustände und geben sie automatisch frei.
API/ABI 3 und alle Mess-/Snapshot-/Wireformate bleiben unverändert.

Die abschließende gezielte macOS-Debug-Prüfung besteht 19/19 unter
`build/contact-world-language-debug-mac/test-results/run-jnqomjeg`.
Sie enthält analytische Warm-/Kaltreste, Zeitschrittskalierung, Kontaktgeometrie,
ID-Grenzen, Kopien/Closures und 2000 Ersetzungszyklen mit tatsächlicher Kontrolle
des verbleibenden Sprachspeichers. Negative Compilerfälle prüfen reservierte
Namen und Arrayelementtypen. Runtimefehler prüfen Collidergrenzen, Formen,
Normalen, Weltsettings, doppelte IDs, dynamische Ebenen, Schrittweiten, Zugriffe
und Seedanzahlen, jeweils mit Freigabe bestehender Besitzer. Ein eigener
Allocator erzwingt alle Allokationsfehler von Konstruktor, Solve, Reset,
Körperarray und Warmgraph; Eingabesnapshots bleiben bytegleich und alle Besitzer
werden anschließend vollständig freigegeben.
Der erste gezielte Lauf scheiterte ausschließlich beim Testinstrumentieren von
POD-Programmen ohne Sprachspeicher. Die betroffenen Fehlerproben besitzen nun
zusätzlich einen echten Weltzustand und prüfen auch dessen Freigabe.

Ein unabhängiger CRC-/Wireparser vergleicht C und Physim in warmem und kaltem
Modus über je 1001 vollständige Messzeilen und sämtliche aufgezeichneten
Objektfelder mit 1e-12 relativer/absoluter Toleranz. Die C-Punktmarker verwenden
nun denselben zweiten Positionswert wie die Sprachbindung. Die Quaternion-
normalisierung der Sprach-Szenenkonstruktoren kann letzte Bits verändern;
Formen, Farben, IDs, Texte und Strukturfelder werden exakt verglichen.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
alle 19 gezielten Prüfungen unter
`build/contact-world-language-asan-linux/test-results/run-f1wai2ee`.
macOS Debug besteht die C- und Physim-Stapelfenster 2/2 unter
`build/contact-world-language-debug-mac/test-results/run-0zkbhpx3`.
Die App erstellt echte Projekte, baut sie, führt 40 kontrollierte Schritte aus
und öffnet 41 aufgezeichnete Messzeilen mit Szenen wieder. Die Physim-
Stapelansicht wurde visuell geprüft.

Die ersten Release-Gesamtläufe bestanden auf beiden Plattformen 549/550;
allein der CLI-Korpus traf zwei alte
Erwartungen an die Sprachversionsausgabe 0.176.0. Diese ersten Läufe liegen unter
`build/contact-world-language-release-mac/test-results/run-w5mvh34p` und in der VM
`build/contact-world-language-release-linux/test-results/run-cp09edzv`. Der Compiler meldete bereits
korrekt 0.177.0. Diese Erwartungen wurden aktualisiert; der Produktionscode
blieb unverändert. Erst die anschließenden Gesamtprüfungen belegen den
abschließenden Katalog mit 550 Fällen ohne Fenster und 60 Fensterfällen.

macOS Release besteht die abschließende Gesamtprüfung 550/550 unter
`build/contact-world-language-release-mac/test-results/run-z18rl376` mit
Apple Clang 16 auf Intel macOS 14.6.1 und SDL 3.2.30.
Linux Release besteht die abschließende Gesamtprüfung 550/550 unter
`build/contact-world-language-release-linux/test-results/run-4crv23h0`.
Der abschließende Katalog enthält 18 eigenständige Sprachprogramme und 33 Module.
Dieselbe Release-App besteht C-/Physim-Stapel und den bisherigen Kontaktablauf
3/3 mit X11/Mesa, Xvfb und Openbox unter
`build/contact-world-language-release-linux/test-results/run-barcukyy`.
Die Physim-Stapelansicht wurde auch auf Linux visuell geprüft; dies ersetzt
keine Gesamtprüfung sämtlicher 60 Fensterfälle.
macOS Release besteht dieselben drei Fensterabläufe 3/3 unter
`build/contact-world-language-release-mac/test-results/run-511vesok`.
Die SHA-256-Manifeste der ersten Pakete enthalten auf beiden Plattformen
320 geprüfte Dateien, darunter 131 Code-/Binärdateien.

Die vollständige verschobene SDK-Prüfung besteht auf macOS unter
`build/contact-world-language-sdk-proof-mac/Native SDK ä paqeof_i` und auf
Debian unter `build/contact-world-language-sdk-proof-linux/Native SDK ä _63wzqhx`.
Beide prüfen unabhängig alle Header, 21 Core-Quellen, 18 Sprachprogramme,
33 Sprachmodule, installierte und neu gebaute Kontaktwelten mit Allokations-
fehlern sowie die vollständigen warmen/kalten C-/Physim-Stapelvergleiche.
Bestehende Logging-/Frame-/Diagnostik-/Indexproben, gemeinsame Analysen,
adaptive Studien und neun eigenständige native Projektneubauten bestehen.
Diese SDK-Prüfungen enthalten keine zusätzliche GUI-Abnahme.

Die abschließenden Pakete `build/Contact Language clean SDK ä mac` und
`build/Contact Language clean SDK ä linux` enthalten die finalen Nachweise.
Ihre SHA-256-Manifeste wurden für alle 320 Dateien geprüft; alle 131 Dateien
unter `include`, `src`, `lib` und `bin` stimmen mit dem jeweils vollständig
geprüften verschobenen SDK überein. Die Apps entsprechen den geprüften
Release-Programmen. Der Produktionscode blieb seit den Gesamtprüfungen
unverändert; die finalen Pakete aktualisieren nur Dokumentation.

Kontaktverwaltung und Warmstart bleiben diskret; Feature-IDs, Kontaktinseln,
Gelenk-Warmstart und nichtlineare Rotationsprojektion sind weiter offen.

## Persistente Kontaktverwaltung am 6. Oktober 2026

Der neue caller-eigene C-Kontaktzustand regeneriert diskrete Kugel-/Box-/
Ebenenkontakte durch die bestehenden Broad-/Narrow-Phase-Funktionen. Stabile
Collider-IDs, beide Körperlokalanker und ein Normalenvergleich ordnen Kontakte
eins zu eins zwischen erfolgreichen Aufrufen zu. Masse-/Trägheits-/Formänderungen
verwerfen die Zuordnung; fehlende Kontakte laufen sofort aus. Startimpulse werden
mit dem Zeitschrittverhältnis skaliert und auf den aktuellen Coulomb-Kegel
projiziert. Restitutionsziele werden vor sämtlichen Warmimpulsen vorbereitet.
Körper, Cache und Ergebnis ändern sich bei Fehlern nicht. Es gibt keine eigenen
Heapallokationen oder implizite Integration. Alte Solver bleiben kalt; ABI 3,
Physim 0.176.0 und alle Daten-/Wireformate bleiben erhalten.

macOS Debug besteht die abschließenden Core-, Kontaktgraph-, gemischten Graph-
und Runnerprüfungen 4/4 unter
`build/contact-world-debug-mac/test-results/run-i_b62wvv`. Ein analytischer
Vier-Körper-Stapel erreicht mit gespeicherten Impulsen bereits nach einer
Geschwindigkeitsiteration einen Rest unter 1e-12, während der kalte Lauf einen
Rest über .04 behält. Timestep-Skalierung, Umordnung von Körper-/Colliderarrays,
Kontaktablauf, geänderte Masse/Trägheit, rotierte Ebenen, alle unterstützten
Formpaare, aktuelle Reibung, Restitution vor Warmstart sowie ungültige Seeds,
Kapazitäts- und numerische Überläufe werden ausdrücklich geprüft.

Der unabhängige Runnerprüfer liest jeweils 1001 Messzeilen eines warmen und
kalten Boxenstapels und CRC-validierte aufgezeichnete Szenen. Kontaktzähler,
Höhen, SI-Metadaten, Abschlussindex und geringerer mittlerer Normalenrest des
warmen Laufs werden geprüft. Die Debug-App-Prüfung besteht 1/1 unter
`build/contact-world-debug-mac/test-results/run-99qjwffa`: neues Projekt bauen,
40 kontrollierte Schritte, aktive Cachezähler und vollständige Wiederöffnung
der 41 Messzeilen/Szenen. Der Screenshot der Körper und Kontaktnormalen wurde
visuell geprüft; die Körperfarben wurden anschließend zur Unterscheidung variiert.

Für weitere Prüfungen wurde die eigene Debian-Testplatte von 16 auf 32 GiB
erweitert. Das vorherige Disk-Image wurde unter
`build/linux-vm/debian-dialogs-before-contact-world.qcow2` erhalten; Debian
erweiterte die bestehende Ext4-Rootpartition automatisch. Betriebssystem und
Compiler bleiben Debian 12/GCC 12.2.0 mit Kernel 6.1.0-53-cloud-amd64.
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
6/6 unter `build/contact-world-asan-linux/test-results/run-yni30whx`: neuer
Kontaktzustand und Runner, bisherige Kontakt-/Gelenkgraphen, Boxkontakte und
Broad Phase. Die tatsächlichen warmen/kalten Simulationen laufen ebenfalls
mit instrumentiertem Runner und Modul.
Linux Release besteht die vollständige Prüfung 533/533 unter
`build/contact-world-release-linux/test-results/run-9b3v74yv` in der VM.
Der Lauf baut alle 17 eigenständigen Sprachprogramme und 32 Module sowie
den nativen Projektbuilder mit nun 21 Core-Einheiten.
macOS Release besteht ebenfalls 533/533 unter
`build/contact-world-release-mac/test-results/run-n2idnt61` mit Apple Clang 16,
Intel macOS 14.6.1 und SDL 3.2.30. Beide Gesamtprüfungen verwenden den
abschließenden unveränderten Produktionscode.
Debian Release mit X11/Mesa, Xvfb und Openbox besteht neue Stapelansicht,
bisherigen C-/Physim-Kontaktablauf und Boxstoß 3/3 unter
`build/contact-world-release-linux/test-results/run-cywzwx6t`. Dies ist keine
vollständige Abnahme aller 59 Fensterfälle.
macOS Release besteht dieselben drei Fensterprüfungen 3/3 unter
`build/contact-world-release-mac/test-results/run-ov7_9k70`.
Die abschließende Linux-Stapelansicht mit getrennten Körperfarben wurde
visuell geprüft; gespeicherte Kontakte und Normale bleiben sichtbar.

Die vollständige verschobene SDK-Prüfung besteht auf macOS unter
`build/contact-world-sdk-proof-mac/Native SDK ä bdjem_8b` und auf Debian unter
`build/contact-world-sdk-proof-linux/Native SDK ä gi_i7c87` in der VM.
Alle öffentlichen Header werden unabhängig kompiliert; die ausgelieferte
Bibliothek und ein Neubau aus 21 Core-Einheiten bestehen die analytische
Kontaktweltprüfung und tatsächliche warme/kalte Stapelläufe mit aufgezeichneten
Szenen. Außerdem bestehen 17 eigenständige Sprachprogramme, 32 Module,
Logging-/Frame-/Diagnostik-/Indexproben, C-/Physim-Analysen, adaptive Studien
und neun unabhängige Projektneubauten. Diese SDK-Prüfungen führen keine
zusätzliche GUI-Prüfung aus.

Die abschließenden Pakete `build/Contact World clean SDK ä mac` und
`build/Contact World clean SDK ä linux` enthalten die aktualisierten Nachweise.
Ihre SHA-256-Manifeste wurden jeweils für alle 315 Dateien geprüft. Alle 128
Dateien unter `include`, `lib`, `src` und `bin` stimmen bytegenau mit dem
jeweils vollständig geprüften verschobenen SDK überein; die App entspricht
der zuvor geprüften Release-App. Die aktualisierte Dokumentation erfordert
keinen erneuten Programmneubau.

Die neue Verwaltung ist diskret, synchron und begrenzt auf 128 Körper/Collider
und 512 Kontakte. Kein CCD, Compound-/konvexe Formen, Feature-IDs/Kontaktinseln,
Gelenk-Warmstart, nichtlineare Rotationsprojektion oder direkte neue Physim-
Bindungen sind damit abgenommen. Der feste Iterationsetat garantiert keine
Konvergenz; Normalen- und Projektionsreste bleiben ausdrücklich sichtbar.

## Physim-Indexabfragen am 6. Oktober 2026

Sprachvertrag 0.176.0 ergänzt `RunIndex`, `RunBlock` und vollständige
`RunSnapshot`-Werte. Indexkopien teilen einen automatisch freigegebenen
Dateibesitzer; `close()` gibt nur die jeweilige Referenz frei. Arrays,
optionale Werte, Strukturfelder, Rückgaben und Closures behalten ihre
Besitzregeln. Blockdaten und Snapshotdaten bleiben nach dem Schließen des
Index verwendbar. Abfragen, Metadaten und SI-Dimensionen verwenden den
geprüften C-Core. `inputPath(index)` erschließt die explizite Analyseauswahl
als eigenen String. API/ABI 3 und alle Datei-/Wireformate bleiben erhalten.

Die Debug-Prüfung auf macOS besteht neue Werte, alte/recovered Dateien,
Arraykopien mit unabhängigem Schließen, optionale Werte und Strukturfelder,
einen aus der Funktion zurückgegebenen Closure sowie 2000 Aufräumschleifen.
Eine direkte C-Prüfung erzwingt jede Allokationsfehlerstelle beim Öffnen und
eine fehlgeschlagene Blockallokation und verlangt null verbleibende Besitzer.
Ein echtes Physim-Analysemodul liest die letzten 256 Zeilen eines
Million-Zeilen-Laufs; ein unabhängiger C-Prüfer vergleicht alle Zeiten/Werte
und die SI-Dimensionen des Berichts. Sechs Compilerchecks prüfen reservierte
Typnamen, Parametertypen, unveränderbare Receiver und den Analyse-Hostvertrag.
Die abschließenden beiden Debugfälle bestehen 2/2 unter
`build/run-index-language-debug-mac/test-results/run-jooqqt1l`.
Ein früher Analyseentwurf verwendete einen nicht vorhandenen Series-Konstruktor
und danach zwei voneinander unabhängige Reihen. Das Beispiel verwendet nun
`Series.fromValues` und ausdrücklich `alignedValues`, bevor es die Kurve erzeugt.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
5/5 unter `build/run-index-language-asan-linux/test-results/run-u71pu0nh`:
neue Indexwerte samt Analyse und Speicherprobes, sechs Compilerchecks,
abgefangene Modulfehler und beide bisherigen Speicherleck-Abläufe.
Die vollständigen Release-Läufe bestehen jeweils 531/531: macOS unter
`build/run-index-language-release-mac/test-results/run-a40iew2d`, Debian unter
`build/run-index-language-release-linux/test-results/run-frpk9b9o` in der VM.
Beide bauen zusätzlich 17 eigenständige Sprachprogramme und 32 Module.
Die Umgebungen sind Intel macOS 14.6.1/Apple Clang 16 und Debian 12/GCC 12.2.0
mit Kernel 6.1.0-53-cloud-amd64, jeweils SDL 3.2.30.
Debian Release mit X11/Mesa, Xvfb und Openbox besteht vollständigen Sprachworkflow
und eigenständige Analyseprojekte 2/2 unter
`build/run-index-language-release-linux/test-results/run-069l_kgq`. Dies ist keine
erneute vollständige Prüfung aller 58 Fensterabläufe.
macOS Release besteht dieselben beiden App-Prüfungen 2/2 unter
`build/run-index-language-release-mac/test-results/run-0oncir9q`.
Das verschobene Linux-SDK besteht die vollständige unabhängige Prüfung unter
`build/run-index-language-sdk-proof-linux/Native SDK ä q6o953s4` in der VM.
Sie kompiliert alle ausgelieferten Header, baut die 20 Core-Quellen neu, führt
17 Sprachprogramme und 32 Module aus bzw. baut sie neu und prüft neun native
Projektbuilds. Die neuen Sprachwerte laufen mit den erzeugten Million-Zeilen-
Fixtures; Besitzer-/Fehlerprobes werden gegen beide Archive gelinkt. Das neue
Analysemodul wird tatsächlich ausgeführt; ein aus dem SDK gelinkter C-Prüfer
vergleicht alle 256 Kurvenwerte und SI-Einheiten.
Das macOS-SDK besteht dieselbe vollständige Prüfung unter
`build/run-index-language-sdk-proof-mac/Native SDK ä wj2v1yh2`, einschließlich
beider Besitzer-/Fehlerprobes und des tatsächlich ausgeführten Analysemoduls.
Die SDK-Manifeste enthalten jeweils 310 geprüfte SHA-256-Dateieinträge.
Abschließende Pakete liegen unter `build/Run Index Language clean SDK ä mac`
bzw. `build/Run Index Language clean SDK ä linux` in der VM; ihre Manifestdateien
werden erneut gehasht. Ausgelieferte Header, Archive und Core-Quellen stimmen
mit den vollständig geprüften SDKs überein, die App mit dem abschließend
geprüften Fensterprogramm. Nach dem Paketneubau unterscheiden sich unter Linux
vier Programmdateien vom ersten SDK-Proof. Das abschließend gebaute Linux-SDK
besteht deshalb die komplette Prüfung erneut unter
`build/run-index-language-clean-sdk-proof-linux/Native SDK ä kxonlhz6`.
Endgültige Pakete mit diesen Nachweistexte liegen unter
`build/Run Index Language final SDK ä mac` bzw.
`build/Run Index Language final SDK ä linux` in der VM. Ihre 310 Manifestdateien
werden erneut geprüft; sämtliche 126 Code-/Programmdateien stimmen bytegenau
mit dem jeweiligen abschließend geprüften SDK überein.

## Rekonstruierbare Laufdatei-Indizes am 6. Oktober 2026

Format 1 erhält optionale Abschlusschunks 6/7 vor dem bisherigen Footer.
Mess- und Szenenpayloads, Reader-/Writerstrukturen, API/ABI 3, Snapshot 3,
IPC 5 und Physim 0.175.0 bleiben erhalten. Writerabschluss validiert einmal
den aufgezeichneten Präfix und erzeugt begrenzte Indexseiten ohne Allokation.
Die neue besitzende C-API validiert beim Öffnen den lesbaren Präfix und
rekonstruiert Checkpoints mit einem expliziten Allocator. Gezielte Messblöcke
und Szenen werden beim Abfragen erneut geprüft; alle Fehler erhalten Ausgaben.
Ein gespeicherter Index gilt erst nach vollständigem Vergleich mit den
tatsächlich gelesenen Daten als bestätigt. Quelllaufdateien bleiben unverändert.
Der native Projektbuilder und ausgelieferte Quellen umfassen nun 20 Core-Einheiten.

Die gezielten macOS-Debug-Prüfungen von Snapshots, Runnern, Frame-Aufzeichnung,
Logging und dem neuen Index bestehen 7/7 unter
`build/run-index-debug-mac/test-results/run-5wfp3ltu`. Nach Ergänzung der
Checkpointprüfung und absichtlicher Beschädigung nach dem Öffnen bestehen
Index und alte Szenenversionen 2/2 unter `run-6fge4idn`.
Die unabhängige Pythonprüfung liest eine Million Messzeilen, 3907 Checkpoints,
16 Indexseiten und den unveränderten Footer mit korrekten CRCs und Bytepositionen.
Sie kontrolliert außerdem Indexfreiheit, Recovery, semantisch falsche
Checkpoints trotz korrekter CRC und eine unbekannte Indexversion.
Die C-Prüfung erzwingt jede Allokationsfehlerstelle und ein Budget von 600000
Bytes beim Million-Zeilen-Lauf; sämtliche Besitzer werden korrekt freigegeben.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
5/5 unter `build/run-index-asan-linux/test-results/run-rea3kiur` in der Debian-VM:
Core, Snapshots, alte Szenenversionen, C-/Physim-Frame-Runner und Index einschließlich
Million-Zeilen- und unabhängiger Codec-Prüfung.
Ein unveränderter Analyse-Runner und das Analysemodul aus dem vorherigen macOS-SDK
`build/Diagnostics clean SDK ä mac` verarbeiten die neue Million-Zeilen-Datei
erfolgreich (`build/run-index-legacy-million-proof.*`). Ein erster Versuch mit
der absichtlich nichtmonotonen Prüffixture bestätigt zwar 1000 lesbare Zeilen,
wird aber erwartbar von deren strenger Analyse-Zeitprüfung abgewiesen.

Die ersten Release-Gesamtläufe enden auf beiden Plattformen 528/529:
`build/run-index-release-mac/test-results/run-4ms85qtl` und in der VM
`build/run-index-release-linux/test-results/run-_jy4js73`. Der bestehende
Mutationsprüfer erwartete noch sechs Chunks; Indexseite und Indexkopf ergeben
nun acht. Nach Korrektur und Erweiterung um die Index-API bestehen dessen
8660 Varianten unter macOS Debug (`run-f1vlwacq`) und zusätzlich unter Linux
ASan/UBSan (`build/run-index-asan-linux/test-results/run-o5qx3yw0`).
Der anschließende Linux-Release-Gesamtlauf besteht 529/529 unter
`build/run-index-release-linux/test-results/run-a0u2wsxq`. Er baut auch alle
16 eigenständigen Sprachprogramme und 31 Experiment-/Analysemodule.
Linux Release mit X11/Mesa, Xvfb und Openbox besteht vollständigen Physim-Ablauf
und C-/Physim-Zeitleiste 2/2 unter
`build/run-index-release-linux/test-results/run-3bqae85o`. Die Abläufe öffnen
aufgezeichnete Szenen erneut und kontrollieren Zeiten, Werte, Auswahl und
unveränderte Rohdateien. Es ist keine vollständige Abnahme aller 58 Fensterfälle.
macOS Release besteht ebenfalls 529/529 unter
`build/run-index-release-mac/test-results/run-sqfa86sl` mit Apple Clang 16,
Intel macOS 14.6.1 und SDL 3.2.30; auch dort werden alle 16 Sprachprogramme und
31 Module gebaut. Debian 12 verwendet GCC 12.2.0, Kernel 6.1.0-53-cloud-amd64
und SDL 3.2.30. Die Wiederholungen verwenden den unveränderten Produktionscode
mit dem korrigierten und erweiterten Mutationsprüfer.
macOS Release besteht die beiden App-Prüfungen ebenfalls 2/2 unter
`build/run-index-release-mac/test-results/run-fx8u_vc0`.
Das verschobene Linux-SDK besteht die vollständige unabhängige Prüfung unter
`build/run-index-sdk-proof-linux/Native SDK ä k2jnfyn3` in der VM. Sie prüft alle
gelieferten Header separat, baut die 20 Core-Quellen neu, führt 16 Sprachprogramme,
31 Module und neun native Projektbuilds aus. Neue Indexprobes laufen gegen das
installierte und das neu gebaute Archiv, jeweils mit Million-Zeilen-Fixture,
Allocatorfehlern, Recovery und unabhängigem Pythoncodec. Alle 305 Manifestdateien
des SDKs stimmen mit ihren SHA-256-Werten überein.
Das macOS-SDK besteht dieselbe vollständige Prüfung unter
`build/run-index-sdk-proof-mac/Native SDK ä ycjz0z88`, ebenfalls einschließlich
beider Indexarchive, Million-Zeilen-Dateien und unabhängiger Codec-Prüfung.
Die abschließenden Pakete liegen unter `build/Run Index clean SDK ä mac` bzw.
`build/Run Index clean SDK ä linux` in der VM. Ihre jeweils 305 Manifestdateien
werden erneut gehasht. Header, Archive, ausgelieferte Core-Quellen und alle
Programme stimmen bytegenau mit den vollständig geprüften verschobenen SDKs
überein; die Pakete enthalten die abschließenden Nachweistexte.

Die Indexfinalisierung und das geprüfte Öffnen sind zusätzliche lineare Scans;
gezielte nachfolgende Abfragen profitieren von Checkpoints. Kompression,
weitere Messdatentypen, schnelleres ungeprüftes Öffnen und eigene direkte
Sprachmethoden für die gezielten C-Abfragen sind damit nicht abgenommen.

## Strukturierte Diagnosen am 6. Oktober 2026

Der neue besitzende UTF-8-Wert trägt Fehlercode, Operation, Argument und
ursprüngliche Quellposition. Ein optionaler Experimentcontext-Tail und ein
optionaler Analysecallback erweitern API/ABI 3 ohne verschobene bestehende
Felder. Physim 0.175.0 bindet Kopien, Formatierung, Bytes, Dateien und Auslösen.
CRC-geschützte `.psdiag`-Sidecars entstehen exklusiv; bereits vorhandene Dateien
bleiben erhalten. IPC 5 sendet Typ 12 nur mit `--diagnostics`, ansonsten weiterhin
Typ 7 mit bisherigem Fehlertext. Messdateien und Snapshots bleiben unverändert.

Die Core-Prüfung kontrolliert UTF-8, Feldgrenzen, Fehlercodes, Koordinaten,
unveränderte Ausgaben bei ungültigen Eingaben, sämtliche gekürzten Payloads,
CRC und Versionen sowie alte Context-Grenzen. Die Runnerprüfung vergleicht
C-/Physim-Fehler beim Schritt und bei Analysen mit ihren gespeicherten Feldern,
beide Wire-Modi und unveränderte fremde Sidecars. Ein Szenenfehler behält seinen
Code `PS_SINGULAR`; ein eingefrorener alter ABI-3-Analysetail mit `run_many`
bleibt für null Eingaben verwendbar. Abgefangene Sprachfehler hinterlassen keine
Hostdiagnose; Rücksetzen und Wertkopien werden ebenfalls ausgeführt.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
4/4 unter `build/diagnostics-asan-linux/test-results/run-dgsghrph` in der
Debian-12-VM (GCC 12.2.0, Kernel 6.1.0-53-cloud-amd64).
Linux Release besteht alle 528 Prüfungen unter
`build/diagnostics-release-linux/test-results/run-c20csgrl`.
Die X11/Mesa-Fensterprüfung unter Xvfb und Openbox besteht 3/3 unter
`build/diagnostics-release-linux/test-results/run-y6ztxcgb`: vollständiger
Physim-Ablauf, eigenständige Analyseprojekte und Diagnosen. Letztere bauen und
führen vier fehlschlagende C-/Physim-Experiment-/Analyseprojekte aus, prüfen
Sidecars und Quellmarkierungen; ein fünfter Ablauf öffnet eine zusätzliche
Unicode-Quelldatei. Es ist keine vollständige Prüfung aller 58 Fensterfälle.

Der erste macOS-Gesamtlauf endet 527/528 unter `run-ujep0jyy`: der während
des laufenden Builds ergänzte Szenenfehler-Test verwendet dort noch den vorher
gebauten Runner. Dieser Lauf bestätigt nicht die abschließende Korrektur.
Der korrigierte Runner gibt die veröffentlichte Szenendiagnose statt des
bisherigen pauschalen `PS_NUMERIC` zurück.
Der anschließende macOS-Release-Gesamtlauf besteht 528/528 unter
`build/diagnostics-release-mac/test-results/run-fg394xdy` (Intel macOS 14.6.1,
Apple Clang 16, SDL 3.2.30). Beide Gesamtläufe bauen außerdem 16 eigenständige
Physim-Programme und 31 Experiment-/Analysemodule.

Das verschobene Linux-SDK besteht die vollständige unabhängige Prüfung unter
`build/diagnostics-sdk-proof-linux/Native SDK ä 3koqddhm` in der VM. Sie kompiliert
alle gelieferten Header separat, baut die Core-Bibliothek aus den gelieferten
Quellen neu und führt Programme, Module und neun native Projektbuilds aus.
Neue Diagnoseprobes werden gegen das installierte und das neu gebaute Archiv
gelinkt. C-/Physim-Runner, beide Wire-Modi, `.psdiag`, Szene und eingefrorenes
Analyse-ABI werden auch aus dieser Installation ausgeführt. Das SDK-Manifest
enthält 300 Dateien; alle SHA-256-Werte sind geprüft.
Die Sichtprüfung fand lange absolute Pfade, welche die Fehlermeldung aus der
anklickbaren Zeile verdrängten, sowie sichtbare Ersatzglyphen für Zeilenumbrüche.
Die App zeigt dort jetzt den Dateinamen und Leerzeichen; die vollständigen
Quellpfade und Nachrichten bleiben in den Diagnosewerten erhalten.
Die abschließenden fünf Linux-Diagnoseabläufe bestehen erneut 1/1 unter
`build/diagnostics-release-linux/test-results/run-qrbgnbgl`;
Screenshots der Fehleranzeige und zusätzlichen Quelldatei wurden geprüft.
macOS Release besteht die drei App-Prüfungen mit der abschließenden Anzeige
ebenfalls 3/3 unter `build/diagnostics-release-mac/test-results/run-i8kxzhzp`.
Der Physim-Screenshot zeigt die ursprüngliche Erstellungszeile des Diagnosewerts,
Code/Operation/Argument und den lesbaren UTF-8-Fehlertext.
Das macOS-SDK mit der abschließend geprüften App besteht dieselbe vollständige
unabhängige Prüfung unter
`build/diagnostics-sdk-proof-mac/Native SDK ä efo47ad0`: 16 Sprachprogramme,
31 Module, Diagnoseprobes gegen beide Archive, C-/Physim- und alte ABI-Runner
sowie neun native Projektbuilds.

Abschließende Pakete liegen unter `build/Diagnostics clean SDK ä mac` bzw.
`build/Diagnostics clean SDK ä linux` in der VM. Ihre jeweils 300 Manifestdateien
werden erneut per SHA-256 geprüft. Header, Core, Compiler, Runner, Builder und
gelieferte Core-Quellen stimmen mit den vollständig geprüften verschobenen SDKs
überein. Die aktualisierte App stimmt jeweils mit dem Programm der abschließenden
Fensterprüfung überein; die Pakete enthalten die abschließenden Nachweistexte.

Die Implementierung ergänzt explizite Diagnosen an Modulgrenzen; bestehende
niedrige C-Funktionen liefern weiterhin ihre dokumentierten Ergebniswerte.
Prozessabstürze und Zeitüberschreitungen besitzen damit noch keine originale
Sprachquellposition. Die Dateiablage ist keine Stromausfall-Durabilitätsgarantie.

## Lokale Szenenkoordinaten am 6. Oktober 2026

Explizite `PS_FRAME`-Knoten ergänzen hierarchische Translation, Quaternionrotation
und nichtuniforme Skalierung. Gruppen und Geometrie-Eltern behalten ihre bisherige
Bedeutung; ausschließlich Frame-Vorfahren transformieren lokale Geometrie.
Spiegelungen und entstehende Scherung wirken auf vollständige Mesh-Vertices und
inverse-transponierte Normalen. Picking und Transparenzsortierung verwenden
dieselben Welt-Vertices, Labels dieselben Weltanker. Ein gemeinsamer lokaler
Polyline-Pool wird nicht verändert. Der Inspektor zeigt Welt- und Lokalposition.

API/ABI 3 und Objekt-/Szenenlayouts bleiben erhalten. Snapshot 3 speichert die
lokalen Daten und unterstützt Versionen 1 und 2 beim Lesen. Version 2 weist
Frame-Form 9 ab. IPC 5 grenzt die neue Interpretation ab; Messdateiformat 1 bleibt
unverändert. Physim 0.174.0 bindet `sceneFrame`, `sceneTransform` und
`sceneWorldPoint`. Die Rendergrenze wird nach Umrechnung angewendet; ein GPU-Test
prüft ausdrücklich eine große lokale Kugel, die durch Skalierung in den
sichtbaren Weltbereich fällt.

Der Core-Test prüft analytische verschachtelte TRS mit Spiegelung, lokale
Annotationen, organisatorische und geometrische Eltern, gemeinsam verwendete
Polyline-Punkte unter unterschiedlichen Rahmen, umgeordnete Snapshots,
unveränderte Eingaben/Ausgaben bei ungültigen Zahlen, Zyklen, null Scale,
null Quaternion, nicht darstellbarer Komposition und numerisch singulären Basen.
Wire- und Messdateirundläufe erhalten alle lokalen Werte. Der Runnervergleich
prüft sämtliche 21 Messwerte und aufgezeichneten C-/Physim-Szenen sowie die
analytische Weltbewegung eines Körpers; fehlende Frame-Capability wird abgewiesen.
Die eigene Sprache prüft ihre Weltpunkt- und Matrixabfragen zusätzlich per
`assert` im ausgeführten Experiment.

Die gezielte macOS-Debug-Prüfung von Frame-Core, Hierarchie und Runner besteht
3/3 (`build/scene-frames-debug-mac/test-results/run-7tv5i269`); nach Ergänzung der
Sprachabfragen bestehen Frame-Core und Runner 2/2 (`run-ke41ga22`).
Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
4/4: Frames, Hierarchie, Runner und Protokollmutationen unter
`build/scene-frames-asan-linux/test-results/run-e48v3kom` in der Debian-VM.

Die App-Abläufe bauen C- und Physim-Projekte, wählen Weltgeometrie und Labels
per Maus, prüfen Frame-Sichtbarkeit, Schrittwerte und Wiederöffnung ohne
Veränderung der Messdatei. Ein früher Test fand alte Label-Klickflächen nach
Ausblenden; die Viewport-Ausgabe setzt diese Bounds nun in jedem Frame zurück.
Die neue Unicode-Beschriftung zeigte außerdem die bisher fehlende griechische
UI-Glyphenrange. UI-Schriften binden nun wie Codeschriften den Bereich
U+0300–U+04FF ein; der Test prüft das gebackene Alpha-Glyph und die gerenderte
Ansicht. Ein fehlerhafter Font-Cast im neuen Testsetup wurde korrigiert.
macOS Release besteht GPU, Plots, bisherige Hierarchie und neue Frame-Abläufe
4/4 unter `build/scene-frames-release-mac/test-results/run-3nxi83q0`.
Debian Release mit X11/Mesa, Xvfb und Openbox besteht dieselben vier
Fensterprüfungen 4/4 unter
`build/scene-frames-release-linux/test-results/run-473b86k9`.
macOS Debug besteht GPU, bisherige Hierarchie und neue Frame-Abläufe 3/3:
`build/scene-frames-debug-mac/test-results/run-swv6vzts`.
Der GPU-Test prüft alle sieben Meshformen in perspektivischer und orthografischer
Sicht unter verschachtelter Rotation, nichtuniformer Scale und Spiegelung.

Ein erster Debian-Gesamtlauf vor den abschließenden Sprachabfragen bestand
525/525 (`build/scene-frames-release-linux/test-results/run-i3kt5ndg`).
Der parallel laufende erste macOS-Gesamtlauf verwendete noch den Compiler vor
diesen Abfragen und endete 524/525 (`run-bobtxr8j`): nur das inzwischen erweiterte
Frame-Beispiel konnte die neuen Namen nicht auflösen. Dieser Lauf ist keine
vollständige Prüfung des abschließenden Stands. Die anschließenden Gesamtprüfungen des feststehenden Quellstands bestehen
jeweils 525/525: macOS Release unter
`build/scene-frames-release-mac/test-results/run-u_zmy4jx` und Debian Release unter
`build/scene-frames-release-linux/test-results/run-rvk6rjdr`.

Die installierten Release-SDKs wurden für die Prüfung in Unicode-Pfade mit
Leerzeichen verschoben. Header-Einzelübersetzung, alle mitgelieferten C-/Physim-
Beispiele, Nutzerprojektbuilds und die bisherigen Sensor-/Analyse-/Logging-
Proben bestehen. Neu hinzu kommen tatsächliche C-/Physim-Frame-Module mit
aufgezeichneten Szenen und unabhängige Frame-API-Proben gegen das installierte
und das aus gelieferten Quellen neu gebaute Core-Archiv.
Beide vollständigen Verifizierungen bestehen:
`build/scene-frames-sdk-proof-mac/Native SDK ä zki8xdhc` und in der VM
`build/scene-frames-sdk-proof-linux/Native SDK ä qmrr709o`.

Der Paketvergleich fand lokale Finder-Metadaten in den zuvor erzeugten SDKs.
Der Installer schließt `.DS_Store` und AppleDouble-Dateien `._*` nun aus;
der Verifier prüft diese Eigenschaft. Die bereinigten Pakete unter
`build/Scene Frames clean SDK ä mac` und entsprechend `... ä Linux` enthalten
je 288 Dateien; alle SHA-256-Werte sind geprüft. Archive, Programme, Header,
Quellen, Beispiele und fachliche Dokumentation sind bytegleich mit den bereits
vollständig verifizierten Paketen. Auf macOS wurde zusätzlich nur dieser
Plattformbericht aktualisiert. Das Entfernen der lokalen Metadaten verändert
keine ausführbaren oder öffentlichen SDK-Inhalte.

Vertrag und Grenzen stehen unter [Szenenkoordinaten](scene-frames.md).
Numerisch singuläre Rahmen und null Scale werden abgewiesen. Die Geometrie
hat eine Kamera-/Rendergrenze und ersetzt keine physikalische Kopplung. Diese
Änderung wurde lokal bislang unter macOS 14.6.1/AppleClang 16 und Debian 12/GCC
12.2 geprüft; andere Plattformkombinationen werden nicht daraus abgeleitet.

## Explizites Experiment-Logging am 6. Oktober 2026

Der Host besitzt einen expliziten synchronen Logger; der optionale Context-Tail
bewahrt die bisherigen Feldpositionen von ABI 3. Core- und Experiment-API
validieren Schweregrad, endliche Modellzeit und 1–1024 UTF-8-Bytes. Physim 0.173.0
bindet Debug/Info/Warning/Error mit Bool-Rückgabe. Der Runner schreibt exklusive
JSONL-Sidecars und sendet ausschließlich mit `--log-events` zusätzliche
Wire-4-Frames vom Typ 11. Typ 10 bleibt SPEED. Meldungen aus Initialisierung und
Zerstörung stehen außerhalb der HELLO/BYE-Phase; die App nimmt sie korrekt an.

Die neue Prüfung verwendet echte C-/Physim-Module und prüft Unicode, JSON-Escaping,
Zeitpunkte, Parameterabfrage ohne Log-I/O, Default-Clients ohne Logframes,
Opt-in-Clients mit lückenlosen Sequenznummern, exklusive Fremddateien und
unveränderte Rohdaten trotz abgewiesenem Logging. Ein Burst von 5000 Aufrufen
bleibt auf 4096 akzeptierte Records plus eine Zusammenfassung begrenzt.
Core-Tests prüfen unabhängige Sinks, Grenzlängen, ungültiges UTF-8/Controls,
kurze ältere Contexts, weitergegebene Fehler, alle vier Sprachwrapper und
transaktionale Wire-Decodierung. Eine zusätzliche Compilerprüfung lehnt die
Experimentfunktion im Standalone-Modus ab.

Tatsächlich ausgeführte Release-Gesamtprüfungen bestehen 523/523 auf beiden
Plattformen: macOS 14.6.1 (23G93), Intel, AppleClang 16.0.0 und SDL 3.2.30 unter
`build/logging-release-mac/test-results/run-vha0_q58`; Debian 12, Kernel
6.1.0-53-cloud-amd64, GCC 12.2 und SDL 3.2.30 unter
`build/logging-release-linux/test-results/run-viquytof` innerhalb der VM.
Diese Gesamtläufe gingen der abschließenden App-Pufferkorrektur voraus;
deren gezielte Nachprüfungen werden separat ausgewiesen.

Der Debug-Loggingtest einschließlich beider Runner besteht auf macOS 3/3
(`build/logging-debug-mac/test-results/run-1r68gma0`), der zusätzliche
Standalone-Compilercheck 1/1 (`run-svg1tt44`). Ein früher Funktionsname kollidierte
mit dem vorhandenen mathematischen Logarithmus und wurde in
`psrt_log_message` geändert. Das C-Beispiel ruft seine Initialisierung aus
`create` auf, wie die übrigen nativen Modelle; der Runner führt keinen
zusätzlichen Reset-Callback aus.

Der App-Ablauf baut C- und Physim-Projekte, startet pausiert, betätigt den
Einzelschritt und prüft Zeit, Kanalwert, Logtexte und gespeicherte Meldungen.
Der zusätzliche C-Burst prüft gültiges UTF-8 nach der Begrenzung des 64-KiB-
Anzeigepuffers und die gespeicherte Drop-Zusammenfassung. Alte Texte werden jetzt
an einer Zeilengrenze entfernt. Die Ansicht wurde nach Scrollen zum Logende
auch visuell kontrolliert. macOS Release besteht 1/1 einschließlich aller drei
Teilabläufe: `build/logging-release-mac/test-results/run-m6rx4ffp`.
Im vorangegangenen Dreifachlauf bestanden Reset und Workspace, während das neue
Burst-Testsetup eine bereits bekannte Parameterauswahl fälschlich erneut als
unbekannten Parameter restaurieren wollte (`run-r2_cczsx`, 2/3). Das Setup setzt
nun den vorhandenen Parameterwert. Eine frühere generische 15-Sekunden-Frist wurde
für diesen Projektbuild-Ablauf auf dieselben 120 Sekunden wie vergleichbare
bestehende Fensterfälle gesetzt. Messwert- und Protokollassertions bleiben erhalten.

Die letzte Linux-Release-Nachprüfung von Core-Logging, C-/Physim-Runnern und
Standalone-Compilercheck besteht 4/4:
`build/logging-release-linux/test-results/run-3kcsx0za`.
Die letzte macOS-Release-Nachprüfung derselben vier Fälle besteht 4/4:
`build/logging-release-mac/test-results/run-3nxgo4et`.
Die Debian-X11-Fensterprüfung mit Mesa 22.3.6, Xvfb und Openbox besteht 3/3:
Logging (C, Physim, Unicode-Burst), Reset und Workspace unter
`build/logging-release-linux/test-results/run-rai17wux`.

Linux GCC Debug mit AddressSanitizer und UndefinedBehaviorSanitizer besteht
3/3 nach der abschließenden Änderung des C-Beispiels zu context-eigenem Zustand:
`build/logging-asan-linux/test-results/run-9f7sa_li`. Eine macOS-Sanitizerprüfung
wird weiterhin nicht behauptet: Der benötigte `ld64.lld` fehlt lokal.

Die portablen Release-SDKs wurden in Pfade mit Leerzeichen und Umlauten
verschoben. `verify-native-sdk.py` prüft unabhängige Header-Übersetzung,
installiertes und aus gelieferten Quellen neu gebautes Core-Archiv, die
mitgelieferten C-/Physim-Beispiele und echte Nutzerprojektbuilds. Zusätzlich
prüft er jetzt das Logging durch das installierte und neu gebaute C-Archiv
sowie ein Physim-Modul mit denselben JSONL-/Wire-Assertions wie die Runner-Suite.
Beide Prüfungen bestehen vollständig:
`build/logging-sdk-proof-mac/Native SDK ä mhoxlqxl` und in der VM
`build/logging-sdk-proof-linux/Native SDK ä 9e4_elo9`.
Die Manifeste enthalten 287 Dateien unter macOS und 284 unter Linux; alle
SHA-256-Werte wurden geprüft.

Auch bereits im vorherigen Masken-Build übersetzte ABI-3-Pendelmodule laufen
unverändert im neuen Runner: je drei Samples unter macOS und Debian, ohne
Log-Sidecar. Beide Tests verwenden die alten Binärdateien aus
`build/series-mask-mac/bin/pendulum.so` bzw.
`build/series-mask-linux/bin/pendulum.so`. Die Referenzprüfung bestätigt alle
20 generierten API-Dokumente und ihre Offline-Navigation.

Vertrag und praktische Grenzen stehen unter [Logging](logging.md). Beliebige
stdout-/stderr-Ausgaben eines interaktiven Moduls werden nicht abgefangen;
`fflush` ist keine Zusicherung gegen Stromausfall. Der Logkanal ersetzt keine
OS-Sandbox und keine allgemeine Ressourcenbegrenzung. Windows, Wayland und
weitere OS-/Grafikkombinationen wurden für diese Änderung nicht lokal geprüft.

## Explizite Masken in Datenreihen und Berichten am 6. Oktober 2026

Die neue C-/Physim-Maske erhält Zeilen, Alignment und Dataset-Lebensdauer.
Slice, affine Rechnung, binäre Operationen, Segmentableitungen, kumulative
Integration, gleitende Mittel, Statistik, Quantile und Resampling beachten die
Gültigkeit. Eine Integral-Lücke macht spätere kumulative Werte unbekannt.
PCHIP berechnet Randsteigungen innerhalb zusammenhängender gültiger Segmente;
Nearest/Previous übernehmen die Gültigkeit ihres ausgewählten Knotens.
Scratch-Quoten berücksichtigen ein zusätzliches Maskenbyte pro Zeile.

Der neue C-Test prüft genaue Werte, Maskenschnittmengen, gültige Nullwerte,
Blockgrenzen, Freigabe der Eingabehandles, 5000 Zeilen mit mehr Lücken als
Vorschaupunkten, quota-bedingte Rücknahme, Allocation-Failure beim Kurvenaufbau,
CSV, erhaltene Segmentgrenzen, Format-2-Rundlauf, CRC-gültige falsche Flags und
unbekannte Versionen mit unverändertem Ausgabeargument. Die Sprachprüfung verwendet
dieselben Segmentwerte und prüft einen Quellfehler beim Zugriff auf eine fehlende
Beobachtung. C-Strukturen und ABI 3 ändern sich nicht. Maskierte Berichte verwenden
Format 2; unmaskierte Berichte werden weiterhin als Format 1 geschrieben und
beide Formate gelesen. Der Sprachvertrag ist 0.172.0.

Die macOS-Debug-Prüfungen von Series, Auswahl, Resampling, PCHIP und Report
bestehen 5/5: `build/series-mask-debug-mac/test-results/run-w948zpe6`.
Neue Masken- und Sprachprüfungen bestehen 2/2:
`build/series-mask-debug-mac/test-results/run-6q4slaya`.
Die erste gezielte Debian-Release-Prüfung besteht 8/8:
`build/series-mask-linux/test-results/run-jdozfbfx` innerhalb der VM.

Der neue Fensterablauf legt ein unabhängiges C-/Physim-Analyseprojekt an,
baut nur die Analyse und startet eigene Daten über die echte Schaltfläche ohne
Eingabedatei. Er prüft maskierte Linien, Segmentableitungen, PCHIP und
Wiederöffnung. Report-/CSV-/SVG-/PNG-Dateien bleiben beim erneuten Öffnen
bytegleich. Eine unabhängige Python-PNG-Prüfung prüft CRC, Pixel und ausdrücklich
das Fehlen blauer Verbindungslinien in zwei Messlücken.
Der frühe Testaufbau blieb auf der Editoransicht; nach Wechsel in die
Ergebnisansicht führte ein vertauschtes Plot-Receiverargument im Physim-Beispiel
zu vertauschten Achsen. Beide Test-/Beispielkorrekturen sind enthalten.
Der macOS-Debug-Fensterablauf besteht danach:
`build/series-mask-debug-mac/test-results/run-e2plk3ot`.

Die ersten vollständigen Release-Läufe bestanden auf beiden Plattformen
517/520: macOS `build/series-mask-mac/test-results/run-o9lo_shg`, Debian
`build/series-mask-linux/test-results/run-dcl3dlhv`. Drei alte Regressionserwartungen
gehörten noch zum Serienformat 3 beziehungsweise zum Abbruch bei fehlender
Endmessung. Der Parameterprüfer verlangt jetzt Serienformat 5, den Status in CSV
und weiterhin die richtigen Parameterwerte/Rohdaten. Der Sensorprüfer verlangt
einen erfolgreichen Abschluss mit expliziter fehlender Messung und überprüft
die reine Messabdeckung ohne erfundene Statistik. Diese drei Prüfungen bestehen
anschließend jeweils 3/3: macOS
`build/series-mask-mac/test-results/run-2mmi837n`, Debian
`build/series-mask-linux/test-results/run-rrik9_te`.

Die anschließenden vollständigen Release-Läufe bestehen auf beiden Plattformen
520/520: macOS `build/series-mask-mac/test-results/run-vivgzdti`, Debian
`build/series-mask-linux/test-results/run-8lw_67_d`. Damit sind auch die bisherigen
Sprach-, Sensor-, Daten-, Builder-, Dokumentations- und Mutationsprüfungen erneut
ausgeführt; die drei überholten Erwartungen sind in dieser Abnahme korrigiert.

Die vollständige Fenstergruppe wurde mit allen 55 Abläufen ausgeführt. Der erste
macOS-Lauf besteht 54/55: `build/series-mask-mac/test-results/run-qpvnlu6t`.
Der erste Debian-Lauf besteht 52/55:
`build/series-mask-linux/test-results/run-e8um47qv`.
Der neue Maskenablauf besteht in beiden Läufen einschließlich Report-Rundlauf,
SVG-Gleichheit und unabhängiger PNG-Pixelprüfung zweier Messlücken.

Der Sensor-Testpräfix `missing-` erfasste auch die bestehende Workspace-Prüfung
`missing-addition`. Die Auswahl verwendet jetzt exakt `missing-c`/`missing-phys`.
Workspace und Sensorablauf bestehen danach auf macOS 2/2:
`build/series-mask-mac/test-results/run-im89m2cu`.
Unter Debian fehlten zusätzlich die erwartete Zoom-Eingabeisolation und der
Mindestfortschritt bei 4×-Tempo. Die Grenzwerte wurden nicht verändert.
Ohne parallele macOS-Fensterprüfung bestehen Zoom-Eingabeisolation, Workspace,
Tempo und Sensorablauf 4/4: `build/series-mask-linux/test-results/run-prq9yeua`.
Die Ursache der beiden früheren Linux-Abweichungen ist damit nicht abschließend
belegt. Eine einzelne fehlerfreie vollständige 55/55-Wiederholung wird nicht
behauptet; alle zuvor fehlgeschlagenen Abläufe sind erfolgreich nachgeprüft.
Maskierte Signal-, Ableitungs- und PCHIP-Ansichten wurden bei 1080 × 740 auf
beiden Plattformen visuell geprüft.

Die installierten SDKs bauen und betreiben die neuen unabhängigen C-/Physim-
Maskenanalysen auch nach Verschieben in Pfade mit Leerzeichen und Umlaut.
Private Core-Helfer wie `src/report_mask.inc` werden vollständig mitgeliefert.
Die Abläufe mit Daten ohne Eingabedatei, Export-Pixelprüfung und Wiederöffnung
bestehen: `build/series-mask-sdk-proof-mac/app-steps.json` und
`build/series-mask-sdk-proof-linux/app-steps.json` innerhalb der VM.
Alle 19 generierten API-Referenzdokumente sind geprüft.

Ausgeführt wurden Intel macOS 14.6.1 mit Apple Clang 16 und SDL 3.2.30 sowie
Debian 12 mit Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30 und
X11/Xvfb/Openbox/Mesa 22.3.6. Diese Nachweise belegen den genannten Stand und
keine vollständige Abnahme aller übrigen Roadmap-Ziele.

Die vier Kernprüfungen (Resampling, Report, Masken, PCHIP) bestehen unter
Debian/GCC mit AddressSanitizer und UndefinedBehaviorSanitizer 4/4:
`build/series-mask-asan-linux/test-results/run-at6ywg_j`.
Der macOS-Sanitizer-Build wurde vor dem Bauen abgewiesen, weil der erforderliche
Mach-O-Linker `ld64.lld` fehlt. Daraus wird keine macOS-Sanitizer-Abnahme abgeleitet.

## Laufserien mit fehlenden Sensor-Endwerten am 6. Oktober 2026

Der Controller unterscheidet erfolgreiche Runner-Abschlüsse von gültigen
Endmessungen. Sensorstatus 0 und 2 beenden die Serie nicht mehr; Journal und
Endpunkt-CSV speichern ihren Status mit leerem Wert. Ein gültiger Nullwert
bleibt numerisch null mit Status 1. Unbekannter Status, defekte Dateien und
Prozessfehler bleiben Fehler. Statistik zählt nur gültige Endwerte. Berichte
weisen Anzahl und Anteil fehlender Messungen aus; ohne gültige Endwerte entsteht
nur die Messabdeckung. Parameterstudien mit Lücken zeigen gültige Einzelpunkte
anstelle einer Verbindung über fehlende Messungen.

Der neue Integrationstest verwendet die echte Sensor-API in C und Physim mit
bekannten gültigen, nicht fälligen und ausgefallenen Endmessungen. Geprüft sind
Statusmasken, erwartete Mittelwerte und Histogrammzählungen, sequenzielle/
parallele Wiederholbarkeit, Status-CSV mit leeren Feldern, Parameterpositionen,
Fortsetzung mit fehlenden Endwerten, gemeinsame/adaptive Zielzeit mit verkürztem
Schlussschritt, vollständig fehlende Messungen, ungültige Statuswerte, ein
gültiger Nullwert und Tabellenexport. 210 abgeschlossene Läufe mit nur 70
gültigen Endwerten erzeugen kein Mittelwert-KI. Die bestehende Referenz prüft
die Grenze 199/200 für die numerische Statistik. Der Fortsetzungstest liest auch
alte Journale ohne Statusspalte und prüft die Übereinstimmung mit den Rohdaten.

Die erste macOS-Debug-Runde bestand 4/5. Die Erwartung des Mittelwerts aus 70
Messungen verwendete exakte Float-Gleichheit trotz skalierter Summation. Sie
verwendet jetzt eine numerische Toleranz; der neue Test besteht danach 1/1:
`build/batch-missing-debug-mac/test-results/run-9cax1hus`.
Die fünf abschließenden Release-Prüfungen (Referenz, Parallelbetrieb, Zielzeit,
Fortsetzung, fehlende Endwerte) bestehen auf beiden Plattformen 5/5: macOS
`build/batch-missing-mac/test-results/run-y3f1141l`, Debian
`build/batch-missing-linux/test-results/run-7vrgjrnz` innerhalb der VM.

Der Fensterablauf baut die unveränderte C-/Physim-Vorlage „Wurf mit Unsicherheit“,
misst jeweils 64 Sensor-x-Endwerte, zeigt Histogramm und Messabdeckung und erzeugt
anschließend eine Serie, deren Endzeit zwischen zwei Sensorzeitpunkten liegt.
Diese vollständig fehlende Endmessung wird als Tabellenbericht angezeigt und
über die Laufbibliothek in einem neuen App-Prozess geöffnet. Rohdaten und der
wieder geöffnete Bericht bleiben bytegleich. Der neue Debug-Fensterablauf besteht
auf dem Intel-Mac: `build/batch-missing-debug-mac/test-results/run-6fepmujp`.

Neue/bestehende Serien, Fortsetzung, Zielzeitstudien, Parametereinheiten und
Dokumentationsfenster bestehen auf beiden Plattformen im Release-Build 6/6:
macOS `build/batch-missing-mac/test-results/run-w6t3gcg3`, Debian
`build/batch-missing-linux/test-results/run-an0rphs9`.
Die CLI beendet eine Serie mit sechs vollständig ausgefallenen Endmessungen
auf beiden Plattformen mit Exitcode 0 und `valid=0`, `missing=6`:
`build/batch-missing-cli-proof-mac/cli-proof.json` beziehungsweise
`build/batch-missing-cli-proof-linux/cli-proof.json` innerhalb der VM.

Installierte SDKs bestehen denselben C-/Physim-Sensorablauf einschließlich
vollständig fehlender Endwerte und Wiederöffnung nach dem Verschieben in Pfade
mit Leerzeichen und Umlaut: `build/batch-missing-sdk-proof-mac/app-steps.json`
und `build/batch-missing-sdk-proof-linux/app-steps.json` innerhalb der VM.
Die Messabdeckung wurde auf beiden Plattformen bei 1080 × 740 visuell geprüft.
Eine zu lange neue Zeilenbeschriftung wurde gekürzt; der abschließende SDK-Ablauf
verwendet die korrigierte Tabelle mit dimensionsloser Laufzahl und Prozentanteil.
Alle 19 generierten API-Referenzdokumente sind geprüft.

Ausgeführt wurden Intel macOS 14.6.1 mit Apple Clang 16 und SDL 3.2.30 sowie
Debian 12 mit Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30 und
X11/Xvfb/Openbox/Mesa 22.3.6. `physim_batch=5` kennzeichnet die private
Statusaggregation; `PSBRES01`, öffentliche API/ABI 3, Sprache 0.171.0,
Messdatei-/Bericht-/Pipe-Formate bleiben unverändert. Der Katalog umfasst nun
518 Prüfungen ohne Fenster (503 ohne SDL) und 54 Fensterabläufe.
Diese gezielten Prüfungen sind keine erneute vollständige 518/54-Abnahme.
Explizite Masken in transformierten Datenreihen und korrelierte Sensorfehler
bleiben offene Ziele; selektive Ausfälle werden nicht automatisch korrigiert.

## Fortsetzung archivierter Laufserien am 6. Oktober 2026

Monte-Carlo-Serien und Parameterstudien speichern einen privaten, versionierten
Checkpoint. GUI und CLI setzen eine Serie in einem neuen Ordner fort. Das alte
Archiv wird ausschließlich gelesen. Konfiguration, Runner, archiviertes Modul
und Quelle müssen übereinstimmen; Journal, Messdatei, Seed, Zeitachse, Endwert
und Einheiten werden geprüft. Nur vollständig journalierte Läufe werden
übernommen. Fehlende Indizes behalten ihre ursprünglichen Seeds und werden
neu ausgeführt. Dateigröße/FNV-Fingerprints dienen der Änderungserkennung,
nicht der kryptografischen Authentifizierung.

Der neue Integrationstest prüft einen Abbruch mit nicht zusammenhängenden
fertigen Indizes, bytegleiche übernommene Rohdaten, fehlende Prozessordner für
übernommene Indizes, identische Endpunkte gegenüber einer frischen Serie,
Fortsetzung vollständig fertiger Serien ohne neue Prozesse, CLI und
Parameterstudien. Konfigurationsabweichungen, Quellenänderungen, Zahlenüberlauf
im Journal, doppelte Indizes, beschädigte Messdateien, Checkpoints und unbekannte
Versionen werden abgewiesen. Der erste Fixture-Build verwendete einen falschen
Parameterfunktionsnamen; die Fixture nutzt jetzt `ps_parameter_define`.

Die macOS-Debug-Prüfungen von Fortsetzung, Parallelbetrieb und Zielzeit bestehen
3/3: `build/batch-resume-debug-mac/test-results/run-nudh14ra`.
macOS Release besteht für Fortsetzung, Referenz, Parallelbetrieb und Zielzeit
4/4: `build/batch-resume-mac/test-results/run-8u7uivgq`; Debian Release ebenfalls
4/4: `build/batch-resume-linux/test-results/run-j0l6wp0b` innerhalb der VM.
Zusätzliche adaptive Fortsetzungen mit C- und Physim-Modulen bestehen jeweils
1/1: macOS `build/batch-resume-mac/test-results/run-kkpqqz8m`, Debian
`build/batch-resume-linux/test-results/run-1h1wz8_a`.
Die abschließenden Fehlerfallprüfungen bestehen jeweils 1/1: macOS
`build/batch-resume-mac/test-results/run-pkg740sr`, Debian
`build/batch-resume-linux/test-results/run-8j9xevqz`.

Der Fensterablauf baut beide Modellsprachen, startet jeweils 64 Läufe, bricht
über die Schaltfläche ab und setzt über den Ordnerdialog-Callback fort. Hashes
aller alten Rohdateien und Serienartefakte bleiben unverändert; ungespeicherte
Editoränderungen und der vollständige Endpunktbericht bleiben erhalten.
Fortsetzung, bestehende Serien, gemeinsame Zielzeit und Parametereinheiten
bestehen auf beiden Plattformen 4/4: macOS
`build/batch-resume-mac/test-results/run-2hulfwe0`, Debian
`build/batch-resume-linux/test-results/run-2evu3r8d`.
Die Callbackprüfung ersetzt keine erneute Prüfung der nativen Ordnerauswahl.

Die Statusseite überschritt durch die ergänzten Nachweise die 256-KiB-Grenze
des Offline-Viewers. Historische Prüfabschnitte sind unverändert nach
`status-history.md` verschoben und als eigener Handbuchpunkt erreichbar.
Dokumentationsfenster und Fortsetzungsablauf bestehen danach jeweils 2/2:
macOS `build/batch-resume-mac/test-results/run-gs3np9v_`, Debian
`build/batch-resume-linux/test-results/run-0wv6lclz`.
Alle 19 generierten API-Referenzdokumente sind geprüft.

Installierte SDKs bestehen den C-/Physim-Abbruch-/Fortsetzungsablauf auch nach
Verschieben in Pfade mit Leerzeichen und Umlaut: macOS
`build/batch-resume-sdk-proof-mac/app-steps.json`, Debian
`build/batch-resume-sdk-proof-linux/app-steps.json` innerhalb der VM.
Fortsetzungsschaltfläche und Ergebnisansicht wurden auf beiden Plattformen
bei 1080 × 740 visuell geprüft. Ausgeführt wurden Intel macOS 14.6.1 mit
Apple Clang 16 und SDL 3.2.30 sowie Debian 12 mit Linux 6.1.0-53-cloud-amd64,
GCC 12.2, SDL 3.2.30 und X11/Xvfb/Openbox/Mesa 22.3.6.

API/ABI 3, Sprache 0.171.0 und Messdatei-/Pipe-Versionen bleiben unverändert.
Der Katalog enthält 517 Prüfungen ohne Fenster (502 ohne SDL) und 53
Fensterabläufe. Diese gezielten Prüfungen sind keine erneute vollständige
517/53-Abnahme. Statistische Behandlung fehlender Endwerte bleibt offen.

## Eigenständige Analyseprojekte am 6. Oktober 2026

Projektformat 2 erfordert einen ausdrücklichen Typ. Analyseprojekte benötigen
keine Experimentquelle; Format 1 bleibt für bestehende Projekte lesbar. Die
Typprüfung erhält Kommentare und unbekannte Einträge, lehnt widersprüchliche
Experimentangaben ab und konvertiert nicht während eines Einstellungsspeicherns.
Ändert sich Typ oder Quellsprache extern, verlangt die App erneutes Öffnen.
Der native Builder baut nur das Analysemodul und verwendet den Typ im Cache.

Die Importprüfung verwendet konstante Blockspeichergröße. Sie kopiert den Lauf,
prüft vollständige Zeitachse und Szenen und veröffentlicht ohne Ersetzungsflag.
Vorhandene Experimentquellen und Ressourcenlimits werden übernommen. Tests
prüfen bytegleiche Daten/Quellen, vorhandene Zieldateien, Rücknahme eigener
Dateien bei Sidecar-Kollisionen, unvollständige Läufe, beschädigte Daten,
rückwärts laufende Zeit und unbekannte Snapshot-Versionen. Fehler erhalten
Original und fremde Dateien; die Source-Prüfung benötigt keine GUI.

Die erste macOS-Debug-Prüfung mit Projektleser, Import und erweitertem echten
Projektbuild hatte falsche Writer-Namen in der neuen Testfixture sowie einen
SDK-Bin-Pfad, der beim Repository-Build nicht vorhanden ist. Die Fixture benutzt
jetzt die tatsächliche Append-/Close-API, schließt unvollständige Dateien ohne
Footer und verwendet ein zuvor gebautes Produzentenmodul. Der Importtest und
der native Projektbuild bestehen danach 2/2:
`build/analysis-project-debug-mac/test-results/run-mzjsoubv`.
Nach Format-2-Umstellung bestehen Projekt- und Importmodell 2/2:
`build/analysis-project-debug-mac/test-results/run-ljevqftl`.

Der native Projektbuild testet C-/Physim-Analyseprojekte ohne Experimentdatei,
Analysemodul ohne Experimentmodul, inkrementellen Neubau und Erhalt eines
lauffähigen Analysemoduls nach einem Compilerfehler. Laufdateien und Quellen
bleiben unverändert. macOS Release besteht 3/3:
`build/analysis-project-mac/test-results/run-6g5_t3tj`; Debian Release 3/3:
`build/analysis-project-linux/test-results/run-b4yeethl` innerhalb der VM.
Die spätere Debian-Format-/Snapshotprüfung besteht 2/2:
`build/analysis-project-linux/test-results/run-qrv3gw71`.

Der neue Fensterablauf baut zunächst einen Produzenten, stoppt seinen Lauf und
legt über die Projekttypauswahl ein eigenständiges Analyseprojekt der jeweils
anderen Sprache an. Die App baut ausschließlich dessen Analyse, importiert
über den Dialog-Callbackpfad den gespeicherten Lauf, erzeugt zwei Ergebnisplots
und öffnet das Projekt in einem weiteren Prozess. Hashes von Original,
importierten Daten, Bericht und Analysequelle bleiben erhalten. Start/Reset
sind für Analyseprojekte gesperrt. Der Dialog-Callback wird im Test mit einem
festen Dateipfad versorgt; dies belegt keinen erneuten nativen Dateidialogtest.

Die frühe Projektmanager-Ansicht hatte die neue Erstellen-Schaltfläche unterhalb
des sichtbaren Bereichs. Experimentvorlage/-sprache werden jetzt bei reinen
Analyseprojekten ausgeblendet. Neuer Ablauf, Autosave und Dokument-Build bestehen
auf macOS Debug 3/3: `build/analysis-project-debug-mac/test-results/run-pzke3zbp`;
auf Debian Release 3/3: `build/analysis-project-linux/test-results/run-lyv38ssw`.
Projektmanager und Ergebnisansicht wurden auf dem Intel-Mac visuell geprüft.

Die abschließenden Release-Modelle bestehen auf macOS 2/2:
`build/analysis-project-mac/test-results/run-ugmxr863`.
Projektablauf, Diagramme/Eingabeisolation, beide Menügrößen, Workspace-Wiederöffnung
und Dokument-Build bestehen auf beiden Plattformen 7/7: macOS
`build/analysis-project-mac/test-results/run-z6trc510`, Debian
`build/analysis-project-linux/test-results/run-mczc7v0l` innerhalb der VM.
Die installierte macOS-App baut Produzent und unabhängige Analyse auch nach dem
Verschieben des SDK in einen Pfad mit Leerzeichen und Umlaut und besteht Import,
Auswertung und Wiederöffnung:
`build/analysis-project-sdk-proof-mac/app-steps.json`.

Die letzte Importprüfung mit zusätzlichem Schutz aller bekannten Ziel-Sidecars
besteht unter Debian 1/1:
`build/analysis-project-linux/test-results/run-10klg9cn`.
Auch die installierte Debian-App besteht nach Verschieben des SDK den gesamten
Produzenten-/Analyseprojekt-/Import-/Wiederöffnungsablauf:
`build/analysis-project-sdk-proof-linux/app-steps.json` innerhalb der VM.
Projektmanager und Berichtansicht wurden ebenfalls unter X11/Xvfb/Openbox/Mesa
bei 1080 × 740 visuell geprüft. Die native Dateiauswahl selbst wird durch diese
Callbackprüfungen weiterhin nicht neu abgenommen.

API/ABI 3, Sprache 0.171.0 und Messdatei-/Pipe-Versionen bleiben unverändert.
Diese gezielten Prüfungen sind keine erneute vollständige 516/52-Abnahme.

## Messkanal-Anzeigeeinheiten am 6. Oktober 2026

Der neue private Katalog besteht auf macOS Debug:
`build/channel-units-debug-mac/test-results/run-baav6ek6`.
Geprüft sind dimensionsabhängige Schlüssel, lineare Konvertierung, signierte Null,
subnormale Faktoren, Über-/Unterlauf ohne Änderung der Ausgabe, 64 Einträge,
Ersetzen und Entfernen, UTF-8, Rundlauf, alle Abschneidepositionen und byteweise
Beschädigungen eines Zwei-Eintrag-Katalogs, CRC-gültige Duplikate, unbekannte
Versionen und fehlgeschlagene Dateiersetzung mit Erhalt des Originals.

Der gezielte Fensterablauf besteht auf macOS Debug:
`build/channel-units-debug-mac/test-results/run-kg4b4qqc`.
C- und Physim-Projekte bauen und laufen in der App. Echte Texteingaben und
Mausereignisse prüfen eigene Symbole/Faktoren, Ablehnung von null,
Live-Konvertierung, Statistik, SI-Rücksetzung und Abbrechen. F7 löst im Dialog
keinen Reset aus; der Runner läuft während der Auswahl weiter. Sensorlücken
bleiben Lücken. Ein Faktor mit Überlauf vorhandener Werte wird abgelehnt;
der ausgewählte Diagrammausschnitt bleibt bei SI-Rücksetzung erhalten.
Weitere Prozesse laden die Auswahl und prüfen die Erhaltung beschädigter Dateien
bis zum ausdrücklichen Reset. Hashes von Quellen und Laufdateien bleiben erhalten.

Die vollständigen Release-Läufe ohne Fenster bestehen auf beiden Systemen 515/515:
macOS `build/channel-units-mac/test-results/run-vkw6s26a`, Debian
`build/channel-units-linux/test-results/run-gb6eh__b` innerhalb der VM.
Die Umgebungen bleiben Intel macOS 14.6.1/AppleClang 16 und Debian 12/GCC 12.2/
SDL 3.2.30. API/ABI 3, Sprache 0.171.0, Pipe-Version 4 und Messdatei-Format 1
bleiben unverändert. Der neue persönliche Katalog verwendet Format 1.

Die finale Katalogprüfung nach Ergänzung unveränderter Schema-Namen (auch mit
Rand-Leerzeichen oder leerem Namen) besteht jeweils 1/1: macOS
`build/channel-units-mac/test-results/run-j3bl9vjk`, Debian
`build/channel-units-linux/test-results/run-bmmz2o54`.

Der erste vollständige macOS-Fensterlauf besteht 50/51:
`build/channel-units-mac/test-results/run-t7zmqdg3`. Der bisherige
Autosave-Verwerfentest beobachtet die Oberfläche zu früh; er lässt jetzt einen
vollständigen Renderframe zwischen Mausereignis und Prüfung. Unveränderte
Wiederherstellungs-/Verwerfungsassertionen und der Einheitenablauf bestehen danach
2/2: `build/channel-units-mac/test-results/run-tk8u6egn`.
Die frühe Statistik-Detailaufnahme hatte zudem eine falsche Nuklear-Signatur im
Testcode; der erkannte Buildfehler ist korrigiert.

Unter Debian verliert die VM zunächst SSH; ihre Konsole dokumentiert Kernel-RCU-Stalls.
Nach Neustart bestehen 46 Fensterfälle in
`build/channel-units-linux/test-results/run-sgmfabef`, bevor der VM-Speicherplatz
erschöpft ist. Ältere generierte Arbeitsdateien unter `build/native` werden
entfernt, die strukturierten Ergebnisberichte bleiben erhalten. Die fünf noch
nicht belegten Fälle und der korrigierte Autosave-Ablauf bestehen anschließend
6/6: `build/channel-units-linux/test-results/run-386kps2s`.
Diese Teilnachweise sind kein ununterbrochener 51/51-Komplettlauf. Über die
abgeschlossenen Läufe sind alle 51 Fensterfälle auf beiden Systemen erfolgreich
belegt; `build/channel-units-proof-mac/coverage.json` und innerhalb der VM
`build/channel-units-proof-linux/coverage.json` prüfen jeden Fall gegen den Katalog.
Frühere Fehlschläge beziehungsweise unvollständige Datenträgerberichte bleiben vermerkt.

Visuelle Prüfung bei 1080 × 740 zeigt zu enge Statistikspalten. Die Tabelle erhält
horizontalen/vertikalen Bildlauf, ausreichend breite Spalten und explizite
Abkürzungen mit vollständigem Tooltip. Die Vorschau verwendet ein lesbares
Gleichheitszeichen. Finale Einheitenansicht: macOS 1/1
`build/channel-units-mac/test-results/run-x7q06_28`; Plot-/Eingabeprüfung macOS
2/2 `build/channel-units-mac/test-results/run-e4_1g9ik`. Debian besteht die drei
Fälle 3/3: `build/channel-units-linux/test-results/run-m6hwh4os`.
Dialog und Statistik wurden auf beiden Systemen visuell geprüft.

Die installierte Release-App baut C-/Physim-Projekte auch nach dem Verschieben
in SDK-Pfade mit Leerzeichen und Umlaut und besteht denselben Einheitenablauf
mit Wiederöffnung, Dateischutz und ausdrücklichem Reset: macOS
`build/channel-units-sdk-proof-mac-final/app-steps.json`, Debian
`build/channel-units-sdk-proof-linux/app-steps.json` innerhalb der VM.
Die SDK-Prüfung betrifft hier die installierte App und deren Projektbuild;
der öffentliche Kern ist seit dem PCHIP-Nachweis unverändert. Die 19 erzeugten
Referenzdokumente bestehen weiterhin den Abgleich.

## Monotone kubische Interpolation am 5. Oktober 2026

Resampling, PCHIP-Modell und ausgeführte Physim-Analysemodule bestehen auf
macOS Debug 3/3: `build/pchip-debug-mac/test-results/run-ehr4gt2h`.
Die erste Debian-Release-Prüfung besteht ebenfalls 3/3:
`build/pchip-linux/test-results/run-fms8h68d` innerhalb der VM.
Analytische Referenzen prüfen gleichmäßige und unregelmäßige Raster,
Einheitenumrechnung, exakte Stützstellen, erste Ableitungen an inneren Knoten,
Monotonie, Plateaus, Extrema und mehrere Blockgrenzen. Endliche Extremwerte,
subnormale Achsenabstände, zwei/ein Quellpunkt, Scratch-Quota und Fehler nach
mehreren Zielblöcken sind geprüft. Ein Fehler hinter dem verwendeten Quellpräfix
wird erkannt. Ergebnisreihen bleiben nach dem Schließen der Quelle lesbar und
werden durch das Schließen des Ziel-Dataset invalidiert.

Das neue Verfahren verwendet lokale Fritsch–Butland-Steigungen mit
begrenzten einseitigen Endsteigungen. Mantissen-/Exponentenarithmetik vermeidet
die Voraussetzung eines breiteren `long double`; Bézier-Kontrollen und
de-Casteljau-Auswertung halten Ergebnisse im jeweiligen Werteintervall.
Methodische Grundlage ist die [PCHIP-Dokumentation von SciPy](https://docs.scipy.org/doc/scipy/reference/generated/scipy.interpolate.PchipInterpolator.html).
Die Implementierung ist in C geschrieben und benötigt SciPy nicht zur Laufzeit.

Der neue Fensterablauf besteht auf macOS Debug:
`build/pchip-debug-mac/test-results/run-iiiwjoeb`.
Ein C- und ein Physim-Projekt bauen ihre Analyse in der App, führen das Modell
aus und erzeugen je einen Bericht mit 129 PCHIP- und linearen Werten.
Numerische Zwischenwerte, Monotonie, Einheiten und Endpunkte sind geprüft.
Die SVGs beider Sprachen sind bytegleich; PNG misst jeweils 1200 × 850 Pixel.
Weitere Prozesse öffnen dieselben Berichte; deren SVG bleibt identisch und
Hashes von Quellen, Bericht und Laufdaten bleiben erhalten. Die Kurvenansicht
wurde bei 1080 × 740 logischen Pixeln auf dem Intel-Mac visuell geprüft.
Frühe Fixtures verwenden zunächst nicht unterstützte freie/static Series-Aufrufe
und falsche App-Hilfsfunktionssignaturen; sie verwenden jetzt die vorhandenen
Methoden und Berichtslader. Ihre Zeitgrenzen bleiben unverändert.

Sprache 0.171.0 ergänzt `Series.resampledPchip`; API und ABI bleiben auf 3.
Die privaten numerischen Helfer werden mit den SDK-Quellen ausgeliefert.
Ein unabhängiger öffentlicher SDK-Verbraucher prüft PCHIP gegen das installierte
Archiv und den aus SDK-Quellen neu gebauten Kern.
Die vollständigen Release-Läufe ohne Fenster bestehen jeweils 514/514:
macOS `build/pchip-final-mac/test-results/run-_wqs83qb`, Debian
`build/pchip-linux/test-results/run-27oiqc36` innerhalb der VM. Die Umgebungen
bleiben Intel macOS 14.6.1/AppleClang 16 und Debian 12/GCC 12.2/SDL 3.2.30.
Die vollständigen Release-Fensterläufe bestehen jeweils 50/50: macOS
`build/pchip-final-mac/test-results/run-h2yktdf9`, Debian
`build/pchip-linux/test-results/run-oo9993th` unter X11/Xvfb/Openbox/Mesa.
Auch die Linux-Kurvenansicht wurde bei 1080 × 740 visuell geprüft.
Die verschobenen Release-SDKs bestehen mit unabhängigen Headern und Verbrauchern,
installiertem und aus SDK-Quellen neu gebautem Kern, C-/Physim-Modulen,
adaptiven Läufen, Zielzeitstudien und neun neu gebauten Projekten:
macOS `build/pchip-sdk-proof-mac/Native SDK ä uftgb797`, Debian
`build/pchip-sdk-proof-linux/Native SDK ä knmhmus1` innerhalb der VM.
Der öffentliche Reihenverbraucher prüft PCHIP-Werte und Zielzuordnung mit beiden
Archiven; dadurch ist auch die Auslieferung des privaten numerischen Headers geprüft.

## Benannte Workspaces am 5. Oktober 2026

Workspace-Zustand und neuer Katalog bestehen auf macOS Debug 2/2:
`build/workspaces-debug-mac/test-results/run-a_g1mn_j`. Der Katalog prüft UTF-8,
Ersetzen gleicher Namen, acht Einträge, Löschen, doppelte Namen, verschachtelte
Zustände, alle Abschneidepositionen und byteweisen Beschädigungen, unbekannte
Versionen und fehlgeschlagene Dateiersetzung. Acht Zustände mit jeweils
32 zusätzlichen Pfaden und 16 Dokumenten an der Pfadlängengrenze bestehen im
Roundtrip. Zustände und Transaktionskopien liegen auf dem Heap. Die gemeinsame
Kodierung erhält die vorhandenen Workspace-Formate 1 und 2.

Neuer Katalogablauf, bisherige Workspace-Wiederöffnung und kleines Dokumentfenster
bestehen unter macOS Debug 3/3:
`build/workspaces-debug-mac/test-results/run-fgj6sp7s`; unter Debian Release
ebenfalls 3/3: `build/workspaces-linux/test-results/run-285x2ixz` innerhalb der VM.
Ein C-Projekt speichert seinen Workspace während einer laufenden Simulation;
der Runner und der ungespeicherte Quelltext bleiben erhalten. Öffnen und F7
sind dabei gesperrt. Ein Physim-Projekt wird anschließend als zweiter Workspace
gespeichert. Eine extern geänderte Dokumentdatei verhindert den Wechsel;
eine fehlende Analysequelle im Zielprojekt erhält den bestehenden Workspace.
Nach Auflösen des Konflikts stellt der erste Eintrag Dokumente und Editoransichten
wieder her, ohne Build oder Runner zu starten. Ersetzen und Löschen halten die
erwartete Eintragszahl. Neue Prozesse prüfen bewusste Wiederöffnung, gekürzten
Unicode-Text, Erhalt beschädigter Kataloge und ausdrückliches Zurücksetzen.
Hashes von C-/Physim-Quellen und Laufdatei bleiben unverändert.
Die erweiterten Grenzmodelle bestehen unter Debian Release 2/2:
`build/workspaces-linux/test-results/run-cpighmxl`.

Der erste macOS-Fenstergesamtlauf besteht 48/49:
`build/workspaces-final-mac/test-results/run-pe0vn8sk`. Der bestehende
`workspace_workflow` versucht einen Ordnerwechsel unmittelbar nach dem Projektladen,
während der Bibliotheksleser noch arbeitet. Die App blockiert diesen Wechsel korrekt.
Der Prüfer wartet jetzt auf den Bibliotheksleser; seine Zeitgrenze und alle
Zustandsprüfungen bleiben erhalten. Korrigierter Workspace-Ablauf und neuer
Katalogablauf bestehen unter macOS Debug 2/2:
`build/workspaces-debug-mac/test-results/run-cy86lc6v`.

Frühe Builds und Prüfungen erkennen eine Namenskollision mit dem Dateibaumtyp,
nicht bereinigte Pfadreste im Grenztest und den
Neustartklick vor dem ersten Renderframe. Diese Fehler sind korrigiert; die Zeitgrenzen bleiben
erhalten. Die Sichtprüfung bei 1080 × 740 erkennt eine zu lange Beschreibung
und abgeschnittene Pfade. Die Beschreibung ist gekürzt; Pfade zeigen einen
passenden UTF-8-Suffix samt vollständigem Pfad als Hinweis.

Die vollständigen Release-Läufe ohne Fenster bestehen jeweils 512/512:
macOS `build/workspaces-final-mac/test-results/run-d6055ur1`, Debian
`build/workspaces-linux/test-results/run-ckleqt__` innerhalb der VM.
Der wiederholte macOS-Fenstergesamtlauf besteht 49/49:
`build/workspaces-final-mac/test-results/run-t5wk63xn`. Debian besteht ebenfalls
49/49: `build/workspaces-linux/test-results/run-k_dkl_m3` unter
X11/Xvfb/Openbox/Mesa. Die finale Verwaltungsansicht mit Pfadsuffix und
gesperrter Öffnen-Aktion wurde auf beiden Systemen bei 1080 × 740 visuell geprüft.
Die neu installierten, verschobenen Release-SDKs bestehen mit unabhängigen
Headern und Verbrauchern, installiertem und neu gebautem Kern, C-/Physim-Modulen,
adaptiven Läufen, Zielzeitstudien und neun neu gebauten Projekten:
macOS `build/workspaces-sdk-proof-mac/Native SDK ä fhqa41w_`, Debian
`build/workspaces-sdk-proof-linux/Native SDK ä 4fpkhmcf` innerhalb der VM.
Die Umgebungen bleiben Intel macOS 14.6.1/AppleClang 16 und Debian 12/GCC 12.2
mit SDL 3.2.30. Sprache 0.170.0, öffentliche API und ABI 3 bleiben unverändert;
der persönliche Katalog verwendet Format 1.

## Benannte Panelanordnungen am 5. Oktober 2026

Katalog, Docking und Einstellungen bestehen auf macOS Debug 3/3:
`build/layouts-debug-mac/test-results/run-xrp18z64`. Die neue Verwaltung besteht
mit aktiven UI-Assertions:
`build/layouts-debug-mac/test-results/run-607y3f86`.
Unter Debian Release bestehen die drei Modelle ebenfalls:
`build/layouts-linux/test-results/run-9g3bs89x` innerhalb der VM; der erste
Verwaltungsablauf besteht unter
`build/layouts-linux/test-results/run-zjet3zqb`.

Die Katalogprüfung deckt leere und volle Kataloge, UTF-8-Namen, byteweise
Beschädigungen, alle Abschneidepositionen, zusätzliche Bytes, unbekannte
Versionen, ungültige Namen, doppelte Namen, Panelgraphen, aktive Tabs,
Größengrenzen und fehlgeschlagene Dateiersetzung ab. Lese- und Modellfehler
erhalten den bisherigen Zustand. Die gemeinsame Dockingkodierung erhält
das bisherige Einstellungsformat und die Migration der Formate 1–3.

C und Physim laufen in eigenen Projekten. Maus- und Texteingabeereignisse
speichern zwei Anordnungen, wenden sie an, ersetzen einen vorhandenen Namen
und löschen einen Eintrag. Teilungen, aktive Tabs, versteckte Panels, freier
Inspektor, Größen und Protokollansicht kehren zurück. Theme und Schriftgröße
bleiben bestehen; die Simulation schreitet fort, und der CRC des ungespeicherten
Editors bleibt gleich. Ein vollständig verborgener Panelbaum lässt sich weiterhin
über die Menüzeile wiederherstellen. Weitere App-Prozesse prüfen Neustart,
Erhalt einer beschädigten Datei und ausdrückliches Zurücksetzen. Die Hashes
von Quellen und Laufdaten bleiben dabei unverändert. Der finale Debugablauf
prüft zusätzlich das Anwenden aus den Einstellungen: Geometrie wird synchronisiert,
der ungespeicherte Theme-/Schriftentwurf bleibt erhalten.

Frühe Verwaltungsprüfungen lesen Texteingaben vor dem folgenden Renderframe;
sie warten jetzt auf dessen Verarbeitung. Dabei entdeckt der Prüfer, dass
die ausgewählte Zeile einen neu eingegebenen Namen pro Frame zurücksetzt.
Die Auswahl kopiert den Namen nun ausschließlich bei einem Klick.
Die bisherigen Zeitgrenzen sind unverändert. Dunkle und helle Ansichten,
UTF-8-Namen, Fehler und Zurücksetzen wurden bei 1080 × 740 logischen Pixeln
auf dem Intel-Mac visuell geprüft. Die neue Kopfzeile verwendet keinen Abstand
zwischen den Fenstersteuerungen; alle drei Buttons bleiben vollständig sichtbar.

Die vollständigen Release-Läufe ohne Fenster bestehen jeweils 511/511:
macOS `build/layouts-final-mac/test-results/run-r6lupogg`, Debian
`build/layouts-linux/test-results/run-s6mgc2eh` innerhalb der VM.
Die vollständigen Release-Fensterläufe bestehen jeweils 48/48: macOS
`build/layouts-final-mac/test-results/run-0_2371a7`, Debian
`build/layouts-linux/test-results/run-b4nqdu99` unter X11/Xvfb/Openbox/Mesa.
Auch die Linux-Ausgabe der neuen Verwaltung wurde bei 1080 × 740 visuell geprüft.
Die verschobenen Release-SDKs bestehen mit unabhängigen Headern und Verbrauchern,
installiertem und aus SDK-Quellen neu gebautem Kern, C-/Physim-Modulen,
adaptiven Läufen, Zielzeitstudien und neun neu gebauten Projekten:
macOS `build/layouts-sdk-proof-mac/Native SDK ä e1zcew_4`, Debian
`build/layouts-sdk-proof-linux/Native SDK ä h7_g5aj5` innerhalb der VM.
Die Umgebungen sind weiterhin Intel macOS 14.6.1/AppleClang 16 und
Debian 12/GCC 12.2/SDL 3.2.30. Die Sprache bleibt auf 0.170.0, öffentliche
API und ABI bleiben auf 3; der private Katalog verwendet Format 1.

## Eigenständiger Inspektor am 5. Oktober 2026

Dockingmodell und Einstellungenmigration bestehen auf macOS Release 2/2:
`build/inspector-mac/test-results/run-r0bg29vm`, und Debian Release 2/2:
`build/inspector-linux/test-results/run-2t56qv2n` innerhalb der VM.
Vier Panels, sieben Knoten, Tab-/Teilungs-/Float-/Hidden-Wechsel und transaktionale
Fehler sind geprüft. Eine echte fünfknotige Format-3-Datei erhält ihre Knoten,
Tabs und Rechtecke; der neue Inspektor bleibt zunächst verborgen. Formate 1/2,
CRC, Abschneiden, ungültige Graphen und fehlgeschlagene Dateiersetzung sind geprüft.

Die ersten vier macOS-Fensterprüfungen bestehen:
`build/inspector-mac/test-results/run-oxkweq7i` (Einstellungen, Hierarchie,
adaptive Schritte und Zielzeitserien). Docking, kleines Menü, Projekteinstellungen
und Parametereinheiten bestehen zusätzlich 4/4:
`build/inspector-mac/test-results/run-6a9mkk3c`.

Der neue Inspektorablauf besteht auf macOS:
`build/inspector-mac/test-results/run-c47n901_`.
C und Physim laufen in separaten Projekten; echte Mausereignisse lösen den
Inspektor aus der Anordnung, bedienen Objektsichtbarkeit, schließen und öffnen
ihn über das Menü, bilden eine Tabgruppe mit dem Dateibaum und ändern sein
freies Rechteck auf 320 × 480. Die Simulation läuft dabei weiter. Ein CRC des
ungespeicherten Editors bleibt identisch; es entsteht je Projekt nur ein Run.
Weitere App-Prozesse öffnen die gespeicherte Anordnung ohne Build/Runner;
Hashes von Quelle und Lauf bleiben unverändert.

Die frühe Sichtprüfung bei 1080 × 740 erkennt zu schmale Simulations- und
Zeitleistenbuttons. Bei schmalem Arbeitsbereich verwenden die Steuerungen nun
zwei Spalten, und der Timeline-Schieber hat eine eigene Reihe. Die aktuelle
Anordnung, beschriftete Steuerung und Inspektor wurden bei 2160 × 1480 physischen
Pixeln auf dem Intel-Mac visuell geprüft.
Die ersten zwei Inspektorprüfungen lesen Float-/Tabwechsel vor dem nächsten
Renderframe; sie warten jetzt wie der bestehende Dockingprüfer auf dessen
Verarbeitung. Die bisherigen Zeitgrenzen sind unverändert.

Die vollständigen Release-Läufe ohne Fenster bestehen 510/510: macOS
`build/inspector-final-mac/test-results/run-d6xhnjy0`, Debian
`build/inspector-linux/test-results/run-u18jcah3` innerhalb der VM.
Docking und Migration bestehen zusätzlich im macOS-Debug-Build 2/2:
`build/inspector-debug-mac/test-results/run-4xy3qbmt`.

Der erste macOS-Fenstergesamtlauf besteht 44/47:
`build/inspector-mac/test-results/run-_udplaoz`.
Eine unbeabsichtigte Erweiterung des Standardwerte-Verhaltens bewahrte alte
Panelbreiten und Protokollhöhe. Das bisherige Zurücksetzen ist wiederhergestellt;
die Einstellungsprüfung besteht mit demselben Prüfer:
`build/inspector-settings-final-mac/test-results/run-dbx1usxt`.
Die beiden Plotfälle erkennen eine feste, insgesamt 700 Pixel breite Exportreihe,
deren letzte Schaltfläche unter dem neuen Inspektor lag. Die Ausschnittbuttons
verwenden in schmalen Arbeitsbereichen nun eine eigene Zeile; die Größenwahl
behält ihre Funktion. Der Prüfer verlangt weiterhin tatsächliche SVG-/PNG-Dateien
und ihre Inhalte, ohne die Prüfung abzuschwächen.

Das verschobene macOS-SDK mit installierter und aus SDK-Quellen neu gebauter
Bibliothek besteht: `build/inspector-sdk-proof-mac/Native SDK ä fj4_k76s`.

Die finalen Plot-/Eingabeprüfungen bestehen auf macOS 2/2:
`build/inspector-plot-final-mac/test-results/run-pt1_ldlp`. Beide Ausschnittformate
werden über echte Schaltflächen erzeugt; die bestehenden Bild- und Bereichsprüfungen
bleiben erhalten. Der neue Inspektorablauf besteht auch mit aktiven UI-Assertions
im macOS-Debug-Build: `build/inspector-debug-mac/test-results/run-f4t25yji`.

Der korrigierte vollständige macOS-Fensterlauf besteht 47/47:
`build/inspector-plot-final-mac/test-results/run-jvfs2qf6`. Das finale verschobene
macOS-SDK einschließlich der korrigierten Exportanordnung besteht:
`build/inspector-sdk-final-proof-mac/Native SDK ä kdd9dg3j`.
Der erste Debian-Fensterlauf besteht 45/47:
`build/inspector-linux/test-results/run-vz7089wn`; ausschließlich die beiden
Plot-/SVG-Ausschnittfälle scheitern am selben zu breiten Exportlayout. Inspektor,
Einstellungen und sämtliche übrigen Abläufe bestehen bereits in diesem Lauf.

Der korrigierte vollständige Debian-Fensterlauf besteht 47/47:
`build/inspector-linux/test-results/run-m6rg6hyd` innerhalb der VM. Er verwendet
X11, Xvfb/Openbox und Mesa 22.3.6. Beide zuvor fehlgeschlagenen Plotfälle,
Inspektor, Docking, Einstellungen und sämtliche übrigen Abläufe bestehen mit
den bisherigen Zeitgrenzen. Es wurden keine zusätzlichen Plattformen oder
Wayland aus dieser Abnahme abgeleitet.

Das finale verschobene Debian-SDK mit installierter und unabhängig aus seinen
Quellen neu gebauter Bibliothek besteht ebenfalls:
`build/inspector-sdk-proof-linux/Native SDK ä 1x1xy_0j` innerhalb der VM.
Damit liegen für diese Änderung auf beiden ausgeführten Systemen vollständige
Release-, Fenster- und SDK-Nachweise vor; die UI- und Datenformate wurden nur
in den beschriebenen Umgebungen ausgeführt.

## Explizite Parametereinheiten am 5. Oktober 2026

Die Änderung ergänzt eigene Symbol-/Skalen-/Dimensionsdaten im optionalen
ABI-3-Kontext-Tail, SI-Persistenz, Runner-Abfrageformat 2 und Anzeigeumrechnung.
Die bestehenden Kontext- und Parameterfelder bleiben an ihrer bisherigen Position.
Der Katalog enthält 510 Prüfungen ohne Fenster (499 ohne SDL) und 46 Fensterfälle.

Die gezielte macOS-Release-Prüfung besteht 8/8:
`build/parameter-units-model-final-mac/test-results/run-edmj8xj3`.
Ein analytisches Modell in C und Physim deklariert `velocity` in `cm/s` mit Skala
0,01; sämtliche numerischen Messungen bleiben SI und werden exakt verglichen.
Der Studienbericht enthält 20/160/300 cm/s zu SI-Overrides 0,2/1,6/3 m/s.
Katalogformat 1 bleibt lesbar; Format 2 transportiert bekannte und unbekannte
Einheiten. Ein eigener CRC-gültiger Runner mit unvollständigen oder zwischen
Läufen widersprüchlichen Einheiten wird ohne Gesamtbericht abgewiesen.
Ein Physim-Modul mit zu langem Symbol erhält eine Quelldiagnose und Startfehler.

Der abschließende gezielte macOS-Release-Lauf besteht 5/5:
`build/parameter-units-final-target-mac/test-results/run-o5i6o17q`.
Er prüft zusätzlich Über-/Unterlauf der Anzeige, exakte Erhaltung von Standards
und bestehenden SI-Auswahlen sowie Speichern/Wiederherstellen von 50 cm als
0,5 m in der Projektdatei. Bekannte Dimensionswechsel setzen die Auswahl zurück.
Ungültige Deklarationen und Metadaten erhalten die bisherigen Ausgaben; alte
Kontextgrößen bleiben für untypisierte Parameter nutzbar.

Die zwei vollständigen Meter-/Zentimeter-Bedienfälle bestehen auf macOS:
`build/parameter-units-mac/test-results/run-vvjxc49n`.
Jeweils C- und Physim-Pendel verwenden echte Text-/Mausereignisse für Start- und
Endwert, erzeugen SI-Laufdaten, 1200 × 850 Pixel große PNGs und SVGs und öffnen
den archivierten Bericht ohne Build/Runner erneut. Sämtliche Zeitpunkte und
Kanalwerte stimmen zwischen beiden Sprachen exakt überein. Die PNG-Achse mit
50/150/250 cm und `length [cm]` wurde visuell geprüft. Wiederöffnung bewahrt
die Datenhashes.

Der erste frühe Modelllauf scheitert an noch nicht deklarierten Einheitenbezeichnern
in der Physim-Pendelvorlage; die konkreten Unit-Werte stehen jetzt vor der
Parameterdefinition. Der erste Zentimeter-Bedienlauf besteht nur für C: Das
Physim-Testmodell änderte irrtümlich auch die Messkanäle auf Skala 0,01, die
kanonische Kanalregistrierung lehnt das korrekt ab. Der korrigierte Prüfer ändert
nur die Parameterdeklaration. Diese frühen Läufe gelten nicht als finale Abnahme.

Die vollständigen Release-Läufe ohne Fenster bestehen 510/510: macOS
`build/parameter-units-model-final-mac/test-results/run-sr6cg0g0`, Debian
`build/parameter-units-linux/test-results/run-q0i1kyh5` innerhalb der VM.
Die vollständigen Fensterläufe bestehen 46/46: macOS
`build/parameter-units-mac/test-results/run-hdgug8rh`, Debian
`build/parameter-units-linux/test-results/run-s6dy7qiv`.

Die zusätzliche rundungskritische Bedienprüfung mit Standard 0,29 m und
Zentimeteranzeige besteht auf macOS 2/2:
`build/parameter-units-mac/test-results/run-gzp_j26s`. Ihr erster Aufruf verlangte
irrtümlich das Schreiben einer unveränderten Einstellung; normales Speichern
ist dann ohne Wirkung. Der Prüfer fordert nun wie das Parameterformular die
Übernahme der Auswahl an und prüft den exakt archivierten SI-Wert. Nach der
abschließenden Parseränderung bestehen beide Bedienfälle nochmals 2/2:
`build/parameter-units-mac/test-results/run-1ru5roi5`.

Die abschließende Zahlenprüfung besteht 7/7 auf macOS:
`build/parameter-units-last-model-mac/test-results/run-q3onl7t4`, und 7/7 unter
Debian: `build/parameter-units-final-linux/test-results/run-qwakwdf2`.
Sie erhält nichtnullige subnormale Skalen und Parameterwerte, einschließlich
`offset=1e-310` in C- und Physim-Runnern sowie im Batch-CLI. Ein unabhängiger
Messkanal enthält den Wert in allen gelesenen Samples exakt. `1e-999` bleibt
ungültig; Vorzeichen der Null bleiben erhalten. Eine bei einer neuen Skala zu
Null gerundete vorhandene Auswahl führt zu atomarer Ablehnung statt zum Verlust
ihres SI-Werts. Bericht, Projektdatei, Default 0,29 m und CRC-gültige Einheitenfehler
bleiben Teil dieser Prüfung. Die finalen sechs Modellfälle bestehen zusätzlich
im macOS-Debug-Build:
`build/parameter-units-debug-mac/test-results/run-ijlnvipu`.

Die SDK-Prüfung besteht mit den finalen öffentlichen und privaten Headern,
installierter und aus SDK-Quellen unabhängig neu gebauter Bibliothek, C-/Physim-
Modulen, Parameterstudien, Metadaten, Diagrammen und SVG-Beschriftungen: macOS
`build/parameter-units-sdk-final-proof-mac/Native SDK ä ppueo3yp`, Debian
`build/parameter-units-sdk-final-proof-linux/Native SDK ä 25v20ne2`.
Beide SDKs sind vor der Prüfung verschoben; keine Bibliothek aus dem Checkout
wird als Ersatz benutzt.

Mit dem finalen Zahlenparser bestehen die Meter-/Zentimeter-Bedienfälle unter
Debian 2/2: `build/parameter-units-final-linux/test-results/run-rdp6q5ef`.
Die vollständige Zentimeterprüfung mit UI-Assertions besteht auch im finalen
macOS-Debug-Build: `build/parameter-units-debug-mac/test-results/run-fj1bmvhw`.
Diese zusätzlichen Läufe prüfen die abschließenden Änderungen an Zahleneingabe
und SI-Erhaltung; die vollständigen Durchläufe oben decken die übrigen Abläufe ab.

## Laufserien mit gemeinsamer Zielzeit am 5. Oktober 2026

Der Zielzeitmodus wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Release) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64,
GCC 12.2, SDL 3.2.30, X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Der Katalog enthält 509 Prüfungen ohne Fenster (498 ohne SDL) und 45 Fensterabläufe.

Die gezielte Runnerprüfung besteht auf macOS:
`build/series-target-mac/test-results/run-ihh0h_f7`. Ein analytisches Translationsmodell
in C und Physim vergleicht alle Messungen exakt. Ziele vor dem ersten Rasterpunkt,
zwischen Punkten und nach mehreren Punkten sind geprüft; ein 0,05-s-Endintervall
endet bei 0,25 s trotz konfiguriertem 0,08-s-Minimum. Ein zu kurzes Budget erhält
Anfang und akzeptierten Punkt als lesbaren Präfix ohne gültigen Footer.
Ungültige Ziele und Verwendung im interaktiven Modus werden vor dem Start abgewiesen.

Die Serienprüfung besteht auf macOS:
`build/series-target-mac/test-results/run-8bhsi5w5`. Seedabhängige Anfangswerte,
seriale/parallelisierte Ausführung, C-/Physim-Daten, Rohendwerte und Berichtsdaten
werden verglichen. Velocity-Sweep, abgeschnittenes Endintervall, fehlender adaptiver
Callback, Budgetfehler und Abbruch mit Prozessbereinigung sind geprüft. Ein eigener
Runner schreibt CRC-gültige Dateien mit falschem Ziel, doppelten Zeiten, zu vielen
Samples, falscher Konfiguration, verletzten Schrittgrenzen, falschem Start oder
fehlendem Footer. Der Controller weist diese Dateien ab und schreibt keinen Gesamtbericht.

Die unveränderten Runner-/Parallelprüfungen bestehen auf macOS 4/4:
`build/series-timing-mac/test-results/run-muc2qsb9`; die bisherigen festen
Pendel-/Integratorreferenzen bestehen zusätzlich:
`build/series-timing-mac/test-results/run-3n83g_ih`.
Die bisherigen Serien-/Adaptive-Bedienabläufe bestehen 2/2:
`build/series-timing-mac/test-results/run-rtxdyaqw`.

Der neue Fensterablauf besteht auf macOS:
`build/series-timing-mac/test-results/run-yyf2512r`. C- und Physim-Pendelprojekte
aktivieren adaptive Serien über echte Widgets, geben zunächst das ungültige Ziel
0 und danach `7e-1` über SDL-Text-/Tastaturereignisse ein und vergleichen die Längen
0,5/1,5/2,5 m. Alle überlappenden Zeiten und Kanalwerte stimmen exakt überein;
die Punktzahlen unterscheiden sich nach Pendellänge. Zwei weitere Prozesse öffnen
den archivierten Bericht ohne Build/Runner. Dateihashes bestätigen unveränderte Daten.
Der erste Wiederöffnungstest wartet irrtümlich in der Messlaufansicht auf den Bericht;
die Prüfung wählt nun vor dem Öffnen die Berichtansicht.

Der erste Debian-Build stoppt wegen eines vollen 16-GiB-Datenträgers. Alte generierte
Arbeitsordner früherer Prüfungen unter `build/native/Release/test-results` werden
entfernt; die zwei neuesten vollständigen Durchläufe und sämtliche strukturierten
Ergebnisberichte bleiben erhalten. Dieser Buildabbruch wird nicht als bestandener
Testdurchlauf gezählt.

Die abschließende gezielte macOS-Release-Prüfung besteht 4/4:
`build/series-final-mac/test-results/run-ankpqg54` (Runner, Serien, Bericht und
unveränderter mathematischer Sprachfehlerfall). Die neuen Modellprüfungen bestehen
auch im Debug-Build 2/2: `build/series-debug-mac/test-results/run-obl8dd0u`;
der adaptive Serien-Fensterablauf mit aktiven Assertions besteht zusätzlich:
`build/series-debug-mac/test-results/run-pve22knr`.

Der finale Export-/Bedienvergleich besteht auf macOS 2/2:
`build/series-display-final-mac/test-results/run-fi5w1rhl`. PNG-Dateien mit
1200 × 850 Pixeln und SVG-Dateien werden für beide Modelle erzeugt und geprüft.
Die PNG-Darstellung wurde außerdem visuell kontrolliert. Parameter besitzen in
der aktuellen API keine formale Einheit; die X-Achse zeigt deshalb `length`
ohne erfundenes `[1]`. Die Kanalachse behält `angle [rad]`. Dieselbe Regel gilt
für die App und beide Exportformate.

Der erste macOS-Gesamtlauf ohne Fenster besteht 508/509; der unveränderte
Sprachfehlerfall `language_native_mathpowdomain` überschreitet seine Zehn-Sekunden-Grenze.
Die gezielte Wiederholung besteht mit derselben Grenze. Der erste vollständige
Mac-Fensterlauf besteht 43/45. Der Workspace-Prüfer wechselte das Projekt,
bevor sein Bibliotheksworker beendet war; er wartet nun wie die anderen
Bedienprüfer auf den tatsächlichen Abschluss. Der gezielte Fall besteht mit
seiner bisherigen Grenze: `build/series-display-final-mac/test-results/run-omhe2m67`.
Der zweite Fehler entstand, weil der laufende Prüfer bereits neue Exportdateien
verlangte, sein zuvor gebautes Testprogramm sie aber noch nicht erzeugte.
Der abschließende Exportvergleich verwendet das aktualisierte Programm.

Die erste SDK-Prüfung mit SVG-Export erkennt im Prüfer eine doppelte Ausgabe:
installierter und neu gebauter Verbraucher schrieben denselben Dateinamen.
Der Exporter lehnt bestehende Dateien korrekt ab. Beide Verbraucher verwenden
jetzt getrennte Ausgaben. Das finale verschobene macOS-SDK besteht vollständig:
`build/series-sdk-corrected-proof-mac/Native SDK ä npnkp6da`. Vorgebauter Pendel,
neu gebaute C-Quellen und Physim-Quellen erzeugen je drei adaptive Läufe mit
exakter gemeinsamer Endzeit. Beide Bibliotheksverbraucher prüfen variable
Punktzahlen, Energie, Rohendwerte, Berichtskurven und SVG-Beschriftung.

Das finale verschobene Debian-SDK besteht ebenfalls vollständig:
`build/series-sdk-final-proof-linux/Native SDK ä goh054ga` innerhalb der VM.
Die Zielzeitstudien laufen für alle drei Modulwege durch beide Verbraucher;
Header, Beispielmodule, native Projekte und Sprachmodule werden unabhängig
gegen die installierte und die aus SDK-Quellen neu gebaute Bibliothek geprüft.

Der vollständige finale macOS-Fensterlauf besteht 45/45:
`build/series-display-final-mac/test-results/run-2pue_3pu`.
Der vollständige Debian-Lauf ohne Fenster besteht 509/509:
`build/series-target/test-results/run-oj_64_2b` innerhalb der VM.
Nach den abschließenden Änderungen an Einheitenanzeige und Exportprüfern bestehen
Runner, Serien und Bericht unter Debian nochmals 3/3:
`build/series-target/test-results/run-xo552x2v`.

Der erste vollständige Debian-Fensterlauf besteht 44/45:
`build/native/Release/test-results/run-xqni5_ig`. Der neue Zielzeitablauf besteht;
der unveränderte Geschwindigkeitstest verfehlt bei parallelen SDK-/Buildarbeiten
seinen Mindestfortschritt (1,36 statt mehr als 1,4 simulierte Sekunden).
Die Folgeprüfung läuft nach Abschluss dieser Zusatzarbeiten mit denselben
Zeit- und Fortschrittsgrenzen.

Ein anschließender Prüfaufruf ohne Xvfb scheitert bei allen 45 Fensterfällen
bereits am SDL-Start (`No available video device`):
`build/native/Release/test-results/run-ioa037qk`. Dieser Aufruffehler liefert
keinen Nachweis zum App-Verhalten. Die finale Wiederholung verwendet wieder
Xvfb, Openbox, X11 und Mesa.

Der abschließende vollständige macOS-Release-Lauf ohne Fenster besteht 509/509:
`build/series-final-mac/test-results/run-6kqmayn0`. Er enthält auch die zuvor
zeitüberschrittene Sprachprüfung mit ihrer unveränderten Grenze und beide
neuen Zielzeitprüfungen.

Der finale vollständige Debian-Fensterlauf mit Xvfb/Openbox besteht 45/45:
`build/native/Release/test-results/run-xx5p4wra` innerhalb der VM. Der zuvor
knapp fehlgeschlagene Geschwindigkeitstest und der vollständige neue
C-/Physim-Zielzeit-/Export-/Wiederöffnungsablauf bestehen mit unveränderten
Prüfgrenzen. Damit liegen für diese Änderung auf beiden ausgeführten Systemen
509/509 Prüfungen ohne Fenster, 45/45 Fensterabläufe und die verschobenen
SDK-Verbraucher vor. Das belegt keine zusätzlichen Plattformen oder Wayland.

## Adaptive Simulationsschritte am 5. Oktober 2026

Die adaptiven Schritte wurden auf dem lokalen Intel-Mac (macOS 14.6.1,
Apple Clang 16, SDL 3.2.30, Release) und in der Debian-12-VM (Linux
6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, X11/Xvfb/Openbox, Mesa 22.3.6)
gebaut und ausgeführt. Der Katalog enthält 507 Prüfungen ohne Fenster,
davon 496 ohne SDL, und 44 Grafik-/Fensterabläufe.

Die Numerikprüfung bestätigt den ersten akzeptierten Dormand–Prince-Schritt,
verworfene Versuche, analytische Exponentialreferenzen, Rückwärtsintegration,
Versuchs-/Mindestschrittgrenzen und unveränderten Zustand bei Fehlern.
Der Runnerprüfer verwendet denselben analytischen Oszillator in C und Physim.
Alle 201 Offline-Referenzmessungen stimmen in sämtlichen Kanälen exakt überein;
variable Intervalle und Verwerfungen sind ausdrücklich erforderlich. Jeder
Zeitpunkt wird außerdem gegen Sinus/Cosinus geprüft. Interaktive Läufe mit
0,5×, 4×, Offline und verzögertem Leser werden kanalweise mit unabhängigen
Offline-CLI-Läufen gleicher Länge verglichen. Pause, Einzelschritt und Live-
Geschwindigkeitswechsel erhalten dieses Raster. Ungültige Dauer, Vorschlag,
veränderte Hostzeit und fehlende ABI-Tailgröße beenden den Lauf, ohne einen
weiteren Messpunkt oder gültigen Footer zu schreiben.

Die unveränderten festen Referenzabläufe und das mit eingefrorenem ABI-3-Header
gebaute Modul bestehen zusätzlich auf macOS 4/4:
`build/adaptive-mac/test-results/run-lrdo8mlx`.
Die erste neue Numerikprüfung besteht; der erste Runnerdurchlauf erkennt einen
zu kurzen Referenzlauf für Offline-Geschwindigkeit. Der Prüfer erzeugt nun eine
unabhängige CLI-Referenz mit der tatsächlichen gespeicherten Punktzahl und vergleicht
weiterhin sämtliche Punkte exakt. Er besteht:
`build/adaptive-mac/test-results/run-f_qfh42l`.
Projektdatei und Zeitkonto bestehen 2/2:
`build/adaptive-mac/test-results/run-u60c_w6m`.
Neue Sprachwerte, Callbackdiagnosen und generierte Referenz bestehen 3/3:
`build/adaptive-mac/test-results/run-8d9tm97z`.

`adaptive_workflow` bedient Auswahl, Einzelschritt, Pause, Reset und Stop in
C- und Physim-Pendelprojekten. Reset reproduziert Zeit und alle Werte des ersten
akzeptierten Schritts. Gespeicherte Grenzen und variable Zeiten werden geprüft;
alle überlappenden C-/Physim-Messungen stimmen exakt überein. Zwei weitere Prozesse
öffnen die archivierten Szenen über die Laufbibliothek und bedienen Rückblick,
Schritt und Wiedergabe ohne Modellstart. Hashes bestätigen unveränderte Dateien.

Der korrigierte vollständige Debian-Release-Lauf besteht 507/507 ohne Fenster:
`build/adaptive-final/test-results/run-bhf2hhku` innerhalb der VM.

Der korrigierte vollständige macOS-Release-Lauf besteht 507/507 ohne Fenster:
`build/adaptive-final-mac/test-results/run-5akn2oys`. Der vollständige
Fensterdurchlauf mit geprüften Texteingaben besteht 44/44:
`build/adaptive-ui-final-mac/test-results/run-76aj8pe8`.
Nach Ergänzung derselben Texteingabe für die Startdauer erkennt der adaptive
Bedienfall eine beim Umschalten versehentlich zusätzlich angelegte Layoutzeile.
Die doppelte Zeile ist entfernt; der gesamte adaptive Ablauf besteht mit dem
finalen Editor auf macOS erneut:
`build/adaptive-editor-final-mac/test-results/run-rn_c1kdu`.

Die abschließenden vollständigen Fensterläufe mit dem finalen Start- und
Grenzeditor bestehen je 44/44: macOS
`build/adaptive-editor-final-mac/test-results/run-s75ualf3`, Debian
`build/native/Release/test-results/run-5kcxf9rh` innerhalb der VM.
Die Startdauer, wissenschaftliche Mindestdauer und Höchstdauer sind bei
1080 × 740 logischen Pixeln auf macOS visuell geprüft (2160 × 1480 physische Pixel).

Die erste Sichtprüfung bei 1080 × 740 logischen Pixeln zeigt einen abgeschnittenen
Erklärungstext und als 0.0000 gerundete Mindestschritte. Wissenschaftliche
Texteingaben ersetzen die Zahlenwidgets für Startdauer und beide Grenzen. Der erweiterte Ablauf
schreibt über echte SDL-Tastatur-/Text-Ereignisse zunächst den unvollständigen
Wert `1e-`, verlangt verhinderten Start und schreibt danach `1e-6`.
Der erste erweiterte Prüflauf liest den Wert zu früh vor Verarbeitung im Widget;
ein Frame Abstand, wie bei den bisherigen Dokumenttests, prüft den tatsächlich
verarbeiteten Zustand. Frühere Prüfläufe werden nicht als finale Abnahme gezählt.


Der erste macOS-Gesamtlauf besteht 504/507, der erste Debian-Gesamtlauf 505/507.
Zwei Tutorialprüfungen erkennen in beiden Läufen den fehlenden `NULL`-Tail in den
abgedruckten C-Descriptoren; die Blöcke entsprechen jetzt wieder exakt ihren
getesteten Quelldateien. Der nachträglich ergänzte macOS-Metadatenfall zeigt am
frühen Runner tatsächlich eine Datei ohne Adaptive-Modus und Modulidentität,
wenn das Modell den Textpuffer füllt. Der Runner prüft jetzt Platz für beide
Provenienzblöcke und weist den Start vor dem Anlegen der Datei ab; die neue
Prüfung verlangt den Fehlercode und eine nicht vorhandene Datei.

Der Debug-Modelllauf besteht 4/4 auf macOS:
`build/adaptive-debug-mac/test-results/run-tp7w9y7v`; der vollständige adaptive
Fensterablauf mit Eingabe und aktiven UI-Assertions besteht zusätzlich:
`build/adaptive-debug-mac/test-results/run-yae_4cau`.

Eine weitere Compilerprüfung zeigt, dass ein generisch deklarierter
`adaptiveStep<T>` im frühen Emitter stillschweigend nicht als Callback exportiert
wird. Der Compiler weist diese Deklaration jetzt ausdrücklich ab. Der abschließende
Sprach-/Runnervergleich besteht auf macOS 3/3:
`build/adaptive-callback-final-mac/test-results/run-t2wni63b`.


Der abschließende Sprach-/Runnervergleich besteht auch unter Debian 3/3:
`build/adaptive-callback-final/test-results/run-h_3b3l8m` innerhalb der VM.
Die finalen verschobenen SDKs bestehen auf beiden Plattformen: unabhängige Header,
installierte und aus SDK-Quellen neu gebaute Bibliothek, C- und Physim-Module und
native Projekte. Zusätzlich laufen je 501 adaptive Pendelmessungen aus einem
vorgebauten C-Modul, neu gebauten C-Quellen und Physim-Quellen; tatsächliche variable
Zeiten und Energie werden geprüft, anschließend Analysen in beiden Sprachen
und erneut die Verbraucherprobe gegen die neu gebaute Bibliothek. macOS:
`build/adaptive-sdk-final-proof-mac/Native SDK ä yp9sjtu8`; Debian:
`build/adaptive-sdk-final-proof-linux/Native SDK ä ysgopwad` innerhalb der VM.
Die SDK-Prüfung verwendet hier keine GUI-Abläufe; deren Nachweise stehen separat.

## Szenenhierarchie und Szenenversionen am 5. Oktober 2026

Die Szenenhierarchie wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Release) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64,
GCC 12.2, SDL 3.2.30, Release, X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Der Katalog enthält 503 Prüfungen ohne Fenster, davon 492 ohne SDL, sowie
43 Grafik-/Fensterabläufe.

Die vollständigen Release-Läufe ohne Fenster bestehen je 503/503:
macOS `build/hierarchy-final-mac/test-results/run-kzr13_49`,
Debian `build/hierarchy-final/test-results/run-bz0zd13y` innerhalb der VM.

Die vollständigen Grafik-/Fensterläufe bestehen je 43/43:
macOS `build/hierarchy-mac/test-results/run-kjhsbh2v`,
Debian `build/native/Release/test-results/run-p0pebaqx` innerhalb der VM.

Die gezielten Modellprüfungen bestehen auf macOS 3/3:
`build/hierarchy-mac/test-results/run-b92wlh8h`. Gruppen, fehlende Eltern,
Selbstbeziehungen, Zyklen, atomare Fehler, Umsortierung, Reparenting und geerbte
Sichtbarkeit sind geprüft. Der maximale neue Snapshot mit 32 Einträgen,
96 Punkten und 16 Kanälen hat 8088 Bytes. Eine handgebaute versionierte alte
Szenendatei wird gelesen; gekürzte aktuelle Payloads werden nicht umgedeutet.

`runner_hierarchy` kompiliert ein C-Modul, dasselbe Modell in Physim und ein
Legacy-Modul mit dem eingefrorenen ABI-3-Header aus `8d69e4e`. Letzteres belegt
seine bisherigen Objekt-Padding-Bytes absichtlich mit 0xa5. Der aktuelle Runner
ignoriert diese Bytes ohne Hierarchie-Capability. Alle 21 Messungen stimmen
zwischen den drei Modulen genau überein; kanonische Snapshot-CRCs bestätigen
dieselben C-/Physim-Hierarchien und nach Entfernen der Gruppen dieselbe alte Geometrie.
Der Fehlerprüfer des direkten Testläufers ist erneut ausgeführt:
`build/hierarchy-harness-final/test runner ä ho03opvy` auf macOS.

`hierarchy_workflow` bedient die echten Widgets mit C- und Physim-Pendelprojekten.
Gruppen einklappen/ausblenden, Untergruppe ausblenden und Auswahl mit erneutem
Aufklappen werden gegen Snapshot-CRC und unveränderte pausierte Zeit geprüft.
Nach Stop liest die App die gespeicherte Hierarchie. Zwei weitere Prozesse öffnen
handkonvertierte Version-1-Szenen derselben Versuche über die Laufbibliothek,
zeigen sie als flache Szenen und bedienen Rückblick, Schritt und Wiedergabe.
Dateihashes bestätigen unveränderte Originale und Legacy-Dateien; Modellcode
läuft beim Wiederöffnen nicht.

Die verschobenen SDKs bestehen auf beiden Plattformen. Die Verbraucherprobe
baut Gruppen und Elternbeziehungen, weist einen Zyklus ab und liest den neuen
Snapshot wieder. Sie läuft gegen die installierte Bibliothek und erneut gegen
die aus SDK-Quellen gebaute Bibliothek. Öffentliche Header werden einzeln
kompiliert; gebündelte und neu gebaute C-Vorlagen sowie Physim-Experimente und
Analysen werden ausgeführt. macOS:
`build/hierarchy-sdk-final-proof-mac/Native SDK ä g9yaqnzw`; Debian:
`build/hierarchy-sdk-final-proof-linux/Native SDK ä g0x_bsmp` innerhalb der VM.

Die ersten vollständigen Läufe ohne Fenster erkennen auf beiden Systemen sechs
veraltete Testannahmen: fünf erwarten Sprachversion 0.167.0, eine erwartet die
Pendelgeometrie ohne die zwei neuen Gruppen. Die korrigierten Prüfungen verlangen
exakt 0.168.0 und prüfen zusätzlich die Beziehungen; die bisherigen numerischen
Referenzvergleiche bleiben erhalten. Alle sechs Fälle und die Referenzprüfung
bestehen anschließend je 7/7: macOS `run-8uflbb9y`, Debian `run-pzqgvpxp`.

Der erste Fensterdurchlauf besteht auf macOS 38/42 und auf Debian 41/42.
Beide erkennen dieselbe veraltete Pendel-Szenenanzahl im vollständigen Sprachablauf.
Auf macOS schlagen außerdem drei Dokument-/Workspace-Abläufe fehl. Der zusätzliche
Fall `documents_input_isolation` reproduziert einen verfehlten Dateiklick durch
native Maus-/Fokusereignisse gezielt ohne Eingabeisolation und bestätigt den
vollständigen Dokumentablauf mit Isolation in beiden Fenstergrößen. Er prüft
auch die tatsächlich gespeicherten Unicode-Texte, Sicherung und externe Änderung.
Die Isolation gilt nur für geskriptete Prüfungen; native Dialogtests behalten ihre
Desktop-Eingabe. Die fünf betroffenen macOS-Abläufe bestehen danach 5/5
(`run-vpp8_ufy`), der zusätzliche Störtest 1/1 (`run-wz1ix52a`). Die Zwischenläufe
werden nicht als bestandene Gesamtabnahme gezählt.

Die auf- und zugeklappte Hierarchie sowie ausgeblendete Nachfahren wurden bei
1080 × 740 logischen Pixeln auf macOS visuell geprüft (2160 × 1480 physische Pixel).
Die organisatorischen Beziehungen belegen keine vererbten Koordinatentransformationen.

Der vollständige Hierarchie-Fensterablauf besteht zusätzlich im macOS-Debug-Build
mit aktiven Nuklear-Assertions: `build/workspace-check/test-results/run-4532_de3`.

## Docking und gespeicherte Panelanordnung am 5. Oktober 2026

Das Docking-Grundsystem wurde auf dem lokalen Intel-Mac (macOS 14.6.1,
Apple Clang 16, SDL 3.2.30, Release) und in der Debian-12-VM
(Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, Release,
X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Die vollständigen Tests ohne Fenster bestehen mit 501/501:
macOS `build/timeline-release-mac/test-results/run-ty4lwbk4`,
Debian `build/native/Release/test-results/run-sndxecqo` innerhalb der VM.
Der gemeinsame Katalog enthält jetzt 501 Prüfungen ohne Fenster, davon
490 ohne SDL, und 41 Grafik-/Fensterabläufe.

Der vollständige Grafik-/Fensterdurchlauf besteht auf beiden Systemen mit 41/41:
macOS `build/docking-ui-final/test-results/run-p75wze89`,
Debian `build/native/Release/test-results/run-viu8navp` innerhalb der VM.
Das umfasst Editor, Autosaves, Wiederöffnung, beide Fenstergrößen, Menübedienung,
Diagramme, Serien, Reset, Geschwindigkeit, Zeitleiste sowie alle vollständigen
Physim-Projekte mit Analyse und Export. Die Anordnung bei 1080 × 740 wurde
visuell geprüft; der Mac liefert dabei 2160 × 1480 physische Pixel, Xvfb 1080 × 740.

Das Modell prüft Split-/Tab-/Float-/Hide-Übergänge, einen vollständig leeren
Panelbaum, Wiederanzeigen sowie tausend weitere gültige oder transaktional
abgewiesene Anordnungsänderungen. Einstellungen prüfen zusätzlich Format-1-
und Format-2-Migration, den vollständigen Baum, freie Rechtecke, Tabs,
240 Kürzungen und Byteänderungen sowie einen ungültigen Baum mit gültiger CRC.
Fehler erhalten sowohl die bestehende Datei als auch bereits geladene Einstellungen.

Die abschließenden Modell-/Einstellungsprüfungen bestehen je 2/2:
macOS `build/docking-ui-final/test-results/run-8it06dng`,
Debian `build/native/Release/test-results/run-pbtygbx4` innerhalb der VM.
Die Fehlerbehandlung des nativen Prüfers ist ebenfalls erneut ausgeführt:
`build/docking-harness-final/test runner ä bqu5b3m6` auf macOS.

`docking_workflow` bedient echte Titelzeilen, Ziele, Tabs, Größenänderungsgriffe,
Schließen, Menüaktionen, Escape und die Mindesthöhen einer verschachtelten
Teilung bei stark vergrößerter Nachbarfläche. Zwei Prozesse bauen C- und Physim-Versuche,
ordnen die Panels während weiterlaufender Simulation um und prüfen erhaltenen
ungespeicherten Quelltext über CRC, Zeitfortschritt und die danach vollständig
lesbaren Messdaten. Der Einstellungsweg stellt das ausgeblendete Protokoll wieder
her und prüft die unmittelbar gespeicherte Anordnung. Drei weitere Prozesse
prüfen Speicherung, Neustart/Wiederherstellung, erneutes Andocken, Zurücksetzen
und den Erhalt einer beschädigten Einstellungsdatei. Frei platzierte Panels
bleiben im Hauptfenster; diese Prüfung belegt keine separaten Betriebssystemfenster.

Die drei abschließenden macOS-Debug-Prüfungen von Diagrammen, Serien und
Docking bestehen mit aktivierten Nuklear-Assertions: `build/workspace-check/test-results/run-55oi60e7`.

Nach dem lesbaren Schließen-Knopf und der Begrenzung verschachtelter Teilungen
besteht der komplette Docking-Ablauf erneut auf beiden Systemen: macOS Debug
`build/workspace-check/test-results/run-ru22fbmp`, Debian Release
`build/native/Release/test-results/run-z6s33n7k` innerhalb der VM.

Zwischenläufe deckten eine verdeckte freie Panelfläche, die erst im nächsten
Frame aufgebaute Menüansicht und verbrauchte Mausrad-/Ziehereignisse auf.
Die korrigierte Reihenfolge und Eingabezuordnung bestehen die unveränderten
Diagramm-, Serien- und Zeitleistenprüfungen im vollständigen Enddurchlauf.
Die Zwischenläufe werden nicht als bestandene Gesamtabnahme gezählt.

## Zeitleiste und Szenenaufzeichnung am 5. Oktober 2026

Die Zeitleiste wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Debug und Release) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64,
GCC 12.2, SDL 3.2.30, Release, X11/Xvfb/Openbox, Mesa 22.3.6) gebaut und ausgeführt.
Die vollständigen Release-Läufe bestehen mit 500/500:
macOS `build/timeline-release-mac/test-results/run-97k6w7xp` und
Debian `build/native/Release/test-results/run-qg1essmn` innerhalb der VM.

Zwölf gezielte Prüfungen von Kern, Snapshotcodec, Streaming, Mutationen, Datenreihen,
Berichten, Taktung, Szenenaufzeichnung und echtem Projektbuild bestehen zuvor auf
beiden Plattformen: macOS `run-3nrdim5a`, Debian `run-o7fqbagl`.
Die C- und Physim-Runner liefern mit und ohne `--record-scenes` dieselben 201
Messungen aller Referenzkanäle. Die optionalen Blöcke enthalten die zugehörigen
Werte und komplette Geometrie; Anfang/Ende, Footerzählung, Legacy-Lesen,
ungültige Snapshotversionen/Szenen und CRC-/Tail-Recovery sind geprüft.
Über 10.000 Vorschauzustände bestätigen die Speichergrenze und erhaltene Endpunkte.

Der gemeinsame Fensterdurchlauf besteht je 9/9 mit Zeitleiste, Geschwindigkeit,
Reset, Themen, Projekteinstellungen, beiden Menügrößen und vollständigen
C-/Physim-App-Abläufen einschließlich Analyse, Export und Wiederöffnung:
macOS `run-44gk6k1o`, Debian `run-pf_6612g`.
Die abschließenden Prüfungen von Zeitleiste, Themen und vollständigem Physim-Ablauf
bestehen je 3/3: macOS `run-iyoym0pp`, Debian `run-uety5xa_`.
Nach der abschließenden Leseränderung, die für beide Durchgänge dieselbe geöffnete
Datei hält, bestehen Zeitleiste, Reset und vollständiger Physim-Ablauf erneut
je 3/3: macOS `run-fuvn_qum`, Debian `run-9hjimzg5`.
Der Zeitleistenfall startet zehn Prozesse für C-/Physim-Projekte sowie archivierte,
alte, alte mit negativen Zeitstempeln und rekonstruierte Dateien. Reale Widgets
und Tastaturereignisse prüfen Rückblick bei weiterlaufendem Versuch, Klick/Ziehen,
Vor/Zurück, Wiedergabe/Leertaste, Live und Reset. Kanonische Snapshot-CRCs und
Dateihashes bestätigen unveränderte Zustände und Originaldateien. Die Darstellung
bei 1080 × 740 ist visuell geprüft.

Die verschobenen SDKs sind auf beiden Systemen verifiziert: öffentliche Header
einschließlich `snapshot.h` einzeln kompilieren, gegen die installierte und aus
den gelieferten Quellen neu gebaute Kernbibliothek linken sowie gebündelte und
neu gebaute C-Vorlagen und Physim-Experimente/Analysen ausführen. macOS:
`build/timeline-sdk-proof-mac/Native SDK ä qn7i69nr`; Debian:
`build/timeline-sdk-proof-linux/Native SDK ä x_vuw3n3` innerhalb der VM.

Der erste Debug-Gesamtlauf auf macOS besteht 498/500. Neben dem bereits bekannten
Signal-9-Abbruch von `language_function_values_runtime` erkennt die Referenzprüfung
den noch nicht eingetragenen neuen öffentlichen Header. Modulbeschreibung,
Funktionsbeschreibungen und Offline-Navigation sind ergänzt; die korrigierte
Referenzprüfung besteht. Der vollständige Release-Lauf enthält beide Prüfungen
und besteht. Die lokale Debug-Systempolitik bleibt eine Einschränkung.

Die [C17-CI zu `a714459`](https://github.com/PhysicSimulator/physim/actions/runs/37281091082)
und [Paket-CI](https://github.com/PhysicSimulator/physim/actions/runs/37281091099)
sind fehlgeschlagen. Der alte Isolationstest verlangt nach RUN sofort Fortschritt;
dieser Fehler ist lokal in Debian reproduziert. Der aktualisierte Test trennt
Zustandsbestätigung und echten Fortschritt und prüft anschließend eine zeitstabile
Pause. Er besteht auf beiden Plattformen, einschließlich der Release-Gesamtläufe.
Ein neuer CI-Nachweis für diesen Stand steht bis zum Push aus.

## Simulationsgeschwindigkeit am 5. Oktober 2026

Die Geschwindigkeitswahl und das feste Echtzeitkonto wurden auf dem lokalen
Intel-Mac (macOS 14.6.1, Apple Clang 16, SDL 3.2.30, Debug) und in der Debian-12-VM
(Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, Release, X11/Xvfb/Openbox,
Mesa 22.3.6) gebaut und ausgeführt.

Die sechs gezielten Prüfungen bestehen auf beiden Plattformen: `pacing`,
`project_file`, `scene_protocol`, `parameters_api`, `runner_pacing_c` und
`runner_pacing_phys`. macOS: `build/workspace-check/test-results/run-al6_mguy`;
Debian: `build/native/Release/test-results/run-1kw0782u` innerhalb der VM.
Die C- und Physim-Zufallsmodelle vergleichen jeweils 201 Messpunkte aller Kanäle
mit dem unverändert offline ausgeführten CLI-Referenzlauf. 0,5×, 4×, Offline und
ein absichtlich verzögerter Pipe-Leser ergeben dieselben Zeiten und Werte.
Die tatsächlichen Laufzeiten werden verglichen; der feste `dt` und der Seed
bleiben erhalten. Geschwindigkeit während Betrieb/Pause, Einzelschritt,
ungültige CLI-/Wire-Werte und ein fehlender Handshake sind geprüft.
Bei `dt=1`, Faktor 0,1 meldet RUN sofort den unpausierten Zustand bei Zeit 0;
der folgende pausierte Einzelschritt erreicht unmittelbar Zeit 1.
Das künstliche Zeitkonto prüft zusätzlich lange Verzögerungen und begrenztes Aufholen.

Der gemeinsame App-Durchlauf besteht jeweils 8/8: Geschwindigkeit, Reset,
Projekteinstellungen, Themen, beide Menügrößen und vollständiger C-/Physim-Ablauf
mit Analyse, Export und Wiederöffnung. macOS: `run-wwivzhlo`; Debian: `run-78d35ayu`.
`speed_workflow` bedient die tatsächlichen Widgets in beiden Sprachen bei
1080 × 740 und prüft 0,25×, 4×, Wechsel auf 1× im laufenden Betrieb, Offline,
Pause, Einzelschritt, Reset und Wiederöffnung mit gespeicherter Geschwindigkeit.
Die Darstellung ist visuell geprüft; Messdateiformat und Modul-ABI bleiben gleich.
Nach der sofortigen RUN-Rückmeldung besteht der abschließende Durchlauf mit
Geschwindigkeit, Reset und beiden vollständigen App-Abläufen auf beiden Plattformen
(je 4/4): macOS `run-1j7s4k_5`, Debian `run-a7ock72l`.
Ein macOS-Zwischenlauf endete im internen Testfristpfad; ein zusätzlicher
Debian-Durchlauf wurde durch die nicht mehr auf SSH-Befehle antwortende VM unterbrochen.
Der erneute gemeinsame macOS-Lauf und der Debian-Lauf nach VM-Wiederherstellung
sind die abschließenden Nachweise. Die unterbrochenen Läufe werden nicht als bestanden gezählt.

Die [C17-CI zu `dec629f`](https://github.com/PhysicSimulator/physim/actions/runs/37271795266)
und die [Linux-Paket-CI](https://github.com/PhysicSimulator/physim/actions/runs/37271795260)
sind inzwischen erfolgreich abgeschlossen. Diese Nachweise gelten für den
vorherigen Reset-Stand und enthalten die neue Geschwindigkeitswahl noch nicht.

## Simulationsreset am 4. Oktober 2026

Die Reset-Steuerung wurde auf dem lokalen Intel-Mac (macOS 14.6.1, Apple Clang 16,
SDL 3.2.30, Debug) und in der Debian-12-VM (Linux 6.1.0-53-cloud-amd64, GCC 12.2,
SDL 3.2.30, Release) gebaut und ausgeführt. Linux verwendet X11/Xvfb/Openbox und
Mesa 22.3.6. Das ist kein neuer Windows-, Apple-Silicon-, Wayland- oder GPU-Nachweis.

`reset_workflow` startet vier Prozesse mit eigenen Projekten bei 1080 × 740:
C- und Physim-Unsicherheitsmodelle, Physim-Stoßmodell mit Reibung 0,25 und ein
C-Hängefall. Der Seed 18446744073709551615 und dt 0,125 bleiben erhalten.
Reale SDL-Bedienereignisse prüfen Reset während des Laufs und der Pause,
Einzelschritt, Fortsetzen, Stoppen und F7 nach Laufende. Anfangswerte und erster
Zeitschritt stimmen mit den gespeicherten Referenzkanälen überein. Pro Projekt
bleiben vier erfolgreiche Läufe erhalten; CRCs bestätigen die alten Dateien,
Quellcode-Snapshots und Grenzdateien sind zusätzlich geprüft.

Fehlende Quellen und ein vorübergehend entferntes Modul melden Fehler und erhalten
Laufverweis, Messwerte, Live-Verlauf und Analyse. Der C-Prüffall hängt tatsächlich
in `step` und später in `create`; die App beendet beide nach der Stop-Frist.
Vollständige Messblöcke des Hängefalls werden mit `PS_RECOVERED` geladen.
Die erneute Initialisierung gelingt nach Beseitigen der Fehlerursache. Geänderte
Editorquellen verhindern einen Reset. Der pausierte Anfangszustand ist visuell
geprüft; Kamera und kleine Fenster bleiben bedienbar.

Der gemeinsame Durchlauf besteht jeweils 8/8: Reset, Projekteinstellungen, Themen,
Plots, beide Menügrößen sowie vollständiger C- und Physim-App-Ablauf einschließlich
Analyse, Export und Wiederöffnung. macOS: `build/workspace-check/test-results/run-iu0zd7di`;
Debian: `build/native/Release/test-results/run-ohc6zzby` innerhalb der VM.
Die abschließende Prüfung der verzögerten Übernahme von Kanalnamen und Statusschema
besteht zusätzlich mit Reset sowie beiden vollständigen App-Abläufen (3/3):
macOS `run-37nsr8sl`, Debian `run-d9ylbk38`.

Die [Linux-Paket-CI zu `2afef89`](https://github.com/PhysicSimulator/physim/actions/runs/37236029010)
besteht inzwischen auf Debian 12 und Ubuntu 24.04. Dieser Paketnachweis bezieht
sich auf den vorherigen Stand mit Themen und enthält noch keinen Reset.

## Darstellungswechsel am 4. Oktober 2026

Die neue helle und kontrastreiche Darstellung wurde tatsächlich auf dem lokalen
Intel-Mac (macOS 14.6.1, Apple Clang 16, SDL 3.2.30, Debug) und in der Debian-12-VM
(Linux 6.1.0-53-cloud-amd64, GCC 12.2, SDL 3.2.30, Release) gebaut und ausgeführt.
Debian verwendet X11/Xvfb/Openbox und Mesa 22.3.6. Das belegt keine Wayland- oder
physische GPU-Abnahme.

- `preferences` prüft die drei Paletten, Format-1-Migration, Grenzen, Prüfsumme,
  alle Kürzungen und Erhaltung bei Fehlern. Die neuen Text-, Syntax- und
  Kurvenlegendenfarben erreichen rechnerisch mindestens 4,5:1 auf Hell und 7:1
  auf Hoher Kontrast.
- `themes_workflow` startet je sieben App-Prozesse, bedient die echten Optionen
  und prüft Übernehmen, Abbrechen, Standardwerte und gespeicherte Neustarts.
  Der Wechsel erreicht das vorher geöffnete Dokumentationsfenster und bewahrt
  dessen Fonts. BMP-Ausgaben prüfen die Hintergrundfarbe in Editor, Einstellungen,
  Diagrammen mit drei Kurven und geladener Dokumentation bei 1080 × 740.
- Die bisherigen Abläufe für Einstellungen, Dokumentation, Diagramme, beide
  Menügrößen und kleine Dokumentansicht bestehen zusätzlich: macOS 8/8 mit
  UI-Rendererprüfung (`build/workspace-check/test-results/run-egjd2dld`),
  Debian 7/7 (`build/native/Release/test-results/run-soyj8nw8` innerhalb der VM).
  Die abschließende erweiterte Themenprüfung besteht separat auf macOS
  (`run-ba1lmh_4`) und Debian (`run-g_gz4yq3`). Die abschließende Menü- und
  Themenprüfung des fertigen Quellstands besteht jeweils 2/2:
  macOS `run-q5yc2y_x`, Debian `run-cek89xm5`. Der abschließende Kontrast-/Format-Test
  besteht auf macOS (`run-rflngm6f`) und Debian (`run-6b2meyq1`).
  macOS-Bilder wurden visuell geprüft.

Die Linux-Paket-CI des vorherigen Commits `fa311dc` besteht mittlerweile vollständig
auf [Debian 12 und Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/37234502965),
einschließlich der echten GTK-Dateidialoge. Diese CI enthält noch keine neuen Themen.

## Lokaler Intel-Mac am 4. Oktober 2026

macOS 14.6.1 (23G93), Intel x86_64, Apple Clang 16.0.0 und lokal aus
`release-3.2.30` gebautes SDL prüfen jetzt zusätzlich die Workspace-Dokumentansichten.
Der Debug-Modelltest besteht mit Format-1-Kompatibilität, maximalen Pfad-/Dokumentgrenzen,
abgeschnittenen und manipulierten Dateien sowie Erhaltung des bisherigen Zustands
bei Fehlern (`build/workspace-check/test-results/run-80qn96q7`). Die Fensterprüfung
startet getrennte App-Prozesse für Projekt-/Ordner-Wiederöffnung, aktive Dokumente,
Analyse-/Simulationsansicht, fehlende und extern gekürzte Unicode-Dateien sowie
vorhandene Autosaves. Dateibaum, beide Dokumentfenstergrößen, Buildinvalidierung
und Dokument-Autosaves sind zusätzlich geprüft. Diese lokalen Prüfungen sind kein
neuer Linux- oder Apple-Silicon-Nachweis. Alle sechs Fensterfälle bestehen unter
`build/workspace-check/test-results/run-sg5hhnt6`.

Der SDL-freie Gesamtlauf des Ausgangsstands `42d1f9e` besteht 480 von 482 Fällen
(`build/native/Debug/test-results/run-vsag2ez2`).
`language_function_values_runtime` wird vor der Programmausführung mit Signal 9
beendet; das macOS-Systemprotokoll nennt `AppleSystemPolicy` und
`Security policy would not allow process`. Eine separat ad hoc signierte Kopie
besitzt eine gültige Signatur, wird aber ebenfalls blockiert. Eine Ursache im
Sprachprogramm ist damit nicht nachgewiesen. `batch_reference` scheitert an der
Prüfung der abgeschlossenen Läufe bzw. der Fünf-Sekunden-Grenze; die Fehlerjournale
nennen Runner-Zeitüberschreitungen. Beide gezielten Wiederholungen scheitern erneut
(`run-6x4lxg2j`).

Beide Menüfenstergrößen scheitern in einem gemeinsamen lokalen Lauf; die jeweils
isolierten Wiederholungen bestehen (`run-t9h7c5ep`, `run-zij55d1b`). Eine separat
aus `42d1f9e` exportierte und gebaute App scheitert ebenfalls im kleinen Menütest
(`build/baseline-app/test-results/run-mozmzzn3`). Die wechselnden Fehlerstellen
und eine mögliche Beeinflussung durch native Fokus-/Mausereignisse bleiben offen.
Die bisherigen macOS-15-CI-Ergebnisse belegen keine Fehlerfreiheit auf macOS 14.6.1.

Die anschließende Diagnose zeigt beim Batchtest für alle drei Fehlerfälle
Zeitüberschreitungen nach ungefähr 0,25 Sekunden, einschließlich des normalen
Schemafixtures. Die Fehlerprüfungen lassen Absturz und Schemaprüfung nun fünf
Sekunden zum Starten, verlangen ausdrücklich `PS_IO` bzw. `PS_INVALID` und behalten
für den Hängefall 0,25 Sekunden und `PS_LIMIT`. Die bisherige Gesamtdauergrenze
von fünf Sekunden sowie die Abschlusszahlen bleiben geprüft. Alle drei Fehlerarten
bestehen (`build/native/Debug/test-results/run-bjamthli`); die parallele Batchreferenz
besteht separat (`run-mkysw5d1`).

Natürliche Maus-/Fokusereignisse sind im Menütest nach dem Hilfefenster protokolliert.
Absichtlich eingeschobene Ereignisse reproduzieren den Prüffehler unter
`build/toolbar-noise-before.log`. Die Teststeuerung isoliert jetzt ihre Mauskennung
auch in den Menüfällen. Mit zusätzlichen Maus-, Mausrad- und Fokusereignissen
bestehen die vollständigen kleinen und großen Menüabläufe, die normalen Menütests,
Plot-Eingabeisolation und Fensterstart (5/5 Fälle unter
`build/workspace-check/test-results/run-23gu5c67`). Der SDL-Eingabetest prüft weiterhin
die normale Freigabe gehaltener Tasten/Maustasten bei Fokusverlust.

Der anschließende vollständige lokale Debug-Build besteht 492 von 493 Tests ohne
Fenster (`build/workspace-check/test-results/run-7288kqxh`), einschließlich
Batchtests, nativen Projektbuilds und Benchmarks. Ausschließlich
`language_function_values_runtime` bleibt durch macOS-Systempolitik blockiert.
Dasselbe Sprachprogramm besteht lokal im Release-Build
(`build/native/Release/test-results/run-203vuguu`).

Die Linux-Paketprüfung zu `1b098d7` baut das Paket und besteht dessen Start sowie
SDK und alle neun GUI-Projektabläufe auf Debian 12 und Ubuntu 24.04. Beide Jobs
scheitern weiterhin am nativen Datei-/Ordnerdialog:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/37223668792/job/111500303931),
[Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/37223668792/job/111500303988).
Die Jobannotationen zu `68bb8a4` bestätigen auf beiden Distributionen, dass der
erste Ordnerdialog gefunden und bedient wird, aber vor der nächsten Dateianfrage
offen bleibt:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/37225049564/job/111504289877),
[Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/37225049564/job/111504289907).
Dieser CI-Lauf bestätigt die Dialogabnahme noch nicht; der folgende lokale
Nachweis prüft die anschließende Korrektur.

Der [C17-Lauf zu `68bb8a4`](https://github.com/PhysicSimulator/physim/actions/runs/37225049628)
besteht alle Build-, Test-, Sanitizer-, Fuzzer- und SDK-Schritte in den acht
Plattformkombinationen, einschließlich der jetzt 36 Grafik-/Fensterfälle.
Im Intel-macOS-Job scheitern anschließend beide Artefakt-Uploads mit
`ArtifactService/CreateArtifact`-Zeitüberschreitung. Der gesamte Job ist dadurch
fehlgeschlagen; seine Prüfungen sind bestanden, die Ergebnisarchive fehlen.

## Lokale Linux-Dialogprüfung am 4. Oktober 2026

Eine lokale QEMU-10.1.0-VM mit HVF auf dem Intel-Mac läuft mit Debian 12,
Linux `6.1.0-53-cloud-amd64` und GCC 12.2.0. SDL 3.2.30 und Physim sind dort
aus Quellen als Release gebaut. Der bisherige Dialogprüfer reproduziert den
CI-Hänger (`build/linux-vm/dialog-baseline.log`). Nach funktionierender Auswahl
zeigt die echte Abbruchprüfung einen App-Fehler: SDLs Zenity-Backend liefert
einen leeren ersten Pfadstring, den Physim als Ordnerpfad öffnet. Die App
behandelt diesen Rückgabewert nun wie eine leere Dateiliste als Abbruch;
Fehler mit einer NULL-Dateiliste bleiben als Fehler gemeldet.

Der korrigierte Prüfer verwendet einen privaten Home-/Konfigurationsbereich,
eine D-Bus-Sitzung und die tatsächlichen AT-SPI-Namen und Fensterkoordinaten der
GTK-Dialogelemente. XTest klickt diese Elemente; es werden keine SDL-Ergebnisse
oder Callback-Pfade eingespeist. Eine direkte Pfadeingabe mit schnellen
Unicode-Tastaturzuordnungen und die anfängliche Recent-Ansicht entfallen.

Das verschobene Release-SDK in `Installed package ä/Physim` besteht die tatsächliche
Ordnerauswahl, externe Datei, Zusatzordner, Abbruch mit unverändertem Workspace und
Status sowie den getrennten Test eines nicht vorhandenen Dialogtreibers:

| Umgebung | Dialogbibliotheken | Protokoll |
| --- | --- | --- |
| Debian 12 in der VM | Zenity 3.44.0, GTK 3.24.38, Mesa 22.3.6 | `build/linux-vm/debian-final-dialogs.log` |
| Ubuntu 24.04 im Container innerhalb derselben VM | Zenity 4.0.1, GTK 4.14.5, Mesa 25.2.8 | `build/linux-vm/ubuntu-final-dialogs.log` |

Dialogbilder, `actions.json`, das abschließende Workspace-Bild und
`unavailable.log` liegen unter `build/linux-vm/dialog-evidence`. Die App-Korrektur
baut zusätzlich lokal mit Apple Clang 16.0.0; Workspace-Dateibaum und
Workspace-Neustart bestehen auf macOS 14.6.1 (2/2 Fälle unter
`build/workspace-check/test-results/run-l9i4rk48`).

Diese Linux-Prüfungen verwenden X11/Xvfb und Mesa llvmpipe. Sie sind ein tatsächlich
ausgeführter Dialognachweis für die genannten GTK-Versionen, aber kein Test von
Portal-/Wayland-Dialogen oder realer Grafikhardware. Der erneute Paket-CI-Lauf
mit dem korrigierten Prüfer steht noch aus. Dessen zusätzliche AT-SPI-/Python-
Testabhängigkeiten werden erst nach dem Paketstart ohne Compiler/Python installiert.

## Windows-ClangCL-Sanitizer

Aktuell offener Fehler: Im
[Windows-ClangCL-Debug-Job zu `0321c85`](https://github.com/PhysicSimulator/physim/actions/runs/36504185474/job/109201756506)
bestehen alle 493 normalen Tests einschließlich der neuen Projektcache-Reparatur.
Die Sanitizer-Probe besteht ebenfalls; danach endet der instrumentierte Core-Test
mit Windows-Zugriffsverletzung `0xC0000005` ohne Diagnoseausgabe. Die übrigen neun
Sanitizer-Tests bestehen. Die Ursache dieses Absturzes ist noch ungeklärt;
der gesamte Job ist damit fehlgeschlagen. Das lokale Protokoll liegt unter
`build/ci-cache-clang-debug.log`. Der parallele
[Job desselben Commits im zweiten Repository](https://github.com/Lulus792/physim/actions/runs/36504182060/job/109201744452)
besteht alle zehn Sanitizer-Tests mit derselben Windows-Image-Version
`20260920.314.1`. Der Fehler tritt damit nicht in jedem Lauf auf; die Ursache
bleibt offen. Das Vergleichsprotokoll ist `build/ci-cache-clang-debug-origin.log`.

Lokal scheitert der Core-Test mit ClangCL 19.1.5 ebenfalls mit `0xC0000005`;
hier meldet die Laufzeit zusätzlich eine unbekannte Interceptor-Instruktion.
Der mit [Microsoft ProcDump](https://learn.microsoft.com/en-us/sysinternals/downloads/procdump)
erfasste Minidump zeigt die Fehleradresse in `clang_rt.asan_dynamic-x86_64.dll`
bei Offset `0x4859b`, innerhalb von `__asan_region_is_poisoned`.
Das ist noch kein Nachweis derselben Ursache wie im CI-Lauf ohne Diagnoseausgabe.
Protokolle: `build/clang-asan-core.log`, `build/clang-asan-core-verbose.log`;
Speicherabbild: `build/clang-asan-dumps/physim-test-core.exe_260929_030111.dmp`.

Die Windows-Debug-Jobs konfigurieren nun
[WER-Minidumps pro Testprogramm](https://learn.microsoft.com/en-us/windows/win32/wer/collecting-user-mode-dumps)
für `physim-test-core.exe` und einen eigenen Absturzprüfer. Der Prüfer verlangt
eine echte Zugriffsverletzung und denselben Fehlercode im erzeugten Minidump.
Das Artefakt `windows-crash-diagnostics-ClangCL` beziehungsweise
`windows-crash-diagnostics-v143` enthält Dumps, Testprogramm, PDB und Laufzeit-DLLs.
Die Aufnahme erfolgt während des ursprünglichen Testlaufs; fehlerhafte Tests
werden nicht automatisch wiederholt. Lokal sind der Prüfer kompiliert, sein
Absturz mit ProcDump erfasst und die Dump-Auswertung geprüft
(`build/crash capture check 1tq7fixm`). Im Lauf zu `620246b` bestehen die echte
WER-Aufzeichnung und alle zehn Sanitizer-Tests sowohl mit
[ClangCL](https://github.com/PhysicSimulator/physim/actions/runs/36506432739/job/109208791289)
als auch mit [MSVC](https://github.com/PhysicSimulator/physim/actions/runs/36506432739/job/109208791471).
Die Protokolle sind `build/ci-wer-clang.log` und `build/ci-wer-msvc.log`.
Der ursprüngliche sporadische Absturz gilt damit noch nicht als behoben.
Lokal wurden keine administrativen WER-Einstellungen verändert.

## Linux

**Ubuntu 24.04 mit GCC und Clang ist für die geprüften Arbeitsabläufe bestätigt.**
Die erfolgreichen Jobs für
[GCC](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553336)
und [Clang](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054552812)
prüfen den Quellstand `0a27cef` mit AddressSanitizer und UndefinedBehaviorSanitizer:

- 276 Tests ohne Fenster, einschließlich Sprachcompiler, Physik, Daten,
  Runner, nativen Projektbuilds und Fehlerbehandlung.
- 35 Grafik- und Fensterabläufe mit X11, Xvfb, Openbox und Mesa: darunter
  Menüwechsel, Tabs, Fensterknöpfe, Workspace, Dokumente, Wiederherstellung,
  Projekteinstellungen und C-/Physim-Projekte.
- Verschobenes installiertes SDK: Vorlagen und Sprachbeispiele neu bauen,
  Experimente und Analysen ausführen, Messdateien und Berichte prüfen.
- Acht vollständige C-App-Abläufe: Pendel, Wurf, Stoß, Bodenkontakt, Feder,
  unsicherer Wurf, Boxstoß und Auftrieb. Die Projektpfade enthalten Leerzeichen
  und Unicode. Der Ablauf umfasst Buildfehler, korrigierten Build, Simulation,
  Pause/Einzelschritt, Analyse, Exporte und Wiederöffnung gespeicherter Ergebnisse.

Nutzerprojekte werden direkt mit `physim-build` gebaut und benötigen keine
`CMakeLists.txt`. Quellen und `physim.project` bleiben im Projektordner;
Buildprodukte liegen in `build/Debug` beziehungsweise `build/Release`.

Das Release-Paket besteht inzwischen auch die unten beschriebene Prüfung auf
frischen Debian-12- und Ubuntu-24.04-Systemen. Wayland, weitere Distributionen,
reale Linux-Grafiktreiber und die Integration in einen echten Desktop bleiben offen.
Äußere Fensterecken hängen unter Linux vom Desktop ab; die Windows-DWM-Rundung
ist kein plattformübergreifender Nachweis.

### Release-Paket auf frischen Linux-Systemen

Der zusätzliche Workflow `linux-package.yml` baut ein Release-SDK mit GCC unter
Debian 12 (`debian:bookworm`) und prüft dort alle Tests ohne Fenster. Das Archiv
`Physim-linux-x86_64.tar.gz` wird anschließend in zwei neue Container entpackt:
Debian 12 und Ubuntu 24.04. Diese Prüfungen haben keinen Repository-Checkout,
keine SDL-Entwicklungsdateien und kein CMake. Zunächst werden nur Desktopbibliotheken
installiert und die Oberfläche ohne Compiler oder Python gestartet. Anschließend
kommen ein C17-Compiler für Nutzerprojekte und Python für die Teststeuerung hinzu.

Ein separates Prüfpaket enthält nur den SDK-Prüfer, dessen Compilerhelfer und
kleine Prüfquellen. Der Prüfer verwendet die Header, Bibliotheken, Vorlagen und
Kernquellen aus dem verschobenen SDK. Er baut alle C-/Physim-Beispiele und Projekte
und führt alle acht C-Vorlagen sowie den vollständigen Physim-Sprachablauf in der
Oberfläche aus. Die Systempakete, aufgelösten App-Bibliotheken, Laufprotokolle und
Screenshots werden als `installed-linux-debian-12` und `installed-linux-ubuntu-24.04`
archiviert. Das Release-Archiv heißt als CI-Artefakt `physim-linux-release-x86_64`.

Der [Debian-12-Release-Build zu `0dbe24f`](https://github.com/PhysicSimulator/physim/actions/runs/36505336425/job/109205387725)
besteht alle 493 Tests ohne Fenster und erzeugt das Paket. Die anschließenden
beiden Installationsprüfungen stoppen vor dem App-Start, weil Openbox Python
als Abhängigkeit installiert hatte. Openbox wird jetzt erst nach dem Starttest
installiert; die Bedingung, dass Python beim ersten Start fehlt, bleibt erhalten.
Im Lauf zu `620246b` besteht der App-Start ohne Compiler, Python und CMake in
beiden frischen Systemen:
[Debian 12](https://github.com/PhysicSimulator/physim/actions/runs/36506432788/job/109210313653)
und [Ubuntu 24.04](https://github.com/PhysicSimulator/physim/actions/runs/36506432788/job/109210313628).
Auch die vollständige SDK-/Projektprüfung besteht auf beiden Systemen: verschobenes
SDK, unabhängige Header und Kernbibliothek, 15 Sprachprogramme, 27 Sprachmodule,
neun neu gebaute Projekte sowie alle acht C-Vorlagen und der vollständige
Physim-Sprachablauf in der Oberfläche. Die Protokolle liegen unter
`build/ci-installed-debian-620246b.log` und `build/ci-installed-ubuntu-620246b.log`.
Das heruntergeladene Paket zu `0dbe24f` hat außerdem die Prüfung der ZIP-/TAR-Struktur,
Ausführungsrechte der App und aller 224 Dateiprüfsummen bestanden.

Die Container verwenden X11, Xvfb und Mesa; sie prüfen die Paketabhängigkeiten
und Anwendungsabläufe, aber keine echte Grafikhardware oder Wayland-Sitzung.
Die native Datei-/Ordnerauswahl ist noch separat zu prüfen. Für Desktops ohne
XDG-Portal enthalten Installationsanleitung und folgende Paketläufe nun Zenity;
[SDL 3.2.30](https://github.com/libsdl-org/SDL/blob/release-3.2.30/src/dialog/unix/SDL_unixdialog.c)
verwendet Portal oder Zenity für diese Dialoge.

Auch der [Paketlauf zu `ae31aee`](https://github.com/PhysicSimulator/physim/actions/runs/36507609163)
besteht in beiden Distributionen mit Zenity als installierter Laufzeitabhängigkeit.
Der neue separate Dialogtest steuert echte Zenity-Fenster mit `xdotool`: Hauptordner
öffnen, externe Datei und zusätzlichen Ordner hinzufügen, anschließend abbrechen.
Alle ausgewählten Pfade enthalten Leerzeichen und Umlaute. Die App prüft die
übernommenen Pfade und den unveränderten Workspace nach Abbruch. Ein zweiter Lauf
erzwingt einen nicht vorhandenen Dialogtreiber und prüft die Fehlermeldung, ohne
den Workspace zu verändern. Protokolle, ausgewählte Pfade und ein abschließendes
Bild liegen im Artefakt unter `Native dialogs*`. Der Linux-Ausführungsnachweis
für diesen neuen Dialogtest steht bei diesem Commit noch aus. Der aktuelle lokale
Nachweis für GTK 3 und 4 steht oben unter „Lokale Linux-Dialogprüfung“.

Im [Dialoglauf zu `bd359c7`](https://github.com/PhysicSimulator/physim/actions/runs/36509517062)
bestehen auf beiden Systemen alle neun SDK-Oberflächenabläufe. Der Dialogprüfer
findet das erste Zenity-Fenster, bleibt aber nach der Ordnerpfadeingabe bis zum
Zeitlimit darin. Er bestätigt jetzt ausdrücklich mit Zenitys OK-Schaltfläche
(`Alt+O`), statt Enter im GTK-Pfadfeld zu verwenden, und archiviert Bilder vor
und nach der Eingabe sowie bei Fehlern. Die erneute Ausführung bleibt offen.
Protokolle: `build/native-linux-dialog-109220128459.log` und
`build/native-linux-dialog-109220128487.log`.

Der gleiche interaktive App-Test besteht lokal unter Windows mit MSVC Debug und
den tatsächlichen Systemdialogen (`build/native-dialog-windows-3.log`,
`build/native-dialog-windows-3/native-dialogs.bmp`). Der Pfadvergleich berücksichtigt
Windows-Pfadtrenner. Die macOS-Systemdialoge und Linux-Portaldialoge sind damit
noch nicht geprüft.

## Windows

Die [CI-Matrix für `0a27cef`](https://github.com/PhysicSimulator/physim/actions/runs/36459604565)
bestand in allen vier Windows-Jobs: MSVC und ClangCL jeweils in Debug und Release,
einschließlich Tests und installiertem SDK. Fenster- und Grafiktests werden
zusätzlich lokal unter Windows ausgeführt; die GitHub-Windows-Worker garantieren
keinen OpenGL-3.3-Treiber.

## macOS

macOS auf Apple Silicon und Intel ist ein verbindliches Plattformziel.
Die CI verwendet `macos-15` und `macos-15-intel` mit Apple Clang und SDL 3.2.30.
Für `0a27cef` haben der
[Apple-Silicon-Job](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553102)
und der [Intel-Job](https://github.com/PhysicSimulator/physim/actions/runs/36459604565/job/109054553310)
alle 276 Tests ohne Fenster, 35 Grafik-/Fenstertests, das verschobene SDK,
und die acht C-App-Abläufe bestanden. Die Paket-Signierreihenfolge wurde anschließend
für Intel korrigiert. Der separate
[Paketlauf für `2de20b6`](https://github.com/PhysicSimulator/physim/actions/runs/36462932066)
bestand auf beiden Architekturen: neu gebaut, signiert, Signatur geprüft,
in einen Pfad mit Leerzeichen und Umlaut verschoben und daraus vollständige
C- und Physim-Sprachprojekte gebaut, simuliert und ausgewertet.

Die Implementierung berücksichtigt Darwins Programmpfaderkennung, Mach-O-Module,
lokalisierte Zahlenkonvertierung, macOS-Systemschriften und OpenGL 4.1 Core.
`tools/package-macos.py` erzeugt ein verschiebbares `.app`-Paket mit SDL und SDK.
Die lokale Ad-hoc-Signatur wird geprüft; Developer-ID-Signierung und Notarisierung
für eine öffentliche Verteilung sind noch offen. Die CI prüft vollständige C- und
Physim-Sprachprojekte aus dem verschobenen Paket.
Die geprüften ZIP-Dateien stehen im damaligen Paketlauf als `physim-app-macos-15`
(Apple Silicon) und `physim-app-macos-15-intel` bereit. Neue direkte Builds verwenden
`physim-native-macos-15` beziehungsweise `physim-native-macos-15-intel`.
Die CI-Pakete sind Debug-Entwicklungsstände. Weitere macOS-Versionen, echte
Mac-Grafikhardware und Installation auf einem frischen Mac bleiben separate
Abnahmen; die gehosteten Grafiktests verwenden Apples Software Renderer.

Eine zusätzliche Paketprüfung startet jetzt die verschobene `.app` über
`/usr/bin/open -W -n -a`, also über macOS LaunchServices. Sie verwendet ein leeres
Arbeitsverzeichnis, den Systempfad `/usr/bin:/bin:/usr/sbin:/sbin` und entfernt
Entwickler-Vorgaben für Compiler, SDK und Bibliotheken aus der Startumgebung.
Die bisherigen Paketprüfungen starteten direkt `Contents/MacOS/physim`.
Der neue Prüfer verlangt für C und Physim einen vollständigen Oberflächenablauf,
einen erfolgreichen Abschluss nach dem Aufräumen der App, gebaute Module,
gespeicherte Messdaten/Berichte und Screenshots. Abschließend muss die Signatur
des Pakets weiterhin gültig sein. Protokolle und Bilder werden unter
`LaunchServices*` archiviert. Der tatsächliche macOS-Nachweis dieser neuen
Startvariante steht noch aus; native macOS-Dateidialoge bleiben separat offen.

## Direkter Build von Physim

Der Quellstand `7f05668` ergänzt `tools/build.py`. Im
[CI-Lauf](https://github.com/PhysicSimulator/physim/actions/runs/36467398347)
hat der Schritt **Direct compiler build without CMake for Physim** auf allen acht
Kombinationen bestanden: Windows mit MSVC/ClangCL jeweils Debug/Release,
Ubuntu 24.04 mit GCC/Clang sowie macOS 15 auf Apple Silicon/Intel.
Dies belegt diesen abgeschlossenen Schritt; die übrigen Schritte des Laufs
waren beim Erfassen dieses Nachweises noch aktiv.

Die Prüfung baut Bibliotheken, Sprachcompiler, Runner, Projektbuilder und App
mit direkten Compiler-/Archiviereraufrufen. Drei Referenzprogramme prüfen Core,
Numerik und Mechanik. Ein separater Test mit echten Compilerprozessen prüft
Unicodepfade, Headeränderungen, unveränderte Ausgaben, Fehlerwiederaufnahme,
beschädigte Ausgabedateien und die Buildsperre. Linux und beide Macs führen
zusätzlich vollständige Pendel- und Physim-Sprachprojekte in der direkt gebauten
App aus. Windows-Grafikabläufe werden lokal geprüft: beide vollständigen
Projektabläufe mit MSVC Debug sowie Renderer und Eingaberegression mit Clang.
Der lokale Projektbuilder-Test besteht mit den direkt gebauten Werkzeugen für
C- und Physim-Projekte in Debug/Release, einschließlich Projektverschiebung.

Die vollständige Testsuite und SDK-/App-Paketierung sind weiterhin CMake-gestützt.
SDL wird für diese CI-Prüfung zuvor mit seinem eigenen CMake-Buildsystem gebaut.
Der direkte Physim-Build ist daher noch keine vollständige Abnahme des Ziels,
CMake aus der gesamten Entwicklung und Auslieferung zu entfernen.

## Direkte SDK- und App-Pakete

Für `d9f920e` hat der Schritt **Native SDK package and relocation** im
[Paketprüflauf](https://github.com/PhysicSimulator/physim/actions/runs/36470030478)
auf allen vorgesehenen Plattformen bestanden:

| Umgebung | Nachweis |
| --- | --- |
| Windows MSVC Release | [Job 109089656019](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089656019) |
| Windows ClangCL Release | [Job 109089656123](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089656123) |
| Ubuntu 24.04 GCC | [Job 109089655982](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655982) |
| Ubuntu 24.04 Clang | [Job 109089655923](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655923) |
| macOS 15 Apple Silicon | [Job 109089655991](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655991) |
| macOS 15 Intel | [Job 109089655751](https://github.com/PhysicSimulator/physim/actions/runs/36470030478/job/109089655751) |

Der direkte Installer verwendet kein CMake. Die SDK-Prüfung verschiebt das Paket,
prüft seine Inhaltsprüfsummen, bindet alle öffentlichen Header einzeln ein und
kompiliert einen unabhängigen Prüfer gegen die installierte Kernbibliothek.
Alle acht mitgelieferten C-Beispielmodule und 15 neu übersetzte Sprachprogramme
werden ausgeführt. Anschließend baut `physim-build` alle acht C-Vorlagen und ein
Physim-Projekt aus den installierten Quellen neu. Der Prüfer kontrolliert die
Messdateien mit jeweils 201 Zeilen und die erzeugten Berichte.

Linux und beide Macs bestehen zusätzlich vollständige C-/Physim-App-Abläufe aus
dem verschobenen SDK. Beide Mac-Jobs erzeugen daraus eine `.app`, prüfen deren
Ad-hoc-Signatur und führen die App-Abläufe erneut aus diesem Paket aus.
Windows besteht die Grafikabläufe lokal auf der RTX 2080 Ti; die gehosteten
Windows-Paketprüfungen verwenden die Kommandozeilenprogramme.

Die heruntergeladenen Linux-/Mac-Archive wurden zusätzlich auf Dateiintegrität,
ausführbare Dateirechte, Architektur und SDK-Inhalt geprüft. Mac-Bundles enthalten
die relative SDK-Verknüpfung und SDL. Artefakte im Lauf:
`physim-native-windows-v143`, `physim-native-windows-ClangCL`,
`physim-native-linux-gcc`, `physim-native-linux-clang`,
`physim-native-macos-15`, `physim-native-macos-15-intel`.
Die Mac-/Linux-CI-Pakete sind Debug-Builds; Windows verwendet Release.
Weitere Betriebssystemversionen, Clean-Machine-Abnahme und öffentliche
Mac-Notarisierung bleiben offen.

## Direkter Testläufer

Der direkte Katalog umfasst **493 Tests ohne Fenster**, davon 482 ohne SDL,
und zusätzlich **35 Fenster- und Grafiktests** unter `--test-display`.
Sieben zuvor übertragene Prüfungen
für Tutorialquellen, API-Referenz, PNG-Dekodierung und Projektbuild bestehen lokal
mit MSVC Debug, Clang Release und über CTest. Zum Stand `a051ea0` besteht auch der
direkte Build- und Testschritt in allen acht CI-Kombinationen.
Die vorherige Erweiterung übernimmt 21 Runner-, Physik- und Dokumentationsabläufe.
Alle 21 bestehen lokal mit
MSVC Debug und Clang Release; alle 47 gemeinsamen CTest-Integrationsprüfungen
bestehen ebenfalls. Für den CTest-Vergleich wurden die vorhandenen Runner neu
gebaut, nachdem zwei Crash-Tests mit veralteten Programmen fehlgeschlagen waren.
Die 419 direkten Sprachtests bestanden bereits zum vorherigen Stand `b95e119` mit
beiden Compilern. Zum Stand `7e38cf1` bestehen alle 484 damaligen Tests in allen
acht CI-Kombinationen. Der anschließende Build ohne Tests scheitert dort an der
Argumentprüfung für `--test-filter`; der direkte CI-Schritt ist daher fehlgeschlagen.
Die Korrektur besteht lokal für normale Builds mit MSVC und Clang. Ein zusätzlicher
Regressionstest prüft normale Builds, SDK-Installation und Testfilter über den
Kommandozeileneinstieg. Die Korrektur ist mit dem direkten CI-Schritt zu `a051ea0`
auf allen acht Kombinationen bestätigt.

Alle 34 neuen Grafikabläufe sind lokal mit MSVC Debug auf der RTX 2080 Ti geprüft.
Der erste Lauf bestand 30 Fälle; nach Korrektur der UTF-8-Ausgabe und der Vorbereitung
zweier Testordner bestanden die vier betroffenen Fälle ebenfalls. Die Prüfungen
behalten Menü-, Dokument-, Wiederherstellungs-, Projekteinstellungs- und Sprachabläufe
bei. Beide Diagrammtests prüfen zusätzlich alle PNG-Exporte mit unabhängigen
Pixelvergleichen. Acht ausgewählte Abläufe bestehen außerdem mit der Clang-Release-App:
Auftrieb, Diagramme, Stapelläufe, Dokumentbuild, Projekteinstellungen,
Dokumentwiederherstellung, Autosave und der vollständige Sprachablauf.
Die Projekt-Buildprotokolle bestätigen dabei Clang als Compiler.
Drei zusätzliche CTest-Vergleiche für `toolbar_small`, `plot_workflow` und
`language_mixed_workflow` bestehen mit den neu gebauten MSVC-Debug-Programmen.
Im [CI-Lauf zu `a621b5b`](https://github.com/PhysicSimulator/physim/actions/runs/36488141869)
besteht der direkte Grafikschritt auf macOS Apple Silicon. Linux GCC und Clang
bestehen jeweils 33 von 34 Fällen; der erste Auftriebstest endet nach 150 Sekunden
ohne App-Ausgabe im Timeout. Die Ursache ist noch offen. Der Linux-Aufruf wartet
nun vor dem App-Start auf den Fenstermanager; zusätzliche App-Testprotokolle halten
Initialisierung und Phasenwechsel auch bei Zeitüberschreitung fest. Die erneute
Linux-CI und der Grafiknachweis für macOS Intel standen bei diesem Stand aus.

Im [Folgelauf zu `37eef01`](https://github.com/PhysicSimulator/physim/actions/runs/36490044371)
besteht der direkte Grafikschritt mit allen 35 Fällen unter Linux GCC und Clang
sowie macOS Apple Silicon und Intel. Damit besteht dort auch der neue UI-Benchmark.
Der erste Linux-Auftriebstest läuft mit der Fenstermanager-Wartebedingung durch;
ein eindeutiger Ursachennachweis für den vorherigen Timeout folgt daraus nicht.
Auch der direkte Build- und Testschritt mit 493 Tests besteht auf allen acht
Plattformkombinationen dieses Stands.

Die drei ergänzten Benchmark-Prüfungen bestehen lokal mit MSVC Debug, Clang Release
und über CTest. Der SDL-freie MSVC-Release-Build besteht beide Prüfungen ohne Fenster.
Die Referenzbilder für leere Ansicht, viele Bedienelemente und acht Kurven sind
zwischen den beiden Compilern bytegleich. In allen drei Messfällen entstehen nach
dem Aufwärmen keine weiteren Zeichenpuffer-Allokationen. Die bisherigen Prüfungen
für Export, Größenwechsel, Referenzwerte und Vergleichsfehler bleiben erhalten.
Dies sind Funktionsprüfungen; die parallel zu Builds gemessenen Zeiten sind keine
Performance-Baseline. Der direkte Grafikschritt des Folgelaufs bestätigt den
UI-Benchmark inzwischen auf den oben genannten vier CI-Kombinationen.

Der [Linux-GCC-Lauf zu `c92dbd3`](https://github.com/PhysicSimulator/physim/actions/runs/36491788018)
besteht erneut alle 34 App-Abläufe, scheitert aber im UI-Benchmark am Größenwechsel.
Das archivierte `empty-restored.bmp` hat noch 640 × 480 statt 1080 × 740 Pixel.
Der Benchmark wartet nun mit `SDL_SyncWindow` auf die Größenänderung und prüft
die tatsächliche Fenstergröße, bevor er zeichnet. Lokal besteht die korrigierte
Prüfung mit MSVC Debug und Clang Release. Der erneute Linux-Nachweis steht noch aus.

Der direkte Builder unterstützt zusätzlich `--sanitizers`: ASan unter Windows,
ASan und UBSan unter Linux/macOS. Lokal bestehen mit MSVC die positive
Instrumentierungsprobe (fehlerfreier Lauf und erkannter Heap-Pufferüberlauf) sowie
zehn Tests für Core, Speicherbesitz, Berichte, Protokoll-/Messdateimutationen und
die vier Sprachspeicherprüfungen. Die Ergebnisdatei liegt unter
`build/native/Debug-sanitized/test-results/run-1t_t37re/results.json`.
ClangCL 19.1.5 scheitert lokal schon beim Start des sicheren ASan-Probeprogramms
in seiner Interception-Laufzeit. Deshalb ist dafür keine lokale Abnahme belegt.
Im [CI-Lauf zu `c92dbd3`](https://github.com/PhysicSimulator/physim/actions/runs/36491788018)
bestehen die Sanitizer-Probe und alle zehn ausgewählten Tests unter Windows
mit MSVC und ClangCL. Die Linux-/macOS-Sanitizer-Abnahme steht noch aus.

Der [CI-Lauf zu `a9accf5`](https://github.com/PhysicSimulator/physim/actions/runs/36492484066)
bestätigt außerdem den direkt gebauten IPC-libFuzzer unter Windows ClangCL und
Linux Clang. Beide Prüfungen erzeugen gültige Eingaben, spielen sie erneut ab
und bestehen 10.000 Durchläufe mit zusätzlicher Codeabdeckung ohne Fehlerfund.
Der Apple-Silicon-Job scheitert mit Apple Clang aus Xcode 16.4 am Linken, weil
`libclang_rt.fuzzer_osx.a` in der Toolchain fehlt. Für die Fuzzer-Prüfung wird nun
Homebrew LLVM 20 auf beiden Mac-Architekturen verwendet. Normale App-Builds
verwenden weiter Apple Clang.

Im [Lauf zu `0d10940`](https://github.com/PhysicSimulator/physim/actions/runs/36493259775)
besteht die Fuzzer-Kampagne mit LLVM 20 auf macOS Intel. Auf
Apple Silicon meldet Apples Linker `invalid r_symbolnum=1` für instrumentierte
Objekte. Der direkte Fuzzer-Build verwendet nun LLVMs Mach-O-Linker `ld64.lld`;
im [Lauf zu `f6cedbf`](https://github.com/PhysicSimulator/physim/actions/runs/36494602802)
bestehen damit beide Mac-Fuzzer-Kampagnen einschließlich 10.000 Durchläufen.

Die direkte Linux-Clang-Sanitizer-Suite zu `c92dbd3` besteht 492 von 493 Tests.
UBSan findet im Clipboard-Test einen `memcmp`-Aufruf mit Nullzeiger bei Länge null.
Der Vergleich behandelt leere Texte nun vor dem Speichervergleich; lokal bestehen
die Clipboard-Prüfungen mit MSVC Debug und Clang Release. Im Lauf zu `f6cedbf`
bestehen alle 493 Sanitizer-Tests ohne Fenster unter Linux GCC und Clang.
Der Mac-ARM-Lauf zu `c92dbd3`
Stands besteht 479 von 482 Sanitizer-Tests. Beim erneuten Laden von Modulen melden
zwei Fälle doppelt registrierte ASan-Globals; `template_ids` bricht in der
ASan-Registrierung ab. Die folgende Minimalprüfung grenzt die Ursache ein.
Eine zusätzliche Minimalprüfung lädt zwei instrumentierte Module je 32-mal,
kontrolliert das Zurücksetzen veränderter Globals und verlangt anschließend
einen erkannten globalen Pufferüberlauf. Lokal besteht sie mit MSVC unter
`build/native/sanitizer probe ä edntxgm2`. Im
[Lauf zu `4339411`](https://github.com/PhysicSimulator/physim/actions/runs/36495445390)
besteht sie auch mit Apple Clang auf Intel, reproduziert aber auf Apple Silicon
die doppelte Registrierung von `probe_globals`. Die archivierte Assemblerdatei
zeigt dort `__mod_term_func` für den ASan-Destruktor. Die LLVM-Umstellung auf
[`__cxa_atexit`](https://reviews.llvm.org/D121327) vermeidet diese veraltete
Darstellung. Die macOS-Sanitizer-CI verwendet nun LLVM 20 und LLD wie der Fuzzer;
im [Lauf zu `69ddae7`](https://github.com/PhysicSimulator/physim/actions/runs/36495859541)
besteht die vollständige Minimalprüfung auf Apple Silicon und Intel, einschließlich
der 64 Modulöffnungen und des erkannten globalen Pufferüberlaufs nach erneutem
Laden. Auf beiden Architekturen bestehen außerdem alle 482 SDL-freien
Sanitizer-Tests sowie die direkten Build-, SDK- und Grafikschritte.

Die korrigierte UI-Größenprüfung besteht im direkten Grafikschritt unter Linux
GCC zu `0d10940`. Im vorherigen GCC-Lauf zu `49efcbb` besteht der UI-Benchmark,
aber der erste App-Ablauf hängt erneut vor der Meldung „window and OpenGL ready“.
Zusätzliche Tracepunkte unterscheiden nun SDL-Initialisierung, Fenstererzeugung
und GL-Kontext. Der Wartepunkt auf den Fenstermanager allein behebt diesen
sporadischen Startfehler damit nicht vollständig.
Zu `f6cedbf` bestehen unter Linux GCC und Clang zusätzlich alle 35 direkt gebauten
Fenster-/Grafiktests mit ASan und UBSan. Das belegt diesen Lauf, schließt den
zuvor beobachteten sporadischen Startfehler aber nicht aus.
Die Linux-CI ergänzt deshalb für `floating_workflow` eine strace-Aufzeichnung
einschließlich Prozess-, Socket- und Warteaufrufen. Der bestehende Timeout bleibt
aktiv; `--kill-on-exit` beendet beim Abbruch auch die verfolgten Prozesse.
Eine zusätzliche Linux-Läuferprüfung kontrolliert echte Traceausgabe und das
Ende eines gestarteten Kindprozesses. Der gemeinsame Läufertest besteht lokal
unter Windows (`build/native-trace-runner-windows.log`). Im
[Lauf zu `1790d87`](https://github.com/PhysicSimulator/physim/actions/runs/36497491668)
bestehen die neuen Linux-Zweige mit GCC und Clang sowie alle 35 normalen
Grafiktests mit aktivierter Aufzeichnung des ersten Ablaufs. Mit Clang bestehen
auch alle 35 instrumentierten Grafiktests. Diese erfolgreichen Läufe allein
belegen keine Behebung des sporadischen Startfehlers.

Die direkte SDK-Prüfung umfasst jetzt auch alle zuvor nur im CMake-SDK-Vergleich
gebauten Sprachmodule: 27 Module, neun Sprachexperimente mit beiden allgemeinen
Analysen, spezielle Sensoranalyse und sechs C-/Physim-Kombinationen.
Die installierten Core-Quellen und alle acht C-Vorlagen samt Analyse werden
unabhängig von den ausgelieferten Bibliotheken neu gebaut. Ein SDL-freies
MSVC-Release-SDK besteht die vollständige erweiterte Prüfung unter
`build/native/Native SDK ä cnycd4y3`. Das Clang-Release-SDK besteht zusätzlich
neun Projektbuilds und beide grafischen Abläufe unter
`build/native/Native SDK ä elwayubq`. Im Lauf zu `f6cedbf` besteht der erweiterte
direkte SDK-Schritt unter Linux GCC/Clang, macOS Apple Silicon/Intel und
Windows MSVC/ClangCL Release.

Die Pendelreferenzen prüfen jeweils 4001 Messpunkte, Energiedrift und Periodendauer
gegen eine analytische Referenz. MSVC und Clang liefern für RK4 eine Periodendauer
von 2,48880586925 s bei einer Referenz von 2,48880587159 s. Die Prüfung abgeleiteter
Daten vergleicht vollständiges CSV und Bericht; der maximale Geschwindigkeitsfehler
beträgt dabei rund 5,59e-5 m/s. Crash-, Hang- und Batch-Prüfer kontrollieren zusätzlich
Prozessende, Datenwiederherstellung und das Aufräumen paralleler Kindprozesse.

Die Analyseprüfungen vergleichen den vollständigen CSV-Inhalt und verlangen,
dass ungültige Exporte keine CSV-Datei anlegen. Der Läufertest prüft erwartete
Exitcodes und Diagnosen, Zeitüberschreitungen, fehlende Programme, Emissions-/
Buildfehler, fehlende Ausgaben, falsche Inhalte und ungültiges UTF-8. Fehlerhafte
Schritte verhindern die Ausführung davon abhängiger Programme. Die Ergebnisberichte
enthalten für erwartete Dateien außerdem Größe und SHA-256-Prüfsumme.
Die Integrationsprüfungen erfassen zusätzlich die Prüfsummen aller verwendeten
Programme und Module. Ihre C-Prüfer kontrollieren Messwerte, Metadaten, Szenen,
Berichte und das Verhalten bei ungültigen Parametern. Tests der Ablaufsteuerung
verhindern Erfolge durch veraltete Prüfer nach C-/Physim-Buildfehlern oder fehlende
Ausgabedateien.

Die folgende Tabelle dokumentiert abgeschlossene **direkte Build- und Testschritte**.
Sie nimmt spätere SDK-, Grafik- oder CTest-Schritte desselben CI-Laufs nicht vorweg.
Die acht Kombinationen sind Windows MSVC/ClangCL jeweils Debug/Release, Linux
GCC/Clang sowie macOS auf Apple Silicon/Intel.

| Stand | Direkte Tests | Ausgeführter CI-Nachweis |
| --- | --- | --- |
| `a55d5e0` | 51 C-Tests | Alle acht Kombinationen: [Lauf 36471876255](https://github.com/PhysicSimulator/physim/actions/runs/36471876255) |
| `199a8f7` | 73 Tests | Alle acht Kombinationen: [Lauf 36473305112](https://github.com/PhysicSimulator/physim/actions/runs/36473305112) |
| `430df15` | 134 Tests | Alle acht Kombinationen: [Lauf 36474491263](https://github.com/PhysicSimulator/physim/actions/runs/36474491263) |
| `0a937f8` | 152 Tests | Alle acht Kombinationen: [Lauf 36475496512](https://github.com/PhysicSimulator/physim/actions/runs/36475496512) |
| `d7fb0b0` | 200 Tests | Alle acht Kombinationen: [Lauf 36476324664](https://github.com/PhysicSimulator/physim/actions/runs/36476324664) |
| `0923b6e` | 222 Tests | Alle acht Kombinationen: [Lauf 36477404252](https://github.com/PhysicSimulator/physim/actions/runs/36477404252) |
| `8516187` | 228 Tests | Alle acht Kombinationen: [Lauf 36478529900](https://github.com/PhysicSimulator/physim/actions/runs/36478529900) |
| `2b3b6e7` | 437 Tests | Alle acht Kombinationen: [Lauf 36480859045](https://github.com/PhysicSimulator/physim/actions/runs/36480859045) |
| `b95e119` | 463 Tests | Alle acht Kombinationen: [Lauf 36482481623](https://github.com/PhysicSimulator/physim/actions/runs/36482481623) |
| `a051ea0` | 491 Tests, Build ohne Testfilter korrigiert | Alle acht Kombinationen: [Lauf 36485408557](https://github.com/PhysicSimulator/physim/actions/runs/36485408557) |
| `87c8ae8` | 493 Tests, zusätzlich 15 Sprachprogramme und 27 Module mit Prüfung inkrementeller Builds und Fehlerkorrektur | Alle acht Kombinationen: [Lauf 36499326457](https://github.com/PhysicSimulator/physim/actions/runs/36499326457) |

Die ursprünglichen 51 C-Tests bestanden auch lokal mit MSVC Debug; die 41 SDL-freien
Fälle zusätzlich mit Clang Release. Der vollständige Satz mit 134 Tests bestand
lokal mit MSVC Debug.

Die CI verwendet nach diesen Vergleichen vollständig den direkten Physim-Build.
Im [Lauf zu `558c2ee`](https://github.com/PhysicSimulator/physim/actions/runs/36500503825)
bestehen alle acht Plattformjobs, einschließlich der neuen SDK- und Paketabläufe.
Die 528 Tests, Sanitizer, SDK-Prüfung, Fuzzer und Benchmarks bleiben
enthalten. Die SDK-Grafikprüfung übernimmt zusätzlich alle acht C-Vorlagen;
Linux prüft sie auch mit der instrumentierten App. Mac-Pakete entstehen nur aus
dem nativen SDK und werden nach dem Signieren verschoben und erneut geprüft.
Neue SDKs enthalten keine CMake-Builddateien. Die alten Repository-Builddateien
und CMake-Testskripte sind entfernt; alle 528 Fälle bleiben im direkten Katalog.
Auch das bereinigte Repository besteht alle acht Plattformjobs im
[Lauf zu `7d39aa7`](https://github.com/PhysicSimulator/physim/actions/runs/36502055743).
Lokal bestehen nach dem Entfernen der 90 Physim-CMake-Dateien alle 493 Prüfungen
ohne Fenster mit MSVC Debug (`build/native/Debug/test-results/run-9wu8gqqs`) und
alle 35 Grafikprüfungen mit MSVC Release (`build/native/Release/test-results/run-hxhqdkcm`).
SDL wird in der CI weiterhin mit seinem CMake-Build gebaut.

## Weitere Änderungen prüfen

Jeder Push startet die [CI](https://github.com/PhysicSimulator/physim/actions/workflows/ci.yml).
Die jeweiligen Jobs und Artefakte zeigen den geprüften Commit. Linux-Screenshots
und direkte Ergebnisberichte werden als `linux-ui-gcc` beziehungsweise
`linux-ui-clang` archiviert. Diese Artefakte enthalten auch SDK-Protokolle.
