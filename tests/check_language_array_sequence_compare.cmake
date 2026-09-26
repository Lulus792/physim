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
reject(starts_missing "let x = [1].starts()\n"
  "Array starts expects with: prefix")
reject(starts_label "let x = [1].starts([1])\n"
  "Array starts expects with: prefix")
reject(starts_type "let x = [1].starts(with: [\"1\"])\n"
  "Expression type does not match required type")
reject(equal_missing "let x = [1].elementsEqual()\n"
  "Array elementsEqual expects one positional array")
reject(equal_label "let x = [1].elementsEqual(other: [1])\n"
  "Array elementsEqual expects one positional array")
reject(equal_type "let x = [1].elementsEqual([\"1\"])\n"
  "Expression type does not match required type")
reject(non_equatable "let x = [Rng(1)].starts(with: [Rng(1)])\n"
  "Array sequence comparison element type must satisfy Equatable")
