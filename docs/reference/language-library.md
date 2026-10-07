# Physim-Sprache: Bibliotheksreferenz

Alle hier aufgeführten Aufrufe sind im Compiler registriert. Die Signaturen zeigen die tatsächlich erlaubte Schreibweise: Empfängermethoden werden auf einem Wert aufgerufen, statische Fabriken auf dem Typ. `Void` bedeutet kein Rückgabewert. Parameter können positional oder mit den gezeigten Namen angegeben werden. Zahlen und Methodenempfänger werden statisch geprüft.

[Sprachanleitung und Beispiele](../language.md) · [Arrays und Wertsemantik](../language-values.md) · [Teil II – Physim](../physim-guide.md)

## Aufrufbeispiel

```text
let direction = Vec3(3, 4, 0)
let distance = direction.length()
let unitDirection = direction.normalized()
let metres = Unit(1, 0, 0, 0, 0, 0, 0, 1, "m")
let position = Quantity(2, metres)
```

Erwartung: distance ist 5, unitDirection ist (0.6, 0.8, 0). Für Experimente stehen globale Channel-Deklarationen sowie create/reset/step/scene bereit; Analysen implementieren analyze. Hostgebundene Funktionen sind nicht in eigenständigen Programmen verfügbar. Ungültige Argumente werden je nach Fall beim Kompilieren oder mit einer Quelldiagnose zur Laufzeit abgewiesen. Die C-Bibliothek ist umfangreicher als die derzeitigen Sprachbindungen.

## Werte und Grenzen

Vektoren haben die Komponenten x/y/z/w, Quantity die Felder value und unit. Measurement enthält value (Quantity), standardUncertainty (Float64 in der Sensoreinheit), time (Float64 in Sekunden), state, index und skipped (Int64). Nur isValid() bestätigt einen verwendbaren Messwert. In der Szene gelten Meter, Y nach oben, höchstens 32 Objekte und insgesamt 96 Polyline-Punkte. Analysehandles leben im Analysecontext; nach release/close dürfen sie nicht erneut verwendet werden. Plotvorschauen sind auf 2048 Punkte pro Kurve begrenzt; export der Reihe erhält alle Werte. Arrayoperationen, eigene Methoden, Schleifen und Konvertierungen erklärt die Wertsemantik-Anleitung.

## Aabb.box

```text
Aabb.box(body: Body, size: Vec3) -> Aabb
```

Nach außen gepolsterte Welt-AABB eines gedrehten Quaders; size sind volle lokale Seitenlängen in m. minimum/maximum sind schreibgeschützt.

Überall verfügbar.

## Aabb.pairs

```text
Aabb.pairs(bounds: [Aabb]) -> [CollisionPair]
```

Kandidatenpaare aus höchstens 1024 AABBs, lexikographisch nach bodyA/bodyB sortiert, ohne Duplikate. bodyA<bodyB sind schreibgeschützte Indizes des Eingabearrays. Berührung zählt; Kandidaten benötigen geometrische Feinprüfung. Das unabhängige Ergebnisarray zählt zum Sprachspeicherbudget. Ebenen separat prüfen.

Überall verfügbar.

## Aabb.sphere

```text
Aabb.sphere(body: Body, radius: Float64) -> Aabb
```

Nach außen gerundete Welt-AABB einer Kugel. minimum/maximum sind schreibgeschützte Vec3-Werte in m.

Überall verfügbar.

## Aabb.sweptSphere

```text
Aabb.sweptSphere(body: Body, radius: Float64, displacement: Vec3) -> Aabb
```

Welt-AABB der gesamten linear verschobenen Kugel; für kontinuierliche Kandidatensuche statt alleiniger Anfangs-/Endhülle verwenden.

Überall verfügbar.

## Array(repeating:count:)

```text
Array(repeating: value, count: n) -> [T]  // value: T, n: Int64
```

Erzeugt `n` unabhängige Kopien eines Elements. Der Zähler muss nichtnegativ sein.

Überall verfügbar.

## Batch

```text
Batch(module: String, directory: String, channel: String, runs: Int64, steps: Int64, dt: Float64, seed: Int64, workers: Int64) -> Batch
```

Erzeugt eine besitzende unveränderliche Serienkonfiguration. Absolute Modul-/Ausgabepfade, 1..1000 Läufe, höchstens acht Worker und fünf Millionen Samples; Int64-Seeds verwenden ihre 64-Bit-Bitfolge.

Überall verfügbar.

## Batch.adaptive

```text
Batch.adaptive(minimumDt: Float64, maximumDt: Float64) -> Batch
```

Aktiviert adaptive Integration mit Minimum und Maximum in Sekunden; eine Zielzeit muss vorher gesetzt sein.

Überall verfügbar.

## Batch.cancelled

```text
Batch.cancelled() -> Bool
```

Ob die Serie kontrolliert unterbrochen wurde.

Überall verfügbar.

## Batch.code

```text
Batch.code() -> Int64
```

Gespeicherter ps_result des Controllers; vor run() null, daher auch executed() prüfen.

Überall verfügbar.

## Batch.completed

```text
Batch.completed() -> Int64
```

Zahl validierter und journalierter Läufe einschließlich fehlender Endwerte.

Überall verfügbar.

## Batch.directory

```text
Batch.directory() -> String
```

Kopierter Ausgabeordner der Konfiguration.

Überall verfügbar.

## Batch.dt

```text
Batch.dt() -> Float64
```

Fester beziehungsweise anfänglicher Zeitschritt in Sekunden.

Überall verfügbar.

## Batch.endTime

```text
Batch.endTime() -> Float64
```

Gemeinsame Zielzeit; 0 bedeutet feste Schrittanzahl.

Überall verfügbar.

## Batch.error

```text
Batch.error() -> String
```

Kopierte begrenzte Fehlermeldung des Controllers.

Überall verfügbar.

## Batch.executed

```text
Batch.executed() -> Bool
```

Ob dieser Snapshot ein Ausführungsergebnis besitzt.

Überall verfügbar.

## Batch.finished

```text
Batch.finished(index: Int64) -> Bool
```

Ob der Lauf validiert und journaliert ist, einschließlich Status 2.

Überall verfügbar.

## Batch.limits

```text
Batch.limits(timeout: Float64, memoryMiB: Int64) -> Batch
```

Setzt das Zeitlimit je Runner (höchstens 3600 Sekunden) und die Speichergrenze in MiB (0 deaktiviert, höchstens 16384).

Überall verfügbar.

## Batch.module

```text
Batch.module() -> String
```

Kopierter Pfad des Experimentmoduls.

Überall verfügbar.

## Batch.parameter

```text
Batch.parameter(name: String, value: Float64) -> Batch
```

Setzt oder ersetzt einen festen SI-Parameter in einer neuen Konfiguration; der ursprüngliche Batch bleibt unverändert.

Überall verfügbar.

## Batch.peakActive

```text
Batch.peakActive() -> Int64
```

Höchste beobachtete gleichzeitige Workerzahl.

Überall verfügbar.

## Batch.requireSuccess

```text
Batch.requireSuccess() -> Void
```

Fordert einen ausgeführten, vollständig abgeschlossenen Batch ohne Fehler oder Abbruch. Andernfalls entsteht eine abfangbare Quelldiagnose mit dem gespeicherten Ergebniscode.

Analysemodul erforderlich.

## Batch.resume

```text
Batch.resume(series: String, directory: String) -> Batch
```

Lädt eine unveränderte archivierte Konfiguration für einen neuen Ausgabeordner; Fingerprints und Checkpoint-Version werden geprüft. run() übernimmt geprüfte frühere Läufe.

Analysemodul erforderlich.

## Batch.reused

```text
Batch.reused() -> Int64
```

Zahl aus dem alten Archiv übernommener Läufe.

Überall verfügbar.

## Batch.run

```text
Batch.run() -> Batch
```

Startet eine neue archivierte Serie über den expliziten Analysehost. Liefert einen neuen Ergebnis-Snapshot; code/error/finished/status bleiben auch bei Laufzeitfehlern abfragbar.

Analysemodul erforderlich.

## Batch.runPath

```text
Batch.runPath(index: Int64) -> String
```

Kopierter Archivpfad am nullbasierten Laufindex; das liefert auch für noch nicht fertige Läufe nur einen Pfad.

Überall verfügbar.

## Batch.runUntil

```text
Batch.runUntil(completions: Int64) -> Batch
```

Pausiert nach der angegebenen Zahl validierter Abschlüsse. 0 oder runs führt die ganze Serie aus; ein früherer Stopp erzeugt ein wiederaufnehmbares Journal ohne Gesamtbericht.

Analysemodul erforderlich.

## Batch.runs

```text
Batch.runs() -> Int64
```

Anzahl konfigurierter Läufe.

Überall verfügbar.

## Batch.seed

```text
Batch.seed() -> Int64
```

Ursprüngliche 64-Bit-Seedfolge als Int64-Bitfolge.

Überall verfügbar.

## Batch.series

```text
Batch.series() -> Series
```

Erzeugt eine SI-Datenreihe ausschließlich aus gültigen Endwerten in Laufindex-Reihenfolge. Null gültige Endpunkte werden abgewiesen; Statistik, Quantile, Histogramme und Exporte verwenden die bestehenden Series-Bindungen.

Analysemodul erforderlich.

## Batch.source

```text
Batch.source(path: String) -> Batch
```

Archiviert die angegebene Quelldatei beim Start zusammen mit dem verwendeten Modul. Der Pfad wird kopiert.

Überall verfügbar.

## Batch.started

```text
Batch.started() -> Int64
```

Zahl neu gestarteter Worker.

Überall verfügbar.

## Batch.status

```text
Batch.status(index: Int64) -> Int64
```

Messstatus am nullbasierten Laufindex.

Überall verfügbar.

## Batch.statuses

```text
Batch.statuses() -> [Int64]
```

Besitzendes Array aller Laufstatus: 0 nicht fällig, 1 gültig, 2 verworfen.

Überall verfügbar.

## Batch.steps

```text
Batch.steps() -> Int64
```

Schritte pro Lauf beziehungsweise akzeptiertes Schrittbudget.

Überall verfügbar.

## Batch.sweep

```text
Batch.sweep(name: String, start: Float64, end: Float64) -> Batch
```

Konfiguriert eine lineare Parameterstudie mit mindestens zwei Läufen und endlichen verschiedenen Grenzen in SI; Konflikte mit festen Parametern werden abgewiesen.

Überall verfügbar.

## Batch.target

```text
Batch.target(endTime: Float64) -> Batch
```

Setzt eine gemeinsame positive Endzeit in Sekunden; steps bleibt das akzeptierte Schrittbudget.

Überall verfügbar.

## Batch.unit

```text
Batch.unit() -> Unit
```

Kanonische SI-Einheit des verifizierten Kanals. Das Symbol bleibt auch nach Freigabe der Batch-Kopie für die Modul-Lebensdauer gültig.

Überall verfügbar.

## Batch.valid

```text
Batch.valid() -> Int64
```

Zahl gültiger Endwerte.

Überall verfügbar.

## Batch.value

```text
Batch.value(index: Int64) -> Float64
```

Gültiger Endwert am nullbasierten Laufindex. Nicht vorhandene oder verworfene Werte werden abgewiesen.

Überall verfügbar.

## Batch.values

```text
Batch.values() -> [Float64]
```

Besitzendes Array gültiger Endwerte in Laufindex-Reihenfolge; fehlende Werte werden nicht ergänzt.

Überall verfügbar.

## Batch.workers

```text
Batch.workers() -> Int64
```

Konfigurierter Parallelitätsgrad.

Überall verfügbar.

## Bezier3

```text
Bezier3(start: Vec3, control1: Vec3, control2: Vec3, end: Vec3) -> Bezier3
```

Kubische räumliche Bézierkurve aus vier endlichen Kontrollpunkten. Der Wert ist unabhängig kopierbar. Alle Koordinaten verwenden dieselbe Längeneinheit des Aufrufers.

Überall verfügbar.

## Bezier3.position

```text
Bezier3.position(t: Float64) -> Vec3
```

Position bei dimensionslosem t im inklusiven Intervall [0, 1]. Die Auswertung verwendet die De-Casteljau-Implementierung der C-Bibliothek.

Überall verfügbar.

## Bezier3.splitLeft

```text
Bezier3.splitLeft(t: Float64) -> Bezier3
```

Linke Teilkurve von 0 bis t, mit eigenem Parameterbereich [0, 1]. Ungültiges t erzeugt eine Quelldiagnose.

Überall verfügbar.

## Bezier3.splitRight

```text
Bezier3.splitRight(t: Float64) -> Bezier3
```

Rechte Teilkurve von t bis 1, ebenfalls auf [0, 1] umparametrisiert.

Überall verfügbar.

## Bezier3.tangent

```text
Bezier3.tangent(t: Float64) -> Vec3
```

Ableitung der Position nach dem dimensionslosen Parameter t. Sie ist weder normiert noch eine physikalische Geschwindigkeit. Ein nicht darstellbares Ergebnis erzeugt eine Quelldiagnose.

Überall verfügbar.

## Body.applyImpulse

```text
Body.applyImpulse(impulse: Vec3, point: Vec3) -> Void
```

Wendet einen Impuls in N s an einem Weltpunkt in m an; verändert Translation und Rotation des Empfängers.

Überall verfügbar.

## Body.box

```text
Body.box(mass: Float64, size: Vec3) -> Body
```

Homogener Quader mit mass in kg und positiven vollen lokalen Seitenlängen size in m, anfangs ruhend im Ursprung. mass null erzeugt einen statischen Körper.

Überall verfügbar.

## Body.forceTorque

```text
Body.forceTorque(force: Vec3, point: Vec3) -> Vec3
```

Drehmoment in N m einer Weltkraft in N an einem Weltpunkt in m; verändert den Körper nicht.

Überall verfügbar.

## Body.kineticEnergy

```text
Body.kineticEnergy() -> Float64
```

Summe der translatorischen und rotatorischen kinetischen Energie in J.

Überall verfügbar.

## Body.pointVelocity

```text
Body.pointVelocity(point: Vec3) -> Vec3
```

Weltgeschwindigkeit in m/s an einem Weltpunkt in m, einschließlich Rotation.

Überall verfügbar.

## Body.setState

```text
Body.setState(position: Vec3, velocity: Vec3, orientation: Quat, angularVelocity: Vec3) -> Void
```

Setzt Position in m, Geschwindigkeit in m/s, Einheitsquaternion und Winkelgeschwindigkeit in rad/s. Verändert den Empfänger erst nach vollständiger Prüfung; statische Körper müssen ruhen.

Überall verfügbar.

## Body.sphere

```text
Body.sphere(mass: Float64, radius: Float64) -> Body
```

Homogene Vollkugel mit mass in kg und positivem radius in m, anfangs ruhend im Ursprung. mass null erzeugt einen statischen Körper.

Überall verfügbar.

## Body.step

```text
Body.step(force: Vec3, torque: Vec3, dt: Float64) -> Void
```

Bewegt den Empfänger mit Weltkraft in N und Drehmoment in N m um positives dt in s. Symplektische Translation, explizite gyroskopische Rotation; Schrittweite verfeinern.

Überall verfügbar.

## Channel

```text
Channel(name: String, unit: Unit, description: String) -> Channel
```

Registriert einen skalaren Messkanal; nur beim Anlegen des Experiments verwenden, nicht in step oder scene.

Experimentmodul erforderlich.

## Channel.sample

```text
Channel.sample(value: Float64) -> Void
```

Setzt den aktuellen Messwert des Kanals in seiner deklarierten Einheit. Auch den Anfangswert in reset setzen.

Experimentmodul erforderlich.

## Collider.body

```text
Collider.body() -> Int64
```

Nullbasierter Körperindex.

Überall verfügbar.

## Collider.box

```text
Collider.box(id: Int64, body: Int64, size: Vec3) -> Collider
```

Erzeugt einen Boxcollider mit stabiler ID, Körperindex und positiven vollständigen Ausmaßen in Metern.

Überall verfügbar.

## Collider.id

```text
Collider.id() -> Int64
```

Stabile Collider-ID in 1..4294967295.

Überall verfügbar.

## Collider.normal

```text
Collider.normal() -> Vec3
```

Lokale Ebenen-Einheitsnormale; andere Formen null.

Überall verfügbar.

## Collider.plane

```text
Collider.plane(id: Int64, body: Int64, normal: Vec3) -> Collider
```

Erzeugt eine Ebene mit stabiler ID, Körperindex und lokaler Einheitsnormale in den freien Halbraum; der zugeordnete Körper muss statisch sein.

Überall verfügbar.

## Collider.shape

```text
Collider.shape() -> Int64
```

Formnummer: Kugel=1, Box=2, Ebene=3.

Überall verfügbar.

## Collider.size

```text
Collider.size() -> Vec3
```

Kugelradius in x beziehungsweise volle Boxausmaße, Meter; Ebene null.

Überall verfügbar.

## Collider.sphere

```text
Collider.sphere(id: Int64, body: Int64, radius: Float64) -> Collider
```

Erzeugt einen Kugelcollider mit stabiler nichtnull u32-ID, Körperindex 0..127 und positivem Radius in Metern.

Überall verfügbar.

## ConstraintResult.bodies

```text
ConstraintResult.bodies() -> [Body]
```

Liefert alle berechneten Körper als unabhängiges Array; dessen Änderungen beeinflussen den Ergebniswert nicht.

Überall verfügbar.

## ConstraintResult.body

```text
ConstraintResult.body(index: Int64) -> Body
```

Liefert eine unabhängige Körperkopie am nullbasierten Index; index muss kleiner als bodyCount sein.

Überall verfügbar.

## ConstraintResult.contactImpulse

```text
ConstraintResult.contactImpulse(index: Int64) -> Vec3
```

Gesamter Impuls auf Körper A des nullbasierten Kontaktconstraints in N s. Reihenfolge wie das übergebene Kontaktarray.

Überall verfügbar.

## ConstraintResult.jointImpulse

```text
ConstraintResult.jointImpulse(index: Int64) -> Vec3
```

Gesamter Impuls auf Körper A des nullbasierten Gelenkconstraints in N s. Reihenfolge wie das übergebene Gelenkarray.

Überall verfügbar.

## ContactResult.impulse

```text
ContactResult.impulse(index: Int64) -> Vec3
```

Resultierender Kontaktimpuls auf A in N s am nullbasierten Index; index muss kleiner als result.count sein.

Überall verfügbar.

## ContactSolver

```text
ContactSolver(iterations: Int64, restitution: Float64, friction: Float64, bounceThreshold: Float64, penetrationSlop: Float64, correctionFraction: Float64) -> ContactSolver
```

Geprüfte Einstellungen: iterations in 1..256, restitution in 0..1, nichtnegative friction, bounceThreshold in m/s und penetrationSlop in m sowie correctionFraction in 0..1. Alle Eigenschaften sind schreibgeschützt.

Überall verfügbar.

## ContactSolver.defaults

```text
ContactSolver.defaults() -> ContactSolver
```

Standardwerte des gemeinsamen iterativen Kontaktpaarsolvers.

Überall verfügbar.

## ContactSolver.solve

```text
ContactSolver.solve(bodies: [Body], contacts: [ContactConstraint], joints: [JointConstraint], dt: Float64) -> ConstraintResult
```

Löst Kontakte und Distanzgelenke gemeinsam für unabhängige Körperkopien. Maximal 128 Körper, 512 Kontakte und 256 Gelenke; dt muss positiv sein. Körperindizes müssen zum Array passen. Eingaben bleiben bei Erfolg und Fehler unverändert. Ergebnis besitzt automatisch verwalteten Speicher im Sprachbudget. bodyCount/contactCount/jointCount sowie maxNormalError, maxProjectionError, maxJointVelocityError und maxJointLengthError sind schreibgeschützt. Ein erfolgreicher Aufruf garantiert keine Konvergenz.

Überall verfügbar.

## ContactSolver.solveWarm

```text
ContactSolver.solveWarm(bodies: [Body], contacts: [ContactConstraint], initialImpulses: [Vec3]) -> ConstraintResult
```

Löst einen manuellen Kontaktgraphen mit genau einem endlichen Startimpuls auf A je Kontakt. Der aktuelle Reibungskegel begrenzt Seeds; Restitution verwendet die Geschwindigkeiten vor allen Seeds. Keine Gelenke.

Überall verfügbar.

## ContactWorld

```text
ContactWorld(matchDistance: Float64, minimumNormalDot: Float64, maximumDtRatio: Float64, warmFraction: Float64) -> ContactWorld
```

Erzeugt einen besitzenden, unveränderlichen Kontaktzustand mit expliziten Zuordnungs- und Warmstartgrenzen; alle vier Einstellungen folgen dem C-Vertrag.

Überall verfügbar.

## ContactWorld.bodies

```text
ContactWorld.bodies() -> [Body]
```

Liefert einen unabhängigen Arraywert der gelösten Körper; Änderungen daran ändern den Kontaktzustand nicht.

Überall verfügbar.

## ContactWorld.body

```text
ContactWorld.body(index: Int64) -> Body
```

Körperwert am geprüften nullbasierten Index. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.bodyCount

```text
ContactWorld.bodyCount() -> Int64
```

Anzahl gespeicherter Körper. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.colliderCount

```text
ContactWorld.colliderCount() -> Int64
```

Anzahl gespeicherter Collider. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.contactCount

```text
ContactWorld.contactCount() -> Int64
```

Anzahl erzeugter Kontaktpunkte. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.contactIdA

```text
ContactWorld.contactIdA(index: Int64) -> Int64
```

Stabile ID von Kontaktpartner A. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.contactIdB

```text
ContactWorld.contactIdB(index: Int64) -> Int64
```

Stabile ID von Kontaktpartner B; größer als A. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.contactImpulse

```text
ContactWorld.contactImpulse(index: Int64) -> Vec3
```

Gesamter Impuls auf A einschließlich Warmstart in N s. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.contactNormal

```text
ContactWorld.contactNormal(index: Int64) -> Vec3
```

Einheitsnormale von A nach B. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.contactPenetration

```text
ContactWorld.contactPenetration(index: Int64) -> Float64
```

Eindringtiefe in Metern vor Projektion. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.contactPoint

```text
ContactWorld.contactPoint(index: Int64) -> Vec3
```

Kontaktpunkt in Weltmetern vor Projektion. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.created

```text
ContactWorld.created() -> Int64
```

Neu erzeugte Kontakte ohne Zuordnung. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.defaults

```text
ContactWorld.defaults() -> ContactWorld
```

Erzeugt einen leeren Kontaktzustand mit den C-Standardwerten; keine implizite Integration.

Überall verfügbar.

## ContactWorld.dt

```text
ContactWorld.dt() -> Float64
```

Letzte erfolgreiche Schrittweite in Sekunden, leer null. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.ended

```text
ContactWorld.ended() -> Int64
```

Seit dem vorigen Schritt ausgelaufene Kontakte. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.localAnchorA

```text
ContactWorld.localAnchorA(index: Int64) -> Vec3
```

Lokaler Körperanker auf A vor Projektion. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.localAnchorB

```text
ContactWorld.localAnchorB(index: Int64) -> Vec3
```

Lokaler Körperanker auf B vor Projektion. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.matched

```text
ContactWorld.matched() -> Int64
```

Eins zu eins zugeordnete Kontakte. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.maxNormalError

```text
ContactWorld.maxNormalError() -> Float64
```

Größter Normalgeschwindigkeitsrest in m/s vor Positionsprojektion. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.maxProjectionError

```text
ContactWorld.maxProjectionError() -> Float64
```

Nicht erfüllte Positionskorrektur in Metern. Der Snapshot bleibt unverändert.

Überall verfügbar.

## ContactWorld.reset

```text
ContactWorld.reset() -> ContactWorld
```

Liefert einen neuen leeren Snapshot mit denselben Einstellungen; der ursprüngliche Zustand und seine Kopien bleiben erhalten.

Überall verfügbar.

## ContactWorld.solve

```text
ContactWorld.solve(bodies: [Body], colliders: [Collider], solver: ContactSolver, dt: Float64) -> ContactWorld
```

Erzeugt diskrete Kugel-/Box-/Ebenenkontakte und löst den warmen C-Graphen. Liefert einen neuen Snapshot mit Körpern, Kontaktverlauf und Resten; Eingaben bleiben erhalten.

Überall verfügbar.

## ContactWorld.warmed

```text
ContactWorld.warmed() -> Int64
```

Zugeordnete Kontakte mit nichtnull Warmseed vor Kegelprojektion. Der Snapshot bleibt unverändert.

Überall verfügbar.

## Contacts.boxPlane

```text
Contacts.boxPlane(body: Body, size: Vec3, point: Vec3, normal: Vec3) -> Contacts
```

Kontaktpunkte eines orientierten Quaders mit einer Ebene. size sind volle lokale Seitenlängen; normal zeigt zur freien Seite.

Überall verfügbar.

## Contacts.boxes

```text
Contacts.boxes(bodyA: Body, sizeA: Vec3, bodyB: Body, sizeB: Vec3) -> Contacts
```

Kontaktpunkte zweier orientierter Quader; sizeA/sizeB sind volle lokale Seitenlängen in m.

Überall verfügbar.

## Contacts.constraint

```text
Contacts.constraint(index: Int64, bodyA: Int64, bodyB: Int64) -> ContactConstraint
```

Bindet einen nullbasierten Kontaktpunkt an Körperindizes bodyA/bodyB. Nur bodyB darf -1 für die feste Welt sein. Gleiche oder außerhalb 0..127 liegende Indizes sind ungültig. Die schreibgeschützten Eigenschaften sind bodyA, bodyB, point, normal und penetration.

Überall verfügbar.

## Contacts.normal

```text
Contacts.normal(index: Int64) -> Vec3
```

Einheitsnormale des nullbasierten Kontakts von A nach B. index muss kleiner als count sein.

Überall verfügbar.

## Contacts.penetration

```text
Contacts.penetration(index: Int64) -> Float64
```

Nichtnegative Eindringtiefe des nullbasierten Kontakts in m.

Überall verfügbar.

## Contacts.point

```text
Contacts.point(index: Int64) -> Vec3
```

Weltpunkt des nullbasierten Kontakts in m. index muss kleiner als count sein.

Überall verfügbar.

## Contacts.resolve

```text
Contacts.resolve(bodyA: Body, bodyB: Body, solver: ContactSolver) -> ContactResult
```

Löst das Kontaktpaar mit Reibung und Rückprall. Ergebniswerte bodyA/bodyB müssen ausdrücklich übernommen werden; Eingaben bleiben unverändert. Die Kontakte müssen zu denselben Körperzuständen und derselben Reihenfolge gehören. Ein Körper mit Masse null repräsentiert eine feste Umgebung.

Überall verfügbar.

## Contacts.resolveSingle

```text
Contacts.resolveSingle(bodyA: Body, bodyB: Body, restitution: Float64, friction: Float64) -> ContactResult
```

Löst genau einen Kontakt mit der C-Einzelkontaktantwort. Rückprall in [0, 1], Reibung nichtnegativ; die Eingabekörper bleiben unverändert und korrigierte Körper sowie Impuls stehen im ContactResult.

Überall verfügbar.

## Contacts.sphereBox

```text
Contacts.sphereBox(sphere: Body, radius: Float64, box: Body, size: Vec3) -> Contacts
```

Kontakt der Kugel A mit einem orientierten Quader B; size sind die vollen lokalen Seitenlängen in m.

Überall verfügbar.

## Contacts.spherePlane

```text
Contacts.spherePlane(body: Body, radius: Float64, point: Vec3, normal: Vec3) -> Contacts
```

Kugelkontakt mit einer Ebene durch point und Einheitsnormalen normal zur freien Seite.

Überall verfügbar.

## Contacts.spheres

```text
Contacts.spheres(bodyA: Body, radiusA: Float64, bodyB: Body, radiusB: Float64) -> Contacts
```

Kontakt einer Kugel A mit Kugel B; Radien in m. Liefert null oder einen Kontakt.

Überall verfügbar.

## Dataset

```text
Dataset(index: Int64) -> Dataset
```

Öffnet den Eingabelauf am nullbasierten Index. Ein Analyselauf hat maximal acht Eingaben.

Analysemodul erforderlich.

## Dataset.channelCount

```text
Dataset.channelCount() -> Int64
```

Anzahl der Messkanäle ohne die Zeitachse. Das Dataset-Handle muss geöffnet sein.

Analysemodul erforderlich.

## Dataset.channelDescription

```text
Dataset.channelDescription(index: Int64) -> String
```

Kopiert die gespeicherte Beschreibung eines nullbasierten Messkanals als UTF-8-String.

Analysemodul erforderlich.

## Dataset.channelExponent

```text
Dataset.channelExponent(index: Int64, axis: Int64) -> Int64
```

Liest den SI-Exponenten des Messkanals für Achse 0 bis 6 als Int64. Ungültige Kanal- oder Achsenindizes sind Fehler.

Analysemodul erforderlich.

## Dataset.channelName

```text
Dataset.channelName(index: Int64) -> String
```

Kopiert den exakten Namen eines nullbasierten Messkanals als UTF-8-String. Ungültige Indizes erzeugen einen Quellfehler.

Analysemodul erforderlich.

## Dataset.channelUnitSymbol

```text
Dataset.channelUnitSymbol(index: Int64) -> String
```

Kopiert das gespeicherte Einheitensymbol eines nullbasierten Messkanals als UTF-8-String.

Analysemodul erforderlich.

## Dataset.close

```text
Dataset.close() -> Void
```

Schließt einen Datensatz und invalidiert seine Quell- und Ergebnisreihen.

Analysemodul erforderlich.

## Dataset.metadata

```text
Dataset.metadata() -> String
```

Kopiert die gespeicherten Laufmetadaten als UTF-8-String. Die Kopie bleibt nach dem Schließen des Datasets gültig.

Analysemodul erforderlich.

## Dataset.recovered

```text
Dataset.recovered() -> Bool
```

Wahr, wenn nur ein gültiger Teil einer unvollständigen Laufdatei gelesen wurde.

Analysemodul erforderlich.

## Dataset.sampleCount

```text
Dataset.sampleCount() -> Int64
```

Anzahl der erfolgreich gelesenen Datensätze. Bei wiederhergestellten Läufen kann dies kleiner als ursprünglich geplant sein.

Analysemodul erforderlich.

## Dataset.series

```text
Dataset.series(name: String) -> Series
```

Holt einen Kanal über seinen exakten Namen. Der reservierte Name time liefert die Zeitachse.

Analysemodul erforderlich.

## Diagnostic

```text
Diagnostic(code: Int64, operation: String, argument: String, source: String, line: Int64, column: Int64, message: String) -> Diagnostic
```

Erzeugt einen besitzenden begrenzten Diagnosewert. Fehlercode 1–10 außer EOF/Recovered, 1-basierte Quellposition oder null für unbekannt. Ungültige Werte werfen eine Quelldiagnose.

Überall verfügbar.

## DistanceJoint

```text
DistanceJoint(anchorA: Vec3, anchorB: Vec3, length: Float64, stabilization: Float64) -> DistanceJoint
```

Erzeugt ein Distanzgelenk mit zwei lokalen Vec3-Ankern, positiver Soll-Länge in m und Stabilisierung in 0..1. Die Eigenschaften anchorA, anchorB, length und stabilization sind schreibgeschützt.

Überall verfügbar.

## DistanceJoint.constraint

```text
DistanceJoint.constraint(bodyA: Int64, bodyB: Int64) -> JointConstraint
```

Bindet das Distanzgelenk an zwei verschiedene Körperindizes. bodyB=-1 verankert anchorB in Weltkoordinaten. bodyA, bodyB und joint sind schreibgeschützt.

Überall verfügbar.

## DistanceJoint.resolve

```text
DistanceJoint.resolve(bodyA: Body, bodyB: Body, dt: Float64) -> JointResult
```

Löst das Gelenk für zwei Körperkopien bei positivem dt in Sekunden. Nach äußeren Geschwindigkeitsänderungen und vor dem Positionsschritt verwenden. Das Ergebnis enthält bodyA/bodyB, impulse auf A in N s, lengthError vor dem Lösen in m und velocityError in m/s. Positionen bleiben unverändert. Ein Körper mit Masse null dient als Weltanker; zusammenfallende Anker sind ein Fehler. Stabilisierung kann Energie zuführen.

Überall verfügbar.

## Distribution.constant

```text
Distribution.constant(value: Float64) -> Distribution
```

Konstante Verteilung ohne Streuung.

Überall verfügbar.

## Distribution.mean

```text
Distribution.mean() -> Float64
```

Erwartungswert der Verteilung.

Überall verfügbar.

## Distribution.normal

```text
Distribution.normal(mean: Float64, standardDeviation: Float64) -> Distribution
```

Normalverteilung mit Mittelwert und nichtnegativer Standardabweichung.

Überall verfügbar.

## Distribution.standardDeviation

```text
Distribution.standardDeviation() -> Float64
```

Standardabweichung der Verteilung.

Überall verfügbar.

## Distribution.uniform

```text
Distribution.uniform(min: Float64, max: Float64) -> Distribution
```

Gleichverteilung zwischen Minimum und Maximum.

Überall verfügbar.

## Float64.parse

```text
Float64.parse(text: String) -> Float64?
```

Liest ASCII-Dezimaltext ohne Leerraum. Ungültige Schreibweisen und Werte außerhalb des Zielbereichs liefern `nil`; ein gültiger Wert kann mit `guard let` gebunden werden. Das Label `text:` ist optional. Die Regeln für Vorzeichen, Dezimalpunkt und Exponent entsprechen der expliziten Wertkonvertierung.

Überall verfügbar.

## Int64.abs

```text
Int64.abs(value: Int64) -> Int64
```

Absolutbetrag als Int64. Der kleinste Int64-Wert hat keinen positiven Int64-Gegenwert und erzeugt einen Laufzeitfehler mit Quellposition.

Überall verfügbar.

## Int64.clamp

```text
Int64.clamp(value: Int64, lower: Int64, upper: Int64) -> Int64
```

Begrenzt einen Int64-Wert auf das inklusive Intervall. Eine untere Grenze über der oberen erzeugt einen Laufzeitfehler mit Quellposition.

Überall verfügbar.

## Int64.isMultiple(of:)

```text
value.isMultiple(of: divisor) -> Bool  // value, divisor: Int64
```

Prüft ganzzahlig, ob value ein Vielfaches von divisor ist. Bei divisor 0 ist nur value 0 ein Vielfaches. Auch der kleinste Int64-Wert ist ein Vielfaches von -1, ohne Überlauf.

Überall verfügbar.

## Int64.max

```text
Int64.max(left: Int64, right: Int64) -> Int64
```

Liefert den größeren von zwei Int64-Werten ohne Umwandlung in Float64.

Überall verfügbar.

## Int64.min

```text
Int64.min(left: Int64, right: Int64) -> Int64
```

Liefert den kleineren von zwei Int64-Werten ohne Umwandlung in Float64.

Überall verfügbar.

## Int64.parse

```text
Int64.parse(text: String) -> Int64?
```

Liest ASCII-Dezimaltext ohne Leerraum. Ungültige Schreibweisen und Werte außerhalb des Zielbereichs liefern `nil`; ein gültiger Wert kann mit `guard let` gebunden werden. Das Label `text:` ist optional. Die Regeln für Vorzeichen, Dezimalpunkt und Exponent entsprechen der expliziten Wertkonvertierung.

Überall verfügbar.

## Int64.signum()

```text
value.signum() -> Int64  // value: Int64
```

Liefert -1, 0 oder 1 entsprechend dem Vorzeichen. Auch der kleinste Int64-Wert wird ohne Überlauf verarbeitet.

Überall verfügbar.

## Mat3

```text
Mat3(column0: Vec3, column1: Vec3, column2: Vec3) -> Mat3
```

3×3-Matrix aus drei Spaltenvektoren. Unabhängiger Wert; alle Komponenten müssen endlich sein. Wirkt auf Spaltenvektoren.

Überall verfügbar.

## Mat3.applied

```text
Mat3.applied(vector: Vec3) -> Vec3
```

Matrix-Vektor-Produkt ohne Normalisierung; nichtendliche Ergebnisse werden abgewiesen.

Überall verfügbar.

## Mat3.element

```text
Mat3.element(row: Int64, column: Int64) -> Float64
```

Liest ein Element mit nullbasierten row/column in 0..<3; ungültige Indizes erzeugen eine Quelldiagnose.

Überall verfügbar.

## Mat3.identity

```text
Mat3.identity() -> Mat3
```

3×3-Einheitsmatrix.

Überall verfügbar.

## Mat3.inverse

```text
Mat3.inverse(pivotTolerance: Float64) -> Mat3
```

Inverse mit skalierter Pivotwahl. pivotTolerance=0 verwendet 3×Maschinengenauigkeit; sonst muss 0<t<1 gelten. Abgewiesene Pivots und numerische Fehler erzeugen Quelldiagnosen. Die Toleranz ist keine Konditionsschätzung.

Überall verfügbar.

## Mat3.multiplied

```text
Mat3.multiplied(right: Mat3) -> Mat3
```

Matrixprodukt; die rechte Matrix wird zuerst angewandt. Nichtendliche Ergebnisse werden abgewiesen.

Überall verfügbar.

## Mat3.transposed

```text
Mat3.transposed() -> Mat3
```

Vertauscht Zeilen und Spalten; verändert den Empfänger nicht.

Überall verfügbar.

## Mat4

```text
Mat4(column0: Vec4, column1: Vec4, column2: Vec4, column3: Vec4) -> Mat4
```

4×4-Matrix aus vier Spaltenvektoren. Unabhängiger Wert; alle Komponenten müssen endlich sein. Homogene Koordinaten mit w in der vierten Komponente.

Überall verfügbar.

## Mat4.applied

```text
Mat4.applied(vector: Vec4) -> Vec4
```

Homogenes Matrix-Vektor-Produkt ohne Division durch w; für Punkte transformPoint verwenden.

Überall verfügbar.

## Mat4.element

```text
Mat4.element(row: Int64, column: Int64) -> Float64
```

Liest ein Element mit nullbasierten row/column in 0..<4; ungültige Indizes erzeugen eine Quelldiagnose.

Überall verfügbar.

## Mat4.identity

```text
Mat4.identity() -> Mat4
```

4×4-Einheitsmatrix.

Überall verfügbar.

## Mat4.inverse

```text
Mat4.inverse(pivotTolerance: Float64) -> Mat4
```

Inverse mit skalierter Pivotwahl. pivotTolerance=0 verwendet 4×Maschinengenauigkeit; sonst muss 0<t<1 gelten. Abgewiesene Pivots und numerische Fehler erzeugen Quelldiagnosen. Die Toleranz ist keine Konditionsschätzung.

Überall verfügbar.

## Mat4.multiplied

```text
Mat4.multiplied(right: Mat4) -> Mat4
```

Matrixprodukt; die rechte Matrix wird zuerst angewandt. Nichtendliche Ergebnisse werden abgewiesen.

Überall verfügbar.

## Mat4.rotation

```text
Mat4.rotation(rotation: Quat) -> Mat4
```

Rechtshändige aktive Rotation; Quaternion wird normiert. Nullquaternion wird abgewiesen.

Überall verfügbar.

## Mat4.scale

```text
Mat4.scale(scale: Vec3) -> Mat4
```

Lokale XYZ-Skalierung. Negative und nullwertige Faktoren sind erlaubt; Nullskalierung ist nicht invertierbar.

Überall verfügbar.

## Mat4.transformDirection

```text
Mat4.transformDirection(direction: Vec3) -> Vec3
```

Transformiert eine Richtung ohne Translation und ohne Normalisierung. Erfordert die affine letzte Zeile [0,0,0,1] exakt.

Überall verfügbar.

## Mat4.transformNormal

```text
Mat4.transformNormal(normal: Vec3) -> Vec3
```

Inverse-transponierte Transformation einer von null verschiedenen Flächennormale, anschließend normiert. Erfordert eine affine Matrix mit invertierbarem linearem Anteil; unter nichtuniformer Skalierung verschieden von transformDirection.

Überall verfügbar.

## Mat4.transformPoint

```text
Mat4.transformPoint(point: Vec3) -> Vec3
```

Transformiert einen Punkt mit homogenem w=1 und dividiert durch das Ergebnis-w. Auch projektive Matrizen erlaubt; w=0 oder nichtendliches Ergebnis erzeugt eine Quelldiagnose.

Überall verfügbar.

## Mat4.translation

```text
Mat4.translation(translation: Vec3) -> Mat4
```

Affine Translation; verschiebt Punkte, nicht Richtungen. Einheiten bestimmt das Modell.

Überall verfügbar.

## Mat4.transposed

```text
Mat4.transposed() -> Mat4
```

Vertauscht Zeilen und Spalten; verändert den Empfänger nicht.

Überall verfügbar.

## Mat4.trs

```text
Mat4.trs(translation: Vec3, rotation: Quat, scale: Vec3) -> Mat4
```

Lokale Skalierung, dann Rotation, dann Translation. Quaternion wird normiert. Nullskalierung erlaubt; keine automatische Einheitenkonvertierung.

Überall verfügbar.

## Material

```text
Material(density: Float64, restitution: Float64, friction: Float64) -> Material
```

Erzeugt ein Material mit Dichte in kg/m³, Rückprallwert in [0, 1] und nichtnegativer Reibung.

Überall verfügbar.

## Material.contactSolver

```text
Material.contactSolver(iterations: Int64, bounceThreshold: Float64, penetrationSlop: Float64, correctionFraction: Float64) -> ContactSolver
```

Erzeugt geprüfte Kontakt-Solver-Einstellungen mit Rückprall und Reibung dieses Materials.

Überall verfügbar.

## Measurement.isDropped

```text
Measurement.isDropped() -> Bool
```

Wahr bei ausgefallener Messung (Status 2).

Überall verfügbar.

## Measurement.isDue

```text
Measurement.isDue() -> Bool
```

Wahr, wenn eine Messung fällig war; auch bei Ausfall möglich.

Überall verfügbar.

## Measurement.isValid

```text
Measurement.isValid() -> Bool
```

Wahr nur für eine gültige Messung (Status 1).

Überall verfügbar.

## Medium

```text
Medium(density: Float64, viscosity: Float64) -> Medium
```

Erzeugt ein homogenes Medium mit nichtnegativer Dichte in kg/m³ und Viskosität in Pa·s.

Überall verfügbar.

## Medium.air

```text
Medium.air() -> Medium
```

Luft bei 15 °C auf Meereshöhe aus der gemeinsamen C-Bibliothek.

Überall verfügbar.

## Medium.dragForce

```text
Medium.dragForce(velocity: Vec3, coefficient: Float64, area: Float64) -> Vec3
```

Quadratische Widerstandskraft entgegen der Relativgeschwindigkeit; Koeffizient und Fläche müssen nichtnegativ sein.

Überall verfügbar.

## Medium.stokesDrag

```text
Medium.stokesDrag(velocity: Vec3, radius: Float64) -> Vec3
```

Stokes-Widerstand einer Kugel mit der Viskosität dieses Mediums; der Radius muss positiv sein.

Überall verfügbar.

## Medium.vacuum

```text
Medium.vacuum() -> Medium
```

Vakuum ohne Dichte und Viskosität aus der gemeinsamen C-Bibliothek.

Überall verfügbar.

## Medium.water

```text
Medium.water() -> Medium
```

Wasser bei 20 °C aus der gemeinsamen C-Bibliothek.

Überall verfügbar.

## Plot.curve

```text
Plot.curve(x: Series, y: Series, label: String) -> Void
```

Fügt dem Plot eine Linienkurve aus passenden Reihen hinzu.

Analysemodul erforderlich.

## Plot.export

```text
Plot.export(suffix: String) -> Void
```

Exportiert das gespeicherte Diagramm als SVG mit dem angegebenen Suffix.

Analysemodul erforderlich.

## Plot.points

```text
Plot.points(x: Series, y: Series, label: String) -> Void
```

Fügt dem Plot Messpunkte aus passenden Reihen hinzu.

Analysemodul erforderlich.

## Quantity

```text
Quantity(value: Float64, unit: Unit) -> Quantity
```

Verbindet einen Zahlenwert mit seiner Einheit.

Überall verfügbar.

## Quantity.adding

```text
Quantity.adding(right: Quantity) -> Quantity
```

Addiert Größen nach Umrechnung in die Einheit des linken Operanden. Bekannte Dimensionskonflikte werden beim Kompilieren erkannt.

Überall verfügbar.

## Quantity.converted

```text
Quantity.converted(to: Unit) -> Quantity
```

Konvertiert eine Größe in eine dimensionskompatible Zieleinheit. Bekannte Dimensionskonflikte werden beim Kompilieren erkannt.

Überall verfügbar.

## Quantity.divided

```text
Quantity.divided(right: Quantity, symbol: String) -> Quantity
```

Dividiert Größen und kombiniert ihre Dimensionen; der Divisor darf nicht null sein.

Überall verfügbar.

## Quantity.multiplied

```text
Quantity.multiplied(right: Quantity, symbol: String) -> Quantity
```

Multipliziert Größen und kombiniert ihre Dimensionen; symbol benennt die neue Einheit.

Überall verfügbar.

## Quantity.subtracting

```text
Quantity.subtracting(right: Quantity) -> Quantity
```

Subtrahiert Größen nach Umrechnung in die Einheit des linken Operanden. Bekannte Dimensionskonflikte werden beim Kompilieren erkannt.

Überall verfügbar.

## Quat

```text
Quat(x: Float64, y: Float64, z: Float64, w: Float64) -> Quat
```

Quaternion in Komponentenreihenfolge x, y, z, w.

Überall verfügbar.

## Quat.axisAngle

```text
Quat.axisAngle(axis: Vec3, angle: Float64) -> Quat
```

Erzeugt eine Rotation aus Achse und Winkel in Radiant.

Überall verfügbar.

## Quat.conjugated

```text
Quat.conjugated() -> Quat
```

Konjugierte Quaternion; bei Einheitsquaternion die inverse Rotation.

Überall verfügbar.

## Quat.multiplied

```text
Quat.multiplied(right: Quat) -> Quat
```

Komponiert Rotationen; die rechte Rotation wird zuerst angewandt.

Überall verfügbar.

## Quat.normalized

```text
Quat.normalized() -> Quat
```

Normiert eine gültige, von null verschiedene Quaternion.

Überall verfügbar.

## Quat.rotate

```text
Quat.rotate(vector: Vec3) -> Vec3
```

Rotiert einen dreidimensionalen Vektor.

Überall verfügbar.

## Quat.slerp

```text
Quat.slerp(end: Quat, fraction: Float64) -> Quat
```

Sphärische Rotationsinterpolation mit fraction in [0,1].

Überall verfügbar.

## Rng

```text
Rng(seed: Int64) -> Rng
```

Erzeugt einen unabhängigen PCG32-Zufallsstrom mit explizitem Int64-Seed.

Überall verfügbar.

## Rng.forRun

```text
Rng.forRun(stream: Int64) -> Rng
```

Erzeugt einen eigenen Zufallsstrom aus dem vollständigen Laufseed und einer Streamnummer; nur im Experiment.

Experimentmodul erforderlich.

## Rng.reseed

```text
Rng.reseed(seed: Int64) -> Void
```

Setzt einen veränderlichen Zufallsstrom auf den angegebenen Seed zurück.

Überall verfügbar.

## Rng.reseedForRun

```text
Rng.reseedForRun(stream: Int64) -> Void
```

Setzt einen veränderlichen Zufallsstrom aus Laufseed und Streamnummer zurück; nur im Experiment.

Experimentmodul erforderlich.

## Rng.sample

```text
Rng.sample(distribution: Distribution) -> Float64
```

Zieht mutierend einen Wert aus der Verteilung; der Zufallsstrom muss als var gebunden sein.

Überall verfügbar.

## RunIndex

```text
RunIndex(path: String, maximumEntries: Int64) -> RunIndex
```

Öffnet und validiert eine Laufdatei mit positivem Checkpointlimit im 64-MiB-Sprachbudget. Alte und unvollständige Dateien werden rekonstruiert. Kopien teilen einen automatisch freigegebenen Dateibesitzer; Abfragen verwenden nullbasierte Indizes.

Überall verfügbar.

## Sensor

```text
Sensor(config: SensorConfig, seed: Int64) -> Sensor
```

Initialisiert einen Sensor mit Konfiguration und explizitem Seed.

Überall verfügbar.

## Sensor.forRun

```text
Sensor.forRun(config: SensorConfig, stream: Int64) -> Sensor
```

Initialisiert einen Sensor mit einem aus Laufseed und Streamnummer abgeleiteten Zufallsstrom.

Experimentmodul erforderlich.

## Sensor.nextTime

```text
Sensor.nextTime() -> Float64
```

Nächster planmäßiger Abtastzeitpunkt in Sekunden.

Überall verfügbar.

## Sensor.read

```text
Sensor.read(time: Float64, truth: Quantity) -> Measurement
```

Wertet den Sensor bei time in Sekunden aus. truth ist eine Quantity mit kompatibler Einheit. Verändert den Sensorzustand; das Ergebnis enthält Wert, Status und Unsicherheit.

Überall verfügbar.

## Sensor.reset

```text
Sensor.reset(seed: Int64) -> Void
```

Setzt Abtastraster und Zufallsstrom des Sensors mit einem expliziten Seed zurück; verändert den Empfänger.

Überall verfügbar.

## Sensor.resetForRun

```text
Sensor.resetForRun(stream: Int64) -> Void
```

Setzt den Sensor anhand von Laufseed und Stream zurück; verändert den Empfänger.

Experimentmodul erforderlich.

## SensorConfig

```text
SensorConfig(unit: Unit, rateHz: Float64, startTime: Float64, resolution: Float64, offset: Float64, driftPerSecond: Float64, noise: Distribution, dropoutProbability: Float64, uncertaintyAbsolute: Float64, uncertaintyRelative: Float64) -> SensorConfig
```

Definiert Einheit, Abtastrate in Hz, Startzeit in s, Auflösung, Offset, Drift pro Sekunde, Rauschen, Ausfallwahrscheinlichkeit und absolute/relative Standardunsicherheit. Auflösung, Offset, Drift und Rauschen beziehen sich auf die Sensoreinheit.

Überall verfügbar.

## Series.adding

```text
Series.adding(right: Series) -> Series
```

Addiert gepaarte Reihen mit kompatiblen Einheiten.

Analysemodul erforderlich.

## Series.affine

```text
Series.affine(factor: Float64, offset: Float64, unit: Unit) -> Series
```

Berechnet factor * input + offset. factor ist dimensionslos, unit beschreibt den Offset.

Analysemodul erforderlich.

## Series.alignedValues

```text
Series.alignedValues(values: [Float64], unit: Unit, name: String) -> Series
```

Kopiert gleich viele endliche Float64-Werte in eine neue Reihe mit Alignment und Lebensdauer des Ankers.

Analysemodul erforderlich.

## Series.count

```text
Series.count() -> Int64
```

Anzahl der Samples der Reihe.

Analysemodul erforderlich.

## Series.derivative

```text
Series.derivative(x: Series) -> Series
```

Sekantenableitung dy/dx; x muss streng steigen. Die Einheit wird abgeleitet.

Analysemodul erforderlich.

## Series.divided

```text
Series.divided(right: Series) -> Series
```

Dividiert gepaarte Reihen; Nulldivision ist ein Fehler.

Analysemodul erforderlich.

## Series.exponent

```text
Series.exponent(axis: Int64) -> Int64
```

Liest den SI-Exponenten der Reihe für Achse 0 bis 6 als Int64; ungültige Achsen sind Fehler.

Analysemodul erforderlich.

## Series.export

```text
Series.export(x: Series, suffix: String) -> Void
```

Exportiert alle Werte eines x/y-Paares als CSV. Der Empfänger ist y; suffix ergänzt den Ausgabepräfix.

Analysemodul erforderlich.

## Series.exportColumns

```text
Series.exportColumns(columns: [Series], suffix: String) -> Void
```

Exportiert 1 bis 32 zugeordnete Reihen gemeinsam als CSV. Reihenfolge, Namen und Einheiten erscheinen als Spalten; leere und nicht zugeordnete Listen sind Fehler.

Analysemodul erforderlich.

## Series.fromValues

```text
Series.fromValues(values: [Float64], unit: Unit, name: String) -> Series
```

Kopiert endliche Float64-Werte in eine eigenständige Datenreihe ohne Eingabelauf; jede Reihe hat zunächst ein eigenes Alignment.

Analysemodul erforderlich.

## Series.histogram

```text
Series.histogram(title: String, bins: Int64) -> Plot
```

Zählt alle Reihenwerte in bis zu 128 gleich breiten Klassen.

Analysemodul erforderlich.

## Series.integral

```text
Series.integral(x: Series, initial: Float64, unit: Unit) -> Series
```

Kumulative Trapezintegration von y nach x mit Anfangswert und dessen Einheit.

Analysemodul erforderlich.

## Series.isAlignedWith

```text
Series.isAlignedWith(right: Series) -> Bool
```

Prüft zwei gültige Reihen auf dasselbe Dataset, dieselbe Auswahl und denselben Samplebereich. Verschiedene Zuordnungen liefern false; ungültige Handles sind Fehler.

Analysemodul erforderlich.

## Series.maximum

```text
Series.maximum() -> Float64
```

Liefert den größten endlichen Wert einer nichtleeren Reihe. Liest blockweise und benötigt keine darstellbare Varianz.

Analysemodul erforderlich.

## Series.mean

```text
Series.mean() -> Float64
```

Mittelwert über alle Werte der Reihe.

Analysemodul erforderlich.

## Series.minimum

```text
Series.minimum() -> Float64
```

Liefert den kleinsten endlichen Wert einer nichtleeren Reihe. Liest blockweise und benötigt keine darstellbare Varianz.

Analysemodul erforderlich.

## Series.movingAverage

```text
Series.movingAverage(window: Int64) -> Series
```

Kausaler Mittelwert über höchstens window Werte; window liegt zwischen 1 und 4096.

Analysemodul erforderlich.

## Series.multiplied

```text
Series.multiplied(right: Series) -> Series
```

Multipliziert gepaarte Reihen und ihre Einheiten.

Analysemodul erforderlich.

## Series.name

```text
Series.name() -> String
```

Kopiert den Reihennamen als eigenen UTF-8-String. Die Kopie bleibt nach Freigabe der Reihe gültig.

Analysemodul erforderlich.

## Series.plot

```text
Series.plot(x: Series, title: String, label: String) -> Plot
```

Erstellt ein Liniendiagramm aus ausgerichteten x/y-Reihen. Der Empfänger der Methode ist y.

Analysemodul erforderlich.

## Series.quantile

```text
Series.quantile(probability: Float64) -> Float64
```

Typ-7-Quantil einer nichtleeren Reihe. Die endliche Wahrscheinlichkeit muss in [0, 1] liegen; die Werte werden mit budgetiertem temporärem Speicher sortiert.

Analysemodul erforderlich.

## Series.release

```text
Series.release() -> Void
```

Gibt ein Reihenhandle frei. Weitere Verwendung dieses Handles ist ein Fehler.

Analysemodul erforderlich.

## Series.resampledLinear

```text
Series.resampledLinear(x: Series, targetX: Series) -> Series
```

Interpoliert y(x) linear auf targetX ohne Extrapolation.

Analysemodul erforderlich.

## Series.resampledNearest

```text
Series.resampledNearest(x: Series, targetX: Series) -> Series
```

Wählt den nächsten Stützpunkt auf targetX; bei gleichem Abstand den früheren.

Analysemodul erforderlich.

## Series.resampledPchip

```text
Series.resampledPchip(x: Series, targetX: Series) -> Series
```

Interpoliert y(x) mit monotoner kubischer Hermite-Interpolation (PCHIP) auf targetX ohne Extrapolation.

Analysemodul erforderlich.

## Series.resampledPrevious

```text
Series.resampledPrevious(x: Series, targetX: Series) -> Series
```

Verwendet den letzten Stützpunkt vor oder an targetX, etwa für stückweise konstante Signale.

Analysemodul erforderlich.

## Series.select

```text
Series.select(columns: [Series], selector: Series, accepted: Float64) -> [Series]
```

Filtert mehrere ausgerichtete Reihen gemeinsam nach dem exakten Selektorwert, etwa Status 1. Ergebnisreihen behalten dieselbe Zeilenzuordnung.

Analysemodul erforderlich.

## Series.slice

```text
Series.slice(first: Int64, count: Int64) -> Series
```

Kopiert count Werte ab dem nullbasierten Index first. Passende Ausschnitte derselben Auswahl bleiben zugeordnet; negative oder zu große Bereiche sind Fehler.

Analysemodul erforderlich.

## Series.stddev

```text
Series.stddev() -> Float64
```

Stichprobenstandardabweichung; benötigt mindestens zwei Werte.

Analysemodul erforderlich.

## Series.subtracting

```text
Series.subtracting(right: Series) -> Series
```

Subtrahiert gepaarte Reihen mit kompatiblen Einheiten.

Analysemodul erforderlich.

## Series.unitScale

```text
Series.unitScale() -> Float64
```

Liefert den Maßstab der gespeicherten Werte zur SI-Einheit als Float64.

Analysemodul erforderlich.

## Series.unitSymbol

```text
Series.unitSymbol() -> String
```

Kopiert das gespeicherte oder bei abgeleiteten Reihen formatierte Einheitensymbol als eigenen UTF-8-String.

Analysemodul erforderlich.

## Series.value

```text
Series.value(index: Int64) -> Float64
```

Liest einen gültigen Zahlenwert am nullbasierten Index. Eine maskierte fehlende Beobachtung erzeugt einen Quellfehler.

Analysemodul erforderlich.

## Series.values

```text
Series.values(first: Int64, count: Int64) -> [Float64]
```

Liest count gültige Werte ab first als unabhängiges Float64-Array. Fehlende Beobachtungen, negative oder zu große Bereiche sind Fehler; ein leerer Ausschnitt am Ende ist erlaubt.

Analysemodul erforderlich.

## StepInterval

```text
StepInterval(elapsed: Float64, nextStep: Float64) -> StepInterval
```

Wert mit den positiven endlichen Dauern elapsed und nextStep in Sekunden. Der adaptiveStep-Callback meldet damit die akzeptierte Dauer und den nächsten Schrittvorschlag. Beide Felder sind schreibgeschützt; Arrays, optionale Werte und Funktionswerte kopieren den Wert.

Überall verfügbar.

## String(repeating:count:)

```text
String(repeating: text, count: n) -> String  // text: String, n: Int64
```

Erzeugt einen neuen String aus `n` Kopien des Texts. Der Zähler muss nichtnegativ sein; Größenüberschreitungen erzeugen eine Quelldiagnose.

Überall verfügbar.

## String.allSatisfy(_:)

```text
text.allSatisfy(predicate) -> Bool  // text: String, predicate: func(String) -> Bool
```

Prüft Unicode-Skalare als eigene Strings in Quellreihenfolge bis zum ersten `false`. Ein leerer String liefert `true`. Empfänger und Funktion werden je einmal ausgewertet.

Überall verfügbar.

## String.compactMap(_:)

```text
text.compactMap(transform) -> [U]  // text: String, transform: func(String) -> U?
```

Ruft die Transformation einmal je Unicode-Skalar als eigenem String auf und sammelt vorhandene Ergebnisse in Quellreihenfolge als unabhängiges Array. `nil` wird ausgelassen; Fehler räumen Teilwerte auf.

Überall verfügbar.

## String.contains(where:)

```text
text.contains(where: predicate) -> Bool  // text: String, predicate: func(String) -> Bool
```

Prüft Unicode-Skalare als eigene Strings in Quellreihenfolge bis zum ersten `true`. Ein leerer String liefert `false`. Empfänger und Funktion werden je einmal ausgewertet.

Überall verfügbar.

## String.drop(while:)

```text
text.drop(while: predicate) -> String  // text: String, predicate: func(String) -> Bool
```

Überspringt anfängliche Elemente, solange das Prädikat wahr ist, und kopiert anschließend den Rest. Geprüft wird je ein Unicode-Skalar als eigener String. Nach dem ersten `false` wird das Prädikat nicht erneut aufgerufen. Das Ergebnis ist ein unabhängiger String mit Skalargrenzen.

Überall verfügbar.

## String.dropFirst(_:)

```text
text.dropFirst() -> String  // text: String
text.dropFirst(count) -> String  // count: Int64
```

Wählt bis zu count Unicode-Skalare aus oder lässt sie weg. Ohne count gilt 1. Negative Werte sind Fehler; Werte über text.count werden begrenzt. Das Ergebnis ist ein unabhängiger String-Wert.

Überall verfügbar.

## String.dropLast(_:)

```text
text.dropLast() -> String  // text: String
text.dropLast(count) -> String  // count: Int64
```

Wählt bis zu count Unicode-Skalare aus oder lässt sie weg. Ohne count gilt 1. Negative Werte sind Fehler; Werte über text.count werden begrenzt. Das Ergebnis ist ein unabhängiger String-Wert.

Überall verfügbar.

## String.filter(_:)

```text
text.filter(predicate) -> String  // text: String, predicate: func(String) -> Bool
```

Prüft jeden Unicode-Skalar als eigenen String und übernimmt passende Skalare in Quellreihenfolge. Ein leerer String ruft das Prädikat nicht auf. Empfänger und Funktion werden je einmal ausgewertet; ein Fehler räumt das Teilergebnis auf. Das Ergebnis ist ein unabhängiger String.

Überall verfügbar.

## String.first

```text
text.first -> String?  // text: String
```

Liefert den ersten beziehungsweise letzten Unicode-Skalar als unabhängigen String; bei einem leeren String `nil`.

Überall verfügbar.

## String.first(where:)

```text
text.first(where: predicate) -> String?  // text: String, predicate: func(String) -> Bool
```

Prüft Unicode-Skalare von vorn und liefert den ersten passenden Skalar als eigenen String. Ohne Treffer `nil`. Empfänger und Funktion werden je einmal ausgewertet; ein leerer String ruft die Funktion nicht auf.

Überall verfügbar.

## String.firstIndex(where:)

```text
text.firstIndex(where: predicate) -> Int64?  // text: String, predicate: func(String) -> Bool
```

Prüft Unicode-Skalare von vorn und liefert den ersten passenden nullbasierten Skalarindex. Ohne Treffer `nil`. Empfänger und Funktion werden je einmal ausgewertet; ein leerer String ruft die Funktion nicht auf.

Überall verfügbar.

## String.flatMap(_:)

```text
text.flatMap(transform) -> [U]  // text: String, transform: func(String) -> [U]
```

Ruft die Transformation einmal je Unicode-Skalar als eigenem String auf und hängt die Teilarrays in Quellreihenfolge zu einem unabhängigen Array zusammen. Leere Teilarrays tragen keine Elemente bei; Fehler räumen Teilwerte auf.

Überall verfügbar.

## String.forEach(_:)

```text
text.forEach(body) -> Void  // text: String, body: func(String) -> Void
```

Ruft body für jeden Unicode-Skalar als eigenen String in Quellreihenfolge auf. Ein leerer String ruft body nicht auf.

Überall verfügbar.

## String.last

```text
text.last -> String?  // text: String
```

Liefert den ersten beziehungsweise letzten Unicode-Skalar als unabhängigen String; bei einem leeren String `nil`.

Überall verfügbar.

## String.last(where:)

```text
text.last(where: predicate) -> String?  // text: String, predicate: func(String) -> Bool
```

Prüft Unicode-Skalare von hinten und liefert den ersten passenden Skalar als eigenen String. Ohne Treffer `nil`. Empfänger und Funktion werden je einmal ausgewertet; ein leerer String ruft die Funktion nicht auf.

Überall verfügbar.

## String.lastIndex(where:)

```text
text.lastIndex(where: predicate) -> Int64?  // text: String, predicate: func(String) -> Bool
```

Prüft Unicode-Skalare von hinten und liefert den ersten passenden nullbasierten Skalarindex. Ohne Treffer `nil`. Empfänger und Funktion werden je einmal ausgewertet; ein leerer String ruft die Funktion nicht auf.

Überall verfügbar.

## String.map(_:)

```text
text.map(transform) -> [U]  // text: String, transform: func(String) -> U
```

Ruft die Transformation einmal je Unicode-Skalar als eigenem String in Quellreihenfolge auf und sammelt die Ergebnisse als unabhängiges Array. Ein leerer String ruft die Funktion nicht auf. Empfänger und Funktion werden je einmal ausgewertet; Fehler räumen Teilwerte auf.

Überall verfügbar.

## String.max()

```text
text.max() -> String?  // text: String
```

Liefert den kleinsten beziehungsweise größten Unicode-Skalar als unabhängigen String oder `nil` bei leerem Text.

Überall verfügbar.

## String.max(by:)

```text
text.max(by: compare) -> String?  // compare: func(String, String) -> Bool
```

Vergleicht Unicode-Skalare als eigene Strings. Die Sortierung ist stabil; Extrema behalten bei gleichem Rang den ersten Skalar. Eingabe und Vergleichsfunktion werden je einmal ausgewertet.

Überall verfügbar.

## String.min()

```text
text.min() -> String?  // text: String
```

Liefert den kleinsten beziehungsweise größten Unicode-Skalar als unabhängigen String oder `nil` bei leerem Text.

Überall verfügbar.

## String.min(by:)

```text
text.min(by: compare) -> String?  // compare: func(String, String) -> Bool
```

Vergleicht Unicode-Skalare als eigene Strings. Die Sortierung ist stabil; Extrema behalten bei gleichem Rang den ersten Skalar. Eingabe und Vergleichsfunktion werden je einmal ausgewertet.

Überall verfügbar.

## String.prefix(_:)

```text
text.prefix(count) -> String  // text: String, count: Int64
```

Wählt bis zu count Unicode-Skalare aus oder lässt sie weg. Negative Werte sind Fehler; Werte über text.count werden begrenzt. Das Ergebnis ist ein unabhängiger String-Wert.

Überall verfügbar.

## String.prefix(while:)

```text
text.prefix(while: predicate) -> String  // text: String, predicate: func(String) -> Bool
```

Kopiert die anfänglichen Elemente, solange das Prädikat wahr ist. Geprüft wird je ein Unicode-Skalar als eigener String. Nach dem ersten `false` wird das Prädikat nicht erneut aufgerufen. Das Ergebnis ist ein unabhängiger String mit Skalargrenzen.

Überall verfügbar.

## String.reduce(_:_:)

```text
text.reduce(initial, combine) -> U  // text: String, combine: func(U, String) -> U
```

Faltet Unicode-Skalare als eigene Strings in Quellreihenfolge. Ein leerer String liefert initial ohne Funktionsaufruf. Fehler räumen den Akkumulator und temporäre Skalarwerte auf.

Überall verfügbar.

## String.sorted()

```text
text.sorted() -> [String]  // text: String
```

Liefert ein unabhängiges Array der Unicode-Skalare in aufsteigender Skalarreihenfolge.

Überall verfügbar.

## String.sorted(by:)

```text
text.sorted(by: compare) -> [String]  // compare: func(String, String) -> Bool
```

Vergleicht Unicode-Skalare als eigene Strings. Die Sortierung ist stabil; Extrema behalten bei gleichem Rang den ersten Skalar. Eingabe und Vergleichsfunktion werden je einmal ausgewertet.

Überall verfügbar.

## String.split(separator:maxSplits:omittingEmptySubsequences:)

```text
text.split(separator: part, maxSplits: limit, omittingEmptySubsequences: omit) -> [String]
```

`part: String` ist erforderlich. `limit: Int64` ist optional und standardmäßig unbegrenzt; ein negativer Wert ist ein Laufzeitfehler. `omit: Bool` ist optional und standardmäßig `true`. Auch ohne `maxSplits:` kann `omittingEmptySubsequences:` angegeben werden. Ausgelassene leere Teilstücke zählen nicht gegen das Split-Limit. Ein leerer Trenner teilt als Physim-Erweiterung an Unicode-Skalargrenzen. Das Ergebnis besitzt unabhängige Stringwerte.

Überall verfügbar.

## String.suffix(_:)

```text
text.suffix(count) -> String  // text: String, count: Int64
```

Wählt bis zu count Unicode-Skalare aus oder lässt sie weg. Negative Werte sind Fehler; Werte über text.count werden begrenzt. Das Ergebnis ist ein unabhängiger String-Wert.

Überall verfügbar.

## String.trimmingCharacters(in:)

```text
text.trimmingCharacters(in: CharacterSet.whitespacesAndNewlines) -> String
```

Entfernt Unicode-Leerraum an beiden Enden des Strings. Derzeit ist nur `CharacterSet.whitespacesAndNewlines` verfügbar.

Überall verfügbar.

## String.utf8.count

```text
text.utf8.count -> Int64
```

Zählt die UTF-8-Bytes des Strings.

Überall verfügbar.

## Submersion.sphere

```text
Submersion.sphere(radius: Float64, centerHeight: Float64) -> Submersion
```

Schnitt einer Kugel mit einer ebenen Flüssigkeitsoberfläche: liefert verdrängtes Volumen und Schwerpunktabstand entlang der nach außen gerichteten Oberflächennormale. Radius muss positiv, die Eingaben endlich sein.

Überall verfügbar.

## Sweep.contacts

```text
Sweep.contacts() -> Contacts
```

Ein Kontakt am berechneten Ereignis oder ein leeres Contacts bei fehlendem Treffer. Vor der Kollisionsantwort Körper bis zur Ereigniszeit bewegen; danach Restzeit neu berechnen.

Überall verfügbar.

## Sweep.fraction

```text
Sweep.fraction() -> Float64
```

Anteil der vorgegebenen Verschiebung bis zum ersten Kontakt in 0..1. Ohne Treffer ist der Aufruf ein Laufzeitfehler; vorher hit prüfen.

Überall verfügbar.

## Sweep.spherePlane

```text
Sweep.spherePlane(body: Body, radius: Float64, displacement: Vec3, point: Vec3, normal: Vec3) -> Sweep
```

Erster Kontakt einer linear verschobenen Kugel mit einer Ebene. point liegt auf der Ebene; die Einheitsnormale zeigt in den freien Halbraum. Keine Beschleunigung oder automatische Kollisionsantwort.

Überall verfügbar.

## Sweep.spheres

```text
Sweep.spheres(bodyA: Body, radiusA: Float64, displacementA: Vec3, bodyB: Body, radiusB: Float64, displacementB: Vec3) -> Sweep
```

Erster Kontakt zweier Kugeln entlang vorgegebener linearer Verschiebungen in m. Gespeicherte Geschwindigkeiten werden nicht verwendet. Das Sweep-Ergebnis enthält hit; Anfangsberührung und Überlappung zählen auch bei Trennung als Treffer bei fraction()=0.

Überall verfügbar.

## Table

```text
Table(title: String, labels: [String], units: [Unit]) -> Table
```

Erstellt eine Berichtstabelle mit Spaltennamen und Einheiten. Beide Arrays müssen gleich lang sein.

Analysemodul erforderlich.

## Table.export

```text
Table.export(suffix: String) -> Void
```

Exportiert alle Tabellenzeilen als CSV mit einem Suffix am Ausgabepräfix.

Analysemodul erforderlich.

## Table.row

```text
Table.row(label: String, values: [Quantity]) -> Void
```

Fügt eine benannte Tabellenzeile mit passenden Quantity-Werten hinzu.

Analysemodul erforderlich.

## Unit

```text
Unit(length: Int64, mass: Int64, time: Int64, current: Int64, temperature: Int64, amount: Int64, luminosity: Int64, scale: Float64, symbol: String) -> Unit
```

Einheit aus sieben SI-Exponenten, positiver Skala und Symbol. Exponentenreihenfolge: Länge, Masse, Zeit, Strom, Temperatur, Stoffmenge, Lichtstärke.

Überall verfügbar.

## Unit.convert

```text
Unit.convert(value: Float64, to: Unit) -> Float64
```

Konvertiert einen Zahlenwert von der Empfängereinheit in eine dimensionskompatible Zieleinheit. Bekannte Dimensionskonflikte sind Compilerfehler, dynamische Konflikte Laufzeitfehler.

Überall verfügbar.

## Unit.divided

```text
Unit.divided(right: Unit, symbol: String) -> Unit
```

Dividiert Dimensionen und Skalen zweier Einheiten.

Überall verfügbar.

## Unit.isCompatible

```text
Unit.isCompatible(right: Unit) -> Bool
```

Prüft gleiche SI-Dimensionen unabhängig von Skala und Symbol.

Überall verfügbar.

## Unit.multiplied

```text
Unit.multiplied(right: Unit, symbol: String) -> Unit
```

Multipliziert Dimensionen und Skalen zweier Einheiten.

Überall verfügbar.

## Unit.powered

```text
Unit.powered(exponent: Int64, symbol: String) -> Unit
```

Erhebt eine Einheit in eine ganzzahlige Potenz.

Überall verfügbar.

## Vec2

```text
Vec2(x: Float64, y: Float64) -> Vec2
```

Vektor aus zwei Komponenten.

Überall verfügbar.

## Vec2.cross

```text
Vec2.cross(right: Vec2) -> Float64
```

Vorzeichenbehaftete Fläche beziehungsweise Z-Komponente des zweidimensionalen Kreuzprodukts.

Überall verfügbar.

## Vec2.dot

```text
Vec2.dot(right: Vec2) -> Float64
```

Skalarprodukt zweier Vektoren gleicher Dimension.

Überall verfügbar.

## Vec2.length

```text
Vec2.length() -> Float64
```

Euklidische Länge des Vektors.

Überall verfügbar.

## Vec2.normalized

```text
Vec2.normalized() -> Vec2
```

Normiert den Vektor; der Nullvektor bleibt null.

Überall verfügbar.

## Vec2.symplectic

```text
Vec2.symplectic(acceleration: Float64, dt: Float64) -> Vec2
```

Symplektischer Euler: Vec2 enthält Position und Geschwindigkeit. Erst Geschwindigkeit, dann Position aktualisieren; dt in Sekunden.

Überall verfügbar.

## Vec3

```text
Vec3(x: Float64, y: Float64, z: Float64) -> Vec3
```

Vektor aus drei Komponenten.

Überall verfügbar.

## Vec3.cross

```text
Vec3.cross(right: Vec3) -> Vec3
```

Rechtshändiges Kreuzprodukt zweier dreidimensionaler Vektoren.

Überall verfügbar.

## Vec3.dot

```text
Vec3.dot(right: Vec3) -> Float64
```

Skalarprodukt zweier Vektoren gleicher Dimension.

Überall verfügbar.

## Vec3.length

```text
Vec3.length() -> Float64
```

Euklidische Länge des Vektors.

Überall verfügbar.

## Vec3.normalized

```text
Vec3.normalized() -> Vec3
```

Normiert den Vektor; der Nullvektor bleibt null.

Überall verfügbar.

## Vec4

```text
Vec4(x: Float64, y: Float64, z: Float64, w: Float64) -> Vec4
```

Vektor aus vier Komponenten.

Überall verfügbar.

## Vec4.dot

```text
Vec4.dot(right: Vec4) -> Float64
```

Skalarprodukt zweier Vektoren gleicher Dimension.

Überall verfügbar.

## Vec4.length

```text
Vec4.length() -> Float64
```

Euklidische Länge des Vektors.

Überall verfügbar.

## Vec4.normalized

```text
Vec4.normalized() -> Vec4
```

Normiert den Vektor; der Nullvektor bleibt null.

Überall verfügbar.

## abs

```text
abs(value: Float64) -> Float64
```

Absolutbetrag.

Überall verfügbar.

## acos

```text
acos(value: Float64) -> Float64
```

Arkuskosinus eines Werts in [-1, 1], Ergebnis in Radiant.

Überall verfügbar.

## array.allSatisfy

```text
array.allSatisfy(predicate) -> Bool  // predicate: func(T) -> Bool
```

Prüft die Elemente in Reihenfolge bis zum ersten `false`; ein leeres Array liefert `true`.

Überall verfügbar.

## array.append(contentsOf:)

```text
array.append(contentsOf: values) -> Void  // array: var [T], values: [T]
```

Hängt alle Elemente eines gleich typisierten Arrays an. Selbstanhang ist erlaubt; ein Kopierfehler lässt den Empfänger unverändert.

Überall verfügbar.

## array.compactMap(_:)

```text
array.compactMap(transform) -> [U]  // array: [T], transform: func(T) -> U?
```

Ruft die Transformation einmal je Element in Arrayreihenfolge auf und sammelt vorhandene Ergebnisse als unabhängigen Arraywert. `nil`-Ergebnisse werden ausgelassen. Ein leeres Array ruft die Transformation nicht auf. Empfänger und Funktionswert werden je einmal ausgewertet; Fehler räumen Teilwerte auf.

Überall verfügbar.

## array.contains(where:)

```text
array.contains(where: predicate) -> Bool  // predicate: func(T) -> Bool
```

Prüft die Elemente in Reihenfolge bis zum ersten `true`; ein leeres Array liefert `false`.

Überall verfügbar.

## array.drop(while:)

```text
array.drop(while: predicate) -> [T]  // array: [T], predicate: func(T) -> Bool
```

Überspringt anfängliche Elemente, solange das Prädikat wahr ist, und kopiert anschließend den Rest. Nach dem ersten `false` wird das Prädikat nicht erneut aufgerufen. Empfänger und Funktionswert werden je einmal ausgewertet; das Ergebnis ist eine unabhängige Arraykopie mit Indexbeginn null.

Überall verfügbar.

## array.dropFirst(_:)

```text
array.dropFirst() -> [T]  // array: [T]
array.dropFirst(count) -> [T]  // count: Int64
```

Wählt bis zu count Elemente aus oder lässt sie weg. Ohne count gilt 1. Negative Werte sind Fehler; Werte über der Länge werden begrenzt. Das Ergebnis ist eine unabhängige Arraykopie mit Indexbeginn null.

Überall verfügbar.

## array.dropLast(_:)

```text
array.dropLast() -> [T]  // array: [T]
array.dropLast(count) -> [T]  // count: Int64
```

Wählt bis zu count Elemente aus oder lässt sie weg. Ohne count gilt 1. Negative Werte sind Fehler; Werte über der Länge werden begrenzt. Das Ergebnis ist eine unabhängige Arraykopie mit Indexbeginn null.

Überall verfügbar.

## array.elementsEqual(_:)

```text
array.elementsEqual(other) -> Bool  // array, other: [T], T: Equatable
```

Prüft gleiche Länge und paarweise gleiche Elemente in Arrayreihenfolge. Beide Eingabewerte bleiben unverändert.

Überall verfügbar.

## array.first

```text
array.first -> T?  // array: [T]
```

Liefert eine unabhängige Kopie des ersten beziehungsweise letzten Elements; bei einem leeren Array `nil`.

Überall verfügbar.

## array.first(where:)

```text
array.first(where: predicate) -> T?  // predicate: func(T) -> Bool
```

Prüft Elemente bis zum ersten Treffer und liefert dessen unabhängige Kopie; ohne Treffer `nil`.

Überall verfügbar.

## array.firstIndex(where:)

```text
array.firstIndex(where: predicate) -> Int64?  // predicate: func(T) -> Bool
```

Liefert den passenden Index in Suchrichtung oder `nil`.

Überall verfügbar.

## array.flatMap(_:)

```text
array.flatMap(transform) -> [U]  // array: [T], transform: func(T) -> [U]
```

Ruft die Transformation einmal je Element in Arrayreihenfolge auf und hängt ihre Teilarrays zu einem unabhängigen Arraywert zusammen. Leere Teilarrays steuern keine Elemente bei. Empfänger und Funktionswert werden je einmal ausgewertet; Fehler räumen Teilwerte auf.

Überall verfügbar.

## array.forEach(_:)

```text
array.forEach(body) -> Void  // array: [T], body: func(T) -> Void
```

Ruft body für jedes Element in Arrayreihenfolge auf. Ein leeres Array ruft body nicht auf. Die Iteration verwendet einen Snapshot des Empfängers.

Überall verfügbar.

## array.insert(contentsOf:at:)

```text
array.insert(contentsOf: values, at: index) -> Void
// array: var [T], values: [T], index: Int64
```

Fügt alle Elemente vor index ein; erlaubt sind 0 bis array.count. Selbsteinfügung ist erlaubt. Index- und Kopierfehler lassen den Empfänger unverändert.

Überall verfügbar.

## array.isEmpty

```text
array.isEmpty -> Bool  // array: [T]
```

Prüft, ob das Array leer ist.

Überall verfügbar.

## array.last

```text
array.last -> T?  // array: [T]
```

Liefert eine unabhängige Kopie des ersten beziehungsweise letzten Elements; bei einem leeren Array `nil`.

Überall verfügbar.

## array.last(where:)

```text
array.last(where: predicate) -> T?  // predicate: func(T) -> Bool
```

Sucht vom Ende aus und liefert eine unabhängige Kopie des letzten passenden Elements; ohne Treffer `nil`.

Überall verfügbar.

## array.lastIndex(where:)

```text
array.lastIndex(where: predicate) -> Int64?  // predicate: func(T) -> Bool
```

Liefert den passenden Index in Suchrichtung oder `nil`.

Überall verfügbar.

## array.max()

```text
array.max() -> T?  // array: [Int64], [Float64] oder [String]
```

Liefert eine unabhängige Kopie des kleinsten beziehungsweise größten Elements; bei leerem Array `nil`. Nicht endliche `Float64`-Werte sind Fehler.

Überall verfügbar.

## array.max(by:)

```text
array.max(by: compare) -> T?  // array: [T]
// compare: func(T, T) -> Bool
```

Durchläuft das Array einmal und behält bei gleichem Rang das erste Element. Die Vergleichsfunktion wird auf leeren und einzelnen Arrays nicht aufgerufen. Das ausgewählte Element wird unabhängig kopiert.

Überall verfügbar.

## array.min()

```text
array.min() -> T?  // array: [Int64], [Float64] oder [String]
```

Liefert eine unabhängige Kopie des kleinsten beziehungsweise größten Elements; bei leerem Array `nil`. Nicht endliche `Float64`-Werte sind Fehler.

Überall verfügbar.

## array.min(by:)

```text
array.min(by: compare) -> T?  // array: [T]
// compare: func(T, T) -> Bool
```

Durchläuft das Array einmal und behält bei gleichem Rang das erste Element. Die Vergleichsfunktion wird auf leeren und einzelnen Arrays nicht aufgerufen. Das ausgewählte Element wird unabhängig kopiert.

Überall verfügbar.

## array.popLast

```text
array.popLast() -> T?  // array: var [T]
```

Entfernt das letzte Element eines veränderlichen Arrays und liefert eine unabhängige Wertkopie. Bei einem leeren Array bleibt dieses unverändert und das Ergebnis ist `nil`.

Überall verfügbar.

## array.prefix(_:)

```text
array.prefix(count) -> [T]  // array: [T], count: Int64
```

Wählt bis zu count Elemente aus oder lässt sie weg. Negative Werte sind Fehler; Werte über der Länge werden begrenzt. Das Ergebnis ist eine unabhängige Arraykopie mit Indexbeginn null.

Überall verfügbar.

## array.prefix(while:)

```text
array.prefix(while: predicate) -> [T]  // array: [T], predicate: func(T) -> Bool
```

Kopiert die anfänglichen Elemente, solange das Prädikat wahr ist. Nach dem ersten `false` wird das Prädikat nicht erneut aufgerufen. Empfänger und Funktionswert werden je einmal ausgewertet; das Ergebnis ist eine unabhängige Arraykopie mit Indexbeginn null.

Überall verfügbar.

## array.reduce(_:_:)

```text
array.reduce(initial, combine) -> U  // array: [T], combine: func(U, T) -> U
```

Faltet Elemente in Arrayreihenfolge mit einem Akkumulator. Ein leeres Array liefert initial ohne Funktionsaufruf. Besitzende Akkumulatoren und Fehler werden nach den Wertregeln behandelt.

Überall verfügbar.

## array.removeAll()

```text
array.removeAll() -> Void  // array: var [T]
```

Leert einen veränderlichen Arraypfad. Vorhandene Snapshots bleiben gültig.

Überall verfügbar.

## array.removeAll(where:)

```text
array.removeAll(where: predicate) -> Void  // array: var [T]
// predicate: func(T) -> Bool
```

Entfernt passende Elemente und erhält die Reihenfolge der übrigen. Das Prädikat läuft einmal je Element des Snapshots. Bei Fehlern bleibt der mutierende Empfänger unverändert.

Überall verfügbar.

## array.removeFirst()

```text
array.removeFirst() -> T  // array: var [T]
```

Entfernt und kopiert das Endelement unabhängig. Ein leeres Array ist ein Laufzeitfehler; Kopierfehler lassen den Empfänger unverändert.

Überall verfügbar.

## array.removeFirst(_:)

```text
array.removeFirst(count) -> Void  // array: var [T], count: Int64
```

Entfernt count Elemente vom entsprechenden Ende. Erlaubt sind 0 bis array.count; Fehler lassen den Empfänger unverändert.

Überall verfügbar.

## array.removeLast()

```text
array.removeLast() -> T  // array: var [T]
```

Entfernt und kopiert das Endelement unabhängig. Ein leeres Array ist ein Laufzeitfehler; Kopierfehler lassen den Empfänger unverändert.

Überall verfügbar.

## array.removeLast(_:)

```text
array.removeLast(count) -> Void  // array: var [T], count: Int64
```

Entfernt count Elemente vom entsprechenden Ende. Erlaubt sind 0 bis array.count; Fehler lassen den Empfänger unverändert.

Überall verfügbar.

## array.removeSubrange(_:)

```text
array.removeSubrange(start..<end) -> Void  // array: var [T]
```

Entfernt einen Bereich. Auch start...end ist möglich. Grenzen müssen ausdrücklich angegeben und gültig sein. Kopierfehler lassen den Empfänger unverändert.

Überall verfügbar.

## array.replaceSubrange(_:with:)

```text
array.replaceSubrange(start..<end, with: values) -> Void
// array: var [T], values: [T]
```

Ersetzt einen Bereich durch ein gleich typisiertes Array. Auch start...end ist möglich. Grenzen werden vor der Auswertung von values geprüft. Selbstersetzung ist erlaubt; Fehler lassen den Empfänger unverändert.

Überall verfügbar.

## array.sort(by:)

```text
array.sort(by: compare) -> Void  // array: var [T]
// compare: func(T, T) -> Bool
```

Sortiert stabil nach einer Vergleichsfunktion. Bei gleichen Elementen bleibt die Eingabereihenfolge erhalten; leere und einzelne Arrays rufen compare nicht auf. Die Sortierung nutzt eine unabhängige Kopie. Bei Fehlern bleibt der mutierende Empfänger unverändert.

Überall verfügbar.

## array.sorted(by:)

```text
array.sorted(by: compare) -> [T]  // array: [T]
// compare: func(T, T) -> Bool
```

Sortiert stabil nach einer Vergleichsfunktion. Bei gleichen Elementen bleibt die Eingabereihenfolge erhalten; leere und einzelne Arrays rufen compare nicht auf. Die Sortierung nutzt eine unabhängige Kopie. Bei Fehlern bleibt der mutierende Empfänger unverändert.

Überall verfügbar.

## array.split(separator:maxSplits:omittingEmptySubsequences:)

```text
array.split(separator: value, maxSplits: limit, omittingEmptySubsequences: omit) -> [[T]]  // array: [T], T: Equatable
```

`value: T` ist erforderlich. `limit: Int64` ist optional und standardmäßig unbegrenzt; ein negativer Wert ist ein Laufzeitfehler. `omit: Bool` ist optional und standardmäßig `true`. Ausgelassene leere Teilarrays zählen nicht gegen das Split-Limit. Teilarrays besitzen unabhängige Wertsemantik und beginnen bei Index null.

Überall verfügbar.

## array.starts(with:)

```text
array.starts(with: prefix) -> Bool  // array, prefix: [T], T: Equatable
```

Prüft den gleich langen Anfang gegen prefix. Der leere Präfix passt immer, ein längerer nie. Beide Eingabewerte bleiben unverändert.

Überall verfügbar.

## array.suffix(_:)

```text
array.suffix(count) -> [T]  // array: [T], count: Int64
```

Wählt bis zu count Elemente aus oder lässt sie weg. Negative Werte sind Fehler; Werte über der Länge werden begrenzt. Das Ergebnis ist eine unabhängige Arraykopie mit Indexbeginn null.

Überall verfügbar.

## array.swapAt(_:_:)

```text
array.swapAt(i, j) -> Void  // array: var [T], i: Int64, j: Int64
```

Vertauscht zwei vorhandene Elemente. Gleiche Indizes haben keine Wirkung; ungültige Indizes sind Laufzeitfehler. Snapshots bleiben gültig, und Kopierfehler lassen den Empfänger unverändert. Die Kopie benötigt O(n).

Überall verfügbar.

## arrow

```text
arrow(start: Vec3, end: Vec3, radius: Float64, color: Int64, id: Int64) -> Void
```

Pfeil von start zur Spitze end mit Schaftradius. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## asin

```text
asin(value: Float64) -> Float64
```

Arkussinus eines Werts in [-1, 1], Ergebnis in Radiant.

Überall verfügbar.

## atan

```text
atan(value: Float64) -> Float64
```

Arkustangens eines Werts, Ergebnis in Radiant.

Überall verfügbar.

## atan2

```text
atan2(y: Float64, x: Float64) -> Float64
```

Winkel des Vektors (x, y) in Radiant; beide Komponenten dürfen nicht zugleich null sein.

Überall verfügbar.

## attempt

```text
attempt(expression) -> T?  // expression: T
```

Führt einen Wertausdruck einmal aus. Ein abgefangener Laufzeitfehler liefert `nil`; temporäre Werte werden aufgeräumt. Bereits sichtbare Seiteneffekte bleiben bestehen. Der Ausdruck darf nicht `Void` sein.

Überall verfügbar.

## box

```text
box(center: Vec3, size: Vec3, rotation: Quat, color: Int64, id: Int64) -> Void
```

Box mit Mittelpunkt, vollen XYZ-Ausdehnungen in Metern und Quaternionrotation. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## buoyancyForce

```text
buoyancyForce(density: Float64, volume: Float64, gravity: Vec3) -> Vec3
```

Auftrieb entgegen der Schwerkraft aus Dichte in kg/m³, verdrängtem Volumen in m³ und Gravitationsvektor in m/s².

Überall verfügbar.

## ceil

```text
ceil(value: Float64) -> Float64
```

Kleinste ganze Float64-Zahl, die nicht kleiner als value ist.

Überall verfügbar.

## clamp

```text
clamp(value: Float64, lower: Float64, upper: Float64) -> Float64
```

Begrenzt value auf das inklusive Intervall. Wenn lower größer als upper ist, entsteht ein Laufzeitfehler mit Quellposition.

Überall verfügbar.

## cos

```text
cos(angle: Float64) -> Float64
```

Kosinus eines Winkels in Radiant.

Überall verfügbar.

## currentDiagnostic

```text
currentDiagnostic() -> Diagnostic
```

Liefert eine Kopie der aktuellen Experimentdiagnose, bei Erfolg einen leeren Wert. Kein globaler Last-error-Zustand.

Experimentmodul erforderlich.

## diagnosticArgument

```text
diagnosticArgument(diagnostic: Diagnostic) -> String
```

Liefert das betroffene Argument als eigenen String-Wert.

Überall verfügbar.

## diagnosticCode

```text
diagnosticCode(diagnostic: Diagnostic) -> Int64
```

Liefert den unveränderten Core-Fehlercode; 0 bezeichnet keinen Fehler.

Überall verfügbar.

## diagnosticColumn

```text
diagnosticColumn(diagnostic: Diagnostic) -> Int64
```

Liefert die 1-basierte Spalte oder 0 bei unbekannter Spalte.

Überall verfügbar.

## diagnosticDecoded

```text
diagnosticDecoded(bytes: [Int64]) -> Diagnostic
```

Dekodiert ein vollständiges Bytearray mit Version-, Längen-, CRC- und UTF-8-Prüfung; Fehler können mit attempt abgefangen werden.

Überall verfügbar.

## diagnosticEncoded

```text
diagnosticEncoded(diagnostic: Diagnostic) -> [Int64]
```

Kodiert einen Fehler als begrenztes versioniertes Bytearray mit CRC; Int64-Werte 0–255.

Überall verfügbar.

## diagnosticFormatted

```text
diagnosticFormatted(diagnostic: Diagnostic) -> String
```

Formatiert den Diagnosewert für die menschliche Anzeige; Quelldaten bleiben separat erhalten.

Überall verfügbar.

## diagnosticHere

```text
diagnosticHere(code: Int64, operation: String, argument: String, message: String) -> Diagnostic
```

Wie Diagnostic mit automatisch erfasstem Quellpfad, Zeile und Spalte dieser Factory-Expression.

Überall verfügbar.

## diagnosticLine

```text
diagnosticLine(diagnostic: Diagnostic) -> Int64
```

Liefert die 1-basierte Zeile oder 0 bei unbekannter Quellposition.

Überall verfügbar.

## diagnosticMessage

```text
diagnosticMessage(diagnostic: Diagnostic) -> String
```

Liefert den vollständigen UTF-8-Nachrichtentext als eigenen String-Wert.

Überall verfügbar.

## diagnosticOperation

```text
diagnosticOperation(diagnostic: Diagnostic) -> String
```

Liefert die Operation als eigenen String-Wert.

Überall verfügbar.

## diagnosticSource

```text
diagnosticSource(diagnostic: Diagnostic) -> String
```

Liefert den ursprünglichen Quellpfad als eigenen String-Wert.

Überall verfügbar.

## diagnosticValid

```text
diagnosticValid(diagnostic: Diagnostic) -> Bool
```

Prüft die Versions-, Text-, Fehlercode- und Positionsregeln des Werts.

Überall verfügbar.

## emptyDiagnostic

```text
emptyDiagnostic() -> Diagnostic
```

Erzeugt einen gültigen leeren Diagnosewert mit Code 0.

Überall verfügbar.

## eulerStep

```text
eulerStep(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], time: Float64, dt: Float64) -> [Float64]
```

Expliziter Euler-Schritt mit der gemeinsamen C-Integrationsroutine. derivative bezeichnet eine freie, nicht generische Funktion des aktuellen Moduls mit (Float64, [Float64]) -> [Float64]. Der Zustand enthält 1 bis 32 endliche Werte; dt muss endlich und positiv sein. Der Callback erhält eine eigene Kopie des aktuellen Zustands und muss gleich viele endliche Ableitungswerte liefern. Das Ergebnis ist ein unabhängiges Array; die Eingabe bleibt unverändert. Ungültige Eingaben, abweichende Dimensionen und nicht endliche Zwischenwerte erzeugen Quelldiagnosen. Für reproduzierbare Ergebnisse sollte der Callback keine sichtbaren Seiteneffekte haben.

Überall verfügbar.

## exp

```text
exp(value: Float64) -> Float64
```

Exponentialfunktion zur Basis e; nicht endliche Ergebnisse sind Laufzeitfehler.

Überall verfügbar.

## floor

```text
floor(value: Float64) -> Float64
```

Größte ganze Float64-Zahl, die nicht größer als value ist.

Überall verfügbar.

## group

```text
group(name: String, id: Int64, parent: Int64) -> Void
```

Benannte Szenengruppe mit eindeutiger ID; parent 0 erzeugt eine Wurzel. Nur im scene-Callback. Gruppen enthalten keine Geometrie und verändern keine Weltkoordinaten.

Experimentmodul erforderlich.

## hypot

```text
hypot(x: Float64, y: Float64) -> Float64
```

Berechnet die euklidische Länge von (x, y) mit skalierter Arithmetik.

Überall verfügbar.

## inputCount

```text
inputCount() -> Int64
```

Anzahl der für diese Analyse ausgewählten Eingabeläufe.

Analysemodul erforderlich.

## inputPath

```text
inputPath(index: Int64) -> String
```

Liefert einen eigenen UTF-8-String mit dem ausgewählten Eingabepfad. Nullbasierter Index muss kleiner als inputCount sein. Nur in Analysemodulen verfügbar.

Analysemodul erforderlich.

## label

```text
label(position: Vec3, text: String, color: Int64, id: Int64) -> Void
```

UTF-8-Beschriftung an einem Weltpunkt, höchstens 63 Bytes. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## line

```text
line(start: Vec3, end: Vec3, radius: Float64, color: Int64, id: Int64) -> Void
```

Linie von start nach end mit räumlichem Linienradius. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## linearSolve

```text
linearSolve(coefficients: [Float64], rhs: [Float64], pivotTolerance: Float64) -> [Float64]
```

Löst A*x = rhs mit skalierter Pivotwahl. rhs enthält 1 bis 32 Werte; coefficients enthält genau rhs.count² Werte in Zeilenreihenfolge. pivotTolerance=0 wählt n mal die Maschinengenauigkeit, sonst gilt 0<t<1. Eingaben bleiben unverändert. Formfehler, Singularität und numerische Fehler erzeugen Quelldiagnosen. Das Ergebnis ist ein eigener Array-Wert im Sprachspeicherbudget.

Überall verfügbar.

## loadDiagnostic

```text
loadDiagnostic(path: String) -> Diagnostic
```

Lädt und validiert einen vollständig gespeicherten Diagnosewert.

Überall verfügbar.

## log

```text
log(value: Float64) -> Float64
```

Natürlicher Logarithmus; der Wert muss positiv sein.

Überall verfügbar.

## log10

```text
log10(value: Float64) -> Float64
```

Zehnerlogarithmus; der Wert muss positiv sein.

Überall verfügbar.

## logDebug

```text
logDebug(message: String) -> Bool
```

Schreibt eine Debug-Meldung mit aktueller Simulationszeit. Nur im Experiment; Bool meldet Annahme durch den Hostlogger.

Experimentmodul erforderlich.

## logError

```text
logError(message: String) -> Bool
```

Schreibt eine Fehlermeldung; der Schweregrad beendet die Simulation nicht. Prüfe Bool bei Bedarf.

Experimentmodul erforderlich.

## logInfo

```text
logInfo(message: String) -> Bool
```

Schreibt eine Info-Meldung mit aktueller Simulationszeit. Nur im Experiment; false bei ungültigem Text, ausgeschöpftem Budget oder I/O-Fehler.

Experimentmodul erforderlich.

## logWarning

```text
logWarning(message: String) -> Bool
```

Schreibt eine Warnung mit aktueller Simulationszeit; verändert weder Modellzustand noch Messwerte.

Experimentmodul erforderlich.

## maskSeries

```text
maskSeries(input: Series, selector: Series, accepted: Float64) -> Series
```

Erhält alle Zeilen und markiert Werte nur dann gültig, wenn Eingabe und Selektor gültig sind und der Selektor exakt accepted entspricht.

Analysemodul erforderlich.

## max

```text
max(left: Float64, right: Float64) -> Float64
```

Größerer Wert; bei Gleichheit bleibt der linke Wert erhalten.

Überall verfügbar.

## metadata

```text
metadata(text: String) -> Void
```

Setzt beschreibenden UTF-8-Modelltext, etwa Parameter und Methode.

Experimentmodul erforderlich.

## min

```text
min(left: Float64, right: Float64) -> Float64
```

Kleinerer Wert; bei Gleichheit bleibt der linke Wert erhalten.

Überall verfügbar.

## minimizeGolden

```text
minimizeGolden(function: func(Float64) -> Float64, lower: Float64, upper: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxIterations: Int64) -> Float64
```

Sucht das Minimum einer unimodalen skalaren Funktion mit dem gemeinsamen Goldener-Schnitt-Verfahren. function bezeichnet eine freie, nicht generische Funktion des aktuellen Moduls mit genau einem Float64-Parameter und Float64-Rückgabe. Das Intervall muss endlich und aufsteigend sein; absoluteTolerance>0, 0<=relativeTolerance<1 und 1<=maxIterations<=100000 sind erforderlich. Das Ergebnis ist die gefundene X-Koordinate; ungültige Argumente, nicht endliche Funktionswerte und fehlende Konvergenz erzeugen Quelldiagnosen. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein; ein numerischer Bericht wird noch nicht ausgegeben.

Überall verfügbar.

## minimizeGoldenReported

```text
minimizeGoldenReported(function: func(Float64) -> Float64, lower: Float64, upper: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxIterations: Int64) -> ScalarResult
```

Wie minimizeGolden mit denselben Eingaben und Fehlerregeln. ScalarResult enthält x und den zugehörigen Funktionswert value, die letzten Intervallgrenzen lower und upper sowie iterations und evaluations. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein.

Überall verfügbar.

## outputPrefix

```text
outputPrefix() -> String
```

Kopiert das Ausgabeprefix des laufenden Analysehosts in einen besitzenden String; etwa als Grundlage für einen neuen Serienordner.

Analysemodul erforderlich.

## parameter

```text
parameter(name: String, default: Float64, minimum: Float64, maximum: Float64, description: String) -> Float64
```

Definiert beim Erzeugen des Experiments einen benannten Float64-Parameter mit Standardwert, inklusiven Grenzen und Beschreibung. Liefert den wirksamen Wert nach dem Runner-Override. Nur während globaler Initialisierung oder create; eindeutiger Name, endliche Werte und höchstens 16 Parameter.

Experimentmodul erforderlich.

## parameterWithUnit

```text
parameterWithUnit(name: String, unit: Unit, default: Float64, minimum: Float64, maximum: Float64, description: String) -> Float64
```

Wie parameter, mit einer eigenen Anzeigeeinheit aus Symbol, positiver Skala und sieben SI-Dimensionen. Standard, Grenzen, Override und Rückgabewert bleiben SI-Zahlen. Die GUI konvertiert Eingaben, Studienberichte skalieren ihre X-Achse; Rohdaten und CSV bewahren SI. Das Symbol muss gültiges UTF-8 ohne Steuerzeichen mit höchstens 15 Bytes sein.

Experimentmodul erforderlich.

## plane

```text
plane(center: Vec3, size: Vec2, rotation: Quat, color: Int64, id: Int64) -> Void
```

Ebene mit Mittelpunkt, vollen X/Z-Seitenlängen und Quaternionrotation. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## point

```text
point(position: Vec3, radius: Float64, color: Int64, id: Int64) -> Void
```

Punktmarker an position. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## polyline

```text
polyline(points: [Vec3], radius: Float64, color: Int64, id: Int64) -> Void
```

Linienzug aus mindestens zwei Weltpunkten; Szene insgesamt maximal 96 Punkte. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## pow

```text
pow(base: Float64, exponent: Float64) -> Float64
```

Potenziert base mit exponent; nicht reelle oder nicht endliche Ergebnisse sind Laufzeitfehler.

Überall verfügbar.

## quadraticDrag

```text
quadraticDrag(relativeVelocity: Vec3, density: Float64, radius: Float64, coefficient: Float64) -> Vec3
```

Quadratischer Kugelwiderstand mit Dichte in kg/m³, Radius in m und dimensionslosem Widerstandsbeiwert.

Überall verfügbar.

## raiseDiagnostic

```text
raiseDiagnostic(diagnostic: Diagnostic) -> Void
```

Löst einen Fehler mit den Feldern dieses Diagnosewerts aus. attempt fängt ihn innerhalb eines Wertausdrucks ab; außerhalb endet der Callback bzw. das Standalone-Programm.

Überall verfügbar.

## randomNormal

```text
randomNormal(mean: Float64, standardDeviation: Float64) -> Float64
```

Zieht normalverteilte Werte aus dem Laufzufallsstrom mit explizitem Mittelwert und Streuung.

Experimentmodul erforderlich.

## randomUniform

```text
randomUniform(min: Float64, max: Float64) -> Float64
```

Zieht aus dem reproduzierbaren Laufzufallsstrom zwischen min und max.

Experimentmodul erforderlich.

## report

```text
report(title: String) -> Void
```

Setzt den Titel des Analyseberichts.

Analysemodul erforderlich.

## rk45Integrate

```text
rk45Integrate(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], start: Float64, end: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxSteps: Int64) -> [Float64]
```

Adaptiver Dormand–Prince-5(4)-Integrator der gemeinsamen C-Bibliothek. derivative ist eine freie, nicht generische Funktion mit (Float64, [Float64]) -> [Float64]. state enthält 1 bis 32 endliche Werte und bleibt unverändert; das Ergebnis ist ein eigenes Array. Die endlichen Grenzen dürfen vorwärts oder rückwärts verlaufen. absoluteTolerance muss positiv und endlich, relativeTolerance endlich und in [0, 1) und maxSteps zwischen 1 und UINT32_MAX/7 liegen. Das Schrittbudget zählt angenommene und verworfene Versuche. Abbrüche melden die Bibliotheksursache mit Quellposition. Der Callback soll deterministisch und frei von sichtbaren Seiteneffekten sein, da Stufen und verworfene Versuche ihn mehrfach aufrufen. Anfangs-, Mindest- und Höchstschritt folgen den Bibliotheksvorgaben; ein numerischer Bericht wird noch nicht ausgegeben.

Überall verfügbar.

## rk45IntegrateReported

```text
rk45IntegrateReported(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], start: Float64, end: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxSteps: Int64, initialStep: Float64, minimumStep: Float64, maximumStep: Float64) -> OdeResult
```

Adaptiver Dormand–Prince-5(4)-Integrator mit denselben Eingaben und Regeln wie rk45IntegrateWithSteps. Das kopierbare OdeResult enthält state als unabhängiges [Float64] sowie acceptedSteps, rejectedSteps, evaluations, reachedTime, nextStep und errorNorm aus dem numerischen Bericht der C-Bibliothek. acceptedSteps und rejectedSteps zählen Schrittversuche; evaluations zählt Ableitungsaufrufe. reachedTime ist die erreichte Zeit, nextStep die vorzeichenbehaftete nächste Schrittweite und errorNorm die letzte skalierte Fehlernorm. Ungültige Optionen, Callbackfehler und erschöpftes Schrittbudget erzeugen Quelldiagnosen.

Überall verfügbar.

## rk45IntegrateWithSteps

```text
rk45IntegrateWithSteps(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], start: Float64, end: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxSteps: Int64, initialStep: Float64, minimumStep: Float64, maximumStep: Float64) -> [Float64]
```

Wie rk45Integrate, zusätzlich mit explizitem initialStep, minimumStep und maximumStep. Alle drei Schrittweiten müssen positiv und endlich sein; maximumStep muss mindestens minimumStep betragen. Der Integrator begrenzt den Anfangsschritt auf das erlaubte Intervall und passt spätere Versuche adaptiv an. Vorwärts- und Rückwärtsintegration verwenden positive Schrittweitenbeträge. Die Eingabe bleibt unverändert und das Ergebnis besitzt seinen Array-Speicher selbst. Ungültige Optionen, numerische Fehler und ausgeschöpftes Schrittbudget melden die Bibliotheksursache mit Quellposition. Diese Funktion stellt alle skalaren ps_ode_options-Felder bereit; komponentenweise Toleranzen bietet rk45IntegrateWithTolerances. Ein numerischer Bericht wird noch nicht ausgegeben.

Überall verfügbar.

## rk45IntegrateWithTolerances

```text
rk45IntegrateWithTolerances(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], start: Float64, end: Float64, absoluteTolerances: [Float64], relativeTolerance: Float64, maxSteps: Int64, initialStep: Float64, minimumStep: Float64, maximumStep: Float64) -> [Float64]
```

Adaptiver Dormand–Prince-5(4)-Integrator mit vollständigen Schrittweitenoptionen und einem absoluten Toleranzwert pro Zustandskomponente. absoluteTolerances muss genau state.count Werte enthalten (1 bis 32); jeder Wert muss positiv und endlich sein. Die Toleranzen werden vor der ersten Ableitungsauswertung kopiert, damit der Integrationslauf einen festen Satz verwendet. relativeTolerance muss endlich und in [0, 1) liegen. Anfangs-, Mindest- und Höchstschritt müssen positiv und endlich sein; maximumStep muss mindestens minimumStep betragen. Der benannte Callback hat (Float64, [Float64]) -> [Float64] und soll keine sichtbaren Seiteneffekte haben. Eingaben bleiben unverändert, das Ergebnis ist ein unabhängiges Array. Ungültige Form, Toleranzen, erschöpftes Schrittbudget und numerische Fehler erzeugen Quelldiagnosen. Ein numerischer Bericht wird noch nicht ausgegeben.

Überall verfügbar.

## rk45IntegrateWithTolerancesReported

```text
rk45IntegrateWithTolerancesReported(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], start: Float64, end: Float64, absoluteTolerances: [Float64], relativeTolerance: Float64, maxSteps: Int64, initialStep: Float64, minimumStep: Float64, maximumStep: Float64) -> OdeResult
```

Wie rk45IntegrateWithTolerances mit einem positiven absoluten Toleranzwert pro Zustandskomponente, jedoch mit einem kopierbaren OdeResult statt nur des Zustandsarrays. state besitzt unabhängigen Speicher; acceptedSteps, rejectedSteps, evaluations, reachedTime, nextStep und errorNorm stammen aus demselben C-Integrationslauf. Das Toleranzarray wird vor dem ersten Callback kopiert. Formfehler, ungültige Toleranzen, ausgeschöpftes Schrittbudget und numerische Fehler erzeugen Quelldiagnosen.

Überall verfügbar.

## rk45StepReported

```text
rk45StepReported(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], start: Float64, end: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxSteps: Int64, initialStep: Float64, minimumStep: Float64, maximumStep: Float64) -> OdeResult
```

Wie rk45IntegrateReported, beendet aber nach genau einem akzeptierten Dormand–Prince-Schritt. start und end müssen verschieden sein; reachedTime kann vor end liegen. Verwerfungen verbrauchen das Versuchslimit, Eingaben bleiben erhalten und state ist ein eigener Array-Wert. nextStep enthält den vorzeichenbehafteten nächsten Vorschlag innerhalb der Optionen. Lokale Fehlertoleranzen sind keine globale Fehlergrenze.

Überall verfügbar.

## rk4Step

```text
rk4Step(derivative: func(Float64, [Float64]) -> [Float64], state: [Float64], time: Float64, dt: Float64) -> [Float64]
```

Klassischer RK4-Schritt mit vier Ableitungsauswertungen der gemeinsamen C-Integrationsroutine. Signatur, Zustandsgrenze, Schrittbedingung, Besitz und Fehlerregeln entsprechen eulerStep. Die Ableitung erhält Zeit und jeweiligen Zwischenzustand als eigene Werte. Adaptive Schrittweite und Ereigniserkennung sind hier nicht enthalten.

Überall verfügbar.

## rootBisect

```text
rootBisect(function: func(Float64) -> Float64, lower: Float64, upper: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxIterations: Int64) -> Float64
```

Sucht eine Nullstelle einer stetigen Funktion mit der gemeinsamen C-Bisektionsroutine. function bezeichnet eine freie, nicht generische Funktion des aktuellen Moduls mit genau einem Float64-Parameter und Float64-Rückgabe. Endliche, aufsteigende Intervallgrenzen müssen eine Nullstelle einklammern oder selbst Nullstelle sein. absoluteTolerance>0, 0<=relativeTolerance<1 und 1<=maxIterations<=100000 sind erforderlich. Das Ergebnis ist die gefundene X-Koordinate; ungültige Argumente, nicht endliche Funktionswerte und fehlende Konvergenz erzeugen Quelldiagnosen. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein; ein numerischer Bericht wird noch nicht ausgegeben.

Überall verfügbar.

## rootBisectReported

```text
rootBisectReported(function: func(Float64) -> Float64, lower: Float64, upper: Float64, absoluteTolerance: Float64, relativeTolerance: Float64, maxIterations: Int64) -> ScalarResult
```

Wie rootBisect mit denselben Eingaben und Fehlerregeln. ScalarResult enthält x und den zugehörigen Funktionswert value, die letzten Intervallgrenzen lower und upper sowie iterations und evaluations. Ein bei einer Intervallgrenze liegender Nullpunkt benötigt keine Iteration. Der Callback sollte deterministisch und frei von sichtbaren Seiteneffekten sein.

Überall verfügbar.

## round

```text
round(value: Float64) -> Float64
```

Nächste ganze Float64-Zahl; ein exakter Gleichstand wird von null weg gerundet.

Überall verfügbar.

## runBlock_channels

```text
runBlock_channels(block: RunBlock) -> Int64
```

Liefert Kanalzahl. Der Block bleibt nach Schließen des Index verwendbar; ungültige Indizes werfen Fehler.

Überall verfügbar.

## runBlock_column

```text
runBlock_column(block: RunBlock, channel: Int64) -> [Float64]
```

Liefert unabhängiges Float64-Array eines nullbasierten Kanals. Der Block bleibt nach Schließen des Index verwendbar; ungültige Indizes werfen Fehler.

Überall verfügbar.

## runBlock_count

```text
runBlock_count(block: RunBlock) -> Int64
```

Liefert Zeilenzahl. Der Block bleibt nach Schließen des Index verwendbar; ungültige Indizes werfen Fehler.

Überall verfügbar.

## runBlock_time

```text
runBlock_time(block: RunBlock, row: Int64) -> Float64
```

Liefert Zeit an der nullbasierten Zeile. Der Block bleibt nach Schließen des Index verwendbar; ungültige Indizes werfen Fehler.

Überall verfügbar.

## runBlock_times

```text
runBlock_times(block: RunBlock) -> [Float64]
```

Liefert unabhängiges Float64-Array aller Zeiten. Der Block bleibt nach Schließen des Index verwendbar; ungültige Indizes werfen Fehler.

Überall verfügbar.

## runBlock_value

```text
runBlock_value(block: RunBlock, row: Int64, channel: Int64) -> Float64
```

Liefert Messwert an nullbasierter Zeile und Kanal. Der Block bleibt nach Schließen des Index verwendbar; ungültige Indizes werfen Fehler.

Überall verfügbar.

## runIndexClose

```text
runIndexClose(run: RunIndex) -> Void
```

Gibt die Referenz dieses veränderbaren Werts frei und schließt ihn. Andere Kopien bleiben verwendbar; die Datei schließt nach der letzten Referenz. Wiederholtes Schließen ist erlaubt.

Überall verfügbar.

## runIndexDimension

```text
runIndexDimension(run: RunIndex, channel: Int64, axis: Int64) -> Int64
```

Liest den SI-Dimensionsexponenten am Kanal und Achsenindex 0–6: Länge, Masse, Zeit, Strom, Temperatur, Stoffmenge, Lichtstärke.

Überall verfügbar.

## runIndexRead

```text
runIndexRead(run: RunIndex, first: Int64, count: Int64) -> RunBlock
```

Liest einen unabhängigen besitzenden RunBlock mit 0–256 Zeilen ab first. Ungültige Grenzen oder CRC lösen typisierte Fehler aus; attempt liefert dann nil.

Überall verfügbar.

## runIndexSnapshot

```text
runIndexSnapshot(run: RunIndex, ordinal: Int64) -> RunSnapshot
```

Liest und validiert einen vollständigen kopierbaren RunSnapshot anhand seiner nullbasierten Szenennummer. Werte, Szene, Eltern und TRS bleiben erhalten.

Überall verfügbar.

## runIndex_channels

```text
runIndex_channels(run: RunIndex) -> Int64
```

Liefert die Kanalzahl. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_checkpoints

```text
runIndex_checkpoints(run: RunIndex) -> Int64
```

Liefert die gemeinsame Zahl aller Checkpoints. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_complete

```text
runIndex_complete(run: RunIndex) -> Bool
```

Liefert ob ein gültiger Footer vorliegt. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_description

```text
runIndex_description(run: RunIndex, channel: Int64) -> String
```

Liefert die Kanalbeschreibung als eigenen String. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_is_open

```text
runIndex_is_open(run: RunIndex) -> Bool
```

Liefert den Öffnungszustand ohne Fehler auch nach close. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_metadata

```text
runIndex_metadata(run: RunIndex) -> String
```

Liefert einen eigenen Metadatenstring. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_name

```text
runIndex_name(run: RunIndex, channel: Int64) -> String
```

Liefert den Kanalnamen als eigenen String. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_persisted

```text
runIndex_persisted(run: RunIndex) -> Bool
```

Liefert ob der gespeicherte Index dem geprüften Präfix entspricht. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_samples

```text
runIndex_samples(run: RunIndex) -> Int64
```

Liefert die Zahl validierter Messzeilen. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_snapshots

```text
runIndex_snapshots(run: RunIndex) -> Int64
```

Liefert die Zahl validierter Szenen. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runIndex_symbol

```text
runIndex_symbol(run: RunIndex, channel: Int64) -> String
```

Liefert das Kanalsymbol als eigenen String. Kanalindizes sind nullbasiert; geschlossene Handles werfen einen Fehler, außer isOpen.

Überall verfügbar.

## runSeed

```text
runSeed() -> Int64
```

Liefert das vollständige 64-Bit-Bitmuster des aktuellen Laufseeds als Int64. Auch Seedwerte oberhalb von INT64_MAX bleiben beim Zurückwandeln in einen Zufallsstrom erhalten; nur im Experiment.

Experimentmodul erforderlich.

## runSnapshot_channels

```text
runSnapshot_channels(snapshot: RunSnapshot) -> Int64
```

Liefert Kanalzahl aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_color

```text
runSnapshot_color(snapshot: RunSnapshot, index: Int64) -> Int64
```

Liefert RGBA-Farbwert aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_id

```text
runSnapshot_id(snapshot: RunSnapshot, index: Int64) -> Int64
```

Liefert stabile Objekt-ID aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_objects

```text
runSnapshot_objects(snapshot: RunSnapshot) -> Int64
```

Liefert Objektzahl aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_parent

```text
runSnapshot_parent(snapshot: RunSnapshot, index: Int64) -> Int64
```

Liefert Eltern-ID aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_paused

```text
runSnapshot_paused(snapshot: RunSnapshot) -> Bool
```

Liefert Pausestatus aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_point

```text
runSnapshot_point(snapshot: RunSnapshot, index: Int64) -> Vec3
```

Liefert lokalen Punkt im gemeinsamen Punktpool aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_point_count

```text
runSnapshot_point_count(snapshot: RunSnapshot, index: Int64) -> Int64
```

Liefert Länge des Polyline-Bereichs aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_point_first

```text
runSnapshot_point_first(snapshot: RunSnapshot, index: Int64) -> Int64
```

Liefert Anfang des Polyline-Bereichs aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_points

```text
runSnapshot_points(snapshot: RunSnapshot) -> Int64
```

Liefert Punktzahl aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_position

```text
runSnapshot_position(snapshot: RunSnapshot, index: Int64) -> Vec3
```

Liefert lokales a.xyz (Frame-Translation) aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_radius

```text
runSnapshot_radius(snapshot: RunSnapshot, index: Int64) -> Float64
```

Liefert Radius aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_rotation

```text
runSnapshot_rotation(snapshot: RunSnapshot, index: Int64) -> Quat
```

Liefert Quaternionorientierung aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_shape

```text
runSnapshot_shape(snapshot: RunSnapshot, index: Int64) -> Int64
```

Liefert Formnummer aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_size

```text
runSnapshot_size(snapshot: RunSnapshot, index: Int64) -> Vec3
```

Liefert lokales b.xyz (Boxausdehnung oder Frame-Skalierung) aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_text

```text
runSnapshot_text(snapshot: RunSnapshot, index: Int64) -> String
```

Liefert eigenen Beschriftungsstring aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_time

```text
runSnapshot_time(snapshot: RunSnapshot) -> Float64
```

Liefert Zeit aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_transform

```text
runSnapshot_transform(snapshot: RunSnapshot, index: Int64) -> Mat4
```

Liefert die lokale-zu-Welt-Matrix eines Objektslots aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_value

```text
runSnapshot_value(snapshot: RunSnapshot, channel: Int64) -> Float64
```

Liefert Messwert am Kanal aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## runSnapshot_world_point

```text
runSnapshot_world_point(snapshot: RunSnapshot, index: Int64, local: Vec3) -> Vec3
```

Liefert den Weltpunkt aus lokalem Punkt und Objektslot aus dem vollständigen Snapshot. Kanal-, Objekt- und Punktindizes sind nullbasiert und werden geprüft; Objektindizes sind keine IDs.

Überall verfügbar.

## saveDiagnostic

```text
saveDiagnostic(diagnostic: Diagnostic, path: String) -> Void
```

Speichert einen Fehler exklusiv in eine neue Datei; vorhandene Dateien bleiben erhalten.

Überall verfügbar.

## sceneFrame

```text
sceneFrame(name: String, id: Int64, parent: Int64, translation: Vec3, rotation: Quat, scale: Vec3) -> Void
```

Erzeugt einen expliziten lokalen TRS-Koordinatenrahmen. Nur im scene-Callback; parent 0 bezeichnet die Wurzel. Endliche Translation, nonzero Quaternion und endliche nonzero Scale; Nachfahren werden für Darstellung und Picking transformiert.

Experimentmodul erforderlich.

## sceneParent

```text
sceneParent(child: Int64, parent: Int64) -> Void
```

Ordnet einen Szeneneintrag einer Eltern-ID zu; parent 0 löst ihn zur Wurzel. Fehlende IDs und Zyklen erzeugen eine Quelldiagnose, ohne die Szene zu verändern.

Experimentmodul erforderlich.

## sceneTransform

```text
sceneTransform(index: Int64) -> Mat4
```

Liefert die zusammengesetzte Local-to-world-Matrix des Szenenslots. Nur im scene-Callback; index beginnt bei 0. Rahmen enthalten ihre eigene TRS, Geometrie nur ihre Frame-Vorfahren.

Experimentmodul erforderlich.

## sceneWorldPoint

```text
sceneWorldPoint(index: Int64, point: Vec3) -> Vec3
```

Konvertiert einen Punkt in der Basis des Szenenslots in Weltkoordinaten; nur im scene-Callback. Fehler erhalten eine Quelldiagnose.

Experimentmodul erforderlich.

## seriesHasMask

```text
seriesHasMask(input: Series) -> Bool
```

Prüft die ausdrücklich gespeicherte Maske der Reihe, auch wenn alle Werte gültig sind.

Analysemodul erforderlich.

## seriesIsValid

```text
seriesIsValid(input: Series, index: Int64) -> Bool
```

Prüft die Gültigkeit am nullbasierten Zeilenindex; ungültige Handles oder Indizes sind Quellfehler.

Analysemodul erforderlich.

## seriesValidity

```text
seriesValidity(input: Series) -> Series
```

Liefert eine ausgerichtete, unmaskierte Reihe aus 0/1-Gültigkeitsflags.

Analysemodul erforderlich.

## simulationTime

```text
simulationTime() -> Float64
```

Aktuelle Hostzeit in Sekunden; beim Eintritt in step die Zeit vor dem Schritt.

Experimentmodul erforderlich.

## sin

```text
sin(angle: Float64) -> Float64
```

Sinus eines Winkels in Radiant.

Überall verfügbar.

## sphere

```text
sphere(center: Vec3, radius: Float64, color: Int64, id: Int64) -> Void
```

Kugel mit Mittelpunkt und Radius in Metern. Nur in scene verwenden. Farbe: dezimales RRGGBBAA, etwa 1407107839; ID 0 ist anonym, andere IDs müssen pro Szene eindeutig sein.

Experimentmodul erforderlich.

## springForce

```text
springForce(position: Vec3, velocity: Vec3, anchor: Vec3, anchorVelocity: Vec3, stiffness: Float64, restLength: Float64, damping: Float64) -> Vec3
```

Axiale Feder-/Dämpferkraft aus Positionen, Geschwindigkeiten, Federkonstante in N/m, Ruhelänge in m und Dämpfung in N·s/m.

Überall verfügbar.

## sqrt

```text
sqrt(value: Float64) -> Float64
```

Quadratwurzel; der Wert muss nichtnegativ sein.

Überall verfügbar.

## stokesDrag

```text
stokesDrag(relativeVelocity: Vec3, viscosity: Float64, radius: Float64) -> Vec3
```

Linearer Stokes-Widerstand für relative Geschwindigkeit in m/s, dynamische Viskosität in Pa·s und Kugelradius in m.

Überall verfügbar.

## tan

```text
tan(angle: Float64) -> Float64
```

Tangens eines Winkels in Radiant; nicht endliche Ergebnisse sind Laufzeitfehler.

Überall verfügbar.

## verletStep

```text
verletStep(acceleration: func(Float64, [Float64]) -> [Float64], phase: [Float64], time: Float64, dt: Float64) -> [Float64]
```

Velocity Verlet mit der gemeinsamen C-Numerik für 1 bis 32 Freiheitsgrade. phase enthält zuerst n endliche Positionen, danach n endliche Geschwindigkeiten; die Länge muss gerade und zwischen 2 und 64 liegen. acceleration ist eine freie, nicht generische Funktion (Float64, [Float64]) -> [Float64], deren Array nur die n Positionen enthält und die n endliche Beschleunigungen liefern muss. Die Beschleunigung darf nicht von der Geschwindigkeit abhängen. dt muss endlich und von null verschieden sein; negative Werte erlauben Rückwärtsschritte. Das Ergebnis ist ein eigenes Array in derselben Reihenfolge, die Eingabe bleibt unverändert. Form-, Zeit- und numerische Fehler erzeugen Quelldiagnosen. Der Callback soll keine sichtbaren Seiteneffekte haben.

Überall verfügbar.
