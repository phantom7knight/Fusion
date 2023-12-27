@echo off
rem This batch script creates a "Build" directory, navigates into it,
rem configures the project using CMake, and then pauses execution.

rem Create a "Build" directory
mkdir Build

rem Change to the "Build" directory
cd Build

rem Run CMake to configure the project
cmake ..

rem Pause execution to keep the console window open
pause
