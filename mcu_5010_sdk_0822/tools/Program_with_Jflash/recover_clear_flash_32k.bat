@echo off
setlocal enabledelayedexpansion
@echo -------------- OCS ODS5010 SWD recover / clear flash front 32KB v2.0 ----------------
@echo purpose: catch the chip's SWD interface in the brief window right at/after
@echo power-on, before app firmware can lock the debug port.
@echo this script retries "connect under reset + halt" in a loop until it
@echo succeeds (Ctrl+C to abort manually if it never connects), then lets you
@echo choose what to do with the halted core.
@echo POWER CYCLE THE TARGET NOW, or keep it powered while this loop retries.
@echo.

:: ==================== need config header ===========================
set base_addr=.
set jlink_exe="%base_addr%\JLink_V634b\JLink.exe"
set device_name=ODS5010
set jlink_if=SWD
set jlink_speed=4000
set zero_bin="%base_addr%\zero_32k.bin"
set write_addr=0x0
set write_size=32768
:: ==================== need config end ===========================

set probe_script="%base_addr%\_probe_cmd.jlink"
set write_script="%base_addr%\_write_cmd.jlink"
set interactive_script="%base_addr%\_interactive_cmd.jlink"

> %probe_script% echo RSetType 3
>>%probe_script% echo r
>>%probe_script% echo h
>>%probe_script% echo q

set attempt=0

:retry_loop
set /a attempt+=1
@echo [attempt !attempt!] trying connect-under-reset + halt ...
%jlink_exe% -Device %device_name% -If %jlink_if% -Speed %jlink_speed% -ExitOnError 1 -CommandFile %probe_script% >nul 2>&1
if !ERRORLEVEL! EQU 0 goto halted_ok
goto retry_loop

:halted_ok
if exist %probe_script% del /f /q %probe_script%
@echo.
@echo *** connect + halt succeeded on attempt !attempt! ***
@echo core is halted at the reset vector. choose an action:
@echo   1 = write zero_32k.bin to flash front 32KB, then exit (blanks old firmware)
@echo   2 = open interactive JLink command line (type any JLink/SWD command
@echo       yourself: erase, mem, reg, loadbin, go, r, h, q ...)
choice /c 12 /n /m "select 1 or 2: "
if !ERRORLEVEL! EQU 1 goto do_write
if !ERRORLEVEL! EQU 2 goto do_interactive
goto end

:do_write
@echo --- generating %write_size% bytes all-zero bin file
if exist %zero_bin% del /f /q %zero_bin%
powershell -NoProfile -Command "[System.IO.File]::WriteAllBytes('%zero_bin%', (New-Object byte[] %write_size%))"
if not exist %zero_bin% goto gen_fail

> %write_script% echo RSetType 3
>>%write_script% echo r
>>%write_script% echo h
>>%write_script% echo loadbin %zero_bin%, %write_addr%
>>%write_script% echo verifybin %zero_bin%, %write_addr%
>>%write_script% echo r
>>%write_script% echo h
>>%write_script% echo q

@echo --- connect under reset, halt at reset vector, clear front 32KB
%jlink_exe% -Device %device_name% -If %jlink_if% -Speed %jlink_speed% -ExitOnError 1 -CommandFile %write_script%
if %ERRORLEVEL% NEQ 0 goto recover_fail

@echo recover success, flash front 32KB cleared to zero, core left halted ***************
goto cleanup

:do_interactive
@echo --- opening interactive JLink command line, core stays halted under reset
@echo --- type "q" to exit when done
> %interactive_script% echo RSetType 3
>>%interactive_script% echo r
>>%interactive_script% echo h
%jlink_exe% -Device %device_name% -If %jlink_if% -Speed %jlink_speed% -CommandFile %interactive_script%
if exist %interactive_script% del /f /q %interactive_script%
goto end

:gen_fail
@echo failed to generate zero bin file ***************
goto end

:recover_fail
@echo recover fail, check SWD wiring / target power / J-Link connection ***************
goto cleanup

:cleanup
if exist %write_script% del /f /q %write_script%

:end
@echo end -------
endlocal
timeout /t 10
