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

reject(non_array "var x = 1\nx.removeAll()\n" "Array removeAll requires an array value")
reject(immutable "let x = [1]\nx.removeAll()\n"
  "Array removeAll requires a mutable var binding")
reject(immutable_where "func remove(x: Int64) -> Bool:\n    return true\nlet x = [1]\nx.removeAll(where: remove)\n"
  "Array removeAll requires a mutable var binding")
reject(wrong_label "func remove(x: Int64) -> Bool:\n    return true\nvar x = [1]\nx.removeAll(using: remove)\n"
  "Array removeAll expects where: predicate")
reject(extra_argument "func remove(x: Int64) -> Bool:\n    return true\nvar x = [1]\nx.removeAll(where: remove, where: remove)\n"
  "Array removeAll expects where: predicate")
reject(non_function "var x = [1]\nx.removeAll(where: true)\n"
  "Array removeAll predicate must be func")
reject(wrong_element "func remove(x: String) -> Bool:\n    return true\nvar x = [1]\nx.removeAll(where: remove)\n"
  "Array removeAll predicate must be func")
