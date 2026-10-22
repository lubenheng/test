@echo off
setlocal

rem Word 文档合并工具 - 打包为独立 exe
rem 用法：build_exe.bat          -> 单文件 dist\WordMergeTool.exe
rem       build_exe.bat onedir   -> 文件夹版 dist\WordMergeTool\（启动更快）
rem 运行本脚本前请确认已安装：pip install pyinstaller python-docx docxcompose pywin32 tkinterdnd2

cd /d "%~dp0"

set "MODE=%~1"
if /I "%MODE%"=="onedir" (
    set "ONEFILE="
    set "TARGET=dist\WordMergeTool\WordMergeTool.exe"
) else (
    set "ONEFILE=--onefile"
    set "TARGET=dist\WordMergeTool.exe"
)

echo [1/3] 清理旧的构建目录...
if exist build rmdir /s /q build
if exist WordMergeTool.spec del /q WordMergeTool.spec
if /I "%MODE%"=="onedir" (
    if exist dist\WordMergeTool rmdir /s /q dist\WordMergeTool
) else (
    if exist dist\WordMergeTool.exe del /q dist\WordMergeTool.exe
)

echo [2/3] 开始打包（首次约需 1-3 分钟）...
python -m PyInstaller ^
    --noconfirm ^
    --clean ^
    %ONEFILE% ^
    --windowed ^
    --name WordMergeTool ^
    --collect-all tkinterdnd2 ^
    --hidden-import win32com ^
    --hidden-import win32com.client ^
    --hidden-import pythoncom ^
    --hidden-import pywintypes ^
    --hidden-import docxcompose ^
    --hidden-import docx ^
    --hidden-import lxml.etree ^
    --exclude-module numpy ^
    --exclude-module pandas ^
    --exclude-module matplotlib ^
    --exclude-module PIL ^
    --exclude-module pytest ^
    word_merge.pyw

if errorlevel 1 (
    echo.
    echo 打包失败，请查看上面的错误信息。
    pause
    exit /b 1
)

if not exist "%TARGET%" (
    echo 未找到输出文件 %TARGET%，请检查打包日志。
    pause
    exit /b 1
)

echo [3/3] 打包完成：%TARGET%
echo.
echo 自检中...
"%TARGET%" --selftest "%~dp0dist\selftest.log"
echo 自检结果：dist\selftest.log
echo.
if /I "%MODE%"=="onedir" (
    echo 将 dist\WordMergeTool 整个文件夹复制到目标电脑即可运行。
) else (
    echo 可以将 dist\WordMergeTool.exe 单独复制到任意目录运行。
)
pause
