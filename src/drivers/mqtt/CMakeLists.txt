
file(GLOB mqtt_SRC
    "*.h"
    "*.cpp"
)

add_library(mqtt ${mqtt_SRC})
