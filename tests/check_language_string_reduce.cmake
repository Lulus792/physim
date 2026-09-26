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

set(sum "func sum(total: Int64, value: String) -> Int64:\n    return total + value.utf8.count\n")
reject(non_sequence "let value = 1\nlet result = value.reduce(0, 1)\n"
  "reduce requires an Array or String value")
reject(missing_combine "let result = \"abc\".reduce(0)\n"
  "String reduce expects initial value and positional combine function")
reject(named "${sum}let result = \"abc\".reduce(initial: 0, combine: sum)\n"
  "String reduce expects initial value and positional combine function")
reject(extra "${sum}let result = \"abc\".reduce(0, sum, sum)\n"
  "String reduce expects initial value and positional combine function")
reject(non_function "let result = \"abc\".reduce(0, 1)\n"
  "String reduce combine must be func")
reject(wrong_element
  "func combine(total: Int64, value: Int64) -> Int64:\n    return total\nlet result = \"abc\".reduce(0, combine)\n"
  "String reduce combine must be func")
reject(wrong_accumulator
  "func combine(total: String, value: String) -> Int64:\n    return 1\nlet result = \"abc\".reduce(0, combine)\n"
  "String reduce combine must be func")
reject(wrong_arity "func combine(value: String) -> Int64:\n    return 0\nlet result = \"abc\".reduce(0, combine)\n"
  "String reduce combine must be func")
reject(void_result "func combine(total: Int64, value: String):\n    print(value)\nlet result = \"abc\".reduce(0, combine)\n"
  "String reduce combine must be func")
reject(wrong_initial "${sum}let result = \"abc\".reduce(\"bad\", sum)\n"
  "Expression type does not match required type")
reject(explicit_type "${sum}let result = \"abc\".reduce<Int64>(0, sum)\n"
  "Explicit type arguments require a generic function or method")
