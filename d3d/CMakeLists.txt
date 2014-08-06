cmake_minimum_required(VERSION 2.8)

project(D3D)

include_directories("${PROJECT_SOURCE_DIR}/source")

# This is only required for the script to work in the version control
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")
 
find_package(OpenCL REQUIRED)
 
include_directories(${OPENCL_INCLUDE_DIRS})
 
if(OPENCL_HAS_CPP_BINDINGS)
    message(STATUS "OpenCL has CPP bindings. Good! Full include is: " ${OPENCL_INCLUDE_DIRS})
else(OPENCL_HAS_CPP_BINDINGS)
    message(STATUS "No OpenCL CPP bindings found. These are required!")
endif(OPENCL_HAS_CPP_BINDINGS)

find_package(SFML 2 REQUIRED system window graphics network audio)
 
include_directories(${SFML_INCLUDE_DIR})

find_package(GLEW REQUIRED)
 
include_directories(${GLEW_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
 
include_directories(${OPENGL_INCLUDE_DIR})

find_package(Bullet REQUIRED)
 
include_directories(${BULLET_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
    "*.cpp"
)

add_library(d3d-static STATIC ${SOURCES})
add_library(d3d-shared SHARED ${SOURCES})

target_link_libraries(d3d-static ${OPENCL_LIBRARIES})
target_link_libraries(d3d-static ${SFML_LIBRARIES})
target_link_libraries(d3d-static ${GLEW_LIBRARIES})
target_link_libraries(d3d-static ${OPENGL_LIBRARIES})
target_link_libraries(d3d-static debug ${BULLET_BulletDynamics_LIBRARY_debug} debug ${BULLET_BulletSoftBody_LIBRARY_debug} debug ${BULLET_BulletSoftBody_LIBRARY_debug} debug ${BULLET_BulletCollision_LIBRARY_debug} debug ${BULLET_BulletMultiThreaded_LIBRARY_debug} debug ${BULLET_LinearMath_LIBRARY_debug})
target_link_libraries(d3d-static optimized ${BULLET_BulletDynamics_LIBRARY} optimized ${BULLET_BulletSoftBody_LIBRARY} optimized ${BULLET_BulletSoftBody_LIBRARY} optimized ${BULLET_BulletCollision_LIBRARY} optimized ${BULLET_BulletMultiThreaded_LIBRARY} optimized ${BULLET_LinearMath_LIBRARY})

target_link_libraries(d3d-shared ${OPENCL_LIBRARIES})
target_link_libraries(d3d-shared ${GLEW_LIBRARIES})
target_link_libraries(d3d-shared ${SFML_LIBRARIES})
target_link_libraries(d3d-shared ${OPENGL_LIBRARIES})
target_link_libraries(d3d-shared debug ${BULLET_BulletDynamics_LIBRARY_debug} debug ${BULLET_BulletSoftBody_LIBRARY_debug} debug ${BULLET_BulletSoftBody_LIBRARY_debug} debug ${BULLET_BulletCollision_LIBRARY_debug} debug ${BULLET_BulletMultiThreaded_LIBRARY_debug} debug ${BULLET_LinearMath_LIBRARY_debug})
target_link_libraries(d3d-shared optimized ${BULLET_BulletDynamics_LIBRARY} optimized ${BULLET_BulletSoftBody_LIBRARY} optimized ${BULLET_BulletSoftBody_LIBRARY} optimized ${BULLET_BulletCollision_LIBRARY} optimized ${BULLET_BulletMultiThreaded_LIBRARY} optimized ${BULLET_LinearMath_LIBRARY})

