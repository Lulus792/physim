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

set(sum "func sum(total: Int64, value: Int64) -> Int64:\n    return total + value\n")
reject(non_array "let x = 1\nlet y = x.reduce(0, 1)\n"
  "reduce requires an Array or String value")
reject(missing_combine "let x = [1]\nlet y = x.reduce(0)\n"
  "Array reduce expects initial value and positional combine function")
reject(named_arguments "${sum}let x = [1]\nlet y = x.reduce(initial: 0, combine: sum)\n"
  "Array reduce expects initial value and positional combine function")
reject(non_function "let x = [1]\nlet y = x.reduce(0, 1)\n"
  "Array reduce combine must be func")
reject(wrong_element
  "func combine(total: Int64, value: String) -> Int64:\n    return total\nlet x = [1]\nlet y = x.reduce(0, combine)\n"
  "Array reduce combine must be func")
reject(wrong_accumulator
  "func combine(total: String, value: Int64) -> Int64:\n    return value\nlet x = [1]\nlet y = x.reduce(0, combine)\n"
  "Array reduce combine must be func")
reject(wrong_arity "func combine(value: Int64) -> Int64:\n    return value\nlet x = [1]\nlet y = x.reduce(0, combine)\n"
  "Array reduce combine must be func")
reject(void_result "func combine(total: Int64, value: Int64):\n    print(value)\nlet x = [1]\nlet y = x.reduce(0, combine)\n"
  "Array reduce combine must be func")
reject(wrong_initial "${sum}let x = [1]\nlet y = x.reduce(\"bad\", sum)\n"
  "Expression type does not match required type")
