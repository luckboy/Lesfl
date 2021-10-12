include_directories("${CPPUNIT_INCLUDE_DIR}")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
include_directories(../../comp)

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/frontend" comp_test_sources)
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}" comp_test_sources)

list(APPEND comp_test_libraries lesfl_static)
list(APPEND comp_test_libraries ${LETINCOMP_LIBRARIES})
list(APPEND comp_test_libraries ${CPPUNIT_LIBRARIES})

add_executable(testcomp "" ${comp_test_sources})
target_link_libraries(testcomp ${comp_test_libraries})
add_test(comp_test "${CMAKE_CURRENT_BINARY_DIR}/testcomp${CMAKE_EXECUTABLE_SUFFIX}")
