
file(GLOB statechart_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(statechart_test ${statechart_test_SRC})

target_link_libraries (statechart_test statechart)
target_link_libraries (statechart_test gtest)

add_test( statechart_test statechart_test)
