set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

set(SRC_FILES
    "Init.h"
    "Init.cpp"
    "main.cpp")

# Create an executable target (application)
add_executable(Init ${SRC_FILES})
# target_include_directories(Init PUBLIC "Include" "../Shaders")
target_link_libraries(Init PUBLIC 
Core
External
d3dcompiler
dxcompiler
dxgi
jsoncpp_static
# libimgui
) #External1 imgui

# Set preprocessor directives
target_compile_definitions(Init PRIVATE
_WINDOWS
_CRT_SECURE_NO_WARNINGS
USE_DX12=1
USE_VK=1
DONUT_WITH_TASKFLOW=1
)

# Now set the FOLDER property for the fetched target
set_target_properties(Init PROPERTIES FOLDER "Applications/Raster")
