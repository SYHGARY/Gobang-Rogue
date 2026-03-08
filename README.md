# Gobang Rogue – 五子棋 Roguelike
 
&emsp;![1](https://img.shields.io/badge/language-C++-blue.svg)
![2](https://img.shields.io/badge/graphics-EasyX-green.svg)
![3](https://img.shields.io/badge/network-Winsock-orange.svg)
![4](https://img.shields.io/badge/license-NonCommercial%20Open%20Source-red)

> **“打破定式，每局都是新体验”**

> **GARYSONG** - 开发与设计

- **Gobang Rogue** is a Gobang game developed using **C++** and the **EasyX graphics library**. The project not only implements classic features such as two-player, human vs. computer, and LAN online battles but also plans to introduce **Roguelike elements** (card skills, growth system), making each game session feel fresh and engaging. Currently in the Alpha testing phase, with core functionalities now playable.

- **Gobang Rogue** 是一款基于 **C++** 和 **EasyX** 图形库开发的五子棋游戏。项目不仅实现了经典的双人、人机、局域网联机对战，还计划引入 **Roguelike** 元素（卡牌技能、成长系统），让每一局对弈都充满新鲜感。目前处于 Alpha 测试阶段，核心功能已可玩。

---

## ✨ 特性
- **🎮 多种游戏模式**
  - 本地双人（同设备轮流操作）
  - 局域网 TCP 联机（服务器/客户端）
  - 单人 AI 对战（预留接口）
- **⏱️ 计时系统**
  - 单步限时（默认 30 秒）
  - 总时限（默认 10 分钟）
  - 剩余 5 秒闪烁提醒
- **🎨 美观的界面**
  - 基于 EasyX 的棋盘渲染，双缓冲无闪烁
  - 鼠标高光、点击落子
  - 独立信息区显示模式、连接状态、计时、功能按钮
- **🌐 网络对战**
  - 使用 Winsock 2.2，TCP 协议
  - IP:端口输入，支持域名解析
  - 落子、超时、退出等消息同步
- **🔄 游戏控制**
  - 重新开始、悔棋（本地）、退出
  - 音乐开关（MP3 背景音乐）
  - 状态机管理页面（菜单、设置、玩法介绍等）
- **🧩 可扩展架构**
  - 模块化设计，预留 Roguelike 卡牌系统、成长树、加密通信等接口

## 🛠️ 技术栈

| 组件          | 技术/库                          |
|---------------|----------------------------------|
| 核心语言      | C++                              |
| 图形界面      | EasyX (2024 版)                  |
| 网络通信      | Winsock2 (TCP)                   |
| 多媒体        | winmm.dll (播放背景音乐)         |
| 构建环境      | Visual Studio 2022 (Windows)     |
| 未来计划      | Python (AI 测试)、MySQL (排行榜) |

## 📦 安装与运行

### 环境要求
- Windows 7 及以上（因使用 EasyX 和 Winsock）
- Visual Studio 2019 或 2022（推荐）
- EasyX 库（[下载安装](https://easyx.cn/)）

## 操作说明

- **主菜单**：鼠标点击选择模式。
- **本地对战**：双击棋盘落子，右侧按钮控制音乐、设置、退出等。
- **局域网联机**：
  - 一方选择“创建服务器”，等待连接。
  - 另一方选择“加入服务器”，输入服务器 IP:端口（例如 `192.168.1.100:8888`），默认端口 `8888`。
- **计时**：回合剩余时间显示在对应棋子旁，超时自动换边；总时间耗尽判负。

## 📝 更新日志

### 2026-02-25
- 完成状态机本地与联网模式。
- 实现图形界面 IP:端口输入。
- 修复联网模式下强制换边、时间耗尽判断及图像显示问题。

### 2026-02-18
- 添加游戏重启功能（本地）。
- 引入状态机，简化代码结构。

### 2026-02-09
- 完善棋盘样式，增加总计时功能。
- 修复音乐开关无法重复开关的问题。

### 2026-01-28
- 添加局域网联机模块（TCP）。
- 棋盘样式更新，信息区移至右侧。
- 优化代码运行效率。

### 2025-12-23
- 修复鼠标消息死循环漏洞（改用非阻塞 `PeekMessage`）。
- 增加单步倒计时功能。

## 🔮 未来规划

- **AI 模式**：基于算法或 API 的人机对战
- **Roguelike 卡牌系统**：每 N 回合随机抽取技能卡
- **成长系统**：玩家可解锁永久升级
- **加密通信**：基于 openHiTLS 国密
- **排行榜与数据存储**：MySQL 数据库
- **Python AI 测试框架**：辅助平衡性调整

## 🤝 贡献

欢迎任何形式的贡献！如果你有兴趣：

- 报告 Bug
- 提出新功能建议
- 提交 Pull Request

请确保代码风格清晰，并遵循现有模块划分。
