set(PHYSIM_LANGUAGE_CASE_GROUP submersion)
include("${CMAKE_CURRENT_LIST_DIR}/check_language_corpus.cmake")
physim_check_language_program(submersion_invalid_radius "${PROGRAM_RADIUS}")
physim_check_language_program(submersion_invalid_range "${PROGRAM_RANGE}")
