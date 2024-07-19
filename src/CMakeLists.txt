cmake_minimum_required(VERSION 3.12)

add_subdirectory(configuration)
add_subdirectory(puma-control-plane)
add_subdirectory(puma-data-plane)

add_executable("${PROJECT_NAME}" main.cpp)

target_link_libraries("${PROJECT_NAME}" PUBLIC 
    spdlog 
    argparse 
    Pcap++ 
    PumaControlPlane
    Configuration
    )
