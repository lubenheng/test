# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['C:/Users/user/Desktop/word比较/app.py'],
    pathex=['C:/Users/user/Desktop/word比较/src'],
    binaries=[],
    datas=[('C:/Users/user/Desktop/word比较/src/wordcompare/templates', 'wordcompare/templates')],
    hiddenimports=['diff_match_patch', 'rapidfuzz.fuzz', 'wordcompare.cli', 'wordcompare.gui'],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['matplotlib', 'numpy', 'pandas', 'PIL', 'pytest'],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='WordCompare-便携版',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=False,
    console=True,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
    version='C:/Users/user/Desktop/word比较/build/version_info.txt',
    icon=['C:/Users/user/Desktop/word比较/assets/wordcompare.ico'],
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=False,
    upx_exclude=[],
    name='WordCompare-便携版',
)
