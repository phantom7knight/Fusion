set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

set(SRC_FILES
    "Deferred.h"
    "Deferred.cpp"
    "main.cpp")

# Create an executable target (application)
add_executable(Deferred ${SRC_FILES})
# target_include_directories(Deferred PUBLIC "Include" "../Shaders")
target_link_libraries(Deferred PUBLIC 
Core
External
d3dcompiler
dxcompiler
dxgi
jsoncpp_static
# libimgui
) #External1 imgui

# Set preprocessor directives
target_compile_definitions(Deferred PRIVATE
_WINDOWS
_CRT_SECURE_NO_WARNINGS
USE_DX12=1
USE_VK=1
DONUT_WITH_TASKFLOW=1
)

# Now set the FOLDER property for the fetched target
set_target_properties(Deferred PROPERTIES FOLDER "Applications/Raster")
