cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
DPDKTimer.cpp
)

# Add header files
set(HEADERS
DPDKTimer.hpp
)

# Create the library
add_library(DPDKTimer STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(DPDKTimer PUBLIC 
    spdlog
    )
