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

reject(non_sequence "let value = 1\nlet result = value.first(where: 1)\n"
  "Array predicate query requires an array value")
reject(wrong_label "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".last(predicate: keep)\n"
  "Unknown method")
reject(extra "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".first(where: keep, where: keep)\n"
  "String predicate search expects where: predicate")
reject(non_function "let result = \"abc\".first(where: 1)\n"
  "String predicate query requires func")
reject(wrong_element "func keep(value: Int64) -> Bool:\n    return true\nlet result = \"abc\".lastIndex(where: keep)\n"
  "String predicate query requires func")
reject(wrong_arity "func keep(a: String, b: String) -> Bool:\n    return true\nlet result = \"abc\".firstIndex(where: keep)\n"
  "String predicate query requires func")
reject(wrong_result "func keep(value: String) -> String:\n    return value\nlet result = \"abc\".last(where: keep)\n"
  "String predicate query requires func")
reject(explicit_type "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".first<String>(where: keep)\n"
  "Explicit type arguments require a generic function or method")
