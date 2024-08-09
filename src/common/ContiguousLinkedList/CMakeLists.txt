cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    ContiguousLinkedList.cpp
)

# Add header files
set(HEADERS
    ContiguousLinkedList.hpp
)

# Create the library
add_library(ContiguousLinkedList STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(ContiguousLinkedList PUBLIC 
    spdlog
    Pcap++
    )
