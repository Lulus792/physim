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
    message(FATAL_ERROR "Generic struct error ${name} was not reported: ${result}\n${output}\n${diagnostic}")
  endif()
endfunction()

reject(duplicate "struct Box<T, T>:\n    let value: T\n"
  "Duplicate generic type parameter")
reject(builtin "struct Box<Int64>:\n    let value: Int64\n"
  "Generic type parameter conflicts with a builtin type")
reject(unknown_field "struct Box<T>:\n    let value: Unknown\n"
  "Unknown or unsupported type")
reject(missing_annotation "struct Box<T>:\n    let value: T\nlet item: Box = Box<Int64>(1)\n"
  "Generic struct requires type arguments")
reject(unbound_constructor "struct Phantom<T>:\n    let fixed: Int64\nlet item = Phantom(1)\n"
  "Cannot infer every generic struct type parameter")
reject(excess "struct Box<T>:\n    let value: T\nlet item = Box<Int64, Float64>(1)\n"
  "Too many generic struct type arguments")
reject(few "struct Pair<A, B>:\n    let first: A\n    let second: B\nlet item = Pair<Int64>(1, 2)\n"
  "Missing generic struct type arguments")
reject(nongeneric "struct Plain:\n    let value: Int64\nlet item: Plain<Int64> = Plain(1)\n"
  "Type arguments require a generic struct")
reject(mismatch "struct Box<T>:\n    let value: T\nlet item = Box<Int64>(true)\n"
  "Expression type does not match required type")
reject(nominal "struct Box<T>:\n    let value: T\nlet item: Box<Float64> = Box<Int64>(1)\n"
  "Expression type does not match required type")
reject(infinite "struct Loop<T>:\n    let next: Loop<T>\nlet item: Loop<Int64> = nil\n"
  "Recursive struct values have infinite size")
reject(polymorphic_recursion "struct Grow<T>:\n    let next: Grow<[T]>?\nlet item: Grow<Int64> = nil\n"
  "Semantic nesting limit exceeded|Generic specialization limit exceeded|Generic struct specialization exceeds compiler capacity")
reject(method_shadow "struct Box<T>:\n    let value: T\n    func echo<T>(item: T) -> T:\n        return item\n"
  "Method type parameter shadows a struct type parameter")
reject(method_unknown "struct Box<T>:\n    let value: T\n    func echo<U>(item: Unknown) -> U:\n        return item\n"
  "Unknown or unsupported type")
reject(method_duplicate "struct Box<T>:\n    let value: T\n    func echo<U, U>(item: U) -> U:\n        return item\n"
  "Duplicate generic type parameter")
reject(constraint_unknown "struct Box<T: Unknown>:\n    let value: T\n"
  "Unknown generic constraint")
reject(constraint_explicit "struct Box<T: Scalar>:\n    let value: T\nlet item = Box<[Int64]>([1])\n"
  "Type argument must satisfy Scalar constraint")
reject(constraint_inferred "struct Box<T: Scalar>:\n    let value: T\nlet item = Box([1])\n"
  "Type argument must satisfy Scalar constraint")
reject(method_constraint "struct Box<T>:\n    let value: T\n    func echo<U: Numeric & Equatable>(item: U) -> U:\n        return item\nlet item = Box(true).echo(false)\n"
  "Type argument must satisfy Numeric constraint")

file(WRITE "${WORK}/BadBox.phys"
  "struct Box<T>:\n    let value: T\n    func invalid() -> T:\n        return self.value + true\n")
file(WRITE "${WORK}/Main.phys"
  "import BadBox\nlet item = BadBox.Box<Int64>(1)\nlet value = item.invalid()\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT output STREQUAL "" OR
   NOT diagnostic MATCHES "BadBox\\.phys:4:[0-9]+: error:")
  message(FATAL_ERROR "Generic struct method diagnostic lost its source: ${result}\n${output}\n${diagnostic}")
endif()
