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

reject(prefix_non_sequence "let value = 1\nlet result = value.prefix(while: 1)\n"
  "prefix\\(while:\\) requires an Array or String value")
reject(drop_non_sequence "let value = 1\nlet result = value.drop(while: 1)\n"
  "drop\\(while:\\) requires an Array or String value")
reject(drop_missing "let value = \"abc\"\nlet result = value.drop()\n"
  "String drop expects while: predicate")
reject(drop_wrong_label "func keep(value: String) -> Bool:\n    return true\nlet value = \"abc\"\nlet result = value.drop(where: keep)\n"
  "String drop expects while: predicate")
reject(prefix_extra "func keep(value: String) -> Bool:\n    return true\nlet value = \"abc\"\nlet result = value.prefix(while: keep, while: keep)\n"
  "String prefix expects while: predicate")
reject(drop_non_function "let value = \"abc\"\nlet result = value.drop(while: 1)\n"
  "String while predicate must be func")
reject(prefix_wrong_element "func keep(value: Int64) -> Bool:\n    return true\nlet value = \"abc\"\nlet result = value.prefix(while: keep)\n"
  "String while predicate must be func")
reject(drop_wrong_arity "func keep(a: String, b: String) -> Bool:\n    return true\nlet value = \"abc\"\nlet result = value.drop(while: keep)\n"
  "String while predicate must be func")
reject(prefix_wrong_result "func keep(value: String) -> String:\n    return value\nlet text = \"abc\"\nlet result = text.prefix(while: keep)\n"
  "String while predicate must be func")
reject(drop_explicit_type "func keep(value: String) -> Bool:\n    return true\nlet text = \"abc\"\nlet result = text.drop<String>(while: keep)\n"
  "Explicit type arguments require a generic function or method")
