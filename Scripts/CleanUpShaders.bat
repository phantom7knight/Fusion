@echo off
rem === Batch Script to Delete Files in "Generated" Folders ===

rem === Applications/Init/Generated ===

rem Get the path where the script resides
set "scriptPath=%~dp0"

rem Define the folder path for Applications/Init/Generated
set "folderPath=%scriptPath%../Assets/Shaders/Applications/Init/Generated"

rem Navigate to the "Applications/Init/Generated" folder
cd /d "%folderPath%" || exit /b

rem Delete all files and subfolders in the "Applications/Init/Generated" folder
rd /s /q .

echo All files in Applications/Init/Generated have been deleted.

rem === RenderPasses/Generated ===

rem Get the path where the script resides
set "scriptPath=%~dp0"

rem Define the folder path for RenderPasses/Generated
set "folderPath=%scriptPath%../Assets/Shaders/RenderPasses/Generated"

rem Navigate to the "RenderPasses/Generated" folder
cd /d "%folderPath%" || exit /b

rem Delete all files and subfolders in the "RenderPasses/Generated" folder
rd /s /q .

echo All files in RenderPasses/Generated have been deleted.

rem === Common/Generated ===

rem Get the path where the script resides
set "scriptPath=%~dp0"

rem Define the folder path for Common/Generated
set "folderPath=%scriptPath%../Assets/Shaders/Common/Generated"

rem Navigate to the "Common/Generated" folder
cd /d "%folderPath%" || exit /b

rem Delete all files and subfolders in the "Common/Generated" folder
rd /s /q .

echo All files in Common/Generated have been deleted.

pause
