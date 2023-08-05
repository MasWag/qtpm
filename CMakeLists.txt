cmake_minimum_required(VERSION 3.5)

project("Quantitative Timed Pattern Matching" CXX)

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

# Add "-march=native" only if it is supported
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

set(CMAKE_CXX_FLAGS "-Wall -std=c++14")
set(CMAKE_CXX_FLAGS_DEBUG "-g -pg -O0 -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-flto -O3 -DRELEASE")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-register")

find_package(Boost 1.59.0 REQUIRED COMPONENTS
  program_options unit_test_framework iostreams graph)
find_package(Eigen3 REQUIRED)

include_directories(
  src/
  ${PROJECT_BINARY_DIR}
  ${Boost_INCLUDE_DIRS})

## Config for Main monaa
add_executable(qtpm
  src/main.cc)
target_link_libraries(qtpm
#  profiler
${Boost_PROGRAM_OPTIONS_LIBRARY}
${Boost_GRAPH_LIBRARY})


## Config for Test
enable_testing()

add_executable(unit_test
  test/unit_test.cc
  test/timed_automaton_test.cc
  test/zone_graph_test.cc
  test/dbm_test.cc
  test/semiring_test.cc
  test/warshall_froid_test.cc
  test/robustness_test.cc
  test/quantitative_timed_pattern_matching_test.cc
  test/bellman_ford_test.cc)

target_link_libraries(unit_test
  ${Boost_GRAPH_LIBRARY}
  ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

add_test(NAME unit_test
  COMMAND $<TARGET_FILE:unit_test>
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create the developers manual with Doxygen"
  ${DOXYGEN_FOUND})

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/doc/Doxyfile)
    set(doxy_main_page ${CMAKE_CURRENT_SOURCE_DIR}/doc/main.md)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)
endif()
