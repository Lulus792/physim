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

reject(sort_immutable "var value = \"abc\"\nvalue.sort()\n"
  "String is immutable; use sorted")
reject(sorted_wrong_label "let value = \"abc\"\nlet result = value.sorted(using: 1)\n"
  "String sorting expects by: comparator")
reject(min_wrong_label "let value = \"abc\"\nlet result = value.min(using: 1)\n"
  "String extrema expect by: comparator")
reject(sorted_non_function "let value = \"abc\"\nlet result = value.sorted(by: 1)\n"
  "String comparator must be func")
reject(max_wrong_arity
  "func compare(value: String) -> Bool:\n    return true\nlet value = \"abc\"\nlet result = value.max(by: compare)\n"
  "String comparator must be func")
reject(min_wrong_element
  "func compare(a: Int64, b: Int64) -> Bool:\n    return true\nlet value = \"abc\"\nlet result = value.min(by: compare)\n"
  "String comparator must be func")
reject(sorted_wrong_result
  "func compare(a: String, b: String) -> Int64:\n    return 0\nlet value = \"abc\"\nlet result = value.sorted(by: compare)\n"
  "String comparator must be func")
reject(explicit_type "let value = \"abc\"\nlet result = value.sorted<String>()\n"
  "Explicit type arguments require a generic function or method")
