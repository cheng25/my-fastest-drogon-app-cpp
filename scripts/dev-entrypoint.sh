#!/usr/bin/env bash
set -euo pipefail

# 启动 Drogon 后端开发容器
echo "[dev] Starting Drogon backend dev container..."

# Wait for PostgreSQL to be ready 等待 PostgreSQL 容器启动
echo "[dev] Waiting for PostgreSQL..."
until PGPASSWORD=postgres psql -h postgres -U postgres -d userdb -c '\q' 2>/dev/null; do
  # 等待 2 秒后重试
  echo "[dev] PostgreSQL is unavailable - sleeping"
  sleep 2
done
echo "[dev] PostgreSQL is up!" # 打印 PostgreSQL 启动成功信息

# 确保依赖项存在
# Ensure dependencies (jwt-cpp, Bcrypt) exist for CMakeLists.txt add_subdirectory
if [ ! -d "/app/dependencies/jwt-cpp" ] || [ ! -d "/app/dependencies/Bcrypt" ]; then
  # 如果依赖项不存在
  echo "[dev] Fetching C++ third-party dependencies into ./dependencies"
  mkdir -p /app/dependencies
  bash /app/dependencies.sh
fi

# 配置并构建应用
# Configure and build (Debug for development) 配置并构建应用（调试模式）
echo "[dev] Building application..."
mkdir -p /app/build
cd /app/build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j"$(nproc)" # 并行构建

echo "[dev] Build complete. Running app..."
cd /app/build
exec ./my_drogon_app
