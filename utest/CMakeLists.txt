include_directories(../external/googletest/googletest/include)

find_package(Boost COMPONENTS regex system filesystem REQUIRED)

add_subdirectory(statechart)
add_subdirectory(eventwrapper)
add_subdirectory(serialtest)
add_subdirectory(hal)
add_subdirectory(drivers)
add_subdirectory(core)
add_subdirectory(master)

