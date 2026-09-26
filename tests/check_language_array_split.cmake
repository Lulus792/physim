foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
function(reject name source expected)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "${expected}")
    message(FATAL_ERROR "${name} was not rejected: ${result}\n${diagnostic}")
  endif()
endfunction()
reject(missing "let values = [1, 0]\nlet parts = values.split()\n"
  "Array split expects separator:")
reject(label "let values = [1, 0]\nlet parts = values.split(by: 0)\n"
  "Array split expects separator:")
reject(element "let values = [1, 0]\nlet parts = values.split(separator: \"0\")\n"
  "Expression type does not match required type")
reject(max_type "let values = [1, 0]\nlet parts = values.split(separator: 0, maxSplits: true)\n"
  "Expression type does not match required type")
reject(omit_type "let values = [1, 0]\nlet parts = values.split(separator: 0, omittingEmptySubsequences: 1)\n"
  "Expression type does not match required type")
reject(non_equatable "let values = [Rng(1)]\nlet parts = values.split(separator: Rng(1))\n"
  "Array split element type must satisfy Equatable")
