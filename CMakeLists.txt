cmake_minimum_required (VERSION 3.2)
project (serial_net C CXX)

# SET (CMAKE_CXX_COMPILER "g++-4.9")
SET (CMAKE_CXX_FLAGS "-std=c++14 -pthread -Wall -ggdb")
# "-Wsuggest-override -Wsuggest-final-methods -Wsuggest-final-types")

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(external/GSL)

add_subdirectory(src)
add_subdirectory(external/googletest/googletest)

add_subdirectory(utest)

