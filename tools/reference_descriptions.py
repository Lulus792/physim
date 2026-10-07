"""Reviewed German purpose statements for each public C function."""
DESCRIPTIONS = {}


def add(prefix, entries):
    for suffix, text in entries.items():
        DESCRIPTIONS[prefix + suffix] = text


add('ps_', {
    'property_validate': 'Prüft SI-Einheit, UTF-8-Metadaten, geschlossenen T/P-Bereich, konstante Werte oder vollständige endliche Tabellen mit höchstens 64 Punkten je Achse.',
    'property_evaluate': 'Wertet konstante oder begrenzt bilinear interpolierte Materialdaten bei Kelvin/Pa aus und erhält die Ausgabe bei Fehlern. Quellen, Einheit und Gültigkeit bleiben explizite Modelldaten.',
    'log_record_valid': 'Prüft Schweregrad, endliche Modellzeit und terminierte UTF-8-Nachricht mit höchstens 1024 Bytes.',
    'logger_emit': 'Validiert und kopiert eine Meldung, ruft den expliziten Sink synchron auf und gibt dessen Ergebnis zurück. Ein deaktivierter Sink akzeptiert gültige Meldungen ohne I/O.',
    'log_level_name': 'Liefert debug, info, warning oder error als unveränderlichen Bibliothekstext; unbekannte Werte liefern unknown.',
    'experiment_log': 'Schreibt über den Hostlogger mit der aktuellen Simulationszeit. Ein älterer Context ohne Logger-Tail liefert PS_VERSION.',
    'result_string': 'Liefert den lesbaren Namen eines Rückgabewerts; der Text gehört der Bibliothek.',
    'convert': 'Konvertiert value zwischen dimensionskompatiblen Einheiten und schreibt das Ergebnis nach output.',
    'rng_seed': 'Initialisiert einen PCG32-Zufallsstrom für einen reproduzierbaren Seed.',
    'rng_u32': 'Zieht die nächste vorzeichenlose 32-Bit-Zufallszahl und verändert den Stromzustand.',
    'rng_uniform': 'Zieht die nächste gleichverteilte Double-Zahl in (0,1).',
    'rng_normal': 'Zieht eine normalverteilte Zahl mit Mittelwert und Standardabweichung.',
    'ode_step': 'Führt einen Euler- oder RK4-Schritt für n Zustandskomponenten aus; state wird bei Erfolg aktualisiert.',
    'symplectic_step': 'Aktualisiert zuerst Geschwindigkeit, dann Position mit symplektischem Euler.',
    'drag_force': 'Berechnet quadratischen Widerstand entgegen der Geschwindigkeit, aus Medium, Widerstandsbeiwert und Stirnfläche.',
    'collide_spheres': 'Einfache Stoßantwort für zwei Partikelkugeln; verändert die Partikel und meldet, ob ein Kontakt behandelt wurde.',
    'close': 'Prüft zwei Zahlen mit kombinierter absoluter und relativer Toleranz.',
    'bezier3_evaluate': 'Wertet eine kubische Bézierkurve bei t aus und liefert Position sowie Ableitung nach t.',
    'bezier3_split': 'Teilt eine kubische Bézierkurve bei t in zwei Kurven mit jeweils eigener Parametrisierung [0,1].',
    'transform_point': 'Transformiert einen Punkt einschließlich Translation und homogener Division.',
    'transform_direction': 'Transformiert eine Richtung ohne Translation.',
    'transform_normal': 'Transformiert eine Normale mit invers transponiertem linearem Anteil; ungeeignete Transformationen werden abgewiesen.',
    'linear_solve': 'Löst A*x=b mit skalierter Pivotisierung; A ist zeilenweise gespeichert, Eingaben bleiben erhalten.',
    'root_bisect': 'Sucht eine Nullstelle einer stetigen Funktion in einem Intervall mit Vorzeichenwechsel.',
    'minimize_golden': 'Sucht ein Minimum einer unimodalen Funktion im vorgegebenen Intervall.',
    'verlet_step': 'Integriert Position und Geschwindigkeit mit Velocity Verlet für eine orts-/zeitabhängige Beschleunigung.',
    'ode_options_default': 'Liefert die Standardtoleranzen und Schrittgrenzen für adaptive Integration.',
    'ode_integrate': 'Integriert ein nichtsteifes ODE-System mit Dormand–Prince 5(4) bis zur Zielzeit.',
    'ode_step_diagnosed': 'Akzeptiert genau einen Dormand–Prince-Schritt in Richtung end; verworfene Versuche ändern den Zustand nicht. Bericht und Diagnose enthalten tatsächliche Zielzeit, nächste Schrittweite und Fehlernorm.',
    'ode_integrate_diagnosed': 'Wie ode_integrate, ergänzt um die konkrete Abbruchursache, Komponente und Stufe.',
    'ode_diagnostic_string': 'Liefert die statische Textbeschreibung einer ODE-Diagnose.',
    'statistics_push': 'Fügt einer mit null initialisierten Statistik einen Wert hinzu; Mittelwert und Streuung werden online aktualisiert.',
    'statistics_stddev': 'Liefert die Stichprobenstandardabweichung der gesammelten Werte; für eine Aussage sind mindestens zwei Werte nötig.',
    'derivative': 'Schreibt Sekantenableitungen dy/dx in out, mit einseitigen Rändern.',
    'trapezoid': 'Berechnet das bestimmte Integral der Werte y über x mit der Trapezregel.',
    'analyze_run': 'Schreibt Standardstatistik, Vorschau und Analysemanifest für einen gespeicherten Lauf.',
    'channel_add': 'Kopiert einen eindeutigen SI-Kanal mit Skala 1 und begrenzten UTF-8-Metadaten ohne Kürzung; liefert Index oder -1 und erhält den Kontext bei Fehlern.',
    'parameter_override': 'Hinterlegt vor create einen endlichen Wert für einen eindeutigen Parameternamen. Der Name muss später definiert werden; der Kontext benötigt die optionale ABI-3-Erweiterung.',
    'parameter_define_unit': 'Definiert SI-Parameterwerte mit deklarierter Anzeigeeinheit. Kopiert Symbol, Skala und Dimensionen in den optionalen ABI-3-Kontext-Tail; Grenzen müssen in der Anzeigeeinheit darstellbar sein. Fehler bewahren Kontext und Ausgabe.',
    'parameter_unit_read': 'Liest die eigene Einheitendeklaration eines Parameters. Alte Kontexte und untypisierte Parameter liefern declared=false und Skala 1; dies behauptet keine Dimensionslosigkeit.',
    'parameter_unit_parse': 'Liest parameter_unit/scale/dimension aus Laufmetadaten. Fehlende Deklarationen bleiben unbekannt; unvollständige, doppelte oder ungültige Felder ergeben PS_CORRUPT. Ausgabe bleibt bei Fehlern erhalten.',
    'parameter_define': 'Definiert einen Parameter mit Beschreibung, endlichem Standardwert und inklusiven Grenzen. Liefert den Override oder Standardwert über value; gültig beim Erzeugen des Experiments.',
    'parameter_finalize': 'Prüft nach create, ob jeder vorgegebene Override durch das Experiment definiert wurde.',
    'channel_status_index': 'Findet den zugehörigen .status-Kanal; index=-1 bedeutet keine Statuszuordnung.',
    'crc32': 'Berechnet die CRC-32-Prüfsumme über size Bytes ohne Allokation.',
    'put_u32': 'Schreibt einen 32-Bit-Wert little-endian in mindestens vier beschreibbare Bytes.',
    'get_u32': 'Liest einen little-endian 32-Bit-Wert aus mindestens vier lesbaren Bytes.',
    'put_f64': 'Schreibt einen Double-Wert little-endian in mindestens acht beschreibbare Bytes.',
    'get_f64': 'Liest einen little-endian Double-Wert aus mindestens acht lesbaren Bytes.',
})
for prefix, dimension in [('ps_v', 3), ('ps_v2', 2), ('ps_v4', 4)]:
    add(prefix, {
        'add': 'Komponentenweise Summe zweier Vektoren.',
        'sub': 'Komponentenweise Differenz a-b.',
        'scale': 'Multipliziert jede Vektorkomponente mit dem Skalar s.',
        'dot': 'Skalarprodukt; beispielsweise für Projektion oder Energie.',
        'length': 'Euklidische Vektorlänge mit skalierter Berechnung gegen unnötigen Überlauf.',
        'normalize': 'Normiert auf Länge eins; null bleibt null, nichtendliche Eingaben ergeben NaN-Komponenten.',
    })
    DESCRIPTIONS[f'ps_v{dimension}'] = f'Erzeugt einen {dimension}D-Vektor aus den angegebenen Komponenten.'
DESCRIPTIONS['ps_vcross'] = 'Rechtshändiges 3D-Kreuzprodukt a×b, etwa für Drehmoment.'
DESCRIPTIONS['ps_v2cross'] = 'Vorzeichenbehaftete Fläche a.x*b.y-a.y*b.x, die Z-Komponente des 2D-Kreuzprodukts.'
for dimension in (3, 4):
    add(f'ps_mat{dimension}_', {
        'identity': 'Liefert die Einheitsmatrix.',
        'multiply': 'Matrixprodukt a*b; bei Anwendung auf Vektoren wirkt b zuerst.',
        'transpose': 'Vertauscht Zeilen und Spalten.',
        'apply': 'Wendet die Matrix auf einen Spaltenvektor an.',
        'inverse': 'Invertiert mit skalierter Pivotisierung; tolerance=0 wählt die Standardtoleranz.',
    })
add('ps_mat4_', {
    'translation': 'Erzeugt eine homogene Verschiebungsmatrix.',
    'scale': 'Erzeugt eine homogene Skalierungsmatrix.',
    'rotation': 'Erzeugt die homogene Rotationsmatrix einer Quaternion.',
    'trs': 'Kombiniert Translation, Rotation und Skalierung; die Skalierung wirkt zuerst.',
})
add('ps_quat_', {
    'identity': 'Liefert die Identitätsrotation (0,0,0,1).',
    'axis_angle': 'Erzeugt eine rechtshändige Rotation aus Achse und Winkel in Radiant.',
    'rotate': 'Rotiert einen Vektor mit einer Einheitsquaternion.',
    'conjugate': 'Negiert den Vektoranteil; bei Einheitsquaternion die inverse Rotation.',
    'multiply': 'Hamilton-Produkt a*b zur Komposition von Rotationen.',
    'normalize': 'Normiert eine gültige, von null verschiedene Quaternion geprüft.',
    'slerp': 'Interpoliert Rotationen sphärisch auf dem kürzeren Weg, mit t in [0,1].',
})
add('ps_unit_', {
    'valid': 'Prüft, ob die Einheit den Vertrag für Dimensionen, Skala und Symbol erfüllt.',
    'compatible': 'Prüft gleiche SI-Dimensionen; unterschiedliche Skalen können kompatibel sein.',
    'multiply': 'Addiert Dimensionsexponenten und multipliziert Skalen; symbol wird geliehen.',
    'divide': 'Subtrahiert Dimensionsexponenten und dividiert Skalen; symbol wird geliehen.',
    'power': 'Potenz einer Einheit mit ganzzahligem Exponenten und geprüftem Überlauf.',
    'format_dimension': 'Schreibt die kanonische SI-Dimensionsdarstellung in einen begrenzten Textpuffer.',
})
add('ps_quantity_', {
    'convert': 'Konvertiert Zahlenwert und Einheit in eine dimensionskompatible Zieleinheit.',
    'add': 'Addiert dimensionskompatible Größen in der Einheit von a mit normierter, kompensierter Umrechnung und abschließender Subnormalrundung; Fehler erhalten die Ausgabe.',
    'subtract': 'Subtrahiert dimensionskompatible Größen in der Einheit von a mit normierter, kompensierter Umrechnung; Rückskalierung erfolgt erst nach der Summe.',
    'multiply': 'Multipliziert Werte und Einheiten; symbol benennt die Produkteinheit.',
    'divide': 'Dividiert Werte und Einheiten mit Prüfung auf ungültigen Divisor.',
})
add('ps_snapshot_', {
    'decode_version': 'Dekodiert die ausdrücklich genannte Szenenversion; Version 1 erhält Eltern-ID 0, Version 2 prüft Hierarchien und Version 3 ergänzt lokale Koordinatenrahmen. Fehler erhalten alle Ausgaben.',
    'encode': 'Kodiert Zeit, Kanalwerte, Pausestatus und validierte Geometrie explizit little-endian; benötigt PS_SNAPSHOT_MAX Bytes und liefert bei ungültigen Eingaben 0.',
    'decode': 'Dekodiert einen vollständigen Zustand mit Größen-, Zahlen-, Text- und Geometrieprüfung; Fehler lassen sämtliche Ausgaben unverändert.',
})

add('ps_run_', {
    'append_snapshot': 'Schreibt einen optionalen versionierten Szenenblock mit zugehörigen Werten, ohne den Messpunktzähler zu verändern. Kanalzahl und Szene müssen zum Lauf passen.',
    'snapshot_next': 'Liest den nächsten validierten Szenenblock mit einem eigenen Reader, zählt übersprungene Messpunkte und prüft den Footer. Fehler lassen den ausgegebenen Snapshot unverändert.',
    'create': 'Erzeugt eine neue Laufdatei mit Kanaldefinitionen und Metadaten; vorhandene Pfade werden geschützt.',
    'append': 'Schreibt einen Messpunkt mit Zeit und einem Wert je registriertem Kanal.',
    'close': 'Finalisiert die Laufdatei mit Abschlussmarker und schließt den Writer.',
    'open': 'Öffnet einen Lauf zum blockweisen Lesen und lädt sein Schema.',
    'next': 'Liest den nächsten vollständigen Messpunkt; nur PS_OK liefert neue Werte.',
    'reader_close': 'Schließt einen geöffneten Reader.',
    'export_csv': 'Exportiert Rohzeit und sämtliche Kanäle als CSV, einschließlich Status und ungültiger Sensorzeilen.',
})
add('ps_scene_', {
    'frame': 'Erzeugt einen expliziten TRS-Koordinatenrahmen mit eindeutiger ID und geprüftem Elternknoten. Skalierungen müssen endlich und ungleich null sein; Fehler erhalten die Szene.',
    'transforms': 'Berechnet für jeden Szenenslot die zusammengesetzte Local-to-world-Matrix. Keine Allokation, kein gemeinsamer Cache; Fehler erhalten das Ausgabearray.',
    'world_point': 'Konvertiert einen Punkt in der Koordinatenbasis des Eintrags in Weltkoordinaten; fehlerhafte Transformationen erhalten die Ausgabe.',
    'group': 'Erzeugt eine benannte Gruppe mit eindeutiger ID und optionalem Elternknoten; fehlerhafte Beziehungen verändern die Szene nicht.',
    'set_parent': 'Ändert die Eltern-ID eines benannten Szeneneintrags; fehlende IDs, Selbstbeziehungen und Zyklen werden transaktional abgewiesen.',
    'parent_index': 'Liefert den Slot des Elternknotens oder -1 für Wurzeln, ungültige Slots und fehlende Eltern.',
    'add': 'Fügt ein anonymes einfaches Objekt hinzu; Fehler sind hier nicht als Rückgabewert verfügbar.',
    'push': 'Prüft und kopiert ein vollständig beschriebenes Szenenobjekt.',
    'polyline': 'Kopiert mindestens zwei lokale Punkte (ohne Rahmen Weltpunkte) in den Szenenpunktpuffer und fügt einen Linienzug hinzu.',
    'label': 'Kopiert eine UTF-8-Beschriftung mit lokalem Anker (ohne Rahmen Weltanker) in die Szene.',
    'valid': 'Prüft den vollständigen Snapshot auf Form-, Zahlen-, Text-, ID- und Punktbereichsregeln.',
    'add_id': 'Fügt ein einfaches Objekt mit optionaler stabiler ID hinzu und meldet Fehler.',
    'polyline_id': 'Fügt einen Linienzug mit stabiler ID hinzu; Fehler verändern weder Objekte noch Punktpuffer.',
    'label_id': 'Fügt eine Beschriftung mit stabiler ID hinzu; doppelte nichtnull IDs werden abgewiesen.',
})
add('ps_analysis_', {
    'create': 'Erzeugt einen Analysecontext mit Ausgabepräfix und temporärer Speicherquote.',
    'create_with_allocator': 'Erzeugt einen Analysecontext in einer eigenen Allokationsdomäne.',
    'destroy': 'Gibt Context und Arbeitsdateien frei; alle darin vergebenen Handles werden ungültig.',
    'scratch_bytes': 'Meldet die aktuell belegten Nutzdatenbytes der temporären Reihenablage.',
    'open_run': 'Validiert einen Lauf und öffnet einen unveränderlichen Datensnapshot; PS_RECOVERED ist lesbarer Teilerfolg.',
})
add('ps_dataset_', {
    'describe': 'Kopiert Samplezahl, Kanalschema, Metadaten und Wiederherstellungsstatus.',
    'close': 'Schließt den Datensatz und invalidiert alle ihm zugehörigen Reihen.',
    'series': 'Holt die Reihe eines benannten Kanals oder die Zeitachse time.',
})
add('ps_series_', {
    'from_values': 'Kopiert endliche Werte in eine eigenständige Datenreihe ohne Eingabedatensatz; Einheit und Name werden geprüft.',
    'aligned_values': 'Kopiert endliche Werte in eine neue Reihe mit der Samplezuordnung einer vorhandenen Ankerreihe.',
    'describe': 'Kopiert Reihenname, Anzahl und Einheit in den Ausgabedeskriptor.',
    'release': 'Gibt dieses Reihenhandle frei; bereits abgeleitete Reihen bleiben erhalten.',
    'aligned': 'Prüft, ob zwei Reihen dieselbe Samplezuordnung besitzen.',
    'read': 'Kopiert einen Ausschnitt ab einem Sampleindex in den Puffer des Aufrufers.',
    'slice': 'Erzeugt eine neue Reihe für einen zusammenhängenden Samplebereich.',
    'select': 'Filtert mehrere Reihen gemeinsam nach einem exakten Selektorwert, etwa gültigem Sensorstatus 1.',
    'affine': 'Skaliert eine Reihe mit dimensionslosem Faktor und addiert einen dimensionsgeprüften Offset.',
    'combine': 'Verknüpft gepaarte Reihen per Summe, Differenz, Produkt oder Quotient und prüft ihre Einheiten.',
    'derivative': 'Erzeugt dy/dx mit zentralen Sekanten und einseitigen Rändern.',
    'integral': 'Erzeugt ein kumulatives Trapezintegral mit explizitem Anfangswert und Anfangseinheit.',
    'moving_average': 'Erzeugt einen kausalen Mittelwert über bis zu window Samples.',
    'resample_linear': 'Interpoliert y(x) linear auf ein neues Zeit-/X-Raster ohne Extrapolation.',
    'resample': 'Resampling mit expliziter Methode: linear, nächster oder vorheriger Stützpunkt.',
    'statistics': 'Berechnet Statistik über die vollständige Reihe, unabhängig von Plotvorschauen.',
    'quantile': 'Berechnet das Typ-7-Quantil einer nichtleeren Reihe für eine endliche Wahrscheinlichkeit in [0, 1]. Verwendet temporären Speicher des Analyseallocators.',
    'quantile_with_allocator': 'Berechnet dasselbe Quantil mit einem separat angegebenen Allocator für den temporären Sortierpuffer.',
    'export_csv': 'Exportiert alle Werte gemeinsam ausgerichteter Spalten als neue CSV-Datei.',
})
add('ps_report_', {
    'create': 'Erzeugt einen leeren Bericht mit Titel und Herkunftstext.',
    'create_with_allocator': 'Erzeugt einen Bericht mit eigener Allokationsdomäne.',
    'destroy': 'Gibt Bericht und alle Kurven/Tabellen frei; Handles und geliehene Ansichten verlieren ihre Gültigkeit.',
    'describe': 'Kopiert Titel, Herkunft und Anzahl der Diagramme und Tabellen.',
    'unit_from': 'Kopiert Dimension, Skala und Symbol einer Einheit in einen Berichtseintrag.',
    'add_plot': 'Legt ein Diagramm mit Achsenbeschriftungen und Einheiten an und liefert sein Handle.',
    'add_curve': 'Kopiert selbst berechnete Plotpunkte; sie müssen bereits die Achseneinheiten verwenden.',
    'add_series': 'Übernimmt ausgerichtete Reihen als Linien- oder Punktkurve, mit geprüfter Einheitenkonvertierung und Vorschaugrenze.',
    'add_histogram': 'Zählt sämtliche Reihenwerte in gleich breiten Klassen und legt ein Histogrammdiagramm an.',
    'add_table': 'Legt eine Tabelle mit benannten Spalten und deren Einheiten an.',
    'add_row': 'Fügt eine benannte Zeile mit endlichen Zahlenwerten zur Tabelle hinzu.',
    'plot_read': 'Kopiert die Diagrammbeschreibung am nullbasierten Index.',
    'curve_read': 'Kopiert die vollständigen gespeicherten Daten einer Kurve in eigenen Speicher.',
    'curve_view': 'Leiht eine unveränderliche Kurvenansicht ohne Kopie; nur bis zur Berichtszerstörung gültig.',
    'table_read': 'Kopiert die Tabellenbeschreibung am nullbasierten Index.',
    'row_read': 'Kopiert eine Tabellenzeile in den Speicher des Aufrufers.',
    'plot_bounds': 'Berechnet die Ausdehnung aller Kurven einschließlich Histogrammkanten und Nullbasis.',
    'axis_fraction': 'Bildet einen Wert relativ zum Achsenbereich auf einen dimensionslosen Anteil ab.',
    'save': 'Schreibt den Bericht als versionierte, CRC-geprüfte neue .psreport-Datei.',
    'load': 'Lädt und validiert eine .psreport-Datei vollständig vor Veröffentlichung des Ergebniszeigers.',
    'load_with_allocator': 'Lädt einen Bericht mit eigener Allokationsdomäne.',
    'export_svg': 'Exportiert ein gesamtes Diagramm mit Achsen und Legende als SVG.',
    'export_svg_region': 'Exportiert einen begrenzten Diagrammausschnitt; NULL als Bereich wählt den gesamten Plot.',
    'export_table_csv': 'Exportiert sämtliche Zeilen einer ausgewählten Tabelle als CSV.',
    'export_plot_csv': 'Exportiert gespeicherte Plotpunkte beziehungsweise Histogrammklassen, nicht die ursprüngliche vollständige Reihe.',
})
add('ps_', {
    'distribution_validate': 'Prüft konstante, uniforme oder normale Verteilungsparameter.',
    'distribution_sample': 'Zieht einen Wert aus der Verteilung mit dem expliziten Zufallsstrom.',
    'distribution_moments': 'Berechnet Erwartungswert und Standardabweichung einer Verteilung ohne Zufallsziehung.',
    'sensor_config_validate': 'Prüft Abtastrate, Einheit, Rauschen, Ausfälle und Unsicherheitsparameter.',
    'sensor_init': 'Initialisiert einen Sensor mit Konfiguration und eigenem Seed.',
    'sensor_reset': 'Setzt Abtastzustand und Zufallsstrom des Sensors zurück.',
    'sensor_next_time': 'Liefert den nächsten planmäßigen Messzeitpunkt in Sekunden.',
    'sensor_read': 'Fragt den Sensor zum Modellzeitpunkt ab; liefert Messwert, Unsicherheit und Status und aktualisiert das Raster.',
    'body_sphere': 'Erzeugt Masse und Hauptträgheit einer homogenen Kugel.',
    'body_box': 'Erzeugt Masse und Hauptträgheit einer homogenen Box mit vollen Seitenlängen.',
    'body_validate': 'Prüft Masse, Hauptträgheit, Pose und Geschwindigkeiten eines Körpers.',
    'body_kinetic_energy': 'Berechnet translatorische plus rotatorische kinetische Energie.',
    'body_point_velocity': 'Berechnet die Weltgeschwindigkeit eines Punkts einschließlich Rotation.',
    'body_force_torque': 'Berechnet das Drehmoment einer an einem Weltpunkt angreifenden Kraft.',
    'body_apply_impulse': 'Ändert lineare und rotatorische Geschwindigkeit durch einen Impuls am Weltpunkt.',
    'body_step': 'Integriert Körperbewegung unter der vorgegebenen Kraft und dem Drehmoment.',
    'distance_joint_resolve': 'Löst ein Distanzgelenk über Geschwindigkeitsimpulse und ein Stabilisierungsziel; Positionen bleiben unverändert.',
    'distance_joint_validate': 'Prüft endliche lokale Anker, eine positive endliche Soll-Länge und eine Stabilisierung in 0..1. Liefert PS_OK oder PS_INVALID; verändert das Gelenk nicht.',
    'contact_spheres': 'Ermittelt Kontaktgeometrie zwischen zwei Kugeln.',
    'contact_sphere_plane': 'Ermittelt den Kontakt einer Kugel mit einer Ebene.',
    'contact_sphere_box': 'Ermittelt den Kontakt einer Kugel mit einer orientierten Box.',
    'contacts_box_plane': 'Erzeugt mehrere Kontaktpunkte zwischen orientierter Box und Ebene.',
    'contacts_boxes': 'Erzeugt ein Kontaktmanifold für zwei orientierte Boxen.',
    'contacts_resolve': 'Löst mehrere Kontakte eines Körperpaars iterativ mit akkumulierten Normal-/Reibungsimpulsen.',
    'contacts_resolve_graph': 'Löst zusammenhängende Kontakte mehrerer Körper gemeinsam, etwa Kontaktketten und Stapel.',
    'constraints_resolve_graph': 'Löst gemischte Kontakte und Distanzgelenke mehrerer Körper gemeinsam.',
    'contact_resolve': 'Wendet die Impulsantwort für einen Einzelkontakt an.',
    'buoyancy_force': 'Berechnet Auftrieb aus Fluiddichte, verdrängtem Volumen und Gravitation.',
    'sphere_submersion': 'Berechnet eingetauchtes Kugelvolumen und dessen Schwerpunkt relativ zu einer ebenen Wasseroberfläche.',
    'sphere_drag': 'Berechnet Stokes- oder quadratischen Widerstand einer Kugel relativ zum Medium.',
    'spring_force': 'Berechnet axiale Feder-/Dämpferkraft zwischen bewegten Endpunkten.',
    'aabb_sphere': 'Berechnet eine konservative achsenparallele Hüllbox der aktuellen Kugelpose.',
    'aabb_box': 'Berechnet eine konservative achsenparallele Hüllbox einer orientierten Box.',
    'aabb_swept_sphere': 'Berechnet die Hüllbox über die ganze geradlinige Verschiebung einer Kugel.',
    'sweep_spheres': 'Ermittelt den ersten Kontakt zweier linear verschobener Kugeln als Anteil der Bewegung in [0,1].',
    'sweep_sphere_plane': 'Ermittelt den ersten Kontakt einer linear verschobenen Kugel mit einer Ebene.',
    'broad_phase': 'Erzeugt deterministisch sortierte Kandidatenpaare überlappender Hüllboxen; noch keine genaue Kontaktprüfung.',
})
add('ps_', {
    'allocator_default': 'Liefert den Standardallocator der Bibliothek.',
    'allocator_valid': 'Prüft, ob die erforderlichen Allokationscallbacks vorhanden sind.',
    'memory_allocate': 'Reserviert bytes mit geprüfter Allokationsdomäne; null Bytes ergeben NULL.',
    'memory_zero': 'Reserviert count Elemente der angegebenen Größe, prüft Multiplikationsüberlauf und nullt die Bytes.',
    'memory_free': 'Gibt einen Bereich mit zugehörigem Allocator und ursprünglicher Größe frei.',
    'memory_resize': 'Ändert die Speichergröße unter Erhaltung des gemeinsamen Präfixes; Fehler lassen den ursprünglichen Speicher bestehen.',
    'arena_init': 'Initialisiert eine feste Arena auf einem Puffer des Aufrufers; die vergebenen Bereiche werden intern ausgerichtet.',
    'arena_allocator': 'Liefert einen Allocator, der Speicher fortlaufend aus der Arena vergibt.',
    'arena_reset': 'Setzt den Arena-Verbrauch zurück; bisher vergebene Bereiche dürfen nicht weiter benutzt werden.',
})
add('ps_array_', {
    'init': 'Initialisiert ein leeres Array mit Elementgröße, Allocator und optionaler Maximalanzahl.',
    'reserve': 'Stellt mindestens die gewünschte Kapazität bereit, ohne die Elementanzahl zu ändern.',
    'resize': 'Ändert die Elementanzahl; neu sichtbare Bytes werden genullt.',
    'insert': 'Fügt Elemente vor index ein und verschiebt den Rest; Selbsteinfügung wird unterstützt.',
    'append': 'Hängt die angegebenen Elemente an das Ende an.',
    'erase': 'Entfernt einen zusammenhängenden Bereich und verschiebt nachfolgende Elemente.',
    'clear': 'Setzt die Anzahl auf null und behält den Speicher.',
    'shrink': 'Reduziert die Kapazität auf die aktuelle Anzahl; kann bei nötiger Allokation fehlschlagen.',
    'destroy': 'Gibt den Speicher frei und nullt den Arraydeskriptor.',
})
add('ps_string_view_', {
    'valid': 'Prüft die Zeiger-/Längenkombination; prüft weder UTF-8 noch tatsächliche Pufferlebensdauer.',
    'make': 'Leiht einen explizit begrenzten Bytebereich.',
    'cstr': 'Leiht eine gültige nullterminierte Zeichenkette ohne ihr abschließendes Nullbyte.',
    'compare': 'Vergleicht lexikografisch als vorzeichenlose Bytes; order wird -1, 0 oder 1.',
    'equal': 'Prüft gleiche Bytefolgen; ungültige Views sind niemals gleich.',
    'slice': 'Leiht einen Teilbereich anhand von Byteindex und Byteanzahl.',
    'find': 'Sucht die erste Teilfolge ab start; PS_EOF bedeutet keinen Treffer.',
    'split': 'Teilt am ersten nichtleeren Trennzeichen in zwei geliehene Views ohne das Trennzeichen.',
    'trim_ascii': 'Entfernt ASCII-Leerraum an beiden Enden ohne Allokation.',
    'copy': 'Kopiert Bytes und ein zusätzliches Nullbyte in einen ausreichend großen Zielpuffer.',
})
add('ps_hashmap_', {
    'create': 'Erzeugt eine Map mit kopierten Byteschlüsseln, fester Wertgröße und expliziten Kapazitätsgrenzen.',
    'set': 'Kopiert einen neuen Schlüssel/Wert oder ersetzt einen bestehenden Wert.',
    'get': 'Kopiert den gefundenen Wert in out; PS_EOF bedeutet fehlenden Schlüssel.',
    'erase': 'Entfernt den Eintrag zum Schlüssel und gibt seinen Speicher frei.',
    'count': 'Liefert die aktuelle Eintragszahl; NULL ergibt null.',
    'reserve': 'Reserviert Buckets für die gewünschte Eintragszahl; einzelne Knoten werden weiterhin separat angelegt.',
    'clear': 'Entfernt alle Einträge und behält die Bucketkapazität.',
    'visit': 'Besucht jeden Eintrag mit geliehenen schreibgeschützten Schlüssel-/Wertzeigern; die Reihenfolge ist nicht zugesichert.',
    'destroy': 'Gibt Map, Schlüssel und Werte frei; während einer aktiven Traversierung wirkungslos.',
})

DESCRIPTIONS.update({
    'ps_series_mask': 'Erhält alle Zeilen und das Alignment; kombiniert die Eingabemasken mit einem expliziten dimensionlosen Selektorwert.',
    'ps_series_read_masked': 'Liest numerische Werte samt Gültigkeit pro Zeile blockweise. Fehlende Werte sind nur mit ihrem Flag zu interpretieren.',
    'ps_series_is_masked': 'Prüft, ob die Reihe eine ausdrücklich gespeicherte Gültigkeitsmaske besitzt, auch wenn alle Zeilen gültig sind.',
    'ps_series_validity': 'Erzeugt eine ausgerichtete, unmaskierte Reihe exakter 0/1-Gültigkeitswerte.',
    'ps_report_add_curve_masked': 'Kopiert eine Kurve samt Gültigkeit und Segmentanfängen. Fehlende Punkte werden beim Zeichnen und bei Grenzen übergangen.',
    'ps_report_curve_mask': 'Leiht unveränderliche Kurvenflags; NULL bezeichnet eine vollständig gültige, zusammenhängende Kurve.',
})

add('ps_diagnostic_', {
    'clear': 'Setzt einen vorhandenen Wert auf eine gültige leere Diagnose mit Code PS_OK.',
    'valid': 'Prüft Größe, Version, Fehlercode, UTF-8-Felder und konsistente Quellkoordinaten.',
    'set': 'Kopiert einen vollständigen Fehler in den Ausgabe-Wert; ungültige Eingaben erhalten ihn unverändert.',
    'format': 'Formatiert Fehlercode, Operation, Argument und Quellstelle als begrenztes UTF-8; PS_LIMIT meldet Kürzung.',
    'encode': 'Kodiert einen Fehler mit exakten Längen, little-endian Feldern und CRC; liefert die Bytezahl oder 0.',
    'decode': 'Prüft den gesamten versionierten Fehlerpayload; Änderungen erfolgen erst nach vollständiger Validierung.',
    'save': 'Schreibt einen gültigen Fehler exklusiv in eine neue Binärdatei; vorhandene Dateien bleiben erhalten.',
    'load': 'Lädt einen vollständigen Fehler und prüft Version, Grenzen, CRC und Text; Fehler erhalten die Ausgabe.',
})
add('ps_experiment_', {
    'fail': 'Speichert eine strukturierte Diagnose und einen kompatiblen Text im Hostcontext; liefert den gespeicherten Fehlercode.',
    'diagnostic': 'Liefert eine unabhängige Kopie der Experimentdiagnose; ein älterer Context ohne optionalen Tail meldet PS_VERSION.',
})
add('ps_run_index_', {
    'open': 'Öffnet und validiert einen Lauf mit begrenzten Allocator-Checkpoints; liefert auch bei PS_RECOVERED einen besitzenden Handle.',
    'destroy': 'Schließt die unverändert geöffnete Datei und gibt alle Checkpoints an den ursprünglichen Allocator zurück.',
    'get_info': 'Kopiert Schema, Metadaten, Präfixzählungen und Status nach Prüfung der Ausgabegröße und Version.',
    'read': 'Liest bis zu 256 Messzeilen ab einer nullbasierten Zeilennummer; alle Ausgaben bleiben bei Fehlern unverändert.',
    'snapshot': 'Liest eine aufgezeichnete Szene anhand ihrer nullbasierten Nummer mit erneuter CRC- und Snapshotprüfung.',
})
add('ps_contact_world_', {
    'init': 'Initialisiert den begrenzten Kontaktzustand mit Größenprüfung und validierten Match-/Warmstart-Einstellungen; Fehler erhalten den bisherigen Wert.',
    'reset': 'Entfernt die gesamte Kontakthistorie und behält gültige Einstellungen; für Modellreset, Teleports oder ersetzte Objekte verwenden.',
    'solve': 'Erzeugt und löst aktuelle diskrete Kontakte, ordnet alte lokale Anker zu und aktualisiert Körper, Cache und Ergebnis atomar.',
})
DESCRIPTIONS['ps_contacts_resolve_graph_warm']='Wie der Kontaktsolver mit expliziten Startimpulsen auf A. Restitution wird vor sämtlichen Warmimpulsen bestimmt; Startwerte werden auf die aktuelle Normale und den Coulomb-Kegel projiziert. NULL wählt den kalten Pfad.'

add('ps_', {
    'ideal_gas_pressure': 'Berechnet p=nRT/V in Pa für positive SI-Zustandsgrößen eines idealen Gases.',
    'ideal_gas_volume': 'Berechnet V=nRT/p in m³ für ein ideales Gas.',
    'ideal_gas_temperature': 'Berechnet T=pV/(nR) in Kelvin für ein ideales Gas.',
    'ideal_gas_energy': 'Berechnet U=n cv T in J bei konstantem molarem cv und Referenz U=0 bei T=0.',
    'ideal_gas_entropy_change': 'Berechnet die reversible Entropiedifferenz zwischen zwei Gleichgewichtszuständen desselben idealen Gases bei konstantem cv.',
    'heat_capacity': 'Berechnet C=m c in J/K bei konstanter spezifischer Wärmekapazität.',
    'sensible_heat': 'Berechnet Q=C(T1-T0) in J, positiv bei Erwärmung.',
    'heat_flow': 'Berechnet P=G(Ta-Tb) in W, positiv von A nach B.',
    'thermal_reservoir_step': 'Berechnet die exakte Relaxation an ein konstantes Reservoir ohne Zeitschritt-Stabilitätsgrenze.',
    'thermal_pair_step': 'Berechnet die exakte isolierte Zweikörperrelaxation mit konstanter Kapazität und Leitwert; beide Temperaturen stehen in Vec2.',
})

add('ps_', {'point_charge_field': 'Berechnet das homogene Coulombfeld in V/m; der Quellpunkt ist singulär.', 'point_charge_potential': 'Berechnet das Punktladungspotential in V mit Nullpunkt im Unendlichen.', 'lorentz_force': 'Berechnet q(E+v×B) in N für ausdrücklich übergebene SI-Felder.', 'resistor_current': 'Berechnet I=V/R in A.', 'resistor_voltage': 'Berechnet V=IR in V.', 'resistor_power': 'Berechnet die nichtnegative Verlustleistung V²/R in W.', 'resistance_series': 'Addiert zwei positive Widerstände.', 'resistance_parallel': 'Berechnet den Gesamtwiderstand zweier positiver Parallelwiderstände.', 'capacitor_energy': 'Berechnet die ideale Kondensatorenergie 0,5 C V² in J.', 'rc_voltage_step': 'Berechnet einen exakten konstanten RC-Spannungsschritt ohne Zeitschritt-Stabilitätsgrenze.'})

add('ps_', {'harmonic_step': 'Berechnet einen exakten undämpften Oszillatorschritt für Position und Geschwindigkeit.', 'string_wave_speed': 'Berechnet die ideale Saitengeschwindigkeit aus Spannungskraft und linearer Dichte.', 'traveling_wave': 'Wertet Verschiebung, Geschwindigkeit und Steigung einer harmonischen Laufwelle aus.', 'string_wave_step': 'Berechnet einen atomaren zentrierten 1D-Wellenschritt mit Nullrändern und geprüfter CFL-Grenze.', 'ray_reflect': 'Reflektiert eine validierte Unit-Richtung an einer orientierten Unit-Normale.', 'ray_refract': 'Berechnet Snell-Brechung oder meldet ausdrücklich Totalreflexion ohne Ausgabeänderung.', 'thin_lens_image': 'Berechnet signierte Bildweite und Vergrößerung einer paraxialen dünnen Linse.'})

add('ps_', {'pipe_conductance': 'Berechnet den Hagen-Poiseuille-Leitwert eines idealen laminaren Rundrohrs.', 'pipe_flow': 'Berechnet den signierten Volumenstrom G(pa-pb).', 'pipe_power': 'Berechnet die nichtnegative hydraulische Verlustleistung.', 'reynolds_number': 'Berechnet die dimensionslose Reynolds-Zahl aus expliziten SI-Stoffdaten.', 'hydrostatic_pressure': 'Berechnet den hydrostatischen Relativdruck mit signierter Tiefe.', 'pipe_network_solve': 'Löst ein verankertes passives lineares Drucknetz atomar und liefert Kantenflüsse.', 'transport_periodic_step': 'Berechnet einen atomaren konservativen Upwind-/Diffusionsschritt für einen periodischen nichtnegativen Tracer.'})

add('ps_', {
    'vdw_gas_pressure': 'Wertet p=nRT/(V-nb)-an²/V² in Pa für konstante molare SI-Koeffizienten aus; V>nb. Keine Phasenauswahl.',
    'vdw_gas_pressure_derivative': 'Wertet die Druckableitung bei festem n,T in Pa/m³ aus; positive Werte sind mechanisch instabile homogene Zustände.',
    'vdw_gas_energy': 'Wertet U=n cv T-an²/V in J bei konstantem a,cv aus; Referenz T→0,V→∞ und signierte Ergebnisse.',
    'vdw_gas_entropy_change': 'Berechnet die Entropiedifferenz mit freiem Volumen V-nb bei konstantem cv,b; keine Entropieproduktion oder Phasenkoexistenz.',
})
