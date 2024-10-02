cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
PcapManager.cpp

)

# Add header files
set(HEADERS
PcapManager.hpp

)

# Create the library
add_library(PcapManager STATIC ${SOURCES} ${HEADERS})


# Link against external libraries
target_link_libraries(PcapManager PUBLIC 
    spdlog
    Pcap++
    )
