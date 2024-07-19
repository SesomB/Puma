cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    puma-data-plane.cpp
)

# Add header files
set(HEADERS
    puma-data-plane.hpp
)

# Create the library
add_library(PumaDataPlane STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(PumaDataPlane PUBLIC 
    Pcap++
    Configuration)
