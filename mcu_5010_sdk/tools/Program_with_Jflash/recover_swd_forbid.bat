@echo off
setlocal EnableExtensions EnableDelayedExpansion

rem ============================================================================
rem ODS5010 SWD recovery - SWD FORBID BIN
rem
rem Operation:
rem   1. Start this BAT while the target is powered off.
rem   2. The BAT repeatedly tries to attach through SWD.
rem   3. Power on the target.
rem   4. On the first successful attach, the same J-Link session executes:
rem   This variant writes 5010_swd_forbid.bin without issuing an explicit reset command.
rem
rem        halt -> loadbin -> verifybin -> halt -> quit
rem
rem IMPORTANT:
rem   - This BAT does not issue r, reset, RSetType, erase, or go commands.
rem   - For strict no-reset recovery, disconnect the J-Link nRESET signal.
rem   - J-Link DLL/device/FLM behavior must still be verified on nRESET with a
rem     scope or logic analyzer before this is used as a production procedure.
rem ============================================================================

set "base_addr=%~dp0"
set "jlink_exe=%base_addr%JLink_V634b\JLink.exe"
set "device_name=ODS5010"
set "jlink_if=SWD"
set "jlink_speed=1000"
set "recover_bin=%base_addr%5010_swd_forbid.bin"
set "write_addr=0x0"

set "tag=%RANDOM%_%RANDOM%"
set "cmd_file=%TEMP%\ods5010_recover_swd_forbid_%tag%.jlink"
set "log_file=%base_addr%recover_swd_forbid_last.log"

call :preflight
if errorlevel 1 goto failed_before_start

>"%cmd_file%" echo h
>>"%cmd_file%" echo loadbin "%recover_bin%", %write_addr%
>>"%cmd_file%" echo verifybin "%recover_bin%", %write_addr%
>>"%cmd_file%" echo h
>>"%cmd_file%" echo q

rem Safety guard: refuse to run if a reset command is accidentally added later.
findstr /R /I /C:"^[ ]*r[ ]*$" /C:"^[ ]*reset" /C:"^[ ]*RSetType" "%cmd_file%" >nul
if not errorlevel 1 (
    echo ERROR: A reset-related command was found in the generated J-Link file.
    goto failed
)

set /a attempt=0

echo.
echo ============================================================================
echo ODS5010 SWD RECOVERY - SWD FORBID BIN
echo J-Link : "%jlink_exe%"
echo BIN    : "%recover_bin%"
echo Address: %write_addr%
echo SWD    : %jlink_speed% kHz
echo Log    : "%log_file%"
echo ============================================================================
echo.
echo Keep the target powered OFF until the retry loop is running.
echo For strict no-reset operation, J-Link nRESET should be physically disconnected.
echo Power ON the target now. Press Ctrl+C to abort.
echo.

:retry
set /a attempt+=1
echo [attempt !attempt!] SWD attach, halt, program and verify...

"%jlink_exe%" -Device "%device_name%" -If "%jlink_if%" -Speed %jlink_speed% -ExitOnError 1 -CommandFile "%cmd_file%" <nul >"%log_file%" 2>&1
set "jlink_rc=!ERRORLEVEL!"

if not "!jlink_rc!"=="0" goto retry

echo.
echo ============================================================================
echo RECOVERY COMMAND COMPLETED SUCCESSFULLY ON ATTEMPT !attempt!.
echo The command sequence contained no explicit reset command.
echo The final J-Link command requested that the core be halted.
echo Review the log before power-cycling the target:
echo "%log_file%"
echo ============================================================================

echo.
echo Download completed. The window will close automatically in 200 seconds.
for /l %%S in (200,-1,1) do (
    echo Closing in %%S second(s)...
    timeout /t 1 /nobreak >nul
)
echo Closing now.

goto success

:preflight
if not exist "%jlink_exe%" (
    echo ERROR: JLink.exe was not found:
    echo "%jlink_exe%"
    exit /b 2
)

if not exist "%recover_bin%" (
    echo ERROR: Recovery BIN was not found:
    echo "%recover_bin%"
    exit /b 3
)

for %%F in ("%recover_bin%") do set "bin_size=%%~zF"
if not defined bin_size (
    echo ERROR: Unable to read recovery BIN size.
    exit /b 4
)
if "!bin_size!"=="0" (
    echo ERROR: Recovery BIN is empty.
    exit /b 5
)

exit /b 0

:failed_before_start
echo.
echo Recovery did not start because a preflight check failed.
goto failed_common

:failed
echo.
echo Recovery stopped because of an internal safety check failure.

:failed_common
if defined cmd_file if exist "%cmd_file%" del /q "%cmd_file%" >nul 2>&1
endlocal
exit /b 1

:success
if exist "%cmd_file%" del /q "%cmd_file%" >nul 2>&1
endlocal
exit /b 0
