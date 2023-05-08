cmake_minimum_required(VERSION 3.5)

project(japan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set (SOURCE
    main.cpp helper.h geo.txt
    quake.txt classes.h
)

add_executable(japan ${SOURCE})

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 3.3 REQUIRED)

include_directories (
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS})

target_link_libraries(japan
    ${OPENGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    glfw)

install(TARGETS japan
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
