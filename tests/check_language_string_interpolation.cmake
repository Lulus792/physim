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
    message(FATAL_ERROR "Invalid interpolation ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(non_scalar "let x = \"value \\(Vec2(1,2))\"\n"
  "String interpolation requires Bool, Int64, Float64 or String")
reject(missing_close "let x = \"value \\(1 + 2\"\n"
  "Unterminated string")
reject(empty_expression "let x = \"value \\()\"\n" "Expected expression")
reject(missing_expression_end "let x = \"value \\(1 2)\"\n"
  "after interpolated expression")
reject(unknown_name "let x = \"value \\(missing)\"\n" ":1:18: error: Unknown name")
reject(multiline_location "let x = \"\"\"a\n\\(missing)\n\"\"\"\n"
  ":2:3: error: Unknown name")

file(WRITE "${WORK}/crlf.phys"
  "let text = \"\"\"a\n\\(1)\nb\"\"\"\nassert(text == \"a\\n1\\nb\")\n")
execute_process(COMMAND "${COMPILER}" --emit-c "${WORK}/crlf.phys"
  RESULT_VARIABLE result OUTPUT_VARIABLE generated ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 0 OR NOT diagnostic STREQUAL "" OR
   NOT generated MATCHES "97,10,0\\};" OR NOT generated MATCHES "10,98,0\\};")
  message(FATAL_ERROR "Interpolated CRLF segments were not normalized: ${result}\n${diagnostic}")
endif()
