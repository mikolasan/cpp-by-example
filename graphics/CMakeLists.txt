set(CMAKE_CXX_STANDARD 17)

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

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIRS})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
endif(OPENSSL_FOUND)

# ================================
# CURL MINIMAL CONFIGURATION
# ================================

# Disable all protocols except HTTP/HTTPS
set(CURL_DISABLE_DICT ON CACHE BOOL "Disable DICT protocol")
set(CURL_DISABLE_FILE ON CACHE BOOL "Disable FILE protocol")
set(CURL_DISABLE_FTP ON CACHE BOOL "Disable FTP protocol")
set(CURL_DISABLE_GOPHER ON CACHE BOOL "Disable GOPHER protocol")
set(CURL_DISABLE_IMAP ON CACHE BOOL "Disable IMAP protocol")
set(CURL_DISABLE_LDAP ON CACHE BOOL "Disable LDAP protocol")
set(CURL_DISABLE_LDAPS ON CACHE BOOL "Disable LDAPS protocol")
set(CURL_DISABLE_POP3 ON CACHE BOOL "Disable POP3 protocol")
set(CURL_DISABLE_RTSP ON CACHE BOOL "Disable RTSP protocol")
set(CURL_DISABLE_SMB ON CACHE BOOL "Disable SMB protocol")
set(CURL_DISABLE_SMTP ON CACHE BOOL "Disable SMTP protocol")
set(CURL_DISABLE_TELNET ON CACHE BOOL "Disable TELNET protocol")
set(CURL_DISABLE_TFTP ON CACHE BOOL "Disable TFTP protocol")
set(CURL_DISABLE_MQTT ON CACHE BOOL "Disable MQTT protocol")

# Keep HTTP and HTTPS enabled (these are ON by default)
set(CURL_DISABLE_HTTP OFF CACHE BOOL "Enable HTTP protocol")
set(CURL_DISABLE_HTTPS OFF CACHE BOOL "Enable HTTPS protocol")

# Disable authentication methods not commonly used for web browsing
set(CURL_DISABLE_NTLM ON CACHE BOOL "Disable NTLM auth")
set(CURL_DISABLE_DIGEST_AUTH OFF CACHE BOOL "Keep digest auth for web")
set(CURL_DISABLE_BASIC_AUTH OFF CACHE BOOL "Keep basic auth for web")
set(CURL_DISABLE_BEARER_AUTH OFF CACHE BOOL "Keep bearer auth for APIs")
set(CURL_DISABLE_KERBEROS_AUTH ON CACHE BOOL "Disable Kerberos auth")
set(CURL_DISABLE_NEGOTIATE_AUTH ON CACHE BOOL "Disable negotiate auth")
set(CURL_DISABLE_AWS ON CACHE BOOL "Disable AWS auth")

# Disable features not needed for web browsing
set(CURL_DISABLE_COOKIES OFF CACHE BOOL "Keep cookies for web browsing")
set(CURL_DISABLE_PROXY OFF CACHE BOOL "Keep proxy support")
set(CURL_DISABLE_VERBOSE_STRINGS OFF CACHE BOOL "Keep error strings")
set(CURL_DISABLE_NETRC ON CACHE BOOL "Disable .netrc support")
set(CURL_DISABLE_PROGRESS_METER ON CACHE BOOL "Disable progress meter")
set(CURL_DISABLE_DOH ON CACHE BOOL "Disable DNS over HTTPS")
set(CURL_DISABLE_MIME OFF CACHE BOOL "Keep MIME for form uploads")
set(CURL_DISABLE_PARSEDATE ON CACHE BOOL "Disable date parsing")
set(CURL_DISABLE_SHUFFLE_DNS ON CACHE BOOL "Disable DNS shuffling")
set(CURL_DISABLE_ALTSVC ON CACHE BOOL "Disable Alt-Svc support")
set(CURL_DISABLE_HSTS ON CACHE BOOL "Disable HSTS (can enable if needed)")

# SSL/TLS Configuration - Keep essential features
set(CURL_DISABLE_CRYPTO_AUTH OFF CACHE BOOL "Keep crypto auth for HTTPS")
set(CURL_CA_BUNDLE_SET ON CACHE BOOL "Set CA bundle")
set(CURL_CA_FALLBACK ON CACHE BOOL "Use system CA store fallback")

# Build options
set(BUILD_CURL_EXE OFF CACHE BOOL "Don't build curl executable")
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static library")
set(CURL_STATIC_CRT ON CACHE BOOL "Use static CRT on Windows")
set(ENABLE_MANUAL OFF CACHE BOOL "Disable manual")
set(CURL_DISABLE_INSTALL ON CACHE BOOL "Don't install curl")

# HTTP-specific optimizations
set(CURL_DEFAULT_SSL_BACKEND "openssl")
set(CURL_USE_OPENSSL ON)
set(CURL_USE_MBEDTLS OFF CACHE BOOL "Don't use mbedTLS")
set(CURL_USE_WOLFSSL OFF CACHE BOOL "Don't use wolfSSL")
set(CURL_USE_SCHANNEL OFF CACHE BOOL "Don't use Schannel")
set(CURL_USE_SECTRANSP OFF CACHE BOOL "Don't use SecTransport")

# HTTP/2 and HTTP/3 support (optional - can disable to reduce size further)
set(CURL_USE_NGHTTP2 OFF CACHE BOOL "Disable HTTP/2 support")
set(CURL_USE_NGTCP2 OFF CACHE BOOL "Disable HTTP/3 support")
set(CURL_USE_QUICHE OFF CACHE BOOL "Disable QUICHE HTTP/3")

# Compression support (useful for web content)
set(CURL_ZLIB ON CACHE BOOL "Enable gzip compression")
set(CURL_BROTLI OFF CACHE BOOL "Disable Brotli compression")
set(CURL_ZSTD OFF CACHE BOOL "Disable Zstd compression")

# Disable extra features
set(USE_LIBIDN2 OFF CACHE BOOL "Disable IDN support")
set(CURL_USE_LIBPSL OFF CACHE BOOL "Disable PSL support")
set(CURL_USE_LIBSSH2 OFF CACHE BOOL "Disable SSH support")
set(CURL_USE_LIBSSH OFF CACHE BOOL "Disable SSH support")
set(CURL_USE_GSSAPI OFF CACHE BOOL "Disable GSSAPI")
set(ENABLE_IPV6 ON CACHE BOOL "Keep IPv6 support")
set(ENABLE_UNIX_SOCKETS OFF CACHE BOOL "Disable Unix sockets")
set(ENABLE_THREADED_RESOLVER ON CACHE BOOL "Keep threaded resolver")

# Testing and examples
set(BUILD_TESTING OFF CACHE BOOL "Don't build tests")
set(CURL_DISABLE_TESTS ON CACHE BOOL "Disable tests")

# If you want even smaller size, uncomment these:

# # Disable all optional features
# set(CURL_DISABLE_COOKIES ON CACHE BOOL "Disable cookies")
# set(CURL_DISABLE_PROXY ON CACHE BOOL "Disable proxy")
# set(CURL_DISABLE_VERBOSE_STRINGS ON CACHE BOOL "Disable error strings")
# set(CURL_ZLIB OFF CACHE BOOL "Disable compression")
# set(ENABLE_IPV6 OFF CACHE BOOL "Disable IPv6")
# set(ENABLE_THREADED_RESOLVER OFF CACHE BOOL "Disable threaded resolver")

add_subdirectory(thirdparty/curl)

# For HTTPS to work, might need to set certificate bundle path:
# Download Mozilla's CA bundle:
# `curl -o cacert.pem https://curl.se/ca/cacert.pem
set(CURL_CA_BUNDLE "cacert.pem")

add_executable(parser_test 
    html/html_parser.cpp
    html/parser_test.cpp
)

add_executable(simple_html WIN32
    thirdparty/imgui/imgui.cpp
    thirdparty/imgui/imgui_widgets.cpp
    thirdparty/imgui/imgui_tables.cpp
    thirdparty/imgui/imgui_draw.cpp
    thirdparty/imgui/backends/imgui_impl_opengl3.cpp
    thirdparty/imgui/backends/imgui_impl_sdl2.cpp
    html/html_parser.cpp
    html/html_renderer.cpp
    html/app.cpp
    html/main.cpp
)

target_compile_definitions(simple_html PRIVATE
    CURL_STATICLIB
)

target_include_directories(simple_html PRIVATE
    thirdparty/imgui
    thirdparty/imgui/backends
    thirdparty/curl/include
)
target_link_libraries(simple_html 
    OpenGL::GL
    ${SDL2_LIBRARIES}
    libcurl
)

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

if (MSVC)
    # bx requirements
    add_compile_options(/Zc:__cplusplus /Zc:preprocessor)
endif ()

add_executable(neurons_cli
    neurons/network.cpp
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
    neurons/network.cpp
    neurons/mnist_data_processor.cpp
    neurons/render/neuron_render.cpp
    neurons/render/network_render.cpp
    # neurons/camera.cpp
    # neurons/mouse.cpp
    # neurons/settings.cpp
    neurons/neurons_gui.cpp
)

target_compile_definitions(neurons_gui PRIVATE BX_CONFIG_DEBUG=0)
target_compile_definitions(neurons_gui PRIVATE USE_ENTRY=1)
target_compile_definitions(neurons_gui PRIVATE ENTRY_CONFIG_USE_SDL=1)
target_compile_definitions(neurons_gui PRIVATE IMGUI_INCLUDE_IMGUI_USER_H)
target_compile_definitions(neurons_gui PRIVATE IMGUI_INCLUDE_IMGUI_USER_INL)

target_include_directories(neurons_gui PRIVATE 
    thirdparty 
    thirdparty/bgfx/include 
    thirdparty/bimg/include 
    thirdparty/bx/include 
    thirdparty/imgui 
    thirdparty/imgui/backends 
    bgfx_ext
    imgui_ext 
    neurons
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
    bgfx${CMAKE_BUILD_TYPE}
    bimg${CMAKE_BUILD_TYPE}
    bimg_decode${CMAKE_BUILD_TYPE}
    bx${CMAKE_BUILD_TYPE}
)

###############################################

add_executable(brain
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
    neurons/brain.cpp
)

target_compile_definitions(brain PRIVATE BX_CONFIG_DEBUG=0)
target_compile_definitions(brain PRIVATE ENTRY_CONFIG_USE_SDL=1)
target_compile_definitions(brain PRIVATE IMGUI_INCLUDE_IMGUI_USER_H)
target_compile_definitions(brain PRIVATE IMGUI_INCLUDE_IMGUI_USER_INL)

target_include_directories(brain PRIVATE 
    thirdparty 
    thirdparty/bgfx/include 
    thirdparty/bimg/include 
    thirdparty/bx/include 
    thirdparty/imgui 
    thirdparty/imgui/backends 
    bgfx_ext
    imgui_ext 
    neurons
)

if (MSVC)
    set_property(TARGET brain PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif ()

target_link_directories(brain PUBLIC 
    thirdparty/bgfx/.build/win64_vs2022/bin)

target_link_libraries(brain 
    OpenGL::GL 
    ${SDL2_LIBRARIES} 
    dirent
    bgfx${CMAKE_BUILD_TYPE}
    bimg${CMAKE_BUILD_TYPE}
    bimg_decode${CMAKE_BUILD_TYPE}
    bx${CMAKE_BUILD_TYPE}
)