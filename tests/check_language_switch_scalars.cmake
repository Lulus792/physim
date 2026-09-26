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

reject(int_missing_default "switch 1:\n    case 1:\n        print(1)\n"
  "Int64 switch requires default")
reject(bool_missing_case "switch true:\n    case true:\n        print(1)\n"
  "Bool switch must cover true and false or provide default")
reject(bool_guard_only "switch true:\n    case true if true:\n        print(1)\n    case false:\n        print(0)\n"
  "Bool switch must cover true and false or provide default")
reject(int_variable "let chosen = 2\nswitch chosen:\n    case chosen:\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch case must be an integer literal")
reject(bool_integer "switch true:\n    case 1:\n        print(1)\n    default:\n        print(0)\n"
  "Bool switch case must be true or false")
reject(int_bool "switch 1:\n    case true:\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch case must be an integer literal")
reject(int_duplicate "switch 0:\n    case 0, +0:\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(int_hex_duplicate "switch 10:\n    case 10, 0xA:\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(range_literal_overlap "switch 2:\n    case 1...3, 2:\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(range_overlap "switch 3:\n    case 1...3:\n        print(1)\n    case 3..<5:\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(range_empty_closed "switch 3:\n    case 3...2:\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch range must contain at least one value")
reject(range_empty_open "switch 3:\n    case 3..<3:\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch range must contain at least one value")
reject(range_empty_min "switch 0:\n    case -0x8000000000000000..< -0x8000000000000000:\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch range must contain at least one value")
reject(range_variable "let lower = 1\nswitch 3:\n    case lower...3:\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch case must be an integer literal")
reject(range_missing_default "switch 3:\n    case 1...5:\n        print(1)\n"
  "Int64 switch requires default")
reject(bool_duplicate "switch false:\n    case false:\n        print(1)\n    case false:\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(int_overflow "switch 0:\n    case 9223372036854775808:\n        print(1)\n    default:\n        print(0)\n"
  "Integer literal is outside Int64 range")
reject(int_hex_overflow "switch 0:\n    case 0x8000000000000000:\n        print(1)\n    default:\n        print(0)\n"
  "Integer literal is outside Int64 range")
reject(string_missing_default "switch \"x\":\n    case \"x\":\n        print(1)\n"
  "String switch requires default")
reject(string_invalid "let value = \"x\"\nswitch value:\n    case value:\n        print(1)\n    default:\n        print(0)\n"
  "String switch case must be a string literal")
reject(string_duplicate "switch \"x\":\n    case \"x\", \"x\":\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(string_multiline_duplicate "switch \"line\\nnext\":\n    case \"line\\nnext\", \"\"\"line\nnext\"\"\":\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_missing_default "switch 1.0:\n    case 1.0:\n        print(1)\n"
  "Float64 switch requires default")
reject(float_variable "let chosen = 1.5\nswitch chosen:\n    case chosen:\n        print(1)\n    default:\n        print(0)\n"
  "Float64 switch case must be a numeric literal")
reject(float_wrong_type "switch 1.5:\n    case true:\n        print(1)\n    default:\n        print(0)\n"
  "Float64 switch case must be a numeric literal")
reject(float_range_variable "let lower = 1.0\nswitch 1.5:\n    case lower...3.0:\n        print(1)\n    default:\n        print(0)\n"
  "Float64 switch case must be a numeric literal")
reject(float_range_empty_closed "switch 1.5:\n    case 2.0...1.0:\n        print(1)\n    default:\n        print(0)\n"
  "Float64 switch range must contain at least one value")
reject(float_range_empty_open "switch 1.5:\n    case 1.0..<1.0:\n        print(1)\n    default:\n        print(0)\n"
  "Float64 switch range must contain at least one value")
reject(float_range_overlap "switch 1.5:\n    case 1.0...2.0:\n        print(1)\n    case 2.0..<3.0:\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_range_literal_overlap "switch 1.5:\n    case 1.0..<2.0, 1.5:\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_range_signed_zero_overlap "switch 0.0:\n    case -1.0...-0.0:\n        print(1)\n    case 0.0:\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_range_nonfinite "switch 1.0:\n    case 0.0...1e999:\n        print(1)\n    default:\n        print(0)\n"
  "Non-finite Float64 pattern literal")
reject(float_same_value "switch 1.5:\n    case 1.5:\n        print(1)\n    case 1.50:\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_signed_zero "switch 0.0:\n    case -0.0, 0.0:\n        print(1)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_integer_equivalent "switch 3.0:\n    case 3:\n        print(1)\n    case 3.0:\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_nonfinite "switch 1.0:\n    case 1e999:\n        print(1)\n    default:\n        print(0)\n"
  "Non-finite Float64 pattern literal")
