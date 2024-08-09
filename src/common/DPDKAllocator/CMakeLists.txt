cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    DPDKAllocator.cpp
)

# Add header files
set(HEADERS
    DPDKAllocator.hpp
)

# Create the library
add_library(DPDKAllocator STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(DPDKAllocator PUBLIC 
    Pcap++)
