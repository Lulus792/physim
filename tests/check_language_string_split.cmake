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
reject(missing "let text = \"a,b\"\nlet parts = text.split()\n"
  "String split expects separator:")
reject(label "let text = \"a,b\"\nlet parts = text.split(by: \",\")\n"
  "String split expects separator:")
reject(max_type "let text = \"a,b\"\nlet parts = text.split(separator: \",\", maxSplits: true)\n"
  "Expression type does not match required type")
reject(omit_type "let text = \"a,b\"\nlet parts = text.split(separator: \",\", omittingEmptySubsequences: 2)\n"
  "Expression type does not match required type")
reject(duplicate "let text = \"a,b\"\nlet parts = text.split(separator: \",\", omittingEmptySubsequences: true, omittingEmptySubsequences: false)\n"
  "String split expects separator:")
