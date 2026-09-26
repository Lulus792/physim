foreach(required EXPERIMENT ANALYSIS EXPERIMENT_RUNNER ANALYSIS_RUNNER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
file(REMOVE "${WORK}/run.psrun" "${WORK}/report.psreport" "${WORK}/report.inputs.csv")
execute_process(COMMAND "${EXPERIMENT_RUNNER}" "${EXPERIMENT}"
  "${WORK}/run.psrun" --steps 2 --dt 0.01
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
if(NOT result EQUAL 0 OR NOT EXISTS "${WORK}/run.psrun" OR
   NOT output MATCHES "3 samples written")
  message(FATAL_ERROR "Generic experiment failed: ${result}\n${output}\n${diagnostic}")
endif()
execute_process(COMMAND "${ANALYSIS_RUNNER}" "${ANALYSIS}" --runs "${WORK}/report"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
if(NOT result EQUAL 0 OR NOT EXISTS "${WORK}/report.psreport" OR
   NOT output MATCHES "Analysis: OK")
  message(FATAL_ERROR "Generic analysis failed: ${result}\n${output}\n${diagnostic}")
endif()
