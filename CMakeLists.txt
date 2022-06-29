cmake_minimum_required(VERSION 3.10)
#设置项目名称
project(raytracer VERSION 0.1)
#添加子模块pybind11
add_subdirectory(pybind11)

if(UNIX)
    message("current platform: Linux ")
    find_package(SDL2 REQUIRED)
    message(STATUS ${SDL2_INCLUDE_DIRS})
elseif(WIN32)
    message("current platform: Windows ")
    set(SDL2_INCLUDE_DIRS "include/sdl/include/SDL2")
    set(SDL2_LIBRARIES "../SDL2")
else()
    message("current platform: unkonw ")
endif()
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(include/tinytiff)
include_directories(include)
find_package(OpenMP REQUIRED)

pybind11_add_module(raytracer
        include/tinytiff/tinytiffreader.cpp
        Source/boundingbox.cpp
        Source/main.cpp
        Source/octree.cpp
        Source/raycast.cpp
        Source/raytrace.cpp
        Source/triangle.cpp
        Source/utils.cpp
        Source/volume.cpp
        )
target_link_libraries(raytracer PRIVATE
		${SDL2_LIBRARIES}
)
if(OpenMP_CXX_FOUND)
    target_link_libraries(raytracer PUBLIC OpenMP::OpenMP_CXX)
endif()
