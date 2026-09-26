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

reject(immutable_append "let x = [1]\nx.append(contentsOf: [2])\n"
  "Array method requires a mutable var binding")
reject(immutable_insert "let x = [1]\nx.insert(contentsOf: [2], at: 0)\n"
  "Array method requires a mutable var binding")
reject(wrong_append_label "var x = [1]\nx.append(using: [2])\n"
  "Array method expects exactly one positional argument")
reject(wrong_insert_label "var x = [1]\nx.insert(using: [2], at: 0)\n"
  "Array insert expects value and at:")
reject(wrong_append_type "var x = [1]\nx.append(contentsOf: [\"bad\"])\n"
  "Expression type does not match required type")
reject(wrong_insert_type "var x = [1]\nx.insert(contentsOf: [\"bad\"], at: 0)\n"
  "Expression type does not match required type")
reject(wrong_insert_index "var x = [1]\nx.insert(contentsOf: [2], at: true)\n"
  "Expression type does not match required type")
