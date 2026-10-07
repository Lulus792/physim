# Historische lokale Prüfungen

Diese Aufzeichnungen gehören zum früheren Entwicklungsstand. Aktuelle Aussagen
und offene Ziele stehen im [Umsetzungsstand](status.md).

## Durchgeführte lokale Prüfungen

- Optionale `switch`-Muster (2026-09-23): `nil`, `Optional.some` und
  `Optional.some(name)` unterscheiden die beiden Varianten und binden den
  Inhalt als unabhängigen Wert. Guards, vollständige Abdeckung, verschachtelte
  Optionals, generische Rückgaben und besitzende Arrays sind geprüft;
  Sprachvertrag 0.16.0.

- Skalare `switch`-Muster (2026-09-23): `Bool`, `Int64` und `String` ergänzen
  Enumfälle. Feste Literale, mehrere Werte pro Zweig, Guards, doppelte Muster,
  vollständige Bool-Abdeckung und erforderliches `default` für offene
  Wertebereiche sind geprüft; Sprachvertrag 0.15.0.

- Eigene Struct-Initialisierer (2026-09-23): `static func init(...) -> Typ`
  steuert `Typ(...)`; im Initialisierer bleibt der direkte Feldkonstruktor
  verfügbar. Parameternamen, Rückgabetyp und ungültige Deklarationen werden
  geprüft. Generische Typargumente werden aus Initialisiererparametern oder
  explizit bestimmt; importierte Typen und besitzende Rückgaben laufen nativ.
  Sprachvertrag 0.14.0.

- Struktur-Feldvorgaben (2026-09-23): `let`- und `var`-Felder können einen
  typgeprüften Ausdruck als Vorgabe tragen. Der erzeugte Konstruktor akzeptiert
  ausgelassene Vorgabefelder, wertet sie bei jedem Aufruf im Modulkontext aus
  und verwaltet auch besitzende Werte. Generische Spezialisierungen und
  Laufzeit-/Compilerfehler sind geprüft; Sprachvertrag 0.13.0.

- Numerische Sprachbibliothek (2026-09-23): Trigonometrische Umkehrfunktionen,
  `atan2`, Exponentialfunktion, Logarithmen, Potenz, skalierte zweidimensionale
  Länge und drei Rundungsfunktionen erweitern die `Float64`-Operationen.
  Ungültige Definitionsbereiche und nicht endliche Ergebnisse erhalten
  Laufzeitdiagnosen mit Quellposition; Sprachvertrag 0.12.0.

- Enum-Raw-Values (2026-09-23): Explizite `Int64`-Werte und fortlaufende
  implizite Werte sind geprüft; doppelte Werte und Überläufe liefern
  Quelldiagnosen. `rawValue` sowie `Type.fromRawValue` funktionieren auch
  bei importierten Enums und an beiden `Int64`-Grenzen. Sprachvertrag 0.11.0.

- Bewachte Enum-Auswahl (2026-09-23): `case Muster if Bool-Ausdruck` prüft den
  Guard nach der Nutzdatenbindung. Ein falscher Guard setzt die Auswahl beim
  nächsten Zweig fort; unbewachte Fälle oder `default` sichern die vollständige
  Abdeckung. `break` und ein `continue` der äußeren Schleife funktionieren auch
  bei besitzenden Bindungen. Sprachvertrag 0.10.0.

- Enum-Nutzdaten (2026-09-23): Fälle tragen benannte typisierte
  Felder. Konstruktoren unterstützen positionsgebundene und benannte Argumente;
  `switch` bindet Felder im jeweiligen Zweig oder prüft nur den Falltag.
  Importierte Enums, Arrays, optionale Werte und Strukturfelder sind nativ
  ausführbar. Sprachvertrag 0.9.0; besitzende Arrays, optionale und
  verschachtelte Strukturwerte sowie über `Optional` rekursive Enums werden
  mit fallabhängigen Kopier- und Freigabefunktionen ausgeführt. Strukturelle
  Gleichheit vergleicht Enums, Strukturen, Arrays und Optionals rekursiv,
  sofern alle enthaltenen Typen vergleichbar sind.

- Bedingte optionale Bindungen (2026-09-20): `if let`, `if var`, `while let`
  und `while var` binden eine unabhängige Inhaltskopie im erfolgreichen Block.
  Der Initialisierer sieht den äußeren Gültigkeitsbereich und wird einmal
  je Prüfung ausgewertet; eine Typannotation beschreibt den Inhaltstyp.
  `else if let`, Namensverdeckung und genau eine Ebene bei verschachtelten
  Optionals sind unterstützt. Gebundene Besitzer werden beim normalen Ende,
  `return`, `break`, `continue` und Laufzeitfehler aufgeräumt. Mutation einer
  lokalen `var`-Kopie verändert die Quelle nicht. `optional_bindings.phys`
  prüft diese Regeln, 20000 verkettete Werte, 1001 Bedingungsauswertungen,
  verschachtelte Schleifen-/Switch-Abbrüche und 300000 Durchläufe. Die native
  Suite prüft zusätzlich die Speicherbilanz nach normalem Ende und zwei
  Fehlerprogrammen im Initialisierer beziehungsweise im gebundenen Block.
  Sensorwurf und Sensoranalyse verwenden die Bindung für Messposition und
  Standardabweichung; ihre C-/Berichtsreferenzen bleiben unverändert.
  Alle 36 Clang-Prüfungen bestanden: 32 Sprachprüfungen einschließlich acht
  App-Abläufen sowie vier Dokumentationsprüfungen. Nachweis:
  `build-language-binding-evidence/clang-suite.log`; visuell geprüfte
  `simulation.bmp` und `analysis.bmp` liegen im selben Ordner.

- Optionale Werte im Sprachkern (2026-09-20): `T?`, `nil`, `Optional.some(value)`,
  `hasValue`, `unwrap()` und `valueOr(fallback)` sind statisch geprüft und im
  C17-Backend umgesetzt. Konstruktion ist ausdrücklich; numerische Literale
  erhalten den erwarteten Inhaltstyp. Verschachtelte Optionals behalten jede
  Ebene, und Vergleiche mit `nil` funktionieren in beiden Reihenfolgen.
  Optionale Inhalte verwenden die gemeinsame Besitzverwaltung und das
  64-MiB-Budget. `nil` allokiert nicht; Kopien teilen unveränderlichen Speicher,
  ausgepackte Werte bleiben unabhängig. Rekursive optionale Strukturfelder
  werden wie Arraywerte iterativ freigegeben. `optional_values.phys` prüft
  Skalare, Arrays, Strukturen, Enums, Matrizen und besitzende Solverergebnisse,
  Argumentreihenfolge, 20000 verkettete Werte sowie 300000 Schleifendurchläufe
  mit `continue`/`break`. Die native Suite kontrolliert nach erfolgreichem Ende
  und fünf Fehlerprogrammen ausdrücklich null verbleibende Allokationsbytes,
  auch nach ausgeschöpftem Speicherbudget und Fehlern in Ersatz-/Konstruktorargumenten.
  Der Sensorwurf speichert seine letzte gültige Messposition als `Vec3?`;
  die Analyse stellt eine bei zu wenigen Messpunkten fehlende Standardabweichung
  als `Quantity?` dar. Sensorreferenz, Berichte und Reset nach fehlgeschlagenem
  `unwrap` werden geprüft. Bedingte Bindungen werden oben ergänzt. Implizite
  Umwandlungen zwischen `T` und `T?` sind nicht implementiert; `valueOr` wertet
  den Ersatz immer aus.
  Alle 35 Clang-Prüfungen bestanden: 31 Sprachprüfungen einschließlich acht
  App-Abläufen sowie vier Dokumentationsprüfungen. Nachweis:
  `build-language-optional-evidence/clang-suite.log`; visuell geprüfte
  `simulation.bmp` und `analysis.bmp` liegen im selben Ordner.
  Acht Prüfungen bestanden auch unter MSVC/AddressSanitizer: optionale Werte,
  Sensorreferenz, Berichte, Parser, Compiler-Mutationen, native Sprachsuite,
  Array-Besitzbasis und Fehler-Aufräumlogik. Nachweis: `asan-tests.log`.
  Das installierte und verschobene SDK bestand mit 14 Sprachprogrammen, neun
  Sprachexperimenten, C-/Physim-Auswertungen und eigenständigen Headern:
  `build-clang-ninja/SDK Test ä c4c32bc68d/verification.log`.

- Matrizen und Koordinatensysteme in der Sprache (2026-09-20): `Mat3` und `Mat4`
  binden die gemeinsame Mathematikbibliothek ein: Spaltenkonstruktoren,
  Einheitsmatrizen, Elementzugriff, Produkt, Transposition, Inversion,
  Matrix-Vektor-Produkte, Translation/Rotation/Skalierung und TRS sowie getrennte
  Punkt-, Richtungs- und Normalentransformationen. Matrixwerte funktionieren in
  Strukturfeldern, verschachtelten Arrays, eigenen Methoden und Rückgaben.
  Ungültige Indizes, nichtaffine Richtungs-/Normalentransformationen, Singularität,
  ungültige Pivot-Toleranzen und nichtendliche Ergebnisse erhalten Quelldiagnosen.
  `coordinate_frames.phys` prüft analytische Werte, projektive Division,
  Normalenorthogonalität, gedrehte Trägheit und Energie, Wertkopien und 200
  weitere Koordinatensysteme. Die Vorlage **Starrer Körper · Physim-Sprache**
  berechnet ihren Kraftangriffspunkt jetzt über `Mat4.trs`; die bestehende
  C-Referenz prüft weiterhin alle 15 Kanäle über 1002 Zustände, Szene, Reset,
  Instanztrennung und 201 gespeicherte Runner-Zeilen. 13 zusätzliche native
  Fehlerprogramme prüfen die Matrixdiagnosen an der ursprünglichen Quelle.
  Alle 35 Clang-Prüfungen bestanden: 30 Sprachprüfungen einschließlich acht
  App-Abläufen, native Mathematikreferenz und vier Dokumentationsprüfungen.
  Direkte Matrixarrays mit Kopie, Ersetzung, Anhängen, Entfernen und Slicing
  bestanden zusätzlich nach Ergänzung des Beispiels. Nachweise:
  `build-language-matrix-evidence/clang-suite.log`, `clang-matrix-arrays.log`
  sowie visuell geprüfte `simulation.bmp` und `analysis.bmp` im selben Ordner.
  Sechs Prüfungen bestanden unter MSVC/AddressSanitizer: Matrixwerte,
  Körperreferenz, native Sprachsuite, Compiler-Mutationstests, Array-Besitzbasis
  und native Mathematik. Nachweis: `asan-tests.log` im selben Evidenzordner.
  Das installierte und verschobene SDK bestand mit 13 Sprachprogrammen, neun
  Sprachexperimenten, Auswertungen in C und Physim sowie eigenständigen Headern:
  `build-clang-ninja/SDK Test ä d4dc6ddf8c/verification.log`.

- Lineare Kugelkontakte und Kollisionskandidaten in der Sprache (2026-09-20):
  `Sweep`, `Aabb` und `CollisionPair` binden die gemeinsame C-Kollisionsbibliothek
  ein. Kugel–Kugel- und Kugel–Ebene-Abfragen liefern ersten Kontakt und
  Bewegungsanteil; `contacts()` verbindet das Ergebnis mit dem Kontaktsolver.
  Statische und überstrichene Hüllquader liefern sortierte Kandidatenpaare für
  bis zu 1024 Körper. Ergebnisarrays besitzen automatisch verwalteten Speicher
  im Sprachbudget und werden ohne zweiten großen Zwischenpuffer aufgebaut.
  `sweeps.phys` prüft Randkontakte, Anfangsüberlappung, verfehlte Kontakte,
  relative Bewegung, große Koordinaten, Wertkopien und alle 523776 Paare bei
  ausgeschöpfter Kapazität. `fast_sphere.phys` und die App-Vorlage
  **Schnelle Kugel · Physim-Sprache** berechnen wiederholte elastische Wandstöße
  mit 80 m/s, Kontaktzeitpunkt und Restbewegung. Sieben Kanäle stimmen über
  2084 Zustände bei vier Schrittweiten mit der C-Referenz überein; zusätzlich
  werden analytische Bahn und Energie geprüft. Ein überschrittenes Budget von
  16 Stößen verwirft den berechneten Schritt mit Quelldiagnose. Tests prüfen
  außerdem Instanztrennung, Reset, Szene, 101 Runner-Zeilen, sieben wiederholte
  Fehlerfälle und Speicherbilanz bei Allokationsfehlern.
  Alle 34 Clang-Prüfungen bestanden: 29 Sprachprüfungen einschließlich acht
  App-Abläufen, native Kandidatensuche und vier Dokumentationsprüfungen.
  Der native Kugel-Sweep-Test bestand separat. Nachweise:
  `build-language-collision-evidence/clang-suite.log`, `clang-native-sweep.log`
  sowie visuell geprüfte `simulation.bmp` und `analysis.bmp` im selben Ordner.
  Sieben Prüfungen bestanden auch unter MSVC/AddressSanitizer: Sweep-Werte
  und Modulreferenz, Körpergruppenregression, native Sprachsuite,
  Compiler-Mutationstests sowie native Kandidatensuche und Kugel-Sweeps.
  Nachweis: `asan-tests.log` im selben Evidenzordner.
  Das installierte, verschobene SDK bestand mit zwölf Sprachprogrammen und neun
  Sprachexperimenten einschließlich Auswertung des Wandstoßmodells in C und Physim:
  `build-clang-ninja/SDK Test ä ea069165d2/verification.log`.
  Beschleunigte Bahnen, Quader-CCD und eine allgemeine automatische
  Mehrkörper-Ereignissteuerung bleiben offen.

- Gemeinsamer Körpergruppen-Solver in der Sprache (2026-09-20):
  `ContactConstraint`, `JointConstraint` und `ConstraintResult` binden den
  gemeinsamen Kontakt-/Distanzgelenksolver über Körperarrays ein. Indizes,
  Weltanker, Geometrie, alle drei Kapazitäten und Ergebniszugriffe werden geprüft.
  Der Ergebniswert besitzt automatisch verwalteten Speicher im 64-MiB-Sprachbudget;
  Kopien, Array-/Strukturfelder, Rückgaben und Ersetzungen verwenden die gemeinsame
  Aufräumlogik. Körperarrays aus `bodies()` sind unabhängig. Compilerdeskriptoren
  unterstützen jetzt auch besitzende Bibliothekswerte und eine 64-Bit-Typmaske.
  `constraint_graph.phys` prüft gemeinsame Abstützung, Rückprall, unvollständige
  Konvergenz, widersprüchliche Gelenke, 2000 Ergebnisersetzungen und gleichzeitig
  128 Körper, 512 Kontakte und 256 Gelenke. `coupled_bodies.phys` und die App-Vorlage
  **Gekoppelte Körper · Physim-Sprache** verbinden zwei Kugeln über ein Gelenk und
  lösen Bodenkontakte gemeinsam. Referenzen vergleichen alle 16 Messkanäle über
  vier Sekunden bei 1 und 2 ms sowie Szene, Instanztrennung, Reset und 801
  gespeicherte Runner-Zeilen. 13 Fehlerfälle prüfen Diagnosen und Reset;
  Allokationsfehler beim Ergebnis und beim Körperarray erhalten bestehende Werte
  und die Speicherbilanz. Alle 31 Clang-Prüfungen bestanden: 26 Sprachprüfungen
  einschließlich sieben App-Abläufen, C-Graphreferenz und vier Dokumentationsprüfungen.
  Nachweise: `build-language-graph-evidence/clang-language-tests.log`,
  `final-graph-test.log`, visuell geprüfte `simulation.bmp` und `analysis.bmp`.
  Sieben betroffene Prüfungen bestanden auch unter MSVC/AddressSanitizer:
  Graphwerte/-referenz, Kontakt-/Gelenkregression, native Sprachsuite,
  Compiler-Mutationstests und Array-Besitzbasis. Nachweis:
  `asan-language-tests.log` im selben Evidenzordner.
  Das installierte, verschobene SDK bestand mit elf Sprachprogrammen und acht
  Sprachexperimenten einschließlich Auswertung der Körpergruppe in C und Physim:
  `build-clang-ninja/SDK Test ä 9c34da98e1/verification.log`.
  Automatische Kontaktverwaltung, Warmstart, Quader-CCD und weitere Gelenkarten
  bleiben offen; ein erfolgreiches Iterationsbudget garantiert keine Konvergenz.

- Distanzgelenke in der Sprache (2026-09-20): `DistanceJoint` und `JointResult`
  verwenden die gemeinsame Mechanikbibliothek mit lokalen Ankern, Driftstabilisierung,
  unabhängigen Körperergebnissen, Impuls und Längen-/Geschwindigkeitsfehler.
  `ps_distance_joint_validate` teilt die Konfigurationsprüfung zwischen C-Solver
  und Sprachkonstruktor. `distance_joints.phys` prüft Impulsbilanz, gedrehte
  außermittige Anker, transformierte statische Körper, Array-/Strukturkopien,
  Stabilisierung und Schrittweitenverfeinerung. `joint_pendulum.phys` und die
  App-Vorlage **Gelenkpendel · Physim-Sprache** zeigen einen aufgehängten Quader
  mit 14 Messkanälen. C-Referenzen prüfen alle Kanalwerte über vier Sekunden
  bei 4 und 2 ms, Szenengeometrie/Orientierung, unabhängige Instanzen, Reset,
  abnehmenden Längendrift und 401 gespeicherte Runner-Zeilen. Sieben Fehlerfälle
  prüfen ungültige Konfiguration, singuläre Anker, Schrittweiten, Überlauf,
  Quelldiagnosen und Wiederherstellung durch Reset. 29 Clang-Prüfungen bestanden:
  23 Sprachprüfungen einschließlich sechs App-Abläufen, zwei Mechanikprüfungen
  und vier Dokumentationsprüfungen. Nachweise:
  `build-language-joint-evidence/clang-language-tests.log` und nach Verbesserung
  der Kamerasicht `final-joint-tests.log`, `simulation.bmp`, `analysis.bmp` im
  selben Evidenzordner. Die Offline-Referenz enthält jetzt ebenfalls Körper-,
  Kontakt- und Gelenkaufrufe und besteht die Prüfung gegen das Compilerregister.
  Acht betroffene Prüfungen bestanden auch unter MSVC/AddressSanitizer:
  Gelenkwerte/-referenz, Körper-/Kontaktregression, native Sprachsuite,
  Compiler-Mutationstests und beide C-Gelenksolver. Nachweis:
  `asan-language-tests.log` im selben Evidenzordner. Ein erster Aufruf ohne
  Visual-Studio-Umgebung scheiterte vor Testbeginn an der fehlenden ASan-DLL
  im Suchpfad (`asan-missing-runtime.log`); mit `vcvars64.bat` bestanden alle acht.
  Das installierte, verschobene SDK bestand mit zehn Sprachprogrammen und sieben
  Sprachexperimenten einschließlich Auswertung des Gelenkpendels in C und Physim:
  `build-clang-ninja/SDK Test ä fd5a298906/verification.log`.
  Der zunächst offene Körpergruppen-Solver wird oben ergänzt; weitere Gelenkarten
  und Quader-CCD bleiben offen. Lineare Kugel-Sweeps werden oben ergänzt.

- Kontakte in der Sprache (2026-09-20): `Contacts`, `ContactSolver` und
  `ContactResult` binden die gemeinsame Erkennung für Kugel–Kugel,
  Kugel–Ebene, Kugel–Quader, Quader–Ebene und Quader–Quader sowie den
  iterativen Paarsolver mit Coulomb-Reibung und Rückprall ein. Eigenschaften
  bleiben schreibgeschützt; Auflösung liefert beide Körper als unabhängige
  Ergebniswerte sowie Kontaktimpulse und Restfehler. Das Beispiel `contacts.phys`
  prüft analytische elastische und reibungsbehaftete Stöße, leere Kontakte,
  mehrere Kontaktpunkte, Projektion und Kopien. Die App-Vorlage
  **Box auf Ebene · Physim-Sprache** und `box_contacts.phys` entsprechen der
  vorhandenen C-Vorlage. Referenzen vergleichen alle elf Kanäle, Dimensionen
  und Szenen über je 2001 Zustände bei 1 und 2 ms, unabhängige Instanzen,
  Reset und 801 gespeicherte Runner-Zeilen einschließlich Bodenkontakt.
  Acht Fehlerfälle prüfen Einstellungen, Geometrie, Indizes, Quelldiagnosen
  und Reset nach Abbruch. Alle 20 Sprachprüfungen einschließlich fünf
  App-Workflows bestanden unter Clang/Debug. Nachweis und visuell geprüfte
  Szenen-/Analyseansichten: `build-language-contact-evidence/clang-language-tests.log`,
  `simulation.bmp` und `analysis.bmp` im selben Evidenzordner.
  Sechs betroffene Prüfungen bestanden unter MSVC/AddressSanitizer, einschließlich
  Kontaktwerten, C-Referenz, Körper-/Sensorregression, nativer Sprachsuite und
  Compiler-Mutationstests: `asan-language-tests.log` im selben Evidenzordner.
  Nach atomarer Übernahme des berechneten Schrittzustands bestanden Kontaktreferenz
  und App-Workflow erneut; `final-contact-tests.log`,
  `final-asan-contact-test.log` und die aktualisierten Bilder dokumentieren
  den Endstand. Das installierte und verschobene SDK bestand mit neun
  Sprachprogrammen und sechs Sprachexperimenten einschließlich C-Auswertung
  des Kontaktversuchs und eigenständiger Header-Prüfung:
  `build-clang-ninja/SDK Test ä b577979dcb/verification.log`.
  Die zunächst offene Distanzgelenkbindung wird oben ergänzt; gemeinsame
  Körpergruppen-Solver und lineare Kugel-Sweeps werden oben ergänzt.

- Starre Körper in der Sprache (2026-09-20): `Body.sphere` und `Body.box`
  verwenden die gemeinsame Mechanikbibliothek. Lesbare SI-Eigenschaften,
  geprüfte Zustandsänderungen, Impulse, Kräfte/Drehmomente, Quaternionrotation,
  Punktgeschwindigkeit und kinetische Energie sind angebunden. Körper haben
  unabhängige Wertkopien, auch in Arrays und Strukturen; Mutationen verlangen
  veränderbare Empfänger und veröffentlichen nur erfolgreiche Ergebnisse.
  Das eigenständige Beispiel `rigid_body.phys` prüft analytische Impuls- und
  Energiewerte sowie Kopien und Argumentauswertung. `spinning_body.phys` und
  die App-Vorlage **Starrer Körper · Physim-Sprache** zeigen einen angetriebenen
  Quader mit 15 Messkanälen. C-Referenzen prüfen zwei Schrittfolgen,
  unabhängige Instanzen, Reset, Orientierung, Szene, gespeicherte Messwerte,
  Quelldiagnosen und unveränderte Zustände nach Fehlern. Alle 17 Sprachprüfungen
  einschließlich vier App-Workflows bestanden unter Clang/Debug; Nachweis:
  `build-language-body-evidence/clang-language-tests.log`.
  Fünf betroffene Prüfungen bestanden unter MSVC/AddressSanitizer ebenfalls,
  einschließlich Körperwerten, C-Referenz, Sensorregression, nativer Sprachsuite
  und Compiler-Mutationstests: `asan-language-tests.log` im selben Evidenzordner.
  Nach Verbesserung der Vektorsichtbarkeit bestanden Körperreferenz und
  App-Workflow erneut; `final-body-tests.log`, `simulation.bmp` und `analysis.bmp`
  dokumentieren den geprüften Endstand. Das installierte, verschobene SDK
  bestand mit acht Sprachprogrammen und fünf Sprachexperimenten einschließlich
  C-Auswertung des starren Körpers:
  `build-clang-ninja/SDK Test ä 830e2d87fa/verification.log`.
  Beim Gesamtbuild wurde außerdem die benötigte `dwmapi`-Verknüpfung des
  optionalen Windows-UI-Benchmarks ergänzt. Die zunächst offene Kontaktbindung
  wird oben ergänzt, ebenso die Distanzgelenk- und Körpergruppenbindung. Diese Bindung schließt
  LANG-004 und LANG-006 noch nicht vollständig ab.

- Gültige Messreihen und Berichtstabellen in der Sprache (2026-09-20):
  `Series.select` filtert bis zu 32 zusammengehörige Spalten mit gemeinsamer
  Ergebniszuordnung. `Table`, `row` und `export` binden Berichtstabellen mit
  Einheitenumrechnung und CSV-Export ein; `Plot.points` zeichnet Messpunkte
  ohne Verbindung über Ausfälle. Die Sensorwurf-Vorlage verwendet bei
  Sprachauswertung `analysis_sensors.phys` mit Verfügbarkeit, Fehlerhistogramm,
  Messfehlerstatistik und mittlerer Standardunsicherheit. Null oder ein gültiger
  Messwert werden ausdrücklich behandelt. Compiler und Backend unterstützen
  typisierte Arrayargumente und besitzende Bibliotheksrückgaben. Referenzen
  prüfen Arraykopien, unabhängige Auswahlzuordnungen, Rohreihenfreigabe,
  Zentimeter-Meter-Konvertierung, Tabellenaliase, CSV-Inhalte, Grenzen und
  Wiederholung nach Fehlern. Alle 14 Sprachprüfungen einschließlich drei
  App-Workflows bestanden unter Clang/Debug; sieben betroffene Prüfungen
  bestanden auch unter MSVC/AddressSanitizer. Zwei ASan-Prüfungen liefen bei
  paralleler Buildlast ins Zeitlimit und bestanden im separaten Wiederholungslauf.
  Nachweise: `build-language-report-evidence/clang-language-tests.log`,
  `asan-first-attempt.log` und `asan-retry.log` im selben Evidenzordner.
  Nach der Kürzung einer Tabellenüberschrift bestanden Report- und Sensor-App-Test
  erneut; finale Logs und geprüfte `analysis.bmp`/`statistics.bmp` liegen ebenfalls
  dort. Das installierte und verschobene SDK bestand einschließlich der neuen
  Sensorauswertung mit Punktkurve, Tabellenwerten und Exporten:
  `build-language-report/SDK Test ä 4cbf5745be/verification.log`.
  Die laufende Entwicklungs-App blockierte zunächst ihre ausführbare Datei;
  der vollständige Build und die App-Prüfungen erfolgten deshalb in
  `build-language-report`. Nach Freigabe der Datei wurden App und Compiler auch
  in `build-clang-ninja/bin` erfolgreich aktualisiert. LANG-005 bleibt wegen weiterer Bindungen und
  sprachgesteuerter Batchausführung offen.

- Sensoren in der Sprache (2026-09-20): `Distribution`, `SensorConfig`, `Sensor`
  und `Measurement` binden die gemeinsame Messbibliothek ein. `read`, `reset`
  und `resetForRun` sind mutierende Methoden mit einmaliger Empfängerauswertung
  und Veröffentlichung erst nach Erfolg, auch in verschachtelten Arrays und
  Strukturmethoden. Sensoren besitzen unabhängige Wertkopien; Messwerte und
  Gültigkeit, Zeit, Unsicherheit, Rasterindex und ausgelassene Slots bleiben
  getrennt. Explizite Seeds sowie Laufseed-XOR-Streams erhalten alle 64 Bits
  und verändern den Modellgenerator nicht. Die neue App-Vorlage
  **Sensorwurf · Physim-Sprache** zeichnet Soll-/Modell-/Sensorwerte auf.
  Native Referenzen prüfen Einheitenumrechnung, Zeitraster, Ausfälle,
  Unsicherheitsrechnung, Kopien, Argument-Seiteneffekte und Fehlerdiagnosen.
  Vier Seeds einschließlich beider 64-Bit-Grenzfälle, unabhängige Instanzen,
  Reset, gespeicherte Runnerdaten und Sensorverwendung in Analysen sind gegen
  die C-API geprüft. Alle 13 Sprachtests einschließlich dreier App-Abläufe
  bestehen mit Clang Debug; die sechs ausgewählten Compiler-/Integrationsprüfungen
  bestehen mit MSVC AddressSanitizer. Der neue App-Test prüft auch die
  Gültigkeitsmaskierung der Sensorstatistik. Der installierte, verschobene
  SDK-Neubau besteht einschließlich Sensorwurf mit C- und Sprachauswertungen.
  Nachweise: `build-language-sensor-evidence/clang-language-tests.log`,
  `build-language-sensor-evidence/asan-language-tests.log` und
  `build-clang-ninja/SDK Test ä ed99d42ce1/verification.log`.
  Die Bildprüfung zeigte eine verdeckte Sensormarkierung; ein gelbes Kreuz am
  Messort ist jetzt auch neben der Modellkugel erkennbar. Sensorreferenz und
  vollständiger Sensor-App-Ablauf bestehen erneut. Simulation und Ergebnisplot
  wurden bei 1080 × 740 visuell geprüft; Bilder und abschließendes Testprotokoll
  liegen unter `build-language-sensor-evidence/`.

- Polylinien in der Sprache (2026-09-20): `polyline(points, radius, color, id)`
  bindet `[Vec3]`-Werte an die gemeinsame Szenen-API. Damit sind alle acht
  Szenenprimitive aus der Sprache erreichbar. Literale, Ausschnitte und
  Funktionsrückgaben behalten ihre Wertsemantik; eine Szene besitzt eigene
  Punktkopien. Native Prüfungen decken benannte Argumente in Quellreihenfolge,
  spätere Änderungen am Quellarray, exakt 96 Punkte, beide Kapazitätsgrenzen,
  leere/einpunktige Arrays, ungültige Radien/Farben, doppelte IDs und falsche
  Aufrufphasen ab. Ein separates Modul prüft den impliziten Elementtyp von `[]`
  ohne sonstige Vec3-Deklaration. Fehlgeschlagene Szenen bleiben vollständig leer
  und sperren weitere Schritte bis zum Reset. Die Feder-Vorlage verwendet jetzt
  eine Spirale mit 65 Punkten, geprüft gegen die C-Vorlage. Alle elf Sprachtests
  einschließlich beider App-Abläufe bestehen mit Clang Debug; Typprüfung und
  Experimentintegration bestehen zusätzlich mit MSVC AddressSanitizer.
  Nachweise: `build-polyline-evidence/clang-language-tests.log` und
  `build-polyline-evidence/asan-language-tests.log`.
  Der installierte, verschobene SDK-Neubau besteht ebenfalls einschließlich
  des Sprach-Federexperiments und seiner Auswertungen.
  Nachweis: `build-clang-ninja/SDK Test ä f11a8b0fb4/verification.log`.

- Mengenwerte mit Einheit (2026-09-20): `Quantity(value, unit)` verbindet einen
  endlichen Zahlenwert mit einer Einheit. `converted`, `adding`, `subtracting`,
  `multiplied` und `divided` verwenden Methodenaufrufe; `value` und `unit` sind
  schreibgeschützte Eigenschaften. Typannotation, Rückgaben, Arrays und eigene
  Strukturen einschließlich mutierender Methoden sind unterstützt. Die native
  Referenz prüft Umrechnung, Rechnung und unabhängige Kopien; Fehlerfälle prüfen
  inkompatible Dimensionen, Nulldivisor sowie numerischen Über- und Unterlauf.
  Wurf und Analysereferenz verwenden `Quantity`; der Editor kennt den Typnamen.
  Die Dimensionsprüfung bleibt dynamisch, statische SI-Typparameter bleiben offen.
  Alle fünf betroffenen Sprachtests bestehen mit Clang Debug; der installierte,
  verschobene SDK-Neubau besteht ebenfalls.
  Nachweis: `build-clang-ninja/SDK Test ä 3a8f1fe8d0/verification.log`.

- Einheitenalgebra in der Sprache (2026-09-20): `unit.multiplied`, `divided`,
  `powered` und `isCompatible` sind als Instanzmethoden gebunden. Ergebnisse
  behalten Wertsemantik und besitzen explizite Symbole. Native Prüfungen decken
  Dimensionen, Maßstäbe, negative/Nullpotenzen, benannte Argumente in Quellreihenfolge,
  Int8-Grenzen sowie Exponentenüberlauf, Maßstabsüberlauf und Unterlauf auf null ab.
  Das Wurfbeispiel erzeugt Geschwindigkeit und Energie durch Einheitenalgebra;
  die Analysereferenz verwendet eine zusammengesetzte Integraleinheit.
  Typprüfung, native Programme, CLI, Experiment- und Analyseintegration bestehen
  mit Clang Debug und MSVC AddressSanitizer. Der installierte, verschobene SDK-Neubau einschließlich
  Sprachprogrammen sowie Experimenten und Auswertungen besteht ebenfalls.
  Nachweis: `build-clang-ninja/SDK Test ä 29c5f771fa/verification.log`.

- Statische Strukturmethoden (2026-09-20): `static func` wird über den Typ
  aufgerufen, beispielsweise `Particle.atRest(position)`. Es gibt keinen
  `self`-Empfänger. Die Typprüfung weist verwechslungsbedingte Aufrufe über
  Instanzen sowie Instanzmethodenaufrufe ohne Empfänger ab. Benannte Argumente,
  Vorwärtsaufrufe, Rekursion und besitzende Rückgaben sind abgedeckt.
  Alle sieben ausgewählten Sprachtests bestehen mit Clang Debug und MSVC
  AddressSanitizer (Lexer, Parser, Typprüfung, CLI, native Programme,
  Experiment- und Analyseintegration).

- Mutierende Strukturmethoden (2026-09-20): `mutating func` erlaubt Änderungen
  über `self` und verlangt am Aufrufort einen veränderlichen Struktur-/Indexpfad.
  Die Empfängerkopie wird erst nach erfolgreicher Rückkehr veröffentlicht.
  Prüfungen decken skalare und besitzende Rückgaben, `Void`, frühe Rückkehr,
  Selbstersetzung, rekursive/verschachtelte Methoden, kopierte Arrayfelder,
  einmalige Indizes und konkurrierende Seiteneffekte im Argument ab. Ein reines
  Strukturprogramm prüft den Codepfad ohne Arraylaufzeit. Der Experimentfehlerfall
  verändert verschachtelte Empfängerkopien und prüft nach abgefangenem Fehler beim
  Reset den unveränderten globalen Zustand. Die Analyse prüft eine mutierende
  Methode auf einem eigenen Datenreihen-Wrapper. Pendel und Partikel verwenden
  jetzt `advance` direkt auf Arrayelementen.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer.
  Der Editor hebt `mutating` und `self` hervor; der vollständige Sprach-App-Ablauf
  besteht mit dem aktualisierten Pendelbeispiel.
  Auch der installierte, verschobene SDK-Neubau mit allen Beispielen besteht.
  Nachweis: `build-clang-ninja/SDK Test ä fcdaa9f42a/verification.log`.

- Eigene Strukturmethoden (2026-09-20): `func`-Deklarationen innerhalb einer
  Struktur verwenden `self` als unveränderlichen Empfänger. Auflösung nach Typ,
  benannte Argumente, Vorwärtsaufrufe, Rekursion, Aufrufketten und Strukturen ohne
  Felder sind implementiert. Native Referenzen prüfen eigene Methoden namens
  `append`/`count`, unabhängige Arrayfelder, besitzende Rückgaben, einmalige
  Auswertung sowie Freigabe bei Assertion und Rekursionsgrenze. Die semantische
  Mutationsprüfung enthält eigene Methodendeklarationen. Partikel und Pendel
  verwenden `advanced`; die Analysereferenz ruft eine eigene Statistikmethode auf.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer;
  die nachträglich ergänzte Analysereferenz besteht ebenfalls auf beiden Compilern.
  Der installierte, verschobene SDK-Neubau besteht mit allen Beispielen.
  Nachweis: `build-clang-ninja/SDK Test ä c5a4a7b8c2/verification.log`.
  Statische Methoden wurden im nachfolgenden Schritt ergänzt (siehe oben).

- Einheitliche Methoden-API (2026-09-20): typgebundene Operationen verwenden
  Empfängeraufrufe, unter anderem `array.append(value)`, `array.remove(at: index)`,
  `vector.normalized()`, `rotation.rotate(vector)`, `channel.sample(value)`,
  `dataset.series(name)` und `series.derivative(time)`. Arraylängen sind über
  `.count` lesbar. Konstruktoren heißen `Unit`, `Channel` und `Dataset`;
  `Quat.axisAngle` ist ein statischer Konstruktor. Die frühere globale Form dieser
  Operationen ist entfernt. Methoden werden nach Empfängertyp aufgelöst und
  können verkettet werden; der Empfänger wird vor den Argumenten einmal ausgewertet.
  Mutierende Arraymethoden unterstützen veränderliche Struktur-/Indexpfade und
  veröffentlichen den aufgebauten Wurzelwert erst nach erfolgreicher Änderung.
  Beispiele, Sprachreferenz und native Fehlerfälle sind auf die Methoden-API umgestellt.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer;
  beide App-Abläufe (vollständig eigene Sprache sowie gemischtes C/Sprachprojekt)
  bestehen ebenfalls. Die vergrößerte native Testsuite erhält 180 Sekunden für
  ihren Build und 300 Sekunden insgesamt; Buildfehler melden nun den Prozessstatus.
  Das installierte, verschobene SDK besteht mit allen Beispielen.
  Nachweis: `build-clang-ninja/SDK Test ä 162d82e893/verification.log`.
  Eigene statische Strukturmethoden wurden anschließend ergänzt (siehe oben).

- Erster Arrayindex (2026-09-23): `array.firstIndex(of: value)` gibt den
  ersten gleichen Index als `Int64?` oder `nil` zurück. Verschachtelte und
  besitzende Elemente, leere Arrays, Typfehler, Auswertungsreihenfolge und
  eine Million wiederholte Suchaufrufe sind unter MSVC und Clang geprüft;
  Sprachvertrag 0.23.0. Der lokale MSVC-ASan-Build hängt schon beim bisherigen
  `array_concat`-Programm ohne CPU-Fortschritt; eine ASan-Bestätigung dieses
  Ausbaus liegt daher nicht vor.

- Array-Suche (2026-09-23): `array.contains(value)` verwendet die vorhandene
  strukturelle Gleichheit für vergleichbare Elementtypen. Leere, verschachtelte
  und besitzende Arrays sowie Typfehler sind nativ geprüft; Sprachvertrag 0.22.0.

- Array-Einfügen (2026-09-23): `array.insert(value, at: index)` verändert
  einen veränderlichen Arraypfad, `array.inserting(value, at: index)` liefert
  eine unabhängige Kopie. Beide akzeptieren die Länge als Einfügeposition,
  prüfen Grenzen und veröffentlichen erst nach erfolgreicher Kopie;
  Sprachvertrag 0.21.0.

- Bereichsschrittweiten (2026-09-23): `for i in 0..<10 by 2` und
  `array[1..<end by 2]` prüfen positive `Int64`-Schritte und werten sie
  einmal aus. Schrittweise Bereichszuweisungen behalten die Arraylänge und
  verlangen ein Ersatzarray mit genau der Zahl ausgewählter Elemente.
  Fehler veröffentlichen keine Teiländerung; Sprachvertrag 0.20.0.

- Arrayverkettung (2026-09-23): `a + b` erzeugt aus zwei gleich typisierten
  Arrays einen unabhängigen Wert. `+=` schreibt auf veränderlichen Namen,
  Feld-/Indexpfaden und ganzen Bereichen. Leere Literale nutzen Kontext vom
  anderen Operanden oder Ergebnistyp; Speicher- und Grenzfehler veröffentlichen
  keine Teiländerung. Referenztests prüfen verschachtelte Besitzer,
  Auswertungsreihenfolge und nur einmal ausgewertete Bereichsgrenzen;
  Sprachvertrag 0.19.0.

- Arraybereiche mit ausgelassenen Grenzen (2026-09-23): In Arrayindizes und
  Bereichszuweisungen dürfen Unter- und/oder Obergrenze fehlen. Der Anfang
  ist dann null, das Ende die Arraylänge; ein fehlendes geschlossenes Ende
  ergibt auf einem leeren Array ebenfalls einen leeren Bereich. Parser,
  Typprüfung, C-Emission und Laufzeitprüfung teilen diesen Vertrag.
  Eigenständige Bereiche und `for` behalten explizite Grenzen;
  Sprachvertrag 0.18.0.

- Bereichszuweisungen (2026-09-23): `array[start..<end] = replacement` und
  die geschlossene Form ersetzen Bereiche veränderlicher Arraypfade. Die
  Ersatzlänge darf abweichen; leere Bereiche fügen ein und leere Ersatzarrays
  entfernen. Der Compiler prüft Typ und Veränderlichkeit und wertet Empfänger,
  Grenzen und Ersatzarray je einmal in dieser Reihenfolge aus. Laufzeitgrenzen
  werden vor dem Ersatzarray geprüft; die transaktionale Array-Ersetzung
  veröffentlicht verschachtelte Feld- und Indexpfade erst bei Erfolg.
  Referenztests prüfen Kopien, Alias-Ersatz, verschachtelte Besitzer,
  Seiteneffekte, Grenzfehler und Speicherbilanz mit MSVC und Clang.

- Array-Ausschnitte (2026-09-20): `array[start..<end]` und `array[start...end]`
  erzeugen unabhängige `[T]`-Werte mit Indexbeginn null. Die Typprüfung verlangt
  ganzzahlige Grenzen und weist Ausschnitte als Zuweisungsziele ab. Native
  Referenzen prüfen Verschachtelung, Strukturen mit Arrayfeldern, Rückgabe,
  Schleifen, Seiteneffekte in den Grenzen und Fehlerexits ohne verbleibende
  Array-Allokationen. Die Besitztests vergleichen 98 Grenzkombinationen sowie
  extreme Ganzzahlgrenzen und injizieren Fehler bei Block- und Elementkopien.
  Das Partikelbeispiel bearbeitet eine unabhängige Teilgruppe; Experiment- und
  Analysefehlerprüfungen halten Ausschnitte über abgefangene Laufzeitfehler.
  Weggelassene Grenzen, Schrittweiten und Bereichszuweisungen waren damals offen.
  Die oben dokumentierten Methoden-/SDK-Prüfungen schließen diese Ausschnittfälle ein.

- Arraygrößen ändern (2026-09-20): `arrayAppending(array, element)` und
  `arrayRemoving(array, index)` erzeugen veränderte Wertkopien. Typprüfung,
  Kontext für angehängte Elemente und Auswertung von links nach rechts sind
  implementiert. Native Referenzen prüfen unabhängige Kopien, verschachtelte
  Arrays, Strukturen mit Arrayfeldern, Seiteneffekte, leere Ergebnisse und
  negative/zu große Indizes sowie Entfernen aus einem leeren Array. Fehlgeschlagene
  Operationen räumen ihre Besitzer vor dem Fehlerexit auf. Pendel-, Partikel-
  und Analysebeispiele verwenden diese Funktionen. Mutierende Methoden und
  Bereichszuweisungen waren damals weiterer Ausbau.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer.
  Der installierte, verschobene SDK-Neubau besteht mit allen Beispielen;
  Nachweis: `build-clang-ninja/SDK Test ä 727102c290/verification.log`.

- Direkte Arrayiteration (2026-09-20): `for element in array:` wertet das
  Array einmal aus und durchläuft einen unabhängigen Snapshot. Die unveränderliche
  Schleifenvariable erhält den Elementtyp, auch bei verschachtelten Arrays und
  Strukturen mit besitzenden Feldern. Native Prüfungen decken Änderungen am
  ursprünglichen Array, einmalige Auswertung, leere Arrays, verschachtelte Schleifen,
  `break`, `continue` und besitzende Rückgabewerte ab. Pendel und Partikelbeispiel
  verwenden die neue Syntax. Alle neun Sprach-/Besitztests bestehen mit Clang Debug
  und MSVC AddressSanitizer; der verschobene SDK-Neubau mit allen Beispielen besteht.
  Ein zusätzlicher Fehlerexit innerhalb einer verschachtelten Arrayiteration
  bestätigt auf beiden Compilern die vollständig ausgeglichene Allocator-Bilanz.
  SDK-Nachweis: `build-clang-ninja/SDK Test ä 0802818f96/verification.log`.

- Native Spracharrays (2026-09-20): alle drei Ausgabearten unterstützen Literale,
  `arrayCount`, gelesene/veränderte Indizes, verschachtelte Arrays, besitzende
  Strukturfelder, Funktionsparameter/-rückgaben und persistente Modulwerte.
  Kopien bleiben unabhängig; indizierte Zuweisungen werten jeden Index und die
  rechte Seite einmal aus und veröffentlichen den aufgebauten Wurzelsnapshot.
  Generierte Kopier-/Zerstörungsfunktionen und Aufräumketten decken auch
  Kurzschlussauswertung, Schleifen, Switch, `break`, `continue` und Rückgabe ab.
  Ein 64-MiB-Budget begrenzt die Arrayblöcke jeder Programmausführung/Modulinstanz.
  Die Freigabe tief verschachtelter Werte nutzt eine Arbeitsliste statt C-Rekursion.
  Native Referenzen prüfen 10.000 verschachtelte Knoten, negative/zu große Indizes,
  Teilkonstruktion, Speichererschöpfung und eine vollständig ausgeglichene
  Allocator-Bilanz auch nach Fehlerexit. Das neue Partikelbeispiel vergleicht drei
  Bewegungen nach 100 Schritten analytisch; Pendel und Analyse nutzen Arraywerte.
  Alle neun Sprach-/Besitztests bestehen mit Clang Debug und MSVC AddressSanitizer;
  beide Sprach-App-Abläufe und der verschobene SDK-Neubau bestehen ebenfalls.
  SDK-Nachweis: `build-clang-ninja/SDK Test ä 788d125a4e/verification.log`.
  Mutierende Methoden und Bereichszuweisungen waren damals weiterer Ausbau.

- Array-Typprüfung (2026-09-20): strukturell kanonisierte `[T]`-Typen,
  homogene Literale mit Kontext für leere/numerische Elemente, verschachtelte
  Indizes, Funktionsparameter/-ergebnisse, Arrays eigener Strukturen/Enums und
  veränderliche Feld-/Indexpfade sind statisch geprüft. Arraytypen bleiben von
  nominalen Typen getrennt; verschachtelte Annotationen und Typvergleiche verwenden
  das begrenzte semantische Arbeitsbudget. Einzelbyte-Mutationen prüfen zusätzlich
  die Gültigkeit kanonischer Elementtypverweise.
  Alle neun Sprach-/Besitztests bestehen mit Clang nach Korrektur der neuen
  CLI-Erwartung auf den vorhandenen Backend-Fehlercode 2. Typprüfung und native
  Regression bestehen zusätzlich mit MSVC AddressSanitizer.
  **Native Array-Codeerzeugung bleibt offen:** alle drei Ausgabearten weisen
  besitzende Arraytypen mit Quelldiagnose ab, bevor C-Code geschrieben wird.

- Besitzbasis für Spracharrays (2026-09-20): unveränderliche, referenzgezählte
  Blöcke ermöglichen Wertkopien ohne Allokation. Bereichsänderungen veröffentlichen
  erst nach erfolgreicher Elementkonstruktion einen neuen Block. Prüfungen decken
  2.000 Änderungen gegen ein Referenzmodell, Selbst-Einfügen, verschachtelte Kopien,
  gültigkeitsgeprüfte Indizes, Größen-/Referenzüberlauf, Spitzenbudgets und jeden
  Allokationsfehler einer Änderung mit besitzenden Elementen ab. Teilkopien werden
  zurückgerollt, ohne die alte Besitzbilanz oder Lesepointer zu ändern.
  Aufräumketten zerstören lokale Besitzer vor `longjmp` und vor dem Fehlerexit;
  Bereichsmarken, umgekehrte Reihenfolge und getrennte verschachtelte Traps sind
  geprüft. Beide neuen Werttests bestehen mit Clang Debug und MSVC AddressSanitizer;
  die sieben bisherigen Sprachtests bestehen mit Clang, native Programme zusätzlich
  mit AddressSanitizer. Die CTest-Suite umfasst jetzt 84 Tests.
  **Noch keine Array-Ausführung in der Sprache:** Typprüfung, generierte Besitzer,
  persistente Modulwerte und Indexzuweisungen bleiben anzubinden. Vertrag und
  offene Punkte: [Array-Werte und Speicherlebensdauer](language-values.md).

- Quaternion-Rechnung in der Sprache (2026-09-20): `normalizeQuat`,
  `conjugateQuat`, `multiplyQuat` und `slerpQuat` binden die gemeinsame
  Mathematikbibliothek ein. Native Referenzen prüfen das nicht kommutative
  Hamilton-Produkt, unveränderte Größen bei roher Multiplikation, Reihenfolge
  zusammengesetzter Rotationen, Rückrotation, große/kleine Normalisierungen,
  antipodale Quaternionen und kleine Interpolationswinkel. Nullrotationen,
  Produktüberlauf und Interpolationswerte außerhalb [0, 1] liefern Quelldiagnosen.
  Das ausgelieferte `rotation_path.phys` prüft einen zusammengesetzten
  Rotationspfad an 101 Punkten gegen eine analytische Referenz auf 1e-12.
  Alle sieben Sprachtests bestehen mit Clang Debug; Typprüfung und native
  Ausführung zusätzlich mit MSVC AddressSanitizer. Der verschobene SDK-Neubau
  mit sechs eigenständigen Sprachprogrammen besteht:
  `build-clang-ninja/SDK Test ä b65741742a/verification.log`.

- Vierdimensionale Sprachvektoren (2026-09-20): `Vec4` ist ein eigener Werttyp
  mit vier veränderlichen Feldern entlang eines `var`-Pfads, Funktionsübergabe,
  Strukturkopien und denselben geprüften Operatoren wie `Vec2`/`Vec3`.
  `dot4`, `length4` und `normalize4` verwenden die gemeinsame Mathematikbibliothek.
  Native Prüfungen decken die vierte Komponente, einmalige Auswertung,
  sehr große/kleine Werte, Nullvektoren, Division durch null und Überlauf ab;
  falsche Dimensionen, Quaternion-Verwechslungen und unveränderliche Ziele
  werden vor Ausführung abgewiesen. Das ausgelieferte `phase_space.phys`
  integriert zwei harmonische Freiheitsgrade über 1.000 RK4-Schritte und prüft
  analytischen Zustand und Energie auf 1e-10.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer.
  Der verschobene SDK-Neubau besteht einschließlich des neuen Beispiels:
  `build-clang-ninja/SDK Test ä 043f075317/verification.log`.

- Sprachintegration und SDK-Auslieferung (2026-09-20): Der vollständige
  Clang-Debug-Build und alle 80 bisherigen CTest-Prüfungen bestehen unter Windows.
  Zwei neu registrierte, anschließend ausgeführte App-Prüfungen bestehen ebenfalls:
  reine Sprachprojekte und C-Experimente mit Sprach-Analyse prüfen Quelldiagnosen,
  Fehlerkorrektur, native Builds, Laufsteuerung, Analyse, Exporte und Wiederöffnen.
  Beide behalten Projekte und Screenshots in eindeutigen Unicode-Pfaden.
  Der erweiterte SDK-Test baut nach Installation und Verschieben alle C-Vorlagen
  und Sprachbeispiele neu. Vier Sprachprogramme sowie Pendel, Wurf und Feder mit
  jeweils beiden Sprach-Analysen bestehen; der installierte Compiler wurde aus
  dem verschobenen SDK gefunden. Messdateien und Berichte werden wieder eingelesen.
  Nachweis: `build-clang-ninja/SDK Test ä a860977fa8/verification.log`.
  Die Suite umfasst damit 82 Tests; Linux ist weiterhin nicht lokal ausgeführt.

- Verkürzte Sprachzuweisungen (2026-09-20): `+=`, `-=`, `*=`, `/=` und `%=`
  verwenden dieselbe Typprüfung und geprüfte Arithmetik wie ausgeschriebene
  Zuweisungen. Native Referenzen prüfen ganze Zahlen, Vektoren, verschachtelte
  Felder, Schleifen und einmalige rechte Auswertung nach Lesen des alten Werts.
  Überlauf und Zugriff auf noch nicht initialisierte Globale werden abgefangen;
  unveränderliche Felder und unpassende Typen scheitern vor Codeerzeugung.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  Experiment-, native und Frontendtests zusätzlich mit MSVC Release.
  Wurf und Feder nutzen die Syntax und bestehen auch aus dem installierten SDK
  die bisherigen Bewegungs- und Energietests.

- Sprachbindungen für Kraftgesetze (2026-09-20): axiale Feder/Dämpfung,
  Stokes-/quadratischer Kugelwiderstand und Auftrieb verwenden die geprüften
  Mechanikfunktionen. Native Referenzen prüfen Vorzeichen, Kompression, axiale
  Dämpfung, Widerstandsleistung, Auftrieb und Fehler bei Parametern, Singularität
  und Überlauf. `spring.phys` integriert Bewegung und dissipierte Arbeit mit
  im Sprachquelltext formulierten RK4-Stufen. 2.000 Zeitschritte mit dt=0,002 s
  vergleichen alle elf Kanäle gegen die C-Vorlage; die analytische Auslenkung
  wird auf 2e-9 m, die Gesamtenergiebilanz auf 2e-8 J geprüft.
  Zusätzlich stimmen die 13 Szeneobjekte und 65 Federpunkte nach dem Start
  und nach 2.000 Schritten überein. Der vollständige Sprachprojekt-Workflow
  besteht unter MSVC und Clang.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  Experiment-, native und Typprüfertests zusätzlich mit MSVC Release.
  Das aus dem installierten SDK gebaute Federmodul besteht dieselben Referenzen.
  Ein SDK-Runner-Lauf schreibt 2.001 Messzeilen und wird mit der Sprach-Auswertung
  erfolgreich zu Bericht, Geschwindigkeits-CSV und Positions-SVG verarbeitet.
  Allgemeine Integrator-Callbacks und die Polyline-Federdarstellung bleiben offen.

- Vektorrechnung in der Sprache (2026-09-20): `Vec2`/`Vec3` unterstützen
  Addition/Subtraktion, Vorzeichen, Skalierung von beiden Seiten, Division und
  exakten Komponentenvergleich. Dot/Cross, Länge und Normalisierung verwenden
  die gemeinsame Mathematikbibliothek. Typfehler, Reihenfolge und einmalige
  Operandenauswertung, verschachtelte Ausdrücke, Nullvektoren, große Komponenten,
  winzige Divisoren sowie Überlauf-/Nullteilerdiagnosen sind geprüft.
  Die Wurfvorlage verwendet jetzt Vektorgleichungen und das Skalarprodukt für
  Energie; Bewegungs-/Energietests und gespeicherte Runner-Werte bleiben korrekt.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  Experiment-, native und Typprüfertests zusätzlich mit MSVC Release. Die aus
  dem installierten SDK gebaute Wurfvorlage besteht dieselben Experimenttests.

- Erweiterte Sprach-Szenenbindung (2026-09-20): `Quat` mit Wertsemantik,
  `axisAngle` und normalisierendem `rotate` sowie Box, Ebene, Pfeil, Punkt und
  UTF-8-Label ergänzen Kugel und Linie. Native Referenzen prüfen Strukturkopien,
  Komponentenänderung und 90°-Rotation. Experimenttests prüfen die Geometrie,
  Orientierung, Farbe, ID und Beschriftung aller sieben Formen sowie Reset.
  Nullquaternionen, nichtpositive Größen, Steuerzeichen, doppelte IDs,
  UInt32-Überlauf und falsche Aufrufphasen werden abgefangen; Teilszenen werden
  verworfen und lassen sich nach Reset wieder aufbauen. Alle sieben Sprachtests
  bestehen mit Clang Debug und MSVC AddressSanitizer; Experiment-, native und
  Typprüfertests zusätzlich mit MSVC Release. `scene_shapes.phys` wurde aus dem
  installierten SDK gebaut und besteht dieselben Szene-/Rotationstests.
  Die Sprachbindung für Polylinien bleibt offen.

- Zufallsverteilungen in Sprachexperimenten (2026-09-20): `randomUniform` und
  `randomNormal` verwenden die geprüfte C-Verteilungsbibliothek und den Generator
  der jeweiligen Experimentinstanz. Tests vergleichen vier Seeds einschließlich
  0 und UINT64_MAX, unverbrauchte Ziehungen bei degenerierten Verteilungen,
  unabhängige Instanzen, Reset mit gleichem/geändertem Seed und 201 aufgezeichnete
  Runner-Werte gegen C-Referenzen. Ungültige Parameter und Aufrufe aus Initialisierung
  oder Szenenaufbau werden mit Quelldiagnosen abgefangen; der Generator bleibt bei
  fehlgeschlagenen Ziehungen unverändert. Alle sieben Sprachtests bestehen mit
  Clang Debug und MSVC AddressSanitizer; Experiment-, CLI- und Typprüfertests
  zusätzlich mit MSVC Release. Das installierte SDK baut `random_samples.phys`,
  dessen Runner-Lauf 201 Messzeilen schreibt. Explizite, kopierbare Sprachströme
  sind inzwischen mit `Rng(seed)`, `sample` und `reseed` auch in eigenständigen
  Programmen und Analysen verfügbar. `Rng.forRun` und `reseedForRun` leiten
  getrennte Ströme aus dem Seed eines Experimentlaufs ab; Tests vergleichen
  ihre Werte und Resets mit der C-Referenz. Weitere Unsicherheitsmodelle bleiben offen.

- Explizite Sprach-Zahlenkonvertierungen (2026-09-20): `Int64(wert)` und
  `Float64(wert)` sind typgeprüfte Aufrufe mit einmaliger Argumentauswertung.
  Float64 nach Int64 prüft den Bereich vor dem Abschneiden; Int64 nach Float64
  verwendet explizite Rundung zur nächsten Zahl mit geradem Gleichstandsfall.
  Native Grenztests prüfen Identität, beide Vorzeichen, Nachkommastellen,
  2^53-Rundung, beide Int64-Grenzen und fehlschlagende Rückkonvertierung.
  Die Analyse-Referenz berechnet einen eigenen Mittelwert aus der Messpunktzahl;
  eine zusätzliche Fehlerreferenz prüft abgefangenen Konvertierungsüberlauf.
  Alle sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  native Tests und Typprüfung bestehen zusätzlich mit MSVC Release. Das neue
  `sampling.phys` wurde aus dem installierten SDK gebaut und ausgeführt:
  die numerische Verschiebung -13,6133 stimmt innerhalb von 1e-12 mit der
  analytischen Lösung überein.

- Enum-Auswahl mit `switch` (2026-09-20): Auswahl und Fallblöcke verwenden `:`
  und Einrückung. Die statische Prüfung verlangt vollständige Fallabdeckung oder
  ein abschließendes `default` und weist doppelte/fremde Fälle sowie dynamische
  Fallmuster ab. Native Referenzen prüfen einmalige Auswertung, Rückgaben,
  automatisches Zweigende, verschachtelte Auswahl und `break`/`continue` mit
  Schleifen. Mutationstests enthalten jetzt ebenfalls Auswahlblöcke.
  Die sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  native Ausführung und Frontendtests bestehen zusätzlich mit MSVC Release.
  Auswahl ist derzeit auf einfache Enums beschränkt; Nutzdaten und erweiterte
  Fallmuster gehören weiterhin zum offenen Sprachausbau.

- Einfache Sprach-Enums (2026-09-20): `enum Name:` mit eingerückten `case`-Zeilen,
  nominale Typprüfung, qualifizierte Fälle, Gleichheit, Kopien, Strukturfelder und
  Funktionsparameter/-ergebnisse sind implementiert. Parser- und Typfehler sowie
  Einzelbyte-Mutationen ergänzen einen nativ ausgeführten Zustandsautomaten.
  Die sieben Sprachtests bestehen mit Clang Debug und MSVC AddressSanitizer;
  der native Enumtest sowie Parser-/Checker-Tests bestehen auch mit MSVC Release.
  Das Flugphasen-Beispiel wurde aus dem installierten SDK gebaut und liefert
  die geprüfte Landung bei 1,02 s. Enums mit Nutzdaten bleiben offen;
  LANG-006 ist damit noch nicht vollständig erfüllt.

- Weitere Sprach-Datenreihenoperationen (2026-09-20): Integral mit einheitengeprüftem
  Anfangswert, affine Transformation, Summe/Differenz/Produkt/Quotient und explizites
  lineares/Nearest/Previous-Resampling verwenden die gemeinsame Series-Bibliothek.
  Numerische Quellreferenzen prüfen Trapezwerte, Anfangswert, Offset, Quotient und
  Zwischenpunkte einschließlich der Nearest-Gleichstandsregel. Ein C-Lauf wird
  explizit auf das Raster eines Sprachlaufs resampelt, mit dessen Position verrechnet
  und nach Schließen der Quelle weiter gelesen. Fehlerreferenzen prüfen falsche
  Integraleinheiten, unabhängige Zuordnungen, Extrapolation und Nullteiler.
  Das Beispiel `analysis_integral.phys` erstellt Positionsrekonstruktion und
  Diskretisierungsresiduum mit Diagrammen und Exporten.
  Die Analyse-Referenzen bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer einschließlich der nativ erzeugten Analysequellen.
  Das Integralbeispiel wurde aus dem installierten SDK gebaut und mit dessen
  Runnern ausgeführt. Der Vakuumwurf liefert 41 exportierte Residuen mit maximal
  1e-12 m zulässiger Abweichung; Bericht und SVG wurden ebenfalls erzeugt.

- Analyseeditor für die Physim-Sprache (2026-09-20): Beim Anlegen wählt
  „Sprache der Auswertung“ unabhängig vom Experiment C oder die erste
  Physim-Auswertungsvorlage. `analysis=analysis.phys` wird von App und CMake
  berücksichtigt; unbekannte und doppelte Einträge werden abgewiesen.
  Laden/Speichern, Backups, Autosave, Syntaxfarben, Tab-Leerzeichen, Builds,
  Quelldiagnosen und archivierte Analysequellen verwenden den gewählten Dateityp.
  Der neue App-Test `language_full` prüft den absichtlichen Analyse-Typfehler,
  die Zeilennavigation, den korrigierten Build, Simulation, zwei Sprachdiagramme,
  CSV/SVG, den bytegleichen Quellsnapshot und erneutes Öffnen beider Sprachdateien.
  `language_mixed` besteht mit C-Experiment und Sprach-Auswertung. Der volle
  Sprachablauf besteht ebenfalls aus dem installierten SDK bei 1080 × 740 in
  einem Pfad mit Umlaut; Editor und Bericht wurden visuell geprüft.
  Analyseeditor und Bericht gehören damit zum ersten durchgängigen Sprachablauf.
  Vollständige API-/Vorlagenparität, weitere Sprachtypen, Debugger und Linux-Abnahme
  bleiben offen; LANG-005 bis LANG-007 sind weiterhin nicht vollständig abgenommen.

- Erste Sprach-Auswertung (2026-09-20): `physimc --emit-analysis` und
  `physim_add_analysis` erzeugen Module für den vorhandenen Analyse-Runner.
  Der Einstieg `analyze()` arbeitet mit geprüften `Dataset`-/`Series`-/`Plot`-Handles,
  ausgewählten Eingabeläufen, Statistik, Ableitung, gleitendem Mittel, Linienplots,
  Histogrammen sowie CSV-/SVG-Export. Ein Bericht wird als normale `.psreport`
  mit Quellprovenienz gespeichert. Kontext, Scratchdateien und Bericht werden
  bei Erfolg und abgefangenen Fehlern freigegeben. Die Referenz verarbeitet
  gemeinsam eine in C erzeugte Polynomreihe und den echten Sprachwurf-Runnerlauf;
  Berichtwerte, SI-Dimensionen und Exporte werden gelesen und geprüft. Zusätzliche
  Quellassertions prüfen Statistik, Einzelwerte, gleitende Mittel, Handlefreigabe
  und erneutes Datasetöffnen. Zwei Fehlerquellen prüfen veraltete Handles und
  inkompatible Kurveneinheiten im Runner und bei wiederholten direkten ABI-Aufrufen.
  Alle sieben Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer mit instrumentierten erzeugten Modulen. Das aus dem installierten
  SDK gebaute Analysebeispiel besteht dieselben Referenzen mit dem installierten
  Analyse-Runner und dem ebenfalls aus dem SDK gebauten Sprachexperiment.
  Tabellen, weitere Series-Funktionen, Sprach-Batchsteuerung und Analyseeditor-
  Auswahl bleiben offen; dies ist keine vollständige LANG-005-/LANG-007-Abnahme.

- Erste Sprachintegration in der App (2026-09-20): Die Projektvorlagen
  „Pendel · Physim-Sprache“ und „Vakuumwurf · Physim-Sprache“ erzeugen `main.phys`
  zusammen mit der vorhandenen C-Auswertung. Projektbeschreibung, Laden,
  Speichern/Backups, Autosave, Editorbeschriftung/Syntaxfarben, CMake-Build,
  Quelldiagnosen und Lauf-/Batch-Quellsnapshots berücksichtigen die Sprache.
  Tab fügt im Sprachexperiment vier Leerzeichen ein. Die Offline-Dokumentation
  enthält den Sprachvertrag und den aktuellen App-Einstieg. CMake reicht
  Compilerdiagnosen ohne eigene Zeilenumbrüche weiter, sodass MSBuild und App
  die ursprüngliche Quellposition erhalten. Die vorhandenen vollständigen
  App-Selbsttests bestehen für Sprachpendel, Sprachwurf mit Leerzeichen/Umlaut
  im Projektpfad und ein C-Pendelprojekt: absichtlicher Typ-/Compilerfehler,
  Zeilennavigation, korrigierter Build, Simulation, Pause/Einzelschritt/Stop,
  C-Auswertung, Plots/Tabellen, Exporte, Vergleich und erneutes Projektöffnen.
  Der Editor bei 1440 × 940 wurde visuell geprüft. Derselbe vollständige
  Sprachpendeltest besteht aus dem installierten SDK-Paket bei 1080 × 740;
  Editor und Quelldiagnose wurden dort ebenfalls visuell geprüft. Der Test
  vergleicht den archivierten Originalquelltext mit dem gespeicherten Editorinhalt.
  Die Batchreferenz besteht
  einschließlich archivierter `.phys`-Quelle; nativer Sprachbuild und
  Experiment-ABI-Referenzen bestehen weiterhin. Ein Physim-Analyseeditor,
  vollständige Sprach-/SDK-Parität und Linux-Abnahme bleiben offen.

- Erste Sprach-Experimentmodule (2026-09-20): `--emit-experiment` und
  `physim_add_experiment` erzeugen native Module für die vorhandene ABI.
  Globale Werte leben pro Instanz; Initialisierung, Create, Reset, Step, Szene und
  Destroy laufen über einen generierten Adapter. Fehler werden an der ABI-Grenze
  abgefangen, fehlgeschlagene Creates freigegeben und fehlerhafte Instanzen bis
  zum erfolgreichen Reset gesperrt. Quelldiagnosen erreichen den echten Runner
  auch bei Create- und Szenenfehlern während Pause/Handshake.
  Vec2/Vec3, mathematische Funktionen, Einheitenkonvertierung, Kanalhandles,
  Messwerte, Metadaten, Kugeln/Linien und der gemeinsame symplektische Integrator
  sind angebunden. Negative Typprüfungen, SDK-Quellmutationen und native
  Mathematik-/Einheitenfehler ergänzen die Referenzen. Das Sprachpendel stimmt
  über 800 Schritte und sechs Kanäle mit der symplektischen C-Variante überein;
  unabhängige Instanzen und Reset sind geprüft. Der Vakuumwurf stimmt mit der
  analytischen Lösung überein. Zwei echte Runnerdateien mit jeweils 201 Samples
  werden vollständig gelesen und gegen Referenzen geprüft, einschließlich
  Compiler-, Quell- und Modulprovenienz. Drei Fehlerexperimente prüfen zusätzlich
  vollständige Fehlernachrichten im interaktiven Protokoll und Fehlerstatus.
  Alle sechs Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer mit instrumentierten erzeugten Programmen/Modulen. Die
  vorhandenen Protokoll-, Szenen- und Runner-Isolationsprüfungen bestehen mit
  Clang ebenfalls. Die aus dem installierten SDK gebauten Module bestehen
  dieselben Referenzen mit dem installierten Runner; auch die beiden
  eigenständigen SDK-Sprachbeispiele laufen erfolgreich.
  Die Beispiele ersetzen noch nicht die vollständigen C-Vorlagen mit RK4,
  Luftwiderstand, Sensorrauschen und Flugbahnen. Analyse-ABI, weitere SDK-Funktionen
  und App-Sprachintegration bleiben offen; LANG-004 bis LANG-007 sind nicht abgenommen.

- Strukturtypen und Zustandswerte (2026-09-20): `struct` verwendet `:` mit
  eingerückten, explizit typisierten `let`-/`var`-Feldern. Parser, Typprüfung und
  Backend unterstützen Konstruktoren, vorwärts deklarierte Feldtypen, nominale
  Typidentität, verschachtelte Wertkopien, Feldzugriff/-zuweisung und Übergabe/Rückgabe
  von Strukturen. Negative Referenzen prüfen unbekannte/doppelte Felder, fehlende
  Initialisierung, Typverwechslungen, unveränderliche Zugriffspfade, temporäre
  Zuweisungsziele und zyklische beziehungsweise zu große/tiefe Layouts.
  Byte-Mutationen prüfen zusätzlich Bindungs- und Typreferenzgrenzen. Native
  Referenzen sichern Kopierunabhängigkeit, Konstruktor-Auswertungsreihenfolge und
  Laufzeitfehler bei globalem Feldzugriff vor Initialisierung ab. Alle fünf
  Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC AddressSanitizer
  einschließlich instrumentierter erzeugter Programme. `examples/language/motion.phys`
  berechnet einen Wurf mit konstantem Schwerefeld in 100 Schritten und besteht
  seine Referenz gegen die analytische Lösung; der Anfangszustand bleibt als
  unabhängiger Wert erhalten. Das Beispiel wurde auch aus dem installierten SDK
  mit automatisch gefundenem Compiler gebaut und ausgeführt. Die Experiment-ABI und
  App-Anbindung sind damit noch nicht implementiert.

- Natives skalares Sprachbackend (2026-09-20): `physimc --emit-c` und
  `physim_add_program` erzeugen nach statischer Prüfung echte C17-Programme.
  `language_native` baut und startet eine Referenz mit Seiteneffekten, benannten
  Argumenten, Kurzschlusslogik, gegenseitiger Rekursion, Schattenvariablen,
  Strings, Fließkommarechnung und Schleifen bis an beide Int64-Grenzen.
  70.000 Byte lange String- und Zahlenliterale werden zusätzlich nativ geprüft.
  14 Fehlerprogramme prüfen Überläufe, Nullteiler, nicht endliche Fließkommazahlen,
  globale Zugriffe vor Initialisierung, Rekursionstiefe und Assertions mit Status 70
  und ursprünglicher Quelldiagnose. Ein nachträglicher Typfehler stoppt den
  CMake-Build und erhält das vorherige vollständige generierte C. MSVC Release,
  Clang Debug und MSVC AddressSanitizer bestehen; ASan instrumentiert auch die
  erzeugten nativen Programme. Das Beispiel `examples/language` läuft im
  Entwicklungsbaum und aus einem installierten SDK ohne manuelle C-Bearbeitung
  mit den Ergebnissen `9`, `1`, `45`. Dies nimmt eigenständige skalare Programme ab,
  noch nicht die Experiment-/Analyse-ABI, App-Anbindung oder LANG-003 bis LANG-007
  insgesamt. Laufzeitvertrag und Grenzen: `docs/language.md`.

- Skalare Typprüfung und Compiler-CLI (2026-09-20): `language_checker` prüft
  `Int64`-/`Float64`-/`Bool`-/`String`-Typen, Literalinferenz, beide Int64-Grenzen,
  lexikalische Bindungsreferenzen, unveränderliche Werte, Vorwärtsaufrufe und
  gegenseitige Rekursion, benannte/positionale Argumente, Rückgabepfade und
  Schleifenkontrolle. Negative Referenzen prüfen gezielt semantische Fehler nach
  erfolgreichem Parsing. AST-Tiefe, Pufferkapazität und Arbeitsbudget werden
  absichtlich überschritten; alle Präfixe und Einzelbyte-Mutationen eines kleinen
  Moduls prüfen Diagnosegrenzen. `language_cli` prüft das tatsächliche Werkzeug
  `physimc --check` mit gültigem Beispiel, Syntax-/Typfehlern, leerer und zu großer
  Datei, fehlender Datei, Rückgabecodes sowie UTF-8-Pfaden mit Leerzeichen/Umlauten.
  Alle vier Sprachtests bestehen unter MSVC Release, Clang Debug und MSVC
  AddressSanitizer. Der Compiler erzeugt noch keinen nativen Code; weitere
  Werttypen, Laufzeitregeln und LANG-003 bis LANG-007 bleiben offen.

- Einrückungssyntax (2026-09-20): Funktions-, Bedingungs- und Schleifenblöcke
  verwenden jetzt verbindlich `:` mit Zeilenumbruch und Einrückung statt `{}`.
  Parserreferenzen prüfen AST-Zuordnung bei mehreren Dedents und äußerem `else`,
  mehrzeilige Aufrufe, Kommentare, LF/CR/CRLF, Dateiende ohne letzten Zeilenumbruch,
  abweichende Blockbreiten, falsche Dedents, Tabs, leere/einzeilige Blöcke und
  abgewiesene Klammersyntax. Tief verschachtelte Einrückungsblöcke sowie sämtliche
  Präfixe/Einzelbyteersetzungen eines gültigen eingerückten Moduls prüfen weiterhin
  die Ressourcen- und Speichergrenzen. Lexer- und Parsertests bestehen unter
  MSVC Release, Clang Debug und MSVC AddressSanitizer. Typprüfung, Backend und
  App-Anbindung bleiben offene Schritte; dies ist keine Abnahme der gesamten Sprache.

- Sprachfrontend (2026-09-20): erster Lexer und CTest `language_lexer`.
  Referenzen für Schlüsselwörter, Funktions-/Typnotationssyntax, Zahlen,
  längste Operatoren, UTF-8-Strings, verschachtelte Kommentare, CR/LF/CRLF,
  stabile EOF-/Fehlerdiagnosen und explizit begrenzte Puffer. Sämtliche
  Bytepositionen eines Beispielquelltexts werden mit allen 256 Bytewerten
  verändert; zusätzlich werden alle gekürzten Präfixe auf Bereichsgrenzen und
  Fortschritt geprüft. MSVC Release, Clang Debug (Ninja) und MSVC
  AddressSanitizer bestehen. Die ClangCL-Integration in Visual Studio fehlt
  lokal; der vorhandene Clang-Treiber wird über Ninja verwendet.
  Diese erste Prüfung allein nimmt weder Parser noch Typsystem oder ausführbare
  Sprachprogramme ab. Die nachfolgende Parserprüfung wird separat dokumentiert.
  LANG-001 bleibt semantisch unvollständig. Details: `docs/language.md`.

- Optionales libFuzzer-Ziel (Dokumentationsabgleich 2026-09-20):
  `PHYSIM_BUILD_FUZZERS` und `physim-protocol-libfuzzer` existieren bereits.
  Die vorhandenen lokalen Buildprotokolle bestätigen das Linken; die
  Kampagnenprotokolle zeigen einen Windows-ASan-Startfehler statt einer
  erfolgreichen Kampagne. `docs/fuzzing.md` trennt jetzt diese offene
  Laufzeitabnahme von den deterministischen IPC-/Dateileser-Tests.

- Report-Mutationen (2026-09-19): 7.402 deterministische Fälle mit Linien-, Punkt-
  und Histogrammkurven, UTF-8 und Tabelle. Alle Kürzungen/Bitflips ohne CRC-Anpassung
  werden ausdrücklich abgewiesen; CRC-korrigierte Nutzdatenmutationen erreichen die
  Inhaltsprüfung. 2.320 akzeptierte Berichte lassen sich über die öffentliche API
  vollständig neu aufbauen. Jede der sechs Allokationen des gültigen Berichts wird
  separat zum Scheitern gebracht; Ausgabeerhalt und vollständige Freigaben bestehen.
  MSVC Release, Clang Debug und AddressSanitizer bestehen. Der neue CTest
  `report_mutations` und die Replay-Funktion sind in `docs/fuzzing.md` dokumentiert.
  Die Kampagne ergab keinen weiteren Leserfehler. Maximale Berichte und
  abdeckungsgeführte Langzeitkampagnen bleiben separate Prüfungen.

- Run-Datei-Mutationen (2026-09-19): 7.554 reproduzierbare Fälle mit vollständigen
  Kürzungen/Bitflips, extremen Chunklängen und Nutzdatenmutationen samt gültiger CRC.
  Ein NaN im letzten Kanal reproduzierte eine Teilausgabe aus `ps_run_next`.
  Messpunkte werden nun vollständig validiert, bevor Zeit und Werte übernommen
  werden; Fehler/EOF erhalten die Ausgabeparameter. MSVC Release, Clang Debug und
  AddressSanitizer bestehen alle Mutationen (2.059 erfolgreiche Opens, 3.858 gültige
  Messpunkte). Bestehende Core-/Reihentests bestehen unter MSVC. CTest, Reproduktion
  und Grenzen sind dokumentiert. Formatversion und Verhalten gültiger Dateien bleiben
  erhalten. Report-Mutationen und abdeckungsgeführte Langzeitkampagnen bleiben offen.

- IPC-Mutationskampagne (2026-09-19): neuer CTest `protocol_mutations` mit 183.462
  reproduzierbaren Snapshot-/Frame-Eingaben, vollständigen Kürzungen und Bitflips,
  Vierbyte-Mutationen und festen Extremwerten. Der Verbrauchspfad prüft Längen vor
  der Headeraddition und verhindert deren Überlauf; `ps_wire_peek` verändert
  Ausgaben nur bei einem vollständigen Frame. Invarianten prüfen Fehleratomizität,
  gültige dekodierte Daten, kanonischen Roundtrip, Restbytes und Sequenzen.
  MSVC Release, Clang Debug und AddressSanitizer bestehen; vorhandene Szenen- und
  Runner-Isolationsreferenzen bestehen ebenfalls unter MSVC. Reproduktion und
  Grenzen sind in `docs/fuzzing.md` beschrieben. Strukturierte Dateileser-Kampagnen
  und abdeckungsgeführtes Langzeit-Fuzzing bleiben offen.

- UTF-8-Zwischenablage (2026-09-19): Ein reproduzierter Fehler im vendorten
  Nuklear-Pastepfad vermischte Byte- und Zeichenzahlen. Der lokale Patch verwendet
  UTF-8-Bytes für den Text und Unicode-Skalare für Cursor/Undo; eine ersetzte Auswahl
  wird mit einem Undo-Schritt wiederhergestellt. Platzreservierung erfolgt vor dem
  Löschen, sodass Puffer-/Allocator-Fehler Text, Auswahl und Undo erhalten.
  Ungültiges UTF-8 wird abgewiesen, SDL begrenzt Einfügungen auf 262144 Bytes.
  Referenzen mit Umlauten, Eurozeichen, Emoji, Zeilenumbruch, Undo/Redo, Ersetzung
  und Fehler-Injektion bestehen unter MSVC Release, Clang Debug und AddressSanitizer.
  Der SDL-Eingabe-/Renderer-Test und der vollständige Pendel-App-Ablauf bestehen
  unter Windows. Tests verändern die Systemzwischenablage nicht. Der Patch samt
  Prüfsumme ist in `third_party/README.md` dokumentiert; Undo-Historienlimits bleiben.

- Gemischter Constraint-Graph (2026-09-19): `ps_constraints_resolve_graph`
  iteriert Kontakte und bis zu 256 Distanzgelenke gemeinsam bei weiterhin maximal
  128 Körpern und 512 Kontaktpunkten. Restitution verwendet Anfangsgeschwindigkeiten;
  die Restfehler werden am gemeinsamen Endzustand ausgewertet. Gekoppelte
  Bodenabstützung und Rückprall, knappe Iterationsbudgets, widersprüchliche Gelenke,
  Kontaktprojektion, gleichzeitig ausgeschöpfte Kapazitäten und Fehleratomizität
  bestehen unter MSVC Release, Clang Debug und AddressSanitizer sowie gegen das
  installierte SDK. Ohne Gelenke ist das Ergebnis bitgleich zur bisherigen API.
  Direkte gemeinsame Positions-/Orientierungsprojektion und Warmstart bleiben offen.

- Distanzgelenk (2026-09-19): `ps_distance_joint_resolve` ergänzt eine bilaterale
  Geschwindigkeitsbedingung mit körperfesten Ankern, Drehimpulsen, statischer Welt
  und einstellbarer Driftkorrektur. Impuls, Längenfehler und Geschwindigkeitsrestfehler
  sind abrufbar. Analytische Impuls-/Drehmomentreferenzen, Kreisbewegung mit
  Zeitschrittverfeinerung, statische Partner und Fehleratomizität bestehen unter
  MSVC Release, Clang Debug und AddressSanitizer sowie separat gegen das installierte
  SDK. Gemeinsame Iteration mit anderen Gelenken/Kontakten und automatische
  Integrationssteuerung bleiben offen; Stabilisierung kann Energie zuführen.

- Kontaktgraph (2026-09-19): `ps_contacts_resolve_graph` löst bis zu 128 Körper
  und 512 Kontaktpunkte gemeinsam, einschließlich statischer Weltkontakte,
  Restitution, Coulomb-Reibung und iterativer Positionskorrektur. Impulse und
  verbleibende Normalgeschwindigkeits-/Projektionsfehler sind abrufbar.
  Kontaktkette, gestützter Dreier-Stapel, Reihenfolgevergleich, unzureichende
  Iterationszahl, volle Kapazität und Fehleratomizität bestehen unter MSVC Release,
  Clang Debug und AddressSanitizer. Der Paarvergleich ist bei Reibung bitgleich;
  bestehende Boxstoß- und Bodenkontaktreferenzen bestehen unter MSVC Release.
  Der Graph-Test besteht separat gegen das installierte SDK. Kontakte erzeugt
  weiterhin der Aufrufer; Warmstart, Gelenke und automatische Verwaltung bleiben offen.

- CCD-Stoßvorlage (2026-09-19): neue Kugelstoßprojekte verwenden im Vakuum
  standardmäßig den Sweep bis zum Kontakt, Impulsantwort und Restbewegung.
  Geschwindigkeit und Verfahren stehen in den Metadaten. Medien mit Widerstand
  bleiben diskret; explizites CCD mit einem Medium wird abgewiesen.
  Runner-Referenzen mit 100 m/s prüfen jeden Messpunkt bei 50-ms-/3-ms-Schritten,
  korrekte Endposition, Energie und einen Impuls; der diskrete Vergleich tunnelt
  erwartungsgemäß. MSVC Release und Clang Debug bestehen, ebenso die bisherigen
  Medien-/Reibungsreferenzen. Der vollständige App-Ablauf mit neuem Projekt,
  Build, Simulation, Analyse und erneutem Laden besteht unter Windows.
- Kontinuierliche Kugelkontakte (2026-09-19): `ps_sweep_spheres` und
  `ps_sweep_sphere_plane` bestimmen den ersten Kontakt entlang linearer
  Verschiebungen. `ps_aabb_swept_sphere` deckt den gesamten Weg für die Broad
  Phase ab. Durchtunneln, Streifkontakt, Anfangsüberlappung, bewegte Partner,
  Intervallgrenzen, lange Wege/kleine Ziele und Fehleratomizität bestehen mit
  MSVC Release, Clang Debug und AddressSanitizer. Eine separate MSVC-Referenz
  prüft den elastischen Rückprall samt Restzeit und Energie. Der vollständige
  Sweep-Test besteht auch separat gegen das installierte SDK. Beschleunigte
  Bahnen, Box-CCD und eine automatische Mehrkörper-Ereignissteuerung bleiben offen.
- Broad Phase (2026-09-19): `physim/collision.h` erzeugt konservative Hüllboxen
  für Kugeln/orientierte Boxen und deterministisch sortierte Kandidatenpaare.
  Feste Arbeitspuffer, bis zu 1024 Hüllboxen, keine Allokation; zu kleine
  Ausgabepuffer liefern die benötigte Paarzahl ohne Teilausgabe. Vergleiche mit
  vollständigen Paarprüfungen, 523.776 dicht gepackte Paare, rotierte Eckpunkte,
  Kapazitäts-/Fehlerfälle und Boxkontakt-Abdeckung bestehen unter MSVC Release,
  Clang Debug und AddressSanitizer. Die API ist im Header-Browser verfügbar.
  Der SDK-Neubau mit verschobenem Installationspfad besteht für alle acht
  Vorlagen; der separat gebaute Prüfer ruft die neue Broad-Phase-API auf.
  Der Dokumentations-Fenstertest lädt alle Themen einschließlich des neuen Headers.
  Exakte Kontakte, Kollisionsfilter, bewegte Hüllvolumen und Mehrkörperlösung
  bleiben separate Schritte.
- Szenentastatur (2026-09-19): Alt-Kürzel für Orbit, Pan, Zoom, Standardkamera,
  zyklische Auswahl eingeblendeter Einträge und Ausblenden der Auswahl. Die
  Auswahl überspringt Formfilter, leere Labels und Alpha 0 und folgt Objekt-IDs
  beim Umsortieren. SDL-Ereignistests prüfen Aktionen, Zoom-/Neigungsgrenzen,
  Tastenwiederholung, leere Szenen und die Abgrenzung zu Editor, Analyse und
  Ctrl+Alt. MSVC Release und Clang Debug bestehen den gesamten Maus-/Tastaturablauf
  auch mit eingestreuten Desktop-Ereignissen. Auswahl und Kameradarstellung wurden
  visuell geprüft. Der Ablauf besteht auch aus der neuen portablen Installation.
  Vollständige Tastaturbedienung der App bleibt offen.
- Installations-/SDK-Prüfung (2026-09-19): `tools/verify-sdk.cmake` installiert in
  einen neuen Pfad mit Leerzeichen/Umlaut, verschiebt das SDK und baut alle acht
  Vorlagen sowie die Analyse aus den installierten Quellen. Alle öffentlichen
  Header werden einzeln kompiliert. Installierte Runner erzeugen je 201 Samples
  und einen Bericht; ein separat gebauter Leser prüft Zeitraster, endliche Werte,
  vollständigen Laufabschluss und lesbaren Bericht. MSVC Release und Clang Debug
  bestehen. Die CI führt diese Prüfung nun für Windows/Linux aus und archiviert
  Protokolle; der Linux-App-Test umfasst auch die Auftriebsvorlage.
  Lokal ist keine WSL-Distribution oder Docker-Laufzeit verfügbar. Eine tatsächlich
  ausgeführte Linux-Abnahme und ein Test auf sauberem Zielsystem bleiben offen.
- RK45-Diagnosen (2026-09-19): additive Schnittstelle
  `ps_ode_integrate_diagnosed` meldet Abbruchursache, Zeitpunkt, Komponente und
  Stufe in einem Objekt des Aufrufers. Die bisherige Schnittstelle und ihre
  Fehleratomizität bleiben erhalten. Alle Diagnosezweige sowie numerische
  Referenzen bestehen unter MSVC Release, Clang Debug und AddressSanitizer.
  Der separat gegen das installierte SDK gebaute Numeriktest und die
  Pendel-Referenzläufe mit RK4, RK45 und Verlet bestehen ebenfalls.
  Strukturierte Diagnosen für andere Bibliotheksbereiche bleiben offen.
- Gemeinsame Zeilenauswahl (2026-09-19): `ps_series_select` filtert 1–32
  ausgerichtete Reihen über einen dimensionslosen Selektor, zum Beispiel
  Sensorstatus 1. Zeit und Messwerte erhalten gemeinsam eine neue Samplezuordnung;
  separate Auswahlen werden nicht versehentlich kombiniert. Die Standardanalyse
  nutzt die API für gültige Sensorpunkte. Blockgrenzen, Statistik/Ableitung über
  Messlücken, Resampling-Zuordnung, CSV, leere/vollständige Auswahlen, 32 Spalten,
  Lebensdauer, Scratch-Limit und Handle-Limit bestehen mit MSVC Release und
  Clang Debug. Die Sensor-Referenz prüft die tatsächlichen Berichtspunkte,
  einschließlich Ausdünnung über 2.048 Punkte, Totalausfall und Einzelmessung.
  Reihen-, Resampling- und Auswahltests bestehen unter AddressSanitizer; der
  Auswahltest wurde separat gegen das installierte SDK gebaut und ausgeführt.
  Der Sensorbericht besteht mit Runner und Analysemodul der portablen Ausgabe.
  Die Auswahl entfernt Zeilen; explizite Gültigkeitsmasken bei unveränderter
  Zeilenanzahl bleiben offen. Ableitung/Integral verbinden behaltene Punkte.
- Szenentransparenz (2026-09-19): RRGGBBAA-Alpha wird für Meshes ausgewertet;
  opake Geometrie schreibt Tiefe, transparente Dreiecke werden von hinten nach
  vorn gemischt. Alpha 0 zeichnet keine Geometrie/Beschriftung und lässt sich
  nicht anklicken. GPU-Pixelprüfungen für Mischwerte, zwei transparente Ebenen,
  vertauschte Objektliste, opake Verdeckung und Auswahl bestehen in Perspektive
  und Orthografie unter MSVC Release und Clang Debug. Beide Builds bestehen
  außerdem Diagrammbedienung und UI-Rendering. Halbtransparente Box und Kugel
  wurden visuell geprüft. Der Renderer-Test besteht auch aus der frisch
  installierten portablen Ausgabe. Sich durchdringende Flächen können Sortierartefakte
  zeigen; eine ordnungsunabhängige Transparenz ist noch nicht implementiert.
- Szenenauswahl (2026-09-19): Linksklick trifft die vorderste tatsächlich
  gezeichnete Dreiecksgeometrie; Beschriftungen werden über ihre Textfläche
  gewählt. Inspektoranzeige und Ausblenden der Auswahl folgen vorhandenen IDs.
  Sieben Mesh-Grundformen in perspektivischer/orthografischer Ansicht,
  Tiefenverdeckung, Hintergrund, ausgeblendete Vektoren und ungültige Koordinaten
  bestanden im GPU-Test unter MSVC Release und Clang Debug. Der Maustest prüft
  Kugel, Beschriftung, Hintergrund und Umsortieren; die Bedienung wurde visuell
  geprüft. Clang bestand auch den Ablauf mit eingestreuten Desktop-Ereignissen.
  Der zusätzliche Fall „umsortieren, dann Auswahl ausblenden“ und der vollständige
  Ablauf aus der portablen Installation bestanden unter MSVC Release.
- IDs in Vorlagen (2026-09-19): alle acht Standardexperimente vergeben feste
  IDs nach Objektbedeutung, auch bei bedingt sichtbaren Sensoren und Pfaden.
  Kurzlebige Kontaktpunkte/-impulse bleiben anonym. Neue SDK-Konstruktoren
  `ps_scene_add_id`, `ps_scene_polyline_id` und `ps_scene_label_id` prüfen
  ID-Duplikate ohne Teiländerung der Szene oder ihres Punktpuffers.
  481 Szenen je Modul (3.848 insgesamt) bestehen mit MSVC Release und Clang
  Debug, einschließlich tatsächlicher Positionsänderungen in der Objektliste.
  Zehn physikalische Referenz-/Vorbereitungstests bestehen weiterhin.
  Der Szenentest besteht unter AddressSanitizer. Der ID-Test bestand erneut
  mit allen acht Modulen aus der frisch installierten portablen Ausgabe.
- Objekt-IDs / ABI 3 (2026-09-19): optionales `ps_object.id`, eindeutige nichtnull
  IDs pro Szene und explizite Übertragung in IPC 3 (172 Bytes pro Objekt).
  Die Sichtbarkeit folgt vorhandenen IDs bei Reihenfolge- und Formänderungen;
  entfernte IDs werden vergessen, ein neuer Lauf setzt die Auswahl zurück.
  Anonyme Einträge behalten die Positionszuordnung. Snapshot-Rundlauf,
  doppelte IDs, Mausklicks und Umsortieren bestanden unter MSVC Release und
  Clang Debug. Der sichtbare Szenenausschnitt bleibt beim Umsortieren pixelgleich.
  Runner- und Analyseprüfungen weisen ABI-2-Module ab; Pendellauf und Analyse
  bestehen mit neu gebauten ABI-3-Modulen. `.psrun` und `.psreport` bleiben
  unverändert. Vorhandene Experiment- und Analysemodule müssen neu gebaut werden.
  Alle 51 Tests ohne Display bestanden nach dem Versionswechsel. Die beiden
  Szenentests bestanden unter AddressSanitizer; der vollständige Export- und
  Sichtbarkeitsablauf bestand auch aus der frischen portablen Installation.
- Szeneneinträge (2026-09-19): einzelne Listenpositionen ausblenden und wieder
  einblenden, zusätzlich zu den vorhandenen Formfiltern. Bewegung erhält die
  Auswahl; neuer Lauf, geänderte Anzahl oder Formenfolge setzen sie zurück.
  Randpositionen 0/31, Bewegung und Strukturwechsel bestanden im Komponententest;
  Mausklicks bestanden unter MSVC Release sowie Clang Debug, auch mit Störereignissen.
  Ein unabhängiger Bildvergleich findet nach dem Ausblenden keine roten Kugelpixel
  mehr (vorher 637), bei unverändert 810 blauen Kugelpixeln. Die Bedienung wurde
  visuell geprüft. Dauerhafte Objekt-IDs sind weiterhin offen.
- Diagramm-Testeingang (2026-09-19): fremde Maus- und Fokusereignisse können
  künstliche Zoom-/Drag-Ereignisse überlagern. Ein reproduzierbarer Störlauf
  scheiterte vor der Korrektur in Stufe 2 und bestand danach vollständig.
  Ausschließlich im Plot-Testmodus werden eigene markierte Mausereignisse
  verarbeitet; die normale App behält ihre Eingabebehandlung. Neuer CTest
  `plot_input_isolation` streut Mausbewegungen, Loslassen und Fokusverlust ein.
  Normaler und gestörter Ablauf bestanden jeweils dreimal unter MSVC Release
  und Clang Debug. Dies belegt den behobenen Störpfad, nicht jede mögliche
  Ursache früherer sporadischer UI-Testfehler.
- Resampling-Verfahren (2026-09-19): `ps_series_resample` bietet zusätzlich zur
  linearen Interpolation Nearest (bei Gleichstand früherer Punkt) und Previous
  (letzten Wert halten). Alle Verfahren prüfen den vollständigen Quellbereich,
  extrapolieren nicht und liefern Reihen mit der Zuordnung des Zieldatensatzes.
  Referenzen mit 3.073 Zielpunkten, Blockgrenzen, exakten Quellpunkten,
  Gleichständen, Extremwerten, Speicherquoten und Datensatz-Lebensdauer bestanden
  mit MSVC Release, Clang Debug und AddressSanitizer; die lineare API bleibt
  kompatibel. Derselbe Resampling-Test wurde separat gegen die installierten
  SDK-Header und Bibliothek kompiliert und erfolgreich ausgeführt.
- SVG-Ausschnitt (2026-09-19): eigener Knopf und SDK-Funktion mit expliziten
  Achsengrenzen. SVG-Clip-Pfade begrenzen Kurven, ohne Titel und Legende abzuschneiden;
  Achsenoffsets erhalten lesbare Teilstriche bei großen Grundwerten. Berichtstest
  und vollständiger Mausklick-/Exportablauf bestanden unter MSVC Release und Clang
  Debug; der Berichtstest bestand auch unter AddressSanitizer. Unabhängige
  XML-Prüfungen bestätigen Referenzgeometrie, Clipping, Legende und Achsenwerte.
  Auch der Exportablauf aus der frischen portablen Installation bestand.
  Die Bedienung wurde im App-Bild geprüft. Eine visuelle Prüfung der SVG-Datei
  im Browser blieb wegen fehlgeschlagener Browser-Verbindung aus.
- PNG-Ausschnitt (2026-09-19): eigener Exportknopf für die aktuellen Achsengrenzen,
  mit Titel, Einheiten, Legende und wählbarer Auflösung. Mausklick und PNG-Ausgabe
  bestanden unter MSVC Release sowie Clang Debug. Clang scheiterte zunächst in
  Zoom-Stufe 34; der isolierte Lauf nach dem letzten Build bestand. Unabhängige
  Pixelprüfung bestätigt eine durchquerende Linie mit beiden Endpunkten außerhalb,
  einen an allen Grenzen beschnittenen Balken und ausgeschlossene Streupunkte.
  Ungültige Grenzen erzeugen keine Datei. Fünf vollständige Standardexporte bleiben
  pixelgleich; die neue Bedienung wurde visuell geprüft. Der Plot-Workflow bestand
  auch aus der frischen portablen Installation. SVG-Ausschnitte sind offen.
- PNG-Kompression (2026-09-19): zeilenweises DEFLATE mit statisch eingebundener
  zlib 1.3.2, ohne zusätzliche Codec-DLL. Neun Exporte einschließlich aller vier
  Auflösungen sind nach unabhängiger Pillow-Dekodierung pixelgleich zur vorherigen
  Version. Die fünf Standarddiagramme schrumpfen von je 12.270.651 Byte auf
  71.791–227.240 Byte; das größte Testbild von 49.021.251 auf 622.412 Byte.
  CRC, Zlib-Datenstrom, Randgrößen, Zeilenpadding und Zufallsbilddaten wurden
  unabhängig mit Python geprüft. MSVC Release: PNG, Decoder, UI-Rendering und
  Plot-Workflow bestanden. Clang Debug: PNG, Decoder und UI-Rendering bestanden;
  Plot-Workflow scheiterte zunächst in Zoom-Stufe 5 und bestand isoliert erneut.
  PNG-Test unter AddressSanitizer und Plot-Workflow der frischen portablen
  Installation bestanden. Linux wurde lokal nicht ausgeführt.
- PNG-Größen (2026-09-19): Auswahl von 1200 × 850, 2400 × 1700 (Standard),
  3600 × 2550 und 4800 × 3400 Pixeln direkt am Diagrammexport. Logisches Layout
  und vollständiger Diagramminhalt bleiben erhalten; fünf Standardexporte sind
  bytegleich zur vorherigen Version. Pillow dekodiert alle vier Größen mit den
  erwarteten Maßen. Mausklicks auf die Auswahl, Exporte, ungültige Skalen und
  Größenlimits sowie fortgesetztes UI-Rendering bestanden unter MSVC Release
  und Clang Debug. Auswahl und Export wurden visuell geprüft. Derselbe
  Plot-Workflow bestand auch aus der portablen Installation.
- Hashmap (2026-09-19): neuer SDK-Baustein mit eigenen Byte-Schlüsseln,
  Werten fester Größe, explizitem Allocator und Schlüssel-/Eintragslimits.
  5.000 Referenzoperationen, 64 Schlüssel im selben Bucket, Löschungen,
  Traversierungsschutz und Allokationsfehler bestanden unter MSVC Release,
  Clang Debug und AddressSanitizer. Fünf gezielte Prüfungen einschließlich des
  Dokumentationsfensters bestanden im separaten Release-Build `build-hashmap-release`.
  Dieser Build umgeht die vom laufenden bisherigen Physim-Fenster gesperrte EXE.
  Das Beispiel wurde mit dem SDK-CMake-Modul aus portablen Quellen gebaut und
  für vorhandene/fehlende Schlüssel samt unveränderter Fehlerausgabe geprüft.
  [Verträge und Beispiel](hashmap.md).
- String-Views (2026-09-19): neuer SDK-Baustein für begrenzte Text-/Bytebereiche
  ohne Allokation, mit Vergleich, Suche, Teilbereichen, Trennen, ASCII-Trimmen
  und überlappendem Kopieren. 5.000 Suchreferenzen und alle 65.536 Bytepaar-
  Vergleiche bestanden unter MSVC Release, Clang Debug und AddressSanitizer.
  Drei gezielte Release-Prüfungen einschließlich Dokumentationsfenster bestanden.
  UTF-8 wird byteweise behandelt; Speicherlebensdauer bleibt beim Aufrufer.
  Das Parserbeispiel wurde mit dem SDK-CMake-Modul aus portablen Quellen gebaut
  und mit einem nichtterminierten Puffer sowie unveränderten Fehlerausgaben geprüft.
  [Verträge und Parserbeispiel](string-view.md).
- Dynamische Arrays (2026-09-19): neuer SDK-Baustein mit explizitem Allocator,
  Elementlimit, geometrischem Wachstum, Selbstkopien und unveränderten Daten bei
  Fehlern. 5.000 Referenzoperationen, Fehler-Injektion, Bytebudget, Größenüberlauf
  und benachbarte Arenablöcke bestanden unter MSVC Release, Clang Debug und
  AddressSanitizer. Vier gezielte Release-Prüfungen einschließlich Speicher-API
  und Dokumentationsfenster bestanden. Das Dokumentationsbeispiel wurde mit dem
  SDK-CMake-Modul aus portablen Quellen gebaut und ausgeführt. [Anleitung](array.md).
- Räumliche Kurven (2026-09-19): kubische Bézierauswertung mit Position und
  Parametertangente sowie geometrische Unterteilung ohne Heap-Allokation.
  Analytische Polynomreferenz, 1.000 deterministische Unterteilungen, skalierte
  Tangenten, Endpunkte, Aliasierung und Zahlenextreme sind geprüft. Der
  Mathematiktest bestand mit MSVC Release, Clang Debug und AddressSanitizer.
  Das dokumentierte Szenenbeispiel wurde gegen die portable SDK-Installation
  mit Warnungen als Fehler gebaut und als gültige 33-Punkte-Polyline geprüft.
  [Konventionen und Szenenbeispiel](math.md).
- Auftriebsvorlage (2026-09-19): achte App-Vorlage mit Wasseroberfläche,
  Kraftpfeilen, zwölf Kanälen und vier Analyseplots. Die gemeinsame Analyse
  unterstützt nun auch reine Y-Bewegung. 6086 Runner-Messpunkte prüfen
  gedämpftes Schwimmen, Gleichgewicht, neutrales Schweben, Sinken/Steigen,
  Energiebilanz und RK4-Verfeinerung (Fehlerverhältnis 15,80). Elf gezielte
  Release-Tests bestanden, außerdem die Referenz unter Clang und AddressSanitizer.
  App-Workflow und Dokumentationsfenster bestanden; Simulations- und
  Energieansicht wurden visuell geprüft. Derselbe Workflow bestand aus der
  portablen Installation mit neu gebautem SDK-Projekt. [Anleitung](buoyancy.md).
- Auftrieb (2026-09-19): neue SDK-Funktionen für archimedische Kraft und die
  teilweise eingetauchte Kugel mit Volumen/Schwerpunkt. Unabhängige Quadratur
  über 101 Eintauchtiefen, Schweben, Sinken/Steigen, rückstellende Kraft und
  numerische Fehlerverträge sind geprüft. Elf gezielte Release-Prüfungen
  einschließlich bestehender Mechanikexperimente und Dokumentation bestanden;
  Auftriebs-/Mechaniktests zusätzlich mit Clang Debug und AddressSanitizer.
  Nutzung und Modellgrenzen stehen in [mechanics.md](mechanics.md).
- CRC32 (2026-09-19, PERF-009): unveränderliche 256-Einträge-Tabelle ersetzt
  die bitweise Berechnung. 31 unabhängige zlib-Referenzen bis 8 MiB, Ausrichtung,
  Einzelbytes, Bitfehler und ein historischer Bericht mit bytegleichem Roundtrip
  sind geprüft. 52 Release-Tests, sieben Clang-Tests und beide CRC-Tests unter
  AddressSanitizer bestanden. Der portable SDK-/App-Selbsttest mit Sensorvorlage,
  Build, Simulation, Analyse, Export und erneutem Öffnen bestand ebenfalls.
  Schema/Samples/Abschluss des Referenzlaufs und
  Exporte bleiben unverändert. Lokal etwa 4,6–5,4-facher CRC-Durchsatz und
  58 Prozent kürzere Lesezeit im 16-Kanal-Benchmark; Flush-Regeln bleiben gleich.
  [Nachweise und Grenzen](crc.md).

- UI-Zeichenpuffer (2026-09-19): Vertex-/Indexspeicher bleibt am Grafik-Kontext
  erhalten; kontrolliertes Wachstum und OpenGL-3.3-Orphaning sind implementiert.
  Referenztests vergleichen sämtliche Geometriedaten und Zeichenbefehle gegen
  den vorherigen Konvertierungspfad. Erschöpfung und alle vier Allokationsfehler
  des Referenzablaufs sind einschließlich Wiederverwendung und Freigabe geprüft,
  auch mit MSVC AddressSanitizer. Neun relevante Release-Prüfungen bestehen,
  darunter die acht Fenster-/Grafiktests. Vier gezielte Clang-Debug-Prüfungen
  (Konvertierung, UI-Messung, Plot-Ablauf und Renderer) bestehen ebenfalls.
  Der portable Boxstoß-App-Test baut neue SDK-Module und besteht Simulation,
  Analyse, Export und Wiederöffnung; die Szene wurde visuell geprüft. Der
  reguläre Build unter `build/bin` ist aktualisiert und hat Konvertierungs-,
  UI-Render- und Fenstertest bestanden. Alle sechs Benchmark-Bilder sowie die
  fünf PNG-Exporte sind bytegleich zum bisherigen Build; elf Diagrammbereiche
  sind pixelgleich. Die neue Messstrecke protokolliert CPU-Median/P95/P99,
  Uploadbytes und Pufferallokationen: nach Aufwärmen null statt 2/14/19 je Bild;
  Konvertierungszeit in dichter UI und achtkurvigem Plot lokal etwa 36 Prozent
  geringer. Details und Grenzen: [UI-Rendering](ui-rendering.md).
  PERF-005 ist für Konvertierungspuffer und GPU-Upload umgesetzt; Messung der
  gesamten App-Allokationen sowie vollständige Frame-/Eingabelatenz bleiben offen.

- Leistungsmessung und Berichtsanzeige (2026-09-19): optionaler nativer Benchmark
  für 16-Kanal-Runs, vollständiges Rücklesen, Analyse-Snapshots, Statistik,
  Ableitung und acht Berichtskurven. Rohdaten, Build-/Hostdaten und Fingerabdrücke
  werden archiviert; ein optionaler Vergleich prüft eine 20-Prozent-Schwelle.
  Die Oberfläche verwendet jetzt unveränderliche Kurven-Views statt zweier
  Vollkopien pro Kurve und Frame. Kopier-API, Datenformat und ABI bleiben erhalten.
  Identische Inhalte, Fehlerfälle und Lebensdauer geprüft; Berichtstest auch mit
  AddressSanitizer bestanden. 47 MSVC-Release-Tests einschließlich sieben
  Fensterprüfungen plus der anschließend ergänzte Python-Wrapper-Test bestanden.
  Clang-Debug: 45 von 46 Tests im ersten Gesamtlauf, der Batch-Test mit
  250-ms-Runnerlimit bestand anschließend ohne parallele Last; beide neuen
  Benchmark-Tests bestanden danach ebenfalls. Kein neuer Linux-Lauf.
  Der portable Pendelablauf baut neue SDK-Module und besteht Simulation,
  Pause, Analyse, Exporte und erneutes Öffnen. Fünf PNG-Exporte sind bytegleich
  zum vorherigen Build; die Diagrammbereiche von elf Plot-Screenshots sind
  pixelgleich (Zoom, Pan, Histogramm, Punkte, Sensor und Wiederöffnung).
  Messwerte, Abdeckungsgrenzen und Wiederholungsanleitung:
  [Leistungsmessung](performance.md). PERF-001 bleibt teilweise offen;
  PERF-003 ist für den Anzeigezugriff umgesetzt.

- Speicherverwaltung: Überlauf, Resize-Fehler, bytegenaue Freigaben, Ausrichtung,
  Arena-Erschöpfung und Reset geprüft. Fehler an allen acht Allokationsstellen
  des Berichtablaufs und sieben Stellen beim Laden hinterlassen keine lebenden
  Testblöcke. Fehlgeschlagene Änderungen erhalten den Bericht bytegleich.
  Alle 670 abgeschnittenen Präfixe einer Referenzdatei, SVG-/CSV-Exportfehler,
  echte Datenreihen und Arena-Besitzer geprüft. Beide neuen Tests bestehen auch
  mit MSVC AddressSanitizer und als eigenständige C17-Consumer gegen installierte
  SDK-Header und Bibliothek. Der portable Sensor-Wurf-App-Test baut neue Module
  einschließlich der Speicherverwaltung und prüft Simulation, Analyse, Export und
  erneutes Öffnen. Der F1-Test lädt das neue Thema und `memory.h`.

- Mathematik: Vec2/3/4, subnormale und maximale Komponenten, normalisierte
  Quaternionen, Aliasierung und transaktionale Fehler geprüft. Je 2000 Fälle
  prüfen allgemeine Matrixinversion mit beidseitigen Residuen, Rotationen gegen
  unabhängige Rodrigues-Referenzen sowie Transformationen mit Scherung/Spiegelung.
  Die neue Quaternion-Normalisierung wird auch im Renderer verwendet.
  Ein eigenständiger C17-Consumer gegen die installierten Header und die statische
  Bibliothek besteht dieselben Referenztests. Der portable App-Ablauf baut die
  Boxstoßvorlage mit den ausgelieferten SDK-Quellen einschließlich `math.c`, prüft
  Simulation, Analyse, Exporte und erneutes Öffnen. Der F1-Test lädt auch das neue
  Mathematikthema und `math.h`. Die sieben Fensterprüfungen sind als `display`
  markiert; die Linux-CI startet sie unter Xvfb/Openbox. Tatsächliche Ausführung
  unter Linux bleibt offen, da lokal keine WSL-Distribution vorhanden ist.

- Einstellungen: Dateiformat, Bereichsprüfung, Prüfsumme, jede abgeschnittene
  Dateilänge und Erhaltung bestehender Daten bei Fehlern geprüft. Sieben App-Starts
  im eigenen Testordner prüfen Schriftwahl, Darstellungsschalter, Übernehmen,
  Abbrechen, Standardwerte, Mausziehen beider Panelgrenzen und Wiederherstellung.
  Normale Fenstermaße und maximierter Zustand bleiben erhalten. Ein künstlich
  vorgerückter App-Timer schreibt vor Ablauf des gewählten Intervalls keine Datei,
  danach eine echte Autosave-Datei und plant die nächste Sicherung korrekt.
  Beschädigte Einstellungen bleiben beim Beenden bytegleich erhalten.
  Die Oberfläche und der vergrößerte Code-Editor wurden visuell geprüft;
  das Einstellungsformular bleibt bei 1080 × 740 bedienbar.
  Dieselben sieben Neustarts und der Dokumentationsfenstertest bestanden aus
  dem portablen Paket. Ein dort neu gebautes Boxstoß-Projekt bestand den
  vollständigen App-Ablauf einschließlich Analyse, Export und erneutem Öffnen.

- Box–Box: 12000 deterministische Paare gegen unabhängige Eckpunktprojektionen
  geprüft, darunter 363 Trennungen, die erst eine Kantenkreuzachse erkennt. 6245
  überlappende Paare liefern gültige Oberflächenpunkte, normierte Richtungen,
  unveränderte Ergebnisse bei Wiederholung und dieselben Kontakte mit umgekehrten
  Normalen bei Vertauschung von A/B. Fläche, Kante, Ecke, achtpunktige Fläche,
  Einschluss, nahezu parallele Boxen, Größen von 1e-150 bis 1e150 und transaktionale
  Eingabefehler geprüft. Zufällige Geschwindigkeiten/Rotationen bestätigen Gesamtimpuls
  und Drehimpuls bei reiner Impulsantwort; inelastische Reibungsstöße erhöhen die
  kinetische Energie nicht. Drei echte Runner-Läufe mit jeweils 401 Samples prüfen
  elastischen Stoß (0,36 J), Restitution 0,5 (0,09 J) sowie gedrehten, versetzten Stoß
  mit Reibung (Endenergie 0,171159938192 J). Alle Messpunkte und Metadaten geprüft.
  Der neue App-Ablauf baut die Boxstoßvorlage, prüft vier Kontaktpunkte und zwei Boxen
  im Szenensnapshot, Pause/Step/Stop, Analyse, Export, Laufvergleich und erneutes Öffnen.
  Derselbe Ablauf besteht aus dem portablen Paket mit frisch aus dessen SDK
  gebauten Modulen. Simulation und Analyse wurden bei 1080 × 740 visuell geprüft.

- MSVC 19.38: Release einschließlich 46 CTest-Prüfungen bestanden; Debug vor dem Rendererwechsel geprüft.
- Clang-Cl 19.1.5 mit Ninja: Debug-Build und dieselben 46 CTest-Prüfungen bestanden.
- PNG: unabhängige Dekodierung mit Python-zlib, CRC-Prüfung und Pillow; exakte
  Referenzpixel einschließlich Zeilenpadding, 1 × 1, 8192 × 1 und 1 × 8192 geprüft.
  Der App-Test klickt den PNG-Export und vergleicht den Export aus einer gezoomten
  Ansicht bytegenau mit dem vollständigen Diagramm. Erneutes Exportieren auf einen
  existierenden Pfad wird abgewiesen. Linien, Punkte, Histogramme, große Achsenoffsets
  und acht Kurven mit insgesamt 16384 Punkten werden exportiert. Histogrammhöhen,
  Orientierung, alle acht Legendenmarker und Reduktionshinweis sind geprüft;
  Beschriftungen, lange Legenden und die weiterhin bedienbare App wurden visuell geprüft.
  PNG-App-Test und interner Dokumentationsbrowser bestehen auch im frisch installierten
  portablen Paket. Dessen vollständiger App-Test baut die Unsicherheitsvorlage aus dem
  mitgelieferten SDK und prüft Simulation, Analyse, Export, Laufvergleich und erneutes Öffnen.
- Diagrammbedienung: Zoom um einen vorgegebenen Anker, Verschieben an die Grenzen,
  minimale Ausschnittgröße, extreme Achsenwerte und Linien-Clipping mit numerischen
  Referenzen geprüft. Der App-Test injiziert Mausbewegung, Mausrad und Mausklicks:
  Linien-, Punkt- und Histogrammzoom, Ziehen, alle drei Zoomschaltflächen,
  unabhängige Diagramm-/Kanalansichten und Rücksetzen beim Öffnen eines Berichts.
  Mausrad außerhalb des Diagramms ändert den Ausschnitt nicht. CSV aus gezoomtem
  Histogramm bleibt bytegleich mit dem vollständigen Export. Ungültige Sensorzeilen
  bleiben auch beim Zoom unsichtbar. Kleine Schwankungen um große Achsenoffsets,
  abgeschnittene Kurven und Bedienelemente bei 1080 × 740 visuell geprüft.
  Vollständiger App-Ablauf mit Build, Simulation, Analyse, Export und Laufvergleich
  der Unsicherheitsvorlage bestanden. Vorschau-Reduktion und Statistik bleiben
  unabhängig vom gewählten Ausschnitt; Exporte umfassen den vollständigen Berichtsplot.
  Diagrammbedienung und integrierter Dokumentationsbrowser bestehen auch aus dem
  frisch installierten portablen Paket.
- Sensoren: Verteilungsmomente, 100000 uniforme/normale Ziehungen, Einheiten,
  Zeitraster, verspätete Abfragen, übersprungene Indizes, Drift/Offset, Quantisierung,
  Standardunsicherheit, Ausfallrate und transaktionale Fehler geprüft. Über 20000
  Rasterplätze bleiben gemeinsame Rauschwerte auch bei zusätzlichen Abfragen,
  ausgelassenen Indizes und geänderter Ausfallwahrscheinlichkeit identisch.
  Drei reale 30-Sekunden-Läufe mit je 6001 Modellzeilen prüfen normalen, idealen
  und vollständig ausgefallenen Sensor bei bitgleicher Physik. Alle gültigen
  Exportzeilen, Statuszahlen, Fehlerkennzahlen und die auf 2048 Punkte reduzierte
  Vorschau stimmen mit Referenzwerten überein. Einzelmessung, falsche Statuswerte,
  falsche Statuseinheit und negative Standardunsicherheit sind ebenfalls geprüft.
  Batch-Endwerte mit Status 0/2 werden abgewiesen, Status 1 wird korrekt aggregiert.
  Vollständiger App-Ablauf mit Unsicherheitsvorlage, Analyse, Export, Laufvergleich
  und erneutem Öffnen bestanden; Sensorplot, Messdatenvorschau und Statuszählung
  bei 1080 × 740 visuell geprüft. Anleitung und öffentlicher Header sind offline
  über die interne Dokumentation verfügbar; interne Verweise auf beide Ziele
  sowie ihre Darstellung wurden geprüft. Derselbe vollständige App-Ablauf besteht
  aus dem portablen Paket mit frisch aus dessen SDK gebauten Modulen. Der normale
  Referenzsensor liefert 2857 gültige Werte, 144 Ausfälle und 3000 nicht fällige Zeilen.
- Parallele Laufserien: dieselben 256 Würfe mit einem und vier Runnern liefern
  bitgleiche Endwert-CSV, Histogrammwerte und Statistik. Acht Wiederholungsläufe
  verwenden acht Runner; ein einzelner längerer Lauf prüft die Begrenzung auf
  die tatsächliche Laufzahl und das abschnittsweise Einlesen. Eine echte
  Vier-Prozess-Barriere erzwingt gleichzeitige Ausführung; Lauf 1 darf erst nach
  den Journal-Einträgen von Lauf 2–4 enden. Ergebnisreihenfolge, separate relative
  Dateien, Teilergebnisse mit Indexlücken und monotone Fortschrittszahlen geprüft.
  Nach Abbruch, Prozessfehler und Zeitlimit bei starkem stdout-Verkehr bestätigen
  Betriebssystemabfragen das Ende sämtlicher gestarteter Test-Runner.
  App-Abläufe mit zwei, vier und acht Runnern prüfen den Parallelitätsregler, erneuten
  Start, Abbruch und das Schließen während einer aktiven Serie über SDL-Ereignisse.
  Die portable CLI liefert auch für die acht höchsten 64-Bit-Seeds bei einem
  beziehungsweise acht Runnern bytegleiche CSV-Dateien. Fortschrittsanzeige und
  Abbruchzustand bei 1080 × 740 visuell geprüft. Der Acht-Runner-Ablauf bestand
  aus dem portablen SDK-Paket mit frisch gebautem Experiment; die korrigierte
  Balkenfüllung ist im aufgenommenen Fortschrittsbild sichtbar.
- Monte Carlo: 256 reale Vakuumwürfe über je 201 Samples liefern für x(1 s)
  einen Mittelwert von 1,00946 m und eine Streuung um den theoretischen Mittelwert
  von 0,15023 m (Soll: 1 m bzw. 0,15 m). Acht komplette Wiederholungsläufe
  reproduzieren alle Modell- und Sensorkanäle bitgenau; jeder Punkt stimmt mit
  der analytischen Bahn und Energiebilanz überein. Andere Seeds ändern die Werte.
  Statistik/Typ-7-Quantile/Normal-KI gegen Zahlenreferenzen geprüft, ebenso
  konstante Daten, Einzelwert, nichtendliche/überlaufende Werte, fehlender Kanal,
  wechselnde Einheit, existierender Ausgabeordner, Prozessabsturz, Zeitlimit und
  Abbruch eines hängenden Runners. App-Ablauf mit realem Build, 256 Läufen,
  Ergebnisanzeige, interner Anleitung und Abbruch per SDL-Ereignissen bestanden;
  Einrichtung, Histogramm und KI-Tabelle bei 1080 × 740 visuell geprüft.
  Die portable CLI verarbeitet auch die drei höchsten 64-Bit-Seeds ohne
  Rundung/Überlauf und erhält konstante Sollbahnwerte.
  Vollständiger App-Ablauf der Unsicherheitsvorlage mit Analyse, Export, Vergleich
  und erneutem Öffnen sowie der Monte-Carlo-Ablauf bestanden aus dem portablen SDK
  mit frisch gebauten Modulen. Sollbahn und Modellbahn bei 1080 × 740 visuell geprüft.
- Feder–Masse–Dämpfer: vier reale Läufe für ungedämpfte, unterkritische, kritische
  und überkritische Bewegung über je zehn Sekunden/2001 Samples gegen geschlossene
  Lösungen bestanden. Kräfte, integrierte dissipierte Arbeit, Energie und Bilanz
  geprüft; Zeitschritthalbierung bestätigt RK4-Konvergenz. Ungültige Schritte und
  Ankerdurchquerung verändern Zustand und Messwerte nicht; Reset reproduziert den
  folgenden Schritt. Alle Punkte der drei Energiekurven sowie Bilanzkennzahl und
  gewählter Manifestkanal geprüft. App-Ablauf mit Build, Analyse, Export, Vergleich
  und erneutem Öffnen bestanden; Federdarstellung und Energieplot bei 1080 × 740
  visuell geprüft. Derselbe Ablauf bestand aus dem portablen SDK-Paket mit frisch
  gebautem Experiment und Analyse. Die Bilanzabweichung des zehnsekündigen
  Standard-Referenzlaufs beträgt maximal 3,42e-9 J; der RK4-Fehlerquotient liegt bei 16.
- Autosave: Unicode und Größenlimits, sämtliche abgeschnittenen Präfixe,
  Byte-Mutationen, ungültiger Text bei gültiger CRC sowie fehlgeschlagene
  Schreib-/Umbenennungsoperationen geprüft. Ein App-Prozess sichert beide geänderten
  Editorinhalte und endet mit `_Exit` ohne reguläres Speichern. Neue Prozesse prüfen
  Wiederherstellen und Verwerfen über SDL-Mausereignisse, externe Quelländerungen,
  Schließen ohne Auswahl und den Erhalt beschädigter Sicherungen. Vor der Auswahl
  starten Ctrl+S/F5 weder Speichern noch Build. Dialog bei 1080 × 740 visuell geprüft.
  Der vollständige normale App-Ablauf mit Pendel bestand ebenfalls. Die fünf
  Wiederherstellungsfälle und der Dokumentationsfenstertest bestanden zusätzlich
  aus dem installierten portablen Paket.
- Boxkontakte: orientierte Kugel–Box-/Box–Ebene-Geometrie, Mehrpunktstoß,
  zweidimensionale Coulomb-Reibung bei ungleichen Trägheitswerten, redundante Punkte,
  transaktionale Fehler und 2000 Schritte ruhenden Bodenkontakt geprüft. Die echte
  geneigte Boxvorlage besteht zehn Sekunden mit 2001 Messpunkten gegen Energie-,
  Bodenabstands-, Kontaktimpuls- und Ruhezustandsreferenzen.
  Vollständiger App-Ablauf einschließlich Vergleich und erneutem Öffnen bestanden,
  auch aus dem portablen SDK mit neu gebautem Experiment bei 1080 × 740.
  Die Bildprüfung deckte eine vom Boden verdeckte Box auf; der korrigierte
  Standardblick wird durch einen GPU-Pixeltest und eine erneute Bildprüfung abgesichert.
- Mechanik: Trägheit, rotierte Hauptachsen, Kräfte/Impulse, gyroskopischer Term,
  Quaternionrotation, Konvergenz, elastischer/inelastischer Stoß, Erhaltung von
  linearem Impuls und Drehimpuls bei Berührung, Reibung und Widerstand geprüft.
  Vier echte Stoßläufe prüfen Vakuum, Luft, eigenes Medium und Reibung/Rotation
  über jeweils 401 Samples einschließlich Modellmetadaten. App-Ablauf mit
  Orientierungsmarkierungen, Geschwindigkeitsvektoren und Kontaktpunkt bestanden,
  auch aus dem portablen SDK-Paket mit neu gebautem Experiment bei 1080 × 740.
- Lineares Resampling: analytische lineare/quadratische Referenzen, Bereichsgrenzen,
  vollständige Achsenprüfung, Einheitenfehler, Blockübergänge, extreme Zahlen,
  Lebensdauer und Plattenbudget bestanden. Differenzberichte und volle CSV mit
  teilweise überlappenden, getrennten und nur an einem Zeitpunkt überlappenden
  Laufzeiten gegen Referenzen geprüft; Stoß-Differenzplot in der App visuell geprüft.
  Portables SDK mit Pendel neu gebaut und vollständigen App-Test bestanden;
  dessen Differenz-CSV zusätzlich unabhängig mit linearer Interpolation nachgerechnet.
- Mehrlaufanalyse: verschiedene Zeitraster, Einheitenfehler, volle CSV und
  Ableitungs-/Statistikreferenzen geprüft. Analyseabsturz, Endlosschleife und
  Kompatibilität mit einem historischen ABI-2-Modul separat geprüft.
- Laufverwaltung: Dateiliste und Auswahl über SDL-Ereignisse, Vergleich zweier
  echter Läufe, Projekt erneut geöffnet, frühere Messdaten/Berichte ohne Build
  geladen und Dateifilter geprüft. Unicode-Projektpfad eingeschlossen.
  Derselbe Ablauf bestand aus dem portablen Paket mit der Stoßvorlage bei 1080 × 740;
  Vergleich und wieder geöffnete Berichte wurden visuell geprüft.
- Eigene Analyseberichte: Roundtrip, alle abgeschnittenen Dateipräfixe, 300 Mutationen,
  Einheiten-/Skalenprüfung, Spitzen in reduzierten Linien, Histogrammzählung und Exporte.
  Angezeigte Pendelkurve gegen die vollständige CSV sowie Kennzahlen gegen die Referenz geprüft.
- App-Ablauf mit erzeugten Linien-/Punkt-/Histogrammplots und Tabellen bei 1440 × 940
  und 1080 × 740 geprüft; SVG-/CSV-Exporte über injizierte Mausklicks ausgelöst.
- Portables Paket: 20-s-Pendellauf mit 4003 Punkten und sieben Periodenintervallen,
  Ergebnistabelle und Exporte bestanden. Ein absichtlich beschädigter Bericht wurde
  im Hintergrund abgewiesen; das vorherige Ergebnis blieb in der App erhalten.
- Datenreihen: Blockgrenzen, SI-Algebra, Handle-Lebensdauer, unveränderliche Snapshots,
  zeitliche Zuordnung, Disk-Limit und Recovery geprüft. Abgeleitete Pendelgeschwindigkeit
  und integrierte Position gegen die physikalische Referenz des echten Laufs geprüft.
- Dokumentation: alle mitgelieferten Themen und Header geladen; F1 und Ctrl+F mit
  SDL-Ereignissen sowie Texteingabe und Sprung zur Fundstelle geprüft. Der Leser
  unterstützt einen begrenzten Markdown-Umfang; Tabellen sind derzeit Textzeilen.
- Szenen-API/IPC 2: maximale Payload, alle abgeschnittenen Längen, fehlerhafte
  UTF-8-Texte/Punktbereiche, 5000 deterministische Mutationen und Ablehnung von IPC/ABI 1.
- Native SDL-App: Fenstersmoke und kompletter App-Selbsttest bestanden.
- Neue Oberfläche bei 1440 × 940 und 1080 × 740 visuell geprüft; Such-/Protokoll-
  Kurzbefehle und Navigation zusätzlich über injizierte SDL-Ereignisse geprüft.
- RK45, Verlet, linearer Solver, Bisektion, Minimierung und Einheitenalgebra mit
  analytischen Referenzen und Fehlerfällen geprüft; drei Pendelverfahren im Runner.
- API-2-App-Abläufe mit Pendel, Wurf inklusive Flugbahn und Stoß inklusive Labels
  bestanden; Stoß zusätzlich direkt aus dem portablen SDK-Paket.
- Reales ABI-1-Modul vor Ausführung abgewiesen; Messdatei aus dem vorherigen
  Entwicklungsstand erfolgreich mit dem neuen Runner nach CSV exportiert.
- OpenGL 3.3 Core auf NVIDIA RTX 2080 Ti: GPU-Readbacktests für Verdeckung,
  Projektion, Größenwechsel, Clipping, Draufsicht, Vektoren, rotierte Boxen/Ebenen,
  Punkte, Polylinien und Labelprojektion bestanden.
- SDL-Eingaberegression für mehrere Unicode-Zeichen pro Ereignis und Fokusverlust.
- App-Selbsttest einschließlich `#error`, erkanntem Compilerfehler, Zeilennavigation,
  korrigiertem Build, Pause/Step/Stop und Analyse bestanden.
- Projektpfad mit Leerzeichen und `ä` erfolgreich getestet.
- Fehlerhafte UTF-8-Quelldateien werden abgewiesen, ohne den aktuellen Editorinhalt
  zu ändern; Projektdateien werden erst nach vollständigem Laden gemeinsam übernommen.
- CSV-Felder mit Kommas und Anführungszeichen, CRC-Verfälschung und unbekannte
  Erweiterungschunks sind durch Regressionstests abgedeckt.
- 4001 Messpunkte / 20 s Pendellauf: Energieabweichung max. 3.40e-10 J,
  Periode 2.488805869 s aus sieben Periodenintervallen.
- Zusätzlicher analytischer Referenztest gegen die elliptische Pendelperiode bestanden.
- Screenshots von Editor, Simulation und Analyse visuell geprüft.
- Installiertes portables Windows-Paket mit eigenem SDK: vollständiger App-Test bestanden.
- Streamingtest: 1.000.001 Punkte / 68.001.526 Bytes; Schreiben 16,8 s,
  Analyse 2,4 s auf diesem Rechner (Release). Das ist eine lokale Messung,
  keine garantierte Leistung für andere Hardware.

Die Windows-Desktopautomationsschnittstelle war nicht erreichbar. Die UI wurde
deshalb mit dem eingebauten App-Test und dessen gerenderten Screenshots geprüft;
ein zusätzlicher externer OS-Mausklick-/Tastaturtest steht aus.

## Archivierte Plattformnachweise bis 6. Oktober 2026

Diese Abschnitte wurden vollständig aus der [Plattformprüfung](platform-validation.md)
übernommen, damit beide Seiten im eingebauten Handbuch lesbar bleiben.

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
