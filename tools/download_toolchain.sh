#!/bin/bash
# 下载 ARM GCC 工具链 (gcc-arm-none-eabi-5_4-2016q3)
# 用于 BK7252N / RT-Thread SDK 编译

set -e

TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/5-2016q3/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2"
TOOLCHAIN_NAME="gcc-arm-none-eabi-5_4-2016q3"
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(dirname "$SCRIPT_DIR")"
TOOLCHAIN_DIR="$PROJECT_ROOT/toolchain"

echo "=========================================="
echo "ARM GCC Toolchain 下载脚本"
echo "=========================================="

# 检查是否已存在
if [ -d "$TOOLCHAIN_DIR/$TOOLCHAIN_NAME" ]; then
    echo "✓ 工具链已存在：$TOOLCHAIN_DIR/$TOOLCHAIN_NAME"
    echo "  如需重新下载，请先删除：rm -rf \"$TOOLCHAIN_DIR/$TOOLCHAIN_NAME\""
    exit 0
fi

# 创建目录
mkdir -p "$TOOLCHAIN_DIR"

# 根据系统选择下载链接
OS_TYPE=$(uname -s)
case "$OS_TYPE" in
    Darwin)
        echo "检测到 macOS 系统"
        # macOS 版本 (需要从其他来源获取，ARM 官方已不提供旧版)
        echo "警告：ARM 官方已移除旧版 macOS 工具链下载"
        echo "请从以下备选来源获取："
        echo "  1. 使用 Homebrew: brew install arm-none-eabi-gcc@5.4 (如果有)"
        echo "  2. 从项目备份恢复"
        echo "  3. 联系项目管理员获取"
        exit 1
        ;;
    Linux)
        echo "检测到 Linux 系统"
        ;;
    *)
        echo "未知系统：$OS_TYPE"
        exit 1
        ;;
esac

# 下载并解压
echo "下载地址：$TOOLCHAIN_URL"
echo "保存位置：$TOOLCHAIN_DIR"

cd "$TOOLCHAIN_DIR"

if command -v curl &> /dev/null; then
    curl -L -o "$TOOLCHAIN_NAME.tar.bz2" "$TOOLCHAIN_URL"
elif command -v wget &> /dev/null; then
    wget -O "$TOOLCHAIN_NAME.tar.bz2" "$TOOLCHAIN_URL"
else
    echo "错误：需要 curl 或 wget"
    exit 1
fi

echo "正在解压..."
tar -xjf "$TOOLCHAIN_NAME.tar.bz2"
rm -f "$TOOLCHAIN_NAME.tar.bz2"

echo "=========================================="
echo "✓ 工具链安装完成！"
echo "  位置：$TOOLCHAIN_DIR/$TOOLCHAIN_NAME"
echo "=========================================="
echo ""
echo "验证安装:"
echo "  $TOOLCHAIN_DIR/$TOOLCHAIN_NAME/bin/arm-none-eabi-gcc --version"
