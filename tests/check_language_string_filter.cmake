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

reject(non_sequence "let value = 1\nlet result = value.filter(1)\n"
  "Array filter requires an array value")
reject(missing "let value = \"abc\"\nlet result = value.filter()\n"
  "String filter expects one positional predicate")
reject(named "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".filter(where: keep)\n"
  "String filter expects one positional predicate")
reject(extra "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".filter(keep, keep)\n"
  "String filter expects one positional predicate")
reject(non_function "let result = \"abc\".filter(1)\n"
  "String filter predicate must be func")
reject(wrong_element "func keep(value: Int64) -> Bool:\n    return true\nlet result = \"abc\".filter(keep)\n"
  "String filter predicate must be func")
reject(wrong_arity "func keep(a: String, b: String) -> Bool:\n    return true\nlet result = \"abc\".filter(keep)\n"
  "String filter predicate must be func")
reject(wrong_result "func keep(value: String) -> String:\n    return value\nlet result = \"abc\".filter(keep)\n"
  "String filter predicate must be func")
reject(explicit_type "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".filter<String>(keep)\n"
  "Explicit type arguments require a generic function or method")
