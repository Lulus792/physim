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

reject(non_array "var x = 1\nx.removeFirst()\n"
  "Array edge removal requires an array value")
reject(immutable_first "let x = [1]\nx.removeFirst()\n"
  "Array edge removal requires a mutable var binding")
reject(immutable_last "let x = [1]\nx.removeLast(1)\n"
  "Array edge removal requires a mutable var binding")
reject(labeled_count "var x = [1]\nx.removeFirst(count: 1)\n"
  "Array edge removal expects zero or one positional count")
reject(extra_count "var x = [1]\nx.removeLast(1, 1)\n"
  "Array edge removal expects zero or one positional count")
reject(wrong_count "var x = [1]\nx.removeFirst(true)\n"
  "Expression type does not match required type")
