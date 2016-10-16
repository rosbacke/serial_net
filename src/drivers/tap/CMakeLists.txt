
file(GLOB tap_SRC
    "*.h"
    "*.cpp"
)

add_library(tap ${tap_SRC})
