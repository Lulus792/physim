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
    message(FATAL_ERROR "Invalid guard ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(missing_else "func f() -> Int64:\n    guard true:\n        return 1\n"
  "Expected 'else' after guard condition")
reject(non_return "func f() -> Int64:\n    guard true else:\n        print(1)\n    return 2\n"
  "Guard else must return on every path")
reject(non_bool "func f() -> Int64:\n    guard 1 else:\n        return 0\n    return 2\n"
  "Expression type does not match required type")
reject(non_optional "func f() -> Int64:\n    guard let x = 1 else:\n        return 0\n    return x\n"
  "Conditional binding requires an optional value")
reject(outside_function "guard true else:\n    print(1)\n"
  "Guard is only allowed inside a function")
reject(binding_in_else "func f(value: Int64?) -> Int64:\n    guard let x = value else:\n        print(x)\n        return 0\n    return x\n"
  "Unknown name or use before declaration")
reject(incomplete_return "func f(flag: Bool) -> Int64:\n    guard flag else:\n        if flag:\n            return 0\n    return 1\n"
  "Guard else must return on every path")
reject(escape "func f() -> Int64:\n    while true:\n        guard false else:\n            if true:\n                break\n            return 0\n        return 1\n    return 2\n"
  "Guard else cannot break or continue the enclosing flow")
