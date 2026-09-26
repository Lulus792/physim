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

reject(duplicate_template
  "func choose<T>(value: T) -> T:\n    return value\nfunc choose<U>(value: U) -> U:\n    return value\n"
  "Duplicate free function signature")
reject(ambiguous_generic
  "func choose<T>(left: T) -> Int64:\n    return 1\nfunc choose<U>(right: U) -> Int64:\n    return 2\nlet x = choose(3)\n"
  "Ambiguous free function overload")
reject(constraint_miss
  "func choose<T: Numeric>(value: T) -> Int64:\n    return 1\nfunc choose<T: Vector>(vector: T) -> Int64:\n    return 2\nlet x = choose(\"text\")\n"
  "No matching free function overload")
reject(ambiguous_reference
  "func choose<T>(value: T) -> T:\n    return value\nfunc choose<T>(values: [T]) -> [T]:\n    return values\nlet f = choose<Int64>\n"
  "Ambiguous generic free function reference")
reject(reference_type_mismatch
  "func choose<T>(value: T) -> T:\n    return value\nfunc choose<T>(values: [T]) -> [T]:\n    return values\nlet f: func(Bool) -> Bool = choose<Int64>\n"
  "No generic free function matches the required function type")
