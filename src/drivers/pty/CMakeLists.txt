
file(GLOB pty_SRC
    "*.h"
    "*.cpp"
)

add_library(pty ${pty_SRC})

target_link_libraries (pty core)
target_link_libraries (pty eventwrapper)
target_link_libraries (pty hal)
target_link_libraries (pty utility)
