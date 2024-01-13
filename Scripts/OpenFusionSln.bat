@echo off
rem This batch script builds a CMake project.
rem It creates a "build" directory, configures the project using CMake,
rem runs CMake to generate the solution file, starts the solution,
rem and then pauses execution.

rem Turn off command echoing
@echo off

rem Move to root folder
cd ../

rem Change to the "build" directory
cd Build

rem Display a message indicating that the generated solution is being run
echo Running the generated solution...

rem Start the generated solution file (assuming it's named Fusion.sln)
start Fusion.sln

rem Pause execution to keep the console window open
pause
