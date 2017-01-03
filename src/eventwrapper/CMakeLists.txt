
file(GLOB eventwrapper_SRC
    "*.h"
    "*.cpp"
)

add_library(eventwrapper ${eventwrapper_SRC})

TARGET_LINK_LIBRARIES(eventwrapper ev)

