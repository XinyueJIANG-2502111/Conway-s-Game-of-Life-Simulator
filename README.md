# Conway's Game of Life Simulator (OpenGL & C++) / 康威生命游戏模拟器

[日本語](README.ja.md)

A real-time, interactive Conway's Game of Life simulator built with **C++17** and **OpenGL 3.3 (Core Profile)**. 

这是一个使用 **C++17** 和 **OpenGL 3.3 (Core Profile)** 构建的实时、交互式康威生命游戏（Conway's Game of Life）模拟器。

---

## Features / 功能特点

- Live cells appear as **glowing teal-green circles** with a slightly warmer center and a soft edge. Dead cells are dark gray.
  存活格子显示为**青绿色的发光圆形**，中心高光略偏黄，亮度向柔和的边缘逐渐衰减。死亡格子显示为深灰色。
- **Interactive Cell Painting**: A left-button press activates a cell immediately. Dragging fills the cells between cursor samples, so fast strokes stay connected. Releasing the button or leaving the window ends the stroke.
  **交互式细胞绘制**：按下左键立即激活格子。快速拖动时会补齐相邻光标采样之间的格子。松开左键或移出窗口会结束当前笔画。
- **Smooth Camera Navigation**: Move around the grid with right-click drag and zoom in/out with the scroll wheel.
  **平滑相机导航**：支持鼠标右键拖拽平移视野，以及鼠标滚轮无级缩放。
- **Pause & Resume**: While paused, the app waits for window events before redrawing. You can still paint and clear the board. Resuming continues from the paused generation.
  **暂停与恢复**：暂停时程序等待窗口事件再重绘，仍可绘制或清空盘面。恢复时从暂停的世代继续演化。
- **Minimized Window**: Rendering stops when GLFW reports that the window is minimized or the framebuffer has zero size. If the simulation is running, it waits between generation updates and shows the latest board when restored.
  **最小化窗口**：当 GLFW 报告窗口已最小化或帧缓冲尺寸为零时，程序会停止绘制。若模拟处于运行状态，程序会等待下一代的更新时间，恢复窗口后显示最新盘面。
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
├── README.ja.md       # Japanese documentation / 日语说明文档
├── src/               # Game source code / 游戏核心源码
│   ├── main.cpp       # Program entry point / 程序入口
│   ├── Camera.hpp     # 2D Orthographic Camera / 2D 正交相机实现
│   ├── Game.hpp       # Life Game rules & state manager / 生命游戏规则与状态管理器
│   ├── GraphicsManager.hpp # OpenGL VAO/VBO wrapper / OpenGL 顶点缓冲管理
│   ├── Shader.hpp     # GLSL Shader loader & compiler / 着色器加载与编译器
│   └── shaders/       # GLSL shaders (Vertex/Fragment) / GLSL 着色器源码
│       ├── shader1.vs # Vertex shader / 顶点着色器
│       ├── shader1.fs # Fragment shader / 片元着色器
│       └── GameShaders.hpp.in # Shader embedding template / 着色器嵌入模板
├── deps/              # Library dependencies / 第三方依赖库
│   ├── glad.c         # GLAD OpenGL loader / GLAD 载入器
│   └── include/       # Include directories (GLM, glad, KHR) / 头文件目录
└── bin/               # Compiled binaries output / 编译输出程序目录 (自动生成)
    ├── Debug/         # Debug executable / 调试版可执行程序
    └── Release/       # Release executable / 发布版可执行程序
```

---

## Build and Run Instructions / 编译与运行说明

### Prerequisites / 前置条件

1. **CMake** (Version 3.15 or newer / 3.15 及以上版本)
2. **C++17 Compiler** (MSVC 2019+ on Windows, GCC 8+ or Clang 7+ on Linux/macOS)
3. **OpenGL 3.3 Core Profile** supported hardware and drivers / 支持 OpenGL 3.3 核心模式的硬件及驱动
4. **GLFW 3.4 or later in the 3.x series** / GLFW 3.4 或更新的 3.x 版本

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
   On the first configure, CMake downloads and verifies GLFW 3.4. The build compiles it from source, so no separate `glfw3.lib` installation is required. The initial download requires Internet access.

   首次配置时，CMake 会下载 GLFW 3.4 并校验源码包，随后从源码编译。首次下载需要联网，无需单独安装 `glfw3.lib`。

3. **Build the project / 编译项目**:
   ```bash
   cmake --build build --config Release
   ```
   Use `cmake --build build --config Debug` to build the Debug executable.

   调试版可使用 `cmake --build build --config Debug` 构建。

4. **Run the game / 运行游戏**:
   Release and Debug executables are written to `bin/Release/GameOfLife.exe` and `bin/Debug/GameOfLife.exe`, respectively. Shaders are embedded, so either executable can be launched from any working directory.

   发布版和调试版分别输出到 `bin/Release/GameOfLife.exe` 和 `bin/Debug/GameOfLife.exe`。着色器已嵌入可执行文件，可以从任意工作目录启动。
   ```bash
   # From project root / 在项目根目录下运行
   ./bin/Release/GameOfLife.exe
   ```

---

### Linux / macOS Build (Optional compatibility) / Linux 与 macOS 编译（可选兼容说明）

Install OpenGL development files and a GLFW 3.x development package that is at least version 3.4 and provides a CMake package configuration. If your package manager offers only GLFW 3.3, install a supported version and point CMake to its installation prefix with `-DCMAKE_PREFIX_PATH=<prefix>`.

请安装 OpenGL 开发文件，以及包含 CMake 包配置的 GLFW 3.4 或更新的 3.x 开发包。如果系统软件源只提供 GLFW 3.3，请另行安装受支持的版本，并用 `-DCMAKE_PREFIX_PATH=<prefix>` 指定其安装前缀。然后在项目根目录运行以下命令。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
# Run / 运行
./bin/Release/GameOfLife
```

GLFW 3.4's Wayland backend does not report window minimization. On that backend, rendering pauses when the framebuffer size reaches zero.

GLFW 3.4 的 Wayland 后端不会报告窗口的最小化状态。在该后端上，帧缓冲尺寸为零时程序才会因窗口不可绘制而暂停渲染。
