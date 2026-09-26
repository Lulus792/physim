foreach(required PROGRAM SOURCE EXPECT)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
execute_process(COMMAND "${PROGRAM}" RESULT_VARIABLE result
  ERROR_VARIABLE diagnostic TIMEOUT 15)
if(result EQUAL 0 OR NOT diagnostic MATCHES
   "${SOURCE}\\.phys:[0-9]+:[0-9]+: runtime error: ${EXPECT}")
  message(FATAL_ERROR "Medium runtime error was not diagnosed: ${result}\n${diagnostic}")
endif()
