#!/bin/bash

# Edge-Model-Infra Docker构建脚本

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"

echo "构建Edge-Model-Infra Docker镜像..."

cd "$PROJECT_ROOT"

# 构建Docker镜像
docker build -t edge-model-infra:latest -f docker/Dockerfile .

echo "Docker镜像构建完成！"
echo "使用以下命令运行容器："
echo "  docker run -it --rm -p 8080:8080 -p 8081:8081 edge-model-infra:latest"
echo ""
echo "或使用docker-compose："
echo "  cd docker && docker-compose up -d"
