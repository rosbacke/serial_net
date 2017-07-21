# This is a first try. Still has some weird even/odd behaviour when building with ninja.


file(GLOB factories_SRC
    "*.h"
    "*.cpp"
)

add_library(factories ${factories_SRC})
