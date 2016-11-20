
file(GLOB statechart_SRC
    "*.h"
    "*.cpp"
)

add_library(statechart ${statechart_SRC})
