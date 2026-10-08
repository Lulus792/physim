# Umsetzungsstand

Stand: erster Entwicklungsdurchstich, ergänzt am 2026-10-07. Der Projektplan ist die Roadmap;
dieses Dokument unterscheidet implementierten Code von noch offenen Produktzielen.

**Gewichtete ODE-Zustände:** Euler, symplektischer Euler, RK4, Verlet und RK45
verwenden skalierte Produkt-/Summenrechnung für problematische Größenordnungen.
Konstante endliche Lösungen scheitern nicht mehr am belegten RK4-Zwischenüberlauf;
echte Stufen-/Endüberläufe bleiben Fehler. [Einheiten und Grenzen](numerics.md).

**Kanonische SI-Series:** Eigene und ausgerichtete Reihen konvertieren deklarierte
Eingabeeinheiten blockweise nach SI und speichern Skala 1. C und Physim rechnen
1 m + 50 cm jetzt als 1,5 m; Analyse, CSV und Berichte verwenden dieselbe
Grundlage. Die Umstellung von value()/unitScale() ist [dokumentiert](series.md).

**Erweiterter Bereich von Quantity-Summen:** C und Physim kombinieren normierte,
kompensierte Umrechnungen vor der Rückskalierung. Darstellbare Summen scheitern
nicht mehr allein an einer separat überlaufenden Konvertierung; kleine Beiträge
und Subnormal-Gleichstände besitzen unabhängige Referenzprüfungen.
[Vertrag und Grenzen](numerics.md).

**Gemeinsame SI-Kanalgrenze:** C und Physim deklarieren Kanäle jetzt über denselben
Validator. Skala 1, begrenzte UTF-8-Metadaten und eindeutige Namen werden vor
jeder Kontextänderung geprüft. Laufdatei und CSV enthalten ausdrücklich nach
SI umgerechnete Werte. [Vertrag](numerics.md).

**Erster realer Gasbaustein:** Ein homogenes Van-der-Waals-Modell ergänzt Druck,
Druckableitung, Energie und Entropiedifferenz in C und Physim. Ein synthetischer
isothermer Vergleich speichert SI-Kanäle und alle Koeffizienten und besitzt
unabhängig geprüfte gemischte Analysen. Phasengleichgewichte, Maxwell-Konstruktion
und Stoffkalibrierung bleiben offen. [Quellen und Grenzen](real-gas.md).

**Materialeigenschaften als SI-Daten:** Konstante und temperatur-/druckabhängige
Tabellen besitzen Quelle, Einheit und geschlossenen Gültigkeitsbereich.
C-/Physim-Modelle wählen die benötigten Daten ausdrücklich; Beispiele speichern
vollständige Tabellenmetadaten und erzeugen gemischte Analysen. Keine Extrapolation
oder erfundenen Stoffdaten. Library und native Projektbuilds verwenden 27
Core-Module. [Vollständige Quellen und Grenzen](properties.md),
[ausgeführte Nachweise](platform-validation.md).

**Basis und explizite RNG-Zustände:** Fünf Basisanforderungen sind konkreten
Implementierungen und Referenztests zugeordnet. Der Normalgenerator legt die
Reihenfolge seiner zwei Ziehungen ausdrücklich fest; unabhängige C-/Physim-Probes
prüfen Zustände, vollständige Seeds und Wertkopien. Die allgemeine Handle-Anforderung
bleibt gesondert ungeprüft. Lokale Referenzen, Sanitizer und isolierte SDK-Prüfungen
bestehen. [Vertrag](measurement.md) und [Nachweise](platform-validation.md).

**Belastbare Timingprüfungen:** Offline-Runner werden anhand fehlender
Scheduling-Wartezeit, identischer Messdaten und echter Steuerungen geprüft;
ein verzögerter Beobachter darf die gemessene Laufzeit beeinflussen. Die
Handbuch-Tastaturprüfung erlaubt langsames Zeichnen und erkennt fehlende
Fokusziele ausdrücklich. Lokale macOS-/Linux- und verschobene SDK-Prüfungen
bestehen; die neue Remote-Matrix bleibt erforderlich. [Nachweise](platform-validation.md).

**Skalierte Series-Numerik:** Array-Helfer, C-Series und Physim-Series verwenden
nun dieselbe Intervallrechnung für Ableitung und Trapezintegral. Darstellbare
Ergebnisse bleiben auch bei extremen Differenzen und subnormalen Signalwerten
erhalten. Echte Überläufe veröffentlichen keinen Handle und verbrauchen keinen
zusätzlichen Scratch-Speicher. [Methoden und Grenzen](numerics.md).

**Projekte per Tastatur:** Tab/Pfeiltasten und Enter bedienen die gesamte
Projektmaske; reine Analyseprojekte überspringen Experimentfelder. Fehlermeldungen
bleiben im Formular lesbar, bestehende Dateien erhalten. C-Kugelstoß, Sensorwurf
und Boxstoß erhalten bei Physim-Auswertung die passende Vorlage.
[Bedienung](workspace.md) und [Nachweise](platform-validation.md).

**Handbuch per Tastatur:** Tab erreicht Lernwege, Themen, Inhalt, Suche,
Navigation, Links und Codekopieren. Sichtbarer Fokus scrollt mit; Lesetasten
bedienen den Text. Gleichzeitige Texteingabe und Tab erhalten das letzte Zeichen.
[Bedienung](workspace.md) und [Nachweise](platform-validation.md).

**Einstellungen per Tastatur:** Tab und Shift+Tab führen durch sämtliche
Einstellungsgruppen mit sichtbarem, automatisch gescrolltem Fokus. Pfeiltasten
wechseln Auswahlen; Enter und Leertaste bedienen Aktionen. Escape verwirft den
Entwurf. [Bedienung](settings.md) und [Plattformnachweise](platform-validation.md).

**Unabhängige UI-Schriftgröße:** Einstellungen bieten 16, 18, 20 und 22 logische
Pixel für die Oberfläche unabhängig von der Codeschrift. Geöffnete Dokumentation,
Menüs und Diagramme wechseln mit; umgebrochene Texte und Schaltflächen erhalten
mehr Platz. Persönliches Format 5 liest weiterhin Formate 1–4. Die automatisierten
Prüfungen kontrollieren Größenwechsel, Neustart, Abbrechen, Standardwerte und
vollständige Zeichenbreiten. Tastaturführung und Screenreader bleiben offene
Produktziele. [Ausgeführte Nachweise und Grenzen](platform-validation.md).

**Vollständiger Ausgangsnachweis (`10d8392`):** Die erneuten Release-Prüfungen einschließlich
der Batch-Sprachbindung bestehen auf dem Intel-Mac und unter Debian
mit jeweils 572/572 Fällen.
Der anschließende [Anforderungsabgleich](project-audit.md) bewahrt sämtliche
Planpunkte und unterscheidet Implementierung, Teilnachweise und fehlende Abnahme.
Die Installation baut jetzt alle 75 kompilierten Sprachprodukte auch ohne
`--examples`. Windows-Prüfungen verwenden logische Textzeilen und ein portables
Pipe-Leseverfahren. Die erneute Remote-CI-Abnahme steht noch aus; lokale Tests
ersetzen diese Releasegates nicht.

**Menüaktion und Editorfokus:** Enter führt die beim Tastendruck gewählte
freigegebene Aktion aus. Eine nachfolgende Pfeiltaste im selben Eingabepaket
kann die Aktion nicht verändern. Maus-/Tastaturprüfungen bestehen auf macOS und
Linux; nach Escape funktioniert die Texteingabe ohne erneuten Mausklick.
[Nachweise](platform-validation.md).

**Hauptmenüs per Tastatur:** F10, Pfeile, Tab/Shift+Tab, Home/End, Enter/Leertaste
und Escape bedienen die Hauptmenüs mit sichtbarem Fokus. Deaktivierte Aktionen
werden übersprungen; Menütasten und Text gelangen nicht in einen aktiven Editor.
Elf betroffene Fensterprüfungen bestehen auf macOS und Linux. Beide isolierten
SDKs bestehen alle bisherigen Gates und die installierten Tastaturmenüs in zwei
Fenstergrößen. Weitere Tastaturführung und Screenreader-Zugang bleiben offen. [Nachweise](platform-validation.md).

**Live-Geschwindigkeitswechsel:** Die GUI-Prüfung berücksichtigt alte gepufferte
Snapshots nach dem Wechsel 4×→1×. Normale und verzögerte C-/Physim-Fensterläufe
bestehen auf macOS und Linux. Ein zusätzlicher direkter Runner-Test prüft die
neue 1×-Rate und identische Referenzdaten; die erneute Apple-Silicon-CI bleibt offen.
[Konkrete Nachweise](platform-validation.md).

**Nativer Projektbuilder und aktuelle Gesamtsuite:** Repository und Projektbuilder
verwenden nun dieselben 26 Core-Module. Acht frische dokumentierte C-/Physim-
Domänenprojekte bestehen unabhängige Lernprüfungen. Die vollständigen aktuellen
Release-Suiten bestehen mit 608/608 auf Intel macOS und Debian; beide korrigierten
isolierten SDK-Prüfungen bestehen einschließlich aller neun grafischen Abläufe.
Eine weitere Apple-Silicon-Geschwindigkeitsprüfung bleibt offen.
[Konkrete Nachweise](platform-validation.md).

**GUI-Geschwindigkeitsprüfung:** Die Fenstertests berücksichtigen das begrenzte
Zeitkonto bei Render-/Pipe-Rückstau. C und Physim bestehen zusätzliche
Ein-Sekunden-Lesepausen unter macOS und Linux; die direkten Runnerprüfungen
behalten ihre Geschwindigkeitsverhältnisse und identischen Referenzdaten bei.
Die erneute Apple-Silicon-CI-Abnahme bleibt offen.
[Nachweise](platform-validation.md).

**Strömungs-Lehrmodelle in C und Physim:** Physim 0.182.0 ergänzt laminare Rohre,
Reynolds/Hydrostatik, passive Drucknetze bis 16 Knoten/32 Kanten und konservativen
periodischen Tracertransport bis 4096 Zellen. Der vollständige Lernpfad prüft
2.412 Messungen, hydraulische Knotenbilanzen, Fourier-/Kontinuumsreferenzen,
Masse, Positivität und 24 gemischte Analysen mit vollständig aufgezeichneten
Tracerpunkten. Netz und Tracer sind ausdrücklich getrennte Lehrmodelle;
ernsthafte CFD bleibt ein eigener späterer Ausbau. [Quellen und Grenzen](fluid.md).

**Wellen und Optik in C und Physim:** Physim 0.181.0 ergänzt exakten Oszillator,
Laufwelle, atomare 1D-Saitenschritte mit 4096 Rechenknoten sowie Reflexion,
Snell/Totalreflexion und dünne Linsen. Der vollständige Lernpfad vergleicht sechs
Gitterprofile mit diskreter Eigenmode und Kontinuum, prüft Energie und zweite
Ordnung bei Verfeinerung und zeichnet alle Knoten des grafischen Beispiels auf.
Physim-Ergebnisse besitzen unabhängige Arrays; Allokationsfehler und verworfene
Schritte geben Besitzer frei. [Quellen, CFL und Modellgrenzen](waves-optics.md).

**Elektromagnetismus in C und Physim:** Physim 0.180.0 bindet Punktladungsfeld,
Potential, Lorentzkraft und ideale Widerstands-/Kondensator-/RC-Funktionen.
`expm1` erhält kleine Exponentialdifferenzen. Der vollständige RC-Lernpfad prüft
2.814 Messungen und 28 gemischte Analysen gegen unabhängige 65-stellige Lösungen,
mit Energieaustausch der Quelle, integriertem Joule-Verlust und vollständigen
Szenen. Homogene Elektrostatik, vorgegebene E/B-Felder und konstante RC-Bauteile
haben ausdrücklich dokumentierte Grenzen. [Quellen und Vertrag](electromagnetism.md).

**Thermodynamik in C und Physim:** Sprachvertrag 0.179.0 liefert zehn reine
SI-Funktionen für ideale Gaszustände, Energie/Entropiedifferenzen, konstante
Wärmekapazität und linearen Wärmefluss. Reservoir und isolierte Körperpaare
verwenden eine exakte Exponentiallösung, mit erhaltenen Ausgaben bei Fehlern.
Der vollständige Lernpfad prüft 2.412 Messungen, sechs unabhängige Decimal-
Szenarien, SI-Metadaten, Szenen und 24 gemischte Analysen. Extremwerte und
abgefangene Fehler sind geprüft. Reale Gase bleiben offen.
[Quellen und Grenzen](thermodynamics.md), [ausgeführte Systeme](platform-validation.md).

**Windows-Dokumentationsprüfung:** Die Artefakte von `3f14f3b` zeigen bei v143
Debug und ClangCL Debug/Release jeweils 576 bestandene Fälle und denselben
CP1252-Lesefehler im neuen Dokumentationsprüfer. Er liest jetzt ausdrücklich
UTF-8; die tatsächliche erneute Windows-CI-Abnahme steht noch aus.

**Zwei vollständige Dokumentationsteile:** [Teil I: C](c-guide.md) und
[Teil II: Physim](physim-guide.md) führen jeweils vom Sprach- und Workfloweinstieg
über Experiment, Messung und Analyse bis zum Debugging und denselben acht
Modelllernwegen. Das Hilfefenster bietet eigene Auswahl und Startnavigation.
Die Einstiegsprogramme, veröffentlichten Quellen und Links bestehen auf macOS
und Debian; beide verschobenen SDKs bestehen auch alle neun grafischen
Beispielabläufe und die Lernweg-Navigation. Der damalige Dokumentationskatalog umfasst 577 Fälle; aktuell sind es 608.
Der zuvor vollständig geprüfte Stand mit 572 Fällen bleibt gesondert dokumentiert.
[Konkrete Nachweise](platform-validation.md).

**Batch und Monte Carlo aus Analysen:** Physim 0.178.0 bindet besitzende,
unveränderliche Serienkonfigurationen und Ergebnisse. Ein ausdrücklicher
Host startet den vorhandenen Controller; feste Schritte, Zielzeit, adaptive
Budgets, SI-Parameter, Sweeps, Grenzen, Quellen, Pause und Wiederaufnahme
verwenden dieselben archivierten Runner wie C und CLI. Messstatus und
Teilfortschritte bleiben abfragbar. Vier Kombinationen vollständiger C-/Physim-Beispiele
prüfen 1024 Rohtrajektorien und ihre Berichte; 40 gemischte Konfigurationen
prüfen Fortsetzung, Fehler und Exklusivität. [Vertrag und Ablauf](batch-language.md).

**Abbruch von Laufserien:** Die mitgelieferten Offline-Runner überwachen den
Elternanschluss auch innerhalb hängender Modellcallbacks. Harter Controllerabbruch
beendet alle aktiven Worker und erhält fertige Archive und Journalzeilen.
Ein-/Vierworker-Tests, Create-/Step-Hänger und Linux-ASan/UBSan sind ausgeführt.
[Vertrag](monte-carlo.md), [konkrete Plattformnachweise](platform-validation.md).

**Gespeicherter Lauf und Monte Carlo:** Die Lernpfade enthalten jetzt vollständige
C-/Physim-Analysen für unabhängige Analyseprojekte mit Archivimport und
Wiederöffnung ohne Build sowie für 256 archivierte Würfe mit unsicheren
Anfangsgeschwindigkeiten. Unabhängige Archivprüfer kontrollieren Ergebnisse,
Einheiten, Wiederholbarkeit und statistische Annahmen.
[Gespeicherte Läufe](saved-run-tutorial.md), [Monte Carlo](monte-carlo-tutorial.md).

**Elastischer und inelastischer Stoß:** Ein vollständiger Lernpfad liefert
C-/Physim-Experimente und Analysen eines zentralen Vakuumstoßes mit wählbaren
Massen, Anfangsgeschwindigkeiten und Restitution. Kontinuierliche Erkennung,
Impulsantwort und verbleibende Schrittzeit erhalten den Ereignisablauf auch
zwischen Messpunkten. Energie und physikalische Dissipation bleiben getrennt;
Impuls und Gesamtbilanz werden überprüft. Unabhängige Formeln prüfen elf
Szenarien, alle Szenenfelder und 44 gemischte Analysen.
[Vollständige Quellen, Gleichungen und Ablauf](collision-tutorial.md).

**Pendel und Vergleich von Integratoren:** Vollständige C-/Physim-Quellen
beschreiben dasselbe konservative nichtlineare Pendel mit fünf wählbaren
Verfahren. Eine Mehrlaufanalyse vergleicht Winkel, Energieabweichungen und
Perioden auf den gespeicherten Zeitachsen, einschließlich adaptiver Läufe.
Ein unabhängiger Test prüft die nichtlineare Periode durch elliptische Quadratur
und die Schrittweitenordnung durch rekursive Taylorentwicklung. Reset,
Parameterinstanzen und Fehlererhaltung sind gesondert geprüft.
[Lernziel, Quellen und vollständiger Ablauf](pendulum-tutorial.md).

**Korrekturen aus dem Prüfbericht vom 6. Oktober:** Hauptquellen, zusätzliche
Texte und Autosaves erzeugen Zwischenfiles exklusiv. POSIX-Speicherung erhält
gewöhnliche Zugriffsrechte einschließlich Ausführbarkeit; Sicherungen erweitern
den Schutz der Quelle nicht. Die Materialreferenz bleibt bei kleiner Viskosität
stabil. Die Standardanalyse maskiert Energie- und Periodenkennzahlen, zeichnet
konstante große Werte endlich und weist überlaufende Statistikakkumulatoren mit
`PS_NUMERIC` zurück. Sekanten und Trapeze vermeiden Zwischenüberläufe; fehlerhafte
Ableitungen lassen die Ausgabe unverändert. Core-ABI und Dateiformate bleiben
unverändert. [Verträge](reference/analysis.md), [Speicherregeln](workspace.md).

**Feder-Lernpfad in C und Physim:** Beide Vorlagen erhalten einen typisierten
Dämpfungsparameter pro Modellinstanz. Vollständige Quellen und Analysen zeigen
vier Dämpfungsfälle, mechanische Energie, integrierte Dissipation und die maximale
Bilanzabweichung. Ein unabhängiger Prüfer vergleicht 16.008 Messzeilen, sämtliche
Szenengeometrien, SI-Metadaten, Verfeinerung und sechzehn gemischte Analysewege.
[Modell, Ablauf und Quellen](spring.md).

**Eigenes Material und Medium:** Ein vollständiger Lernpfad liefert gleiche
C-/Physim-Kugelversuche und beide Analysesprachen. Eigene Materialdichte bestimmt
die Masse; eigene Fluiddichte und Viskosität bestimmen Auftrieb und Stokes-
Widerstand. Manifest, analytische Kontrollkanäle, Energieabrechnung und explizite
Reynolds-/Zeitschrittgrenzen erhalten die Modellannahmen. Ein unabhängiger
Parser prüft sieben Szenarien, Verfeinerung, alle vier Analysekombinationen und
ungültige Modelle. [Quellen, Gleichungen und Ablauf](material-tutorial.md).

**Physim-Kontaktzustände:** Sprachvertrag 0.177.0 bindet `Collider` und
`ContactWorld` als Werte. `solve()` und `reset()` erzeugen neue unveränderliche,
automatisch freigegebene Snapshots; Kopien, Arrays, optionale Werte,
Strukturfelder und Closures können unabhängig weiterlaufen. Körper, alle
Kontaktgeometrien, stabile IDs, Impulse, Lebenszykluszähler und Solverreste
bleiben abfragbar. `ContactSolver.solveWarm()` bindet manuelle Graphseeds.
C- und Physim-Stapel verwenden dieselben Core-Funktionen und liefern gleiche
Messungen/Szenen. [Sprachvertrag und Beispiele](contact-world.md).

**Persistente Kontaktverwaltung:** Ein expliziter C-Zustand erzeugt Kugel-/Box-/
Ebenenkontakte über Broad/Narrow Phase und ordnet sie zwischen erfolgreichen
Schritten mit stabilen Collider-IDs und lokalen Ankern zu. Der Graphsolver
erhält projizierte, zeitabhängig skalierte Warmimpulse; Restitution verwendet
die Geschwindigkeiten vor dem Warmstart. Körper, Cache und Ergebnisse bleiben
bei Fehlern unverändert. Begrenzter Stack-Scratch, kein Heap und keine implizite
Integration. [Vertrag, Stapelbeispiel und verbleibende Grenzen](contact-world.md).

**Physim-Indexabfragen:** Sprachvertrag 0.176.0 ergänzt besitzende `RunIndex`-/
`RunBlock`-Werte und vollständige kopierbare `RunSnapshot`-Zustände. Kopien, Arrays,
optionale Werte und Strukturfelder räumen Ressourcen automatisch auf; `close()`
gibt nur die jeweilige Referenz frei. Abfragen und Kanal-SI-Metadaten verwenden
den geprüften C-Core. `inputPath()` verbindet Analyseauswahl und direkte Abfragen.
[Benutzung und Besitz](run-index.md).

**Laufdatei-Index:** Finalisierte Messdateien erhalten begrenzte CRC-geschützte
Indexseiten vor dem bisherigen Footer. Die neue C-API validiert und rekonstruiert
Checkpoints mit explizitem Allocator, auch für alte und unterbrochene Läufe;
gezielte Messblöcke und Szenenabfragen erhalten Ausgaben bei Fehlern. Writer-
Finalisierung benötigt keinen Heap. Format 1 und ABI 3 bleiben erhalten.
[Vertrag und Kosten](run-index.md).

**Strukturierte Diagnosen:** Eigene begrenzte UTF-8-Werte tragen Fehlercode,
Operation, Argument und ursprüngliche Quellposition ohne globalen Last-error-
Zustand. Experiment- und Analyse-Runner speichern CRC-geschützte Sidecars;
IPC 5 liefert strukturierte Fehler mit Opt-in. Die App verwendet die Felder für
Codeauswahl und zusätzliche Quelldateien. Physim 0.175.0 bindet den kopierbaren
Wert samt Formatierung, Bytes, Dateien und Auslösen; abgefangene Fehler bleiben
lokal. API/ABI 3 behält bisherige Feldpositionen. [Vertrag](diagnostics.md).

**Lokale Szenenkoordinaten:** Explizite TRS-Rahmen erhalten lokale Geometrie
und setzen Translation, Quaternionrotation und nichtuniforme Skalierung über
mehrere Ebenen zusammen. Spiegelungen und resultierende Scherung wirken auf
Meshes, Normalen, Picking und Labels; Sichtbarkeit bleibt separat vererbt.
Gewöhnliche Gruppen behalten ihre bisherige Bedeutung. Snapshot 3 zeichnet
lokale Werte auf und liest Versionen 1/2; IPC 5 grenzt die neue Interpretation ab.
C-/Physim-Bindungen stehen unter [Szenenkoordinaten](scene-frames.md).

**Experiment-Logging:** API/ABI 3 erhält einen optionalen Context-Tail mit
explizitem synchronem Sink. Der Runner schreibt begrenzte UTF-8-JSONL-Sidecars
und sendet mit Opt-in eigene Wire-5-Logevents; die App zeigt Zeit und Schweregrad.
Physim 0.173.0 bindet alle vier Schweregrade mit Bool-Rückgabe. Bestehende
Logdateien werden erhalten, Parameterabfragen erzeugen keine Ausgaben.
Verträge und Grenzen stehen unter [Logging](logging.md).

**Messlücken in Datenreihen:** Explizite Masken erhalten die ursprünglichen Zeilen
und das Alignment. Numerische Operationen, Statistik, CSV und Kurven übernehmen
die Gültigkeit; Ableitungen und PCHIP verbinden keine Lücken, ein kumulatives
Integral bleibt nach der ersten unbeobachteten Strecke ungültig. GUI, SVG und PNG
beachten Segmentgrenzen. Berichtformat 2 ergänzt Masken, Format 1 bleibt lesbar
und wird für normale Berichte weiter geschrieben. Physim 0.172.0 bietet passende
Series-Methoden; API/ABI 3 und die öffentlichen C-Strukturen bleiben erhalten.
Eigenständige Analyseprojekte können eigene Daten ohne Eingabedatei auswerten.
Die Plattformabnahme steht im [Plattformnachweis](platform-validation.md).

**Fehlende Sensor-Endwerte:** Laufserien erfassen nicht fällige und ausgefallene
Endmessungen, ohne die ganze Serie abzubrechen. Statistik verwendet nur gültige
Werte, CSV erhält den Status und ein leeres Feld für fehlende Werte. Berichte
zeigen Anzahl und Anteil der Messabdeckung, auch ohne gültige Messung.
Parameterstudien mit Lücken zeigen einzelne gültige Punkte; Fortsetzung
übernimmt journalisierte fehlende Endmessungen. API/ABI und öffentliche
Dateiformate bleiben unverändert. Gezielte Nachweise stehen im
[Plattformnachweis](platform-validation.md).

**Wiederaufnahme von Laufserien:** Neue Monte-Carlo-Serien und Parameterstudien
speichern einen versionierten Checkpoint ihrer archivierten Konfiguration und
Dateifingerprints. App und CLI setzen unterbrochene Serien in einem neuen
Serienordner fort: journalisierte Endwerte und Messdateien werden vollständig
geprüft und übernommen, nur fehlende Indizes starten erneut. Seeds, Parameter
und feste/adaptive Zeitvorgaben bleiben erhalten. Aktuelle Editoränderungen und
die alte Serie werden nicht verändert. Vollständig vorhandene Serien starten
keine neuen Prozesse. Frühere Serien ohne Checkpoint bleiben nicht fortsetzbar.
Der Katalog umfasst 559 Prüfungen ohne Fenster (544 ohne SDL) und 64 Fensterfälle.
Ausgeführte Nachweise stehen im [Plattformbericht](platform-validation.md).

**Eigenständige Analyseprojekte:** Die App legt jetzt C-/Physim-Auswertungen ohne
Experimentquelle an. Projektformat 2 kennzeichnet den Typ; vorhandene
Experimentprojekte bleiben in Format 1 unterstützt. Der native Builder erzeugt
nur das Analysemodul, die App benötigt keine Parameterabfrage und startet kein
Experiment. Messläufe mit vorhandenen Quellsnapshots lassen sich im Hintergrund
importieren. Daten bleiben bytegleich, Zeitachse und Szenen werden geprüft,
Namenskollisionen erhalten bestehende Dateien und rekonstruierbare Präfixe bleiben
nutzbar. Die Vorlagen zeigen die ersten zwei Kanäle pro Eingabe mit gültigen
Sensorpunkten; eigene Analyseprogramme können die gesamte bestehende API nutzen.
Modelle, native Projektbuilds und C-/Physim-Fensterabläufe bestehen auf macOS und
Debian. Der Katalog umfasst jetzt 516 Prüfungen ohne Fenster (501 ohne SDL) und
52 Fensterfälle. Einzelheiten und Grenzen stehen im [Plattformbericht](platform-validation.md).

**Messkanal-Anzeigeeinheiten:** Live-Werte, Messkurven und Gesamtstatistiken
verwenden jetzt persönliche lineare Einheiten mit passenden Presets oder eigenem
Symbol/Faktor. Kanalname und SI-Dimensionen identifizieren bis zu 64 Auswahlen;
Rohdaten, Modell, CSV und Analyseberichtseinheiten bleiben erhalten. Vorschau,
SI-Rücksetzung, numerische Grenzen und beschädigte Kataloge sind geprüft.
Der Katalog umfasst 515 Prüfungen ohne Fenster (501 ohne SDL) und 51 Fensterfälle.
Die vollständigen Prüfungen ohne Fenster bestehen auf macOS und Debian 515/515;
alle 51 Fensterfälle sind über abgeschlossene Gesamt- und gezielte Läufe belegt.
Ein Autosave-Test erhält eine korrekte Renderframe-Abfolge; Linux-VM-Stalls und
voller Testdatenträger sind im [Plattformbericht](platform-validation.md) getrennt
dokumentiert. Finale Statistikansicht, Plot-Eingaben und verschobene installierte
Apps mit C-/Physim-Projektbuild bestehen auf beiden Systemen. Das Gesamtprojekt
hat weiterhin die offenen Ziele der Roadmap.

Die bewusste Workspace-Wiederöffnung stellt jetzt bis zu 16 zusätzliche Dokumente
in ihrer Reihenfolge, das aktive Dokument und den Hauptbereich wieder her. Cursor,
Unicode-Textauswahl und Scrollposition bleiben auch in den beiden Projekteditoren
erhalten. Gekürzte Dateien begrenzen die Ansichten auf vorhandenen Text; fehlende
Dokumente werden gemeldet. Autosaves werden weiterhin ausdrücklich angeboten.
Workspace-Format 2 liest Format 1; beschädigte oder unbekannte Dateien bleiben
erhalten. Auf einem Intel-Mac mit macOS 14.6.1 bestehen Modelltest, Neustartablauf,
Dateibaum, beide Dokumentfenstergrößen, Buildinvalidierung und Autosave-Ablauf.
Der unveränderte Ausgangsstand besteht lokal 480 von 482 SDL-freien Tests:
macOS-Systempolitik blockiert das Sprachprogramm `language_function_values_runtime`,
und `batch_reference` scheitert an seiner Abschluss-/Zeitprüfung. Zwei zusätzliche
Menüabläufe scheitern nach dem Hilfefenster; der kleine Menütest besteht in einer
gezielten Wiederholung, die Ursache ist noch nicht belegt. Diese Befunde sind keine bestandene
vollständige Mac-Abnahme; bisherige CI-Nachweise gelten für ihre genannten Systeme.

Die anschließende Batch-Diagnose zeigt, dass das 0,25-Sekunden-Limit alle drei
Fehlerprüfungen vor ihrem eigentlichen Fehler beendet. Nur der Hängefall verwendet
weiterhin dieses kurze Limit; Absturz und Schemafehler erhalten fünf Sekunden
für ihren Start. Der Test verlangt jetzt ausdrücklich `PS_IO`, `PS_LIMIT` und
`PS_INVALID` sowie dieselben abgeschlossenen Läufe und die bisherige Fünf-Sekunden-Grenze.
Batchreferenz und parallele Referenz bestehen damit auf dem lokalen Intel-Mac.
Die Menüprüfung isoliert jetzt ihre geplanten Klicks von nativen Maus-/Fokusereignissen.
Eine zusätzliche Prüfung reproduziert die Störung ohne Isolation und besteht mit
Isolation in beiden Fenstergrößen. Menüs, Plot-Eingabeisolation und Fensterstart
bestehen lokal; der aktuelle Katalog enthält 493 Prüfungen ohne Fenster und
36 Grafik-/Fensterfälle. Der anschließende Gesamtlauf besteht 492/493 Tests ohne
Fenster; ausschließlich das Sprach-Testprogramm wird im Debug-Build von macOS
blockiert. Im Release-Build besteht dasselbe Programm. Die Linux-Paket-CI zu
`1b098d7` besteht Paketstart, SDK und alle neun grafischen Projekte auf Debian
und Ubuntu, scheitert aber weiterhin an der nativen Dialogprüfung. Die Annotationen
zu `68bb8a4` zeigen auf beiden Systemen den Hänger bei der ersten Ordnerbestätigung.
Im zugehörigen C17-Lauf bestehen alle Prüfschritte der acht Plattformkombinationen,
einschließlich der 36 Grafikfälle. Der Intel-Mac-Job scheitert anschließend nur
beim Upload beider Ergebnisarchive mit GitHub-ArtifactService-Zeitüberschreitung.

Die native Linux-Dialogprüfung besteht inzwischen lokal unter Debian 12 mit
GTK 3 und in einem Ubuntu-24.04-Container mit GTK 4, jeweils mit dem verschobenen
Release-SDK. Die App behandelt den leeren Pfad, den SDL 3.2.30 beim Zenity-Abbruch
liefern kann, jetzt als Abbruch ohne Workspace- oder Statusänderung. Der Prüfer
bedient echte Dialogelemente über AT-SPI und XTest in Fensterkoordinaten; Unicode-
Tastaturzuordnungen und feste Klickpositionen entfallen. Ordner, externe Datei,
Zusatzordner, Abbruch und fehlender Treiber sind geprüft. Der Debian-Build läuft
in einer lokalen QEMU-VM; beide Dialogprüfungen verwenden X11/Xvfb/Mesa. Die zwei
betroffenen Workspace-Abläufe bestehen außerdem auf dem lokalen Intel-Mac.
Der erneute [Paket-CI-Lauf zu `fa311dc`](https://github.com/PhysicSimulator/physim/actions/runs/37234502965)
besteht Paketierung und sämtliche Prüfungen auf Debian 12 und Ubuntu 24.04,
einschließlich echter Dialoge. Portal-/Wayland-Dialoge bleiben separat zu prüfen.

Die App bietet jetzt zusätzlich Hell und Hoher Kontrast in den Einstellungen.
Die gespeicherte Auswahl gilt für Navigation, Code-Editor, Protokoll, Diagramme
und das schon geöffnete Dokumentationsfenster. Das erweiterte Einstellungsformat
liest Format 1 mit dunkler Darstellung. Migration, Fehlererhaltung und Textkontraste
bestehen unter macOS Debug und Debian 12 Release. Sieben App-Starts je System
prüfen Auswahl, Abbrechen, Standardwerte und Neustarts; tatsächliche BMP-Ausgaben
prüfen Editor, Einstellungen, Diagramme und geladene Hilfe bei 1080 × 740.
Der Katalog umfasst damit 493 Prüfungen ohne Fenster und 37 Grafik-/Fensterfälle.
UI-weite Schriftvergrößerung, vollständige Tastatur- und Screenreader-Bedienung
bleiben offen. Die ausgeführten Umgebungen stehen im [Plattformnachweis](platform-validation.md).

Die Simulationssteuerung bietet jetzt **Zurücksetzen / F7**. Ein laufender Runner
wird im Hintergrund beendet, der alte Datensatz bleibt erhalten und ein neuer
Lauf öffnet pausiert bei 0 Sekunden. Anfangswerte und erster Schritt sind mit dem
bisherigen Lauf identisch, einschließlich Zufallsfolgen und Parameterwerten.
Der neue Zustand ersetzt Verlauf, Szenenauswahl und Analyse erst nach einem
gültigen Anfangssnapshot; fehlende Quellen, Module und hängende Initialisierung
bewahren den bisherigen Zustand. Stoppen ist auch vor dem Versionsabgleich möglich.
Die vier Reset-Abläufe und die bisherigen C-/Physim-App-Abläufe bestehen auf dem
lokalen Intel-Mac und in der Debian-12-VM. Der direkte Katalog umfasst jetzt
493 Prüfungen ohne Fenster und 38 Grafik-/Fensterfälle. Zeitleiste, Docking und
weitere offene Anforderungen bleiben im Projektplan bestehen.
Die [Linux-Paket-CI zu `2afef89`](https://github.com/PhysicSimulator/physim/actions/runs/37236029010)
besteht zusätzlich auf Debian 12 und Ubuntu 24.04; sie enthält noch keinen Reset.

Die Simulation hat eine **Geschwindigkeitswahl von 0,25× bis 16× und Offline**.
Ein begrenztes Echtzeitkonto steuert feste Physikschritte unabhängig von der
Darstellung. Wechsel sind während Lauf und Pause möglich; Einzelschritt und Reset
behalten ihre Bedeutung. Die Auswahl wird im Projekt gespeichert. C- und
Physim-Zufallsmodelle liefern bei verschiedenen Geschwindigkeiten sowie mit
verzögertem Pipe-Leser kanalweise identische Referenzmessungen. Die native
Bedienprüfung bestätigt langsamen/schnellen Betrieb, Live-Wechsel, Offline,
Reset und Wiederöffnung. Der Katalog umfasst jetzt 496 Prüfungen ohne Fenster
und 39 Grafik-/Fensterfälle. Zeitleiste, adaptive Runner-Schritte und Docking
bleiben offen; interne adaptive Integratoren sind davon unabhängig.
Die tatsächlich ausgeführten macOS-/Debian-Prüfungen stehen im
[Plattformnachweis](platform-validation.md).

Die **Zeitleiste** bietet Rückblick, Vor-/Zurückschalten, Klick/Ziehen und
Wiedergabe von Szene und zugehörigen Kanalwerten. Der laufende Versuch bleibt
unverändert; Live kehrt zur aktuellen Ansicht zurück. Neue Läufe speichern
versionierte Szenenblöcke mit CRC in derselben Messdatei. Wiederöffnung führt
keinen Modellcode aus. Alte Dateien zeigen ihre Messwerte ohne Szene; negative
Zeitstempel und gültige Präfixe rekonstruierter Läufe sind unterstützt.
Die begrenzte Vorschau erhält Anfang und letzten Zustand, während die Datei
weiterhin sämtliche Messpunkte und aufgezeichneten Szenen enthält.
Der gemeinsame Katalog umfasst 500 Prüfungen ohne Fenster und 40 Grafikabläufe.
Die vollständigen Release-Läufe bestehen mit 500/500 auf macOS und Debian.
Im lokalen macOS-Debug-Gesamtlauf bleibt das bereits bekannte Sprachprogramm
mit Signal 9 blockiert. Die neun ausgewählten App-Abläufe bestehen auf beiden
Systemen; abschließende Kompatibilitätsprüfungen und SDK-Nachweise stehen im
[Plattformnachweis](platform-validation.md).
Die CI zu `a714459` scheitert vor den Fensterprüfungen. Der lokal reproduzierte
alte Isolationstest erwartet bereits bei der RUN-Bestätigung einen Physikschritt.
Er prüft jetzt zuerst die sofortige Zustandsbestätigung, danach echten Fortschritt
und eine über Zeit unveränderte Pause. Der korrigierte Test besteht lokal auf
beiden Systemen. Docking, adaptive Runner-Schritte und weitere Produktziele bleiben offen.

Das **Docking-Grundsystem** ordnet Seitenleiste, Arbeitsbereich und Protokoll jetzt
als Tabgruppen, horizontale oder vertikale Teilungen und frei platzierte Panels
im Hauptfenster an. Titelzeilen dienen zum Verschieben; Trennlinien und Griffe
ändern die Größe. Ausblenden, Wiederanzeigen, Escape-Abbruch und Zurücksetzen
sind umgesetzt. Einstellungen im Format 3 speichern den validierten Panelbaum,
ausgewählte Tabs und Rechtecke; Formate 1 und 2 erhalten die Standardanordnung.
Die festen drei globalen Arbeitsbereich-Tabs bleiben erhalten. Laufende Versuche
und ungespeicherte Editorinhalte bestehen beim Umordnen weiter. Der Katalog umfasst
nun 501 Prüfungen ohne Fenster und 41 Grafik-/Fensterabläufe. Die zugehörigen
tatsächlich ausgeführten Prüfungen bestehen mit je 501/501 ohne Fenster und
41/41 Grafik-/Fensterabläufen auf macOS und Debian. Details stehen im
[Plattformnachweis](platform-validation.md).
Eigenständige Betriebssystemfenster für Panels, ein separat verschiebbarer
Inspektor und benannte Anordnungen bleiben weitere Produktziele.

Die **Szenenhierarchie** ergänzt benannte Gruppen und Eltern-IDs im unveränderlichen
Snapshot. Der Inspektor zeigt einen aufklappbaren Baum; Sichtbarkeit eines Elternknotens
wirkt auf Nachfahren, während deren eigene Wahl erhalten bleibt. Eine Objektauswahl
öffnet ihre Vorfahren. Sichtbarkeit und auf-/zugeklappte Zweige folgen stabilen IDs.
Alle Koordinaten bleiben Weltwerte; Gruppen erzeugen keine Transformation oder
physikalische Kopplung. C- und Physim-Pendelvorlagen zeigen eine gemeinsame Hierarchie.
Sprachvertrag 0.168.0 ergänzt `group` und `sceneParent`.
Snapshotversion 2 zeichnet Beziehungen auf, Version 1 bleibt als flache Szene lesbar.
IPC 4 grenzt den neuen Transport ab. ABI 3 bleibt durch die ausdrücklich angekündigte
Nutzung bisherigen Objekt-Paddings kompatibel; ein mit eingefrorenem alten Header
gebautes Modul prüft tatsächlich beliebige Padding-Bytes. Der Katalog enthält
503 Prüfungen ohne Fenster (492 ohne SDL) und 43 Grafik-/Fensterabläufe.
Die vollständigen Release-Läufe bestehen auf macOS und Debian jeweils mit
503/503 ohne Fenster und 43/43 Grafik-/Fensterabläufen. Der Hierarchie-Fensterablauf
besteht außerdem unter macOS Debug mit aktiven UI-Assertions.
Die neuen Modell-, Runner- und Bedienprüfungen sowie SDK-Nachweise stehen im
[Plattformnachweis](platform-validation.md). Weitere Produktziele bleiben offen.

**Adaptive Runner-Schritte** ergänzen das feste Raster. Ein ausdrücklich
angekündigter optionaler ABI-3-Callback meldet akzeptierte Dauer und nächsten
Vorschlag innerhalb gespeicherter Grenzen; ältere Descriptoren behalten den
Basisprefix. Der gemeinsame Dormand–Prince-Löser kann nach dem ersten akzeptierten
Schritt zurückkehren. Verwerfungen verändern keinen endgültigen Zustand und
speichern keine Messung. Der Runner prüft Dauer, Hostzeit und nächsten Vorschlag,
verbucht nur die akzeptierte Zeit und erhält bei Fehlern den gültigen Dateipräfix.
Geschwindigkeit, Pause, Einzelschritt, Reset, Zeitleiste und wiedergeöffnete Daten
verwenden diesen Zeitfortschritt. C- und Physim-Pendelvorlagen bieten denselben
adaptiven Weg; ihr fester Modus bleibt erhalten. Sprachvertrag 0.169.0 ergänzt
`rk45StepReported`, `StepInterval` und den optionalen `adaptiveStep`-Einstieg.
Die GUI speichert Auswahl und Grenzen und akzeptiert wissenschaftliche Schreibweise.
Der Katalog umfasst 507 Prüfungen ohne Fenster (496 ohne SDL) und 44 Fensterabläufe.
Die vollständigen Release-Läufe bestehen auf macOS und Debian mit jeweils
507/507 ohne Fenster und 44/44 Fensterabläufen. Zusätzliche macOS-Debug-Prüfungen
und die verschobenen SDKs auf beiden Systemen bestehen ebenfalls.
Die ausgeführten Nachweise stehen im [Plattformbericht](platform-validation.md).
Adaptive Batch-/Parameterstudien und weitere Produktziele bleiben offen.

**Laufserien mit gemeinsamer Zielzeit** ergänzen feste und adaptive Schritte
in CLI und App. Der letzte Schritt endet genau an der Zielzeit, auch wenn sie
zwischen Rasterpunkten liegt. Schrittzahl ist dann ein Budget; fehlende Endzeit,
ungültige Konfiguration oder erschöpftes Budget erzeugen keinen Gesamtbericht.
Manifestversion 4 speichert Ziel, Modus und Grenzen. Der Controller prüft auch
CRC-gültige Dateien auf exakte Endzeit, steigende Zeiten, Intervallgrenzen,
passende Seeds und Metadaten. C- und Physim-Pendelvorlagen bieten Länge und
Anfangswinkel als Parameter; Standards und feste Referenzläufe bleiben erhalten.
Die neue Bedienprüfung vergleicht drei Pendellängen mit unterschiedlichen
Schrittzahlen bis zum selben Zeitpunkt und öffnet ihren Bericht erneut.
Der Katalog enthält 509 Prüfungen ohne Fenster (498 ohne SDL) und 45 Fensterabläufe.
Die vollständigen Release-Läufe bestehen auf macOS und Debian jeweils mit
509/509 ohne Fenster und 45/45 Fensterabläufen; beide verschobenen SDKs bestehen
mit installierter und unabhängig neu gebauter Bibliothek.
Ausgeführte Nachweise stehen im [Plattformbericht](platform-validation.md).
Wiederaufnahme, fehlende Endwerte und weitere Produktziele bleiben offen.

**Explizite Parametereinheiten** ergänzen C und Physim (Sprachvertrag 0.170.0).
Ein optionaler ABI-3-Kontext-Tail trägt eigene Symbole, Skalen und SI-Dimensionen;
bestehende Parameter-/Kontextfelder und die Modul-ABI bleiben erhalten.
Die Pendelvorlagen deklarieren Länge in Metern und Anfangswinkel in Radiant.
Formulare und Studienachsen verwenden die Anzeigeeinheit, während Modelle,
Projektdateien, CLI und Rohdaten SI erhalten. Fehlende Deklarationen bleiben
unbekannt; explizite Dimensionslosigkeit wird gesondert deklariert.
Unvollständige, doppelte oder zwischen Läufen widersprüchliche Einheiten werden
abgewiesen. Unveränderte Auswahlwerte, Defaults und Grenzen bleiben exakt in SI;
Neubauten mit kompatibler Skala erhalten die Auswahl. Der Katalog umfasst
510 Prüfungen ohne Fenster (499 ohne SDL) und 46 Fensterabläufe.
Die vollständigen Release-Läufe bestehen auf macOS und Debian jeweils 510/510
ohne Fenster und 46/46 Fensterabläufe. Finale Zahlen-/Projekt-/Runnerprüfungen
bestehen je 7/7, beide verschobenen SDKs mit unabhängigen Verbrauchern ebenfalls.
Die gezielten API-, Projekt-, Runner-, Bericht- und C-/Physim-Bediennachweise
stehen im [Plattformbericht](platform-validation.md). Die damalige Lücke bei
Messkanal-Anzeigeeinheiten ist inzwischen geschlossen; weitere Produktziele bleiben offen.

**Eigenständiger Inspektor:** Szene, Kamera, Darstellung, Experimentparameter
und Laufgrenzen sind aus der Seitenleiste gelöst und bilden ein viertes
Docking-Panel. Dateibaum und Inspektion lassen sich gleichzeitig oder als
Tabgruppe nutzen; frei platzierte Inspektoren besitzen eigene Größe und
Eingabeführung. Einstellungenformat 4 erhält Format 1–3 und vorhandene
Dreipanelanordnungen. Kleine Simulationsbereiche verwenden zusätzliche Reihen
für Steuerung und Zeitleiste. Die neuen C-/Physim-Bedienprüfungen bewegen,
schließen, öffnen, gruppieren und vergrößern den Inspektor während eines Laufs;
ungespeicherte Quellen und fortschreitende Messungen bleiben erhalten.
Der Katalog umfasst 510 Prüfungen ohne Fenster (499 ohne SDL) und 47 Fensterfälle.
Die vollständigen Release-Läufe bestehen auf macOS und Debian jeweils 510/510
ohne Fenster und 47/47 Fensterabläufe; zusätzliche Debugprüfungen bestehen.
Ausgeführte Nachweise stehen im [Plattformbericht](platform-validation.md).
Separate Panelfenster und weitere Produktziele bleiben offen.

**Benannte Panelanordnungen:** Die Verwaltung unter **Ansicht** speichert bis zu
acht Anordnungen mit UTF-8-Namen. Sie erhält Panelbaum, aktive Tabs, verborgene
Panels, freie Rechtecke, Panelbreiten und Protokollzustand. Gleiche Namen werden
bewusst ersetzt; Anwenden und Löschen sind eigene Aktionen. Quelltexte,
Arbeitsbereich, Theme, Schriftgröße, Kamera und laufender Runner bleiben beim
Anwenden erhalten. Der persönliche Katalog `layouts.bin` ist von Projekten und
Workspace-Pfaden getrennt; beschädigte oder unbekannte Dateien werden erst auf
ausdrückliches Zurücksetzen ersetzt. Der Katalog umfasst 511 Prüfungen ohne Fenster
(499 ohne SDL) und 48 Fensterfälle. Die vollständigen Release-Läufe bestehen auf
macOS und Debian jeweils 511/511 ohne Fenster und 48/48 Fensterabläufe; die neuen
Modelle und C-/Physim-Bedienabläufe bestehen zusätzlich unter macOS Debug.
Ausgeführte Nachweise stehen im [Plattformbericht](platform-validation.md).
Benannte Workspaces ergänzt der folgende Abschnitt.

**Benannte Workspaces:** Unter **Datei** werden bis zu acht benannte Arbeitsumgebungen
verwaltet. Hauptordner, zusätzliche Pfade, offene Dokumente und Editoransichten
kehren bewusst zurück; der Katalog ist vom letzten Workspace und von
Panelanordnungen getrennt. Speichern erfasst Ansichten auch bei laufender
Simulation. Öffnen speichert aktuelle Änderungen und blockiert bei Fehlern,
laufenden Jobs oder ungelösten Wiederherstellungen. Zielprojekte werden vor der
Freigabe des bisherigen Workspace geladen. Dateiformate 1/2 bleiben kompatibel;
Katalogformat 1 erhält beschädigte Dateien bis zum ausdrücklichen Zurücksetzen.
Der Katalog umfasst 512 Prüfungen ohne Fenster (499 ohne SDL) und 49 Fensterfälle.
Die vollständigen Release-Läufe bestehen auf macOS und Debian jeweils 512/512
ohne Fenster und 49/49 Fensterabläufe. Modelle und neue Bedienabläufe bestehen
zusätzlich unter macOS Debug. Ausgeführte Nachweise und der korrigierte
Hintergrundjob-Wartefall stehen im [Plattformbericht](platform-validation.md).
Portable Workspaces, separate Panelfenster und weitere Produktziele bleiben offen.

**Monotone kubische Interpolation:** `PS_RESAMPLE_PCHIP` und
`Series.resampledPchip` ergänzen die bisherigen Resampling-Methoden. PCHIP
verwendet lokale Hermite-Kurven mit stetigen ersten Ableitungen, begrenzten
Endsteigungen und ohne neue Extrema zwischen monotonen Stützstellen.
Skalierte Mantissen-/Exponentenarithmetik verarbeitet endliche Spannweiten
und Sekanten außerhalb der direkten Double-Reichweite, auch ohne breitere
Zahlentypen. Das Verfahren benötigt begrenzten Blockspeicher und erhält
Einheiten, Zielraster, Dataset-Lebensdauer und transaktionale Fehlerbehandlung.
C-/Physim-Analysen erzeugen denselben Vergleichsbericht samt SVG und PNG;
Berichte werden ohne Änderung an Quellen oder Laufdaten erneut geöffnet.
Der Sprachvertrag ist 0.171.0; öffentliche API/ABI bleiben auf 3.
Der Katalog umfasst 514 Prüfungen ohne Fenster (501 ohne SDL) und 50 Fensterfälle.
Die vollständigen Release-Läufe bestehen auf macOS und Debian jeweils 514/514
ohne Fenster und 50/50 Fensterabläufe. Zusätzliche Debug- und ausgeführte
C-/Physim-Prüfungen stehen im [Plattformbericht](platform-validation.md).
Weitere Interpolationsverfahren und andere Produktziele bleiben offen.

Aktueller [Plattformnachweis](platform-validation.md): Der direkte Physim-Build nach
Entfernen der eigenen CMake-Dateien besteht alle acht CI-Kombinationen. Ubuntu 24.04
mit GCC/Clang und macOS 15 auf Apple Silicon/Intel bestehen jeweils 493 Tests ohne
Fenster, 35 Grafik-/Fenstertests und die SDK-Prüfung. Verschobene Mac-App-Pakete
bestehen Signaturprüfungen und vollständige C-/Physim-Sprachabläufe.
Das Release-Paket besteht außerdem auf frischen Debian-12- und Ubuntu-24.04-Systemen:
Start ohne Compiler/Python, verschobenes SDK, native Projekte und alle neun
grafischen Vorlagenabläufe. Diese Linux-Prüfungen verwenden X11/Xvfb/Mesa.
Ein sporadischer Windows-Clang-ASan-Absturz bleibt offen; seine Aufzeichnung als
Minidump ist nun in beiden Windows-Debug-Jobs geprüft. Ältere Entwicklungsnotizen
geben den damaligen Prüfstand wieder.

Projektdateien speichern jetzt zusätzlich den Zeitschritt und den vollständigen
64-Bit-Seed. Die App prüft beide vor Speichern und Laufstart. Modelltests bestehen
mit MSVC, Clang und AddressSanitizer auf Windows. Der App-Test prüft Neustart,
Release-Build, Ablehnung eines überlaufenden Seeds und einen echten Runnerlauf
mit 0,125 Sekunden und Seed 18446744073709551615 einschließlich der Messdatei.

Ein zusätzlicher direkter Entwicklungsbuild (`tools/build.py`) baut jetzt auch
Physim selbst über die Compiler-/Archiviererprogramme. Bibliotheken, Sprachcompiler,
Runner, Projektbuilder und Oberfläche benötigen dabei keinen CMake-Aufruf.
Windows/MSVC besteht Debug und Release sowie den SDL-freien Build und die drei
Core-/Numerik-/Mechanik-Referenzprogramme. Vollständige C- und Physim-App-Abläufe
bestehen mit dem direkt gebauten Debug-Programm. Der Buildtest prüft Unicodepfade,
unveränderte Ausgaben, Headeränderungen, Compiler-/Linkerfehler mit erhaltenem
Programm, beschädigte Cacheausgaben und die Sperre gleichzeitiger Builds.
Der direkte Buildschritt besteht außerdem in allen acht CI-Kombinationen unter
Windows (MSVC/ClangCL, Debug/Release), Linux (GCC/Clang) und macOS (Apple Silicon/Intel).
Die Linux-/Mac-Prüfung schließt vollständige C- und Physim-App-Abläufe ein.
[Nachweise und genaue Abdeckung](platform-validation-history.md).
Der direkte Testkatalog umfasst alle 528 übernommenen Prüfungen: 493 ohne Fenster
und 35 Fenster-/Grafiktests. Ohne SDL bleiben 482 Prüfungen verfügbar. Die bisherigen
Repository-Dateien für CMake sind entfernt. Die CI verwendet jetzt ausschließlich
den direkten Physim-Build. Der SDL-Quellbuild
verwendet weiterhin dessen eigenes Buildsystem.

Der direkte Build unterstützt inzwischen auch Sanitizer. Lokal erkennt MSVC den
absichtlich eingebauten Speicherfehler und besteht zehn instrumentierte Core-,
Speicher-, Berichts-, Mutations- und Sprachspeichertests. Unter Windows bestehen
die Probe und diese zehn Prüfungen in der CI mit MSVC und ClangCL. Unter Linux
GCC und Clang bestehen zu `f6cedbf` alle 493 Prüfungen ohne Fenster und alle
35 Grafikprüfungen mit ASan/UBSan. Auf macOS Apple Silicon und Intel bestehen zu
`69ddae7` alle 482 SDL-freien Prüfungen mit LLVM 20 und LLD.

Auch der optionale IPC-libFuzzer hat jetzt einen direkten Buildmodus
`--fuzzer`. Kernbibliothek und Harness erhalten Clangs Abdeckungsinstrumentierung
und Sanitizer. Ein eigener Prüfer erzeugt gültige Eingaben, spielt sie erneut ab
und verlangt bei 10.000 libFuzzer-Durchläufen zusätzliche Codeabdeckung.
Der lokale ClangCL-Build besteht; der Laufzeitstart bleibt durch den bereits
dokumentierten ASan-Fehler blockiert. Die Kampagnenprüfung besteht in der CI
unter Windows ClangCL und Linux Clang sowie zu `f6cedbf` unter macOS auf Apple
Silicon und Intel mit LLVM 20 und LLD.

Ein weiterer Linux-GCC-Lauf hat im UI-Benchmark eine noch ausstehende
Fenstergrößenänderung sichtbar gemacht: Das wiederhergestellte Bild hatte noch
640 × 480 statt 1080 × 740 Pixel. Der Benchmark wartet nun auf den Abschluss
und kontrolliert die Größe vor dem Rendern. Der Bildvergleich besteht lokal
mit MSVC Debug und Clang Release sowie zu `f6cedbf` unter Linux GCC und Clang,
dort auch mit ASan/UBSan.

Die direkte SDK-Prüfung übernimmt nun auch den vollständigen bisherigen
Sprachmodulvergleich: 27 neu gebaute Module, neun Experimente mit beiden allgemeinen
Sprach-Analysen, Sensoranalyse und sechs gemischte C-/Physim-Auswertungen.
Core-Quellen und alle acht C-Vorlagen werden unabhängig von den ausgelieferten
Bibliotheken neu gebaut. Die erweiterte Prüfung besteht lokal mit einem SDL-freien
MSVC-Release-SDK und mit Clang Release einschließlich neun Projektbuilds und
beider grafischer Abläufe. Der erweiterte SDK-Schritt zu `f6cedbf` besteht unter
Linux GCC/Clang, macOS Apple Silicon/Intel und Windows MSVC/ClangCL Release.

Die neuen Sanitizer-Prüfungen decken zusätzliche Probleme auf: Der Nullzeiger-
Vergleich im Clipboard-Test ist korrigiert; alle 493 Sanitizer-Tests ohne Fenster
bestehen unter Linux GCC und Clang im Lauf zu `f6cedbf`. Die ASan-Registrierung beim erneuten
Laden von Modulen auf macOS wurde auf Apples Destruktorausgabe eingegrenzt. Eine gezielte Prüfung
wiederholt den Modulwechsel und kontrolliert den Speicherschutz nach dem
erneuten Laden; lokal besteht sie mit MSVC und in der CI mit Apple Clang auf Intel.
Auf Apple Silicon reproduziert sie den Fehler mit Apples veralteter ASan-
Destruktorausgabe. Mit LLVM 20 und LLD besteht diese Minimalprüfung im Lauf zu
`69ddae7` auf beiden Mac-Architekturen. Die vollständige Suite besteht dort auf
Apple Silicon und Intel mit jeweils 482 Prüfungen.
Beide Mac-Fuzzerkampagnen bestehen
im CI-Lauf zu `f6cedbf` mit LLVM 20 und LLD.
Der sporadische Linux-Startfehler wird durch feinere SDL-/Fenster-/GL-Protokolle
weiter eingegrenzt. Die CI zeichnet beim ersten Fensterablauf zusätzlich
Systemaufrufe auf. Der betroffene Test behält seine Zeitgrenze und wird bei
einem Fehler nicht automatisch wiederholt. Zu `1790d87` bestehen die neue
Trace-/Prozessbereinigungsprüfung und alle 35 normalen Grafiktests mit GCC und
Clang; mit Clang bestehen auch alle 35 instrumentierten Grafiktests. Der zuvor
beobachtete sporadische Startfehler ist damit noch nicht als behoben nachgewiesen.

Der direkte Builder kann nun außerdem ein SDK in einen neuen Ordner installieren.
Es enthält die App, Werkzeuge, alle acht C-Beispielmodule, Analysemodul, öffentliche
Header, Kernbibliothek, SDK-Quellen, Vorlagen, Dokumentation und Lizenzen. Windows
verwendet dafür Release und die Visual-Studio-Laufzeitbibliotheken. Mac-App-Pakete
können dieses SDK über `package-macos.py --sdk` ohne CMake übernehmen.
Die Paket-README beschreibt den Start der bereits gebauten App, die benötigten
Compiler für Nutzerprojekte und die mitgelieferten CLI-Beispiele. Die Anleitung
zum Bauen des vollständigen Repositorys bleibt in dessen eigener README.
`verify-native-sdk.py` prüft ein verschobenes Paket mit unabhängigen Headern,
15 Sprachprogrammen, mitgelieferten Modulen und neu gebauten C-/Physim-Projekten.
Bei dieser Prüfung wurde die Windows-Compilererkennung für unterschiedlich
geschriebene Umgebungsvariablennamen korrigiert; die Windows-API übernimmt jetzt
die Suche nach `ProgramFiles(x86)` unabhängig von der Großschreibung.

Der direkte Build übernimmt mit `--examples` außerdem die 15 eigenständigen
Sprachprogramme und 27 Module des bisherigen CMake-Beispielprojekts. Lokal bestehen
MSVC Debug und Clang Release: alle Programme laufen, das Energiebeispiel liefert
die erwarteten Werte und das Sprachpendel erzeugt über Runner und Analyse einen
Bericht. Ein isolierter Test prüft unveränderte Ausgaben, Sprachfehler mit
erhaltenem C-Code/Programm und anschließende Fehlerkorrektur. Der gemeinsame
Beispielkatalog besteht außerdem die vollständige Prüfung eines verschobenen
MSVC-Release-SDKs (`build/native/example-sdk-check/Native SDK ä yfndauk_`).
Die Sprach- und Tutorialanleitungen verwenden nun die direkten Buildbefehle. Die neue
Beispielprüfung besteht im Lauf zu `87c8ae8` in allen acht CI-Kombinationen.

Die CI baut und prüft Physim jetzt vollständig über den direkten Builder; die
doppelten CMake-/CTest-Schritte entfallen. Neue SDKs enthalten keine
`CMakeLists.txt` oder CMake-Anbindungen mehr, und die SDK-Prüfung kontrolliert das.
`--app-tests` übernimmt alle acht C-Vorlagen plus den vollständigen Sprachablauf;
Linux prüft alle C-Vorlagen zusätzlich mit der instrumentierten App. Mac-Pakete
entstehen ausschließlich aus dem nativen SDK und werden nach dem Verschieben
erneut auf ihre Signatur geprüft. Im Lauf zu `558c2ee` bestehen alle acht
Plattformjobs dieses direkten CI-Ablaufs. Lokal besteht das neue MSVC-Release-SDK die
vollständige Prüfung einschließlich aller neun grafischen Abläufe unter
`build/native/direct-only-verification/Native SDK ä sv58dunt`. Der direkte
Release-Benchmark besteht mit 100.000 Samples und fünf Wiederholungen unter
`build/native-ci-benchmark-results`. SDL verwendet weiterhin sein eigenes Buildsystem.

Die 90 bisherigen Physim-CMake-Dateien sind jetzt entfernt: Root-Build,
SDK-Anbindungen, Beispielbuilds und alte Testaufrufe. Testprogramme, Sprachquellen
und die direkten JSON-Kataloge bleiben erhalten; der Katalog umfasst weiterhin
528 Fälle. README, Tutorials, CRC- und Fuzzing-Anleitungen verwenden den direkten
Builder und dessen tatsächliche Ausgabepfade. Die drei Replay-Aufrufe sind mit
abgeschnittenen Eingaben aus ihren Testordnern geprüft; Arbeitsdateien bleiben
unter `build/`. Drittanbieterquellen behalten ihre eigenen Builddateien.
Nach der Entfernung bestehen lokal alle 493 Tests ohne Fenster mit MSVC Debug
(`build/native/Debug/test-results/run-9wu8gqqs`) und alle 35 Grafik-/Fensterprüfungen
mit MSVC Release (`build/native/Release/test-results/run-hxhqdkcm`), einschließlich
der beiden Benchmark-Prüfer. Auch das bereinigte Repository besteht im
[CI-Lauf zu `7d39aa7`](https://github.com/PhysicSimulator/physim/actions/runs/36502055743)
alle acht Plattformjobs.

Der native Projektbuilder schreibt Sprachübersetzungen jetzt zunächst in eine
temporäre Datei. Ein Sprachfehler lässt den bisherigen generierten C-Code und
die lauffähigen Module bestehen; identische Ausgaben behalten ihren Zeitstempel.
Die vorherigen Fehler sind unter `build/emission-regression-before.log` reproduziert.
Der erweiterte Projektbuild-Test besteht mit MSVC und Clang, einschließlich
Fehlern in Experiment und Analyse sowie anschließender Korrektur. Beide Profile
werden geprüft. Der vollständige grafische Sprachablauf besteht zusätzlich mit
MSVC Debug (`build/native/Debug/test-results/run-kzh44qyj`).

Der native Projektbuilder erkennt nun beschädigte Objektdateien und Module über
Dateigröße und CRC32-Prüfsummen in `build.artifacts` im jeweiligen Buildordner.
Er kompiliert betroffene Objekte neu und verlinkt beschädigte Module erneut.
Fehlende oder unvollständige Prüfdaten lösen einen vollständigen Neubau aus;
unveränderte Prüfdaten behalten ihren Zeitstempel. Der frühere Fehler ist unter
`build/cache-regression-before.log` auch bei unveränderter Größe und Zeit reproduziert.
Die Reparaturprüfung besteht mit MSVC Debug (`run-44khj5c2`) und Clang Release
(`run-88j2cd1_`): Nach jeder Beschädigung laufen Experiment und Analyse wieder.
Die grafischen Projekt-Einstellungs- und Sprachabläufe bestehen zusätzlich mit
MSVC Debug (`run-yd0x76sr`). Der plattformübergreifende CI-Lauf dafür folgt.

Ein separater Linux-Paketlauf baut nun ein Release-Archiv unter Debian 12 und
prüft es in frischen Debian-12- und Ubuntu-24.04-Containern. Die App muss zunächst
ohne Compiler, Python, CMake oder SDL-Entwicklungsdateien starten. Anschließend
prüft ein unabhängig übertragenes Prüfpaket das verschobene SDK einschließlich
aller neun grafischen Projektabläufe. Lokal besteht dieses separate Prüfpaket
mit dem MSVC-Release-SDK vollständig, einschließlich aller neun Oberflächenabläufe
(`build/native/clean-kit-verification/Native SDK ä 50brmrgx`). Workflow-Struktur und
Shellsyntax sind geprüft; der tatsächliche Linux-Paketnachweis steht noch aus.
Im bisherigen Lauf zu `0321c85` ist außerdem ein sporadischer Windows-Clang-ASan-
Absturz sichtbar geworden: Core scheitert ohne Stacktrace, während derselbe Commit
im zweiten Repository alle zehn Sanitizer-Tests besteht. Der
[Plattformnachweis](platform-validation.md) hält beide Ergebnisse fest.

Der neue Debian-12-Release-Build besteht inzwischen alle 493 Tests ohne Fenster
und erzeugt das SDK. Die beiden frischen Installationssysteme haben einen Fehler
im Testaufbau offengelegt: Openbox installierte Python vor dem Starttest. Die
Installation von Openbox erfolgt nun später; der Start ohne Python bleibt ein
verbindlicher Prüfschritt. Die vollständige Paketprüfung folgt erneut.

Für den offenen Windows-Clang-ASan-Absturz sammeln die Debug-CI-Jobs nun
Minidumps des Core-Tests sowie das passende Programm, PDB und Laufzeit-DLLs.
Eine echte Zugriffsverletzung prüft vorher die Dump-Aufzeichnung. Der Prüfer ist
lokal kompiliert und mit ProcDump samt Dump-Auswertung geprüft; WER selbst wird
im CI-System geprüft. Ein lokaler Core-Absturz ist ebenfalls als Minidump erfasst:
Die Fehleradresse liegt in der ASan-Laufzeit, die Gleichheit mit der CI-Ursache
ist noch unbewiesen. Details und Nachweise stehen im Plattformdokument.

Der Paketlauf zu `620246b` besteht jetzt vollständig auf Debian 12 und Ubuntu
24.04, einschließlich SDK und aller neun Oberflächenabläufe. Die README enthält
die konkreten Befehle für das heruntergeladene Release-Archiv; dessen tatsächliche
ZIP-/TAR-Struktur, Ausführungsrechte und 224 Dateiprüfsummen sind geprüft.
Paket- und Quellbuildanleitung nennen zudem Zenity für die native Dateiauswahl
auf Desktops ohne Portal. Die native Auswahl selbst bleibt eine separate Abnahme.

Ein neuer Dialogtest öffnet die tatsächlichen SDL-Systemdialoge und prüft
Hauptordner, zusätzliche Datei und Ordner sowie Abbrechen mit Umlautpfaden.
Der Ablauf besteht lokal unter Windows mit MSVC Debug. Die Linux-Paket-CI
erhält einen externen Zenity-Prüfer, einschließlich fehlendem Dialogtreiber;
dessen Ausführung unter Debian und Ubuntu steht noch aus. Der Paketlauf zu
`ae31aee` besteht bereits auf beiden Systemen mit installierter Zenity-Abhängigkeit.

Der erste native Dialoglauf zu `bd359c7` bleibt auf Debian und Ubuntu nach der
Ordnerpfadeingabe im Auswahlfenster hängen; alle vorherigen Projektabläufe bestehen.
Der Prüfer aktiviert jetzt Zenitys OK-Schaltfläche und zeichnet Dialogbilder auf.
Zu diesem Commit ist die neue Dialogabnahme noch nicht bestanden. Der aktuelle
lokale Nachweis für Debian und Ubuntu steht am Anfang dieses Dokuments.

Die Mac-Paketprüfung erhält außerdem einen Start über LaunchServices (`open`)
mit Systempfad und ohne Compiler-/SDK-Vorgaben aus der Entwicklershell.
Der Prüfer verlangt vollständige C-/Physim-Oberflächenabläufe, gespeicherte
Ergebnisse und eine weiterhin gültige Paketsignatur. Die dafür ergänzte
Abschlussmeldung nach dem Aufräumen ist lokal mit beiden Windows-App-Abläufen
geprüft (`build/launch-check-c.log`, `build/launch-check-language.log`). Der
tatsächliche LaunchServices-Start auf beiden Mac-Architekturen steht noch aus.

Der Projektbuilder nennt bei fehlendem Compiler nun die zur Plattform passende
Einrichtung: Visual Studio unter Windows, Xcode Command Line Tools unter macOS
und GCC/Clang unter Linux. Der erweiterte Projektbuild-Test besteht mit MSVC
Debug (`run-y6luocgh`); der Clang-Release-Builder ist gebaut und der Fehlerpfad
mit einem nicht vorhandenen Compiler separat geprüft.

## Bisherige Umsetzungsschritte des direkten Builders

Die folgenden Abschnitte dokumentieren die einzelnen Übertragungen. Ihre
Testzahlen und damaligen offenen Punkte beschreiben den jeweiligen Zwischenstand.

Der direkte Testläufer übernimmt jetzt 51 vorhandene C-Referenztests ohne CTest;
41 davon benötigen kein SDL. Der vollständige übernommene Satz besteht lokal
mit MSVC Debug, die 41 SDL-freien Fälle zusätzlich mit Clang Release,
einschließlich Protokoll-/Messdatei-/Berichtsmutationen und der
erwarteten Sprach-Laufzeitfehler. Die Prüfbedingungen der C-Tests bleiben erhalten.
Eigene Unterordner isolieren ihre Dateien; JSON-Berichte halten Kommando, Ausgabe,
Dauer und Status fest. Fehler stoppen nachfolgende Fälle nicht, führen aber zu
einem fehlgeschlagenen Gesamtlauf. Der Test des Läufers prüft erwartete Exitcodes,
Diagnosetexte, Timeout, fehlende Programme, Buildfehler und unvollständige Erfolge.
Die übrigen Sprach-, Runner- und Grafikabläufe bleiben bis zu ihrer eigenen
Übernahme an CTest gebunden.

Elf Sprachgruppen für Array- und Stringoperationen laufen jetzt ebenfalls direkt:
elf übersetzte Laufzeitprogramme und 86 Diagnosefälle in elf Gruppen. Damit enthält
der direkte Testkatalog 73 Einträge beziehungsweise 63 ohne SDL. Die 29 ausgewählten
Sprachtests bestehen lokal mit MSVC Debug und Clang Release. Die elf bisherigen
CTest-Diagnoseprüfungen bestehen mit demselben JSON-Katalog weiterhin. Der Buildtest
prüft erzeugte C-Quellen unter Unicodepfaden und hält ihre Objektdateien im Buildordner.
Der Testläufer prüft außerdem, dass fehlgeschlagene Übersetzung keinen alten Code
ausführt und ein Diagnosefehler spätere Fälle nicht unterdrückt. Die neuen
Sprachgruppen bestehen inzwischen im direkten Build- und Testschritt aller acht
Plattformkombinationen zum Commit `199a8f7`.

Die folgende Erweiterung übernimmt alle verbleibenden Array-/String-Diagnosegruppen,
weitere vorhandene Laufzeitprogramme und das Experimentmodul für sicheres Entfernen
von Arrayelementen. Der direkte Katalog enthält damit 134 Tests, davon 124 ohne SDL.
36 Gruppen prüfen 297 abgelehnte Programme sowie zwei Ausgaben mit LF-/CRLF-Eingabe;
47 Programme prüfen normales Verhalten, zwei erwartete Laufzeitfehler und den
Modulablauf einschließlich Reset, Szene und Freigabe. Alle 134 direkten Tests bestehen
lokal mit MSVC Debug, die 90 Sprachtests zusätzlich mit Clang Release. Alle 82
betroffenen CTest-Prüfungen bestehen ebenfalls. Die Einstiegspunkte verwenden denselben
Katalog; die Umwandlung bereits vorhandener CRLF-Zeilenenden wurde für CMake unter
Windows korrigiert. Der direkte Build- und Testschritt dieses Stands besteht
in allen acht CI-Plattformkombinationen zum Commit `430df15`.

Neun weitere Diagnosegruppen und acht Laufzeitprogramme prüfen Generics,
Funktions-/Methodenüberladungen, Strukturdefaults und eigene Initialisierer direkt.
Importierte Fehlerfälle erhalten getrennte Quelldateien; die erwarteten Diagnosen
verweisen weiterhin auf den richtigen Import und dessen Zeile. Ein weiterer Ablauf
baut generische Experiment-/Analysemodule und prüft ihre separaten Runner und
Ausgabedateien. Damit umfasst der direkte Katalog 152 Tests, davon 142 ohne SDL.
Alle 108 Sprachtests bestehen lokal mit MSVC Debug und Clang Release; alle 100
betroffenen CTest-Prüfungen bestehen ebenfalls. Der Läufertest prüft außerdem,
dass Emissions-, Build-, Prozessfehler und fehlende Dateien Folgeschritte verhindern.
Dieser Stand besteht inzwischen im direkten CI-Schritt aller acht
Plattformkombinationen zum Commit `0a937f8`.

Weitere 16 Gruppen übernehmen Kontrollfluss, optionale Werte, Enums, verzögerte
Initialisierung, Unicode, Zahlkonvertierung und Ganzzahlfehler in den direkten Läufer.
32 zusätzliche Programme prüfen auch Closures, Bereichszuweisungen, importierte
Enums und die Erholung des `attempt`-Experimentmoduls nach einem Fehler. Erwartete
Laufzeitfehler müssen Exitcode 70 und ihre genaue Quelldiagnose liefern. Die
Byteprüfung mehrzeiliger Strings vergleicht weiterhin das erste erzeugte Literal;
ein späteres passendes Literal kann ein falsches erstes Ergebnis nicht verdecken.
Der direkte Katalog umfasst jetzt 200 Tests, davon 190 ohne SDL. Alle 156 Sprachtests
bestehen lokal mit MSVC Debug und Clang Release, ebenso die 147 zugehörigen
CTest-Prüfungen. Der direkte CI-Schritt zum Commit `d7fb0b0` besteht inzwischen
in allen acht Plattformkombinationen.

Physikalische Einheiten, Materialien, Medien und Kugelauftrieb werden jetzt
ebenfalls direkt geprüft, einschließlich statischer Dimensionsfehler und
Quelldiagnosen bei ungültigen Laufzeitwerten. Drei weitere Analyseabläufe prüfen
Reihenwerte, Quantile und CSV-Spalten. Exportprüfungen vergleichen den vollständigen
CSV-Inhalt und verlangen, dass abgelehnte Exporte keine CSV-Datei anlegen.
Der direkte Katalog enthält jetzt 222 Tests, davon 212 ohne SDL. Alle 178 Sprachtests
bestehen lokal mit MSVC Debug und Clang Release; die 163 zugehörigen CTest-Prüfungen
bestehen ebenfalls. Der Läufertest erkennt falsche Inhalte, ungültiges UTF-8,
fehlende Dateien und unerwünschte Ausgaben. Der direkte CI-Schritt zum Commit
`0923b6e` besteht in allen acht CI-Plattformkombinationen.

Compiler-CLI und Modulimporte laufen nun ebenfalls direkt. 52 Compileraufrufe
prüfen Host-Beschränkungen, Unicodepfade, Diagnosepositionen, Größenbegrenzung,
Abhängigkeiten, Importzyklen und die Suchreihenfolge. Die lokale Modulpriorität
wird zusätzlich anhand der tatsächlich aufgelisteten Datei geprüft. Drei weitere
Programme prüfen Importe, zusätzliche Suchpfade und einen Fehler aus einer
importierten Funktion. Der direkte Katalog umfasst 228 Tests, davon 218 ohne SDL.
Alle 184 Sprachtests bestehen lokal mit MSVC Debug und Clang Release; die 169
zugehörigen CTest-Prüfungen bestehen ebenfalls mit den gemeinsamen Erwartungen.
Der direkte Build- und Testschritt dieser Erweiterung besteht zum Commit
`8516187` in allen acht CI-Plattformkombinationen.

Der direkte Läufer übernimmt jetzt auch alle 203 Programme des bisherigen nativen
Sprachtests. Der gemeinsame Katalog erhält 29 erfolgreiche Läufe und 174 erwartete
Laufzeitfehler; 56 Programme prüfen zusätzlich die Speicherfreigabe nach normalem
Ende und Fehlerabbruch. Vier C-Proben prüfen den Speicherzusatz selbst, einschließlich
absichtlich gesetzter Restbytes. Testheader werden in der Buildabhängigkeit erfasst.
Zwei weitere Fälle prüfen die Physim-Zeile in C-Compilerdiagnosen und den Erhalt
des letzten vollständigen C-Codes und Programms nach einer abgelehnten Quelle.
Damit enthält der direkte Katalog 437 Tests beziehungsweise 427 ohne SDL.
Alle 209 neuen Fälle bestehen mit MSVC Debug und Clang Release; der bisherige
CTest-Sprachtest besteht mit denselben Quellen und Erwartungen. Die Tests des
Läufers weisen falsche Diagnosezeilen, geänderte Ausgaben, temporäre Restdateien
und unpassende Compilerfehler ab. Zum Commit `2b3b6e7` besteht der direkte Build-
und Testschritt in allen acht CI-Plattformkombinationen.

Weitere 26 Integrationsprüfungen laufen nun ohne CMake. Der gemeinsame Katalog
erfasst 97 Programme und Module und 48 Ausführungsschritte. Bestehende C-Prüfer
vergleichen C- und Physim-Modelle einschließlich Integratoren, Kollisionen,
Auftrieb, Sensoren, Kontakten, Gelenken, Körpergruppen und Sweeps. Weitere Fälle
prüfen Analyseberichte, Einheiten, exportierte Daten, Parameterbeschreibungen
und parallele Parameterreihen. Ungültige Parameter dürfen keine Ergebnisdateien
oder Ordner erzeugen. Quellen, Modulvarianten und Argumentreihenfolgen bleiben
erhalten; die bisherigen Parameter-CMake-Skripte sind durch den gemeinsamen
Katalog ersetzt. Arbeitsordner mit Leerzeichen und Umlauten isolieren die Läufe.
Der direkte Katalog umfasst jetzt 463 Tests beziehungsweise 453 ohne SDL.
Alle 419 direkten Sprachtests bestehen lokal mit MSVC Debug und Clang Release;
die 26 neuen CTest-Einstiegspunkte bestehen ebenfalls. Der Testläufer prüft außerdem,
dass Emissions-/C-Buildfehler, fehlende Module und fehlende Ausgaben abhängige
Schritte stoppen. Zum Commit `b95e119` besteht der direkte CI-Schritt in allen
acht Kombinationen aus Betriebssystem, Compiler und Konfiguration.

Weitere 21 Abläufe übernehmen Runner-Isolation, parallele Läufe, physikalische
Referenzen und ausführbare Dokumentationsbeispiele. Die bestehenden C-Prüfer
kontrollieren weiterhin Abstürze, Endlosschleifen, ABI-Ablehnung, Prozessende,
Wiederherstellung von Laufdaten und das Beenden paralleler Kindprozesse.
Pendel- und Box-Prüfungen erzeugen ihre Messdaten jetzt im eigenen Arbeitsordner,
führen bei Bedarf die Analyse aus und prüfen anschließend die Ergebnisse.
Die gemeinsame CTest-Vorbereitung und globale Messdatei-Abhängigkeiten entfallen.
Der direkte Katalog umfasst damit 484 Tests, davon 474 ohne SDL. Alle 21 neuen
Abläufe bestehen mit MSVC Debug und Clang Release; die 47 gemeinsamen
CTest-Integrationsprüfungen bestehen ebenfalls mit aktuell gebauten Runnern.
Mehrfaches `--test-filter` kombiniert Testgruppen ohne doppelte Ausführung;
der Läufertest prüft außerdem den ungefilterten Aufruf und Quellenpfade für
archivierte Experimentquellen. Zum Commit `7e38cf1` bestehen alle 484 Tests in
allen acht CI-Kombinationen. Der anschließende Build ohne Tests scheiterte jedoch
an einer fehlerhaften Prüfung des nun optionalen Testfilters. Diese Prüfung ist
korrigiert; Regressionen erfassen normale Builds, SDK-Installation, ungefilterte
Tests und wiederholte Filter. Normale Builds bestehen lokal mit MSVC und Clang.

Weitere sieben Prüfungen übernehmen Tutorialquellen, API-Referenz, PNG-Dekodierung
und den direkten Projektbuild. Vier alte CMake-Skripte entfallen; beide Testwege
lesen denselben Katalog mit elf Tutorialquellen. Der PNG-Test erzeugt seine
Eingaben selbst. Der Projektbuild-Test prüft auch Compiler-/Linkerfehler,
unveränderte Builds, Headeränderungen, gesperrte und verschobene Projektordner.
Alle sieben bestehen mit MSVC Debug, Clang Release und über CTest. Der direkte
Katalog umfasst nun 491 Tests beziehungsweise 480 ohne SDL. Der Test des Läufers
prüft zusätzlich Zeilenenden und falsche Tutorialblöcke sowie Unicode-Pfade für
Python-Aufrufe. Zum Commit `a051ea0` bestehen diese Erweiterung und die CLI-Korrektur
im direkten Build- und Testschritt aller acht CI-Kombinationen.

Die 34 bisherigen Fenster- und Grafikabläufe laufen nun ebenfalls ohne CMake
über `tools/build.py --test-display`. 13 CMake-Hilfsskripte sind durch einen
gemeinsamen Python-Prüfer ersetzt. Die 491 Prüfungen ohne Fenster bleiben unter
`--test` verfügbar; beide Gruppen lassen sich mit wiederholten Filtern eingrenzen.
Die Grafikgruppe umfasst Menüs in zwei Fenstergrößen, Dokumente, Autosave und
Wiederherstellung, Workspace-/Projekteinstellungen, Diagramme, Stapelläufe,
Sprachvorschauen und 13 vollständige Sprachprojekte. Beide Diagrammabläufe prüfen
die dekodierten PNG-Pixel zusätzlich mit dem unabhängigen Python-Prüfer.
Quellen, Backups, beschädigte Sicherungen, Projektprofile und unbekannte
Projektfelder werden nach den App-Aufrufen mit ihren erwarteten Inhalten verglichen.
Alle 34 Abläufe sind lokal mit MSVC Debug erfolgreich geprüft. Acht ausgewählte
Abläufe bestehen außerdem mit der Clang-Release-App; deren neue Projekte werden
ebenfalls mit Clang gebaut. Der Prüfer protokolliert einzelne Aufrufe samt Ausgaben
auch bei Zeitüberschreitung. Der Testläufer prüft zusätzlich die Auswahl der
Fenstergruppe, widersprüchliche CLI-Optionen, Unicode-Ausgaben, fehlende
Erfolgsmarkierungen und veränderte Dateien. Drei CTest-Vergleiche für Menübedienung,
Diagramme und ein gemischtes C-/Physim-Projekt bestehen ebenfalls.
Linux und beide macOS-Architekturen
erhalten eigene direkte Grafikschritte in der CI; deren Nachweis steht noch aus.
Die drei Benchmarks waren zu diesem Stand noch an CMake gebunden.

`benchmark_smoke`, `benchmark_driver` und `ui_rendering` sind ebenfalls auf den
direkten Katalog übertragen. `--benchmarks` baut die optionalen Messprogramme;
die Tests bauen sie bei Bedarf automatisch. Compilerkennung und Profil stammen
aus dem C-Programm. Messmetadaten erfassen den direkten Builder und benötigen
keine CMake-Konfiguration. Der Regressionstest prüft dies in einem eigenen
Quellordner ohne CMake-Datei und kontrolliert weiterhin ungültige Eingaben,
veränderte Baselines, Regressionen und erhaltene Fehlerausgaben.
Alle drei Prüfungen bestehen mit MSVC Debug und Clang Release sowie über CTest.
Die beiden Prüfungen ohne Fenster bestehen zusätzlich im SDL-freien MSVC-Release-Build.
Die drei UI-Referenzbilder stimmen zwischen MSVC und Clang bytegenau überein;
nach dem Aufwärmen entstehen keine neuen Zeichenpuffer-Allokationen.
Der Katalog umfasst nun 493 Prüfungen ohne Fenster (482 ohne SDL) und 35 Grafikabläufe.
Der direkte Buildtest prüft außerdem den Neubau nach Änderungen privater Toolheader.
Die vollständige Ablösung der CMake-CI einschließlich Sanitizer- und SDK-Abnahmen
bleibt offen.

Die Grafik-CI zum Stand `a621b5b` besteht auf macOS Apple Silicon. Linux GCC und
Clang bestehen jeweils 33 von 34 Abläufen; nur der zuerst gestartete Auftriebstest
endet nach 150 Sekunden ohne App-Ausgabe im Timeout. Die Ursache ist noch nicht
belegt. Der Linux-Aufruf wartet jetzt ausdrücklich auf einen lebenden Fenstermanager;
zusätzliche App-Testprotokolle zeigen Initialisierung und Phasenwechsel und bleiben
auch bei Timeout erhalten. Die Bereitschaftsprüfung wird gegen fehlende, ungültige
und veraltete Fenstermanager-Einträge geprüft. Die erneute Linux-CI steht aus.

| Bereich | Implementiert | Noch offen |
| --- | --- | --- |
| Eigene Sprache | verbindliches Ziel als vollständige C-Alternative, C17-Lexer/Parser mit `:`-/Einrückungsblöcken, skalare und nominale Struktur-/Enumtypen mit typisierten und besitzenden Nutzdaten sowie optionale Werte mit Wertsemantik und struktureller Gleichheit, normale/mutierende/statische Strukturmethoden, eigene Struct-Initialisierer mit Überladung nach Parameterform und Parametertyp auch bei generischen Typen, `physimc --check`/`--emit-c`/`--emit-experiment`/`--emit-analysis`, C17-Backend und direkter Builder für Programme und Experiment-/Analysemodule, Vec2/Vec3/Vec4, Quaternionen und Mat3/Mat4, Einheiten/Kanäle, explizite PCG32-Wertströme, starre Körper mit Impulsen/Quaternionrotation, Kontaktpaare mit Reibung/Rückprall, Distanzgelenke mit lokalen Ankern, gemeinsamer Körpergruppen-Solver mit besitzenden Ergebniswerten, lineare Kugel-Sweeps und Hüllquader-Kandidatenpaare, gemeinsamer Integrator, Messdaten/Szene, Dataset-/Series-/Plot-/Table-Handles einschließlich erzeugter Datenreihen, gemeinsame Messstatusauswahl, Statistik/Diagramme/Tabellen/Exporte, abgefangene Laufzeitfehler mit Quelldiagnosen, erste App-Vorlagen mit Editor/Build und Quellsnapshots | vollständiger semantischer Sprachvertrag, weitere Werttypen und Fallmuster, Überladungsauflösung für weitere Ausdrücke, erweiterte Module, vollständige Experiment- und Analysebindungen, vollständige Integration beider Editoren, vollständiger Sprachausbau und zwei getestete Dokumentationsteile (LANG-001 bis LANG-007) |
| Foundation | C17, direkter Build ohne eigene CMake-Dateien, MIT, Windows-/POSIX-/Darwin-Schicht, erfolgreiche Windows-CI und Linux-CI mit GCC und Clang, macOS-CI und geprüfte App-Pakete für Apple Silicon und Intel, Release-Paket auf frischem Debian/Ubuntu geprüft | weitere macOS-Versionen und reale Mac-Grafikhardware, öffentliche Mac-Signierung/Notarisierung, Wayland, Installation auf frischen Windows-/Mac-Systemen |
| Mathematik | Vec2/3/4 mit skalierter Normalisierung, Mat3/4 mit Inversion, Quaternion-Verknüpfung und Rotationsinterpolation, affine/projektive Punkttransformation sowie Richtungs-/Normalentransformation, absolute/relative Vergleiche, Euler/RK4, symplektischer Euler, Verlet, RK45, linearer Solver, Bisektion, Minimierung, kubische räumliche Bézierkurven mit Tangente und Unterteilung | weitere Kurven-/Interpolationsverfahren, Events/dichte Ausgabe, steife Verfahren |
| Basis | Fehlercodes und besitzende strukturierte Diagnosen an Modulgrenzen, expliziter Logger mit synchronem Sink, expliziter RNG, explizite Allocatoren mit Fehlerprüfungen, feste Arenen, eigene Speicherdomänen für Berichte/Analysekontexte, Test-Allocator mit Fehler-Injektion und Bytebudget, dynamische Arrays mit Größenlimit und Selbstkopien, begrenzte String-Views ohne Kopie, Hashmap mit eigenen Schlüsseln und Größenlimits | Allocator-Anbindung weiterer Subsysteme |
| Einheiten | SI-Dimensionen, Konvertierung, Einheitenalgebra, Quantity-Rechnung und Dimensionsprüfung von Datenreihen, deklarierte Anzeigeeinheiten für Experimentparameter samt Formulare und Studienberichte, persönliche lineare Anzeigeeinheiten für Messkanäle mit Live-Werten, Kurven und Gesamtstatistik | — |
| Runner | versionierte Modul-ABI, Handshake, Pause/Step/Run/Stop, Heartbeat, Crash-/Hang-Isolation, feste/adaptive Modellschritte, Echtzeittaktung von 0,1× bis 16× und Offline, eigener begrenzter Logkanal mit JSONL-Speicherung | weitergehende Ressourcenbegrenzungen, echtes OS-Sandboxing |
| Daten | CRC-Chunks, Streaming, Recovery, CSV, Seed-/Modellmetadaten, optionale versionierte Szenenblöcke und rekonstruierbare Abschlussindizes mit begrenzten gezielten C-Abfragen | mehr Datentypen, komprimierte Blöcke, Schemaerweiterung |
| App | leerer Workspace-Einstieg mit gespeicherter Ordnerauswahl und bewusster Wiederöffnung, kompakte Menüleiste, Projektmanager, aufklappbarer Dateibaum und bis zu 16 editierbare Textdokumente mit separaten Autosaves und gespeicherten Editoransichten, drei Arbeitsbereiche, integrierte Offline-Dokumentation, Systemtypografie, Einstellungen mit Code-Schriftgröße und Autosave-Intervall, dunkle, helle und kontrastreiche Darstellung, gespeicherte Fenstergröße/Maximierung, verschiebbare Seitenleiste, Arbeitsbereich, Protokoll und eigenständiger Inspektor mit Teilungen, Tabgruppen, gespeicherten frei platzierten Panels und bis zu acht benannten Panelanordnungen im Hauptfenster und acht benannten Workspaces mit Ordnern und Editoransichten, Reset zum pausierten Anfangszustand mit erhaltenen Alt-Läufen, Vorlagen, Editor, direkter Projektbuild ohne CMake mit Ausgaben unter `build/`, Diagramme | portable Workspaces, separate Panelfenster, weitere Panelzustände, UI-weite Schriftvergrößerung, vollständige Barrierefreiheit |
| Editor | C- und Physim-Dateien bearbeiten, sprachspezifische Syntaxfarben, Zeilennummern, Debug/Release, anklickbare Compilerdiagnosen, öffentlicher Header-Browser | Completion |
| Szene | OpenGL 3.3 Core, Tiefenpuffer, MSAA, alle acht Grundprimitive, orientierte Boxen/Ebenen, RGBA-Transparenz mit Dreieckssortierung, UTF-8-Labels, Wurf-Flugbahn, Grid/Achsen, Kamera, Ansichten und Sichtbarkeit und Mausklickauswahl einzelner Szeneneinträge mit optionalen Objekt-IDs, gespeicherte Szenen mit Zeitleiste und Wiedergabe, benannte Gruppen, Elternbeziehungen und explizite hierarchische TRS-Koordinatenrahmen mit aufklappbarem Inspektorbaum, geerbter Sichtbarkeit und konsistenter Weltgeometrie für Darstellung/Picking | artefaktfreie Transparenz bei sich durchdringenden Flächen |
| Analyse | eigener C-Editor/Runner, eigenständige C-/Physim-Analyseprojekte ohne Experiment mit geprüftem Dateiimport, Dataset-/Series-Handles, blockweise Transformationen mit Einheitenprüfung, eigene Ergebnisplots/-tabellen, Linien/Punkte/Histogramme mit Zoom am Mauszeiger, Verschieben, separaten Ausschnitten und Achsenoffsets, PNG-/SVG-Export des sichtbaren Ausschnitts, CSV/SVG und verlustfrei komprimiertes PNG mit vier Größen von 1200 × 850 bis 4800 × 3400 Pixeln, Statistik, Ableitung, Integral, gleitendes Mittel, Periode, Energieabweichung, Auswahl und Vergleich von bis zu acht Läufen, gemeinsame Statusauswahl von Datenreihen, lineares, Nearest-/Previous- und monotones kubisches Resampling (PCHIP) in C/Physim und Differenzkurven, frühere Berichte öffnen | weitere Interpolationsverfahren/Transformationen |
| Mechanik | starre Körper mit Kugel-/Boxträgheit, Quaternionrotation, Drehmomente/Impulse, Kugel–Kugel/Kugel–Ebene/Kugel–Box/Box–Ebene/Box–Box, iterative Paar- und Graph-Solver mit Coulomb-Reibung/Restitution (bis zu 128 Körper und 512 Kontakte), persistente diskrete C-/Physim-Kontaktverwaltung mit stabilen IDs und projizierten Warmimpulsen, Distanzgelenk mit lokalen Ankern und Driftkorrektur, gemeinsamer Geschwindigkeits-Solver für Kontakte und bis zu 256 Gelenke, Feder/Dämpfung, Stokes-/quadratischer Widerstand, Kugelstoß-, Boxstoß- und Bodenkontaktvorlagen mit Debug-Vektoren, Feder–Masse–Dämpfer mit dissipierter Arbeit und Energiebilanz, archimedischer Auftrieb und Kugel-Eintauchvolumen samt Auftriebsmittelpunkt, Auftriebsvorlage mit Kraftanzeige und Energiebilanz | persistente Feature-IDs/Kontaktinseln, Gelenk-Warmstart, gemeinsame nichtlineare Positionsprojektion, weitere Gelenke, Box-CCD, erweiterte Stoffmodelle |
| Unsicherheit | PCG32, geprüfte konstante/uniforme/normale Verteilungen, öffentliche Sensor-API mit Einheiten, Zeitraster, Auflösung, Offset, Drift, Rauschen, Ausfällen und Standardunsicherheit, getrennte Modell-/Soll-/Messwerte, gültigkeitsbewusste Vorschau/Statistik/CSV, Batchcontroller in App/CLI mit bis zu acht Runnern, expliziten Seeds, eigenen Arbeitsordnern, Abbruch und fester Auswertungsreihenfolge, Endwert-Histogramm/Typ-7-Quantile, Normalnäherung des Mittelwert-KI ab 200 gültigen Endwerten und lineare Parameterstudien in CLI und App mit Kurvenbericht, geprüfte Wiederaufnahme journalisierter Läufe aus archivierten Konfigurationen in neue Serienordner, gültigkeitsbewusste Endwertaggregation mit Messstatus-CSV und Messabdeckung einschließlich vollständig fehlender Messungen, explizite Masken in transformierten C-/Physim-Datenreihen und Berichten mit erhaltenen Segmentgrenzen | korrelierte Sensor-/Unsicherheitsmodelle, weitere Konfidenzverfahren und Verteilungsdiagnostik |
| Produktreife | Quellen-Snapshots, versionierte Dateien, Backup beim Speichern, Autosave beider C-Editoren mit wählbarem Intervall (Standard 30 Sekunden), Wiederherstellung mit Erkennung extern geänderter Quellen | gleichzeitige Autoren/Dateizusammenführung, Projektmigration, Installer, Leistungsbudget, abdeckungsgeführte Langzeit-Fuzzing-Kampagne |

Keine Kennzeichnung als stabile 1.0 und keine Behauptung, dass alle 25 Starttickets
oder die Phasen 0–10 bereits abgenommen sind. Die offene Arbeit bleibt nachvollziehbar
an den Kriterien des ursprünglichen Projektplans orientiert.

Die globale Leiste verwendet jetzt kompakte Menüs für **Datei** und **Ansicht**
sowie direkte Zugänge zu Hilfe und Einstellungen. Die Arbeitsbereiche liegen
als 30 Pixel hohe, gleich breite Tabs über die gesamte Fensterbreite direkt darunter.
Die Tab-Auswahl hat eine abgerundete, leicht eingerückte, neutral graue Fläche.
Die Menüzeile hat einen einheitlichen Hintergrund ohne einzelne Buttonflächen
im Ruhezustand. Nur Hover und das offene Menü werden hervorgehoben.
Ein gemeinsames Popup ermöglicht den direkten
Wechsel in beide Richtungen sowie zu Hilfe und Einstellungen mit einem Klick.
Bei offenem Menü wechselt auch das Bewegen des Mauszeigers zwischen Datei und
Ansicht das Popup. Menüeinträge sind bündig ausgerichtet, Tastenkürzel stehen
rechts; die Einträge erhalten nur beim Überfahren eine eigene Farbfläche.
Windows 11 zeichnet abgerundete äußere Ecken im normalen Fenstermodus.
Menüwechsel und Darstellung wurden per SDL-Test und echter Windows-Eingabe geprüft.
Menüs und Fensterknöpfe teilen sich den 32 Pixel hohen Fensterkopf; die zusätzliche
native Titelzeile entfällt. SDL-Hit-Testing ermöglicht Verschieben und Größenänderung.
Die großen Seitenleisten-Schaltflächen entfallen; der gemeinsame Kopfbereich ist
64 Pixel hoch. UI-Tests prüfen
Menüaktionen, deaktivierte Einträge, Tab-Wechsel und Tastaturbedienung bei
1080 × 740 und 1440 × 940. Einstellungen, Dokumentationsfenster und die
Workspace-Abläufe bestehen weiterhin. Maximieren, Wiederherstellen, Minimieren und
Schließen über die integrierten Fensterknöpfe sowie die Größenänderungs- und
Verschieberegionen sind ebenfalls geprüft. Zusätzlich wurden Verschieben,
Größenänderung, Darstellung und Schließen mit echten Windows-Mauseingaben geprüft.
Der Wiederherstellungsdialog behält
seine eigenen Fensterknöpfe und verwendet denselben sicheren Schließpfad.

Der Workspace besitzt einen aufklappbaren Dateibaum. Verzeichnisse werden beim
Öffnen gelesen und vor Dateien sortiert; Aktualisieren erhält aufgeklappte Pfade
je Wurzeleintrag. Die frühere Grenze von 128 flachen Einträgen wurde durch eine
sichtbar begrenzte Baumansicht mit bis zu 8192 Einträgen und 64 Ebenen ersetzt.
Zusätzliche Wurzeleinträge behalten ihre Plätze, fehlende Pfade und Lesefehler
werden angezeigt. Bis zu 16 UTF-8-Textdateien mit jeweils höchstens 256 KiB
öffnen sich als unabhängige editierbare Dokumente mit Suche und Ersetzen.
Binärdateien, ungültiges UTF-8 und größere Dateien werden abgewiesen.
Speichern prüft externe Änderungen und erhält die vorherige Datei als Backup.
Schließen und Neuladen bieten bei Änderungen Speichern, Verwerfen und Abbrechen.
Workspace-Wechsel, Build und normales Beenden speichern offene Dokumente;
ein Konflikt blockiert den Übergang und zeigt das betroffene Dokument.
Projektquellen verwenden weiterhin ihre bestehenden Editoren.
Modell- und App-Tests für Dokumente bestehen unter MSVC Debug bei beiden
Fenstergrößen. Allgemeine Dokumente haben jetzt eigene Autosaves außerhalb des
Workspace im persönlichen App-Datenverzeichnis. Beim erneuten Öffnen derselben
Datei werden Wiederherstellen, Verwerfen und Schließen ohne Auswahl unterstützt.
Externe Änderungen werden vor dem Wiederherstellen angezeigt; beschädigte
Sicherungen bleiben erhalten und pausieren nur Autosave der betroffenen Datei.
Explizites Verwerfen aktiviert Autosave wieder. Das Intervall folgt den Einstellungen.
Der Prozess-Test beendet den Schreiber ohne normalen Speicher-/Aufräumpfad und
prüft zwei unabhängige Dokumente, zeitgesteuerte Sicherung, Änderungen gleicher
Länge, alle Wiederherstellungsentscheidungen und unveränderte Quelldateien.
Die Modelltests prüfen zusätzlich fremde Pfade im Cache, beschädigte Daten,
leere Entwürfe und Erhalt der vorherigen Sicherung bei fehlgeschlagenem Schreiben.
Eine nach dem Öffnen beschädigte Sicherung bleibt auch beim normalen Speichern
erhalten. Explizites Zurücksetzen und das vollständige Rückgängigmachen eines
wiederhergestellten Entwurfs werden ebenfalls geprüft.
Pfadänderungen/gelöschte Quellen und automatisches Öffnen der Dokumente nach
Neustart bleiben offen.

Die Dokument-Wiederherstellung besteht in separaten Prozessen mit MSVC Debug,
Clang Debug und MSVC AddressSanitizer. Letzterer prüft zusätzlich den gesamten
Dokumentablauf einschließlich Entfernen und Verschieben offener Editorzustände.
Dabei wurde ein Heap-Lesezugriff außerhalb der Grenzen im eingebundenen
TrueType-Parser beim Laden der Systemschrift gefunden und behoben: Konturen
mit einem einzelnen Kurvenkontrollpunkt bleiben jetzt innerhalb ihrer Grenzen.
Der synthetische `font_shape`-Test besteht unter MSVC Debug und AddressSanitizer.
Zusätzliche Dokumentänderungen und das Neuladen extern geänderter Dateien machen
den Build ungültig. Eine Änderungsnummer bindet den erfolgreichen Build samt
Parameterabfrage an den ursprünglichen Bearbeitungsstand und das Buildprofil.
Zwischenzeitliches Speichern kann einen veralteten Build nicht wieder freigeben.
Der neue App-Test `documents_build` baut ein Projekt mit zusätzlichem Header und
prüft Änderungen während Konfiguration und Parameterabfrage, erfolgreiche
Neubauten sowie die Startsperre nach Bearbeiten und externem Neuladen.
Beim fehlgeschlagenen Speichern aus dem Schließen-Dialog bleibt die Auswahl
zwischen Speichern, Verwerfen und Abbrechen erhalten. Tab-Einrückung mit vier
Leerzeichen ist jetzt in allen Editoren ein einzelner Undo-/Redo-Schritt.
Nachweis unter Windows: Dokumentabläufe und der echte Buildtest bestehen mit
MSVC Debug und Clang Debug; Clang prüft zusätzlich beide Menügrößen und die
Dokument-/Clipboard-Modelle (7/7). Beide Modelltests bestehen auch mit MSVC
AddressSanitizer. Die bestehenden Unicode- und Mehrzeilen-Sprachvorschauen
bestehen unter MSVC. Linux bleibt offen.

Zusätzliches verbindliches Ziel, konkretisiert: Alle Projektdateien bleiben im
Projektordner; Buildprodukte liegen gesammelt unter `build/`. Die App erzeugt
`physim.project` und keine `CMakeLists.txt`. `physim-build` übernimmt direkte
Compileraufrufe, Headerabhängigkeiten über vorverarbeitete Quellen, getrennte
Debug-/Release-Ausgaben und den Schutz vor gleichzeitigen Builds. Der neue Weg
verwendet keine CMake-Installation für Nutzerprojekte. Vorhandene CMake-Dateien
bleiben unberührt. Projekt-Autosaves, Backups und Laufdaten behalten ihre bisherigen
Pfade im Projekt. Allgemeine Dokument-Sicherungen liegen weiterhin im App-Datenbereich.
Langfristig soll auch Physim selbst ohne CMake gebaut werden. Das bleibt offen,
ebenso wie der Nachweis des neuen Projektbuilders auf Linux und auf einem frischen Windows-System.
Nachweis unter Windows: Der native Builder und echte Runner bestehen mit MSVC
und Clang für C- und Physim-Projekte in Debug/Release. Geprüft sind unveränderte
Builds, transitive Headeränderungen, Compiler-/Linkerfehler mit erhaltenen Modulen,
gesperrte Buildordner, ungültige Projektdateien, Unicodepfade, Projektverschiebung
und ignorierte alte CMake-Konfigurationen. Die App-Abläufe für reine Physim- und
gemischte Projekte, Änderungen während des Builds, Laufserien, Autosaves sowie
beide Menügrößen bestehen ebenfalls. Neue Projektbuilds schreiben ausschließlich
in den jeweiligen `build/`-Unterordner; saubere Quellen werden beim Bauen nicht
erneut gespeichert und erzeugen damit keine unnötigen Sicherungskopien.

Die Projektdatei wird von App und Builder über einen gemeinsamen Leser geprüft.
Das Buildprofil wird mit den Experimentparametern gespeichert und beim erneuten
Öffnen wiederhergestellt; Projekte ohne Profileintrag verwenden Debug. Der
Builder verwendet das gespeicherte Profil, wenn kein `--profile` angegeben ist.
Der Schreiber erhält Kommentare, zusätzliche Einträge und die vorhandenen
Zeilenenden, prüft die Daten vor dem Ersetzen und legt bei Änderungen eine
Sicherung der Projektdatei an. Unveränderte gespeicherte Einstellungen werden
nicht erneut geschrieben. Der Modelltest besteht mit MSVC, Clang und MSVC
AddressSanitizer; der App-Test prüft Neustart, Release-Build, Wechsel zu Debug,
Parameterauswahl sowie erhaltene Erweiterungseinträge und unveränderte Quellen.
Der Modelltest deckt 200 Dateien, verschachtelte und doppelt eingebundene Ordner,
Sortierung, Aktualisieren, fehlende Pfade, ungültige Eingaben sowie Eintrags- und
Tiefengrenzen ab. Er besteht unter MSVC Debug, Clang Debug und MSVC mit
AddressSanitizer (RelWithDebInfo). Der App-Test öffnet und schließt Ordner per
SDL-Mausereignissen, liest eine verschachtelte Datei und prüft zusätzliche Wurzeln.
Die Baumansicht samt griechischen Zeichen in der Vorschau wurde bei 1080 × 740
visuell geprüft. Der Linux-Lauf bleibt ausstehend.

Die App speichert jetzt beim normalen Beenden den Hauptordner und bis zu 32
zusätzliche Pfade. **Letzten Workspace öffnen** stellt sie nach einem leeren
Start ausdrücklich wieder her; **Eintrag vergessen** entfernt die Auswahl.
Die separate versionierte Datei prüft Größenlimits, absolute UTF-8-Pfade und CRC.
Fehlende Pfade bleiben erhalten, beschädigte Dateien werden bis zum ausdrücklichen
Zurücksetzen nicht überschrieben. Mehrere benannte Workspaces und die
Wiederherstellung einzelner Dokumentansichten waren zu diesem Stand weiterhin offen.
Die gezielten Tests bestehen unter Windows mit MSVC Debug und Clang Debug.
Sie prüfen Dateigrenzen und beschädigte Inhalte sowie separate App-Prozesse für
Speichern, Öffnen per UI-Klick, fehlende Pfade, Vergessen und leeren Neustart.
Die bisherigen Workspace-, Einstellungs- und Autosave-Abläufe bestehen ebenfalls;
die neue Startansicht wurde bei 1080 × 740 visuell geprüft. Ein Linux-Lauf dieser
Erweiterung steht aus.

Sprachvertrag 0.167.0 ergänzt die Swift-benannten Arraymethoden
`starts(with:)` und `elementsEqual(_:)` für gleich typisierte Arrays mit
`Equatable`-Elementen. Der Compiler prüft Labels, Elementtypen und
Vergleichbarkeit; beide Operationen werten ihre Eingaben einmal aus und
verändern sie nicht. Laufzeit-, Negativ- und Experimenttests decken leere,
verschachtelte, besitzende und eigene Strukturwerte ab.

Sprachvertrag 0.166.0 ergänzt `Array.split` mit den Swift-Labels `separator:`,
`maxSplits:` und `omittingEmptySubsequences:` für `Equatable`-Elemente. Das
Ergebnis ist ein besitzendes `[[T]]` mit unabhängigen Teilarrays. Die
Typprüfung weist unpassende Separatoren und nicht vergleichbare Elemente ab;
Laufzeit- und Speicherfehlertests prüfen Grenzfälle und Rollback. Ein
Experimentmodul verwendet die neue Methode. Veraltete Array-Namen sind auch
aus der Typvorschau entfernt und werden in Negativtests zurückgewiesen. Der
Clang-Gesamtbuild, 199 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Die
gezielten Tests bestehen auch unter MSVC Release und ASan Debug.

Sprachvertrag 0.165.0 erweitert `String.split` um Swifts Labels
`maxSplits:` und `omittingEmptySubsequences:`. Leere Teilstücke werden
standardmäßig ausgelassen; bei `false` bleiben sie erhalten. Ein nichtnegatives
Limit zählt nur Teilstücke vor dem verbleibenden Rest; ausgelassene leere
Teilstücke verbrauchen keinen Split. Der leere Trenner bleibt als
Physim-Erweiterung definiert. Laufzeit-,
Negativ- und Besitztests sowie ein Experimentmodul prüfen die neue Signatur.
Der Clang-Gesamtbuild, 197 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.164.0 ergänzt die Swift-benannten `Int64`-Methoden
`isMultiple(of:)` und `signum()`. Die Vielfachheitsprüfung behandelt den
Divisor null und den kleinsten `Int64`-Wert mit Divisor -1 ohne Fehler oder
Überlauf. Laufzeit- und Negativtests prüfen Grenzwerte, Argumentlabels,
Typfehler und einmalige Auswertung von Empfänger und Argument; ein
Experimentmodul verwendet beide Methoden. Der Clang-Gesamtbuild, 196
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Die
gezielten Tests bestehen auch unter MSVC Release und ASan Debug.

Sprachvertrag 0.163.0 ergänzt `Comparable` als generischen Constraint für
`Int64`, `Float64` und `String` unter Swifts Protokollnamen. `<`, `<=`, `>` und
`>=` ordnen nun auch Strings lexikografisch nach Unicode-Skalaren. Der
Constraint gilt für Funktionen, Methoden, Strukturen und Enums;
Spezialisierungen mit `Bool` und Arrays werden abgewiesen. Tests decken
Unicode-Vergleiche, generische Sortierung, negative Typfälle und ein
Experimentmodul ab. Der Clang-Gesamtbuild, 196 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen. Gezielte Tests bestehen
unter MSVC Release und ASan Debug.

Sprachvertrag 0.162.0 erkennt vollständige Enum-Nutzdatenabdeckung durch
mehrere unbewachte `switch`-Zweige. Der Checker prüft Bool-Kombinationen und
verschachtelte optionale Felder auch über mehrere Nutzdatenfelder hinweg.
Unbegrenzte Skalarfelder benötigen für die vollständige Abdeckung eine
Bindung oder `_`; Guards tragen nicht dazu bei. Laufzeit- und Negativtests
decken fehlende Kombinationen, bewachte Zweige, String-/Bool-Felder und ein
Experimentmodul ab. Der Clang-Gesamtbuild, 194 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen. Gezielte Tests bestehen
unter MSVC Release und ASan Debug.

Sprachvertrag 0.161.0 erkennt die vollständige Abdeckung verschachtelter
Optionalwerte im `switch` ohne unnötiges `default`: Auf jeder Ebene werden
unbewachte `nil`- und `some`-Muster zusammengeführt; ein innerer `Bool` ist
durch `true` und `false` vollständig. Guards zählen nicht zur Abdeckung.
Laufzeit- und Negativtests prüfen verschachtelte Werte einschließlich
`String`-Bereichsmustern, und ein Experimentmodul verwendet den neuen Pfad.
Der Clang-Gesamtbuild, 194 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen;
gezielte Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.160.0 erweitert feste geschlossene und halboffene
`String`-Bereichsmuster auf `Optional.some` und Enum-Nutzdaten, auch in
verschachtelten optionalen Feldern. Die Typprüfung erkennt ungültige Grenzen
und vollständig verdeckte Muster; teilweise Überlappung bleibt in
Fallreihenfolge zulässig. Der C-Emitter vergleicht die Stringgrenzen in allen
diesen Mustern. Laufzeit- und Diagnosetests prüfen Grenzen, Unicode,
Überdeckung, Guards und Experimentmodule. Der Clang-Gesamtbuild,
194 Sprachtests, beide App-Workflows und die Referenzprüfung bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.159.0 ergänzt feste geschlossene und halboffene
`String`-Bereichsmuster im direkten `switch`. Die Typprüfung vergleicht
decodierte UTF-8-Inhalte, weist leere Bereiche und überlappende Fälle ab
und berücksichtigt Guards. Der C-Emitter prüft dieselben Grenzen ohne
temporäre Stringallokation. Tests decken Unicode, Grenzwerte, mehrere
Muster, Guards, Diagnosen und ein Experimentmodul ab. Der
Clang-Gesamtbuild, 192 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release
und ASan Debug.

Sprachvertrag 0.158.0 ergänzt `String.sorted()`/`sorted(by:)` sowie
`String.min()`/`max()` mit und ohne `by:` unter Swifts Methodennamen.
Die Methoden ordnen Unicode-Skalare als eigene Strings; Sortierung ist
stabil, leere Strings liefern `[]` beziehungsweise `nil`. Eine gemeinsame
Umwandlung in ein Array besitzender Skalarwerte nutzt die vorhandenen
Sortier- und Extremfunktionen. Tests prüfen Unicode, Vergleicher,
gebundene und lokale Methoden, Closures, Snapshots, abgefangene Fehler,
Allokationsfehler und ein Experimentmodul. Der Clang-Gesamtbuild, 190
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.157.0 ergänzt `String.first` und `String.last` mit Swifts
Eigenschaftsnamen. Die Werte sind unabhängige `String?` mit je einem
Unicode-Skalar; leere Strings liefern `nil`. Tests prüfen Unicode,
Momentaufnahmen, einmalige Empfängerauswertung, unveränderliche Eigenschaften
und ein Experimentmodul. Eine Überschneidung der internen Bindungskennungen
mit Physikfeldern wurde beim Gesamtbuild entdeckt und beseitigt; eine
Kompilierprüfung schützt den Kennungsbereich. Der Clang-Gesamtbuild, 188
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.156.0 ergänzt `Array.forEach(_:)` und `String.forEach(_:)`
mit Swifts Methodennamen. Beide rufen eine `Void`-Funktion in
Elementreihenfolge auf; Strings liefern Unicode-Skalare als einzelne Strings.
Tests prüfen leere Eingaben, Funktionswerte, Closures, gebundene und lokale
Methoden, Snapshots, Fehlerbereinigung und ein Experimentmodul. Der
Clang-Gesamtbuild, 186 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.155.0 ergänzt `String.reduce(_:_:)` mit Swifts Methodennamen.
Die Methode verarbeitet Unicode-Skalare in Quellreihenfolge und gibt bei
leerem String den Startwert unverändert zurück. Der Akkumulator kann auch
ein besitzender Wert sein. Tests prüfen skalare und besitzende Akkumulatoren,
leere Eingaben, Closures, gebundene und lokale Methoden, Snapshots,
Fehlerbereinigung und ein Experimentmodul. Der Clang-Gesamtbuild, 184
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.154.0 ergänzt `String.compactMap(_:)` und
`String.flatMap(_:)` mit Swifts Methodennamen. Die erste Methode sammelt
vorhandene Ergebnisse von `func(String) -> U?`, die zweite fügt Teilarrays
von `func(String) -> [U]` zusammen. Beide verarbeiten Unicode-Skalare in
Quellreihenfolge und erzeugen unabhängige Arrays mit geometrisch wachsendem
Puffer. Tests prüfen Unicode, leere Eingaben, optionale und verschachtelte
Werte, Structs, Closures, gebundene und lokale Methoden, Snapshots,
Fehlerbereinigung und ein Experimentmodul. Der Clang-Gesamtbuild, 182
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.153.0 ergänzt `String.map(_:)` mit Swifts Methodennamen und
Array-Ergebnis. `func(String) -> U` verarbeitet jeden Unicode-Skalar als
eigenen String. Der Ausgabepuffer wächst geometrisch; besitzende Elemente
werden nach ihren Wertregeln kopiert und bei Fehlern freigegeben. Tests prüfen
Unicode, leere Eingaben, Arrays, Structs, Enums, Funktionswerte, Closures,
gebundene und lokale Methoden, Snapshots, Fehlerbereinigung und ein
Experimentmodul. Der Clang-Gesamtbuild, 178 Sprachtests, beide App-Workflows
und die Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release
und ASan Debug.

Sprachvertrag 0.152.0 ergänzt `String.first(where:)`, `last(where:)`,
`firstIndex(where:)` und `lastIndex(where:)` mit Swifts Methodennamen.
Passende Unicode-Skalare werden als unabhängige `String?` und nullbasierte
Skalarindizes als `Int64?` geliefert; die letzten Treffer werden rückwärts
gesucht. Tests prüfen Unicode, leere Eingaben, Closures, gebundene und lokale
Methoden, Snapshots, Fehlerbereinigung und ein Experimentmodul. Ein
Speichertest prüft die rückwärts laufende Skalariteration auch bei
Allokationsfehlern und ungültigen Cursorpositionen. Der
Clang-Gesamtbuild, 176 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.151.0 ergänzt `String.contains(where:)` und
`String.allSatisfy(_:)` mit Swifts Methodennamen und Kurzschlussregeln.
Das Prädikat `func(String) -> Bool` erhält Unicode-Skalare als einzelne
Strings; leere Eingaben liefern `false` beziehungsweise `true` ohne Aufruf.
Tests prüfen Unicode, Closures, gebundene und lokale Methoden, Snapshots,
Fehlerbereinigung und ein Experimentmodul. Der Clang-Gesamtbuild, 174
Sprachtests, beide App-Workflows und die Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.150.0 ergänzt Swifts Methodennamen `String.filter(_:)`.
Das Prädikat `func(String) -> Bool` erhält jeden Unicode-Skalar als eigenen
String; passende Skalare werden in Quellreihenfolge zu einem unabhängigen
String zusammengesetzt. Der Ausgabepuffer wächst geometrisch und wird bei
einem abgefangenen Fehler vollständig freigegeben. Laufzeit- und Negativtests
prüfen Unicode, leere Werte, Funktionswerte, Closures, gebundene und lokale
Methoden, Snapshots, Fehlerbereinigung und ein Experimentmodul.
Der Clang-Gesamtbuild, 172 Sprachtests, beide App-Workflows und die
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.149.0 ergänzt `String.prefix(while:)` und
`String.drop(while:)` mit Swifts Methodennamen. Das Prädikat hat den Typ
`func(String) -> Bool` und erhält je einen Unicode-Skalar als eigenen String.
Beim ersten `false` endet die Prüfung; die Ergebnisse sind unabhängige Strings
an Skalargrenzen. Laufzeit- und Negativtests prüfen Unicode, leere Werte,
Closures, gebundene und lokale Methoden, Snapshots, Fehlerbereinigung und ein
generiertes Experimentmodul. Der Clang-Gesamtbuild, 170 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen. Gezielte Tests bestehen unter
MSVC Release und ASan Debug.

Sprachvertrag 0.148.0 ergänzt Swifts `Array.prefix(while:)` und
`Array.drop(while:)` für `func(T) -> Bool`. Beide stoppen die Prädikatsprüfung
beim ersten `false`; `prefix` liefert die passende Anfangsfolge und `drop` den
Rest ab diesem Element. Physim liefert eigenständige `[T]`-Werte. Der Parser
akzeptiert dafür das Schlüsselwort `while` als Argumentlabel, ohne die
Zählerform `prefix(count)` zu verändern. Laufzeit- und Negativtests decken
leere und vollständig passende Arrays, besitzende Elemente, Closures,
gebundene und lokale Methoden, Snapshots, Fehler sowie ein generiertes
Experimentmodul ab. Der Clang-Gesamtbuild, 168 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen; gezielte Tests bestehen unter
MSVC Release und ASan Debug.

Der Array-Ergebnisaufbau für `map`, `filter`, `compactMap` und `flatMap` verwendet
im Sprachvertrag 0.147.0 nun einen privaten, geometrisch wachsenden Puffer
statt einer vollständigen Arrayersetzung je Element. Ein Laufzeittest baut 512
Elemente mit acht Blockallokationen auf und prüft Snapshots, Bytebudget sowie
Rollback bei Allokations- und Elementkopierfehlern. Größere String-Ergebnisse
laufen auch im generierten Code. Der Clang-Gesamtbuild, 166 Sprachtests, beide
App-Workflows und die Referenzprüfung bestehen; die fünf gezielten Laufzeittests
bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.147.0 ergänzt Swifts `Array.flatMap(_:)` für Transformationen
`func(T) -> [U]`. Die Ergebnisarrays werden in Eingabereihenfolge zu einem
eigenständigen `[U]` zusammengefügt; leere Teilarrays tragen nichts bei.
Laufzeit- und Negativtests decken freie Funktionen, Closures, gebundene und
lokale Methoden, verschachtelte und besitzende Elemente, Snapshots und Fehler
ab. Ein generiertes Experimentmodul verwendet dieselbe Methode. Der
Clang-Gesamtbuild, 166 Sprachtests, beide App-Workflows und die generierte
Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC Release und
ASan Debug.

Sprachvertrag 0.146.0 ergänzt Swifts `Array.compactMap(_:)`: Eine Funktion
`func(T) -> U?` wird für jedes Eingabeelement einmal aufgerufen, `nil` wird
ausgelassen und vorhandene Werte bilden ein eigenständiges `[U]`. Tests decken
freie Funktionen, Closures, gebundene und lokale Methoden, verschachtelte
Optionals, besitzende Werte, Snapshots, Fehlerbereinigung und ein generiertes
Experimentmodul ab. Der Clang-Gesamtbuild, 164 Sprachtests, beide App-Workflows
und die generierte Referenzprüfung bestehen. Gezielte Tests bestehen unter
MSVC Release und ASan Debug.

Sprachvertrag 0.145.0 ergänzt `String.prefix(_:)`, `suffix(_:)`,
`dropFirst(_:)` und `dropLast(_:)` mit Swifts Methodennamen. Physim schneidet
wie bei seinen Stringindizes an Unicode-Skalargrenzen und liefert einen
unabhängigen `String` statt `Substring`; die Drop-Methoden verwenden ohne
Argument eins. Laufzeit-, Negativ-, Speicher- und Experimentmodultests decken
die API ab. Der Clang-Gesamtbuild, 162 Sprachtests, beide App-Workflows und
die generierte Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC
Release und ASan Debug.

Sprachvertrag 0.144.0 ergänzt Swifts Arraynamen `prefix(_:)`, `suffix(_:)`,
`dropFirst(_:)` und `dropLast(_:)`. Die beiden Drop-Methoden erlauben die
Swift-Vorgabe von einem Element; größere Anzahlen werden begrenzt, negative
Anzahlen abgewiesen. Physim liefert wegen seiner vorhandenen Slice-Wertsemantik
einen unabhängigen `[T]`-Wert statt `ArraySlice`. Der Clang-Gesamtbuild,
160 Sprachtests, beide App-Workflows und die generierte Referenzprüfung
bestehen. Gezielte Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.143.0 ergänzt Swifts `array.removeSubrange(_:)` und
`array.replaceSubrange(_:with:)` für ausdrücklich begrenzte halboffene und
geschlossene Bereiche. Beide Methoden verwenden die vorhandene geprüfte
Bereichs- und Ersetzungslogik; Snapshots, Selbstersetzung und verschachtelte
Arraypfade bleiben gültig. Ungültige Grenzen verhindern die Auswertung des
Ersatzarrays. Der Clang-Gesamtbuild, 158 Sprachtests, beide App-Workflows und
die generierte Referenzprüfung bestehen. Gezielte Tests bestehen unter MSVC
Release und ASan Debug.

Sprachvertrag 0.142.0 ergänzt Swifts `array.swapAt(_:_:)` für veränderliche
Arraypfade. Zwei positionale Indizes müssen vorhandene Elemente bezeichnen;
ein Selbsttausch ist wirkungslos. Die Implementierung kopiert vor dem Tausch,
damit Snapshots und besitzende Elemente gültig bleiben und Kopierfehler keine
teilweise Mutation veröffentlichen. Die gesamte Sprachtestsuite mit 156 Tests,
beide App-Workflows und die generierte Referenzprüfung bestehen. Gezielte
Tests bestehen unter MSVC Release und ASan Debug.

Sprachvertrag 0.141.0 ergänzt Swifts `array.removeFirst()` und
`array.removeLast()` sowie die Varianten mit positionaler Anzahl. Die
Einzelelementformen liefern einen eigenen `T`-Wert, die Anzahlformen `Void`.
Leere Arrays und ungültige Anzahlen melden Laufzeitfehler. Laufzeit-,
Negativ-, Allocator- und Experimentmodultests prüfen besitzende Rückgaben,
Snapshots, verschachtelte Empfänger und Fehler-Rollback.
Der Clang-Gesamtbuild, 154 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.140.0 ergänzt Swifts `array.append(contentsOf:)` und
`array.insert(contentsOf:at:)` für gleich typisierte Arrays. Selbstkopien,
besitzende Elemente und verschachtelte Zugriffspfade behalten Wertsemantik;
fehlgeschlagene Kopien veröffentlichen keinen Teilwert. Laufzeit-, Negativ-,
Allocator- und Experimentmodultests prüfen die neuen Methoden.
Der Clang-Gesamtbuild, 152 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.139.0 ergänzt Swifts `array.removeAll()` und
`array.removeAll(where:)`. Die Prädikatform arbeitet auf einem festen Snapshot,
bewahrt die Reihenfolge der übrigen Elemente und veröffentlicht die Mutation
erst nach erfolgreicher Auswahl und Kopie. Laufzeit-, Negativ-, Allocator- und
Experimentmodultests prüfen besitzende Werte, verschachtelte Empfänger,
Closures, gebundene Methoden und Fehler.
Der Clang-Gesamtbuild, 150 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.138.0 ergänzt Swifts `array.min()` und `array.max()` sowie
`min(by:)` und `max(by:)`. Leere Arrays liefern `nil`; für eigene Elementtypen
entscheidet die Vergleichsfunktion. Bei Gleichständen bleibt das erste
Element ausgewählt. Laufzeit- und Negativtests prüfen primitive und besitzende
Typen, Closures, gebundene Methoden und fehlerhafte Vergleichsfunktionen.
Der Clang-Gesamtbuild, 148 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.137.0 ergänzt Swifts `array.sorted(by:)` und `array.sort(by:)`
für Vergleichsfunktionen beliebiger Array-Elementtypen. Die Reihenfolge ist
stabil; eine fehlgeschlagene mutierende Sortierung lässt den Empfänger
unverändert. Laufzeit-, Negativ- und Allocator-Tests decken besitzende Werte,
Closures, gebundene Methoden, verschachtelte Empfänger und Fehler ab.
Der Clang-Gesamtbuild, 146 Sprachtests und beide App-Workflows bestehen.
Gezielte Tests bestehen unter MSVC Release und ASan Debug; die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.136.0 ergänzt `array.last(where:)`,
`array.firstIndex(where:)` und `array.lastIndex(where:)`. Die Suchrichtung
bestimmt den ersten geprüften Wert; alle drei Ergebnisse sind optional.
Gezielte Laufzeit- und Negativtests prüfen frühes Beenden, leere Arrays,
fehlende Treffer und besitzende Elemente. Der Clang-Gesamtbuild, 145
Sprachtests und beide App-Workflows bestehen. Ein Experimentmodultest prüft
alle vier Prädikatsuchen unter Clang und MSVC Release; der Programmlauf besteht
zusätzlich unter MSVC Release und ASan Debug. Die generierte Sprachreferenz
ist geprüft.

Sprachvertrag 0.135.0 ergänzt die Swift-Eigenschaften `array.isEmpty`,
`array.first` und `array.last` sowie `array.first(where:)`. Die Endpunkte
liefern `T?`, bei leerem Array `nil`, und kopieren auch besitzende Elemente
unabhängig; die Prädikatsuche endet beim ersten Treffer. Laufzeit- und
Negativtests prüfen leere Arrays, verschachtelte und optionale Elemente,
Snapshots sowie unveränderliche Eigenschaften. Der vollständige Clang-Build,
145 Sprachtests und zwei App-Workflows bestehen; der gezielte Laufzeittest
besteht zusätzlich unter MSVC Release und ASan Debug. Die generierte
Sprachreferenz ist geprüft. Der Checker weist Zuweisungen an eingebaute
schreibgeschützte Eigenschaften nun ohne Absturz zurück.

Sprachvertrag 0.134.0 gleicht sichere Arrayoperationen an Swift an:
`array.popLast()` entfernt das letzte Element und liefert `T?`, bei einem
leeren Array `nil`. Für sichere Indexzugriffe dienen nun
`attempt(array[index])` und `attempt(text[index])`; die nicht zu Swift
passenden `get(at:)`-Methoden sind entfernt. Entsprechend ersetzt
`attempt(array.remove(at: index))` den kurzzeitig eingeführten Aufruf
`removeIfPresent(at:)`. Die Prädikatmethoden heißen jetzt
`contains(where:)` und `allSatisfy(_)` statt `any` und `all`.
Die String-Wiederholung nutzt nun Swifts Konstruktorform
`String(repeating: text, count: n)` statt `text.repeated(count: n)`.
`Array(repeating: value, count: n)` ersetzt die bisherige Arraymethode
`repeated(count:)` mit Swift-Semantik für ein einzelnes Element. String-Trimmen
verwendet `trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)`;
UTF-8-Bytes werden über `text.utf8.count` gezählt.
Die nicht mutierenden Arraynamen `appending`, `inserting` und `removing` sind
entfernt; Kopien verwenden `+` oder `var` mit den Swift-Namen `append`,
`insert` und `remove`. Die entfernten abweichenden Namen sind in negativen
Sprachtests abgewiesen. Der volle
Clang-Build, 143 Sprachtests und beide App-Workflows bestehen; MSVC Release
und ASan Debug bestehen die gezielten Laufzeittests. Die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.133.0 ergänzt `String.get(at:)` als sicheren Zugriff auf einen
Unicode-Skalar. Gültige Indizes liefern einen besitzenden optionalen String,
ungültige `nil`; Empfänger und Index werden je einmal ausgewertet. Laufzeit-
und Negativtests prüfen Unicode, Grenzen, Besitz und Typfehler. Der Clang-
Gesamtbuild, 142 Sprachtests und zwei App-Workflows bestehen. MSVC Release
besteht beide gezielten Tests, der ASan-Debug-Build den Laufzeittest. Die
generierte Sprachreferenz ist geprüft.

Sprachvertrag 0.132.0 ergänzt `attempt(ausdruck)` als erste lokale Behandlung
von Laufzeitfehlern. Der Ausdruck liefert einen Wert als Optional oder `nil`
bei einem abgefangenen Fehler. Temporäre Besitzer und Rekursionstiefe werden
bei Fehlern wiederhergestellt; bereits sichtbare Seiteneffekte bleiben bestehen.
Gezielte Tests decken verschachtelte Aufrufe, Besitz, wiederholte Fehler und
den Experimentmodul-ABI-Pfad ab. Der Clang-Gesamtbuild, 140 Sprachtests und
zwei App-Workflows bestehen; MSVC Release baut und besteht die drei gezielten
`attempt`-Tests. Die generierte Sprachreferenz ist geprüft.
AddressSanitizer fand eine zunächst zu spät ausgeführte Bereinigung im
Erfolgszweig; nach Korrektur bestehen die eigenständigen und Modul-Tests auch
im ASan-Debug-Build.

Sprachvertrag 0.131.0 ergänzt `array.get(at:)` für sichere Arraylesezugriffe.
Die Methode liefert einen unabhängigen optionalen Elementwert oder `nil` bei
ungültigem Index, auch für besitzende und bereits optionale Elemente. Gezielte
Laufzeit- und Negativtests prüfen Grenzen, Typen, Kopien und Auswertung genau
einmal. Der Clang-Gesamtbuild, 137 Sprachtests und zwei App-Workflows
bestehen; MSVC Release baut und besteht die beiden gezielten Array-get-Tests.

Sprachvertrag 0.130.0 ergänzt `Int64.parse` und `Float64.parse` als
fehlerverträgliche Textkonvertierungen mit optionalem Ergebnis. Ungültige
Schreibweisen und Bereichsüberschreitungen liefern `nil`; die bestehenden
Konvertierungsaufrufe behalten ihre Quelldiagnosen. Compiler-, Laufzeit- und
Negativtests prüfen Grenzen, Argumenttypen und die Verwendung mit `guard`.
Der Clang-Gesamtbuild, 135 Sprachtests und zwei App-Workflows bestehen;
MSVC Release baut und besteht die beiden Parse-Tests. Die generierte
Sprachreferenz ist geprüft.

Sprachvertrag 0.129.0 ergänzt `guard … else:` für frühe Rückkehr aus Funktionen.
`guard let` und `guard var` halten den entpackten optionalen Wert im
umgebenden Block verfügbar, einschließlich gleichnamiger Bindung.
Die Typprüfung verlangt einen auf allen Pfaden zurückkehrenden `else`-Block
und verhindert Sprünge, die ihn umgehen. Compiler- und Laufzeittests prüfen
Typen, Sichtbarkeit, Eigentum und Kontrollfluss. Der Clang-Gesamtbuild,
133 Sprachtests und zwei App-Workflows bestehen; unter MSVC Release bestehen
die gezielten Lexer-, Parser-, Checker- und Guard-Tests.

Sprachvertrag 0.128.0 ergänzt `\(ausdruck)`-Interpolation in ein- und
mehrzeiligen Strings. Der Lexer erfasst verschachtelte Ausdrücke; der Parser
erhält ihre Quellpositionen. Die Typprüfung akzeptiert `Bool`, `Int64`,
`Float64` und `String`; das Backend wertet jeden Ausdruck einmal aus und
räumt Zwischenwerte nach dem Zusammenfügen auf. Laufzeit-, Typ-, Syntax- und
Mutationstests decken die Funktion ab, einschließlich CRLF-Normalisierung.
Der Clang-Gesamtbuild, 131 Sprachtests und zwei App-Workflows bestehen;
unter MSVC Release bestehen die gezielten Lexer-, Parser-, Checker- und
Interpolationstests.

Sprachvertrag 0.127.0 ergänzt den rechtsassoziativen bedingten Ausdruck
`bedingung ? wennWahr : wennFalsch` mit `Bool`-Bedingung, gleichem Werttyp in
beiden Zweigen und verzögerter Auswertung. Die C17-Ausgabe erhält besitzende
Ergebnisse auch beim Aufräumen der Zweig-Zwischenwerte. Laufzeit- und
Negativtests prüfen Auswertung, Typen und Syntax; die Mutationstests decken
auch unvollständige `?`-Ausdrücke ab. Der Clang-Gesamtbuild, 129 Sprachtests
und zwei App-Workflows bestehen. Unter MSVC Release bestehen die vier
gezielten Parser-, Checker- und Ausdruckstests.

Sprachvertrag 0.126.0 ergänzt `String.trimmed()` auf Basis der Unicode-15.0-
Eigenschaft `White_Space`. Die Methode erhält innere Zeichen, lässt U+200B und
U+FEFF stehen und wahrt den Eingabewert auch bei Allokationsfehlern.
Compiler- und Laufzeittests prüfen die Methode und eigene gleichnamige Methoden.
Der Clang-Gesamtbuild, 127 Sprachtests und beide Kollisions-Workflows bestehen;
der gezielte MSVC-Build und fünf Sprachtests bestehen ebenfalls.

Sprachvertrag 0.125.0 ergänzt `[T].repeated(count:)` für alle Array-Elementtypen.
Die Laufzeit prüft den Zähler und die Ergebnisgröße vor der Allokation und
räumt bei Kopierfehlern besitzende Elemente auf. Compiler- und Laufzeittests
decken Skalare, Strings, verschachtelte Arrays und Strukturen ab.
Der Clang-Gesamtbuild, 125 Sprachtests und beide Kollisions-Workflows bestehen;
der gezielte MSVC-Build und fünf Sprachtests bestehen ebenfalls.

Sprachvertrag 0.124.0 ergänzt `String.repeated(count:)` mit expliziter
`Int64`-Typprüfung, nichtnegativem Zähler, Größenlimit und besitzendem
Stringergebnis. Der native Stringtest prüft Unicode, Grenzwerte und
Allokationsfehler; Sprachtests prüfen Aufruf, Typfehler und Quelldiagnose.
Der Clang-Gesamtbuild, 123 Sprachtests und beide Kollisions-Workflows bestehen;
der gezielte MSVC-Build und fünf Sprachtests bestehen ebenfalls. Nach der
Kopieroptimierung wurden die drei betroffenen Tests auf beiden Compilern
erneut ausgeführt.

Sprachvertrag 0.123.0 ergänzt Unicode-Skalar-Escapes `\u{...}` in beiden
Stringformen. Der Lexer weist ungültige Skalare und U+0000 vor der Ausführung ab;
Typprüfung und C17-Emitter dekodieren gültige Zeichen identisch, auch für
`switch`-Fallmuster. Lexer-, Laufzeit- und Duplikattests decken die Regel ab.
Der Clang-Gesamtbuild, 121 Sprachtests und beide Kollisions-Workflows bestehen;
der MSVC-Build und sieben gezielte Sprachtests bestehen ebenfalls.

Sprachvertrag 0.122.1 vereinheitlicht die Stringdekodierung von Typprüfung und
C17-Emitter. `switch` erkennt nun gleiche einzeilige und dreifach zitierte
Muster, auch bei CRLF im Quelltext, als Duplikat. Ein ausdrückliches `\r`
bleibt von einem normalisierten Zeilenumbruch unterscheidbar. Compiler- und
Laufzeittests prüfen beide Seiten. Der Clang-Build und alle 119 Sprachtests sowie
beide Kollisions-Workflows bestehen; der MSVC-Build und zehn gezielte Sprachtests
einschließlich Editor-Vorschau bestehen ebenfalls.

Sprachvertrag 0.122.0 ergänzt `Int64.abs`, `Int64.min`, `Int64.max` und
`Int64.clamp` als statische Methoden. Sie erhalten die volle 64-Bit-Genauigkeit;
`abs` am kleinsten Wert und vertauschte Clamp-Grenzen melden die Quellposition.
Positive Laufzeitfälle prüfen Grenzen, benannte Argumente und einmalige
Auswertung; negative Fälle prüfen Typen, Argumentzahl und Laufzeitfehler.
Der vollständige Clang/Ninja-Build und 118 Tests im Sprachfilter bestehen.
Zehn betroffene Tests bestehen zusätzlich unter MSVC Release; Kugelstoß- und
Boxstoß-App-Abläufe bestehen im Clang-Build, der Boxstoß-Ablauf auch unter MSVC.
Die generierte Bibliotheksreferenz enthält die vier neuen Methoden.

Sprachvertrag 0.121.0 ergänzt dreifach zitierte mehrzeilige Strings. Der Lexer
verfolgt Zeile und Spalte auch innerhalb des Literals; der C17-Emitter speichert
echte CRLF-, CR- und LF-Zeilenumbrüche einheitlich als LF. Escapes behalten
ihre ausdrückliche Bedeutung. Native Laufzeittests decken leere Werte,
Anführungszeichen und `switch`-Muster ab; ein separater Test prüft die
emittierten Bytes bei gemischten Quell-Zeilenumbrüchen.
Der vollständige Clang/Ninja-Build und 116 gezielte Sprachtests bestehen;
Lexer-, CLI-, Experiment-, Analyse-, Kollisions- und Laufzeittests bestehen
zusätzlich unter MSVC Release.

Die Boxstoß-Sprachvorlage ergänzt den Sprachvertrag 0.120.0 ohne neue
Bindungen. Winkel, Versatz, Rückprall und Reibung sind zur Laufzeit einstellbar.
Der Paritätstest vergleicht die drei C-Referenzvarianten über 401 Zeitpunkte
in allen 15 Messkanälen und die Szene vor und nach dem Stoß. Die eigene
Sprachauswertung liest C- und Sprachläufe. Der App-Workflow prüft Editor,
Build, Simulation, Analyse, Exporte sowie das Speichern und erneute Laden
eines geänderten Rückprallparameters. Paritäts- und App-Test bestehen unter
MSVC Release und Clang/Ninja. Der Workspace-Test legt die Sprachvorlage auch
über den Projektmanager an und prüft Experiment- und Analysequellen.
Auch die 115 nicht als Workflow markierten Sprach- und Boxstoßreferenztests
bestehen im Clang/Ninja-Build.

Sprachvertrag 0.120.0 bindet die C-Einzelkontaktantwort als
`Contacts.resolveSingle`. Eine neue Kugelstoß-Sprachvorlage reproduziert die
vier C-Referenzvarianten für Vakuum, Luft, benutzerdefiniertes Medium und
Reibung mit Anfangsrotation, jeweils mit elf Kanälen und 401 Messpunkten.
Die Vakuumszene stimmt vor und nach dem Stoß überein. Die eigene
Sprachauswertung und der App-Workflow bestehen
unter MSVC und Clang; beide Kreuzrichtungen von C- und Physim-Auswertung sind
ebenfalls geprüft. Ungültige Kontaktzahl und Rückprallparameter liefern
Quelldiagnosen. Medium, Widerstand, Reibung, Rotation, Geschwindigkeit und CCD
sind Experimentparameter; der App-Test prüft Reibung und Rotation bis zur
Laufdatei und zur erneuten Projektöffnung. Der vollständige Clang/Ninja-Build und 113 Sprachtests
bestehen; sechs betroffene Sprachtests bestehen zusätzlich unter MSVC.

Sprachvertrag 0.119.0 schreibt die tatsächliche Sprachversion und die getrennte
Compilerversion in generierte Experiment- und Analysemodule. Deren gespeicherte
Läufe und Berichte tragen damit die Version des übersetzten Quelltexts statt
des früheren festen Entwicklungswerts. Der vollständige Clang/Ninja-Build und
112 gezielte Sprachtests bestehen; die betroffenen Experiment-, Analyse-,
Sensor- und CLI-Tests bestehen zusätzlich unter MSVC.

Die App fragt nach dem Build den Parameterkatalog über `physim-runner --describe`
in einem getrennten Prozess ab. Der Inspector zeigt daraus Eingabefelder mit
Beschreibung, Standardwert und Grenzen; ein interaktiver Lauf übergibt die
gewählten Werte und speichert sie in `.psrun`. Die Laufserien-Ansicht startet
lineare Parameterstudien und lädt deren Kurvenbericht. Der Sprach-Wurf-Workflow
prüft App-Override und App-Parameterstudie bis zur gespeicherten Datei und zum
Bericht. Ausgewählte Parameterwerte bleiben außerdem in `physim.project` über
das erneute Öffnen erhalten. Katalogparser und Runner-Discovery sind separat getestet; die beiden
gezielten Tests bestehen auch unter MSVC.

Sprachvertrag 0.118.0 bindet `Submersion.sphere` an die gemeinsame
Kugel-Eintauchgeometrie. Volumen und Schwerpunktabstand sind kopierbare,
unveränderliche Werte; positive und negative Sprachtests prüfen Geometrie,
Typfehler und Laufzeitdiagnosen. Ein weiteres Sprachbeispiel bildet die
C-Auftriebsvorlage mit zwölf Kanälen ab und wird über 401 Zeitpunkte sowie
26 Szeneobjekte verglichen. Die Sprachauswertung erzeugt drei Diagramme und
einen CSV-Export. Der App-Workflow prüft Projektanlage, Build, Lauf,
Auswertung und erneutes Öffnen unter MSVC und Clang. Der Projektmanager zeigt
dazu passende Vorlagennamen für die gewählte Sprache.

Laufserien übergeben jetzt zusätzlich feste Parameterwerte aus dem Inspector.
Die CLI akzeptiert dafür wiederholte `--param name=wert`-Argumente. Ein
Sprachintegrationstest prüft den gemeinsamen Effekt eines linearen Sweeps
und eines konstanten zweiten Parameters in allen Rohdaten und im Bericht.

Der Batch-CLI-Controller führt nun lineare Studien über einen benannten
Physim-Experimentparameter aus. Jeder Lauf erhält einen eigenen Wert und Seed;
Manifest, Rohdateien und CSV halten die Zuordnung fest. Der Bericht zeigt
Endwert gegen Parameter anstelle einer irreführenden Monte-Carlo-Statistik.
Ein End-to-End-Test prüft drei Sprachläufe, Metadaten, CSV und Bericht.
Die App-Steuerung wurde im folgenden Schritt ergänzt.
Clang/Ninja und MSVC bestehen den neuen Sweep-Test; fünf weitere gezielte
Clang-Tests für Batch, Parameter-API, Einzellauf und Referenz bestanden ebenfalls.

Sprachvertrag 0.117.0 ergänzt benannte Experimentparameter mit Standardwert,
inklusiven Grenzen, Runner-Overrides und gespeicherten wirksamen Werten in `.psrun`.
Ein Sprach-Experiment prüft den Standardwert, einen Override und ungültige Eingaben
vom CLI bis zum gelesenen Lauf. Das App-Parameterformular wurde später ergänzt.
Der vollständige Clang/Ninja-Build besteht. Von 197 CTests bestanden 196 sofort;
der Test der veralteten Tutorial-Codekopie bestand nach deren Aktualisierung
im gezielten Wiederholungslauf. Alle 18 generierten Referenzdokumente sind aktuell.
Der MSVC-Debug-Build und beide gezielten Parametertests bestehen ebenfalls.

Sprachvertrag 0.116.0 ergänzt `Medium.stokesDrag` mit der Viskosität des
Mediums und dem gemeinsamen geprüften Kugel-Widerstandsmodell. Ungültige
Radien melden die Quellposition.
Der vollständige Clang-Build und die Sprachsuite (115/115), fünf gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.115.0 bindet `Material` mit geprüfter Dichte, Rückprall und
Reibung als Werttyp. Der Materialwert liefert die Solverparameter und im
Box-Stoßexperiment die Körpermasse.
Der vollständige Clang-Build und die Sprachsuite (114/114), fünf gezielte
MSVC-Tests einschließlich Kontakt-Workflow und 18 Referenzdokumente bestehen.

Sprachvertrag 0.114.0 bindet `Medium` als Werttyp an die gemeinsame
C-Bibliothek: geprüfte eigene Medien, Luft/Wasser/Vakuum, SI-Eigenschaften
und quadratische Widerstandskraft mit Quellfehlern.
Das Wurfexperiment verwendet die Bindung; der vollständige Clang-Build und
die Sprachsuite (112/112), sechs gezielte MSVC-Tests und 18
Referenzdokumente bestehen.

Sprachvertrag 0.113.0 ergänzt Skalarmultiplikation und -division sowie
Vorzeichenoperatoren für `Quantity`; Einheit und statische Dimension bleiben
erhalten. Division durch null erhält die Quellposition des Operators.
Der vollständige Clang-Build und die Sprachsuite (108/108), sechs gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.112.0 ergänzt `+`, `-`, `+=` und `-=` für `Quantity`-Werte
mit nativer Ausführung, Einheitenumrechnung und statischer Dimensionsprüfung.
Der vollständige Clang-Build und die Sprachsuite (107/107), fünf gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.111.0 wertet begrenzte ganzzahlige Arithmetik in bekannten
Dimensionsexponenten statisch aus. Der vollständige Clang-Build und die
Sprachsuite (105/105), drei gezielte MSVC-Tests und 18 Referenzdokumente
bestehen.

Sprachvertrag 0.110.0 prüft statisch bekannte SI-Dimensionen bei Konvertierung
und Addition/Subtraktion von `Quantity`. Literale, Einheitenalgebra und
unveränderliche Aliase tragen die Dimension; dynamische Werte bleiben
laufzeitgeprüft. Vollständig dimensionierte Typannotationen sind noch offen.
Der vollständige Clang-Build und die Sprachsuite (105/105), drei gezielte
MSVC-Tests und 18 generierte Referenzdokumente bestehen.

Sprachvertrag 0.109.0 ergänzt `array.sorted()` und `array.sort()` für
`[Int64]`, `[Float64]` und `[String]`. Die geordnete Kopie und die mutierende
Form halten vorhandene Array-Snapshots und räumen fehlgeschlagene Kopien auf.
Der vollständige Clang-Build und die Sprachsuite (104/104), drei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.108.0 ergänzt `Series.quantile(probability)` mit Typ-7-Interpolation
für gespeicherte und abgeleitete Reihen. Die gemeinsame Series-Bibliothek stellt
dieselbe Funktion auch der C-API bereit.
Der vollständige Clang-Build, die Sprach- und Series-Suite (103/103), zwei
gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.107.0 ergänzt das blockweise Lesen eines begrenzten
Series-Ausschnitts als unabhängiges `[Float64]`. Damit können Physim-Analysen
eigene Arrayberechnungen auf gespeicherten oder abgeleiteten Reihen ausführen.
Der vollständige Clang-Build und die Sprachsuite (101/101), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.106.0 bindet den CSV-Export von 1 bis 32 zugeordneten
Datenreihen als `Series.exportColumns` ein. Der Analyse-Runner prüft einen
dreispaltigen Export samt Einheiten sowie leere und nicht zugeordnete Eingaben.
Der vollständige Clang-Build und die Sprachsuite (100/100), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.105.0 erlaubt typisierte `var`-Deklarationen ohne sofortigen
Initialwert. Die erste einfache Zuweisung initialisiert den Wert; frühe Zugriffe
auf lokale oder globale Variablen führen zu einer Quelldiagnose. Besitzende
Werttypen und Closure-Captures folgen denselben Regeln.
Der vollständige Clang-Build und die Sprachsuite (99/99), drei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.104.0 ergänzt geordnete, explizite Modulsuchpfade für CLI und
CMake. Lokale Module haben Vorrang; kanonische Dateipfade verhindern mehrfaches
Laden desselben Moduls über verschiedene Importwege. Die Suchpfade gelten auch
für transitive Imports und werden beim CMake-Neubau berücksichtigt.
Der vollständige Clang-Build und die Sprachsuite (97/97), zwei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.103.0 ergänzt `any` und `all` für Arrays mit typisierten
Prädikaten, früher Beendigung und festgelegtem Verhalten für leere Arrays.
Die Auswertung bewahrt den Array-Snapshot; Laufzeit- und Diagnosetests prüfen
Kurzschluss, Aufrufreihenfolge, Funktionswerte und Typfehler. Der vollständige
Clang-Build und die Sprachsuite (95/95), zwei gezielte MSVC-Tests und 18
Referenzdokumente bestehen.

Sprachvertrag 0.102.0 ergänzt Überladung konkreter und generischer Methoden
auf Strukturen und Enums, auch für statische und gebundene Methodenwerte.
Die Auswahl berücksichtigt Empfängerart, Argumente, Constraints und erwartete
Funktionstypen; mutierende Methoden prüfen die Veränderlichkeit nach der
Auswahl. Tests umfassen generische und importierte Typen sowie Mehrdeutigkeit.
Der vollständige Clang-Build und die Sprachsuite (93/93), vier gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.101.0 erweitert die Überladungsauflösung freier Funktionen
auf generische Vorlagen. Argumente, explizite Typargumente, Constraints,
erwartete Rückgabetypen und spezialisierte Funktionsreferenzen entscheiden
zwischen konkreten und generischen Kandidaten, auch über Modulgrenzen.
Methodenüberladung und weitere Sprachziele bleiben offen. Der vollständige
Clang-Build und die Sprachsuite (91/91), sechs gezielte MSVC-Tests und 18
Referenzdokumente bestehen.

Sprachvertrag 0.100.0 ergänzt Überladung nichtgenerischer freier Funktionen
nach Argumentform und Parametertyp, einschließlich Modulimporten und
Funktionswerten mit erwartetem Typ. Gleichrangige Aufrufe, fehlende Treffer
und doppelte Signaturen werden diagnostiziert. Generische freie Funktionen
und Methoden bleiben für die allgemeine Überladungsauflösung offen. Der
vollständige Clang-Build und die Sprachsuite (89/89), vier gezielte MSVC-Tests
sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.99.0 ergänzt `Array.reduce(initial, combine)` mit
`func(U, T) -> U`. Der Compiler leitet den Akkumulatortyp aus der
Funktionssignatur ab, prüft den Startwert und verwaltet besitzende
Zwischenwerte bei jedem Schritt. Tests decken leere Arrays, Auswertungsreihenfolge,
Closures, Methoden, gemischte Element- und Akkumulatortypen und ungültige
Signaturen ab. Der vollständige Clang-Build und die Sprachsuite (87/87),
vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.98.0 ergänzt `Array.map(transform)` mit aus `func(T) -> U`
abgeleitetem Ergebnistyp. Das C-Backend übernimmt besitzende Rückgabewerte
in ein eigenständiges `[U]` und gibt temporäre Werte nach jeder Kopie frei.
Tests prüfen Zahlen, Strings, Strukturen, verschachtelte Arrays,
Funktionswerte, Enumfall-Konstruktoren, Closures, gebundene Methoden und
ungültige Signaturen. Der vollständige Clang-Build und die Sprachsuite
(85/85), vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.97.0 ergänzt `Array.filter(predicate)` für Funktionswerte vom
Typ `func(T) -> Bool`. Der C-Emitter verarbeitet freie Funktionen, Closures,
gebundene Methoden und lokale Funktionen; das Ergebnis besitzt seine Elemente.
Tests prüfen Auswahlreihenfolge, Eingabesnapshot, Kopien besitzender Werte und
ungültige Prädikatsignaturen. Der vollständige Clang-Build und die Sprachsuite
(83/83), vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.96.0 erlaubt feste `Float64`-Bereiche auch in
`Optional.some(...)` und Enum-Nutzdatenmustern. Die Typprüfung erkennt
vollständig verdeckte Teilmuster; der C-Emitter vergleicht die beiden Grenzen
direkt am Nutzdatenwert. Positive und negative Tests decken optionale Werte
und Enums ab. Der vollständige Clang-Build und die Sprachsuite (81/81), fünf
gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.95.0 ergänzt feste `Float64`-Bereiche im direkten `switch`.
Der Checker prüft endliche, nichtleere Grenzen und Überschneidungen mit
Literalen und anderen Bereichen. Der C-Emitter vergleicht die Grenzen mit
offener oder geschlossener Obergrenze. Der vollständige Clang-Build und die
Sprachsuite (81/81), vier gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.94.0 ergänzt `Series.minimum()` und `maximum()` für
nichtleere Reihen. Die blockweise Berechnung bleibt bei sehr großen endlichen
Wertespannweiten nutzbar, auch wenn die Varianz nicht darstellbar ist.
Der vollständige Clang-Build und die Sprachsuite (81/81), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.93.0 ergänzt eine optionale `String`-Diagnose für `assert`.
Der Compiler prüft Argumentzahl und Typ, erhält die normale Auswertungsreihenfolge
und gibt bei Fehlschlag die Quellposition mit dem angegebenen Text aus.
Der vollständige Clang-Build und die Sprachsuite (81/81), drei gezielte
MSVC-Tests und 18 Referenzdokumente bestehen.

Der Sprach-Lernpfad enthält nun eine Mehrlaufanalyse mit C- und Physim-Eingaben.
Der eingebettete Code wird gegen das ausgeführte Beispiel synchron geprüft;
das App-Handbuch führt direkt zu dieser Anleitung. Gemischte Lauf- und
Berichtsprüfung sowie Quelldokument-Abgleich bestehen unter Clang und MSVC;
der Clang-Dokumentationsfenstertest lädt die neuen Inhalte.

Sprachvertrag 0.92.0 erlaubt das Auslesen von Einheitensymbol, Maßstab und
SI-Exponenten jeder `Series`, auch nach Transformationen. `isAlignedWith`
prüft die Zuordnung zweier gültiger Reihen vor gemeinsamen Operationen.
Der vollständige Clang-Build und die Sprachsuite (81/81), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.91.0 macht gespeicherte Messkanäle ohne vorab bekannten Namen
zugänglich: `Dataset.channelName`, `channelUnitSymbol`, `channelDescription`
und `channelExponent` prüfen Indizes und liefern eigenständige Textkopien.
Der vollständige Clang-Build und die Sprachsuite (81/81), der gezielte
MSVC-Analysetest und 18 Referenzdokumente bestehen.

Sprachvertrag 0.90.0 bindet `Series.slice(first, count)` und `Series.name()` an
das C-Analyse-SDK. Ausschnitte kopieren ihre Werte und erhalten für passende
Zeit- und Messreihen die Zuordnung; Namen werden als eigenständige Strings
kopiert. Der vollständige Clang-Build und die Sprachsuite (81/81) bestehen;
der gezielte MSVC-Analysetest und alle 18 Referenzdokumente sind geprüft.

Sprachvertrag 0.89.0 ergänzt Host- und Dataset-Provenienz: `runSeed()` liefert
das vollständige Laufseed-Bitmuster in Experimenten; Analysen lesen
Datensatz- und Kanalzahl, Recovery-Status und eigene Metadatenkopien aus
`Dataset`. Gemischte C-/Sprachläufe und Seedwerte bis `UINT64_MAX` werden
in den nativen Runner-Tests geprüft. Der vollständige Clang-Build und die
Sprachsuite (81/81), drei gezielte MSVC-Tests und 18 Referenzdokumente bestehen.

Sprachvertrag 0.88.0 ergänzt den bedingten Optional-Ersatzoperator `??`.
Er bindet schwächer als logisches Oder, gruppiert von rechts und wertet den
Ersatz nur bei `nil` aus. Typprüfung, C-Emitter und Besitzverwaltung sind für
skalare, Array-, String- und Structwerte sowie verschachtelte Optionals geprüft.
Der vollständige Clang-Build und die Sprachsuite (81/81) bestanden; fünf
gezielte MSVC-Tests und 18 Referenzdokumente wurden ebenfalls geprüft.

Sprachvertrag 0.87.0 löst explizit spezialisierte generische
Initialisiererwerte bei mehreren `init`-Methoden anhand des erwarteten
Funktionstyps auf. Verschachtelte Formen wie `T?` und `[T]` sowie
generische Structs sind geprüft; fehlende oder mehrdeutige Signaturen
erhalten gezielte Compilerdiagnosen.

Sprachvertrag 0.86.0 ergänzt Struct-Initialisierer als Funktionswerte.
Ein erwarteter Funktionstyp wählt überladene Initialisierer anhand ihrer
Signatur aus; generische Structs, explizit spezialisierte Initialisierer und
importierte Structs sind abgedeckt. Mehrdeutige Signaturen und fehlende
Spezialisierung erhalten eigene Compilerdiagnosen.

Sprachvertrag 0.85.0 ergänzt rekursive `Optional.some`-Nutzlastmuster in
optionalen Werten und Enumfeldern. Der Checker prüft innere Typen,
Fallabdeckung und verdeckte Muster; der C-Emitter kopiert und räumt auch
verschachtelte besitzende Bindungen korrekt auf. Generische und importierte
Enums sind in Laufzeittests enthalten.
Der vollständige Clang-Build, die Sprachsuite (81/81), sechs gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.84.0 erlaubt direkte `Float64`-Werte im `switch`. Feste
endliche Zahlenliterale werden binär exakt verglichen; der Checker erkennt
wertgleiche Fälle, verlangt `default` und lehnt variable oder Bereichsmuster ab.
Der vollständige Clang-Build, die Sprachsuite (81/81), drei gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.83.0 erweitert feste `Int64`-Bereiche auf optionale und
Enum-Nutzlastmuster. Die Typprüfung verlangt gültige Literalgrenzen;
Erreichbarkeit und vollständige Abdeckung berücksichtigen Teilbereiche.
Laufzeittests umfassen generische und importierte Enums.
Der vollständige Clang-Build, die Sprachsuite (81/81), sechs gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.82.0 ergänzt `Int64`-Bereichsmuster im `switch` mit geschlossener
oder offener Obergrenze. Der Checker verlangt feste gültige Grenzen und erkennt
Überschneidungen mit unbewachten Fällen; Guard-Fallbacks bleiben möglich.
Der vollständige Clang-Build, die Sprachsuite (81/81), drei gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.81.1 korrigiert die Erreichbarkeitsprüfung von `Float64`-
Nutzlastmustern: verschiedene Schreibweisen desselben binären Werts, auch
gerundete Werte und ±0, gelten als Duplikate. Die Prüfung verwendet wie die
Laufzeit einen festen Dezimalpunkt und weist nicht endliche Literale zurück.
Der vollständige Clang-Build, die Sprachsuite (81/81), sechs gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.81.0 erlaubt `nil` als Teilmuster für optionale Nutzlasten in
Enums und verschachtelten Optionalwerten. Dabei bleiben äußerer und innerer
Leerwert unterscheidbar. Typprüfung, Fallabdeckung, Duplikate und Laufzeitverhalten
sind auch für generische und importierte Enumfälle geprüft.
Der vollständige Clang-Build und die Sprachsuite (81/81), sieben gezielte
MSVC-Tests sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.80.0 ergänzt Literalvergleiche in Enum- und Optional-Nutzlastmustern
für `Bool`, `Int64`, `Float64` und `String`. Teilmuster zählen nur ihren tatsächlich
verglichenen Wert; umfassende Muster und `default` sichern die Fallabdeckung.
Typfehler, fehlende Abdeckung und unerreichbare Zweige sind geprüft.
Der vollständige Clang-Build, die Sprachsuite (81/81), fünf gezielte MSVC-Tests
und 18 Referenzdokumente bestehen.

Sprachvertrag 0.79.0 macht Enumfälle mit Nutzlast zu speicherbaren
Konstruktor-Funktionswerten. Indirekte Aufrufe erzeugen eigene Enumwerte;
String-Nutzlasten, mehrere Felder, Arrays und Optionalwerte als Ablage,
generische sowie importierte Enums und Typfehler sind geprüft.
Der vollständige Clang-Build und die Sprachsuite (81/81), die gezielten
MSVC-Tests (5/5) sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.78.0 ergänzt generische Enums mit typisierten und besitzenden
Nutzdaten. Konstruktoren können Typargumente aus Nutzlast oder erwartetem Typ
ableiten; Methoden, Fallmuster, Raw Values, Imports, Funktionswerte, rekursive
Optionalfelder und Constraints verwenden die spezialisierte Enumidentität.
Der vollständige Clang-Build und die Sprachsuite (81/81), die gezielten
MSVC-Tests (3/3) sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.77.0 ergänzt qualifizierte Modulimporte aus Unterordnern und
optionale Aliase. Der Loader teilt identische transitive Abhängigkeiten,
meldet fehlende Dateien und Zyklen am Importort; CMake beobachtet nun auch
Unterordner der Einstiegsmodule.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (4/4) sowie 18 Referenzdokumente bestehen. Eine berührte
Unterordnerdatei löste den erneuten CMake-Build des Einstiegsmoduls aus.

Sprachvertrag 0.76.0 erlaubt mutierende Instanzmethoden als gebundene
Funktionswerte. Die Bindung kopiert den Empfänger; Kopien des Funktionswerts
teilen seinen veränderlichen Zustand. Structs mit besitzenden Strings/Arrays,
Enums mit besitzender Nutzlast, generisch spezialisierte Methoden, Array- und
Optionalspeicherung sowie
skalare Analyse-Callbacks sind geprüft. Das ursprüngliche Objekt bleibt
unverändert; gleichzeitige Zugriffe auf denselben Wert sind nicht unterstützt.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (2/2) sowie 18 Referenzdokumente bestehen. Das Analysemodul wurde
auch mit MSVC gebaut.

Sprachvertrag 0.75.0 spezialisiert generische benannte lokale Funktionen im
lexikalischen Deklarationsbereich. Jede verwendete Spezialisierung erzeugt
einen besitzenden Funktionswert; Inferenz, explizite Typargumente, Constraints,
Rekursion, verschachtelte Closures und numerische Analyse-Callbacks sind geprüft.
Später deklarierte lokale Werte bleiben unsichtbar. Mutierende Methoden als
Funktionswerte wurden in 0.76.0 ergänzt.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (6/6) sowie 18 Referenzdokumente bestehen.

Sprachvertrag 0.74.0 ergänzt benannte lokale Funktionen als besitzende Closures.
Sie können lokale Werte aus mehreren Schachtelungsebenen erfassen, über ihren
Gültigkeitsbereich hinaus zurückgegeben werden und sich selbst rekursiv aufrufen.
Direkte Aufrufe erlauben benannte Argumente; gespeicherte Werte bleiben
positionsgebunden. Native Programme und Analyse-Callbacks prüfen diese Fälle.
Generische lokale Funktionen wurden in 0.75.0 und mutierende Methoden als
Werte in 0.76.0 ergänzt.
Der vollständige Clang-Build und die Sprachsuite (79/79), die gezielten
MSVC-Tests (6/6) und 18 Referenzdokumente bestehen.

Sprachvertrag 0.73.0 ergänzt anonyme Funktionen mit typisierten Parametern und
eingerückten Körpern. Closures besitzen Kopien verwendeter lokaler Werte,
einschließlich verschachtelter und generisch spezialisierter Fälle; direkte
und gespeicherte Aufrufe sowie skalare und ODE-Callbacks sind geprüft.
Erfasste Werte sind innerhalb der Closure unveränderlich. Modulvariablen
bleiben globale Zugriffe. Benannte lokale Funktionen wurden in 0.74.0 ergänzt;
mutierende Methoden als Werte in 0.76.0.
Der vollständige Clang-Build und die Sprachsuite (79/79), gezielte MSVC-Tests
(7/7) sowie 18 erzeugte Referenzdokumente bestehen.

Sprachvertrag 0.72.0 führt gebundene, nicht mutierende Instanzmethoden als
besitzende Funktionswerte ein. Die Empfängerkopie überlebt lokale Gültigkeits-
bereiche, kann in Arrays und Optionalwerten liegen und wird über die
Analyse-Callback-Brücke aufgerufen. Mutierende Methoden bleiben als Werte offen.
Die vollständige Clang-Sprachsuite besteht mit 78/78 Tests; die gezielten
MSVC-Tests und der neue Enum-Empfänger mit besitzender Nutzlast bestehen.
Der zusätzliche MSVC-AddressSanitizer-Lauf liefert hier keine Diagnose:
sowohl der Funktionswerttest als auch der unveränderte `memory`-Test
überschreiten ihr Zeitlimit ohne Ausgabe.

Sprachvertrag 0.71.0 übergibt typisierte Funktionswerte an skalare Suche,
ODE-Integratoren und Verlet-Integration. Die native Callback-Brücke wählt das
Ziel zur Laufzeit, übernimmt den Modulzustand und prüft die Signatur vor dem Build.

Sprachvertrag 0.70.0 erlaubt explizit spezialisierte generische Funktionen und
statische Methoden als Funktionswerte, auch aus importierten Modulen und
generischen Strukturen. Parser, Typprüfung und nativer Aufruf wurden für den
Analyse-Runner geprüft.

Sprachvertrag 0.69.0 erweitert Funktionswerte um statische Methoden von
Strukturen und Enums, einschließlich spezialisierter generischer Strukturen
und importierter Typen. Die indirekten Aufrufe verwenden denselben Modulzustand.

Sprachvertrag 0.68.0 führt strukturell typisierte Werte freier Funktionen ein.
Programme, importierte Module und die Analyse-Runner-ABI prüfen Übergabe,
Rückgabe, indirekte Aufrufe, generische Argumente, Struct-/Array-/Optional-
Speicherung sowie besitzende String-Ergebnisse. Gebundene Methoden und
Closures waren zu diesem Stand offen.

Sprachvertrag 0.67.0 erlaubt absteigende `for`-Bereiche mit negativem,
von null verschiedenem `by`-Schritt. Die Codeerzeugung prüft beide
`Int64`-Überlaufgrenzen; Slice-Indizes bleiben auf positive Schritte
beschränkt. Native Referenzen prüfen Richtung, Kontrollfluss und Grenzwerte.

Sprachvertrag 0.66.0 verbindet komponentenweise RK45-Toleranzen mit dem
besitzenden `OdeResult`-Bericht. Native und Analyse-Referenzen prüfen
unterschiedlich skalierte Zustände, Berichtszähler und Fehlerpfade.

Sprachvertrag 0.65.0 stellt die Berichte für Bisektion und Goldener-Schnitt-
Minimierung als `ScalarResult` bereit. Der Wert enthält Koordinate,
Funktionswert, Intervall und Iterations-/Auswertungszähler. Native und
Analyse-Referenzen prüfen beide Verfahren sowie Fehlerpfade.

Sprachvertrag 0.64.0 stellt den RK45-Bericht als kopierbaren `OdeResult` mit
eigenem Zustandsarray bereit. Native Referenzen prüfen Vorwärts- und
Rückwärtsintegration, Berichtsfelder, Kopien, Array- und Optionalwerte sowie
Fehlerpfade; das Analysemodul verwendet dieselbe Runner-ABI.

Sprachvertrag 0.63.0 bindet die komponentenweisen absoluten RK45-Toleranzen
der C-API als `rk45IntegrateWithTolerances`. Das Array muss für jede
Zustandskomponente einen positiven Wert enthalten und wird vor den Callbacks
kopiert. Native Referenzen prüfen zwei unterschiedlich skalierte Größen,
unveränderte Eingaben und ungültige Toleranzen; das Analysemodul führt die
Funktion über die Runner-ABI aus.

Sprachvertrag 0.62.0 bindet Velocity Verlet mit einem benannten
Beschleunigungs-Callback an die C-Numerik. Ein Phasenarray enthält 1–32
Positionen und danach gleich viele Geschwindigkeiten; das Ergebnis besitzt
seinen Speicher selbst. Native Referenzen prüfen Umkehrbarkeit, 32
Freiheitsgrade sowie Fehlerpfade. Analyse-Runner und Pendel-Experiment verwenden
die Callback-Brücke; das Pendel wird über 4.000 Schritte mit der C-Verlet-
Referenz verglichen, einschließlich Szene und gespeicherter Runnerdatei.

Sprachvertrag 0.61.0 ergänzt `rk45IntegrateWithSteps` mit Anfangs-, Mindest-
und Höchstschritt. Die RK45-Pendelvorlage übernimmt damit die vollständigen
Schrittweitenoptionen der C-Referenz. Native Programme und das Analysemodul
prüfen die konfigurierte Integration; ungültige Optionen erzeugen
Quelldiagnosen. Numerische Berichte und komponentenweise Toleranzen fehlen noch
(Stand 0.61).

Sprachvertrag 0.60.0 bindet den adaptiven Dormand–Prince-Integrator als
`rk45Integrate` mit benannter Physim-Ableitungsfunktion, Vorwärts- und
Rückwärtsintegration, Toleranzen und Schrittbudget ein. Die Bibliotheksdiagnose
wird als Laufzeitfehler mit Quellposition gemeldet. Native Referenzen prüfen
Oszillator und zeitabhängige Gleichung sowie Dimensions-, Options-, Budget-
und Callbackfehler; das Analysemodul führt RK45 über die Runner-ABI aus.
`pendulum_rk45.phys` verwendet die Funktion als vollständiges Experiment.
Sechs Messkanäle und die Szene entsprechen über 4.000 Schritte der C-RK45-
Vorlage; ein Runnerlauf prüft die gespeicherten Messdaten.
Numerische Berichte und weitere RK45-Optionen bleiben offen (Stand 0.60).

Sprachvertrag 0.59.0 bindet den allgemeinen Euler- und RK4-Schritt mit
Ableitungs-Callbacks aus Physim-Funktionen an die gemeinsame C-Integrationsroutine.
Zustand und Ableitung sind besitzende `[Float64]`-Werte mit 1–32 Komponenten;
Zwischenwerte, Dimensionen und Eingaben werden geprüft. Die native Referenz
deckt den harmonischen Oszillator, eine zeitabhängige Gleichung, die obere
Dimensionsgrenze sowie Callback-Abbruch mit Speicherbilanz ab. Die Analysemodul-
Referenz führt die Callback-Brücke auch über die Runner-ABI aus. Beide Gesamtbuilds
und je 77/77 Sprach-CTest-Einträge bestehen unter Clang Debug und MSVC Release.
Der Referenzgenerator beschreibt auch die nach 0.53 ergänzten Sprachbindungen;
alle neun Dokumentations-CTest-Einträge bestehen unter Clang Debug.
Das vollständige Pendel-Experiment `pendulum_integrator.phys` nutzt `rk4Step`
im Modul. Der ABI-Test vergleicht alle sechs Kanäle und die Szene über 4.000
Schritte mit dem C-Pendel; Clang Debug und MSVC Release bestehen.

Sprachvertrag 0.58.0 bindet Bisektion und Goldener-Schnitt-Minimierung der
gemeinsamen Numerikbibliothek über benannte skalare Sprachfunktionen ein.
Der Compiler prüft `func(Float64) -> Float64`, erzeugt eine native Callback-Brücke
für eigenständige Programme und Hostmodule und meldet ungültige Intervalle,
Iterationsgrenzen sowie fehlende Konvergenz mit Quellposition.
Zu diesem Stand waren ODE-Funktionswerte und vollständige numerische Berichte
noch offen. Gesamtbuilds und vollständige Sprachtests bestehen unter
Clang Debug und MSVC Release (jeweils 77/77 CTest-Einträge einschließlich
zweier benötigter Testvorbereitungen).

Sprachvertrag 0.57.0 bindet den linearen Löser der gemeinsamen C-Bibliothek als
`linearSolve(coefficients, rhs, pivotTolerance)` an. Koeffizienten liegen
zeilenweise in einem `[Float64]`; der neue Ergebniswert besitzt seinen Speicher
selbst. Der native Referenztest prüft das C-3×3-System, ein 32×32-System,
unveränderte Eingaben und getrennte Diagnosen für Form, Singularität, Toleranz
und numerischen Bereich. Ein 33er-Grenzfall wird vor dem festen Löserpuffer
abgewiesen und räumt sein Eingabearray auf. Der native Test besteht unter Clang
Debug und MSVC Release.
Beide Gesamtbuilds und alle Sprachtests bestehen ebenfalls in beiden
Konfigurationen (jeweils 74/74).

Sprachvertrag 0.56.0 bindet die kubische räumliche Bézierkurve der gemeinsamen
C-Bibliothek als kopierbaren `Bezier3`-Wert ein. `position` und `tangent` werten
die Kurve aus; `splitLeft` und `splitRight` liefern umparametrisierte Teilkurven.
Die native Referenz prüft das kubische Polynom an 101 Stellen, Teilkurven und
Ableitungen sowie ungültige Parameter und numerischen Überlauf. Beide
Gesamtbuilds und alle Sprachtests bestehen unter Clang Debug und MSVC Release
(jeweils 74/74). Allgemeine Integrator-Callbacks bleiben als größere
Sprachbindung offen.
Der App-Editor zeigt `Bezier3` im Physim-Quelltext als Typ; die gezielte
Editoraufnahme mit der nativen Referenzdatei wurde visuell geprüft.

Sprachvertrag 0.55.0 erlaubt `_` als Verwerfmuster in Nutzdatenzweigen von
Enums und in `Optional.some(_)`. Andere Felder bleiben für Guard und Zweigkörper
gebunden; verworfene besitzende Werte werden nicht kopiert. Positive native
Referenzen mit Speicherbilanz und negative Gültigkeitsbereichsprüfungen bestehen.
Beide Gesamtbuilds und alle Sprachtests bestehen unter Clang Debug und MSVC
Release (jeweils 74/74). Weitere Fallmuster bleiben offen.

Sprachvertrag 0.54.0 ergänzt `min`, `max` und `clamp` für `Float64`.
Gleiche Werte behalten bei `min`/`max` den linken Operanden; `clamp` prüft die
Reihenfolge der Grenzen und meldet einen Laufzeitfehler mit Quellposition.
Die native Referenz prüft positive und benannte Aufrufe, einmalige
Argumentauswertung und vertauschte Grenzen. Nach der Versionsanhebung bestehen
der native Sprachtest, alle 63 Compiler-/Laufzeittests und zehn Sprach-App-Tests
jeweils unter Clang Debug und MSVC Release (74/74).

Die Physim-Pendelvorlage besitzt jetzt auch eine native RK4-Variante
(`examples/language/pendulum_rk4.phys`). Ihre vier Integrationsstufen stehen im
Sprachquelltext. Der ABI-Referenztest vergleicht alle sechs Messkanäle über
4.000 Schritte bei dt=0,005 s mit dem Standard-C-Pendel und prüft Reset,
Szenenposition und Integratormetadaten. MSVC Release und Clang Debug bestehen.

Sprachvertrag 0.47.0 ergänzt `0x`-/`0X`-Ganzzahlliterale für Farben, Bitmuster
und Seeds. Der Lexer weist fehlende Ziffern und ungültige Suffixe ab; die
Typprüfung setzt die vorzeichenbehafteten `Int64`-Grenzen auch in `Float64`-
Kontexten durch. Enum-Rohwerte, `switch`-Muster und generiertes C verwenden
denselben Zahlenwert; das RK4-Pendel prüft damit auch die Farben seiner
Szenenobjekte gegen die C-Referenz. Die vollständigen Sprachtests bestehen
unter MSVC Release (63/63) und Clang Debug (72/72).

Sprachvertrag 0.48.0 ergänzt bitweises `Int64`-Und/Oder/Xor/Not sowie Links-
und Rechtsschieben. Die Laufzeit verwendet definierte 64-Bit-Muster und
weist Schiebezahlen außerhalb 0–63 mit Quellposition ab. Verschachtelte
generische Typen bleiben trotz des `>>`-Tokens parsbar. Die native Referenz
prüft Grenzwerte, Operatorbindung und Auswertungsreihenfolge. Beide
Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (63/63) und
Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.49.0 ergänzt `&=`, `|=`, `^=`, `<<=` und `>>=` für veränderliche
`Int64`-Ziele, auch für Felder und Arrayelemente. Der Parser trennt in
Typkontexten `>=` und `>>=` wieder in schließende Generics und `=`, sodass
verschachtelte Typen ohne zusätzliche Leerzeichen initialisiert werden können.
Lexer-, Parser-, Typ- und native Referenzen decken die neuen Schreibweisen ab.
Beide Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (63/63)
und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.50.0 ergänzt Binär- (`0b`/`0B`) und Oktalliterale
(`0o`/`0O`). Lexer und Typprüfung weisen ungültige Ziffern sowie Werte
außerhalb des `Int64`-Bereichs ab. Die C-Ausgabe verwendet den geprüften
Zahlenwert auch für `Float64`-Kontexte.
Beide Gesamtbuilds und vollständigen Sprachtests bestehen unter MSVC Release
(63/63) und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.51.0 erlaubt `_` als Zifferntrenner innerhalb aller
Ganzzahlbasen sowie in Dezimalbruchteil und Exponent. Der Lexer weist
fehlplatzierte Trennzeichen ab; Typprüfung und native C-Ausgabe verarbeiten
die bereinigten Werte, einschließlich beider `Int64`-Grenzen.
Beide Gesamtbuilds und vollständigen Sprachtests bestehen unter MSVC Release
(63/63) und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows.

Sprachvertrag 0.52.0 ergänzt Instanz-, mutierende, statische und generische
Methoden für Enums. Die Typprüfung erkennt Kollisionen mit Fällen und
Raw-Value-Mitgliedern. Native Referenzen prüfen Nutzdaten mit Besitzsemantik,
Methoden auf importierten Enumtypen und die Ersetzung von `self`.
Beide Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (63/63)
und Clang Debug (72/72), einschließlich der neun Clang-App-Workflows. Die
nachträglich erweiterte Kopierprobe bestand in beiden nativen Tests erneut.

Sprachvertrag 0.53.0 erlaubt Unicode-15.0-Bezeichner in UTF-8-Quellen.
Generierte Tabellen definieren Start- und Fortsetzungszeichen unabhängig von
der Systemlocale. Compiler und native Tests prüfen Unicode-Namen für Typen,
Felder, Funktionen, Parameter und importierte Module; verschiedene
Normalisierungsformen bleiben unterscheidbar.
Beide Gesamtbuilds und alle Sprachtests bestehen unter MSVC Release (64/64)
und Clang Debug (73/73), einschließlich nativer Unicode-Modulimporte und
der neun Clang-App-Workflows.

Beide App-Editoren verwenden für Physim-Bezeichner jetzt dieselbe
Unicode-15.0-Erkennung wie der Compiler. Die Farbdarstellung trennt C- und
Physim-Schlüsselwörter, berücksichtigt Zifferntrenner und verschachtelte
Physim-Blockkommentare. Der Editor lädt zusätzlich Glyphen für kombinierende
Zeichen, Griechisch, Kyrillisch und Latin Extended Additional. Die Aufnahme
von `unicode_identifiers.phys` zeigt `π` und lateinische Unicode-Bezeichner
in Clang Debug und MSVC Release lesbar; zuvor wurde `π` als `?` angezeigt.
Der reproduzierbare `language_editor_unicode_preview`-Test, Workspace-Test
und vollständige Sprach-App-Workflow bestehen in beiden App-Builds.

Die Simulationsansicht wählt ihre hervorgehobene Aktion jetzt nach dem aktuellen
Zustand: Bei fehlendem Build `Build`, im Leerlauf `Neuer Lauf`, während eines
steuerbaren Laufs `Pause` und nach Pause `Fortsetzen`. Farben, Abstände und
Radien der dunklen Oberfläche liegen in `app/design_tokens.h`; die
[Zustandsmatrix](ui-states.md) beschreibt die Entscheidung.

Der neue Workspace-Einstieg ist begonnen: normaler Start ohne vorausgewähltes
Projekt, globale Helferleiste, echte SDL-Datei-/Ordnerauswahl, Öffnen beliebiger
Ordner, schreibgeschützte Textvorschau, zusätzliche Datei-/Ordnerpfade und ein
getrennter Projektmanager mit Zielordner, Name, Vorlage und beiden Sprachen.
Der lokale `workspace_workflow` prüft leeren Start, Ordner ohne Projekt,
Dateivorschau, Projekterzeugung und Rückkehr zum allgemeinen Workspace.
Dateibaum, mehrere editierbare Dokumente, Workspace-Persistenz und Prüfungen
für große oder ungültige Textdateien sind inzwischen ergänzt; siehe den aktuellen
Stand oben. Offen bleibt die manuelle Abnahme des nativen Auswahldialogs auf
Windows und Linux. Die [Designrichtung](design-direction.md) ist daher noch
nicht vollständig abgenommen.

Der erste durchgängige Sprachlernweg steht in
[Eigenes Experiment in der Physim-Sprache](language-tutorial.md). Die beiden
kopierbaren Quellen für gleichförmige Bewegung und Auswertung werden als native
Sprachmodule gebaut und mit demselben Referenztest wie das C-Tutorial geprüft:
201 Positionswerte, Reset, Szene und abgeleitete Geschwindigkeit. Die F1-Hilfe
enthält den Lernweg; ein Quellabgleich hält die Codeblöcke mit den getesteten
Dateien synchron. Der [Vakuumwurf-Lernpfad](projectile-tutorial.md) ergänzt
vollständige C- und Physim-Quellen für Experiment und Auswertung. Ein gemeinsamer
Referenztest prüft 201 Messungen, fünf Kanäle, Energie, Reset, Szene und
Geschwindigkeitsbericht; ein Quellabgleich hält alle vier Codeblöcke mit den
gebauten Dateien synchron. Der [Luftwiderstands-Lernpfad](projectile-drag-tutorial.md)
ergänzt die gleiche Vier-Quellen-Kette mit quadratischer Kraft und RK4. Sein
Paritätstest vergleicht C- und Sprachläufe mit 201 gespeicherten Zeilen,
Energieabnahme, Spur, Reset und beiden Geschwindigkeitsberichten. Die übrigen
Lernpfade und die vollständige LANG-007-Dokumentation bleiben offen.
Die App-Vorlage „Wurf mit Luftwiderstand“ verwendet bei Sprachwahl jetzt
`projectile_drag.phys` statt des Vakuummodells; der vollständige Sprachworkflow
ist als App-Test registriert.
Die Vorlage „Wurf mit Unsicherheit“ verwendet bei Sprachwahl jetzt
`uncertain_projectile.phys` mit denselben Anfangsverteilungen, Sensorparametern,
Seed-Strömen und 15 Kanälen wie die C-Variante. Der Runner-Paritätstest prüft
zwei 64-Bit-Seeds mit je 201 Zeilen und lässt C- sowie Sprachauswertung beide
Laufdateien verarbeiten. Die App-Workflows bestanden unter Clang und MSVC.

Die Sprache unterstützt nun Gleichheit und Ungleichheit zwischen zwei
optionalen Werten desselben Typs, sofern deren Inhalt `Bool`, `Int64`,
`Float64`, `String`, ein einfaches Enum oder ein Vektor ist. Fehlende Werte
vergleichen wie erwartet gleich; vorhandene Werte vergleichen ihren Inhalt.
Optionale Arrays, Strukturen und weitere SDK-Werte bleiben ausgeschlossen.
Die Sprachversion 0.24.0 verwendet jetzt besitzende, unveränderliche UTF-8-Strings.
`+` verkettet Strings; Variablen, Funktionen, Strukturen, Arrays und Optionals
halten geteilte Werte korrekt am Leben. Dynamisch erzeugte `Unit`- und
`Quantity`-Symbole bleiben bis zum Modulende gültig und werden nach Inhalt
zusammengefasst. Eine native Referenz und der interne Speichertest bestanden
unter MSVC und Clang; die Analyse-Referenz prüft zusätzlich dynamische Titel und
Einheitensymbole über die Modulgrenze. Weitere Stringfunktionen und der übrige LANG-001-Ausbau
bleiben offen. Sprachvertrag 0.25.0 ergänzt Unicode-Skalarindizes, bereichsweise
und geschrittete Stringausschnitte sowie `count`, `utf8ByteCount` und `isEmpty`.
Schreibzugriffe auf Stringindizes weist der Compiler ab; die positive Referenz
und negative Compilerfälle bestanden unter MSVC und Clang.
Sprachvertrag 0.26.0 ergänzt `String.contains` und `String.firstIndex(of:)` mit
Unicode-Skalarindizes. Die native Referenz und Fehlertypprüfung bestehen unter
MSVC und Clang; weitere Stringfunktionen und LANG-001-Arbeit bleiben offen.
Sprachvertrag 0.27.0 ergänzt `String.lastIndex(of:)`, `hasPrefix` und
`hasSuffix`. Leere Suchtexte und Unicode-Skalarindizes sind festgelegt;
die Sprachtests bestanden unter MSVC und Clang. LANG-001 bleibt weiter offen.
Sprachvertrag 0.28.0 ergänzt die unveränderliche Ersetzung von Stringteilen
einschließlich leerer Suchtexte an Unicode-Skalargrenzen. Native Referenz,
Speicherfehlerfälle und die Sprachtests bestanden unter MSVC und Clang. Der
übrige Sprachausbau bleibt offen.
Sprachvertrag 0.29.0 ergänzt `String.split(separator:)` mit besitzenden
`[String]`-Ergebnissen, erhaltenen Leerfeldern und Unicode-Skalarteilung.
Laufzeit-, Speicherfehler- und Compilerprüfungen bestanden unter MSVC und
Clang. Der übrige LANG-001-Ausbau bleibt offen.
Sprachvertrag 0.30.0 ergänzt `[String].joined(separator:)` für die umgekehrte
Textverarbeitung von `split`. Ergebnis und Eingaben besitzen unabhängige
Lebensdauer. Die Sprachtests bestanden unter MSVC und Clang; LANG-001 bleibt
darüber hinaus offen.
Sprachvertrag 0.31.0 ergänzt Stringkonvertierungen für `Bool`, `Int64` und
`Float64` sowie streng geprüfte Dezimaltext-Eingaben für beide Zahlentypen.
Die Prozesslocale war für Float64 noch maßgeblich. Native Fehlerfälle und die
Sprachtests bestanden unter MSVC und Clang; LANG-001 bleibt offen.
Sprachvertrag 0.32.0 verwendet für Float64-Literale und Dezimaltextumwandlung
einen festen C-Dezimalpunkt, auch wenn der Host eine andere Zahlenlocale setzt.
Stringkonvertierung und `print(Float64)` schreiben denselben Dezimalpunkt.
Der Localetest lief mit tatsächlich umgestellter Zahlenlocale unter MSVC und
Clang; beide vollständigen Builds, jeweils 60 Sprachtests und die native
Sprachsuite bestanden. Ein Linux-Build steht noch aus.
Sprachvertrag 0.33.0 ergänzt `array.lastIndex(of:)` für alle gleichheitsfähigen
Elementtypen. Die Suche läuft rückwärts, liefert einen optionalen Index und
wertet Empfänger sowie Suchwert je einmal aus. Positive und negative
Compilerfälle sowie native Referenzen mit verschachtelten und besitzenden
Werten bestanden unter MSVC und Clang. Nach vollständigem Neubau bestanden
61/61 Sprachtests unter MSVC und 70/70 unter Clang einschließlich neun
App-Workflows.
Sprachvertrag 0.34.0 ergänzt `array.reversed()` für beliebige Elementtypen.
Der neue Wert ist unabhängig vom Original. Kopierfehler bei jedem Teilschritt
sind im Speichertest abgedeckt; die nativen Beispiele mit Strings und
verschachtelten Arrays sowie positive und negative Compilerfälle bestanden
unter MSVC und Clang. Nach vollständigem Neubau bestanden 62/62 Sprachtests
unter MSVC und 71/71 unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.35.0 ergänzt das mutierende `array.reverse()` einschließlich
verschachtelter `var`-Zugriffspfade. Ein Fehler beim Kopieren eines Elements
ändert weder Empfänger noch bereits vorhandene Kopien. Gezielte Compiler-,
Laufzeit- und Speicherfehlerprüfungen bestanden unter MSVC und Clang. Nach
vollständigem Neubau bestanden 62/62 Sprachtests unter MSVC und 71/71 unter
Clang einschließlich neun App-Workflows.
Sprachvertrag 0.36.0 ergänzt `for` über Strings. Die Schleife liefert pro
Unicode-Skalar einen eigenen Stringwert, hält eine feste Eingabemomentaufnahme
und räumt Werte auch bei `break` und `continue` auf. Compiler-, Laufzeit- und
Allocator-Fehlerfälle bestanden unter MSVC und Clang. Nach vollständigem
Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72 unter Clang
einschließlich neun App-Workflows.
Sprachvertrag 0.37.0 ergänzt `String.reversed()` für Unicode-Skalare. Der
unveränderliche Eingabestring bleibt erhalten; das Ergebnis besitzt eigenen
Speicher. Gezielte Compiler-, Laufzeit- und Allokationsfehlerprüfungen sowie
native Experiment- und Analysepfade bestanden unter MSVC und Clang. Nach
vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72 unter
Clang einschließlich neun App-Workflows.
Sprachvertrag 0.38.0 erlaubt mehrere `static init`-Methoden pro Struktur.
Parameteranzahl und benannte Parametersätze wählen den Initialisierer; gleiche
Signaturen und mehrdeutige positionale Aufrufe sind Compilerfehler. Lokale,
importierte und generische Structs sowie ausdrückliche `.init`-Aufrufe wurden
unter MSVC und Clang geprüft. Nach vollständigem Neubau bestanden 63/63
Sprachtests unter MSVC und 72/72 unter Clang einschließlich neun App-Workflows.
Typbasierte Überladung war in dieser Fassung noch offen.
Sprachvertrag 0.39.0 ergänzt typbasierte `init`-Überladung für konkrete Structs.
Gleiche Parameternamen mit unterschiedlichen Typen sind möglich; exakte
Argumenttypen haben Vorrang vor der `Float64`-Verwendung eines Ganzzahlliterals.
Nicht typisierte `nil`-Argumente mit mehreren passenden Signaturen und
Argumente ohne passenden Typ erhalten eindeutige Compilermeldungen. Die
Referenz prüft benannte und positionale Aufrufe, geänderte Parameterreihenfolge,
importierte Typen und generische Formüberladung. Nach vollständigem Neubau
bestanden 63/63 Sprachtests unter MSVC und 72/72 unter Clang einschließlich
neun App-Workflows. Typbasierte Überladung generischer Initialisierer war in
dieser Fassung noch offen.
Sprachvertrag 0.40.0 löst `init`-Überladungen auch für generische Structs nach
Parametertypen auf. Der Checker leitet Typparameter pro passender Signatur ab,
bevor er die konkrete Struktur spezialisiert. `[T]`, `T?` und verschachtelte
generische Struct-Typen können so gegen `T` unterschieden werden; explizite
und kontextuelle Typargumente sowie importierte generische Structs sind in
der Referenz geprüft. Doppelte
Templatesignaturen, mehrdeutige Aufrufe und nach Spezialisierung kollidierende
Signaturen werden abgewiesen. Kontextabhängige Auflösung untypisierter
Argumentausdrücke und eigene Methodentypparameter für `init` waren in dieser
Fassung noch offen.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.41.0 nutzt den Parametertyp jetzt auch für `nil` und
Arrayliterale bei der Wahl einer `init`-Überladung. Ein einzelner optionaler
beziehungsweise Array-Kandidat kann damit ausgewählt werden; mehrere passende
Kandidaten bleiben mehrdeutig. Generische Konstruktoren leiten Typen aus
nichtleeren Arrayliteralen und späteren Argumenten ab; ein bereits bekannter
Ergebnistyp gibt den Parametern den benötigten Kontext. Gezielte Compiler- und
Laufzeitreferenzen bestehen unter MSVC und Clang. Nach vollständigem Neubau
bestanden 63/63 Sprachtests unter MSVC und 72/72 unter Clang einschließlich
neun App-Workflows. Die Typableitung weiterer Ausdrucksformen und eigene
Typparameter für `init` bleiben offen.
Sprachvertrag 0.42.0 berücksichtigt `Optional.some(...)` bei der Wahl einer
`init`-Überladung. Sein Inhalt wird gegen den optionalen Parametertyp geprüft;
die Referenz umfasst skalare Literale, berechnete Werte, benannte Argumente,
generische Typableitung aus Optional-Werten und Arrayliteralen sowie
verschachtelte Arrayinhalte mit Ergebniskontext. Mehrdeutige und typfremde
Fälle bleiben Compilerfehler.
Eigene Typparameter für `init` und weitere kontextabhängige Ausdrucksformen
stehen weiterhin aus.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.43.0 leitet den skalaren generischen Typ eines `init`-Aufrufs
unabhängig von der Argumentreihenfolge aus einem späteren `Float64`-Wert ab.
Frühere Ganzzahlliterale erhalten diesen Kontext; typisierte `Int64`-Variablen
bleiben unverändert. Compiler- und Laufzeitreferenzen prüfen beide Fälle.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.44.0 reicht bei generischen `init`-Aufrufen den während der
Überladungswahl bestimmten Typ an die vollständige Argumentprüfung weiter.
`T?`, `[T]` und verschachtelte generische Structs erhalten dadurch den Kontext
eines späteren Arguments, auch bei umgekehrter Reihenfolge benannter Argumente.
Ein typisierter `Int64`-Wert bleibt bei verlangtem `Float64` ein Compilerfehler.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.45.0 erlaubt eigene Typparameter und Constraints auf `init`.
Generische Initialisierer funktionieren bei konkreten, generischen und
importierten Structs, mit abgeleiteten oder expliziten Methodentypargumenten
und zusammen mit konkreten sowie unterschiedlich geformten generischen
Überladungen. Gleiche Signaturen nach Umbenennung der Methodentypparameter,
unpassende Constraints und nicht ableitbare Typargumente werden abgewiesen.
Explizite Methodentypargumente wählen bei überladenen Initialisierern eine
generische Signatur.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Sprachvertrag 0.46.0 bestimmt generische Funktionstypparameter vor der
vollständigen Argumentprüfung, sofern alle Typen aus den Ausdrücken ableitbar
sind. Dadurch geben spätere `Float64`-Argumente früheren Ganzzahlliteralen
auch in `T?` und `[T]` den richtigen Kontext. Die Referenz umfasst freie,
Instanz- und statische Methoden; typisierte `Int64`-Werte bleiben unverändert.
Nach vollständigem Neubau bestanden 63/63 Sprachtests unter MSVC und 72/72
unter Clang einschließlich neun App-Workflows.
Die Sprachversion `0.1.0` ist jetzt getrennt von App-Version und SDK-ABI
definiert und erscheint in `physimc --version` sowie im generierten C-Kopf.
Compilerprüfung, natives Optional-Beispiel und vollständige native Sprachsuite
bestanden lokal unter MSVC Debug.

Die Sprache kann jetzt aus berechneten `[Float64]`-Werten eigene `Series`
erzeugen und Reihen mit gemeinsamem Raster koppeln. Die neue C-API prüft
Einheiten, endliche Werte, Alignment und Scratch-Budget vor der Übernahme.
`analysis_monte_carlo.phys` erzeugt aus einem expliziten Seed 1024 Ziehungen
und daraus ohne Eingabedatei Diagramm, Histogramm, Tabelle und CSV. Der Analyse-Runner-Test
vergleicht alle Ziehungen und Kennzahlen mit der gemeinsamen C-Messbibliothek.
Damit ist ein vollständiger kleiner Monte-Carlo-Auswertungspfad in der Sprache
vorhanden. `analysis_batch_endpoints.phys` fasst außerdem die Endwerte von C-
und Sprachläufen in einer gemeinsamen Datenreihe zusammen; der Runner-Test
prüft beide Eingabeformate, Plot, Histogramm und Tabelle. Die übergeordnete
LANG-005-Abnahme bleibt wegen weiterer Batch- und Auswertungsfunktionen offen.
Die betroffenen Serien- und Analyseprüfungen bestanden unter MSVC Debug,
Clang/Ninja und MSVC AddressSanitizer.

Das C17-Backend erzeugt jetzt `#line`-Zuordnungen für Funktionen,
Strukturfelder und Anweisungen. Ein gezielt ausgelöster nativer Compilerfehler
meldet die ursprüngliche `.phys`-Datei; die native Sprachsuite prüft dies mit
MSVC und Clang. Das erste Mehrdatei-Modulsystem mit qualifizierten Namen,
transitiven Imports und Dateidiagnosen ist implementiert und unter beiden
Compilern geprüft. `switch` unterstützt mehrere Enumfälle pro Zweig und prüft
Duplikate sowie vollständige Abdeckung. Erweiterte Modulauflösung und ein durchgängiger Debugger
bleiben offen.

Freie generische Funktionen werden für konkrete Aufruftypen spezialisiert;
Imports, Arrays, optionale und nominale Werte sowie rekursive Aufrufe sind
damit ausführbar. Generische Instanzmethoden, mutierende und statische Methoden
sind ebenfalls ausführbar. Das Beispiel und negative Compilerfälle bestehen
unter MSVC und Clang. Explizite Typargumente für freie Funktionen und Methoden
sind ebenfalls möglich. Generische Strukturen werden aus Konstruktorfeldern,
Ergebniskontext oder expliziten Typargumenten spezialisiert; Felder, Methoden,
verschachtelte Werte und optionale Rekursion sind nativ ausführbar. Die
Sprachversion wurde dafür auf 0.6.0 angehoben. Eingebaute Constraints
`Numeric`, `Scalar`, `Equatable` und `Vector` werden bei Spezialisierungen
geprüft. Eigene Protokolle und die vollständige LANG-006-Abnahme bleiben offen.

## Historische Prüfungen

Die [historischen lokalen Prüfungen](status-history.md) sind separat erhalten.
Sie ersetzen keine aktuelle Abnahme des gesamten Projektplans.


Lineare Systeme (§7.2 / PP-0360) verwenden jetzt separate Exponenten für
rechte Seiten und Teillösungen. Belegte Zwischenüberläufe bei endlichen
Lösungen sind geschlossen; C und Physim werden gegen unabhängige rationale
Lösungen und Residuen geprüft. [Verträge und Grenzen](numerics.md) bleiben
ausdrücklich begrenzt; der vollständige Projektplan ist weiterhin offen.


Ein erster nativer macOS-Accessibility-Baum für sichtbare einfache Schaltflächen
und statische Texte ergänzt die bestehende Tastaturführung. Kopierte Snapshots,
stabile Kennungen und sichere native Press-Aktionen werden tatsächlich geprüft.
[Umfang und verbleibende Arbeit](accessibility.md) halten vollständige VoiceOver-,
Windows-Anbindung und weitere Controltypen ausdrücklich offen. Linux bietet
nun einen AT-SPI-Einstieg für denselben Controlumfang mit externem Clientnachweis
unter Debian/X11; Wayland und praktische Orca-Bedienung bleiben offen.


Nullstellensuche und Optimierungsgrundlagen (§7.2 / PP-0361) besitzen nun
unabhängig geprüfte binäre Abbruchbedingungen für Bisection und Golden Section.
C-/Physim-Berichte, extrem kleine/große Werte, Callback-Fehler und Stagnation
werden geprüft. Modulblock-Closures übernehmen lokale Werte jetzt als besitzende
Snapshots; echte Modulglobals behalten ihren globalen Speicher.
[Methoden und Grenzen](numerics.md) begrenzen diesen Implementierungsnachweis;
der gesamte Projektplan bleibt weiterhin offen.


Vektoren, Matrizen und Transformationen (§7.2 / PP-0357) sind den gemeinsamen
C-/Physim-APIs und unabhängigen Referenzen zugeordnet. Punkte/Richtungen erhalten
exakte binäre Zeilensummen und projektive Quotienten; Normalen werden direkt aus
dem skalierten transponierten System berechnet. [Konventionen und Grenzen](math.md)
begrenzen diesen Nachweis; der vollständige Projektplan bleibt offen.


Run-Dateien besitzen nun eine zusätzliche opake Streaming-API mit explizitem
Store, geprüften Generationen und kopierten Statuswerten. Experiment-Runner und
Importvalidierung benutzen sie; Abbruch erhält unvollständige Präfixdaten.
[Lebensdauer und offene Migration](run-streams.md) begrenzen diesen Fortschritt;
Legacy-Dateideskriptoren und die allgemeine Handle-Forderung bleiben offen.


`ps_close` und Physim `isClose` verwenden jetzt explizite symmetrische Toleranzen
mit exakten Grenzentscheidungen für binäre Double-Eingaben. Subnormalwerte und
überlaufende Differenzen/Produkte verändern die Entscheidung nicht. Eine
unabhängige rationale Gegenprobe prüft C und Physim. Die gesamte Modell- und
Algorithmusabnahme wird daraus nicht abgeleitet; [Verträge](math.md).


Kubische Bézierkurven erhalten jetzt Position, analytische Tangente und
Unterteilungskontrollpunkte aus exakten binären Polynomen mit abschließender
Rundung. Große benachbarte Punkte und Subnormalwerte verlieren dadurch keine
kleinen Tangenten in Zwischenstufen. C und Physim teilen den Vertrag;
`controlPoint(index)` liefert geprüfte kopierte Sprachwerte. Die unabhängige
rationale Gegenprobe ergänzt die bestehenden Interpolations-/Kurventests.
[Einheiten, Grenzen und Methoden](math.md).


Einheitenkonvertierung erhält nun identische Eingabewerte bitgenau und rundet
allgemeine binäre Skalenverhältnisse einmal abschließend. Core und Quantity
teilen den Vertrag; Physim bewahrt die ursprünglichen Invalid-/Numeric-Codes
in Runtime-Diagnosen. Unabhängige rationale C-/Physim-Proben, SI-Basisdefinitionen,
Anzeigeeinheiten und Exporte sind konkret geprüft. Die neue [SI-Anleitung](units.md)
ist im Offline-Handbuch erreichbar; globale SI-/Metadatenabnahme bleibt offen.


Quantity-Werte lassen sich nun mit `ps_channel_sample_quantity` und Physim
`Channel.sampleQuantity` geprüft und atomar in SI-Kanäle übernehmen. Dimensionen,
Konvertierungsbereich, Besitzer und Samplingphase werden abgeglichen. Sensorwerte
und Unsicherheit aus Zentimetereinheiten werden in echten C-/Physim-Läufen als
Meterwerte persistiert. Die Sensor-Wurfbeispiele nutzen den Pfad; Statusmasken
bleiben für Platzhalter erforderlich. [Verträge](measurement.md).


Kugel- und Boxträgheiten runden jetzt die analytischen rationalen Formeln einmal
abschließend. Kinetische Energie vermeidet überlaufende Geschwindigkeitsquadrate
und erhält Ausgaben bei Bereichsfehlern. Unabhängige C-/Physim-Referenzen prüfen
Hauptachsen; Decimal-Referenzen prüfen gedrehte Körper mit dokumentierter
Double-Rotationsgenauigkeit. Die gesamte Starrkörperdynamik bleibt separat
abzunehmen.
[Verträge](mechanics.md).


Allgemeine konvexe Polyeder besitzen nun diskrete Paar-, Kugel- und
Ebenenkontakte sowie konservative Hüllgrenzen. Geschlossene Dreiecksnetze
werden auf Konvexität, Orientierung und Grenzen geprüft; Fehler erhalten
Ausgaben. C und Physim teilen die Kontaktprüfung, Hauptträgheiten sind für
eigene Formen explizit vorgebbar. Unabhängige Trennachsen und Oberflächenzeugen
prüfen gedrehte und enthaltene Netze. Einzelkontakte ersetzen keine
Ruhemanifolds; allgemeine zeitabhängige Rotation und automatische Kontaktverwaltung bleiben offen.
[Beispiele und Grenzen](mechanics.md).


Lineare konvexe Sweeps ergänzen Paar-, Kugel- und Ebenenkontakte sowie Hüllen
des gesamten Bewegungswegs. Flächen-/Kanten-Zeitintervalle beziehungsweise
Flächen-/Kanten-/Vertexeintritt erfassen Durchtunneln bei fester Orientierung.
Die C-/Physim-Beispiele bewegen eine Kugel zum Ereignis, lösen den elastischen
Impuls und integrieren die Restzeit gegen unabhängige Position-/Energiereferenzen.
Allgemeine zeitabhängige Rotation/Kräfte und Mehrkörper-Ereignissteuerung bleiben offen.
[Verträge und Beispiele](mechanics.md).


Konservative CCD unterstützt nun explizite Weltachsenrotation einschließlich
voller Drehungen sowie quadratische Translation. C und Physim teilen
`RigidMotion`, Genauigkeits-/Iterationsvorgaben, kopierte Pfadlagen und
konservative Bewegungshüllen. Analytische Erstkontakte prüfen rotierende
Netz-/Kugel-/Ebenenfälle und gekrümmte Wege mit klaren Endlagen. Ausgeschöpfte
Budgets melden einen offenen Fall mit erhaltenen Ausgaben; allgemeine
zeitabhängige Kräfte/Drehungen und Mehrkörpersteuerung bleiben offen.
[Verträge](mechanics.md).


Ein begrenzter kontinuierlicher Mehrkörper-Controller integriert jetzt einen
vollständigen Kick-Drift-Schritt mit Kugeln, Boxen, Ebenen und konvexen Netzen.
Nach jedem Ereignis berechnet er die verbleibende Bewegung neu; nahe Kontakte
werden gemeinsam gelöst. C erhält atomische Körper-/Berichtsausgaben, Physim
besitzt kopierbare Modelle und Ergebnis-Snapshots. Zwei tatsächliche Stöße in
einem Schritt und 181 unabhängige rationale Szenarien prüfen den Ablauf.
Zeitabhängige Kraft-/Rotationspfade und vollständige Produktabnahme bleiben offen.
[Verträge, numerischer Kontaktabstand und Beispiele](mechanics.md).


Native Checkboxen ergänzen den macOS-/Linux-Barrierefreiheitseinstieg mit
booleschen Werten, Änderungsmitteilungen und einer Umschaltaktion. Die vorhandenen
UI-Checkboxen und die optisch unbeschriftete Bibliotheksauswahl erhalten native
Namen. Tatsächliche App-/AppKit-/AT-SPI-Prüfungen belegen Wertänderungen und
Ablehnung deaktivierter, verborgener und veralteter Ziele. Editoren, Auswahlfelder,
Fokusdienste, Windows/UIA und praktische Screenreader-Abnahme bleiben offen.
[Umfang und Grenzen](accessibility.md).


Native Optionsgruppen machen Darstellung und beide Schriftgrößengruppen auf
macOS und Linux erreichbar. Rollen, Eltern-/Kindbeziehungen, Auswahlwerte und
native Select-Aktionen sind an gezeichneten Controls und am tatsächlichen
Einstellungsentwurf geprüft. Gleichlautende Optionen besitzen unterschiedliche
Gruppen; erneute Auswahl schaltet den Wert nicht ab. Vollständige Fokus-,
Dropdown-, Editor- und praktische Screenreader-Abnahme bleiben offen.
[Verträge und Grenzen](accessibility.md).
