# Conway's Game of Life Simulator (OpenGL & C++) / 康威生命游戏模拟器

A real-time, interactive Conway's Game of Life simulator built with **C++17** and **OpenGL 3.3 (Core Profile)**. 

这是一个使用 **C++17** 和 **OpenGL 3.3 (Core Profile)** 构建的实时、交互式康威生命游戏（Conway's Game of Life）模拟器。

---

## Features / 功能特点

- **Interactive Cell Painting**: Directly draw new live cells on the grid using the mouse left click/drag.
  **交互式细胞绘制**：支持使用鼠标左键点击或拖拽直接在网格中绘制并激活存活细胞。
- **Smooth Camera Navigation**: Move around the grid with right-click drag and zoom in/out with the scroll wheel.
  **平滑相机导航**：支持鼠标右键拖拽平移视野，以及鼠标滚轮无级缩放。
- **Pause & Resume**: Stop the simulation to draw custom patterns, and resume whenever you want.
  **暂停与恢复**：随时暂停模拟进行图案设计，然后再恢复模拟运行。
- **Clean Restructured Layout**: Clean structure optimized for easy GitHub hosting, with standard CMake configuration.
  **清晰的目录结构**：为 GitHub 托管进行了结构重构优化，配备标准的 CMake 构建配置。

---

## Control Guide / 控件操作说明

| Control / 操作 | Action (English) | 动作 (中文) |
| :--- | :--- | :--- |
| **Mouse Left Click / Drag** | Draw / Activate cells | 鼠标左键点击/拖拽：绘制/激活存活细胞 |
| **Mouse Right Drag** | Pan camera view | 鼠标右键按住拖拽：平移摄像机视野 |
| **Mouse Scroll Wheel** | Zoom camera view | 鼠标滚轮滚动：放大/缩小视野 |
| **Space Bar (空格键)** | Toggle Pause / Resume | 切换 暂停 / 恢复 模拟 |
| **C Key (C键)** | Clear all cells | 清空整个网格（全部细胞死亡） |
| **ESC Key (ESC键)** | Close the simulator | 关闭并退出模拟器 |

---

## Directory Structure / 目录结构

```text
├── CMakeLists.txt     # CMake build configuration / CMake 构建配置文件
├── .gitignore         # Git ignore rules / Git 忽略规则文件
├── README.md          # Project documentation (this file) / 项目说明文档（本文件）
├── src/               # Game source code / 游戏核心源码
│   ├── main.cpp       # Program entry point / 程序入口
│   ├── Camera.hpp     # 2D Orthographic Camera / 2D 正交相机实现
│   ├── Game.hpp       # Life Game rules & state manager / 生命游戏规则与状态管理器
│   ├── GraphicsManager.hpp # OpenGL VAO/VBO wrapper / OpenGL 顶点缓冲管理
│   ├── Shader.hpp     # GLSL Shader loader & compiler / 着色器加载与编译器
│   └── shaders/       # GLSL shaders (Vertex/Fragment) / GLSL 着色器源码
│       ├── shader1.vs # Vertex shader / 顶点着色器
│       └── shader1.fs # Fragment shader / 片元着色器
├── deps/              # Library dependencies / 第三方依赖库
│   ├── glad.c         # GLAD OpenGL loader / GLAD 载入器
│   ├── include/       # Include directories (GLFW, GLM, glad, KHR) / 头文件目录
│   └── lib/           # Pre-compiled static libraries (glfw3.lib) / 预编译静态库目录
└── bin/               # Compiled binaries output / 编译输出程序目录 (自动生成)
    ├── GameOfLife.exe # Executable / 可执行程序
    └── Source/shaders # Copied shaders / 自动复制的着色器文件
```

---

## Build and Run Instructions / 编译与运行说明

### Prerequisites / 前置条件

1. **CMake** (Version 3.15 or newer / 3.15 及以上版本)
2. **C++17 Compiler** (MSVC 2019+ on Windows, GCC 8+ or Clang 7+ on Linux/macOS)
3. **OpenGL** supported hardware and drivers

---

### Windows (Visual Studio) Build / Windows (使用 VS) 编译步骤

Open a terminal (Command Prompt or PowerShell) in the project root directory and run:

在项目根目录下打开终端（命令提示符或 PowerShell），运行以下命令：

1. **Create build directory / 创建构建目录**:
   ```bash
   mkdir build
   ```

2. **Configure project with CMake / 使用 CMake 配置项目**:
   ```bash
   cmake -B build -S .
   ```

3. **Build the project / 编译项目**:
   ```bash
   cmake --build build --config Release
   ```

4. **Run the game / 运行游戏**:
   Once built, the executable will be output to the `bin/` directory, and shaders will be copied automatically:
   
   编译完成后，可执行程序会自动生成至根目录下的 `bin/` 文件夹，着色器文件也会自动复制至对应位置。运行以下命令即可：
   ```bash
   # From project root / 在项目根目录下运行
   ./bin/GameOfLife.exe
   ```

---

### Linux / macOS Build (Optional compatibility) / Linux 与 macOS 编译（可选兼容说明）

If you wish to build on other platforms, install GLFW and OpenGL development packages using your package manager (e.g., `apt install libglfw3-dev` on Ubuntu, or `brew install glfw` on macOS). Then build with:

若想在 Linux 或 macOS 上进行编译，请确保已通过包管理器安装了 GLFW 和 OpenGL 库（例如 Ubuntu 使用 `apt install libglfw3-dev`，macOS 使用 `brew install glfw`）。之后在项目根目录运行：

```bash
mkdir build
cmake -B build -S .
cmake --build build --config Release
# Run / 运行
./bin/GameOfLife
```
