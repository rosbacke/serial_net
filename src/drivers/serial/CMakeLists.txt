
file(GLOB serial_SRC
    "*.h"
    "*.cpp"
)

add_library(serial ${serial_SRC})

target_link_libraries (serial reactcpp)
target_link_libraries (serial hal)
