# ⚫⚪ Gobang Rogue – 五子棋 Roguelike

> "打破定式，每局都是新体验"

**GARYSONG** - 开发与设计

Gobang Rogue is a Gobang game developed using C++ and the EasyX graphics library. The project implements classic two-player local and LAN online battles, and plans to introduce Roguelike elements (card skills, growth system) to make each game session feel fresh and engaging. Currently in the Alpha testing phase, with core functionalities now playable.

Gobang Rogue 是一款基于 C++ 和 EasyX 图形库开发的五子棋游戏。项目实现了经典的双人本地对战与局域网联机对战，并计划引入 Roguelike 元素（卡牌技能、成长系统），让每一局对弈都充满新鲜感。目前处于 Alpha 测试阶段，核心功能已可玩。

---

## 🎮 快速开始（玩家指南）

### 环境要求：
- **操作系统**：Windows 7 及以上
- **无需安装**：直接运行 exe 即可

### 操作说明

#### 本地对战
- **落子**：在棋盘空位上**双击鼠标左键**落子
- **悬停预览**：鼠标移动到空位时显示高亮预览
- **黑棋先行**，双方轮流落子
- 率先在横、竖、斜方向连成五子者获胜

#### 局域网联机
1. **创建服务器（服务器方）**：
   - 选择「多人对战」→「创建服务器」
   - 等待客户端连接，默认端口 `8888`
   - 服务器方执黑棋，先行

2. **加入服务器（客户端方）**：
   - 选择「多人对战」→「加入服务器」
   - 输入服务器 IP 地址和端口，格式：`IP:端口`（例如 `192.168.1.100:8888`）
   - 客户端执白棋，后行

> **提示**：双方需在同一局域网内；可通过 `ipconfig` 查看本机 IP 地址。 **同时，可以通过内网穿透完成互联网联机。**

#### 计时规则
- **回合计时**：每回合 30 秒，超时自动换边
- **总计时**：每方总时间 10 分钟，总时间耗尽判负

#### 游戏内按钮
游戏界面右侧提供以下功能按钮：
| 按钮 | 功能 |
|------|------|
| 🔄 重新开始 | 重置棋盘，开始新一局（仅本地模式） |
| ↩️ 悔棋 | 悔回上一步（开发中） |
| 🎵 音乐 | 开启 / 关闭背景音乐 |
| ⚙️ 设置 | 进入设置页面 |
| 🚪 退出 | 返回主菜单 |

---

## 📦 源码编译指南（开发者）

### 环境要求
- **操作系统**：Windows 10 / 11
- **编译器**：MinGW-w64 GCC (推荐通过 MSYS2 安装)
- **编辑器**：VS Code（可选）

### 技术栈

| 组件 | 技术 / 库 |
|------|-----------|
| 核心语言 | C++17 |
| 图形界面 | EasyX (MinGW 版本) |
| 网络通信 | Winsock2 (TCP) |
| 多媒体 | winmm.dll (MCI 播放背景音乐) |
| 构建环境 | MinGW-w64 (MSYS2) / VS Code |
| 未来计划 | Python (AI 测试)、MySQL (排行榜)、openHiTLS (加密通信) |

### 目录结构
```
Gobang-Rogue/
├── src/                  # 源码文件
│   ├── Main.cpp          # 主程序入口 & 游戏状态机
│   ├── GameLogic.cpp     # 游戏逻辑（判赢、计时、悔棋）
│   ├── GameUI.cpp        # 游戏界面绘制
│   ├── Menu.cpp          # 菜单界面
│   ├── Network.cpp       # 网络通信 (TCP)
│   ├── Utils.cpp         # 工具函数 & 素材加载
│   └── Globals.cpp       # 全局变量定义
├── include/              # 头文件
│   ├── easyx/            # EasyX 库头文件
│   └── *.h               # 各模块头文件
├── lib/                  # 库文件
│   ├── libeasyx.a        # EasyX 静态库 (ANSI)
│   └── libeasyxw.a       # EasyX 静态库 (Unicode)
├── assets/               # 游戏资源
│   ├── audio/            # 背景音乐
│   └── image/            # 图片素材（棋盘、棋子、菜单）
└── Gobang-Rogue.exe      # 编译产物
```

### 快速编译

#### 方式一：VS Code（推荐）
1. 用 VS Code 打开项目文件夹
2. 安装 **C/C++** 扩展
3. 确保 MinGW 已安装并配置好 PATH（或修改 `.vscode/tasks.json` 中的编译器路径）
4. 按 `Ctrl+Shift+B` 执行编译任务
5. 生成的 `Gobang-Rogue.exe` 在项目根目录

#### 方式二：命令行
在项目根目录执行：
```bash
g++ -std=c++17 -Wall -Wno-unknown-pragmas -DUNICODE -D_UNICODE \
    -I include \
    src/*.cpp \
    -o Gobang-Rogue.exe \
    -L lib \
    -leasyxw -lws2_32 -lwinmm -lgdi32 -luser32 -lshcore \
    -static-libgcc -static-libstdc++
```

### 模块说明

| 模块 | 文件 | 职责 |
|------|------|------|
| 主循环 | `Main.cpp` | 游戏状态机，调度各场景 |
| 游戏逻辑 | `GameLogic.cpp` | 判赢算法、计时器、游戏控制 |
| 界面绘制 | `GameUI.cpp` | 棋盘、信息栏、按钮、高亮绘制 |
| 菜单系统 | `Menu.cpp` | 主菜单、多人菜单、说明、设置页面 |
| 网络通信 | `Network.cpp` | Winsock TCP 封装、消息序列化 |
| 工具函数 | `Utils.cpp` | 素材加载、透明贴图、按钮绘制工具 |
| 全局定义 | `Globals.h/.cpp` | 常量、枚举、全局变量声明 |

### 核心常量（Globals.h）
```cpp
LINE_NUM        = 15      // 棋盘路数
BOARD_SIZE      = 650     // 棋盘像素大小
PIECE_SIZE      = 40      // 棋子大小
TIME_LIMIT      = 30      // 每回合秒数
TIME_LIMIT_TOTAL = 600    // 总时间秒数
PORT            = 8888    // 默认端口
```

### 网络协议
基于 TCP 的二进制消息结构：
```cpp
struct NetworkMessage {
    int msgtype;    // 消息类型（落子/退出/超时等）
    int x, y;       // 落子坐标
    int player;     // 玩家标识
    char data[256]; // 扩展数据
};
```

消息类型：`MSG_MOVE`（落子）、`MSG_QUIT`（退出）、`MSG_TIMEOUT`（回合超时）、`MSG_TIMEOUT_TOTAL`（总超时）

---

## 🔮 未来规划

- [ ] **AI 模式**：基于算法或 API 的人机对战
- [ ] **Roguelike 卡牌系统**：每 N 回合随机抽取技能卡
- [ ] **成长系统**：玩家可解锁永久升级
- [ ] **加密通信**：基于 openHiTLS 国密算法
- [ ] **排行榜与数据存储**：MySQL 数据库
- [ ] **Python AI 测试框架**：辅助平衡性调整
- [ ] **悔棋功能完善**：当前为占位实现

---

## 🤝 贡献

欢迎任何形式的贡献！如果你有兴趣：

- 🐛 报告 Bug
- 💡 提出新功能建议
- 🔧 提交 Pull Request

请确保代码风格清晰，并遵循现有模块划分。

---

## 📄 许可证

本项目仅供学习与交流使用。
