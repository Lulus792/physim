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

reject(first_write "var value = \"abc\"\nvalue.first = Optional.some(\"z\")\n"
  "Assignment requires a mutable var binding")
reject(last_write "var value = \"abc\"\nvalue.last = Optional.some(\"z\")\n"
  "Assignment requires a mutable var binding")
reject(first_call "let value = \"abc\"\nlet letter = value.first()\n"
  "Unknown method for this receiver type")
reject(last_call "let value = \"abc\"\nlet letter = value.last()\n"
  "Unknown method for this receiver type")
reject(wrong_type "let value: Int64? = \"abc\".first\n"
  "Expression type does not match required type")
