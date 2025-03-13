@echo off
setlocal

REM Check if CMake is already installed
where cmake >nul 2>&1
if %errorlevel% equ 0 (
    echo CMake is already installed.
    pause
    exit /b
)

echo CMake is not installed. Do you want to start the download? (yes/no)
set /p "CONFIRM="

if /i "%CONFIRM%" NEQ "yes" (
    echo Download canceled.
    exit /b
)

REM Create cmake directory if it doesn't exist
set "INSTALL_DIR=%~dp0cmake"
if not exist "%INSTALL_DIR%" (
    mkdir "%INSTALL_DIR%"
)

REM Set version and download link for installer
set "CMAKE_VERSION=3.10.3"
set "CMAKE_INSTALLER=cmake-%CMAKE_VERSION%-win64-x64.msi"
set "DOWNLOAD_URL=https://github.com/Kitware/CMake/releases/download/v%CMAKE_VERSION%/%CMAKE_INSTALLER%"

REM Download CMake
echo Downloading CMake...
powershell -Command "Invoke-WebRequest -Uri %DOWNLOAD_URL% -OutFile %CMAKE_INSTALLER%"

if %errorlevel% neq 0 (
    echo Error downloading CMake. Please check your internet connection.
    exit /b
)

REM Extract files from CMake MSI with progress bar
echo Extracting files from CMake...
msiexec /a %CMAKE_INSTALLER% /qn TARGETDIR="%INSTALL_DIR%" /L*v "%INSTALL_DIR%\install.log"

if %errorlevel% neq 0 (
    echo Error extracting files. Please check the installation log.
    exit /b
)

REM Cleanup
del %CMAKE_INSTALLER%

REM Add CMake to system PATH variable
setx PATH "%PATH%;%INSTALL_DIR%\bin" /M

echo CMake has been successfully extracted to %INSTALL_DIR% and added to the PATH variable!
pause
endlocal
