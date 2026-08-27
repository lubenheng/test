@echo off
setlocal enabledelayedexpansion
@echo -------------- OCS program with jflash loop v1.0 ----------------

:: ==================== need config header ===========================
set base_addr=.

:: set target_bin_x=[bin file path],[addr offset]
set target_bin_55="%base_addr%\all_55.bin",0x0
set target_bin_aa="%base_addr%\all_aa.bin",0x0
set target_bin_ff="%base_addr%\all_ff.bin",0x0
set target_bin_00="%base_addr%\all_00.bin",0x0

:: ==================== need config end ===========================

set jflash_exe="%base_addr%\JLink_V634b\JFlash.exe"
set jflash_file="%base_addr%\5010_main_260728.jflash"

set /a cycle_num=0
set /a total_success=0
set /a total_fail=0

@echo start time is:
time /t
@echo cycle order: all_55 -^> all_aa -^> all_ff -^> all_00
@echo runs continuously on success; on failure you will be asked to continue or stop
@echo.

:loop_start
set /a cycle_num+=1
@echo ============ cycle %cycle_num% ============

set step_name=all_55
set target_bin=%target_bin_55%
call :program_step
if errorlevel 1 call :ask_on_fail
if errorlevel 1 goto end

set step_name=all_aa
set target_bin=%target_bin_aa%
call :program_step
if errorlevel 1 call :ask_on_fail
if errorlevel 1 goto end

set step_name=all_ff
set target_bin=%target_bin_ff%
call :program_step
if errorlevel 1 call :ask_on_fail
if errorlevel 1 goto end

set step_name=all_00
set target_bin=%target_bin_00%
call :program_step
if errorlevel 1 call :ask_on_fail
if errorlevel 1 goto end

goto loop_start

:program_step
@echo --- cycle %cycle_num% : %step_name%
%jflash_exe% -openprj%jflash_file% -open%target_bin% -program -hide -disconnect -exit
set step_errlevel=%ERRORLEVEL%
if !step_errlevel! NEQ 0 (
	set /a total_fail+=1
	@echo [FAIL] cycle !cycle_num! !step_name! : ERRORLEVEL=!step_errlevel!
	exit /b 1
)
set /a total_success+=1
@echo [OK]   cycle !cycle_num! !step_name!  ^(success=!total_success! fail=!total_fail!^)
exit /b 0

:ask_on_fail
set user_input=
set /p user_input="download failed, continue with next step? (Enter=continue, stop=end): "
if /i "%user_input%"=="stop" exit /b 1
exit /b 0

:end
@echo ============ stopped by user ============
goto summary

:summary
time /t
@echo total success=%total_success%  total fail=%total_fail%
@echo end -------
pause
