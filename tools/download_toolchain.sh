#!/bin/bash
# 下载 ARM GCC 工具链 (gcc-arm-none-eabi-5_4-2016q3)
# 用于 BK7252N / RT-Thread SDK 编译

set -e

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
ARCH=$(uname -m)

case "$OS_TYPE" in
    Darwin)
        echo "检测到 macOS 系统 ($ARCH)"

        # macOS 下载链接 (使用归档版本)
        if [ "$ARCH" = "arm64" ]; then
            # Apple Silicon - 使用 Intel 版本通过 Rosetta 2 运行
            echo "注意：Apple Silicon Mac 将通过 Rosetta 2 运行工具链"
        fi

        TOOLCHAIN_URL="https://github.com/Jason-bandy/AIVoicePrinter/releases/download/toolchain/gcc-arm-none-eabi-5_4-2016q3-20160926-mac.tar.bz2"
        echo "使用 macOS 版本工具链"
        ;;
    Linux)
        echo "检测到 Linux 系统 ($ARCH)"
        TOOLCHAIN_URL="https://developer.arm.com/-/media/Files/downloads/gnu-rm/5-2016q3/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2"
        ;;
    *)
        echo "未知系统：$OS_TYPE"
        exit 1
        ;;
esac

# 备选方案：Homebrew
echo ""
echo "备选方案（如果下载失败）:"
echo "  macOS: brew install arm-none-eabi-gcc@5.4 或 brew install arm-none-eabi-gcc"
echo "  然后设置环境变量：export RTT_EXEC_PATH=/opt/homebrew/bin"
echo ""

# 下载并解压
echo "下载地址：$TOOLCHAIN_URL"
echo "保存位置：$TOOLCHAIN_DIR"

cd "$TOOLCHAIN_DIR"

if command -v curl &> /dev/null; then
    if ! curl -L -o "$TOOLCHAIN_NAME.tar.bz2" "$TOOLCHAIN_URL"; then
        echo ""
        echo "下载失败！"
        echo ""
        echo "请手动下载工具链:"
        echo "  macOS: 从项目 releases 页面或其他来源获取"
        echo "  Linux: https://developer.arm.com/-/media/Files/downloads/gnu-rm/5-2016q3/gcc-arm-none-eabi-5_4-2016q3-20160926-linux.tar.bz2"
        echo ""
        echo "解压到：$TOOLCHAIN_DIR"
        exit 1
    fi
elif command -v wget &> /dev/null; then
    if ! wget -O "$TOOLCHAIN_NAME.tar.bz2" "$TOOLCHAIN_URL"; then
        echo ""
        echo "下载失败！请手动下载 (见上方)"
        exit 1
    fi
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
