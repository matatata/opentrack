@echo off

echo Setting up opentrack stuff

IF EXIST C:\opentrack rmdir /S /Q C:\opentrack

pushd "@opentrack-install-winebridge-winepath@"
mkdir C:\opentrack
copy Registry-Tool*.exe C:\opentrack\
popd

@echo off
IF "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
    CALL "@opentrack-install-winebridge-bin-winepath@\ftnoir-registry-tool64.exe" -v -e all
) ELSE (
    CALL "@opentrack-install-winebridge-bin-winepath@\ftnoir-registry-tool.exe" -v -e all
)


pause
exit
