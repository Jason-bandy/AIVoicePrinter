# 1. 安装 Homebrew（如果还没有）
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 2. 安装 ARM GCC 工具链
brew install arm-none-eabi-gcc@5-4-2016q3
# 或者最新版本（可能需要调整 rtconfig.py 中的版本检查）
brew install arm-none-eabi-gcc

# 3. 安装 scons
pip3 install scons
