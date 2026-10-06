# Eigene Physim-Sprache

Stand: 2026-10-06. Sprachvertrag 0.172.0; noch keine freigegebene Anwendersprache.
Der Arbeitsname ist „Physim-Sprache“. Das vollständige Ziel und die Abnahmen
LANG-001 bis LANG-007 stehen in Abschnitt 10 des Projektplans.

Die Sprachversion `0.172.0` steht unabhängig von der App-Version und der SDK-ABI
in `src/language/version.h`. `physimc --version` und der Kopf des generierten C
geben sie aus. Änderungen an Syntax oder Semantik erfordern eine bewusste
Anhebung; die vorliegende Fassung ist noch ein Entwicklungsvertrag und keine
Kompatibilitätsgarantie für künftige 1.0-Quellen.
Generierte Module schreiben die tatsächliche Sprachversion und die getrennte
`physimc`-Version in Laufdateien und Analyseberichte.

Für den Weg vom ersten Sprachprojekt über ein Experiment bis zu Messwerten,
Diagramm und Fehlerdiagnose dient das [ausführbar geprüfte Tutorial](language-tutorial.md).

## Ziel und Übersetzung

Die Sprache bietet einen vollständigen zweiten Zugang zu Experimenten und
Auswertung. Statische Typprüfung erfolgt vor nativer Ausführung. Der eigene
Compiler wird in C17 geschrieben; sein erstes Backend erzeugt C17. Der direkte
Build übersetzt dieses zu nativen eigenständigen Programmen sowie
Experiment- und Analysemodulen. Die eigene
Grammatik und Typprüfung sind unabhängig vom C-Compiler. Physik, Messdaten und
Berichte verwenden dieselbe Bibliothek und ABI wie C-Projekte.

Diese Syntax ist als eigenständiges Programm kompilierbar:

```text
func kineticEnergy(mass: Float64, speed: Float64) -> Float64:
    let energy = 0.5 * mass * speed * speed
    return energy

let mass: Float64 = 2.0
var speed = 3.0
let energy: Float64 = kineticEnergy(mass: mass, speed: speed)
```

`let` ist unveränderlich, `var` veränderlich. Typinferenz und explizite Typen
führen zum gleichen statischen Typ. Funktionsparameter und Rückgabewerte sind
explizit typisiert; öffentliche APIs sollen ohne Kenntnis des Funktionskörpers
verständlich sein. Der Parser erkennt benannte und positionale Argumente;
die Typprüfung ordnet sie Parametern zu. Freie Funktionen eines Moduls dürfen
denselben Namen tragen, wenn ihre Parameterlabels oder -typen verschieden sind;
das gilt für konkrete und generische Signaturen. Beim Aufruf entscheiden
Argumentform, statische Typen, Typargumente und Constraints. Ein konkreter
Treffer hat bei sonst gleicher Bewertung Vorrang vor einer generischen Vorlage.
Gleich gute Treffer und fehlende Treffer sind Fehler. Noch nicht gebundene
generische Typen können bei einem Aufruf aus dem erwarteten Rückgabetyp folgen.
Ein Funktionswert aus einer überladenen Gruppe benötigt einen erwarteten
Funktionstyp; eine generische Vorlage muss mit expliziten Typargumenten
spezialisiert werden. Dies gilt auch für importierte Modul-Funktionen.
Struktur- und Enum-Methoden dürfen ebenfalls nach Parameterlabels und -typen
überladen werden, auch als statische oder mutierende Methoden und auf
generischen Strukturtypen. Beim Aufruf entscheidet zusätzlich, ob der Empfänger
ein Typ oder ein Wert ist; die Prüfung auf Veränderlichkeit erfolgt für die
gewählte Methode. Gebundene und statische Methodenwerte benötigen bei mehreren
Kandidaten einen erwarteten Funktionstyp. Generische Methodenwerte werden mit
expliziten Typargumenten spezialisiert. Eigene Strukturinitialisierer besitzen
eine gesonderte Überladungsauflösung nach Parameterform und Parametertyp.

Neben `=` sind `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=` und `>>=`
als Anweisungen unterstützt.
Das Ziel muss eine veränderliche Variable oder ein vollständig veränderlicher
Feldpfad sein. Es gelten die Typregeln und Laufzeitprüfungen des jeweiligen
Rechenoperators. Beispielsweise ist `velocity += acceleration * dt` für `Vec3`
zulässig, `%=` und die fünf bitweisen Zuweisungen dagegen nur für `Int64`.
Der bisherige Zielwert wird vor dem
rechten Ausdruck gelesen; dieser wird genau einmal ausgewertet. Anschließend
wird das Ergebnis zurückgeschrieben. Verkürzte Zuweisungen liefern keinen Wert
und sind daher keine Unterausdrücke oder Variablendeklarationen.

## Lexikalischer Vertrag 0.1

Der interne Lexer unter `src/language/` arbeitet ohne Allokation auf einem
geliehenen Bytepuffer mit expliziter Länge. Tokens enthalten Byteoffset und
Bytelänge sowie 1-basierte Zeile und Byte-Spalte. Der Puffer muss während des
Lesens gültig bleiben. Er benötigt keinen abschließenden Null-Byte.

- Bezeichner: ASCII `[A-Za-z_][A-Za-z0-9_]*` sowie die Start- und
  Fortsetzungszeichen für Bezeichner aus Unicode 15.0.0. Die Tabellen werden
  durch `tools/generate-language-identifiers.py` erzeugt und sind im Compiler
  enthalten; ein Python-Paket ist zur Laufzeit nicht nötig. Die Schreibweise
  bleibt bytegenau: `é` und `é` sind verschiedene Namen, ohne automatische
  Unicode-Normalisierung. Strings und Kommentare erlauben gültiges UTF-8.
- Reservierte Wörter: `let`, `var`, `func`, `mutating`, `static`, `return`, `if`, `guard`, `else`, `while`,
  `for`, `in`, `break`, `continue`, `struct`, `enum`, `case`, `switch`, `default`,
  `import`, `true`, `false`, `nil`.
- Dezimale, hexadezimale (`0x2A`, `0Xff`), binäre (`0b1010`) oder oktale
  (`0o755`) Ganzzahlen sowie Dezimalzahlen mit Nachkommastellen und/oder Exponent
  (`1`, `1.25`, `2e-3`). Ein Vorzeichen ist ein eigener Operator. Dezimalpunkt
  benötigt eine nachfolgende Ziffer; `1...3` bleibt Zahl, Bereichsoperator, Zahl.
  Literale mit Basispräfix benötigen mindestens eine passende Ziffer und enthalten
  keine Bruchteile oder Exponenten. `_` darf ausschließlich zwischen zwei Ziffern
  desselben Zahlenteils stehen, auch in Bruchteil und Exponent (`1_000.5_0e+2_0`).
  Aufeinanderfolgende oder abschließende Trennzeichen und Zahlensuffixe sind Fehler.
- Strings verwenden `"..."` für eine Zeile und `"""..."""` für mehrere
  Zeilen. Beide Formen verstehen `\"`, `\\`, `\n`, `\r`, `\t` und
  `\u{...}` mit einer bis sechs hexadezimalen Ziffern. Der Wert muss ein
  Unicode-Skalar außer U+0000 sein; Surrogate und Werte über U+10FFFF sind
  Fehler. Das Escape wird als UTF-8 kodiert. In der
  dreifach zitierten Form sind einzelne und doppelte Anführungszeichen ohne
  Escape zulässig; erst drei schließen das Literal. Zeilenumbrüche im Inhalt
  werden unabhängig von LF, CRLF oder CR als LF gespeichert. Einrückung und
  Leerzeilen bleiben Teil des Werts; es gibt kein automatisches Entfernen von
  Einrückung. Ein Escape `\r` bleibt dagegen ein CR-Zeichen. Unbekannte Escapes,
  sonstige Steuerzeichen und ungültiges UTF-8 werden abgewiesen.
  Beide Formen unterstützen Interpolation mit `\(ausdruck)`, zum Beispiel
  `"Messwert: \(value)"`. Der Ausdruck wird einmal ausgewertet und muss
  `Bool`, `Int64`, `Float64` oder `String` ergeben. Die Darstellung entspricht
  `String(ausdruck)`; verschachtelte Klammern und Strings sind erlaubt.
  `\\(` bleibt ein wörtlicher Rückstrich gefolgt von `(`. Mehrzeilige
  Interpolation behält die Quellpositionen eingebetteter Ausdrücke.
- `//`-Kommentare und verschachtelte `/* ... */`-Kommentare. Zeilenumbrüche
  innerhalb von Blockkommentaren bleiben als NEWLINE-Tokens sichtbar.
- LF, CRLF und CR zählen jeweils als ein Zeilenumbruch. NEWLINE bleibt ein
  Token, damit der Parser später Anweisungsgrenzen und Fortsetzungen entscheidet.
- Operatoren: `+ - * / % = += -= *= /= %= &= |= ^= <<= >>= == != < <= > >= ! && || & | ^ ~ << >> -> ... ..<`.
  Satzzeichen: `( ) { } [ ] , : ; . ?`. `&` verbindet auch generische Constraints.
  Längste gültige Schreibweise gewinnt.
- Ein Null-Byte im Quellpuffer ist ein Fehler, kein Dateiende. Fehler beenden
  diesen Lexerlauf; weitere Aufrufe liefern dieselbe Diagnose. EOF ist stabil.

Tests prüfen Tokenfolgen, Quellpositionen, nicht terminierte Eingaben,
UTF-8-/Escape-Fehler sowie Fortschritt und Bereichsgrenzen bei Byte-Mutationen.
Lexikalische Annahme allein bedeutet keine syntaktische oder semantische Gültigkeit.

## Parser und Syntaxbaum 0.1

`src/language/parser.c` baut aus dem Quellpuffer einen internen Syntaxbaum auf.
Der Parser läuft ohne Allokation in einen vom Aufrufer bereitgestellten
Knotenpuffer. Index null bedeutet „kein Kind“; der Modulwurzelindex ist bei
Fehlern null. Ein Teilbaum nach einem Fehler darf nicht weiterverarbeitet werden.
Der erste Fehler enthält Byteoffset, Länge, 1-basierte Zeile/Byte-Spalte und eine
statische Meldung. Lexerfehler werden unverändert übernommen.

Die folgende Grammatik ist syntaktisch implementiert. Sie verspricht noch keine
Typprüfung oder ausführbare Programme:

```text
module       = { separator | import | statement }
import       = "import" IDENTIFIER {"." IDENTIFIER} ["as" IDENTIFIER]
block        = ":" NEWLINE INDENT statement { separator | statement } DEDENT
separator    = NEWLINE | ";"
type         = (IDENTIFIER ["." IDENTIFIER] | "[" type "]") {"?"}
function     = "func" IDENTIFIER ["<" IDENTIFIER {"," IDENTIFIER} ">"]
               "(" [parameters] ")" ["->" type] block
record       = "struct" IDENTIFIER ":" NEWLINE INDENT recordMember {separator | recordMember} DEDENT
recordMember = field | ["mutating" | "static"] function
enumeration  = "enum" IDENTIFIER ["<" IDENTIFIER {"," IDENTIFIER} ">"] ":" NEWLINE INDENT
               enumMember {separator | enumMember} DEDENT
enumMember   = "case" IDENTIFIER ["(" parameters ")" | "=" expression]
             | ["mutating" | "static"] function
field        = ("let" | "var") IDENTIFIER ":" type
parameters   = parameter {"," parameter} [","]
parameter    = IDENTIFIER ":" type
variable     = ("let" | "var") IDENTIFIER [":" type] "=" expression
return       = "return" [expression]
condition    = expression | ("let" | "var") IDENTIFIER [":" type] "=" expression
conditional  = "if" condition block ["else" (block | conditional)]
guard        = "guard" condition "else" block
whileLoop    = "while" condition block
forLoop      = "for" IDENTIFIER "in" expression block
assignment   = expression ("=" | "+=" | "-=" | "*=" | "/=" | "%=" | "&="
             | "|=" | "^=" | "<<=" | ">>=") expression
statement    = record | function | variable | return | conditional | guard | whileLoop | forLoop
             | "break" | "continue" | assignment | expression
argument     = [IDENTIFIER ":"] expression
```

Ausdrücke umfassen Namen, Literale, geklammerte Ausdrücke, Arrayliterale,
Funktionsaufrufe, Memberzugriffe und Indizes. Aufrufe und Arrays erlauben ein
abschließendes Komma. Präfixoperatoren sind `+`, `-`, `!` und `~`.
Der bedingte Ausdruck `bedingung ? wennWahr : wennFalsch` bindet schwächer als
alle Binäroperatoren. Die Bedingung muss `Bool` sein; beide Zweige müssen
denselben Werttyp haben. Nur der gewählte Zweig wird ausgewertet. Der Ausdruck
ist rechtsassoziativ, sodass `a ? b : c ? d : e` wie
`a ? b : (c ? d : e)` gelesen wird. Besitzende Ergebnisse bleiben nach dem
Aufräumen des gewählten Zweigs gültig.

Binäroperatoren binden in dieser Reihenfolge von schwach nach stark:

1. `??`
2. `||`
3. `&&`
4. `|`
5. `^`
6. `&`
7. `==`, `!=`
8. `<`, `<=`, `>`, `>=`
9. `...`, `..<`
10. `<<`, `>>`
11. `+`, `-`
12. `*`, `/`, `%`

Binäroperatoren außer `??` werden linksassoziativ geparst; `??` ist
rechtsassoziativ. Präfixoperatoren binden stärker,
danach folgen Aufruf, Member und Index. Typprüfung muss insbesondere unsinnige
verkettete Vergleiche, ungeeignete Operatoroperanden und Zuweisungsziele abweisen.

Blöcke beginnen mit `:` am Ende der Kopfzeile. Der erste Inhalt steht auf einer
neuen, stärker eingerückten Zeile; alle Anweisungen derselben Blockebene beginnen
mit gleich vielen Leerzeichen. Einrückung auf eine äußere Ebene beendet den Block.
Die Modulebene beginnt ohne Einrückung. Vier Leerzeichen pro Ebene werden empfohlen;
andere konsistente Breiten sind zulässig. Tabs in führendem Whitespace werden auf
Codezeilen abgewiesen. Leere und reine Kommentarzeilen ändern die Blockzuordnung
nicht. Leere Blöcke und einzeilige Blöcke sind nicht zulässig. `{}` sind keine
Blocksyntax. Normale Funktionsaufrufe behalten `(...)` und benannte Argumente.
`INDENT` und `DEDENT` in der Grammatik beschreiben Parserentscheidungen; der Lexer
liefert weiterhin Quellpositionen und NEWLINE, keine synthetischen Layouttokens.
Bei Blockkommentaren zählt der führende Whitespace der physischen Codezeile.

Ein einfacher Ausdruck oder eine einfache Anweisung endet an NEWLINE, Semikolon
oder Dateiende. Nach einem Operator oder `=` darf die Fortsetzung in
der nächsten Zeile stehen. Innerhalb von Ausdrucksklammern, Argumentlisten und
Arrayliteralen sind Zeilenumbrüche Whitespace. Ein Operator am Anfang der nächsten
Zeile setzt einen außerhalb dieser Klammern abgeschlossenen Ausdruck nicht fort.
Die Einrückung von Fortsetzungszeilen innerhalb solcher Ausdrücke bestimmt keine
Blockgrenze. `else:` beziehungsweise `else if bedingung:` muss auf derselben
Einrückungsebene wie das zugehörige `if` stehen. Ein nacktes `return` vor NEWLINE
enthält keinen Rückgabewert. Der Doppelpunkt muss auf der letzten Zeile des Kopfes
stehen; eine allgemeine automatische Semikolonerzeugung gibt es nicht.

Die syntaktische Verschachtelung ist auf 128 rekursive Parserstufen begrenzt;
zusätzlich begrenzt die vom Aufrufer übergebene Knotenanzahl den Speicherbedarf.
Das ist kein festes Produktlimit für Quelldateigrößen. Eine spätere Compiler-CLI
muss zusätzlich Eingabe-, Zeit- und AST-Verarbeitungsbudgets vorgeben.

`language_parser` prüft AST-Struktur, Operatorbindung, benannte Argumente,
Typannotationen, Kontrollfluss, Quellpositionen, Kapazitätsgrenzen und tiefe
Verschachtelung. Alle Präfixe und alle Einzelbyteersetzungen eines Beispielmoduls
werden auf kontrollierte Fehler beziehungsweise gültige Knotengrenzen geprüft.
Enums mit eingerückten `case`-Deklarationen und typisierten Nutzdatenfeldern
sind unterstützt. Importe werden vor dem Parsen der Module
aufgelöst.

## Statische Prüfung 0.1

`src/language/checker.c` verarbeitet einen erfolgreichen Parser-Syntaxbaum und
erzeugt pro Knoten Typinformation und aufgelöste Deklarationsreferenzen. Der
Aufrufer stellt diesen zusätzlichen Puffer bereit; die Prüfung allokiert nicht.
Bei Fehlern sind weder Teilbaum noch teilweise Typinformationen zur Codeerzeugung
freigegeben. Die erste Diagnose verweist auf die ursprüngliche Quellposition.

Implementierte Regeln:

- Werttypen sind `Int64`, `Float64`, `Bool`, `String`, eigene Strukturen und Enums sowie
  die SDK-Typen `Vec2`, `Vec3`, `Vec4`, `Quat`, `Mat3`, `Mat4`, `Bezier3`, `Unit`, `Quantity`, `Channel`, `Dataset`, `Series`, `Plot`, `Table`,
  `Distribution`, `SensorConfig`, `Sensor`, `Measurement`, `Rng`, `OdeResult`, `ScalarResult`, `Body`, `Contacts`,
  `ContactSolver`, `ContactResult`, `DistanceJoint`, `JointResult`,
  `ContactConstraint`, `JointConstraint`, `ConstraintResult`, `Sweep`, `Aabb`,
  `CollisionPair` und `Submersion`.
  `Void` ist nur
  als Funktionsergebnis zulässig; ohne Ergebnisannotation ist eine Funktion `Void`.
  `let`-Deklarationen benötigen einen Initialwert. Bei `var` darf dieser nur
  fehlen, wenn eine Werttypannotation vorhanden ist (`var ergebnis: Int64`).
  Mit Initialwert wird der Typ ohne Annotation inferiert; eine Annotation muss
  passen.
- Ganzzahlliterale sind standardmäßig `Int64`; beide Grenzen einschließlich
  `-9223372036854775808` beziehungsweise `-0x8000000000000000` werden geprüft.
  Literale mit `0x`-, `0b`- oder `0o`-Präfix liegen auch in einem
  `Float64`-Kontext im vorzeichenbehafteten `Int64`-Bereich. Ein erwarteter `Float64`-Typ erlaubt
  Ganzzahlliterale als Fließkommaliterale. Der numerische Kontext einer Rechnung
  wirkt auf beide Seiten: `1 + 2.5` und `2.5 + 1` sind `Float64`. Bereits als
  `Int64` deklarierte Werte werden nicht implizit konvertiert. Explizite
  Konvertierungen verwenden `Float64(wert)`, `Int64(wert)` oder `String(wert)`;
  die Regeln stehen unten.
- Skalare Arithmetik verlangt gleiche numerische Typen; `%` verlangt `Int64`.
  Bitweises `&`, `|`, `^`, `~`, `<<` und `>>` verlangt `Int64` und arbeitet
  auf allen 64 Bits des Zweierkomplementmusters. `<<` verwirft herausgeschobene
  Bits; `>>` erweitert bei negativen Zahlen das Vorzeichen. Die Schiebezahl muss
  zwischen 0 und 63 liegen, sonst entsteht ein Laufzeitfehler mit Quellposition.
  Die Operatoren werten ihre Operanden einmal von links nach rechts aus.
  Numerische Vergleiche liefern `Bool`; `==`/`!=` verlangen gleiche Werttypen.
  Logische Operatoren und Bedingungen verlangen `Bool`. Strings sind unveränderliche
  UTF-8-Werte. `+` verkettet zwei Strings; `==` und `!=` vergleichen den Inhalt.
  `<`, `<=`, `>` und `>=` ordnen Strings lexikografisch nach
  Unicode-Skalarwerten. Ein Präfix steht vor dem längeren String.
  Variablen, Parameter und Ergebnisse teilen den Wert bis zur Freigabe.
  `text.count` zählt Unicode-Skalare, `text.utf8.count` die UTF-8-Bytes und
  `text.isEmpty` prüft auf einen leeren Wert. `text[index]` liefert den einzelnen
  Skalar als String. Bereiche wie `text[1..<3]`, `text[1...2]` und
  `text[... by 2]` erzeugen neue Strings; Grenzen und Schritte folgen den
  Arrayregeln. Unicode-Graphemcluster werden nicht als eine Einheit behandelt.
  Stringindizes und -bereiche sind nicht schreibbar.
  `text.first` und `text.last` liefern den ersten beziehungsweise letzten
  Unicode-Skalar als unabhängigen `String?`; bei einem leeren String `nil`.
  Wie bei der String-Iteration ist ein Physim-Skalar nicht immer ein
  vollständiger Swift-`Character`.
  `text.sorted()` liefert die Unicode-Skalare als unabhängiges `[String]` in
  aufsteigender Skalarreihenfolge. `text.sorted(by: compare)` verwendet
  `func(String, String) -> Bool` und sortiert stabil. `text.min()` und
  `text.max()` liefern den kleinsten beziehungsweise größten Skalar als
  `String?`; mit `by:` gilt dieselbe Vergleichsfunktion wie bei Arrays.
  Leere Strings liefern `[]` beziehungsweise `nil`. Bei gleichem Rang
  behalten `min` und `max` den zuerst gefundenen Skalar. Alle Varianten
  verwenden einen Snapshot und räumen temporäre Werte bei Fehlern auf.
  `text.prefix(n)` und `text.suffix(n)` liefern die ersten beziehungsweise
  letzten höchstens `n` Unicode-Skalare. `text.dropFirst(n)` und
  `text.dropLast(n)` lassen höchstens `n` Skalare weg; ohne Argument gilt
  `n = 1`. Negative Anzahlen sind Laufzeitfehler, größere Anzahlen werden
  auf `text.count` begrenzt. Anders als Swifts `Substring` ist das Ergebnis
  ein unabhängiger Physim-`String`. Graphemcluster bleiben weiterhin keine
  einzelne Zähleinheit. Eingabe und Snapshots bleiben bei Allokationsfehlern
  unverändert.
  `text.prefix(while: predicate)` nimmt die anfänglichen Unicode-Skalare,
  solange `func(String) -> Bool` wahr ist. `text.drop(while: predicate)` lässt
  diese Skalare weg und liefert den Rest ab dem ersten `false`. Danach wird das
  Prädikat nicht mehr aufgerufen. Jeder geprüfte Skalar ist ein eigener
  ein-Skalar-`String`. Empfänger und Funktion werden je einmal ausgewertet;
  ein leerer String ruft die Funktion nicht auf. Beide Ergebnisse sind
  unabhängige `String`-Werte. Die Auswahl erfolgt an Skalargrenzen, nicht an
  Graphemclustergrenzen.
  `text.filter(predicate)` prüft mit `func(String) -> Bool` jeden Unicode-Skalar
  und übernimmt diejenigen mit Ergebnis `true` in Quellreihenfolge. Jeder
  geprüfte Skalar ist ein eigener ein-Skalar-`String`. Empfänger und Funktion
  werden je einmal ausgewertet; ein leerer String ruft die Funktion nicht auf.
  Das Ergebnis ist ein unabhängiger `String`. Bei einem abgefangenen Fehler im
  Prädikat wird ein begonnenes Ergebnis freigegeben.
  `text.map(transform)` erhält `func(String) -> U` und liefert `[U]` mit einem
  transformierten Wert je Unicode-Skalar in Quellreihenfolge. `U` muss ein
  Werttyp sein. Empfänger und Transformation werden je einmal ausgewertet;
  ein leerer String ruft die Funktion nicht auf. Besitzende Ergebnisse werden
  in das unabhängige Array kopiert; Fehler räumen Teilwerte auf.
  `text.compactMap(transform)` erhält `func(String) -> U?` und sammelt nur
  vorhandene Ergebnisse als `[U]`. `text.flatMap(transform)` erhält
  `func(String) -> [U]` und hängt die Teilarrays in Quellreihenfolge zu `[U]`
  zusammen. Leere Strings rufen die Transformation nicht auf; `nil` und leere
  Teilarrays steuern keine Elemente bei. Die Ergebnisse sind unabhängige
  Arraywerte. Bei Fehlern werden angefangene Werte freigegeben.
  `text.reduce(initial, combine)` faltet die Unicode-Skalare von links nach
  rechts mit `func(U, String) -> U`. `initial` und `combine` werden je einmal
  ausgewertet; bei einem leeren String wird `initial` ohne Funktionsaufruf
  zurückgegeben. Besitzende Akkumulatoren bleiben unabhängige Werte;
  Fehler räumen den letzten Akkumulator und temporäre Skalarwerte auf.
  `text.forEach(body)` ruft `func(String) -> Void` für jeden Unicode-Skalar
  in Quellreihenfolge auf und liefert `Void`. Ein leerer String ruft `body`
  nicht auf. Empfänger und Funktion werden je einmal ausgewertet; die
  Iteration verwendet einen Snapshot und gibt temporäre Skalarwerte auch
  nach einem abgefangenen Fehler frei.
  `text.contains(where: predicate)` prüft mit `func(String) -> Bool` jeden
  Unicode-Skalar bis zum ersten `true`; `text.allSatisfy(predicate)` prüft bis
  zum ersten `false`. Ein leerer String ergibt dabei `false` beziehungsweise
  `true`, ohne das Prädikat aufzurufen. Empfänger und Funktion werden je einmal
  ausgewertet. Die Prüfungen verwenden einen Snapshot des Eingabestrings.
  `text.first(where: predicate)` und `text.last(where: predicate)` liefern den
  ersten beziehungsweise letzten passenden Unicode-Skalar als `String?`.
  `text.firstIndex(where: predicate)` und `text.lastIndex(where: predicate)`
  liefern dessen nullbasierten Skalarindex als `Int64?`. Ohne Treffer liefern
  alle vier Methoden `nil`. Die Suche nach dem letzten Treffer prüft von hinten
  und endet beim ersten passenden Skalar. Ein leerer String ruft das Prädikat
  nicht auf; Empfänger und Funktion werden je einmal ausgewertet.
  `attempt(text[index])` liefert bei gültigem Unicode-Skalarindex einen
  String mit genau einem Skalar als `String?`. Negative Indizes und Werte ab
  `text.count` liefern `nil`; Empfänger und Index werden je einmal ausgewertet.
  `text.contains(needle)` prüft eine Teilzeichenfolge. `text.firstIndex(of: needle)`
  liefert ihren ersten Unicode-Skalarindex als `Int64?` oder `nil`. Ein leerer
  Suchtext passt immer an Index null. `text.lastIndex(of: needle)` liefert den
  letzten Unicode-Skalarindex oder `nil`; ein leerer Suchtext passt nach dem
  letzten Skalar an Index `text.count`. `text.hasPrefix(part)` und
  `text.hasSuffix(part)` liefern `Bool`; der leere String ist immer Präfix und
  Suffix. `text.replacingOccurrences(of: search, with: replacement)` liefert
  einen neuen String. Es ersetzt von links nach rechts alle nicht überlappenden
  Treffer an Skalargrenzen. Ein leerer Suchtext fügt den Ersatz vor, zwischen und
  nach allen Skalaren ein; ein leerer Ersatz löscht Treffer. Alle Methoden
  verändern ihre Eingabewerte nicht.
  `text.split(separator: part, maxSplits: n, omittingEmptySubsequences: omit)`
  liefert `[String]`. `separator:` ist erforderlich; `maxSplits:` ist optional
  und standardmäßig unbegrenzt, `omittingEmptySubsequences:` ist optional und
  standardmäßig `true`. Das Limit muss nichtnegativ sein; ausgelassene leere
  Teilstücke zählen nicht gegen das Limit. Mit `false` bleiben leere
  Felder am Anfang, zwischen zwei Treffern und am Ende erhalten; der leere
  String ergibt dann ein Feld. Ein leerer Trenner ist eine Physim-Erweiterung:
  Er teilt an Unicode-Skalargrenzen und liefert beim leeren String ein leeres
  Array. Treffer überlappen nicht. Das Ergebnis besitzt seine Elemente und
  kann unabhängig von den Eingabewerten gespeichert oder zurückgegeben werden.
  `parts.joined(separator: text)` verbindet ein `[String]` in Arrayreihenfolge
  mit dem Trenner zwischen den Elementen. Ein leeres Array ergibt `""`, ein
  einzelnes Element unveränderten Text. Das Ergebnis besitzt eigenen Speicher;
  Eingabearray und Trenner bleiben unverändert.
  `text.reversed()` liefert einen neuen String mit umgekehrter Reihenfolge der
  Unicode-Skalare. Die Bytes innerhalb jedes UTF-8-Skalars bleiben zusammen;
  der Eingabewert ändert sich nicht. Graphemcluster werden nicht als Einheit
  umgekehrt. Eine mutierende `reverse()`-Methode gibt es für Strings nicht.
  `String(repeating: text, count: n)` liefert einen neuen String aus `n` Kopien des
  Texts. Null und ein leerer Eingabetext ergeben `""`; ein negativer `Int64`-
  Zähler ist ein Laufzeitfehler an der Aufrufstelle. Überlauf und zu große
  Ergebnisse werden vor dem Kopieren abgewiesen. Die Unicode-Skalare bleiben
  unverändert, der Eingabewert bleibt gültig.
  `text.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines)` entfernt
  am Anfang und Ende die Zeichen mit der Eigenschaft
  [`White_Space` aus Unicode 15.0.0](https://www.unicode.org/Public/15.0.0/ucd/PropList.txt).
  Innere Zeichen und UTF-8-Skalare bleiben erhalten. Ein reiner Leerraumwert
  ergibt `""`; U+200B und U+FEFF gehören nicht zu dieser Eigenschaft und bleiben
  erhalten. Die Methode ändert den Eingabewert nicht.
- Jeder eingerückte Block hat einen lexikalischen Gültigkeitsbereich.
  Doppelte Deklarationen in derselben Ebene werden abgewiesen; äußere Namen dürfen
  verdeckt werden. Parameter und der direkte Funktionskörper teilen eine Ebene.
  Variablen mit Initialwert sind innerhalb dieser Ebene erst nach dessen
  Auswertung sichtbar; `var` ohne Initialwert ist nach der Deklaration sichtbar.
  `let`, Parameter und Schleifenvariablen sind unveränderlich; nur `var` ist ein
  zulässiges Zuweisungsziel.
  Ein `var` ohne Initialwert wird bei der ersten einfachen Zuweisung nach
  Auswertung der rechten Seite initialisiert. Vorheriges Lesen liefert eine
  Laufzeitdiagnose mit Quellposition. Das gilt auch für besitzende Werttypen,
  Funktions-Captures und globale Variablen. Zusammengesetzte Zuweisungen sowie
  Feld- und Indexänderungen benötigen bereits einen initialisierten Wurzelwert.
  Die Laufzeitprüfung deckt Zuweisungen in Bedingungen und Schleifen ab; beim
  Verlassen des Gültigkeitsbereichs werden nur tatsächlich gehaltene Werte
  freigegeben.
- Funktionen auf Modulebene werden vorab mit vollständiger Signatur registriert,
  sodass Vorwärtsaufrufe und gegenseitige Rekursion möglich sind. Funktionskörper
  sehen die Modulvariablen. Modulcode läuft in Quellreihenfolge; Lesen oder
  Schreiben einer noch nicht initialisierten globalen Variable ist ein Laufzeitfehler.
  Freie Funktionen auf Modulebene können als Werte verwendet werden. Ihr
  struktureller Typ hat die Form `func(T1, T2) -> R`; auch `Void` als Ergebnis
  ist zulässig. Aufrufe über
  einen Funktionswert verwenden positionale Argumente, während direkte Aufrufe
  weiterhin benannte Argumente unterstützen. Funktionswerte können in Variablen,
  Strukturen, Arrays und Optionalwerten liegen sowie übergeben und zurückgegeben
  werden. Statische Methoden von Strukturen und Enums können ebenfalls als
  Funktionswerte genutzt werden, auch auf spezialisierten generischen Strukturen
  und importierten Typen. Konstruktoren und unspezialisierte generische Methoden
  sind ausgenommen. Der Aufruf verwendet den aktuellen Modulzustand, sodass die
  Funktion ihre Modulvariablen sieht. Generische freie Funktionen und statische
  Methoden können mit expliziten Typargumenten, etwa `identity<Int64>`, als Werte
  verwendet werden. Instanzmethoden können als gebundene Werte
  gespeichert werden: Der Empfänger wird beim Binden kopiert und gehört danach
  dem Funktionswert. Auch besitzende Felder folgen den normalen Kopier- und
  Aufräumregeln; eine spätere Änderung des ursprünglichen Structs ändert die
  gebundene Kopie nicht. Mutierende gebundene Methoden ändern den Zustand dieser
  Empfängerkopie. Kopien desselben Funktionswerts teilen den fortgeschriebenen
  Zustand; erneutes Binden erzeugt eine unabhängige Empfängerkopie. Das gilt
  auch für Enum-Methoden, generisch spezialisierte Methoden und numerische
  Callbacks. Gleichzeitige Zugriffe auf denselben Funktionswert sind nicht
  unterstützt. Anonyme Funktionen mit typisierten Parametern und
  eingerücktem Körper sind ebenfalls Funktionswerte:

  ```text
  func makeAdder(base: Int64) -> func(Int64) -> Int64:
      return func(value: Int64) -> Int64:
          return base + value
  ```

  Sie kopieren verwendete lokale Werte beim Erzeugen. Die Kopie überlebt den
  ursprünglichen Gültigkeitsbereich und folgt den Besitzregeln für Arrays,
  Strings und eigene Werttypen. Auch verschachtelte anonyme Funktionen können
  Werte aus äußeren Funktionen verwenden. Benannte lokale Funktionen verwenden
  dieselbe Besitzregel und können sich selbst rekursiv aufrufen. Ihre Deklaration
  gilt ab ihrer Quellposition; direkte Aufrufe erlauben benannte Argumente,
  Aufrufe über gespeicherte Funktionswerte verwenden positionale Argumente.
  Auch benannte lokale Funktionen mit Typparametern und Constraints werden bei
  Verwendung spezialisiert. Jede Spezialisierung besitzt die beim Deklarieren
  sichtbaren lokalen Werte; `format<String>` kann als Funktionswert gespeichert
  werden. Ein generischer lokaler Körper sieht keine erst später deklarierten
  Werte.
  Innerhalb der Closure sind erfasste Werte unveränderlich; ein späteres Ändern
  des ursprünglichen `var` ändert
  ihre Kopie nicht. Modulvariablen werden weiterhin über ihren globalen Namen
  gelesen. Anonyme Funktionen können numerische Callbacks sein; generische
  Funktionen dürfen Funktionswerte als Argumente erhalten.
- Aufrufe verwenden entweder ausschließlich positionale oder ausschließlich
  benannte Argumente. Benannte Argumente dürfen in anderer Reihenfolge stehen;
  unbekannte, doppelte, fehlende und überzählige Argumente sind Fehler. Jeder
  Argumentknoten erhält die Referenz auf seinen Parameter.
- Nicht-`Void`-Funktionen müssen auf allen strukturellen Pfaden einen passenden
  Wert zurückgeben. Ein `if` mit zwei zurückgebenden Zweigen genügt; Schleifen
  werden konservativ als möglicherweise nicht ausgeführt behandelt. `return`
  außerhalb von Funktionen sowie `continue` außerhalb von Schleifen sind Fehler.
  `break` benötigt eine Schleife oder einen `switch`.
  `for` akzeptiert `Int64`-Bereiche `...` und `..<`, Arrays und Strings. Die
  unveränderliche Schleifenvariable hat bei Bereichen den Typ `Int64`, bei Arrays
  den Elementtyp und bei Strings den Typ `String` mit genau einem Unicode-Skalar.
  Arrays und Strings werden einmal ausgewertet und als feste Wertkopie durchlaufen.
  Bereiche sind keine speicherbaren Werte.
- `print(wert)` gibt einen skalaren Wert mit anschließendem Zeilenumbruch aus;
  `assert(bedingung)` verlangt `Bool` und beendet bei `false` das Programm mit einer
  Quelldiagnose. `assert(bedingung, message: text)` akzeptiert zusätzlich einen
  `String` als Diagnose; das Label `message:` kann weggelassen werden. Bedingung
  und Text werden wie andere Funktionsargumente einmal von links nach rechts
  ausgewertet, auch wenn die Bedingung wahr ist. Ein leerer Text verwendet die
  Standardmeldung. `print` erwartet genau ein positionales Argument. Beide
  Funktionen liefern `Void`.
  Eigene Deklarationen dürfen diese Namen verdecken.

Der [Array- und Besitzvertrag](language-values.md) beschreibt die implementierte
Speicherbasis, native Codeerzeugung und Auswertungsreihenfolge besitzender Werte.

## Mehrdatei-Module

`import Name` sucht `Name.phys` zuerst im Verzeichnis der importierenden Datei.
`import Modelle.Pendel` sucht entsprechend `Modelle/Pendel.phys`; der letzte
Name ist der Modulbezeichner. Wiederverwendbare Modulverzeichnisse können mit
`physimc --check --module-path bibliothek quelle.phys` angegeben werden.
Mehrere `--module-path`-Optionen werden in ihrer Reihenfolge durchsucht, auch
für transitive Imports; höchstens 16 sind zulässig. Die lokale Datei hat immer
Vorrang. Dateipfade werden für die Erkennung mehrfacher Imports und von Zyklen
normalisiert.
Mit `import Modelle.Pendel as Versuch` kann ein anderer Bezeichner gewählt
werden. Pfadteile müssen gültige Bezeichner sein; Elternpfade sind nicht
zulässig. Das gilt auch für transitive Imports. Jede Datei wird pro
Compileraufruf nur einmal geladen; Importzyklen und fehlende Dateien liefern
Quelldiagnosen. Deklarationen bleiben im Namensraum ihrer Datei: `Name.funktion()`,
`Name.Struktur(...)`, `Name.Struktur.statischeMethode()`, `Name.Enum.fall`,
`Name.globaleVariable` und Typannotationen wie `Name.Struktur` sind möglich.
Gleichnamige Deklarationen in verschiedenen Dateien kollidieren nicht. Ein
Modul muss direkt importiert werden, um seine Namen zu verwenden.

Importierte Modulvariablen sind über den qualifizierten Namen lesbar. Änderungen
erfolgen innerhalb ihres eigenen Moduls; ein importierendes Modul kann ihnen
nicht direkt zuweisen. Beim Programmstart werden importierte Module vor ihrem
Importeur initialisiert. Die vorhandene Laufzeitprüfung verhindert auch dabei
das Lesen noch nicht initialisierter globaler Werte. Experiment- und
Analysemodule verwenden denselben Importmechanismus; Einstiegscallbacks werden
nur in der als Einstieg angegebenen Datei gesucht.

`physimc --deps quelle.phys` listet die geladenen Dateien einmalig auf; auch
`--deps` akzeptiert `--module-path`, ebenso alle drei Ausgabearten
`--emit-c`, `--emit-experiment` und `--emit-analysis`. In App-Projekten löst der
Compiler relative Importe ausgehend von den Quelldateien auf. `physim-build`
prüft und übersetzt sie bei jedem Build erneut; der Vergleich des vorverarbeiteten
C-Codes entscheidet anschließend über den nötigen C-Neubau. Der Compiler begrenzt
alle Quellen zusammen auf 1 MiB, alle AST-Knoten zusammen auf 65.536 und den
Importgraphen auf 128 Dateien.
[Das Modulbeispiel](../examples/language/modules/main.phys) enthält einen
geteilten transitiven Import, einen Unterordnerimport mit Alias und
gleichnamige Funktionen in zwei Modulen.

## Generische Funktionen

Freie Funktionen und Strukturmethoden können bis zu acht Typparameter deklarieren. Der Compiler
leitet konkrete Typen aus den Argumenten und, falls vorhanden, dem erwarteten
Ergebnistyp ab. Er erzeugt für jede verwendete Typkombination eine eigene,
vollständig typgeprüfte C-Funktion. Beispielsweise erzeugt
`func identity<T>(value: T) -> T: ...` getrennte Varianten für `Int64`,
`Float64` und `[Int64]`. Auch `[T]`, `T?`, mehrere Typparameter, benannte
Argumente, rekursive Aufrufe und Imports sind unterstützt. Nominale Struktur-
und Enumtypen bleiben verschieden. Eine Spezialisierung ist auf Werttypen
beschränkt und übernimmt deren vorhandene Kopier-, Besitz- und Fehlerregeln.

Unbekannte Typen und doppelte Namen in einer generischen Signatur werden
bereits bei der Deklaration gemeldet. Der Funktionskörper wird für jede
verwendete Typkombination geprüft; ein darin typabhängig ungültiger Ausdruck
meldet die Position der generischen Quelldatei. Typparameter müssen beim Aufruf
ableitbar sein. Ein untypisiertes `nil` oder `[]` liefert allein keinen Typ;
ein anderes Argument, eine annotierte Variable oder ein erwarteter Ergebnistyp
kann Kontext geben. Leere Arrays und `nil` dürfen dabei auch vor dem typgebenden
Argument stehen. Bei mehrfach verwendetem `T` kann ein späteres
`Float64`-Argument frühere Ganzzahlliterale typisieren. Das gilt auch
für Literale mit Vorzeichen, innerhalb von `T?` und `[T]` sowie für freie,
Instanz- und statische generische Funktionen. Eine bereits als `Int64`
typisierte Variable wird
dabei nicht umgewandelt.
Der Compiler begrenzt ein Programm auf 256 verschiedene Spezialisierungen und
alle erzeugten Knoten auf das gemeinsame AST-Budget. Damit wird auch
polymorphe Rekursion begrenzt. Instanzmethoden, mutierende Methoden und statische
Methoden werden ebenso spezialisiert; ihre Empfänger- und Besitzregeln gelten
unverändert. Aufrufe können alle Typargumente ausdrücklich zwischen Funktionsnamen
und Argumentliste angeben, etwa `identity<Float64>(2)`,
`identity<[Int64]>([])` oder `slot.echo<Int64>(3)`. Auch importierte Typen
wie `Module.Struktur` sind als Typargument erlaubt. Die Anzahl muss der
Deklaration entsprechen; eine teilweise Angabe ist nicht vorgesehen.
Die Folge `<Typen>(` nach einem Funktions- oder Methodennamen wird als
generischer Aufruf gelesen. Eine abgeschlossene Folge `<Typen>` vor dem Ende
eines Ausdrucks oder vor `,`, `)` oder `]` ist eine spezialisierte
Funktionsreferenz, etwa `identity<Int64>` oder `Math.scale<Float64>`.
In anderen Fällen bleibt `<` ein Vergleichsoperator.
Die eingebauten Constraints sind im Abschnitt zu generischen Strukturen
beschrieben.
[Das Beispiel](../examples/language/generics/main.phys) prüft auch besitzende
Werte aus importierten generischen Funktionen und Methoden.

## Generische Strukturen

Strukturen können bis zu acht Typparameter besitzen. Jede verwendete
Typkombination erzeugt einen eigenen nominalen Werttyp mit geprüftem Layout und
den bestehenden Kopier- und Besitzregeln. Konstruktoren leiten Typargumente
aus den Feldwerten oder dem erwarteten Ergebnistyp ab; alternativ können alle
Typargumente ausdrücklich angegeben werden. Typannotationen und statische
Methodenaufrufe geben die Typargumente ausdrücklich an.

```text
struct Box<T>:
    var value: T
    func get() -> T:
        return self.value

let numbers: Box<[Int64]> = Box<[Int64]>([1, 2])
assert(numbers.get()[1] == 2)
let inferred = Box(3)
assert(inferred.get() == 3)
```

Felder können andere generische Strukturen, Arrays und optionale Werte
enthalten. `Node<T>?` als rekursives Feld ist zulässig; ein unmittelbar
enthaltener `Node<T>` hätte unendliche Wertgröße und wird abgewiesen.
Instanzmethoden, mutierende und statische Methoden sowie deren eigene
Typparameter funktionieren für spezialisierte Strukturen. Ein Methodentypparameter
darf keinen Typparameter der Struktur verdecken. Importierte Typen werden als
`Modul.Box<Int64>` geschrieben. Bei expliziter Angabe sind fehlende oder
überzählige Typargumente Compilerfehler; bei Ableitung muss jeder Typparameter
aus Feldwerten oder Ergebniskontext bestimmbar sein. Generische Funktionen
können Typparameter auch aus einem spezialisierten Strukturargument ableiten.
Die gemeinsame Grenze von 256 Spezialisierungen gilt für
Funktionen, Methoden, Strukturen und Enums zusammen.

## Generische Enums

Enums können ebenfalls bis zu acht Typparameter besitzen. Jede verwendete
Typkombination ist ein eigener nominaler Werttyp. Nutzlasten verwenden die
Kopier- und Besitzregeln der konkreten Typen; rekursive Felder sind über
Optionals möglich. Fälle mit Nutzlast leiten Typargumente aus ihren Werten ab.
Ein Fall ohne Nutzlast benötigt ausdrücklich angegebene Typargumente oder einen
erwarteten Ergebnistyp.

```text
enum Choice<T>:
    case none
    case some(value: T)

let filled = Choice.some(value: 7)
let empty: Choice<Int64> = Choice.none
```

Generische Enums unterstützen Instanzmethoden, mutierende und statische
Methoden, Fallmuster, Raw Values, Constraints und Methoden mit eigenen
Typparametern. `Choice<Int64>.some(value: 7)` und
`Modul.Choice<Int64>.some(value: 7)` geben den Typ ausdrücklich an. Funktionen
können Typparameter aus spezialisierten Enumargumenten ableiten.

## Generische Constraints

Ein Typparameter kann mit `:` eine oder mehrere Anforderungen tragen. `&`
verbindet Anforderungen, zum Beispiel `T: Numeric & Equatable`. Unterstützt
sind derzeit diese eingebauten Kategorien:

| Constraint | Zulässige Typen |
| --- | --- |
| `Numeric` | `Int64`, `Float64` |
| `Scalar` | `Bool`, `Int64`, `Float64`, `String` |
| `Equatable` | Skalare, `Vec2`/`Vec3`/`Vec4`, Arrays und Optionals mit vergleichbarem Inhalt sowie Strukturen und Enums, deren Felder vergleichbar sind |
| `Comparable` | `Int64`, `Float64`, `String`; diese Typen unterstützen `<`, `<=`, `>` und `>=` und sind auch `Equatable` |
| `Vector` | `Vec2`, `Vec3`, `Vec4` |

Constraints gelten für freie Funktionen, Methoden, Strukturen und Enums. Sie werden
bei jeder Spezialisierung geprüft, auch wenn Typargumente abgeleitet wurden.
Der Funktionskörper wird weiterhin für den konkreten Typ geprüft; `Numeric`
erlaubt beispielsweise `Float64`, aber `%` verlangt weiterhin `Int64`.
Unbekannte oder doppelte Constraintnamen sind Deklarationsfehler. Eigene
Protokolle, allgemeine `where`-Klauseln und ein unabhängig vom Aufruf geprüfter
generischer Funktionskörper sind noch nicht implementiert.

Typgebundene Operationen werden als Methoden am Empfänger aufgerufen:
`vector.normalized()`, `rotation.rotate(vector)`, `channel.sample(value)` und
`dataset.series("time")`. Die Methodenauflösung verwendet den statischen
Empfängertyp; etwa `dot` funktioniert für `Vec2`, `Vec3` und `Vec4` ohne
Dimensionssuffix. Aufrufe können verkettet werden. Zuerst wird der Empfänger
einmal ausgewertet, danach die Argumente in ihrer Quellreihenfolge. Benannte
Argumente bezeichnen ausschließlich die verbleibenden Methodenparameter;
der Empfänger darf nicht nochmals als Argument übergeben werden.
Konstruktoren heißen wie ihr Typ (`Unit`, `Channel`, `Dataset`); eine alternative
Quaternion-Konstruktion ist `Quat.axisAngle(axis, angle)`.
Die früheren globalen Schreibweisen typgebundener Operationen sind entfernt.
Mathematische Funktionen sowie Operationen des aktuellen Simulations-/Berichtskontexts
bleiben frei aufrufbar. Eigene freie Funktionen können weiterhin definiert werden.

Arraytypen `[T]`, homogene Literale, Indexzugriffe und veränderliche Elementpfade
werden statisch geprüft und nativ ausgeführt, einschließlich verschachtelter Arrays
und Strukturfelder. `array.count` liefert ihre Länge als `Int64`.
`attempt(array[index])` liefert `T?`: Bei einem gültigen Index enthält es
eine unabhängige Kopie des Elements, andernfalls `nil`. Empfänger und Index
werden einmal von links nach rechts ausgewertet. Bei einem `[T?]`-Array hat das Ergebnis den Typ
`T??`, sodass ein vorhandenes `nil`-Element von einem fehlenden Index
unterschieden werden kann.
`array.contains(value)` liefert `Bool` und sucht vom Anfang bis zum ersten
gleichen Element. Es verwendet dieselbe strukturelle Gleichheit wie `==`,
auch für verschachtelte Arrays, Strukturen, Enums und optionale Werte.
Der Elementtyp muss Gleichheit unterstützen. Empfänger und Suchwert werden
in dieser Reihenfolge genau einmal ausgewertet; die Suche verändert das Array nicht.
`array.filter(predicate)` erwartet einen Funktionswert vom Typ
`func(T) -> Bool` für ein Array `[T]`. Es prüft die Elemente in Reihenfolge,
ruft das Prädikat je Element einmal auf und kopiert angenommene Werte in ein
eigenständiges `[T]`. Empfänger und Prädikat werden vorher je einmal
ausgewertet. Ein leeres Array ruft das Prädikat nicht auf. Das Eingabearray
bleibt auch dann als Wert-Snapshot erhalten, wenn das Prädikat eine andere
Kopie verändert. Fehler im Prädikat und beim Kopieren werden mit den üblichen
Quelldiagnosen und Ressourcenbereinigungen behandelt.
`array.contains(where: predicate)` und `array.allSatisfy(predicate)` erwarten
ebenfalls `func(T) -> Bool` und liefern `Bool`. `contains(where:)` liefert beim
ersten `true`, `allSatisfy` beim ersten `false` sofort das Ergebnis; sonst werden
die Elemente in Reihenfolge geprüft. Auf einem leeren Array ist `contains(where:)`
falsch und `allSatisfy` wahr.
Empfänger und Prädikat werden jeweils einmal vor der Suche ausgewertet. Der
Arraywert bleibt ein Snapshot, auch wenn das Prädikat eine andere Kopie ändert.
`array.map(transform)` erwartet einen Funktionswert `func(T) -> U` für ein
Array `[T]` und liefert ein eigenständiges `[U]`. `U` muss ein Werttyp sein;
`Void` ist kein gültiges Element. Transformation und Eingabearray werden je
einmal ausgewertet, anschließend wird die Funktion in Arrayreihenfolge je
Element einmal aufgerufen. Ein leeres Array ruft sie nicht auf. Besitzende
Ergebnisse wie Strings, Strukturen, Arrays und Funktionswerte werden in das
Ergebnis kopiert und ihre temporären Werte danach freigegeben. Änderungen an
anderen Kopien des Eingabearrays verändern den ausgewerteten Snapshot nicht.
Auch ein Enumfall-Konstruktor mit genau einem passenden Nutzdatenfeld kann als
Transformation verwendet werden.
`array.compactMap(transform)` erwartet `func(T) -> U?` und liefert ein
eigenständiges `[U]` aus den vorhandenen Ergebnissen in Eingabereihenfolge.
`nil`-Ergebnisse werden ausgelassen; auch ein leeres Array ruft die Funktion
nicht auf. Empfänger und Transformation werden je einmal ausgewertet, die
Transformation je Eingabeelement einmal. Besitzende optionale Rückgabewerte
werden nach der Übernahme ihres Elements freigegeben. Der Eingabewert bleibt
ein Snapshot, auch wenn die Funktion eine andere Kopie verändert. Fehler beim
Auswerten oder Kopieren geben das Teilergebnis frei.
`array.flatMap(transform)` erwartet `func(T) -> [U]` und hängt die Elemente
der zurückgegebenen Arrays in Eingabereihenfolge zu einem eigenständigen
`[U]` zusammen. Leere Teilarrays steuern keine Elemente bei. Empfänger und
Transformation werden je einmal ausgewertet, die Funktion je Eingabeelement
einmal; ein leeres Eingabearray ruft sie nicht auf. Jedes Teilarray und seine
besitzenden Elemente werden nach der Übernahme freigegeben. Fehler beim
Auswerten oder Kopieren räumen das Teilergebnis auf. Der Empfänger bleibt ein
Snapshot. Physims Sequenzvariante unterstützt derzeit Array-Rückgaben.
`array.reduce(initial, combine)` faltet `[T]` von links nach rechts mit einem
Startwert vom Typ `U` und einem Funktionswert `func(U, T) -> U`. Das Ergebnis
hat den Typ `U`; bei leerem Array ist es eine unabhängige Kopie des Startwerts.
Eingabearray, Startwert und Funktion werden einmal in dieser Reihenfolge
ausgewertet. Die Funktion wird je Element einmal aufgerufen. Besitzende
Zwischenwerte werden nach dem nächsten Schritt freigegeben, während der letzte
Wert an den Aufrufer übergeht. Der Eingabewert bleibt ein Snapshot, auch wenn
die Funktion eine andere Kopie des Arrays verändert.
`array.forEach(body)` ruft `func(T) -> Void` für jedes Element in
Arrayreihenfolge auf und liefert `Void`. Auf leeren Arrays wird `body` nicht
aufgerufen. Empfänger und Funktion werden je einmal ausgewertet; Änderungen
an der ursprünglichen Arrayvariablen während eines Aufrufs verändern den
Iterations-Snapshot nicht. Ein Fehler beendet die Iteration und räumt
temporäre Werte auf.
`array.isEmpty` liefert `true`, wenn das Array keine Elemente enthält.
`array.first` und `array.last` liefern das erste beziehungsweise letzte Element
als `T?`; bei einem leeren Array liefern beide `nil`. Ein vorhandenes Element
wird unabhängig kopiert, auch bei besitzenden Elementtypen. Empfänger werden
jeweils einmal ausgewertet und das Eingabearray bleibt unverändert.
`array.first(where: predicate)` prüft Elemente in Reihenfolge bis zum ersten
Treffer und liefert dessen unabhängige Kopie als `T?`. Ohne Treffer ist das
Ergebnis `nil`. Das Prädikat hat den Typ `func(T) -> Bool` und wird auf einem
leeren Array nicht aufgerufen.
`array.last(where: predicate)` sucht vom Ende aus und liefert das letzte
passende Element als unabhängige Kopie. `array.firstIndex(where: predicate)`
und `array.lastIndex(where: predicate)` liefern den ersten beziehungsweise
letzten passenden Index als `Int64?`. Alle drei Methoden beenden die Suche
beim ersten Treffer in ihrer Suchrichtung; ohne Treffer liefern sie `nil`.
`array.firstIndex(of: value)` verwendet dieselbe Suche und liefert den ersten
Index als `Int64?`. Ohne Treffer ist das Ergebnis `nil`, auch bei leeren Arrays.
`array.lastIndex(of: value)` sucht mit derselben strukturellen Gleichheit vom
Ende aus und liefert den letzten Index als `Int64?`; ohne Treffer `nil`.
Der optionale Ergebniswert besitzt seine eigene Lebensdauer; ein vorhandener
Index kann mit `if let` oder `unwrap()` gelesen werden.
`array.reversed()` liefert einen unabhängigen Array-Wert in umgekehrter
Elementreihenfolge. Die Methode funktioniert auch für verschachtelte und
besitzende Elementtypen; das Eingabearray bleibt unverändert. Kopierfehler
geben den bisher aufgebauten Ergebniswert vollständig frei.
`array.prefix(n)` und `array.suffix(n)` liefern die ersten beziehungsweise
letzten höchstens `n` Elemente. `array.dropFirst(n)` und `array.dropLast(n)`
lassen höchstens `n` Elemente am jeweiligen Ende weg; ohne Argument gilt `n = 1`.
`n` muss nichtnegativ sein. Größere Werte als `array.count` werden auf die
Arraylänge begrenzt. Anders als Swifts `ArraySlice` ist das Ergebnis in Physim
ein unabhängiger `[T]`-Wert mit Indexbeginn null. Besitzende Elemente werden
kopiert; ein Kopierfehler lässt Eingabe und vorhandene Snapshots unverändert.
`array.prefix(while: predicate)` liefert die zusammenhängenden Anfangselemente,
für die `func(T) -> Bool` wahr ist. Beim ersten `false` endet die Prüfung.
`array.drop(while: predicate)` lässt diese Anfangselemente weg und liefert den
Rest einschließlich des ersten nicht passenden Elements. Danach wird das
Prädikat nicht erneut aufgerufen. Empfänger und Prädikat werden je einmal
ausgewertet; ein leeres Array ruft das Prädikat nicht auf. Beide Methoden
liefern unabhängige `[T]`-Werte mit Indexbeginn null und erhalten den
Eingabesnapshot auch bei Seiteneffekten oder Kopierfehlern.
`array.split(separator: value, maxSplits: n, omittingEmptySubsequences: omit)`
zerlegt ein `[T]` mit `Equatable`-Elementen in `[[T]]`. `separator:` ist
erforderlich; `maxSplits:` ist optional und standardmäßig unbegrenzt,
`omittingEmptySubsequences:` ist optional und standardmäßig `true`.
Ausgelassene leere Teilarrays verbrauchen das Limit nicht. Ein negatives Limit
ist ein Laufzeitfehler. Alle Teilarrays sind unabhängige Werte mit Indexbeginn
null; Änderungen am Ursprungsarray beeinflussen sie nicht. Scheitert eine
Elementkopie, wird das Teilresultat vollständig freigegeben.
`array.starts(with: prefix)` prüft, ob ein `[T]` mit `Equatable`-Elementen mit
allen Elementen eines gleich typisierten Arrays beginnt. Ein leerer Präfix passt
immer; ein längerer Präfix passt nie. `array.elementsEqual(other)` prüft gleiche
Länge und paarweise gleiche Elemente. Beide Methoden werten Empfänger und
Argument genau einmal von links nach rechts aus und verändern die Werte nicht.
`Array(repeating: value, count: n)` liefert `n` Kopien eines Elements als neuen
`[T]`-Wert, auch für verschachtelte oder besitzende Elemente. Null ergibt
ein leeres Array desselben Typs. Ein negativer
`Int64`-Zähler ist ein Laufzeitfehler an der Aufrufstelle; Größenlimits werden
vor der Allokation geprüft. Scheitert eine Elementkopie, wird das bisherige
Ergebnis vollständig freigegeben; Eingabe und andere Snapshots bleiben gültig.
`array.reverse()` kehrt einen veränderlichen Array-Zugriffspfad um und liefert
`Void`. Der bisherige Wert bleibt bei einem Kopierfehler erhalten; andere
Kopien des Arrays ändern sich nicht. Auch ein indiziertes Strukturfeld kann
Empfänger sein. Empfänger und Indizes werden je einmal ausgewertet.
`array.sorted()` erstellt eine aufsteigend geordnete, eigenständige Kopie von
`[Int64]`, `[Float64]` oder `[String]`. Strings werden nach Unicode-Skalarwerten
verglichen; bei Zahlen ordnen gleiche Werte gleich, auch `-0.0` und `0.0`.
Nicht endliche `Float64`-Werte sind Laufzeitfehler. `array.sort()` veröffentlicht
dieselbe Ordnung in einem veränderlichen `var`-Zugriffspfad und liefert `Void`.
Beide Methoden kopieren alle Elemente vor dem Sortieren. Ein Speicherfehler lässt
Empfänger und vorhandene Snapshots unverändert; der zusätzliche Speicher zählt
zum Sprachspeicherbudget.
`array.sorted(by: compare)` und `array.sort(by: compare)` nehmen eine Funktion
`func(T, T) -> Bool` für beliebige Array-Elemente an. Das Ergebnis ist stabil:
Elemente, für die `compare` in beide Richtungen `false` liefert, behalten ihre
ursprüngliche Reihenfolge. Ein Aufruf des Prädikats für zwei Elemente soll `true`
liefern, wenn das erste vor dem zweiten stehen soll. Bei leerem oder einzelnem
Array wird es nicht aufgerufen. `compare` muss eine strenge schwache Ordnung
bilden. Die Sortierung benötigt zusätzlichen, im
Sprachbudget gezählten Speicher und hat `O(n log n)` Vergleiche. `sorted(by:)`
liefert einen unabhängigen Array-Wert; `sort(by:)` veröffentlicht den neuen Wert
erst nach erfolgreicher Sortierung. Scheitert ein Vergleich oder eine Allokation,
bleiben der mutierende Empfänger und ältere Snapshots erhalten. Sichtbare
Seiteneffekte des Prädikats werden dadurch nicht rückgängig gemacht.
`array.min()` und `array.max()` liefern das kleinste beziehungsweise größte
Element als `T?`, bei leerem Array `nil`. Ohne `by:` werden `[Int64]`,
`[Float64]` und `[String]` unterstützt; nicht endliche `Float64`-Werte sind
Laufzeitfehler. `array.min(by: compare)` und `array.max(by: compare)` akzeptieren
`func(T, T) -> Bool` für beliebige Elementtypen. Sie durchlaufen das Array in
`O(n)` und behalten bei gleichem Rang das erste Element. `compare` muss eine
strenge schwache Ordnung bilden. Das Ergebnis kopiert
das ausgewählte Element unabhängig, auch bei besitzenden Typen. Auf leeren
Arrays und Arrays mit einem Element wird `compare` nicht aufgerufen. Fehler
beim Vergleich oder Kopieren geben Teilwerte frei; das Eingabearray bleibt
unverändert.
`array.append(element)` erweitert einen veränderlichen Array-Wert direkt;
`array.remove(at: index)` entfernt ein Element und gibt es zurück. Diese Methoden
funktionieren auch über veränderliche Strukturfelder und indizierte Arraypfade.
`array.popLast()` entfernt das letzte Element und liefert `T?`; ein leeres Array
bleibt unverändert und liefert `nil`. Auch besitzende und optionale Elemente
bleiben als unabhängiger Ergebniswert gültig. Zum sicheren Entfernen an einem
beliebigen Index kann `attempt(array.remove(at: index))` verwendet werden.
`array.removeFirst()` und `array.removeLast()` entfernen und liefern das erste
beziehungsweise letzte Element als `T`. Ein leeres Array ist ein Laufzeitfehler;
`popLast()` bleibt die optionale Variante für das letzte Element. Die Formen
`array.removeFirst(n)` und `array.removeLast(n)` entfernen `n` Elemente und
liefern `Void`. `n` muss zwischen null und `array.count` liegen. Alle vier
Formen benötigen einen veränderlichen Zugriffspfad. Besitzende Rückgabewerte
werden unabhängig kopiert; bei Index-, Anzahl- oder Kopierfehlern wird keine
teilweise Mutation veröffentlicht. Vorhandene Snapshots bleiben gültig.
`array.swapAt(i, j)` vertauscht zwei Elemente eines veränderlichen Arraypfads
und liefert `Void`. Beide positionalen `Int64`-Indizes müssen vorhandene
Elemente bezeichnen; gleiche Indizes ändern nichts. Empfänger und Indizes
werden je einmal von links nach rechts ausgewertet. Das Array wird vor dem
Tausch unabhängig kopiert, damit Snapshots erhalten bleiben und ein
Kopierfehler den Empfänger unverändert lässt. Daher kostet die Operation hier
`O(n)` statt Swifts dokumentiertem `O(1)`.
`array.removeAll()` leert einen veränderlichen Arraypfad und liefert `Void`.
`array.removeAll(where: predicate)` entfernt alle Elemente, für die
`func(T) -> Bool` wahr ergibt, und erhält die Reihenfolge der übrigen Werte.
Das Prädikat wird auf jedem Element des ausgewerteten Array-Snapshots genau
einmal aufgerufen; bei leerem Array gar nicht. Die Auswahl benötigt einen
budgetierten temporären Puffer und kopiert besitzende Elemente unabhängig.
Erst nach erfolgreicher Auswahl und Kopie wird der neue Wert veröffentlicht.
Schlägt das Prädikat oder eine Allokation/Kopie fehl, bleibt der mutierende
Empfänger unverändert; sichtbare Seiteneffekte des Prädikats bleiben bestehen.
Für eine veränderte Kopie lässt sich `var copy = values` verwenden und danach
`copy.append(element)` oder `copy.remove(at: index)` aufrufen. `left + [element]`
erzeugt eine neue Array-Kopie mit angehängtem Element.
`array.insert(value, at: index)` fügt auf einem veränderlichen Arraypfad ein
Element vor `index` ein und liefert `Void`. Gültig sind Indizes von null bis
einschließlich `array.count`; negative oder größere Indizes sind Laufzeitfehler.
`array.append(contentsOf: values)` hängt alle Elemente eines `[T]`-Arrays an
einen veränderlichen `[T]`-Pfad an. `array.insert(contentsOf: values, at: index)`
fügt sie in derselben Reihenfolge vor einem gültigen Index ein. Empfänger,
Inhalt und danach Index werden je einmal ausgewertet. Auch Selbstanhang und
Selbsteinfügung verwenden einen festen Snapshot. Die neuen Elemente werden
gemäß Wertsemantik kopiert; bei einem Index- oder Kopierfehler bleibt der
Empfänger unverändert. Leere Eingabearrays sind erlaubt. Beide Methoden
liefern `Void`.
`left + right` verkettet zwei Arrays desselben `[T]`-Typs zu einem unabhängigen
Wert. `var values = [1, 2]; values += [3]` aktualisiert einen veränderlichen
Arraypfad. Auch `values[start..<end] += extra` ist erlaubt: Der Zielbereich
wird einmal als Wert gelesen, mit `extra` verkettet und als ganzer Bereich
ersetzt. Leere Literale erhalten ihren Elementtyp vom anderen Operanden oder
vom erwarteten Ergebnistyp; ohne einen solchen Typ bleibt `[] + []` ungültig.
Andere Array-Rechenoperatoren sind nicht definiert.
Die Argumente werden einmal von links nach rechts ausgewertet; ungültige
Einfüge- oder Entfernungsindizes sind Laufzeitfehler mit Quellposition.
Bereichsindizes erzeugen unabhängige Ausschnitte desselben Arraytyps:
`values[1..<4]` enthält die Elemente 1, 2 und 3, ebenso `values[1...3]`.
Der Ergebnisindex beginnt bei null. Grenzen müssen aufsteigend und innerhalb
des Arrays liegen; ein halboffener Bereich darf leer sein und an der Länge enden.
Ausschnitte sind unabhängige Werte und keine schreibbaren Zugriffspfade zum
Original. Auf einem veränderlichen Arraypfad ersetzt dagegen
`values[start..<end] = replacement` oder `values[start...end] = replacement`
den bezeichneten Bereich durch ein Array desselben Typs. Die Länge kann sich
dabei ändern. Ein leerer halboffener Bereich fügt Werte ein; ein leeres rechtes
Array entfernt den Bereich. Empfänger, Grenzen und Ersatzarray werden je einmal
in dieser Reihenfolge ausgewertet. Die Grenzen werden vor dem Ersatzarray
geprüft; der Empfänger-Snapshot wird erst nach erfolgreicher Ersetzung
veröffentlicht. Schlägt die Ersetzung fehl, wird der Empfänger-Snapshot nicht
veröffentlicht; Seiteneffekte der bereits ausgewerteten Ausdrücke bleiben bestehen.
`array.removeSubrange(start..<end)` entfernt den angegebenen Bereich und
liefert `Void`; `array.replaceSubrange(start..<end, with: values)` ersetzt ihn
durch ein gleich typisiertes Array. Auch geschlossene Bereiche mit `...` sind
zulässig. Die Grenzen müssen ausdrücklich angegeben werden; Bereiche mit
`by` sind für diese Methoden nicht zugelassen. Die Methoden verwenden dieselben
Grenz-, Besitz- und Fehlerregeln wie die Bereichszuweisung. Bei der Ersetzung
werden Empfänger, Grenzen und Ersatzarray genau einmal in dieser Reihenfolge
ausgewertet; ungültige Grenzen verhindern die Auswertung des Ersatzarrays.
Nur in Arrayindizes dürfen Grenzen fehlen: `values[..<end]` und
`values[...end]` beginnen bei null, `values[start..<]` und `values[start...]`
reichen bis zum Arrayende. `values[..<]` und `values[...]` bezeichnen das
gesamte Array. Ein fehlendes Ende ist effektiv exklusiv; deshalb sind auch
`empty[...]` und `empty[0...]` leer. Explizite geschlossene Enden müssen
weiterhin ein vorhandenes Element bezeichnen. Dieselben Formen sind als
Bereichszuweisungsziel erlaubt; in `for` und anderen Ausdrücken bleiben
Bereichsgrenzen verpflichtend.
Mit `by step` wählen Bereiche jeden `step`-ten Index, etwa
`values[1..<6 by 2]`. Die Schrittweite ist ein positiver `Int64`-Wert;
Null und negative Werte sind Laufzeitfehler. Die Kurzformen erlauben auch
`values[... by 2]`. Eine Zuweisung wie `values[... by 2] = replacement`
behält die Arraylänge bei und verlangt genau so viele Ersatzwerte wie
ausgewählte Indizes. `+=` auf einem Bereich mit Schrittweite ist nicht erlaubt.
`by` bleibt außerhalb dieser Bereichssyntax ein normaler Bezeichner.
Optionale Werte `T?` werden ebenfalls statisch geprüft und nativ ausgeführt;
der folgende Abschnitt beschreibt Konstruktion, Zugriff und Speichervertrag.
Die Prüfung begrenzt rekursive AST-Besuche auf 128 Ebenen und AST-Besuche sowie
Namensvergleiche auf vier Millionen Arbeitseinheiten einschließlich verglichener Bezeichnerbytes.
Diese internen Schutzgrenzen sind kein endgültiges Sprach- oder Produktlimit.
`language_checker` prüft Typen, Bindungsreferenzen, Gültigkeitsbereiche, Aufrufzuordnung,
Kontrollfluss, Diagnosen, Puffergrenzen, tiefe Ausdrucksbäume und Einzelbyte-Mutationen.
Eine erfolgreiche statische Prüfung allein führt keinen Code aus.

## Optionale Werte

Ein Typ `T?` enthält entweder einen Wert vom Typ `T` oder keinen Wert. `nil`
benötigt einen bekannten optionalen Zieltyp, etwa `let result: Float64? = nil`.
`Optional.some(value)` erzeugt ausdrücklich einen vorhandenen Wert; es gibt
keine implizite Umwandlung zwischen `T` und `T?`. Ein bekannter Zieltyp leitet
den Typ an das Argument weiter, beispielsweise bei numerischen Literalen.

```text
func positive(value: Float64) -> Float64?:
    if value > 0:
        return Optional.some(value)
    return nil

let measured = positive(2.5)
if measured.hasValue:
    let value = measured.unwrap()
let missing: Float64? = nil
let fallback = missing.valueOr(0) // Float64 0
```

`hasValue` ist eine schreibgeschützte `Bool`-Eigenschaft. `unwrap()` liefert
eine unabhängige Kopie des enthaltenen Wertes; auf `nil` bricht es mit einer
Quelldiagnose ab. `valueOr(fallback)` liefert Inhalt oder Ersatzwert. Empfänger
und Ersatzargument werden dabei immer einmal von links nach rechts ausgewertet,
auch wenn der Inhalt vorhanden ist.
Die Methoden akzeptieren bei ihrem einzelnen Argument auch `value:` bzw.
`fallback:`; `unwrap()` erwartet kein Argument.

`optional ?? ersatz` liefert ebenfalls einen Wert vom Inhaltstyp `T`. Die linke
Seite muss `T?` sein. Sie wird zuerst genau einmal ausgewertet; der Ersatz
wird nur bei `nil` ausgewertet und muss `T` liefern. Das Ergebnis ist eine
unabhängige Wertkopie, auch bei Strings, Arrays, Strukturen und verschachtelten
Optionalwerten. `??` bindet schwächer als `||` und gruppiert von rechts,
beispielsweise `a ?? b ?? 0` als `a ?? (b ?? 0)`. Zwei direkt benachbarte
Fragezeichen bilden den Operator; ein optionaler Typ `T??` behält seine
bisherige Bedeutung.

Vergleiche mit `nil` sind mit `==` und `!=` in beiden Reihenfolgen erlaubt.
Zwei optionale Werte desselben Typs können mit `==` und `!=` verglichen werden,
wenn ihr Inhalt vergleichbar ist. Zwei `nil`-Werte sind
gleich; `nil` und ein vorhandener Wert sind ungleich. Zwei vorhandene Werte
vergleichen ihren Inhalt mit der bereits festgelegten Gleichheit des Inhaltstyps.
Optionale SDK-Werte ohne festgelegte Gleichheit haben keine Inhaltsgleichheit.
Optionale Werte können
nicht direkt in Bedingungen oder arithmetischen Ausdrücken verwendet werden.

Alle Werttypen sind als Inhalt erlaubt: auch Strukturen, SDK-Werte und Arrays.
`[T?]` ist ein Array optionaler Elemente, `[T]?` ein optionales Array.
Verschachtelung bleibt erhalten: `T??` unterscheidet äußeres `nil`,
`Optional.some(nil)` mit innerem `nil` und einen zweifach verpackten Wert.
Bei einem nicht bestimmbaren Inhaltstyp ist eine Annotation erforderlich;
beispielsweise genügt `let x = Optional.some(nil)` nicht.

Optionale Werte verwenden die gemeinsame automatische Speicherverwaltung.
Kopien und Rückgaben bleiben unabhängig; der Inhalt kann nur durch Ersetzen
des gesamten optionalen Werts geändert werden. Das interne unveränderliche
Speicherobjekt wird beim Kopieren geteilt; `nil` benötigt keine Allokation,
`some` reserviert Speicher im gemeinsamen 64-MiB-Budget. Abbruch, Rückkehr,
`break` und `continue` räumen Zwischenwerte auf. Rekursive Strukturen über
optionale Felder sind endlich speicherbare Werte; es entstehen keine
veränderlichen Referenzzyklen. Gespeicherte Analysehandles behalten ihre
bisherige Gültigkeitsdauer, unabhängig von der Lebensdauer der Optionalhülle.

### Bedingte Bindung mit if und while

`if let value = optional:` führt den Block nur bei vorhandenem Inhalt aus und
bindet eine unabhängige Kopie als lokale Konstante. `if var` erzeugt stattdessen
eine veränderliche lokale Kopie. Eine optionale Typannotation beschreibt den
**Inhaltstyp**, beispielsweise `if let value: Float64 = Optional.some(1):`.
Die rechte Seite muss einen optionalen Wert liefern und wird genau einmal
ausgewertet. Die Bindung gilt nur im erfolgreichen Block, nicht in `else` oder
nach dem `if`. Ihr Name darf einen äußeren Namen verdecken; die rechte Seite
sieht dabei noch den äußeren Namen. Derselbe Name darf im Block nicht erneut
deklariert werden.

```text
let measurement: Float64? = Optional.some(2.5)
if let value = measurement:
    print(value)
else:
    print("Keine Messung")
```

`while let value = next():` beziehungsweise `while var` wertet den Ausdruck
vor jedem Durchlauf erneut aus. `nil` beendet die Schleife. Der gebundene Wert
wird am Ende jedes Durchlaufs freigegeben, auch bei `continue`, `break`,
`return` und Laufzeitfehlern. Änderungen an der Quelle oder der lokalen Kopie
verändern den jeweils anderen Wert nicht. Aus `T??` wird genau eine Ebene
ausgepackt: Ein vorhandener äußerer Wert mit innerem `nil` betritt den Block.
Pro Kopf ist eine Bindung oder eine Bool-Bedingung zulässig; weitere Prüfungen
lassen sich verschachteln. `else if let` ist ebenfalls erlaubt.

`guard bedingung else:` prüft eine `Bool`-Bedingung oder bindet mit
`guard let`/`guard var` einen optionalen Inhalt. Der `else`-Block muss auf
jedem Pfad aus der umgebenden Funktion zurückkehren; `break` oder `continue`
dürfen ihn nicht verlassen. `guard` ist deshalb nur in Funktionen zulässig.
Bei Erfolg bleibt eine optionale Bindung bis zum Ende des umgebenden Blocks
sichtbar. Sie ist eine unabhängige Kopie und darf mit `guard let value = value`
den optionalen Namen im selben Block durch den Inhalt ersetzen. Die rechte
Seite sieht dabei noch den optionalen Wert; im `else`-Block ist die neue
Bindung noch nicht sichtbar. Besitzende Werte werden bei Rückkehr, Fehlern
und am Blockende freigegeben.

```text
func first(values: [String]?) -> String:
    guard let values = values else:
        return "Keine Werte"
    return values[0]
```

## Vektorrechnung


`Vec2`, `Vec3` und `Vec4` unterstützen Addition und Subtraktion mit einem Vektor gleicher
Dimension, unäres `+`/`-`, Multiplikation mit einem `Float64` auf beiden Seiten
und Division durch einen `Float64`. Ganzzahlliterale erhalten dabei den
Fließkommakontext; bereits ganzzahlig typisierte Variablen benötigen `Float64(n)`.
Alle Komponenten des Ergebnisses müssen endlich sein. Division durch null und
Überlauf führen zu einem Laufzeitfehler mit der Quellposition des Operators.
Division berechnet jede Komponente direkt, ohne einen möglicherweise
überlaufenden Kehrwert des Divisors zu bilden.

```text
let acceleration = Vec3(0, -9.80665, 0)
var velocity = Vec3(2, 5, 0)
var position = Vec3(-2, 0, 0)
let dt = 0.01
position = position + velocity * dt + 0.5 * acceleration * dt * dt
velocity = velocity + acceleration * dt
let kineticEnergy = 0.5 * velocity.dot(velocity)
```

`==` prüft alle Komponenten exakt; `!=` ist dessen Negation. Für numerische
Toleranzvergleiche lässt sich zum Beispiel `(a - b).length() < tolerance` verwenden.
Ordnung, komponentenweise Multiplikation und Division zweier Vektoren sind nicht
definiert. `dot` berechnet das Skalarprodukt, `cross` bei `Vec2` die orientierte
Fläche und bei `Vec3` das rechtshändige Kreuzprodukt. `length` und
`normalized` verwenden die skalierten SDK-Verfahren, die unnötigen
Überlauf beim Quadrieren großer Komponenten vermeiden. Ein Nullvektor wird zum
Nullvektor normalisiert; eine nicht darstellbare Länge ist ein Laufzeitfehler.

`Vec4(x, y, z, w)` hat vier endliche `Float64`-Komponenten mit Wertsemantik.
Er ist ein eigener Typ und kann nicht implizit in `Quat` umgewandelt werden.
Das mitgelieferte `examples/language/phase_space.phys` nutzt die Komponenten
für zwei Positionen und zwei Geschwindigkeiten. Es integriert einen
zweidimensionalen harmonischen Oszillator mit RK4 und prüft nach 1.000 Schritten
Bewegung und Energie gegen die analytische Referenz. Die Komponenteneinheiten
legt hier das Modell fest; Vektoren selbst tragen keine SI-Dimensionen.

## Matrizen und Koordinatensysteme

`Mat3` und `Mat4` sind unabhängige Werte mit 9 beziehungsweise 16
`Float64`-Komponenten. Sie sind in allen drei Ausführungsarten verfügbar,
auch als Strukturfelder, Arrayelemente, Parameter und Rückgabewerte.
Eine Matrix wird aus **Spalten** konstruiert und wirkt auf Spaltenvektoren.
`a.multiplied(b)` wendet zuerst `b`, dann `a` an. Keine Operation verändert
ihren Empfänger; eine veränderliche Variable kann durch das Ergebnis ersetzt werden.

| Aufruf | Vertrag |
| --- | --- |
| `Mat3(column0, column1, column2)` | Drei `Vec3`-Spalten |
| `Mat4(column0, column1, column2, column3)` | Vier `Vec4`-Spalten |
| `Mat3.identity()`, `Mat4.identity()` | Einheitsmatrix |
| `matrix.element(row, column)` | `Float64`, nullbasierte Indizes innerhalb der Dimension |
| `matrix.multiplied(right)` | Produkt mit einer Matrix desselben Typs |
| `matrix.transposed()` | Vertauschte Zeilen und Spalten |
| `matrix.applied(vector)` | `Mat3` × `Vec3` oder `Mat4` × `Vec4`, ohne homogene Division |
| `matrix.inverse(pivotTolerance)` | Inverse; null wählt Standardtoleranz, sonst `0 < t < 1` |
| `Mat4.translation(translation)`, `Mat4.scale(scale)` | Translation bzw. Skalierung aus `Vec3` |
| `Mat4.rotation(rotation)` | Rechtshändige aktive Rotation aus einer intern normierten `Quat` |
| `Mat4.trs(translation, rotation, scale)` | Skalierung, dann Rotation, dann Translation |
| `matrix.transformPoint(point)` | `Vec3`-Punkt mit w=1, anschließend Division durch Ergebnis-w |
| `matrix.transformDirection(direction)` | `Vec3`-Richtung ohne Translation oder Normalisierung |
| `matrix.transformNormal(normal)` | Normierte inverse-transponierte Flächennormale |

Die letzten drei Methoden gehören zu `Mat4`. Richtungen und Normalen benötigen
eine affine letzte Zeile `[0,0,0,1]` exakt. Punkte dürfen auch projektiv
transformiert werden; Ergebnis-w gleich null ist ein Laufzeitfehler.
Nullskalierung ist als Vorwärtsabbildung erlaubt, aber nicht invertierbar.
Negative Skalen sind erlaubt. Eine Nullquaternion oder Nullnormale wird abgewiesen.
Alle Matrixkomponenten und Ergebnisse müssen endlich sein.

```text
let rotation = Quat.axisAngle(Vec3(0,0,1),1.5707963267948966)
let frame = Mat4.trs(Vec3(3,-2,5),rotation,Vec3(2,3,4))
let point = frame.transformPoint(Vec3(1,2,3))       // (-3,0,17)
let direction = frame.transformDirection(Vec3(1,2,3)) // (-6,2,12)
let local = frame.inverse(0).transformPoint(point) // (1,2,3)
let normal = frame.transformNormal(Vec3(1,1,0))    // (-2,3,0)/sqrt(13)
```

Insbesondere bei nichtuniformer Skalierung ist eine Normale anders zu behandeln
als eine Richtung, damit sie senkrecht zur transformierten Fläche bleibt.
Matrixwerte tragen keine Einheiten; bei der Anbindung an `Body` und Szene sind
Positionen und Translationen in Metern, Skalen dimensionslos. Eine skalierende
Darstellung ändert weder Körpermasse noch Trägheit oder Kontaktgeometrie.

Die Inversion verwendet die gemeinsame C-Bibliothek mit skalierter Pivotwahl.
Die Standardtoleranz ist die Dimension mal Maschinengenauigkeit. Ein durch die
Toleranz abgewiesener Pivot, ungültige Indizes oder numerischer Überlauf erzeugen
eine Laufzeitdiagnose an der `.phys`-Quelle. Die Pivotprüfung ist keine
Konditionsschätzung; empfindliche Modelle brauchen eigene Restfehlerprüfungen.
Es gibt keine automatische Pseudoinverse und keine elementweise Mutation.

Das ausführbare Beispiel `examples/language/coordinate_frames.phys` prüft
Matrixprodukte, Inversion, affine und projektive Koordinaten, Normalen,
gedrehte Trägheitstensoren sowie Kopien in verschachtelten Arrays. Das
Körperexperiment `spinning_body.phys` verwendet `Mat4.trs` für den Kraftangriffspunkt
in Weltkoordinaten und wird weiterhin gegen die C-Mechanik geprüft.

## Explizite Wertkonvertierung

`Float64(wert)` und `Int64(wert)` erwarten genau ein unbenanntes Argument.
Sie akzeptieren beide Zahlentypen sowie `String` und erhalten bei gleichem
Quell- und Zieltyp den Wert unverändert. `String(wert)` akzeptiert `Bool`,
`Int64`, `Float64` und `String`. Boolesche Werte, Enums und Strukturen werden
nicht in Zahlen umgewandelt. Eigene Funktionen oder lokale Bindungen
können diese Aufrufnamen wie andere eingebaute Funktionen verdecken.

```text
let count: Int64 = 100
let duration = 2.0
let dt = duration / Float64(count)
let index = Int64(3.9)  // 3
let negative = Int64(-3.9)  // -3
```

Das Argument wird einmal ausgewertet und erhält seinen eigenen Typ, unabhängig
vom Konvertierungsziel. `Float64(1 / 2)` konvertiert daher das ganzzahlige Ergebnis
0; `Float64(1) / 2` ergibt 0,5. Auch innerhalb von `Float64(...)` muss ein
Ganzzahlliteral im Int64-Bereich liegen; größere Zahlen brauchen ein
Fließkommaliteral wie `9223372036854775808.0`.

`Int64(Float64)` schneidet Nachkommastellen in Richtung null ab. Nicht endliche
Werte und Werte außerhalb von [-2^63, 2^63) führen vor einer C-Konvertierung zu
einem Laufzeitfehler mit Quellposition. `Float64(Int64)` rundet zur nächsten
Binary64-Zahl, bei Gleichstand zur geraden Signifikandenzahl. Bis einschließlich
±2^53 sind alle ganzen Zahlen exakt darstellbar; darüber können Bits verloren
gehen. Insbesondere wird `Float64(9223372036854775807)` zu 2^63 gerundet;
die Rückkonvertierung ist deshalb ein Bereichsfehler.

`Int64(String)` liest ASCII-Dezimalziffern mit optionalem `+` oder `-`.
Leerzeichen, Dezimalpunkt, Exponent und Zahlen außerhalb des Int64-Bereichs
erzeugen eine Laufzeitdiagnose. `Float64(String)` akzeptiert ein optionales
Vorzeichen, Dezimalpunkt und Exponent; mindestens eine Ziffer ist erforderlich.
`NaN`, Unendlich, Leerzeichen und zusätzliche Zeichen sind ungültig. Nicht
endliche Ergebnisse sind Bereichsfehler, Unterlauf zu null bleibt zulässig.
Wie Float64-Literale verwendet die Umwandlung einen festen C-Dezimalpunkt,
unabhängig von der Prozesslocale.
`Int64.parse(text: text)` und `Float64.parse(text: text)` verwenden dieselbe
Syntax und liefern bei gültigem Text `Int64?` beziehungsweise `Float64?` mit
einem Wert. Ungültige Schreibweisen und Zahlen außerhalb des Zielbereichs
liefern `nil`, sodass Programme Eingaben ohne Laufzeitabbruch prüfen können.
Der Argumentname `text:` kann entfallen. Fehler beim Anlegen des optionalen
Ergebniswerts bleiben Laufzeitfehler. Beispielsweise erlaubt
`guard let count = Int64.parse(input) else:` eine frühe Rückkehr bei ungültiger
Eingabe. Beide Methoden werten das Stringargument einmal aus.
`String(Int64)` schreibt den vollständigen Dezimalwert, `String(Bool)` `true`
oder `false`. `String(Float64)` verwendet 17 signifikante Dezimalziffern,
damit endliche Binary64-Werte beim Rücklesen erhalten bleiben; `-0.0` wird
als `"-0"` geschrieben. Auch `print(Float64)` verwendet einen Dezimalpunkt.
`String(String)` teilt den unveränderlichen Wert.

Das SDK-Beispiel `examples/language/sampling.phys` integriert die Geschwindigkeit
eines Wurfs auf einem ganzzahlig indizierten Zeitraster und prüft das Ergebnis
gegen die analytische Verschiebung. Analyseprogramme können beispielsweise mit
`Float64(s.count())` eigene Mittelwerte berechnen.

## Laufzeitfehler lokal behandeln

`attempt(ausdruck)` akzeptiert genau einen positionsgebundenen Ausdruck mit
einem Werttyp `T` und liefert `T?`. Bei erfolgreicher Auswertung enthält das
Optional eine unabhängige Kopie des Werts. Löst der Ausdruck selbst oder ein
von ihm aufgerufener Funktionskörper einen Laufzeitfehler aus, liefert
`attempt` stattdessen `nil` und schreibt für diesen abgefangenen Fehler keine
Diagnose. Typ- und Syntaxfehler werden weiterhin vor der Ausführung abgewiesen.
Ein `Void`-Ausdruck ist nicht zulässig. Eine eigene Funktion namens `attempt`
verdeckt den eingebauten Aufruf.

```text
func quotient(divisor: Int64) -> Int64:
    guard let value = attempt(100 / divisor) else:
        return 0
    return value
```

Der Ausdruck wird einmal ausgewertet. Bei einem Fehler räumt die Laufzeit die
während dieser Auswertung registrierten temporären Werte auf und setzt die
Rekursionstiefe zurück; ältere Werte im umgebenden Bereich bleiben gültig.
Änderungen an bereits veröffentlichten Variablen, Messkanälen oder Ausgaben
werden nicht zurückgenommen. Fehler beim Erzeugen oder Kopieren des optionalen
Ergebniswerts liegen außerhalb des abgefangenen Ausdrucks und bleiben normale
Laufzeitfehler. `attempt` liefert nur Erfolg oder `nil`, keine Fehlerursache;
die weitergehende Fehlerweitergabe bleibt Teil des offenen Sprachvertrags.

## Enums

Enums deklarieren auf Modulebene einen eigenen nominalen Werttyp. Jeder Fall
steht auf einer eigenen eingerückten Zeile:

```text
enum FlightPhase:
    case rising
    case falling
    case landed

func phase(speed: Float64) -> FlightPhase:
    if speed > 0:
        return FlightPhase.rising
    return FlightPhase.falling

var current = phase(5)
assert(current == FlightPhase.rising)
current = FlightPhase.landed
```

Fälle werden über den Typnamen ausgewählt. Enumwerte lassen sich kopieren, als
Strukturfelder speichern sowie an Funktionen übergeben und zurückgeben.
Typannotationen dürfen auf später deklarierte Enums verweisen. `==` und `!=`
verlangen denselben Enumtyp; andere Enumtypen, Zahlen und arithmetische Operationen
sind nicht kompatibel. Es gibt keine implizite Zahlenkonvertierung. Enums ohne
Raw Values haben keine zugesicherte numerische Fallreihenfolge.

Mindestens ein Fall ist erforderlich; doppelte Fallnamen sind Fehler.
Enums können Instanzmethoden, `mutating func` und `static func` wie Strukturen
deklarieren. `self` hat den Enumtyp und kann in einer mutierenden Methode durch
einen anderen Fall ersetzt werden. Generische Methoden und Methoden auf
importierten Enumtypen sind möglich. Methoden dürfen keine Fallnamen oder
eingebauten Raw-Value-Mitglieder überdecken; `init` bleibt Structs vorbehalten.
Verschachtelte Typdeklarationen sind noch nicht umgesetzt. Das native Beispiel
`examples/language/flight_phases.phys` prüft die Phasen eines Vakuumwurfs.

Ein Enum ohne Nutzdaten kann `Int64`-Raw-Values deklarieren:

```text
enum Status:
    case ready = -2
    case waiting
    case done = 5

assert(Status.waiting.rawValue == -1)
let found: Status? = Status.fromRawValue(5)
assert(found.unwrap() == Status.done)
assert(Status.fromRawValue(4) == nil)
```

Sobald ein Fall einen expliziten Wert erhält, beginnen vorherige implizite
Werte bei null. Nach einem expliziten Wert zählt jeder folgende implizite Fall
um eins weiter. Ein weiterer expliziter Wert setzt die Folge neu. Nur ganzzahlige
Literale mit optionalem Vorzeichen sind erlaubt; doppelte Werte, Werte außerhalb
des `Int64`-Bereichs und Überlauf beim Weiterzählen sind Fehler.
`rawValue` liest den Wert eines Enumwerts. `Type.fromRawValue(rawValue: zahl)`
akzeptiert auch ein positionsgebundenes Argument und liefert `nil`, wenn kein
Fall diesen Wert trägt. Raw Values und Nutzdatenfelder können nicht im selben
Enum kombiniert werden. Die Fallnamen `rawValue` und `fromRawValue` sind bei
Raw-Value-Enums reserviert.

Ein Fall darf benannte Nutzdatenfelder beliebiger unterstützter Werttypen tragen.
Arrays, Optionals und Strukturen behalten dabei ihre unabhängige Wertsemantik:

```text
enum Reading:
    case missing
    case sample(value: Float64, valid: Bool)

let reading = Reading.sample(value: 2.5, valid: true)

enum Batch:
    case numbers(items: [Int64])
    case nested(child: Batch?)
```

Konstruktoren nehmen positionsgebundene oder benannte Argumente an. Bei benannten
Argumenten ist ihre Reihenfolge frei. Fälle ohne Nutzdaten bleiben Werte ohne
Klammern. Ein Fall mit Nutzdaten braucht alle deklarierten Argumente.
Sein Konstruktor kann als Funktionswert gespeichert und weitergegeben werden:
`let make: func(Float64, Bool) -> Reading = Reading.sample`. Ein solcher Funktionswert
verwendet positionsgebundene Argumente und besitzt den erzeugten Enumwert nach
denselben Kopierregeln wie ein direkter Konstruktoraufruf. Das gilt auch für
importierte und spezialisierte generische Enums; bei `Choice.some` ohne
ausdrückliche Typargumente muss der erwartete Funktionstyp das Ergebnis
festlegen.
Enumwerte mit Nutzdaten lassen sich kopieren, in Arrays und Strukturen speichern,
an Funktionen übergeben und zurückgeben. Besitzende Felder werden beim Kopieren
des aktiven Falls unabhängig geklont und beim Verlassen des Gültigkeitsbereichs
freigegeben. Direkte rekursive Wertfelder sind unzulässig; über `Optional`
vermittelte Rekursion ist erlaubt. `==` und `!=` vergleichen zuerst den Fall
und danach jedes Feld des aktiven Falls. Ein Enum ist vergleichbar, wenn alle
seine Feldtypen vergleichbar sind. Das gilt auch für verschachtelte Arrays,
Optionals und Strukturen. Ein Vergleich mit `nil` bei einem optionalen Enumwert
ist unabhängig vom Inhaltstyp zulässig.

### Zustände mit switch auswählen

```text
func phaseName(value: FlightPhase) -> String:
    switch value:
        case FlightPhase.rising, FlightPhase.falling:
            return "moving"
        case FlightPhase.landed:
            return "landed"
```

`switch` akzeptiert Enumwerte, optionale Werte, `Bool`, `Int64`, `Float64` und
`String` und wertet den Ausdruck
genau einmal aus. Bei Enums nennt jeder `case` einen oder mehrere Fälle desselben
Typs. Bei skalaren Werten sind nur feste Muster zulässig: `true`/`false`,
vorzeichenbehaftete `Int64`- und `Float64`-Literale oder Stringliterale. Für
`Int64` und `Float64` sind auch geschlossene Bereiche (`2...4`) und Bereiche
mit offener Obergrenze (`5..<8`) aus zwei festen numerischen Literalen des
jeweiligen Typs zulässig. Direkte `String`-Fälle erlauben entsprechend
`"a"..."z"` und `"a"..<"z"` aus zwei Stringliteralen. Verglichen werden
die decodierten UTF-8-Inhalte lexikografisch nach Unicode-Skalaren; auch
mehrskalare Strings sind erlaubt. Leere Bereiche und Überschneidungen
mit unbewachten früheren Mustern sind Fehler. Mehrere Muster teilen
sich einen Zweigkörper. Variablen oder Funktionsaufrufe als Fallmuster sind
Fehler. Ein Enum braucht alle Fälle ohne Guard oder ein abschließendes `default:`.
Für `Bool` genügen `true` und `false` ohne Guard; `Int64`, `Float64` und `String` benötigen
immer `default:`. Jeder Zweig braucht einen nichtleeren eingerückten Block und
hat einen eigenen Namensraum.

`Float64`-Fälle vergleichen den endlichen binären Wert exakt. Ganzzahlliterale
erhalten dabei den `Float64`-Kontext; verschiedene Schreibweisen desselben
Werts sind doppelte Fälle. Bereichsgrenzen werden als `Float64` gerundet und
mit `>=` sowie `<` oder `<=` verglichen. `-0.0` und `0.0` gelten dabei als
derselbe Wert. Nicht endliche Grenzen sind Fehler.

Bei `T?` prüfen `case nil` und `case Optional.some` die beiden Möglichkeiten.
`case Optional.some(value)` bindet zusätzlich eine unabhängige Kopie des
Inhalts vom Typ `T` und muss allein in seinem Zweig stehen. Der gebundene Name
ist im Guard und im Zweigkörper sichtbar. Ohne `default:` müssen `nil` und
`some` jeweils durch einen unbewachten Zweig abgedeckt sein; nach einem
bewachten Muster darf derselbe Fall erneut folgen. Die gebundene Kopie wird
auch bei falschem Guard, `break`, Rückgabe oder Laufzeitfehler freigegeben.
`case Optional.some(_)` prüft nur die Anwesenheit und kopiert den Inhalt nicht;
`_` führt keine neue Bindung ein.
Für `Bool`, `Int64`, `Float64` und `String` kann `some` stattdessen ein Literal
prüfen, etwa `Optional.some(5)` oder `Optional.some("ready")`. Ein solches
Teilmuster zählt nicht als Abdeckung aller vorhandenen Werte.
Für `Int64?`, `Float64?` und `String?` kann `some` auch einen Bereich prüfen, etwa
`Optional.some(1...5)`, `Optional.some(1.5..<2.5)` oder
`Optional.some("a"..<"m")`. Überlappen Teilmuster nur teilweise, gilt die
Reihenfolge der Zweige; ein späteres vollständig verdecktes Muster ist ein Fehler.
Ist `T` selbst optional, prüft `case Optional.some(nil)` einen vorhandenen
äußeren Wert mit leerem innerem Wert. `case nil` prüft dagegen den leeren
äußeren Wert. Verschachtelte optionale Werte lassen sich mit weiteren
`Optional.some`-Mustern prüfen, etwa `case Optional.some(Optional.some(value))`.
Auf jeder Ebene sind `nil`, `Optional.some`, `Optional.some(_)`, Bindungen und
passende feste Skalarmuster möglich. Bindungen gelten im Guard und Zweigkörper
und werden auch bei einem falschen Guard freigegeben. Ein verschachteltes
Teilmuster kann zusammen mit weiteren unbewachten Mustern die vollständige
Fallabdeckung bilden: Für jede optionale Ebene müssen `nil` und alle `some`-Werte
erfasst sein. Bei einem inneren `Bool` genügen dazu `true` und `false`; für
`Int64`, `Float64` und `String` ist eine Bindung oder ein umfassendes
`Optional.some`-Muster nötig. Bewachte Muster zählen nicht zur Abdeckung.

```text
switch measurement:
    case nil:
        print("missing")
    case Optional.some(value) if value > 0:
        print("positive")
    case Optional.some:
        print("other")
```

```text
switch resultCode:
    case -3...-1, 0:
        print("retry")
    default:
        print("done")
```

Bei einem Fall mit Nutzdaten bindet ein Muster die Felder in Deklarationsreihenfolge:

```text
switch reading:
    case Reading.missing:
        print("missing")
    case Reading.sample(value, valid):
        if valid:
            print(value)
```

Die Bindungen gelten nur im Zweig. Ein Fall mit Nutzdaten darf auch ohne
Klammern als reines Tagmuster stehen. Ein Muster mit Klammern steht allein
in seinem Zweig; jedes Feld benötigt einen Namen, `_` oder ein passendes
Skalarliteral. Ein Literal vergleicht den Feldwert, ohne ihn zu binden.
Für ein optionales Feld kann an dieser Stelle `nil` stehen.
Ein `Int64`-, `Float64`- oder `String`-Feld darf auch ein Bereichsmuster wie
`Reading.sample("a"..<"m", valid)` verwenden. Die Bereichsgrenzen sind feste
Literale; ein solches Muster deckt
den Enumfall nicht vollständig ab. Optionale Enumfelder akzeptieren dieselben
verschachtelten `Optional.some`-Muster; innere Bindungen erhalten eine eigene
Kopie des gebundenen Werts.
Bei `Float64` erkennt die Prüfung gleiche Werte auch bei unterschiedlicher
Dezimalschreibweise; nicht endliche Musterliterale sind Fehler.
Teilmuster wie `Reading.sample(5, valid)` benötigen für vollständige Abdeckung
einen späteren unbewachten Falltag, ein umfassendes Bindungsmuster oder `default:`.
Mehrere unbewachte Nutzdatenmuster können einen Enumfall auch gemeinsam vollständig
abdecken. Bei `Bool` müssen `true` und `false` vorkommen; bei optionalen Feldern
werden `nil` und alle `Optional.some`-Werte betrachtet, auch verschachtelt.
Dies gilt ebenso für Kombinationen mehrerer Felder. Für ein unbegrenztes Feld
wie `String`, `Int64` oder `Float64` muss jeder vollständig abgedeckte Zweig
das Feld binden oder mit `_` verwerfen; einzelne Literale und Bereiche genügen
dafür nicht. Bewachte Muster zählen nicht zur vollständigen Abdeckung.
Ein `_` verwirft das jeweilige Nutzdatenfeld. Es darf mehrfach im selben
Muster stehen und erzeugt weder eine lokale Variable noch eine Kopie des
verworfenen Werts. Andere Feldnamen bleiben im Guard und Zweigkörper verfügbar.

Ein `if` nach dem Fallmuster ist ein `Bool`-Guard. Er wird nur ausgewertet,
wenn der Falltag passt, und darf die gebundenen Felder verwenden:

```text
switch reading:
    case Reading.sample(value, valid) if valid && value > 0:
        print(value)
    case Reading.sample:
        print("invalid")
    default:
        print("other")
```

Ein falscher Guard lässt die Auswahl beim nächsten Zweig fortfahren. Deshalb
darf ein Falltag oder Skalarwert nach einem bewachten Zweig erneut vorkommen.
Innerhalb eines Zweigs sind doppelte Muster Fehler; nach einem unbewachten
Zweig ist derselbe Wert in späteren Zweigen unerreichbar und ebenfalls ein
Fehler. Stringmuster vergleichen den Inhalt einschließlich decodierter
Escape-Zeichen und normalisierter Zeilenumbrüche. Ein einzeiliges Literal und
ein dreifach zitiertes Literal mit demselben Wert sind deshalb doppelte Muster.
Ein Guard zählt nicht als vollständige Abdeckung. Der
`switch`-Ausdruck wird genau einmal ausgewertet; ein erreichter Guard ebenfalls
genau einmal. Temporäre Kopien der gebundenen Felder werden auch bei falschem
Guard freigegeben.

Am Ende des passenden Zweigs endet der `switch` automatisch. `break` beendet
vorzeitig den innersten `switch` oder die innerste Schleife; `continue` bezieht
sich weiterhin auf die innerste Schleife und benötigt eine solche. Für die
Rückgabeprüfung zählt ein vollständiger `switch`, dessen Zweige alle zurückgeben.
Ein `break`, der diesen `switch` verlassen kann, verhindert konservativ diese
Rückgabegarantie. Es gibt kein implizites Weiterlaufen zum nächsten Fall.

## Eigene Strukturen und Wertsemantik

Strukturen auf Modulebene verwenden dieselbe Einrückungssyntax wie Funktionen:

```text
struct Position:
    var x: Float64
    var y: Float64

struct Particle:
    let id: Int64
    var position: Position

var original = Particle(id: 1, position: Position(0, 2))
var copy = original
copy.position.x = 5
assert(original.position.x == 0.0)
```

Jedes Feld hat einen expliziten Typ und kann mit `= Ausdruck` einen Vorgabewert
erhalten. Ohne Vorgabe bleibt das Feld beim Konstruktoraufruf erforderlich.
Positionsargumente entsprechen der Deklarationsreihenfolge; nur nachfolgende
Felder mit Vorgabe dürfen ausgelassen werden. Benannte Argumente dürfen in
beliebiger Reihenfolge stehen und jedes Feld mit Vorgabe auslassen. Zuerst werden
alle angegebenen Argumente in Quellreihenfolge ausgewertet, danach die Vorgaben
für ausgelassene Felder in Feldreihenfolge. Jede Vorgabe wird pro Aufruf neu
ausgewertet und sieht Modulnamen, aber weder `self` noch lokale Namen des
Aufrufers oder andere Felder. Besitzende Vorgaben wie Arrays werden unabhängig
kopiert und bei Laufzeitfehlern freigegeben. Bei generischen Strukturen müssen
Typparameter weiterhin aus angegebenen Feldern, dem Ergebniskontext oder
expliziten Typargumenten ableitbar sein. Rekursive Vorgaben, die beim Konstruktoraufruf erneut eine
ausgelassene Vorgabe benötigen, werden bereits bei `--check` abgewiesen.
Zwei verschiedene Strukturdeklarationen sind
verschiedene nominale Typen, auch wenn ihre Felder identisch aussehen.

Ein Struct kann seinen öffentlichen Aufruf `Typ(...)` mit einer oder mehreren
`static func init(...) -> Typ` selbst definieren. Dann gelten die Parameter und
die Rückgabe dieser Funktion; Feldnamen sind keine Konstruktorparameter mehr.
Innerhalb dieser `init`-Funktion baut `Typ(...)` den Wert weiterhin direkt aus
Feldern und deren Vorgaben auf. `Typ.init(...)` ruft die Funktion ausdrücklich
auf, auch innerhalb ihres Rumpfs. Der Initialisierer muss auf jedem Pfad einen
Wert seines Struct-Typs zurückgeben. Er kann besitzende Werte zurückgeben und
funktioniert auch für importierte und generische Structs; bei generischen
Structs werden Typargumente aus den `init`-Parametern, dem Ergebniskontext oder
expliziten Typargumenten abgeleitet. Überladungen werden nach Parameteranzahl,
bei benannten Aufrufen nach Parameternamen und zusätzlich nach Parametertypen
aufgelöst. Das gilt auch für generische Structs mit expliziten oder ableitbaren
Typargumenten und für importierte Structs. Gleiche Parameternamen sind zulässig, sofern
sich mindestens ein Parametertyp unterscheidet; dieselbe vollständige Signatur
ist unzulässig. Ein exakt passender Typ hat Vorrang. Ein Ganzzahlliteral kann
auch einen `Float64`-Parameter wählen, wenn keine bessere `Int64`-Überladung
passt. `nil` passt bei der Überladungswahl nur zu einem optionalen Parameter;
ein Arrayliteral passt nur zu einem Arrayparameter. Dessen Elemente werden
gegen den Elementtyp geprüft, auch wenn das Literal leer ist. Zwei passende
optionale beziehungsweise Arraytypen bleiben bei `nil` oder `[]` mehrdeutig.
`Optional.some(value)` wird bei der Überladungswahl gegen den Inhalt eines
optionalen Parameters geprüft. Das gilt auch für berechnete skalare Werte und
für einen aus dem Inhalt ableitbaren generischen Typ. Mehrere gleich gut
passende Optional-Parameter bleiben mehrdeutig. Bleibt nur eine generische
Überladung mit noch unbekanntem Typ übrig, kann die anschließende Prüfung des
vollständigen Arguments den Typparameter bestimmen.
Ein bei der Überladungswahl bestimmter generischer Typ dient auch als Kontext
für die vollständige Prüfung verschachtelter `T?`-, `[T]`- und generischer
Struct-Argumente. Daher können etwa `Optional.some(1)`, `[1]` und `Box(1)`
einem bereits bestimmten `Float64`-Elementtyp folgen.
Generische Typparameter können aus nichtleeren Arrayliteralen und späteren
Argumenten abgeleitet werden; ein leeres Array oder `nil` allein bestimmt
keinen neuen Typparameter. Steht der spezialisierte Ergebnistyp bereits fest,
werden die Parameter dieser Spezialisierung als Kontext verwendet; etwa kann
ein ganzzahliges Arrayliteral einen `[Float64]`-Parameter füllen. Bei
generischer Typableitung sind `[T]`, `T?` und
verschachtelte generische Strukturtypen spezifischer als ein bloßes `T`.
Bei mehrfach verwendeten skalaren Typparametern gibt ein späteres
`Float64`-Argument auch früheren Ganzzahlliteralen den passenden Kontext;
die Reihenfolge benannter Argumente ändert das Ergebnis nicht. Eine
`Int64`-Variable wird dabei nicht zu `Float64` umgewandelt.
Eine Spezialisierung, die zwei gleiche Initialisierersignaturen erzeugt, ist
ein Compilerfehler. `init` kann eigene Typparameter und die bestehenden
Constraints deklarieren, beispielsweise
`static func init<U: Scalar>(value: U) -> Typ`. Sie werden aus den
Aufrufargumenten abgeleitet oder bei einem
nichtgenerischen Struct mit `Typ.init<U>(...)` beziehungsweise `Typ<U>(...)`
angegeben. Bei einem generischen Struct gehören die Typargumente von
`Typ<T>(...)` zum Struct; `Typ<T>.init<U>(...)` gibt zusätzlich den Methodentyp
an. Der Struct-Typ kann auch bei einem generischen `init` aus einem Argument
abgeleitet werden. Überladungen mit konkreten und generischen Parametern sowie
verschiedenen Containerformen sind zulässig; identische Signaturen nach
Umbenennung der Methodentypparameter sind unzulässig. Der Rückgabetyp muss
weiterhin der eigene Struct-Typ sein. Die Auflösung weiterer Ausdrücke ohne
vorab bekannten Typ bleibt offen.

`Typ.init` kann als Funktionswert gespeichert, weitergegeben und indirekt
aufgerufen werden. Bei mehreren Initialisierern wählt der erwartete
`func(...) -> Typ`-Typ anhand der Parameter- und Ergebnistypen; fehlt er, ist
der Wert mehrdeutig. Haben zwei Initialisierer dieselben Typen und
unterscheiden sich nur durch Parameternamen, bleibt der Funktionswert
mehrdeutig, weil Funktionswertaufrufe positionale Argumente verwenden.
Generische Structs benötigen vor dem Zugriff ihre Typargumente, etwa
`Box<Int64>.init`. Generische Initialisierermethoden benötigen eigene
Typargumente, etwa `Typ.init<Float64>`.
Bei mehreren generischen `init`-Methoden mit denselben Typargumenten
entscheidet der erwartete Funktionstyp auch zwischen verschiedenen
Parameterformen wie `T?` und `[T]`.

```text
struct Limited:
    let value: Int64
    static func init(input: Int64) -> Limited:
        if input < 0:
            return Limited(0)
        return Limited(input)

assert(Limited(input: -4).value == 0)
let factory: func(Int64) -> Limited = Limited.init
assert(factory(-4).value == 0)
```

Felder dürfen andere Strukturen enthalten; Vorwärtsreferenzen sind zulässig.
Direkte oder indirekte Zyklen durch Wertfelder werden wegen unendlicher Größe
abgewiesen. Ein konservativ berechnetes Layoutbudget von 1 MiB pro Struktur
(einschließlich möglichem Padding) begrenzt auch exponentiell wachsende, zyklenfreie
Typgraphen. Die bestehende Verschachtelungs-/Arbeitsgrenze gilt ebenfalls.
Diese Compilergrenzen ersetzen keine künftige Laufzeit-Speicherbudgetierung.

Zuweisungen sowie Funktionsparameter und Rückgaben kopieren den gesamten Wert.
Das C17-Backend erzeugt echte C-Strukturen in Abhängigkeitsreihenfolge; es verwendet
keine versteckten Referenzen auf veränderliche Strukturdaten. Stringfelder halten
ihren unveränderlichen Wert selbst, auch wenn er zur Laufzeit verkettet wurde.
Feldzugriff funktioniert auch auf zurückgegebenen Werten, etwa `advance(state).position.x`.

Eine Feldzuweisung verlangt eine `var`-Variable als Wurzel und `var` an jedem Feld
des Zugriffspfads. `let`-Variablen, Parameter, `let`-Felder und temporäre Ergebnisse
sind nicht veränderlich. Eine gesamte `var`-Struktur darf durch einen neuen Wert
ersetzt werden, auch wenn sie `let`-Felder enthält. Ein noch nicht initialisierter
globaler Strukturwert ist auch bei Feldzugriffen ein Laufzeitfehler.
`==` und `!=` vergleichen Strukturen Feld für Feld, sofern jeder Feldtyp
vergleichbar ist. Arrays vergleichen Länge und Elemente in Reihenfolge.
Arithmetische Operatoren und direktes `print` ganzer Strukturen sind noch nicht
unterstützt; skalare Felder können ausgegeben werden.

Methoden werden mit `func` innerhalb der Struktur deklariert. `self` bezeichnet
den Empfänger; Feldzugriffe verwenden ausdrücklich `self.feld`.
Methoden können weitere Methoden aufrufen und Werte zurückgeben:

```text
struct Distance:
    let metres: Float64
    func scaled(factor: Float64) -> Distance:
        return Distance(self.metres * factor)
    func value() -> Float64:
        return self.metres

let distance = Distance(2).scaled(factor: 3)
assert(distance.value() == 6)
```

Der Empfänger wird vor den Argumenten einmal ausgewertet und als Wert übergeben.
Lesende Methoden können auf `let`-Werten, temporären Ergebnissen und Arrayelementen laufen.
Auch besitzende Arrayfelder bleiben bei Kopie und Rückgabe unabhängig; lokale
Besitzer werden vor Rückgabe und Fehlerabbruch freigegeben. `var copy = self`
erzeugt eine veränderliche lokale Kopie, die eine Methode zurückgeben kann.
Eine gewöhnliche `func` darf ihren Empfänger nicht ändern. `mutating func`
erlaubt Änderungen an `var`-Feldern sowie das Ersetzen des gesamten `self`:

```text
struct Counter:
    var value: Int64
    mutating func increment() -> Int64:
        self.value += 1
        return self.value

var counter = Counter(0)
assert(counter.increment() == 1)
assert(counter.value == 1)
```

Mutierende Methoden verlangen am Aufrufort einen vollständig veränderlichen
Zugriffspfad, etwa `group.particles[index].advance(dt)`. `let`-Werte,
unveränderliche Parameter, Ausschnitte und temporäre Rückgaben werden abgewiesen.
Einzelne `let`-Felder bleiben unveränderlich; `self = Type(...)` ersetzt dagegen
den gesamten Empfänger, auch wenn dieser `let`-Felder besitzt.

Empfänger und Pfadindizes werden einmal vor den Argumenten ausgewertet. Die
Methode arbeitet auf einer Wertkopie und übernimmt diese bei erfolgreicher
Rückkehr in den aufrufenden Zugriffspfad. Das gilt für frühe Rückgaben,
`Void`-Methoden und Rückgaben besitzender Werte. Verschachtelte mutierende
Methoden ändern zunächst die lokale Empfängerkopie der äußeren Methode.
Bei einem Laufzeitfehler wird die Methodenänderung nicht veröffentlicht.
Andere Seiteneffekte wie explizite globale Zuweisungen oder ausgegebene Messwerte
werden nicht rückgängig gemacht. Ändert ein Argument oder die Methode den
gleichen globalen Wurzelwert zusätzlich, überschreibt die abschließende
Veröffentlichung ihn mit dem aufgebauten Snapshot, wie bei Arraymethoden und
indizierten Zuweisungen.

Benannte Argumente, Vorwärtsaufrufe und Rekursion funktionieren wie bei freien
Funktionen. Der Laufzeitschutz begrenzt die gemeinsame Funktionstiefe auf 256.

Methodennamen gehören zum jeweiligen Typ und sind keine globalen Funktionen.
Zwei Typen dürfen denselben Methodennamen verwenden. Innerhalb eines Typs sind
doppelte Methodennamen und Kollisionen mit Feldnamen Fehler; Überladung ist noch
nicht vorgesehen. `self` darf nicht als Parameter oder lokale Variable neu
deklariert werden. Eine Struktur kann ausschließlich Methoden enthalten und wird
dann mit `Type()` konstruiert. Mit `static func` deklarierte Methoden werden
am Typ aufgerufen und besitzen kein `self`:

```swift
struct Particle:
    var position: Vec3
    var velocity: Vec3
    static func atRest(position: Vec3) -> Particle:
        return Particle(position, Vec3(0, 0, 0))
let particle = Particle.atRest(Vec3(1, 2, 3))
```

Statische Methoden unterstützen benannte Argumente, Vorwärtsaufrufe, Rekursion
und Rückgaben besitzender Werte. Der Aufruf über einen Wert statt über den Typ
wird abgewiesen; Instanzmethoden benötigen umgekehrt einen Wert als Empfänger.

`examples/language/motion.phys` berechnet einen Wurf mit konstanter Beschleunigung
über eigene `Vector2`-/`Motion`-Werte und prüft ihn gegen die analytische Lösung.
Es ist ein eigenständiges Sprachprogramm; die Runner-/Szenenanbindung ist noch offen.

## Compiler-Kommandozeile

Der Build erzeugt `bin/physimc` (Windows: `physimc.exe`); das Werkzeug wird auch
in das SDK installiert. Die Dateiendung der Sprachbeispiele ist `.phys`.

```powershell
python tools/build.py --no-app --config Release
.\build\native\Release\bin\physimc.exe --check examples/language/energy.phys
```

Unter Linux/macOS `python3` und `./build/native/Release/bin/physimc` verwenden.
In einem entpackten SDK ist der Compiler bereits unter `bin/physimc` vorhanden
(Windows: `bin/physimc.exe`); dort entfällt der Repository-Build.

`--check` liest eine Datei binär, prüft Syntax und die oben unterstützte Semantik
und führt nichts aus. Diagnosen haben das Format `Datei:Zeile:Spalte: error: Meldung`.
Der Rückgabecode ist 0 bei Erfolg, 1 bei Sprachfehlern und 2 bei Aufruf-, Datei-,
Speicher- oder Quelldateigrößenfehlern. `--help` und `--version` sind verfügbar.
Die CLI begrenzt Eingaben auf 1 MiB und Syntaxbäume auf 65536 Knotenslots;
Parser- und Prüfungsbudgets gelten zusätzlich. UTF-8-Pfade mit Leerzeichen und
Umlauten, Syntax-/Typdiagnosen, Rückgabecodes, leere Dateien und zu große Eingaben
werden von der Prüfung `language_cli` im direkten Testkatalog geprüft.

`physimc --emit-c quelle.phys` prüft dieselben Regeln und schreibt C17 auf stdout.
Bei Syntax-/Typfehlern wird kein C ausgegeben. Ein Ausgabefehler kann eine unvollständige
Datei hinterlassen; nur Rückgabecode 0 erlaubt das Weiterkompilieren. Die Ausgabe
benötigt den experimentellen SDK-Header `physim/language_runtime.h` und ist auf
32 MiB begrenzt. C-Bezeichner werden aus AST-IDs erzeugt; Zeichenketten und Quellpfade
werden byteweise maskiert und nicht als C-Code übernommen.
Vor Funktionsdefinitionen, Strukturfeldern und Anweisungen schreibt das Backend
`#line`-Zuordnungen zur `.phys`-Datei. Native Compilerfehler verweisen damit
auf die Sprachquelle; `PSRT_AT` liefert zusätzlich die genaue
Spalte bei Laufzeitfehlern. Ein absichtlich ausgelöster C-Compilerfehler wird
im nativen Sprachtest an der ursprünglichen `.phys`-Datei nachgewiesen.

Der direkte Repository-Build mit `--examples` übersetzt alle 15 eigenständigen
Sprachbeispiele sowie 27 Experiment-/Analysemodule. Generiertes C liegt ausschließlich
im Buildordner unter `examples/` und wird erst nach erfolgreicher Sprachprüfung
ersetzt. Unveränderte Quellen und Programme bleiben erhalten. Sprachfehler brechen
mit der ursprünglichen Quelldiagnose ab; der letzte vollständige C-Code und das
zugehörige Programm bleiben verfügbar. Nutzerprojekte in der App verwenden F5
und ihre automatisch gepflegte `physim.project`.

Windows, aus dem Repository-Verzeichnis:

```powershell
python tools/build.py --no-app --config Release --examples
.\build\native\Release\bin\language-energy.exe
```

Linux und macOS:

```sh
python3 tools/build.py --no-app --config Release --examples
./build/native/Release/bin/language-energy
```

Das Beispiel gibt `9`, `1` und `45` aus und prüft diese Ergebnisse mit `assert`.
Diese Beispiele benötigen weder SDL noch CMake; Python ab 3.10 und ein C17-Compiler
genügen. Die Programme heißen `language-<Beispiel>`, Module tragen zusätzlich
`.dll` unter Windows beziehungsweise `.so` unter Linux/macOS.

## Laufzeitvertrag des skalaren C17-Backends

- Der Moduleinstieg führt Anweisungen in Quellreihenfolge aus. Funktionsdefinitionen
  registrieren Funktionen, rufen sie aber nicht automatisch auf; auch eine Funktion
  namens `main` muss ausdrücklich aufgerufen werden.
- Operanden und Aufrufargumente werden strikt von links nach rechts ausgewertet.
  Benannte Argumente ändern nur die Zuordnung zu Parametern, nicht die Reihenfolge
  möglicher Seiteneffekte. `&&` und `||` werten rechts nur bei Bedarf aus.
- `Int64` verwendet geprüfte Addition, Subtraktion, Multiplikation und Negation.
  Überlauf ist ein Laufzeitfehler. Division rundet gegen null; der Rest hat das
  Vorzeichen des Dividenden. Division/Rest durch null sind Fehler. `INT64_MIN / -1`
  ist Überlauf; `INT64_MIN % -1` ergibt ohne C-Überlauf null.
- `Float64` verlangt IEEE-754-Binary64. Nicht endliche Literale oder Ergebnisse
  und Division durch ±0 sind Laufzeitfehler. Unterlauf zu subnormalen Zahlen oder
  null ist zulässig. Literale werden unabhängig von der Prozesslocale mit
  festem C-Dezimalpunkt gelesen. Der direkte Build
  schaltet Fast-Math/FMA-Kontraktion aus beziehungsweise verwendet `/fp:strict`.
- Bereiche werten ihre Grenzen genau einmal aus und steigen ohne Angabe einer
  Schrittweite in Schritten von eins. `for i in start..<end by step` und die
  geschlossene Form verwenden einen von null verschiedenen `Int64`-Schritt:
  positive Werte steigen, negative Werte fallen. Grenzen und Schritt werden
  einmal vor dem ersten Durchlauf ausgewertet. Passt die Richtung nicht zu
  den Grenzen, gibt es keine Iteration. Ein nächster Wert außerhalb des
  `Int64`-Bereichs beendet die Schleife ohne Überlauf; einschließlich dürfen
  deshalb `INT64_MAX` und `INT64_MIN` enthalten sein. `continue` führt zum
  nächsten Wert. Array- und String-Slices mit `by` verlangen weiterhin einen
  positiven Indexschritt. `while` wertet seine Bedingung vor jeder Iteration neu aus.
- Strings sind unveränderliche, geteilte UTF-8-Werte mit begrenztem Speicherbudget.
  Literale und `+` erzeugen besitzende Werte; Kopien in Variablen, Strukturen,
  Arrays und Optionals halten sie unabhängig vom ursprünglichen Ausdruck am Leben.
  `Unit`- und `Quantity`-Symbole werden bis zum Modulende festgehalten, weil die
  C-SDK-Werte ihren Symboltext nur als Zeiger speichern. Gleiche Symbole teilen
  diesen gehaltenen Wert.
- Fehler melden ursprünglichen Quellpfad, Zeile und Spalte und beenden das
  eigenständige Programm mit Status 70. Die Funktionsaufruftiefe ist auf 256 begrenzt.
  Das ist keine Betriebssystem-Sandbox und ersetzt keine künftigen Runnerbudgets.

`language_native` erzeugt, kompiliert und startet echte Programme. Positive
Referenzen prüfen Auswertungsreihenfolge, benannte Argumente mit Seiteneffekten,
Kurzschlusslogik, Rekursion, Schleifen/Int64-Grenzen, Strings, Schattenvariablen
und Fließkommarechnung. Negative Programme prüfen die Laufzeitdiagnosen und
Status 70. Der direkte Beispielbuild wird auch nach einem absichtlich eingebauten
Typfehler auf Buildabbruch und Erhalt des vorherigen generierten C geprüft.
Zusätzliche Strukturreferenzen prüfen nominale Typen, verschachtelte Kopien,
Feldmutabilität, Übergabe/Rückgabe, Konstruktorreihenfolge, zyklische und zu große
Layouts sowie Mutationen einer Strukturquelle. Vollständige Experiment-/Analyse-API,
weitere Werttypen bleiben offen.

## Experimentmodule und gemeinsame Bibliothek

In der App unter **Datei → Neues Projekt** eine der Vorlagen **Pendel · Physim-Sprache**
oder **Wurf mit Luftwiderstand · Physim-Sprache** auswählen. Das Projekt enthält `main.phys`
und je nach **Sprache der Auswertung** `analysis.c` oder `analysis.phys`.
Die Auswertungssprache lässt sich beim Anlegen unabhängig von der Experimentvorlage
wählen. Die erste Sprach-Auswertung erwartet den Kanal `position.x` und bildet
dessen Ableitung. Der vorhandene Buildknopf führt Typprüfung,
C17-Übersetzung und nativen Build aus. Compilerdiagnosen lassen sich im Protokoll
anklicken; sie verweisen auf `main.phys`. Der Experimenteditor färbt die
Sprachschlüsselwörter ein und fügt mit Tab vier Leerzeichen ein. Blöcke bleiben
mit `:` und Einrückung definiert. Speichern, Backups und Autosave gelten für
beide Quellen. Eine automatische Übersetzung bestehender C-Projekte gibt es nicht.

Der direkte Testkatalog enthält die App-Abläufe `language_full_workflow` und
`language_mixed_workflow`. Sie prüfen ein
reines Sprachprojekt beziehungsweise ein C-Experiment mit Sprach-Auswertung:
absichtlicher Compilerfehler mit Quelldiagnose, Korrektur und nativer Build,
Run/Pause/Step/Stop, Analysebericht mit Exporten, Quellsnapshots und erneutes
Öffnen. Beide laufen mit `--test-display`, verwenden ein kleines
Fenster und legen neue Projektordner mit Leerzeichen und Umlaut an. Die
Testordner samt Screenshots bleiben zur Untersuchung erhalten.

```powershell
python tools/build.py --test-display --test-filter language_full_workflow --test-filter language_mixed_workflow
```

`physim.project` wählt über `experiment=main.phys` die Sprache; `experiment=main.c`
bleibt unterstützt. Andere Experimentdateinamen werden derzeit abgewiesen.
Der mitgelieferte `physim-build` liest dieselbe Angabe. Jeder gestartete Lauf archiviert
den Originalquelltext als `.experiment.phys`; Batchläufe verwenden `experiment.phys`.
Simulation, Datenbibliothek und die bestehende C-Auswertung verarbeiten beide
Sprachen über dieselbe ABI. Für die Auswertung wählt `analysis=analysis.phys` in
der Projektbeschreibung die eigene Sprache. Analysequellen werden als
`.source.phys` archiviert. Beide Editoren unterstützen Syntaxfarben, Quellfehler,
Speichern/Backups, Autosave und Tab als vier Leerzeichen für Sprachdateien.

`physimc --emit-experiment quelle.phys` erzeugt C17 für die bestehende Experiment-ABI.
Die App baut daraus über `physim-build` ein natives Modul; Compiler, Header und
gemeinsame C-Bibliothek stammen aus dem SDK. Die mitgelieferten Sprachmodule
lassen sich im Repository außerdem mit `python3 tools/build.py --no-app --examples`
bauen (Windows: `python`).
Die vier erforderlichen Funktionen sind:

```text
func create():
    metadata("model=example")

func reset():
    return

func step(dt: Float64):
    return

func scene():
    return
```

Globale Initialisierungen laufen einmal pro Instanz vor `create`. Danach ruft der
Adapter automatisch `reset` auf. Weitere Resets führen nur den Resetkörper aus;
er muss veränderlichen Modellzustand ausdrücklich zurücksetzen. Jede Instanz hat
eigene globale Werte und Kanalhandles. Parallelzugriff auf dieselbe Instanz ist
nicht vorgesehen. Ein Modul darf auch mehrere voneinander unabhängige Instanzen haben.

Die erste Bibliotheksanbindung umfasst folgende Funktionen. Alle Argumente sind
entweder positional oder vollständig benannt; benannte Argumente dürfen in anderer
Reihenfolge stehen und werden weiterhin in Quellreihenfolge ausgewertet.

| Aufruf | Ergebnis / Bedeutung |
| --- | --- |
| `Vec2(x, y)`, `Vec3(x, y, z)`, `Vec4(x, y, z, w)` | Werttypen mit `Float64`-Feldern; veränderliche Felder erfordern einen `var`-Zugriffspfad |
| `Quat(x, y, z, w)` | Quaternion-Wert mit vier endlichen `Float64`-Feldern, mit Kopiersemantik wie Vektoren |
| `Quat.axisAngle(axis, angle)` | Quaternion einer rechtshändigen Rotation um eine `Vec3`-Achse, Winkel in Radiant; Nullachse ergibt Identität |
| `rotation.rotate(vector)` | Rotiert einen `Vec3` mit intern normalisiertem `Quat`; Nullquaternion ist ein Fehler |
| `rotation.normalized()` | Quaternion auf Einheitslänge bringen; endliche große und kleine Komponenten werden skaliert, Nullquaternion ist ein Fehler |
| `value.conjugated()` | Negiert x/y/z und erhält w; ohne Normalisierung |
| `left.multiplied(right)` | Hamilton-Produkt ohne Normalisierung; bei Rotationen wird zuerst `right`, dann `left` angewendet |
| `start.slerp(end, fraction)` | Kürzeste sphärische Rotationsinterpolation mit normalisierten Endpunkten, `fraction` in [0, 1] |
| `left.dot(right)` | Skalarprodukt von zwei `Vec2`, `Vec3` oder `Vec4` derselben Dimension, Ergebnis `Float64` |
| `left.cross(right)` | Bei `Vec2` orientierte Fläche (`Float64`), bei `Vec3` Kreuzprodukt (`Vec3`) |
| `vector.length()` | Euklidische Länge eines `Vec2`, `Vec3` oder `Vec4` als `Float64` |
| `vector.normalized()` | Normalisierter Vektor derselben Dimension; Nullvektor bleibt null |
| `Bezier3(start, control1, control2, end)` | Kopierbarer Kurvenwert aus vier endlichen `Vec3`-Kontrollpunkten |
| `curve.position(t)`, `curve.tangent(t)` | De-Casteljau-Position und Ableitung für dimensionsloses `t` in [0, 1]; die Tangente ist keine normierte Richtung oder Geschwindigkeit |
| `curve.splitLeft(t)`, `curve.splitRight(t)` | Geometrisch exakte Teilkurven, jeweils auf einen eigenen Parameterbereich [0, 1] abgebildet |
| `springForce(position, velocity, anchor, anchorVelocity, stiffness, restLength, damping)` | Axiale Hooke-Feder mit viskoser axialer Dämpfung; Kraft auf den ersten Endpunkt als `Vec3` |
| `stokesDrag(relativeVelocity, viscosity, radius)` | Stokes-Widerstand einer Kugel als `Vec3` |
| `quadraticDrag(relativeVelocity, density, radius, coefficient)` | Quadratischer Kugelwiderstand als `Vec3`, projizierte Fläche πr² |
| `Medium(density, viscosity)` | Homogenes Medium mit Dichte in kg/m³ und dynamischer Viskosität in Pa·s; beide Werte endlich und nichtnegativ |
| `Medium.air()`, `Medium.water()`, `Medium.vacuum()` | Vordefinierte C-Bibliothekswerte für Luft bei 15 °C, Wasser bei 20 °C und Vakuum |
| `medium.density`, `medium.viscosity` | Lesbare `Float64`-Eigenschaften mit den obigen SI-Einheiten |
| `medium.dragForce(velocity, coefficient, area)` | Quadratische Widerstandskraft als `Vec3` entgegen der Relativgeschwindigkeit in m/s; dimensionsloser Koeffizient und Fläche in m² müssen nichtnegativ sein |
| `medium.stokesDrag(velocity, radius)` | Linearer Stokes-Widerstand einer Kugel als `Vec3` mit der Viskosität des Mediums; Radius in Metern muss positiv sein |
| `buoyancyForce(density, volume, gravity)` | Archimedische Auftriebskraft als `Vec3`, Gewichtskraft separat addieren |
| `Submersion.sphere(radius, centerHeight)` | Schnitt einer Kugel mit einer ebenen Flüssigkeitsoberfläche; der Flüssigkeitsraum liegt auf der negativen Seite der nach außen zeigenden Oberflächennormale |
| `submersion.volume`, `submersion.centroidOffset` | Unveränderliche `Float64`-Werte für verdrängtes Volumen in m³ und Schwerpunktabstand entlang der Oberflächennormale in m |
| `sin(angle)`, `cos(angle)`, `sqrt(value)`, `abs(value)` | `Float64`; nicht endliche Ergebnisse sind Laufzeitfehler |
| `tan(angle)`, `asin(value)`, `acos(value)`, `atan(value)`, `atan2(y, x)` | Trigonometrie in Radiant; `asin`/`acos` verlangen Werte in [-1, 1], `atan2` mindestens eine von null verschiedene Koordinate |
| `exp(value)`, `log(value)`, `log10(value)`, `pow(base, exponent)` | Exponential- und Logarithmusfunktionen; ungültige Bereiche und nicht endliche Ergebnisse sind Laufzeitfehler |
| `hypot(x, y)` | Skalierte euklidische Länge aus zwei `Float64`-Komponenten, ohne unnötigen Zwischenüberlauf |
| `linearSolve(coefficients, rhs, pivotTolerance)` | Löst ein lineares System mit 1–32 Unbekannten; `coefficients` ist ein zeilenweise abgelegtes `[Float64]` mit `n*n` Werten, `rhs` enthält `n` Werte, das Ergebnis ist ein unabhängiges `[Float64]` |
| `eulerStep(derivative, state, time, dt)`, `rk4Step(derivative, state, time, dt)` | Integrieren 1–32 Zustandswerte mit einem Ableitungswert vom Typ `func(Float64, [Float64]) -> [Float64]` durch die gemeinsame C-Bibliothek; das Ergebnis ist ein unabhängiges Array |
| `rk45Integrate(derivative, state, start, end, absoluteTolerance, relativeTolerance, maxSteps)` | Adaptiver Dormand–Prince-Integrator für 1–32 Zustandswerte; Vorwärts- und Rückwärtsintegration mit endlichen Grenzen, positiver absoluter Toleranz, relativer Toleranz in `[0, 1)` und positivem Schrittbudget |
| `rk45IntegrateWithSteps(derivative, state, start, end, absoluteTolerance, relativeTolerance, maxSteps, initialStep, minimumStep, maximumStep)` | Wie `rk45Integrate`, mit expliziten positiven Schrittweiten; `maximumStep` muss mindestens `minimumStep` betragen |
| `rk45IntegrateWithTolerances(derivative, state, start, end, absoluteTolerances, relativeTolerance, maxSteps, initialStep, minimumStep, maximumStep)` | Wie die konfigurierte RK45-Integration, mit einem positiven absoluten Toleranzwert je Zustandskomponente statt eines gemeinsamen Werts |
| `rk45IntegrateReported(derivative, state, start, end, absoluteTolerance, relativeTolerance, maxSteps, initialStep, minimumStep, maximumStep)` | Wie `rk45IntegrateWithSteps`, gibt aber `OdeResult` mit dem Zustand und dem numerischen Bericht zurück |
| `rk45IntegrateWithTolerancesReported(derivative, state, start, end, absoluteTolerances, relativeTolerance, maxSteps, initialStep, minimumStep, maximumStep)` | Verbindet komponentenweise absolute Toleranzen mit dem `OdeResult`-Bericht |
| `verletStep(acceleration, phase, time, dt)` | Velocity Verlet für 1–32 Freiheitsgrade; `phase` enthält zuerst alle Positionen, danach gleich viele Geschwindigkeiten. Der Beschleunigungswert vom Typ `func(Float64, [Float64]) -> [Float64]` erhält `(time, position)` und gibt ein gleich langes `[Float64]` zurück. Das Ergebnis ist ein eigenes Array in derselben Reihenfolge. `dt` darf positiv oder negativ, aber nicht null sein. |
| `rootBisect(function, lower, upper, absoluteTolerance, relativeTolerance, maxIterations)` | Nullstelle mit einem Wert vom Typ `func(Float64) -> Float64` im Intervall mit der gemeinsamen Bisektionsroutine |
| `minimizeGolden(function, lower, upper, absoluteTolerance, relativeTolerance, maxIterations)` | Minimum mit einem Wert vom Typ `func(Float64) -> Float64` und dem gemeinsamen Goldener-Schnitt-Verfahren |
| `rootBisectReported(function, lower, upper, absoluteTolerance, relativeTolerance, maxIterations)`, `minimizeGoldenReported(function, lower, upper, absoluteTolerance, relativeTolerance, maxIterations)` | Wie die skalaren Suchfunktionen, liefern aber `ScalarResult` mit `x`, `value`, `lower`, `upper`, `iterations` und `evaluations` |
| `floor(value)`, `ceil(value)`, `round(value)` | Abrundung, Aufrundung und Rundung auf die nächste ganze `Float64`-Zahl; `round` rundet bei Gleichstand von null weg |
| `min(left, right)`, `max(left, right)` | Kleinerer beziehungsweise größerer `Float64`-Wert; bei Gleichheit bleibt der linke Wert einschließlich seines Nullvorzeichens erhalten |
| `clamp(value, lower, upper)` | Begrenzt einen `Float64`-Wert auf das inklusive Intervall; `lower > upper` ist ein Laufzeitfehler mit Quellposition |
| `Int64.abs(value)`, `Int64.min(left, right)`, `Int64.max(left, right)`, `Int64.clamp(value, lower, upper)` | Ganze Zahlen bleiben `Int64`; `abs` von `-9223372036854775808` und vertauschte Clamp-Grenzen sind Laufzeitfehler mit Quellposition |
| `value.isMultiple(of: divisor)`, `value.signum()` | Swift-benannte `Int64`-Methoden: Vielfachheit einschließlich Divisor 0 und `Int64.min` mit −1; Vorzeichen als −1, 0 oder 1 ohne Überlauf |
| `state.symplectic(acceleration, dt)` | `Vec2`: Position in `x`, Geschwindigkeit in `y`; gemeinsamer symplektischer Euler, `dt > 0` |
| `Unit(length, mass, time, current, temperature, amount, luminosity, scale, symbol)` | Opaquer `Unit`-Wert mit sieben ganzzahligen SI-Exponenten, positivem Maßstab und Stringsymbol |
| `from.convert(value, to)` | `Float64`; statisch bekannte inkompatible Dimensionen sind Compilerfehler, dynamische Laufzeitfehler |
| `unit.isCompatible(right)` | `Bool`; vergleicht die sieben SI-Dimensionen unabhängig von Maßstab und Symbol |
| `unit.multiplied(right, symbol)` | Neue `Unit`: Dimensionsexponenten addieren, Maßstäbe multiplizieren |
| `unit.divided(right, symbol)` | Neue `Unit`: Dimensionsexponenten subtrahieren, Maßstäbe dividieren |
| `unit.powered(exponent, symbol)` | Neue `Unit` für eine ganzzahlige Potenz; negative Exponenten und null sind erlaubt |
| `Quantity(value, unit)` | Endlicher `Float64`-Wert mit Einheit; `.value: Float64` und `.unit: Unit` sind lesbar |
| `quantity.converted(to)` | Neue `Quantity` in der Zieleinheit, Dimensionen müssen kompatibel sein |
| `quantity.adding(right)`, `quantity.subtracting(right)` | Rechnet den rechten Wert in die linke Einheit um und gibt die Summe/Differenz in dieser Einheit zurück |
| `quantity + right`, `quantity - right` | Wie `adding`/`subtracting` für zwei `Quantity`-Werte; `+=` und `-=` aktualisieren eine veränderliche Bindung |
| `quantity * scalar`, `scalar * quantity`, `quantity / scalar` | Skaliert den `Float64`-Messwert und erhält die Einheit; `*=` und `/=` aktualisieren eine veränderliche Bindung |
| `+quantity`, `-quantity` | Identität beziehungsweise Vorzeichenwechsel bei unveränderter Einheit |
| `quantity.multiplied(right, symbol)`, `quantity.divided(right, symbol)` | Multipliziert/dividiert Werte und Einheiten; explizites Symbol für das Ergebnis |
| `Channel(name, unit, description)` | Opaquer `Channel`-Handle; nur während Initialisierung/`create`, eindeutige Namen, aktuell nur Maßstab 1 |
| `channel.sample(value)` | Messwert setzen; während Initialisierung, `create`, `reset` oder `step` |
| `sphere(center, radius, color, id)` | Szenenkugel; nur in `scene`, `center: Vec3` |
| `line(start, end, radius, color, id)` | Szenenlinie; nur in `scene`, Endpunkte vom Typ `Vec3` |
| `arrow(start, end, radius, color, id)` | Pfeil mit `Vec3`-Endpunkten und Schaftradius; nur in `scene` |
| `point(position, radius, color, id)` | Punktmarkierung an einer `Vec3`-Position; nur in `scene` |
| `polyline(points, radius, color, id)` | Linienzug aus einem `[Vec3]`-Array mit mindestens zwei Punkten; nur in `scene` |
| `box(center, size, rotation, color, id)` | Orientierte Box, `Vec3`-Mittelpunkt und volle positive XYZ-Seitenlängen, `Quat`-Rotation; nur in `scene` |
| `plane(center, size, rotation, color, id)` | Orientierte Ebene, `Vec3`-Mittelpunkt und positive `Vec2`-Seitenlängen entlang lokal X/Z, `Quat`-Rotation; nur in `scene` |
| `label(position, text, color, id)` | UTF-8-Text an einer `Vec3`-Position, maximal 63 Bytes und keine Steuerzeichen; nur in `scene` |
| `metadata(text)` | Modellmetadaten ergänzen; nur während Initialisierung/`create` |
| `simulationTime()` | Zeit des Hostkontexts als `Float64` |
| `runSeed()` | Vollständiges 64-Bit-Bitmuster des Laufseeds als `Int64`; in allen Experiment-Callbacks verfügbar |
| `parameter(name, default, minimum, maximum, description)` | Benannten `Float64`-Parameter definieren und seinen wirksamen Wert liefern; nur während globaler Initialisierung oder `create` |
| `randomUniform(min, max)` | Gleichverteiltes `Float64` im angegebenen Intervall; `min <= max` |
| `randomNormal(mean, standardDeviation)` | Normalverteiltes `Float64`; Standardabweichung muss nichtnegativ sein |

Parameter werden während der globalen Initialisierung oder in `create` einmalig
definiert. Der Runner kann sie vor dem Erzeugen des Experiments überschreiben:

```text
let initialSpeed = parameter("initialSpeed", 2.0, -10.0, 10.0, "Initial speed")
```

```powershell
physim-runner.exe experiment.dll run.psrun --param initialSpeed=4.5
```

Namen müssen eindeutig sein und beginnen mit einem ASCII-Buchstaben oder `_`;
danach sind auch Ziffern, Punkt und Bindestrich erlaubt. Höchstens 16 Parameter
und 47 ASCII-Bytes je Name sind möglich. Standardwert, Grenzen und Override
müssen endlich sein; die Grenzen sind inklusiv und der Standardwert muss darin
liegen. Unbekannte oder doppelte Overrides brechen den Lauf ab. Der wirksame
Wert, Standardwert und Grenzen werden in den `.psrun`-Metadaten gespeichert.
Für mehrere Werte kann `physim-batch` mit `--sweep name=start:end` eine lineare
Parameterstudie ausführen; [Monte Carlo und Laufserien](monte-carlo.md) beschreibt
die reproduzierbare Zuordnung und den Endwertbericht.
Nach dem Build fragt die App den Parameterkatalog in einem eigenen Runner-Prozess
ab. Im Inspector stehen Name, Beschreibung, Standardwert und Grenzen; der gewählte
Wert wird vor dem Simulationsstart geprüft. Die Laufserien-Ansicht verwendet
denselben Katalog für Parameterstudien.

Farben und Objekt-IDs sind auf den Wertebereich von `uint32_t` begrenzte `Int64`.
`medium.dragForce` verwendet die Dichte; die Viskosität beeinflusst dieses
quadratische Modell nicht. `medium.stokesDrag` verwendet dagegen die
Viskosität und einen Kugelradius. Die freie Funktion `stokesDrag` bleibt für
einen explizit angegebenen Viskositätswert verfügbar. Bei Dichte, Koeffizient oder
Fläche null liefert das quadratische Modell exakt den Nullvektor.
Einheitenalgebra verändert den Empfänger nicht. Das Ergebnissymbol ist ein
expliziter String. Alle sieben resultierenden Exponenten müssen in `Int8` passen;
der Maßstab muss positiv und endlich bleiben. Auch Unterlauf auf null ist ein
Laufzeitfehler. Der `Int64`-Parameter von `powered` muss zusätzlich in den
nativen C-`int` passen (32 Bit auf den unterstützten Windows-/Linux-Zielen).
Fehler tragen die Quellposition des Methodenaufrufs. Beispielsweise erzeugt
`metres.divided(seconds, "m/s")` eine Geschwindigkeitseinheit und
`seconds.powered(-1, "Hz")` eine Frequenzeinheit.
Sind die sieben Dimensionsexponenten aus `Unit(...)`, Einheitenalgebra und
unveränderlichen `let`-Werten bekannt, weist der Compiler unverträgliche
`convert`-, `Quantity.converted`-, `Quantity.adding`- und
`Quantity.subtracting`-Aufrufe an ihrer Quellposition zurück. Der Maßstab
spielt für die Dimensionsverträglichkeit keine Rolle. In Exponenten wertet der
Compiler außerdem begrenzte `Int64`-Ausdrücke mit `+`, `-`, `*`, `/` und `%`
aus. Division durch null, Überlauf und zu tiefe oder zu große Ausdrücke werden
hier nicht als Konstante behandelt; die reguläre Laufzeitprüfung bleibt bestehen.
Dieselbe statische Prüfung gilt für `Quantity + Quantity` und
`Quantity - Quantity`, einschließlich verketteter Ausdrücke. Die linke Einheit
bestimmt jeweils die Einheit des Ergebnisses. Beide Operanden werden genau einmal
in Quellreihenfolge ausgewertet. Multiplikation und Division mit einem
`Float64`-Skalar sowie ein Vorzeichenwechsel erhalten die Einheit; nicht
endliche Ergebnisse und Division durch null sind Laufzeitfehler mit
Quellposition. Die statische Dimensionsprüfung folgt auch diesen Ausdrücken.
Für Multiplikation und Division zweier `Quantity`-Werte bleibt das
Ergebnissymbol beim Methodenaufruf ausdrücklich anzugeben.
Parameter, veränderliche
Werte, Funktionen und geladene Daten bleiben dynamisch; ihre Einheiten prüft
die gemeinsame Bibliothek zur Laufzeit. Diese Prüfung ersetzt noch keine
statisch dimensionierten `Quantity`-Typannotationen.
Die derzeitige Konstantenerkennung versteht ganzzahlige Exponentenliterale
einschließlich Vorzeichen und unveränderliche Aliase; allgemein berechnete
Ganzzahlausdrücke bleiben dynamisch.

Szenengeometrie, Kapazitäten, Phasen und Kanalzugehörigkeit werden geprüft.
`polyline` übernimmt eine Kopie der Punkte in die Szene. Spätere Änderungen am
Array verändern den Linienzug nicht. Literale, Ausschnitte und Rückgabewerte
sind zulässig; `[]` erhält den Elementtyp aus der Signatur, scheitert aber zur
Laufzeit an der Mindestlänge. Der Radius muss endlich und nichtnegativ sein.
Alle Polylinien einer Szene teilen sich das Limit von 96 Punkten; zusätzlich
gilt das Limit von 32 Szenenobjekten. Zu kurze Linienzüge, Kapazitätsüberschreitung,
doppelte Nichtnull-IDs und Aufrufe außerhalb von `scene` liefern Quelldiagnosen.
Auch nach einem solchen Fehler wird die gesamte unvollständige Szene verworfen.
Argumente werden einmal in Quellreihenfolge ausgewertet, einschließlich der
Wertkopie des Arrays vor Seiteneffekten späterer Argumente.

`Quantity` ist ein Werttyp und kann in Arrays, eigenen Strukturen, Parametern
und Rückgaben verwendet werden. Seine Eigenschaften sind unveränderlich;
eine `var`-Bindung kann durch eine neue `Quantity` ersetzt werden. Beispielsweise
ergibt `Quantity(3, metres).adding(Quantity(50, centimetres)).value` den Wert `3.5`.
Inkompatible Dimensionen, Division durch null, Exponentenüberlauf und nicht
darstellbare Ergebnisse werden mit Quellposition als Laufzeitfehler gemeldet.
Die Dimensionsprüfung erfolgt zur Laufzeit; `Quantity` führt noch keine
SI-Dimensionen als statische Typparameter ein.
Einheitenprüfung erfolgt hier zur Laufzeit; statische SI-Dimensionstypen fehlen noch.

`multiplied` und `conjugated` arbeiten auf den gespeicherten Komponenten:
Auch eine Nullquaternion ist hier zulässig; nicht endliche Ergebnisse sind
Laufzeitfehler. Konjugation entspricht bei Einheitsquaternionen der inversen
Rotation. `normalized` und `slerp` verlangen dagegen von null verschiedene
Quaternionen. Bei `slerp` beschreiben `q` und `-q` dieselbe Orientierung;
das Vorzeichen des Ergebnisses muss deshalb nicht dem Vorzeichen des Endpunkts
entsprechen. Extrapolation außerhalb [0, 1] wird mit Quelldiagnose abgewiesen.
Das SDK-Beispiel `examples/language/rotation_path.phys` kombiniert eine lokale
Neigung mit einer interpolierten Weltrotation und prüft Richtung, Längenerhaltung
und Rückrotation an 101 Stützstellen gegen eine analytische Referenz.

Rotationen sind Werttypen: `Quat(0, 0, 0, 1)` ist die Identität. Die vier
Komponenten lassen sich durch einen `var`-Zugriffspfad verändern. Konstruktion und
Speicherung verlangen keine Einheitslänge; `rotate`, `box` und `plane` normalisieren
intern und weisen Nullquaternionen ab. Das Szenenbeispiel
`examples/language/scene_shapes.phys` zeigt alle acht Formen einschließlich einer
rotierenden Box, eines mitgedrehten Pfeils und eines Linienzuges aus einem Array.
Nichtnull-IDs müssen innerhalb einer Szene eindeutig sein;
bei einem Fehler verwirft der Adapter auch bereits erzeugte Objekte dieser Szene.

Die Kraftfunktionen verwenden SI-Zahlenwerte und die gemeinsame Mechanikbibliothek;
sie sind auch in eigenständigen Programmen und Analysen verfügbar. Steifigkeit,
Ruhelänge, Dämpfung, Dichte, Viskosität, Volumen und Widerstandsbeiwert dürfen nicht
negativ sein; Kugelradien müssen positiv sein. Die Relativgeschwindigkeit für
Widerstand ist Körpergeschwindigkeit minus Fluidgeschwindigkeit. Stokes setzt
schleichende Strömung voraus; das Modell wechselt nicht automatisch anhand einer
Reynoldszahl. Auftrieb berücksichtigt weder Oberflächenspannung noch Zusatzmasse.
`Submersion.sphere` verwendet dieselbe geprüfte Kugelgeometrie wie die
C-API. Ein trockener oder vollständig eingetauchter Körper erhält den
Schwerpunktabstand null; bei halbem Eintauchen liegt er auf der Flüssigkeitsseite
und ist daher negativ. Radius null, nicht endliche Eingaben und numerisch nicht
darstellbare Ergebnisse erzeugen Quelldiagnosen.
`examples/language/buoyancy.phys` verwendet den Wert für einen geführten
schwimmenden Körper. Der Test `language_buoyancy_parity` vergleicht alle zwölf
Kanäle, 401 Zeitpunkte und 26 Szeneobjekte mit der C-Vorlage bei gleichem Seed
und Zeitschritt. Die eigene Sprachauswertung erzeugt drei Diagramme und einen
CSV-Export; der App-Workflow baut und öffnet das Sprachprojekt erneut.
Im Projektmanager stehen die Vorlagenamen jeweils für die gewählte
Experiment-Sprache; für „Auftrieb“ ist die Sprachvorlage direkt auswählbar.
Zusammenfallende Federendpunkte mit aktiver Steifigkeit oder Dämpfung sind ein
Laufzeitfehler. Fehlerhafte Parameter und nicht darstellbare Kräfte führen zu
Quelldiagnosen; die Funktionen verändern ihre Eingabewerte nicht.

`examples/language/spring.phys` implementiert einen horizontal geführten
Feder-Masse-Dämpfer mit elf Messkanälen. Die vier RK4-Stufen sind im Sprachquelltext
formuliert und integrieren auch die dissipierte Arbeit. Die Federkraft kommt aus
der gemeinsamen Bibliothek. Die Feder wird aus 65 Punkten als räumliche Spirale
gezeichnet. Alle 13 Szeneobjekte einschließlich Feder, Dämpfer, Kraftpfeilen und
Beschriftungen stimmen nach dem Start und nach 2.000 Schritten mit der C-Vorlage
überein. Ein App-Workflow prüft Anlage, Build, Lauf, Auswertung und erneutes
Öffnen des Sprachprojekts unter MSVC und Clang. Der allgemeine Euler-
und RK4-Schritt akzeptiert eine Ableitungsfunktion als Wert vom Typ
`func(Float64, [Float64]) -> [Float64]`. `rk45Integrate`
verwendet dieselbe Callbackform und die adaptive C-Routine. Der Callback kann bei
verworfenen Schritten mehrfach aufgerufen werden und soll daher ohne sichtbare
Seiteneffekte sein. Das Eingabearray bleibt unverändert; Fehler nennen die
Abbruchursache mit Quellposition. `rk45IntegrateWithSteps` erlaubt zusätzlich
Anfangs-, Mindest- und Höchstschritt. `rk45IntegrateWithTolerances` verwendet
ein gleich langes Array positiver absoluter Toleranzen für verschieden skalierte
Zustandsgrößen. Es kopiert die Toleranzen vor dem ersten Callback.
`rk45IntegrateReported` liefert den Zustand als unabhängiges Array zusammen mit
`acceptedSteps`, `rejectedSteps`, `evaluations`, `reachedTime`, `nextStep` und
`errorNorm`. `OdeResult` ist ein kopierbarer Wert; auch seine `state`-Kopie
besitzt eigenen Speicher. `nextStep` trägt bei Rückwärtsintegration ein negatives
Vorzeichen. Die Berichtszähler stammen direkt aus der C-Routine.
`rk45IntegrateWithTolerancesReported` verwendet pro Zustandskomponente einen
positiven absoluten Toleranzwert und gibt denselben Berichtstyp zurück. Das
Toleranzarray wird vor dem ersten Callback kopiert.
`rootBisectReported` und `minimizeGoldenReported` liefern `ScalarResult` aus
der C-Numerik. `x` ist die gefundene Koordinate, `value` der dort berechnete
Funktionswert, `lower` und `upper` das zuletzt geprüfte Intervall. Die Zähler
`iterations` und `evaluations` sind `Int64`. Der Wert kann kopiert und in
Arrays oder Optionalwerten gespeichert werden. Fehler folgen denselben Regeln
wie bei den Varianten ohne Bericht.
`verletStep` verwendet einen Beschleunigungs-Funktionswert und getrennte
Positions- und Geschwindigkeitswerte in
einem Array. Die Beschleunigung darf nicht von der Geschwindigkeit abhängen;
die Eingabe bleibt auch hier unverändert. Numerische Callback-Parameter der
Bibliothek akzeptieren freie Funktionen, statische Methoden und gespeicherte
Funktionswerte mit passender Signatur. Ein Wert darf zwischen Aufrufen neu
zugewiesen werden; der Callback wird vor jedem numerischen Aufruf einmal gelesen.

Zufallsziehungen sind nur in `reset` und `step` zulässig, auch wenn der Aufruf über
eine Hilfsfunktion erfolgt. Der Adapter initialisiert vor jedem Reset den
instanzeigenen PCG32-Generator aus dem Lauf-Seed des Runners. Das gilt auch für den
ersten Reset nach `create`. Gleicher Seed und gleiche Aufrufreihenfolge erzeugen
mit demselben Build dieselbe Folge. Ein geänderter Seed wirkt beim nächsten Reset.
Globale Initialisierungen, `create` und `scene` dürfen keine Ziehungen ausführen;
eigenständige Programme und Analysemodule besitzen diese Hostbindung nicht.

Beide Funktionen verwenden `ps_distribution_sample` aus der gemeinsamen
Messbibliothek. Ein Intervall mit identischen Grenzen und eine Normalverteilung
mit Standardabweichung null verbrauchen keine Zufallszahlen. Ungültige Parameter
oder nicht endliche Ergebnisse erzeugen Quelldiagnosen, ohne den Generator bei
der fehlgeschlagenen Ziehung weiterzuschalten. Alle Ziehungen einer Instanz teilen
denselben Strom: Zusätzliche Rauschziehungen können spätere zufällige Start- oder
Modellwerte beeinflussen. Für getrennte Ströme steht `Rng(seed)` bereit.
Sensoren besitzen dagegen eigene Seeds und indexierte Zufallsziehungen; ihre
Messungen verändern diesen gemeinsamen Generator nicht.

`examples/language/random_samples.phys` zeichnet Gleich- und Normalverteilungen
in zwei Kanälen auf. `physim_add_experiment(language_random random_samples.phys)`
baut das Beispiel; der normale Runner wählt den Seed mit `--seed`.
Die mathematischen Funktionen, Vektoren, Einheiten und der Integrator funktionieren
auch in eigenständigen Programmen. Hostfunktionen erfordern den Experimentmodus.

Laufzeitfehler werden an der ABI-Grenze abgefangen und als `PS_NUMERIC` mit
Quellpfad, Zeile und Spalte gemeldet. Danach verweigert die Instanz weitere Schritte,
bis ein erfolgreicher Reset erfolgt. Fehlgeschlagenes Erzeugen gibt den Zustand
wieder frei. Szenenfehler verwerfen die unvollständige Szene und sperren weitere
Schritte. `print` ist in Experimentmodulen vorerst nicht erlaubt, da die Ausgabe
den Runner-Protokollstrom beschädigen würde; ein eigener Logkanal ist noch offen.
Compilerkennung, Backend und FNV-1a-Quellhash stehen in den Laufmetadaten; der Runner
ergänzt seinen Modulhash. Diese Hashes dienen der Zuordnung, nicht der Sicherheit.

`pendulum.phys` verwendet dieselbe Bibliothek und sechs Kanäle wie die symplektische
C-Pendelvariante. `pendulum_rk4.phys` formuliert die vier RK4-Stufen in der
Sprache und entspricht dem Standard-C-Pendel. `pendulum_integrator.phys` verwendet
denselben Modellzustand mit dem allgemeinen `rk4Step` und einer Physim-Ableitungsfunktion.
`language_experiment` vergleicht bei beiden RK4-Varianten alle sechs Kanäle
über 4.000 Schritte bei 0,005 s mit der C-Referenz.
`pendulum_rk45.phys` integriert denselben Zustand adaptiv mit denselben
Schrittweitenoptionen wie die C-Vorlage; der ABI-Test vergleicht
alle sechs Kanäle und die Szene über 4.000 Schritte mit dem C-RK45-Pendel.
Ein Runnerlauf prüft die erzeugte Messdatei und die Energieerhaltung.
`pendulum_verlet.phys` verwendet `verletStep` mit einem Freiheitsgrad. Der gleiche
ABI-Test vergleicht Kanäle und Szene mit dem C-Verlet-Pendel; ein Runnerlauf
prüft die gespeicherten Daten.
`projectile.phys` ist ein analytisch geprüfter Vakuumwurf.
`projectile_drag.phys` bildet mit `Medium.air().dragForce` und expliziten RK4-Stufen die
C-Wurfvorlage mit Luftwiderstand nach. Der
[gemeinsame Lernpfad](projectile-drag-tutorial.md) vergleicht beide Runnerläufe,
Szenen und Auswertungen. `language_experiment` prüft native Module, unabhängige
Instanzen, Reset, Fehler und Messdateien des echten Runners. Dies ist die erste
Experimentanbindung, noch keine vollständige Abnahme von LANG-004 bis LANG-007.
`uncertain_projectile.phys` verwendet wie die C-Vorlage zwei normalverteilte
Anfangsgeschwindigkeiten und zwei unabhängig gesetzte Sensorströme. Der
Paritätstest vergleicht für zwei 64-Bit-Seeds alle 201 Zeilen und 15 Kanäle
einschließlich Messstatus, Unsicherheit und Energie. Beide Auswertungsmodule
verarbeiten Laufdateien beider Sprachen.

## Erste Analysemodule

`physimc --emit-analysis analysis.phys` erzeugt C17 für ein Analysemodul mit
`analyze() -> Void` als Einstieg. `physim-build` kompiliert es zum nativen Modul.
Beispiel: `examples/language/analysis.phys`. Die App kann diese
Vorlage beim Anlegen als `analysis.phys` übernehmen und über den Analyseeditor bauen.

```text
func analyze():
    report("Horizontal position")
    let run: Dataset = Dataset(0)
    let time: Series = run.series("time")
    let position = run.series("position.x")
    let graph: Plot = position.plot(time, "Position", "position.x")
    graph.export("position")
```

Aufruf unter Windows nach dem Build der Sprachbeispiele:

```powershell
physim-analysis-runner language_analysis.dll lauf.psrun ergebnis
physim-analysis-runner language_analysis.dll --runs vergleich lauf1.psrun lauf2.psrun
physim-analysis-runner physim-language-analysis-monte-carlo.dll --runs monte-carlo
```

Runner-/Modulpfade entsprechend dem Buildordner setzen. Der Runner schreibt ein
Manifest der Eingabedateien und des Moduls. Der Sprachadapter speichert nach
erfolgreichem Abschluss automatisch `ergebnis.psreport`; der Bericht enthält
Compilerkennung, Backend und Quellhash. Er lässt sich in der bestehenden App öffnen.

| Aufruf | Vertrag |
| --- | --- |
| `inputCount()` | Anzahl ausgewählter Läufe, 0 bis 8; null für selbst erzeugte Analysen |
| `Dataset(index)` | `Dataset`, nullbasierter Index; C- und Sprachläufe werden gleich behandelt |
| `dataset.sampleCount()` | Anzahl gelesener Datensätze als `Int64` |
| `dataset.channelCount()` | Anzahl Messkanäle ohne Zeitachse als `Int64` |
| `dataset.channelName(index)` | Exakter Name des nullbasierten Messkanals als eigene `String`-Kopie |
| `dataset.channelUnitSymbol(index)` | Gespeichertes Einheitensymbol als eigene `String`-Kopie |
| `dataset.channelDescription(index)` | Gespeicherte Kanalbeschreibung als eigene `String`-Kopie |
| `dataset.channelExponent(index, axis)` | SI-Exponent des Kanals als `Int64`; Achsen 0–6: Länge, Masse, Zeit, Strom, Temperatur, Stoffmenge, Lichtstärke |
| `dataset.recovered()` | `Bool`, ob eine teilweise Laufdatei wiederhergestellt wurde |
| `dataset.metadata()` | Eigenständige `String`-Kopie der gespeicherten Laufmetadaten, auch nach `close()` gültig |
| `dataset.series(name)` | `Series`; `"time"` oder exakter Kanalname |
| `Series.fromValues(values, unit, name)` | Kopiert ein `[Float64]` als eigenständige `Series` mit Einheit und Name |
| `anchor.alignedValues(values, unit, name)` | Kopiert gleich viele Werte in eine `Series` mit Raster und Lebensdauer des Ankers |
| `input.count()` | Anzahl Werte als `Int64` |
| `input.name()` | Eigenständige `String`-Kopie des Reihennamens, auch nach `release()` gültig |
| `input.unitSymbol()` | Eigenständige `String`-Kopie des gespeicherten Einheitensymbols; bei abgeleiteten Reihen formatiert |
| `input.unitScale()` | Maßstab der gespeicherten Werte zur SI-Einheit als `Float64` |
| `input.exponent(axis)` | SI-Exponent als `Int64`, Achse 0–6 wie bei `dataset.channelExponent` |
| `input.isAlignedWith(other)` | `Bool` für gültige Reihen desselben Rasters und Samplebereichs; ungültige Handles sind Fehler |
| `input.slice(first, count)` | Kopiert einen nullbasierten Ausschnitt einschließlich `first` mit `count` Werten; passende Ausschnitte bleiben zugeordnet |
| `input.value(index)` | Einzelner `Float64`-Wert; geprüfter nullbasierter Index |
| `input.values(first, count)` | Eigenständiges `[Float64]` mit `count` Werten ab `first`; liest blockweise und erlaubt einen leeren Ausschnitt am Reihenende |
| `input.mean()`, `input.stddev()` | Mittelwert bzw. Stichprobenstandardabweichung; mindestens 1 bzw. 2 Werte |
| `input.quantile(probability)` | Typ-7-Quantil einer nichtleeren Reihe; endliche Wahrscheinlichkeit in [0, 1], einschließlich Minimum, Median und Maximum |
| `input.minimum()`, `input.maximum()` | Kleinster bzw. größter Wert einer nichtleeren Reihe, blockweise auch bei großer Spannweite |
| `y.derivative(x)` | Abgeleitete Series mit berechneten SI-Dimensionen |
| `y.integral(x, initial, unit)` | Kumulatives Trapezintegral; `initial` und `unit` beschreiben den Anfangswert in der Ergebnisdimension |
| `input.affine(factor, offset, unit)` | Skalierung plus Offset; Offseteinheit muss zur Eingabereihe passen |
| `left.adding(right)`, `left.subtracting(right)` | Zuordnungs- und dimensionsgeprüfte Summe/Differenz |
| `left.multiplied(right)`, `left.divided(right)` | Zugeordnete Reihen mit zusammengesetzten Einheiten; Nullteiler sind Fehler |
| `y.resampledLinear(x, targetX)` | Explizite lineare Interpolation auf ein anderes Zeitraster, auch aus einem anderen Dataset |
| `y.resampledPchip(x, targetX)` | Monotone kubische Hermite-Interpolation auf ein anderes Raster; keine Extrapolation |
| `y.resampledNearest(x, targetX)` | Nächster Messpunkt; bei gleichem Abstand der frühere |
| `y.resampledPrevious(x, targetX)` | Letzten vorherigen Messwert halten; exakte Stützstellen bleiben erhalten |
| `input.movingAverage(window)` | Kausaler gleitender Mittelwert, Fenster 1 bis 4096 |
| `Series.select(columns, selector, accepted)` | `[Series]`; 1 bis 32 gemeinsam gefilterte Reihen, dimensionsloser Selektor und exakter Vergleich mit `accepted` |
| `input.release()` | Series freigeben; sämtliche Kopien dieses Handles werden ungültig |
| `dataset.close()` | Dataset samt zugehörigen Datenreihen schließen; ein späteres `Dataset(index)` öffnet eine neue Generation |
| `report(title)` | Genau einen Bericht pro Analyse erzeugen, vor Plot- und Tabellenaufrufen |
| `y.plot(x, title, label)` | `Plot` mit erster Linienkurve; Achseneinheiten stammen aus den Datenreihen |
| `plot.curve(x, y, label)` | Weitere Linienkurve mit Einheiten- und Zuordnungsprüfung |
| `plot.points(x, y, label)` | Weitere Punktkurve mit gleicher Prüfung; keine Verbindung über Messlücken |
| `input.histogram(title, bins)` | `Plot` mit Histogramm; 1 bis 128 Klassen |
| `y.export(x, suffix)` | Vollständige zugeordnete Datenreihen nach `<prefix>-<suffix>.csv` |
| `Series.exportColumns(columns, suffix)` | 1 bis 32 zugeordnete `[Series]` in ihrer Reihenfolge nach `<prefix>-<suffix>.csv`; Kopfzeilen enthalten Name und Einheit jeder Reihe |
| `plot.export(suffix)` | Diagramm nach `<prefix>-<suffix>.svg` |
| `Table(title, labels, units)` | Berichtstabelle mit 1 bis 8 Spalten; `[String]` und `[Unit]` müssen gleich lang sein |
| `table.row(label, values)` | Eine Zeile aus `[Quantity]`; gleiche Spaltenzahl, Umrechnung in die jeweilige Spalteneinheit |
| `table.export(suffix)` | Tabelle mit Einheiten nach `<prefix>-<suffix>.csv` |

Für `quantile(p)` werden die endlichen Werte aufsteigend sortiert. Bei `n` Werten
liegt die Position bei `(n - 1) * p`; zwischen benachbarten Werten wird linear
interpoliert. Das Ergebnis hat dieselbe Einheit wie die Eingabereihe. Die Methode
benötigt temporär acht Byte je Wert aus dem Sprachspeicherbudget; eine leere Reihe
oder eine Wahrscheinlichkeit außerhalb von [0, 1] ist ein Laufzeitfehler.

Export-Suffixe bestehen aus 1 bis 64 ASCII-Buchstaben, Ziffern, `_` oder `-`.
`Series.exportColumns` verlangt lebende Reihen desselben Rasters und
Samplebereichs. Leere, zu große oder nicht zugeordnete Spaltenlisten werden
vor dem Öffnen der Ausgabedatei abgewiesen.
Dateien werden exklusiv erstellt; vorhandene Ergebnisse werden nicht überschrieben.
Vor einem späteren Analysefehler bereits exportierte Dateien bleiben erhalten.
Ein Bericht wird erst nach erfolgreichem Quellprogramm gespeichert; ein Fehler beim
Schreiben selbst kann entsprechend der C-API eine unvollständige Datei hinterlassen.

Handles werden als Werte kopiert und über Besitzer, Slot und Generation geprüft.
Ein mit `input.values` gelesenes Array besitzt seine Werte selbst und bleibt auch
nach `input.release()` oder `dataset.close()` gültig. Negative oder zu große
Bereiche sind Fehler; die Ausgabegröße unterliegt dem Speicherbudget.
Sie gelten nur innerhalb eines Analyseaufrufs. Abgeleitete Reihen verwenden die
blockweise arbeitende gemeinsame Bibliothek mit zunächst 1 GiB Scratch-Limit.
`Series.fromValues` liest keinen Eingabelauf. Mit `--runs ausgabepräfix` kann
der Analyse-Runner auch ohne Eingabedatei starten; dann ist `inputCount()` null
und `Dataset(0)` ungültig. Jede so erzeugte Reihe erhält
ein eigenes Alignment; für gemeinsame X-/Y-Daten erzeugt
`x.alignedValues(yValues, unit, name)` die zweite Reihe mit demselben Raster.
Alle Werte müssen endlich sein. Die Erzeugung kopiert die Werte in den
budgetierten Scratch-Speicher und ändert bei einem Fehler weder Handle noch
Speicherbilanz. Ein aus einem Dataset abgeleiteter Anker verleiht auch der neuen
Reihe dessen Lebensdauer; eigenständige Reihen bleiben beim Schließen eines
Datasets gültig. `examples/language/analysis_monte_carlo.phys` zeigt damit eine
seedbare Monte-Carlo-Auswertung mit Diagramm, Histogramm, Tabelle und CSV-Export.
`examples/language/analysis_batch_endpoints.phys` fasst Endwerte aus mehreren
gespeicherten C- und Sprachläufen in denselben Artefakten zusammen.
Der Adapter räumt Kontext, Scratchdateien und Bericht auch nach abgefangenen
Sprachfehlern auf. Die nächste Analyse beginnt mit frischem globalem Zustand.
Plots kopieren ihre begrenzte Vorschau; sie bleiben nach `close` gültig.
Tabellen kopieren Beschriftungen, Einheiten und Zahlen ebenfalls in den Bericht.
Kopien eines `Plot`- oder `Table`-Handles bezeichnen dasselbe Berichtsobjekt.
Ein Bericht erlaubt höchstens 8 Tabellen mit jeweils 256 Zeilen. Dimensionsfehler
oder ungültige Zahlen verhindern das Hinzufügen der gesamten Zeile.
Eingabeläufe mit rekonstruierbaren Daten liefern nach erfolgreicher Auswertung
`PS_RECOVERED`. API-Fehler erhalten ihren Fehlercode, Sprach-/Rechenfehler liefern
`PS_NUMERIC`; beide melden die ursprüngliche Quellposition.

Experimentfunktionen sind im Analysemodus nicht verfügbar und umgekehrt.
`--check` prüft zunächst die Typen unabhängig vom späteren Modulmodus; der gewählte
Emissionsmodus prüft zusätzlich die Hostfunktionen und Einstiegssignaturen.
Resampling verlangt streng steigende, einheitenkompatible Achsen und erlaubt keine
Extrapolation. Sein Ergebnis übernimmt die Zuordnung des Zielrasters und kann mit
dessen Reihen verrechnet werden. Es überlebt das Schließen des Quelldatasets,
aber nicht des Zieldatasets. Ohne explizites Resampling bleiben Rechenoperationen
zwischen unabhängig geöffneten Läufen unzulässig, auch bei gleichen Zeitwerten.
`examples/language/analysis_integral.phys` zeigt die Rekonstruktion einer Position
aus ihrer numerischen Ableitung und zeichnet den verbleibenden Diskretisierungsfehler.

`slice(first, count)` erstellt eine eigene Reihe mit demselben Namen und derselben
Einheit. Beide Grenzen müssen innerhalb der Quellreihe liegen; `count = 0` ist
auch am Reihenende gültig. Gleich ausgeschnittene Reihen derselben Auswahl
bleiben zugeordnet und können gemeinsam abgeleitet oder verrechnet werden.
Das Freigeben der Quellreihe lässt den Ausschnitt bestehen, das Schließen ihres
Datasets macht ihn ungültig.

`Series.select` erhält Reihenfolge und Einheiten. Alle Eingabespalten und der
Selektor müssen zusammengehören. Die Ergebnisreihen desselben Aufrufs erhalten
eine neue gemeinsame Zuordnung; zwei unabhängige Auswahlaufrufe haben auch bei
identischen Werten verschiedene Zuordnungen. Ohne Treffer enthält das Ergebnis
weiterhin eine leere `Series` pro Eingabespalte. Deshalb vor `mean`, `stddev`,
Histogrammen oder Ableitungen die jeweilige Mindestanzahl prüfen.
Der Arraycontainer hat die normale Wertsemantik; enthaltene Handles bleiben
Aliase der vom Analysekontext verwalteten Reihen. Die ausgewählten Reihen
überleben `release` ihrer Eingabereihen, jedoch nicht `close` ihres Datasets.

```physim
let columns = Series.select([time, measured], status, 1)
if columns[0].count() > 0:
    plot.points(columns[0], columns[1], "Gültige Messungen")
let table = Table("Verfügbarkeit", ["Gültig", "Gesamt"], [one, one])
table.row("Lauf 1", [Quantity(Float64(columns[0].count()), one),
                     Quantity(Float64(time.count()), one)])
table.export("availability")
```

`examples/language/analysis_sensors.phys` ist die Sprachauswertung der
Sensorwurf-Vorlage. Sie filtert Zeit, Modell, Messwert und Standardunsicherheit
gemeinsam nach Status 1, zeichnet Messpunkte und berechnet Messfehlerstatistik
und mittlere Standardunsicherheit. Bei null gültigen Werten bleiben Modell und
Verfügbarkeit sichtbar; bei weniger als zwei Werten entfällt die Statistikzeile.
Die mittlere Standardunsicherheit ist kein Konfidenzintervall des Mittelwerts.

Weitere Series-Operationen, sprachgesteuerte Batchausführung und vollständige
Vorlagenparität bleiben offen.

## Starre Körper

`Body` bindet die gemeinsame C-Mechanikbibliothek ein und besitzt Wertsemantik,
auch in Arrays, Strukturen und Funktionsrückgaben. Seine Komponenten werden
in SI angegeben: Meter, Sekunden, Kilogramm, Newton, Newtonmeter und Radiant.
Diese Dimensionen sind derzeit Konvention der Schnittstelle und werden nicht
statisch durch `Quantity`-Typen erzwungen.

| Aufruf oder Eigenschaft | Vertrag |
| --- | --- |
| `Body.sphere(mass, radius)` | Homogene Vollkugel, anfänglich ruhend im Ursprung; Masse ≥ 0, Radius > 0 |
| `Body.box(mass, size)` | Homogener Quader; `size: Vec3` enthält volle positive Kantenlängen |
| `position`, `velocity` | Schwerpunktposition und lineare Geschwindigkeit als `Vec3` |
| `orientation` | Einheitsquaternion von lokalen Hauptträgheitsachsen in Weltkoordinaten |
| `angularVelocity` | Winkelgeschwindigkeit als `Vec3` in Weltkoordinaten, rad/s |
| `mass`, `inertia` | Masse in kg und Hauptträgheitsmomente als `Vec3` in kg·m² |
| `body.setState(position, velocity, orientation, angularVelocity)` | Mutierende, gemeinsam geprüfte Zustandsänderung |
| `body.applyImpulse(impulse, point)` | Mutierender Impuls in N·s am Weltpunkt; ändert lineare und Winkelgeschwindigkeit |
| `body.step(force, torque, dt)` | Mutierender Schritt mit Kraft und Drehmoment in Weltkoordinaten; `dt > 0` |
| `body.kineticEnergy()` | Translations- plus Rotationsenergie in Joule |
| `body.pointVelocity(point)` | Weltgeschwindigkeit am angegebenen Weltpunkt |
| `body.forceTorque(force, point)` | Drehmoment einer Kraft am Weltpunkt um den Schwerpunkt |

Die Eigenschaften sind schreibgeschützt, einschließlich ihrer Vektorkomponenten.
`setState` verlangt eine normierte Orientierung; bei Bedarf vorher
`orientation.normalized()` verwenden. Masse null bezeichnet einen statischen
Körper mit verschwindenden Geschwindigkeiten und Trägheitsmomenten. Kräfte,
Impulse und Zeitschritte bewegen ihn nicht; `setState` kann seine Lage ändern.
Die Trägheit wird beim Konstruktor aus der Geometrie berechnet; eine andere
Masse oder Geometrie erfordert einen neuen Körperwert.

Mutierende Aufrufe verlangen einen veränderbaren Empfänger. Empfänger und
Argumente werden genau einmal in Quellreihenfolge ausgewertet. Das Ergebnis
wird erst nach erfolgreicher Berechnung zurückgeschrieben. Nicht darstellbare
Zustände und ungültige Argumente führen zu einem Laufzeitfehler mit Quellposition.
Kopien in anderen Variablen oder Arrays bleiben unabhängig.

```physim
var body = Body.sphere(mass: 2, radius: 0.5)
body.applyImpulse(impulse: Vec3(2, 0, 0), point: Vec3(0, 1, 0))
body.step(force: Vec3(0, 0, 0), torque: Vec3(0, 0, 0), dt: 0.01)
let energy = body.kineticEnergy()
```

`step` verwendet symplektisches Euler für die Translation, eine explizite
gyroskopische Winkelbeschleunigung und eine exponentielle Quaternionaktualisierung.
Das Verfahren ist erster Ordnung und garantiert keine Energieerhaltung bei
allgemeiner Rotation; Zeitschritte durch Verfeinerung prüfen. Körperwerte
erzeugen keine automatische Gravitation, Kontaktgeometrie oder Kollisionen.

`examples/language/rigid_body.phys` ist ein eigenständig ausführbares Beispiel
mit Impulsbilanz, Rotationsenergie, statischem Körper und unabhängigen Kopien.
`examples/language/spinning_body.phys` beziehungsweise die App-Vorlage
**Starrer Körper · Physim-Sprache** zeigt einen Quader mit außermittiger Kraft,
Quaternionrotation und 15 Messkanälen. Es berechnet keine Kontakte; die folgende
Bindung ergänzt dafür die gemeinsame Kontaktbibliothek. Distanzgelenke sind
ebenfalls angebunden, ebenso der gemeinsame Solver für Körpergruppen weiter unten.
Die äußere Kraft führt Energie zu oder ab; die Änderung der kinetischen Energie
ist in diesem Beispiel deshalb kein reiner numerischer Energiefehler.

## Kontakte und Reibung

`Material`, `Contacts`, `ContactSolver` und `ContactResult` sind kopierbare Werttypen ohne
versteckten Besitz. Sie sind in Programmen, Experimenten und Analysen verfügbar,
auch in Arrays und Strukturen. Die Erkennung und Auflösung verwenden dieselben
C-Funktionen wie die C-Vorlagen. Größen sind SI; Quadergrößen sind volle positive
Kantenlängen, Kugelradien strikt positiv.

| Aufruf | Ergebnis und Vertrag |
| --- | --- |
| `Contacts.spheres(bodyA, radiusA, bodyB, radiusB)` | Höchstens ein Kugel–Kugel-Kontakt |
| `Contacts.spherePlane(body, radius, point, normal)` | Kugel gegen statische Ebene; normierte Ebenennormale weist in den freien Halbraum |
| `Contacts.sphereBox(sphere, radius, box, size)` | Kugel ist A, Quader ist B; auch orientierte Quader |
| `Contacts.boxPlane(body, size, point, normal)` | Bis zu acht Quaderpunkte im oder auf dem festen Halbraum |
| `Contacts.boxes(bodyA, sizeA, bodyB, sizeB)` | Orientierte Quader mit SAT und Kontaktmannigfaltigkeit |
| `contacts.count` | `Int64`, 0 bis 8; Berührung zählt bereits als Kontakt |
| `contacts.point(index)`, `normal(index)`, `penetration(index)` | Weltpunkt, Einheitsnormale von A nach B und Eindringtiefe in Metern |
| `ContactSolver.defaults()` | Aktuelle Standardeinstellungen der gemeinsamen C-Bibliothek |
| `Material(density, restitution, friction)` | Material mit Dichte in kg/m³, Rückprall in [0, 1] und nichtnegativer Reibung; Werte müssen endlich sein |
| `material.density`, `material.restitution`, `material.friction` | Lesbare Materialeigenschaften |
| `material.contactSolver(iterations, bounceThreshold, penetrationSlop, correctionFraction)` | Geprüfter `ContactSolver` mit Rückprall und Reibung des Materials |
| `ContactSolver(iterations, restitution, friction, bounceThreshold, penetrationSlop, correctionFraction)` | Geprüfte Einstellungen; Argumente auch vollständig benannt und umgeordnet |
| `contacts.resolve(bodyA, bodyB, solver)` | `ContactResult` mit beiden korrigierten Körperwerten; Eingaben bleiben unverändert |
| `contacts.resolveSingle(bodyA, bodyB, restitution, friction)` | Antwort für genau einen Kontakt mit derselben C-Einzelkontaktfunktion; `restitution` in [0, 1], `friction` nichtnegativ; liefert korrigierte Körper und den Impuls als `ContactResult` |
| `result.bodyA`, `result.bodyB` | Ergebniszustände der Körper |
| `result.count`, `result.impulse(index)` | Kontaktanzahl und Weltimpuls auf A je Kontakt, in N·s |
| `result.maxNormalError` | Maximaler Restfehler der normalen Geschwindigkeitsbedingungen, in m/s, vor Positionsprojektion |

Alle Eigenschaften sind schreibgeschützt. Die Einstellungen besitzen gleichnamige
lesbare Eigenschaften. `iterations` liegt in 1 bis 256; Rückprallkoeffizient
`restitution` und `correctionFraction` in 0 bis 1. Reibungskoeffizient,
Rückprallschwelle in m/s und Eindringtoleranz in m sind endlich und nichtnegativ.
Kontakt- und Impulsindizes werden geprüft. Ohne Treffer ist `count` null; eine
Auflösung dieser leeren Kontaktmenge liefert unveränderte Körper und null Restfehler.
`resolveSingle` verlangt dagegen genau einen Kontakt. Sein `maxNormalError`
ist null, weil die C-Einzelkontaktfunktion keinen Iterationsrestfehler liefert.
Die Materialdichte ändert die Körpermasse nicht automatisch; für einen homogenen
Körper berechnet das Experiment `mass = material.density * volume` und übergibt
die Masse an `Body.sphere` oder `Body.box`. Das Box-Stoßbeispiel verwendet
denselben Materialwert für Masse und Solverparameter.

```physim
let contacts = Contacts.spheres(a, radiusA, b, radiusB)
let result = contacts.resolve(a, b, ContactSolver.defaults())
a = result.bodyA
b = result.bodyB
```

Kontakte gehören zur erkannten Anordnung und Reihenfolge A/B. Die Auflösung
verlangt dieselben Körperzustände; nach Bewegung müssen Kontakte neu berechnet
werden. Kopien speichern Werte und verfolgen spätere Körperänderungen nicht.
Für Ebenenkontakte wird als B ein statischer Körper mit Masse null übergeben,
etwa `Body.box(0, Vec3(8, 0.1, 8))`; die Ebenengeometrie kommt aus der Erkennung.
Der statische Körper bleibt unverändert. Der Solver prüft numerische Gültigkeit,
kann jedoch nicht erkennen, ob eine Kontaktmenge zu anderen Körperwerten gehört.

Der Solver arbeitet mit akkumulierten Normalimpulsen und einem projizierten
Coulomb-Reibkegel. Die feste Iterationszahl garantiert keine Konvergenz;
`maxNormalError` dient zur Kontrolle. Die Eindringkorrektur verschiebt nur
Positionen. Kontakte bleiben während der Iteration fest. Diskrete Erkennung
verhindert kein Durchtunneln bei großen Zeitschritten; CCD und Warmstart sind
im Paarsolver nicht enthalten. Die folgende Sweep-Bindung ergänzt lineare
Kugelabfragen. Für mehrere gekoppelte Körperpaare dient der unten
beschriebene gemeinsame Körpergruppen-Solver.
Früher erfasste Kontakte und Ergebnisimpulse beziehen sich auf die Lage vor
der Positionskorrektur.

`examples/language/collision.phys` bildet die C-Kugelstoßvorlage mit
Sweep-Abfrage oder diskreter Kontakterkennung, Einzelkontaktimpuls,
Restzeitintegration und den auswählbaren Medien-/Widerstandsparametern ab.
Der Paritätstest vergleicht Vakuum, Luft, benutzerdefiniertes Medium und
Reibung mit Anfangsrotation in jeweils elf Messkanälen über 401 Zeitpunkte;
die Szene wird vor und nach dem Vakuumstoß verglichen. Die zugehörige
Sprachauswertung zeigt Position und Geschwindigkeit beider Kugeln und
exportiert SVG und CSV. Der Paritätstest wertet zusätzlich den C-Lauf in Physim
und den Physim-Lauf in C aus. Der App-Workflow prüft das Sprachprojekt unter MSVC
und Clang. Medium, Widerstandsmodell, Reibung, Rotation, Geschwindigkeit und
CCD lassen sich im Sprachprojekt als geprüfte Experimentparameter wählen. Der
App-Test wählt Reibung und Anfangsrotation, prüft die gespeicherten Laufwerte
und öffnet das Projekt mit denselben Werten erneut.

`examples/language/box_collision.phys` bildet auch die C-Boxstoßvorlage ab.
Winkel, Höhenversatz, Rückprall und Reibung sind Experimentparameter. Alle
15 Messkanäle und die Szene mit den letzten Kontaktpunkten entsprechen der
C-Referenz. Der Paritätstest prüft elastischen, unelastischen und schrägen
Stoß mit Reibung über jeweils 401 Zeitpunkte. Die Auswertung zeigt Positionen
und Geschwindigkeiten beider Boxen und verarbeitet Laufdateien beider Sprachen.
Im Projektmanager ist **Boxstoß · Physim-Sprache** direkt auswählbar; der
App-Test prüft zudem den geänderten Rückprallparameter in Laufdatei und
erneut geöffnetem Projekt.

`examples/language/contacts.phys` prüft unter anderem einen elastischen
Kugelstoß und einen analytischen Reibungsfall. `box_contacts.phys` und die
App-Vorlage **Box auf Ebene · Physim-Sprache** entsprechen dem Modell der
C-Vorlage `examples/box_floor/main.c`. Der Schritt integriert zuerst Gewichtskraft
und Rotation, erkennt danach Kontakte und übernimmt anschließend den gelösten
Körper. Elf Messkanäle zeigen Bewegung, Energie, Kontaktanzahl, Impuls,
Bodenabstand und Restfehler. Reibung und Rückprallkoeffizient unter eins
führen hier absichtlich zu Energieverlusten.

## Distanzgelenke

`DistanceJoint` und `JointResult` sind unabhängige Wertkopien, auch in Arrays und
eigenen Strukturen. Die Bindungen verwenden `ps_distance_joint_validate` und
`ps_distance_joint_resolve` aus der gemeinsamen Mechanikbibliothek. Sie stehen
in Programmen, Experimenten und Analysen zur Verfügung.

| Aufruf oder Eigenschaft | Ergebnis / Vertrag |
| --- | --- |
| `DistanceJoint(anchorA, anchorB, length, stabilization)` | Zwei endliche lokale `Vec3`-Anker, positive endliche Soll-Länge in Metern, endliche Stabilisierung in `0..1` |
| `joint.anchorA`, `joint.anchorB`, `joint.length`, `joint.stabilization` | Schreibgeschützte Konfiguration |
| `joint.resolve(bodyA, bodyB, dt)` | `JointResult`; positive endliche Schrittweite in Sekunden; Eingabekörper bleiben unverändert |
| `result.bodyA`, `result.bodyB` | Schreibgeschützte Körperwerte mit korrigierten Geschwindigkeiten; Position und Orientierung bleiben unverändert |
| `result.impulse` | `Vec3`-Impuls auf A in N s; auf B wirkt sein negatives Gegenstück |
| `result.lengthError` | Vorzeichenbehafteter Abstand minus Soll-Länge vor der Auflösung, in Metern |
| `result.velocityError` | Absoluter Geschwindigkeitsrestfehler entlang der Verbindung, einschließlich Stabilisierung, in m/s |

Beide Anker werden mit Position und Orientierung ihres jeweiligen Körpers in
Weltkoordinaten transformiert. Für einen festen Weltanker dient ein Körper mit
Masse null, Identitätsrotation und Position null; dessen `anchorB` entspricht
dann direkt dem Weltpunkt. Auch verschobene und gedrehte statische Körper sind
möglich. Wie bei Kontakten bezeichnet die Bindung Körperwerte ohne versteckte
Identität: Die beiden Argumente sind unabhängige Kopien, auch bei gleicher
Quellvariable. Ergebnisse müssen ausdrücklich übernommen werden.

```text
let zero = Vec3(0,0,0)
let fixed = Body.sphere(0,0.1)
let joint = DistanceJoint(zero,Vec3(0,2,0),1,0.2)
var body = Body.sphere(1,0.1)
body.setState(Vec3(0,1,0),Vec3(1,0,0),Quat(0,0,0,1),zero)
let dt = 0.005
body.applyImpulse(Vec3(0,-9.81,0) * dt,body.position)
let result = joint.resolve(body,fixed,dt)
body = result.bodyA
body.step(zero,zero,dt)
```

Die Auflösung erfolgt nach äußeren Geschwindigkeitsänderungen und vor dem
Positionsschritt. Eine außermittige Verankerung verändert auch die Rotation.
Zusammenfallende Weltanker sind singulär und führen zu einer Quelldiagnose.
Zwei statische Körper liefern keinen Impuls, können aber einen unerfüllten
Geschwindigkeitsrestfehler melden. Ungültige Werte und numerische Überläufe
brechen den Aufruf ab, ohne einen Teilzustand zurückzugeben.

Die Stabilisierung korrigiert einen Anteil des Längenfehlers pro Schritt über
eine Zielgeschwindigkeit; sie kann Energie zuführen. Die Position wird weder
projiziert noch nachträglich auf die Soll-Länge gesetzt. Schrittweitenverfeinerung
ist deshalb erforderlich. Ein einzelner Aufruf löst ein Gelenk; gekoppelte
Gelenk-/Kontaktgraphen verwenden die nachfolgende Gruppenbindung. Warmstart und
weitere Gelenkarten stehen noch nicht zur Verfügung.

`distance_joints.phys` prüft Impulsbilanz, gedrehte lokale Anker, statische Körper,
Wertkopien, Stabilisierung und Schrittweitenverfeinerung. `joint_pendulum.phys`
und die App-Vorlage **Gelenkpendel · Physim-Sprache** zeigen einen an einem
außermittigen Anker aufgehängten Quader. 14 Kanäle erfassen Bewegung, Rotation,
Energie, Impulse und Gelenkfehler. Der Schritt übernimmt den neuen Körper und
das Solverergebnis erst nach erfolgreicher Berechnung.

## Gemeinsamer Solver für Körpergruppen

`ContactSolver.solve` bindet `ps_constraints_resolve_graph` ein. Kontakte und
Distanzgelenke wirken innerhalb derselben Geschwindigkeitsiteration auf ein
Array von Körpern. Die Reihenfolge bleibt deterministisch: zuerst Kontakte,
dann Gelenke in der jeweiligen Eingabereihenfolge. Rückprallziele stammen aus
den Anfangsgeschwindigkeiten. Der Solver integriert keine Kräfte oder Bewegung.

| Aufruf / Eigenschaft | Ergebnis / Vertrag |
| --- | --- |
| `contacts.constraint(index, bodyA, bodyB)` | `ContactConstraint` für einen Kontaktpunkt; `index` ist nullbasiert |
| `joint.constraint(bodyA, bodyB)` | `JointConstraint` mit lokalen Ankern |
| `constraint.bodyA`, `constraint.bodyB` | Schreibgeschützte `Int64`-Indizes; nur B darf `-1` für die feste Welt sein |
| `contact.point`, `contact.normal`, `contact.penetration` | Schreibgeschützte Kontaktgeometrie in Weltkoordinaten beziehungsweise Metern |
| `constraint.joint` | Schreibgeschützte `DistanceJoint`-Kopie eines Gelenkconstraints |
| `solver.solve(bodies, contacts, joints, dt)` | `ConstraintResult` aus `[Body]`, `[ContactConstraint]`, `[JointConstraint]` und positiver endlicher Schrittweite in Sekunden |
| `result.bodyCount`, `result.contactCount`, `result.jointCount` | Schreibgeschützte Anzahlen |
| `result.body(index)` | Unabhängige Körperkopie am nullbasierten Index |
| `result.bodies()` | Alle berechneten Körper als unabhängiges `[Body]`-Array |
| `result.contactImpulse(index)`, `result.jointImpulse(index)` | Summierter Impuls auf A in N s, geordnet wie das zugehörige Eingabearray |
| `result.maxNormalError` | Größter Kontakt-Geschwindigkeitsrestfehler vor Positionsprojektion, in m/s |
| `result.maxProjectionError` | Größte unerfüllte Kontaktverschiebung, in m |
| `result.maxJointVelocityError` | Größter Gelenk-Geschwindigkeitsrestfehler vor Kontaktprojektion, in m/s |
| `result.maxJointLengthError` | Größter absoluter Gelenk-Längenfehler nach Kontaktprojektion, in m |

Höchstens 128 Körper, 512 einzelne Kontaktpunkte und 256 Distanzgelenke sind
zulässig. Bei der Bindung werden Indizes auf `0..127` beziehungsweise B=`-1`
geprüft; erst `solve` kennt die tatsächliche Körperanzahl und prüft alle
Referenzen gegen dieses Array. A und B müssen verschiedene Indizes sein.
Für B=`-1` ist der Gelenkanker B direkt ein Weltpunkt. Ein statischer Körper
im Array kann alternativ einen verschobenen oder gedrehten Anker tragen.

Die Geometrie muss zur aktuellen Anordnung und A/B-Reihenfolge passen. Nach
Körperbewegung Kontakte erneut erkennen und binden. Ein Kontaktarray enthält
einzelne Punkte; alle Punkte eines Quader-Manifolds werden in einer Schleife
über `contacts.count` gebunden. Es gibt keine automatische Kontaktverwaltung.

```text
let zero = Vec3(0,0,0)
var a = Body.sphere(1,0.5)
var b = a
a.setState(Vec3(0,0.5,0),Vec3(0,-1,0),Quat(0,0,0,1),zero)
b.setState(Vec3(0,1.5,0),Vec3(0,-1,0),Quat(0,0,0,1),zero)
let floor = Contacts.spherePlane(a,0.5,zero,Vec3(0,1,0))
let joint = DistanceJoint(zero,zero,1,0.2)
let settings = ContactSolver(128,0,0,0.5,0,1)
let result = settings.solve([a,b],[floor.constraint(0,0,-1)],
                            [joint.constraint(0,1)],0.01)
var bodies = result.bodies()
// Beide Körper werden gemeinsam vom Boden abgestützt.
assert(abs(bodies[0].velocity.y) < 1e-12)
assert(abs(bodies[1].velocity.y) < 1e-12)
```

Ergebnisse sind unveränderliche Werte mit automatisch verwaltetem Speicher,
auch als Felder eigener Strukturen, in Arrays, bei Rückgabe und Ersetzung.
Kopien teilen intern unveränderliche Daten, besitzen aber unabhängige Lebensdauer.
`bodies()` gibt ein eigenes Array zurück. Alle Allokationen zählen zum
64-MiB-Sprachbudget. Fehler bei Lösung oder Allokation verändern weder
Eingabekörper noch vorhandene Ergebnisse; Laufzeitfehler tragen die Quellposition.
Leere Arrays sind zulässig; `solve([],[],[],dt)` liefert ein leeres Ergebnis.

Ein erfolgreich beendetes Iterationsbudget garantiert keine Konvergenz. Auch
widersprüchliche Gelenke können Restfehler behalten. Die anschließende reine
Kontakt-Translationsprojektion kann Gelenklängen ändern; der letzte Restfehler
macht das sichtbar. Es gibt keine gemeinsame nichtlineare Gelenkprojektion,
kein Warmstart und keine automatische CCD-Schrittsteuerung. Insbesondere ersetzt der Solver keine explizite
Schrittweitenverfeinerung.

`constraint_graph.phys` enthält analytische Abstützungs-/Rückpralltests,
widersprüchliche Gelenke, Kopien und alle Kapazitätsgrenzen. `coupled_bodies.phys`
und die App-Vorlage **Gekoppelte Körper · Physim-Sprache** zeigen zwei verbundene
Kugeln auf einer Ebene. 16 Kanäle erfassen Bewegung, Energie, Kontaktanzahl,
Impulse und alle vier Restfehler. Jeder Schritt berechnet zunächst
Schwerkraftimpulse, löst den gemeinsamen Graphen und integriert danach die
Positionen; der neue Zustand wird erst nach erfolgreicher Berechnung übernommen.

## Sweep-Abfragen und Kollisionskandidaten

`Sweep`, `Aabb` und `CollisionPair` sind unabhängige, unveränderliche Werte der
gemeinsamen `collision.h`-Bibliothek. Sie sind in Programmen, Experimenten und
Analysen sowie in Arrays und eigenen Strukturen verfügbar.

| Aufruf / Eigenschaft | Ergebnis / Vertrag |
| --- | --- |
| `Sweep.spheres(bodyA, radiusA, displacementA, bodyB, radiusB, displacementB)` | Erster Kugel–Kugel-Kontakt entlang zweier linearer Verschiebungen in Metern |
| `Sweep.spherePlane(body, radius, displacement, point, normal)` | Erster Kugel–Ebene-Kontakt; Einheitsnormale zeigt in den freien Halbraum |
| `sweep.hit` | `Bool`, ob ein Kontakt im geschlossenen Intervall `0..1` liegt |
| `sweep.fraction()` | Anteil der Verschiebung bis zum ersten Kontakt; ohne Treffer Laufzeitfehler |
| `sweep.contacts()` | `Contacts` mit null oder einem Kontaktpunkt am Ereignis; Normalen zeigen A nach B |
| `Aabb.sphere(body, radius)`, `Aabb.box(body, size)` | Nach außen gerundete Welt-Hüllkörper für die aktuelle Lage; Quaderseitenlängen sind volle lokale Ausdehnungen |
| `Aabb.sweptSphere(body, radius, displacement)` | Hüllkörper der gesamten linearen Kugelbewegung |
| `bounds.minimum`, `bounds.maximum` | Schreibgeschützte `Vec3`-Grenzen in Weltkoordinaten, in Metern |
| `Aabb.pairs(bounds)` | `[CollisionPair]` für höchstens 1024 `[Aabb]`-Einträge |
| `pair.bodyA`, `pair.bodyB` | Schreibgeschützte `Int64`-Indizes des Eingabearrays, `bodyA < bodyB` |

Die Abfragen verwenden die angegebenen Verschiebungen, unabhängig von den
gespeicherten Geschwindigkeiten. Berührung zu Beginn und Anfangsüberlappung
liefern `hit=true` und Anteil null, auch wenn sich die Körper bereits trennen.
Am Intervallende ist Anteil eins möglich. Ein Treffer verändert keinen Körper.
Der Modellcode bewegt die Körper zuerst bis zum Ereignis, löst den Kontakt
und berechnet die verbleibende Bewegung mit den neuen Geschwindigkeiten.

```text
let zero = Vec3(0,0,0)
var body = Body.sphere(1,1)
body.setState(Vec3(0,10,0),Vec3(0,-20,0),Quat(0,0,0,1),zero)
let event = Sweep.spherePlane(body,1,body.velocity,zero,Vec3(0,1,0))
if event.hit:
    let first = event.fraction()
    if first > 0:
        body.step(zero,zero,first)
    let response = event.contacts().resolve(body,Body.sphere(0,1),ContactSolver(64,1,0,0,0,1))
    body = response.bodyA
    if first < 1:
        body.step(zero,zero,1 - first)
// Für dieses eine elastische Ereignis: y=12 m, vy=20 m/s, E=200 J.
```

Ein Kandidatenpaar ist noch kein geometrischer Kontakt. `Aabb.pairs` liefert
alle überlappenden Hüllen in lexikographischer Reihenfolge ohne Duplikate;
auch geschlossene Berührung zählt. Die Indizes beziehen sich auf die Reihenfolge
der Hüllen, nicht auf eine versteckte Körperidentität. Ausschlüsse, statische
Paare und Feinprüfung bleiben Aufgabe des Modells. Für CCD-Kandidaten reichen
Hüllen der Anfangslage nicht aus: `sweptSphere` berücksichtigt den gesamten Weg.
Ebenen sind unbeschränkt und werden separat geprüft.

Das Ergebnisarray besitzt eigene Lebensdauer; Änderungen an einer Kopie lassen
andere Kopien unverändert. Bei 1024 vollständig überlappenden Hüllen entstehen
523776 Paare. Speicher wird aus dem gemeinsamen 64-MiB-Sprachbudget bezogen;
leere Eingaben liefern ein leeres Array. Fehler verändern keine Eingaben.

Die Sweep-Funktionen berücksichtigen weder Beschleunigung noch gekrümmte Bahnen,
Quader-Sweeps oder automatische Restzeitsteuerung. Extreme Größenverhältnisse
begrenzen die numerische Genauigkeit. Für mehrere Ereignisse muss nach jeder
Antwort neu gesucht werden; gegen Nullzeit-Endlosschleifen ist eine zum Modell
passende Behandlung von Anfangsberührungen und ein explizites Ereignisbudget nötig.

`sweeps.phys` prüft Grenzfälle, Streifkontakte, bewegte Kugelpaare, Ereignisantwort,
Hüllkörper und den maximalen Kandidatensatz. Die App-Vorlage
**Schnelle Kugel · Physim-Sprache** beziehungsweise `fast_sphere.phys` beschreibt
eine Kugel mit 80 m/s zwischen zwei festen Wänden ohne äußere Kräfte. Sie löst
bis zu 16 eingehende Wandstöße pro Schritt und übernimmt den neuen Zustand erst
nach vollständig verarbeiteter Restzeit. Ein zu großer Schritt wird mit einer
Quelldiagnose abgebrochen. Anfangsberührungen mit trennender Geschwindigkeit
werden in diesem nicht überlappenden Modell übersprungen. Sieben Messkanäle
zeigen Position, Geschwindigkeit, Energie, Stoßzahl, Impuls, erste Ereigniszeit
als Schrittanteil und Gesamtstoßzahl.

## Sensoren und Messwerte

`Rng`, `Distribution`, `SensorConfig`, `Sensor` und `Measurement` sind kopierbare
Werttypen. Sie funktionieren in eigenständigen Programmen, Experimenten und
Analysen sowie in Arrays und eigenen Strukturen. Sensoren besitzen keine
gemeinsame veränderliche Identität: Nach einer Kopie können beide Sensoren
unabhängig weitergemessen werden. Die Bindungen verwenden `measurement.h`.

| Aufruf | Ergebnis / Vertrag |
| --- | --- |
| `Rng(seed)` | Eigener PCG32-Strom mit explizitem `Int64`-Seed |
| `Rng.forRun(stream)` | Nur Experiment: Seed aus vollständigem 64-Bit-Laufseed XOR `Int64`-Streamwert |
| `rng.sample(distribution)` | Mutierende `Float64`-Ziehung aus einer `Distribution` |
| `rng.reseed(seed)` | Mutierend: Strom auf den angegebenen Seed zurücksetzen |
| `rng.reseedForRun(stream)` | Mutierend, nur Experiment: Strom aus aktuellem Laufseed XOR Streamwert zurücksetzen |
| `Distribution.constant(value)` | Konstantes additives Rauschen |
| `Distribution.uniform(min, max)` | Gleichverteilung mit `min <= max` |
| `Distribution.normal(mean, standardDeviation)` | Normalverteilung mit nichtnegativer Standardabweichung |
| `distribution.mean()`, `distribution.standardDeviation()` | Momente als `Float64` |
| `SensorConfig(unit, rateHz, startTime, resolution, offset, driftPerSecond, noise, dropoutProbability, uncertaintyAbsolute, uncertaintyRelative)` | Geprüfte Konfiguration; Argumente auch vollständig benannt in beliebiger Reihenfolge |
| `Sensor(config, seed)` | Sensor mit explizitem `Int64`-Seed |
| `Sensor.forRun(config, stream)` | Experimentbindung: Seed ist der vollständige 64-Bit-Laufseed XOR dem `Int64`-Streamwert |
| `sensor.read(time, truth)` | Mutierende Messung; `time: Float64` in Sekunden, `truth: Quantity`, Ergebnis `Measurement` |
| `sensor.reset(seed)` | Mutierend: Zeitraster mit explizitem Seed zurücksetzen, Konfiguration erhalten |
| `sensor.resetForRun(stream)` | Mutierend, nur Experiment: Zurücksetzen mit aktuellem Laufseed XOR Streamwert |
| `sensor.nextTime()` | Nächster geplanter Zeitpunkt als `Float64` |
| `measurement.isDue()`, `isValid()`, `isDropped()` | Gültigkeitsprüfung als `Bool` |

`Rng(seed)` ist unabhängig vom Laufseed des Experiment-Hosts; `Rng.forRun(stream)`
und `reseedForRun(stream)` binden dagegen getrennte Ströme an diesen Seed. Der
`Int64`-Wert wird als vollständiges 64-Bit-Muster übernommen. `runSeed()` gibt
dasselbe Bitmuster zurück; Seeds mit gesetztem obersten Bit erscheinen dabei
als negative `Int64`-Werte. Kopien behalten ihren Stromzustand und
laufen danach unabhängig weiter. `sample` und `reseed` erfordern eine veränderliche
`var`-Bindung. Konstante und degenerierte Verteilungen verbrauchen keine Ziehung.
Bei ungültigen Parametern oder nicht endlichem Ergebnis meldet `sample` einen
Quellfehler, ohne den Strom weiterzuschalten. Das Beispiel
`examples/language/rng_streams.phys` prüft Replay, Kopien, degenerierte
Verteilungen und die Speicherung in Arrays, Strukturen und optionalen Werten.
`examples/language/rng_run_streams.phys` zeigt zwei unabhängige Laufströme.

Die Konfiguration verlangt eine gültige Einheit, eine positive endliche Rate,
einen nichtnegativen Startzeitpunkt und ein darstellbares Zeitraster. Auflösung,
absolute und relative Standardunsicherheit sind nichtnegativ; die
Ausfallwahrscheinlichkeit liegt in `[0, 1]`. Offset und Drift sind endliche,
vorzeichenbehaftete Größen in der Sensoreinheit beziehungsweise pro Sekunde.
`noise` ist eine `Distribution`. Die Standardunsicherheit kombiniert absolute
und relative Komponente, Rauschstandardabweichung und Auflösung/√12 quadratisch.
Dies setzt unabhängige Beiträge und gleichverteilten Quantisierungsfehler voraus.
Bekannte Offsets/Drift werden nicht als Unsicherheit behandelt oder korrigiert.

`Measurement` hat schreibgeschützte Eigenschaften: `value: Quantity`,
`time: Float64`, `standardUncertainty: Float64` in der Sensoreinheit sowie
`state`, `index` und `skipped` als `Int64`. Status `0` bedeutet noch nicht fällig,
`1` gültig und `2` ausgefallen. Nur bei Status `1` sind `value` und
`standardUncertainty` Messwerte; sonst sind ihre Zahlenwerte Nullplatzhalter.
Eine verspätete Abfrage misst zum tatsächlichen Zeitpunkt, zählt ausgelassene
Rasterplätze in `skipped` und erfindet keine vergangenen Messwerte. Die
Rasterindizes sind wie in der C-API auf weniger als 2^52 begrenzt.

```text
let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
let config = SensorConfig(unit: metres, rateHz: 100, startTime: 0,
    resolution: 0.005, offset: 0.01, driftPerSecond: 0.002,
    noise: Distribution.normal(0, 0.02), dropoutProbability: 0.05,
    uncertaintyAbsolute: 0.003, uncertaintyRelative: 0.001)
var sensor = Sensor(config, 42)
let measurement = sensor.read(time: 0, truth: Quantity(2, metres))
if measurement.isValid():
    assert(measurement.standardUncertainty > 0)
```

`read`, `reset` und `resetForRun` erfordern einen veränderlichen Zugriffspfad,
auch bei `group.sensors[index]`. Empfänger und Indizes werden einmal vor den
Argumenten ausgewertet. Erst nach erfolgreichem Aufruf wird die veränderte
Empfängerkopie zurückgeschrieben. Spätere Argument-Seiteneffekte an demselben
Wurzelwert werden dabei wie bei anderen mutierenden Methoden überschrieben.
Bei einem Fehler erfolgt keine Veröffentlichung dieser Empfängerkopie;
unabhängige, bereits ausgeführte Argument-Seiteneffekte bleiben bestehen.
Ungültige Konfigurationen, inkompatible Dimensionen, rückwärts laufende Zeit,
Indexüberschreitung oder nicht darstellbare Werte erzeugen Quelldiagnosen.

Explizite Seeds und Streamwerte werden modulo 2^64 konvertiert; `-1` bezeichnet
beispielsweise das Bitmuster `0xffffffffffffffff`. `forRun` und `resetForRun`
erhalten auch Laufseeds oberhalb von 2^63−1 unverändert. Gleicher Seed,
Rasterindex und Konfiguration reproduzieren dieselbe Messung; Sensorziehungen
verbrauchen keine Zufallszahlen des Modellgenerators. Ein Experiment muss seine
Sensoren in `reset` ausdrücklich zurücksetzen oder neu anlegen. Der Adapter
setzt eigene Sensorwerte nicht automatisch zurück.

Die App-Vorlage **Wurf mit Unsicherheit** verwendet bei Sprachwahl
`examples/language/uncertain_projectile.phys`. Sie zeichnet Sollbahn,
Modellbahn, Sensormessung,
Status und Standardunsicherheit auf. Die Namenskonvention `<kanal>.status`
verhindert, dass ungültige Werte in App-Statistiken gelangen. Rohdateien und
ungefilterte Datenreihen enthalten weiterhin sämtliche Zeilen; eigene Analysen
müssen den Status berücksichtigen. Die Vorlage verwendet dieselben Ströme,
Parameter und Kanäle wie `examples/uncertain_projectile/main.c`. Das
Vakuummodell besitzt keinen Luftwiderstand und keinen Bodenkontakt. Die Szene
zeigt die letzte gleichzeitig gültige x/y-Messung als Punkt.
Zwei kurze Linien machen den Messort auch neben der Modellkugel sichtbar.
`examples/language/sensors.phys` bleibt ein kleineres Sensorreferenzbeispiel
mit nur einer unsicheren Anfangsgeschwindigkeit und anderen Sensorparametern.

## Noch festzulegender semantischer Vertrag

LANG-001 ist erst vollständig, wenn folgende Regeln spezifiziert und durch
Compiler-Referenztests abgesichert sind:

- weitere Zahlenbreiten, explizite Konvertierungen und numerische Standardbibliothek;
- vollständige Kontrollfluss-/Initialisierungsregeln für weitere Werttypen und Module;
- weitere Array- und Stringfunktionen sowie weitere Lebensdauer-/API-Handle-Regeln;
- weiterer Strukturausbau (Überladungsauflösung für weitere Ausdrücke und Initialisierungsregeln), weitere Fallmuster, eigene generische Protokolle und erweiterte Modulauflösung;
- Fehlerweitergabe, bereinigte Ressourcen bei Fehlern und FFI-Grenzen;
- SI-Dimensionen als statische Typinformation und dynamische Dataset-Prüfungen;
- vollständige Experimentbindungen und Analyseadapter mit Provenienz und Quelldiagnosen.

Diese Punkte sind verbindlicher Ausbau, keine bereits implementierten Fähigkeiten.
Die Anwenderdokumentation erhält bei Integration zwei vollständige Teile:
„Physim mit C“ und „Physim mit der eigenen Sprache“. Bis dahin bleiben die
bestehenden C-Anleitungen gültig; dieses Dokument ist der Compilerentwurf.

## Szenengruppen und Elternbeziehungen

Im `scene`-Callback erzeugt `group(name: String, id: Int64, parent: Int64)` eine
benannte Gruppe. Eine Gruppe benötigt eine eindeutige nichtnull ID; Eltern-ID 0
steht für die Wurzel. `sceneParent(child: Int64, parent: Int64)` ordnet einen
vorhandenen Eintrag einer vorhandenen Eltern-ID zu oder löst ihn mit parent 0
zur Wurzel. Fehlende IDs, doppelte IDs, Selbstbeziehungen und Zyklen erzeugen eine
Quelldiagnose. Die geprüfte Änderung erhält die bisherige Szene bei Fehlern.

```physim
func scene():
    group("Versuch", 100, 0)
    group("Modell", 200, 100)
    sphere(Vec3(1, 0, 0), 0.2, 0x53DEC2FF, 1)
    sceneParent(1, 200)
```

Gruppen enthalten keine Geometrie. Alle Positionen und Richtungen bleiben in
Weltkoordinaten; es gibt hier keine automatische Elterntransformation. Gruppen
zählen zum Limit von 32 Szeneneinträgen. Die App zeigt Beziehungen im aufklappbaren
Szenenbaum und blendet beim Ausblenden eines Elternknotens seine Nachfahren mit
aus. Die eigene Sichtbarkeitswahl eines Kindes bleibt dabei erhalten.
Neue Sprachmodule melden die Hierarchiefähigkeit automatisch. Der aktuelle Runner
und Snapshotversion 2 zeichnen die Beziehungen mit auf. Ältere Szenenblöcke in
Version 1 bleiben als flache Szenen lesbar. [Dateiformat](data-format.md)


## Adaptive Runner-Schritte

Ein Experiment kann zusätzlich zu den vier Pflichtcallbacks anbieten:

```physim
func adaptiveStep(dt: Float64, minimum: Float64, maximum: Float64) -> StepInterval:
    let time = simulationTime()
    let result = rk45StepReported(derivative, state, time, time + dt,
                                  1e-10, 1e-8, 100000, dt, minimum, maximum)
    state = result.state
    measure()
    return StepInterval(result.reachedTime - time, result.nextStep)
```

`derivative` hat `(Float64, [Float64]) -> [Float64]`; `state` und `measure`
gehören zum eigenen Modell. Der Callback hat genau drei `Float64`-Parameter und
liefert `StepInterval`; generische Funktionen und Überladungen dieses Einstiegspunkts sind nicht erlaubt.
Der Compiler exportiert nur bei vorhandenem Callback die Adaptive-Capability.
Ohne adaptive Auswahl ruft der Runner weiterhin `step(dt)` auf.

`StepInterval` hat die schreibgeschützten `Float64`-Felder `elapsed` und
`nextStep` in Sekunden. Der Konstruktor verlangt positive endliche Werte.
Der Runner prüft zusätzlich die vorgegebenen Grenzen. Der Wert lässt sich in
Arrays, eigenen Strukturen, optionalen Werten und Funktionswerten verwenden.
Wie andere SDK-Aggregate ohne definierte Gleichheit hat er keine Inhaltsgleichheit.

`rk45StepReported` hat dieselben Eingaben und dieselben kopierenden Ergebniswerte
wie `rk45IntegrateReported`, beendet aber nach dem ersten akzeptierten Schritt.
`start` und `end` müssen verschieden sein. `result.acceptedSteps` ist bei Erfolg
1; `reachedTime` kann vor der gewünschten Zielzeit liegen. `nextStep` ist die
vorgeschlagene nächste Dauer, bei Rückwärtsintegration negativ. Verwerfungen
verbrauchen `maxSteps`; Fehler melden Diagnose und Quellposition. Zustand,
Messwerte und Zufallsströme erst nach erfolgreicher Integration aktualisieren.
Die Ableitung darf keine sichtbaren Seiteneffekte haben, weil sie auch für
verworfene Versuche aufgerufen wird. Lokale Fehlertoleranzen sind keine globale
Fehlergrenze und ersetzen keine Konvergenz- oder Erhaltungsprüfung.

Die fünf Pendelvarianten unter `examples/language/` zeigen diesen Callback.
Die Auswahl in der App und CLI ist im [Workspace-Handbuch](workspace.md#adaptive-simulationsschritte)
beschrieben. Die gespeicherten Daten enthalten die tatsächlich akzeptierten Zeiten;
Analysecode sollte diese Zeitwerte verwenden.


Die Pendelvorlagen definieren `length` und `initialAngle` über `parameterWithUnit(...)`
mit `m` beziehungsweise `rad`.
Die Werte wirken im festen und adaptiven Modus einschließlich Szenengeometrie;
die Standards bleiben 1,5 m und 0,45 rad. [Laufserien und Parameterstudien](monte-carlo.md#gemeinsame-zielzeit-und-adaptive-serien)
verwenden die gleichen Physim-Module wie Einzelversuche und speichern die tatsächlichen
akzeptierten Zeiten. Die unabhängige Analyse liest die Zeitspalte des Datensatzes.


## Parametereinheiten (Sprachvertrag 0.170.0)

`parameterWithUnit(name, unit, default, minimum, maximum, description)` ergänzt
`parameter` um eine ausdrückliche Anzeigeeinheit. Der Rückgabewert sowie Standard,
Grenzen und Overrides sind SI-Zahlen. Die Skala gehört zur Anzeige und konvertiert
keine Modellvariablen oder Messkanäle. Beispielsweise:

```physim
let centimetres = Unit(1, 0, 0, 0, 0, 0, 0, 0.01, "cm")
let length = parameterWithUnit("length", centimetres, 1.5, 0.1, 10, "Pendulum length")
```

`length` liefert hier standardmäßig 1,5 m; die App zeigt 150 cm und Grenzen
von 10 bis 1000 cm. Ein Override `--param length=0.5` bleibt 0,5 m. Die Funktion
ist wie `parameter` nur bei globaler Initialisierung oder in `create` erlaubt.
Ungültige Definitionen erhalten Quelldiagnosen. Das Symbol muss gültiges UTF-8
ohne Steuerzeichen sein und darf höchstens 15 Bytes belegen; Skala und alle
SI-Werte müssen endlich sein, die Anzeige darf weder unendlich werden noch nichtnullige SI-Werte zu Null runden.
Messkanäle verwenden weiterhin eigene kanonische SI-Einheiten mit Skala 1.

Deklarationen werden in den Laufmetadaten und in Studienberichten archiviert.
Die Anzeigeeinheit eines geladenen Berichts bleibt auch ohne Neubau verfügbar.
Untypisierte Parameter bleiben unbekannt; Dimensionen werden weder aus Namen
noch aus Beschreibungen abgeleitet. [Serien und Anzeigeeinheiten](monte-carlo.md#einheiten-von-parameterstudien)
beschreibt Formulare, CLI, SI-Persistenz und Berichte.


Sprachvertrag 0.172.0 ergänzt `Series.masked(selector, accepted)`, `validity()`,
`hasMask()` und `isValid(index)` für ausdrücklich erhaltene Messlücken.
Transformationen und Berichte übernehmen die Masken. `value`/`values` verlangen
gültige Beobachtungen im angeforderten Bereich. Numerische Regeln und Beispiele
stehen unter [Datenreihen mit Masken](series.md#messlücken-mit-expliziten-masken).
