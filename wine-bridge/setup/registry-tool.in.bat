@echo off
IF "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    call "@opentrack-install-winebridge-bin-winepath@\ftnoir-registry-tool-gui64.exe"
) ELSE (
    call "@opentrack-install-winebridge-bin-winepath@\ftnoir-registry-tool-gui.exe"
)
exit

