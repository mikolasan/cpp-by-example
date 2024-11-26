if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    message(STATUS "Using SDL2 ${SDL2_VERSION}")
endif(SDL2_FOUND)

find_package(OpenGL REQUIRED)
if(OpenGL_FOUND)
    message(STATUS "OpenGL found!")
    message(STATUS "OpenGL include directories: ${OPENGL_INCLUDE_DIR}")
    message(STATUS "OpenGL libraries: ${OPENGL_gl_LIBRARY}")
else()
    message(FATAL_ERROR "OpenGL not found!")
endif()

add_executable(nodes_example
    imgui/imgui.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_draw.cpp
    imgui/backends/imgui_impl_opengl3.cpp
    imgui/backends/imgui_impl_sdl2.cpp
    imnodes/imnodes.cpp
    hello.cpp
    nodes_example.cpp
)

target_include_directories(nodes_example 
    PRIVATE imgui imgui/backends imnodes
)
target_link_libraries(nodes_example OpenGL::GL ${SDL2_LIBRARIES})