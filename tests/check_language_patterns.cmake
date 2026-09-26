foreach(required COMPILER WORK)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "Missing ${required}")
  endif()
endforeach()
file(MAKE_DIRECTORY "${WORK}")
set(header "enum State:\n    case idle\n    case paused\n    case running\n")
set(prefix "func category(value: State) -> Int64:\n    switch value:\n")

file(WRITE "${WORK}/duplicate.phys" "${header}${prefix}        case State.idle, State.idle:\n            return 0\n        default:\n            return 1\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/duplicate.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES ":7:[0-9]+: error: Duplicate switch case")
  message(FATAL_ERROR "Same-arm duplicate was not rejected: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/cross.phys" "${header}${prefix}        case State.idle, State.paused:\n            return 0\n        case State.running, State.paused:\n            return 1\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/cross.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "error: Duplicate switch case")
  message(FATAL_ERROR "Cross-arm duplicate was not rejected: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/incomplete.phys" "${header}${prefix}        case State.idle, State.paused:\n            return 0\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/incomplete.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "error: Switch must cover every enum case")
  message(FATAL_ERROR "Incomplete multi-pattern switch was accepted: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/invalid.phys" "${header}${prefix}        case State.idle, State.running == State.running:\n            return 0\n        default:\n            return 1\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/invalid.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "error:")
  message(FATAL_ERROR "Non-case pattern was accepted: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/guard_type.phys" "${header}${prefix}        case State.idle if 1:\n            return 0\n        default:\n            return 1\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/guard_type.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Expression type does not match required type")
  message(FATAL_ERROR "Non-Bool switch guard was accepted: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/guard_incomplete.phys" "${header}${prefix}        case State.idle if true:\n            return 0\n        case State.paused, State.running:\n            return 1\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/guard_incomplete.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Switch must cover every enum case")
  message(FATAL_ERROR "Guarded case incorrectly counted as exhaustive: ${result}\n${diagnostic}")
endif()

file(WRITE "${WORK}/guard_unreachable.phys" "${header}${prefix}        case State.idle:\n            return 0\n        case State.idle if true:\n            return 1\n        default:\n            return 2\n")
execute_process(COMMAND "${COMPILER}" --check "${WORK}/guard_unreachable.phys"
  RESULT_VARIABLE result ERROR_VARIABLE diagnostic TIMEOUT 15)
if(NOT result EQUAL 1 OR NOT diagnostic MATCHES "Duplicate switch case")
  message(FATAL_ERROR "Unreachable guarded case was accepted: ${result}\n${diagnostic}")
endif()
