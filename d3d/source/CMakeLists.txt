cmake_minimum_required(VERSION 2.8)

project(D3D)

include_directories("${PROJECT_SOURCE_DIR}/source")

# This is only required for the script to work in the version control
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
 
find_package(OpenCL REQUIRED)
 
include_directories(${OPENCL_INCLUDE_DIRS})
 
if(OPENCL_HAS_CPP_BINDINGS)
    message("OpenCL has CPP bindings. Good! Full include is: " ${OPENCL_INCLUDE_DIRS})
else(OPENCL_HAS_CPP_BINDINGS)
    message("No OpenCL CPP bindings found. These are required!")
endif(OPENCL_HAS_CPP_BINDINGS)

find_package(SFML 2 REQUIRED system window graphics network audio)
 
include_directories(${SFML_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
 
include_directories(${OPENGL_INCLUDE_DIR})

find_package(Bullet REQUIRED)
 
include_directories(${BULLET_INCLUDE_DIR})

add_library(engine OBJECT "${PROJECT_SOURCE_DIR}/source")
add_library(static STATIC $<TARGET_OBJECTS:engine>)
add_library(shared SHARED $<TARGET_OBJECTS:engine>)

target_link_libraries(D3D ${OPENCL_LIBRARIES})
target_link_libraries(D3D ${SFML_LIBRARIES})
target_link_libraries(D3D ${OPENGL_LIBRARIES})
target_link_libraries(D3D ${BULLET_LIBRARIES})