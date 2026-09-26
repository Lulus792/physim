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

set(overloads
  "func describe(value: Int64) -> String:\n    return String(value)\nfunc describe(value: String) -> String:\n    return value\n")
reject(duplicate_signature
  "func choose(value: Int64) -> Int64:\n    return value\nfunc choose(value: Int64) -> String:\n    return String(value)\n"
  "Duplicate free function signature")
reject(no_match "${overloads}let x = describe(true)\n"
  "No matching free function overload")
reject(ambiguous_positional
  "func choose(left: Int64) -> Int64:\n    return left\nfunc choose(right: Int64) -> Int64:\n    return right\nlet x = choose(1)\n"
  "Ambiguous free function overload")
reject(ambiguous_nil
  "func choose(value: Int64?) -> Int64:\n    return 1\nfunc choose(value: String?) -> Int64:\n    return 2\nlet x = choose(nil)\n"
  "Ambiguous free function overload")
reject(untyped_value "${overloads}let f = describe\n"
  "Overloaded function value requires an expected function type")
reject(wrong_function_type
  "${overloads}let f: func(Bool) -> String = describe\n"
  "No free function overload matches the required function type")
reject(ambiguous_function_argument
  "${overloads}func apply(function: func(Int64) -> String) -> String:\n    return function(1)\nfunc apply(function: func(String) -> String) -> String:\n    return function(\"a\")\nlet x = apply(describe)\n"
  "Ambiguous free function overload")
