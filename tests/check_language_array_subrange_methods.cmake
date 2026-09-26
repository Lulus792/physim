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
    message(FATAL_ERROR "Invalid subrange method ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(remove_immutable "let a = [1, 2]\na.removeSubrange(0..<1)\n"
  "Array subrange method requires a mutable var binding")
reject(replace_immutable "let a = [1, 2]\na.replaceSubrange(0..<1, with: [3])\n"
  "Array subrange method requires a mutable var binding")
reject(nonarray "var a = 1\na.removeSubrange(0..<1)\n"
  "Array subrange method requires an array value")
reject(remove_scalar "var a = [1, 2]\na.removeSubrange(0)\n"
  "Array removeSubrange expects one bounded range")
reject(replace_scalar "var a = [1, 2]\na.replaceSubrange(0, with: [3])\n"
  "Array replaceSubrange expects a bounded range and with: array")
reject(remove_extra "var a = [1, 2]\na.removeSubrange(0..<1, 2)\n"
  "Array removeSubrange expects one bounded range")
reject(remove_label "var a = [1, 2]\na.removeSubrange(bounds: 0..<1)\n"
  "Array removeSubrange expects one bounded range")
reject(replace_missing "var a = [1, 2]\na.replaceSubrange(0..<1)\n"
  "Array replaceSubrange expects a bounded range and with: array")
reject(replace_label "var a = [1, 2]\na.replaceSubrange(0..<1, using: [3])\n"
  "Array replaceSubrange expects a bounded range and with: array")
reject(replace_type "var a = [1, 2]\na.replaceSubrange(0..<1, with: [true])\n"
  "Expression type does not match required type")
reject(strided "var a = [1, 2]\na.removeSubrange(0..<2 by 2)\n"
  "Array removeSubrange expects one bounded range")
reject(type_args "var a = [1, 2]\na.removeSubrange<Int64>(0..<1)\n"
  "Explicit type arguments require a generic function or method")
