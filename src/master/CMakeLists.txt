
file(GLOB master_SRC
    "*.h"
    "*.cpp"
)

add_library(master ${master_SRC})

target_link_libraries (master core)
target_link_libraries (master statechart)
target_link_libraries (master utility)
