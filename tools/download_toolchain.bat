@echo off
REM 下载 ARM GCC 工具链 (gcc-arm-none-eabi-5_4-2016q3) - Windows 版本
REM 用于 BK7252N / RT-Thread SDK 编译

setlocal

set TOOLCHAIN_NAME=gcc-arm-none-eabi-5_4-2016q3
set SCRIPT_DIR=%~dp0
set PROJECT_ROOT=%SCRIPT_DIR%..
set TOOLCHAIN_DIR=%PROJECT_ROOT%\toolchain

echo ==========================================
echo ARM GCC Toolchain 下载脚本 (Windows)
echo ==========================================

REM 检查是否已存在
if exist "%TOOLCHAIN_DIR%\%TOOLCHAIN_NAME%\bin\arm-none-eabi-gcc.exe" (
    echo ✓ 工具链已存在：%TOOLCHAIN_DIR%\%TOOLCHAIN_NAME%
    echo   如需重新下载，请先删除：rmdir /s /q "%TOOLCHAIN_DIR%\%TOOLCHAIN_NAME%"
    goto :EOF
)

REM 创建目录
if not exist "%TOOLCHAIN_DIR%" mkdir "%TOOLCHAIN_DIR%"

REM Windows 下载链接 (ARM 官方已移除，需从其他来源)
echo 警告：ARM 官方已移除旧版工具链下载
echo.
echo 请从以下来源获取工具链:
echo   1. 官方备选：https://developer.arm.com/downloads/-/gnu-rm
echo   2. 手动下载后放到：%TOOLCHAIN_DIR%\
echo   3. 联系项目管理员获取
echo.
echo 推荐版本：gcc-arm-none-eabi-5_4-2016q3-20160926-win32.zip
echo.

:ask_continue
set /p continue_install="是否继续从其他来源下载？(Y/N): "
if /i "%continue_install%"=="Y" (
    echo.
    echo 请自行下载并解压到：%TOOLCHAIN_DIR%\%TOOLCHAIN_NAME%
    goto :EOF
) else if /i "%continue_install%"=="N" (
    goto :EOF
) else (
    goto :ask_continue
)

endlocal
