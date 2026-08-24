@echo off
rem ============================================================
rem  Regenerate ODS5010_Alpha.SFR from ODS5010_Alpha.svd
rem  Toolchain: SVDConv (SVD -> SFD) + SfrCC2 (SFD -> SFR)
rem  If Keil is installed elsewhere, adjust KEIL_UV4 below.
rem ============================================================
set KEIL_UV4=C:\Keil_v5\UV4
cd /d "%~dp0"

"%KEIL_UV4%\SVDConv.exe" ODS5010_Alpha.svd --generate=sfd
rem SVDConv may exit 1 even with "0 Error(s)" - gate on the output file instead
if not exist ODS5010.sfd goto :err
if exist ODS5010.sfd move /y ODS5010.sfd ODS5010_Alpha.sfd

"%KEIL_UV4%\SfrCC2.Exe" ODS5010_Alpha.sfd
if errorlevel 1 goto :err

echo.
echo Done: ODS5010_Alpha.SFR regenerated.
goto :eof

:err
echo.
echo BUILD FAILED
exit /b 1
