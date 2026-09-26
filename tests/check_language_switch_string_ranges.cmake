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

set(start "let value = \"m\"\nswitch value:\n")
reject(reversed "${start}    case \"z\"...\"a\":\n        print(1)\n    default:\n        print(0)\n"
  "String switch range must contain at least one value")
reject(empty_open "${start}    case \"a\"..<\"a\":\n        print(1)\n    default:\n        print(0)\n"
  "String switch range must contain at least one value")
reject(nonliteral "${start}    case value...\"z\":\n        print(1)\n    default:\n        print(0)\n"
  "String switch range requires two string literals")
reject(wrong_type "${start}    case \"a\"...3:\n        print(1)\n    default:\n        print(0)\n"
  "String switch range requires two string literals")
reject(overlap_same "${start}    case \"a\"...\"m\", \"m\"...\"z\":\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(overlap_later "${start}    case \"a\"...\"z\":\n        print(1)\n    case \"m\":\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(escaped_duplicate "${start}    case \"a\"...\"z\":\n        print(1)\n    case \"\\u{6d}\":\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
