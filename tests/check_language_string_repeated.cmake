set(PHYSIM_LANGUAGE_CASE_GROUP string_repeated)
include("${CMAKE_CURRENT_LIST_DIR}/check_language_corpus.cmake")
physim_check_language_program(string_repeated_negative "${PROGRAM}")
