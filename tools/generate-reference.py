"""Build offline API references; --check detects stale signatures/contracts.

Only the public C17 headers are inputs, not implementation helpers. Language
signatures come from the compiler registry, including receiver/factory spelling.
Handwritten guides remain the teaching layer. No third-party packages required.
"""
import argparse
import posixpath
import re
from pathlib import Path
from reference_descriptions import DESCRIPTIONS

ROOT = Path(__file__).resolve().parents[1]
MODULES = {
    'properties': ('Material- und Medieneigenschaften', 'properties.md', 'Eigenschaften sind explizite SI-Daten mit Quellenangabe, Gültigkeitsbereich und konstantem oder tabellarischem Modell. Tabellen werden begrenzt linear/bilinear interpoliert; keine Extrapolation oder automatische Materialauswahl. Das konsumierende Modell prüft die benötigten Eigenschaften.'),
    'fluid': ('Laminare Rohre, Netze und Tracertransport', 'fluid.md', 'Lehrmodelle für inkompressible Newtonsche Rohre, passive stationäre Drucknetze und konservativen periodischen 1D-Tracer. Explizite Stoffdaten, Größen- und Stabilitätsgrenzen; keine turbulente oder mehrdimensionale CFD.'),
    'waves': ('Oszillatoren und eindimensionale Wellen', 'waves-optics.md', 'Exakter undämpfter Oszillator, ideale Saitengeschwindigkeit, harmonische Laufwelle und diskrete 1D-Wellengleichung mit festen Nullrändern. Der allokationsfreie Saitenschritt prüft CFL und übernimmt Ausgaben erst nach vollständigem Erfolg.'),
    'optics': ('Geometrische Optik', 'waves-optics.md', 'Reflexion, Snell-Brechung mit ausdrücklicher Totalreflexion und paraxiale dünne Linsen. Unit-Richtungen und Normalen, explizite Brechungsindizes und signierte Bildweiten. Keine automatische Strahlverfolgung, Fresnelamplituden oder Beugung.'),
    'electromagnetism': ('Elektromagnetismus und RC-Schaltungen', 'electromagnetism.md', 'Reine SI-Funktionen für homogene Punktladungsfelder, Potential, Lorentzkraft, Widerstände, Kondensatorenergie und exakte RC-Schritte. Modelle liefern Permittivität und Feldwerte ausdrücklich; Singularität und Fehler bewahren Ausgaben. Kein Maxwell- oder beliebiger Netzwerk-Solver.'),
    'thermodynamics': ('Thermodynamik: Gasmodelle und Wärmefluss', 'thermodynamics.md', 'SI-Werte, Kelvin, konstante Wärmekapazitäten und explizite lineare Leitwerte. Ideale und homogene Van-der-Waals-Zustandsgrößen ohne Phasenauswahl, Energie und Entropiedifferenzen sowie exakte Reservoir-/Zweikörperrelaxation. Keine Allokation, kein impliziter Integrator und keine Stofftabellen; Fehler erhalten Ausgaben.'),
    'batch': ('Archivierte Serien und Analyse-Hostdienste', 'batch-language.md', 'Ein expliziter Analysehost stellt versionierte, synchrone Batch-Dienste bereit. Requests beschreiben getrennte Runner mit SI-Parametern, Seeds, Zeit- und Speichergrenzen. Ergebnisse enthalten validierte Teilfortschritte und Messstatus. Der Core startet keine Prozesse; der Analyse-Runner liefert die Dienste über den optionalen run_host-Tail. Geliehene Dienstzeiger leben nur während dieses Aufrufs.'),
    'contact_world': ('Persistente Kontakte und Warmstart', 'contact-world.md', 'Ein expliziter, caller-eigener Kontaktzustand erzeugt diskrete Kugel-/Box-/Ebenenkontakte. Stabile Collider-IDs und lokale Anker ordnen Kontakte zwischen erfolgreichen Schritten zu; alte Impulse werden zeitabhängig skaliert und im aktuellen Coulomb-Kegel gelöst. Keine Heapallokation, keine automatische Integration oder CCD. Körper, Cache und Ergebnisse bleiben bei Fehlern unverändert.'),
    'run_stream': ('Versionierte Run-Streams', 'run-streams.md', 'Ein opaker Store besitzt begrenzte Reader-/Writer-Slots. Handles prüfen Besitzer und Generation; Schließen invalidiert alle Kopien. Reader geben kopierte Metadaten und atomar validierte Zeilen/Snapshots zurück. Writer finalisieren ausdrücklich oder bewahren durch Abort einen unvollständigen Präfix. Der Store und sein Allocator müssen alle Handle-Nutzungen überleben.'),
    'run_index': ('Indizierte Laufdateien', 'run-index.md', 'Ein Index besitzt eine unverändert geöffnete Messdatei und Checkpoints aus einem expliziten Allocator. Das Öffnen prüft den gesamten lesbaren Präfix einmal und rekonstruiert auch alte oder unvollständige Dateien. Gezielte Messblöcke und Szenen verwenden diese geprüften Positionen. PS_OK und PS_RECOVERED liefern beide einen zu zerstörenden Handle; andere Fehler erhalten die Ausgabe.'),
    'diagnostic': ('Strukturierte Diagnosen', 'diagnostics.md', 'Diagnosen sind eigene begrenzte UTF-8-Werte mit Fehlercode, Operation, Argument und Quellposition. Kein globaler Last-error-Zustand. Konstruktion und Decodierung sind transaktional, Dateien werden exklusiv erstellt. Experiment- und Analyse-Runner transportieren die Daten zusätzlich zur bisherigen Textausgabe.'),
    'log': ('Logging mit explizitem Sink', 'logging.md', 'Ein Logger verbindet einen benannten Schweregrad und eine endliche Modellzeit mit einem synchronen Sink. Kein globaler Logger und keine implizite Ausgabe. Kontextlogger gehören dem Host; ps_experiment_log verwendet die aktuelle Simulationszeit. Nachrichten sind begrenztes UTF-8; Rückgabewerte entscheiden über die Behandlung verworfener Meldungen.'),
    'core': ('Grundlagen und Zufall', 'api.md', 'Vektoren, Rotation, Zufallsströme, Basiseinheiten und einfache Integrations-/Kollisionshelfer. Zufallsströme zuerst mit ps_rng_seed initialisieren; derselbe Seed wiederholt den Strom. ps_rng_uniform liefert Werte in (0,1). Winkel werden im Bogenmaß angegeben.'),
    'experiment': ('Experimente und Szenen', 'experiment-tutorial.md', 'Ein Experiment exportiert ps_get_experiment. Der Host ruft create, reset, step, build_scene und destroy auf. ps_channel_add liefert einen Kanalindex oder -1; Messwerte werden über context->values[index] gesetzt. Kanäle nur einmal registrieren. Szenen werden pro Snapshot neu aufgebaut.'),
    'numerics': ('Numerische Verfahren', 'numerics.md', 'Wähle Integrator, Zustand und Ableitung passend zum Modell. Numerische Callbacks müssen alle Komponenten setzen und frei von sichtbaren Nebenwirkungen sein. Adaptive Zwischenstufen sind keine Messzeitpunkte. Prüfe Rückgabewert und gegebenenfalls Diagnose vor Verwendung des Ergebnisses.'),
    'units': ('Einheiten und Größen', 'numerics.md', 'ps_unit beschreibt Dimension, positive Skala und Symbol. ps_quantity kombiniert Zahlenwert und Einheit. Addition/Subtraktion konvertieren den rechten Operanden in die Einheit des linken. Produkte und Quotienten kombinieren Dimensionen. Symbole müssen solange wie die Einheit gültig bleiben. Temperatur-Offsets werden nicht unterstützt.'),
    'series': ('Datenreihen', 'series.md', 'Erzeuge einen Analysecontext, öffne einen Datensatz und hole time sowie Messkanäle als Reihen. Transformationen erzeugen neue Handles. Wähle gemeinsam ausgerichtete Reihen; Länge allein garantiert keine Zuordnung. Nach ps_analysis_destroy sind alle zugehörigen Handles ungültig.'),
    'data': ('Dateien und Daten', 'data-format.md', 'Reader und Writer werden vom Aufrufer gehalten. Schließe jeden erfolgreich geöffneten Reader/Writer. Wiederhole ps_run_next bis zu einem Ergebnis ungleich PS_OK. PS_EOF bedeutet vollständiger Lauf, PS_RECOVERED einen lesbaren Teil eines beschädigten oder unvollständigen Laufs. ps_put/get_u32 benötigen vier Bytes, ps_put/get_f64 acht Bytes; sie kodieren little-endian.'),
    'snapshot': ('Aufgezeichnete Szenenzustände', 'data-format.md', 'Ein Snapshot verbindet Simulationszeit, Kanalwerte, Pausestatus und die vollständige Szene. Er enthält keine geliehenen Zeiger. Der Encoder benötigt PS_SNAPSHOT_MAX beschreibbare Bytes; der Decoder validiert das gesamte Format und erhält alle Ausgaben bei Fehlern. Die versionierten Datenblöcke teilen diesen Codec mit der Runner-Pipe.'),
    'analysis': ('Statistik und Analysemodule', 'series.md', 'Initialisiere ps_statistics mit null und füge endliche Werte mit ps_statistics_push hinzu. ps_statistics_stddev liefert die Stichprobenstreuung. ps_derivative berechnet Sekanten, ps_trapezoid ein bestimmtes Integral. ps_analyze_run erstellt eine Standardauswertung. Eigene Module exportieren ps_get_analysis; das vollständige Beispiel steht im Experimenttutorial.'),
    'report': ('Diagramme und Tabellen', 'reports.md', 'Erzeuge einen Bericht, füge Diagramme/Tabellen und deren Inhalte hinzu, speichere als .psreport und zerstöre ihn. Plot- und Tabellenhandles gelten nur in ihrem eigenen Bericht. Exporte erzeugen neue Dateien. Achseneinheiten müssen zu den Daten passen; Plotvorschauen können reduziert sein.'),
    'mechanics': ('Mechanik und Medien', 'mechanics.md', 'Erzeuge Körper und Trägheit, summiere Kräfte/Drehmomente, integriere und löse Kontakte beziehungsweise Gelenke. SI-Einheiten und Welt-/Lokalkoordinaten beachten. Geometrische Kontaktprüfung und Impulsantwort sind getrennte Schritte. Der Lerntext enthält vollständige Abläufe für Einzelkontakte, Graphen, Gelenke, Medien und CCD.'),
    'collision': ('Kollisionserkennung', 'mechanics.md', 'Broad Phase liefert mögliche Paare, keine fertigen Kontakte. Prüfe Kandidaten geometrisch und löse anschließend ihre Impulse. Sweep-Funktionen liefern den ersten Kontakt entlang einer vorgegebenen Verschiebung; die verbleibende Bewegung muss der Aufrufer selbst integrieren. Ebenen separat behandeln.'),
    'measurement': ('Messungen und Sensoren', 'measurement.md', 'Definiere Verteilung und Sensorkonfiguration, initialisiere den Sensor mit explizitem Seed und frage ihn zu Modellzeitpunkten ab. Verwende einen Wert nur bei gültigem Status. Messwert, Standardunsicherheit und Status getrennt aufzeichnen. Ein Sensor besitzt seinen eigenen Zufallsstrom und Abtastzustand.'),
    'math': ('Vektoren, Matrizen und Kurven', 'math.md', 'Alle Komponenten sind double. Matrizen sind spaltenweise gespeichert und wirken auf Spaltenvektoren; A*B führt B zuerst aus. Winkel sind Radiant. Verwende für Richtungen, Punkte und Normalen die jeweils passende Transformation. Geprüfte Operationen melden ungültige/nicht darstellbare Ergebnisse.'),
    'memory': ('Allocator und Arena', 'memory.md', 'Speicher gehört einem expliziten Allocator. Verwende denselben Allocator und die korrekte Größe beim Freigeben. Eine Arena nutzt einen Puffer des Aufrufers; Reset macht alle daraus vergebenen Bereiche logisch ungültig. PS_MEMORY_ALIGNMENT bezeichnet die unterstützte fundamentale Ausrichtung.'),
    'array': ('Dynamische Arrays', 'array.md', 'Initialisieren, anhängen/einfügen, lesen und am Ende zerstören. Elemente werden byteweise kopiert, enthalten also keine automatisch verwalteten Unterobjekte. Wachstum kann alle Elementzeiger invalidieren. clear hält die Kapazität; destroy gibt den Speicher frei.'),
    'string_view': ('String-Views', 'string-view.md', 'Ein View leiht Bytes und besitzt keinen Speicher. Der Quellpuffer muss leben. Indizes zählen Bytes, nicht Unicode-Zeichen; abschließende Nullzeichen sind nicht erforderlich. Verwende copy, wenn du einen eigenen terminierenden Puffer benötigst.'),
    'hashmap': ('Hashmap', 'hashmap.md', 'Erzeuge eine Map mit Wertgröße und expliziten Grenzen. set kopiert Schlüssel und Wert, get kopiert in deinen Ausgabepuffer. PS_EOF bedeutet fehlender Schlüssel. Besuchercallbacks dürfen lesen, aber keine Änderungen an derselben Map vornehmen. destroy gibt alle map-eigenen Speicherbereiche frei.'),
}


def comment_text(value):
    return re.sub(r'\s+', ' ', re.sub(r'/\*|\*/|^\s*\*\s?', '', value, flags=re.M)).strip()


def declarations(source):
    """Split top-level C declarations, keeping comments adjacent to declarations."""
    source = re.sub(r'^\s*#.*(?:\\\n.*)*$', '', source, flags=re.M)
    # The only platform-dependent public typedef; document the portable name.
    source = source.replace('typedef max_align_t ps_memory_alignment;', '')
    depth, start = 0, 0
    token = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|[{};]', re.S)
    for m in token.finditer(source):
        t = m.group()
        if t == '{': depth += 1
        elif t == '}': depth -= 1
        elif t == ';' and depth == 0:
            raw = source[start:m.end()].strip()
            start = m.end()
            comments = re.findall(r'/\*.*?\*/|//[^\n]*', raw, re.S)
            code = re.sub(r'/\*.*?\*/|//[^\n]*', '', raw, flags=re.S).strip()
            code = re.sub(r'\n\s*\n', '\n', code)
            code = '\n'.join(line.rstrip() for line in code.splitlines())
            if code: yield code, [comment_text(c) for c in comments]


def c_reference(module, title, guide, intro):
    source = (ROOT / 'include/physim' / (module + '.h')).read_text(encoding='utf-8')
    result = [f'# C-Referenz: {title}', intro,
              f'[Anleitung und Beispiele](../{guide}) · [Teil I – C](../c-guide.md) · [Fehlercodes](../troubleshooting.md)',
              f'Einbinden: `#include "physim/{module}.h"`. Die folgenden Signaturen, Typen und SDK-Verträge sind vollständig für dieses Modul. Die SDK-Verträge sind im englischen Original wiedergegeben; die verlinkte Anleitung erklärt den Einsatz auf Deutsch. Funktionen mit `out` schreiben in Speicher des Aufrufers; konkrete Fehler- und Lebensdauerregeln stehen beim jeweiligen Vertrag.',
              '## Konstanten']
    macros = [(n, v) for n, v in re.findall(r'^#define (PS_\w+)\s+([^\n]+)', source, re.M)
              if n != 'PS_EXPORT']
    if macros:
        result.append('```c\n' + '\n'.join('#define ' + n + ' ' + v for n, v in macros) + '\n```')
    else: result.append('Dieses Modul definiert keine zusätzlichen Zahlenkonstanten.')
    if module == 'experiment':
        result.append('`PS_EXPORT` kennzeichnet den Moduleinstieg für den Export. Das SDK wählt dafür automatisch die passende Windows- beziehungsweise Unix-Deklaration.')
    result.append('## Typen und Funktionen')
    for code, comments in declarations(source):
        names = re.findall(r'\b(ps_\w+)\s*\(', code)
        if names and not code.startswith('typedef'):
            result.append('## ' + names[0])
            assert names[0] in DESCRIPTIONS, f'Missing C description: {names[0]}'
            result.append(DESCRIPTIONS[names[0]])
            code = re.sub(r'\s+', ' ', code)
            if len(code) > 88:
                opening = code.index('(') + 1
                code = code[:opening] + '\n    ' + ',\n    '.join(code[opening:-2].split(', ')) + ');'
        else:
            named = re.findall(r'\b(?:ps_\w+|PS_\w+)\b', code)
            result.append('### ' + (named[-1] if named else 'Datentyp'))
        result.append('```c\n' + code + '\n```')
        result.extend(comments)
    return '\n\n'.join(result) + '\n'


LANG_DESCRIPTIONS = {
    'propertyConstant': 'Wertet eine konstante SI-Quantity im geschlossenen T/P-Gültigkeitsbereich domain=(Tmin,Tmax,Pmin,Pmax) aus; name/source sind explizite Metadaten. Außerhalb des Bereichs entsteht ein abfangbarer Fehler.',
    'propertyTable': 'Interpoliert temperatur-/druckabhängige SI-Eigenschaftsdaten mit je 1–64 streng steigenden Achsen und temperaturweise angeordneten Werten. Eine Einpunktachse bedeutet Unabhängigkeit von dieser Koordinate; keine Extrapolation. Arrays und Metadaten können in eigenen besitzenden Wertstrukturen gespeichert werden.',
    'pipeConductance': 'Berechnet π r⁴/(8 μ L) in m³/(s Pa) für ein ideales laminares kreiszylindrisches Rohr. Positive SI-Radius-, Längen- und Viskositätswerte.',
    'pipeFlow': 'Berechnet Q=G(pa-pb) in m³/s mit signierten Relativdrücken und G≥0, positiv von A nach B.',
    'pipePower': 'Berechnet die nichtnegative hydraulische Dissipation G(pa-pb)² in W.',
    'reynoldsNumber': 'Berechnet ρ |v| d/μ. Die Gültigkeitsgrenze eines Modells wählt der Aufrufer, nicht diese Funktion.',
    'hydrostaticPressure': 'Berechnet p0+ρ g h für signierte Relativdrücke und Tiefe nach unten; Dichte positiv, g≥0.',
    'pipeNetwork': 'Löst ein passives stationäres Netz mit bis zu 16 Knoten und 32 Kanten. Int64-Endpunkte, Leitwerte, 0/1-Fixflags und Drücke. Liefert ein besitzendes Float64-Array: zuerst alle Knotendrücke, dann A→B-Kantenflüsse. Jede positive Verbindungskomponente braucht einen Fixknoten.',
    'transportStep': 'Erzeugt ein neues besitzendes Konzentrationsarray für periodische Upwind-Advektion und explizite Diffusion. 3–4096 nichtnegative Zellen; |v|dt/dx+2Ddt/dx²≤1, D≥0. Masse und Maximumprinzip bleiben bis auf Rundung erhalten. Kein Fluidimpulssolver.',
    'harmonicStep': 'Exakter undämpfter Oszillator mit Vec2.x=Position in m und y=Geschwindigkeit in m/s; omega>0 in rad/s, dt≥0.',
    'stringWaveSpeed': 'Berechnet sqrt(T/μ) in m/s aus positiver Spannungskraft in N und linearer Dichte in kg/m.',
    'travelingWave': 'Wertet A sin(kx-ωt+φ) aus: Vec3.x Verschiebung in m, y Geschwindigkeit in m/s, z dimensionslose Steigung. k,ω>0; keine Dispersion wird implizit gewählt.',
    'stringWaveStep': 'Erzeugt ein neues besitzendes Float64-Array für einen zentrierten 1D-Saitenschritt. Gleiche 3–4096 Knotenanzahl, endliche Meterwerte, feste Nullränder, konstantes dt und c dt/dx≤1. Eingabekopien bleiben unabhängig; Fehler sind abfangbar.',
    'reflectRay': 'Reflektiert eine Unit-Richtung an einer Unit-Normale in das Einfallsmedium; Normalenorientierung wird geprüft.',
    'refractRay': 'Berechnet eine Unit-Richtung nach Snell für positive Indizes. Bei Totalreflexion entsteht eine abfangbare Singularitätsdiagnose; Reflexion muss ausdrücklich gewählt werden.',
    'thinLensImage': 'Paraxiale dünne Linse: Vec2.x signierte Bildweite in m, y Vergrößerung. Positive Objektweite und signierte nonzero Brennweite; am Fokus liegt das Bild im Unendlichen und erzeugt eine abfangbare Singularitätsdiagnose.',
    'simulationTimeStep': 'Liest das aktuell konfigurierte positive dt in s aus dem Experimentcontext, auch bei create/reset. Nur im Experimenthost; variable Intervalle sind dadurch kein erlaubter Leapfrog-Zeitschritt.',
    'vacuumPermittivity': 'Liefert die gemessene Vakuumpermittivität nach CODATA 2022 in F/m, keine exakte SI-Konstante.',
    'pointChargeField': 'Berechnet das elektrische Feld einer Punktladung in V/m in einem homogenen unendlichen Medium mit ausdrücklich angegebener Permittivität. Am Quellpunkt entsteht eine abfangbare Singularitätsdiagnose.',
    'pointChargePotential': 'Berechnet q/(4π ε r) in V mit Nullpunkt im Unendlichen, ohne Softening oder Grenzflächen.',
    'lorentzForce': 'Berechnet q(E+v×B) in N für vorgegebene SI-Felder und nichtrelativistische Geschwindigkeit. Keine automatische Integration oder Strahlungsreaktion.',
    'resistorCurrent': 'Berechnet I=V/R in A für einen positiven idealen Widerstand in ohm.',
    'resistorVoltage': 'Berechnet V=IR in V, mit signiertem Strom.',
    'resistorPower': 'Berechnet die nichtnegative Verlustleistung V²/R in W.',
    'seriesResistance': 'Addiert zwei strikt positive Widerstände, Ergebnis in ohm.',
    'parallelResistance': 'Berechnet den Gesamtwiderstand zweier positiver Parallelwiderstände ohne Zwischenüberlauf.',
    'capacitorEnergy': 'Berechnet 0,5 C V² in J für eine positive Kapazität in F.',
    'rcVoltageStep': 'Exakter Spannungsschritt eines konstanten RC-Serienkreises mit konstanter Quelle: dt≥0, positiver Widerstand und Kapazität. Keine Zeitschritt-Stabilitätsgrenze; Fehler sind mit attempt abfangbar.',
    'idealGasPressure': 'Berechnet p=nRT/V in Pa für positive Stoffmenge, Kelvin und Volumen in m³.',
    'idealGasVolume': 'Berechnet V=nRT/p in m³ für ein ideales Gas.',
    'idealGasTemperature': 'Berechnet T=pV/(nR) in Kelvin für ein ideales Gas.',
    'idealGasEnergy': 'Berechnet U=n cv T in J bei konstantem molarem cv; Referenz U=0 bei T=0.',
    'idealGasEntropyChange': 'Berechnet n[cv ln(T1/T0)+R ln(V1/V0)] in J/K zwischen zwei Gleichgewichtszuständen desselben idealen Gases; konstantes molares cv.',
    'vdwGasPressure': 'Homogenes Van-der-Waals-Modell: p=nRT/(V-nb)-an²/V². Molares a in Pa m⁶/mol², b in m³/mol; V>nb. Signierte Werte, keine Phasenauswahl.',
    'vdwGasPressureDerivative': 'Ableitung bei festem n,T: -nRT/(V-nb)²+2an²/V³ in Pa/m³. Positiv bedeutet mechanisch instabile homogene Algebra; keine Maxwell-Konstruktion.',
    'vdwGasEnergy': 'U=n cv T-an²/V in J bei konstantem a,cv. Referenz T→0,V→∞; keine Kalibrierung. Signierte Werte.',
    'vdwGasEntropyChange': 'n[cv ln(T1/T0)+R ln((V1-nb)/(V0-nb))] in J/K bei konstantem cv,b und derselben Stoffmenge. V0,V1>nb. Fehler mit attempt abfangbar.',
    'heatCapacity': 'Berechnet C=m c in J/K aus Masse in kg und konstanter spezifischer Wärmekapazität in J/(kg K).',
    'sensibleHeat': 'Berechnet Q=C(T1-T0) in J; positiv bedeutet Erwärmung. Keine latente Wärme.',
    'heatFlow': 'Berechnet P=G(Ta-Tb) in W; positiv fließt Wärme von A nach B, G in W/K ist nichtnegativ.',
    'thermalReservoirStep': 'Exakte Temperatur nach dt an einem Reservoir fester Temperatur: konstante Kapazität und Leitwert, kein Zeitschritt-Stabilitätslimit. Kelvin; G und dt dürfen null sein.',
    'thermalPairStep': 'Exakte isolierte Zweikörperrelaxation bei konstanten Kapazitäten und Leitwert. Vec2.x/y enthält A/B in Kelvin; Energie bleibt bis auf Rundung erhalten. Fehler sind mit attempt abfangbar.',
    'group': 'Benannte Szenengruppe mit eindeutiger ID; parent 0 erzeugt eine Wurzel. Nur im scene-Callback. Gruppen enthalten keine Geometrie und verändern keine Weltkoordinaten.',
    'Diagnostic': 'Erzeugt einen besitzenden begrenzten Diagnosewert. Fehlercode 1–10 außer EOF/Recovered, 1-basierte Quellposition oder null für unbekannt. Ungültige Werte werfen eine Quelldiagnose.',
    'diagnosticHere': 'Wie Diagnostic mit automatisch erfasstem Quellpfad, Zeile und Spalte dieser Factory-Expression.',
    'emptyDiagnostic': 'Erzeugt einen gültigen leeren Diagnosewert mit Code 0.',
    'diagnosticValid': 'Prüft die Versions-, Text-, Fehlercode- und Positionsregeln des Werts.',
    'diagnosticCode': 'Liefert den unveränderten Core-Fehlercode; 0 bezeichnet keinen Fehler.',
    'diagnosticLine': 'Liefert die 1-basierte Zeile oder 0 bei unbekannter Quellposition.',
    'diagnosticColumn': 'Liefert die 1-basierte Spalte oder 0 bei unbekannter Spalte.',
    'diagnosticOperation': 'Liefert die Operation als eigenen String-Wert.',
    'diagnosticArgument': 'Liefert das betroffene Argument als eigenen String-Wert.',
    'diagnosticSource': 'Liefert den ursprünglichen Quellpfad als eigenen String-Wert.',
    'diagnosticMessage': 'Liefert den vollständigen UTF-8-Nachrichtentext als eigenen String-Wert.',
    'diagnosticFormatted': 'Formatiert den Diagnosewert für die menschliche Anzeige; Quelldaten bleiben separat erhalten.',
    'diagnosticEncoded': 'Kodiert einen Fehler als begrenztes versioniertes Bytearray mit CRC; Int64-Werte 0–255.',
    'diagnosticDecoded': 'Dekodiert ein vollständiges Bytearray mit Version-, Längen-, CRC- und UTF-8-Prüfung; Fehler können mit attempt abgefangen werden.',
    'saveDiagnostic': 'Speichert einen Fehler exklusiv in eine neue Datei; vorhandene Dateien bleiben erhalten.',
    'loadDiagnostic': 'Lädt und validiert einen vollständig gespeicherten Diagnosewert.',
    'raiseDiagnostic': 'Löst einen Fehler mit den Feldern dieses Diagnosewerts aus. attempt fängt ihn innerhalb eines Wertausdrucks ab; außerhalb endet der Callback bzw. das Standalone-Programm.',
    'currentDiagnostic': 'Liefert eine Kopie der aktuellen Experimentdiagnose, bei Erfolg einen leeren Wert. Kein globaler Last-error-Zustand.',
    'sceneTransform': 'Liefert die zusammengesetzte Local-to-world-Matrix des Szenenslots. Nur im scene-Callback; index beginnt bei 0. Rahmen enthalten ihre eigene TRS, Geometrie nur ihre Frame-Vorfahren.',
    'sceneWorldPoint': 'Konvertiert einen Punkt in der Basis des Szenenslots in Weltkoordinaten; nur im scene-Callback. Fehler erhalten eine Quelldiagnose.',
    'sceneFrame': 'Erzeugt einen expliziten lokalen TRS-Koordinatenrahmen. Nur im scene-Callback; parent 0 bezeichnet die Wurzel. Endliche Translation, nonzero Quaternion und endliche nonzero Scale; Nachfahren werden für Darstellung und Picking transformiert.',
    'sceneParent': 'Ordnet einen Szeneneintrag einer Eltern-ID zu; parent 0 löst ihn zur Wurzel. Fehlende IDs und Zyklen erzeugen eine Quelldiagnose, ohne die Szene zu verändern.',
    'Bezier3': 'Kubische räumliche Bézierkurve aus vier endlichen Kontrollpunkten. Der Wert ist unabhängig kopierbar. Alle Koordinaten verwenden dieselbe Längeneinheit des Aufrufers.',
    'bezierControlPoint': 'Kopiert einen Kontrollpunkt mit Index 0..3; ungültige Indizes werfen einen Fehler.',
    'bezierPosition': 'Position bei dimensionslosem t im inklusiven Intervall [0, 1]. Die Auswertung verwendet die De-Casteljau-Implementierung der C-Bibliothek.',
    'bezierTangent': 'Ableitung der Position nach dem dimensionslosen Parameter t. Sie ist weder normiert noch eine physikalische Geschwindigkeit. Ein nicht darstellbares Ergebnis erzeugt eine Quelldiagnose.',
    'bezierSplitLeft': 'Linke Teilkurve von 0 bis t, mit eigenem Parameterbereich [0, 1]. Ungültiges t erzeugt eine Quelldiagnose.',
    'bezierSplitRight': 'Rechte Teilkurve von t bis 1, ebenfalls auf [0, 1] umparametrisiert.',
    'Mat3': '3×3-Matrix aus drei Spaltenvektoren. Unabhängiger Wert; alle Komponenten müssen endlich sein. Wirkt auf Spaltenvektoren.',
    'Mat4': '4×4-Matrix aus vier Spaltenvektoren. Unabhängiger Wert; alle Komponenten müssen endlich sein. Homogene Koordinaten mit w in der vierten Komponente.',
    'identityMat3': '3×3-Einheitsmatrix.',
    'identityMat4': '4×4-Einheitsmatrix.',
    'elementMat3': 'Liest ein Element mit nullbasierten row/column in 0..<3; ungültige Indizes erzeugen eine Quelldiagnose.',
    'elementMat4': 'Liest ein Element mit nullbasierten row/column in 0..<4; ungültige Indizes erzeugen eine Quelldiagnose.',
    'multiplyMat3': 'Matrixprodukt; die rechte Matrix wird zuerst angewandt. Nichtendliche Ergebnisse werden abgewiesen.',
    'multiplyMat4': 'Matrixprodukt; die rechte Matrix wird zuerst angewandt. Nichtendliche Ergebnisse werden abgewiesen.',
    'transposeMat3': 'Vertauscht Zeilen und Spalten; verändert den Empfänger nicht.',
    'transposeMat4': 'Vertauscht Zeilen und Spalten; verändert den Empfänger nicht.',
    'applyMat3': 'Matrix-Vektor-Produkt ohne Normalisierung; nichtendliche Ergebnisse werden abgewiesen.',
    'applyMat4': 'Homogenes Matrix-Vektor-Produkt ohne Division durch w; für Punkte transformPoint verwenden.',
    'inverseMat3': 'Inverse mit skalierter Pivotwahl. pivotTolerance=0 verwendet 3×Maschinengenauigkeit; sonst muss 0<t<1 gelten. Abgewiesene Pivots und numerische Fehler erzeugen Quelldiagnosen. Die Toleranz ist keine Konditionsschätzung.',
    'inverseMat4': 'Inverse mit skalierter Pivotwahl. pivotTolerance=0 verwendet 4×Maschinengenauigkeit; sonst muss 0<t<1 gelten. Abgewiesene Pivots und numerische Fehler erzeugen Quelldiagnosen. Die Toleranz ist keine Konditionsschätzung.',
    'translationMat4': 'Affine Translation; verschiebt Punkte, nicht Richtungen. Einheiten bestimmt das Modell.',
    'scaleMat4': 'Lokale XYZ-Skalierung. Negative und nullwertige Faktoren sind erlaubt; Nullskalierung ist nicht invertierbar.',
    'rotationMat4': 'Rechtshändige aktive Rotation; Quaternion wird normiert. Nullquaternion wird abgewiesen.',
    'trsMat4': 'Lokale Skalierung, dann Rotation, dann Translation. Quaternion wird normiert. Nullskalierung erlaubt; keine automatische Einheitenkonvertierung.',
    'transformPoint': 'Transformiert einen Punkt mit homogenem w=1 und dividiert durch das Ergebnis-w. Auch projektive Matrizen erlaubt; w=0 oder nichtendliches Ergebnis erzeugt eine Quelldiagnose.',
    'transformDirection': 'Transformiert eine Richtung ohne Translation und ohne Normalisierung. Erfordert die affine letzte Zeile [0,0,0,1] exakt.',
    'transformNormal': 'Inverse-transponierte Transformation einer von null verschiedenen Flächennormale, anschließend normiert. Erfordert eine affine Matrix mit invertierbarem linearem Anteil; unter nichtuniformer Skalierung verschieden von transformDirection.',
    'sweepSpheres': 'Erster Kontakt zweier Kugeln entlang vorgegebener linearer Verschiebungen in m. Gespeicherte Geschwindigkeiten werden nicht verwendet. Das Sweep-Ergebnis enthält hit; Anfangsberührung und Überlappung zählen auch bei Trennung als Treffer bei fraction()=0.',
    'sweepSpherePlane': 'Erster Kontakt einer linear verschobenen Kugel mit einer Ebene. point liegt auf der Ebene; die Einheitsnormale zeigt in den freien Halbraum. Keine Beschleunigung oder automatische Kollisionsantwort.',
    'sweepFraction': 'Anteil der vorgegebenen Verschiebung bis zum ersten Kontakt in 0..1. Ohne Treffer ist der Aufruf ein Laufzeitfehler; vorher hit prüfen.',
    'sweepContacts': 'Ein Kontakt am berechneten Ereignis oder ein leeres Contacts bei fehlendem Treffer. Vor der Kollisionsantwort Körper bis zur Ereigniszeit bewegen; danach Restzeit neu berechnen.',
    'sphereBounds': 'Nach außen gerundete Welt-AABB einer Kugel. minimum/maximum sind schreibgeschützte Vec3-Werte in m.',
    'boxBounds': 'Nach außen gepolsterte Welt-AABB eines gedrehten Quaders; size sind volle lokale Seitenlängen in m. minimum/maximum sind schreibgeschützt.',
    'sweptSphereBounds': 'Welt-AABB der gesamten linear verschobenen Kugel; für kontinuierliche Kandidatensuche statt alleiniger Anfangs-/Endhülle verwenden.',
    'collisionPairs': 'Kandidatenpaare aus höchstens 1024 AABBs, lexikographisch nach bodyA/bodyB sortiert, ohne Duplikate. bodyA<bodyB sind schreibgeschützte Indizes des Eingabearrays. Berührung zählt; Kandidaten benötigen geometrische Feinprüfung. Das unabhängige Ergebnisarray zählt zum Sprachspeicherbudget. Ebenen separat prüfen.',
    'contactConstraint': 'Bindet einen nullbasierten Kontaktpunkt an Körperindizes bodyA/bodyB. Nur bodyB darf -1 für die feste Welt sein. Gleiche oder außerhalb 0..127 liegende Indizes sind ungültig. Die schreibgeschützten Eigenschaften sind bodyA, bodyB, point, normal und penetration.',
    'jointConstraint': 'Bindet das Distanzgelenk an zwei verschiedene Körperindizes. bodyB=-1 verankert anchorB in Weltkoordinaten. bodyA, bodyB und joint sind schreibgeschützt.',
    'solveConstraints': 'Löst Kontakte und Distanzgelenke gemeinsam für unabhängige Körperkopien. Maximal 128 Körper, 512 Kontakte und 256 Gelenke; dt muss positiv sein. Körperindizes müssen zum Array passen. Eingaben bleiben bei Erfolg und Fehler unverändert. Ergebnis besitzt automatisch verwalteten Speicher im Sprachbudget. bodyCount/contactCount/jointCount sowie maxNormalError, maxProjectionError, maxJointVelocityError und maxJointLengthError sind schreibgeschützt. Ein erfolgreicher Aufruf garantiert keine Konvergenz.',
    'constraintBody': 'Liefert eine unabhängige Körperkopie am nullbasierten Index; index muss kleiner als bodyCount sein.',
    'constraintBodies': 'Liefert alle berechneten Körper als unabhängiges Array; dessen Änderungen beeinflussen den Ergebniswert nicht.',
    'constraintContactImpulse': 'Gesamter Impuls auf Körper A des nullbasierten Kontaktconstraints in N s. Reihenfolge wie das übergebene Kontaktarray.',
    'constraintJointImpulse': 'Gesamter Impuls auf Körper A des nullbasierten Gelenkconstraints in N s. Reihenfolge wie das übergebene Gelenkarray.',
    'DistanceJoint': 'Erzeugt ein Distanzgelenk mit zwei lokalen Vec3-Ankern, positiver Soll-Länge in m und Stabilisierung in 0..1. Die Eigenschaften anchorA, anchorB, length und stabilization sind schreibgeschützt.',
    'resolveJoint': 'Löst das Gelenk für zwei Körperkopien bei positivem dt in Sekunden. Nach äußeren Geschwindigkeitsänderungen und vor dem Positionsschritt verwenden. Das Ergebnis enthält bodyA/bodyB, impulse auf A in N s, lengthError vor dem Lösen in m und velocityError in m/s. Positionen bleiben unverändert. Ein Körper mit Masse null dient als Weltanker; zusammenfallende Anker sind ein Fehler. Stabilisierung kann Energie zuführen.',
    'ContactSolver': 'Geprüfte Einstellungen: iterations in 1..256, restitution in 0..1, nichtnegative friction, bounceThreshold in m/s und penetrationSlop in m sowie correctionFraction in 0..1. Alle Eigenschaften sind schreibgeschützt.',
    'defaultContactSolver': 'Standardwerte des gemeinsamen iterativen Kontaktpaarsolvers.',
    'sphereContacts': 'Kontakt einer Kugel A mit Kugel B; Radien in m. Liefert null oder einen Kontakt.',
    'spherePlaneContacts': 'Kugelkontakt mit einer Ebene durch point und Einheitsnormalen normal zur freien Seite.',
    'sphereBoxContacts': 'Kontakt der Kugel A mit einem orientierten Quader B; size sind die vollen lokalen Seitenlängen in m.',
    'boxPlaneContacts': 'Kontaktpunkte eines orientierten Quaders mit einer Ebene. size sind volle lokale Seitenlängen; normal zeigt zur freien Seite.',
    'boxContacts': 'Kontaktpunkte zweier orientierter Quader; sizeA/sizeB sind volle lokale Seitenlängen in m.',
    'contactPoint': 'Weltpunkt des nullbasierten Kontakts in m. index muss kleiner als count sein.',
    'contactNormal': 'Einheitsnormale des nullbasierten Kontakts von A nach B. index muss kleiner als count sein.',
    'contactPenetration': 'Nichtnegative Eindringtiefe des nullbasierten Kontakts in m.',
    'resolveContacts': 'Löst das Kontaktpaar mit Reibung und Rückprall. Ergebniswerte bodyA/bodyB müssen ausdrücklich übernommen werden; Eingaben bleiben unverändert. Die Kontakte müssen zu denselben Körperzuständen und derselben Reihenfolge gehören. Ein Körper mit Masse null repräsentiert eine feste Umgebung.',
    'resolveSingleContact': 'Löst genau einen Kontakt mit der C-Einzelkontaktantwort. Rückprall in [0, 1], Reibung nichtnegativ; die Eingabekörper bleiben unverändert und korrigierte Körper sowie Impuls stehen im ContactResult.',
    'contactImpulse': 'Resultierender Kontaktimpuls auf A in N s am nullbasierten Index; index muss kleiner als result.count sein.',
    'sphereBody': 'Homogene Vollkugel mit mass in kg und positivem radius in m, anfangs ruhend im Ursprung. mass null erzeugt einen statischen Körper.',
    'boxBody': 'Homogener Quader mit mass in kg und positiven vollen lokalen Seitenlängen size in m, anfangs ruhend im Ursprung. mass null erzeugt einen statischen Körper.',
    'bodySetState': 'Setzt Position in m, Geschwindigkeit in m/s, Einheitsquaternion und Winkelgeschwindigkeit in rad/s. Verändert den Empfänger erst nach vollständiger Prüfung; statische Körper müssen ruhen.',
    'bodyApplyImpulse': 'Wendet einen Impuls in N s an einem Weltpunkt in m an; verändert Translation und Rotation des Empfängers.',
    'bodyStep': 'Bewegt den Empfänger mit Weltkraft in N und Drehmoment in N m um positives dt in s. Symplektische Translation, explizite gyroskopische Rotation; Schrittweite verfeinern.',
    'bodyKineticEnergy': 'Summe der translatorischen und rotatorischen kinetischen Energie in J.',
    'bodyPointVelocity': 'Weltgeschwindigkeit in m/s an einem Weltpunkt in m, einschließlich Rotation.',
    'bodyForceTorque': 'Drehmoment in N m einer Weltkraft in N an einem Weltpunkt in m; verändert den Körper nicht.',
    'maskSeries': 'Erhält alle Zeilen und markiert Werte nur dann gültig, wenn Eingabe und Selektor gültig sind und der Selektor exakt accepted entspricht.',
    'seriesValidity': 'Liefert eine ausgerichtete, unmaskierte Reihe aus 0/1-Gültigkeitsflags.',
    'seriesHasMask': 'Prüft die ausdrücklich gespeicherte Maske der Reihe, auch wenn alle Werte gültig sind.',
    'seriesIsValid': 'Prüft die Gültigkeit am nullbasierten Zeilenindex; ungültige Handles oder Indizes sind Quellfehler.',
    'selectSeries': 'Filtert mehrere ausgerichtete Reihen gemeinsam nach dem exakten Selektorwert, etwa Status 1. Ergebnisreihen behalten dieselbe Zeilenzuordnung.',
    'Table': 'Erstellt eine Berichtstabelle mit Spaltennamen und Einheiten. Beide Arrays müssen gleich lang sein.',
    'tableRow': 'Fügt eine benannte Tabellenzeile mit passenden Quantity-Werten hinzu.',
    'exportTable': 'Exportiert alle Tabellenzeilen als CSV mit einem Suffix am Ausgabepräfix.',
    'constantDistribution': 'Konstante Verteilung ohne Streuung.',
    'uniformDistribution': 'Gleichverteilung zwischen Minimum und Maximum.',
    'normalDistribution': 'Normalverteilung mit Mittelwert und nichtnegativer Standardabweichung.',
    'distributionMean': 'Erwartungswert der Verteilung.',
    'distributionDeviation': 'Standardabweichung der Verteilung.',
    'SensorConfig': 'Definiert Einheit, Abtastrate in Hz, Startzeit in s, Auflösung, Offset, Drift pro Sekunde, Rauschen, Ausfallwahrscheinlichkeit und absolute/relative Standardunsicherheit. Auflösung, Offset, Drift und Rauschen beziehen sich auf die Sensoreinheit.',
    'Sensor': 'Initialisiert einen Sensor mit Konfiguration und explizitem Seed.',
    'sensorForRun': 'Initialisiert einen Sensor mit einem aus Laufseed und Streamnummer abgeleiteten Zufallsstrom.',
    'sensorReset': 'Setzt Abtastraster und Zufallsstrom des Sensors mit einem expliziten Seed zurück; verändert den Empfänger.',
    'sensorResetForRun': 'Setzt den Sensor anhand von Laufseed und Stream zurück; verändert den Empfänger.',
    'sensorRead': 'Wertet den Sensor bei time in Sekunden aus. truth ist eine Quantity mit kompatibler Einheit. Verändert den Sensorzustand; das Ergebnis enthält Wert, Status und Unsicherheit.',
    'sensorNextTime': 'Nächster planmäßiger Abtastzeitpunkt in Sekunden.',
    'measurementValid': 'Wahr nur für eine gültige Messung (Status 1).',
    'measurementDue': 'Wahr, wenn eine Messung fällig war; auch bei Ausfall möglich.',
    'measurementDropped': 'Wahr bei ausgefallener Messung (Status 2).',
    'Rng': 'Erzeugt einen unabhängigen PCG32-Zufallsstrom mit explizitem Int64-Seed.',
    'rngForRun': 'Erzeugt einen eigenen Zufallsstrom aus dem vollständigen Laufseed und einer Streamnummer; nur im Experiment.',
    'rngSample': 'Zieht mutierend einen Wert aus der Verteilung; der Zufallsstrom muss als var gebunden sein.',
    'rngReseed': 'Setzt einen veränderlichen Zufallsstrom auf den angegebenen Seed zurück.',
    'rngReseedForRun': 'Setzt einen veränderlichen Zufallsstrom aus Laufseed und Streamnummer zurück; nur im Experiment.',
    'seriesFromValues': 'Konvertiert endliche Float64-Werte aus der deklarierten Einheit in eine eigenständige SI-Reihe; unitScale ist 1, value liefert SI. Jede Wurzel hat ein eigenes Alignment.',
    'seriesAlignedValues': 'Konvertiert gleich viele endliche Float64-Werte aus der Eingabeeinheit nach SI; Skala 1, Alignment und Lebensdauer des Ankers. Eingabearrays bleiben erhalten.',
    'sin': 'Sinus eines Winkels in Radiant.', 'cos': 'Kosinus eines Winkels in Radiant.',
    'tan': 'Tangens eines Winkels in Radiant; nicht endliche Ergebnisse sind Laufzeitfehler.',
    'asin': 'Arkussinus eines Werts in [-1, 1], Ergebnis in Radiant.',
    'acos': 'Arkuskosinus eines Werts in [-1, 1], Ergebnis in Radiant.',
    'atan': 'Arkustangens eines Werts, Ergebnis in Radiant.',
    'atan2': 'Winkel des Vektors (x, y) in Radiant; beide Komponenten dürfen nicht zugleich null sein.',
    'expm1': 'Berechnet exp(x)-1 ohne Auslöschung für kleine x; nicht endliche Ergebnisse sind Laufzeitfehler.',
    'exp': 'Exponentialfunktion zur Basis e; nicht endliche Ergebnisse sind Laufzeitfehler.',
    'clamp': 'Begrenzt value auf das inklusive Intervall. Wenn lower größer als upper ist, entsteht ein Laufzeitfehler mit Quellposition.',
    'min': 'Kleinerer Wert; bei Gleichheit bleibt der linke Wert erhalten.',
    'max': 'Größerer Wert; bei Gleichheit bleibt der linke Wert erhalten.',
    'linearSolve': 'Löst A*x = rhs mit skalierter Pivotwahl. rhs enthält 1 bis 32 Werte; coefficients enthält genau rhs.count² Werte in Zeilenreihenfolge. pivotTolerance=0 wählt n mal die Maschinengenauigkeit, sonst gilt 0<t<1. Eingaben bleiben unverändert. Formfehler, Singularität und numerische Fehler erzeugen Quelldiagnosen. Das Ergebnis ist ein eigener Array-Wert im Sprachspeicherbudget.',
    'rootBisect': 'Sucht eine Nullstelle einer stetigen Funktion mit der gemeinsamen C-Bisektionsroutine. function bezeichnet eine freie, nicht generische Funktion des aktuellen Moduls mit genau einem Float64-Parameter und Float64-Rückgabe. Endliche, aufsteigende Intervallgrenzen müssen eine Nullstelle einklammern oder selbst Nullstelle sein. absoluteTolerance>0, 0<=relativeTolerance<1 und 1<=maxIterations<=100000 sind erforderlich. Das Ergebnis ist die gefundene X-Koordinate; ungültige Argumente, nicht endliche Funktionswerte und fehlende Konvergenz erzeugen Quelldiagnosen. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein; ein numerischer Bericht wird noch nicht ausgegeben.',
    'minimizeGolden': 'Sucht das Minimum einer unimodalen skalaren Funktion mit dem gemeinsamen Goldener-Schnitt-Verfahren. function bezeichnet eine freie, nicht generische Funktion des aktuellen Moduls mit genau einem Float64-Parameter und Float64-Rückgabe. Das Intervall muss endlich und aufsteigend sein; absoluteTolerance>0, 0<=relativeTolerance<1 und 1<=maxIterations<=100000 sind erforderlich. Das Ergebnis ist die gefundene X-Koordinate; ungültige Argumente, nicht endliche Funktionswerte und fehlende Konvergenz erzeugen Quelldiagnosen. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein; ein numerischer Bericht wird noch nicht ausgegeben.',
    'rootBisectReported': 'Wie rootBisect mit denselben Eingaben und Fehlerregeln. ScalarResult enthält x und den zugehörigen Funktionswert value, die letzten Intervallgrenzen lower und upper sowie iterations und evaluations. Ein bei einer Intervallgrenze liegender Nullpunkt benötigt keine Iteration. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein.',
    'minimizeGoldenReported': 'Wie minimizeGolden mit denselben Eingaben und Fehlerregeln. ScalarResult enthält x und den zugehörigen Funktionswert value, die letzten Intervallgrenzen lower und upper sowie iterations und evaluations. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein.',
    'eulerStep': 'Expliziter Euler-Schritt mit der gemeinsamen C-Integrationsroutine. derivative bezeichnet eine freie, nicht generische Funktion des aktuellen Moduls mit (Float64, [Float64]) -> [Float64]. Der Zustand enthält 1 bis 32 endliche Werte; dt muss endlich und positiv sein. Der Callback erhält eine eigene Kopie des aktuellen Zustands und muss gleich viele endliche Ableitungswerte liefern. Das Ergebnis ist ein unabhängiges Array; die Eingabe bleibt unverändert. Ungültige Eingaben, abweichende Dimensionen und nicht endliche Zwischenwerte erzeugen Quelldiagnosen. Für reproduzierbare Ergebnisse sollte der Callback keine sichtbaren Seiteneffekte haben.',
    'rk4Step': 'Klassischer RK4-Schritt mit vier Ableitungsauswertungen der gemeinsamen C-Integrationsroutine. Signatur, Zustandsgrenze, Schrittbedingung, Besitz und Fehlerregeln entsprechen eulerStep. Die Ableitung erhält Zeit und jeweiligen Zwischenzustand als eigene Werte. Adaptive Schrittweite und Ereigniserkennung sind hier nicht enthalten.',
    'rk45Integrate': 'Adaptiver Dormand–Prince-5(4)-Integrator der gemeinsamen C-Bibliothek. derivative ist eine freie, nicht generische Funktion mit (Float64, [Float64]) -> [Float64]. state enthält 1 bis 32 endliche Werte und bleibt unverändert; das Ergebnis ist ein eigenes Array. Die endlichen Grenzen dürfen vorwärts oder rückwärts verlaufen. absoluteTolerance muss positiv und endlich, relativeTolerance endlich und in [0, 1) und maxSteps zwischen 1 und UINT32_MAX/7 liegen. Das Schrittbudget zählt angenommene und verworfene Versuche. Abbrüche melden die Bibliotheksursache mit Quellposition. Der Callback soll deterministisch und frei von sichtbaren Seiteneffekten sein, da Stufen und verworfene Versuche ihn mehrfach aufrufen. Anfangs-, Mindest- und Höchstschritt folgen den Bibliotheksvorgaben; ein numerischer Bericht wird noch nicht ausgegeben.',
    'rk45IntegrateWithSteps': 'Wie rk45Integrate, zusätzlich mit explizitem initialStep, minimumStep und maximumStep. Alle drei Schrittweiten müssen positiv und endlich sein; maximumStep muss mindestens minimumStep betragen. Der Integrator begrenzt den Anfangsschritt auf das erlaubte Intervall und passt spätere Versuche adaptiv an. Vorwärts- und Rückwärtsintegration verwenden positive Schrittweitenbeträge. Die Eingabe bleibt unverändert und das Ergebnis besitzt seinen Array-Speicher selbst. Ungültige Optionen, numerische Fehler und ausgeschöpftes Schrittbudget melden die Bibliotheksursache mit Quellposition. Diese Funktion stellt alle skalaren ps_ode_options-Felder bereit; komponentenweise Toleranzen bietet rk45IntegrateWithTolerances. Ein numerischer Bericht wird noch nicht ausgegeben.',
    'rk45IntegrateWithTolerances': 'Adaptiver Dormand–Prince-5(4)-Integrator mit vollständigen Schrittweitenoptionen und einem absoluten Toleranzwert pro Zustandskomponente. absoluteTolerances muss genau state.count Werte enthalten (1 bis 32); jeder Wert muss positiv und endlich sein. Die Toleranzen werden vor der ersten Ableitungsauswertung kopiert, damit der Integrationslauf einen festen Satz verwendet. relativeTolerance muss endlich und in [0, 1) liegen. Anfangs-, Mindest- und Höchstschritt müssen positiv und endlich sein; maximumStep muss mindestens minimumStep betragen. Der benannte Callback hat (Float64, [Float64]) -> [Float64] und soll keine sichtbaren Seiteneffekte haben. Eingaben bleiben unverändert, das Ergebnis ist ein unabhängiges Array. Ungültige Form, Toleranzen, erschöpftes Schrittbudget und numerische Fehler erzeugen Quelldiagnosen. Ein numerischer Bericht wird noch nicht ausgegeben.',
    'StepInterval': 'Wert mit den positiven endlichen Dauern elapsed und nextStep in Sekunden. Der adaptiveStep-Callback meldet damit die akzeptierte Dauer und den nächsten Schrittvorschlag. Beide Felder sind schreibgeschützt; Arrays, optionale Werte und Funktionswerte kopieren den Wert.',
    'rk45StepReported': 'Wie rk45IntegrateReported, beendet aber nach genau einem akzeptierten Dormand–Prince-Schritt. start und end müssen verschieden sein; reachedTime kann vor end liegen. Verwerfungen verbrauchen das Versuchslimit, Eingaben bleiben erhalten und state ist ein eigener Array-Wert. nextStep enthält den vorzeichenbehafteten nächsten Vorschlag innerhalb der Optionen. Lokale Fehlertoleranzen sind keine globale Fehlergrenze.',
    'rk45IntegrateReported': 'Adaptiver Dormand–Prince-5(4)-Integrator mit denselben Eingaben und Regeln wie rk45IntegrateWithSteps. Das kopierbare OdeResult enthält state als unabhängiges [Float64] sowie acceptedSteps, rejectedSteps, evaluations, reachedTime, nextStep und errorNorm aus dem numerischen Bericht der C-Bibliothek. acceptedSteps und rejectedSteps zählen Schrittversuche; evaluations zählt Ableitungsaufrufe. reachedTime ist die erreichte Zeit, nextStep die vorzeichenbehaftete nächste Schrittweite und errorNorm die letzte skalierte Fehlernorm. Ungültige Optionen, Callbackfehler und erschöpftes Schrittbudget erzeugen Quelldiagnosen.',
    'rk45IntegrateWithTolerancesReported': 'Wie rk45IntegrateWithTolerances mit einem positiven absoluten Toleranzwert pro Zustandskomponente, jedoch mit einem kopierbaren OdeResult statt nur des Zustandsarrays. state besitzt unabhängigen Speicher; acceptedSteps, rejectedSteps, evaluations, reachedTime, nextStep und errorNorm stammen aus demselben C-Integrationslauf. Das Toleranzarray wird vor dem ersten Callback kopiert. Formfehler, ungültige Toleranzen, ausgeschöpftes Schrittbudget und numerische Fehler erzeugen Quelldiagnosen.',
    'verletStep': 'Velocity Verlet mit der gemeinsamen C-Numerik für 1 bis 32 Freiheitsgrade. phase enthält zuerst n endliche Positionen, danach n endliche Geschwindigkeiten; die Länge muss gerade und zwischen 2 und 64 liegen. acceleration ist eine freie, nicht generische Funktion (Float64, [Float64]) -> [Float64], deren Array nur die n Positionen enthält und die n endliche Beschleunigungen liefern muss. Die Beschleunigung darf nicht von der Geschwindigkeit abhängen. dt muss endlich und von null verschieden sein; negative Werte erlauben Rückwärtsschritte. Das Ergebnis ist ein eigenes Array in derselben Reihenfolge, die Eingabe bleibt unverändert. Form-, Zeit- und numerische Fehler erzeugen Quelldiagnosen. Der Callback soll keine sichtbaren Seiteneffekte haben.',
    'log': 'Natürlicher Logarithmus; der Wert muss positiv sein.',
    'log10': 'Zehnerlogarithmus; der Wert muss positiv sein.',
    'pow': 'Potenziert base mit exponent; nicht reelle oder nicht endliche Ergebnisse sind Laufzeitfehler.',
    'hypot': 'Berechnet die euklidische Länge von (x, y) mit skalierter Arithmetik.',
    'sqrt': 'Quadratwurzel; der Wert muss nichtnegativ sein.', 'abs': 'Absolutbetrag.',
    'intAbs': 'Absolutbetrag als Int64. Der kleinste Int64-Wert hat keinen positiven Int64-Gegenwert und erzeugt einen Laufzeitfehler mit Quellposition.',
    'intMin': 'Liefert den kleineren von zwei Int64-Werten ohne Umwandlung in Float64.',
    'intMax': 'Liefert den größeren von zwei Int64-Werten ohne Umwandlung in Float64.',
    'intClamp': 'Begrenzt einen Int64-Wert auf das inklusive Intervall. Eine untere Grenze über der oberen erzeugt einen Laufzeitfehler mit Quellposition.',
    'floor': 'Größte ganze Float64-Zahl, die nicht größer als value ist.',
    'ceil': 'Kleinste ganze Float64-Zahl, die nicht kleiner als value ist.',
    'round': 'Nächste ganze Float64-Zahl; ein exakter Gleichstand wird von null weg gerundet.',
    'Vec2': 'Vektor aus zwei Komponenten.', 'Vec3': 'Vektor aus drei Komponenten.',
    'Vec4': 'Vektor aus vier Komponenten.', 'Quat': 'Quaternion in Komponentenreihenfolge x, y, z, w.',
    'axisAngle': 'Erzeugt eine Rotation aus Achse und Winkel in Radiant.',
    'rotate': 'Rotiert einen dreidimensionalen Vektor.',
    'isClose': 'Prüft die exakte symmetrische absolute/relative Toleranzbedingung für binäre Double-Eingaben; ungültige Werte liefern false.',
    'normalizeQuat': 'Normiert eine gültige, von null verschiedene Quaternion.',
    'conjugateQuat': 'Konjugierte Quaternion; bei Einheitsquaternion die inverse Rotation.',
    'multiplyQuat': 'Komponiert Rotationen; die rechte Rotation wird zuerst angewandt.',
    'slerpQuat': 'Sphärische Rotationsinterpolation mit fraction in [0,1].',
    'springForce': 'Axiale Feder-/Dämpferkraft aus Positionen, Geschwindigkeiten, Federkonstante in N/m, Ruhelänge in m und Dämpfung in N·s/m.',
    'stokesDrag': 'Linearer Stokes-Widerstand für relative Geschwindigkeit in m/s, dynamische Viskosität in Pa·s und Kugelradius in m.',
    'quadraticDrag': 'Quadratischer Kugelwiderstand mit Dichte in kg/m³, Radius in m und dimensionslosem Widerstandsbeiwert.',
    'buoyancyForce': 'Auftrieb entgegen der Schwerkraft aus Dichte in kg/m³, verdrängtem Volumen in m³ und Gravitationsvektor in m/s².',
    'sphereSubmersion': 'Schnitt einer Kugel mit einer ebenen Flüssigkeitsoberfläche: liefert verdrängtes Volumen und Schwerpunktabstand entlang der nach außen gerichteten Oberflächennormale. Radius muss positiv, die Eingaben endlich sein.',
    'symplectic': 'Symplektischer Euler: Vec2 enthält Position und Geschwindigkeit. Erst Geschwindigkeit, dann Position aktualisieren; dt in Sekunden.',
    'Unit': 'Einheit aus sieben SI-Exponenten, positiver Skala und Symbol. Exponentenreihenfolge: Länge, Masse, Zeit, Strom, Temperatur, Stoffmenge, Lichtstärke.',
    'convert': 'Konvertiert einen Zahlenwert von der Empfängereinheit in eine dimensionskompatible Zieleinheit. Bekannte Dimensionskonflikte sind Compilerfehler, dynamische Konflikte Laufzeitfehler.',
    'Quantity': 'Verbindet einen Zahlenwert mit seiner Einheit.',
    'Medium': 'Erzeugt ein homogenes Medium mit nichtnegativer Dichte in kg/m³ und Viskosität in Pa·s.',
    'mediumAir': 'Luft bei 15 °C auf Meereshöhe aus der gemeinsamen C-Bibliothek.',
    'mediumWater': 'Wasser bei 20 °C aus der gemeinsamen C-Bibliothek.',
    'mediumVacuum': 'Vakuum ohne Dichte und Viskosität aus der gemeinsamen C-Bibliothek.',
    'mediumDragForce': 'Quadratische Widerstandskraft entgegen der Relativgeschwindigkeit; Koeffizient und Fläche müssen nichtnegativ sein.',
    'mediumStokesDrag': 'Stokes-Widerstand einer Kugel mit der Viskosität dieses Mediums; der Radius muss positiv sein.',
    'Material': 'Erzeugt ein Material mit Dichte in kg/m³, Rückprallwert in [0, 1] und nichtnegativer Reibung.',
    'materialContactSolver': 'Erzeugt geprüfte Kontakt-Solver-Einstellungen mit Rückprall und Reibung dieses Materials.',
    'convertQuantity': 'Konvertiert eine Größe in eine dimensionskompatible Zieleinheit. Bekannte Dimensionskonflikte werden beim Kompilieren erkannt.',
    'addQuantity': 'Addiert Größen in der Einheit des linken Operanden mit normierter, kompensierter Umrechnung vor der Rückskalierung. Bekannte Dimensionskonflikte werden beim Kompilieren erkannt; Laufzeitfehler sind mit attempt abfangbar.',
    'subtractQuantity': 'Subtrahiert Größen in der Einheit des linken Operanden mit normierter, kompensierter Umrechnung. Bekannte Dimensionskonflikte werden beim Kompilieren erkannt; Fehler erhalten die Werte und sind mit attempt abfangbar.',
    'multiplyQuantity': 'Multipliziert Größen und kombiniert ihre Dimensionen; symbol benennt die neue Einheit.',
    'divideQuantity': 'Dividiert Größen und kombiniert ihre Dimensionen; der Divisor darf nicht null sein.',
    'multiplyUnit': 'Multipliziert Dimensionen und Skalen zweier Einheiten.',
    'divideUnit': 'Dividiert Dimensionen und Skalen zweier Einheiten.',
    'powerUnit': 'Erhebt eine Einheit in eine ganzzahlige Potenz.',
    'compatibleUnit': 'Prüft gleiche SI-Dimensionen unabhängig von Skala und Symbol.',
    'Channel': 'Deklariert einen eindeutigen Messkanal mit kanonischer SI-Skala 1 und begrenzten UTF-8-Metadaten. Werte vorher ausdrücklich nach SI umrechnen. Fehler sind mit attempt abfangbar und verbrauchen keinen Slot.',
    'sample': 'Setzt den aktuellen Messwert des Kanals in seiner deklarierten Einheit. Auch den Anfangswert in reset setzen.',
    'logDebug': 'Schreibt eine Debug-Meldung mit aktueller Simulationszeit. Nur im Experiment; Bool meldet Annahme durch den Hostlogger.',
    'logInfo': 'Schreibt eine Info-Meldung mit aktueller Simulationszeit. Nur im Experiment; false bei ungültigem Text, ausgeschöpftem Budget oder I/O-Fehler.',
    'logWarning': 'Schreibt eine Warnung mit aktueller Simulationszeit; verändert weder Modellzustand noch Messwerte.',
    'logError': 'Schreibt eine Fehlermeldung; der Schweregrad beendet die Simulation nicht. Prüfe Bool bei Bedarf.',
    'metadata': 'Setzt beschreibenden UTF-8-Modelltext, etwa Parameter und Methode.',
    'simulationTime': 'Aktuelle Hostzeit in Sekunden; beim Eintritt in step die Zeit vor dem Schritt.',
    'runSeed': 'Liefert das vollständige 64-Bit-Bitmuster des aktuellen Laufseeds als Int64. Auch Seedwerte oberhalb von INT64_MAX bleiben beim Zurückwandeln in einen Zufallsstrom erhalten; nur im Experiment.',
    'parameterWithUnit': 'Wie parameter, mit einer eigenen Anzeigeeinheit aus Symbol, positiver Skala und sieben SI-Dimensionen. Standard, Grenzen, Override und Rückgabewert bleiben SI-Zahlen. Die GUI konvertiert Eingaben, Studienberichte skalieren ihre X-Achse; Rohdaten und CSV bewahren SI. Das Symbol muss gültiges UTF-8 ohne Steuerzeichen mit höchstens 15 Bytes sein.',
    'parameter': 'Definiert beim Erzeugen des Experiments einen benannten Float64-Parameter mit Standardwert, inklusiven Grenzen und Beschreibung. Liefert den wirksamen Wert nach dem Runner-Override. Nur während globaler Initialisierung oder create; eindeutiger Name, endliche Werte und höchstens 16 Parameter.',
    'randomUniform': 'Zieht aus dem reproduzierbaren Laufzufallsstrom zwischen min und max.',
    'randomNormal': 'Zieht normalverteilte Werte aus dem Laufzufallsstrom mit explizitem Mittelwert und Streuung.',
    'inputCount': 'Anzahl der für diese Analyse ausgewählten Eingabeläufe.',
    'Dataset': 'Öffnet den Eingabelauf am nullbasierten Index. Ein Analyselauf hat maximal acht Eingaben.',
    'datasetSampleCount': 'Anzahl der erfolgreich gelesenen Datensätze. Bei wiederhergestellten Läufen kann dies kleiner als ursprünglich geplant sein.',
    'datasetChannelCount': 'Anzahl der Messkanäle ohne die Zeitachse. Das Dataset-Handle muss geöffnet sein.',
    'datasetChannelName': 'Kopiert den exakten Namen eines nullbasierten Messkanals als UTF-8-String. Ungültige Indizes erzeugen einen Quellfehler.',
    'datasetChannelUnitSymbol': 'Kopiert das gespeicherte Einheitensymbol eines nullbasierten Messkanals als UTF-8-String.',
    'datasetChannelDescription': 'Kopiert die gespeicherte Beschreibung eines nullbasierten Messkanals als UTF-8-String.',
    'datasetChannelExponent': 'Liest den SI-Exponenten des Messkanals für Achse 0 bis 6 als Int64. Ungültige Kanal- oder Achsenindizes sind Fehler.',
    'datasetRecovered': 'Wahr, wenn nur ein gültiger Teil einer unvollständigen Laufdatei gelesen wurde.',
    'datasetMetadata': 'Kopiert die gespeicherten Laufmetadaten als UTF-8-String. Die Kopie bleibt nach dem Schließen des Datasets gültig.',
    'series': 'Holt einen Kanal über seinen exakten Namen. Der reservierte Name time liefert die Zeitachse.',
    'seriesCount': 'Anzahl der Samples der Reihe.',
    'seriesName': 'Kopiert den Reihennamen als eigenen UTF-8-String. Die Kopie bleibt nach Freigabe der Reihe gültig.',
    'seriesUnitSymbol': 'Kopiert das gespeicherte oder bei abgeleiteten Reihen formatierte Einheitensymbol als eigenen UTF-8-String.',
    'seriesUnitScale': 'Liefert den Maßstab der gespeicherten Werte zur SI-Einheit als Float64.',
    'seriesExponent': 'Liest den SI-Exponenten der Reihe für Achse 0 bis 6 als Int64; ungültige Achsen sind Fehler.',
    'seriesAligned': 'Prüft zwei gültige Reihen auf dasselbe Dataset, dieselbe Auswahl und denselben Samplebereich. Verschiedene Zuordnungen liefern false; ungültige Handles sind Fehler.',
    'sliceSeries': 'Kopiert count Werte ab dem nullbasierten Index first. Passende Ausschnitte derselben Auswahl bleiben zugeordnet; negative oder zu große Bereiche sind Fehler.',
    'seriesValue': 'Liest einen gültigen Zahlenwert am nullbasierten Index. Eine maskierte fehlende Beobachtung erzeugt einen Quellfehler.',
    'seriesValues': 'Liest count gültige Werte ab first als unabhängiges Float64-Array. Fehlende Beobachtungen, negative oder zu große Bereiche sind Fehler; ein leerer Ausschnitt am Ende ist erlaubt.',
    'mean': 'Mittelwert über alle Werte der Reihe.', 'stddev': 'Stichprobenstandardabweichung; benötigt mindestens zwei Werte.',
    'quantile': 'Typ-7-Quantil einer nichtleeren Reihe. Die endliche Wahrscheinlichkeit muss in [0, 1] liegen; die Werte werden mit budgetiertem temporärem Speicher sortiert.',
    'minimum': 'Liefert den kleinsten endlichen Wert einer nichtleeren Reihe. Liest blockweise und benötigt keine darstellbare Varianz.',
    'maximum': 'Liefert den größten endlichen Wert einer nichtleeren Reihe. Liest blockweise und benötigt keine darstellbare Varianz.',
    'derivative': 'Sekantenableitung dy/dx; x muss streng steigen. Die Einheit wird abgeleitet.',
    'integral': 'Kumulative Trapezintegration von y nach x mit Anfangswert und dessen Einheit.',
    'affine': 'Berechnet factor * input + offset. factor ist dimensionslos, unit beschreibt den Offset.',
    'addSeries': 'Addiert gepaarte Reihen mit kompatiblen Einheiten.',
    'subtractSeries': 'Subtrahiert gepaarte Reihen mit kompatiblen Einheiten.',
    'multiplySeries': 'Multipliziert gepaarte Reihen und ihre Einheiten.',
    'divideSeries': 'Dividiert gepaarte Reihen; Nulldivision ist ein Fehler.',
    'resampleLinear': 'Interpoliert y(x) linear auf targetX ohne Extrapolation.',
    'resamplePchip': 'Interpoliert y(x) mit monotoner kubischer Hermite-Interpolation (PCHIP) auf targetX ohne Extrapolation.',
    'resampleNearest': 'Wählt den nächsten Stützpunkt auf targetX; bei gleichem Abstand den früheren.',
    'resamplePrevious': 'Verwendet den letzten Stützpunkt vor oder an targetX, etwa für stückweise konstante Signale.',
    'movingAverage': 'Kausaler Mittelwert über höchstens window Werte; window liegt zwischen 1 und 4096.',
    'release': 'Gibt ein Reihenhandle frei. Weitere Verwendung dieses Handles ist ein Fehler.',
    'closeDataset': 'Schließt einen Datensatz und invalidiert seine Quell- und Ergebnisreihen.',
    'report': 'Setzt den Titel des Analyseberichts.',
    'plot': 'Erstellt ein Liniendiagramm aus ausgerichteten x/y-Reihen. Der Empfänger der Methode ist y.',
    'curve': 'Fügt dem Plot eine Linienkurve aus passenden Reihen hinzu.',
    'points': 'Fügt dem Plot Messpunkte aus passenden Reihen hinzu.',
    'histogram': 'Zählt alle Reihenwerte in bis zu 128 gleich breiten Klassen.',
    'exportSeries': 'Exportiert alle Werte eines x/y-Paares als CSV. Der Empfänger ist y; suffix ergänzt den Ausgabepräfix.',
    'exportColumns': 'Exportiert 1 bis 32 zugeordnete Reihen gemeinsam als CSV. Reihenfolge, Namen und Einheiten erscheinen als Spalten; leere und nicht zugeordnete Listen sind Fehler.',
    'exportPlot': 'Exportiert das gespeicherte Diagramm als SVG mit dem angegebenen Suffix.',
}
LANG_DESCRIPTIONS.update({'RunIndex': 'Öffnet und validiert eine Laufdatei mit positivem Checkpointlimit im '
             '64-MiB-Sprachbudget. Alte und unvollständige Dateien werden rekonstruiert. Kopien '
             'teilen einen automatisch freigegebenen Dateibesitzer; Abfragen verwenden '
             'nullbasierte Indizes.',
 'runIndexClose': 'Gibt die Referenz dieses veränderbaren Werts frei und schließt ihn. Andere '
                  'Kopien bleiben verwendbar; die Datei schließt nach der letzten Referenz. '
                  'Wiederholtes Schließen ist erlaubt.',
 'inputPath': 'Liefert einen eigenen UTF-8-String mit dem ausgewählten Eingabepfad. Nullbasierter '
              'Index muss kleiner als inputCount sein. Nur in Analysemodulen verfügbar.',
 'runIndexRead': 'Liest einen unabhängigen besitzenden RunBlock mit 0–256 Zeilen ab first. '
                 'Ungültige Grenzen oder CRC lösen typisierte Fehler aus; attempt liefert dann '
                 'nil.',
 'runIndexSnapshot': 'Liest und validiert einen vollständigen kopierbaren RunSnapshot anhand '
                     'seiner nullbasierten Szenennummer. Werte, Szene, Eltern und TRS bleiben '
                     'erhalten.',
 'runIndexDimension': 'Liest den SI-Dimensionsexponenten am Kanal und Achsenindex 0–6: Länge, '
                      'Masse, Zeit, Strom, Temperatur, Stoffmenge, Lichtstärke.',
 'runIndex_is_open': 'Liefert den Öffnungszustand ohne Fehler auch nach close. Kanalindizes sind '
                     'nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_samples': 'Liefert die Zahl validierter Messzeilen. Kanalindizes sind nullbasiert; '
                     'geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_snapshots': 'Liefert die Zahl validierter Szenen. Kanalindizes sind nullbasiert; '
                       'geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_checkpoints': 'Liefert die gemeinsame Zahl aller Checkpoints. Kanalindizes sind '
                         'nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_channels': 'Liefert die Kanalzahl. Kanalindizes sind nullbasiert; geschlossene Handles '
                      'werfen einen Fehler, außer isOpen.',
 'runIndex_complete': 'Liefert ob ein gültiger Footer vorliegt. Kanalindizes sind nullbasiert; '
                      'geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_persisted': 'Liefert ob der gespeicherte Index dem geprüften Präfix entspricht. '
                       'Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, '
                       'außer isOpen.',
 'runIndex_metadata': 'Liefert einen eigenen Metadatenstring. Kanalindizes sind nullbasiert; '
                      'geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_name': 'Liefert den Kanalnamen als eigenen String. Kanalindizes sind nullbasiert; '
                  'geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_symbol': 'Liefert das Kanalsymbol als eigenen String. Kanalindizes sind nullbasiert; '
                    'geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runIndex_description': 'Liefert die Kanalbeschreibung als eigenen String. Kanalindizes sind '
                         'nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.',
 'runBlock_count': 'Liefert Zeilenzahl. Der Block bleibt nach Schließen des Index verwendbar; '
                   'ungültige Indizes werfen Fehler.',
 'runBlock_channels': 'Liefert Kanalzahl. Der Block bleibt nach Schließen des Index verwendbar; '
                      'ungültige Indizes werfen Fehler.',
 'runBlock_time': 'Liefert Zeit an der nullbasierten Zeile. Der Block bleibt nach Schließen des '
                  'Index verwendbar; ungültige Indizes werfen Fehler.',
 'runBlock_value': 'Liefert Messwert an nullbasierter Zeile und Kanal. Der Block bleibt nach '
                   'Schließen des Index verwendbar; ungültige Indizes werfen Fehler.',
 'runBlock_times': 'Liefert unabhängiges Float64-Array aller Zeiten. Der Block bleibt nach '
                   'Schließen des Index verwendbar; ungültige Indizes werfen Fehler.',
 'runBlock_column': 'Liefert unabhängiges Float64-Array eines nullbasierten Kanals. Der Block '
                    'bleibt nach Schließen des Index verwendbar; ungültige Indizes werfen Fehler.',
 'runSnapshot_time': 'Liefert Zeit aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                     'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                     'IDs.',
 'runSnapshot_paused': 'Liefert Pausestatus aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                       'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                       'IDs.',
 'runSnapshot_channels': 'Liefert Kanalzahl aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                         'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind '
                         'keine IDs.',
 'runSnapshot_objects': 'Liefert Objektzahl aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                        'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind '
                        'keine IDs.',
 'runSnapshot_points': 'Liefert Punktzahl aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                       'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                       'IDs.',
 'runSnapshot_value': 'Liefert Messwert am Kanal aus dem vollständigen Snapshot. Kanal-, Objekt- '
                      'und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind '
                      'keine IDs.',
 'runSnapshot_id': 'Liefert stabile Objekt-ID aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                   'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                   'IDs.',
 'runSnapshot_parent': 'Liefert Eltern-ID aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                       'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                       'IDs.',
 'runSnapshot_shape': 'Liefert Formnummer aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                      'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                      'IDs.',
 'runSnapshot_color': 'Liefert RGBA-Farbwert aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                      'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                      'IDs.',
 'runSnapshot_position': 'Liefert lokales a.xyz (Frame-Translation) aus dem vollständigen '
                         'Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden '
                         'geprüft; Objektindizes sind keine IDs.',
 'runSnapshot_size': 'Liefert lokales b.xyz (Boxausdehnung oder Frame-Skalierung) aus dem '
                     'vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert '
                     'und werden geprüft; Objektindizes sind keine IDs.',
 'runSnapshot_radius': 'Liefert Radius aus dem vollständigen Snapshot. Kanal-, Objekt- und '
                       'Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine '
                       'IDs.',
 'runSnapshot_rotation': 'Liefert Quaternionorientierung aus dem vollständigen Snapshot. Kanal-, '
                         'Objekt- und Punktindizes sind nullbasiert und werden geprüft; '
                         'Objektindizes sind keine IDs.',
 'runSnapshot_point_first': 'Liefert Anfang des Polyline-Bereichs aus dem vollständigen Snapshot. '
                            'Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; '
                            'Objektindizes sind keine IDs.',
 'runSnapshot_point_count': 'Liefert Länge des Polyline-Bereichs aus dem vollständigen Snapshot. '
                            'Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; '
                            'Objektindizes sind keine IDs.',
 'runSnapshot_text': 'Liefert eigenen Beschriftungsstring aus dem vollständigen Snapshot. Kanal-, '
                     'Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes '
                     'sind keine IDs.',
 'runSnapshot_point': 'Liefert lokalen Punkt im gemeinsamen Punktpool aus dem vollständigen '
                      'Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden '
                      'geprüft; Objektindizes sind keine IDs.',
 'runSnapshot_world_point': 'Liefert den Weltpunkt aus lokalem Punkt und Objektslot aus dem '
                            'vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind '
                            'nullbasiert und werden geprüft; Objektindizes sind keine IDs.',
 'runSnapshot_transform': 'Liefert die lokale-zu-Welt-Matrix eines Objektslots aus dem '
                          'vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind '
                          'nullbasiert und werden geprüft; Objektindizes sind keine IDs.'})

for n in (2, 3, 4):
    LANG_DESCRIPTIONS[f'dot{n}'] = 'Skalarprodukt zweier Vektoren gleicher Dimension.'
    LANG_DESCRIPTIONS[f'length{n}'] = 'Euklidische Länge des Vektors.'
    LANG_DESCRIPTIONS[f'normalize{n}'] = 'Normiert den Vektor; der Nullvektor bleibt null.'
LANG_DESCRIPTIONS['cross2'] = 'Vorzeichenbehaftete Fläche beziehungsweise Z-Komponente des zweidimensionalen Kreuzprodukts.'
LANG_DESCRIPTIONS['cross3'] = 'Rechtshändiges Kreuzprodukt zweier dreidimensionaler Vektoren.'
for shape in ('sphere', 'line', 'arrow', 'point', 'polyline', 'box', 'plane', 'label'):
    LANG_DESCRIPTIONS[shape] = {
        'sphere': 'Kugel mit Mittelpunkt und Radius in Metern.',
        'line': 'Linie von start nach end mit räumlichem Linienradius.',
        'arrow': 'Pfeil von start zur Spitze end mit Schaftradius.',
        'point': 'Punktmarker an position.',
        'polyline': 'Linienzug aus mindestens zwei Weltpunkten; Szene insgesamt maximal 96 Punkte.',
        'box': 'Box mit Mittelpunkt, vollen XYZ-Ausdehnungen in Metern und Quaternionrotation.',
        'plane': 'Ebene mit Mittelpunkt, vollen X/Z-Seitenlängen und Quaternionrotation.',
        'label': 'UTF-8-Beschriftung an einem Weltpunkt, höchstens 63 Bytes.',
    }[shape] + ' Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.'


LANG_DESCRIPTIONS.update({
    'ContactWorld': 'Erzeugt einen besitzenden, unveränderlichen Kontaktzustand mit expliziten Zuordnungs- und Warmstartgrenzen; alle vier Einstellungen folgen dem C-Vertrag.',
    'defaultContactWorld': 'Erzeugt einen leeren Kontaktzustand mit den C-Standardwerten; keine implizite Integration.',
    'worldReset': 'Liefert einen neuen leeren Snapshot mit denselben Einstellungen; der ursprüngliche Zustand und seine Kopien bleiben erhalten.',
    'worldSolve': 'Erzeugt diskrete Kugel-/Box-/Ebenenkontakte und löst den warmen C-Graphen. Liefert einen neuen Snapshot mit Körpern, Kontaktverlauf und Resten; Eingaben bleiben erhalten.',
    'sphereCollider': 'Erzeugt einen Kugelcollider mit stabiler nichtnull u32-ID, Körperindex 0..127 und positivem Radius in Metern.',
    'boxCollider': 'Erzeugt einen Boxcollider mit stabiler ID, Körperindex und positiven vollständigen Ausmaßen in Metern.',
    'planeCollider': 'Erzeugt eine Ebene mit stabiler ID, Körperindex und lokaler Einheitsnormale in den freien Halbraum; der zugeordnete Körper muss statisch sein.',
    'worldBodies': 'Liefert einen unabhängigen Arraywert der gelösten Körper; Änderungen daran ändern den Kontaktzustand nicht.',
    'solveWarmContacts': 'Löst einen manuellen Kontaktgraphen mit genau einem endlichen Startimpuls auf A je Kontakt. Der aktuelle Reibungskegel begrenzt Seeds; Restitution verwendet die Geschwindigkeiten vor allen Seeds. Keine Gelenke.',
})
for name,description in {
    'id':'Stabile Collider-ID in 1..4294967295.', 'body':'Nullbasierter Körperindex.', 'shape':'Formnummer: Kugel=1, Box=2, Ebene=3.',
    'size':'Kugelradius in x beziehungsweise volle Boxausmaße, Meter; Ebene null.', 'normal':'Lokale Ebenen-Einheitsnormale; andere Formen null.'
}.items(): LANG_DESCRIPTIONS['collider_'+name]=description
for name,description in {
    'bodyCount':'Anzahl gespeicherter Körper.', 'colliderCount':'Anzahl gespeicherter Collider.', 'contactCount':'Anzahl erzeugter Kontaktpunkte.',
    'matched':'Eins zu eins zugeordnete Kontakte.', 'created':'Neu erzeugte Kontakte ohne Zuordnung.', 'ended':'Seit dem vorigen Schritt ausgelaufene Kontakte.',
    'warmed':'Zugeordnete Kontakte mit nichtnull Warmseed vor Kegelprojektion.', 'maxNormalError':'Größter Normalgeschwindigkeitsrest in m/s vor Positionsprojektion.',
    'maxProjectionError':'Nicht erfüllte Positionskorrektur in Metern.', 'dt':'Letzte erfolgreiche Schrittweite in Sekunden, leer null.',
    'body':'Körperwert am geprüften nullbasierten Index.', 'contactIdA':'Stabile ID von Kontaktpartner A.', 'contactIdB':'Stabile ID von Kontaktpartner B; größer als A.',
    'contactPoint':'Kontaktpunkt in Weltmetern vor Projektion.', 'contactNormal':'Einheitsnormale von A nach B.', 'contactPenetration':'Eindringtiefe in Metern vor Projektion.',
    'localAnchorA':'Lokaler Körperanker auf A vor Projektion.', 'localAnchorB':'Lokaler Körperanker auf B vor Projektion.',
    'contactImpulse':'Gesamter Impuls auf A einschließlich Warmstart in N s.'
}.items(): LANG_DESCRIPTIONS['world_'+name]=description+' Der Snapshot bleibt unverändert.'

LANG_DESCRIPTIONS.update({'Batch': 'Erzeugt eine besitzende unveränderliche Serienkonfiguration. Absolute '
          'Modul-/Ausgabepfade, 1..1000 Läufe, höchstens acht Worker und fünf Millionen Samples; '
          'Int64-Seeds verwenden ihre 64-Bit-Bitfolge.',
 'batchAdaptive': 'Aktiviert adaptive Integration mit Minimum und Maximum in Sekunden; eine '
                  'Zielzeit muss vorher gesetzt sein.',
 'batchLimits': 'Setzt das Zeitlimit je Runner (höchstens 3600 Sekunden) und die Speichergrenze in '
                'MiB (0 deaktiviert, höchstens 16384).',
 'batchParameter': 'Setzt oder ersetzt einen festen SI-Parameter in einer neuen Konfiguration; der '
                   'ursprüngliche Batch bleibt unverändert.',
 'batchRequireSuccess': 'Fordert einen ausgeführten, vollständig abgeschlossenen Batch ohne Fehler '
                        'oder Abbruch. Andernfalls entsteht eine abfangbare Quelldiagnose mit dem '
                        'gespeicherten Ergebniscode.',
 'batchResume': 'Lädt eine unveränderte archivierte Konfiguration für einen neuen Ausgabeordner; '
                'Fingerprints und Checkpoint-Version werden geprüft. run() übernimmt geprüfte '
                'frühere Läufe.',
 'batchRun': 'Startet eine neue archivierte Serie über den expliziten Analysehost. Liefert einen '
             'neuen Ergebnis-Snapshot; code/error/finished/status bleiben auch bei Laufzeitfehlern '
             'abfragbar.',
 'batchRunUntil': 'Pausiert nach der angegebenen Zahl validierter Abschlüsse. 0 oder runs führt '
                  'die ganze Serie aus; ein früherer Stopp erzeugt ein wiederaufnehmbares Journal '
                  'ohne Gesamtbericht.',
 'batchSeries': 'Erzeugt eine SI-Datenreihe ausschließlich aus gültigen Endwerten in '
                'Laufindex-Reihenfolge. Null gültige Endpunkte werden abgewiesen; Statistik, '
                'Quantile, Histogramme und Exporte verwenden die bestehenden Series-Bindungen.',
 'batchSource': 'Archiviert die angegebene Quelldatei beim Start zusammen mit dem verwendeten '
                'Modul. Der Pfad wird kopiert.',
 'batchSweep': 'Konfiguriert eine lineare Parameterstudie mit mindestens zwei Läufen und endlichen '
               'verschiedenen Grenzen in SI; Konflikte mit festen Parametern werden abgewiesen.',
 'batchTarget': 'Setzt eine gemeinsame positive Endzeit in Sekunden; steps bleibt das akzeptierte '
                'Schrittbudget.',
 'batch_cancelled': 'Ob die Serie kontrolliert unterbrochen wurde.',
 'batch_code': 'Gespeicherter ps_result des Controllers; vor run() null, daher auch executed() '
               'prüfen.',
 'batch_completed': 'Zahl validierter und journalierter Läufe einschließlich fehlender Endwerte.',
 'batch_directory': 'Kopierter Ausgabeordner der Konfiguration.',
 'batch_dt': 'Fester beziehungsweise anfänglicher Zeitschritt in Sekunden.',
 'batch_endTime': 'Gemeinsame Zielzeit; 0 bedeutet feste Schrittanzahl.',
 'batch_error': 'Kopierte begrenzte Fehlermeldung des Controllers.',
 'batch_executed': 'Ob dieser Snapshot ein Ausführungsergebnis besitzt.',
 'batch_finished': 'Ob der Lauf validiert und journaliert ist, einschließlich Status 2.',
 'batch_module': 'Kopierter Pfad des Experimentmoduls.',
 'batch_peakActive': 'Höchste beobachtete gleichzeitige Workerzahl.',
 'batch_reused': 'Zahl aus dem alten Archiv übernommener Läufe.',
 'batch_runPath': 'Kopierter Archivpfad am nullbasierten Laufindex; das liefert auch für noch '
                  'nicht fertige Läufe nur einen Pfad.',
 'batch_runs': 'Anzahl konfigurierter Läufe.',
 'batch_seed': 'Ursprüngliche 64-Bit-Seedfolge als Int64-Bitfolge.',
 'batch_started': 'Zahl neu gestarteter Worker.',
 'batch_status': 'Messstatus am nullbasierten Laufindex.',
 'batch_statuses': 'Besitzendes Array aller Laufstatus: 0 nicht fällig, 1 gültig, 2 verworfen.',
 'batch_steps': 'Schritte pro Lauf beziehungsweise akzeptiertes Schrittbudget.',
 'batch_unit': 'Kanonische SI-Einheit des verifizierten Kanals. Das Symbol bleibt auch nach '
               'Freigabe der Batch-Kopie für die Modul-Lebensdauer gültig.',
 'batch_valid': 'Zahl gültiger Endwerte.',
 'batch_value': 'Gültiger Endwert am nullbasierten Laufindex. Nicht vorhandene oder verworfene '
                'Werte werden abgewiesen.',
 'batch_values': 'Besitzendes Array gültiger Endwerte in Laufindex-Reihenfolge; fehlende Werte '
                 'werden nicht ergänzt.',
 'batch_workers': 'Konfigurierter Parallelitätsgrad.',
 'outputPrefix': 'Kopiert das Ausgabeprefix des laufenden Analysehosts in einen '
                 'besitzenden String; etwa als Grundlage für einen neuen Serienordner.'})

def language_reference():
    source = (ROOT / 'src/language/builtins.c').read_text(encoding='utf-8')
    table = source.split('library[] = {', 1)[1].split('static const ps_lang_builtin *library_find', 1)[0]
    pattern = re.compile(r'\{\s*"(\w+)"\s*,\s*"\w+"\s*,\s*(\w+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,\s*\{([^}]+)\}\s*,\s*\{([^}]+)\}\s*\}', re.S)
    types = dict(BATCH='Batch', B='Bool', F='Float64', I='Int64', S='String', U='Unit', QUANTITY='Quantity', MEDIUM='Medium', MATERIAL='Material', SUBMERSION='Submersion', C='Channel',
                 V2='Vec2', V3='Vec3', V4='Vec4', Q='Quat', M3='Mat3', M4='Mat4', B3='Bezier3', RNG='Rng', ODE_RESULT='OdeResult', STEP_INTERVAL='StepInterval', SCALAR_RESULT='ScalarResult', DIAGNOSTIC='Diagnostic', RUN_INDEX='RunIndex', RUN_BLOCK='RunBlock', RUN_SNAPSHOT='RunSnapshot', COLLIDER='Collider', WORLD='ContactWorld', PS_LANG_COLLIDER_ARRAY='[Collider]', VOID='Void', D='Dataset', R='Series',
                 P='Plot', TABLE='Table', DIST='Distribution', CONFIG='SensorConfig', SENSOR='Sensor',
                 BODY='Body', CONTACTS='Contacts', SOLVER='ContactSolver', RESULT='ContactResult',
                 JOINT='DistanceJoint', JOINT_RESULT='JointResult',
                 CONTACT_CONSTRAINT='ContactConstraint', JOINT_CONSTRAINT='JointConstraint', GRAPH_RESULT='ConstraintResult',
                 SWEEP='Sweep', AABB='Aabb', PS_LANG_AABB_ARRAY='[Aabb]', PS_LANG_PAIR_ARRAY='[CollisionPair]',
                 PS_LANG_BODY_ARRAY='[Body]', PS_LANG_CONTACT_CONSTRAINT_ARRAY='[ContactConstraint]',
                 PS_LANG_JOINT_CONSTRAINT_ARRAY='[JointConstraint]',
                 SAMPLE='Measurement', PS_TYPE_BOOL='Bool', PS_TYPE_FUNCTION='func(Float64) -> Float64',
                 PS_LANG_ODE_CALLBACK='func(Float64, [Float64]) -> [Float64]',
                 PS_LANG_VEC3_ARRAY='[Vec3]', PS_LANG_FLOAT_ARRAY='[Float64]', PS_LANG_INT_ARRAY='[Int64]',
                 PS_LANG_SERIES_ARRAY='[Series]', PS_LANG_STRING_ARRAY='[String]',
                 PS_LANG_UNIT_ARRAY='[Unit]', PS_LANG_QUANTITY_ARRAY='[Quantity]')
    methods_text = source.split('methods[] =', 1)[1].split('const ps_lang_method *ps_lang_method_find', 1)[0]
    methods = re.findall(r'\{"(\w+)", "(\w+)", (\w+), ([^}]+)\}', methods_text)
    compact = re.findall(r'\{\s*"(\w+)"\s*,\s*"(\w+)"\s*,\s*(\w+)\s*,\s*([^}]+)\}', methods_text)
    methods += [m for m in compact if m not in methods and (m[2] in ('WORLD','COLLIDER','BATCH') or m[1]=='solveWarmContacts')]
    factory_text = source.split('factories[] =', 1)[1].split('const ps_lang_builtin *ps_lang_builtin_find', 1)[0]
    factories = re.findall(r'\{"(\w+)", "(\w+)", "(\w+)"\}', factory_text)
    result = ['# Physim-Sprache: Bibliotheksreferenz',
              'Alle hier aufgeführten Aufrufe sind im Compiler registriert. Die Signaturen zeigen die tatsächlich erlaubte Schreibweise: Empfängermethoden werden auf einem Wert aufgerufen, statische Fabriken auf dem Typ. `Void` bedeutet kein Rückgabewert. Parameter können positional oder mit den gezeigten Namen angegeben werden. Zahlen und Methodenempfänger werden statisch geprüft.',
              '[Sprachanleitung und Beispiele](../language.md) · [Arrays und Wertsemantik](../language-values.md) · [Teil II – Physim](../physim-guide.md)',
              '## Aufrufbeispiel',
              '```text\nlet direction = Vec3(3, 4, 0)\nlet distance = direction.length()\nlet unitDirection = direction.normalized()\nlet metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")\nlet position = Quantity(2, metres)\n```',
              'Erwartung: distance ist 5, unitDirection ist (0.6, 0.8, 0). Für Experimente stehen globale Channel-Deklarationen sowie create/reset/step/scene bereit; Analysen implementieren analyze. Hostgebundene Funktionen sind nicht in eigenständigen Programmen verfügbar. Ungültige Argumente werden je nach Fall beim Kompilieren oder mit einer Quelldiagnose zur Laufzeit abgewiesen. Die C-Bibliothek ist umfangreicher als die derzeitigen Sprachbindungen.',
              '## Werte und Grenzen',
              'Vektoren haben die Komponenten x/y/z/w, Quantity die Felder value und unit. Measurement enthält value (Quantity), standardUncertainty (Float64 in der Sensoreinheit), time (Float64 in Sekunden), state, index und skipped (Int64). Nur isValid() bestätigt einen verwendbaren Messwert. In der Szene gelten Meter, Y nach oben, höchstens 32 Objekte und insgesamt 96 Polyline-Punkte. Analysehandles leben im Analysecontext; nach release/close dürfen sie nicht erneut verwendet werden. Plotvorschauen sind auf 2048 Punkte pro Kurve begrenzt; export der Reihe erhält alle Werte. Arrayoperationen, eigene Methoden, Schleifen und Konvertierungen erklärt die Wertsemantik-Anleitung.']
    entries = []
    for name, ret, count, host, args, labels in pattern.findall(table):
        count = int(count)
        params = list(zip(re.findall(r'"([^"]+)"', labels), args.replace(' ', '').split(',')))[:count]
        aliases = []
        for method, fn, owner, receiver in methods:
            if fn == name:
                slot = 0 if 'MUTATING' in receiver else int(receiver.strip())
                aliases.append((types[owner] + '.' + method, [p for i, p in enumerate(params) if i != slot]))
        for owner, method, fn in factories:
            if fn == name: aliases.append((owner + '.' + method, params))
        if not aliases: aliases = [(name, params)]
        assert name in LANG_DESCRIPTIONS, f'Missing teaching description: {name}'
        for display, parameters in aliases:
            signature = display + '(' + ', '.join(label + ': ' + types[t] for label, t in parameters) + ')'
            context = ['Überall verfügbar.', 'Experimentmodul erforderlich.', 'Analysemodul erforderlich.'][int(host)]
            entries.append((display, f'## {display}\n\n```text\n{signature} -> {types[ret]}\n```\n\n{LANG_DESCRIPTIONS[name]}\n\n{context}'))
    for numeric in ('Int64', 'Float64'):
        display = numeric + '.parse'
        entries.append((display,
                        f'## {display}\n\n```text\n{display}(text: String) -> {numeric}?\n```\n\n'
                        'Liest ASCII-Dezimaltext ohne Leerraum. Ungültige Schreibweisen und '
                        'Werte außerhalb des Zielbereichs liefern `nil`; ein gültiger Wert '
                        'kann mit `guard let` gebunden werden. Das Label `text:` ist optional. '
                        'Die Regeln für Vorzeichen, Dezimalpunkt und Exponent entsprechen '
                        'der expliziten Wertkonvertierung.\n\nÜberall verfügbar.'))
    entries.append(('Int64.isMultiple(of:)',
                    '## Int64.isMultiple(of:)\n\n```text\n'
                    'value.isMultiple(of: divisor) -> Bool  // value, divisor: Int64\n'
                    '```\n\nPrüft ganzzahlig, ob value ein Vielfaches von divisor ist. '
                    'Bei divisor 0 ist nur value 0 ein Vielfaches. Auch der kleinste '
                    'Int64-Wert ist ein Vielfaches von -1, ohne Überlauf.\n\n'
                    'Überall verfügbar.'))
    entries.append(('Int64.signum()',
                    '## Int64.signum()\n\n```text\n'
                    'value.signum() -> Int64  // value: Int64\n'
                    '```\n\nLiefert -1, 0 oder 1 entsprechend dem Vorzeichen. '
                    'Auch der kleinste Int64-Wert wird ohne Überlauf verarbeitet.\n\n'
                    'Überall verfügbar.'))
    entries.append(('String.split(separator:maxSplits:omittingEmptySubsequences:)',
                    '## String.split(separator:maxSplits:omittingEmptySubsequences:)\n\n'
                    '```text\ntext.split(separator: part, maxSplits: limit, '
                    'omittingEmptySubsequences: omit) -> [String]\n'
                    '```\n\n`part: String` ist erforderlich. `limit: Int64` ist optional '
                    'und standardmäßig unbegrenzt; ein negativer Wert ist ein '
                    'Laufzeitfehler. `omit: Bool` ist optional und standardmäßig '
                    '`true`. Auch ohne `maxSplits:` kann `omittingEmptySubsequences:` '
                    'angegeben werden. Ausgelassene leere Teilstücke zählen '
                    'nicht gegen das Split-Limit. Ein leerer '
                    'Trenner teilt als Physim-Erweiterung an Unicode-Skalargrenzen. '
                    'Das Ergebnis besitzt unabhängige Stringwerte.\n\nÜberall verfügbar.'))
    entries.append(('array.split(separator:maxSplits:omittingEmptySubsequences:)',
                    '## array.split(separator:maxSplits:omittingEmptySubsequences:)\n\n'
                    '```text\narray.split(separator: value, maxSplits: limit, '
                    'omittingEmptySubsequences: omit) -> [[T]]  // array: [T], T: Equatable\n'
                    '```\n\n`value: T` ist erforderlich. `limit: Int64` ist optional '
                    'und standardmäßig unbegrenzt; ein negativer Wert ist ein '
                    'Laufzeitfehler. `omit: Bool` ist optional und standardmäßig '
                    '`true`. Ausgelassene leere Teilarrays zählen nicht gegen '
                    'das Split-Limit. Teilarrays besitzen unabhängige Wertsemantik '
                    'und beginnen bei Index null.\n\nÜberall verfügbar.'))
    entries.append(('array.starts(with:)',
                    '## array.starts(with:)\n\n```text\n'
                    'array.starts(with: prefix) -> Bool  // array, prefix: [T], T: Equatable\n'
                    '```\n\nPrüft den gleich langen Anfang gegen prefix. Der leere '
                    'Präfix passt immer, ein längerer nie. Beide Eingabewerte '
                    'bleiben unverändert.\n\nÜberall verfügbar.'))
    entries.append(('array.elementsEqual(_:)',
                    '## array.elementsEqual(_:)\n\n```text\n'
                    'array.elementsEqual(other) -> Bool  // array, other: [T], T: Equatable\n'
                    '```\n\nPrüft gleiche Länge und paarweise gleiche Elemente '
                    'in Arrayreihenfolge. Beide Eingabewerte bleiben '
                    'unverändert.\n\nÜberall verfügbar.'))
    entries.append(('array.popLast',
                    '## array.popLast\n\n```text\narray.popLast() -> T?  // array: var [T]\n```\n\n'
                    'Entfernt das letzte Element eines veränderlichen Arrays und liefert '
                    'eine unabhängige Wertkopie. Bei einem leeren Array bleibt dieses '
                    'unverändert und das Ergebnis ist `nil`.\n\nÜberall verfügbar.'))
    entries.append(('array.isEmpty',
                    '## array.isEmpty\n\n```text\narray.isEmpty -> Bool  // array: [T]\n```\n\n'
                    'Prüft, ob das Array leer ist.\n\nÜberall verfügbar.'))
    entries.append(('array.compactMap(_:)',
                    '## array.compactMap(_:)\n\n```text\n'
                    'array.compactMap(transform) -> [U]  // array: [T], transform: func(T) -> U?\n'
                    '```\n\nRuft die Transformation einmal je Element in Arrayreihenfolge auf '
                    'und sammelt vorhandene Ergebnisse als unabhängigen Arraywert. '
                    '`nil`-Ergebnisse werden ausgelassen. Ein leeres Array ruft '
                    'die Transformation nicht auf. Empfänger und Funktionswert '
                    'werden je einmal ausgewertet; Fehler räumen Teilwerte auf.\n\n'
                    'Überall verfügbar.'))
    entries.append(('array.flatMap(_:)',
                    '## array.flatMap(_:)\n\n```text\n'
                    'array.flatMap(transform) -> [U]  // array: [T], transform: func(T) -> [U]\n'
                    '```\n\nRuft die Transformation einmal je Element in Arrayreihenfolge auf '
                    'und hängt ihre Teilarrays zu einem unabhängigen Arraywert '
                    'zusammen. Leere Teilarrays steuern keine Elemente bei. '
                    'Empfänger und Funktionswert werden je einmal ausgewertet; '
                    'Fehler räumen Teilwerte auf.\n\nÜberall verfügbar.'))
    for method in ('prefix', 'drop'):
        description = ('Kopiert die anfänglichen Elemente, solange das Prädikat '
                       'wahr ist.' if method == 'prefix' else
                       'Überspringt anfängliche Elemente, solange das Prädikat '
                       'wahr ist, und kopiert anschließend den Rest.')
        entries.append((f'array.{method}(while:)',
                        f'## array.{method}(while:)\n\n```text\n'
                        f'array.{method}(while: predicate) -> [T]  // array: [T], '
                        'predicate: func(T) -> Bool\n```\n\n'
                        + description + ' Nach dem ersten `false` wird das '
                        'Prädikat nicht erneut aufgerufen. Empfänger und '
                        'Funktionswert werden je einmal ausgewertet; das '
                        'Ergebnis ist eine unabhängige Arraykopie mit Indexbeginn '
                        'null.\n\nÜberall verfügbar.'))
        entries.append((f'String.{method}(while:)',
                        f'## String.{method}(while:)\n\n```text\n'
                        f'text.{method}(while: predicate) -> String  // text: String, '
                        'predicate: func(String) -> Bool\n```\n\n'
                        + description + ' Geprüft wird je ein Unicode-Skalar '
                        'als eigener String. Nach dem ersten `false` wird das '
                        'Prädikat nicht erneut aufgerufen. Das Ergebnis ist ein '
                        'unabhängiger String mit Skalargrenzen.\n\n'
                        'Überall verfügbar.'))
    entries.append(('String.filter(_:)',
                    '## String.filter(_:)\n\n```text\n'
                    'text.filter(predicate) -> String  // text: String, '
                    'predicate: func(String) -> Bool\n```\n\n'
                    'Prüft jeden Unicode-Skalar als eigenen String und übernimmt '
                    'passende Skalare in Quellreihenfolge. Ein leerer String '
                    'ruft das Prädikat nicht auf. Empfänger und Funktion werden '
                    'je einmal ausgewertet; ein Fehler räumt das Teilergebnis '
                    'auf. Das Ergebnis ist ein unabhängiger String.\n\n'
                    'Überall verfügbar.'))
    entries.append(('String.map(_:)',
                    '## String.map(_:)\n\n```text\n'
                    'text.map(transform) -> [U]  // text: String, '
                    'transform: func(String) -> U\n```\n\n'
                    'Ruft die Transformation einmal je Unicode-Skalar als '
                    'eigenem String in Quellreihenfolge auf und sammelt '
                    'die Ergebnisse als unabhängiges Array. Ein leerer String '
                    'ruft die Funktion nicht auf. Empfänger und Funktion werden '
                    'je einmal ausgewertet; Fehler räumen Teilwerte auf.\n\n'
                    'Überall verfügbar.'))
    entries.append(('String.compactMap(_:)',
                    '## String.compactMap(_:)\n\n```text\n'
                    'text.compactMap(transform) -> [U]  // text: String, '
                    'transform: func(String) -> U?\n```\n\n'
                    'Ruft die Transformation einmal je Unicode-Skalar als '
                    'eigenem String auf und sammelt vorhandene Ergebnisse '
                    'in Quellreihenfolge als unabhängiges Array. `nil` wird '
                    'ausgelassen; Fehler räumen Teilwerte auf.\n\n'
                    'Überall verfügbar.'))
    entries.append(('String.flatMap(_:)',
                    '## String.flatMap(_:)\n\n```text\n'
                    'text.flatMap(transform) -> [U]  // text: String, '
                    'transform: func(String) -> [U]\n```\n\n'
                    'Ruft die Transformation einmal je Unicode-Skalar als '
                    'eigenem String auf und hängt die Teilarrays in '
                    'Quellreihenfolge zu einem unabhängigen Array zusammen. '
                    'Leere Teilarrays tragen keine Elemente bei; Fehler '
                    'räumen Teilwerte auf.\n\nÜberall verfügbar.'))
    entries.append(('array.reduce(_:_:)',
                    '## array.reduce(_:_:)\n\n```text\n'
                    'array.reduce(initial, combine) -> U  // array: [T], '
                    'combine: func(U, T) -> U\n```\n\n'
                    'Faltet Elemente in Arrayreihenfolge mit einem Akkumulator. '
                    'Ein leeres Array liefert initial ohne Funktionsaufruf. '
                    'Besitzende Akkumulatoren und Fehler werden nach den '
                    'Wertregeln behandelt.\n\nÜberall verfügbar.'))
    entries.append(('String.reduce(_:_:)',
                    '## String.reduce(_:_:)\n\n```text\n'
                    'text.reduce(initial, combine) -> U  // text: String, '
                    'combine: func(U, String) -> U\n```\n\n'
                    'Faltet Unicode-Skalare als eigene Strings in '
                    'Quellreihenfolge. Ein leerer String liefert initial '
                    'ohne Funktionsaufruf. Fehler räumen den Akkumulator '
                    'und temporäre Skalarwerte auf.\n\nÜberall verfügbar.'))
    entries.append(('array.forEach(_:)',
                    '## array.forEach(_:)\n\n```text\n'
                    'array.forEach(body) -> Void  // array: [T], '
                    'body: func(T) -> Void\n```\n\n'
                    'Ruft body für jedes Element in Arrayreihenfolge auf. '
                    'Ein leeres Array ruft body nicht auf. Die Iteration '
                    'verwendet einen Snapshot des Empfängers.\n\nÜberall verfügbar.'))
    entries.append(('String.forEach(_:)',
                    '## String.forEach(_:)\n\n```text\n'
                    'text.forEach(body) -> Void  // text: String, '
                    'body: func(String) -> Void\n```\n\n'
                    'Ruft body für jeden Unicode-Skalar als eigenen String '
                    'in Quellreihenfolge auf. Ein leerer String ruft body '
                    'nicht auf.\n\nÜberall verfügbar.'))
    for method in ('prefix', 'suffix', 'dropFirst', 'dropLast'):
        dropping = method.startswith('drop')
        signatures = (f'array.{method}() -> [T]  // array: [T]\n'
                      f'array.{method}(count) -> [T]  // count: Int64') if dropping else (
                          f'array.{method}(count) -> [T]  // array: [T], count: Int64')
        entries.append((f'array.{method}(_:)',
                        f'## array.{method}(_:)\n\n```text\n'
                        f'{signatures}\n'
                        '```\n\nWählt bis zu count Elemente aus oder lässt sie weg. '
                        + ('Ohne count gilt 1. ' if dropping else '')
                        + 'Negative Werte sind Fehler; Werte über der Länge werden '
                        'begrenzt. Das Ergebnis ist eine unabhängige Arraykopie '
                        'mit Indexbeginn null.\n\nÜberall verfügbar.'))
        string_signatures = (f'text.{method}() -> String  // text: String\n'
                             f'text.{method}(count) -> String  // count: Int64') if dropping else (
                                 f'text.{method}(count) -> String  // text: String, count: Int64')
        entries.append((f'String.{method}(_:)',
                        f'## String.{method}(_:)\n\n```text\n'
                        f'{string_signatures}\n'
                        '```\n\nWählt bis zu count Unicode-Skalare aus oder lässt sie weg. '
                        + ('Ohne count gilt 1. ' if dropping else '')
                        + 'Negative Werte sind Fehler; Werte über text.count '
                        'werden begrenzt. Das Ergebnis ist ein unabhängiger '
                        'String-Wert.\n\nÜberall verfügbar.'))
    for endpoint in ('first', 'last'):
        entries.append((f'array.{endpoint}',
                        f'## array.{endpoint}\n\n```text\narray.{endpoint} -> T?  // array: [T]\n```\n\n'
                        'Liefert eine unabhängige Kopie des ersten beziehungsweise letzten '
                        'Elements; bei einem leeren Array `nil`.\n\nÜberall verfügbar.'))
        entries.append((f'String.{endpoint}',
                        f'## String.{endpoint}\n\n```text\n'
                        f'text.{endpoint} -> String?  // text: String\n```\n\n'
                        'Liefert den ersten beziehungsweise letzten '
                        'Unicode-Skalar als unabhängigen String; bei einem '
                        'leeren String `nil`.\n\nÜberall verfügbar.'))
    for method in ('sorted', 'min', 'max'):
        result_type = '[String]' if method == 'sorted' else 'String?'
        action = ('Liefert ein unabhängiges Array der Unicode-Skalare in '
                  'aufsteigender Skalarreihenfolge.' if method == 'sorted' else
                  'Liefert den kleinsten beziehungsweise größten Unicode-Skalar '
                  'als unabhängigen String oder `nil` bei leerem Text.')
        entries.append((f'String.{method}()',
                        f'## String.{method}()\n\n```text\n'
                        f'text.{method}() -> {result_type}  // text: String\n```\n\n'
                        + action + '\n\nÜberall verfügbar.'))
        entries.append((f'String.{method}(by:)',
                        f'## String.{method}(by:)\n\n```text\n'
                        f'text.{method}(by: compare) -> {result_type}  // '
                        'compare: func(String, String) -> Bool\n```\n\n'
                        'Vergleicht Unicode-Skalare als eigene Strings. '
                        'Die Sortierung ist stabil; Extrema behalten bei '
                        'gleichem Rang den ersten Skalar. Eingabe und '
                        'Vergleichsfunktion werden je einmal ausgewertet.\n\n'
                        'Überall verfügbar.'))
    entries.append(('array.first(where:)',
                    '## array.first(where:)\n\n```text\n'
                    'array.first(where: predicate) -> T?  // predicate: func(T) -> Bool\n'
                    '```\n\nPrüft Elemente bis zum ersten Treffer und liefert dessen '
                    'unabhängige Kopie; ohne Treffer `nil`.\n\nÜberall verfügbar.'))
    entries.append(('array.last(where:)',
                    '## array.last(where:)\n\n```text\n'
                    'array.last(where: predicate) -> T?  // predicate: func(T) -> Bool\n'
                    '```\n\nSucht vom Ende aus und liefert eine unabhängige Kopie '
                    'des letzten passenden Elements; ohne Treffer `nil`.\n\nÜberall verfügbar.'))
    for endpoint in ('firstIndex', 'lastIndex'):
        entries.append((f'array.{endpoint}(where:)',
                        f'## array.{endpoint}(where:)\n\n```text\n'
                        f'array.{endpoint}(where: predicate) -> Int64?  '
                        '// predicate: func(T) -> Bool\n```\n\n'
                        'Liefert den passenden Index in Suchrichtung oder `nil`.\n\n'
                        'Überall verfügbar.'))
    for endpoint in ('first', 'last', 'firstIndex', 'lastIndex'):
        indexed = 'Index' in endpoint
        direction = 'von hinten' if endpoint.startswith('last') else 'von vorn'
        result_type = 'Int64?' if indexed else 'String?'
        value = 'nullbasierten Skalarindex' if indexed else 'Skalar als eigenen String'
        entries.append((f'String.{endpoint}(where:)',
                        f'## String.{endpoint}(where:)\n\n```text\n'
                        f'text.{endpoint}(where: predicate) -> {result_type}  '
                        '// text: String, predicate: func(String) -> Bool\n```\n\n'
                        f'Prüft Unicode-Skalare {direction} und liefert den '
                        f'ersten passenden {value}. Ohne Treffer `nil`. '
                        'Empfänger und Funktion werden je einmal ausgewertet; '
                        'ein leerer String ruft die Funktion nicht auf.\n\n'
                        'Überall verfügbar.'))
    entries.append(('array.contains(where:)',
                    '## array.contains(where:)\n\n```text\narray.contains(where: predicate) -> Bool  // predicate: func(T) -> Bool\n```\n\n'
                    'Prüft die Elemente in Reihenfolge bis zum ersten `true`; '
                    'ein leeres Array liefert `false`.\n\nÜberall verfügbar.'))
    entries.append(('array.allSatisfy',
                    '## array.allSatisfy\n\n```text\narray.allSatisfy(predicate) -> Bool  // predicate: func(T) -> Bool\n```\n\n'
                    'Prüft die Elemente in Reihenfolge bis zum ersten `false`; '
                    'ein leeres Array liefert `true`.\n\nÜberall verfügbar.'))
    entries.append(('String.contains(where:)',
                    '## String.contains(where:)\n\n```text\n'
                    'text.contains(where: predicate) -> Bool  // text: String, '
                    'predicate: func(String) -> Bool\n```\n\n'
                    'Prüft Unicode-Skalare als eigene Strings in Quellreihenfolge '
                    'bis zum ersten `true`. Ein leerer String liefert `false`. '
                    'Empfänger und Funktion werden je einmal ausgewertet.\n\n'
                    'Überall verfügbar.'))
    entries.append(('String.allSatisfy(_:)',
                    '## String.allSatisfy(_:)\n\n```text\n'
                    'text.allSatisfy(predicate) -> Bool  // text: String, '
                    'predicate: func(String) -> Bool\n```\n\n'
                    'Prüft Unicode-Skalare als eigene Strings in Quellreihenfolge '
                    'bis zum ersten `false`. Ein leerer String liefert `true`. '
                    'Empfänger und Funktion werden je einmal ausgewertet.\n\n'
                    'Überall verfügbar.'))
    for method in ('sort', 'sorted'):
        result_type = 'Void  // array: var [T]' if method == 'sort' else '[T]  // array: [T]'
        entries.append((f'array.{method}(by:)',
                        f'## array.{method}(by:)\n\n```text\n'
                        f'array.{method}(by: compare) -> {result_type}\n'
                        '// compare: func(T, T) -> Bool\n```\n\n'
                        'Sortiert stabil nach einer Vergleichsfunktion. Bei gleichen '
                        'Elementen bleibt die Eingabereihenfolge erhalten; leere und '
                        'einzelne Arrays rufen compare nicht auf. Die Sortierung nutzt '
                        'eine unabhängige Kopie. Bei Fehlern bleibt der mutierende Empfänger '
                        'unverändert.\n\nÜberall verfügbar.'))
    for method in ('min', 'max'):
        entries.append((f'array.{method}()',
                        f'## array.{method}()\n\n```text\n'
                        f'array.{method}() -> T?  // array: [Int64], [Float64] oder [String]\n'
                        '```\n\nLiefert eine unabhängige Kopie des kleinsten beziehungsweise '
                        'größten Elements; bei leerem Array `nil`. Nicht endliche '
                        '`Float64`-Werte sind Fehler.\n\nÜberall verfügbar.'))
        entries.append((f'array.{method}(by:)',
                        f'## array.{method}(by:)\n\n```text\n'
                        f'array.{method}(by: compare) -> T?  // array: [T]\n'
                        '// compare: func(T, T) -> Bool\n```\n\n'
                        'Durchläuft das Array einmal und behält bei gleichem Rang '
                        'das erste Element. Die Vergleichsfunktion wird auf leeren '
                        'und einzelnen Arrays nicht aufgerufen. Das ausgewählte '
                        'Element wird unabhängig kopiert.\n\nÜberall verfügbar.'))
    entries.append(('array.removeAll()',
                    '## array.removeAll()\n\n```text\narray.removeAll() -> Void  // array: var [T]\n'
                    '```\n\nLeert einen veränderlichen Arraypfad. Vorhandene Snapshots '
                    'bleiben gültig.\n\nÜberall verfügbar.'))
    entries.append(('array.removeAll(where:)',
                    '## array.removeAll(where:)\n\n```text\n'
                    'array.removeAll(where: predicate) -> Void  // array: var [T]\n'
                    '// predicate: func(T) -> Bool\n```\n\n'
                    'Entfernt passende Elemente und erhält die Reihenfolge der übrigen. '
                    'Das Prädikat läuft einmal je Element des Snapshots. Bei Fehlern '
                    'bleibt der mutierende Empfänger unverändert.\n\nÜberall verfügbar.'))
    entries.append(('array.append(contentsOf:)',
                    '## array.append(contentsOf:)\n\n```text\n'
                    'array.append(contentsOf: values) -> Void  // array: var [T], values: [T]\n'
                    '```\n\nHängt alle Elemente eines gleich typisierten Arrays an. Selbstanhang '
                    'ist erlaubt; ein Kopierfehler lässt den Empfänger unverändert.\n\n'
                    'Überall verfügbar.'))
    entries.append(('array.insert(contentsOf:at:)',
                    '## array.insert(contentsOf:at:)\n\n```text\n'
                    'array.insert(contentsOf: values, at: index) -> Void\n'
                    '// array: var [T], values: [T], index: Int64\n```\n\n'
                    'Fügt alle Elemente vor index ein; erlaubt sind 0 bis array.count. '
                    'Selbsteinfügung ist erlaubt. Index- und Kopierfehler lassen den '
                    'Empfänger unverändert.\n\nÜberall verfügbar.'))
    entries.append(('array.swapAt(_:_:)',
                    '## array.swapAt(_:_:)\n\n```text\n'
                    'array.swapAt(i, j) -> Void  // array: var [T], i: Int64, j: Int64\n'
                    '```\n\nVertauscht zwei vorhandene Elemente. Gleiche Indizes haben keine '
                    'Wirkung; ungültige Indizes sind Laufzeitfehler. Snapshots bleiben '
                    'gültig, und Kopierfehler lassen den Empfänger unverändert. '
                    'Die Kopie benötigt O(n).\n\nÜberall verfügbar.'))
    entries.append(('array.removeSubrange(_:)',
                    '## array.removeSubrange(_:)\n\n```text\n'
                    'array.removeSubrange(start..<end) -> Void  // array: var [T]\n'
                    '```\n\nEntfernt einen Bereich. Auch start...end ist möglich. '
                    'Grenzen müssen ausdrücklich angegeben und gültig sein. '
                    'Kopierfehler lassen den Empfänger unverändert.\n\nÜberall verfügbar.'))
    entries.append(('array.replaceSubrange(_:with:)',
                    '## array.replaceSubrange(_:with:)\n\n```text\n'
                    'array.replaceSubrange(start..<end, with: values) -> Void\n'
                    '// array: var [T], values: [T]\n```\n\n'
                    'Ersetzt einen Bereich durch ein gleich typisiertes Array. '
                    'Auch start...end ist möglich. Grenzen werden vor der '
                    'Auswertung von values geprüft. Selbstersetzung ist erlaubt; '
                    'Fehler lassen den Empfänger unverändert.\n\nÜberall verfügbar.'))
    for method in ('removeFirst', 'removeLast'):
        entries.append((f'array.{method}()',
                        f'## array.{method}()\n\n```text\n'
                        f'array.{method}() -> T  // array: var [T]\n```\n\n'
                        'Entfernt und kopiert das Endelement unabhängig. Ein leeres '
                        'Array ist ein Laufzeitfehler; Kopierfehler lassen den '
                        'Empfänger unverändert.\n\nÜberall verfügbar.'))
        entries.append((f'array.{method}(_:)',
                        f'## array.{method}(_:)\n\n```text\n'
                        f'array.{method}(count) -> Void  // array: var [T], count: Int64\n'
                        '```\n\nEntfernt count Elemente vom entsprechenden Ende. '
                        'Erlaubt sind 0 bis array.count; Fehler lassen den '
                        'Empfänger unverändert.\n\nÜberall verfügbar.'))
    entries.append(('String(repeating:count:)',
                    '## String(repeating:count:)\n\n```text\nString(repeating: text, count: n) -> String  // text: String, n: Int64\n```\n\n'
                    'Erzeugt einen neuen String aus `n` Kopien des Texts. '
                    'Der Zähler muss nichtnegativ sein; Größenüberschreitungen '
                    'erzeugen eine Quelldiagnose.\n\nÜberall verfügbar.'))
    entries.append(('Array(repeating:count:)',
                    '## Array(repeating:count:)\n\n```text\nArray(repeating: value, count: n) -> [T]  // value: T, n: Int64\n```\n\n'
                    'Erzeugt `n` unabhängige Kopien eines Elements. Der Zähler muss '
                    'nichtnegativ sein.\n\nÜberall verfügbar.'))
    entries.append(('String.trimmingCharacters(in:)',
                    '## String.trimmingCharacters(in:)\n\n```text\n'
                    'text.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines) -> String\n'
                    '```\n\nEntfernt Unicode-Leerraum an beiden Enden des Strings. Derzeit ist '
                    'nur `CharacterSet.whitespacesAndNewlines` verfügbar.\n\nÜberall verfügbar.'))
    entries.append(('String.utf8.count',
                    '## String.utf8.count\n\n```text\ntext.utf8.count -> Int64\n```\n\n'
                    'Zählt die UTF-8-Bytes des Strings.\n\nÜberall verfügbar.'))
    entries.append(('attempt',
                    '## attempt\n\n```text\nattempt(expression) -> T?  // expression: T\n```\n\n'
                    'Führt einen Wertausdruck einmal aus. Ein abgefangener Laufzeitfehler '
                    'liefert `nil`; temporäre Werte werden aufgeräumt. Bereits sichtbare '
                    'Seiteneffekte bleiben bestehen. Der Ausdruck darf nicht `Void` sein.\n\n'
                    'Überall verfügbar.'))
    assert len(pattern.findall(table)) == len(LANG_DESCRIPTIONS), 'Unparsed or stale language entries'
    result.extend(body for _, body in sorted(entries))
    return '\n\n'.join(result) + '\n'


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--check', action='store_true')
    args = parser.parse_args()
    public_modules = {p.stem for p in (ROOT / 'include/physim').glob('*.h')
                      if not p.name.startswith('language_')}
    assert public_modules == set(MODULES), 'Public C modules changed: update MODULES and teaching descriptions'
    outputs = {f'docs/reference/{name}.md': c_reference(name, *info) for name, info in MODULES.items()}
    outputs['docs/reference/language-library.md'] = language_reference()
    tutorial = (ROOT / 'docs/experiment-tutorial.md').read_text(encoding='utf-8')
    for filename, heading, following in [('main.c', 'Experimentcode', 'Wie der Host dein Modell aufruft'),
                                         ('analysis.c', 'Analysecode', 'Was die Auswertung tut')]:
        code = (ROOT / 'examples/documentation' / filename).read_text(encoding='utf-8').rstrip()
        replacement = f'## {heading}\n\n```c\n{code}\n```\n\n## {following}'
        tutorial, count = re.subn(f'## {heading}\n.*?## {following}', lambda _: replacement,
                                  tutorial, flags=re.S)
        assert count == 1, f'Missing tutorial section: {heading}'
    outputs['docs/experiment-tutorial.md'] = tutorial
    stale = []
    for path, expected in outputs.items():
        target = ROOT / path
        if args.check:
            if not target.exists() or target.read_text(encoding='utf-8') != expected: stale.append(path)
        else:
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_text(expected, encoding='utf-8', newline='\n')
    if stale: raise SystemExit('Stale reference; run tools/generate-reference.py: ' + ', '.join(stale))
    # Every help page and Markdown-to-Markdown link must be reachable offline.
    ui = (ROOT / 'app/documentation_ui.inc').read_text(encoding='utf-8')
    topics = set(re.findall(r'\{"[^"]+", "([^"]+)", (?:true|false)\}', ui))
    for topic in topics:
        page = ROOT / topic
        assert page.is_file(), f'Missing bundled page: {topic}'
        assert len(page.read_bytes()) <= 256 * 1024, f'Page exceeds viewer limit: {topic}'
        if not topic.startswith('docs/') or not topic.endswith('.md'): continue
        body = page.read_text(encoding='utf-8')
        for link in re.findall(r'\]\(([^)]+)\)', body):
            if link.startswith(('https://', 'http://')): continue
            target = posixpath.normpath(posixpath.join(posixpath.dirname(topic), link.split('#')[0]))
            if target.endswith('.md'):
                assert target in topics, f'Unreachable offline link: {topic} -> {target}'
    assert set(outputs) <= topics, 'Generated reference is missing from the help navigation'
    print(f'{len(outputs)} reference documents verified' if args.check else f'{len(outputs)} reference documents written')


if __name__ == '__main__':
    main()
