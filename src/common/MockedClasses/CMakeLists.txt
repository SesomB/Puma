cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    LogManager.cpp
)

# Add header files
set(HEADERS
    LogManager.hpp
)

# Create the library
add_library(MockedClasses STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(MockedClasses PUBLIC 
    spdlog
    )
