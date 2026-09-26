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
reject(medium_wrong_type "let m = Medium(\"air\", 0)\n"
  "Expression type does not match required type")
reject(medium_property_mutation "var m = Medium(1, 0)\nm.density = -1\n"
  "Assignment requires a mutable var binding")
reject(medium_wrong_velocity "let m = Medium.air()\nlet f = m.dragForce(Vec2(1, 0), 0.47, 0.01)\n"
  "Expression type does not match required type")
reject(medium_wrong_stokes_radius "let m = Medium.air()\nlet f = m.stokesDrag(Vec3(1, 0, 0), \"large\")\n"
  "Expression type does not match required type")
