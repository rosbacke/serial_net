
file(GLOB serialtest_SRC
    "*.h"
    "*.cpp"
)

add_executable(serialecho ${serialtest_SRC})
target_link_libraries (serialecho ev)
target_link_libraries (serialecho eventwrapper)
target_link_libraries(serialecho serial utility)
target_link_libraries (serialecho reactcpp)
