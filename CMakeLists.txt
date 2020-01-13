cmake_minimum_required(VERSION 3.10.2)
project(learnopengl CXX)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

add_definitions("-DDEBUG=1")
find_package(OpenGL REQUIRED)

add_executable(hello-window hello-window.cpp)
target_link_libraries(hello-window glew glfw ${OPENGL_gl_LIBRARY})
