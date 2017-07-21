
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Allow pre-generated code to be found. (e.g. version_string.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(core)
add_subdirectory(master)
add_subdirectory(drivers)
add_subdirectory(mains)
add_subdirectory(utility)
add_subdirectory(interfaces)
add_subdirectory(hal)
add_subdirectory(statechart)
add_subdirectory(eventwrapper)
add_subdirectory(factories)
