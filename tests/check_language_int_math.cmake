set(PHYSIM_LANGUAGE_CASE_GROUP int_math_errors)
include("${CMAKE_CURRENT_LIST_DIR}/check_language_corpus.cmake")
physim_check_language_program(int_abs_overflow "${ABS_PROGRAM}")
physim_check_language_program(int_clamp_bounds "${CLAMP_PROGRAM}")
