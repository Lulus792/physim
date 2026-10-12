# Plattformprüfung

Stand: 7. Oktober 2026. Diese Nachweise gelten für die genannten Umgebungen
und ersetzen keine Abnahme aller Ziele des Projektplans.

Ältere vollständige Belege stehen zusätzlich in [Archivband 2](platform-validation-history-2.md).

## Projekterstellung per Tastatur am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#projekterstellung-per-tastatur-am-7-oktober-2026) erhalten.

## Handbuch per Tastatur am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#handbuch-per-tastatur-am-7-oktober-2026) erhalten.

## Persönliche Einstellungen per Tastatur am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#persönliche-einstellungen-per-tastatur-am-7-oktober-2026) erhalten.

## Unabhängige UI-Schriftgröße am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#unabhängige-ui-schriftgröße-am-7-oktober-2026) erhalten.

## Reihenfolge der Menüaktionen und Editorfokus am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#reihenfolge-der-menüaktionen-und-editorfokus-am-7-oktober-2026) erhalten.

## Hauptmenüs per Tastatur am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#hauptmenüs-per-tastatur-am-7-oktober-2026) erhalten.

## Live-Geschwindigkeitswechsel und gepufferte Snapshots am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#live-geschwindigkeitswechsel-und-gepufferte-snapshots-am-7-oktober-2026) erhalten.

## Gemeinsamer Core im nativen Projektbuilder am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#gemeinsamer-core-im-nativen-projektbuilder-am-7-oktober-2026) erhalten.

## GUI-Geschwindigkeit bei Rückstau am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#gui-geschwindigkeit-bei-rückstau-am-7-oktober-2026) erhalten.

## Strömungs-Lehrmodelle und Physim 0.182.0 am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#strömungs-lehrmodelle-und-physim-01820-am-7-oktober-2026) erhalten.

## Wellen/Optik und Physim 0.181.0 am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#wellenoptik-und-physim-01810-am-7-oktober-2026) erhalten.

## Elektromagnetismus und Physim 0.180.0 am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#elektromagnetismus-und-physim-01800-am-7-oktober-2026) erhalten.

## Thermodynamik und Physim 0.179.0 am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#thermodynamik-und-physim-01790-am-7-oktober-2026) erhalten.

## Getrennte C-/Physim-Dokumentation am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#getrennte-c-physim-dokumentation-am-7-oktober-2026) erhalten.

## Batch-Hostdienste und Physim 0.178.0 am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#batch-hostdienste-und-physim-01780-am-7-oktober-2026) erhalten.

## Elternprozess-Abbruch und Paketierungsfehler am 7. Oktober 2026

Der vollständige Nachweis bleibt in [Archivband 2](platform-validation-history-2.md#elternprozess-abbruch-und-paketierungsfehler-am-7-oktober-2026) erhalten.

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


Ältere Plattformnachweise stehen vollständig unter
[Historische lokale Prüfungen](status-history.md). Die genannten Laufstände
und Dateipfade behalten ihren ursprünglichen Beweisumfang.


## Gemeinsame Series-Numerik am 7. Oktober 2026

Die Folgekorrektur zu CR-007 verwendet eine gemeinsame interne Rechnung in
`src/analysis_numeric.h` für die bisherigen Array-Helfer und die C-/Physim-Series.
Die Sekantenmethode bleibt erhalten. Extreme darstellbare Steigungen und
Trapezflächen werden nicht mehr durch überlaufende Differenzen oder vorzeitig
zu null gerundete subnormale Mittelwerte abgewiesen. Echte Überläufe nehmen
Handle und Scratch-Speicher vollständig zurück. Ein fehlender Messwert bleibt
im kumulativen Integral auch über extreme Achsenabstände unbekannt.
[Verfahren, Einheiten und Grenzen](numerics.md) erklären die Sekanten- und
Trapezapproximation; PP-0363 besitzt jetzt eine explizite Zuordnung.
API/ABI 3 und sämtliche Datenformate bleiben erhalten.

### Lokale Tests und ihre genaue Reichweite

Der unveränderte Series-Pfad scheitert am neuen Gegenbeispiel unter
`build/contact-world-language-release-mac/test-results/run-wo9rwlxg`:
`x=y={-1e308,1e308}` muss die Steigung 1 ergeben. Der korrigierte gezielte
Sieben-Fälle-Lauf besteht unter `run-19yzguj5`. Ein weiterer Zwischenlauf
`run-o8t_3nt1` enthielt doppelte Achsenwerte durch einen zu kleinen Testabstand;
das Raster wurde korrigiert und dieser Lauf bleibt fehlgeschlagen.

Der vollständige macOS-Release-Lauf besteht mit **609/609** unter
`build/contact-world-language-release-mac/test-results/run-mrbtiqsb`.
Unter Debian/GCC enthält der vollständige Lauf `run-mbhcuca2` **608/609**:
alle Laufzeitfälle bestehen, `documentation_reference` scheitert an
Apple-Metadatendateien aus der Dateiübertragung. Die zugehörigen erzeugten
Begleitdateien wurden vor dem Entfernen in
`build/series-numeric-transfer-metadata.tar.gz` gesichert und gegen das Archiv
geprüft (SHA-256 `a2e35542025afb38317c72f3f56ba4312f89aa677fc038179209e6c5c67d8021`).
Der ursprüngliche Lauf wird dadurch nicht nachträglich grün.

Die vier abschließenden Fälle `series_numeric_extremes`,
`language_analysis_numeric_extremes`, `verification_kit` und
`documentation_reference` bestehen mit **4/4** auf macOS unter `run-y4a44_hx`
und Linux unter `run-00f11gmw`. Sie verwenden den endgültigen Teststand mit
Polynomreferenzen, Konvergenzordnung, Blockgrenzen, Messmasken, subnormalen
Flächen, Vorzeichenauslöschung, Einheiten und Fehler-Rücknahme. Der separate
Physim-Analyseprozess prüft selbst erzeugte Reihen ohne Eingabedatensatz.
29 Referenzseiten sind aktuell. Der Katalog enthält jetzt 610 Fälle ohne
Fenster, 591 ohne SDL und 75 Fensterfälle; die nach dem Gesamtlauf ergänzte
separate Sprachprüfung wird durch den Vier-Fälle-Lauf belegt.

### Noch offene Remote-Befunde

Die vor dieser Korrektur abgefragte
[Linux-Paket-CI für `37a55bd`](https://github.com/PhysicSimulator/physim/actions/runs/37609456711)
scheitert ausschließlich an `runner_pacing_phys`: der Offline-Lauf benötigt
1,426 Sekunden gegenüber 2,299 Sekunden bei 0,5× und verletzt den bisherigen
relativen Zeitvergleich. Die Ursache ist noch nicht nachgeprüft; ein
Hardware-/Lastproblem wird nicht als bewiesen behandelt. Das Artefakt mit
608 Ergebnisdatensätzen stimmt mit dem veröffentlichten SHA-256
`e73958f216105338e4d5564b643fdf1b957b19c08b614fda556d726da79b9f46` überein.

Die [C17-CI für `0fd3ee7`](https://github.com/PhysicSimulator/physim/actions/runs/37603493468)
enthält weitere Fehler. Der geprüfte Intel-macOS-SDK-Log endet bei der
Handbuch-Tastaturprüfung an Stufe 13 nach 15,029 Sekunden; die 608 nativen
Debug-Fälle bestehen. Das Artefakt besitzt SHA-256
`dc1b6f65b5dd6150001e169dd223d10417a5002c5a6aa892d3b3940b2a36c08e`.
Das separat geprüfte Windows/v143-Release-Artefakt besitzt SHA-256
`f17510e40bd81f6d17de142149924bd19448721820c47c59010ca960bd2f4ffd`;
607/608 Fälle bestehen, die Referenzprüfung weist die damals zu große
Plattformseite zurück. Die anschließende Archivierung in `37a55bd` hat die
Seite verkleinert; eine neue Windows-Gesamtabnahme wird damit nicht behauptet.
Diese Befunde bleiben von der lokalen Numerikkorrektur getrennt offen.


### Frische verschobene SDKs

Beide vollständigen SDK-Prüfverfahren **ohne Fensterprüfungen** bestehen:

- macOS: `build/Series numeric SDK ä mac tdu_luc2`, verschobenes SDK unter
  `proof/Native SDK ä o3ewk32f/Relocated SDK ä`.
- Linux: `build/Series numeric SDK ä linux p5iekc7w`; der genaue verschobene
  Pfad steht im zugehörigen `verify.log` und `PASSED.json`.

Jeder Lauf prüft ein frisches SDK mit 430 SHA-256-erfassten Dateien und ein
isoliertes Kit mit 60 Eingaben. Die neue private Datei `src/analysis_numeric.h`
ist in der Source-Distribution enthalten. Header und alle 26 Core-Module werden
ausschließlich aus dem verschobenen SDK kompiliert. Die neuen C- und
Physim-Gegenbeispiele bestehen jeweils gegen das installierte und das neu
kompilierte Archiv. Alle bisherigen SDK-Gates bleiben aktiv, darunter die
gekoppelten Lernpfade, unabhängigen Referenzprüfer und kalten nativen Domänenbuilds.
`verify.log` enthält den ausdrücklichen Numerik-Erfolgsmarker; das jeweils
verschobene SDK-Prüfverzeichnis enthält `PASSED.txt` und den vollständigen
Befehlslog. Die übergeordneten Receipts stehen unter
`build/series-numeric-sdk-mac-PASSED.json` und
`build/series-numeric-sdk-linux-PASSED.json`.

Der erste Linux-Hilfsaufruf endete **vor** der SDK-Verifikation an der unter
Python 3.11 nicht verfügbaren `tarfile.extractall(filter=...)`-Option. Die
Fortsetzung prüft die Archivpfade, regulären Dateien und Kit-Hashes explizit
und verwendet dasselbe bereits erfolgreich gebaute SDK. Dieser ursprüngliche
Hilfsfehler bleibt in `series-numeric-sdk-proof-linux.log` erhalten; die
Fortsetzung steht in `series-numeric-sdk-resume-linux.log`.

Die zwölf implementierungs- und prüfrelevanten Dateien stimmen auf beiden
Systemen mit `build/series-numeric-source-freeze.json` überein. Nach dem Entfernen
der Übertragungsmetadaten wurde zusätzlich der gesamte erwartete
Repository-Dateibestand auf Linux byteweise mit dem Mac abgeglichen.
Neue Fenster-, Windows- und Apple-Silicon-Prüfungen gehören nicht zum
Nachweis dieser Runde. Die oben aufgeführten Remote-Fehler bleiben zu bearbeiten.


## CI-Timingprüfungen am 7. Oktober 2026

Die [C17-CI für `37a55bd`](https://github.com/PhysicSimulator/physim/actions/runs/37609456911)
ist abgeschlossen: beide Linux- und alle vier Windows-Jobs bestehen. Die beiden
macOS-Jobs scheitern weiterhin in der SDK-Handbuchprüfung an Stufe 13 nach
15,061 beziehungsweise 15,055 Sekunden. Die gezielt gelesenen Artefakte stimmen
mit den veröffentlichten SHA-256-Digests überein: Intel
`5a41d123f7a87ef8b2382ba176bbbfbf9d623b8f9f2ac2e5a6761c1e509ddaaa`,
Apple Silicon `3227a33d7ea1ebdc21afa9ca5f1bf31dcafc46b7cf5a7139041d666dfa3e9d0d`.

### Reproduzierte Testannahmen

Ein deterministisch verzögertes Zeichnen mit 80 ms pro Frame reproduziert unter
Intel macOS genau die Handbuchstufe 13 und den bisherigen 15-Sekunden-Abbruch:
`build/contact-world-language-release-mac/test-results/run-8x2xqqpy`.
Der Trace zeigt weiter wechselnde Fokusziele; die Navigation hängt dort nicht.
Die genaue Ursache des langsameren Zeichnens auf den Remote-Macs bleibt ohne
deren detaillierten Trace unbewiesen. Die Prüfung muss langsames Zeichnen
zulassen, ohne fehlende oder unerreichbare Bedienelemente zu akzeptieren.

Die Handbuch-Tastaturtests besitzen nun ein eigenes 60-Sekunden-Gesamtlimit
(innerhalb der 75-/85-Sekunden-Prozessgrenzen). Jede Fokussuche prüft zusätzlich,
dass das semantische Ziel existiert, und begrenzt die Versuche anhand der
aktuellen Zahl navigierbarer Elemente. Fehlschläge melden Stufe, Ziel, Fokus,
Sichtbarkeitszustand, Thema und Versuchszahl. Eine neue negative Prüfung erzwingt
ein fehlendes Ziel und verlangt Exitcode 1 mit der passenden Diagnose.
`docs-keyboard-delayed` verwendet die gesamte bisherige Tastaturfolge und ist
auch im SDK-Prüfverfahren enthalten; der normale Produktbetrieb bekommt keine
künstliche Verzögerung.

Der unveränderte Offline-Zeitvergleich scheitert mit einem um 1,5 Sekunden
verzögerten Beobachter unter `run-bkhk2803`: 0,5× benötigt 2,003 Sekunden,
4× 0,260 Sekunden, Offline 1,501 Sekunden. Datenvergleich, Pause und Einzelschritt
sind bereits erfolgreich; nur die bisher geforderte relative Beobachtungszeit
scheitert. Offline bedeutet fehlende Scheduling-Wartezeit, keine garantierte
Schranke für Experimentrechnung, Datenträger, Pipes und den Beobachter.
Die genaue zusätzliche Latenz des früheren Linux-CI-Laufs bleibt unbewiesen.
Der neue Vertragstest fordert 100.000 fällige Schritte bei **unveränderter** Uhr
und prüft zusätzlich die tatsächlichen Runner-Daten und Steuerungen mit dem
verzögerten Beobachter. Die Zeitprüfungen der realen 0,5×-/4×-Raten und die
Prüfung des langsamen Lesers bleiben erhalten. Runner und Pacer wurden nicht
geändert.

### Ausgeführte Nachweise

Die vier gezielten Runner-Fälle bestehen auf macOS unter `run-yfpgwbav` und
Linux unter `run-sst862ow`. Die sieben bisherigen Handbuch-/Menüprüfungen
mit dem neuen langsamen Ablauf bestehen unter macOS `run-8_xgmqrz` und
Linux `run-plnrml_9`. Nach Ergänzung der negativen Kontrolle bestehen sämtliche
vier Tastaturfälle am endgültigen App-Stand unter macOS `run-cdtaiyvk` und
Linux `run-_n21xkus`. Der langsame macOS-Fall benötigt 25,417 Sekunden.
Ein einzelner neuer Gesamtlauf aller Fälle wird damit nicht behauptet.

Beide gezielten SDK-App-Prüfungen bestehen mit vier erfolgreichen Abläufen
(16 px, 22 px, langsames Zeichnen, sämtliche Handbuchthemen/Lernwege) sowie der
korrekt fehlschlagenden Fokus-Kontrolle:

- macOS: `build/CI regression SDK ä mac vxc7i35j/Relocated SDK ä`,
  App-SHA-256 `f3b748b31e2eb709aab78378f28b82951a22c451439519f6c57034e22d8a0b81`.
- Linux: `build/CI regression SDK ä linux h05lz9j2/Relocated SDK ä`,
  App-SHA-256 `53fbb29c403b639ed88a7c04e03c2e1403df5ed06cea42d49031cff6b02a50e8`.

Sie verwenden die zuvor vollständig geprüften numerischen SDKs mit neuer App;
alle anderen 429 manifestierten Dateien bleiben bytegleich. Die fünf
Befehle, erwarteten Exitcodes und acht identischen Implementierungs-/Prüfquellen
stehen in `build/ci-followup-sdk-mac-PASSED.json`,
`build/ci-followup-sdk-linux-PASSED.json` und `ci-followup-source-freeze.json`.
Vorherige Hilfsaufrufe endeten vor dem App-Start an fehlenden Arbeitsordnern;
die Korrektur und Fortsetzung sind in `ci-followup-sdk-ui-proof-v3-*.log`
festgehalten. Diese Hilfsfehler sind keine erfolgreichen App-Prüfungen.

29 Referenzseiten und das isolierte Kit mit 60 Eingaben bestehen auf beiden
Systemen. Der Katalog enthält jetzt 611 Fälle ohne Fenster, 592 ohne SDL und
77 Fensterfälle. Bei der letzten Abfrage ist der Paketbuild der
[Linux-CI für `d4ac0f3`](https://github.com/PhysicSimulator/physim/actions/runs/37619178591)
erfolgreich; die beiden installierten Systemprüfungen und der Großteil der
[C17-Matrix](https://github.com/PhysicSimulator/physim/actions/runs/37619178905)
laufen noch. Das bestätigt weder diese neue Korrektur noch die vollständige
Plattformabnahme; die nächste Remote-Matrix bleibt erforderlich.


## Einzelabgleich der Basis und RNG-Vertrag am 7. Oktober 2026

PP-0348/0349/0350/0351/0353 sind jetzt einzeln vorhandenen APIs, Quellen und
Referenztests zugeordnet. Feste Integer-/Ergebniswerte, Allocatoren, alle vier
Container, explizite Logger und RNG-Zustände besitzen konkrete Implementierungen.
Die allgemeine Handle-Anforderung PP-0352 bleibt ungeprüft: sichere generationale
Series-/Dataset-/Report-APIs ersetzen keine Einzelabnahme sämtlicher öffentlicher
Zustandsdeskriptoren. Der Abgleich bewahrt alle 531 Originalblöcke und enthält
jetzt 20 implementierte, fünf unvollständige und 506 ungeprüfte Blöcke.

Der Normalgenerator zieht den Radius und anschließend den Winkel jetzt in
getrennten C-Anweisungen. Die Probe des vorherigen Ausdrucks auf Debian/GCC und
Clang liefert dieselben sechs Werte; ein tatsächlicher Unterschied dieser Builds
wird **nicht** behauptet. Die Änderung macht die Reihenfolge ausdrücklich zum
Vertrag, statt sie der Operandenauswertung zu überlassen. [RNG-Vertrag](measurement.md)
unterscheidet exakt prüfbare Integerzustände von libm-Rundung und dem ungeprüften
C-Helfer von geprüften, verbrauchsfreien degenerierten Verteilungen. API/ABI 3,
Sprachversion und Dateiformate bleiben erhalten.

### Lokale Referenzen und Sanitizer

Die neuen unabhängigen Probes prüfen vier vollständige 64-Bit-Seeds, 384 C-Referenzwerte
mit ihren jeweiligen Integerzuständen und 128 Physim-Referenzwerte. Dazu kommen
Snapshots, abwechselnd genutzte unabhängige Ströme, degenerierte Verteilungen und die
transaktionale Ablehnung ungültiger Parameter. Integerwerte und Zustände werden
exakt, Normalwerte mit absoluter/relativer Toleranz `2e-14` geprüft.
Die erste Sprachfixture enthielt eine nicht unterstützte if-Ausdruckssyntax und
scheiterte unter `run-fpl88l8n`; sie wurde auf reguläre if-Anweisungen korrigiert.
Dieser ursprüngliche Lauf bleibt fehlgeschlagen.

Die 24 gezielten Basis-/RNG-/Sensor-/Monte-Carlo-/Sprachfälle bestehen auf Intel
macOS unter `build/contact-world-language-release-mac/test-results/run-2_wbxiq7`
und Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-t24apbz3`.
Der frische Linux/Clang-Debuglauf mit ASan/UBSan besteht mit 9/9 unter
`build/base-contract-asan-linux/test-results/run-58n1v87k`. Dies umfasst Core,
Memory, Memory-Owners, Array, Hashmap, String-View, Logging, Measurement und die
C-/Physim-RNG-Referenz. Die separate direkte C-Probe bestätigt bei sd=0 genau
zwei Ziehungen auf beiden Systemen (`build/rng-zero-probe-*.log`).
Ein neuer vollständiger Gesamtlauf sämtlicher Fälle wird damit nicht behauptet.

Die erweiterten Handbuchseiten und Tastaturnavigation bei 16/22 px bestehen
jeweils mit 3/3 auf macOS unter `run-znrkp4yn` und Linux unter `run-3ea1l5qg`.
Alle 29 Referenzseiten sind aktuell. Der Katalog besitzt jetzt 612 Fälle ohne
Fenster, 593 ohne SDL und 77 Fensterfälle.

### Frische verschobene SDKs

Das vollständige isolierte SDK-Prüfverfahren ohne Fensterprüfungen besteht
auf beiden Systemen mit jeweils 360 erfolgreich ausgeführten Prüfbefehlen:

- macOS: `build/Base RNG SDK ä mac 6er9kgzl`, verschobenes SDK unter
  `proof/Native SDK ä 0qtgust5/Relocated SDK ä`.
- Linux: `build/Base RNG SDK ä linux qfr5yce6`, verschobenes SDK unter
  `proof/Native SDK ä 08t06e77/Relocated SDK ä`.

Das frische SDK besitzt 430 manifestierte Dateien, das unabhängige Kit 63
Eingaben. Die RNG-Referenzen laufen gegen das installierte **und** aus den 26
mitgelieferten Core-Modulen neu gebaute Archiv, jeweils in C und Physim.
Die bisherigen Domänen-, Lernweg-, Analyse- und nativen Projektbuild-Gates
bleiben aktiv. Receipts und Quellhashes stehen in
`build/base-contract-sdk-mac-PASSED.json`,
`build/base-contract-sdk-linux-PASSED.json` und
`build/base-contract-source-freeze.json`. Neue Windows-/Apple-Silicon- oder
SDK-Fensterprüfungen werden damit nicht behauptet.

### Weiterer CI-Abbruch nach der nativen Suite

Die [CI für `2564e38`](https://github.com/PhysicSimulator/physim/actions/runs/37621422129)
meldet Fehler im zusammengesetzten Schritt „Direct compiler build“. Die
geprüften Linux/GCC- und Apple-Silicon-Artefakte enthalten jeweils **611/611**
bestandene native Fälle; deren SHA-256 stimmt mit den veröffentlichten Digests
überein: `f91ca69bf965c1b6fac4a8e507ef8bfd73cf474df56bab723542800d3b04891a`
und `6e464e3601665ff577806574eab4458009da6555916a0a784e01a09c2eb8e7c4`.
Die absichtlich ausgelösten Sanitizerfehler der dortigen Probe sind Testdaten;
auch die Fuzzerkampagne ist erfolgreich.

Ein deterministischer nachfolgender Abbruch ist lokal belegt:
`test_native_test_runner.py` vergleicht den vollständigen Fensterkatalog mit
Python-Workflows und kennt die neue direkt gestartete negative Handbuchprüfung
nicht. Der unveränderte Prüfer scheitert an diesem Mengenvergleich unter
`build/base-contract-test-runner-audit.log`. Die erwartete Liste erfasst nun die
negative direkte Prüfung ausdrücklich und kontrolliert deren Exitcode und
Diagnose. Fehlende/zusätzliche künftige Fälle bleiben Fehler, mit konkreter Liste.
Die vollständige Selbstprüfung besteht auf macOS und Linux unter
`base-contract-test-runner-final-*.log`; ihre absichtlich fehlschlagende
Drei-Fälle-Suite ist kein Produktfehler. Weitere mögliche Fehler des
zusammengesetzten Remote-Schritts und die neue Gesamtmatrix bleiben zu bestätigen.


## Materialeigenschaften als SI-Daten am 7. Oktober 2026

`properties.h` und der neue Core-Baustein `properties.c` beschreiben eine
Eigenschaft durch Quelle, Einheit, geschlossenen Temperatur-/Druckbereich und
konstantes oder tabellarisches Modell. 1–64 Achsenpunkte, maximal 4096 Werte,
Einpunktachsen für unabhängige Koordinaten, keine Extrapolation und eine
beschränkte konvexe Interpolation bilden den expliziten Vertrag.
[Verträge und vollständige C-/Physim-Quellen](properties.md) verwenden synthetische
Referenzdaten; es wird kein gemessener Stoffdatensatz erfunden. Beide wirklichen
Experimentsprachen speichern Quellen, Bereich, Einheit, Achsen, Eckwerte und
Methode im Lauf. Die unabhängige Prüfung liest CRC, Schema, sämtliche Messwerte
und Metadaten und kontrolliert alle vier Analyse-Kombinationen.
API/ABI 3 und Datenformate bleiben erhalten; die Library und der native
Projektbuilder verwenden jetzt denselben Satz von **27 Core-Modulen**.

### Lokale Builds, Referenzen und genaue Reichweite

Die vollständigen Release-Suiten bestehen mit **620/620** auf Intel macOS unter
`build/contact-world-language-release-mac/test-results/run-i9b1hv9g` und
Debian/GCC unter `build/contact-world-language-release-linux/test-results/run-yx2gke1o`.
Danach wurde die zusätzliche kalte Eigenschaftsprojekt-Prüfung registriert und
eine Header-Abhängigkeit korrigiert. Diese ursprünglichen Gesamtläufe werden
damit nicht zu einem neuen 621-Fälle-Lauf umbenannt.

Die endgültigen zehn gezielten Fälle bestehen unter macOS `run-of580rj2` und
Linux `run-0breuqjc`. Sie umfassen C-Eigenschaften, fünf Physim-Wert-/Fehlerfälle,
den tatsächlichen Experiment-/Analyseablauf, kalte native C-/Physim-Projekte,
Referenzseiten und das isolierte Kit. Der separate Quellenvergleich der vier
publizierten Programmquellen besteht ebenfalls. Die C-Prüfung verwendet über
10000 unabhängige T/P-Paare, einen nichtquadratischen 3×2-Aufbau mit echtem
T·P-Term, Extremwerte und das maximale 64×64-Raster. Quellen und Arrays bleiben
bei fehlgeschlagenen Abfragen erhalten; Ausgaben werden nicht teilweise geändert.

Die kalten Eigenschaftsprojekte samt unverändertem Cache und Quellen bestehen
separat unter macOS `run-t_bp6a00` und Linux `run-_k3_pnz7`. Unter Linux/Clang
bestehen die sieben neuen C-/Physim-Eigenschaftsfälle einschließlich des wirklichen
Analyseablaufs mit ASan/UBSan unter
`build/base-contract-asan-linux/test-results/run-i81ymy_i`.
Die vier Handbuchfenster-/Tastaturfälle (alle Themen, 16 px, 22 px, langsames
Zeichnen) bestehen unter macOS `run-cvq8czm3` und Linux `run-s4k2glpn`.
30 Referenzseiten und das isolierte Kit mit 67 Eingaben sind geprüft.
Der Katalog enthält jetzt 621 Fälle ohne Fenster, 601 ohne SDL und 77 Fensterfälle.

### Erhaltene Fehlversuche

Die erste Sprachfixture verwendete eine nicht vorhandene `unitOne`-Funktion
und einen falschen Optionalzugriff (`run-fwo1icnr`); beides wurde auf die vorhandene
Unit-/attempt-Syntax korrigiert. Die ersten SDK-Läufe
`Material properties SDK ä mac p9epbn11` und
`Material properties SDK ä linux f1f3gvqi` scheitern an der eigenständigen
Einbindung von `language_properties.h`: `ps_vec4` war nur über den Sammelheader
bekannt. Der eigene Header bindet `math.h` jetzt selbst ein. Die fehlgeschlagenen
Läufe bleiben erhalten und werden nicht als erfolgreiche SDK-Nachweise gezählt.

Die erste Registrierung der kalten Eigenschaftsprojekte referenzierte den
falsch benannten Oracle `test_property_tutorial.py`. Die Projekte bauen bereits,
doch der Folgeprüfer startet nicht; die Läufe `run-nfhq9k7k` und `run-uflqhux9`
bleiben mit 10/11 fehlgeschlagen. Die Registrierung verwendet jetzt den tatsächlich
vorhandenen `test_property_workflow.py`; die Folgeprüfungen oben bestehen.

### Frische verschobene SDKs mit der Korrektur

Beide vollständigen isolierten SDK-Prüfverfahren ohne Fensterprüfungen bestehen
mit jeweils **370 erfolgreichen Prüfbefehlen**:

- macOS: `build/Material properties SDK ä mac 9yakyf10`, verschobenes SDK unter
  `proof/Native SDK ä bptlzb_k/Relocated SDK ä`.
- Linux: `build/Material properties SDK ä linux 0d75c6hg`, verschobenes SDK unter
  `proof/Native SDK ä eiggnpef/Relocated SDK ä`.

Das SDK besitzt 441 SHA-256-erfasste Dateien. Der Prüfer verwendet ein isoliertes
Kit mit 67 Eingaben; Header und alle 27 Core-Module stammen ausschließlich aus
dem SDK. Die neuen C-/Physim-Eigenschaften und gespeicherten Metadaten bestehen
gegen das installierte und das neu gebaute Archiv. Zehn kalte native Projekte
(davon die beiden neuen Eigenschaftsprojekte), unveränderter Cache und fünf
vollständige unabhängige Domänenorakel bestehen. Alle früheren SDK-Gates bleiben
aktiv. Receipts und 27 eingefrorene Quellen stehen in
`build/properties-sdk-mac-PASSED.json`, `build/properties-sdk-linux-PASSED.json`
und `build/properties-source-freeze.json`. Das belegt diese lokalen Systeme und
diesen Umfang, keine neue Windows-/Apple-Silicon-Gesamtabnahme.

Die letzte Abfrage der
[früheren C17-CI für `915e431`](https://github.com/PhysicSimulator/physim/actions/runs/37625776693)
bestätigt alle vier Windows-Jobs und Linux/Clang; macOS und Linux/GCC laufen
noch. Die [frische Linux-Paket-CI](https://github.com/PhysicSimulator/physim/actions/runs/37625776779)
besteht vollständig einschließlich der installierten Debian-/Ubuntu-Prüfungen.
Diese Ergebnisse gelten für die frühere Revision. Die nächste gesamte Matrix
mit dem neuen Eigenschaftsbaustein bleibt erforderlich.

## Homogenes Van-der-Waals-Modell am 7. Oktober 2026

Dieser begrenzte reale Gasbaustein ergänzt das bestehende Thermodynamikmodul;
Core bleibt bei 27 Modulen, API/ABI 3, Wire 5, Snapshot 3 und psrun 1 bleiben
unverändert. Vier C-/Physim-Funktionen berechnen Druck, Druckableitung,
innere Energie und Entropiedifferenz mit konstanten molaren SI-Koeffizienten.
[Modell, Primärquelle und vollständige Beispiele](real-gas.md) dokumentieren
insbesondere die fehlende Phasenauswahl und Kalibrierung. PP-0412 bleibt
unvollständig; der gesamte Projektplan ist weiterhin nicht abgenommen.

### Tatsächlich ausgeführte gezielte Prüfungen

Die acht Release-Fälle bestehen auf macOS/Apple Clang unter `run-rds54d3j`
und auf Debian 12/GCC unter `run-c481q_od`. Enthalten sind C-Grenzfälle,
Physim-Werte/attempt, das bestehende Thermodynamikmodul, eine unabhängige
60-stellige Decimal-Referenz für reale aufgezeichnete C-/Physim-Läufe, vier
gemischte Analysen, zwei kalte native Projekte samt unverändertem Cache,
publizierte Quellen, Referenzseiten und das isolierte Kit.
Die Läufe speichern sieben geprüfte SI-Kanäle und synthetische Koeffizienten;
sie setzen die Kompression nach einer Sekunde auf ein festes Volumen fort.
Es werden keine gemessenen Gasdaten behauptet.

Vier Fälle bestehen zusätzlich unter Linux/Clang mit ASan/UBSan in
`build/base-contract-asan-linux/test-results/run-homy18es`: C-Gasmodell,
Physim-Bindungen, bisherige Thermodynamik und der wirkliche Experiment-/Analyseablauf.
Die C-Prüfung enthält idealen Grenzfall, Druckableitung per endlicher Differenz,
Stoffmengen-/Volumenskalierung, Entropieumkehr, mechanisch instabile homogene
Algebra, negative Ausgaben, Domainfehler und endliche Rechnung trotz separatem
Überlauf der Energieterme. Bei Fehlern bleibt die Ausgabe erhalten.

Vier Handbuchprüfungen bestehen je Plattform: sämtliche Themen, Tastatur mit
16/22 px und künstlich verzögerte Tastaturnavigation. Belege liegen unter
`build/real-gas-handbook-mac-wlhwzc34` und auf Linux unter
`build/real-gas-handbook-linux-ssktfa0m`; der Linux-Receipt ist zusätzlich als
`build/real-gas-handbook-linux-PASSED.json` lokal erhalten. 30 generierte
Referenzseiten bestehen einschließlich neuer C-/Physim-Einträge.

### Vollständige verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **380 erfolgreichen Prüfbefehlen**:

- macOS: `build/Real gas SDK ä mac nu580m90`, verschoben unter
  `proof/Native SDK ä zwjx2pql/Relocated SDK ä`.
- Linux: `build/Real gas SDK ä linux d33yv2j1`, verschoben unter
  `proof/Native SDK ä _7m5hk7u/Relocated SDK ä`.

Jedes SDK enthält 448 SHA-256-erfasste Dateien; das isolierte Kit hat 71 Eingaben.
Alle 27 Core-Module und öffentlichen Header stammen aus dem SDK. Das neue
Gasmodell und seine Physim-Bindungen bestehen gegen das installierte und das
neu gebaute Archiv. Zwölf kalte native C-/Physim-Projekte einschließlich beider
Gasprojekte, unveränderter Cache und sechs unabhängige Domänenorakel bestehen.
Alle früheren SDK-Gates bleiben erhalten. Receipts stehen in
`build/real-gas-sdk-mac-PASSED.json` und `build/real-gas-sdk-linux-PASSED.json`.
26 Implementierungs-/Test-/Dokumentationsdateien sind in
`build/real-gas-source-freeze.json` festgehalten. Nach den SDK-Läufen wurde nur
Text im abschließenden Verifier-Receipt auf zwölf Projekte und die tatsächlich
geprüften Material-/Gasfälle berichtigt; die geprüften Operationen sind identisch.
Das erneut paketierte Kit hat ebenfalls 71 Eingaben.

### Erhaltene Fehlversuche und Grenzen

Die erste Sprachbindung rief `psrt_raise` ohne dessen viertes Argument auf
(`run-41b7k717`); der Aufruf wurde auf den vorhandenen Vertrag korrigiert.
Die ersten C-Experiment-/Projektfälle (`run-1h0gckvs`) zeigten die fehlende
eigenständige Einbindung von `units.h` für `PS_PASCAL`. Beide C-Beispiele binden
sie nun ausdrücklich ein. Die erfolgreichen Folgeprüfungen verwenden diese
Korrekturen. Zwei früh gestartete SDK-Läufe (`l5dn54s5`, `en_fcx5k`) wurden vom
Build-Lock abgewiesen; ihre Logs bleiben erhalten. Die erfolgreichen SDK-Läufe
starteten erst nach Abschluss der vorherigen Builds.

Diese Belege gelten für den lokalen Intel-Mac und Debian-VM-Lauf. Sie beweisen
keine neue Windows-/Apple-Silicon-Gesamtabnahme, kein Phasengleichgewicht und
keine Stoffkalibrierung. Der Katalog enthält jetzt 626 Fälle ohne Fenster und
77 Fensterfälle; Katalogzahlen allein sind keine bestandenen Gesamtläufe.

### Abschließende Gesamtläufe

Die tatsächlichen Release-Gesamtläufe mit diesem Baustein bestehen auf beiden
lokalen Systemen vollständig, jeweils **626/626**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-ngt3aqiy`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-w7lsv7gh`.

Sie enthalten auch die früheren Compiler-, Runtime-, Runner-, Batch-, Format-,
Domänen- und Projektbuildfälle. Der Linux-Ergebnisreceipt ist zusätzlich als
`build/real-gas-full-linux-results.json` lokal erhalten. Die zuvor genannten
acht gezielten Fälle, Sanitizer- und Handbuchprüfungen sind separate Belege;
sie werden nicht zu einer erfundenen größeren Testsuite zusammengezählt.

## Gemeinsame SI-Kanaldeklaration am 7. Oktober 2026

Der Einzelabgleich der Einheitenanforderungen deckt einen konkreten Unterschied
zwischen C und Physim auf: `ps_channel_add` nahm in C eine Skala ungleich 1 an,
speicherte aber nur Dimensionen und Symbol. Die Skala ging damit verloren.
Die unveränderte Ausgangsrevision `d9b167a` scheitert reproduzierbar an der neuen
C-Prüfung: Ein Zentimeterkanal wird angenommen (`run-58yp6qw6`). Das ist ein
belegter Fehler an dieser API-Grenze, keine Aussage über alle historischen Läufe.

C und Physim verwenden nun denselben Deklarationsvalidator: Skala exakt 1,
begrenzte UTF-8-Metadaten ohne Kürzung, eindeutiger nichtleerer Name und höchstens
16 Kanäle. Beschreibungen erlauben Tabs/Zeilenumbrüche. Fehler ändern keinen
Kontextwert und verbrauchen keinen Slot. Texte werden vor der Zuweisung kopiert,
auch wenn Eingaben auf den eigenen Zielslot zeigen. Das bestehende Rohschema-
und Dateiformat bleibt erhalten. [Vertrag und ausdrückliche SI-Umrechnung](numerics.md)
unterscheiden Kanalwerte von Anzeigeeinheiten und physikalischer Bedeutung.
Eine nackte Zahl kann die Bibliothek nicht auf ihre richtige Einheit prüfen.

### Gezielte tatsächlich ausgeführte Nachweise

Je **22/22** Release-Fälle bestehen auf dem Intel-Mac/Apple Clang
(`run-ixa2ludj`) und Debian 12/GCC (`run-83jdka8c`). Sie enthalten Kern-/Numerik-,
Kanal-/Anzeigeeinheiten-, Runner-, Katalog-, Referenz- und Kitprüfungen. Die neuen
Fälle behandeln exakte Byte-Grenzen, UTF-8/Steuerzeichen, Duplikate, nichtkanonische und nichtendliche
Skalen, volle/ungültige Zähler, Input-Aliasing und unveränderte Fehlerausgaben.

Tatsächliche C-/Physim-Experimentmodule weisen ungültige Deklarationen ab und
speichern nach ausdrücklicher Konvertierung von 125 cm exakt 1,25 m.
Ein unabhängiger Python-Prüfer liest Magic, CRCs, zwei Kanalschemata samt sieben
SI-Exponenten, drei reale Zeilen und Footer und vergleicht den fertigen CSV-Export.
Er bestätigt die kompakten Indizes nach abgefangenen Fehlern und gleiche Werte
in beiden Sprachen. Der CSV-Prüfer benutzt die öffentliche Export-API, keinen
nicht vorhandenen Runner-Schalter.

Vier Fälle bestehen zusätzlich unter Linux/Clang mit ASan/UBSan:
`build/base-contract-asan-linux/test-results/run-93fmxd0p`. Sie umfassen Core,
Numerik, C-Deklarationen und den echten C-/Physim-Runner-/CSV-Ablauf.
Linux-Receipts liegen zusätzlich lokal unter `build/channel-targeted-linux-results.json`
und `build/channel-asan-linux-results.json`. 30 Referenzseiten bestehen.

Die erste Sprachfixture nutzte eine in diesem Bindungskontext nicht globale
`convert`-Funktion (`run-2ibf7ctf`). Die korrigierte Quelle verwendet die vorhandene
Methode `centimetre.convert(125,metre)` und besteht unter `run-j404182x` sowie in
allen gezielten und SDK-Folgeläufen. Fehlversuche und Quellen bleiben erhalten.

### Vollständige frische und verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **385 erfolgreichen Prüfbefehlen**:

- macOS: `build/Canonical channels SDK ä mac 7itzrr5z`, verschoben unter
  `proof/Native SDK ä 0n6v3i18/Relocated SDK ä`.
- Linux: `build/Canonical channels SDK ä linux 9gpwkv9q`, verschoben unter
  `proof/Native SDK ä aee01x5u/Relocated SDK ä`.

Je 448 SDK-Dateien und 76 isolierte Kit-Eingaben sind SHA-256-erfasst. Alle
27 Core-Module und öffentlichen Header stammen aus dem SDK. Die C-Grenzfälle
und tatsächlich aufgezeichneten C-/Physim-Läufe samt CSV bestehen gegen das
installierte und das aus SDK-Quellen neu gebaute Archiv. Alle früheren Gates,
zwölf kalte native Projekte, unveränderte Cache-Wiederverwendung und sechs
vollständige unabhängige Domänenorakel bleiben aktiv. Receipts stehen in
`build/channel-declaration-sdk-mac-PASSED.json` und
`build/channel-declaration-sdk-linux-PASSED.json`. Alle 19 in
`build/channel-declaration-source-freeze.json` festgehaltenen Implementierungs-,
Test- und Dokumentationsdateien sind während beider Prüfungen unverändert.
API/ABI 3, Wire 5, Snapshot 3 und psrun 1 bleiben unverändert.

PP-0372 bleibt für sämtliche API-/Analysegrenzen ungeprüft. Auch die gesamte
Einheiten-, Windows-/Apple-Silicon- und Produktabnahme bleibt offen. Die letzte
Abfrage der [C17-CI für die vorherige Revision `d9b167a`](https://github.com/PhysicSimulator/physim/actions/runs/37641732728)
zeigt drei erfolgreiche Windows-Jobs; die übrigen Jobs laufen noch. Das ist kein
Windows-Nachweis der aktuellen Kanalkorrektur.

### Abschließende Gesamtläufe der Kanalkorrektur

Die tatsächlichen Release-Gesamtläufe bestehen vollständig, jeweils **628/628**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-5f9izwxu`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-ovk8fbvv`.

Sie enthalten die früheren Compiler-, Runtime-, Format-, Runner-, Batch-,
Domänen- und Projektbuildfälle sowie beide neuen Kanaldeklarationsfälle.
Der Linux-Receipt ist zusätzlich lokal unter `build/channel-full-linux-results.json`
erhalten. Die 22 gezielten Fälle, vier Sanitizerfälle und beide SDK-Prüfungen
sind separate Nachweise; sie werden nicht zu einer erfundenen Gesamtsuite
zusammengezählt. Die Fensterfallzahl im Katalog bleibt 77; dieser Schritt
behauptet keine erneut ausgeführte gesamte Grafikabnahme.

## Normierte und kompensierte Quantity-Summen am 7. Oktober 2026

Eine neue C-Regression am unveränderten Ausgangsstand `934d5fc` reproduziert
`PS_NUMERIC` für die endliche Summe `-2^1023 m + 2^1023 (Skala 2 m)`
(`run-tq5m9snf`). Die Einzelkonvertierung läuft zuvor über. C und Physim
kombinieren jetzt normierte Operanden und Konvertierungs-/Summenreste vor der
Rückskalierung. Subnormale Ergebnisse werden auf dem endgültigen Gitter gerundet;
sehr kleine Beiträge können dadurch einen Rundungsgleichstand beeinflussen.
[Vertrag, Primärquelle und Grenzen](numerics.md) nennen die normale Rundung zur
nächsten Zahl, Alias-/Fehlervertrag und fehlende allgemeine Exaktheitsgarantie.
Die linke Einheit und ihr geliehenes Symbol bleiben erhalten. Eigenständige
Konvertierung sowie Produkt-/Quotientenoperationen behalten ihren Vertrag.

### Tatsächlich ausgeführte gezielte Nachweise

Je **16/16** Release-Fälle bestehen auf dem Intel-Mac/Apple Clang
(`run-2uvhcho3`) und Debian 12/GCC (`run-qonkc1zv`). Sie enthalten C-Grenzfälle,
bestehende Quantity-Operatoren/-Fehler, numerische Kernprüfungen, Kanaldeklarationen,
Anzeigeeinheiten, Referenz und isoliertes Kit. Der neue Prüfer benutzt Python
`Fraction` für exakte rationale Werte der tatsächlich eingelesenen Double-Eingaben:
**20038 C-Eingaben und 38 tatsächliche Physim-Operatorfälle**. Die gezielten 38
Fälle verlangen exakt den aus der rationalen Referenz gerundeten Double-Wert;
in beiden Sprachen werden erfolgreiche Werte, Fehler und linke Skala geprüft.

Die erzeugten Fälle decken 10000 dyadische Skalenverhältnisse und 10000 weitere
breit verteilte endliche Werte/Skalen ab. Dafür werden Bereich und dokumentierte
Double-Abweichung unabhängig geprüft; unvermeidbare Endrundung wird bei der
Fehlerschranke berücksichtigt. Dies ist keine Aussage über sämtliche möglichen
Double-Paare. Die C-Prüfung verlangt zusätzlich volle unveränderte Fehlerausgaben,
Symbol-/Dimensionsintegrität, Aliasverhalten, Auslöschung und vorzeichenbehaftete
Null. Subnormale Fälle auf und beiderseits einer halben Gitterweite sind enthalten.

Unter Linux/Clang mit ASan/UBSan bestehen die elf ausgewählten Fälle in
`build/base-contract-asan-linux/test-results/run-s2j2pkmr`. Linux-Receipts sind
zusätzlich unter `build/quantity-targeted-linux-results.json` und
`build/quantity-asan-linux-results.json` lokal erhalten. 30 Referenzseiten bestehen.

Die erste einfache normierte Version besteht die Überlaufregression, verliert
aber bei einer Subnormal-Grenze den Rundungsrest. Dieser konkrete Probe führte
zur Kompensation und abschließenden Gitterrundung. Der erste kompensierte
Oracle-Lauf (`run-nzu0y43_`) scheitert an einer zu engen Oracle-Fehlerschranke,
die unvermeidbare Subnormal-Endrundung nicht berücksichtigt. Die Schranke ist nun
mindestens eine halbe finale ULP; die 38 gezielten Fälle verlangen zusätzlich
exakte Gleichheit zur rationalen Referenz. Die erfolgreichen Folgeprüfungen
verwenden diese strengeren gezielten und korrigierten allgemeinen Prüfungen.
Fehlversuche und Quellen bleiben erhalten.

### Vollständige verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **390 erfolgreichen Prüfbefehlen**:

- macOS: `build/Quantity sums SDK ä mac cv7rk3x8`, verschoben unter
  `proof/Native SDK ä eegxtvz7/Relocated SDK ä`.
- Linux: `build/Quantity sums SDK ä linux p5n8cppa`, verschoben unter
  `proof/Native SDK ä gmosenm4/Relocated SDK ä`.

Je 448 SDK-Dateien und 81 Eingaben im isolierten Kit sind SHA-256-erfasst.
Die neuen C-/Physim-Summen bestehen gegen installierte und aus SDK-Quellen
neu gebaute Archive. Alle früheren Gates, zwölf kalte native Projekte, unveränderter
Cache und sechs unabhängige Domänenorakel bleiben aktiv. Die 18 in
`build/quantity-sum-source-freeze.json` festgehaltenen Implementierungs-/Test-/
Dokumentationsdateien waren während beider Prüfungen unverändert. Receipts liegen
in `build/quantity-sum-sdk-mac-PASSED.json` und `build/quantity-sum-sdk-linux-PASSED.json`.
Der anschließende Planabgleich ergänzt ausschließlich den unten belegten Series-
Gegenbefund. API/ABI 3, Wire 5, Snapshot 3 und psrun 1 bleiben unverändert.

### Offene Analysegrenze und Windows-Nachweis

Ein tatsächlicher C-Probe gegen das frisch installierte macOS-SDK addiert eine
Series mit 1 m und eine ausgerichtete Series mit 50 cm als **51 statt 1,5 m**.
`import_values` speichert rohe Werte mit ihrer Skala; `ps_series_combine` prüft
Dimensionen, berücksichtigt beim Addieren aber die rechte Skala nicht.
Quellen und Ergebnis stehen in `build/series-unit-scale-probe.c`,
`build/series-unit-scale-probe.txt` und `build/series-unit-scale-audit.json`.
PP-0370 ist deshalb jetzt konkret unvollständig. Der Plan enthält unverändert
24 implementierte, nun sechs unvollständige und 501 ungeprüfte Blöcke.
Diese Series-/SI-Speicherlücke wird als nächstes bearbeitet; die Quantity-Korrektur
ist keine gesamte Einheiten-/Analyseabnahme.

Der [Windows-v143-Debug-Job der vorherigen Revision `934d5fc`](https://github.com/PhysicSimulator/physim/actions/runs/37645634144/job/112875535813)
ist fehlgeschlagen. Seine öffentlichen Check-Annotationen melden mehrfach ein
nicht mehr vorhandenes Laufwerk für Runner-Dateien auf `D:` und einen Prozess-
Exitcode 1. Der Compilerlog ist über die öffentliche API nicht abrufbar (403).
Daraus wird weder ein konkreter Produktcodefehler noch ein bestandener Job
abgeleitet. Die neue gesamte Windows-/Apple-Silicon-/Plattformabnahme bleibt offen.

### Abschließende Gesamtläufe der Quantity-Korrektur

Die tatsächlichen Release-Gesamtläufe bestehen auf beiden lokalen Systemen
vollständig, jeweils **630/630**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-ptuom6qd`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-hp1counu`.

Sie enthalten die früheren Compiler-, Runtime-, Format-, Runner-, Batch-,
Domänen- und Projektbuildfälle sowie beide neuen Quantity-Summenfälle.
Der Linux-Receipt ist zusätzlich unter `build/quantity-full-linux-results.json`
lokal erhalten. Die 16 gezielten Fälle, elf Sanitizerfälle und SDK-Läufe sind
separate Nachweise. Die 77 Fensterfälle im Katalog sind in diesem Schritt
nicht als erneut ausgeführte Grafikgesamtabnahme ausgewiesen. Der weiterhin
belegte Series-Skalierungsfehler wird durch diese grünen Tests nicht widerlegt;
der Katalog erhält dafür als nächsten Schritt einen eigenen Gegenbeleg.

## Kanonische SI-Series am 7. Oktober 2026

Die neue C-Regression am unveränderten Ausgangsstand `00fec46` reproduziert
**1 m + 50 cm = 51 statt 1,5 m** (`run-z8v7c6hg`). Eigene Series speicherten
Rohwerte mit Eingabeskala; die Addition prüfte Dimensionen und addierte die
Rohzahlen ohne Skalenumrechnung. Beide Importfunktionen übernehmen deklarierte
Eingabeeinheiten jetzt blockweise nach SI. Eingabearrays bleiben erhalten;
Ausgabe-Metadaten und Werte verwenden Skala 1 und SI-Dimensionssymbole.

[Vertrag und Umstellung](series.md) beschreiben die absichtliche Änderung von
`value()/unitScale()`: Bestehende Analysen müssen eine anschließende eigene
Umrechnung entfernen. Die bisherige Physim-Referenzfixture erwartet für einen
Zentimeterimport nun Skala 1 und Werte 0,02/0,03 statt 2/3. Sie bleibt Teil
der tatsächlichen Sprach-/Analyseprüfung; der geänderte Wertvertrag wird nicht
mit einer bloßen Korrektur der fehlerhaften Summe verdeckt. Historische Laufdateien
werden nicht umgeschrieben. Plot-/Report-Anzeigeeinheiten bleiben eine ausdrückliche
Darstellungskonvertierung und ändern die Series nicht.

### Tatsächlich ausgeführte gezielte Nachweise

Je **21/21** Release-Fälle bestehen auf dem Intel-Mac/Apple Clang
(`run-mbago8z8`) und Debian 12/GCC (`run-ug7tw11k`). Sie enthalten eigene und
Dataset-Series, Masks, Auswahl/Quantile, alle Resampling-Verfahren, PCHIP,
numerische Extremfälle, die bestehende Physim-Analyse, Referenz und isoliertes Kit.
Unter Linux/Clang mit ASan/UBSan bestehen die 19 ausgewählten Fälle in
`build/base-contract-asan-linux/test-results/run-8pf3i131`. Linux-Receipts sind
zusätzlich lokal als `build/series-si-targeted-linux-results.json` und
`build/series-si-asan-linux-results.json` erhalten.

Die neue C-Prüfung verarbeitet **513 Zeilen über mehrere 256er-Blöcke** aus
cm-/ms-Eingaben. Sie prüft SI-Import, Addition/Subtraktion/Produkt/Quotient,
Affine, Ableitung, Integral mit nichtkanonischem Anfangswert, Prozent-Selektoren,
Masken, leere Reihen sowie **1025 Ziele** für linear/nearest/previous/PCHIP.
Die affine Rechnung verwendet Quantity-Addition pro gültiger Zeile, sodass ein
allein nichtdarstellbarer Offset noch eine endliche Summe ergeben kann. Maskierte
Zeilen werden nicht ausgewertet; factor*value muss selbst endlich sein.

Rangefehler im letzten Importblock, nichtdarstellbarer nichtnulliger Unterlauf
und erschöpfte Quota erhalten Ausgabehandles, vorhandene Werte und belegte
Scratch-Bytes. Der unveröffentlichte Entwurf wird geschlossen. Ein tatsächliches
Physim-Analysemodul prüft dieselben Eingaben und skalierte Einheiten, abgefangene
Rangefehler sowie lineare/PCHIP-Ausgaben. Ein unabhängiger 64-stelliger Decimal-
Prüfer liest beide fertigen CSVs: alle 513 Zeilen, fünf Spalten und kanonische
Kopfzeilen. Ein separater Report-Prüfer lädt beide Berichte und prüft jede
Kurvenkoordinate sowie alle sieben Dimensionsfelder und Skala 1.

### Erhaltene Fehlversuche

Die erste neue Sprachfixture verwendete nicht unterstützte Series-Operatoren
(`run-4b_irekn`) und danach in diesem Kontext nicht globale Methodennamen
(`run-8hwak2fh`). Sie verwendet nun die vorhandenen adding/subtracting/multiplied/
divided-Methoden und besteht in allen Folgeprüfungen. Der zusätzliche Nearest-
Test nahm anfangs den späteren Punkt am Abstandsgleichstand an (`run-p_xpc4fm`);
laut bestehendem Headervertrag ist der frühere Punkt richtig. Die Erwartung
wurde korrigiert, das Verfahren bleibt erhalten. Der erste breitere Lauf
(`run-o2v_sw8e`) besteht funktional mit 20/21, scheitert aber an noch nicht
regenerierten Referenzseiten. Die endgültigen 21/21 verwenden die aktualisierten
30 Referenzseiten. Alle Fehlversuche und Quellen bleiben erhalten.

### Vollständige verschobene SDKs

Beide vollständigen isolierten SDK-Prüfungen ohne Fensterprüfungen bestehen
mit jeweils **393 erfolgreichen Prüfbefehlen**:

- macOS: `build/Series SI SDK ä mac 198cckdp`, verschoben unter
  `proof/Native SDK ä 3pbff4ir/Relocated SDK ä`.
- Linux: `build/Series SI SDK ä linux gzozrs0z`, verschoben unter
  `proof/Native SDK ä ep8tyb0h/Relocated SDK ä`.

Je 448 SDK-Dateien und 85 Kit-Eingaben sind SHA-256-erfasst. C-/Physim-SI-Import,
Analysen und Report-/CSV-Ausgaben bestehen gegen das installierte und das aus
SDK-Quellen neu gebaute Archiv. Alle früheren Gates, zwölf kalte native Projekte,
unveränderter Cache und sechs unabhängige Domänenorakel bleiben aktiv.
Die 18 in `build/series-si-source-freeze.json` festgehaltenen Implementierungs-/
Test-/Dokumentationsdateien waren während beider SDK-Prüfungen unverändert.
Receipts: `build/series-si-sdk-mac-PASSED.json` und
`build/series-si-sdk-linux-PASSED.json`. API/ABI 3, Wire 5, Snapshot 3 und psrun 1
bleiben unverändert; die numerische Importsemantik ist ausdrücklich dokumentiert.

Der konkrete Gegenbefund zu PP-0370 ist geschlossen. Der Block kehrt zu
`unverified` zurück, weil sämtliche übrigen internen Speicherschichten noch
nicht einzeln gegen den gesamten Planumfang abgenommen sind. Aktuell 24
implementierte, fünf unvollständige und 502 ungeprüfte Blöcke. Die letzte Abfrage
der [C17-CI der vorherigen Quantity-Revision `00fec46`](https://github.com/PhysicSimulator/physim/actions/runs/37651910156)
bestätigt Windows/MSVC und Windows/ClangCL in Debug; übrige Jobs laufen noch.
Das ist kein Windows-Nachweis der aktuellen SI-Series-Änderung. Die gesamte
Einheiten-, Grafik- und Plattformabnahme bleibt offen.

### Abschließende SI-Series-Gesamtläufe

Die tatsächlichen Release-Gesamtläufe bestehen vollständig, jeweils **632/632**:

- macOS/Apple Clang: `build/contact-world-language-release-mac/test-results/run-c052_5rb`.
- Debian 12/GCC: `build/contact-world-language-release-linux/test-results/run-uj9k60h2`.

Sie enthalten sämtliche bisherigen Compiler-, Runtime-, Runner-, Batch-, Format-,
Domänen- und Projektbuildfälle sowie beide neuen SI-Series-Fälle. Der Linux-
Receipt ist zusätzlich lokal als `build/series-si-full-linux-results.json`
erhalten. Die 21 gezielten Fälle, 19 Sanitizerfälle und SDK-Prüfungen sind
separate Nachweise. 77 Fensterfälle bleiben im Katalog; dieser Schritt behauptet
keine erneut ausgeführte gesamte Grafikabnahme. Die Semantikumstellung für eigene
Series bleibt trotz grüner Gesamtläufe ausdrücklich dokumentiert.


## Gewichtete ODE-Arithmetik (§7.2 / PP-0362)

Ausgangsrevision `f604d2a`. Ein getrennt gegen das vorherige installierte Archiv
gelinkter Probe liefert für `y'=1e308`, `y(0)=0`, `dt=1e-308` den Fehlercode 10
und Zustand 0 statt eines darstellbaren Zuwachses nahe 1. Die neue Regression
scheitert am unveränderten Stand in `run-oh2dqpir`. Die frühere RK4-Summe
`a+2b+2c+d` läuft vor Multiplikation mit der kleinen Schrittweite über.

Der gemeinsame interne Baustein kombiniert endliche Gewichte, Ableitungen und
Schrittweite mit kompensierten Summen und FMA. Bei problematischen Größenordnungen
verwendet er Mantissen/Exponenten. Euler, symplektischer Euler, RK4, Verlet und
RK45 samt Fehlerschätzung verwenden ihn. Echte nichtdarstellbare Stufen oder
Endzustände bleiben Fehler; die geprüften Schnittstellen erhalten ihre Zustände.
[Methoden, Einheiten und Grenzen](numerics.md) erklären weiterhin lokale statt
globale Fehlerschätzung, nichtsteife Probleme und gewöhnliche Double-Rundung.

### Gezielte Laufzeitprüfungen

Je **49/49** Release-Fälle bestehen auf Intel macOS/Apple Clang
(`run-6gcg9jz5`) und Debian 12/GCC (`run-j25bww01`). Unter Linux/Clang Debug
mit ASan/UBSan bestehen **44/44** (`run-ukgrsnk_`). Diese Läufe enthalten
Core/Numerics, tatsächliche Sprachprogramme, RK45-Diagnosen, Verlet sowie
bestehende nichtkonstante Pendel- und Konvergenzprüfungen.

Der zusätzliche Fraction-Prüfer vergleicht jeweils **244 C- und Physim-Fälle**
aller fünf Verfahren gegen unabhängige rationale Lösungen für konstante
Ableitungen/Beschleunigungen. Hex-Eingaben und die erzeugte Sprachfixture
bewahren dieselben binären Eingaben. Geprüft werden große und kleine Größen,
Vorzeichen, endliche Auslöschung und echte Rangefehler. Das Fehlerbudget ist
eine ausdrücklich begrenzte Prüfung, keine Garantie exakt gerundeter Summen.
Eine separate C-Prüfung enthält außerdem 32 Zustände und atomare Fehlerausgaben.

### Gemessene Kosten und erste SDK-Lücke

`tools/ode_range_benchmark.c` führt 200000 zurückgesetzte, eindimensionale RK4-
Schritte mit konstanter Ableitung aus. Auf diesem Intel-Mac ergeben sich als
CPU-Zeiten 0,005608 s am vorherigen Archiv, 0,036980 s mit durchgehend skalierter
Rechnung und 0,017237 s mit dem geprüften direkten Rechenweg für normale Größen.
Alle drei akkumulierten Werte sind 200,00000000059092. Das ist ein einzelner
synthetischer Vergleich: Der endgültige Weg kostet hier etwa das Dreifache des
vorherigen Wegs. Er beweist weder allgemeine Leistungsgleichheit noch ein
Leistungsbudget für vollständige Experimente. Die Messausgaben bleiben unter
`build/ode-cost-before.txt`, `ode-cost-after.txt` und `ode-cost-fast.txt` erhalten.

Der erste isolierte macOS-SDK-Versuch (`ODE range SDK ä mac _a6b6q2f`) entdeckt
einen fehlenden privaten Header beim Neubau der installierten Quellen.
`tools/build.py` übernimmt jetzt auch `src/ode_numeric.h` ins SDK. Dieser
Fehlversuch bleibt erhalten; die endgültige SDK-Abnahme benötigt einen frischen
vollständigen Lauf.

### Wiederherstellbare ältere Linux-Nachweise

Zwei abgeschlossene generierte Linux-Prüfverzeichnisse wurden nach vollständiger
Inventar-/SHA-256-Prüfung archiviert: `Base RNG SDK ä linux qfr5yce6` und
`Material properties SDK ä linux 0d75c6hg`. Sie liegen jetzt in
`build/archived-proofs-rng-properties.tar.gz` auf Linux und macOS. Das Archiv
enthält 28166 Dateien/Links, ist 602375720 Byte groß und hat SHA-256
`e4194290dadf4ca95fc095710f38ad7480955882bdf0c7975bba11ac663ebcc0`.
`build/archived-proofs-rng-properties.json` enthält das vollständige Inventar.
Alle Inhalte wurden vor Entfernen der beiden ausgepackten Linux-Verzeichnisse
gegen das Archiv geprüft; die Mac-Kopie wurde zusätzlich nach Größe/SHA geprüft.
Die historischen Nachweise bleiben damit wiederherstellbar.

Die Linux-Paket-CI der Ausgangsrevision `f604d2a`
([Lauf 37657198234](https://github.com/PhysicSimulator/physim/actions/runs/37657198234))
ist erfolgreich abgeschlossen. Ihre C17-Matrix läuft bei der letzten Abfrage
noch. Beide Aussagen betreffen die vorherige Revision und ersetzen keine
Windows-/Apple-Silicon- oder gesamte Grafikabnahme der ODE-Änderung.

### Korrigierte Linux-SDK-Abnahme und macOS-Gesamtlauf

Das korrigierte Linux-SDK besteht vollständig unter
`build/ODE range SDK ä linux lpk4sq92`, verschoben unter
`proof/Native SDK ä r8gpcgqf/Relocated SDK ä`. Alle **398 Prüfbefehle**
enden erfolgreich. 449 SDK-Dateien und 90 Kit-Eingaben sind SHA-256-erfasst.
`build/ode-range-sdk-linux-PASSED.json` und der zusätzlich lokal erhaltene
`build/ode-range-sdk-linux-verification.log` enthalten die Receipts.
Das Kit läuft außerhalb der Repositoryquellen und prüft sowohl das installierte
Archiv als auch den Neubau aus ausgelieferten SDK-Quellen. Die 21 eingefrorenen
Implementierungs-/Test-/Dokumentationsdateien bleiben während dieses Nachweises
unverändert. Die laufend ergänzte Plattformchronik ist nicht Teil dieses Freeze.

Der vollständige macOS/Apple-Clang-Release-Lauf besteht mit **634/634** unter
`build/contact-world-language-release-mac/test-results/run-r8al1p35`.
API/ABI 3, Wire 5, Snapshot 3, psrun 1 und Sprache 0.182.0 bleiben unverändert.
27 Core-Module und sämtliche bisherigen Compiler-, Runtime-, Format-, Batch-,
Projekt- und Domänenprüfungen bleiben enthalten. Die 77 Fensterfälle im Katalog
sind durch diesen Lauf ohne Fenster nicht erneut abgenommen.

Der vollständige Debian-12/GCC-Release-Lauf besteht ebenfalls mit **634/634**
unter `build/contact-world-language-release-linux/test-results/run-bzxu9w5y`.
Der zusätzliche lokale Receipt ist `build/ode-range-full-linux-results.json`.
Die beiden gezielten Linux-Receipts liegen lokal unter
`build/ode-range-targeted-linux-results.json` (49/49) und
`build/ode-range-asan-linux-results.json` (44/44). Beide vollständigen
Release-Läufe enden erfolgreich; die SDK- und Sanitizerprüfungen sind getrennte
Nachweise mit dem jeweils beschriebenen Umfang.

Die spätere Jobabfrage derselben C17-Matrix bestätigt Windows/v143 und
Windows/ClangCL jeweils Debug und Release erfolgreich. Der macOS-Intel-Job
[112915204504](https://github.com/PhysicSimulator/physim/actions/runs/37657198365/job/112915204504)
scheitert im SDK-/Relocation-Schritt, nachdem der direkte Build bestanden ist.
Die öffentliche Annotation nennt nur Exitcode 1; daraus folgt keine belegte
Ursache. macOS-ARM und beide Linux-Jobs laufen bei dieser Abfrage noch.
Receipts: `build/ode-range-prior-c17-jobs.json` und
`build/ode-range-prior-mac-annotations.json`. Der Fehler bleibt offen, bis
vollständige Logs oder ein entsprechender reproduzierbarer Gegenbefund vorliegen.
Die grünen lokalen Prüfungen ersetzen diesen Remote-Fehler nicht.

Der öffentliche Abruf der vollständigen Logs für den älteren macOS-Intel-Job
liefert HTTP 403 (`build/ode-range-prior-mac-log-response.txt`). Die Annotation
ersetzt keinen Compiler-/Testlog; es wurde daher keine spekulative CI-Korrektur
vorgenommen.

### Abschließender macOS-SDK-Nachweis

Das korrigierte macOS-SDK besteht vollständig unter
`build/ODE range SDK ä mac tk_g682x`, verschoben unter
`proof/Native SDK ä j3kb1qwy/Relocated SDK ä`. Auch hier enden alle
**398 Prüfbefehle** erfolgreich; 449 SDK-Dateien und 90 Kit-Eingaben sind
SHA-256-erfasst. Receipt: `build/ode-range-sdk-mac-PASSED.json`. Beide
SDK-Prüfungen verwenden die gleichen 21 eingefrorenen Dateien und alle 27
Core-Module. Gegen installierte und neu gebaute Archive bestehen die fünf
ODE-Verfahren, unabhängige C-/Physim-Referenzen, tatsächliche Lernpfade und
sämtliche bisherigen SDK-Gates.

Der endgültige Implementierungsnachweis für PP-0362 ist damit lokal auf Intel
macOS und Debian belegt. Der vollständige Plan bleibt mit 25 implementierten,
fünf unvollständigen und 501 ungeprüften Blöcken offen. Eine neue gesamte
Remote-/Grafik-/Produktabnahme wird durch diesen Schritt nicht behauptet.


## Lineare Gleichungssysteme (§7.2 / PP-0360)

Ausgangsrevision `03c42f6`. Ein getrennt gegen ihr vorheriges SDK-Archiv
gelinkter C-Probe bestätigt den Fehler im Dreieckssystem mit Zeilen
`(1,-1,1)`, `(0,1,0)`, `(0,0,1)` und rechter Seite `(1e308,1e308,1e308)`.
Die endliche Lösung `(1e308,1e308,1e308)` ergibt zuvor `PS_NUMERIC` und
unveränderte Ausgabe `(7,8,9)`. Die neue Regression scheitert am unveränderten
Stand in `run-1s9ckng5`; der separate Probe liegt in
`build/linear-range-before.c`/`.txt`. Auch die Elimination des Systems
`(1,1)`, `(-1,1)` mit rechter Seite `(1e308,1e308)` läuft früher über,
obwohl die Lösung `(0,1e308)` endlich ist. Die Ergebnisse aller 53 neuen
Eingabesysteme gegen das alte SDK sind unter
`build/linear-range-old-sdk-results.json` erhalten.

Die Koeffizienten bleiben zeilennormierte Double-Werte mit partieller
Pivotisierung. Jede rechte Seite und Teillösung erhält jetzt eine eigene
Mantisse/Exponent, kompensierte gewichtete Summen und eine abschließende
Rückskalierung. Das bewahrt auch gleichzeitig große und kleine Komponenten.
Die vorhandene Pivottoleranz und der Zustandserhalt auf allen Fehlern bleiben.
[Einheiten, Algorithmus und Grenzen](numerics.md) dokumentieren mögliche
Koeffizientenverluste bei der Normierung, schlechte Kondition, subnormale
Ergebnisrundung und die fehlende allgemeine Fehlerschranke.

Der unabhängige Prüfer löst 53 binäre Eingabesysteme durch rationale Elimination
und prüft zusätzlich ihre Residuen in C und tatsächlichem Physim. Die Systeme
haben 1–32 Unbekannte, starke Zeilenskalierung, Zeilentausch, Auslöschung,
Eliminationswachstum und echte Range-/Singularitätsfehler. Der separate C-Test
prüft Eingabeintegrität, Alias x=A/x=b, unabhängige Exponenten, subnormale Eingaben
und Zustandserhalt bei ungültigen Werten, Toleranz, Dimension oder Singularität.
Der erste breitere macOS-Lauf besteht mit 13/13 (`run-g253hgl5`); die endgültige
Prüfung ergänzt Dokumentation, Kit und zusätzliche C-Fehler-/Aliasfälle.

Die aktuelle Remote-CI der Ausgangsrevision
([C17 37666123958](https://github.com/PhysicSimulator/physim/actions/runs/37666123958),
[Linux-Paket 37666123932](https://github.com/PhysicSimulator/physim/actions/runs/37666123932))
läuft bei der ersten Abfrage noch. Die ältere fehlgeschlagene macOS-Intel-SDK-
Abnahme bleibt getrennt offen; die lokalen Nachweise ersetzen sie nicht.

### Gezielte endgültige Nachweise

Intel macOS/Apple Clang besteht **15/15** Release-Fälle in
`build/contact-world-language-release-mac/test-results/run-3eouq3yf`.
Debian/Clang Debug mit ASan/UBSan besteht **13/13** unter
`build/base-contract-asan-linux/test-results/run-j7kubmeb`. Die Läufe enthalten
Core, Numerics, sämtliche bestehenden Linear-Solve-Sprachfälle, beide neuen
C-/Physim-Prüfungen und die vollständigen ODE-Range-Referenzen. Die Release-
Auswahl enthält zusätzlich Referenz- und Kitprüfung. Eine lediglich unklare
Einrückung im stdin-Probe wurde anschließend korrigiert; die endgültigen
Gesamtläufe und SDKs verwenden diesen unverändert funktionalen Probe.

Die zusätzliche Linux/Clang-Debug-ASan/UBSan-Auswahl für Mathematik,
Matrixinversionen und Transformationen besteht mit **15/15** in
`build/base-contract-asan-linux/test-results/run-bfl5a3y7`. Das ist erforderlich,
weil Matrixinversionen denselben linearen Solver verwenden.

Eine spätere Jobabfrage der Ausgangsrevision `03c42f6` bestätigt beide
Windows/ClangCL-Jobs erfolgreich. Windows/v143 Debug scheitert dagegen im
separaten Crash-Dump-Nachweis
([Job 112945682723](https://github.com/PhysicSimulator/physim/actions/runs/37666123958/job/112945682723));
die öffentliche Annotation nennt nur Exitcode 1. Die übrigen Jobs laufen
bei dieser Abfrage noch. Receipts: `build/linear-audit-prior-ci-jobs.json`,
`build/linear-audit-prior-failed-job.json` und
`build/linear-audit-prior-failed-annotations.json`. Dieser Remote-Fehler
bleibt getrennt offen; die lokalen Solver-/SDK-Prüfungen ersetzen ihn nicht.

### Kompakte Sprachfixture mit identischen Eingaben

Die erste explizite Fixture erzeugt 67725 C-Zeilen mit 12535867 Byte. Der
Linux/GCC-Compiler arbeitet damit nachweislich über 17 Minuten bei nahezu
100 % CPU und ungefähr 1,5 GB RSS. Die Fixture wurde deshalb strukturell
verkleinert: Eine kleine Tabelle eindeutiger binärer Koeffizienten und ein
ASCII-Indextext rekonstruieren dieselben 53 vollständigen Matrizen. Der
Referenzprüfer bestätigt die identische Hex-Eingabefolge beim Erzeugen und
prüft weiterhin sämtliche Lösungen und Residuen in beiden Sprachen.
Die kompakte Fixture erzeugt 2105931 Byte C-Code. Kein Testfall, Koeffizient,
Toleranzwert oder Solververhalten wurde entfernt oder vereinfacht.

Der überholte Linux-Build wurde gezielt wegen dieser Fixture-Änderung beendet
(Exit 143), nicht aufgrund eines Beobachtungstimeouts. Prozess-/Ressourcendaten
liegen in `build/linear-fixture-expanded-termination.json`, sein erzeugter Code
in `build/linear-fixture-expanded-linux.c`. Der ursprüngliche Log bleibt unter
`build/linear-range-targeted-linux.log` erhalten. Die Solverquellen sind während
dieser Umstellung unverändert geblieben. Die kompakte macOS-Referenzprüfung
besteht in `build/linear-fixture-check-mac/test-results/run-duz3g303` (1/1).

Das vollständige macOS-SDK mit der ersten Fixture besteht mit **403 erfolgreichen
Prüfbefehlen**, 449 SDK-Dateien und 95 Kit-Eingaben unter
`build/Linear systems SDK ä mac izmu2853`, verschoben unter
`proof/Native SDK ä 7n_29a1f/Relocated SDK ä`. Receipt:
`build/linear-range-sdk-mac-PASSED.json`. Seine 15 ursprünglichen eingefrorenen
Dateien bleiben im Receipt und in `build/linear-range-source-freeze-first.json`
erhalten. Die einzige anschließende funktionale Prüferänderung ist die kompakte
Fixture samt deren Erzeuger; die Implementierung bleibt identisch.

Die kompakte Fixture besteht anschließend gegen beide bereits geprüften
macOS-SDK-Archive in `build/Linear compact SDK ä mac 66zz_exu`. Alle sechs
zusätzlichen Befehle bestehen: Emission durch den ausgelieferten Compiler,
zwei unabhängige Consumer-Builds, zwei rationale C-/Physim-Prüfungen und ein
frisches Kit mit denselben 95 erfassten Eingaben. Das SDK-Manifest bleibt
unverändert SHA-256-validiert. Receipt:
`build/linear-compact-sdk-mac-PASSED.json`. Die aktualisierten 15 Dateien sind
in `build/linear-range-source-freeze.json` erfasst; die Plattformchronik wird
separat ergänzt.

Die endgültige kompakte Linux/GCC-Release-Auswahl besteht mit **15/15** unter
`build/contact-world-language-release-linux/test-results/run-wdkjpvr0`. Die
kompakte Sprachfixture besteht erneut unter Clang/ASan/UBSan (1/1) in
`build/base-contract-asan-linux/test-results/run-6vz08jmm`. Die vorherigen 13
Solver-/ODE-Sanitizerfälle und 15 Matrix-/Transformations-Sanitizerfälle bleiben
separate Nachweise. Alle vier Linux-Receipts sind zusätzlich auf dem Mac
als `build/linear-range-targeted-linux-results.json`,
`linear-range-asan-linux-results.json`, `linear-range-math-asan-linux-results.json`
und `linear-compact-asan-linux-results.json` erhalten.

### Abschließender macOS-Gesamtlauf

Der vollständige Intel-macOS/Apple-Clang-Release-Lauf besteht mit **636/636**
unter `build/contact-world-language-release-mac/test-results/run-g99aohmt`.
Er verwendet bereits die kompakte endgültige Fixture und enthält sämtliche
bisherigen Compiler-, Runtime-, Format-, Batch-, Lernpfad- und Projektfälle.
Zusammen mit den 403 vollständigen SDK-Prüfbefehlen, sechs ergänzenden
SDK-/Kit-Befehlen und den gezielten Sanitizerprüfungen belegt dies den
beschriebenen lokalen Umfang. Die 77 Fensterfälle wurden in diesem Schritt
nicht erneut vollständig ausgeführt. API/ABI 3, Wire 5, Snapshot 3, psrun 1,
Sprache 0.182.0 und die 27 Core-Module bleiben unverändert.

### Endgültiges vollständiges Linux-SDK

Das frische Linux-SDK mit der kompakten endgültigen Fixture besteht vollständig
unter `build/Linear systems SDK ä linux lk2mfuc5`, verschoben unter
`proof/Native SDK ä fl1nbqwq/Relocated SDK ä`. Alle **403 Prüfbefehle** enden
erfolgreich; 449 SDK-Dateien und 95 Kit-Eingaben sind SHA-256-erfasst. Der
Receipt `build/linear-range-sdk-linux-PASSED.json` stimmt exakt mit den 15
endgültigen Dateien in `build/linear-range-source-freeze.json` überein. Alle
Dateihashes bleiben auch lokal unverändert. Das vollständige Befehlslog liegt
zusätzlich auf dem Mac als `build/linear-range-sdk-linux-verification.log`.
Die C-/Physim-Systeme, beide Archive, zwölf kalte native Projektbuilds, Cache-
Wiederverwendung und alle bisherigen SDK-Gates bleiben in der Abnahme enthalten.

Der begrenzte Implementierungsnachweis zu PP-0360 umfasst damit die tatsächliche
Solverausführung und SDK-Verteilung auf Intel macOS und Debian. Der Plan bleibt
mit 26 implementierten, fünf unvollständigen und 500 ungeprüften Blöcken offen.
Die neue gesamte Remote-/Grafik-/Produktabnahme und die dokumentierten alten
Remote-Fehler bleiben gesonderte Anforderungen.

### Abschließender Linux-Gesamtlauf

Der vollständige Debian-12/GCC-Release-Lauf besteht mit **636/636** unter
`build/contact-world-language-release-linux/test-results/run-iubzzpfg`.
Der zusätzlich lokal erhaltene Receipt ist `build/linear-range-full-linux-results.json`.
Er enthält die endgültige kompakte Fixture und sämtliche bisherigen Compiler-,
Runtime-, Format-, Runner-, Batch-, Domänen- und Projektprüfungen. Zusammen mit
dem vollständigen Linux-SDK, dem vollständigen macOS-Gesamtlauf, dem macOS-SDK
samt kompakter Ergänzung und den getrennten Sanitizerprüfungen belegt dies
den dokumentierten Umfang. Die Originaltexte und SHA-256 des vollständigen
Projektplans bleiben erhalten; die gesamte Projektabnahme bleibt offen.


## Erster nativer macOS-Accessibility-Baum (PP-0710)

Ausgangsrevision `121cff6`. Die UI besaß bislang keine native Accessibility-
Anbindung für ihre gezeichneten Nuklear-Controls. Ein privates C17-Modell
veröffentlicht jetzt kopierte sichtbare Texte und einfache Buttons; eine
AppKit-Brücke erzeugt tatsächliche `NSAccessibilityElement`-Objekte mit
Rollen, UTF-8-Beschriftungen, sichtbaren Rahmen und Press-Aktionen. Die
Anbindung verwendet typisierte Objective-C-Laufzeitaufrufe aus C17.

Geometrieänderungen bewahren Kennungen; verschwundene Kennungen werden nicht
wiederverwendet. Aktivierungen werden nur einmal an ein aktuelles, aktiviertes
Control im nächsten Frame geliefert. Deaktivierte, entfernte oder veraltete
Elemente und statische Texte aktivieren nichts. Ein SDL-Mutex serialisiert
Modellzugriff und Aktionen. Native Referenzen werden vor Freigabe des Modells
ungültig und behalten ihren Bridge-Lebenszyklus bis zum letzten Element.

[Umfang und Grenzen](accessibility.md) nennt insbesondere 256 sichtbare Elemente,
1023 Label-Byte mit UTF-8-sicherer Kürzung, fehlende komplexe Controltypen und
Fokusführung. Linux/AT-SPI und Windows/UIA sind noch nicht implementiert.
Eine praktische VoiceOver-/Screenreader-Abnahme wird durch die Modell- und
nativen Selektorprüfungen nicht behauptet. PP-0710 bleibt unvollständig.

### Ausgeführte erste Nachweise

- macOS-Modell: 1/1 in `run-odno3039`.
- macOS-Modell, UI-Geometrie und Zwischenablage: 3/3 in `run-ga2l1gau`.
- Tatsächlicher AppKit-Probe: 1/1 in `run-_64aw_87`; UTF-8-Label, Press-Aktion,
  Modellzustellung und sichere Invalidierung zurückbehaltener Referenzen.
- Tatsächlicher gezeichneter Nuklear-Button und native Aktion: 2/2 inklusive
  AppKit-Probe in `run-3u_lj575`; einmalige Aktivierung, deaktivierte und
  entfernte Controls sowie Freigabe werden geprüft.
- macOS-Menü-/Einstellungsregressionen: 8/8 in `run-40f9vcd7`.
- Native/UI- sowie Menü-, Einstellungs- und beide Dokumentationsfenster-
  Tastaturprüfungen: 12/12 in `run-hynhki76`.
- Modell, Geometrie, Zwischenablage, Dokumentation, Referenz und Kit: 6/6
  in `run-4lp3pp_3`. Die neue Dokumentationsseite ist offline als Thema erreichbar.
- Linux/GCC-Release baut die App und besteht Modell, Geometrie und
  Zwischenablage: 3/3 in `run-aqkikorc`. Das belegt den portablen Modell-/
  normalen App-Build, keinen Linux-Screenreader-Provider.
- Linux/Clang Debug ASan/UBSan: Modell 1/1 in `run-w_mpf753`.

macOS-Pfade beziehen sich auf `build/contact-world-language-release-mac/test-results`,
Linux-Release auf `build/contact-world-language-release-linux/test-results` und
Sanitizer auf `build/base-contract-asan-linux/test-results`. Die erste
Dokumentationsreferenzprüfung entdeckt einen noch nicht als Offline-Thema
registrierten Link zur neuen Seite. Das Thema wurde am Ende der bestehenden
Liste ergänzt, bestehende Themenindizes bleiben erhalten. Die Referenzprüfung
besteht anschließend; dieser erste Fehlversuch bleibt im Toolprotokoll erhalten.

### Belegter Linux-Fehlversuch und wiederherstellbare Archivierung

Die gezielte Linux-Grafikauswahl endet mit **9/10** unter
`build/contact-world-language-release-linux/test-results/run-0z04_xme`.
`toolbar_input_isolation` scheitert in der großen Pointer-Störvariante
am internen App-Zeitlimit nach 15,341 Sekunden bei Stufe 39; die normale
kleine/große Menüprüfung und alle Einstellungs-/Dokumentationsprüfungen bestehen.
Der Trace zeigt fortlaufende Stufen bis zum Timeout, keine fehlgeschlagene
Eingabeassertion. Zeitgleich besitzt die VM nur ungefähr 404 MB freien
Plattenplatz. Das belegt keine eindeutige Ursache; die Timeout-Grenze wird
nicht aufgrund dieses einzelnen Versuchs verändert.

Vier abgeschlossene generierte SDK-Prüfwurzeln wurden daraufhin verlustfrei
archiviert: `Canonical channels SDK ä linux 9gpwkv9q`,
`Quantity sums SDK ä linux p5n8cppa`, `Real gas SDK ä linux d33yv2j1` und
`Series SI SDK ä linux gzozrs0z`. Sie liegen wiederherstellbar in
`build/accessibility-archived-proofs.tar.gz` mit 1201861120 Byte und SHA-256
`d35026022c4ac2a13bfc77ea3ff3acfbaed3c6c564a5a6ee62228955f57bbc50`.
Alle **58040 Dateien/Links** im Archiv wurden gegen das Inventar geprüft;
vor Entfernen der ausgepackten Linux-Verzeichnisse wurden ihre Originalgrößen,
Hashes und Linkziele erneut geprüft und aktive Prozesse ausgeschlossen.
Die VM gewinnt dadurch ungefähr 3,7 GB freien Platz. Quelldateien und aktuelle
Prüfläufe bleiben erhalten. Inventar und Receipts: `build/accessibility-archived-proof-inventory.json`,
`accessibility-archive-copy.json`, `accessibility-archive-verified.json` und
`accessibility-archive-removed.json` auf Linux. Die erste lokale Inventarprüfung
begann vor Ende der SCP-Übertragung und scheiterte beim Lesen des noch
unvollständigen JSONs. Nach deren bestätigtem Abschluss besteht die vollständige
Prüfung. Beide Versuchslogs bleiben erhalten.

Der gezielte Linux-Menü-Wiederholungslauf besteht anschließend mit **1/1**
unter `build/contact-world-language-release-linux/test-results/run-l72srk_8`.
Die App und alle 19 eingefrorenen Dateien sowie die interne Timeout-Grenze
bleiben unverändert. Dadurch ist der vorherige Fehlversuch nicht rückwirkend
bestanden und seine Ursache weiterhin nicht eindeutig belegt. Nach
Archivierung besitzt die VM etwa 4,1 GB frei; die zurückkopierte Archivdatei
wurde auf Linux nach Größe und SHA-256 geprüft und lässt etwa 2,9 GB frei.
Receipt: `build/accessibility-archive-linux-copy-verified.json`. Das Archiv
und die verifizierten Inventare liegen nun auf beiden Systemen.

### Verschobenes Linux-Paket, gezielter UI-Nachweis

Die frische Paketierung besteht, einschließlich der neuen Offline-Seite,
unter `build/Accessibility SDK ä linux od5pk550`, anschließend verschoben nach
`Relocated SDK ä`. Der erste GUI-Aufruf scheitert vor App-Start, weil der
separate Prüfhelfer seine Arbeitsverzeichnisse noch nicht angelegt hat.
Nach dieser Helferkorrektur bestehen am **identischen Paket** Menü,
Einstellungs- und Dokumentationstastaturführung vollständig. Installations-,
Kit- und erster GUI-Fehlerlog bleiben erhalten; es wurde kein Paket ersetzt.

Das Paket enthält 450 SHA-256-erfasste Dateien, das Kit 95 Eingaben. Die
installierte App stimmt bytegenau mit dem geprüften Build überein, alle
Paketdateien bleiben vor/nach den GUI-Läufen unverändert. Die 19 eingefrorenen
Implementierungs-/Test-/Dokumentationsdateien stimmen mit dem Receipt
`build/accessibility-sdk-linux-PASSED.json` überein. Diese gezielte Paketprüfung
belegt Relocation, neue Offline-Dokumentation und normale GUI-Abläufe. Sie
ersetzt keine erneute vollständige Core-SDK-Prüfung und keinen Linux-Provider,
der noch nicht implementiert ist.

### Vollständige macOS-Grafiksuite

Der tatsächliche Intel-macOS/Apple-Clang-Grafiklauf besteht vollständig mit
**79/79** unter `build/contact-world-language-release-mac/test-results/run-uto33o1u`.
Er enthält die beiden neuen nativen/UI-Probes und sämtliche 77 bisherigen
Fensterfälle: Rendering, Tempo, Timeline, Docking, Hierarchie, Sprach-/Domänen-
Workflows, Menüs, Einstellungen, beide Dokumentationsfenster und die komplette
Vorlagen-/Sprachmatrix per Tastatur mit 16/22-Pixel-UI einschließlich Fehlerpfaden.
Alle 19 eingefrorenen Dateien bleiben unverändert. Die neuen nativen Probes
prüfen zusätzlich AppKit-Rollen, statischen UTF-8-Text, Rahmen und abgewiesene
Schreib-/Press-Aktionen für lesbare Texte. Ein vorheriger abschließender
Native-/Dokumentationslauf besteht mit 6/6 in `run-ocu0nq0g`.

Diese Grafikabnahme betrifft den beschriebenen macOS-Build. Sie ersetzt keine
praktische VoiceOver-Abnahme, Apple-Silicon-Ausführung oder native Linux-/
Windows-Provider. Die allgemeine Tastatur-/Screenreader-Anforderung bleibt offen.

### Verschobenes macOS-Paket und endgültiger Umfang

Das frische macOS-Paket besteht unter `build/Accessibility SDK ä mac 6veyshbc`,
verschoben nach `Relocated SDK ä`. Alle fünf Helferbefehle bestehen: Installation,
Kit sowie tatsächliche Menü-, Einstellungs- und Dokumentationstastaturabläufe.
450 Paketdateien und 95 Kit-Eingaben sind SHA-256-erfasst. Die App ist bytegleich
mit dem in der vollständigen Grafiksuite geprüften Build; neue Offline-Seite,
Paketmanifest und sämtliche Dateihashes bleiben vor/nach den GUI-Läufen erhalten.
Receipt: `build/accessibility-sdk-mac-PASSED.json`. Beide Paketnachweise passen
zu den 19 unveränderten Dateien in `build/accessibility-source-freeze.json`;
nur die Plattformchronik wird anschließend weiter ergänzt.

API/ABI 3, Wire 5, Snapshot 3, psrun 1, Sprache 0.182.0 und die 27 Core-Module
bleiben unverändert. Der Kopflose Katalog ergänzt einen portablen Modellfall;
die macOS-Grafikauswahl ergänzt zwei echte Native-/UI-Fälle. Die bereits
abgeschlossene Core-Abnahme der Ausgangsrevision bleibt ein historischer
Nachweis, keine erneute gesamte Core-Abnahme dieses UI-Schritts. Die aktuellen
gezielten Modell-, Sanitizer-, Native-, GUI- und Paketprüfungen besitzen den
jeweils beschriebenen Umfang. Grundlegender Zugang auf allen drei Plattformen,
komplexe Controltypen, Fokus und tatsächliche Screenreader-Bedienung bleiben
weiterhin offen.





## Linux-AT-SPI und asynchrone UI-Aktionen (PP-0710)

Ausgangspunkt ist `b1d678cb6221eeb41775857c7508407542a09e53`. Linux besitzt
jetzt eine libdbus-Anbindung für die vorhandenen einfachen Buttons und Texte.
Ein unabhängiger PyAT-SPI-Client entdeckt zwei tatsächlich gerenderte
SDL-/Nuklear-Fenster über die Registry und prüft UTF-8, Rollen, Eltern,
Fenster-/Bildschirmrahmen, Hit-Tests, Bulk-Cache, native Buttonaktionen und
Zustands-/Entfernungsereignisse. Ungültige RPC-Signaturen, Zahlenüberläufe in
Objektpfaden und schreibgeschützte Properties werden ausdrücklich geprüft.
Die App bleibt ohne Bus sowie mit `NO_AT_BRIDGE=1` nutzbar.

Unter Debian 12/X11, Clang 14 Debug mit AddressSanitizer und
UndefinedBehaviorSanitizer besteht diese Prüfung nach den unten beschriebenen
Korrekturen **1/1** in `build/atspi-asan-linux/test-results/run-4sg6kclt`
(`build/atspi-race-asan-linux.log`). `ASAN_OPTIONS=detect_leaks=0` betrifft
SDL/Mesa-Leaks; Address-/UndefinedBehavior-Prüfungen bleiben aktiv. Die
isolierte D-Bus-Sitzung verwendet temporäre GSettings und einen ausdrücklich
zugeordneten Accessibility-Bus, damit X11-Sitzungsdaten keine andere
Testsitzung auswählen.

Die erste Instrumentierung belegte einen C17-Grenzfehler in der vendorten
Nuklear-Struktur: `nk_draw_text` schrieb über `char string[2]` hinaus, obwohl
der Befehlsbuffer bereits dynamisch genug Speicher reservierte. Ein echtes
flexibles Array `char string[]` beschreibt jetzt diese Speicherung korrekt.
Die neue Gegenprobe prüft eine vollständige 1500-Byte-UTF-8-Nutzlast samt NUL.
Ein weiterer Lauf belegte eine echte Timing-Lücke: Nach dem Zeichnen
angenommene Aktionen wurden beim Veröffentlichen gelöscht. Sie bleiben nun
für den nächsten Besuch eines weiterhin vorhandenen, aktivierten Controls
erhalten; verschwundene/deaktivierte Ziele verwerfen sie. Eine deterministische
Modellprüfung deckt diesen Übergang und die einmalige Zustellung ab.

Die Linux-ASan-/UBSan-Prüfungen für Modell, Textbefehle, UI-Geometrie und
Clipboard bestehen **4/4** in `run-c8w74e68`; das frühere Textpuffer-Ergebnis
`run-vrhrjna7` lag noch vor der Aktionskorrektur. Auf Intel-macOS bestehen die
vier Release-Prüfungen in `run-zv47b3bg`, die zwei tatsächlichen AppKit-/UI-
Prüfungen nach der Textpufferkorrektur in `run-colyd17p`. Der zusätzlich mit
Apple Clang/C17/Werror gebaute Modelltest mit Aktionskorrektur besteht in
`build/atspi-model-mac.log`. Der macOS-Sanitizer-Aufruf wurde wegen fehlendem
`ld64.lld` vor dem Build abgewiesen und zählt nicht als ausgeführte Prüfung.

Die frühen Fehlversuche bleiben erhalten: falscher Statusobjektpfad
`run-vc5gg8bf`; Busadress-/Cacheprobleme `run-wofa__h8`, `run-zqx281r6`;
Clientbehandlung veralteter Interfaces `run-8j42i9ay`; lokale Python-
Signaturprüfung `run-lbuqeen1`; ungetrennte X11-Buswahl `run-fpyigr37`;
Nuklear-Grenzfehler `run-30hri577`/`run-hxak4vac`; verlorene späte Aktion
`run-e_nqfms8`. Die beiden letzten waren Implementierungsfehler und wurden
behoben, ohne Sanitizer auszuschalten oder Wartezeiten zu verlängern.
Die Release-Gegenprobe für geprüfte Objektpfade vor der zusätzlichen
Fallback-/Aktionskorrektur bestand in `run-6wyxse59`.

Dieser Nachweis gilt für Debian/X11 und Intel-macOS. Vollständige Orca-/
VoiceOver-Bedienung, Wayland, Fokus, Textfelder und weitere Widgets sowie
Windows/UI Automation bleiben offen. PP-0710 bleibt unvollständig; der gesamte
Projektplan wird damit nicht abgenommen.


Die zusätzliche externe Gegenprobe für Elternkoordinaten belegte anschließend
noch einen Hit-Test-Fehler: Das Elternobjekt eines Fensters verwendet
Bildschirmkoordinaten, das Elternobjekt seiner Controls Fensterkoordinaten.
Die Child-Gegenprobe wird jetzt im Koordinatenraum des Aufrufers ausgeführt.
`build/atspi-parent-before-2.log` hält die fehlgeschlagene echte Clientassertion
fest; der erste Harness-Aufruf (`atspi-parent-before.log`) scheiterte bereits
am fehlenden Legacy-PyAT-SPI-Namen für den standardisierten Koordinatentyp 2.
Die komplette externe Prüfung inklusive Fenster-, Bildschirm- und Eltern-Hit-
Tests besteht danach unter ASan/UBSan **1/1** in `run-fk9dyxub`
(`build/atspi-parent-final-asan-linux.log`).


Der vollständige Intel-macOS-Release-Fensterlauf besteht **79/79** in
`run-ogg1ybol` (`build/atspi-all-gui-mac.log`), einschließlich aller 68
Projektmanager-Kombinationen mit 16/22-Pixel-Schrift. Sein App-Binary wurde vor
der zusätzlichen späten Aktionskorrektur gebaut; diese wird separat durch den
finalen Modell-/Native-/SDK-Nachweis geprüft. Der Nuklear-Textpuffer war bereits
korrigiert. Dadurch wird der breite GUI-Lauf nicht als neuer vollständiger
Core- oder plattformübergreifender Screenreader-Nachweis umgedeutet.

Für freien Linux-Prüfspeicher wurde ausschließlich der abgeschlossene ältere
`Linear systems SDK ä linux lk2mfuc5`-Prüflauf archiviert. Das Archiv
`build/atspi-linear-archived-proofs.tar.gz` enthält **14617 Dateien/Verweise**,
ist **313895774 Byte** groß und besitzt SHA-256
`883b00c02b36bb6941e467e025387a9e19dcb9d9096271d3ae0529bced278986`.
Jeder Inhalt wurde gegen das Originalinventar geprüft, die Originale vor dem
Entfernen nochmals vollständig geprüft, und die spätere Linux-Archivkopie
hat denselben Hash. Archiv und Inventar liegen auf Mac und Linux. Wiederherstellung
entpackt die archivierten `build/`-Pfade; der vorherige SDK-Nachweis bleibt damit
nachprüfbar. Receipts: `build/atspi-linear-archived-proof-inventory.json`,
`atspi-linear-archive-verified.json`, `atspi-linear-archive-removed.json` und
`atspi-linear-archive-linux-copy-verified.json`. Der erste gleichzeitige SCP-
Transfer wurde vor Beginn mit einem SSH-Bannerfehler abgewiesen; der serielle
Transfer und seine Hashprüfung waren erfolgreich.


Am **8. Oktober 2026** bestehen die finalen AppKit-/UI-Prüfungen mit der
späten Aktionskorrektur **2/2** in `run-k5qmbdrk`
(`build/atspi-final-native-mac.log`). Das frisch gebaute und nach
`AT-SPI SDK ä mac 0er2rk56/Relocated SDK ä` verschobene SDK besteht seine
Manifest-/Bytegleichheitsprüfung für 450 SDK-Dateien, das unabhängige
Verification-Kit mit 95 Dateien sowie echte
Menü-, Einstellungs- und Handbuch-GUI-Abläufe. Receipt:
`build/atspi-sdk-mac-PASSED.json`. Die unveränderten UI-/Build-/Guide-Eingaben
sind in `build/atspi-source-freeze.json` festgehalten; diese fortgeschriebene
Prüfchronik ist bewusst nicht Teil des Freeze. Die Paketprüfung ist gezielt
und ersetzt keinen neuen vollständigen Core- oder Screenreader-Lauf.


Der vollständige Debian/GCC-Release-Fensterlauf besteht **78/78** in
`run-38r48_zv` (`build/atspi-all-gui-linux.log`), einschließlich aller 68
Projektmanager-Kombinationen und des finalen externen AT-SPI-Clients. Das
App-Binary dieses breiten Laufs enthält die Aktions-/Textpufferkorrekturen,
wurde aber vor der zusätzlichen Eltern-Hit-Test-Korrektur gebaut. Die letzte
native Fixture wurde anschließend mit der korrigierten Implementierung gebaut
und geprüft; die ASan-/UBSan-Gegenprobe `run-fk9dyxub` deckt denselben finalen
Hit-Test-Code ab. Ein aktuelles Linux-SDK wird danach aus diesem Code neu gebaut.


Das am 8. Oktober frisch gebaute Debian/GCC-Release-SDK wurde nach
`AT-SPI SDK ä linux chlu_b3p/Relocated SDK ä` verschoben. Alle **450 SDK-Dateien**
und **95 Kit-Dateien** stimmen mit ihren Manifesten überein; der App-Binary
ist bytegleich mit dem finalen frischen Build. `ldd` löst `libdbus-1.so.3`
auf und zeigt keine fehlende Bibliothek. Menü, Einstellungen und Handbuch
bestehen danach als tatsächliche GUI-Abläufe aus dem verschobenen Paket.
Alle sechs aufgezeichneten Schritte (Install, Kit, Linkage, drei GUI-Abläufe)
bestehen; Receipt: `build/atspi-sdk-linux-PASSED.json`, zusammen mit dem
macOS-Receipt und den 21 unveränderten Dateien in
`build/atspi-source-freeze.json`. Dies ergänzt die native externe Clientprüfung,
behauptet aber keine praktische Screenreader- oder vollständige Core-Abnahme.


## Skalare Suche, exakte Toleranzen und Modulblock-Closures (PP-0361)

Ausgangspunkt am 8. Oktober 2026 ist
`c07e4b832d504d85581284cd74f453f4bcf1ceb2`. Die Abbruchbedingung von Bisection
und Golden Section wird jetzt als exakter Vergleich der binären Werte behandelt.
87 vorbereitete Gegenbeispiele belegten falsche Konvergenz bei subnormalen
Intervallen. Eine einfache Differenzkorrektur schloss diese, ließ aber 1007
relative Grenzfälle in der separaten Probe offen. Die finale Implementierung
entscheidet klar entfernte Fälle über eine konservative Fehlerumhüllung und
Rundungsgrenzen über einen festen Ganzzahlakkumulator, ohne Heap oder breiteren
Gleitkommatyp. Das betrifft die Abbruchentscheidung, nicht die Präzision der
Callback-Ergebnisse.

Die öffentliche Gegenprobe `tests/test_scalar_range_oracle.py` prüft **7787 C**-
und **243 Physim**-Berichte mit unabhängigen Fraction-Vergleichen. Sie prüft
anfängliche sowie finale Abbruchentscheidungen, Callback-Bereiche/Zähler und
bekannte Lösungen. Der ältere Code scheitert auch an dieser öffentlichen
Gegenprobe (`build/scalar-original-oracle-failure.txt`). Zusätzlich bestanden
1152 vorbereitete C-Gegenproben an den größten endlichen Intervallgrenzen.

Ein weiterer belegter Grenzfehler war vorzeitige Stagnation: Bei
`[DBL_TRUE_MIN, 4*DBL_TRUE_MIN]` vertauschten getrennt gerundete Produkte die
Golden-Section-Punkte. Der ältere Code lieferte `PS_LIMIT` nach null Iterationen,
obwohl zwei Innenpunkte vorhanden waren. Skalierte Mischwerte liefern nun
geordnet die beiden Innenpunkte und nach einer Iteration `PS_OK` mit erfüllter
Toleranz. Auch der bisezierende Mittelpunkt wird in kleinen/engen Intervallen
stabil berechnet; gewöhnliche breite Intervalle behalten ihre normale Rechnung.
Die neue öffentliche C-Gegenprobe enthält beide konkreten Sample-Grenzfälle.

Die erste Physim-Gegenprobe belegte außerdem einen echten Compilerfehler:
Schleifen-/Blockwerte auf Modulebene wurden wie echte Modulglobals behandelt,
aber als C-Lokalvariablen ausgegeben. Closures referenzierten so undeclared
Identifikatoren. Der Checker unterscheidet jetzt direkte Modulvariablen von
Blocklocals und übernimmt letztere als besitzende Captures. Zurückbehaltene
Schleifenwerte, veränderte lokale Entwürfe, echte lebende Modulglobals,
verschachtelte String-Captures, lokale Funktionen mit Array-Captures und
optionale Bindungen werden nativ ausgeführt. Der frühere Harness-Aufruf mit
`Optional.none()` war ein Syntaxfehler der Probe; die Sprache verwendet `nil`.

Die gezielte Linux-Clang-Debug-ASan-/UBSan-Prüfung vor der zusätzlichen
Sample-Korrektur besteht **5/5** in `run-7ggfzxaa`
(`build/scalar-final-initial-asan-linux.log`). Die frühere fünffache Prüfung
`run-v71uny34` hatte die verstärkte anfängliche Entscheidungsgegenprobe noch
nicht enthalten. Unter Intel-macOS/Apple Clang bestehen vor der Sample-Korrektur
**5/5** in `run-lzvgypq1`; danach bestehen **19/19** relevante Numerik-, Scalar-,
Closure-, ODE- und Referenzfälle in `run-h1nrywai`
(`build/scalar-sampling-final-mac.log`).

Der breite Intel-macOS-Release-Lauf besteht **640/641** in `run-b17mylss`
(`build/scalar-full-mac.log`). Sein einziger Fehler war die veraltete erzeugte
API-Referenz nach der Header-Vertragsänderung. `tools/generate-reference.py`
aktualisierte nur `docs/reference/numerics.md`; alle 30 Referenzdateien bestehen
anschließend `--check`, und die Referenzgegenprobe besteht im gezielten finalen
Lauf. Dieser breite Lauf lag noch vor der zusätzlichen Sample-Korrektur; die
finale 19er-Prüfung deckt diese gezielt ab. Die Fehlerbelege `run-ho7tfwlu`
(falsche erwartete Iterationslage der neuen C-Probe), `run-823gb3qm`
(Harness-Syntax) und `run-b5ug0q9u` (echte Capture-Lücke) bleiben erhalten.

Das finale Intel-macOS-SDK `Scalar search SDK ä mac nn54utn3` besteht seinen
Manifestnachweis für **451 SDK-Dateien** und das unabhängige Kit mit **100 Dateien**.
Der aus dem Kit entpackte Prüfer verwendet `--scalar-only`, verschiebt das Paket
und baut den Core ausschließlich aus dessen Quellen neu. Öffentliche C-Probe,
Fraction-/C-/Physim-Berichte und Modulblock-Closures bestehen mit installiertem
und frisch gebautem Core. Receipt: `build/scalar-sdk-mac-PASSED.json`. Dieser
gezielte Paketnachweis ist keine Wiederholung der vollständigen früheren
SDK-Domänen- oder GUI-Abnahme.

Für die Linux-Prüfung wurden nur die abgeschlossenen älteren SDK-Prüfläufe
`ODE range SDK ä linux lpk4sq92` und `Series numeric SDK ä linux p5iekc7w`
archiviert. `build/scalar-old-archived-proofs.tar.gz` enthält **28513 Dateien/Verweise**,
**594890472 Byte**, SHA-256
`7cdd96fd1c440adfd32b066f8263792f74d28004bdce6aef8de6dc50742323be`.
Inhalte wurden gegen das Inventar geprüft, Originale vor dem Entfernen nochmals
geprüft und die Linux-Archivkopie gegen denselben Hash geprüft. Archiv und
Inventar liegen auf Mac und Linux; Entpacken stellt die ursprünglichen `build/`-
Pfade wieder her. Receipts: `build/scalar-old-archived-proof-inventory.json`,
`scalar-old-archive-verified.json`, `scalar-old-archive-removed.json` und
`scalar-old-archive-linux-copy-verified.json`.

PP-0361 besitzt damit einen begrenzten Implementierungsnachweis für Bisection
und Golden Section; globale Optimierung, weitere Mathematikforderungen und
die gesamte Plattform-/Produktabnahme bleiben offen. Die 531 Originalplanblöcke
bleiben vollständig erhalten: 27 implementiert, fünf unvollständig, 499 ungeprüft.


Der breite Debian/GCC-Release-Lauf besteht **641/641** in `run-t4s0ldly`
(`build/scalar-full-linux.log`). Die erzeugte Referenz war dort bereits
aktualisiert. Dieser Lauf lag vor der zusätzlichen Sample-Korrektur.
Mit dem finalen stabilen Mischwert bestehen danach die **19/19** relevanten
Fälle sowohl unter GCC/Release in `run-h51lh8ze` als auch unter
Clang/Debug/ASan/UBSan in `run-z0mxt__a`
(`build/scalar-sampling-final-release-linux.log`,
`build/scalar-sampling-final-asan-linux.log`). Es wurden keine Wartezeiten
verlängert und keine Sanitizer-Prüfungen unterdrückt; `detect_leaks=0` ist wie
zuvor für SDL/Mesa gesetzt, Address-/UndefinedBehavior bleiben aktiv.


Der eng begrenzte Apple-Clang-O2-Zeitvergleich von 20000 flachen Golden-Section-
Suchen auf `[-10,10]` mit absoluter Toleranz `1e-10` behält 53 Iterationen/
55 Auswertungen. Drei ältere Läufe brauchen 0,00519–0,00534 Sekunden, drei finale
0,01284–0,01531 Sekunden. Der exakte Vertrag und stabile Randpunkte verursachen
hier etwa Faktor 2,5 im Median; das sind in dieser Probe rund 0,4 Mikrosekunden
zusätzliche Zeit pro Suche. `build/scalar-benchmark-final.json` bewahrt die Werte.
Dies ist kein allgemeines Leistungsversprechen für andere Callbacks/Intervalle;
Grenzfälle verwenden zusätzliche exakte Arbeit.


Das finale Debian/GCC-SDK `Scalar search SDK ä linux pe_dtue1` besteht ebenfalls
seine **451 SDK-Dateien** und das unabhängige Kit mit **100 Dateien**. Der aus
dem Kit entpackte Prüfer verschiebt es und führt `--scalar-only` mit dem
installierten sowie allein aus Paketquellen neu gebauten Core aus. Alle drei
äußeren Schritte (Install, Kit, Verify) bestehen; die sechs jeweiligen
C-/Closure-/Orakel-Gegenproben bestehen. Receipt:
`build/scalar-sdk-linux-PASSED.json`. Die beiden Plattform-Receipts passen zu
allen 20 Dateien in `build/scalar-source-freeze.json`; die fortgeschriebene
Prüfchronik ist davon bewusst ausgenommen. Der gezielte SDK-Nachweis ersetzt
keinen neuen vollständigen SDK-Domänen-/GUI-Lauf und keine aktuelle Windows-
oder Apple-Silicon-Abnahme. Der Gesamtplan bleibt unvollständig.


[Aktuelle Plattformnachweise](platform-validation.md).

## Transformationsbereiche und Mathematikzuordnung (PP-0357)

Ausgangspunkt am 8. Oktober 2026 ist
`e33094d920708b85991842f6cedfdaf90429172a`. Die lokalen API-/Quellen-/Testprüfungen
ordnen Vec2/3/4, Mat3/4, Quaternionen und Transformationen konkret zu. Die
bestehenden 2000er-Inversions-, Rodrigues-, Slerp- und Senkrechtstellungsprüfungen
bleiben aktiv. Der gesamte Plan und die übrigen Mathematikblöcke bleiben offen.

Drei direkte C-Gegenbeispiele lieferten zuvor `PS_NUMERIC` bei endlichen
Ergebnissen: `DBL_MAX*2-DBL_MAX`, der projektive Quotient
`(DBL_MAX*2)/DBL_MAX` und die normierte Richtung der invers-transponierten
Skalierung `diag(DBL_TRUE_MIN,1,1)`. Ein weiterer Fall verlor den kleinsten
Rest zwischen `DBL_MAX` und `-DBL_MAX`. Die Zeilensummen behalten nun exakte
binäre Produkte/Summen. Projektive Punkte dividieren den exakten Zähler durch
die exakte homogene Koordinate; Ausgabe wird einmal nearest-even gerundet.
Ein eigens provozierter halber kleinster Zahlenwert plus positiver Rest belegte
auch doppelte Rundung; die finale Konvertierung erhält diesen Rest korrekt.

Normalen lösen das transponierte System nach Zeilenskalierung der ursprünglichen
Matrix direkt mit unabhängigen Lösungsexponenten. Eine überlaufende vollständige
Inverse ist dafür nicht erforderlich. Der interne lineare Solver wurde ohne
Änderung seiner Elimination/Pivotregeln in ein privates gemeinsames Header
überführt; die öffentliche Lösung konvertiert weiterhin atomar nach Double und
meldet echte nichtdarstellbare Ausgaben. Normalen bleiben durch die dokumentierte
Double-Koeffizienten-/Pivot-/Konditionsgenauigkeit begrenzt.

Die dauerhafte unabhängige Gegenprobe umfasst **1288 C**- und **100 Physim**-
Fälle. Fraction bestimmt die exakt gerundeten Punkt-/Richtungswerte, rationale
Elimination und 100-stellige Decimal-Normierung bestimmen Normalen. Enthalten
sind allgemeine projektive Matrizen, Auslöschung, subnormale Halbwege, negative
homogene Koordinaten, starke Skalierungsunterschiede/Scherungen und atomare
Fehler. Zusätzliche vorbereitete C-Prüfungen bestanden für 3000 gemischte und
3000 allgemeine projektive Fälle sowie 1770 Rundungsgrenzen.

Vor der abschließenden Beschleunigung bestehen auf Intel-macOS **10/10** relevante
Mathematik-/Transformations-/Linear-/Scalar-/Referenzfälle in `run-dq6ziv03`
(`build/transform-final-targeted-mac.log`) und unter Linux/Clang/Debug/ASan/UBSan
**10/10** in `run-j3sb1yjj` (`build/transform-final-asan-linux.log`). Die früheren
sechsteiligen Nachweise `run-zwsfs74t` und `run-_oxj56zk` liegen vor den finalen
Fast-Path-/Dokumentationsänderungen. Der erste vierteilige Regressionstest
`run-crffb4gf` lag noch vor der exakten Zeilen-/Quotientenrechnung.

Die ersten vier tatsächlichen Intel-macOS-Grafikprüfungen bestehen **4/4** in
`run-m8k_1_vt` (`build/transform-gui-mac.log`): Grafik/Picking, UI-Renderer,
Hierarchie und Szenenframes. Die Mikroprüfung zeigte dennoch erhebliche
Mehrarbeit der ausschließlich ganzzahligen affinen Rechnung. Der finale Pfad
verwendet gewöhnliche fehlerfreie Produkt-/Summenentwicklungen und fällt nur
bei kleinen Produkten oder überlaufenden Entwicklungsstufen auf Ganzzahlen
zurück. Alle 15000 zusätzlich vorbereiteten gewöhnlichen C-Summen stimmen
weiterhin mit rationalen Referenzen überein. Diese Grafikprüfung lag vor der
zusätzlichen Beschleunigung; deren finale Numerik-/Grafiknachweise folgen separat.

Für 10000 affine Punkttransformationen mit Scherung 0,2 und Translation 10
braucht die ältere Apple-Clang-O2-Probe rund 0,000294 Sekunden. Die reine
Ganzzahlfassung brauchte 0,034–0,036 Sekunden, die finale Entwicklung
0,000868–0,000917 Sekunden. Das sind in dieser bewusst kleinen Probe rund
0,06 Mikrosekunden zusätzliche Zeit pro Punkt; keine allgemeine Garantie für
andere Matrizen/Callsites. Werte: `build/transform-benchmark-proof.json`.


Die finale beschleunigte Intel-macOS-Numerikprüfung besteht **10/10** in
`run-ysdfd81_` (`build/transform-expansion-final-mac.log`), ihre vier tatsächlichen
Grafik-/Szenenprüfungen bestehen **4/4** in `run-ng4ovmww`
(`build/transform-expansion-gui-mac.log`). Unter Linux/Clang/Debug/ASan/UBSan
bestehen dieselben zehn finalen Numerikfälle in `run-19keu_1u`
(`build/transform-expansion-asan-linux.log`). Die Linux-Numerik-/Grafikläufe vor
der Beschleunigung bestehen **10/10** in `run-2n32hh01` und **4/4** in
`run-rh_pczdj`; sie werden nicht als finale Fast-Path-Prüfung ausgegeben.

Das finale Intel-macOS-SDK `Transform SDK ä mac 822s18g_` besteht seine
Manifest-/Bytehash-Prüfung und das unabhängig entpackte Verification-Kit.
Der gezielte `--transform-only`-Prüfer verschiebt das Paket und prüft die
1288 C-/100 Physim-Gegenproben sowie atomare C-Fehler mit installiertem und
allein aus Paketquellen neu gebautem Core. Receipt:
`build/transform-sdk-mac-PASSED.json`. Kein Repository-Implementierungscode
wird zum Neubau herangezogen; der gezielte Modus ist keine vollständige
SDK-Domänen- oder Grafikabnahme.


Die finale beschleunigte Debian/GCC-Release-Numerik besteht **10/10** in
`run-3_5kpyom` (`build/transform-expansion-release-linux.log`); ihre tatsächlichen
Grafik-/Picking-/Hierarchie-/Szenenprüfungen bestehen **4/4** in `run-o36d9nbr`
(`build/transform-expansion-gui-linux.log`). Damit liegen für den finalen Pfad
gezielte Release-Numerik-/Grafiknachweise auf beiden Plattformen und die
Linux-ASan-/UBSan-Gegenprobe vor. Keine vollständige neue Core-/Windows-/
Apple-Silicon-Abnahme wird daraus abgeleitet.


Das finale Debian/GCC-SDK `Transform SDK ä linux r2044mtc` besteht den gleichen
gezielten Paketnachweis: **453 SDK-Dateien**, **104 unabhängige Kit-Dateien**,
verschobene Paketkopie sowie öffentliche C-/Physim-Gegenproben mit installiertem
und allein aus Paketquellen neu gebautem Core. Alle drei äußeren Schritte
(Install, Kit, Verify) und die vier jeweiligen C-/Orakel-Ausführungen bestehen.
Receipts: `build/transform-sdk-linux-PASSED.json` und
`build/transform-sdk-mac-PASSED.json`. Beide passen zu den 19 unveränderten Dateien
in `build/transform-source-freeze.json`; diese fortgeschriebene Prüfchronik ist
bewusst ausgenommen. Die Quellen und Originaltexte aller 531 Planblöcke bleiben
erhalten. PP-0357 ist begrenzt implementiert; der Gesamtplan bleibt offen.

## Opake Run-Streams am 8. Oktober 2026

`run_stream.h` ergänzt einen explizit allozierten opaken Store mit jeweils acht
Reader-/Writer-Slots und Besitzer-/Slot-/Generationsprüfung. Schließen invalidiert
alle Handlekopien; eine erschöpfte Generation wird stillgelegt. Fehler erhalten
Reader-Ausgaben, bekannte Fehlerläufe lassen sich ohne Erfolgsfooter abbrechen.
Experiment-Runner und Run-Importvalidierung verwenden die Schnittstelle tatsächlich.
Die alte `data.h`-API und weitere öffentliche Ressourcenzustände bleiben bestehen;
PP-0352 ist deshalb weiterhin unvollständig. [Verträge](run-streams.md).

Intel macOS 14.6.1/Apple Clang 16 mit SDL 3.2.30 besteht die 20 ausgewählten
Release-Integrationsfälle einschließlich C-/Physim-Runner, Run-Import, Snapshots,
Core-Katalog und nativem Projektbuild unter
`build/contact-world-language-release-mac/test-results/run-drpye0_5`.
Die zusätzliche Generationsgrenze und öffentliche API bestehen zusammen mit der
Referenzprüfung unter `run-q_t_abw5` (drei Fälle). Der abschließende Test der
formatierten Grenzfixture und des unabhängigen Prüfkits besteht unter
`run-3qh4h8f7` (zwei Fälle). Der gemeinsame Katalog umfasst jetzt 28 Core-Module.

Debian 12/GCC 12.2 besteht die entsprechende Release-Auswahl einschließlich der
Generationsgrenze mit 21/21 Fällen unter
`build/contact-world-language-release-linux/test-results/run-1dszgzhw`.
Clang 14 mit ASan/UBSan besteht 20/20 ausgewählte API-, Import-, Snapshot-,
Runner-, Katalog- und Referenzprüfungen unter
`build/atspi-asan-linux/test-results/run-v95w1uph`; der längere native Projektbuild
ist dort nicht Teil der Auswahl. `ASAN_OPTIONS=detect_leaks=0` deaktiviert nur
Leak-Prüfung, nicht Address-/UndefinedBehavior-Prüfungen. macOS-Sanitizer sind
weiterhin wegen des fehlenden `ld64.lld` nicht nachgewiesen.

Analyseprojekte mit Run-Import, gespeicherter Run-Lernpfad und Handbuch-Tastatur-
führung bestehen jeweils 3/3 auf macOS unter `run-isqgpkfk` und auf Linux/X11
unter `run-obxfnhkn` in den jeweiligen Release-Testverzeichnissen. Der erste
Linux-GUI-Aufruf ohne `DISPLAY` scheitert mit „No available video device“ unter
`run-uoizmvbz` (0/3); er wird nicht als bestanden gewertet. Der nachfolgende
Aufruf verwendet den tatsächlich laufenden Xvfb-/Openbox-Desktop mit `DISPLAY=:99`.

Die zusätzlich ausgeführte Selbstprüfung des nativen Test-Runners deckt zwei
bestehende Katalogannahmen auf: macOS-Accessibility-Unitfälle besitzen keinen
Integrationseintrag, der Linux-AT-SPI-Fall verwendet einen eigenen Client statt
des Workflow-Wrappers. Die Zuordnung ist korrigiert; sämtliche erwarteten Fehler,
Timeouts, Start-/Buildfehler und vollständigen Ergebnisberichte bestehen danach
auf beiden Plattformen (`build/run-stream-test-runner-final-mac.log` und
`build/run-stream-test-runner.log` auf Linux). Die Prüfkiterstellung bestätigt
105 unabhängige Eingaben mit SHA-256, Paketgrenzen und exklusiver Veröffentlichung.

Das macOS-SDK enthält 457 manifestierte Dateien einschließlich öffentlichem
Stream-Header und Stream-Implementierung. Die fokussierte `--stream-only`-Prüfung
verschiebt es nach `build/run-stream-sdk-checks/Native SDK ä j0z8_tve` und baut
Core ausschließlich aus dessen Quellen neu. Derselbe öffentliche API-Test besteht
gegen installierte und neu gebaute Bibliothek. Ownership, alte/fremde/gefälschte
Handles, Slotgrenzen, Allocatorfehler, atomare Reads, Snapshots, Release und
recoverable Abort sind enthalten. Dieser Nachweis ersetzt keine vollständige
SDK-Domänenabnahme; direkte Low-Level-Physim-Bindungen sind nicht ergänzt.

Das Linux-SDK besteht denselben Test mit 457 manifestierten Dateien unter
`build/run-stream-sdk-checks/Native SDK ä 8zn5r21_`. GCC warnt dabei in der
Legacy-Kompatibilitätsgegenprobe wegen eines Zweierarrays am alten Parameter
`double values[PS_MAX_CHANNELS]`. Die Testfixture verwendet abschließend ein
Array in der dort deklarierten Größe; die Kapazitäts-/Fehlerprüfungen der neuen
API bleiben bestehen. Der endgültige öffentliche Test besteht unter macOS in
`run-728edbkz` und unter Linux-ASan/UBSan in `run-g800s406` (je 1/1).
Beide verschobenen SDKs mit installierter und aus Paketquellen gebauter Bibliothek
bestehen danach erneut: macOS `Native SDK ä b9b9zvd3`, Linux `Native SDK ä kfyip904`.
Das endgültige unabhängige Kit besitzt weiterhin 105 exakt manifestierte Dateien
und besteht den Paketgrenzentest auf beiden Plattformen.

Die geänderten Quellen sind zwischen macOS und Linux per SHA-256 abgeglichen
(`build/run-stream-source-freeze.json`). Laufdaten, SDK-Kopien und Prüfberichte
bleiben ignorierte Nachweise im Build-Verzeichnis. Aktuelle Windows-, Apple-
Silicon-, Wayland- und praktische Screenreader-Nachweise entstehen durch diese
fokussierten Läufe nicht.

## Exakte Toleranzvergleiche am 8. Oktober 2026

`ps_close` und Physim `isClose` entscheiden die symmetrische absolute/relative
Bedingung für exakte binäre Eingabewerte. Ein konservatives Fehlerintervall
beschleunigt eindeutige Fälle; die vorhandenen begrenzten Integer-Hilfen erhalten
kritische Differenzen, Produkte und Summen ohne Heapallokation. Beide Toleranzen
null verlangen exakte Gleichheit; nichtendliche Werte/Toleranzen und negative
Toleranzen ergeben false. [Einheiten und Grenzvertrag](math.md).

Die unabhängige Fraction-Gegenprobe findet beim bisherigen, im Run-Stream-SDK
aufbewahrten Core 1751 abweichende Grenzentscheidungen in 9176 Fällen
(`build/close-baseline-final.log`). Darunter sind 1 gegen den negativen kleinsten
Subnormalwert bei absoluter Toleranz 1 und entgegengesetzte `DBL_MAX`-Werte
knapp unter der kombinierten Toleranzgrenze. Die endgültige Probe prüft jede
C-Eingabe in beiden Vergleichsrichtungen; die Sprachfixture umfasst 263 aus
binären Mantissen/Exponenten exakt rekonstruierbare Eingaben. Referenzen werden
mit Python Fraction unabhängig von der Produktionsimplementierung berechnet.

Intel macOS 14.6.1/Apple Clang 16 besteht sechs ausgewählte Release-Fälle unter
`build/contact-world-language-release-mac/test-results/run-13xdmja8`:
Math, Sprachchecker, Referenzdokumentation sowie skalare Suche, Transformationen
und Zahlenvergleich mit unabhängigen Orakeln. Debian 12/Clang 14 besteht dieselbe
Auswahl unter ASan/UBSan in
`build/atspi-asan-linux/test-results/run-nuynpn3v` (6/6).
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UndefinedBehavior-Prüfungen aktiv;
macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` unbestätigt.

Das macOS-SDK besitzt 457 manifestierte Dateien. Die fokussierte Prüfung
`--comparison-only` verschiebt es nach
`build/close-sdk-checks/Native SDK ä 5gngu1fr`, prüft dessen Manifest und baut
Core ausschließlich aus Paketquellen neu. Der Paketcompiler übersetzt die
263 Physim-Eingaben. C-/Physim-Programme bestehen gegen installierte und neu
gebaute Bibliothek dieselbe rationale Gegenprobe. Das unabhängige Prüfkit
umfasst 108 Dateien und besteht Paketgrenzen, SHA-256, fehlende/dynamische
Eingaben und exklusives Veröffentlichen.

Debian 12/GCC 12.2 besteht dieselben sechs Release-Fälle unter
`build/contact-world-language-release-linux/test-results/run-mx8_srnb`.
Die großen bestehenden Such-/Transformations-Sprachfixtures benötigen längere
Optimierung; GCC meldet ein erschöpftes Variablen-Tracking-Budget und kompiliert
diese Dateien anschließend ohne dieses optionale Debug-Tracking. Der tatsächlich
abgeschlossene Gesamtlauf besteht inklusive der neuen Vergleichsfixture (6/6).

Das Linux-SDK enthält ebenfalls 457 manifestierte Dateien. Die verschobene
Prüfung unter `build/close-sdk-checks/Native SDK ä 5x7emncm` besteht dieselbe
C-/Physim-Gegenprobe gegen installierten und ausschließlich aus Paketquellen
neu gebauten Core. Alle 18 geänderten Dateien stimmen zwischen macOS und Linux
per SHA-256 überein (`build/close-source-freeze.json`). Die endgültige
Prüfkiterstellung mit 108 Dateien besteht auf beiden Plattformen.

PP-0358 ist mit diesen begrenzten Nachweisen als implementiert erfasst. Die
Originaltexte und Umfänge aller 531 Planblöcke bleiben erhalten; PP-0365 und die
vollständige Algorithmus-/Produkt-/Plattformabnahme bleiben offen. Die Nachweise
enthalten keine aktuelle Windows-, Apple-Silicon- oder macOS-Sanitizer-Abnahme.


## Kurvenauswertung am 8. Oktober 2026

Die unabhängige rationale Kurvenprobe findet in der bisherigen Core-Bibliothek
627 abweichende Fälle bei 956 Eingaben (`build/curve-baseline.log`). Besonders
kleine Tangenten zwischen benachbarten großen Kontrollpunkten gehen in gerundeten
De-Casteljau-Zwischenstufen verloren; bei Subnormalzahlen kann das Vorzeichen kippen.
Position, analytische Tangente und Unterteilungskontrollpunkte verwenden jetzt
exakte binäre Polynome und jeweils eine abschließende nearest-even-Rundung.
Begrenzte Integerarrays decken vier Double-Faktoren samt kleinen Koeffizienten
ab; Heapallokation und breitere Fließkommapräzision sind nicht erforderlich.
[Einheiten und Kurvenvertrag](math.md).

956 C-Fälle vergleichen Position, Tangente und alle acht Unterteilungskontrollpunkte
mit einer unabhängig implementierten rationalen De-Casteljau-Referenz. Die
77 Physim-Fälle prüfen denselben Core und kopierte `controlPoint(index)`-Werte;
negative/zu große Indizes werden abgefangen. Tangentenüberlauf erhält die gesamte
C-Ausgabe; die Unterteilung kann trotzdem gelingen. Bestehende Kurvenprüfungen
sichern Aliasierung, Parameterfehler und die analytische Kurve `(t,t²,t³)`.

Intel macOS 14.6.1/Apple Clang 16 besteht alle zehn ausgewählten Release-Fälle
unter `build/contact-world-language-release-mac/test-results/run-86mgjon4`.
Debian 12/Clang 14 besteht dieselben zehn Fälle mit ASan/UBSan unter
`build/atspi-asan-linux/test-results/run-dosv5bmb`. Die Auswahl enthält Math,
lineares Resampling, PCHIP, dessen Physim-Analyse, vier bestehende native Physim-
Bézierfälle, Referenzdokumentation und das neue rationale Orakel.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UndefinedBehavior-Prüfungen aktiv;
macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Die zusätzliche kleine C-Laufzeitprobe verwendet dieselbe vierpunktige Kurve
bei 10000 Parametern. Alte Auswertung: 0,000976–0,001257 CPU-Sekunden;
neue exakte Auswertung: 0,033960 in einem ersten und 0,042365–0,042924 Sekunden
in drei weiteren Läufen (`build/curve-benchmark-proof.json`). Das entspricht
rund 3,4–4,3 µs pro Auswertung; 33 Punkte rechnerisch etwa 0,11–0,14 ms.
Dies ist ein konkreter Genauigkeits-/Laufzeittradeoff eines einzelnen Kernels,
kein Nachweis allgemeiner Renderer- oder Simulationsleistung.

Zwei ältere abgeschlossene SDK-Prüfstände sind zur Platzgewinnung vollständig
archiviert: `AT-SPI SDK ä linux chlu_b3p` und `Accessibility SDK ä linux od5pk550`.
`build/curve-old-archived-proofs.tar.gz` enthält 948 geprüfte Dateien/Links,
63833110 Bytes, SHA-256
`0520c56ffe7aeca4a6caca7b0cfc44b42f9455cd325679a4580299654738ad61`.
Mac- und Linux-Kopie sind vollständig gegen das Inhaltsinventar geprüft;
Originale wurden vor Entfernen erneut gehasht. Inventar und Löschbeleg liegen
unter `build/curve-old-archived-proof-inventory.json` beziehungsweise
`build/curve-old-archive-removed.json`. Die jüngsten Scalar-/Transform-/Run-Stream-
und Vergleichsnachweise bleiben separat vorhanden.

Die endgültige Gegenprobe vergleicht Double-Bits einschließlich signierter Null
an kopierten Endpunkten. Der erste Fixture-Generator hatte negative Null als
positive Null ausgegeben; die Literale sind korrigiert. Der alte Core weicht in
der verschärften Prüfung bei 629 Fällen ab (`build/curve-baseline-bits.log`),
davon zwei zusätzliche Unterschiede der Nullvorzeichen gegenüber der ersten
rein numerischen Gegenprobe.

Der während dieser Testverschärfung bereits laufende erste GCC-Release-Aufruf
endet unter `run-p194e12o` mit 9/10: sein übersetztes Programm enthält noch die
alten Null-Literale und scheitert am neuen Bitvergleich. Dieser Lauf wird nicht
nachträglich als bestanden gewertet. Das sehr große ursprüngliche Main benötigt
zudem mehrere Minuten Variablen-Tracking/Optimierung. Die Fixture erzeugt nun
für jeden der unverändert 77 Fälle eine eigene Funktion und ruft alle in derselben
Reihenfolge auf; Referenzwerte und Abdeckung bleiben erhalten.

Die endgültige Fixture besteht auf macOS unter `run-jfdbgack` und auf Linux mit
ASan/UBSan unter `run-p6o3rk9h` (je 1/1 Orakel). Die gesamte endgültige
Linux-Release-Auswahl besteht mit 10/10 unter
`build/contact-world-language-release-linux/test-results/run-emecok__`.
Die jeweilige ältere Zehnerauswahl und die endgültige Bitprüfung sind getrennte
Nachweise; Änderungen an der Fixture werden nicht früheren Programmen zugerechnet.

Das macOS-SDK enthält 458 manifestierte Dateien einschließlich `curve_numeric.h`.
Die endgültige fokussierte Abnahme verschiebt es nach
`build/curve-sdk-checks/Native SDK ä cwuq2v11` und besteht mit installierter sowie
allein aus Paketquellen neu gebauter Bibliothek: 956 C- und 77 Physim-Fälle,
bitgleiche Positionen/Tangenten/Unterteilungskontrollpunkte, Fehlererhaltung,
kopierte Endpunkte und geprüfte Kontrollpunktindizes. Der ausgelieferte Compiler
übersetzt die endgültige Physim-Fixture selbst. Das unabhängige Kit umfasst
111 Dateien und besteht Paketgrenzen, SHA-256 und exklusives Veröffentlichen.

Das Linux-SDK enthält ebenfalls 458 manifestierte Dateien. Die endgültige
verschobene Abnahme unter `build/curve-sdk-checks/Native SDK ä 7mi0ajrh`
besteht dieselbe bitgenaue C-/Physim-Gegenprobe gegen installierten und aus
Paketquellen neu gebauten Core. Alle 20 geänderten Dateien stimmen zwischen
macOS und Linux per SHA-256 überein (`build/curve-source-freeze.json`).
Die Quellen, Tests, Referenzen und SDK-Hilfen werden gemeinsam versioniert;
SDK-Kopien, Archive und Laufdaten bleiben ignorierte Build-Nachweise.

PP-0359 ist mit den genannten Interpolations-/Kurvennachweisen implementiert.
Der gesamte Projektplan, die allgemeine Algorithmusabnahme PP-0365 und weitere
Plattformen bleiben offen. Die fokussierten Läufe beweisen keine aktuelle
Windows-/Apple-Silicon-, allgemeine GUI- oder macOS-Sanitizer-Abnahme.

## Einheitenkonvertierung am 8. Oktober 2026

Die bisherige Konvertierung verändert bei 220 von 2000 Identitätsfällen den
Eingabewert um ein Bit. Die breitere unabhängige Fraction-Gegenprobe findet
540 abweichende Fälle bei 8214 Eingaben (`build/unit-baseline.log`). Core und
Quantity konvertieren nun das exakte binäre Verhältnis mit einer abschließenden
Rundung; gleiche Skalen erhalten jedes Bit, einschließlich signierter Null.
Konvertierung in SI beziehungsweise aus SI verwendet eine einzelne
Multiplikation/Division, allgemeine Verhältnisse begrenzte exakte Hilfen.
[SI-Grenzen und Verträge](units.md).

8214 C-Fälle und 264 Physim-Fälle prüfen Identitäten, Exponentengrenzen, direkte
SI-Pfade, Dimensions-/Bereichsfehler, Aliasierung und unveränderte Fehlerausgaben.
Der separate Host-Trap-Test prüft sieben SI-Basisvektoren, benannte abgeleitete
Dimensionen, Formatierung und ursprüngliche Invalid-/Numeric-Codes samt
strukturierten Diagnosen und Quellposition. Unit-/Quantity-Wrapper bewahren jetzt
die tatsächlichen C-Codes, statt numerische Fehler als ungültige Eingaben zu melden.

Intel macOS 14.6.1/Apple Clang 16 besteht 15 ausgewählte Release-Fälle unter
`build/contact-world-language-release-mac/test-results/run-dgl3cubi`.
Die erweiterte Definitions-/Formatierungsgegenprobe besteht separat unter
`run-rdv_oss6` (1/1). Berichte, SI-Reihen, CSV-Export, Anzeigeeinheiten,
Units-/Parameter-Sprachprüfungen bestehen zusätzlich mit 11/11 unter `run-914vc28f`.
Der erste neue Orakelaufruf scheitert unter `run-6yegrvh9` mit 14/15, weil die
Fixture einen internen globalen Builtin-Namen statt der veröffentlichten
`Unit.convert`-Methode verwendet. Die Fixture und Anleitung sind korrigiert;
der fehlgeschlagene Lauf wird nicht als bestanden gewertet.

Debian 12/Clang 14 besteht dieselbe Fünfzehnerauswahl mit ASan/UBSan unter
`build/atspi-asan-linux/test-results/run-mut9n60w`.
Debian 12/GCC 12.2 besteht die erweiterte Release-Auswahl mit 23/23 unter
`build/contact-world-language-release-linux/test-results/run-qjnqrlge`.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UndefinedBehavior-Prüfungen aktiv.
macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Anzeigeeinheiten-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit
2/2 unter `run-8rzs0e8e`. Die neue SI-Anleitung und beide C-Referenzseiten sind
im Offline-Handbuch erreichbar; alle 31 generierten Referenzdokumente sind geprüft.
Die unabhängige Prüfkiterstellung umfasst 115 Dateien mit SHA-256, Grenzen und
exklusiver Veröffentlichung.

Die älteren abgeschlossenen SDK-Prüfstände `CI regression SDK ä linux h05lz9j2`
und `Scalar search SDK ä linux pe_dtue1` sind vollständig archiviert:
`build/units-old-archived-proofs.tar.gz`, 96393124 Bytes, 1592 Dateien/Links,
SHA-256 `0c837d63f9cf9cdceebeace9fe75288f0ab6d02559de3a77006f24971b0e8c93`.
Mac- und Linux-Kopie wurden gegen das Inhaltsinventar vollständig geprüft;
Originale vor dem Entfernen erneut gehasht. Inventar, Prüf- und Löschbelege
liegen unter `build/units-old-*`. Jüngere Transform-, Run-Stream-, Vergleichs-
und Kurvennachweise bleiben separat vorhanden.

Die beiden betroffenen GUI-Fälle bestehen auch unter Linux/X11 mit 2/2 unter
`build/contact-world-language-release-linux/test-results/run-d3qdbc80`.
Beide SDKs besitzen 459 manifestierte Dateien einschließlich der SI-Anleitung.
Die verschobene fokussierte Abnahme besteht auf macOS unter
`build/unit-sdk-checks/Native SDK ä 55xqlecr` und auf Linux unter
`build/unit-sdk-checks/Native SDK ä d5p64sin`. Der Paketcompiler übersetzt die
264 Physim-Fälle. Derselbe rationale Probe und Host-Trap-Test bestehen gegen
installierte und ausschließlich aus Paketquellen neu gebaute Bibliotheken.

Der erste Linux-SDK-Aufruf unter `Native SDK ä z9sd1tyk` scheitert beim Schreiben
der temporären Compiler-Assembly mit „No space left on device“
(`build/unit-sdk-verify-linux.log`). Er bleibt ein fehlgeschlagener Nachweis.
Weitere vollständig archivierte Prüfstände schaffen Platz für die anschließende
bestandene Abnahme:

- `Transform SDK ä linux r2044mtc`: `build/units-space-archived-proofs.tar.gz`,
  70562996 Bytes, 1139 Dateien/Links,
  SHA-256 `01093cd2336a9cba527ae52210c0b441a38c169173246dd9b9c1c0b971793480`.
- `run-stream-sdk-checks`, `close-sdk-checks`, `curve-sdk-checks` mit allen
  vier bestandenen SDK-Unterverzeichnissen: `build/units-recent-archived-proofs.tar.gz`,
  144444440 Bytes, 2305 Dateien/Links,
  SHA-256 `0a066cc47d494736a4b97c7ad1f944896c1e8e37042d16ba5998a0d4e82f617e`.

Jeder Bestand wurde vollständig auf macOS gegen sein Inhaltsinventar geprüft;
Originale vor Entfernen erneut gehasht. Weil zunächst selbst der Platz für eine
zweite Archivkopie fehlte, erfolgte das Entfernen nach der verifizierten Mac-
Sicherung. Danach wurden beide Archive zurück auf Linux kopiert und dort erneut
vollständig geprüft. Aktuelle Inventare und Lösch-/Prüfbelege liegen unter
`build/units-space-*` und `build/units-recent-*`; die früher genannten Pfade
sind daraus wiederherstellbar. Es wurden keine Quell-/Test-/Asset-Dateien entfernt.

Die 21 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/units-source-freeze.json`). PP-0369 und PP-0373 sind mit den genannten
Nachweisen implementiert; vollständige SI-/Metadatenabnahme aller Datenpfade,
Windows/Apple Silicon, Kalibrierung, affine Temperaturskalen und gesamte
Produktabnahme werden daraus nicht abgeleitet.

## Quantity-Werte an der SI-Kanalgrenze am 8. Oktober 2026

`ps_channel_sample_quantity` prüft Kontextprefix, Index, Schema und Dimensionen,
konvertiert die Eingabeskala nach SI und aktualisiert erst danach genau einen
Kanalwert. Jeder Fehler erhält den gesamten Kontext. Physim
`Channel.sampleQuantity` ergänzt Besitzer-/Samplingphasenprüfung und erhält die
ursprünglichen Invalid-/Numeric-/Version-Codes. Der rohe Sampler verlangt
weiterhin bereits in SI vorliegende Zahlen; deren Dimension kann er nicht erraten.
[SI-Grenze](units.md), [Messwert und Unsicherheit veröffentlichen](measurement.md).

Die echte C-/Physim-Runner-Gegenprobe liest einen Zentimetersensor samt
Standardunsicherheit und persistiert 1,25 m sowie 0,01 m. Ein unabhängiger Parser
prüft Run-Header, sämtliche CRCs, Schema, Zeilen, Footer und CSV-Einheiten/Werte.
Abgefangene Dimensions-/Bereichsfehler ändern keine zuvor gespeicherte Länge.
Der separate C-Test prüft ganze Kontexte, verkürzte Prefixe, malformed Schema,
Indexgrenzen, signierte Null, Sensor-/Unsicherheitsübergang, falsche Besitzer,
Samplingphase und strukturierte Diagnosecodes samt Quellposition.

Die Sensor-Wurfbeispiele verwenden den typisierten Weg für Messwert und
Unsicherheit tatsächlich. Der C-Wurf sammelt Werte in einem lokalen Kontext und
committet sie zusammen mit dem Modellzustand. Die Physim-Würfe erhalten separate
Statuskanäle und konvertieren gültige Szenenpositionen ausdrücklich in Meter.
Platzhalter werden durch den Quantity-Setter nicht zu gültigen Messungen.

Intel macOS 14.6.1/Apple Clang 16 besteht die endgültige native Auswahl mit 12/12
unter `build/contact-world-language-release-mac/test-results/run-ds5n4hbw`.
Debian 12/GCC 12.2 besteht dieselbe Auswahl mit 12/12 unter
`build/contact-world-language-release-linux/test-results/run-kn0htfu5`.
Clang 14/ASan/UBSan besteht 12/12 unter
`build/atspi-asan-linux/test-results/run-srg3k2uq`. Die Auswahl enthält C- und
Physim-Kanalläufe, Measurement, Sensor-Nativfälle, Sensor-Wurfparität und
Referenzdokumentation. `ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen
aktiv; macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Sensor-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-d6hx8l5h` und auf Linux/X11 mit 2/2 unter `run-g625xyws` in den jeweiligen
Release-Testverzeichnissen. Alle 31 generierten Referenzdokumente sind geprüft.
Die unabhängige Prüfkiterstellung enthält 116 genau manifestierte Eingaben und
besteht Paketgrenzen, SHA-256, fehlende/dynamische Pfade und exklusive Erstellung.

Der ältere vollständig bestandene SDK-Prüfstand
`build/ui-size-sdk-proof-linux/Native SDK ä 91etu6ft` ist vollständig archiviert:
`build/channel-space-archived-proofs.tar.gz`, 305565157 Bytes, 14467 Dateien/Links,
SHA-256 `2b1dc34c0a197032c2e5934ba7d5a71ecb79137c2ddf2c5faf07745e7d2850e4`.
Inhalt und Links wurden auf macOS vollständig gegen das Inventar geprüft;
Originale vor Entfernen erneut gehasht. Danach wurde das Archiv zurück auf Linux
kopiert und dort vollständig nachgeprüft. Inventar und Lösch-/Prüfbelege liegen
unter `build/channel-space-*`; der alte Prüfstand bleibt wiederherstellbar.
Dadurch stehen wieder rund zwei GiB für tatsächliche Linux-Builds zur Verfügung.

Das Linux-SDK besteht die erweiterte `--units-only`-Abnahme unter
`build/channel-quantity-sdk-checks/Native SDK ä 7jqcwary`: installierter und
allein aus Paketquellen neu gebauter Core, Quantity-Sampler, ursprüngliche
Diagnosecodes, exakte Konvertierung sowie echte C-/Physim-Runner und CSV-Export.
Der Paketcompiler übersetzt die unabhängigen Physim-Fixtures selbst.

Die erweiterte macOS-SDK-Abnahme besteht unter
`build/channel-quantity-sdk-checks/Native SDK ä x8hnbi_r` gegen installierten und
nur aus Paketquellen neu gebauten Core, einschließlich echter C-/Physim-
Sensorläufe und CSV-Gegenprobe. Beide SDKs besitzen weiterhin 459 manifestierte
Dateien; der neue öffentliche Sampler benötigt keinen ABI-Strukturumbau.
Alle 24 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/channel-quantity-source-freeze.json`).

Die umfassenden SI-/Metadatenanforderungen PP-0370/0371/0372/0375 bleiben für
alle Datenpfade einzeln offen. Der neue typisierte Übergang beweist die genannten
konkreten Pfade; er macht aus rohen Doubles keine dimensionsgeprüften Werte.
Weitere Plattformen, allgemeine Produkt-/Modellabnahme und macOS-Sanitizer sind
mit diesen fokussierten Nachweisen nicht abgenommen.

## Starrkörperträgheit und Energiebereich am 8. Oktober 2026

Die alten Formeln liefern in 253 von 1046 unabhängigen rationalen Fällen andere
Rundungen oder Bereichsentscheidungen (`build/body-range-baseline.log`). Konkret
scheitert eine Box mit Masse `1e-300` kg und Kanten `1e200` m am vorzeitig
überlaufenden Quadrat, obwohl ihre Trägheiten endlich sind. Bei einer Kugel mit
kleinster positiver Double-Masse und Radius `1e160` m rundet der alte Vorfaktor
zu null. Die neue positive Integerakkumulation rundet die rationalen
Kugel-/Boxformeln einmal. Identitätsenergie nutzt dieselbe Summation;
allgemeine Quaternionrotation bleibt eine skalierte Binary64-Näherung.
[Vertrag und Grenzen](mechanics.md).

Die unabhängige Prüfung enthält 1046 rationale C-Fälle, 48 Physim-Fälle und
260 gedrehte Körper gegen eine 120-stellige Decimal-Rotationsmatrix. Für die
allgemeine Rotation gilt in diesen Referenzen relative Toleranz `2e-14` und
absolute Toleranz von zwei kleinsten Subnormalwerten, keine Bitgleichheitszusage.
Körper und Energieausgaben bleiben bei Fehlern unverändert. Die erste Sprach-
Fixture wurde wegen einer verbotenen Mutation eines erfassten Werts abgewiesen
(`run-__uaslei`); korrigierte Funktionen verwenden eigene lokale Körperkopien.
Der korrigierte Mac-Vorlauf besteht 4/4 (`run-kwelpa5e`), der zusätzliche
Rotationslauf 1/1 (`run-rbobnntl`).

Die erweiterte native Auswahl besteht auf Intel macOS 14.6.1/Apple Clang 16 mit
26/26 unter `build/contact-world-language-release-mac/test-results/run-3v6rc0qy`
und auf Debian 12/GCC 12.2 mit 26/26 unter
`build/contact-world-language-release-linux/test-results/run-vwkp9oqa`.
Sie umfasst Mechanik, Box-/Paar-/Graphkontakte, Gelenke, Kontaktverwaltung,
Broad Phase, C-/Physim-Stoßparität, echte Stoß-/Boxläufe und Prüfkiterstellung.
Linux/Clang 14 besteht dieselbe Auswahl mit zusätzlicher Kugel-Sweep-Prüfung
unter ASan/UBSan mit 27/27 in
`build/atspi-asan-linux/test-results/run-k9oardvp`.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft.

Die tatsächlichen Physim-App-Workflows für Kugel- und Boxstöße bestehen auf
macOS mit 2/2 (`run-ab687ilq`) und Linux/X11 mit 2/2 (`run-h5p7p25i`) in den
jeweiligen Release-Testverzeichnissen. Alle 31 Referenzdokumente sind geprüft.
Die unabhängige Prüfkiterstellung besteht mit 119 genau manifestierten Eingaben,
SHA-256, Paketgrenzen, fehlenden/dynamischen Pfaden und exklusiver Erstellung.
Ein parallel zum GUI-Build gestarteter SDK-Build wurde durch die Build-Sperre
korrekt zurückgewiesen; erst nach Abschluss dieses Builds wurde er gestartet.

PP-0382 bleibt für umfassende Lage-/Winkelgeschwindigkeits-/Dynamikabnahme offen.
PP-0385 besitzt eine konkrete Lücke: Allgemeine konvexe Körper fehlen im
Narrow-Phase-Vertrag. Keine vollständige Mechanik-, Windows-, Apple-Silicon-
oder gesamte Produktabnahme wird aus diesen fokussierten Nachweisen abgeleitet.

Das macOS-SDK besteht `--body-only` unter
`build/body-sdk-checks/Native SDK ä jn4ccpv4`, das Linux-SDK unter
`build/body-sdk-checks/Native SDK ä 5nk_v3fm`. Beide enthalten 460 manifestierte
Dateien. Installierter und allein aus SDK-Quellen neu gebauter Core bestehen
jeweils rationale C-/Physim- und Decimal-Rotationsgegenproben. Der Paketcompiler
übersetzt die Sprach-Fixture selbst. Unter Linux läuft der Prüfer ausschließlich
aus `build/body-independent-kit`, ohne Implementierung oder Entwicklerheader im
Prüfkit. Die letzte Belegergänzung dieser Dokumentation erfolgte nach Paketierung;
Implementierung, öffentliche Header und Testeingaben blieben unverändert.
Die 16 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/body-source-freeze.json`); SDK-Belege liegen unter
`build/body-sdk-{mac,linux}-PASSED.json`.

## Native benannte Optionsgruppen

Darstellung und beide Schriftgrößen besitzen nun echte native Gruppen mit
Eltern-/Kindbeziehungen, Geschwisterindizes und boolescher Auswahl. macOS meldet
`AXRadioGroup`/`AXRadioButton`, Zahlenwerte und ausgewählte Kinder; Werte und
Änderungen der ausgewählten Kinder werden gemeldet, auch wenn die ausgewählte
Option aus dem sichtbaren Snapshot verschwindet. AT-SPI meldet `grouping` (99)
und `radio button` (44), `checkable`/`checked`, Select-Aktionen, hierarchische
Caches, Indizes und Rahmen in Fenster-/Elternkoordinaten. Gruppenknoten zählen
zum bestehenden 256er-Budget. Core-API/ABI, Sprachvertrag und Dateiformate bleiben
unverändert. [Verträge und Grenzen](accessibility.md).

Die portable Gruppenprüfung belegt unterschiedliche Eltern für gleichlautende
Optionen, exklusive/idempotente Auswahl, späte/gesperrte Aktionen, Kapazität,
UTF-8 und einen ausdrücklich späteren Pointerwechsel nach einer nativen Auswahl.
Die AppKit-Prüfung liest Rollen, Eltern, Kinder, ausgewählte Kinder, echte Rahmen
und Zahlenwerte; eine zurückbehaltene, verschwundene Auswahl kann nicht aktiviert
werden. Der UI-Prüfer nutzt tatsächlich gezeichnete Optionen. Der unabhängige
AT-SPI-Client prüft zwei Fenster mit je zwei gleichlautenden Optionsgruppen,
beide Richtungen der Auswahl, unabhängige Gruppen/Fenster, Parent-Koordinaten,
Zustandsereignisse und deaktivierte/verborgene/veraltete Ziele.

Die tatsächliche App führt sechs native Auswahlen aus: Oberfläche 22→16 px,
Darstellung Hell→Dunkel und Editor 20→16 px. Entwürfe ändern sich; angewandte
Konfiguration bleibt erhalten. Speichern und Neustart bleiben im vorhandenen
Einstellungstastaturprüfer abgedeckt. Die ausgeführten nativen Prüfer sind keine
praktische VoiceOver-/Orca-Abnahme oder ein programmatischer Fokusdienst.

Intel macOS 14.6.1/Apple Clang 16 besteht Modell, Gruppenmodell, Geometrie,
Handbuch, Referenzen und Prüfkit 6/6 (`run-0yzbnazn`). Der endgültige AppKit-/UI-/
App-/Einstellungstastaturstand mit ausgewählten Kindern und verschwundener
Auswahl besteht 5/5 (`run-dhdot21h`). Debian 12/GCC 12.2 besteht die sechs
Prüfungen ohne Fenster 6/6 (`run-adj892wc`) und die vier AT-SPI-/App-/
Tastaturabläufe 4/4 (`run-h8b1xqqr`). Linux/Clang 14/ASan/UBSan besteht dieselben
vier Fensterprüfungen 4/4 (`run-zjum7e5z`) und die drei Modell-/Geometrieprüfungen
3/3 (`run-kl573nta`). `detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen
bleiben aktiv. Eine abschließende Hit-Test-Korrektur gibt in Leerraum zwischen
Optionen die Gruppe selbst zurück. Der Prüfer verlangt ausdrücklich einen
realen Zwischenraum; der zusätzliche AT-SPI-Lauf besteht auf Linux/Release 1/1
(`run-5jwtme3r`) und ASan/UBSan 1/1 (`run-6lpnk13l`).

Die endgültigen verschobenen SDKs bestehen auf macOS unter
`build/a11y-radio-final-sdk-checks/Native SDK ä c4tfdtu1` und Linux unter
`build/a11y-radio-checked-sdk-checks/Native SDK ä ksz3mret`. Beide enthalten 469
manifestierte Dateien und führen die tatsächliche App mit Checkbox- und beiden
Richtungen der sechs Optionsauswahlen aus. Linux verwendet ausschließlich
`build/a11y-radio-checked-independent-kit` mit 138 manifestierten Eingaben, ohne
Entwicklerheader oder Physim-Implementierung. Derselbe fokussierte Prüfschritt
gehört auch zur vollständigen SDK-Prüfung mit `--app-tests`; diese fokussierten
Läufe sind keine erneute vollständige SDK-/Produktabnahme.

Belege stehen in `build/a11y-radio-source-freeze.json` und
`build/a11y-radio-sdk-{mac,linux}-PASSED.json`. Dieser Nachtrag erfolgt nach
Paketierung; geprüfte Implementierung und Testeingaben bleiben unverändert.
Der vollständige Plan bleibt erhalten. PP-0710 bleibt für Dropdowns, Listen,
Text-/Editor-/Fokusdienste, Windows/UIA, Wayland und praktische Screenreader-
Abnahme unvollständig. Die Zahl ungeprüfter Planblöcke bleibt unverändert.

## Konvexe Narrow Phase am 8. Oktober 2026

`ps_convex_mesh` ergänzt geschlossene, nach außen orientierte konvexe
Dreiecksnetze bis zu 64 Vertices und 128 Dreiecken. Paarprüfung benutzt Flächen-
und Kantenkreuzproduktachsen; minimale Trennverschiebung und Oberflächenzeugen
liefern einen repräsentativen gemeinsamen Kontakt. Kugel-/Ebenenkontakte und
konservative Hüllgrenzen ergänzen die API. `Body.withInertia` beziehungsweise
`ps_body_with_inertia` erlauben eigene positive Hauptträgheiten. Die C- und
Physim-Beispiele prüfen einen homogenen regulären Tetraeder; Masseneigenschaften
werden ausdrücklich angegeben, keine Punktwolkenhülle oder Trägheit erraten.
[Netzvertrag, Beispiele und Grenzen](mechanics.md).

Die unabhängige Welt-Vertex-Projektion prüft 1014 Box-/Tetraeder-/Oktaederpaare,
darunter vertauschte Partner. 24 getrennte Paare verlangen Kantenkreuzprodukt-
Trennachsen; Flächennormalen allein reichen dort nicht. Alle 230 erkannten
Kontakte besitzen unabhängig rekonstruierte Zeugen auf beiden ursprünglichen
Oberflächen. Separate analytische Fälle prüfen Kugelkontakte an Flächen/Kanten,
innere und grenzständige Kugelmittelpunkte, Enthaltensein, ein geschlossenes
64-Vertex-/124-Dreieck-Netz, SI-Skalen `1e-200` und `1e200`, offene/konkave/
falsch orientierte Netze, Index-/Kapazitätsgrenzen, Numeric-Fehleratomizität,
konservative AABBs und echte Impulsantwort mit dem bestehenden Solver.

Die ersten Physim-Fixtures benutzten einmal einen nur als Methode verfügbaren
Solvernamen und anschließend einen durch die statische Aabb-Methode verdeckten
Funktionsnamen. Diese Prüferfehler sind unter `run-hc5exynr` und `run-c974ua32`
erhalten; korrigierte Aufrufe verwenden `resolveSingle` und `Aabb.convex`.
Der erste Diagnoseprüfer dereferenzierte nach Entfernen des aktiven Traps noch
das trapabhängige Cleanup-Makro und scheiterte auf beiden Plattformen:
macOS `run-0b9x0n7w`, Linux `run-ndq11iik`, jeweils 22/23. Der Prüfer liest jetzt
den erhaltenen `trap.cleanup` direkt. Ein vorläufiger Feldnamen-Tippfehler wurde
vom Compiler zurückgewiesen (`build/convex-runtime-corrected-mac.log`).
Die fünf endgültigen Trapfälle prüfen Invalid-/Limit-/Numeric-Codes,
strukturierte Diagnosen, Quellpositionen und vollständig abgewickelte Cleanup-
Zustände. Der Debuggernachweis liegt unter `build/convex-runtime-assembly.log`.

Die endgültige native Auswahl besteht auf Intel macOS 14.6.1/Apple Clang 16 mit
23/23 unter `build/contact-world-language-release-mac/test-results/run-ilp0uvf2`
und Debian 12/GCC 12.2 mit 23/23 unter
`build/contact-world-language-release-linux/test-results/run-yelx7jkf`.
Linux/Clang 14 besteht 23/23 mit ASan/UBSan unter
`build/atspi-asan-linux/test-results/run-i3y8hd1q`.
Die Auswahl umfasst neue C-/Physim-Geometrie, Beispiele und Diagnosecodes,
bestehende Mechanik-, Box-/Kontakt-/Graph-/Constraint-, Broad-Phase-/Sweep-
Prüfungen, C-/Physim-Stoßparität, echte Stoßläufe, Energieorakel und Prüfkiterstellung.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft.

Stoß-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-lf9yfvyb` und Linux/X11 mit 2/2 unter `run-4llri2qu` in den jeweiligen
Release-Testverzeichnissen. Alle 31 Referenzdokumente sind geprüft. Das endgültige
unabhängige Prüfkit enthält 124 manifestierte Eingaben; Paketgrenzen, SHA-256,
fehlende/dynamische Pfade und exklusive Erstellung sind geprüft.

PP-0385 ist für die genannten diskreten Geometrieverträge implementiert.
Ein repräsentativer Kontakt ist kein Ruhemanifold; automatische konvexe
Kontaktweltverwaltung, konvexes CCD, aktuelle Windows-/Apple-Silicon- und gesamte
Produktabnahme bleiben gesondert offen. Das Gesamtziel ist nicht erreicht.

Die verschobene SDK-Abnahme `--convex-only` besteht auf macOS unter
`build/convex-sdk-checks/Native SDK ä ply2mr4r` und Linux unter
`build/convex-sdk-checks/Native SDK ä 6hvnic_q`. Beide SDKs enthalten 463
manifestierte Dateien. Installierter und ausschließlich aus Paketquellen neu
gebauter Core bestehen die unabhängigen Geometriegegenproben, Invalid-/Limit-/
Numeric-Diagnosecodes und C-/Physim-Tetraederbeispiele. Der Paketcompiler
übersetzt die Sprachquellen selbst. Die SDK-Sprachbeispiel-Binärdatei läuft
zusätzlich direkt. Unter Linux stammt der Prüfer ausschließlich aus
`build/convex-independent-kit`, ohne Entwicklerheader oder Implementierung
im Prüfkit. Die letzten Paketbelege wurden nach Paketierung ergänzt;
Implementierung, Header und Testeingaben blieben unverändert.
Die 30 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/convex-source-freeze.json`). SDK-Prüfbelege liegen unter
`build/convex-sdk-{mac,linux}-PASSED.json`.


## Lineare konvexe CCD am 8. Oktober 2026

`ps_sweep_convexes` schneidet Eintritts-/Austrittsintervalle sämtlicher
Flächen-/Kantenkreuzproduktachsen bei fester Orientierung. Der Ebenensweep
verwendet den ersten Vertexkontakt. Der Kugel–Netz-Sweep prüft Flächen,
Kantenzylinder und Vertexkugeln. Eine Bewegungshülle vereinigt gepufferte
Anfangs-/Endgrenzen für den gesamten linearen Weg. Displacements bleiben
explizit und unabhängig von gespeicherten Geschwindigkeiten; Fehler erhalten
Körper, Netze, Trefferwert und Flag. Neue Treffer haben Eindringtiefe null,
Anfangsüberlappungen behalten den diskreten Kontakt bei Anteil null.
[Verträge und vollständige Ereignisbeispiele](mechanics.md).

Die C-/Physim-Tetraederbeispiele führen ein kraftfreies elastisches Ereignis
gegen einen statischen Vertex tatsächlich aus: Kugel von `(10,1,1)` mit
`vx=-20` m/s, Kontakt bei Anteil `0.425`, Impulsantwort und anschließende Restzeit
bis `x=13` m mit 200 J kinetischer Energie. Der zentrale Impuls induziert hier
keine Rotation. Das ist ein isoliertes Ereignis, keine Mehrkörpersteuerung.

Die neue unabhängige Gegenprobe enthält 705 Fälle: 500 gedrehte
Box-/Tetraeder-/Oktaeder-Sweeps mit separaten Welt-Vertex-Zeitintervallen,
180 Kugelpfade mit unabhängiger Distanzminimierung und Eintrittsnullstelle
sowie 25 exakt rationale achsenparallele Zeitreferenzen. Darunter liegen
72 Polyeder- und 20 Kugeltreffer. Neue Kontaktpunkte werden auf ursprünglichen
Netzoberflächen zum Ereigniszeitpunkt geprüft, Kugelkontakte zusätzlich auf
der Kugeloberfläche. Anteilvergleich verwendet `2e-9` absolute/relative Toleranz,
Oberflächenzeugen `2e-8` absolute Toleranz in diesen Referenzfällen.

Separate analytische C-/Physim-Fälle prüfen Durchtunneln, beide bewegten Partner,
Streifkontakte, genau einen gemeinsamen Zeitanteil zweier Achsenintervalle,
Intervallende, Anfangsüberlappung auch bei Trennung, lange Wege `1e12` m,
Kugelkontakt an Flächen/Kanten/Vertices, Ebenen, Bewegungshüllen und atomare
Invalid-/Numeric-Fehler. Acht strukturierte Runtime-Traps erhalten die
Invalid-/Limit-/Numeric-Codes, Quellpositionen und vollständig abgewickelte
Cleanup-Zustände. Die erste Orakelversion prüfte auch bei Anfangsüberlappung
fälschlich einen nulltiefen Oberflächenkontakt und wurde unter `run-r09b_1an`
abgewiesen; sie folgt jetzt dem dokumentierten diskreten Überlappungsvertrag.

Intel macOS 14.6.1/Apple Clang 16 besteht 22/22 unter
`build/contact-world-language-release-mac/test-results/run-zfyxprb1`.
Debian 12/GCC 12.2 besteht 22/22 unter
`build/contact-world-language-release-linux/test-results/run-i29eq4e7`.
Clang 14/ASan/UBSan besteht 22/22 unter
`build/atspi-asan-linux/test-results/run-bpqptbnl`.
Die Auswahl enthält neue Sweeps, beide Ereignisbeispiele, Diagnosecodes,
bestehende Mechanik-/Box-/Paar-/Broad-Phase-/Kugel-Sweep-Prüfungen,
C-/Physim-Stoßparität, echte Stoßläufe, 1014 diskrete Geometriefälle,
Dokumentation und Prüfkiterstellung.

Die abschließend verstärkte Kugel-Netz-Zeugenprüfung besteht separat auf
macOS mit 1/1 (`run-z4o3irvx`), Linux/Release mit 1/1 (`run-5xowqt3r`) und
Linux/ASan/UBSan mit 1/1 (`run-un6j2n1m`) in den jeweiligen Testverzeichnissen.
Implementierung und andere Testeingaben blieben seit der Auswahl unverändert.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft. Alle 31 Referenzdokumente sind geprüft;
das unabhängige Prüfkit enthält jetzt 127 genau manifestierte Eingaben.

PP-0387 ist konkret unvollständig: Allgemeine rotierende und beschleunigte
Bahnen sowie Mehrkörper-Ereignissteuerung fehlen. Lineare CCD mit festen
Orientierungen ersetzt diese Anforderungen nicht. Keine allgemeine CCD-,
Windows-/Apple-Silicon- oder gesamte Produktabnahme wird daraus abgeleitet.

Stoß-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-imop6leh` und Linux/X11 mit 2/2 unter `run-4e5sxkkx` in den jeweiligen
Release-Testverzeichnissen. Die Prüfkiterstellung besteht mit 127 Eingaben,
Paketgrenzen, SHA-256, fehlenden/dynamischen Pfaden und exklusiver Erstellung.

Die abschließende Dokumentationsprüfung erkannte einen vorhandenen
Paketgrenzfehler: Durch die fortgeschriebenen Nachweise war diese Seite auf
266081 Bytes gewachsen und überschritt `PS_DOC_MAX_BYTES=256*1024`.
`generate-reference.py --check` wies sie deshalb zurück. Die ersten SDK-
Installationen unter `build/convex-sweep-sdk-{mac,linux}` waren zu diesem
Zeitpunkt gebaut, wurden aber nicht als bestandene Abnahmen ausgegeben.
Die neue SDK-Markdown-Größenprüfung weist das erste Mac-Paket zurück
(`build/convex-sweep-sdk-document-rejection.txt`).

207181 historische Bytes mit SHA-256
`331f4bbddba84bb535ce873161f77cc9720895c8c6dabc12d2e755cf6fe6dcb9`
sind unverändert in [Historische Plattformnachweise](platform-validation-history.md)
übernommen. Die neue Seite besitzt 207237 Bytes einschließlich Rücklink;
die aktuelle Seite liegt wieder deutlich unter der Grenze. Beide sind im
Offline-Handbuch registriert. Verweise aus README, SDK-README und Status
führen zur erhaltenen Historie. Der Abgleich liegt unter
`build/convex-sweep-document-split.json`.

Die Dokumentationsauswahl besteht nach der Aufteilung auf macOS mit 3/3
(`run-dsedhz69`) und Linux mit 3/3 (`run-hfyar3ak`). Die GUI-Nachprüfung nach
der zusätzlichen Offline-Seite besteht auf macOS mit 2/2 (`run-jp4qa776`)
und Linux mit 2/2 (`run-52x7vby5`). Der echte Markdown-Parser prüft inzwischen
zusätzlich beide ausgelieferten Plattformseiten samt Byte-/Blockgrenzen;
diese verstärkte macOS-Auswahl besteht mit 3/3 (`run-aohfjxdw`).

Die verstärkte Parserauswahl besteht auf Linux/Release mit 3/3
(`run-28476klb`) und auf Linux/ASan/UBSan mit 1/1 (`run-36ig_aus`).
Der historische Bericht lädt als 558 echte Markdownblöcke; die aktuelle Seite
besitzt deutlich weniger Blöcke. Beide liegen unter den tatsächlichen
Byte-/Blockgrenzen des ausgelieferten Parsers. Der Textnachtrag dieser Belege
ändert weder Parser noch historische Bytes.

Die korrigierten SDKs `build/convex-sweep-final-sdk-{mac,linux}` besitzen
jeweils 464 manifestierte Dateien. `--convex-only` besteht auf macOS unter
`build/convex-sweep-sdk-checks/Native SDK ä 0sc9q9f9` und Linux unter
`build/convex-sweep-sdk-checks/Native SDK ä rqagd4sc`. Installierter und nur
aus SDK-Quellen neu gebauter Core bestehen beide Geometrieorakel, lineare
Sweeps, Diagnosecodes und die tatsächlichen C-/Physim-Ereignisbeispiele.
Der Paketcompiler übersetzt die Sprachquellen selbst; das installierte
Sprachbeispiel läuft zusätzlich direkt. Unter Linux stammt der Prüfer nur aus
`build/convex-sweep-independent-kit`, ohne Implementierung oder Entwicklerheader
im Prüfkit. Der neue Größencheck prüft die manifestierten Markdownseiten in
allen fokussierten und vollständigen SDK-Modi.

Die 29 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/convex-sweep-source-freeze.json`). SDK-Belege liegen unter
`build/convex-sweep-sdk-{mac,linux}-PASSED.json`. Der letzte Dokumentationsnachtrag
erfolgte nach Paketierung; Programmcode, öffentliche Header und Testeingaben
blieben unverändert. Die historischen 207181 Bytes bleiben bytegleich erhalten.


## Konservative rotierende und quadratische CCD am 8. Oktober 2026

`ps_rigid_motion` erhält vollständige Weltachsenrotationen und quadratische
Translation. `ps_body_motion_pose` kopiert Lage und Orientierung auf dem
expliziten Pfad, ohne Kräfte oder gespeicherte Geschwindigkeiten zu integrieren.
Konvexe Paar-/Kugel-/Ebenen-Anfragen verwenden konservative Abstandsschritte,
Trennachsen und Zeugenrichtungen mit einer oberen Bewegungsschranke. Erfolg
innerhalb der expliziten Distanzhülle liefert einen Kontakt; ein freier Restweg
wird erst nach Ausschluss durch die Schranke gemeldet. `PS_LIMIT` erhält alle
Ausgaben und bedeutet einen offenen Suchfall. Eine nicht auflösbare
Koordinaten-/Rotationsgenauigkeit liefert `PS_NUMERIC` statt eines freien Wegs.
[Pfad, Hülle, Budgets und Grenzen](mechanics.md).

Die unabhängige analytische Gegenprobe enthält 234 Fälle mit 220 Erstkontakten:
60 Größenfamilien jeweils als rotierender Stab gegen Ebene, Kugel und Box;
40 quadratische Wege mit gleicher Anfangs-/Endhöhe und innerem Ebenenkontakt;
zwölf vollständig ausgeschlossene Pfade sowie Budget-/Invalid-Fälle.
Winkel reichen bis zu sechs vollständigen Drehungen. Referenzen lösen die
Sinus-/Kosinus-/Quadratikbedingungen des ersten Kontakts separat und vergleichen
mit der gewählten Distanzhülle, Kontaktpunkt und Normalen. Ein neuer Treffer darf
zwischen der analytischen Hüllgrenze und der idealen Berührung liegen.
Zusätzliche C-Fälle prüfen volle Drehungen mit identischen Endorientierungen,
Posen, konservative Radien/Translationsextrema, 1608 tatsächlich transformierte
Vertices im Bewegungs-AABB, fehlende Auflösung und atomare Budgetfehler.

Physim besitzt `RigidMotion` und `CcdSettings` als kopierbare Werte mit lesbaren,
geschützten Feldern. Strukturen, Arrays, optionale Werte, annotierte Funktionen,
Posen und alle Sweep-/Bounds-Aufrufe laufen tatsächlich. Negative Compilerfälle
verwerfen Feldmutation. 13 Host-Traps prüfen ursprüngliche Invalid-/Limit-/
Numeric-Codes, strukturierte Diagnosen, Quellpositionen und abgewickelte Cleanups.
Die C-/Physim-Tetraederbeispiele prüfen zusätzlich eine volle Drehung mit freien
Endlagen gegen eine Ebene und vergleichen den ersten Anteil mit einer unabhängigen
Winkelreferenz. Die bereits geprüfte lineare elastische Ereignis-/Restzeit bleibt
enthalten; eine automatisch gelöste rotierende Kontaktfolge wird nicht behauptet.

Die erste Sprachfixture scheiterte mit 1/2 unter `run-i6p2tusj`, weil die neuen
Werttypen noch nicht in allen Checker-/Emitter-Grenzen registriert waren.
Die Typgrenzen, Deskriptorerfassung und Größenabschätzung umfassen sie jetzt;
zugleich nutzt die Fixture die bestehende Quaternion-Rotationsmethode.
Die erste Orakelversion benutzte irrtümlich Code 7 für `PS_LIMIT`; die tatsächliche
Enumposition ist 9. Dieser Prüferfehler bleibt unter `run-lw5p8o5d` erhalten.
Die korrigierte analytische Auswahl besteht mit 3/3 (`run-iznldat5`), die
optionale Typprüfung mit 1/1 (`run-j4gkp9e_`).

Intel macOS 14.6.1/Apple Clang 16 besteht die endgültige Auswahl mit 16/16 unter
`build/contact-world-language-release-mac/test-results/run-l9g_yi27`.
Debian 12/GCC 12.2 besteht 16/16 unter
`build/contact-world-language-release-linux/test-results/run-03p9fphg`.
Clang 14/ASan/UBSan besteht 16/16 unter
`build/atspi-asan-linux/test-results/run-1w5wnt0d`.
Die Auswahl enthält neue C-/Physim-Pfade, Compiler-/Wertregressionen,
Diagnosecodes, Tetraederbeispiele, 1014 diskrete Geometriefälle,
705 lineare Sweep-Fälle, Referenzdokumentation und Prüfkiterstellung.
`ASAN_OPTIONS=detect_leaks=0` lässt Address-/UB-Prüfungen aktiv. macOS-Sanitizer
bleiben wegen fehlendem `ld64.lld` ungeprüft. Die anschließende Aufteilung
mehrdeutiger einzeiliger Rückgaben ist rein formatierend; Paketprüfungen bauen
und prüfen diese endgültigen Header/Probes erneut.

Stoß-Workflow und Handbuch-Tastaturführung bestehen auf macOS mit 2/2 unter
`run-gg79ead_` und Linux/X11 mit 2/2 unter `run-r3zytq2y` in den jeweiligen
Release-Testverzeichnissen. Alle 31 Referenzdokumente sind geprüft. Das
unabhängige Prüfkit umfasst jetzt 131 genau manifestierte Eingaben.

Drei bereits vollständig bestandene ältere SDK-Prüfstände sind vollständig
archiviert: `parent-watch-sdk-proof-linux/Native SDK ä m7a3cycd`,
`collision-tutorial-sdk-proof-linux/Native SDK ä 4sht9ghs` und
`monte-carlo-tutorial-sdk-proof-linux/Native SDK ä 9_kllbwq`.
`build/motion-space-archived-proofs.tar.gz` besitzt 559487548 Bytes,
20873 Dateien/Links und SHA-256
`bf58a19bc9bef46775bb7ddaa18a1566421dba8e3d356a992b2460f523e8e4ba`.
Alle Inhalte/Links wurden auf macOS gegen das Inventar geprüft, Originale vor
Entfernen erneut vollständig gehasht. Danach wurde das Archiv zurück nach Linux
kopiert und dort vollständig geprüft. Belege liegen unter `build/motion-space-*`;
die ursprünglichen Pfade bleiben wiederherstellbar. Keine Quellen wurden entfernt.

PP-0387 bleibt unvollständig für allgemeine zeitabhängige Rotations-/Kraftpfade
und Mehrkörper-Ereignissteuerung. Der explizite Pfadvertrag, Distanzhülle und
Work-Budget ersetzen diese Anforderungen nicht. Aktuelle Windows-/Apple-Silicon-
und gesamte Produktabnahme bleiben gesondert offen.

Die abschließende Posenprüfung normiert auch bei Nullrotation gültige,
fast einheitliche Anfangsquaternionen, damit Pose und Bewegungshülle denselben
Rotationsvertrag verwenden. Der zusätzliche C-Fall prüft dies ausdrücklich.
Die drei betroffenen C-/Physim-/Analytikfälle bestehen anschließend auf macOS
mit 3/3 (`run-twvtgor0`), Linux/Release mit 3/3 (`run-0be1rdv_`) und
Linux/ASan/UBSan mit 3/3 (`run-ycufv4hc`). Andere Algorithmen und Tests blieben
gegenüber der erweiterten 16er-Auswahl unverändert.

Das verschobene macOS-SDK besteht unter
`build/motion-sweep-sdk-checks/Native SDK ä s4jc0vbz`, das Linux-SDK unter
`build/motion-sweep-sdk-checks/Native SDK ä ydssmug2`. Beide besitzen 464
manifestierte Dateien. Installierter und ausschließlich aus SDK-Quellen neu
gebauter Core bestehen die drei unabhängigen Geometrie-/Sweep-Orakel,
C-/Physim-Beispiele, typisierte Bewegungs-/Einstellungswerte und erhaltene
Invalid-/Limit-/Numeric-Diagnosen. Der Paketcompiler übersetzt die Quellen selbst;
das installierte Sprachbeispiel läuft zusätzlich direkt. Unter Linux stammt der
Prüfer allein aus `build/motion-sweep-independent-kit`, ohne Entwicklerheader
oder Implementierung im Prüfkit. Die endgültigen Header/Probes besitzen die
formatierend aufgeteilten Rückgaben; die Paketkompilierung prüft diesen Stand.

Die 29 geänderten Dateien stimmen zwischen macOS und Linux per SHA-256 überein
(`build/motion-sweep-source-freeze.json`). SDK-Belege liegen unter
`build/motion-sweep-sdk-{mac,linux}-PASSED.json`. Dieser letzte Belegnachtrag
erfolgte nach Paketierung; Implementierung, öffentliche Header und Testeingaben
blieben unverändert. Vollständige Projektabnahme bleibt offen.


## Kontinuierlicher Mehrkörperschritt und erneute Review-Regressionen

Der externe Bericht vom 6. Oktober enthält sieben Befunde. Ihre bereits
committierten Korrekturen sind oben im historischen Nachweis zugeordnet; am
aktuellen Stand bestehen Core, Analysegrenzen, Autosave, Textdokumente und
Materialtutorial erneut auf Intel macOS mit 5/5 unter `run-j0hzzzhi`.
Debian/GCC besteht dieselben fünf Fälle zusammen mit sieben neuen CCD-/Paket-
Regressionen mit 12/12 unter `run-o3_2r_rt`. CR-006 verwendet weiterhin die
kontrollierte numerische Ablehnung des überlaufenden unskalierten Moments.
Diese Nachprüfung ändert die dokumentierten Grenzen zu ACLs/xattrs nicht.

`ps_ccd_step` integriert jetzt einen vollständigen Kraft-/Drehmoment-Kick und
ereignisgeteilten Drift für Kugeln, Boxen, statische Ebenen und konvexe Netze.
Nach jeder Kontaktgruppe werden Hüllen und Erstkontakte neu berechnet.
ID-geordnete Graph-Lösungen erfassen nahe gleichzeitige Kontakte; Boxen nutzen
Manifolds. C leiht Netze, Physim besitzt kopierbare Modelle und Ergebnis-Snapshots.
Der vollständige Schritt ist atomisch. Ereignisbudget, ungelöste Suche oder
stagnierende Nullzeitlösung erhalten alle Ausgaben. Das Verfahren erster Ordnung
besitzt einen expliziten Kontaktabstand und Residuen; es beweist keine allgemeine
Kraftpfad-/Gelenk-/Solverkonvergenz. [Vertrag und Beispiele](mechanics.md).

Ein unabhängiges rationales 1D-Orakel prüft 181 Folgen mit 470 elastischen
Ereignissen, Endlagen, Geschwindigkeiten, Impuls und Energie. C-/Physim-Runner
führen tatsächlich zwei Stöße innerhalb einer Sekunde aus; ein separater Parser
prüft elf Kanäle, vollständige Zeit, CRC und Abschluss. Weitere C-Fälle prüfen
simultane Kontaktgruppen, ruhende Kugel/Box unter Gravitation, einen rotierenden
Box-Ebenenstoß, Körper-/Modellreihenfolge und unveränderte Ausgaben bei Budget-,
ID-, Kraft- und Kapazitätsfehlern. Die Besitzprüfung provoziert Allocator- und
Referenzsättigungsfehler und behält unabhängige Ergebnis-Kopien.

Die erste Budgetfixture benutzte unbeabsichtigt vorzeichenlose Anfangspositionen;
`run-79f2_wl0` bleibt als fehlgeschlagener Prüflauf erhalten. Nach expliziter
Double-Konvertierung besteht der Fall. Der erste Referenzlauf `run-l39o9zto`
scheiterte 4/5 am fehlenden deutschen API-Zwecktext; dieser ist ergänzt.
Die Allocatorfixture benutzt nach `setjmp` statischen Zustand und eine volatile
Fehlernummer, damit `longjmp` keine unbestimmten automatischen Werte prüft.
Die erweiterte macOS-Auswahl besteht mit 9/9 unter `run-ieqk8gfq`, Debian/GCC
mit 9/9 unter `run-ug__cg2c`. Alle 31 Referenzdokumente sind geprüft;
das unabhängige Prüfkit umfasst 137 manifestierte Eingaben.

PP-0387 bleibt unvollständig für allgemeine zeitabhängige Kräfte/Drehungen;
der neue Controller schließt den begrenzten Mehrkörper-Ereignisablauf.
Aktuelle Windows-/Apple-Silicon- und vollständige Produktabnahme bleiben offen.

Beim abschließenden Bau aller Sprachbeispiele wurde eine neue Emitterregression
sichtbar: Der neue CCD-Drop-Zweig hatte den bestehenden Batch-Drop-Zweig ersetzt.
Dieser ist wiederhergestellt. Alle Sprachbeispiele und Module werden danach
nativ gebaut; die macOS-Auswahl besteht 10/10 (`run-mq3pp23c`), einschließlich
eines tatsächlichen Batch-Workflows. Die vier Batch-Wert-/Fehlerprogramme bestehen
zusätzlich unter `run-acybds0m`. Debian/GCC besteht 12/12 (`run-ix7kcrse`),
einschließlich dieser Batch-Programme, Checker, Werte, CCD und Paketprüfungen.
Linux/Clang/ASan/UBSan besteht die ursprüngliche 14er-Auswahl unter
`run-gpdahpp9` und die sieben betroffenen CCD-/Batch-/Checkerprüfungen mit dem
endgültigen Emitter/Header unter `run-o3zqb1db`. `detect_leaks=0` bleibt gesetzt;
explizite Allocatorbilanzen prüfen die neue Besitzverwaltung zusätzlich.

Die App besteht Autosave, Physim-Feder mit echtem Editor-Speicherpfad,
Stoß-Workflow und Handbuch-Tastaturführung mit 4/4 auf macOS (`run-tamfgs7m`)
und Linux/X11 (`run-7fevhvbf`). Der erste Linux-Fensteraufruf ohne `DISPLAY`
scheiterte 0/4 unter `run-q4ar2y_l`; der korrigierte Aufruf verwendet tatsächlich
Xvfb/Openbox auf `DISPLAY=:99`. Keine Plattformgleichheit wird aus CI abgeleitet.

Die verschobenen SDKs enthalten jeweils 469 manifestierte Dateien. Der endgültige
Prüfer besteht auf macOS unter
`build/ccd-step-final-sdk-checks/Native SDK ä rj46dc_d` und Linux unter
`build/ccd-step-final-sdk-checks/Native SDK ä ocdsm3to`. Beide prüfen installierten
und allein aus SDK-Quellen neu gebauten Core, alle drei bestehenden unabhängigen
Geometrie-/Sweep-Orakel, das neue rationale Ereignisorakel, C-/Physim-Werte,
Allocatorfehler und beide tatsächlichen CCD-Experimentmodule. Unter Linux stammt
der Prüfer allein aus `build/ccd-step-final-independent-kit` mit 137 Dateien;
Entwicklerheader oder Implementierung gehören nicht zum Prüfkit. Das zusätzliche
CCD-Beispiel ist kein neuer App-Projekttemplate; die bestehende achtteilige
Template-Selbsttestauswahl bleibt getrennt. Die Paketprüfung behauptet keine
vollständige Mechanik-/Produktabnahme.

Belege liegen unter `build/ccd-step-sdk-{mac,linux}-PASSED.json` und
`build/ccd-step-source-freeze.json`. Dieser Nachweisnachtrag erfolgt nach
Paketierung; geprüfte Implementierung, Header und Testeingaben bleiben unverändert.


## Native Checkboxen und tatsächlicher Einstellungsentwurf

Die native Accessibility-Schicht veröffentlicht nun zweistufige Checkboxen mit
Namen, aktuellen booleschen Werten und genau einer Umschaltaktion. AppKit meldet
`AXCheckBox`, `NSNumber` 0/1 und Wertänderungen. AT-SPI meldet Rolle 7,
`checkable` (41), gegebenenfalls `checked` (4), `toggle`/`Umschalten` und
Checked-Zustandsereignisse. Die AT-SPI-Werte wurden zusätzlich gegen die tatsächlich
installierten PyAT-SPI-Konstanten geprüft. Optisch unbeschriftete Bibliotheks-
Checkboxen erhalten den jeweiligen Laufnamen. Core-API/ABI, Sprachvertrag und
Dateiformate bleiben unverändert. [Verträge und Grenzen](accessibility.md).

Die portable Modellprüfung prüft Wertänderungen unter stabiler Kennung, späte
Aktionen nach dem Zeichnen, Einzelzustellung und gesperrte/veraltete Ziele.
Der tatsächliche AppKit-Prüfer liest Zahlenwerte und Rollen, prüft Press-Aktionen,
Readonly-Selektoren und zurückbehaltene ungültige Elemente. Der gerenderte UI-
Prüfer schaltet beschriftete und optisch unbeschriftete Controls. Der unabhängige
PyAT-SPI-Client prüft zwei reale Fenster, unabhängige Werte, Checkable-/Checked-
Zustände, zwei Richtungen der Wertänderungsereignisse sowie deaktivierte,
verborgene und entfernte Checkboxen.

Ein zusätzlicher Prüfer führt die tatsächliche App aus. Native Aktionen ändern
den Einstellungsentwurf zweimal und erhalten die bereits angewandten
Darstellungsflags. Bestehende Tastaturprüfungen prüfen Speichern und Neustart.
Die ersten Zusatzprüfer scheiterten auf macOS mit 3/4 (`run-bveppbp9`) und
Linux mit 2/3 (`run-99bzum0f`), weil sie eine noch inaktive Dock-Fläche aktivieren
wollten. Deren Snapshot meldete korrekt deaktivierte Controls. Der korrigierte
Prüfer aktiviert zuerst den Einstellungsbereich; Checkbox-Klicks erfolgen danach
weiterhin ausschließlich über die native Schnittstelle. Das Hilfsverzeichnis
für Testeinstellungen wird vor dem Appstart angelegt.

Intel macOS 14.6.1/Apple Clang 16 besteht Modell, Geometrie, Handbuch, Referenzen
und Prüfkit 5/5 (`run-_qbv8k1s`), die abschließenden AppKit-/UI-/App- und
Einstellungstastaturabläufe 4/4 (`run-8aatbj7v`). Debian 12/GCC 12.2 besteht
die fünf Prüfungen ohne Fenster 5/5 (`run-0y02erkz`) und AT-SPI, tatsächliche App
sowie Einstellungstastaturführung 3/3 (`run-pxt0xohr`). Linux/Clang 14 mit
ASan/UBSan besteht dieselben drei Fensterprüfungen 3/3 (`run-xfv89loo`) und
Modell/Geometrie 2/2 (`run-fds_hcl3`). `detect_leaks=0` bleibt gesetzt;
Address-/UB-Prüfungen sind aktiv. macOS-Sanitizer, Windows/UIA, Wayland und
praktische VoiceOver-/Orca-Abnahme bleiben ungeprüft. PP-0710 bleibt unvollständig.


Das verschobene macOS-SDK besteht unter
`build/a11y-checkbox-final-sdk-checks/Native SDK ä _mmkcqy6`, das Linux-SDK unter
`build/a11y-checkbox-final-sdk-checks/Native SDK ä xu7hiise`. Beide enthalten 469
manifestierte Dateien und führen den tatsächlichen nativen Einstellungsablauf aus.
Der endgültige gemeinsame Prüfschritt wird sowohl von `--accessibility-only` als
auch der vollständigen SDK-Prüfung mit `--app-tests` verwendet. Die fokussierten
Nachweise sind keine erneute vollständige SDK-/Produktabnahme. Unter Linux kommt
der Prüfer ausschließlich aus `build/a11y-checkbox-final-independent-kit` mit 138
manifestierten Eingaben, ohne Entwicklerheader oder Physim-Implementierung.
Alle 31 Referenzdokumente sind weiterhin geprüft. Die letzten Handbuch-/Referenz-
und Paketgrenzen bestehen auf macOS mit 3/3 unter `run-h_mcwa4b`.

Quellgleichheit steht in `build/a11y-checkbox-source-freeze.json`, SDK-Belege in
`build/a11y-checkbox-sdk-{mac,linux}-PASSED.json`. Dieser Nachtrag erfolgt nach
Paketierung; geprüfte App-Implementierung und Testeingaben bleiben unverändert.
Der Plan behält alle 531 Originalblöcke; 33 besitzen Implementierungsnachweise,
7 bleiben konkret unvollständig und 491 weiterhin ungeprüft. Native Checkboxen
ersetzen keine vollständige Tastatur-/Fokus-/Screenreader- oder Plattformabnahme.


## Nativer Tastaturfokus für einfache Controls

Buttons, Checkboxen und Optionen besitzen jetzt Fokusanforderungen, eindeutige
veröffentlichte Fokuszustände und Tastaturaktivierung. AppKit bietet Focus-Getter/
Setter und Fokusmitteilungen; AT-SPI bietet GrabFocus und Fokuszustandsereignisse.
Die D-Bus-Anforderung wartet höchstens eine Sekunde auf den tatsächlichen UI-
Fokus, gibt währenddessen die Servermutex frei und verwirft offene Anfragen bei
Misserfolg. Der UI-Thread aktiviert das besitzende Nuklear-/SDL-Fenster und
veröffentlicht Fokus nur bei tatsächlicher Tastatureigentümerschaft. Echte
Sperren, verdeckende Fenster, verschwundene Ziele und die abgeschlossene
Fensterreihenfolge werden erneut geprüft. Eine erst später im Frame gezeichnete
Überdeckung beendet Fokus und Bedienbarkeit. [Verträge](accessibility.md).

Enter/Leertaste aktiviert einmal, Key-Repeats wiederholen keine Aktivierung.
Tab/Shift+Tab führt durch sichtbare einfache Controls; pro Optionsgruppe ist
nur die ausgewählte sichtbare Option ein Tab-Stopp. Pfeile ändern Auswahl und
Fokus innerhalb der Optionsgruppe. Globale modifizierte App-Tastenkürzel bleiben
verfügbar. Pointerbetätigung und SDL-Fokusverlust geben diesen Fokus frei.
Texte/Container besitzen keine Fokusaktion. Die vorhandene Einstellungen-
Tastaturführung bleibt geprüft. Core-API/ABI, Sprache und Dateiformate ändern
sich nicht; der vollständige Barrierefreiheitsblock bleibt unvollständig.

Die portable Prüfung belegt aufgeschobene/eindeutige Zustellung, Tab-/Options-
navigation, Keyboard-Ownership und deaktivierte/veraltete Ziele. Die gezeichnete
AppKit/UI-Prüfung fokussiert eine Checkbox per Setter, aktiviert sie mit
Leertaste, unterdrückt Key-Repeat und weist ein verdeckendes Fenster ab. Der
unabhängige AT-SPI-Client prüft die bestätigte GrabFocus-Anforderung, Zustände,
Keyboardaktivierung und nicht fokussierbare Ziele an realen Fenstern.
Der tatsächliche App-Prüfer fokussiert `Standardwerte` im zuvor inaktiven Dock-
Bereich. Leertaste, Tab und Pfeiltaste verändern danach den Einstellungsentwurf;
die angewandte Konfiguration bleibt erhalten. Das macOS-Fokusbild unter
`run-nsvjxlco/accessibility_focus_app/files ä/settings/native-focus.bmp` wurde
verlustfrei nach `build/a11y-focus-preview-mac.png` konvertiert und visuell geprüft:
Der helle Rahmen um `Standardwerte` ist deutlich sichtbar.

Der erste zusätzliche App-Prüfer scheiterte auf macOS 5/6 (`run-smsr4jgq`) und
Linux 4/5 (`run-v17hz4cc`) an einem falschen erwarteten Abschlussmarker. Beide
Apps meldeten bereits den richtigen Focus-PASS und Exit 0. Der Marker ist
korrigiert. Die erste abschließende Stackprüfung verwendete irrtümlich Nuklears
private Fenster-Suche; der C17-Compile wies sie ab. Sie verwendet jetzt die
öffentliche `nk_window_find`-API.

Intel macOS 14.6.1/Apple Clang 16 besteht die sieben Modell-/Geometrie-/Handbuch-/
Referenz-/Paketprüfungen 7/7 (`run-engor7h7`) und die sechs AppKit-/UI-/App-/
Tastaturabläufe 6/6 (`run-nsvjxlco`). Debian 12/GCC 12.2 besteht dieselben sieben
Prüfungen ohne Fenster 7/7 (`run-dp8i68pk`), die abschließenden fünf AT-SPI-/App-/
Tastaturabläufe 5/5 (`run-q1_75fjw`). Linux/Clang 14/ASan/UBSan besteht diese
fünf Fensterprüfungen 5/5 (`run-6q7lnxgu`) und vier Modell-/Geometrieprüfungen
4/4 (`run-ut14me1r`). `detect_leaks=0` bleibt gesetzt; Address-/UB-Prüfungen
bleiben aktiv. macOS-Sanitizer bleiben wegen fehlendem `ld64.lld` ungeprüft.

Die verschobenen SDKs bestehen auf macOS unter
`build/a11y-focus-sdk-checks/Native SDK ä _mum46hk` und Linux unter
`build/a11y-focus-sdk-checks/Native SDK ä 96353203`. Beide enthalten 469
manifestierte Dateien und führen die tatsächlichen Checkbox-/Options-/Fokus-
Appabläufe aus. Linux verwendet ausschließlich `build/a11y-focus-independent-kit`
mit 138 manifestierten Eingaben, ohne Entwicklerheader oder Implementierung.
Die fokussierte Prüfung ist keine vollständige neue SDK-/Produktabnahme.
Belege stehen in `build/a11y-focus-source-freeze.json` und
`build/a11y-focus-sdk-{mac,linux}-PASSED.json`. Dieser Nachtrag erfolgt nach
Paketierung; geprüfte Implementierung und Testeingaben bleiben unverändert.

PP-0710 bleibt für Text-/Editor-/Menü-/Dropdownfokus, virtuelle Listen,
Scroll-to-Reveal, Windows/UIA, Wayland und praktische VoiceOver-/Orca-Abnahme
unvollständig. Alle 531 Originalblöcke des vollständigen Plans bleiben erhalten.


## Projektmigration der vorhandenen Formate 1 → 2

Gültige Format-1-Experimente lassen sich ausdrücklich über Build-Einstellungen
oder `physim-build --migrate-project --project DIR` aktualisieren. Neue App-
Projekte verwenden Format 2. Öffnen und normales Speichern erhalten das jeweilige
Format. Migration schreibt nur die Projektbeschreibung und deren vorherige
Fassung als Backup; Format 2 ist ein unveränderter Leerlauf. Der App-Snapshot
stammt vom Öffnen; externe Änderungen blockieren die Migration. Ungespeicherte
Experiment-/Analyse-/Einstellungsänderungen, laufende Jobs und Wiederherstellung
sperren die Aktion. [Bedienung](workspace.md#editor-und-build) und
[Dateivertrag](data-format.md#eigenständige-analyseprojekte).

Der Modelltest vergleicht unabhängige Sollbytes für LF/CRLF, fehlenden letzten
Zeilenumbruch, Unicodekommentare, Erweiterungseinträge, schon vorhandenen Typ,
maximalen Seed sowie gemischte C-/Physim-Quellen. Er prüft Originalbackup und
POSIX-Rechte 0700, veraltete Snapshots bei beiden Versionen, unbekannte/ungültige
Versionen, Größenlimit und nicht schreibbaren Backuppfad mit erhaltenen Ausgaben.
Der Workflow baut tatsächliche C- und Physim-Pendel vor und nach der Migration.
Er erhält Quellen, vorhandene Archive und sämtliche Cachebytes und belegt
identische Mess-/Abschlusspayloads mit eigenständiger Chunk-CRC-Prüfung. Der zweite
Build kompiliert/verlinkt nicht erneut. Die App-Prüfung klickt den sichtbaren
Button und prüft normales Upgrade, extern veränderten Snapshot und schmutzige
Einstellungen; Quellen und Archivbytes bleiben erhalten.

Intel macOS 14.6.1/Apple Clang 16 besteht die fünf Projekt-/Dokument-/Workflow-/
Prüfpaketfälle 5/5 (`run-u25vvaxz`) und die zwei App-/Einstellungsabläufe
2/2 (`run-im0_bati`). Debian 12/GCC 12.2 besteht dieselben fünf Fälle 5/5
(`run-i4wemoul`) und beide Appabläufe unter X11 2/2 (`run-y218evs3`).
Die Handbuch-/Prüfpaketfälle bestehen auf macOS 2/2 (`run-s62q_b99`), die
Referenzprüfung separat 1/1 (`run-mwn6d6aw`). Dies sind fokussierte Läufe,
keine vollständige neue Testmatrix.

Die verschobenen SDKs bestehen unter
`build/project-migration-sdk-checks/Native SDK ä 3p6m72wk` (macOS) und
`build/project-migration-sdk-checks/Native SDK ä 9z54pmav` (Linux). Beide enthalten
469 manifestierte Dateien. Der unabhängige Linux-Prüfer verwendet ausschließlich
`build/project-migration-independent-kit` mit 140 manifestierten Testeingaben,
ohne Implementierung oder Entwicklerheader. Er baut beide Quellsprachen,
vergleicht Messdaten/Cache/Dateien und führt die tatsächlichen drei Appfälle aus.
Die echten Abschlussmarker und Protokolle sind mit SHA-256 in
`build/project-migration-sdk-{mac,linux}-PASSED.json` erfasst; der eingefrorene
Quellstand steht in `build/project-migration-source-freeze-final.json`.
Dieser Dokumentationsnachtrag erfolgt nach Paketierung; die geprüfte
Implementierung und Testeingaben bleiben unverändert.

PP-0708 gilt für die vorhandenen Formate 1 und 2 als implementiert. Unbekannte
künftige Versionen werden erhalten und abgewiesen. Der bestehende Textwriter
besitzt weiterhin ein letztes Vergleich-/Umbenennungsfenster gegenüber parallelen
Autoren und keine Stromausfallgarantie. ACLs/erweiterte Attribute/besondere
Modusbits sind nicht Teil der Rechteerhaltung. Windows, Apple Silicon und die
vollständige Produktabnahme wurden hier nicht erneut ausgeführt. Alle 531
Originalplanblöcke bleiben erhalten: 34 implementiert, 7 unvollständig und
490 ungeprüft; das Gesamtziel bleibt offen.

Linux/Clang 14 mit ASan/UBSan besteht die zwei Fensterabläufe 2/2
(`run-poh95ran`) und die vier Projekt-/Datei-/Buildfälle 4/4 (`run-j9hjo4th`).
`ASAN_OPTIONS=detect_leaks=0` bleibt gesetzt; Address- und UndefinedBehavior-
Prüfungen bleiben aktiv. macOS-Sanitizer wurden wegen des bereits dokumentierten
fehlenden `ld64.lld` nicht ausgeführt.
