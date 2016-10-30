
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_subdirectory(core)
add_subdirectory(master)
add_subdirectory(drivers)
add_subdirectory(mains)
add_subdirectory(utility)
add_subdirectory(interfaces)
add_subdirectory(hal)
