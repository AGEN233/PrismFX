# prismFX ARGB 控制器固件

prismFX ARGB 控制器的 **下位机固件**。

本项目是一个 **基于 ESP-IDF 的开源 ARGB 控制器固件**，用于驱动可寻址 RGB 灯带，并通过 **Bluetooth Low Energy（BLE）** 与 prismFX ARGB PC Host 上位机进行通信和配置。

---

# ✨ 项目特性

* ⚙️ 基于 **ESP-IDF**
* 📡 **BLE 设备通信**
* 🌈 **ARGB 灯带驱动**
* 🎛 **设备配置与状态管理**
* 🔧 模块化架构设计
* 🚀 方便后续扩展

---

# 🧠 系统架构

固件运行在 ESP32 系列芯片上，主要负责：

* ARGB 灯效控制
* BLE 通信
* 设备注册与管理
* 数据事件处理

系统架构：

```
PC Host (Python + Qt)
        │
        │  BLE
        ▼
prismFX Firmware (ESP-IDF)
        │
        ├─ ARGB Driver
        ├─ BLE Protocol
        └─ Device Logic
        │
        ▼
   ARGB LED Hardware
```

---

# 📂 项目结构

```
prismfx-firmware/
│
├─ application/              # 应用层逻辑
│   ├─ include/              # 应用层头文件
│   ├─ src/
│   │   ├─ argb_core.c       # ARGB 控制核心
│   │   ├─ dev_data_event.c  # 设备数据事件处理
│   │   ├─ dev_register.c    # 设备注册逻辑
│   │   └─ led_pilot.c       # LED 控制逻辑
│   │
│   ├─ app_main.c            # 程序入口
│   ├─ info_config.h         # 设备信息配置
│   └─ CMakeLists.txt
│
├─ modules/                  # 功能模块
│   ├─ argb_driver/          # ARGB 灯带驱动
│   ├─ ble_iot/              # BLE 通信模块
│   └─ utils/                # 工具函数
│
├─ project_esp32c3/          # ESP32-C3 工程配置 (后面考虑和其他使用IDF的芯片兼容，但目前为止以C3开发(因为硬件是C3画的板))
│
├─ esp_idf/                  # ESP-IDF 子模块（SDK）
│
├─ .gitmodules
└─ .gitignore
```

---

# 🔧 开发环境

推荐配置：

* **ESP-IDF 5.x / 6.x**
* ESP32 / ESP32-S3 / ESP32-C3

本项目使用 **ESP-IDF 作为 Git 子模块（Submodule）** 引入，作为项目的 **独立局部 SDK 环境**。

这样可以：

* 固定 SDK 版本
* 避免依赖系统全局 ESP-IDF
* 保证不同开发者构建环境一致
* 通过 Git 同步 SDK 版本

ESP-IDF 位于：

```
esp_idf/
```

---

# 🚀 使用方法

## 1. 克隆项目

```
git clone https://github.com/AGEN233/PrismFX.git
cd PrismFX
```

---

## 2. 初始化子模块

本项目使用 **ESP-IDF 作为 Git Submodule 引入**，SDK 版本 **固定绑定到仓库提交版本**。

请不要自行升级 SDK，否则可能导致编译或运行问题。

初始化子模块：

```
git submodule update --init --recursive
```

该命令会拉取 **与当前工程提交绑定的 ESP-IDF 版本**，确保所有开发者使用一致的 SDK 环境。

如果仓库更新了子模块版本，可以执行：

```
git submodule update --recursive
```

这会同步到 **项目指定的 ESP-IDF 提交版本**。

---

## 3. 配置 ESP-IDF 工具脚本

进入 ESP-IDF 目录：

```
cd esp_idf
```

如有需要，可以修改 `tools/idf_tools.py` 中的下载源地址（例如替换为国内镜像）。

---

## 4. 安装工具链并导出环境

在 `esp_idf` 目录执行：

```
./install.sh
```

安装完成后导出环境：

```
. ./export.sh
```

---

## 5. 编译项目

返回项目根目录：

```
cd ..
```

选择芯片目标（示例为 ESP32-C3）：

```
idf.py set-target esp32c3
```

编译项目：

```
idf.py build
```

---

## 6. 烧录与调试

烧录固件：

```
idf.py flash
```

打开串口监视：

```
idf.py monitor
```

---

# 📡 通信方式

设备与 PC Host 之间使用：

* **Bluetooth Low Energy (BLE)**
* **自定义通信协议**

未来计划支持：

* WiFi 控制
* USB 通信
* OTA 固件升级

---

# 🔗 相关项目

### prismFX ARGB PC Host

用于控制设备的桌面软件。

主要功能：

* 设备连接
* 灯效控制
* 参数配置
* 后续灯效编辑器

项目地址：

https://github.com/AGEN233/PrismFX

---

# ⚖️ 开源协议

本项目采用 **MIT License**。

你可以自由：

* 使用
* 修改
* 分发
* 用于商业用途

但必须保留原始版权声明。

---

# 📌 说明

prismFX PC Host 由于使用 **QFluentWidgets**，采用 **GPL-3.0 协议**。

本固件项目与 PC Host **仅通过 BLE 协议通信**，属于 **独立项目**，因此不受 GPL 协议约束。

---

# 💡 项目目标

prismFX 旨在构建一个 **开放、可扩展的 ARGB 控制生态系统**：

* 开源控制器固件
* 桌面控制软件
* 可扩展通信协议
* 自定义灯效系统

欢迎提出建议或参与贡献 ⭐

---

# 🤖 AI 辅助说明

本 README 文档由 **ChatGPT 5.3 模型**协助编写。

本项目的诞生离不开 **OpenAI 提供的强大的生成式大模型支持**，在开发、设计和文档编写过程中提供了重要帮助。
