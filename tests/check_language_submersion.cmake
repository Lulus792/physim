foreach(required COMPILER PROGRAM_RADIUS PROGRAM_RANGE WORK)
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
reject(submersion_wrong_type "let s = Submersion.sphere(\"large\", 0)\n"
  "Expression type does not match required type")
reject(submersion_property_mutation "var s = Submersion.sphere(2, 0)\ns.volume = 1\n"
  "Assignment requires a mutable var binding")
foreach(kind RADIUS RANGE)
  execute_process(COMMAND "${PROGRAM_${kind}}"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 70 OR NOT diagnostic MATCHES "submersion_invalid" OR
     NOT diagnostic MATCHES "Sphere submersion")
    message(FATAL_ERROR "Invalid ${kind} was not diagnosed: ${result}\n${diagnostic}")
  endif()
endforeach()
