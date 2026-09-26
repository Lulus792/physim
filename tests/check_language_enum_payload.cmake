foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
set(enum "enum Event:\n    case quiet\n    case value(number: Int64, flag: Bool)\n")

function(reject name source expected)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "${expected}")
    message(FATAL_ERROR "${name} was not rejected as expected: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(wrong_type "${enum}let x = Event.value(1, 2)\n"
  "Expression type does not match required type")
reject(missing_field "${enum}let x = Event.value(1)\n"
  "Missing enum payload fields")
reject(duplicate_field "${enum}let x = Event.value(number: 1, number: 2)\n"
  "Enum payload field supplied more than once")
reject(missing_binding
  "${enum}switch Event.value(1, true):\n    case Event.quiet:\n        assert(true)\n    case Event.value(number):\n        assert(true)\n"
  "Missing enum payload bindings")
reject(literal_incomplete
  "${enum}switch Event.value(1, true):\n    case Event.quiet:\n        assert(true)\n    case Event.value(1, true):\n        assert(true)\n"
  "Switch must cover every enum case")
reject(literal_after_full
  "${enum}switch Event.value(1, true):\n    case Event.quiet:\n        assert(true)\n    case Event.value:\n        assert(true)\n    case Event.value(1, true):\n        assert(true)\n"
  "Duplicate switch case")
reject(duplicate_literal
  "${enum}switch Event.value(1, true):\n    case Event.value(1, flag):\n        assert(flag)\n    case Event.value(1, true):\n        assert(true)\n    default:\n        assert(true)\n"
  "Duplicate switch case")
reject(literal_expression
  "${enum}switch Event.value(1, true):\n    case Event.quiet:\n        assert(true)\n    case Event.value(1 + 1, true):\n        assert(true)\n    case Event.value:\n        assert(true)\n"
  "Payload pattern requires a matching scalar literal, Int64, Float64 or String range or nil for an optional field")
reject(nil_nonoptional
  "${enum}switch Event.value(1, true):\n    case Event.value(nil, true):\n        assert(true)\n    default:\n        assert(true)\n"
  "Payload pattern requires a matching scalar literal, Int64, Float64 or String range or nil for an optional field")
reject(float_integer_equivalent
  "enum Value:\n    case number(value: Float64)\nswitch Value.number(1.0):\n    case Value.number(1):\n        assert(true)\n    case Value.number(1.0):\n        assert(false)\n    default:\n        assert(false)\n"
  "Duplicate switch case")
reject(float_range_covered_literal
  "enum Value:\n    case number(value: Float64)\nswitch Value.number(1.5):\n    case Value.number(1.0...2.0):\n        assert(true)\n    case Value.number(1.5):\n        assert(false)\n    default:\n        assert(false)\n"
  "Duplicate switch case")
reject(float_range_covered_range
  "enum Value:\n    case number(value: Float64)\nswitch Value.number(1.5):\n    case Value.number(1.0...3.0):\n        assert(true)\n    case Value.number(1.5..<2.5):\n        assert(false)\n    default:\n        assert(false)\n"
  "Duplicate switch case")
reject(float_range_empty
  "enum Value:\n    case number(value: Float64)\nswitch Value.number(1.5):\n    case Value.number(2.0..<2.0):\n        assert(true)\n    default:\n        assert(false)\n"
  "Float64 switch range must contain at least one value")
reject(range_covered_literal
  "${enum}switch Event.value(3, true):\n    case Event.value(1...5, _):\n        assert(true)\n    case Event.value(3, true):\n        assert(false)\n    default:\n        assert(false)\n"
  "Duplicate switch case")
reject(range_covered_range
  "${enum}switch Event.value(3, true):\n    case Event.value(1...5, _):\n        assert(true)\n    case Event.value(2..<5, _):\n        assert(false)\n    default:\n        assert(false)\n"
  "Duplicate switch case")
reject(range_incomplete
  "${enum}switch Event.value(3, true):\n    case Event.quiet:\n        assert(true)\n    case Event.value(1...5, _):\n        assert(true)\n"
  "Switch must cover every enum case")
reject(nested_covered
  "enum Signal:\n    case note(value: Int64??)\nswitch Signal.note(nil):\n    case Signal.note(Optional.some(Optional.some)):\n        assert(true)\n    case Signal.note(Optional.some(Optional.some(5))):\n        assert(false)\n    default:\n        assert(false)\n"
  "Duplicate switch case")
reject(nested_binding_scope
  "enum Signal:\n    case note(value: Int64??)\nswitch Signal.note(Optional.some(Optional.some(5))):\n    case Signal.note(Optional.some(Optional.some(inner))):\n        assert(inner == 5)\n    default:\n        assert(false)\nassert(inner == 5)\n"
  "Unknown name")
set(finite_enum "enum Toggle:\n    case pair(left: Bool, right: Bool)\n    case maybe(value: Bool?)\n    case empty\n")
reject(finite_pair_missing
  "${finite_enum}switch Toggle.pair(true, true):\n    case Toggle.pair(true, true):\n        print(1)\n    case Toggle.pair(true, false):\n        print(2)\n    case Toggle.pair(false, true):\n        print(3)\n    case Toggle.maybe:\n        print(4)\n    case Toggle.empty:\n        print(5)\n"
  "Switch must cover every enum case")
reject(finite_pair_guarded
  "${finite_enum}switch Toggle.pair(true, true):\n    case Toggle.pair(true, true) if true:\n        print(1)\n    case Toggle.pair(true, false):\n        print(2)\n    case Toggle.pair(false, true):\n        print(3)\n    case Toggle.pair(false, false):\n        print(4)\n    case Toggle.maybe:\n        print(5)\n    case Toggle.empty:\n        print(6)\n"
  "Switch must cover every enum case")
reject(finite_optional_missing_false
  "${finite_enum}switch Toggle.maybe(nil):\n    case Toggle.pair:\n        print(1)\n    case Toggle.maybe(nil):\n        print(2)\n    case Toggle.maybe(Optional.some(true)):\n        print(3)\n    case Toggle.empty:\n        print(4)\n"
  "Switch must cover every enum case")
reject(unbounded_field_not_covered
  "enum Value:\n    case number(value: Int64, flag: Bool)\nswitch Value.number(1, true):\n    case Value.number(1, true):\n        print(1)\n    case Value.number(1, false):\n        print(2)\n"
  "Switch must cover every enum case")
reject(binding_scope
  "${enum}switch Event.value(1, true):\n    case Event.quiet:\n        assert(true)\n    case Event.value(number, flag):\n        assert(flag)\nassert(number == 1)\n"
  "Unknown name")
reject(wildcard_not_binding
  "${enum}switch Event.value(1, true):\n    case Event.quiet:\n        assert(true)\n    case Event.value(_, flag):\n        assert(flag)\n        print(_)\n"
  "Unknown name")
reject(direct_recursion "enum Loop:\n    case direct(next: Loop)\n"
  "Recursive enum values have infinite size")
reject(void_field "enum Event:\n    case invalid(value: Void)\n"
  "Unknown or unsupported type in this compiler stage")
set(opaque "enum Opaque:\n    case value(state: Rng)\n")
reject(opaque_equality
  "${opaque}func equal(a: Opaque, b: Opaque) -> Bool:\n    return a == b\n"
  "Equality requires values of the same type")
reject(equatable_constraint
  "${opaque}func same<T: Equatable>(value: T) -> T:\n    return value\nfunc use(value: Opaque) -> Opaque:\n    return same(value)\n"
  "Type argument must satisfy Equatable constraint")
