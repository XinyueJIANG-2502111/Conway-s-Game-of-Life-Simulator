cmake_minimum_required(VERSION 3.15)
project(GameOfLife LANGUAGES C CXX)

# C++標準を設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC向けのUTF-8コンパイルオプションを設定（文字化け・ビルドエラー防止）
if(MSVC)
    add_compile_options(/utf-8)
endif()

# ソースファイルとヘッダーファイルの定義
set(SOURCE_FILES
    "src/main.cpp"
    "src/Camera.hpp"
    "src/Game.hpp"
    "src/GraphicsManager.hpp"
    "src/Shader.hpp"
    "deps/glad.c"
)

# 可実行ファイルの作成
add_executable(GameOfLife ${SOURCE_FILES})

# Embed the shaders so their source is independent of the working directory.
set(VERTEX_SHADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shader1.vs")
set(FRAGMENT_SHADER_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders/shader1.fs")
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

# プラットフォームごとのリンク設定
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
    # 他のプラットフォーム（Mac/Linux）でのビルドも考慮
    find_package(glfw3 3.4 CONFIG REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(GameOfLife PRIVATE
        glfw
        OpenGL::GL
        ${CMAKE_DL_LIBS}
    )
endif()

