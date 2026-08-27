echo off
 
set cur_path=%cd%


call :del_func demo\ODS5010\*
call :del_func demo\*
call :del_func to_customer_sdk\demo\ODS5010\*
call :del_func internal\*
call :del_func application\5010_htol\*
call :del_func motor_application\*
call :del_func application\pressure_test\*
call :del_func application\ODS5010_APP_Burnin\*
call :del_func application\ODS5010_APP_Burnin_SRAM\*
call :del_func application\ODS5010_htol_test\*
call :del_func application\pressure_test\tool_board_ODS5010\*
call :del_func application\pressure_test\traget_board_ODS5010\*

pause

exit /b 0

:del_func
set target_path=%1
for /d %%a in (%target_path%)  do (
	echo %%a
	echo %%%%%%%%%
	cd %%a
	call :do_del
	cd %cur_path%
)
goto:eof

:do_del
if exist Debug (
	del Debug\* /q /f /s
	rd /q /s Debug 
)

if exist gcc_build (
	del gcc_build\* /q /f /s
	rd /q /s gcc_build 
)

if exist gcc_build (
	del gcc_build\* /q /f /s
	rd /q /s gcc_build 
)

if exist ghs_build (
	del ghs_build\* /q /f /s
	rd /q /s ghs_build 
)

if exist Objects (
	del Objects\* /q /f /s
	rd /q /s Objects 
)

if exist Listings (
	del Listings\* /q /f /s
	rd /q /s Listings 
)

if exist *.uvguix.* (
	del *.uvguix.*
)
if exist JLinkLog.txt (
	del JLinkLog.txt
)

if exist JLinkSettings.ini (
	del JLinkSettings.ini
)

if exist *.dep (
	del *.dep
)

if exist settings\*.crun (
	del settings\*.crun
)

if exist settings\*.dbgdt (
	del settings\*.dbgdt
)

if exist settings\*.dnx (
	del settings\*.dnx
)

if exist settings\*.cspy.bat (
	del settings\*.cspy.bat
)

if exist settings\*.cspy.ps1 (
	del settings\*.cspy.ps1
)

if exist settings\*.driver.xcl (
	del settings\*.driver.xcl
)

if exist settings\*.general.xcl (
	del settings\*.general.xcl
)

if exist settings\*.wsdt (
	del settings\*.wsdt
)

if exist settings\*.xml (
	del settings\*.xml
)

if exist EventRecorderStub.scvd (
	del EventRecorderStub.scvd
)

goto:eof
