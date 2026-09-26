if(NOT DEFINED PROGRAM)
  message(FATAL_ERROR "Missing PROGRAM")
endif()
execute_process(COMMAND "${PROGRAM}" RESULT_VARIABLE result
  ERROR_VARIABLE diagnostic TIMEOUT 15)
if(result EQUAL 0 OR NOT diagnostic MATCHES
   "quantity_scalar_error\\.phys:2:[0-9]+: runtime error: Division by zero")
  message(FATAL_ERROR "Scalar division by zero was not diagnosed at the operator: ${result}\n${diagnostic}")
endif()
