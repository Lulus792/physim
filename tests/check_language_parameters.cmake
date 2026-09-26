foreach(required EXPERIMENT RUNNER VERIFY WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
execute_process(COMMAND "${RUNNER}" "${EXPERIMENT}" --describe
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
if(NOT result EQUAL 0 OR NOT output MATCHES
   "PHYSIM_PARAMETERS_1\n1\ninitialSpeed\t2\t-10\t10\tInitial speed\n")
  message(FATAL_ERROR "Parameter discovery failed: ${result}\n${output}\n${diagnostic}")
endif()
foreach(value 2 4.5)
  set(run "${WORK}/parameter-${value}.psrun")
  file(REMOVE "${run}")
  if(value STREQUAL "2")
    execute_process(COMMAND "${RUNNER}" "${EXPERIMENT}" "${run}" --steps 1 --dt 0.005
      RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
  else()
    execute_process(COMMAND "${RUNNER}" "${EXPERIMENT}" "${run}" --steps 1 --dt 0.005
      --param initialSpeed=${value}
      RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
  endif()
  if(NOT result EQUAL 0 OR NOT EXISTS "${run}" OR NOT output MATCHES "2 samples written")
    message(FATAL_ERROR "Parameter run failed: ${result}\n${output}\n${diagnostic}")
  endif()
  execute_process(COMMAND "${VERIFY}" "${run}" "${value}"
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Parameter verification failed: ${result}\n${output}\n${diagnostic}")
  endif()
endforeach()
foreach(invalid "missing=1" "initialSpeed=11" "initialSpeed=nan" "1bad=3")
  set(run "${WORK}/invalid.psrun")
  file(REMOVE "${run}")
  execute_process(COMMAND "${RUNNER}" "${EXPERIMENT}" "${run}" --steps 1 --param ${invalid}
    RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
  if(result EQUAL 0 OR EXISTS "${run}")
    message(FATAL_ERROR "Invalid override was accepted: ${invalid}")
  endif()
endforeach()
execute_process(COMMAND "${RUNNER}" "${EXPERIMENT}" "${WORK}/invalid.psrun" --steps 1
  --param initialSpeed=3 --param initialSpeed=4
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE diagnostic TIMEOUT 20)
if(result EQUAL 0 OR EXISTS "${WORK}/invalid.psrun")
  message(FATAL_ERROR "Duplicate override was accepted")
endif()
