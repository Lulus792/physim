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

reject(non_sequence "let value = 1\nlet result = value.contains(where: 1)\n"
  "Array predicate query requires an array value")
reject(contains_missing "let result = \"abc\".contains(where: )\n"
  "Expected expression")
reject(contains_wrong_label "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".contains(predicate: keep)\n"
  "String contains expects exactly one positional argument")
reject(contains_extra "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".contains(where: keep, where: keep)\n"
  "String contains expects where: predicate")
reject(all_missing "let result = \"abc\".allSatisfy()\n"
  "String allSatisfy expects one positional predicate")
reject(all_named "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".allSatisfy(where: keep)\n"
  "String allSatisfy expects one positional predicate")
reject(contains_non_function "let result = \"abc\".contains(where: 1)\n"
  "String predicate query requires func")
reject(all_wrong_element "func keep(value: Int64) -> Bool:\n    return true\nlet result = \"abc\".allSatisfy(keep)\n"
  "String predicate query requires func")
reject(contains_wrong_arity "func keep(a: String, b: String) -> Bool:\n    return true\nlet result = \"abc\".contains(where: keep)\n"
  "String predicate query requires func")
reject(all_wrong_result "func keep(value: String) -> String:\n    return value\nlet result = \"abc\".allSatisfy(keep)\n"
  "String predicate query requires func")
reject(explicit_type "func keep(value: String) -> Bool:\n    return true\nlet result = \"abc\".contains<String>(where: keep)\n"
  "Explicit type arguments require a generic function or method")
