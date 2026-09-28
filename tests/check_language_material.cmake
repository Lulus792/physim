set(PHYSIM_LANGUAGE_CASE_GROUP material)
include("${CMAKE_CURRENT_LIST_DIR}/check_language_corpus.cmake")
physim_check_language_program(material_invalid "${PROGRAM}")
