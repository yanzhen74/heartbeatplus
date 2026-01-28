# HeartbeatPlus 构建说明

## 概述

HeartbeatPlus 是一个TCP心跳检测工具库，包含客户端库和服务端软件。本项目使用nanoMSG库进行网络通信。

## 依赖项

- Visual Studio 2013 (安装路径: C:\Program Files (x86)\Microsoft Visual Studio 12.0)
- nanoMSG 库 (已预装在项目中)

## 依赖说明

nanoMSG 库已预装在项目中的 `vcpkg/installed/` 目录下，无需额外安装或配置依赖管理器。

## 构建项目

### 方法一：使用批处理脚本（推荐）

```bash
.\build_fixed.bat
```

**注意**：当前 `build_fixed.bat` 脚本中硬编码了本机的VS2013环境路径：
- 脚本中指定了VS2013路径：`C:\Program Files (x86)\Microsoft Visual Studio 12.0`
- 无需修改此脚本，它已适配当前开发环境

### 方法二：使用 VSCode 任务

1. 打开 VSCode
2. 按 `Ctrl+Shift+P` 打开命令面板
3. 输入 "Tasks: Run Build Task" 并选择
4. 选择 "build-all" 任务

## 运行程序

构建成功后，可执行文件位于 `build\heartbeat.exe`，可以：

### 作为服务器运行
```bash
build\heartbeat.exe server tcp://0.0.0.0:6969
```

### 作为客户端运行
```bash
build\heartbeat.exe client tcp://localhost:6969 clientname
```

## 配置说明

- 客户端库位于 `src/client.cpp`
- 主程序入口位于 `src/heartbeat.cpp`
- 头文件位于 `include/client.h`
- 项目配置文件位于 `.vscode/` 目录
- VSCode开发环境已配置，支持在VSCode中开发并使用本机VS2013编译

## 故障排除

1. 如果遇到编译错误，请确保已安装Visual Studio 2013及C++开发组件
2. 如果找不到nanoMSG库，请检查 `vcpkg/installed/x64-windows/` 目录下是否有相应的头文件和库文件
3. 如果构建脚本无法运行，请确保以管理员身份运行命令提示符或PowerShell