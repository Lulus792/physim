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
    message(FATAL_ERROR "Invalid swapAt ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(immutable "let a = [1, 2]\na.swapAt(0, 1)\n"
  "Array swapAt requires a mutable var binding")
reject(nonarray "var a = 1\na.swapAt(0, 0)\n"
  "Array swapAt requires an array value")
reject(missing "var a = [1, 2]\na.swapAt(0)\n"
  "Array swapAt expects two positional indices")
reject(extra "var a = [1, 2]\na.swapAt(0, 1, 2)\n"
  "Array swapAt expects two positional indices")
reject(label "var a = [1, 2]\na.swapAt(first: 0, 1)\n"
  "Array swapAt expects two positional indices")
reject(type_args "var a = [1, 2]\na.swapAt<Int64>(0, 1)\n"
  "Explicit type arguments require a generic function or method")
reject(type "var a = [1, 2]\na.swapAt(0, true)\n"
  "Expression type does not match required type")
