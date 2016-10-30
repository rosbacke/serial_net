
file(GLOB hal_SRC
    "*.h"
    "*.cpp"
)

add_library(hal ${hal_SRC})
