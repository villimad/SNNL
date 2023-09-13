cmake_minimum_required(VERSION 3.26)

project(SNNL)

include_directories("${CMAKE_SOURCE_DIR}/headers")
file(GLOB SOURCES 
     headers/*.cpp
    )
add_executable(neural "${SOURCES}" main.cpp)

# cmake .\CMakeLists.txt
# cmake --build .