
@echo OFF

REM  adb > null|| goto START
REM  goto END


:START
ECHO "====================================="
ECHO "===   Start Device ID Write!!!   ==="
ECHO "====================================="


set /p DeviceID="Enter Your Device ID: "
ECHO "you Device ID is '%DeviceID%' !!"

goto WRITE_ID

REM  set /p var_confirm="Confirm[ Y/N/Q ]"
REM  if %VAR_CONFIRM%==Y GOTO WRITE_ID
REM  if %VAR_CONFIRM%==y GOTO WRITE_ID
REM  if %VAR_CONFIRM%==N GOTO START
REM  if %VAR_CONFIRM%==n GOTO START
REM  if %VAR_CONFIRM%==Q GOTO END
REM  if %VAR_CONFIRM%==q GOTO END



goto START
REM  goto END






:WRITE_ID
	ECHO "Writing your ID..."
	adb shell "mkdir -p /protect_t/ /protect_s/"
	adb shell "mount -t ext4 /dev/block/platform/mtk-msdc.0/by-name/protect1 /protect_t/"
	adb shell "mount -t ext4 /dev/block/platform/mtk-msdc.0/by-name/protect2 /protect_s/"
	adb shell "mkdir -p /protect_t/IBoxConfig/"
	adb shell "mkdir -p /protect_s/IBoxConfig/"

	adb push IBoxConfig\BroadcastConfig.txt /protect_s/IBoxConfig/
	adb push IBoxConfig\DeviceIDConfig.txt  /protect_s/IBoxConfig/
	adb push IBoxConfig\OBDServerConfig.txt /protect_s/IBoxConfig/
	adb push IBoxConfig\SocketConfig.txt /protect_s/IBoxConfig/
	adb push IBoxConfig\CaptureConfig.txt /protect_s/IBoxConfig/
	adb push IBoxConfig\FolderConfig.txt /protect_s/IBoxConfig/
	adb push IBoxConfig\RTSPConfig.txt /protect_s/IBoxConfig/
	adb push IBoxConfig\VideoRecordConfig.txt /protect_s/IBoxConfig/

	adb push IBoxConfig\BroadcastConfig.txt /protect_t/IBoxConfig/
	adb push IBoxConfig\DeviceIDConfig.txt  /protect_t/IBoxConfig/
	adb push IBoxConfig\OBDServerConfig.txt /protect_t/IBoxConfig/
	adb push IBoxConfig\SocketConfig.txt /protect_t/IBoxConfig/
	adb push IBoxConfig\CaptureConfig.txt /protect_t/IBoxConfig/
	adb push IBoxConfig\FolderConfig.txt /protect_t/IBoxConfig/
	adb push IBoxConfig\RTSPConfig.txt /protect_t/IBoxConfig/
	adb push IBoxConfig\VideoRecordConfig.txt /protect_t/IBoxConfig/

	adb shell "echo %DeviceID% > /protect_s/IBoxDeviceID.config"
	adb shell "echo %DeviceID% > /protect_t/IBoxDeviceID.config"
	adb shell "cat /protect_s/IBoxDeviceID.config"
	adb shell sync

	set /p check_device_id="Check Your Device ID, Does it right?(Y/N) "
	ECHO "you Device ID is '%check_device_id%' !!"
	if %check_device_id%==Y GOTO CHECK_OK
	if %check_device_id%==y GOTO CHECK_OK
	if %check_device_id%==N GOTO CHECK_ERR
	if %check_device_id%==n GOTO CHECK_ERR

	goto START


:CHECK_OK
	echo "Check device id OK!"
	goto END

:CHECK_ERR
	echo "Check device id Error!"
	goto END


:END
pause




