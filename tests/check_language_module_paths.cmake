foreach(required COMPILER WORK SOURCE MODULE_ROOT)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")

execute_process(COMMAND "${COMPILER}" --deps --module-path "${MODULE_ROOT}" "${SOURCE}"
  RESULT_VARIABLE result OUTPUT_VARIABLE deps ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "")
  message(FATAL_ERROR "Module paths failed: ${result}\n${diagnostic}")
endif()
foreach(name main Helpers Scale)
  string(REGEX MATCHALL "[/\\]${name}\\.phys" matches "${deps}")
  list(LENGTH matches count)
  if(NOT count EQUAL 1)
    message(FATAL_ERROR "Expected ${name}.phys once: ${deps}")
  endif()
endforeach()

execute_process(COMMAND "${COMPILER}" --check "${SOURCE}"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 2 OR NOT diagnostic MATCHES "Cannot open imported module")
  message(FATAL_ERROR "Missing module path did not fail: ${result}\n${diagnostic}")
endif()

file(MAKE_DIRECTORY "${WORK}/Local")
file(WRITE "${WORK}/Local/Main.phys" "import Shared\nassert(Shared.value == 1)\n")
file(WRITE "${WORK}/Local/Shared.phys" "let value = 1\n")
file(WRITE "${WORK}/Shared.phys" "let value = 2\n")
execute_process(COMMAND "${COMPILER}" --check --module-path "${WORK}" "${WORK}/Local/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "")
  message(FATAL_ERROR "Local import precedence failed: ${result}\n${diagnostic}")
endif()
file(MAKE_DIRECTORY "${WORK}/First" "${WORK}/Second" "${WORK}/Client")
file(WRITE "${WORK}/Client/Main.phys" "import Shared\nassert(Shared.value == 3)\n")
file(WRITE "${WORK}/First/Shared.phys" "let value = 3\n")
file(WRITE "${WORK}/Second/Shared.phys" "let value = 4\n")
execute_process(COMMAND "${COMPILER}" --deps --module-path "${WORK}/First"
  --module-path "${WORK}/Second" "${WORK}/Client/Main.phys"
  RESULT_VARIABLE result OUTPUT_VARIABLE ordered_deps ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "" OR
   NOT ordered_deps MATCHES "First[/\\]Shared\\.phys" OR
   ordered_deps MATCHES "Second[/\\]Shared\\.phys")
  message(FATAL_ERROR "Module path order failed: ${result}\n${diagnostic}\n${ordered_deps}")
endif()

execute_process(COMMAND "${COMPILER}" --check --module-path "${MODULE_ROOT}" --module-path
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 2 OR NOT diagnostic MATCHES "Usage: physimc")
  message(FATAL_ERROR "Malformed module path options were accepted: ${result}\n${diagnostic}")
endif()
