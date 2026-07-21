# ⚫⚪Gobang Rogue – 五子棋 Roguelike
 
&emsp;![1](https://img.shields.io/badge/language-C++-blue.svg)
![2](https://img.shields.io/badge/graphics-EasyX-green.svg)
![3](https://img.shields.io/badge/network-Winsock-orange.svg)
![4](https://img.shields.io/badge/license-NonCommercial%20Open%20Source-red)

> **“打破定式，每局都是新体验”**

> **GARYSONG** - 开发与设计

- **Gobang Rogue** is a Gobang game developed using **C++** and the **EasyX graphics library**. The project not only implements classic features such as two-player, human vs. computer, and LAN online battles but also plans to introduce **Roguelike elements** (card skills, growth system), making each game session feel fresh and engaging. Currently in the Alpha testing phase, with core functionalities now playable.

- **Gobang Rogue** 是一款基于 **C++** 和 **EasyX** 图形库开发的五子棋游戏。项目不仅实现了经典的双人、人机、局域网联机对战，还计划引入 **Roguelike** 元素（卡牌技能、成长系统），让每一局对弈都充满新鲜感。目前处于 Alpha 测试阶段，核心功能已可玩。

---

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
