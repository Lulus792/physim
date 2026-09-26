foreach(required APP ROOT MODE)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
if(NOT MODE STREQUAL "language_full" AND NOT MODE STREQUAL "language_mixed" AND
   NOT MODE STREQUAL "language_projectile" AND
   NOT MODE STREQUAL "language_sensors" AND NOT MODE STREQUAL "language_body" AND
   NOT MODE STREQUAL "language_contact" AND NOT MODE STREQUAL "language_joint" AND
   NOT MODE STREQUAL "language_graph" AND NOT MODE STREQUAL "language_sweep" AND
   NOT MODE STREQUAL "language_buoyancy" AND NOT MODE STREQUAL "language_spring" AND
   NOT MODE STREQUAL "language_collision" AND
   NOT MODE STREQUAL "language_box_collision")
  message(FATAL_ERROR "Unsupported language workflow mode: ${MODE}")
endif()
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef nonce)
set(project "${ROOT}/${MODE} ä ${nonce}")
# The app creates a fresh project; never remove or reuse an existing directory.
if(EXISTS "${project}")
  message(FATAL_ERROR "Language workflow project already exists")
endif()
file(MAKE_DIRECTORY "${ROOT}")
execute_process(COMMAND "${APP}" --self-test "${project}" "${MODE}"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE errors TIMEOUT 210)
if(NOT result EQUAL 0 OR NOT output MATCHES "APP SELF-TEST: PASSED")
  message(FATAL_ERROR "Language app workflow failed: ${result}\n${output}\n${errors}\nProject: ${project}")
endif()
file(READ "${project}/physim.project" description)
if(NOT MODE STREQUAL "language_mixed")
  set(experiment main.phys)
else()
  set(experiment main.c)
endif()
if(NOT description MATCHES "experiment=${experiment}" OR
   NOT description MATCHES "analysis=analysis.phys")
  message(FATAL_ERROR "Language selections were not preserved: ${description}")
endif()
set(artifacts "${experiment}" analysis.phys diagnostics.bmp editor.bmp simulation.bmp
              language-analysis.bmp language-analysis-editor.bmp)
if(MODE STREQUAL "language_sensors")
  list(APPEND artifacts language-statistics.bmp)
endif()
foreach(required IN LISTS artifacts)
  if(NOT EXISTS "${project}/${required}")
    message(FATAL_ERROR "Missing workflow artifact: ${project}/${required}")
  endif()
  file(SIZE "${project}/${required}" size)
  if(size EQUAL 0)
    message(FATAL_ERROR "Empty workflow artifact: ${project}/${required}")
  endif()
endforeach()
message(STATUS "${MODE}: diagnostics, native build, run controls, analysis, exports and reopen passed. Evidence: ${project}")
