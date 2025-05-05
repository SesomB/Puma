cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
SessionHandler.cpp
)

# Add header files
set(HEADERS
SessionHandler.hpp
)

# Create the library
add_library(SessionHandler STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(SessionHandler PUBLIC 
    spdlog
    )
