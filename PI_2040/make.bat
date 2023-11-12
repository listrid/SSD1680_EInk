rem SET PATH=%~dp0../sdk;%PATH%;
SET RP2040_SDK=%~dp0../sdk
@echo off

%RP2040_SDK%\bin\make.exe
if %ERRORLEVEL% NEQ 0  (echo. Error make && exit /b 1 )


SET BUILD_UF2=
for /r %%x in (*.uf2) do SET BUILD_UF2="%%x"
if NOT EXIST %BUILD_UF2% ( echo 	'build\*.uf2' file not found && pause && exit -1
)
%RP2040_SDK%\bin\picotool.exe reboot -f -u
timeout 2
%RP2040_SDK%\bin\picotool.exe load -x %BUILD_UF2%
rem pause