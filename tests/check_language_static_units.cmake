foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")

function(reject name source)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT diagnostic MATCHES
     "${name}\\.phys:[0-9]+:[0-9]+: error: Statically incompatible unit dimensions")
    message(FATAL_ERROR "${name} was not rejected at compile time: ${result}\n${diagnostic}")
  endif()
endfunction()

function(reject_operator_type name source expected)
  file(WRITE "${WORK}/${name}.phys" "${source}")
  execute_process(COMMAND "${COMPILER}" --check "${WORK}/${name}.phys"
    RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
  if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "${expected}")
    message(FATAL_ERROR "${name} was not rejected as an invalid Quantity operator: ${result}\n${diagnostic}")
  endif()
endfunction()

set(units "let metres = Unit(1,0,0,0,0,0,0,1,\"m\")\nlet seconds = Unit(0,0,1,0,0,0,0,1,\"s\")\n")
reject(quantity_add "${units}let distance = Quantity(1,metres)\nlet duration = Quantity(2,seconds)\nlet bad = distance.adding(duration)\n")
reject(quantity_subtract "${units}let bad = Quantity(1,metres).subtracting(Quantity(2,seconds))\n")
reject(quantity_convert "${units}let bad = Quantity(1,metres).converted(seconds)\n")
reject(unit_convert "${units}let alias = metres\nlet bad = alias.convert(to: seconds, value: 1)\n")
reject(composed "${units}let speed = metres.divided(seconds,\"m/s\")\nlet bad = Quantity(1,speed).converted(metres)\n")
reject(powered "${units}let area = metres.powered(2,\"m2\")\nlet bad = Quantity(1,area).converted(metres)\n")
reject(member_unit "${units}let lengthExponent = 1\nlet alias = Unit(lengthExponent,0,0,0,0,0,0,1,\"m\")\nlet bad = Quantity(1,alias).unit.convert(1,seconds)\n")
reject(arithmetic_exponents "${units}let lengthExponent = (8 / 2 - 3) * 1\nlet computed = Unit(lengthExponent,0,0,0,0,0,0,1,\"m\")\nlet bad = Quantity(1,computed).converted(seconds)\n")
reject(arithmetic_power "${units}let area = metres.powered(5 % 3,\"m2\")\nlet bad = Quantity(1,area).converted(metres)\n")
reject(operator_add "${units}let bad = Quantity(1,metres) + Quantity(2,seconds)\n")
reject(operator_subtract "${units}let bad = Quantity(1,metres) - Quantity(2,seconds)\n")
reject(operator_chain "${units}let bad = (Quantity(1,metres) + Quantity(2,metres)) - Quantity(3,seconds)\n")
reject(operator_method "${units}let bad = (Quantity(1,metres) + Quantity(2,metres)).converted(seconds)\n")
reject(scalar_chain "${units}let bad = (2 * Quantity(1,metres)).converted(seconds)\n")
reject(negated_chain "${units}let bad = (-Quantity(1,metres)).converted(seconds)\n")
reject_operator_type(operator_number "${units}let bad = Quantity(1,metres) + 2\n"
  "Expression type does not match required type")
reject_operator_type(operator_multiply "${units}let bad = Quantity(1,metres) * Quantity(2,metres)\n"
  "Invalid Quantity operator or operand types")
reject_operator_type(operator_inverse_division "${units}let bad = 2 / Quantity(1,metres)\n"
  "Invalid Quantity operator or operand types")

file(WRITE "${WORK}/dynamic.phys" "${units}func unknown() -> Unit:\n    return seconds\nlet value = Quantity(1,unknown())\nlet result = value.adding(Quantity(2,metres))\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/dynamic.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Dynamic unit should retain runtime check: ${result}\n${diagnostic}")
endif()
file(WRITE "${WORK}/operator_dynamic.phys" "${units}func unknown() -> Quantity:\n    return Quantity(1,seconds)\nlet value = Quantity(2,metres) + unknown()\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/operator_dynamic.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Dynamic Quantity operator should retain runtime check: ${result}\n${diagnostic}")
endif()
file(WRITE "${WORK}/compatible.phys" "${units}let centimetres = Unit(1,0,0,0,0,0,0,0.01,\"cm\")\nlet value = Quantity(1,metres).adding(Quantity(50,centimetres))\nlet different = metres.isCompatible(seconds)\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/compatible.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Compatible dimensions should compile: ${result}\n${diagnostic}")
endif()
file(WRITE "${WORK}/arithmetic_compatible.phys" "${units}let exponent = 3 - 2\nlet computed = Unit(exponent,0,0,0,0,0,0,1,\"m\")\nlet value = Quantity(1,computed).adding(Quantity(2,metres))\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/arithmetic_compatible.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Compatible arithmetic dimensions should compile: ${result}\n${diagnostic}")
endif()
file(WRITE "${WORK}/overflow_dynamic.phys" "${units}let exponent = 9223372036854775807 + 1\nlet computed = Unit(exponent,0,0,0,0,0,0,1,\"m\")\nlet value = Quantity(1,computed).converted(seconds)\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/overflow_dynamic.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Overflowing constant arithmetic should retain runtime check: ${result}\n${diagnostic}")
endif()
