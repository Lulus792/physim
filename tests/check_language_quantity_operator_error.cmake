if(NOT DEFINED PROGRAM)
  message(FATAL_ERROR "Missing PROGRAM")
endif()
execute_process(COMMAND "${PROGRAM}" RESULT_VARIABLE result
  ERROR_VARIABLE diagnostic TIMEOUT 15)
if(result EQUAL 0 OR NOT diagnostic MATCHES
   "quantity_operator_error\\.phys:5:[0-9]+: runtime error: Invalid quantity operation")
  message(FATAL_ERROR "Dynamic dimension mismatch was not diagnosed at the operator: ${result}\n${diagnostic}")
endif()
