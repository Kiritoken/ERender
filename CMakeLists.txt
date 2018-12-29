cmake_minimum_required(VERSION 3.13)
project(ERender)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#Required packages
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(CUDA REQUIRED)
find_package(Thrust REQUIRED)
find_package(GLFW REQUIRED)
find_package(GLEW REQUIRED)

#link
LINK_LIBRARIES(glfw3)
LINK_LIBRARIES(X11)
LINK_LIBRARIES(Xxf86vm)
LINK_LIBRARIES(Xrandr)
LINK_LIBRARIES(pthread)
LINK_LIBRARIES(Xi)
LINK_LIBRARIES(Xcursor)
LINK_LIBRARIES(Xinerama)
LINK_LIBRARIES(GLU)
LINK_LIBRARIES(dl)
LINK_LIBRARIES(GLEW)
LINK_LIBRARIES(GL)

add_executable(ERender main.cpp src/viewer/viewer.cpp
               src/viewer/viewer.h src/render/render.h
               src/render/application.cpp src/render/application.h
               src/camera/camera_info.h src/camera/camera.cpp
               src/camera/camera.h src/utilis/misc.h src/mesh/polymesh.h
               src/obj_parser/obj_parser.cpp src/obj_parser/obj_parser.h src/utilis/timer.h
               src/mesh/mesh.cpp src/mesh/mesh.h src/mesh/polymesh.cpp)