@echo off
rem This batch script creates a "Build" directory, navigates into it,
rem configures the project using CMake, and then pauses execution.

rem Move to root folder
cd ../

rem Create a "Build" directory
mkdir Build

rem Change to the "Build" directory
cd Build

rem Run CMake to configure the project
rem cmake ..
cmake -G "Visual Studio 17 2022" -A x64 -T host=x64 ..

rem Pause execution to keep the console window open
pause
