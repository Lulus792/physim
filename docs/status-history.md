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
