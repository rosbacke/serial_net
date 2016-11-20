
file(GLOB core_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(core_test ${core_test_SRC})

target_link_libraries (core_test core)
target_link_libraries (core_test utility)
target_link_libraries (core_test gtest)

add_test( core_test core_test)
