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
    message(FATAL_ERROR "${name} was not rejected: ${result}\n${diagnostic}")
  endif()
endfunction()
reject(first_write "var values = [1]\nvalues.first = Optional.some(2)\n"
  "Assignment requires a mutable var binding")
reject(last_write "var values = [1]\nvalues.last = Optional.some(2)\n"
  "Assignment requires a mutable var binding")
reject(empty_write "var values = [1]\nvalues.isEmpty = true\n"
  "Assignment requires a mutable var binding")
reject(first_where_wrong_label "func positive(value: Int64) -> Bool:\n    return value > 0\nlet values = [1]\nlet found = values.first(test: positive)\n"
  "Unknown method for this receiver type")
reject(last_where_wrong_predicate "let values = [1]\nlet found = values.last(where: 1)\n"
  "Array predicate query requires func")
reject(first_index_where_wrong_predicate "let values = [1]\nlet found = values.firstIndex(where: 1)\n"
  "Array predicate query requires func")
reject(last_index_where_wrong_predicate "let values = [1]\nlet found = values.lastIndex(where: 1)\n"
  "Array predicate query requires func")
