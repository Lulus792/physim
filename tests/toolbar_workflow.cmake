string(TIMESTAMP stamp "%Y%m%d-%H%M%S")
string(RANDOM LENGTH 8 nonce)
set(directory "${ROOT}-${stamp}-${nonce}")
execute_process(COMMAND "${APP}" --workspace-state-test "${directory}" toolbar
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors TIMEOUT 25)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Toolbar: ${result}\n${output}\n${errors}")
endif()
message(STATUS "Compact menus, disabled actions, workspace tabs, settings and keyboard navigation passed")
