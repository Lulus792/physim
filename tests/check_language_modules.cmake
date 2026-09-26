foreach(required COMPILER WORK EXAMPLE)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")

execute_process(COMMAND "${COMPILER}" --deps "${EXAMPLE}"
  RESULT_VARIABLE result OUTPUT_VARIABLE deps ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "")
  message(FATAL_ERROR "Module dependency listing failed: ${result}\n${diagnostic}")
endif()
foreach(name main Shared Left Right Scale)
  string(REGEX MATCHALL "[/\\]${name}\\.phys" matches "${deps}")
  list(LENGTH matches count)
  if(NOT count EQUAL 1)
    message(FATAL_ERROR "Expected ${name}.phys exactly once in module dependencies: ${deps}")
  endif()
endforeach()
string(REGEX MATCHALL "[/\\]Value\\.phys" value_matches "${deps}")
list(LENGTH value_matches value_count)
if(NOT value_count EQUAL 1)
  message(FATAL_ERROR "Qualified and transitive imports must share Value.phys: ${deps}")
endif()

file(WRITE "${WORK}/Main.phys" "import Dependency\nlet x = Dependency.value\n")
file(WRITE "${WORK}/Dependency.phys" "let value = missing\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Dependency\\.phys:1:13: error: Unknown name")
  message(FATAL_ERROR "Imported semantic diagnostic lost its source: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/Dependency.phys" "func broken(:\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Dependency\\.phys:1:[0-9]+: error:")
  message(FATAL_ERROR "Imported syntax diagnostic lost its source: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/Main.phys" "import Missing\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 2 OR NOT diagnostic MATCHES "Main\\.phys:1:8: error: Cannot open imported module")
  message(FATAL_ERROR "Missing import diagnostic is incorrect: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/Main.phys" "import Dependency\n")
file(WRITE "${WORK}/Dependency.phys" "import Main\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Dependency\\.phys:1:8: error: Cyclic module import")
  message(FATAL_ERROR "Import cycle diagnostic is incorrect: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/Main.phys" "import Folder.Missing\n")
file(MAKE_DIRECTORY "${WORK}/Folder")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 2 OR NOT diagnostic MATCHES "Main\\.phys:1:15: error: Cannot open imported module")
  message(FATAL_ERROR "Missing qualified import diagnostic is incorrect: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/Main.phys" "import Folder.\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Expected module name after")
  message(FATAL_ERROR "Malformed qualified import was not rejected: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/Main.phys" "import Folder.One as Left\nimport Other.One as Right\nlet sum = Left.value + Right.value\n")
file(MAKE_DIRECTORY "${WORK}/Other")
file(WRITE "${WORK}/Folder/One.phys" "let value = 1\n")
file(WRITE "${WORK}/Other/One.phys" "let value = 2\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "")
  message(FATAL_ERROR "Qualified imports with distinct aliases failed: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/Main.phys" "import Folder.A\n")
file(WRITE "${WORK}/Folder/A.phys" "import B\n")
file(WRITE "${WORK}/Folder/B.phys" "import A\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/Main.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "B\\.phys:1:8: error: Cyclic module import")
  message(FATAL_ERROR "Nested import cycle diagnostic is incorrect: ${result}\n${diagnostic}")
endif()
