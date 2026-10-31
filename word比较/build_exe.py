# -*- mode: python ; coding: utf-8 -*-
"""PyInstaller 构建脚本。

    python build_exe.py            # 单文件 exe（dist/WordCompare.exe）
    python build_exe.py --onedir   # 免解压的便携目录 + zip（分发给别人更稳）

单文件版每次启动都要把运行时解压到 %TEMP%；少数机器上临时目录被策略
限制或杀软拦截，表现就是"双击没反应"。遇到这种情况改用 --onedir。
"""

import shutil
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent
NAME = "WordCompare"
VERSION = "0.1.0"

VERSION_INFO = """VSVersionInfo(
  ffi=FixedFileInfo(
    filevers=(0, 1, 0, 0), prodvers=(0, 1, 0, 0),
    mask=0x3f, flags=0x0, OS=0x40004, fileType=0x1, subtype=0x0, date=(0, 0)
  ),
  kids=[
    StringFileInfo([StringTable('080404B0', [
        StringStruct('CompanyName', 'wordcompare'),
        StringStruct('FileDescription', 'Word 文档比较'),
        StringStruct('FileVersion', '{version}'),
        StringStruct('InternalName', '{name}'),
        StringStruct('OriginalFilename', '{name}.exe'),
        StringStruct('ProductName', 'Word 文档比较'),
        StringStruct('ProductVersion', '{version}')])]),
    VarFileInfo([VarStruct('Translation', [2052, 1200])])
  ]
)
"""


USER_GUIDE = """Word 文档比较（WordCompare）使用说明
====================================

一、启动
  1. 双击 WordCompare.exe。
  2. 若提示"Windows 已保护你的电脑"：点"更多信息" → "仍要运行"。
  3. 若双击没反应：右键 exe → 属性 → 勾选最下方"解除锁定" → 确定，再双击。
  4. 若杀毒软件报毒：这是无签名程序的常见误报，请将程序加入信任/白名单。
     仍不行可按 README 里的说明改用"便携版"目录（不解压到临时目录，更稳）。

二、图形界面
  1. 选择"基准文档（旧稿）"和"对比文档（新稿）"，均需 .docx 格式。
  2. 勾选比较范围：文本 / 格式 / 表格（默认全选）。
  3. 点"开始比较"，完成后自动打开 HTML 报告。

三、生成的产物
  <文档A>__vs__<文档B>.html        双栏差异报告，单文件，可直接发邮件
  <文档A>__vs__<文档B>.md / .csv   统计摘要与变更明细
  <文档A>__vs__<文档B>.修订.docx   可在 Word 中逐条"接受/拒绝"的修订稿

四、命令行用法
  WordCompare.exe 旧稿.docx 新稿.docx -o 输出目录
  WordCompare.exe --help         查看全部参数（--no-text / --no-format / --no-tables 等）

五、程序打不开时
  启动日志：%LOCALAPPDATA%\\WordCompare\\启动日志.txt
  * 日志不存在 —— 程序根本没被执行，多半被系统或杀软拦住了；
  * 日志里有报错 —— 把最后几行发回给提供程序的人。
"""


def _version_file() -> Path:
    path = ROOT / "build" / "version_info.txt"
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(
        VERSION_INFO.format(name=NAME, version=VERSION), encoding="utf-8")
    return path


def main(argv: list[str] | None = None) -> int:
    argv = argv if argv is not None else sys.argv[1:]
    onedir = "--onedir" in argv

    sep = ";" if sys.platform.startswith("win") else ":"
    # 必须用绝对路径：--specpath 会让相对路径按 spec 所在目录解析
    add_data = (f"{ROOT / 'src' / 'wordcompare' / 'templates'}"
                f"{sep}wordcompare/templates")
    icon = ROOT / "assets" / "wordcompare.ico"

    cmd = [
        sys.executable, "-m", "PyInstaller",
        "--noconfirm", "--clean",
        "--onedir" if onedir else "--onefile",
        "--console",                       # 保留 CLI 用法；GUI 模式下自动隐藏
        "--noupx",                         # UPX 压缩会显著提高杀软误报率
        "--name", NAME + ("-便携版" if onedir else ""),
        "--add-data", add_data,
        "--version-file", str(_version_file()),
        "--paths", str(ROOT / "src"),
        "--hidden-import", "diff_match_patch",
        "--hidden-import", "rapidfuzz.fuzz",
        "--hidden-import", "wordcompare.cli",
        "--hidden-import", "wordcompare.gui",
        "--exclude-module", "matplotlib",
        "--exclude-module", "numpy",
        "--exclude-module", "pandas",
        "--exclude-module", "PIL",
        "--exclude-module", "pytest",
        "--distpath", str(ROOT / "dist"),
        "--workpath", str(ROOT / "build"),
        "--specpath", str(ROOT / "build"),
    ]
    if icon.exists():
        cmd += ["--icon", str(icon)]
    cmd.append(str(ROOT / "app.py"))

    print("运行：" + " ".join(cmd))
    result = subprocess.run(cmd)
    if result.returncode != 0:
        return result.returncode

    guide = ROOT / "dist" / "使用说明.txt"
    guide.parent.mkdir(parents=True, exist_ok=True)
    guide.write_text(USER_GUIDE, encoding="utf-8-sig")   # 记事本可直接打开

    if onedir:
        folder = ROOT / "dist" / f"{NAME}-便携版"
        shutil.copyfile(guide, folder / "使用说明.txt")
        archive = shutil.make_archive(str(folder), "zip", root_dir=folder)
        print(f"\n便携版目录：{folder}")
        print(f"分发压缩包：{archive}")
        print("把这个 zip 发给别人，解压后双击 WordCompare-便携版.exe 即可。")
    else:
        exe = ROOT / "dist" / (NAME + (".exe" if sys.platform.startswith("win") else ""))
        if exe.exists():
            size = exe.stat().st_size / 1024 / 1024
            print(f"\n构建完成：{exe}（{size:.1f} MB）")
            print(f"说明文档：{ROOT / 'dist' / '使用说明.txt'}")
            print("如对方双击没反应，改用 python build_exe.py --onedir 生成便携版。")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
