#include "language/builtins.h"
#include "language/checker.h"
#include <locale.h>
#include <stdio.h>
#include <string.h>

#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language checker line %d: %s\n", __LINE__, #x);                       \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static ps_lang_node nodes[4096];
static ps_lang_semantic info[4096];
static ps_lang_parse_result parsed;
static ps_lang_check_result check(const char *source) {
    parsed = ps_lang_parse(source, strlen(source), nodes, 4096);
    return ps_lang_check(source, strlen(source), nodes, parsed, info, 4096, 4096);
}
static int accepts(const char *source) {
    ps_lang_check_result r = check(source);
    if (!r.ok)
        fprintf(stderr, "%s\n%zu:%zu: %s\n", source, r.diagnostic.line, r.diagnostic.column,
                r.diagnostic.error);
    CHECK(r.ok && r.diagnostic.kind != PS_LANG_ERROR);
    return 0;
}
static int rejects(const char *source, const char *message) {
    ps_lang_check_result r = check(source);
    CHECK(parsed.root); /* Must exercise semantics, not just parser rejection. */
    if (r.ok || !strstr(r.diagnostic.error, message))
        fprintf(stderr, "Source: %s\nExpected: %s; got: %s\n", source, message,
                r.ok ? "accepted" : r.diagnostic.error);
    CHECK(!r.ok && r.diagnostic.kind == PS_LANG_ERROR);
    CHECK(strstr(r.diagnostic.error, message));
    CHECK(r.diagnostic.offset <= strlen(source));
    return 0;
}
int main(void) {
    const char *previous_locale = setlocale(LC_NUMERIC, NULL);
    char saved_locale[128];
    CHECK(previous_locale && strlen(previous_locale) < sizeof saved_locale);
    memcpy(saved_locale, previous_locale, strlen(previous_locale) + 1);
    const char *numeric_locales[] = {"de-DE", "German_Germany.1252", "French_France.1252",
                                     "de_DE.UTF-8", "fr_FR.UTF-8", "de_DE.utf8", "fr_FR.utf8"};
    for (size_t i = 0; i < sizeof numeric_locales / sizeof numeric_locales[0]; i++)
        if (setlocale(LC_NUMERIC, numeric_locales[i]) &&
            strcmp(localeconv()->decimal_point, "."))
            break;
    CHECK(rejects("let value: Float64? = nil\nswitch value:\n"
                  "    case Optional.some(1.5):\n        print(1)\n"
                  "    case Optional.some(1.50):\n        print(2)\n"
                  "    default:\n        print(0)\n", "Duplicate switch case") == 0);
    CHECK(setlocale(LC_NUMERIC, saved_locale) != NULL);
    CHECK(accepts("var parts: [String] = []\nfor scalar in \"aü\":\n"
                  "    parts.append(scalar)\n") == 0);
    CHECK(rejects("for item in 1:\n    print(item)", "array, String or range") == 0);
    CHECK(rejects("for scalar in \"a\":\n    scalar = \"b\"", "mutable var") == 0);
    CHECK(accepts("let text = \"aü\"\nlet reverse: String = text.reversed()\n") == 0);
    CHECK(rejects("let text = \"a\"\nlet reverse = text.reversed(1)",
                  "expects no arguments") == 0);
    CHECK(rejects("var text = \"a\"\ntext.reverse()", "String is immutable") == 0);
    CHECK(accepts("let a = [1,2,3]\nlet yes: Bool = a.contains(2)\n"
                  "let nested = [[1],[2]]\nlet another = nested.contains([2])\n"
                  "let index: Int64? = nested.firstIndex(of: [2])\n"
                  "let last: Int64? = nested.lastIndex(of: [2])\n") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.contains()", "exactly one positional") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.contains(1,2)", "exactly one positional") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.contains(value: 1)", "exactly one positional") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.contains(false)", "required type") == 0);
    CHECK(rejects("let a = 1\nlet b = a.contains(1)", "array value") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.firstIndex()", "of: value") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.firstIndex(1)", "of: value") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.firstIndex(value: 1)", "of: value") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.firstIndex(of: false)", "required type") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.lastIndex()", "of: value") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.lastIndex(1)", "of: value") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.lastIndex(of: false)", "required type") == 0);
    CHECK(rejects("let a: [Rng] = []\nlet b = a.firstIndex(of: Rng(1))",
                  "does not support equality") == 0);
    CHECK(rejects("let a: [Rng] = []\nlet b = a.lastIndex(of: Rng(1))",
                  "does not support equality") == 0);
    CHECK(rejects("let a: [Rng] = []\nlet b = a.contains(Rng(1))",
                  "does not support equality") == 0);
    CHECK(accepts("let a = [[1], [2]]\nlet b: [[Int64]] = a.reversed()\n") == 0);
    CHECK(rejects("let a = [1]\nlet b = a.reversed(1)", "expects no arguments") == 0);
    CHECK(rejects("let a = 1\nlet b = a.reversed()", "requires an array value") == 0);
    CHECK(accepts("var a = [[1], [2]]\na[0].reverse()\n") == 0);
    CHECK(rejects("let a = [1]\na.reverse()", "mutable var binding") == 0);
    CHECK(rejects("var a = [1]\na.reverse(1)", "expects no arguments") == 0);
    CHECK(rejects("var a = 1\na.reverse()", "requires an array value") == 0);
    CHECK(accepts("var a = [1,3]\na.insert(2, at: 1)\n"
                  "var b = a\nb.insert(4, at: b.count)\n") == 0);
    CHECK(rejects("let a = [1]\na.insert(2, at: 0)", "mutable var") == 0);
    CHECK(rejects("var a = [1]\na.insert(2, at: true)", "required type") == 0);
    CHECK(rejects("var a = [1]\na.insert(false, at: 0)", "required type") == 0);
    CHECK(rejects("var a = [1]\na.insert(2, 0)", "value and at:") == 0);
    CHECK(rejects("var a = [1]\na.insert(at: 0, 2)", "value and at:") == 0);
    CHECK(rejects("var a = 1\na.insert(2, at: 0)", "array value") == 0);
    CHECK(accepts("let by = 2\nvar a = [1,2,3,4]\n"
                  "for i in 0..<4 by by:\n    a[0..<4 by 2] = [i, i]\n"
                  "let b = a[... by 2]\n") == 0);
    CHECK(rejects("for i in 0..<5 by true:\n    print(i)", "required type") == 0);
    CHECK(rejects("let a = [1,2,3]\nlet b = a[... by 1.5]", "required type") == 0);
    CHECK(rejects("var a = [1,2,3]\na[... by 2] += [4]", "stepped range") == 0);
    CHECK(rejects("let a = [1,2,3]\na[... by 2] = [4,5]", "mutable var") == 0);
    CHECK(accepts("let x: Int64? = nil\nif let x = x:\n    let y: Int64 = x\n"
                  "else:\n    let y: Int64? = x\n"
                  "while var a: [Float64] = Optional.some([1]):\n    a.append(2)\n    break\n"
                  "if let x: Int64? = Optional.some(nil):\n    let inner: Int64? = x\n") == 0);
    CHECK(accepts("let maybe: Int64? = nil\nlet value: Int64 = maybe ?? 3\n"
                  "let text: String = nil ?? \"fallback\"\n") == 0);
    CHECK(rejects("let value = 1 ?? 2\n", "Left operand of coalescing operator must be optional") == 0);
    CHECK(rejects("let maybe: Int64? = nil\nlet value = maybe ?? true\n",
                  "Expression type does not match required type") == 0);
    CHECK(rejects("let value = nil ?? nil\n", "optional type") == 0);
    CHECK(rejects("if let x = 1:\n    print(x)", "optional value") == 0);
    CHECK(rejects("while let x = true:\n    break", "optional value") == 0);
    CHECK(rejects("if let x = nil:\n    print(x)", "optional type") == 0);
    CHECK(rejects("if let x: Float64 = Optional.some(true):\n    print(x)", "required type") == 0);
    CHECK(rejects("if let x = Optional.some(1):\n    x = 2", "mutable var") == 0);
    CHECK(rejects("if let x = Optional.some(1):\n    let x = 2", "Duplicate declaration") == 0);
    CHECK(rejects("if let x = Optional.some(1):\n    print(x)\nelse:\n    print(x)", "Unknown name") == 0);
    CHECK(rejects("while let x = Optional.some(1):\n    break\nprint(x)", "Unknown name") == 0);
    CHECK(rejects("if let self = Optional.some(1):\n    print(1)", "self is reserved") == 0);
    CHECK(rejects("func f(x: Int64?) -> Int64:\n    if let y = x:\n        return y", "return") == 0);
    CHECK(accepts("var x: Float64? = nil\nx = Optional.some(1)\nlet f: Float64 = x.unwrap()\n"
                  "let empty: Bool = x == nil\nlet other: Bool = nil != x\n"
                  "let nested: [Int64?]? = Optional.some([nil,Optional.some(1)])\n"
                  "let double: Int64?? = Optional.some(nil)\n") == 0);
    CHECK(accepts("struct Node:\n    let next: Node?\nlet n = Node(nil)\n"
                  "func value(x: [Float64]?) -> [Float64]?:\n    return x\n"
                  "let a: [Float64]? = value(nil)\n") == 0);
    CHECK(rejects("let x: Int64? = 3", "required type") == 0);
    CHECK(rejects("let x: Int64 = nil", "optional type") == 0);
    CHECK(rejects("let x = Optional.some(nil)", "optional type") == 0);
    CHECK(rejects("let x = Optional.some()", "one value") == 0);
    CHECK(rejects("let x = Optional.some(wrong: 2)", "one value") == 0);
    CHECK(rejects("let x = Optional.some(2,3)", "one value") == 0);
    CHECK(rejects("let x: Float64? = Optional.some(true)", "required type") == 0);
    CHECK(rejects("let x = Optional.some(1).valueOr(false)", "required type") == 0);
    CHECK(rejects("let x = Optional.some(1).unwrap(0)", "Optional methods") == 0);
    CHECK(rejects("var x: Int64? = nil\nx.hasValue = true", "mutable var") == 0);
    CHECK(rejects("let x = Optional.some(1) + 2", "required type") == 0);
    CHECK(rejects("if Optional.some(true):\n    print(1)", "required type") == 0);
    CHECK(accepts("let a: Int64? = nil\nlet b = Optional.some(1)\n"
                  "let equal = a == a && b == Optional.some(1) && b != a\n"
                  "let words = Optional.some(\"same\") == Optional.some(\"same\")\n"
                  "let vectors = Optional.some(Vec2(1,2)) != Optional.some(Vec2(2,1))") == 0);
    CHECK(rejects("let x = Optional.some(1) == Optional.some(1.0)", "required type") == 0);
    CHECK(accepts("let x = Optional.some([1]) == Optional.some([1])") == 0);
    CHECK(rejects("func compare(a: Rng?, b: Rng?) -> Bool:\n    return a == b",
                  "does not support equality") == 0);
    CHECK(rejects("let x = nil == nil", "optional value") == 0);
    CHECK(rejects("let x = 1 == nil", "optional value") == 0);
    CHECK(rejects("let x = Optional.some(1)[0]", "array value") == 0);
    CHECK(rejects("let x: Void? = nil", "unsupported type") == 0);
    CHECK(rejects("struct Optional:\n    var x: Int64", "builtin type") == 0);
    CHECK(rejects("var x: Int64? = nil\nx.unwrap() = 1", "mutable var") == 0);
    CHECK(rejects("func nothing():\n    return\nlet x = Optional.some(nothing())", "Optional content") == 0);
    char optional_depth[256] = "let x: Int64";
    size_t optional_prefix = strlen(optional_depth);
    memset(optional_depth + optional_prefix, '?', 140);
    strcpy(optional_depth + optional_prefix + 140, " = nil");
    CHECK(rejects(optional_depth, "nesting limit") == 0);
    CHECK(accepts("func convert(m: Mat4, v: Vec3) -> Vec3:\n    return m.inverse(0).transformPoint(v)\n"
                  "struct Frame:\n    var basis: Mat3\n    let world: [Mat4]\n"
                  "let f = Frame(Mat3.identity(),[Mat4.identity()])\n"
                  "let v: Vec3 = f.basis.applied(Vec3(1,2,3))\n") == 0);
    CHECK(rejects("struct Mat3:\n    var x: Int64", "builtin type") == 0);
    CHECK(rejects("enum Mat4:\n    case zero", "builtin type") == 0);
    CHECK(rejects("let m: Mat3 = Mat4.identity()", "required type") == 0);
    CHECK(rejects("let m = Mat3.identity().multiplied(Mat4.identity())", "required type") == 0);
    CHECK(rejects("let m = Mat4.identity().applied(Vec3(1,2,3))", "required type") == 0);
    CHECK(rejects("let m = Mat4.identity().inverse()", "Missing library arguments") == 0);
    CHECK(rejects("let m = Mat3.identity().element(true,0)", "required type") == 0);
    CHECK(rejects("let m = Mat3(Vec3(1,0,0),Vec3(0,1,0))", "Missing library arguments") == 0);
    CHECK(rejects("let m = Mat4.identity().inverse(tolerance: 0)", "Unknown parameter name") == 0);
    CHECK(rejects("let m = inverseMat3(Mat3.identity(),0)", "Unknown") == 0);
    CHECK(rejects("let m = Mat3.identity().x", "Unknown field") == 0);
    CHECK(accepts("func f(b: Body):\n"
        "    let s: Sweep = Sweep.spherePlane(b,1,Vec3(0,-1,0),Vec3(0,0,0),Vec3(0,1,0))\n"
        "    let hit: Bool = s.hit\n    let c: Contacts = s.contacts()\n"
        "    let box: Aabb = Aabb.sphere(b,1)\n"
        "    let pairs: [CollisionPair] = Aabb.pairs([box])\n"
        "    let p: Vec3 = box.minimum + box.maximum\n") == 0);
    CHECK(rejects("var b = Aabb.sphere(Body.sphere(1,1),1)\nb.minimum.x = 2", "mutable") == 0);
    CHECK(rejects("func f(p: CollisionPair):\n    p.bodyA = 1", "mutable") == 0);
    CHECK(rejects("func f(s: Sweep):\n    s.hit = false", "mutable") == 0);
    CHECK(rejects("let pairs = Aabb.pairs([1])", "type") == 0);
    CHECK(accepts("func f(b: Body):\n"
        "    let c: ContactConstraint = Contacts.spheres(b,1,b,1).constraint(0,0,1)\n"
        "    let j: JointConstraint = DistanceJoint(Vec3(0,0,0),Vec3(1,0,0),1,0.2).constraint(0,-1)\n"
        "    let r: ConstraintResult = ContactSolver.defaults().solve([b,b],[c],[j],0.01)\n"
        "    let n: Int64 = r.bodyCount + r.jointCount + r.contactCount + c.bodyB\n"
        "    let copy: [Body] = r.bodies()\n    let p: Vec3 = r.body(0).position\n") == 0);
    CHECK(rejects("func f(c: ContactConstraint):\n    c.bodyB = 1", "mutable") == 0);
    CHECK(rejects("func f(r: ConstraintResult):\n    r.bodyCount = 1", "mutable") == 0);
    CHECK(rejects("let r = ContactSolver.defaults().solve([1],[],[],0.01)", "type") == 0);
    CHECK(rejects("func f(r: ConstraintResult):\n    let b = r.body(true)", "type") == 0);
    CHECK(accepts("func f(a: Body, b: Body):\n"
                  "    let j: DistanceJoint = DistanceJoint(Vec3(0,0,0),Vec3(0,1,0),1,0.2)\n"
                  "    let r: JointResult = j.resolve(bodyB: b, dt: 0.01, bodyA: a)\n"
                  "    let bodies: [Body] = [r.bodyA,r.bodyB]\n"
                  "    let anchors: Vec3 = j.anchorA + j.anchorB + r.impulse\n"
                  "    let errors: Float64 = r.lengthError + r.velocityError\n") == 0);
    CHECK(rejects("var j = DistanceJoint(Vec3(0,0,0),Vec3(0,1,0),1,0.2)\nj.length = 2", "mutable") == 0);
    CHECK(rejects("func f(r: JointResult):\n    r.bodyA.step(Vec3(0,0,0),Vec3(0,0,0),1)", "mutable") == 0);
    CHECK(rejects("func f(j: DistanceJoint, b: Body):\n    let r = j.resolve(b,b,true)", "type") == 0);
    CHECK(accepts("func f(a: Body, b: Body):\n"
                  "    let settings: ContactSolver = ContactSolver.defaults()\n"
                  "    let contacts: Contacts = Contacts.spheres(a,1,b,1)\n"
                  "    let result: ContactResult = contacts.resolve(a,b,settings)\n"
                  "    let bodies: [Body] = [result.bodyA, result.bodyB]\n"
                  "    let n: Int64 = contacts.count + result.count + settings.iterations\n"
                  "    let e: Float64 = result.maxNormalError\n") == 0);
    CHECK(rejects("var s = ContactSolver.defaults()\ns.friction = 1", "mutable") == 0);
    CHECK(rejects("func f(r: ContactResult):\n    r.bodyA.step(Vec3(0,0,0),Vec3(0,0,0),1)", "mutable") == 0);
    CHECK(rejects("func f(c: Contacts):\n    let p = c.point(1.0)", "type") == 0);
    CHECK(rejects("func f(c: Contacts, a: Body):\n    let r = c.resolve(a,a,1)", "type") == 0);
    CHECK(accepts("func f(b: Body):\n    var a = [b]\n"
                  "    a[0].step(dt: 0.1, force: Vec3(0,0,0), torque: Vec3(0,0,0))\n"
                  "    let p: Vec3 = a[0].position\n    let q: Quat = a[0].orientation\n"
                  "    let e: Float64 = a[0].kineticEnergy()") == 0);
    CHECK(rejects("let b = Body.sphere(1,1)\nb.step(Vec3(0,0,0),Vec3(0,0,0),1)", "mutable") == 0);
    CHECK(rejects("var b = Body.sphere(1,1)\nb.position.x = 1", "mutable") == 0);
    CHECK(rejects("var b = Body.sphere(1,1)\nb.mass = 2", "mutable") == 0);
    CHECK(rejects("var b = Body.box(1,1)", "type") == 0);
    CHECK(rejects("let b = sphereBody(1,1)", "Unknown") == 0);
    CHECK(accepts("func f(s: Series, u: Unit):\n"
                  "    let selected: [Series] = Series.select(columns: [s], selector: s, accepted: 1)\n"
                  "    let t: Table = Table(units: [u], labels: [\"Mean\"], title: \"Summary\")\n"
                  "    t.row(values: [Quantity(selected[0].mean(), u)], label: \"sample\")\n"
                  "    t.export(\"summary\")") == 0);
    CHECK(rejects("func f(s: Series):\n    let x = Series.select([1], s, 1)", "type") == 0);
    CHECK(rejects("func f(s: Series):\n    let x = Series.select([s], 1, 1)", "type") == 0);
    CHECK(rejects("func f(t: Table):\n    t.row(\"x\", [1.0])", "type") == 0);
    CHECK(rejects("let t = Table(\"t\", [1], [])", "type") == 0);
    CHECK(accepts("func f(s: Series):\n    let n = 1 + Series.select([s],s,1)[0].mean()") == 0);
    CHECK(accepts("let noise: Distribution = Distribution.normal(mean: 0, standardDeviation: 1)\n"
                  "let c: SensorConfig = SensorConfig(Unit(1,0,0,0,0,0,0,1,\"m\"), 10,0,0,0,0,noise,0,0,0)\n"
                  "var sensors: [Sensor] = [Sensor(c, -1), Sensor.forRun(c, 1)]\n"
                  "let m: Measurement = sensors[0].read(truth: Quantity(2, Unit(1,0,0,0,0,0,0,1,\"m\")), time: 0)\n"
                  "sensors[1].reset(seed: 1)\nsensors[0].resetForRun(stream: 2)\n"
                  "let state: Int64 = m.state\nlet index: Int64 = m.index\nlet skipped: Int64 = m.skipped\n"
                  "let time: Float64 = m.time\nlet u: Float64 = m.standardUncertainty\n"
                  "let q: Quantity = m.value\nlet ok: Bool = m.isValid() && m.isDue() && !m.isDropped()") == 0);
    CHECK(rejects("func f(s: Sensor, q: Quantity):\n    s.read(0, q)", "mutable") == 0);
    CHECK(rejects("func f(c: SensorConfig, q: Quantity):\n    Sensor(c, 1).read(0, q)", "mutable") == 0);
    CHECK(rejects("func f(s: Sensor):\n    s.resetForRun(0)", "mutable") == 0);
    CHECK(rejects("func f(c: SensorConfig):\n    let s = Sensor(c, 0)\n    s.reset(0)", "mutable") == 0);
    CHECK(rejects("func f(m: Measurement):\n    var sample = m\n    sample.value = m.value", "mutable") == 0);
    CHECK(rejects("func f(m: Measurement):\n    var sample = m\n    sample.index = 2", "mutable") == 0);
    CHECK(rejects("func f(c: SensorConfig):\n    var s = Sensor(c, 0)\n    s.read(0, 1)", "type") == 0);
    CHECK(rejects("let d = Distribution.normal(0, true)", "type") == 0);
    CHECK(rejects("let d = normalDistribution(0, 1)", "Unknown function") == 0);
    CHECK(accepts("polyline([], 0.01, 0, 1)\n"
                  "let p: [Vec3] = [Vec3(0, 0, 0), Vec3(1, 0, 0)]\n"
                  "polyline(id: 2, color: 0, radius: 0, points: p[0..<2])") == 0);
    CHECK(rejects("polyline([Vec2(0, 0)], 0.01, 0, 1)", "type") == 0);
    CHECK(rejects("polyline(Vec3(0, 0, 0), 0.01, 0, 1)", "type") == 0);
    CHECK(rejects("polyline([[Vec3(0, 0, 0)]], 0.01, 0, 1)", "type") == 0);
    CHECK(rejects("polyline([], 0.01, 0)", "Missing") == 0);
    CHECK(rejects("polyline(points: [], radius: 0, color: 0, color: 1)", "more than once") == 0);
    CHECK(accepts("let u = Unit(1, 0, 0, 0, 0, 0, 0, 1, \"m\")\n"
                  "let q: Quantity = Quantity(1, u)\nlet n: Float64 = q.value\n"
                  "let unit: Unit = q.unit\nlet v = q.adding(q).converted(unit)") == 0);
    CHECK(rejects("let u = Unit(1, 0, 0, 0, 0, 0, 0, 1, \"m\")\n"
                  "var q = Quantity(1, u)\nq.value = 2", "mutable") == 0);
    CHECK(rejects("let u = Unit(1, 0, 0, 0, 0, 0, 0, 1, \"m\")\n"
                  "var q = Quantity(1, u)\nq.unit = u", "mutable") == 0);
    CHECK(accepts("let u = Unit(1, 0, 0, 0, 0, 0, 0, 1, \"m\")\n"
                  "let area: Unit = u.multiplied(right: u, symbol: \"m2\")\n"
                  "let same: Bool = area.isCompatible(u.powered(2, \"m2\"))\n"
                  "let one = area.divided(area, \"1\")") == 0);
    CHECK(rejects("let u = Unit(1, 0, 0, 0, 0, 0, 0, 1, \"m\")\n"
                  "let x = u.powered(1.5, \"m\")", "type") == 0);
    CHECK(rejects("let u = Unit(1, 0, 0, 0, 0, 0, 0, 1, \"m\")\n"
                  "let x = multiplyUnit(u, u, \"m2\")", "Unknown function") == 0);
    CHECK(accepts("struct S:\n    static func make(x: Int64) -> S:\n        return S()\n"
                  "let s = S.make(x: 2)") == 0);
    CHECK(rejects("struct S:\n    static func f():\n        print(self)",
                  "only available inside") == 0);
    CHECK(rejects("struct S:\n    static func f():\n        return\nS().f()",
                  "called on its type") == 0);
    CHECK(rejects("struct S:\n    func f():\n        return\nS.f()",
                  "static method") == 0);
    CHECK(rejects("struct S:\n    static func f():\n        return\nf()",
                  "Unknown") == 0);
    CHECK(accepts("struct S:\n    var n: Int64\n    mutating func set(value: Int64):\n"
                  "        self.n = value\nvar s = S(1)\ns.set(2)") == 0);
    CHECK(rejects("struct S:\n    var n: Int64\n    mutating func set():\n"
                  "        self.n = 2\nlet s = S(1)\ns.set()",
                  "mutable var") == 0);
    CHECK(rejects("struct S:\n    var n: Int64\n    mutating func set():\n"
                  "        self.n = 2\nS(1).set()",
                  "mutable var") == 0);
    CHECK(rejects("struct S:\n    let n: Int64\n    mutating func set():\n"
                  "        self.n = 2",
                  "mutable var") == 0);
    CHECK(rejects("struct S:\n    var n: Int64\n    mutating func set():\n"
                  "        self.n = 2\n    func f():\n        self.set()",
                  "mutable var") == 0);
    CHECK(accepts("struct S:\n    let n: Int64\n    mutating func reset():\n"
                  "        self = S(0)\nvar s = S(1)\ns.reset()") == 0);
    CHECK(accepts("struct S:\n    var x: Float64\n    func twice() -> Float64:\n"
                  "        return self.get() * 2\n    func get() -> Float64:\n"
                  "        return self.x\nlet n = 1 + S(2).twice()") == 0);
    CHECK(accepts("struct S:\n    func value() -> Int64:\n        return 1\n"
                  "let n = S().value()") == 0);
    CHECK(rejects("struct S:\n    var x: Int64\n    func set():\n        self.x = 2",
                  "mutable var") == 0);
    CHECK(rejects("struct S:\n    var a: [Int64]\n    func set():\n        self.a.append(2)",
                  "mutable var") == 0);
    CHECK(rejects("struct S:\n    var x: Int64\n    func x():\n        return",
                  "conflicts with a field") == 0);
    CHECK(rejects("struct S:\n    func f():\n        return\n    func f():\n        return",
                  "Duplicate method") == 0);
    CHECK(rejects("let n = self", "only available inside") == 0);
    CHECK(rejects("struct S:\n    func f(self: Int64):\n        return", "reserved") == 0);
    CHECK(rejects("struct S:\n    func f() -> Int64:\n        let x = 1", "Not all") == 0);
    CHECK(rejects("struct S:\n    func f():\n        return\nf()", "Unknown function") == 0);
    CHECK(accepts("var a = [[1], [2]]\nlet removed: [Int64] = a.remove(at: 0)\n"
                  "a.append(removed)\nlet n = a.count") == 0);
    CHECK(rejects("let a = [1]\na.remove(at: 0)", "mutable var") == 0);
    CHECK(rejects("var a = [1]\na.count = 2", "mutable var") == 0);
    CHECK(rejects("let a = [1]\narrayAppending(a, 2)", "Unknown function") == 0);
    CHECK(rejects("dot3(Vec3(1, 2, 3), Vec3(1, 2, 3))", "Unknown function") == 0);
    CHECK(rejects("let v = Vec3(1, 2, 3)\nv.dot(left: v, right: v)", "more than once") == 0);
    CHECK(accepts("let v = Vec3(1, 2, 3)\nlet n = 1 + v.normalized().length()\n"
                  "let q = Quat.axisAngle(axis: v, angle: 1)\nlet rotated = q.rotate(v)") == 0);
    CHECK(accepts("var a: [Float64] = []\na.append(1)\n"
                  "struct S:\n    var a: [[Float64]]\nvar s = S([[]])\ns.a[0].append(2)") == 0);
    CHECK(rejects("let a = [1]\na.append(2)", "mutable var") == 0);
    CHECK(rejects("var a = [1]\na.append(true)", "required type") == 0);
    CHECK(rejects("var a = [1]\na.append()", "one positional") == 0);
    CHECK(rejects("var a = [1]\na.append(2, 3)", "one positional") == 0);
    CHECK(rejects("var a = [1]\na.append(value: 2)", "one positional") == 0);
    CHECK(rejects("var a = 1\na.append(2)", "requires an array") == 0);
    CHECK(rejects("var a = [1]\na[0..<1].append(2)", "mutable var") == 0);
    CHECK(rejects("[1].append(2)", "mutable var") == 0);
    CHECK(rejects("struct S:\n    let a: [Int64]\nvar s = S([1])\ns.a.append(2)", "mutable var") ==
          0);
    CHECK(rejects("var a = [1]\nlet b = a.append(2)", "produce a value") == 0);
    CHECK(accepts("let a = [1, 2, 3]\nlet b: [Int64] = a[1..<3]\n"
                  "let c: [Int64] = a[0...1]\nvar d = b\nd[0] = 7") == 0);
    CHECK(accepts("let a = [1.5, 2.5]\nlet b = [a[0..<1], a[1...1]]\n"
                  "let x = 1 + a[0..<2][0]") == 0);
    CHECK(accepts("let a = [[1.5, 2.5][0..<1]]") == 0);
    CHECK(rejects("let a = [1, 2][0.5..<1]", "required type") == 0);
    CHECK(rejects("let a = [1, 2][0...true]", "required type") == 0);
    CHECK(accepts("var a = [1, 2]\na[0..<1] = [3]\na[1...1] = []") == 0);
    CHECK(accepts("var a = [1, 2]\nlet b = a[..<1]\nlet c = a[1...]\n"
                  "a[..<] = b\na[...] = c\na[1..<] = []") == 0);
    CHECK(rejects("let a = [1]\na[...1] = []", "mutable var") == 0);
    CHECK(rejects("var a = [1]\na[..<true] = []", "required type") == 0);
    CHECK(rejects("var a = [1]\na[true..<] = []", "required type") == 0);
    CHECK(rejects("let a = [1, 2]\na[0..<1] = [3]", "mutable var") == 0);
    CHECK(rejects("var a = [1, 2]\na[0..<1] = [true]", "required type") == 0);
    CHECK(rejects("var a = [1, 2]\na[0..<1] = 3", "required type") == 0);
    CHECK(accepts("var a = [1, 2]\na[0..<1] += [3]") == 0);
    CHECK(rejects("struct S:\n    let a: [Int64]\nvar s = S([1])\ns.a[0..<1] = [2]", "mutable var") == 0);
    CHECK(rejects("var a = [1, 2]\na[0..<1][0] = 3", "mutable var") == 0);
    CHECK(accepts("let a = [1, 2] + [3]\nlet b = [] + a\n"
                  "let c: [Int64] = [] + []\nvar d = a\nd += b") == 0);
    CHECK(rejects("let a = [] + []", "explicit element type") == 0);
    CHECK(rejects("let a = [1] + [true]", "required type") == 0);
    CHECK(rejects("let a = [1] + 2", "required type") == 0);
    CHECK(
        accepts("for row in [[1, 2], [3]]:\n    for value in row:\n        let n: Int64 = value") ==
        0);
    CHECK(rejects("for value in [1, 2]:\n    value = 3", "mutable var") == 0);
    CHECK(rejects("for row in [[1]]:\n    row[0] = 2", "mutable var") == 0);
    CHECK(rejects("for value in [1]:\n    print(value)\nprint(value)", "Unknown name") == 0);
    CHECK(accepts("let a: [Int64] = []\nlet n: Int64 = a.count") == 0);
    CHECK(rejects("let n = (1).count", "Unknown field") == 0);
    CHECK(rejects("let n = arrayCount()", "Unknown function") == 0);
    CHECK(accepts("let a: [Float64] = []\nlet b = a + [1]\nvar c: [Float64] = b\n"
                  "c.remove(at: 0)") == 0);
    CHECK(accepts("let a = [[1]]\nlet b = a + [[]]") == 0);
    CHECK(rejects("let a = [1] + [true]", "type") == 0);
    CHECK(rejects("var a = [1]\na.remove(at: 0.5)", "required type") == 0);
    CHECK(accepts("func copy(a: [Int64]) -> [Int64]:\n    return a\n"
                  "var a: [Int64] = []\na = copy([1, 2, 3])\na[1] += 4\n"
                  "let b: [[Float64]] = [[], [1, 2.5]]\nlet x: Float64 = 1 + b[1][0]") == 0);
    CHECK(accepts("struct Particle:\n    var position: Vec3\n"
                  "struct Cloud:\n    var particles: [Particle]\n"
                  "var cloud = Cloud([Particle(Vec3(1, 2, 3))])\n"
                  "cloud.particles[0].position.x = 4\n"
                  "let v = [Vec3(1, 2, 3)][0] * 2\n"
                  "let f = 1 + [1, 2.5][0]") == 0);
    CHECK(accepts("struct Node:\n    var children: [Node]\n"
                  "let root = Node([Node([])])") == 0);
    CHECK(accepts("enum E:\n    case a\nlet values: [E] = [E.a]\n"
                  "switch values[0]:\n    case E.a:\n        print(1)") == 0);
    CHECK(rejects("let a = []", "explicit element type") == 0);
    CHECK(rejects("let a = [1, true]", "required type") == 0);
    CHECK(rejects("let a: [Float64] = [1]\nlet b: [Int64] = a", "required type") == 0);
    CHECK(rejects("let i = 1\nlet a = [i, 2.5]", "required type") == 0);
    CHECK(rejects("let a = [1]\na[0] = 2", "mutable var") == 0);
    CHECK(rejects("var a = [1]\na[0.0] = 2", "required type") == 0);
    CHECK(rejects("var a = 1\na[0] = 2", "Indexing requires") == 0);
    CHECK(rejects("struct S:\n    let a: [Int64]\nvar s = S([1])\ns.a[0] = 2", "mutable var") == 0);
    CHECK(rejects("struct S:\n    let x: Int64\nvar a = [S(1)]\na[0].x = 2", "mutable var") == 0);
    CHECK(rejects("func f() -> [Int64]:\n    return [1]\nf()[0] = 2", "mutable var") == 0);
    CHECK(rejects("let a: [Void] = []", "unsupported type") == 0);
    CHECK(rejects("let a = [1]\nlet x = a.missing", "Unknown field") == 0);
    CHECK(accepts("let q = Quat(0, 0, 0, 1)\nlet p: Quat = (q.normalized()).multiplied(right: "
                  "q.conjugated())\nlet r: Quat = q.slerp(fraction: 0.5, end: p)") == 0);
    CHECK(rejects("let q: Quat = (Vec4(0, 0, 0, 1)).normalized()", "required type") == 0);
    CHECK(rejects("let q = (1).conjugated()", "Unknown method") == 0);
    CHECK(rejects("let q = (Quat(0, 0, 0, 1)).multiplied(Vec3(0, 0, 0))", "required type") == 0);
    CHECK(rejects("let q = Quat(0, 0, 0, 1)\nlet r = q.slerp(q, true)", "required type") == 0);
    CHECK(rejects("let q = Quat(0, 0, 0, 1)\nlet r = q.slerp(end: q, time: 1)",
                  "Unknown parameter") == 0);
    CHECK(
        accepts("struct Phase:\n    var state: Vec4\nfunc f(v: Vec4) -> Vec4:\n    return "
                "v.normalized()\nvar p = Phase(Vec4(w: 4, x: 1, z: 3, y: 2))\np.state.w += "
                "2\np.state *= 3\nlet n: Float64 = (f(p.state)).dot(p.state) + p.state.length()") ==
        0);
    CHECK(rejects("struct Vec4:\n    var x: Float64", "builtin type") == 0);
    CHECK(rejects("let v = Vec4(1, 2, 3, 4)\nv.w = 0", "mutable var") == 0);
    CHECK(rejects("let v = Vec4(1, 2, 3)", "Missing library arguments") == 0);
    CHECK(rejects("let v = Vec4(1, 2, 3, true)", "required type") == 0);
    CHECK(rejects("let v = Vec4(1, 2, 3, 4) + Vec3(1, 2, 3)", "Invalid vector") == 0);
    CHECK(rejects("let v: Quat = Vec4(0, 0, 0, 1)", "required type") == 0);
    CHECK(rejects("let v = (Vec4(0, 0, 0, 1)).dot(Quat(0, 0, 0, 1))", "required type") == 0);
    CHECK(rejects("let v: Vec4 = (Vec3(1, 2, 3)).normalized()", "required type") == 0);
    CHECK(rejects("let v = Vec4(1, 2, 3, 4).q", "Unknown field") == 0);
    CHECK(accepts("var x = 1\nx += 2\nx *= 3\nx -= 1\nx /= 2\nx %= 3\n"
                  "var p = Vec3(1, 2, 3)\np += Vec3(1, 0, 0)\np *= 2\np.x /= 3") == 0);
    CHECK(rejects("let x = 1\nx += 2", "mutable var") == 0);
    CHECK(rejects("var x = 1\nx += 0.5", "required type") == 0);
    CHECK(rejects("var x = 1.0\nx %= 2", "Remainder") == 0);
    CHECK(rejects("var x = Vec3(0, 0, 0)\nx += 1", "Invalid vector") == 0);
    CHECK(rejects("struct S:\n    let x: Int64\nvar s = S(1)\ns.x += 2", "mutable var") == 0);
    CHECK(accepts("let f: Vec3 = buoyancyForce(1000, 0.01, Vec3(0, -10, 0))\n"
                  "let drag = stokesDrag(radius: 1, viscosity: 0.01, relativeVelocity: f)") == 0);
    CHECK(rejects("let f = buoyancyForce(1, 1, 9.81)", "required type") == 0);
    CHECK(rejects("let f = quadraticDrag(Vec3(1, 0, 0), 1, 1)", "Missing library arguments") == 0);
    CHECK(accepts("let a = Vec3(1, 2, 3)\nlet b: Vec3 = 2 * (a + a) / 4\n"
                  "let c = +b - -a\nassert(c == a * 2)\n"
                  "let x = 1 + (a + b).x") == 0);
    CHECK(rejects("let a = Vec2(1, 2) + Vec3(1, 2, 3)", "Invalid vector operator") == 0);
    CHECK(rejects("let a = Vec2(1, 2) * Vec2(1, 2)", "required type") == 0);
    CHECK(rejects("let a = Vec2(1, 2) + 1", "Invalid vector operator") == 0);
    CHECK(rejects("let a = 1 / Vec2(1, 2)", "Invalid vector operator") == 0);
    CHECK(rejects("let a = Vec2(1, 2) < Vec2(2, 3)", "Invalid vector operator") == 0);
    CHECK(rejects("let n = 2\nlet a = n * Vec2(1, 2)", "required type") == 0);
    CHECK(rejects("let a = (Vec2(1, 2)).dot(Vec3(2, 3, 4))", "required type") == 0);
    CHECK(
        accepts("var q: Quat = Quat(0, 0, 0, 1)\nq.w = 2\nlet p: Vec3 = q.rotate(Vec3(1, 0, 0))") ==
        0);
    CHECK(rejects("let q = Quat(0, 0, 0, 1)\nq.w = 2", "mutable var") == 0);
    CHECK(rejects("let q = Vec3(0, 0, 1).w", "Unknown field") == 0);
    CHECK(rejects("struct Quat:\n    var x: Int64", "builtin type") == 0);
    CHECK(rejects("box(Vec3(0, 0, 0), Vec3(1, 1, 1), Vec3(0, 0, 0), 0, 1)", "required type") == 0);
    CHECK(accepts("let u: Float64 = randomUniform(max: 3, min: -2)\n"
                  "let n: Float64 = randomNormal(5, 0.25)") == 0);
    CHECK(rejects("let n = randomNormal(0, true)", "required type") == 0);
    CHECK(rejects("let n = randomUniform(min: 0, upper: 1)", "Unknown parameter") == 0);
    CHECK(accepts("var rng: Rng = Rng(seed: 42)\n"
                  "let draw: Float64 = rng.sample(Distribution.uniform(0, 1))\n"
                  "rng.reseed(42)\n"
                  "let streams: [Rng] = [rng]\n"
                  "let optional: Rng? = Optional.some(streams[0])\n") == 0);
    CHECK(accepts("let unit = Unit(0,0,0,0,0,0,0,1,\"1\")\n"
                  "let x: Series = Series.fromValues([0.0,1.0],unit,\"x\")\n"
                  "let y: Series = x.alignedValues([3.0,4.0],unit,\"y\")\n") == 0);
    CHECK(rejects("let unit = Unit(0,0,0,0,0,0,0,1,\"1\")\n"
                  "let x = Series.fromValues([true],unit,\"x\")", "required type") == 0);
    CHECK(rejects("let x = Series.fromValues([1.0],true,\"x\")", "required type") == 0);
    CHECK(rejects("let rng = Rng(1)\nlet value = rng.sample(Distribution.constant(0))",
                  "mutable var") == 0);
    CHECK(rejects("let rng = Rng(1)\nrng.reseed(2)", "mutable var") == 0);
    CHECK(rejects("let rng = Rng(true)", "required type") == 0);
    CHECK(rejects("var rng = Rng(1)\nlet x = rng.sample(1)", "required type") == 0);
    CHECK(rejects("struct Rng:\n    var seed: Int64", "builtin type") == 0);
    CHECK(rejects("struct ScalarResult:\n    var x: Float64", "builtin type") == 0);
    CHECK(rejects("struct OdeResult:\n    var state: [Float64]", "builtin type") == 0);
    CHECK(rejects("func target(x: Float64) -> Float64:\n    return x\n"
                  "var result = rootBisectReported(target, -1, 1, 1e-9, 0, 10)\n"
                  "result.x = 0", "mutable var binding") == 0);
    CHECK(accepts("var rng = Rng.forRun(stream: 3)\n"
                  "rng.reseedForRun(4)\n") == 0);
    CHECK(rejects("let rng = Rng.forRun(true)", "required type") == 0);
    CHECK(accepts("let n = 3\nlet x: Float64 = Float64(n)\nlet i: Int64 = Int64(x)\n"
                  "let a = 1 + Float64(n)\nlet b = Float64(n) + 1\n"
                  "let same = Int64(9223372036854775807)") == 0);
    CHECK(accepts("func Float64(x: Bool) -> Bool:\n    return x\n"
                  "assert(Float64(true))") == 0);
    CHECK(rejects("let x = Float64(true)", "requires Int64, Float64 or String") == 0);
    CHECK(rejects("let x = String([1])", "requires Bool, Int64, Float64 or String") == 0);
    CHECK(accepts("let x = Int64(\"12\")\nlet y = Float64(\"1.25\")\n"
                  "let z = String(true)") == 0);
    CHECK(rejects("let x = Float64(Vec2(1, 2))", "requires Int64, Float64 or String") == 0);
    CHECK(rejects("enum E:\n    case a\nlet x = Int64(E.a)", "requires Int64, Float64 or String") == 0);
    CHECK(rejects("let x = Int64()", "exactly one positional") == 0);
    CHECK(rejects("let x = Float64(1, 2)", "exactly one positional") == 0);
    CHECK(rejects("let x = Float64(value: 1)", "exactly one positional") == 0);
    CHECK(rejects("let x: Int64 = Float64(1)", "required type") == 0);
    CHECK(rejects("let x = Float64(9223372036854775808)", "range") == 0);
    CHECK(rejects("enum E:\n    case a\nfunc f(e: E) -> Int64:\n    switch e:\n"
                  "        case E.a:\n            break\n            return 1",
                  "Not all function paths") == 0);
    CHECK(rejects("enum E:\n    case a\nenum F:\n    case a\nswitch E.a:\n"
                  "    case F.a:\n        break",
                  "required type") == 0);
    CHECK(accepts("enum E:\n    case a\nfunc f(e: E) -> Int64:\n    switch e:\n"
                  "        case E.a:\n            while true:\n                break\n"
                  "            switch e:\n                default:\n                    break\n"
                  "            return 1") == 0);
    CHECK(accepts("enum E:\n    case a\n    case b\n"
                  "func f(e: E) -> Int64:\n    switch e:\n"
                  "        case E.a:\n            return 1\n"
                  "        case E.b:\n            return 2\nassert(f(E.b) == 2)") == 0);
    CHECK(accepts("enum E:\n    case a\n    case b\n"
                  "func f(e: E) -> Int64:\n    switch e:\n"
                  "        case E.a:\n            let x = 1\n            return x\n"
                  "        default:\n            let x = 2\n            return x") == 0);
    CHECK(rejects("enum E:\n    case a\n    case b\nswitch E.a:\n"
                  "    case E.a:\n        print(1)",
                  "cover every") == 0);
    CHECK(rejects("enum E:\n    case a\nswitch E.a:\n"
                  "    case E.a:\n        print(1)\n    case E.a:\n        print(2)",
                  "Duplicate switch") == 0);
    CHECK(rejects("enum E:\n    case a\nlet e = E.a\nswitch e:\n"
                  "    case e:\n        print(1)",
                  "must name an enum case") == 0);
    CHECK(accepts("switch 1:\n    default:\n        print(1)") == 0);
    CHECK(rejects("switch \"a\\nb\":\n"
                  "    case \"\"\"a\r\nb\"\"\":\n        print(1)\n"
                  "    case \"a\\nb\":\n        print(2)\n"
                  "    default:\n        print(0)\n",
                  "Duplicate switch case") == 0);
    CHECK(accepts("switch \"a\\rb\":\n"
                  "    case \"\"\"a\\rb\"\"\":\n        print(1)\n"
                  "    case \"\"\"a\r\nb\"\"\":\n        print(2)\n"
                  "    default:\n        print(0)\n") == 0);
    CHECK(rejects("enum E:\n    case a\nswitch E.a:\n"
                  "    case Vec2(0, 0).x:\n        print(1)",
                  "required type") == 0);
    CHECK(rejects("enum E:\n    case a\nswitch E.a:\n"
                  "    case E.a:\n        continue",
                  "enclosing loop") == 0);
    CHECK(rejects("enum E:\n    case a\nswitch E.a:\n"
                  "    case E.a:\n        let local = 1\nprint(local)",
                  "Unknown") == 0);
    CHECK(accepts("") == 0);
    CHECK(accepts("func gr\xc3\xb6\xc3\x9f" "e(wert: Int64) -> Int64:\n"
                  "    return wert + 1\n"
                  "let \xcf\x80 = gr\xc3\xb6\xc3\x9f" "e(wert: 2)\n"
                  "let e\xcc\x81 = 3\nlet \xc3\xa9 = 4\n"
                  "assert(\xcf\x80 == e\xcc\x81 && \xc3\xa9 == 4)") == 0);
    CHECK(accepts("enum Mode:\n    case idle\n    case active\n"
                  "    func isActive() -> Bool:\n        return self == Mode.active\n"
                  "    mutating func reset():\n        self = Mode.idle\n"
                  "    static func running() -> Mode:\n        return Mode.active\n"
                  "var mode = Mode.running()\nassert(mode.isActive())\n"
                  "mode.reset()\nassert(!mode.isActive())") == 0);
    CHECK(accepts("enum Mode:\n    case idle\n    case active\n"
                  "struct State:\n    var mode: Mode\n"
                  "func next(value: Mode) -> Mode:\n    if value == Mode.idle:\n"
                  "        return Mode.active\n    return Mode.idle\n"
                  "var state = State(Mode.idle)\nstate.mode = next(state.mode)\n"
                  "assert(state.mode != Mode.idle)") == 0);
    CHECK(
        accepts("func analyze():\n    let d: Dataset = Dataset(0)\n    let s: Series = "
                "d.series(name: \"time\")\n    report(\"Reference\")\n    let p: Plot = s.plot(s, "
                "\"x\", \"y\")\n    let n: Int64 = s.count()\n    let value = 1 + s.mean()") == 0);
    CHECK(accepts("var v: Vec3 = Vec3(z: 3, y: 2, x: 1)\nv.x = 4\nlet a: Vec2 = (Vec2(1, "
                  "2)).symplectic(-3, 0.1)\nlet x = 1 + sin(0) + a.x\nlet u: Unit = Unit(1, 0, 0, "
                  "0, 0, 0, 0, 1, \"m\")\nlet c: Channel = Channel(description: \"x\", unit: u, "
                  "name: \"x\")\nc.sample(1)") == 0);
    CHECK(accepts("struct S:\n    var x: Float64\nlet s = S(x: 1)\nlet y = s.x\nassert(1 + s.x == "
                  "2.0)") == 0);
    size_t record = nodes[parsed.root].a, member_value = nodes[nodes[nodes[record].next].next].b;
    CHECK(info[record].type == (ps_lang_type)(PS_TYPE_RECORD_BASE + record));
    CHECK(info[member_value].binding == nodes[record].a &&
          info[member_value].type == PS_TYPE_FLOAT64);
    CHECK(accepts("struct A:\n    var b: B\nstruct B:\n    let x: Int64\n"
                  "func f(a: A) -> B:\n    return a.b\nlet result = f(A(B(3)))") == 0);
    CHECK(accepts("struct S:\n    let tag: Int64\n    var value: Float64\n"
                  "var s = S(1, 2)\ns.value = 3\ns = S(4, 5)") == 0);
    CHECK(accepts("assert(true)\nprint(1)\nprint(0.5)\nprint(false)\nprint(\"ok\")") == 0);
    CHECK(accepts("let a = 1\nvar b: Int64 = a * 2\nb = b + 1\n"
                  "let f: Float64 = 2\nlet ok: Bool = f >= 1.5 && !false\n"
                  "let name: String = \"Physim\"\nlet same = name == \"Physim\"") == 0);
    CHECK(accepts("func energy(mass: Float64, speed: Float64) -> Float64:\n"
                  "    return 0.5 * mass * speed * speed\n"
                  "let value = energy(speed: 3, mass: 2.0)") == 0);
    size_t fn = nodes[parsed.root].a;
    size_t variable = nodes[fn].next;
    size_t call = nodes[variable].b;
    size_t arg = nodes[call].b;
    CHECK(info[variable].type == PS_TYPE_FLOAT64 && info[call].binding == fn);
    CHECK(info[arg].binding == nodes[nodes[fn].a].next);
    CHECK(info[nodes[arg].next].binding == nodes[fn].a);
    CHECK(info[nodes[arg].a].type == PS_TYPE_FLOAT64);
    CHECK(accepts("let a = 1 + 2.5\nlet b = 2.5 + 1\n"
                  "let c = (1 + 2) * 0.5\nlet d = 1 < 1.5\nlet e = 1.5 > 1") == 0);
    CHECK(info[nodes[nodes[parsed.root].a].b].type == PS_TYPE_FLOAT64);
    CHECK(accepts("func number() -> Float64:\n    return 2\n"
                  "let x = 1 + number()\nlet y = 1 + x") == 0);

    CHECK(accepts("func odd(n: Int64) -> Bool:\n"
                  "    if n == 0:\n        return false\n    else:\n        return even(n - 1)\n"
                  "func even(n: Int64) -> Bool:\n"
                  "    if n == 0:\n        return true\n    return odd(n - 1)\n"
                  "let result = even(8)") == 0);
    CHECK(accepts("func test(limit: Int64) -> Int64:\n"
                  "    var result = 0\n    for i in 0..<limit:\n"
                  "        if i == 2:\n            continue\n        result = result + i\n"
                  "    while result > 0:\n        result = result - 1\n"
                  "        if result == 3:\n            break\n    return result") == 0);
    CHECK(accepts("func done() -> Void:\n    return\nfunc again():\n    done()\nagain()") == 0);
    CHECK(accepts("let lo = -9223372036854775808\nlet hi = 9223372036854775807") == 0);
    CHECK(accepts("let lo = -0x8000000000000000\nlet hi = 0X7FFFFFFFFFFFFFFF\n"
                  "let f: Float64 = 0x10\nlet negative: Float64 = -0x8000000000000000") == 0);
    CHECK(accepts("let lo = -0b1000000000000000000000000000000000000000000000000000000000000000\n"
                  "let hi = 0o777777777777777777777\n"
                  "let binary: Float64 = 0B1010\nlet octal: Float64 = 0O17") == 0);
    CHECK(accepts("let lo = -9_223_372_036_854_775_808\n"
                  "let hi = 9_223_372_036_854_775_807\n"
                  "let bits = 0b1010_0101\nlet mode = 0o7_55\n"
                  "let fraction = 1_2.3_4e+2") == 0);
    CHECK(accepts("let a = 1 & 2\nlet b = 1 | 2\nlet c = 1 ^ 2\n"
                  "let d = ~a\nlet e = b << 2\nlet f = c >> 1") == 0);
    CHECK(accepts("var a = 1\na &= 3\na |= 4\na ^= 2\na <<= 1\na >>= 2\n"
                  "var values = [1]\nvalues[0] <<= 3\n"
                  "struct Holder:\n    var bits: Int64\nvar h = Holder(1)\nh.bits |= 2") == 0);
    CHECK(accepts("let x = 1\nif true:\n    let x = false\n    let y = x\nlet z = x") == 0);
    size_t x = nodes[parsed.root].a, conditional = nodes[x].next;
    size_t inner_x = nodes[nodes[conditional].b].a;
    size_t inner_y = nodes[inner_x].next, z = nodes[conditional].next;
    CHECK(info[nodes[inner_y].b].binding == inner_x && info[inner_y].type == PS_TYPE_BOOL);
    CHECK(info[nodes[z].b].binding == x && info[z].type == PS_TYPE_INT64);

    const struct {
        const char *source, *message;
    } bad[] = {
        {"enum E:\n    case a\n    case a", "Duplicate enum case"},
        {"enum E:\n    func f():\n        return", "at least one case"},
        {"enum E:\n    case a\n    func a():\n        return", "conflicts with an enum case"},
        {"enum E:\n    case a\n    func f():\n        return\n"
         "    func f():\n        return", "Duplicate method signature"},
        {"enum E:\n    case a\n    func f():\n        return\nE.f()",
         "Instance method must be called"},
        {"enum E:\n    case a\n    static func f() -> E:\n        return E.a\nE.a.f()",
         "Static method must be called"},
        {"enum E:\n    case a\n    mutating func reset():\n        self = E.a\n"
         "let value = E.a\nvalue.reset()", "mutable var"},
        {"enum E:\n    case a\n    static func init() -> E:\n        return E.a",
         "cannot be named init"},
        {"enum E:\n    case a = 1\n    static func fromRawValue() -> E:\n"
         "        return E.a", "conflicts with an enum raw-value member"},
        {"enum E:\n    case a\nlet x = E.missing", "Unknown field"},
        {"enum E:\n    case a\nlet x = E", "not used as values"},
        {"enum E:\n    case a\nlet x = E.a\nlet y = x.a", "type name"},
        {"enum E:\n    case a\nlet x: Int64 = E.a", "required type"},
        {"enum E:\n    case a\nenum F:\n    case a\nassert(E.a == F.a)", "required type"},
        {"enum E:\n    case a\nlet x = E.a + E.a", "requires numbers"},
        {"func f():\n    enum E:\n        case a", "module scope"},
        {"let d: Series = Dataset(0)", "required type"},
        {"let d = Dataset(true)", "required type"},
        {"let s = (0).series(\"time\")", "Unknown method"},
        {"struct Dataset:\n    var x: Int64", "builtin type"},
        {"let x = sin(true)", "required type"},
        {"let x = sin()", "Missing library arguments"},
        {"let x = Vec3(1, 2)", "Missing library arguments"},
        {"let x = Vec2(x: 1, x: 2)", "supplied more than once"},
        {"let x = Vec2(1, y: 2)", "not both"},
        {"let x = Vec2(z: 1, y: 2)", "Unknown"},
        {"let x = Vec2(1, 2).z", "Unknown"},
        {"let v = Vec3(1, 2, 3)\nv.x = 4", "mutable var"},
        {"print(Vec2(1, 2))", "scalar value"},
        {"(0).sample(1)", "Unknown method"},
        {"struct Vec2:\n    var x: Int64", "builtin type"},
        {"let x = missing", "Unknown name"},
        {"struct S:\n    var x: S", "Recursive struct"},
        {"struct A:\n    var b: B\nstruct B:\n    var a: A", "Recursive struct"},
        {"struct S:\n    var x: Int64\n    var x: Float64", "Duplicate struct field"},
        {"struct S:\n    var x: Void", "unsupported type"},
        {"struct S:\n    var x: Missing", "unsupported type"},
        {"struct Int64:\n    var x: Int64", "builtin type"},
        {"struct S:\n    var x: Int64\nstruct S:\n    var y: Int64", "Duplicate declaration"},
        {"struct S:\n    var x: Int64\nfunc S():\n    return", "Duplicate declaration"},
        {"struct S:\n    var x: Int64\nlet S = 1", "Duplicate declaration"},
        {"func f():\n    struct S:\n        var x: Int64", "module scope"},
        {"struct S:\n    var x: Int64\nlet value = S", "Types must be constructed"},
        {"struct S:\n    var x: Int64\nlet s = S()", "Missing struct fields"},
        {"struct S:\n    var x: Int64\nlet s = S(y: 1)", "Unknown parameter"},
        {"struct S:\n    var x: Int64\nlet s = S(false)", "required type"},
        {"struct S:\n    var x: Int64\nlet s = S(1, 2)", "too many arguments"},
        {"struct S:\n    var x: Int64\nlet s = S(x: 1, x: 2)", "more than once"},
        {"struct S:\n    var x: Int64\nlet s = S(1)\ns.x = 2", "mutable var"},
        {"struct S:\n    let x: Int64\nvar s = S(1)\ns.x = 2", "mutable var"},
        {"struct S:\n    var x: Int64\nS(1).x = 2", "mutable var"},
        {"struct S:\n    var x: Int64\nfunc f(s: S):\n    s.x = 2", "mutable var"},
        {"struct S:\n    var x: Int64\nvar s = S(1)\ns.x = false", "required type"},
        {"struct S:\n    var x: Int64\nlet y = S(1).missing", "Unknown field"},
        {"let x = 1\nlet y = x.field", "not a struct"},
        {"struct S:\n    var x: Int64\nprint(S(1))", "scalar value"},
        {"struct A:\n    var x: Int64\nstruct B:\n    var x: Int64\nlet a: A = B(1)",
         "required type"},
        {"struct A:\n    let b: B\nstruct B:\n    var x: Int64\nvar a = A(B(1))\na.b.x = 2",
         "mutable var"},
        {"assert(1)", "required type"},
        {"assert()", "assert expects a Bool"},
        {"assert(true, 1)", "required type"},
        {"assert(true, \"why\", \"extra\")", "assert expects a Bool"},
        {"assert(true, reason: \"why\")", "assert expects a Bool"},
        {"print(1, 2)", "exactly one positional"},
        {"print(value: 1)", "exactly one positional"},
        {"let x = print(1)", "produce a value"},
        {"var print = 1\nprint(2)", "not callable"},
        {"let x = x", "Unknown name"},
        {"let x = y\nlet y = 1", "Unknown name"},
        {"let x = 1\nlet x = 2", "Duplicate declaration"},
        {"let x = 1\nx = 2", "mutable var"},
        {"var x = 1\nx = false", "required type"},
        {"1 = 2", "mutable var"},
        {"var x = 1\nx + 1 = 2", "mutable var"},
        {"let x: Wrong = 1", "unsupported type"},
        {"let x: Void = 1", "unsupported type"},
        {"let x: Int64 = 1.0", "required type"},
        {"let i = 1\nlet x = i + 1.0", "required type"},
        {"let i = 1\nlet x = 1.0 + i", "required type"},
        {"let x = true + false", "numbers"},
        {"let x = 1 && 2", "required type"},
        {"let x = !1", "required type"},
        {"let x = -true", "unary operand"},
        {"let x = 1.5 % 1.0", "Remainder"},
        {"let x = 1 < 2 < 3", "required type"},
        {"if 1:\n    let x = 1", "required type"},
        {"while 1:\n    break", "required type"},
        {"for i in 4:\n    break", "required type"},
        {"for i in 0...2:\n    i = 1", "mutable var"},
        {"for i in 0...2:\n    let i = 1", "Duplicate declaration"},
        {"for i in 0...2:\n    break\nlet x = i", "Unknown name"},
        {"if true:\n    let x = 1\nlet y = x", "Unknown name"},
        {"break", "enclosing loop"},
        {"continue", "enclosing loop"},
        {"return 1", "inside a function"},
        {"func f() -> Int64:\n    let x = 1", "Not all function paths"},
        {"func f() -> Int64:\n    if true:\n        return 1", "Not all function paths"},
        {"func f() -> Int64:\n    while true:\n        return 1", "Not all function paths"},
        {"func f() -> Int64:\n    return", "Expected a return value"},
        {"func f():\n    return 1", "required type"},
        {"func f():\n    return\nfunc g():\n    return f()", "without a value"},
        {"func f(x: Bool) -> Int64:\n    return x", "required type"},
        {"func f(x: Int64):\n    x = 1", "mutable var"},
        {"func f(x: Int64):\n    let x = 1", "Duplicate declaration"},
        {"func f(x: Int64, x: Bool):\n    return", "Duplicate parameter"},
        {"func f():\n    return\nfunc f():\n    return", "Duplicate free function signature"},
        {"func f():\n    return\nlet f = 1", "Duplicate declaration"},
        {"func f():\n    return\nlet x = f()", "produce a value"},
        {"func f():\n    return\nlet x: func() -> Int64 = f", "required type"},
        {"unknown()", "Unknown function"},
        {"let x = 1\nx()", "not callable"},
        {"func f(x: Int64):\n    return\nf()", "Missing function arguments"},
        {"func f(x: Int64):\n    return\nf(1, 2)", "too many arguments"},
        {"func f(x: Int64):\n    return\nf(y: 1)", "Unknown parameter"},
        {"func f(x: Int64):\n    return\nf(x: 1, x: 2)", "more than once"},
        {"func f(x: Int64, y: Int64):\n    return\nf(1, y: 2)", "not both"},
        {"func f(x: Int64):\n    return\nf(false)", "required type"},
        {"let x = 9223372036854775808", "outside Int64"},
        {"let x = 0x8000000000000000", "outside Int64"},
        {"let x: Float64 = 0x8000000000000000", "outside Int64"},
        {"let x = -0x8000000000000001", "outside Int64"},
        {"let x = 0xFFFFFFFFFFFFFFFF", "outside Int64"},
        {"let x = 0b1000000000000000000000000000000000000000000000000000000000000000", "outside Int64"},
        {"let x: Float64 = 0o1000000000000000000000", "outside Int64"},
        {"let x = 9_223_372_036_854_775_808", "outside Int64"},
        {"let x = 0x8000_0000_0000_0000", "outside Int64"},
        {"let x = 1.0 & 2", "required type"},
        {"let x = 1 | false", "required type"},
        {"let x = ~true", "required type"},
        {"let x = 1 << 1.0", "required type"},
        {"let x = 1\nx &= 2", "mutable var"},
        {"var x = 1.0\nx >>= 2", "required type"},
        {"var values = [1]\nvalues[0] ^= false", "required type"},
        {"let x = -9223372036854775809", "outside Int64"},
        {"let x = 9999999999999999999999999999999999999999999999", "outside Int64"},
        {"let x: [Int64] = [true]", "required type"},
        {"let x = nil", "optional type"},
        {"let x = 0..<3", "produce a value"}};
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++)
        CHECK(rejects(bad[i].source, bad[i].message) == 0);
    ps_lang_check_result r = check("let x = 1\nx = 2");
    CHECK(!r.ok && r.diagnostic.line == 2 && r.diagnostic.column == 1);

    const char *small = "let x = 1";
    CHECK(check(small).ok);
    size_t count = parsed.count;
    info[count - 1].binding = 123456;
    r = ps_lang_check(small, strlen(small), nodes, parsed, info, 4096, count - 1);
    CHECK(!r.ok && info[count - 1].binding == 123456);
    CHECK(!ps_lang_check(NULL, 1, nodes, parsed, info, 4096, 4096).ok);
    CHECK(!ps_lang_check(small, strlen(small), NULL, parsed, info, 4096, 4096).ok);
    CHECK(!ps_lang_check(small, strlen(small), nodes, parsed, NULL, 4096, 4096).ok);
    nodes[parsed.root].a = parsed.root;
    CHECK(!ps_lang_check(small, strlen(small), nodes, parsed, info, 4096, 4096).ok);
    CHECK(!check("let = 1").ok); /* Preserve lexical/parser diagnostics. */
    char deep[2048] = "let x = 1";
    for (unsigned i = 0; i < 300; i++)
        strcat(deep, "+1");
    CHECK(rejects(deep, "nesting limit") == 0);
    static char wide[65536];
    size_t used = 0;
    for (unsigned i = 0; i < 2000; i++) {
        int written = snprintf(wide + used, sizeof(wide) - used, "let name%04u = 1\n", i);
        CHECK(written > 0 && (size_t)written < sizeof(wide) - used);
        used += (size_t)written;
    }
    CHECK(rejects(wide, "work budget") == 0);
    used = 0;
    for (unsigned i = 0; i < 20; i++) {
        int written =
            i == 0 ? snprintf(wide + used, sizeof(wide) - used, "struct R0:\n    var x: Int64\n")
                   : snprintf(wide + used, sizeof(wide) - used,
                              "struct R%u:\n    var a: R%u\n    var b: R%u\n", i, i - 1, i - 1);
        CHECK(written > 0 && (size_t)written < sizeof(wide) - used);
        used += (size_t)written;
    }
    CHECK(rejects(wide, "layout budget") == 0);
    used = 0;
    for (unsigned i = 0; i < 140; i++) {
        int written =
            snprintf(wide + used, sizeof(wide) - used, "struct R%u:\n    var x: R%u\n", i, i + 1);
        CHECK(written > 0 && (size_t)written < sizeof(wide) - used);
        used += (size_t)written;
    }
    snprintf(wide + used, sizeof(wide) - used, "struct R140:\n    var x: Int64\n");
    CHECK(rejects(wide, "nesting limit") == 0);
    CHECK(accepts("func f(x: Int64) -> Int64:\n    return x + 1\n"
                  "let value: func(Int64) -> Int64 = f\n"
                  "let answer = value(2)\n") == 0);
    CHECK(accepts("func make(base: Int64) -> func(Int64) -> Int64:\n"
                  "    let result = func(x: Int64) -> Int64:\n"
                  "        return base + x\n"
                  "    return result\n"
                  "let answer = make(2)(3)\n") == 0);
    CHECK(rejects("func make() -> func() -> Int64:\n"
                  "    var count = 1\n"
                  "    let result = func() -> Int64:\n"
                  "        count += 1\n"
                  "        return count\n"
                  "    return result\n",
                  "mutable var binding") == 0);
    CHECK(rejects("let bad = func(x: Int64) -> Int64:\n"
                  "    if x > 0:\n"
                  "        return x\n",
                  "Not all anonymous function paths return") == 0);
    CHECK(accepts("func outer(base: Int64) -> func(Int64) -> Int64:\n"
                  "    func add(value: Int64) -> Int64:\n"
                  "        return base + value\n"
                  "    let answer = add(value: 2)\n"
                  "    return add\n"
                  "let result = outer(3)(4)\n") == 0);
    CHECK(rejects("func outer():\n"
                  "    func add(value: Int64) -> Int64:\n"
                  "        return value\n"
                  "    func add(value: Int64) -> Int64:\n"
                  "        return value\n",
                  "Duplicate declaration") == 0);
    CHECK(rejects("func outer():\n"
                  "    let value = add(1)\n"
                  "    func add(x: Int64) -> Int64:\n"
                  "        return x\n",
                  "Unknown function") == 0);
    CHECK(rejects("func outer():\n"
                  "    func add(x: Int64) -> Int64:\n"
                  "        if x > 0:\n"
                  "            return x\n",
                  "Not all local function paths return") == 0);
    CHECK(accepts("func outer(base: String) -> func(String) -> String:\n"
                  "    func join<T>(value: T) -> String:\n"
                  "        return base + String(value)\n"
                  "    return join<String>\n"
                  "let result = outer(\"x\")(\"y\")\n") == 0);
    CHECK(rejects("func outer() -> Int64:\n"
                  "    func read<T>(value: T) -> Int64:\n"
                  "        return later\n"
                  "    let later = 3\n"
                  "    return read<Int64>(value: 1)\n",
                  "Unknown name") == 0);
    CHECK(rejects("func outer() -> Int64:\n"
                  "    var offset = 1\n"
                  "    func change<T>(value: T) -> Int64:\n"
                  "        offset += 1\n"
                  "        return offset\n"
                  "    return change<Int64>(value: 2)\n",
                  "mutable var binding") == 0);
    CHECK(rejects("func outer() -> String:\n"
                  "    func numeric<T: Numeric>(value: T) -> String:\n"
                  "        return String(value)\n"
                  "    return numeric<String>(value: \"x\")\n",
                  "Numeric constraint") == 0);
    CHECK(rejects("func f(x: Int64) -> Int64:\n    return x\n"
                  "let value = f\nlet answer = value(x: 1)",
                  "positional arguments") == 0);
    CHECK(rejects("func f(x: Int64) -> Int64:\n    return x\n"
                  "let value = f\nlet answer = value(true)",
                  "required type") == 0);
    CHECK(accepts("struct S:\n    let value: Int64\n"
                  "    static func twice(x: Int64) -> Int64:\n        return x * 2\n"
                  "let f: func(Int64) -> Int64 = S.twice\nlet x = f(3)\n") == 0);
    CHECK(rejects("struct S:\n    let value: Int64\n"
                  "    func read() -> Int64:\n        return self.value\n"
                  "let f = S.read\n", "bound receiver") == 0);
    CHECK(accepts("struct S:\n    let value: Int64\n"
                  "    func read() -> Int64:\n        return self.value\n"
                  "let s = S(3)\nlet f: func() -> Int64 = s.read\n"
                  "let value = f()\n") == 0);
    CHECK(accepts("struct S:\n    var value: Int64\n"
                  "    mutating func increment() -> Int64:\n"
                  "        self.value += 1\n        return self.value\n"
                  "let s = S(0)\nlet f: func() -> Int64 = s.increment\n"
                  "let result = f()\n") == 0);
    CHECK(accepts("struct S:\n    var value: Int64\n"
                  "    mutating func echo<T>(value: T) -> T:\n"
                  "        self.value += 1\n        return value\n"
                  "let f: func(String) -> String = S(0).echo<String>\n"
                  "let result = f(\"value\")\n") == 0);
    CHECK(accepts("struct S:\n    let value: Int64\n"
                   "    static func init(x: Int64) -> S:\n        return S(x)\n"
                   "let f = S.init\nlet value = f(3)\n") == 0);
    CHECK(rejects("struct S:\n    let value: Int64\n"
                  "    static func identity<T>(x: T) -> T:\n        return x\n"
                  "let f = S.identity\n", "Generic method values require specialization") == 0);
    CHECK(accepts("func identity<T>(value: T) -> T:\n    return value\n"
                  "let f: func(Int64) -> Int64 = identity<Int64>\n"
                  "let result = f(2)\n") == 0);
    CHECK(rejects("func identity<T>(value: T) -> T:\n    return value\n"
                  "let f = identity<Int64, Bool>\n", "Too many explicit type arguments") == 0);
    CHECK(rejects("func identity<T>(value: T) -> T:\n    return value\n"
                  "let f = identity<Unknown>\n", "Unknown or unsupported type") == 0);
    CHECK(rejects("func identity<T: Numeric>(value: T) -> T:\n    return value\n"
                  "let f = identity<String>\n", "Numeric constraint") == 0);
    CHECK(accepts("let a = 1\nlet b = 2\nlet c = a < b\n") == 0);
    CHECK(rejects("func wrong(x: Int64) -> Int64:\n    return x\n"
                  "let callback = wrong\n"
                  "let root = rootBisect(callback, 1, 2, 1e-9, 0, 100)\n",
                  "Scalar callback requires func(Float64) -> Float64") == 0);

    unsigned char function_source[] =
        "func f(x: Int64) -> Int64:\n    return x + 1\nvar y = f(x: 3)\ny += 2";
    unsigned char record_source[] = "struct S:\n    var x: Float64\n"
                                    "    static func make() -> S:\n        return S(1)\n"
                                    "    func value() -> Float64:\n        return self.x\n"
                                    "    mutating func add():\n        self.x += 1\n"
                                    "var s = S.make()\ns.x = s.value() + 1\ns.add()";
    unsigned char sdk_source[] = "var v = Vec3(z: 3, x: 1, y: 2)\nv = (v + v) / 2\nvar q: Quat = "
                                 "Quat(0, 0, 0, 1)\nq.w = 2\nv = q.rotate(v)";
    unsigned char enum_source[] = "enum E:\n    case a\n    case b\nvar e: E = E.a\n"
                                  "switch e:\n    case E.a:\n        e = E.b\n"
                                  "    default:\n        break";
    unsigned char conversion_source[] = "let n = 7\nlet x = Float64(n)\nlet i = Int64(x)\n"
                                        "let parsed = Int64.parse(\"42\")\n"
                                        "let recovered = attempt(Int64(\"bad\"))\n";
    unsigned char array_source[] =
        "var a: [[Float64]] = [[1, 2], []]\na[0][1] += 2\nlet x = a[0][0]\na += [[]]\n"
        "a.remove(at: 0)\nlet s = a[0..<1]\na[0].append(3)\nlet maybe = attempt(a[0])";
    unsigned char polyline_source[] = "polyline([], 0, 0, 1)\n"
                                      "polyline([Vec3(0, 0, 0), Vec3(1, 0, 0)], 0, 0, 2)";
    unsigned char sensor_source[] = "func f(s: Sensor, q: Quantity):\n"
                                    "    var sensors = [s]\n"
                                    "    let m = sensors[0].read(0, q)\n"
                                    "    let v = m.value.value\n    sensors[0].reset(1)";
    unsigned char selection_source[] = "func f(s: Series):\n"
        "    let a = Series.select([s],s,1)\n    let n = 1 + Series.select([],s,0).count\n"
        "    let t = Table(\"T\",[],[])\n    t.row(\"x\",[])";
    unsigned char body_source[] = "var b: Body = Body.sphere(1,1)\n"
        "var a = [b]\na[0].applyImpulse(a[0].velocity, a[0].position)\n"
        "let q = a[0].orientation\nlet m = b.mass";
    unsigned char contact_source[] = "func f(a: Body):\n"
        "    let c = Contacts.spheres(a,1,a,1)\n"
        "    let r = c.resolve(a,a,ContactSolver.defaults())\n"
        "    let v = r.bodyA.velocity\n    let p = r.impulse(c.count)";
    unsigned char joint_source[] = "func f(a: Body):\n"
        "    let j: DistanceJoint = DistanceJoint(a.position,Vec3(1,0,0),1,0.2)\n"
        "    let r: JointResult = j.resolve(a,a,0.01)\n"
        "    let p = r.impulse + j.anchorB\n    let v = r.bodyA.velocity";
    unsigned char graph_source[] = "func f(b: Body):\n"
        "    let j = DistanceJoint(b.position,Vec3(1,0,0),1,0.2).constraint(0,-1)\n"
        "    let r: ConstraintResult = ContactSolver.defaults().solve([b],[],[j],0.01)\n"
        "    var copies = [r]\n    let bs = copies[0].bodies()\n";
    unsigned char sweep_source[] = "func f(b: Body):\n"
        "    let s: Sweep = Sweep.spheres(b,1,Vec3(2,0,0),b,1,Vec3(0,0,0))\n"
        "    let bounds: [Aabb] = [Aabb.sweptSphere(b,1,Vec3(2,0,0))]\n"
        "    let pairs = Aabb.pairs(bounds)\n    let hit = s.hit\n";
    unsigned char optional_source[] = "struct N:\n    var value: [Float64]?\n"
        "var a: N? = Optional.some(N(nil))\n"
        "let b: Float64?? = Optional.some(nil)\n"
        "if a != nil:\n    let x = a.unwrap().value.valueOr([1.0])\n";
    unsigned char binding_source[] = "var a: [Float64]? = nil\n"
        "if var b: [Float64] = a:\n    b.append(2)\nelse:\n    a = Optional.some([])\n"
        "while let b = a:\n    a = nil\n    if b.count == 0:\n        continue\n    break\n";
    unsigned char *corpora[] = {function_source, record_source,     sdk_source,
                                enum_source,     conversion_source, array_source, polyline_source,
                                sensor_source, selection_source, body_source, contact_source, joint_source, graph_source, sweep_source, optional_source, binding_source};
    size_t sizes[] = {sizeof(function_source) - 1,   sizeof(record_source) - 1,
                      sizeof(sdk_source) - 1,        sizeof(enum_source) - 1,
                      sizeof(conversion_source) - 1, sizeof(array_source) - 1,
                      sizeof(polyline_source) - 1, sizeof(sensor_source) - 1,
                      sizeof(selection_source) - 1, sizeof(body_source) - 1, sizeof(contact_source) - 1,
                      sizeof(joint_source) - 1, sizeof(graph_source) - 1, sizeof(sweep_source) - 1, sizeof(optional_source) - 1, sizeof(binding_source) - 1};
    for (size_t corpus = 0; corpus < sizeof(corpora) / sizeof(corpora[0]); corpus++) {
        unsigned char *source = corpora[corpus];
        size_t size = sizes[corpus];
        for (size_t n = 0; n <= size; n++) {
            parsed = ps_lang_parse(source, n, nodes, 4096);
            r = ps_lang_check(source, n, nodes, parsed, info, 4096, 4096);
            if (!r.ok)
                CHECK(r.diagnostic.kind == PS_LANG_ERROR && r.diagnostic.offset <= n &&
                      r.diagnostic.length <= n - r.diagnostic.offset);
        }
        for (size_t i = 0; i < size; i++) {
            unsigned char saved = source[i];
            for (unsigned b = 0; b < 256; b++) {
                source[i] = (unsigned char)b;
                parsed = ps_lang_parse(source, size, nodes, 4096);
                r = ps_lang_check(source, size, nodes, parsed, info, 4096, 4096);
                if (!r.ok) {
                    CHECK(r.diagnostic.kind == PS_LANG_ERROR && r.diagnostic.error);
                    CHECK(r.diagnostic.offset <= size &&
                          r.diagnostic.length <= size - r.diagnostic.offset);
                } else {
                    for (size_t node = 1; node < parsed.count; node++) {
                        CHECK(info[node].binding < parsed.count ||
                              info[node].binding == PS_LANG_BUILTIN_PRINT ||
                              info[node].binding == PS_LANG_BUILTIN_ASSERT ||
                              info[node].binding == PS_LANG_BUILTIN_INT64 ||
                              info[node].binding == PS_LANG_BUILTIN_FLOAT64 ||
                              info[node].binding == PS_LANG_BUILTIN_INT64_PARSE ||
                              info[node].binding == PS_LANG_BUILTIN_FLOAT64_PARSE ||
                              info[node].binding == PS_LANG_BUILTIN_ATTEMPT ||
                              info[node].binding == PS_LANG_BUILTIN_ARRAY_POP_LAST ||
                              info[node].binding == PS_LANG_BUILTIN_ARRAY_COUNT ||
                              info[node].binding == PS_LANG_BUILTIN_ARRAY_APPEND ||
                              info[node].binding == PS_LANG_BUILTIN_ARRAY_REMOVE ||
                              info[node].binding == PS_LANG_BUILTIN_ARRAY_INSERT ||
                              info[node].binding == PS_LANG_BUILTIN_OPTIONAL_SOME ||
                              info[node].binding == PS_LANG_BUILTIN_OPTIONAL_UNWRAP ||
                              info[node].binding == PS_LANG_BUILTIN_OPTIONAL_OR ||
                              info[node].binding == PS_LANG_BUILTIN_OPTIONAL_HAS ||
                              ps_lang_builtin_get(info[node].binding) ||
                              ps_lang_member_name(info[node].binding));
                        if (ps_lang_record_type(info[node].type)) {
                            size_t definition = (size_t)(info[node].type - PS_TYPE_RECORD_BASE);
                            CHECK(definition < parsed.count &&
                                  (nodes[definition].kind == PS_AST_STRUCT ||
                                   nodes[definition].kind == PS_AST_ENUM));
                        }
                        if (ps_lang_optional_type(info[node].type)) {
                            size_t definition = (size_t)(info[node].type - PS_TYPE_OPTIONAL_BASE);
                            CHECK(definition < parsed.count && info[definition].optional_element);
                            CHECK(nodes[definition].kind == PS_AST_OPTIONAL_TYPE ||
                                  nodes[definition].kind == PS_AST_OPTIONAL_BINDING ||
                                  nodes[definition].kind == PS_AST_CALL);
                        }
                        if (info[node].type >= PS_TYPE_ARRAY_BASE) {
                            size_t definition = (size_t)(info[node].type - PS_TYPE_ARRAY_BASE);
                            CHECK(definition < parsed.count && info[definition].array_element);
                            CHECK(nodes[definition].kind == PS_AST_ARRAY_TYPE ||
                                  nodes[definition].kind == PS_AST_ARRAY ||
                                  nodes[definition].kind == PS_AST_ARGUMENT ||
                                  nodes[definition].kind == PS_AST_CALL);
                        }
                    }
                }
            }
            source[i] = saved;
        }
    }
    puts("Language checker: scopes, types, calls, control flow, limits and mutations passed");
    return 0;
}
