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

set(number_action "func use(value: Int64):\n    print(value)\n")
set(string_action "func use(value: String):\n    print(value)\n")
reject(non_sequence "1.forEach(1)\n"
  "forEach requires an Array or String value")
reject(array_missing "let values = [1]\nvalues.forEach()\n"
  "Array forEach expects one positional function")
reject(string_named "${string_action}\"a\".forEach(body: use)\n"
  "String forEach expects one positional function")
reject(array_extra "${number_action}let values = [1]\nvalues.forEach(use, use)\n"
  "Array forEach expects one positional function")
reject(string_non_function "\"a\".forEach(1)\n"
  "String forEach body must be func")
reject(array_wrong_element "${string_action}let values = [1]\nvalues.forEach(use)\n"
  "Array forEach body must be func")
reject(string_wrong_element "${number_action}\"a\".forEach(use)\n"
  "String forEach body must be func")
reject(array_wrong_result
  "func use(value: Int64) -> Int64:\n    return value\nlet values = [1]\nvalues.forEach(use)\n"
  "Array forEach body must be func")
reject(string_wrong_arity
  "func use(a: String, b: String):\n    print(a)\n\"a\".forEach(use)\n"
  "String forEach body must be func")
reject(explicit_type "${number_action}let values = [1]\nvalues.forEach<Int64>(use)\n"
  "Explicit type arguments require a generic function or method")
