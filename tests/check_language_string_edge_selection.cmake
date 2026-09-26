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
    message(FATAL_ERROR "Invalid String edge selection ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(prefix_missing "let a = \"abc\"\nlet b = a.prefix()\n"
  "prefix and suffix require one positional count")
reject(suffix_missing "let a = \"abc\"\nlet b = a.suffix()\n"
  "prefix and suffix require one positional count")
reject(extra "let a = \"abc\"\nlet b = a.dropFirst(1, 2)\n"
  "dropFirst and dropLast accept zero or one")
reject(label "let a = \"abc\"\nlet b = a.dropLast(count: 1)\n"
  "dropFirst and dropLast accept zero or one")
reject(type "let a = \"abc\"\nlet b = a.prefix(true)\n"
  "Expression type does not match required type")
reject(type_args "let a = \"abc\"\nlet b = a.suffix<Int64>(1)\n"
  "Explicit type arguments require a generic function or method")
