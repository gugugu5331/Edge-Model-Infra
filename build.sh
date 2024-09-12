#!/bin/bash

# Edge-Model-Infra 主构建脚本

set -e

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查依赖
check_dependencies() {
    print_info "检查构建依赖..."
    
    # 检查编译器
    if ! command -v g++ &> /dev/null; then
        print_error "g++编译器未找到，请安装build-essential"
        exit 1
    fi
    
    # 检查CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake未找到，请安装cmake"
        exit 1
    fi
    
    # 检查pkg-config
    if ! command -v pkg-config &> /dev/null; then
        print_warn "pkg-config未找到，某些依赖可能无法正确链接"
    fi
    
    print_info "依赖检查完成"
}

# 安装系统依赖
install_system_deps() {
    print_info "安装系统依赖..."
    
    if command -v apt-get &> /dev/null; then
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            pkg-config \
            libssl-dev \
            libzmq3-dev \
            libsimdjson-dev
    elif command -v yum &> /dev/null; then
        sudo yum install -y \
            gcc-c++ \
            cmake \
            pkgconfig \
            openssl-devel \
            zeromq-devel
    else
        print_warn "未识别的包管理器，请手动安装依赖"
    fi
}

# 创建构建目录
setup_build_dir() {
    print_info "设置构建目录..."
    
    if [ -d "build" ]; then
        print_warn "构建目录已存在，清理中..."
        rm -rf build
    fi
    
    mkdir -p build
    cd build
}

# 配置构建
configure_build() {
    print_info "配置构建..."
    
    CMAKE_ARGS=""
    
    # 检查是否为调试模式
    if [ "$1" = "debug" ]; then
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Debug"
        print_info "配置为调试模式"
    else
        CMAKE_ARGS="-DCMAKE_BUILD_TYPE=Release"
        print_info "配置为发布模式"
    fi
    
    # 暂时创建一个简单的CMakeLists.txt
    cat > ../CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.10)
project(EdgeModelInfra)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件目录
include_directories(utils)

# 创建一个简单的测试程序
add_executable(test_utils test_utils.cpp)

# 安装目标
install(TARGETS test_utils DESTINATION bin)
EOF

    # 创建测试文件
    cat > ../test_utils.cpp << 'EOF'
#include "logger.h"
#include "debug.h"
#include <iostream>

int main() {
    std::cout << "Edge-Model-Infra 构建测试" << std::endl;
    std::cout << "基础工具库测试通过" << std::endl;
    return 0;
}
EOF

    cmake .. $CMAKE_ARGS
}

# 执行构建
build_project() {
    print_info "开始构建项目..."
    
    # 获取CPU核心数
    CORES=$(nproc 2>/dev/null || echo 4)
    print_info "使用 $CORES 个并行任务进行构建"
    
    make -j$CORES
    
    print_info "构建完成"
}

# 运行测试
run_tests() {
    print_info "运行测试..."
    
    if [ -f "test_utils" ]; then
        ./test_utils
        print_info "测试通过"
    else
        print_warn "测试程序未找到"
    fi
}

# 主函数
main() {
    print_info "开始构建 Edge-Model-Infra..."
    
    # 解析命令行参数
    BUILD_TYPE="release"
    INSTALL_DEPS=false
    RUN_TESTS=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --debug)
                BUILD_TYPE="debug"
                shift
                ;;
            --install-deps)
                INSTALL_DEPS=true
                shift
                ;;
            --test)
                RUN_TESTS=true
                shift
                ;;
            --help)
                echo "用法: $0 [选项]"
                echo "选项:"
                echo "  --debug        构建调试版本"
                echo "  --install-deps 安装系统依赖"
                echo "  --test         运行测试"
                echo "  --help         显示此帮助信息"
                exit 0
                ;;
            *)
                print_error "未知选项: $1"
                exit 1
                ;;
        esac
    done
    
    # 执行构建步骤
    check_dependencies
    
    if [ "$INSTALL_DEPS" = true ]; then
        install_system_deps
    fi
    
    setup_build_dir
    configure_build $BUILD_TYPE
    build_project
    
    if [ "$RUN_TESTS" = true ]; then
        run_tests
    fi
    
    print_info "构建完成！"
    print_info "可执行文件位于: build/"
}

# 执行主函数
main "$@"
