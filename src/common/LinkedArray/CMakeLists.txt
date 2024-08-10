cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    LinkedArray.cpp
)

# Add header files
set(HEADERS
    LinkedArray.hpp
)

# Create the library
add_library(LinkedArray STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(LinkedArray PUBLIC 
    spdlog
    Pcap++
    )
