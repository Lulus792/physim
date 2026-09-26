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

reject(non_array "let x = 1\nlet y = x.map(1)\n"
  "Array map requires an array value")
reject(no_transform "let x = [1]\nlet y = x.map()\n"
  "Array map expects one positional transform")
reject(named_transform "func change(x: Int64) -> Int64:\n    return x\nlet a = [1]\nlet b = a.map(transform: change)\n"
  "Array map expects one positional transform")
reject(non_function "let x = [1]\nlet y = x.map(2)\n"
  "Array map transform must be func")
reject(wrong_element "func change(x: String) -> String:\n    return x\nlet a = [1]\nlet b = a.map(change)\n"
  "Array map transform must be func")
reject(wrong_arity "func change(x: Int64, y: Int64) -> Int64:\n    return x\nlet a = [1]\nlet b = a.map(change)\n"
  "Array map transform must be func")
reject(void_result "func change(x: Int64):\n    print(x)\nlet a = [1]\nlet b = a.map(change)\n"
  "Array map transform must be func")
