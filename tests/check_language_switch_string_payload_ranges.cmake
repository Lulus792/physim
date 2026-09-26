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

function(accept name source)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "${name} was rejected: ${result}\n${diagnostic}")
  endif()
endfunction()

set(optional_start "let value: String? = Optional.some(\"m\")\nswitch value:\n")
reject(optional_empty "${optional_start}    case Optional.some(\"a\"..<\"a\"):\n        print(1)\n    default:\n        print(0)\n"
  "String switch range must contain at least one value")
reject(optional_nonliteral "${optional_start}    case Optional.some(value...\"z\"):\n        print(1)\n    default:\n        print(0)\n"
  "String switch range requires two string literals")
reject(optional_covered "${optional_start}    case Optional.some(\"a\"...\"z\"):\n        print(1)\n    case Optional.some(\"m\"):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(optional_escaped_covered "${optional_start}    case Optional.some(\"a\"...\"z\"):\n        print(1)\n    case Optional.some(\"\\u{6d}\"):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
accept(optional_open_not_covering "${optional_start}    case Optional.some(\"a\"..<\"m\"):\n        print(1)\n    case Optional.some(\"m\"):\n        print(2)\n    default:\n        print(0)\n")
accept(optional_guard "${optional_start}    case Optional.some(\"a\"...\"z\") if false:\n        print(1)\n    case Optional.some(\"m\"):\n        print(2)\n    default:\n        print(0)\n")

set(enum_prefix "enum Message:\n    case text(value: String)\nlet value = Message.text(\"m\")\nswitch value:\n")
reject(enum_reversed "${enum_prefix}    case Message.text(\"z\"...\"a\"):\n        print(1)\n    default:\n        print(0)\n"
  "String switch range must contain at least one value")
reject(enum_covered "${enum_prefix}    case Message.text(\"a\"...\"z\"):\n        print(1)\n    case Message.text(\"m\"...\"n\"):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
accept(enum_open_not_covering "${enum_prefix}    case Message.text(\"a\"..<\"m\"):\n        print(1)\n    case Message.text(\"m\"):\n        print(2)\n    default:\n        print(0)\n")
