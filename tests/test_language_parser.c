#include "language/parser.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Language parser line %d: %s\n", __LINE__, #x);                        \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)

static ps_lang_node nodes[4096];
static ps_lang_parse_result parse(const char *s) {
    return ps_lang_parse(s, strlen(s), nodes, sizeof(nodes) / sizeof(nodes[0]));
}
static int rejected(const char *source) {
    ps_lang_parse_result r = parse(source);
    CHECK(!r.root && r.diagnostic.kind == PS_LANG_ERROR && r.diagnostic.error);
    CHECK(r.diagnostic.offset <= strlen(source));
    return 0;
}
static int bounded(const unsigned char *source, size_t size) {
    ps_lang_node small[96];
    ps_lang_parse_result r = ps_lang_parse(source, size, small, 96);
    CHECK(r.count <= 96);
    if (!r.root) {
        CHECK(r.diagnostic.kind == PS_LANG_ERROR && r.diagnostic.error);
        CHECK(r.diagnostic.offset <= size);
        CHECK(r.diagnostic.length <= size - r.diagnostic.offset);
        return 0;
    }
    CHECK(r.root < r.count && small[r.root].kind == PS_AST_MODULE);
    CHECK(r.diagnostic.kind != PS_LANG_ERROR);
    for (size_t i = 1; i < r.count; i++) {
        CHECK(small[i].a < r.count && small[i].b < r.count && small[i].c < r.count);
        CHECK(small[i].next < r.count);
        CHECK(small[i].token.offset <= size);
        CHECK(small[i].token.length <= size - small[i].token.offset);
        /* Children are emitted before their parent, making cycles impossible.
         * Siblings are emitted later, so a next chain is strictly increasing. */
        CHECK(small[i].a < i && small[i].b < i && small[i].c < i);
        CHECK(!small[i].next || small[i].next > i);
    }
    return 0;
}
int main(void) {
    ps_lang_parse_result guarded = parse("func f(value: Int64?) -> Int64:\n"
                                         "    guard let item = value else:\n"
                                         "        return 0\n"
                                         "    return item\n");
    CHECK(guarded.root);
    size_t guard_function = nodes[guarded.root].a;
    size_t guard_node = nodes[nodes[guard_function].c].a;
    CHECK(nodes[guard_node].kind == PS_AST_GUARD &&
          nodes[nodes[guard_node].a].kind == PS_AST_OPTIONAL_BINDING &&
          nodes[nodes[guard_node].b].kind == PS_AST_BLOCK);
    const char *interpolation_source = "let s = \"a \\(f(2)) b\"\n";
    ps_lang_parse_result interpolation = parse(interpolation_source);
    CHECK(interpolation.root);
    size_t interpolated = nodes[nodes[interpolation.root].a].b;
    CHECK(nodes[interpolated].kind == PS_AST_INTERPOLATED_STRING);
    size_t first_part = nodes[interpolated].a;
    size_t embedded = nodes[first_part].next;
    size_t final_part = nodes[embedded].next;
    CHECK(nodes[first_part].kind == PS_AST_STRING_SEGMENT &&
          nodes[embedded].kind == PS_AST_CALL &&
          nodes[nodes[embedded].a].token.column == 14 &&
          nodes[final_part].kind == PS_AST_STRING_SEGMENT &&
          !nodes[final_part].next);
    ps_lang_parse_result coalesced = parse("let x = a ?? b ?? c\n");
    CHECK(coalesced.root);
    size_t outer_coalesce = nodes[nodes[coalesced.root].a].b;
    CHECK(nodes[outer_coalesce].kind == PS_AST_BINARY &&
          nodes[outer_coalesce].token.kind == PS_LANG_COALESCE &&
          nodes[nodes[outer_coalesce].b].token.kind == PS_LANG_COALESCE);
    ps_lang_parse_result precedence = parse("let x = a ?? b || c\n");
    CHECK(precedence.root);
    outer_coalesce = nodes[nodes[precedence.root].a].b;
    CHECK(nodes[outer_coalesce].token.kind == PS_LANG_COALESCE &&
          nodes[nodes[outer_coalesce].b].token.kind == PS_LANG_OR);
    CHECK(parse("let x: Int64?? = nil\nlet y = x ?? 0\n").root);
    CHECK(rejected("let x = a ? ? b\n") == 0);
    CHECK(rejected("let x = a ??\n") == 0);
    ps_lang_parse_result binding = parse("if let x: Float64 = source:\n    print(x)\n"
                                        "else if var y = other:\n    y += 1\n"
                                        "while let z = next():\n    break\n");
    CHECK(binding.root);
    size_t binding_if = nodes[binding.root].a;
    size_t binding_condition = nodes[binding_if].a;
    CHECK(nodes[binding_condition].kind == PS_AST_OPTIONAL_BINDING &&
          nodes[binding_condition].token.kind == PS_LANG_LET && nodes[binding_condition].b);
    CHECK(nodes[nodes[nodes[binding_if].c].a].token.kind == PS_LANG_VAR);
    CHECK(nodes[nodes[binding_if].next].kind == PS_AST_WHILE);
    CHECK(rejected("if let = source:\n    print(1)") == 0);
    CHECK(rejected("if let x source:\n    print(1)") == 0);
    CHECK(rejected("if let x = source, let y = other:\n    print(1)") == 0);
    CHECK(parse("var x = 1\nx +=\n    2\nx *= 3\nx -= 1\nx /= 2\nx %= 4").root);
    CHECK(parse("var x = 1\nx &= 3\nx |= 4\nx ^= 2\nx <<= 1\nx >>= 2").root);
    CHECK(rejected("var x = 1\nx +=") == 0);
    CHECK(rejected("var x += 1") == 0);
    CHECK(rejected("var x = 1\nlet y = (x += 2)") == 0);
    CHECK(parse("switch phase:\n    case Phase.up:\n        print(1)\n"
                "    default:\n        print(2)")
              .root);
    CHECK(rejected("switch phase { case Phase.up: print(1) }") == 0);
    CHECK(rejected("switch phase:\ncase Phase.up:\n    print(1)") == 0);
    CHECK(rejected("switch phase:\n    case Phase.up:\n    print(1)") == 0);
    CHECK(rejected("switch phase:\n    default:\n        print(1)\n"
                   "    case Phase.up:\n        print(2)") == 0);
    CHECK(rejected("switch phase:\n    default:\n        print(1)\n"
                   "    default:\n        print(2)") == 0);
    CHECK(parse("enum Mode:\n    case idle\n    case running\nlet x = Mode.idle").root);
    ps_lang_parse_result method_enum = parse(
        "enum Mode:\n    case idle\n    func code() -> Int64:\n        return 1\n"
        "    mutating func reset():\n        self = Mode.idle\n"
        "    static func make() -> Mode:\n        return Mode.idle\n");
    CHECK(method_enum.root);
    size_t method_enum_id = nodes[method_enum.root].a;
    CHECK(nodes[method_enum_id].kind == PS_AST_ENUM && nodes[method_enum_id].a &&
          nodes[method_enum_id].b && nodes[nodes[method_enum_id].b].next);
    CHECK(rejected("enum E:\ncase a") == 0);
    CHECK(rejected("enum E:\n    case a\n      case b") == 0);
    CHECK(rejected("enum E { case a }") == 0);
    CHECK(rejected("enum E:\n    case a(Int64)") == 0);
    ps_lang_parse_result raw_enum = parse("enum E:\n    case a = 1");
    CHECK(raw_enum.root);
    size_t raw_case = nodes[nodes[raw_enum.root].a].a;
    CHECK(nodes[raw_case].kind == PS_AST_CASE && nodes[raw_case].b &&
          nodes[nodes[raw_case].b].token.kind == PS_LANG_INTEGER);
    CHECK(rejected("enum E:\n    case a, b") == 0);
    CHECK(rejected("enum E:\n    var a: Int64") == 0);
    ps_lang_parse_result r =
        parse("let energy: Float64 = 1 + 2 * 3 - 4\nvar ok = true || false && !false");
    CHECK(r.root && r.diagnostic.kind != PS_LANG_ERROR);
    size_t v = nodes[r.root].a;
    CHECK(nodes[v].kind == PS_AST_VARIABLE && nodes[v].token.kind == PS_LANG_LET);
    CHECK(nodes[nodes[v].a].kind == PS_AST_TYPE);
    size_t minus = nodes[v].b, plus = nodes[minus].a, multiply = nodes[plus].b;
    CHECK(nodes[minus].kind == PS_AST_BINARY && nodes[minus].token.kind == PS_LANG_MINUS);
    CHECK(nodes[plus].token.kind == PS_LANG_PLUS && nodes[multiply].token.kind == PS_LANG_STAR);
    v = nodes[v].next;
    CHECK(nodes[v].token.kind == PS_LANG_VAR && !nodes[v].next && !nodes[v].a);
    size_t logical = nodes[v].b;
    CHECK(nodes[logical].token.kind == PS_LANG_OR);
    logical = nodes[logical].b;
    CHECK(nodes[logical].token.kind == PS_LANG_AND);
    CHECK(nodes[nodes[logical].b].kind == PS_AST_UNARY);
    r = parse("let x = 8 - 3 - 1");
    CHECK(r.root);
    v = nodes[nodes[r.root].a].b;
    CHECK(nodes[v].token.kind == PS_LANG_MINUS);
    CHECK(nodes[nodes[v].a].token.kind == PS_LANG_MINUS); /* left associative */
    CHECK(nodes[nodes[v].b].kind == PS_AST_LITERAL);
    r = parse("let bits = ~1 & 2 ^ 3 | 4 && true");
    CHECK(r.root);
    v = nodes[nodes[r.root].a].b;
    CHECK(nodes[v].token.kind == PS_LANG_AND);
    v = nodes[v].a;
    CHECK(nodes[v].token.kind == PS_LANG_PIPE);
    CHECK(nodes[nodes[v].a].token.kind == PS_LANG_CARET);
    CHECK(nodes[nodes[nodes[v].a].a].token.kind == PS_LANG_AMP);
    r = parse("let bits = 1 + 2 << 3 * 4");
    CHECK(r.root);
    v = nodes[nodes[r.root].a].b;
    CHECK(nodes[v].token.kind == PS_LANG_SHIFT_LEFT);
    CHECK(nodes[nodes[v].a].token.kind == PS_LANG_PLUS);
    CHECK(nodes[nodes[v].b].token.kind == PS_LANG_STAR);
    CHECK(parse("struct Box<T>:\n    let value: T\n"
                "let box: Box<Box<Int64>> = Box(Box(1))\n").root);
    CHECK(parse("struct Box<T>:\n    let value: T\n"
                "let box: Box<Box<Int64>>=Box(Box(1))\n"
                "let single: Box<Int64>=Box(1)\n").root);
    r = parse("struct State:\n    var x: Float64\n    let label: String\nlet s = State(1, \"x\")");
    CHECK(r.root);
    size_t record = nodes[r.root].a, field = nodes[record].a;
    CHECK(nodes[record].kind == PS_AST_STRUCT && nodes[field].kind == PS_AST_FIELD);
    CHECK(nodes[field].token.kind == PS_LANG_VAR && nodes[nodes[field].a].kind == PS_AST_TYPE);
    CHECK(nodes[nodes[field].next].token.kind == PS_LANG_LET);
    CHECK(nodes[nodes[record].next].kind == PS_AST_VARIABLE);
    r = parse("struct S:\n    var x: Int64 = 1\n");
    CHECK(r.root);
    field = nodes[nodes[r.root].a].a;
    CHECK(nodes[field].b && nodes[nodes[field].b].kind == PS_AST_LITERAL);
    const char *bad_records[] = {"struct S:",
                                 "struct S:\nvar x: Int64",
                                 "struct S:\n    x: Int64",
                                 "struct S:\n    var x",
                                 "struct S:\n    var x: Int64\n      var y: Int64",
                                 "struct S:\n    return"};
    for (size_t i = 0; i < sizeof(bad_records) / sizeof(bad_records[0]); i++)
        CHECK(rejected(bad_records[i]) == 0);

    const char *function = "func energy(mass: Float64,\nspeed: Float64,) -> Float64:\n"
                           " let result = 0.5 * mass * speed * speed\n return result\n"
                           "let value = energy(mass: 2.0, speed: 3.0)";
    r = parse(function);
    CHECK(r.root);
    size_t fn = nodes[r.root].a;
    CHECK(nodes[fn].kind == PS_AST_FUNCTION);
    size_t parameter = nodes[fn].a;
    CHECK(nodes[parameter].kind == PS_AST_PARAMETER && nodes[parameter].next);
    CHECK(!nodes[nodes[parameter].next].next);
    CHECK(nodes[nodes[fn].b].kind == PS_AST_TYPE && nodes[nodes[fn].c].kind == PS_AST_BLOCK);
    size_t body = nodes[nodes[fn].c].a;
    CHECK(nodes[body].kind == PS_AST_VARIABLE);
    CHECK(nodes[nodes[body].next].kind == PS_AST_RETURN);
    v = nodes[nodes[fn].next].b;
    CHECK(nodes[v].kind == PS_AST_CALL && nodes[nodes[v].a].kind == PS_AST_NAME);
    size_t arg = nodes[v].b;
    CHECK(nodes[arg].kind == PS_AST_ARGUMENT && nodes[arg].token.length == 4);
    CHECK(!memcmp(function + nodes[arg].token.offset, "mass", 4));
    CHECK(nodes[nodes[arg].next].token.length == 5);

    r = parse("var values: [Float64]? = [1.0,\n2.0,]\n"
              "values[0] = model.position.x + f(\n1,\n2\n)");
    CHECK(r.root);
    v = nodes[r.root].a;
    CHECK(nodes[nodes[v].a].kind == PS_AST_OPTIONAL_TYPE);
    CHECK(nodes[nodes[nodes[v].a].a].kind == PS_AST_ARRAY_TYPE);
    CHECK(nodes[nodes[v].b].kind == PS_AST_ARRAY);
    size_t assignment = nodes[v].next;
    CHECK(nodes[assignment].kind == PS_AST_ASSIGN);
    CHECK(nodes[nodes[assignment].a].kind == PS_AST_INDEX);
    size_t add = nodes[assignment].b;
    CHECK(nodes[nodes[add].a].kind == PS_AST_MEMBER);
    CHECK(nodes[nodes[nodes[add].a].a].kind == PS_AST_MEMBER);
    r = parse("var a = [1]\na[0..<1] += [2]\n");
    CHECK(r.root);
    assignment = nodes[nodes[r.root].a].next;
    CHECK(nodes[assignment].kind == PS_AST_ASSIGN &&
          nodes[assignment].token.kind == PS_LANG_PLUS_EQUAL &&
          nodes[nodes[assignment].b].a == nodes[assignment].a);
    r = parse("let a = values[..<2]\nlet b = values[1...]\nlet c = values[...]\n");
    CHECK(r.root);
    v = nodes[r.root].a;
    size_t range = nodes[nodes[v].b].b;
    CHECK(nodes[range].kind == PS_AST_BINARY && nodes[range].token.kind == PS_LANG_RANGE_OPEN &&
          !nodes[range].a && nodes[range].b);
    v = nodes[v].next;
    range = nodes[nodes[v].b].b;
    CHECK(nodes[range].token.kind == PS_LANG_RANGE_CLOSED && nodes[range].a && !nodes[range].b);
    v = nodes[v].next;
    range = nodes[nodes[v].b].b;
    CHECK(nodes[range].token.kind == PS_LANG_RANGE_CLOSED && !nodes[range].a && !nodes[range].b);
    CHECK(rejected("let x = ..<3") == 0);
    CHECK(rejected("let x = 1..<") == 0);
    r = parse("let value: Int64?? = Optional.some(nil)\n");
    CHECK(r.root);
    v = nodes[nodes[r.root].a].a;
    CHECK(nodes[v].kind == PS_AST_OPTIONAL_TYPE &&
          nodes[nodes[v].a].kind == PS_AST_OPTIONAL_TYPE &&
          nodes[nodes[nodes[v].a].a].kind == PS_AST_TYPE);

    r = parse("func test():\n    var i = 0\n    while i < 10:\n        i = i + 1\n"
              "    for j in 0..<10:\n        if j == 3:\n            continue\n"
              "        else if j == 9:\n            break\n"
              "    if true:\n        return\n    else:\n        return\n");
    CHECK(r.root);
    body = nodes[nodes[nodes[r.root].a].c].a;
    CHECK(nodes[nodes[body].next].kind == PS_AST_WHILE);
    body = nodes[nodes[body].next].next;
    CHECK(nodes[body].kind == PS_AST_FOR);
    CHECK(nodes[nodes[body].a].token.kind == PS_LANG_RANGE_OPEN);
    body = nodes[nodes[body].b].a;
    CHECK(nodes[body].kind == PS_AST_IF && nodes[nodes[body].c].kind == PS_AST_IF);
    r = parse("let x = (1\n+ 2) *\n3; let y = []\nlet z = f()\n");
    CHECK(r.root);
    r = parse("let x = 1 /*\n*/ let y = 2");
    CHECK(r.root && nodes[nodes[r.root].a].next);

    /* The dedent assigns else to the outer if and closes both nested blocks. */
    r = parse("if true:\n    if false:\n        return 1\n"
              "else:\n    return 2\nlet done = true");
    CHECK(r.root);
    size_t outer = nodes[r.root].a;
    size_t inner = nodes[nodes[outer].b].a;
    CHECK(nodes[inner].kind == PS_AST_IF && !nodes[inner].c);
    CHECK(nodes[nodes[outer].c].kind == PS_AST_BLOCK);
    CHECK(nodes[nodes[outer].next].kind == PS_AST_VARIABLE);
    CHECK(!nodes[inner].next);
    CHECK(nodes[nodes[nodes[outer].c].a].kind == PS_AST_RETURN);

    /* Comments and empty lines do not decide block indentation. */
    r = parse("func f(): // header\r\n\r\n// unindented comment\r\n"
              "  // comment\r\n  if true:\r\n      return 1\r\n"
              "/* comment\r\ncontinued */\r\n  return 2");
    CHECK(r.root);
    body = nodes[nodes[nodes[r.root].a].c].a;
    CHECK(nodes[body].kind == PS_AST_IF && nodes[nodes[body].next].kind == PS_AST_RETURN);
    r = parse("if true:\r  return\rlet x = 1");
    CHECK(r.root && nodes[nodes[r.root].a].next);
    r = parse("func f():\n    let x = f(\n1,\n        value: [\n2, 3\n]\n)\n"
              "    return x");
    CHECK(r.root);
    r = parse("if true:\n    return\n    return");
    CHECK(r.root); /* Last block ends at EOF without a newline. */

    const char *bad_layout[] = {"func f() {}",
                                "if true { return }",
                                "while true { break }",
                                "{ return }",
                                "func f():",
                                "func f():\n",
                                "func f():\n// comment\n",
                                "if true:\nreturn",
                                "if true: return",
                                "if true\n    return",
                                "if true\n:\n    return",
                                "    let x = 1",
                                "\tlet x = 1",
                                "if true:\n\treturn",
                                "if true:\n \treturn",
                                "if true:\n    return\n      return",
                                "if true:\n    return\n  return",
                                "if true:\n    if true:\n        return\n      return",
                                "if true:\n    return\n  else:\n    return",
                                "if true:\n    return\nelse: return",
                                "if true:\n    return\nelse:\nreturn",
                                "if true:\n    return\nelse\n:\n    return"};
    for (size_t i = 0; i < sizeof(bad_layout) / sizeof(bad_layout[0]); i++)
        CHECK(rejected(bad_layout[i]) == 0);
    r = parse("if true:\n    return\n  return");
    CHECK(!r.root && r.diagnostic.line == 3 && r.diagnostic.column == 3);

    const char *bad[] = {"let = 1",
                         "let x",
                         "var x",
                         "let x =",
                         "let x = 1 let y = 2",
                         "let x = (1 + 2",
                         "let x = [1,,2]",
                         "let x = f(,)",
                         "let x = f(a:)",
                         "let x = f(a: 1 b: 2)",
                         "let x = a.",
                         "let x = a[]",
                         "let x: [] = []",
                         "func f(x) {}",
                         "func f(x:) {}",
                         "func f(x: Float64 {}",
                         "func f() -> {}",
                         "func f() { return 1",
                         "if true return 1",
                         "if true {} else return 2",
                         "for in 0..<5 {}",
                         "for x 0..<5 {}",
                         "while {}",
                         "}",
                         "else {}",
                         "struct Thing {}",
                         "let x = \"\\q\"",
                         "let x = 1e+"};
    for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++)
        CHECK(rejected(bad[i]) == 0);
    r = parse("var x: Float64\n");
    CHECK(r.root && nodes[nodes[r.root].a].kind == PS_AST_VARIABLE &&
          nodes[nodes[r.root].a].a && !nodes[nodes[r.root].a].b);
    r = parse("let x = 1\nlet y = )");
    CHECK(!r.root && r.diagnostic.line == 2 && r.diagnostic.column == 9);
    r = ps_lang_parse(NULL, 0, nodes, 4096);
    CHECK(r.root && !nodes[r.root].a);
    r = ps_lang_parse(NULL, 1, nodes, 4096);
    CHECK(!r.root && r.diagnostic.kind == PS_LANG_ERROR);
    r = ps_lang_parse("", 0, NULL, 4096);
    CHECK(!r.root);
    r = ps_lang_parse("", 0, nodes, 1);
    CHECK(!r.root);
    for (size_t capacity = 0; capacity < 12; capacity++) {
        nodes[capacity].a = 123456;
        r = ps_lang_parse("let x = 1 + 2 * 3", 17, nodes, capacity);
        CHECK(r.count <= capacity);
        CHECK(nodes[capacity].a == 123456); /* no write past caller capacity */
    }
    char deep[2048];
    memset(deep, '(', 512);
    deep[512] = '1';
    memset(deep + 513, ')', 512);
    deep[1025] = 0;
    CHECK(rejected(deep) == 0);
    CHECK(strstr(parse(deep).diagnostic.error, "nesting"));
    char nested[40000];
    size_t used = 0;
    for (size_t i = 0; i < 180; i++) {
        memset(nested + used, ' ', i);
        used += i;
        memcpy(nested + used, "if true:\n", 9);
        used += 9;
    }
    memset(nested + used, ' ', 180);
    used += 180;
    memcpy(nested + used, "return", 7);
    CHECK(rejected(nested) == 0);
    CHECK(strstr(parse(nested).diagnostic.error, "nesting"));
    memset(deep, '{', 512);
    memset(deep + 512, '}', 512);
    deep[1024] = 0;
    CHECK(rejected(deep) == 0);
    unsigned char source[] =
        "func f(x: Float64):\n    let y = [1,2]\n    return x * y[0]\nlet z=f(x:3)";
    size_t size = sizeof(source) - 1;
    for (size_t n = 0; n <= size; n++)
        CHECK(bounded(source, n) == 0);
    for (size_t i = 0; i < size; i++) {
        unsigned char saved = source[i];
        for (unsigned b = 0; b < 256; b++) {
            source[i] = (unsigned char)b;
            CHECK(bounded(source, size) == 0);
        }
        source[i] = saved;
    }
    puts("Language parser: AST, precedence, diagnostics, resource limits and mutations passed");
    return 0;
}
