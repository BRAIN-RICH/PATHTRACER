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
include_directories(Source)
find_package(OpenMP REQUIRED)

pybind11_add_module(raytracer
        include/tinytiff/tinytiffreader.cpp
        include/tinytiff/tinytiffreader.h
        Source/boundingbox.cpp
        Source/boundingbox.h
        Source/camera.h
        Source/enums.h
        Source/geometry.h
        Source/intersection.h
        Source/light.h
        Source/material.h
        Source/math_utils.h
        Source/math_vector_utils.h
        Source/main.cpp
        Source/octree.cpp
        Source/octree.h
        Source/object.h
        Source/pathtrace.h
        Source/ray.h
        Source/raycast.cpp
        Source/raycast.h
        Source/raytrace.cpp
        Source/raytrace.h
        Source/sampler.h
        Source/scatterevent.h
        Source/scene.h
        Source/SDLauxiliary.h
        Source/settings.h
        Source/texture.h
        Source/transferfunction.h
        Source/triangle.cpp
        Source/triangle.h
        Source/utils.cpp
        Source/utils.h
        Source/volume.cpp
        Source/volume.h
        Source/vector_type.h 
        Source/enums.h)
target_link_libraries(raytracer PRIVATE
		${SDL2_LIBRARIES}
)
if(OpenMP_CXX_FOUND)
    target_link_libraries(raytracer PUBLIC OpenMP::OpenMP_CXX)
endif()
