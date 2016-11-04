file(GLOB drivers_serial_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(drivers_serial_test ${drivers_serial_test_SRC})

target_link_libraries (drivers_serial_test serial)
target_link_libraries (drivers_serial_test gtest)
target_link_libraries (drivers_serial_test utility)

add_test( drivers_serial_test drivers_serial_test)