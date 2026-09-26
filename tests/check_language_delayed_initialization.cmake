foreach(required COMPILER WORK PROGRAM PROGRAM_GLOBAL PROGRAM_COMPOUND PROGRAM_FIELD PROGRAM_CAPTURE)
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

reject(untyped "var value\n" "requires 'var' and a type annotation")
reject(immutable "let value: Int64\n" "requires 'var' and a type annotation")
reject(void "var value: Void\n" "Unknown or unsupported type")

execute_process(COMMAND "${PROGRAM}" RESULT_VARIABLE result
  ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 70 OR NOT diagnostic MATCHES
   "delayed_initialization_error\\.phys:3:12: runtime error: Local used before initialization")
  message(FATAL_ERROR "Uninitialized read was not diagnosed: ${result}\n${diagnostic}")
endif()

function(runtime_error program expected)
  execute_process(COMMAND "${program}" RESULT_VARIABLE result
    ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 70 OR NOT diagnostic MATCHES "${expected}")
    message(FATAL_ERROR "Expected runtime initialization error: ${result}\n${diagnostic}")
  endif()
endfunction()
runtime_error("${PROGRAM_GLOBAL}" "delayed_global_error\\.phys:2:7: runtime error: Global used before initialization")
runtime_error("${PROGRAM_COMPOUND}" "delayed_compound_error\\.phys:3:5: runtime error: Local used before initialization")
runtime_error("${PROGRAM_FIELD}" "delayed_field_error\\.phys:5:10: runtime error: Local used before initialization")
runtime_error("${PROGRAM_CAPTURE}" "delayed_capture_error\\.phys:4:[0-9]+: runtime error: Local used before initialization")
