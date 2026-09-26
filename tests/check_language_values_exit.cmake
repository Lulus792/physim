execute_process(COMMAND "${PROGRAM}" RESULT_VARIABLE result OUTPUT_VARIABLE output
  ERROR_VARIABLE error TIMEOUT 10)
string(REPLACE "\r\n" "\n" output "${output}")
if(NOT result EQUAL 70 OR NOT output STREQUAL "standalone owners released\n" OR
   NOT error MATCHES "array-exit.phys:8:4: runtime error: owned array exit probe")
  message(FATAL_ERROR "Standalone ownership cleanup failed: ${result}\n${output}\n${error}")
endif()
