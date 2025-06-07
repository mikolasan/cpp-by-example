set(CMAKE_CXX_STANDARD 17)

# find_package(OpenGL REQUIRED)
# if(OpenGL_FOUND)
#     message(STATUS "OpenGL found!")
#     message(STATUS "OpenGL include directories: ${OPENGL_INCLUDE_DIR}")
#     message(STATUS "OpenGL libraries: ${OPENGL_gl_LIBRARY}")
# else()
#     message(FATAL_ERROR "OpenGL not found!")
# endif()

add_executable(neurons_cli
    network.hpp
    neurons_cli.cpp
)

add_executable(neurons_gui
    renderer.cpp
    network.hpp
    neurons_gui.cpp
)

# target_include_directories(neurons_cli 
#     PRIVATE 
# )
# target_link_libraries(neurons_cli OpenGL::GL)