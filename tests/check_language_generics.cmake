foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
function(reject name source message)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT output STREQUAL "" OR NOT diagnostic MATCHES "${message}")
    message(FATAL_ERROR "Generic error ${name} was not reported: ${result}\n${output}\n${diagnostic}")
  endif()
endfunction()

reject(conflict "func choose<T>(a: T, b: T) -> T:\n    return a\nlet first: Int64 = 1\nlet x = choose(first, 2.0)\n"
  "Expression type does not match required type|Conflicting types for generic parameter")
reject(unbound "func make<T>() -> T:\n    return nil\nlet x = make()\n"
  "Cannot infer every generic type parameter")
reject(empty_unbound "func pass<T>(value: [T]) -> [T]:\n    return value\nlet x = pass([])\n"
  "Cannot infer every generic type parameter")
reject(nil_unbound "func pass<T>(value: T?) -> T?:\n    return value\nlet x = pass(nil)\n"
  "Cannot infer every generic type parameter")
reject(explicit_excess "func identity<T>(value: T) -> T:\n    return value\nlet x = identity<Int64, Float64>(1)\n"
  "Too many explicit type arguments")
reject(explicit_missing "func pair<A, B>(left: A, right: B) -> A:\n    return left\nlet x = pair<Int64>(1, 2)\n"
  "Missing explicit type arguments")
reject(explicit_mismatch "func identity<T>(value: T) -> T:\n    return value\nlet x = identity<Float64>(true)\n"
  "Expression type does not match required type")
reject(explicit_unknown "func identity<T>(value: T) -> T:\n    return value\nlet x = identity<Unknown>(1)\n"
  "Unknown or unsupported type")
reject(explicit_void "func identity<T>(value: T) -> T:\n    return value\nlet x = identity<Void>(1)\n"
  "Unknown or unsupported type")
reject(explicit_nongeneric "func plain(value: Int64) -> Int64:\n    return value\nlet x = plain<Int64>(1)\n"
  "Explicit type arguments require a generic function")
reject(explicit_builtin "print<Int64>(1)\n"
  "Explicit type arguments require a generic function")
reject(constraint_unknown "func identity<T: Unknown>(value: T) -> T:\n    return value\n"
  "Unknown generic constraint")
reject(constraint_duplicate "func identity<T: Numeric & Numeric>(value: T) -> T:\n    return value\n"
  "Duplicate generic constraint")
reject(constraint_numeric "func identity<T: Numeric>(value: T) -> T:\n    return value\nlet x = identity(true)\n"
  "Type argument must satisfy Numeric constraint")
reject(constraint_numeric_explicit "func identity<T: Numeric>(value: T) -> T:\n    return value\nlet x = identity<Bool>(true)\n"
  "Type argument must satisfy Numeric constraint")
reject(constraint_scalar "func identity<T: Scalar>(value: T) -> T:\n    return value\nlet x = identity([1, 2])\n"
  "Type argument must satisfy Scalar constraint")
reject(constraint_equatable "func identity<T: Equatable>(value: T) -> T:\n    return value\nfunc use(value: Rng) -> Rng:\n    return identity(value)\n"
  "Type argument must satisfy Equatable constraint")
reject(constraint_vector "func identity<T: Vector>(value: T) -> T:\n    return value\nlet x = identity(1)\n"
  "Type argument must satisfy Vector constraint")
reject(duplicate "func same<T, T>(value: T) -> T:\n    return value\n"
  "Duplicate generic type parameter")
reject(builtin "func wrong<Int64>(value: Int64) -> Int64:\n    return value\n"
  "Generic type parameter conflicts with a builtin type")
reject(signature "func wrong<T>(value: Unknown) -> T:\n    return value\n"
  "Unknown or unsupported type")
reject(parameters "func wrong<T>(value: T, value: T) -> T:\n    return value\n"
  "Duplicate parameter name")
reject(method_body "struct Box:\n    let value: Int64\n    func bad<T>(value: T) -> T:\n        return self.value + value\nlet x = Box(1).bad(true)\n"
  "Invalid|requires|operand|Expression type does not match required type")
reject(method_static "struct Box:\n    let value: Int64\n    static func echo<T>(value: T) -> T:\n        return value\nlet x = Box(1).echo(2)\n"
  "Static method must be called on its type")
reject(method_mutating "struct Box:\n    var value: Int64\n    mutating func echo<T>(value: T) -> T:\n        self.value += 1\n        return value\nlet box = Box(0)\nlet x = box.echo(1)\n"
  "Mutating method requires a mutable var binding")
reject(method_signature "struct Box:\n    let value: Int64\n    static func echo<T>(value: Unknown) -> T:\n        return value\n"
  "Unknown or unsupported type")
reject(method_type_parameter "struct Box:\n    let value: Int64\n    func echo<self>(value: self) -> self:\n        return value\n"
  "self is reserved")
reject(body "func add<T>(a: T, b: T) -> T:\n    return a + b\nlet x = add(true, false)\n"
  "Invalid|requires|operand")
reject(value "func identity<T>(value: T) -> T:\n    return value\nlet f = identity\n"
  "Generic function values require specialization")
reject(recursion_limit "func grow<T>(value: T) -> Int64:\n    return grow([value])\nlet x = grow(1)\n"
  "Generic specialization limit exceeded")

file(WRITE "${WORK}/Imported.phys"
  "func invalid<T>(value: T) -> T:\n    return value + true\n")
file(WRITE "${WORK}/Main.phys" "import Imported\nlet x = Imported.invalid(1)\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Imported\\.phys:2:[0-9]+: error:")
  message(FATAL_ERROR "Imported generic diagnostic lost its source: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/BadMethod.phys"
  "struct Box:\n    let value: Int64\n    func invalid<T>(value: T) -> T:\n        return value + true\n")
file(WRITE "${WORK}/MainMethod.phys"
  "import BadMethod\nlet x = BadMethod.Box(1).invalid(2)\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/MainMethod.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "BadMethod\\.phys:4:[0-9]+: error:")
  message(FATAL_ERROR "Imported generic method diagnostic lost its source: ${result}\n${diagnostic}")
endif()
