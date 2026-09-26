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

reject(missing_case "let value: Int64? = nil\nswitch value:\n    case nil:\n        print(0)\n"
  "Optional switch must cover nil and some or provide default")
reject(guard_only "let value: Int64? = nil\nswitch value:\n    case nil if true:\n        print(0)\n    case Optional.some:\n        print(1)\n"
  "Optional switch must cover nil and some or provide default")
reject(duplicate_nil "let value: Int64? = nil\nswitch value:\n    case nil, nil:\n        print(0)\n    default:\n        print(1)\n"
  "Duplicate switch case")
reject(duplicate_some "let value: Int64? = nil\nswitch value:\n    case Optional.some:\n        print(1)\n    case Optional.some(item):\n        print(item)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(invalid_value "let value: Int64? = nil\nswitch value:\n    case true:\n        print(1)\n    default:\n        print(0)\n"
  "Optional switch case must be nil or Optional.some")
reject(missing_binding "let value: Int64? = nil\nswitch value:\n    case Optional.some():\n        print(1)\n    default:\n        print(0)\n"
  "Optional.some pattern requires one positional value")
reject(combined_binding "let value: Int64? = nil\nswitch value:\n    case Optional.some(item), nil:\n        print(item)\n    default:\n        print(0)\n"
  "Optional.some pattern requires one positional value and its own arm")
reject(nonliteral_value "let value: Int64? = nil\nswitch value:\n    case Optional.some(1 + 1):\n        print(1)\n    default:\n        print(0)\n"
  "Payload pattern requires a matching scalar literal, Int64, Float64 or String range or nil for an optional field")
reject(literal_incomplete "let value: Int64? = nil\nswitch value:\n    case nil:\n        print(0)\n    case Optional.some(1):\n        print(1)\n"
  "Optional switch must cover nil and some or provide default")
reject(literal_after_full "let value: Int64? = nil\nswitch value:\n    case Optional.some:\n        print(0)\n    case Optional.some(1):\n        print(1)\n    default:\n        print(2)\n"
  "Duplicate switch case")
reject(duplicate_literal "let value: Int64? = nil\nswitch value:\n    case Optional.some(1):\n        print(0)\n    case Optional.some(1):\n        print(1)\n    default:\n        print(2)\n"
  "Duplicate switch case")
reject(literal_wrong_type "let value: Int64? = nil\nswitch value:\n    case Optional.some(\"one\"):\n        print(1)\n    default:\n        print(0)\n"
  "Payload pattern requires a matching scalar literal, Int64, Float64 or String range or nil for an optional field")
reject(nil_nonoptional "let value: Int64? = nil\nswitch value:\n    case Optional.some(nil):\n        print(1)\n    default:\n        print(0)\n"
  "Payload pattern requires a matching scalar literal, Int64, Float64 or String range or nil for an optional field")
reject(duplicate_nested_nil "let value: Int64?? = nil\nswitch value:\n    case Optional.some(nil):\n        print(1)\n    case Optional.some(nil):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(nested_nil_incomplete "let value: Int64?? = nil\nswitch value:\n    case nil:\n        print(0)\n    case Optional.some(nil):\n        print(1)\n"
  "Optional switch must cover nil and some or provide default")
reject(float_same_value "let value: Float64? = nil\nswitch value:\n    case Optional.some(1.5):\n        print(1)\n    case Optional.some(1.50):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_rounded_value "let value: Float64? = nil\nswitch value:\n    case Optional.some(0.1):\n        print(1)\n    case Optional.some(0.10000000000000001):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_signed_zero "let value: Float64? = nil\nswitch value:\n    case Optional.some(-0.0):\n        print(1)\n    case Optional.some(0.0):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_nonfinite "let value: Float64? = nil\nswitch value:\n    case Optional.some(1e999):\n        print(1)\n    default:\n        print(0)\n"
  "Non-finite Float64 pattern literal")
reject(float_range_covered_literal "let value: Float64? = nil\nswitch value:\n    case Optional.some(1.0...2.0):\n        print(1)\n    case Optional.some(1.5):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_range_covered_range "let value: Float64? = nil\nswitch value:\n    case Optional.some(1.0...3.0):\n        print(1)\n    case Optional.some(1.5..<2.5):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(float_range_empty "let value: Float64? = nil\nswitch value:\n    case Optional.some(1.0..<1.0):\n        print(1)\n    default:\n        print(0)\n"
  "Float64 switch range must contain at least one value")
reject(float_range_variable "let lower = 1.0\nlet value: Float64? = nil\nswitch value:\n    case Optional.some(lower...2.0):\n        print(1)\n    default:\n        print(0)\n"
  "Float64 switch case must be a numeric literal")
reject(float_range_nonfinite "let value: Float64? = nil\nswitch value:\n    case Optional.some(0.0...1e999):\n        print(1)\n    default:\n        print(0)\n"
  "Non-finite Float64 pattern literal")
reject(range_covered_literal "let value: Int64? = nil\nswitch value:\n    case Optional.some(1...5):\n        print(1)\n    case Optional.some(3):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(range_covered_range "let value: Int64? = nil\nswitch value:\n    case Optional.some(1...5):\n        print(1)\n    case Optional.some(2..<5):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(range_incomplete "let value: Int64? = nil\nswitch value:\n    case nil:\n        print(0)\n    case Optional.some(1...5):\n        print(1)\n"
  "Optional switch must cover nil and some or provide default")
reject(range_variable "let lower = 1\nlet value: Int64? = nil\nswitch value:\n    case Optional.some(lower...5):\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch case must be an integer literal")
reject(range_empty "let value: Int64? = nil\nswitch value:\n    case Optional.some(5..<5):\n        print(1)\n    default:\n        print(0)\n"
  "Int64 switch range must contain at least one value")
reject(range_wrong_type "let value: String? = nil\nswitch value:\n    case Optional.some(1...5):\n        print(1)\n    default:\n        print(0)\n"
  "String switch range requires two string literals")
reject(nested_missing_value "let value: Int64?? = nil\nswitch value:\n    case Optional.some(Optional.some()):\n        print(1)\n    default:\n        print(0)\n"
  "Optional.some payload pattern requires one positional value")
reject(nested_wrong_type "let value: Int64?? = nil\nswitch value:\n    case Optional.some(Optional.some(\"bad\")):\n        print(1)\n    default:\n        print(0)\n"
  "Payload pattern requires a matching scalar literal")
reject(nested_covered "let value: Int64?? = nil\nswitch value:\n    case Optional.some(Optional.some):\n        print(1)\n    case Optional.some(Optional.some(5)):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(nested_binding_covers_tag "let value: Int64?? = nil\nswitch value:\n    case Optional.some(Optional.some(inner)):\n        print(inner)\n    case Optional.some(Optional.some):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(nested_range_covered "let value: Int64?? = nil\nswitch value:\n    case Optional.some(Optional.some(1...5)):\n        print(1)\n    case Optional.some(Optional.some(3)):\n        print(2)\n    default:\n        print(0)\n"
  "Duplicate switch case")
reject(nested_incomplete "let value: Int64?? = nil\nswitch value:\n    case nil:\n        print(0)\n    case Optional.some(nil):\n        print(1)\n    case Optional.some(Optional.some(5)):\n        print(2)\n"
  "Optional switch must cover nil and some or provide default")
reject(nested_bool_missing_false "let value: Bool?? = nil\nswitch value:\n    case nil:\n        print(0)\n    case Optional.some(nil):\n        print(1)\n    case Optional.some(Optional.some(true)):\n        print(2)\n"
  "Optional switch must cover nil and some or provide default")
reject(nested_bool_guarded_true "let value: Bool?? = nil\nswitch value:\n    case nil:\n        print(0)\n    case Optional.some(nil):\n        print(1)\n    case Optional.some(Optional.some(true)) if true:\n        print(2)\n    case Optional.some(Optional.some(false)):\n        print(3)\n"
  "Optional switch must cover nil and some or provide default")
