include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

include_directories("${FLEX_INCLUDE_DIR}")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/frontend/lexer.cpp"
	COMMAND "${CMAKE_COMMAND}"
	ARGS -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/frontend"
	COMMAND "${FLEX_EXECUTABLE}"
	ARGS -o"${CMAKE_CURRENT_BINARY_DIR}/frontend/lexer.cpp" frontend/lexer.lpp
	DEPENDS frontend/lexer.lpp
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/frontend/bison_parser.cpp"
	"${CMAKE_CURRENT_BINARY_DIR}/frontend/bison_parser.hpp"
	COMMAND "${CMAKE_COMMAND}"
	ARGS -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/frontend"
	COMMAND "${BISON_EXECUTABLE}"
	ARGS -d -L c++ -r all -o "${CMAKE_CURRENT_BINARY_DIR}/frontend/bison_parser.cpp" frontend/bison_parser.ypp
	DEPENDS frontend/bison_parser.ypp
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set_source_files_properties("${CMAKE_CURRENT_BINARY_DIR}/frontend/lexer.cpp"
	PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/frontend/bison_parser.hpp")

list(APPEND comp_sources "${CMAKE_CURRENT_BINARY_DIR}/frontend/bison_parser.cpp")
list(APPEND comp_sources "${CMAKE_CURRENT_BINARY_DIR}/frontend/lexer.cpp")
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/frontend" comp_sources)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" comp_sources)

list(APPEND comp_libraries ${LETINCOMP_LIBRARIES})

add_library(lesfl SHARED ${comp_sources})
target_link_libraries(lesfl ${comp_libraries})
add_library(lesfl_static STATIC ${comp_sources})
target_link_libraries(lesfl_static ${comp_libraries})
set_target_properties(lesfl_static PROPERTIES OUTPUT_NAME lesfl)
