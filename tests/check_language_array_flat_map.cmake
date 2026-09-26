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

reject(non_array "let x = 1\nlet y = x.flatMap(1)\n"
  "Array flatMap requires an array value")
reject(no_transform "let x = [1]\nlet y = x.flatMap()\n"
  "Array flatMap expects one positional transform")
reject(named_transform "func change(x: Int64) -> [Int64]:\n    return [x]\nlet a = [1]\nlet b = a.flatMap(transform: change)\n"
  "Array flatMap expects one positional transform")
reject(extra_transform "func change(x: Int64) -> [Int64]:\n    return [x]\nlet a = [1]\nlet b = a.flatMap(change, change)\n"
  "Array flatMap expects one positional transform")
reject(non_function "let x = [1]\nlet y = x.flatMap(2)\n"
  "Array flatMap transform must be func")
reject(wrong_element "func change(x: String) -> [String]:\n    return [x]\nlet a = [1]\nlet b = a.flatMap(change)\n"
  "Array flatMap transform must be func")
reject(wrong_arity "func change(x: Int64, y: Int64) -> [Int64]:\n    return [x]\nlet a = [1]\nlet b = a.flatMap(change)\n"
  "Array flatMap transform must be func")
reject(nonarray_result "func change(x: Int64) -> Int64:\n    return x\nlet a = [1]\nlet b = a.flatMap(change)\n"
  "Array flatMap transform must be func")
reject(optional_result "func change(x: Int64) -> Int64?:\n    return nil\nlet a = [1]\nlet b = a.flatMap(change)\n"
  "Array flatMap transform must be func")
reject(void_result "func change(x: Int64):\n    print(x)\nlet a = [1]\nlet b = a.flatMap(change)\n"
  "Array flatMap transform must be func")
reject(explicit_type "func change(x: Int64) -> [Int64]:\n    return [x]\nlet a = [1]\nlet b = a.flatMap<Int64>(change)\n"
  "Explicit type arguments require a generic function or method")
