
file(GLOB master_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(master_test ${master_test_SRC})


target_link_libraries (master_test master)
target_link_libraries (master_test hal)
target_link_libraries (master_test eventwrapper)
target_link_libraries (master_test statechart)
target_link_libraries (master_test gtest)
target_link_libraries (master_test utility)

add_test( master_test master_test)
