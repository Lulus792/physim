# Array-Werte und Speicherlebensdauer

Entwurf 0.1, 2026-09-20. Dieser Vertrag beschreibt den implementierten Array-Ausbau.
Die Besitzbasis unter `include/physim/language_array.h` und die Aufräumkette in
`language_runtime.h` sind implementiert. Arraytypen, Literale, Indizes und
Elementzuweisungen werden statisch geprüft und in allen drei Ausgabearten nativ
übersetzt: eigenständige Programme, Experimente und Analysen.
Die internen C-Helfer sind keine stabile öffentliche ABI.

Eine `for`-Schleife über einen String durchläuft Unicode-Skalare in Quellreihenfolge.
Direkte String-`switch`-Fälle können feste geschlossene oder halboffene
Literalbereiche prüfen; die Grenzen vergleichen decodierte Skalarfolgen
lexikografisch.
`text.first` und `text.last` liefern den ersten beziehungsweise letzten
Unicode-Skalar als unabhängigen `String?`, bei leerem Text `nil`.
`text.sorted()` liefert ein `[String]` der Unicode-Skalare in aufsteigender
Skalarreihenfolge; `sorted(by:)` verwendet eine stabile Vergleichsfunktion
`func(String, String) -> Bool`. `min()` und `max()` liefern den kleinsten
beziehungsweise größten Skalar als `String?`; mit `by:` gilt die angegebene
Vergleichsfunktion. Ein leerer Text liefert `[]` beziehungsweise `nil`.
`prefix(n)`, `suffix(n)`, `dropFirst(n)` und `dropLast(n)` wählen Stringteile
nach Unicode-Skalaren. Die Drop-Methoden verwenden ohne Argument eins.
Negative Anzahlen sind Fehler, zu große Anzahlen werden begrenzt. Die
Ergebnisse sind unabhängige `String`-Werte; Allokationsfehler lassen die
Eingabe unverändert.
`prefix(while: predicate)` und `drop(while: predicate)` wählen eine
zusammenhängende Anfangsfolge beziehungsweise den Rest danach. Das Prädikat
`func(String) -> Bool` erhält je einen Unicode-Skalar als eigenen String und
wird ab dem ersten `false` nicht weiter aufgerufen. Beide Ergebnisse sind
unabhängige `String`-Werte.
`filter(predicate)` prüft mit `func(String) -> Bool` jeden Unicode-Skalar und
übernimmt passende Skalare in Quellreihenfolge in einen unabhängigen String.
Ein leerer String ruft das Prädikat nicht auf; Fehler räumen Teilwerte auf.
`map(transform)` nimmt `func(String) -> U` und liefert `[U]` mit einem
transformierten Wert je Unicode-Skalar. Besitzende Ergebnisse werden nach
ihren Wertregeln übernommen; Fehler räumen Teilwerte auf.
`compactMap(transform)` sammelt mit `func(String) -> U?` nur vorhandene Werte
als `[U]`. `flatMap(transform)` hängt die Teilarrays von
`func(String) -> [U]` in Quellreihenfolge zu `[U]` zusammen. `nil` und leere
Teilarrays tragen keine Elemente bei; Fehler räumen Teilwerte auf.
`reduce(initial, combine)` faltet Unicode-Skalare in Quellreihenfolge mit
`func(U, String) -> U`. Ein leerer String liefert den Anfangswert ohne
Funktionsaufruf; Fehler räumen den Akkumulator und temporäre Werte auf.
`forEach(body)` ruft `func(String) -> Void` auf jedem Unicode-Skalar in
Quellreihenfolge auf. Ein leerer String ruft die Funktion nicht auf. Der
Eingabestring bleibt während der Iteration ein Snapshot; temporäre Skalare
werden auch bei einem Fehler freigegeben.
`contains(where: predicate)` und `allSatisfy(predicate)` prüfen Unicode-Skalare
mit `func(String) -> Bool` in Quellreihenfolge. Die erste Methode endet beim
ersten `true`, die zweite beim ersten `false`. Für leere Strings liefern sie
`false` beziehungsweise `true`, ohne das Prädikat aufzurufen.
`first(where:)` und `last(where:)` liefern den ersten beziehungsweise letzten
passenden Skalar als `String?`. `firstIndex(where:)` und `lastIndex(where:)`
liefern dessen nullbasierten Skalarindex als `Int64?`. Die Suche nach dem
letzten Treffer prüft rückwärts und endet beim ersten Treffer. Ohne Treffer
liefern alle vier Methoden `nil`.
Jedes Element ist ein eigener unveränderlicher `String`-Wert. Der Eingabestring
wird einmal ausgewertet und als Momentaufnahme gehalten; Änderungen an der
ursprünglichen Variable beeinflussen die laufende Iteration nicht.

## Sprachvertrag

`[T]` bezeichnet einen homogenen Array-Wert mit variabler Länge. Zuweisung,
Parameterübergabe und Rückgabe kopieren den Wert. Änderungen an einer Kopie
dürfen keine andere Kopie verändern; das gilt auch für Arrays in Strukturen und
verschachtelte Arrays. `let` verhindert Änderungen entlang dieses Zugriffspfads.
Es gibt keine vom Sprachcode beobachtbare Speicheridentität und keine geliehenen
schreibbaren Elementreferenzen. Index und Länge verwenden `Int64`.
Negative Indizes sowie Indizes ab der Länge sind Laufzeitfehler mit Quellposition.
`array.count` liefert die Länge als schreibgeschützte `Int64`-Eigenschaft.
`attempt(array[index])` liefert dagegen `T?` für ein `[T]`-Array. Ein Index
außerhalb von `0..<array.count` ergibt `nil`; bei einem gültigen Index enthält
das Optional eine unabhängige Wertkopie, auch für Strings, Arrays, Strukturen
und bereits optionale Elemente. Empfänger und
Index werden in dieser Reihenfolge je einmal ausgewertet; Allokationsfehler
beim Kopieren werden ebenfalls von `attempt` abgefangen.
`array.contains(value)` sucht mit der strukturellen Gleichheit von `==` und
liefert `Bool`. Es funktioniert nur für vergleichbare Elementtypen. Empfänger
und Suchwert werden je einmal in dieser Reihenfolge ausgewertet; die Suche
endet beim ersten Treffer und verändert keinen Wert.
`array.firstIndex(of: value)` sucht in derselben Reihenfolge und liefert den
nullbasierten ersten Treffer als `Int64?`, sonst `nil`. Das optionale Ergebnis
gehört dem Aufrufer und wird wie andere optionale Werte aufgeräumt.
`array.lastIndex(of: value)` verwendet dieselbe Gleichheit, sucht vom Ende aus
und liefert den nullbasierten letzten Treffer als `Int64?`, sonst `nil`.
Empfänger und Suchwert werden ebenfalls je einmal ausgewertet.
`array.reversed()` erzeugt einen unabhängigen Wert mit umgekehrter
Elementreihenfolge. Auch verschachtelte und besitzende Elemente werden nach
ihren Wertkopierregeln übernommen; ein Kopierfehler räumt den Teilwert auf.
`array.reverse()` ändert einen `var`-Zugriffspfad nach denselben Wertregeln.
Erst wenn alle Elementkopien gelungen sind, wird der neue Wert veröffentlicht;
vorherige Snapshots und der Empfänger bei Fehlern bleiben unverändert.
`array.sorted()` erstellt für `[Int64]`, `[Float64]` und `[String]` eine
unabhängige, aufsteigend sortierte Kopie. `array.sort()` ändert einen
veränderlichen Zugriffspfad erst nach vollständigem Kopieren und Sortieren.
Gleiche Werte dürfen ihre interne Reihenfolge tauschen; ihre beobachtbaren Werte
sind gleich. Strings folgen der lexikografischen Unicode-Skalarfolge. Nicht
endliche `Float64`-Elemente werden abgewiesen.
`array.sorted(by: compare)` und `array.sort(by: compare)` sortieren beliebige
Elementtypen anhand von `func(T, T) -> Bool`. Sie ordnen stabil: Bei gleichem
Rang bleibt die ursprüngliche Reihenfolge erhalten. Die Sortierung arbeitet auf
einer eigenen Elementkopie und nutzt einen budgetierten Hilfspuffer. Scheitert
eine Kopie, Allokation oder ein Vergleich, bleibt der mutierende Empfänger
unverändert; sichtbare Seiteneffekte der Vergleichsfunktion bleiben bestehen.
`array.min()` und `array.max()` liefern für `Int64`, `Float64` und `String`
das Extremum als unabhängiges `T?`, bei einem leeren Array `nil`. Mit
`min(by:)` und `max(by:)` kann eine Funktion `func(T, T) -> Bool` Elemente
beliebiger Typen ordnen. Gleichstände behalten das erste Element; die Suche
kopiert erst das ausgewählte Element und lässt den Arraywert unverändert.
`array.append(element)` erweitert einen veränderlichen Array-Wert direkt.
`array.remove(at: index)` entfernt ein Element und gibt den entfernten Wert zurück.
Beide Methoden benötigen einen vollständig veränderlichen Zugriffspfad, etwa
`state.particles.append(particle)` oder `groups[index].append(value)`.
`array.removeAll()` leert den veränderlichen Arraypfad. Die Form
`array.removeAll(where: predicate)` prüft jedes Element des Snapshots einmal
und behält die übrigen Elemente in ihrer Reihenfolge. Bei einem Fehler im
Prädikat oder beim Kopieren bleibt der Empfänger unverändert; der Auswahlpuffer
und unvollständige Kopien werden freigegeben.
`compactMap(transform)` wendet `func(T) -> U?` in Reihenfolge auf jedes Element
an und erstellt einen eigenständigen `[U]`-Wert aus den vorhandenen Resultaten.
Ein `nil`-Resultat wird übersprungen. Die Funktion sieht einen Snapshot des
Eingabearrays; temporäre optionale Ergebnisse werden nach dem Kopieren
freigegeben.
`flatMap(transform)` wendet `func(T) -> [U]` an und hängt die Teilarrays in
Reihenfolge zu einem eigenständigen `[U]` zusammen. Leere Teilarrays werden
übersprungen; jedes temporäre Teilarray wird nach dem Kopieren freigegeben.
Die Transformation sieht einen Snapshot des Eingabearrays.
`forEach(body)` ruft `func(T) -> Void` auf jedem Element in Arrayreihenfolge
auf und liefert `Void`. Ein leeres Array ruft die Funktion nicht auf. Die
Iteration sieht einen Snapshot des Empfängers, auch wenn der Funktionskörper
die ursprüngliche Arrayvariable ändert.
Die vier nicht mutierenden Methoden `map`, `filter`, `compactMap` und `flatMap`
bauen ihre Ergebnisse in einem privaten Puffer mit geometrischem Wachstum auf.
Bereits kopierte Elemente werden nur bei einer Vergrößerung erneut kopiert.
Schlägt eine Allokation oder Elementkopie fehl, wird das Teilresultat mit seinen
besitzenden Elementen freigegeben; veröffentlichte Arrays bleiben unverändert.
`prefix(n)` und `suffix(n)` wählen bis zu `n` Elemente am Anfang oder Ende aus.
`dropFirst(n)` und `dropLast(n)` lassen bis zu `n` Elemente weg; ohne Argument
lassen sie eines weg. Negative Anzahlen sind Fehler, größere Anzahlen werden
auf die Arraylänge begrenzt. Das Ergebnis ist ein eigenständiges `[T]` mit
Indexbeginn null. Auch besitzende Elemente und verschachtelte Arrays werden
unabhängig kopiert; Kopierfehler verändern die Eingabe nicht.
`prefix(while: predicate)` kopiert passende Anfangselemente bis zum ersten
`false`; `drop(while: predicate)` kopiert ab diesem Element den Rest. Das
Prädikat wird ab dem ersten `false` nicht mehr aufgerufen. Beide Ergebnisse
sind eigenständige `[T]`-Werte, auch wenn Elemente Besitz tragen.
`removeFirst()` und `removeLast()` geben das entfernte Endelement als eigenen
Wert zurück; auf leeren Arrays sind sie Fehler. Mit einer positionalen
`Int64`-Anzahl entfernen sie die entsprechende Zahl von Elementen und liefern
`Void`. Null ist erlaubt, negative oder zu große Anzahlen sind Fehler.
Fehlschlagende Kopien lassen den mutierenden Arraywert unverändert.
`swapAt(i, j)` vertauscht zwei vorhandene Elemente eines veränderlichen Arrays
und liefert `Void`. Gleiche Indizes sind wirkungslos, ungültige Indizes Fehler.
Der Tausch erstellt wegen der Wertsemantik eine unabhängige Arraykopie; bei
Kopierfehlern bleibt der alte Wert einschließlich seiner Snapshots erhalten.
`let`-Werte, Funktionsparameter und temporäre Ausschnitte sind keine mutierbaren
Empfänger. `append` und `insert` liefern `Void`; `remove` liefert den Elementtyp.
`array.insert(value, at: index)` fügt unmittelbar vor `index` ein. Der Index
darf auch `array.count` sein, um am Ende einzufügen.
`append(contentsOf: values)` und `insert(contentsOf: values, at: index)`
übernehmen alle Elemente eines gleich typisierten Arrays. Selbstkopien und
leere Eingaben sind erlaubt. Die neue Folge wird erst nach erfolgreichem
Kopieren veröffentlicht; der bisherige Wert und seine Snapshots bleiben bei
einem Fehler erhalten.

`removeSubrange(start..<end)` entfernt einen Bereich; mit
`replaceSubrange(start..<end, with: values)` wird er durch ein Array desselben
Elementtyps ersetzt. Beide Methoden akzeptieren auch geschlossene Bereiche.
Bei ungültigen Grenzen oder Kopierfehlern bleibt der Empfänger unverändert.

`left + [element]` liefert einen neuen Array-Wert mit dem zusätzlichen Element
am Ende. Für eine geänderte Kopie kann ein Array einem neuen `var`-Wert
zugewiesen und dort mit `append`, `remove(at:)` oder `insert(_:at:)` verändert
werden. Die Einfügeform verlangt einen `Int64`-Index
zwischen null und der Länge einschließlich.
Das Element muss zum Arraytyp passen; der Entfernungsindex ist `Int64` und muss
ein vorhandenes Element bezeichnen. Auch Entfernen aus einem leeren Array ist
ein Laufzeitfehler. Der Empfänger einschließlich aller Indizes wird vor dem
Argument einmal ausgewertet. Beim Einfügen werden Wert und Index nach dem
Empfänger in dieser Reihenfolge je einmal ausgewertet. Mutierende Methoden
arbeiten auf diesem Snapshot
und veröffentlichen den vollständig aufgebauten Wurzelwert nach erfolgreicher
Änderung. Seiteneffekte des Arguments am selben Wurzelwert werden dadurch beim
Veröffentlichen überschrieben, wie bei indizierten Zuweisungen. Bei Fehlern wird
die Methodenänderung nicht veröffentlicht; andere Seiteneffekte bleiben bestehen.
Verschachtelte Werte behalten dieselben Kopier- und Fehlerregeln. Leere Empfänger
brauchen einen bekannten Elementtyp; angehängte Werte erhalten diesen Typkontext.

`array[start..<end]` liefert einen Ausschnitt mit ausgeschlossener Obergrenze;
`array[start...end]` schließt das letzte Element ein. Angegebene Grenzen sind `Int64`.
Es gilt `0 <= start <= end <= array.count` für `..<`, für `...` muss
zusätzlich `end < array.count` gelten. Negative, umgekehrte oder zu große
Grenzen sind Laufzeitfehler; sie werden nicht still gekürzt. `array[n..<n]`
ist für `0 <= n <= array.count` leer, auch bei einem leeren Array mit `n = 0`.
Ein beidseitig begrenzter geschlossener Bereich ist niemals leer.
Die Formen `array[..<end]` und `array[...end]` lassen die Untergrenze null
weg. `array[start..<]` und `array[start...]` reichen bis zur Arraylänge;
`array[..<]` und `array[...]` erfassen das ganze Array. Ein fehlendes Ende
hat unabhängig vom Operator eine exklusive effektive Obergrenze, sodass ein
leeres Array auch mit `array[...]` einen leeren Ausschnitt ergibt. Explizite
geschlossene Obergrenzen behalten die Pflicht, ein Element zu bezeichnen.
Diese Kurzformen gelten auch bei Bereichszuweisungen, aber nicht für
eigenständige Bereichswerte oder `for`-Schleifen.

Der Ausschnitt hat denselben `[T]`-Typ und beginnt wieder bei Index null.
Er besitzt seinen Inhalt unabhängig von späteren Änderungen am Original.
Das Array, die Untergrenze und die Obergrenze werden einmal in dieser Reihenfolge
ausgewertet; Seiteneffekte in den Grenzen verändern den bereits gehaltenen
Array-Snapshot nicht. `var part = array[1..<3]` erlaubt Änderungen an `part`.
Ein Ausschnitt selbst ist kein schreibbarer Zugriffspfad: `array[1..<3][0] = 4`
wird abgewiesen. `array[1..<3] = replacement` ersetzt dagegen auf einem
veränderlichen Arraypfad den ganzen Bereich. Das Ersatzarray muss denselben
`[T]`-Typ haben; seine Länge darf abweichen. Auch leere Bereiche und leere
Ersatzarrays sind erlaubt. Empfänger und Grenzen werden je einmal vor dem
Ersatzarray ausgewertet; ungültige Grenzen verhindern dessen Auswertung.
Die Änderung wird nach erfolgreicher Konstruktion des neuen Blocks durch
den gesamten Feld-/Indexpfad veröffentlicht. Bei Fehlern entfällt diese
Veröffentlichung; vorangegangene Seiteneffekte bleiben bestehen.
Die Laufzeit kopiert die ausgewählten Elemente in einen neuen Arrayblock;
besitzende Elementfelder werden dabei referenzgezählt übernommen. Speicherbedarf
und Laufzeit wachsen mit der Ausschnittlänge, unter dem gemeinsamen Arraybudget.
`array[start..<end by step]` und die geschlossene Form wählen jeden
`step`-ten Index; auch ausgelassene Grenzen sind möglich. `step` muss ein
positiver `Int64`-Wert sein. Array und Grenzen werden vor dem Schritt je einmal
ausgewertet. `array[... by 2] = replacement` ersetzt nur die ausgewählten
Positionen, behält die Arraylänge bei und verlangt genau einen Ersatzwert pro
Position. Ein Fehler bei Länge, Allokation oder Kopie verändert den Arraywert
nicht. Verkettendes `+=` ist auf einem Bereich mit Schrittweite ausgeschlossen.

`for element in array:` durchläuft die Werte direkt; für veränderliche
Elementzugriffe dient `for index in 0..<array.count:`.

Der Iterable-Ausdruck wird einmal vor Schleifenbeginn ausgewertet. Die Schleife
hält eine Wertkopie dieses Arrays bis zum Schleifenende. Änderungen an der
ursprünglichen Variable beeinflussen weder Elemente noch Anzahl der laufenden
Iteration. Die Schleifenvariable ist unveränderlich, hat den Elementtyp und lebt
nur im Schleifenkörper. Das gilt auch für Strukturfelder und innere Arrays;
mit `var copy = element` kann eine veränderliche Kopie erzeugt werden.
Leere Arrays führen den Körper nicht aus. `break`, `continue` und `return`
geben temporäre Besitzer und Elementkopien an den jeweiligen Bereichsgrenzen frei.

## Bereits implementierte Typprüfung

Arraytypen sind strukturell: Zwei getrennte Annotationen `[T]` bezeichnen
denselben Typ, wenn ihre Elementtypen gleich sind. Eigene Struktur- und Enumtypen
behalten dabei ihre nominale Identität. Arraytypen werden ohne zusätzliche
Allokation im semantischen Knotenpuffer kanonisiert; Vergleiche sind durch das
gemeinsame Arbeitsbudget begrenzt. Verschachtelte Arrayannotationen unterliegen
derselben Tiefengrenze wie andere semantische Besuche.

Leere Literale benötigen einen Elementtyp aus einer Annotation oder dem Kontext,
etwa `let a: [[Float64]] = [[], [1, 2]]`. Ohne Kontext liefert das erste Element
den Typ; bekannte Fließkommaausdrücke eines flachen Literals geben numerischen
Literalen einen `Float64`-Kontext. `[1, 2.5]` ist daher `[Float64]`, während eine
bereits deklarierte `Int64`-Variable dafür explizit konvertiert werden muss.
Ein zuerst auftretendes leeres inneres Array braucht den äußeren Typkontext.
`Void` und Bereiche sind keine Elementwerte. Ganze Arrays werden nicht
implizit konvertiert. `a + b` verkettet zwei Arrays
desselben `[T]`-Typs zu einem unabhängigen Wert; beide Operanden werden einmal
von links nach rechts ausgewertet. `+=` veröffentlicht diesen Wert über einen
veränderlichen Zugriffspfad. Bei `a[start..<end] += b` wird der Bereich nach
einmaliger Prüfung der Grenzen als Wert gelesen, verkettet und transaktional
ersetzt. Auch ein leerer Zielbereich ist erlaubt. Bei Allokations- oder
Grenzfehlern bleibt die Zielveröffentlichung aus; bereits erfolgte Seiteneffekte
bleiben bestehen. Die Größe des Ergebnisses unterliegt dem Arraybudget.
`==` und `!=` vergleichen Arrays gleicher Typen elementweise, auch bei
verschachtelten vergleichbaren Werten. Weitere Array-Rechenoperatoren sind
nicht definiert.

Indexausdrücke verlangen `Int64`. Elementzuweisungen verlangen entlang des
gesamten Namens-/Feld-/Indexpfads einen veränderlichen Besitzer. Ein Feld mit
`let`, ein unveränderlicher Parameter oder das Ergebnis eines Funktionsaufrufs
werden durch einen nachfolgenden Index nicht veränderlich. Beispielsweise ist
`cloud.particles[0].position.x = 4` nur bei vollständig veränderlichem Pfad gültig.
Strukturen dürfen sich über Arrayfelder referenzieren; diese besitzen einen
fest großen Handle und vergrößern das statische Layout nicht rekursiv. Direkte
rekursive Strukturwerte werden weiterhin als unendlich groß abgewiesen.

Das SDK-Beispiel `examples/language/particles.phys` bewegt drei Partikel mit
Array-/Strukturwerten in gleichförmiger Schwerkraft. Es prüft Position und
Geschwindigkeit nach 100 Schritten gegen die analytische Lösung und erhält den
ursprünglichen Zustand als unabhängige Kopie. Pendel und Analyse verwenden
ebenfalls Arraywerte über die vorhandenen Modul-ABIs.

## Speicherung und Änderungen

Die Implementierung darf Kopien durch gemeinsam genutzte unveränderliche Blöcke
darstellen. Ein Schreibzugriff veröffentlicht erst nach erfolgreichem Abschluss
einen neuen Block. Die erste Implementierung kopiert bei jeder Änderung den
betroffenen Block; eine spätere Optimierung für alleinige Besitzer darf die
beobachtbare Wertsemantik und Fehlergarantien nicht verändern.

Bei verschachteltem Schreiben wird zuerst das Element als Wert kopiert und
geändert. Danach wird es im äußeren Array ersetzt. Weitere äußere Ebenen werden
ebenso von innen nach außen aufgebaut. Erst der vollständig konstruierte Wert
wird dem Zuweisungsziel zugeordnet. Bei indizierten Zuweisungen wird zuerst der
Wurzelwert gelesen, danach werden die Indizes von außen nach innen genau einmal
ausgewertet und geprüft, zuletzt die rechte Seite. Verkürzte Zuweisungen verwenden
den bereits gelesenen Elementwert. Der neue Wurzelwert basiert auf diesem Snapshot.
Ändern Indexausdrücke oder die rechte Seite dieselbe Wurzel zusätzlich, überschreibt
die abschließende Zuweisung diese Änderungen mit dem aufgebauten Snapshot.
Andere Seiteneffekte werden bei Fehlern nicht zurückgenommen. Nicht indizierte
Feldzuweisungen behalten ihre bisherige Semantik und ersetzen nur das Zielfeld.

## Besitzbasis

Ein `psrt_array` besitzt einen Verweis auf einen unveränderlichen Block. Klonen
erhöht dessen geprüften Referenzzähler ohne Allokation. Leere Arrays haben keinen
Block. Ersetzen eines Bereichs konstruiert neue Elemente und übernimmt den neuen
Block erst nach vollständigem Erfolg. Ein Lesepointer bleibt bei einem Fehler
gültig; nach erfolgreicher Änderung darf ihn generierter Code nicht weiterverwenden.
Quellen dürfen aus dem alten Block stammen, insbesondere bei Selbst-Einfügen.

Elementbeschreibungen definieren Größe, Kopierkonstruktion und Zerstörung.
Skalare Werte brauchen nur Bytekopien. Array-Elemente behalten ihre eigenen
Blöcke; Struktur-Elemente müssen alle besitzenden Felder kopieren und bei einem
Fehler bereits kopierte Felder in umgekehrter Reihenfolge zerstören.
Eine fehlgeschlagene Elementkopie hinterlässt keine Ressourcen im Ziel.
Der Bibliothekswert `ConstraintResult` verwendet dieselbe Besitzbasis: Seine
unveränderlichen Solverdaten liegen in einem referenzgezählten Block. Der
Compiler behandelt ihn auch ohne sichtbares Array als Besitzer und erzeugt
passende Kopier-/Aufräumoperationen für Parameter, Rückgaben und Strukturfelder.
`result.bodies()` erzeugt ein unabhängiges Körperarray. Diese Solverdaten zählen
ebenfalls zum folgenden Sprachbudget; reine Körper- und Constraintwerte bleiben
bytekopierbare Werte ohne eigenen Speicherbesitz.
Destruktoren und Allocator-Callbacks dürfen keine Laufzeitfehler auslösen oder
mit `longjmp` abbrechen. Beschreibungen und Allocator-Kontexte überleben ihre Werte.

Jeder Block speichert seine Allocator-Domäne und exakte Allokationsgröße.
Längen-, Bytegrößen- und Referenzzählerüberlauf werden vor Mutation abgewiesen.
Allocator-Budgets gelten auch für den Spitzenbedarf aus altem und neuem Block.
Fehler erhalten alten Inhalt, Metadaten, geliehene Lesepointer und Besitzbilanz.
Generierte Programme und jede Modulinstanz besitzen eine eigene Allocator-Domäne
mit derzeit 64 MiB Spitzenbudget für Arrayblöcke einschließlich Headern und
gleichzeitig lebenden Kopien. Überschreitung oder fehlgeschlagene Allokation ist
ein Laufzeitfehler mit Quellposition. Das ist kein Budget für andere SDK-Daten,
den C-Stack oder den gesamten Prozess. Eine Benutzereinstellung dafür bleibt offen.
Gemeinsame Nutzung über Threads erfordert äußere Synchronisation.

## Aufräumen bei Rückkehr und Fehlern

Dieselbe Besitzverwaltung gilt für optionale Werte `T?`. Ein `nil` besitzt
einen leeren, typisierten Handle ohne Speicherblock; `Optional.some(value)`
besitzt einen unveränderlichen Block mit genau einem kopierten Wert. Die Hülle
wird beim Kopieren geteilt, `unwrap()` und `valueOr(fallback)` geben einen
eigenständigen Wert zurück. Optionale Inhalte zählen einschließlich Blockheader
zum gemeinsamen 64-MiB-Budget. Rekursive Strukturen über optionale Felder
werden wie rekursive Arraywerte iterativ freigegeben.
Die [Sprachanleitung](language.md) beschreibt Syntax und Zugriff.

Eine bedingte Bindung (`if let`/`if var`, `while let`/`while var`) besitzt eine
eigene Kopie des Inhalts im erfolgreichen Block. Ihr Initialisierer wird einmal
je Prüfung ausgewertet; die neue Variable ist darin noch nicht sichtbar.
Am Ende der Auswahl beziehungsweise des Schleifendurchlaufs werden sowohl
der gespeicherte optionale Quellwert als auch die gebundene Kopie freigegeben.
`return`, `break`, `continue` und Laufzeitfehler verwenden dieselbe Aufräumkette.
Eine mit `var` gebundene Kopie kann geändert werden, ohne die Quelle zu verändern.

Generierter Code trägt lokale Besitzer und temporäre Ergebnisse unmittelbar nach
ihrer Konstruktion in eine Aufräumkette ein. Ein Eintrag lebt auf dem
C-Stack und verweist auf das besitzende Objekt. Vor normalem Verlassen eines
Gültigkeitsbereichs, auch bei `return`, `break` oder `continue`, werden Einträge
bis zur beim Eintritt gespeicherten Marke in umgekehrter Reihenfolge zerstört.
Ein zurückgegebener Wert muss vorher einen eigenen Besitzverweis erhalten.

Bei einem abgefangenen Laufzeitfehler wird die Kette **vor** `longjmp` abgearbeitet,
solange sämtliche registrierten Stackobjekte noch leben. Jede Modul-Trap hat
ihre eigene Kette; eine innere Trap darf keine Besitzer einer äußeren zerstören.
Eigenständige Programme räumen vor dem Fehlerexit auf. Persistente Modulwerte
werden bei Modulabbau und fehlgeschlagener Initialisierung getrennt zerstört.
Analysen zerstören ihre globalen Werte nach jedem Lauf. Ein fehlgeschlagener
Experiment-Callback räumt temporäre Werte auf; persistente Werte bleiben bis zu
Reset/Abbau erhalten, entsprechend dem bisherigen Fehlerzustand des Moduls.
Generierte Strukturdeskriptoren kopieren besitzende Felder und rollen Teilkopien
zurück. Die Freigabe von Arrayblöcken nutzt eine intrusive Arbeitsliste statt
Rekursion durch den Wertgraphen; tiefe rekursive Arraywerte belasten dadurch nicht
zusätzlich den C-Aufrufstack. Temporäre Werte leben derzeit bis zum Ende des
umgebenden Blocks, Schleifendurchlaufs oder Funktionsaufrufs. Frühere Freigabe
innerhalb langer geradliniger Blöcke ist eine noch offene Optimierung.
