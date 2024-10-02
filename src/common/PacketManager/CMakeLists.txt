cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    CustomPacket.cpp
    PacketManager.cpp
    RandomGenerator.cpp
)

# Add header files
set(HEADERS
    CustomPacket.hpp
    PacketManager.hpp
    RandomGenerator.hpp
)

# Create the library
add_library(PacketManager STATIC ${SOURCES} ${HEADERS})


# Link against external libraries
target_link_libraries(PacketManager PUBLIC 
    spdlog
    Pcap++
    )
