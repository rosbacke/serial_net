# This is a first try. Still has some weird even/odd behaviour when building with ninja.

add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/version_string.h ALL
  COMMAND "${CMAKE_SOURCE_DIR}/tools/get_git_version.sh" ${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/version_string.h
  #BYPRODUCTS ${CMAKE_CURRENT_BINARY_DIR}/version_string.h
)


add_custom_target(version_string ALL
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/version_string.h
)


file(GLOB utility_SRC
	${CMAKE_CURRENT_BINARY_DIR}/version_string.h
    "*.h"
    "*.cpp"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(utility ${utility_SRC})

add_dependencies(utility version_string)
