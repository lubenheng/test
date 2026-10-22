@echo off
chcp 65001 >nul
cd /d "%~dp0"

where pythonw >nul 2>nul
if errorlevel 1 (
    echo 未找到 pythonw.exe，请先安装 Python 并加入 PATH。
    pause
    exit /b 1
)

start "" pythonw "word_merge.pyw"
