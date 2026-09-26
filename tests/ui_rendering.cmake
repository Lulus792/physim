string(TIMESTAMP stamp "%Y%m%d-%H%M%S")
string(RANDOM LENGTH 8 nonce)
execute_process(COMMAND "${PYTHON}" "${DRIVER}" "${BENCHMARK}"
  --output "${ROOT}-${stamp}-${nonce}" --smoke
  RESULT_VARIABLE result TIMEOUT 110)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "UI rendering workload failed: ${result}")
endif()
