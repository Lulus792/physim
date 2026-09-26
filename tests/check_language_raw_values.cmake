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

reject(duplicate "enum Code:\n    case a = 1\n    case b = 1\n"
  "Duplicate enum raw value")
reject(hex_duplicate "enum Code:\n    case a = 10\n    case b = 0xA\n"
  "Duplicate enum raw value")
reject(implicit_duplicate "enum Code:\n    case a = 1\n    case b\n    case c = 2\n"
  "Duplicate enum raw value")
reject(implicit_overflow "enum Code:\n    case a = 9223372036854775807\n    case b\n"
  "Implicit enum raw value exceeds Int64 range")
reject(out_of_range "enum Code:\n    case a = 9223372036854775808\n"
  "Integer literal is outside Int64 range")
reject(hex_out_of_range "enum Code:\n    case a = 0x8000000000000000\n"
  "Integer literal is outside Int64 range")
reject(expression "enum Code:\n    case a = 1 + 2\n"
  "Enum raw value must be an Int64 literal")
reject(payload "enum Code:\n    case a(value: Int64) = 1\n"
  "Enum raw values cannot be combined with payload fields")
reject(no_raw_property "enum Code:\n    case a\nlet x = Code.a.rawValue\n"
  "Enum does not define raw values")
reject(no_raw_factory "enum Code:\n    case a\nlet x = Code.fromRawValue(0)\n"
  "Enum does not define raw values")
reject(factory_type "enum Code:\n    case a = 1\nlet x = Code.fromRawValue(1.0)\n"
  "Expression type does not match required type")
reject(reserved "enum Code:\n    case fromRawValue = 1\n"
  "Enum raw value case name is reserved")
