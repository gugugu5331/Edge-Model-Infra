FROM ubuntu:20.04

# 设置非交互模式
ENV DEBIAN_FRONTEND=noninteractive

# 安装基础依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    wget \
    curl \
    pkg-config \
    libssl-dev \
    libzmq3-dev \
    libsimdjson-dev \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 复制项目文件
COPY . .

# 创建构建目录
RUN mkdir -p build

# 设置环境变量
ENV PATH="/app/build:${PATH}"

# 暴露端口
EXPOSE 8080 8081

# 默认命令
CMD ["bash"]
