foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")

function(reject source expected)
  file(WRITE "${WORK}/Main.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "${expected}")
    message(FATAL_ERROR "Expected ${expected}: ${result}\n${diagnostic}")
  endif()
endfunction()

reject("enum E<T>:\n    case none\nlet value = E.none\n"
  "Generic enum case needs explicit type arguments or an expected type")
reject("enum E<T>:\n    case some(value: T)\nlet value = E.some(value: nil)\n"
  "Cannot infer every generic enum type parameter")
reject("enum E<T>:\n    case none\nlet value = E<Int64, String>.none\n"
  "Too many generic enum type arguments")
reject("enum E<T: Numeric>:\n    case some(value: T)\nlet value = E<String>.some(value: \"bad\")\n"
  "Type argument must satisfy Numeric constraint")
reject("enum E<T>:\n    case one(value: T)\n    case one(value: T)\n"
  "Duplicate enum case")
reject("enum E<T>:\n    case one(value: T, value: T)\n"
  "Duplicate enum payload field")
reject("enum E<T>:\n    case one = 1\n    case two = 1\n"
  "Duplicate enum raw value")
reject("enum E<T>:\n    case some(value: T)\nlet constructor = E.some\n"
  "Generic enum case needs explicit type arguments or an expected type")
reject("enum E<T>:\n    case some(value: T)\nlet constructor: func(String) -> E<Int64> = E<Int64>.some\n"
  "type does not match")
