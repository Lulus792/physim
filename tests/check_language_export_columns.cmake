foreach(required RUNNER MODULE ERROR_MODULE UNALIGNED_MODULE COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
file(REMOVE "${WORK}/report-three.csv" "${WORK}/report.psreport"
  "${WORK}/report.inputs.csv" "${WORK}/invalid.psreport"
  "${WORK}/invalid.inputs.csv" "${WORK}/invalid-empty.csv"
  "${WORK}/unaligned.psreport" "${WORK}/unaligned.inputs.csv"
  "${WORK}/unaligned-unaligned.csv")

execute_process(COMMAND "${RUNNER}" "${MODULE}" --runs "${WORK}/report"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 30)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "")
  message(FATAL_ERROR "Analysis export failed: ${result}\n${diagnostic}")
endif()
if(NOT EXISTS "${WORK}/report-three.csv" OR NOT EXISTS "${WORK}/report.psreport")
  message(FATAL_ERROR "Analysis output is missing")
endif()
file(READ "${WORK}/report-three.csv" csv)
string(REPLACE "\r\n" "\n" csv "${csv}")
if(NOT csv STREQUAL
   "\"time [s]\",\"distance [m]\",\"speed [m s^-1]\"\n0,0,1\n1,2,2\n2,5,3\n")
  message(FATAL_ERROR "Three-column CSV content is wrong:\n${csv}")
endif()

execute_process(COMMAND "${RUNNER}" "${ERROR_MODULE}" --runs "${WORK}/invalid"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 30)
if(result EQUAL 0 OR NOT diagnostic MATCHES
   "analysis_export_columns_error\\.phys:3:[0-9]+: runtime error: Invalid argument")
  message(FATAL_ERROR "Empty export was not rejected: ${result}\n${diagnostic}")
endif()
if(EXISTS "${WORK}/invalid-empty.csv")
  message(FATAL_ERROR "Rejected export created a CSV file")
endif()

execute_process(COMMAND "${RUNNER}" "${UNALIGNED_MODULE}" --runs "${WORK}/unaligned"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 30)
if(result EQUAL 0 OR NOT diagnostic MATCHES
   "analysis_export_columns_unaligned\\.phys:6:[0-9]+: runtime error: Invalid argument")
  message(FATAL_ERROR "Unaligned export was not rejected: ${result}\n${diagnostic}")
endif()
if(EXISTS "${WORK}/unaligned-unaligned.csv")
  message(FATAL_ERROR "Rejected unaligned export created a CSV file")
endif()

file(WRITE "${WORK}/wrong.phys" "func analyze():\n    Series.exportColumns([1],\"bad\")\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/wrong.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Expression type does not match required type")
  message(FATAL_ERROR "Wrong column type was not rejected: ${result}\n${diagnostic}")
endif()
