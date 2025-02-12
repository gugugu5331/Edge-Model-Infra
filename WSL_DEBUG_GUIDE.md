# WSL调试指南

## 概述

本指南提供了在Windows Subsystem for Linux (WSL)环境下调试Edge-Model-Infra项目的详细说明。

## 环境配置

### WSL版本要求
- WSL 2 (推荐)
- Ubuntu 20.04 LTS 或更高版本

### 必需工具
```bash
# 安装调试工具
sudo apt update
sudo apt install -y gdb valgrind strace ltrace

# 安装CUDA调试工具
sudo apt install -y cuda-gdb cuda-memcheck
```

## 调试技巧

### 1. 网络层调试
```bash
# 启用网络调试日志
export NETWORK_DEBUG=1
./unit-manager

# 监控网络连接
netstat -tulpn | grep unit-manager
```

### 2. CUDA调试
```bash
# CUDA内存检查
cuda-memcheck ./your_cuda_program

# CUDA调试器
cuda-gdb ./your_cuda_program
```

### 3. ZeroMQ调试
```bash
# 启用ZMQ调试
export ZMQ_DEBUG=1
export ZMQ_TRACE=1
```

## 常见问题

### CUDA相关问题
1. **CUDA初始化失败**
   - 检查NVIDIA驱动版本
   - 验证CUDA工具包安装

2. **内存泄漏**
   - 使用cuda-memcheck检测
   - 检查内存池实现

### 网络问题
1. **端口占用**
   ```bash
   sudo lsof -i :8080
   ```

2. **连接超时**
   - 检查防火墙设置
   - 验证网络配置

## 性能分析

### CPU性能分析
```bash
# 使用perf工具
perf record ./unit-manager
perf report
```

### GPU性能分析
```bash
# 使用nvprof
nvprof ./cuda_program
```

## 日志分析

### 日志级别
- ERROR: 错误信息
- WARN: 警告信息  
- INFO: 一般信息
- DEBUG: 调试信息

### 日志配置
```json
{
  "log_level": "DEBUG",
  "log_file": "/tmp/edge-model-infra.log",
  "enable_console": true
}
```
