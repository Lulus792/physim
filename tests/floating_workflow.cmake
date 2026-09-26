string(TIMESTAMP stamp "%Y%m%d-%H%M%S")
string(RANDOM LENGTH 8 nonce)
execute_process(COMMAND "${APP}" --self-test "${ROOT}-${stamp}-${nonce} ä" buoyancy
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors TIMEOUT 150)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Floating sphere app workflow failed: ${result}\n${output}\n${errors}")
endif()
message(STATUS "Floating sphere: template build, scene, pause/step, analysis, export and reopen passed")
