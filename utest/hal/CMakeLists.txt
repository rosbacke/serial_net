
file(GLOB hal_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(hal_test ${hal_test_SRC})

target_link_libraries (hal_test hal)
target_link_libraries (hal_test gtest)

add_test( hal_test hal_test)
