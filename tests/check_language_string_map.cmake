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

reject(non_sequence "let value = 1\nlet result = value.map(1)\n"
  "Array map requires an array value")
reject(missing "let result = \"abc\".map()\n"
  "String map expects one positional transform")
reject(named "func change(value: String) -> String:\n    return value\nlet result = \"abc\".map(transform: change)\n"
  "String map expects one positional transform")
reject(extra "func change(value: String) -> String:\n    return value\nlet result = \"abc\".map(change, change)\n"
  "String map expects one positional transform")
reject(non_function "let result = \"abc\".map(1)\n"
  "String map transform must be func")
reject(wrong_element "func change(value: Int64) -> String:\n    return String(value)\nlet result = \"abc\".map(change)\n"
  "String map transform must be func")
reject(wrong_arity "func change(a: String, b: String) -> String:\n    return a\nlet result = \"abc\".map(change)\n"
  "String map transform must be func")
reject(void_result "func change(value: String):\n    print(value)\nlet result = \"abc\".map(change)\n"
  "String map transform must be func")
reject(explicit_type "func change(value: String) -> String:\n    return value\nlet result = \"abc\".map<String>(change)\n"
  "Explicit type arguments require a generic function or method")
