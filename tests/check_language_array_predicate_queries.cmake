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

reject(no_predicate "let a = [1]\nlet b = a.allSatisfy()\n"
  "Array predicate query expects one positional predicate")
reject(named_predicate "func test(x: Int64) -> Bool:\n    return true\nlet a = [1]\nlet b = a.allSatisfy(predicate: test)\n"
  "Array predicate query expects one positional predicate")
reject(non_array "let a = 1\nlet b = a.contains(where: 1)\n"
  "Array predicate query requires an array value")
reject(non_function "let a = [1]\nlet b = a.allSatisfy(1)\n"
  "Array predicate query requires func")
reject(wrong_element "func test(x: String) -> Bool:\n    return true\nlet a = [1]\nlet b = a.contains(where: test)\n"
  "Array predicate query requires func")
reject(wrong_result "func test(x: Int64) -> Int64:\n    return x\nlet a = [1]\nlet b = a.allSatisfy(test)\n"
  "Array predicate query requires func")
reject(wrong_arity "func test(x: Int64, y: Int64) -> Bool:\n    return true\nlet a = [1]\nlet b = a.contains(where: test)\n"
  "Array predicate query requires func")
reject(old_any_name "let a = [1]\nlet x = a.any(1)\n"
  "Unknown method for this receiver type")
reject(old_all_name "let a = [1]\nlet x = a.all(1)\n"
  "Unknown method for this receiver type")
