set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

set(SRC_FILES
    "MeshShaders.h"
    "MeshShaders.cpp"
    "main.cpp")

# Create an executable target (application)
add_executable(MeshShaders ${SRC_FILES})
# target_include_directories(MeshShaders PUBLIC "Include" "../Shaders")
target_link_libraries(MeshShaders PUBLIC 
Core
External
d3dcompiler
dxcompiler
dxgi
jsoncpp_static
)

# Set preprocessor directives
target_compile_definitions(MeshShaders PRIVATE
_WINDOWS
_CRT_SECURE_NO_WARNINGS
USE_DX12=1
USE_VK=1
DONUT_WITH_TASKFLOW=1
)

# Now set the FOLDER property for the fetched target
set_target_properties(MeshShaders PROPERTIES FOLDER "Applications/Mesh Shading")

add_compile_options(/TP)
