file(GLOB drivers_pty_test_SRC
    "*.h"
    "*.cpp"
)

add_executable(drivers_pty_test ${drivers_pty_test_SRC})

target_link_libraries(drivers_pty_test 
  ${Boost_FILESYSTEM_LIBRARY}
  ${Boost_SYSTEM_LIBRARY}
  ${Boost_REGEX_LIBRARY}
)

target_link_libraries (drivers_pty_test gtest)
target_link_libraries (drivers_pty_test eventwrapper)
target_link_libraries (drivers_pty_test serial)
target_link_libraries (drivers_pty_test utility)
target_link_libraries (drivers_pty_test pty)
target_link_libraries (drivers_pty_test hal)
target_link_libraries (drivers_pty_test interfaces)



add_test( drivers_pty_test drivers_pty_test)
