
file(GLOB interfaces_SRC
    "*.h"
    "*.cpp"
)

add_library(interfaces ${interfaces_SRC})
