cmake_minimum_required(VERSION 3.12)

# Add source files
set(SOURCES
    configuration.cpp
)

# Add header files
set(HEADERS
    configuration.hpp
)

# Create the library
add_library(Configuration STATIC ${SOURCES} ${HEADERS})

# Link against external libraries
target_link_libraries(Configuration PUBLIC)
