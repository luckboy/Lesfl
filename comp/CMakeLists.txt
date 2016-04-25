include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/frontend" comp_sources)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" comp_sources)

add_library(lesfl SHARED ${comp_sources})
add_library(lesfl_static STATIC ${comp_sources})
set_target_properties(lesfl_static PROPERTIES OUTPUT_NAME lesfl)
