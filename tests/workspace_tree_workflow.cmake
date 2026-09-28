string(TIMESTAMP stamp "%Y%m%d-%H%M%S")
string(RANDOM LENGTH 8 nonce)
set(directory "${ROOT}-${stamp}-${nonce} ä")
execute_process(COMMAND "${APP}" --workspace-state-test "${directory}" tree
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors TIMEOUT 25)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Workspace tree: ${result}\n${output}\n${errors}")
endif()
message(STATUS "Workspace tree expansion, nested preview, refresh and added roots passed")
