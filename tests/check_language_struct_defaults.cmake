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
    message(FATAL_ERROR "${name} was not rejected as expected: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(default_type "struct S:\n    let value: Int64 = true\n"
  "Expression type does not match required type")
reject(default_unknown "struct S:\n    let value: Int64 = unknown\n"
  "Unknown name")
reject(required_field "struct S:\n    let required: Int64\n    let optional: Int64 = 2\nlet s = S()\n"
  "Missing struct fields")
reject(generic_inference "struct Box<T>:\n    let item: T\n    let count: Int64 = 1\nlet b = Box()\n"
  "Missing or excess generic struct fields")
reject(named_field "struct S:\n    let value: Int64 = 1\nlet s = S(missing: 2)\n"
  "Unknown parameter name")
reject(recursive_default "struct S:\n    let value: Int64 = S().value\nlet s = S()\n"
  "Recursive struct field default")
reject(indirect_default "struct A:\n    let value: Int64 = B().value\nstruct B:\n    let value: Int64 = A().value\nlet a = A()\n"
  "Recursive struct field default")
