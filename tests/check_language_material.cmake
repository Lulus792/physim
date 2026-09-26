foreach(required COMPILER WORK PROGRAM)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
file(WRITE "${WORK}/immutable.phys"
  "var material = Material(1000,0.2,0.6)\nmaterial.friction = -1\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/immutable.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Assignment requires a mutable var binding")
  message(FATAL_ERROR "Material properties were writable: ${result}\n${diagnostic}")
endif()
execute_process(COMMAND "${PROGRAM}" RESULT_VARIABLE result
  ERROR_VARIABLE diagnostic TIMEOUT 15)
if(result EQUAL 0 OR NOT diagnostic MATCHES
   "material_invalid\\.phys:1:[0-9]+: runtime error: Material density, restitution or friction is invalid")
  message(FATAL_ERROR "Invalid restitution was not diagnosed: ${result}\n${diagnostic}")
endif()
