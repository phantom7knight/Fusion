@echo off
rem This batch script deletes all files in the "Build" folder located in the same path as the script.

rem Get the path where the script resides
set "scriptPath=%~dp0"

rem Define the folder path by concatenating the script path and "\Build"
set "folderPath=%scriptPath%../Build"

rem Navigate to the "Build" folder
cd /d "%folderPath%" || exit /b

rem Delete all files and subfolders in the "Build" folder
rd /s /q .

rem Update the folder path by changing it to the GLTF Models
set "folderPath=%scriptPath%../Assets/GLTFModels"

rem Navigate to the "Build" folder
cd /d "%folderPath%" || exit /b

rem Delete all files and subfolders in the "Build" folder
rd /s /q .

echo All files in %folderPath% have been deleted.

pause