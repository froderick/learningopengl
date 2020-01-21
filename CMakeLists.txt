cmake_minimum_required(VERSION 3.10.2)
project(learnopengl CXX)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-DDEBUG=1 -DSHADER_DIR="${CMAKE_CURRENT_LIST_DIR}")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

add_executable(hello-window hello-window.cpp stb_image.cpp)
target_link_libraries(hello-window glew glfw glm ${OPENGL_gl_LIBRARY})
