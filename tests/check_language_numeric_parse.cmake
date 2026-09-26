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
    message(FATAL_ERROR "Invalid numeric parse ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(missing "let x = Int64.parse()\n" "Numeric parse expects one String argument")
reject(extra "let x = Float64.parse(\"1\", \"2\")\n" "Numeric parse expects one String argument")
reject(wrong_label "let x = Int64.parse(value: \"1\")\n" "Numeric parse expects one String argument")
reject(wrong_type "let x = Float64.parse(1)\n" "Expression type does not match required type")
reject(type_arguments "let x = Int64.parse<Int64>(\"1\")\n"
  "Explicit type arguments require a generic function or method")
