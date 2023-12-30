set(SRC_FILES
    "Init.h"
    "Init.cpp"
    "main.cpp")

# Create an executable target (application)
add_executable(Init ${SRC_FILES})
# target_include_directories(Init PUBLIC "Include" "../Shaders")
target_link_libraries(Init PUBLIC External Core) #nvrhi.lib nvrhi_vk.lib nvrhi_d3d12.lib d3dcompiler.lib dxcompiler.lib)
# Set preprocessor directives
target_compile_definitions(Init PRIVATE USE_DX12=1) #USE_VK=1

# Now set the FOLDER property for the fetched target
set_target_properties(Init PROPERTIES FOLDER "Applications/Raster")