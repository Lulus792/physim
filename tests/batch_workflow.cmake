string(TIMESTAMP stamp "%Y%m%d-%H%M%S")
string(RANDOM LENGTH 8 nonce)
execute_process(COMMAND "${APP}" --batch-test "${ROOT}-${stamp}-${nonce} ä"
  RESULT_VARIABLE result TIMEOUT 160)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Batch app workflow failed: ${result}")
endif()
