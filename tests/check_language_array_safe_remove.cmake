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
    message(FATAL_ERROR "Invalid safe removal ${name} was accepted or misdiagnosed: ${result}\n${diagnostic}")
  endif()
endfunction()

reject(immutable "let a = [1]\nlet x = a.popLast()\n"
  "Safe removal requires a mutable var binding")
reject(nonarray "var a = 1\nlet x = a.popLast()\n"
  "Safe removal requires an array value")
reject(pop_argument "var a = [1]\nlet x = a.popLast(0)\n"
  "Array popLast expects no arguments")
reject(type_args "var a = [1]\nlet x = a.popLast<Int64>()\n"
  "Explicit type arguments require a generic function or method")
reject(old_remove_name "var a = [1]\nlet x = a.removeIfPresent(at: 0)\n"
  "Unknown method for this receiver type")
reject(old_get_name "let a = [1]\nlet x = a.get(at: 0)\n"
  "Unknown method for this receiver type")
reject(old_appending_name "let a = [1]\nlet x = a.appending(2)\n"
  "Unknown method for this receiver type")
reject(old_removing_name "let a = [1]\nlet x = a.removing(at: 0)\n"
  "Unknown method for this receiver type")
reject(old_inserting_name "let a = [1]\nlet x = a.inserting(2, at: 0)\n"
  "Unknown method for this receiver type")
