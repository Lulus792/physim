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
    message(FATAL_ERROR "Invalid attempt ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(missing "let x = attempt()\n" "attempt expects exactly one positional value expression")
reject(extra "let x = attempt(1, 2)\n" "attempt expects exactly one positional value expression")
reject(labeled "let x = attempt(value: 1)\n" "attempt expects exactly one positional value expression")
reject(void_value "let x = attempt(print(1))\n" "attempt requires a value expression")
reject(type_arguments "let x = attempt<Int64>(1)\n"
  "Explicit type arguments require a generic function or method")
