foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")

function(reject name source expected)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "${expected}")
    message(FATAL_ERROR "${name} was not rejected as expected: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(nonstatic "struct S:\n    let value: Int64\n    func init(value: Int64) -> S:\n        return S(value)\n"
  "Struct initializer must be a static function")
reject(result_type "struct S:\n    let value: Int64\n    static func init(value: Int64) -> Int64:\n        return value\n"
  "Struct initializer must return its struct type")
reject(method_type_parameter_nonstatic "struct S:\n    let value: Int64\n    func init<T>(value: T) -> S:\n        return S(1)\n"
  "Struct initializer must be a static function")
reject(method_type_parameter_result "struct S:\n    let value: Int64\n    static func init<T>(value: T) -> Int64:\n        return 1\n"
  "Struct initializer must return its struct type")
reject(method_type_parameter_constraint "struct S:\n    let value: Int64\n    static func init<T: Numeric>(value: T) -> S:\n        return S(1)\nlet s = S(\"x\")\n"
  "Type argument must satisfy Numeric constraint")
reject(method_type_parameter_uninferred "struct S:\n    let value: Int64\n    static func init<T>() -> S:\n        return S(1)\nlet s = S()\n"
  "Cannot infer every generic type parameter")
reject(method_type_parameter_nil_uninferred "struct S:\n    let value: Int64\n    static func init<T>(value: T?) -> S:\n        return S(1)\nlet s = S(nil)\n"
  "Cannot infer every generic type parameter")
reject(method_type_parameter_duplicate "struct S:\n    let value: Int64\n    static func init<T>(value: T) -> S:\n        return S(1)\n    static func init<U>(value: U) -> S:\n        return S(2)\n"
  "Duplicate struct initializer signature")
reject(method_type_argument_without_generic_init "struct S:\n    let value: Int64\n    static func init(value: Int64) -> S:\n        return S(value)\nlet s = S<Int64>(value: 1)\n"
  "Explicit type arguments require a generic function or method")
reject(parameter_name "struct S:\n    let value: Int64\n    static func init(input: Int64) -> S:\n        return S(input)\nlet s = S(value: 1)\n"
  "Unknown parameter name")
reject(generic_parameter_name "struct Box<T>:\n    let item: T\n    static func init(value: T) -> Box<T>:\n        return Box(value)\nlet box = Box(item: 1)\n"
  "Unknown field name")
reject(generic_result_type "struct Box<T>:\n    let item: T\n    static func init(value: T) -> Int64:\n        return 1\n"
  "Struct initializer must return its struct type")
reject(generic_uninferred "struct Box<T>:\n    let item: [T]\n    static func init() -> Box<T>:\n        return Box([])\nlet box = Box()\n"
  "Cannot infer every generic struct type parameter")
reject(overload_ambiguous "struct S:\n    let value: Int64\n    static func init(left: Int64) -> S:\n        return S(left)\n    static func init(right: Int64) -> S:\n        return S(right)\nlet s = S(1)\n"
  "Ambiguous struct initializer overload")
reject(value_ambiguous "struct S:\n    let value: Int64\n    static func init() -> S:\n        return S(0)\n    static func init(value: Int64) -> S:\n        return S(value)\nlet factory = S.init\n"
  "Ambiguous struct initializer value")
reject(value_same_type_ambiguous "struct S:\n    let value: Int64\n    static func init(left: Int64) -> S:\n        return S(left)\n    static func init(right: Int64) -> S:\n        return S(right)\nlet factory: func(Int64) -> S = S.init\n"
  "Ambiguous struct initializer value")
reject(value_type_mismatch "struct S:\n    let value: Int64\n    static func init(value: Int64) -> S:\n        return S(value)\nlet factory: func(String) -> S = S.init\n"
  "No struct initializer matches the required function type")
reject(value_generic_unspecialized "struct S:\n    let value: Int64\n    static func init<T>(value: T) -> S:\n        return S(1)\nlet factory = S.init\n"
  "Generic initializer values require explicit type arguments")
reject(value_generic_ambiguous "struct S:\n    let value: Int64\n    static func init<T>(value: T?) -> S:\n        return S(1)\n    static func init<T>(value: [T]) -> S:\n        return S(2)\nlet factory = S.init<Int64>\n"
  "Ambiguous generic initializer value")
reject(value_generic_no_match "struct S:\n    let value: Int64\n    static func init<T>(value: T?) -> S:\n        return S(1)\n    static func init<T>(value: [T]) -> S:\n        return S(2)\nlet factory: func(Bool) -> S = S.init<Int64>\n"
  "No generic initializer matches the type arguments and function type")
reject(overload_duplicate "struct S:\n    let value: Int64\n    static func init(value: Int64) -> S:\n        return S(value)\n    static func init(value: Int64) -> S:\n        return S(1)\n"
  "Duplicate struct initializer signature")
reject(overload_duplicate_reordered "struct S:\n    let value: Int64\n    static func init(left: Int64, right: String) -> S:\n        return S(left)\n    static func init(right: String, left: Int64) -> S:\n        return S(left)\n"
  "Duplicate struct initializer signature")
reject(overload_untyped_nil "struct S:\n    let value: Int64\n    static func init(value: Int64?) -> S:\n        return S(1)\n    static func init(value: String?) -> S:\n        return S(2)\nlet s = S(nil)\n"
  "Ambiguous struct initializer overload")
reject(overload_untyped_array "struct S:\n    let value: Int64\n    static func init(value: [Int64]) -> S:\n        return S(1)\n    static func init(value: [String]) -> S:\n        return S(2)\nlet s = S([])\n"
  "Ambiguous struct initializer overload")
reject(overload_no_type_match "struct S:\n    let value: Int64\n    static func init(value: Int64) -> S:\n        return S(value)\n    static func init(value: String) -> S:\n        return S(1)\nlet s = S(true)\n"
  "No matching struct initializer overload")
reject(optional_some_no_type_match "struct S:\n    let value: Int64\n    static func init(value: Int64?) -> S:\n        return S(1)\n    static func init(value: String?) -> S:\n        return S(2)\nlet s = S(Optional.some(true))\n"
  "No matching struct initializer overload")
reject(optional_some_ambiguous "struct S:\n    let value: Int64\n    static func init(value: Int64??) -> S:\n        return S(1)\n    static func init(value: String??) -> S:\n        return S(2)\nlet s = S(Optional.some(nil))\n"
  "Ambiguous struct initializer overload")
reject(generic_overload_duplicate "struct Box<T>:\n    let value: T\n    static func init(value: T) -> Box<T>:\n        return Box(value)\n    static func init(value: T) -> Box<T>:\n        return Box(value)\n"
  "Duplicate struct initializer signature")
reject(generic_overload_collision "struct Box<T>:\n    let value: T\n    static func init(value: T) -> Box<T>:\n        return Box(value)\n    static func init(value: Int64) -> Box<T>:\n        return Box(value)\nlet box = Box<Int64>(value: 1)\n"
  "Generic struct initializer signatures collide after specialization")
reject(generic_method_overload_collision "struct Box<T>:\n    let value: T\n    static func init(value: T) -> Box<T>:\n        return Box(value)\n    static func init<U>(value: Int64) -> Box<T>:\n        return Box(value)\nlet box = Box<Int64>(value: 1)\n"
  "Generic struct initializer signatures collide after specialization")
reject(generic_overload_ambiguous "struct Box<T>:\n    let value: T\n    static func init(value: T, other: String?) -> Box<T>:\n        return Box(value)\n    static func init(value: T, other: Bool?) -> Box<T>:\n        return Box(value)\nlet box = Box(value: 1, other: nil)\n"
  "Ambiguous generic struct initializer overload")
reject(generic_context_uninferred "struct Box<T>:\n    let value: Int64\n    static func init(value: T?) -> Box<T>:\n        return Box(1)\n    static func init(value: [T]) -> Box<T>:\n        return Box(2)\nlet box = Box(nil)\n"
  "Cannot infer every generic struct type parameter")
reject(generic_some_uninferred "struct Box<T>:\n    let value: Int64\n    static func init(value: T?) -> Box<T>:\n        return Box(1)\n    static func init(value: [T]) -> Box<T>:\n        return Box(2)\nlet box = Box(Optional.some(nil))\n"
  "nil requires an explicit optional type")
reject(generic_numeric_variable_conflict "struct Pair<T>:\n    let left: T\n    let right: T\n    static func init(left: T, right: T) -> Pair<T>:\n        return Pair(left, right)\nlet integer: Int64 = 1\nlet pair = Pair(left: integer, right: 2.5)\n"
  "Expression type does not match required type")
reject(generic_numeric_optional_variable_conflict "struct Box<T>:\n    let value: T\n    static func init(optional: T?, value: T) -> Box<T>:\n        return Box(value)\nlet integer: Int64 = 1\nlet box = Box(optional: Optional.some(integer), value: 2.5)\n"
  "Expression type does not match required type")
