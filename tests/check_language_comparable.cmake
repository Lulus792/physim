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

set(generic "func earlier<T: Comparable>(a: T, b: T) -> T:\n    if a < b:\n        return a\n    return b\n")
reject(bool_argument "${generic}let value = earlier(true, false)\n"
  "Type argument must satisfy Comparable constraint")
reject(array_argument "${generic}let value = earlier([1], [2])\n"
  "Type argument must satisfy Comparable constraint")
reject(explicit_bool "${generic}let value = earlier<Bool>(true, false)\n"
  "Type argument must satisfy Comparable constraint")
reject(struct_bool "struct Interval<T: Comparable>:\n    let value: T\nlet interval = Interval<Bool>(true)\n"
  "Type argument must satisfy Comparable constraint")
reject(enum_bool "enum Choice<T: Comparable>:\n    case value(item: T)\nlet value = Choice<Bool>.value(true)\n"
  "Type argument must satisfy Comparable constraint")
reject(duplicate "func earlier<T: Comparable & Comparable>(value: T) -> T:\n    return value\n"
  "Duplicate generic constraint")
reject(bool_order "let invalid = true < false\n"
  "Operator requires numbers of the same type")
reject(mixed_order "let invalid = \"a\" < 1\n"
  "Expression type does not match required type")
