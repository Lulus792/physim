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

reject(non_array "let x = 1\nlet y = x.min()\n" "Array extrema require")
reject(unsupported_element "let x = [Vec2(1,2)]\nlet y = x.max()\n"
  "Array extrema require")
reject(wrong_label "func less(a: Int64, b: Int64) -> Bool:\n    return a < b\nlet x = [2,1]\nlet y = x.min(using: less)\n"
  "Array extrema expect by: comparator")
reject(extra_comparator "func less(a: Int64, b: Int64) -> Bool:\n    return a < b\nlet x = [2,1]\nlet y = x.max(by: less, by: less)\n"
  "Array extrema expect by: comparator")
reject(non_function "let x = [2,1]\nlet y = x.min(by: true)\n"
  "Array comparator must be func")
reject(wrong_result "func key(a: Int64, b: Int64) -> Int64:\n    return a\nlet x = [2,1]\nlet y = x.max(by: key)\n"
  "Array comparator must be func")
reject(wrong_element "func less(a: String, b: String) -> Bool:\n    return a == b\nlet x = [2,1]\nlet y = x.min(by: less)\n"
  "Array comparator must be func")
