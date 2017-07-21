
file(GLOB eventwrapper_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(eventwrapper_test ${eventwrapper_test_SRC})

target_link_libraries (eventwrapper_test eventwrapper)
target_link_libraries (eventwrapper_test ev)
target_link_libraries (eventwrapper_test gtest)
target_link_libraries (eventwrapper_test serial)

add_test( eventwrapper_test eventwrapper_test)
