cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    CLIManager.cpp
)

# Add header files
set(HEADERS
    CLIManager.hpp
)

# Create the library
add_library(CLIManager STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(CLIManager PUBLIC 
    spdlog
    cli
    )
