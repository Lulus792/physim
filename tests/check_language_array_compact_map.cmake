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

reject(non_array "let x = 1\nlet y = x.compactMap(1)\n"
  "Array compactMap requires an array value")
reject(no_transform "let x = [1]\nlet y = x.compactMap()\n"
  "Array compactMap expects one positional transform")
reject(named_transform "func change(x: Int64) -> Int64?:\n    return nil\nlet a = [1]\nlet b = a.compactMap(transform: change)\n"
  "Array compactMap expects one positional transform")
reject(extra_transform "func change(x: Int64) -> Int64?:\n    return nil\nlet a = [1]\nlet b = a.compactMap(change, change)\n"
  "Array compactMap expects one positional transform")
reject(non_function "let x = [1]\nlet y = x.compactMap(2)\n"
  "Array compactMap transform must be func")
reject(wrong_element "func change(x: String) -> String?:\n    return nil\nlet a = [1]\nlet b = a.compactMap(change)\n"
  "Array compactMap transform must be func")
reject(wrong_arity "func change(x: Int64, y: Int64) -> Int64?:\n    return nil\nlet a = [1]\nlet b = a.compactMap(change)\n"
  "Array compactMap transform must be func")
reject(nonoptional_result "func change(x: Int64) -> Int64:\n    return x\nlet a = [1]\nlet b = a.compactMap(change)\n"
  "Array compactMap transform must be func")
reject(void_result "func change(x: Int64):\n    print(x)\nlet a = [1]\nlet b = a.compactMap(change)\n"
  "Array compactMap transform must be func")
reject(explicit_type "func change(x: Int64) -> Int64?:\n    return nil\nlet a = [1]\nlet b = a.compactMap<Int64>(change)\n"
  "Explicit type arguments require a generic function or method")
