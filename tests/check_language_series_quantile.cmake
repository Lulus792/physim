foreach(required RUNNER MODULE ERROR_MODULE COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
file(REMOVE "${WORK}/valid.psreport" "${WORK}/valid.inputs.csv"
  "${WORK}/invalid.psreport" "${WORK}/invalid.inputs.csv")

execute_process(COMMAND "${RUNNER}" "${MODULE}" --runs "${WORK}/valid"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 30)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "" OR
   NOT EXISTS "${WORK}/valid.psreport")
  message(FATAL_ERROR "Series quantile failed: ${result}\n${diagnostic}")
endif()

execute_process(COMMAND "${RUNNER}" "${ERROR_MODULE}" --runs "${WORK}/invalid"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 30)
if(result EQUAL 0 OR NOT diagnostic MATCHES
   "analysis_series_quantile_error\\.phys:5:[0-9]+: runtime error: Invalid argument")
  message(FATAL_ERROR "Invalid probability was not rejected: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/wrong.phys" "func analyze():\n    let values = Series.fromValues([1.0],Unit(0,0,0,0,0,0,0,1,\"1\"),\"x\")\n    let read = values.quantile(\"half\")\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/wrong.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Expression type does not match required type")
  message(FATAL_ERROR "Nonfloating probability was not rejected: ${result}\n${diagnostic}")
endif()
