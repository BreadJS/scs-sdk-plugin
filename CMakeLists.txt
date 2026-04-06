cmake_minimum_required(VERSION 3.10)
project(scs-telemetry VERSION 1.12 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add subdirectories
add_subdirectory(scs-telemetry)

# Installation
install(TARGETS scs-telemetry
    LIBRARY DESTINATION bin
    RUNTIME DESTINATION bin
)
