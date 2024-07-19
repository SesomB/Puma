cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    puma-control-plane.cpp
)

# Add header files
set(HEADERS
    puma-control-plane.hpp
)

# Create the library
add_library(PumaControlPlane STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(PumaControlPlane PUBLIC 
    spdlog
    Pcap++
    Configuration 
    PumaDataPlane
    )
