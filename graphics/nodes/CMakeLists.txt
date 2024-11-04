
find_package(SDL2 CONFIG REQUIRED)

if (SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIRS})
    message(STATUS "Using SDL2 ${SDL2_VERSION}")
endif(SDL2_FOUND)

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
target_link_libraries(nodes_example GL ${SDL2_LIBRARIES})