cmake_minimum_required(VERSION 3.20)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
    if (NOT DEFINED CMAKE_GENERATOR)
        set(CMAKE_GENERATOR "Visual Studio 16 2019" CACHE STRING "Generator for Visual Studio 2019" FORCE)
    endif()
endif()

project(Fusion LANGUAGES CXX)

# Disable building of tests and CTestDashboardTargets
option(BUILD_TESTING "Build testing targets" OFF)

set(CMAKE_CXX_STANDARD 20)

# Specify output paths for all configurations
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)

# Ensure compile options are applied globally
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/TP>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:/EHsc>")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(PREDEFINED_TARGETS_FOLDER "CustomTargets")

set(FusionRootDir "${CMAKE_CURRENT_SOURCE_DIR}")

add_subdirectory(External)
add_subdirectory(Fusion)

set_target_properties(uninstall PROPERTIES FOLDER "CMakePredefinedTargets")
