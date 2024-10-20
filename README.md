# Edge-Model-Infra

🚧 **开发中** - 用于大语言模型（LLM）部署和管理的分布式边缘计算基础设施

## 📋 项目状态

**当前版本**: v0.2.0-alpha
**开发阶段**: 网络通信层开发完成，开始消息协议设计
**最后更新**: 2024年10月
**开发者**: gugugu5331

## 🎯 项目目标

构建高性能、可扩展的边缘LLM推理基础设施，支持：

- 分布式模型部署
- 高效通信协议  
- GPU加速推理
- 容器化部署

## 🏗️ 开发进度

- [x] 项目初始化和基础架构
- [x] 基础工具库开发 (日志系统、JSON处理、调试工具)
- [x] Docker环境配置
- [x] 构建系统搭建
- [x] 网络通信层实现 (Socket、EventLoop、TCP服务器)
- [x] 网络调试和性能监控工具
- [ ] 消息协议设计
- [ ] LLM集成接口
- [ ] CUDA优化支持
- [ ] 示例应用和测试
- [ ] 生产部署优化

## 🚀 计划功能

- **分布式架构**: 模块化设计，组件独立
- **LLM集成**: 内置模型部署和推理支持
- **高性能通信**: 基于ZeroMQ的消息系统
- **Docker支持**: 容器化部署方案
- **TCP/JSON API**: 类RESTful API，便于外部集成
- **事件驱动**: 异步事件处理架构
- **跨平台支持**: 基于Linux的部署方案

## 📁 项目结构

```
Edge-Model-Infra/
├── README.md           # 项目文档
├── build.sh           # 主构建脚本
├── utils/             # 基础工具库
│   ├── logger.h       # 日志系统
│   ├── json_helper.h  # JSON处理工具
│   └── debug.h        # 调试工具
├── network/           # 网络通信层
│   ├── include/       # 网络层头文件
│   │   └── network/   # Socket、EventLoop、TCP等
│   ├── src/           # 网络层实现
│   └── CMakeLists.txt # 网络层构建配置
├── docker/            # Docker配置
│   ├── Dockerfile     # Docker镜像配置
│   ├── docker-compose.yml
│   └── scripts/       # Docker脚本
└── thirds/            # 第三方依赖
    └── README.md      # 依赖说明
```

## 🛠️ 技术栈

- **编程语言**: C++17, Python
- **通信协议**: ZeroMQ, TCP/JSON
- **容器化**: Docker
- **构建系统**: CMake
- **GPU加速**: CUDA (计划中)
- **第三方库**: simdjson, eventpp (计划中)

## 📝 开发日志

**2024年10月14日**: 完成网络层构建配置和调试实现
**2024年10月9日**: 实现TCP服务器和连接管理
**2024年10月5日**: 添加网络层调试工具和性能监控
**2024年10月3日**: 实现事件驱动架构 - EventLoop和Channel
**2024年10月1日**: 初始化网络层基础架构 - Socket和地址处理
**2024年9月15日**: 完成9月基础架构开发总结
**2024年9月12日**: 完成构建脚本和依赖管理
**2024年9月8日**: 添加Docker环境配置
**2024年9月5日**: 集成第三方依赖管理
**2024年9月4日**: 添加调试工具和错误处理
**2024年9月3日**: 完成基础工具库开发
**2024年9月1日**: 项目初始化，确定技术架构

## 🔧 环境要求

- **操作系统**: Ubuntu 20.04 或兼容的Linux发行版
- **编译器**: 支持C++17的GCC/G++
- **CMake**: 3.10或更高版本

## 🚀 快速开始

### 1. 克隆仓库
```bash
git clone https://github.com/gugugu5331/Edge-Model-Infra.git
cd Edge-Model-Infra
```

### 2. 后续步骤
项目正在开发中，构建和部署说明将在后续版本中提供。

## 📄 许可证

本项目采用MIT许可证 - 详见 [LICENSE](LICENSE) 文件

## 👥 贡献

欢迎提交Issue和Pull Request来帮助改进项目。

## 📞 联系方式

- GitHub: [@gugugu5331](https://github.com/gugugu5331)
- 项目地址: https://github.com/gugugu5331/Edge-Model-Infra
