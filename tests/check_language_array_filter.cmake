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

reject(no_predicate "let a = [1]\nlet b = a.filter()\n"
  "Array filter expects one positional predicate")
reject(named_predicate "func keep(x: Int64) -> Bool:\n    return true\nlet a = [1]\nlet b = a.filter(predicate: keep)\n"
  "Array filter expects one positional predicate")
reject(non_array "let a = 1\nlet b = a.filter(1)\n"
  "Array filter requires an array value")
reject(non_function "let a = [1]\nlet b = a.filter(1)\n"
  "Array filter predicate must be func")
reject(wrong_element "func keep(x: String) -> Bool:\n    return true\nlet a = [1]\nlet b = a.filter(keep)\n"
  "Array filter predicate must be func")
reject(wrong_result "func keep(x: Int64) -> Int64:\n    return x\nlet a = [1]\nlet b = a.filter(keep)\n"
  "Array filter predicate must be func")
reject(wrong_arity "func keep(x: Int64, y: Int64) -> Bool:\n    return true\nlet a = [1]\nlet b = a.filter(keep)\n"
  "Array filter predicate must be func")
