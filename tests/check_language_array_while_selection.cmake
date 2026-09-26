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

reject(prefix_non_array "let value = 1\nlet result = value.prefix(while: 1)\n"
  "prefix\\(while:\\) requires an Array or String value")
reject(drop_non_array "let value = 1\nlet result = value.drop(while: 1)\n"
  "drop\\(while:\\) requires an Array or String value")
reject(prefix_missing "let values = [1]\nlet result = values.prefix()\n"
  "prefix and suffix require one positional count")
reject(drop_missing "let values = [1]\nlet result = values.drop()\n"
  "Array drop expects while: predicate")
reject(drop_wrong_label "func keep(value: Int64) -> Bool:\n    return true\nlet values = [1]\nlet result = values.drop(where: keep)\n"
  "Array drop expects while: predicate")
reject(prefix_extra "func keep(value: Int64) -> Bool:\n    return true\nlet values = [1]\nlet result = values.prefix(while: keep, while: keep)\n"
  "Array prefix expects while: predicate")
reject(drop_non_function "let values = [1]\nlet result = values.drop(while: 1)\n"
  "Array while predicate must be func")
reject(prefix_wrong_element "func keep(value: String) -> Bool:\n    return true\nlet values = [1]\nlet result = values.prefix(while: keep)\n"
  "Array while predicate must be func")
reject(drop_wrong_arity "func keep(a: Int64, b: Int64) -> Bool:\n    return true\nlet values = [1]\nlet result = values.drop(while: keep)\n"
  "Array while predicate must be func")
reject(prefix_wrong_result "func keep(value: Int64) -> Int64:\n    return value\nlet values = [1]\nlet result = values.prefix(while: keep)\n"
  "Array while predicate must be func")
reject(drop_explicit_type "func keep(value: Int64) -> Bool:\n    return true\nlet values = [1]\nlet result = values.drop<Int64>(while: keep)\n"
  "Explicit type arguments require a generic function or method")
