@echo off
echo Removing opentrack stuff

IF "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    call "@opentrack-install-winebridge-bin-winepath@\ftnoir-registry-tool64.exe" -v -d all
) ELSE (
    call "@opentrack-install-winebridge-bin-winepath@\ftnoir-registry-tool.exe" -v -d all
)

if exist C:\opentrack (
     rmdir /S /Q C:\opentrack
) else (
    echo nothing to do
)

echo Done
pause
exit
