
file(GLOB hw_test_SRC
    "*.h"
    "*.cpp"
)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(hw_test ${hw_test_SRC})
target_link_libraries (hw_test interfaces)
target_link_libraries (hw_test boost_program_options)
target_link_libraries (hw_test ev)
target_link_libraries (hw_test serial)
target_link_libraries (hw_test utility)
target_link_libraries (hw_test hal)

