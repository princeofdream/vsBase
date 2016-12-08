
@echo OFF

REM  adb > null|| goto START
REM  goto END


adb devices |find "recovery"
if %errorlevel%==1 goto MODE_ERROR
ECHO "====================================="
echo "===         Mode OK!!!!!!!        ==="

	


:START
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
	ECHO "================================================================="
	ECHO "================================================================="

REM      adb shell "mount -o remount,rw /"
REM      adb shell "mkdir -p /protect_f/ /protect_s/"
REM      adb shell "mount -t ext4 /dev/block/platform/mtk-msdc.0/by-name/protect1 /protect_f/"
REM      adb shell "mount -t ext4 /dev/block/platform/mtk-msdc.0/by-name/protect2 /protect_s/"


REM  REM  Mount Fail and do format!!!! May cause SIM Card Error!!!!
REM      adb shell "mount |grep protect_s" |find "protect"
REM      if %errorlevel%==1 goto MOUNT_FAIL_LOOP1


REM  :REMOUNT
REM      adb shell "mount -t ext4 /dev/block/platform/mtk-msdc.0/by-name/protect1 /protect_f/"
REM      adb shell "mount -t ext4 /dev/block/platform/mtk-msdc.0/by-name/protect2 /protect_s/"

REM      adb shell "mount -o remount,rw /protect_f"
REM      adb shell "mount -o remount,rw /protect_s"


	adb shell "mount |grep protect" |find "protect"
	if %errorlevel%==1 goto MOUNT_FAIL



	adb shell "rm -rf /protect_f/IBoxConfig/ /protect_s/IBoxConfig/ "
	adb shell "mkdir -p /protect_f/IBoxConfig/"
	adb shell "mkdir -p /protect_s/IBoxConfig/"

	adb push .\IBoxConfig\CaptureConfig.txt /protect_s/IBoxConfig/ 2>null
	adb push .\IBoxConfig\FolderConfig.txt /protect_s/IBoxConfig/ 2>null
	adb push .\IBoxConfig\RTSPConfig.txt /protect_s/IBoxConfig/ 2>null
	adb push .\IBoxConfig\VideoRecordConfig.txt /protect_s/IBoxConfig/ 2>null

	adb push .\IBoxConfig\CaptureConfig.txt /protect_f/IBoxConfig/ 2>null
	adb push .\IBoxConfig\FolderConfig.txt /protect_f/IBoxConfig/ 2>null
	adb push .\IBoxConfig\RTSPConfig.txt /protect_f/IBoxConfig/ 2>null
	adb push .\IBoxConfig\VideoRecordConfig.txt /protect_f/IBoxConfig/ 2>null

	adb shell "echo %DeviceID% > /protect_s/IBoxDeviceID.config"
	adb shell "echo %DeviceID% > /protect_f/IBoxDeviceID.config"
	
	adb shell sync

	ECHO "================================================================="
	ECHO "Your ID is :"
	adb shell "cat /protect_s/IBoxDeviceID.config"
	ECHO "================================================================="
	set /p check_device_id="Check Your Device ID, Does it correct?(Y/N) "
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


REM  :MOUNT_FAIL_LOOP1
REM      echo "!!!!!!!!!!!!!!!!! FORMAT !!!!!!!!!!!!!!!!!!!!!"
REM      adb shell "mke2fs /dev/block/platform/mtk-msdc.0/by-name/protect1"
REM      adb shell "mke2fs /dev/block/platform/mtk-msdc.0/by-name/protect2"
REM      goto REMOUNT

:MOUNT_FAIL
	ECHO "================================================================="
	echo "Mount Fail !!!!!"
	ECHO "================================================================="
	goto END
	
:MODE_ERROR
	ECHO "================================================================="
	echo "Mode Fail! Not in recovery mode!"
	ECHO "================================================================="

:END
pause




