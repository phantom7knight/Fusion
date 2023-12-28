@echo off
rem This batch script fetches a Git repository and copies it to a specific directory.

rem Set the Git repository URL
set "repoUrl=https://github.com/KhronosGroup/glTF-Sample-Models.git"

rem Get the path where the script resides
set "scriptPath=%~dp0"

rem Set the assets directory
set "assetsDir=%scriptPath%\Assets"

set "destinationDir=%assetsDir%\GLTFModels"

cd Assets

mkdir GLTFModels

rem Clone the Git repository
git clone --depth=1 %repoUrl% %TEMP%\repo_temp

rem Check if the clone was successful
if errorlevel 1 (
    echo Failed to clone the GLTF Models Git repository.
    exit /b 1
)

rem Copy the cloned repository to the destination directory
xcopy /E /Y %TEMP%\repo_temp %destinationDir%

rem Clean up the temporary directory
rd /s /q %TEMP%\repo_temp

echo Git repository has been fetched and copied to %destinationDir%.

pause
