
file(GLOB core_SRC
    "*.h"
    "*.cpp"
)

add_library(core ${core_SRC} AddressCache.cpp AddressCache.h)
