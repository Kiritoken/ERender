cmake_minimum_required(VERSION 3.13)
project(ERender)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

#Required packages
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)
find_package(CUDA REQUIRED)
find_package(Thrust REQUIRED)
find_package(GLFW REQUIRED)

#link
LINK_LIBRARIES(glfw3)
LINK_LIBRARIES(X11)
LINK_LIBRARIES(Xxf86vm)
LINK_LIBRARIES(Xrandr)
LINK_LIBRARIES(pthread)
LINK_LIBRARIES(Xi)
LINK_LIBRARIES(Xcursor)
LINK_LIBRARIES(Xinerama)
LINK_LIBRARIES(GL)
LINK_LIBRARIES(GLU)
LINK_LIBRARIES(dl)

add_executable(ERender main.cpp)