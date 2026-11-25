cmake_minimum_required(VERSION 3.15)
project(GameOfLife LANGUAGES C CXX)

# Require C++17 for the application.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tell MSVC to read UTF-8 source files and string literals correctly.
if(MSVC)
    add_compile_options(/utf-8)
endif()

# Sources and headers for the executable.
set(SOURCE_FILES
    "src/main.cpp"
    "src/Camera.hpp"
    "src/Game.hpp"
    "src/GraphicsManager.hpp"
    "src/Shader.hpp"
    "deps/glad.c"
)

# Build the desktop executable.
add_executable(GameOfLife ${SOURCE_FILES})

# Embed the shaders so their source is independent of the working directory.
set(VERTEX_SHADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shader1.vs")
set(FRAGMENT_SHADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shader1.fs")
# Regenerate the embedded sources whenever either shader changes.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${VERTEX_SHADER_FILE}" "${FRAGMENT_SHADER_FILE}")
file(READ "${VERTEX_SHADER_FILE}" VERTEX_SHADER_SOURCE)
file(READ "${FRAGMENT_SHADER_FILE}" FRAGMENT_SHADER_SOURCE)
configure_file(src/shaders/GameShaders.hpp.in generated/GameShaders.hpp @ONLY)
target_include_directories(GameOfLife PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

# Keep Debug and Release executables in separate directories.
set_target_properties(GameOfLife PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/$<CONFIG>"
)

# Build a pinned GLFW release on Windows and use installed packages elsewhere.
if(WIN32)
    include(FetchContent)
    if(POLICY CMP0135)
        cmake_policy(SET CMP0135 NEW)
    endif()
    set(GLFW_LIBRARY_TYPE STATIC)
    set(GLFW_BUILD_DOCS OFF)
    set(GLFW_INSTALL OFF)
    FetchContent_Declare(glfw
        URL https://codeload.github.com/glfw/glfw/tar.gz/refs/tags/3.4
        URL_HASH SHA256=c038d34200234d071fae9345bc455e4a8f2f544ab60150765d7704e08f3dac01
        TLS_VERIFY ON
    )
    FetchContent_MakeAvailable(glfw)
    target_link_libraries(GameOfLife PRIVATE
        glfw
        opengl32
    )
else()
    # Resolve GLFW and OpenGL through their CMake targets on Linux and macOS.
    find_package(glfw3 3.4 CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(GameOfLife PRIVATE
        glfw
        OpenGL::GL
        # GLAD's loader calls dlopen on Unix platforms.
        ${CMAKE_DL_LIBS}
    )
endif()

