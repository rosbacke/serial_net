file(GLOB drivers_serial_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(drivers_serial_test ${drivers_serial_test_SRC})

target_link_libraries(drivers_serial_test 
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
)

target_link_libraries (drivers_serial_test gtest)
target_link_libraries (drivers_serial_test eventwrapper)
target_link_libraries (drivers_serial_test serial)
target_link_libraries (drivers_serial_test utility)



add_test( drivers_serial_test drivers_serial_test)
