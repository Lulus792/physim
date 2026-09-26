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

reject(index_write "var text = \"abc\"\ntext[0] = \"A\"\n"
  "String indices and slices are immutable")
reject(range_write "var text = \"abc\"\ntext[0..<2] = \"AB\"\n"
  "String indices and slices are immutable")
reject(stepped_write "var text = \"abc\"\ntext[0..<3 by 2] = \"AC\"\n"
  "String indices and slices are immutable")
reject(noninteger_index "let text = \"abc\"\nlet value = text[1.5]\n"
  "Expression type does not match required type")
reject(count_write "var text = \"abc\"\ntext.count = 2\n"
  "Assignment requires a mutable var binding")
reject(bytes_write "var text = \"abc\"\ntext.utf8.count = 2\n"
  "Assignment requires a mutable var binding")
reject(empty_write "var text = \"abc\"\ntext.isEmpty = true\n"
  "Assignment requires a mutable var binding")
reject(contains_number "let text = \"abc\"\nlet found = text.contains(2)\n"
  "Expression type does not match required type")
reject(contains_named "let text = \"abc\"\nlet found = text.contains(of: \"a\")\n"
  "String contains expects exactly one positional argument")
reject(first_index_positional "let text = \"abc\"\nlet found = text.firstIndex(\"a\")\n"
  "String firstIndex expects of: value")
reject(last_index_positional "let text = \"abc\"\nlet found = text.lastIndex(\"a\")\n"
  "String lastIndex expects of: value")
reject(last_index_number "let text = \"abc\"\nlet found = text.lastIndex(of: 2)\n"
  "Expression type does not match required type")
reject(prefix_named "let text = \"abc\"\nlet found = text.hasPrefix(of: \"a\")\n"
  "String hasPrefix expects exactly one positional argument")
reject(suffix_number "let text = \"abc\"\nlet found = text.hasSuffix(2)\n"
  "Expression type does not match required type")
reject(replace_positional "let text = \"abc\"\nlet found = text.replacingOccurrences(\"a\", \"b\")\n"
  "String replacingOccurrences expects of: search, with: replacement")
reject(replace_missing "let text = \"abc\"\nlet found = text.replacingOccurrences(of: \"a\")\n"
  "String replacingOccurrences expects of: search, with: replacement")
reject(replace_number "let text = \"abc\"\nlet found = text.replacingOccurrences(of: 1, with: \"b\")\n"
  "Expression type does not match required type")
reject(replace_result_number "let text = \"abc\"\nlet found = text.replacingOccurrences(of: \"a\", with: 2)\n"
  "Expression type does not match required type")
reject(split_positional "let text = \"a,b\"\nlet parts = text.split(\",\")\n"
  "String split expects separator:")
reject(split_missing "let text = \"a,b\"\nlet parts = text.split()\n"
  "String split expects separator:")
reject(split_number "let text = \"a,b\"\nlet parts = text.split(separator: 1)\n"
  "Expression type does not match required type")
reject(joined_positional "let parts = [\"a\", \"b\"]\nlet text = parts.joined(\",\")\n"
  "String array joined expects separator: value")
reject(joined_number "let parts = [\"a\", \"b\"]\nlet text = parts.joined(separator: 1)\n"
  "Expression type does not match required type")
reject(joined_wrong_array "let parts = [1, 2]\nlet text = parts.joined(separator: \"-\")\n"
  "joined requires a \\[String\\] value")
