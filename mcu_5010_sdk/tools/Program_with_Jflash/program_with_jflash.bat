@echo off
@echo -------------- OCS program with jflash v0.0 ----------------

:: ==================== need config header ===========================
set base_addr=.

:: set target_bin_x=[bin file path],[addr offset]
set target_bin_1="%base_addr%\all_55.bin",0x0
set target_bin_2="%base_addr%\APP.bin",0x4000

:: ==================== need config end ===========================

set jflash_exe="%base_addr%\JLink_V634b\JFlash.exe"
set jflash_file="%base_addr%\5010_main_260728.jflash"

@echo start time is:
time /t


:: program 
@echo program

@echo --- bin 1
::%jflash_exe% -openprj%jflash_file% -open%target_bin_1% -programverify -hide -disconnect -exit
%jflash_exe% -openprj%jflash_file% -open%target_bin_1% -program -hide -disconnect -exit
if %ERRORLEVEL% NEQ 0 goto program_fail

::@echo --- bin 2
::%jflash_exe% -openprj%jflash_file% -open%target_bin_2% -programverify -hide -disconnect -exit
::%jflash_exe% -openprj%jflash_file% -open%target_bin_2% -program -hide -disconnect -exit
::if %ERRORLEVEL% NEQ 0 goto program_fail

time /t

@echo program success ***************
goto end

:program_fail
	@echo program fail ***************
	goto end
	:: exit /b 2

:end

	:: ping 1.2.3.4 -n 1 >nul
	@echo end -------
	timeout /t 10
	:: exit /b 0
