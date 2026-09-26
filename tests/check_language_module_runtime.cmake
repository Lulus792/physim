if(NOT DEFINED PROGRAM)
  message(FATAL_ERROR "Missing PROGRAM")
endif()
execute_process(COMMAND "${PROGRAM}"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 70 OR NOT diagnostic MATCHES "Failure\\.phys:2:14: runtime error: Division by zero")
  message(FATAL_ERROR "Imported runtime error lost its source: ${result}\n${diagnostic}")
endif()
