foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
set(source "${WORK}/multiline.phys")
file(WRITE "${source}" "let text = \"\"\"a\nb\rc\"\"\"\nassert(text == \"a\\nb\\nc\")\nlet escaped = \"\"\"\\r\"\"\"\n")
execute_process(COMMAND "${COMPILER}" --emit-c "${source}"
  RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error TIMEOUT 15)
string(REGEX MATCH "static const unsigned char pslit_[0-9]+\\[\\] = \\{[^}]*\\}"
  first_literal "${output}")
string(REGEX REPLACE "^[^{]*\\{" "" first_bytes "${first_literal}")
string(REGEX REPLACE "\\}.*$" "" first_bytes "${first_bytes}")
string(STRIP "${first_bytes}" first_bytes)
if(NOT result EQUAL 0 OR NOT error STREQUAL "" OR
   NOT first_bytes STREQUAL "97,10,98,10,99,0" OR
   NOT output MATCHES "13,0\\};")
  message(FATAL_ERROR "Multiline newline normalization failed: ${result}\n${error}")
endif()
