# Edge-Model-Infra

🚧 **开发中** - 用于大语言模型（LLM）部署和管理的分布式边缘计算基础设施

## 📋 项目状态

**当前版本**: v0.3.0-alpha
**开发阶段**: 核心架构完成，开始LLM集成和CUDA优化
**最后更新**: 2024年12月
**开发者**: gugugu5331

## 🎯 项目目标

构建高性能、可扩展的边缘LLM推理基础设施，支持：

- 分布式模型部署
- 高效通信协议
- GPU加速推理
- 容器化部署

## 🏗️ 开发进度

- [x] 项目初始化
- [x] 基础工具库开发 (JSON处理、日志系统)
- [x] Docker环境配置
- [x] 构建系统搭建
- [x] 网络通信层实现 (TCP服务器、事件循环、线程池)
- [x] 消息协议设计 (ZeroMQ、序列化)
- [x] StackFlow事件驱动框架
- [x] 单元管理器核心架构
- [ ] LLM集成接口
- [ ] CUDA优化支持

## 📝 最近更新

- **2024-10-18**: 完成网络层单元测试和压力测试
- **2024-10-16**: 优化网络I/O性能和缓冲区管理
- **2024-10-14**: 实现多线程事件循环和线程池
- **2024-10-11**: 修复TCP连接内存泄漏问题
- **2024-10-09**: 完成TCP服务器和连接管理
- **2024-10-05**: 实现事件驱动架构(epoll支持)
- **2024-10-01**: 初始化网络层基础架构

## 🔧 网络层特性

- **高性能I/O**: 基于epoll的事件驱动架构
- **多线程支持**: 线程池和多线程事件循环
- **连接管理**: TCP服务器和客户端连接管理
- **缓冲区优化**: 高效的网络数据缓冲和处理
- **调试支持**: 完整的调试日志和性能监控

## 🚀 计划功能

- **分布式架构**: 模块化设计，组件独立
- **LLM集成**: 内置模型部署和推理支持
- **高性能通信**: 基于ZeroMQ的消息系统
- **Docker支持**: 容器化部署方案
- **事件驱动**: 异步事件处理架构

## 📁 Project Structure

```
Edge-Model-Infra/
├── infra-controller/     # Infrastructure control and flow management
├── unit-manager/         # Core unit management and coordination
├── network/             # Network communication layer
├── hybrid-comm/         # Hybrid communication protocols (ZMQ wrapper)
├── node/               # Node management and LLM integration
├── sample/             # Example implementations and test clients
├── docker/             # Docker configuration and build scripts
├── utils/              # Utility libraries and helpers
└── thirds/             # Third-party dependencies
```

## 🛠️ Components

### Infrastructure Controller (`infra-controller/`)
- **StackFlow**: Event-driven workflow management system
- **Channel Management**: Communication channel abstraction
- **Flow Control**: Request/response flow coordination

### Unit Manager (`unit-manager/`)
- **Core Service**: Main service orchestration (`main.cpp`)
- **Remote Actions**: RPC-style action handling
- **Session Management**: Client session lifecycle management
- **ZMQ Bus**: Message bus implementation
- **TCP Communication**: HTTP-like TCP server for external APIs

### Network Layer (`network/`)
- **Event Loop**: High-performance event-driven networking
- **TCP Server/Client**: Robust TCP communication
- **Connection Management**: Connection pooling and lifecycle management
- **Buffer Management**: Efficient data buffering

### Hybrid Communication (`hybrid-comm/`)
- **pzmq**: ZeroMQ wrapper with enhanced functionality
- **Message Serialization**: Efficient data serialization/deserialization
- **Protocol Abstraction**: Unified communication interface

## 🔧 Prerequisites

- **OS**: Ubuntu 20.04 or compatible Linux distribution
- **Compiler**: GCC/G++ with C++17 support
- **CMake**: Version 3.10 or higher
- **Dependencies**:
  - libzmq3-dev
  - libgoogle-glog-dev
  - libboost-all-dev
  - libssl-dev
  - libbsd-dev
  - eventpp
  - simdjson

## 🚀 Quick Start

### 1. Clone the Repository
```bash
git clone https://github.com/gugugu5331/Edge-Model-Infra.git
cd Edge-Model-Infra
```

### 2. Build with Docker (Recommended)
```bash
# Build the Docker image
cd docker/scripts
./llm_docker_run.sh

# Enter the container
./llm_docker_into.sh
```

### 3. Manual Build
```bash
# Install dependencies
sudo ./build.sh

# Build the project
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### 4. Run the System
```bash
# Start the unit manager
cd unit-manager
./unit_manager

# The system will start on port 10001 (configurable in master_config.json)
```

## 📖 Usage Examples

### Python Client Example
```python
import socket
import json

# Connect to the service
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect(('localhost', 10001))

# Setup LLM
setup_data = {
    "request_id": "llm_001",
    "work_id": "llm",
    "action": "setup",
    "object": "llm.setup",
    "data": {
        "model": "DeepSeek-R1-Distill-Qwen-1.5B",
        "response_format": "llm.utf-8.stream",
        "max_token_len": 1023,
        "prompt": "You are a helpful assistant."
    }
}

# Send setup request
sock.sendall((json.dumps(setup_data) + '\n').encode('utf-8'))
response = sock.recv(4096).decode('utf-8')
print("Setup response:", response)
```

### C++ RPC Example
```cpp
#include "pzmq.hpp"
using namespace StackFlows;

// Create RPC server
pzmq rpc_server("my_service");
rpc_server.register_rpc_action("process", [](pzmq* self, const std::shared_ptr<pzmq_data>& msg) {
    return "Processed: " + msg->string();
});

// Create RPC client
pzmq rpc_client("client");
auto result = rpc_client.rpc_call("my_service", "process", "Hello World");
```

## ⚙️ Configuration

### Unit Manager Configuration (`unit-manager/master_config.json`)
```json
{
    "config_tcp_server": 10001,
    "config_zmq_min_port": 5010,
    "config_zmq_max_port": 5555,
    "config_zmq_s_format": "ipc:///tmp/llm/%i.sock",
    "config_zmq_c_format": "ipc:///tmp/llm/%i.sock"
}
```

## 🧪 Testing

Run the included test client:
```bash
cd sample
python3 test.py --host localhost --port 10001
```

Run C++ samples:
```bash
cd sample
# Terminal 1: Start RPC server
./rpc_server

# Terminal 2: Run RPC client
./rpc_call
```

## 🐳 Docker Deployment

The project includes comprehensive Docker support:

```bash
# Build and run with Docker
cd docker/scripts
./llm_docker_run.sh    # Build and start container
./llm_docker_into.sh   # Enter running container
```

## 📊 API Reference

### Setup LLM Model
```json
POST /
{
    "request_id": "unique_id",
    "work_id": "llm",
    "action": "setup",
    "object": "llm.setup",
    "data": {
        "model": "model_name",
        "response_format": "llm.utf-8.stream",
        "max_token_len": 1023
    }
}
```

### Inference Request
```json
POST /
{
    "request_id": "unique_id",
    "work_id": "returned_work_id",
    "action": "inference",
    "object": "llm.utf-8.stream",
    "data": {
        "delta": "user_input",
        "index": 0,
        "finish": true
    }
}
```

## 🤝 Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 🏢 Copyright

Copyright © 2024 M5Stack Technology CO LTD

## 🔗 Links

- [GitHub Repository](https://github.com/gugugu5331/Edge-Model-Infra)
- [Issues](https://github.com/gugugu5331/Edge-Model-Infra/issues)
- [Pull Requests](https://github.com/gugugu5331/Edge-Model-Infra/pulls)

## 📞 Support

For support and questions, please open an issue on GitHub or contact the development team.

---

**Note**: This project is actively under development. Some features may be experimental or subject to change.
