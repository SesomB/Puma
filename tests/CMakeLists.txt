cmake_minimum_required(VERSION 3.12)

project(Puma-tests)
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR}/src/common)
# Add source files
set(SOURCES
    UnitTests/TestTemplate.cpp
    UnitTests/ContiguousLinkedListTest.cpp
    UnitTests/LinkedArrayTests.cpp
    UnitTests/InteractiveCliTest.cpp
    UnitTests/HyperScanTest.cpp
    UnitTests/DPDKTimerTest.cpp
)

# Add header files
set(HEADERS

)


add_executable("${PROJECT_NAME}" "${SOURCES}" "${HEADERS}" main.cpp)

target_link_libraries("${PROJECT_NAME}" PUBLIC 
    gtest
    cli
    ContiguousLinkedList
    DPDKAllocator
    LinkedArray
    CLIManager
    MockedClasses
    hs
    DPDKTimer
    )