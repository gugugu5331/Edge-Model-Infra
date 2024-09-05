# 第三方依赖库

本目录包含项目所需的第三方库和依赖。

## 当前依赖

### simdjson
- **版本**: 3.13.0
- **用途**: 高性能JSON解析
- **许可证**: Apache 2.0
- **说明**: 用于高效解析JSON数据，特别是在网络通信和配置文件处理中

## 安装说明

### Ubuntu/Debian
```bash
# 安装基础依赖
sudo apt update
sudo apt install -y build-essential cmake

# 安装simdjson
sudo apt install -y libsimdjson-dev
```

### 从源码编译
```bash
# simdjson
wget https://github.com/simdjson/simdjson/archive/v3.13.0.tar.gz
tar -xzf v3.13.0.tar.gz
cd simdjson-3.13.0
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

## 计划添加的依赖

- **ZeroMQ**: 高性能消息队列
- **eventpp**: 事件处理库
- **CUDA**: GPU计算支持
- **cuDNN**: 深度学习加速库
