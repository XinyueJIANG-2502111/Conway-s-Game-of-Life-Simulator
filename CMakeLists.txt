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

# インクルードディレクトリの指定
include_directories(
    "${CMAKE_CURRENT_SOURCE_DIR}/deps/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

# 可実行ファイルの作成
add_executable(GameOfLife ${SOURCE_FILES})

# 可実行ファイルの出力先をプロジェクト内の 「bin」 フォルダに設定
# マルチ構成のジェネレータ（Visual Studioなど）における出力先も統一するため、プロパティを設定
set_target_properties(GameOfLife PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

# ライブラリのリンク設定
# GLFW スタティックライブラリの検索ディレクトリを指定
target_link_directories(GameOfLife PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/deps/lib")

# プラットフォームごとのリンク設定
if(WIN32)
    target_link_libraries(GameOfLife PRIVATE
        glfw3
        opengl32
    )
else()
    # 他のプラットフォーム（Mac/Linux）でのビルドも考慮
    find_package(GLFW3 REQUIRED)
    find_package(OpenGL REQUIRED)
    target_link_libraries(GameOfLife PRIVATE
        glfw
        OpenGL::GL
    )
endif()

# ポストビルドイベントとして、シェーダーディレクトリを実行ファイルの出力先にコピー
add_custom_command(TARGET GameOfLife POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:GameOfLife>/Source/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/src/shaders"
        "$<TARGET_FILE_DIR:GameOfLife>/Source/shaders"
    COMMENT "Copying shaders to output directory..."
)

