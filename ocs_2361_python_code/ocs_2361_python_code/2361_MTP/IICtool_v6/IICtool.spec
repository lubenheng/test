# -*- mode: python ; coding: utf-8 -*-


a = Analysis(
    ['c:\\Users\\Administrator\\Desktop\\2361\\IICtool_v6\\IICtool(5)(1).py'],
    pathex=[],
    binaries=[],
    datas=[('C:\\Users\\Administrator\\Desktop\\2361\\IICtool_v6\\_internal\\USB2UARTSPIIICDLL.dll', '_internal')],
    hiddenimports=[],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[],
    noarchive=False,
    optimize=0,
)
pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='IICtool',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)
coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='IICtool',
)
