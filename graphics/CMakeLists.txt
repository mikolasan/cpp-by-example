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

# nodes

add_executable(nodes_example
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/backends/imgui_impl_sdl2.cpp
    nodes/imnodes/imnodes.cpp
    nodes/hello.cpp
    nodes/nodes_example.cpp
)

target_include_directories(nodes_example 
    PRIVATE imgui imgui/backends imnodes
)
target_link_libraries(nodes_example OpenGL::GL ${SDL2_LIBRARIES})

# neurons

add_subdirectory(thirdparty/dirent)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    # bx requirements
    add_compile_options(/Zc:__cplusplus /Zc:preprocessor)
endif ()

add_compile_definitions(BX_CONFIG_DEBUG=0)
add_compile_definitions(ENTRY_CONFIG_USE_SDL=1)
add_compile_definitions(IMGUI_INCLUDE_IMGUI_USER_H)
add_compile_definitions(IMGUI_INCLUDE_IMGUI_USER_INL)

add_executable(neurons_cli
    neurons/network.hpp
    neurons/neurons_cli.cpp
)

add_executable(neurons_gui
    bgfx_ext/bgfx_utils.cpp
    bgfx_ext/entry/cmd.cpp
    bgfx_ext/entry/dialog.cpp
    bgfx_ext/entry/entry.cpp
    bgfx_ext/entry/entry_sdl.cpp
    bgfx_ext/entry/input.cpp
    bgfx_ext/imgui/bgfx_imgui.cpp
    bgfx_ext/example-glue.cpp
    thirdparty/meshoptimizer/src/allocator.cpp
    thirdparty/meshoptimizer/src/clusterizer.cpp
    thirdparty/meshoptimizer/src/indexanalyzer.cpp
    thirdparty/meshoptimizer/src/indexcodec.cpp
    thirdparty/meshoptimizer/src/indexgenerator.cpp
    thirdparty/meshoptimizer/src/overdrawoptimizer.cpp
    thirdparty/meshoptimizer/src/partition.cpp
    thirdparty/meshoptimizer/src/quantization.cpp
    thirdparty/meshoptimizer/src/rasterizer.cpp
    thirdparty/meshoptimizer/src/simplifier.cpp
    thirdparty/meshoptimizer/src/spatialorder.cpp
    thirdparty/meshoptimizer/src/stripifier.cpp
    thirdparty/meshoptimizer/src/vcacheoptimizer.cpp
    thirdparty/meshoptimizer/src/vertexcodec.cpp
    thirdparty/meshoptimizer/src/vertexfilter.cpp
    thirdparty/meshoptimizer/src/vfetchoptimizer.cpp
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/backends/imgui_impl_sdl2.cpp
    neurons/renderer.cpp
    neurons/network.hpp
    neurons/neurons_gui.cpp
)

target_include_directories(neurons_gui PRIVATE 
    thirdparty 
    thirdparty/bgfx/include 
    thirdparty/bimg/include 
    thirdparty/bx/include 
    thirdparty/imgui 
    thirdparty/imgui/backends 
    bgfx_ext
    imgui_ext 
)

if (MSVC)
    set_property(TARGET neurons_gui PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

target_link_directories(neurons_gui PUBLIC 
    thirdparty/bgfx/.build/win64_vs2022/bin)

target_link_libraries(neurons_gui 
    OpenGL::GL 
    ${SDL2_LIBRARIES} 
    dirent
    bgfxRelease bimgRelease bimg_decodeRelease bxRelease
)