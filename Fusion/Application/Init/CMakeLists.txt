set(SRC_FILES
    "Init.h"
    "Init.cpp"
    "main.cpp")
	

# Create an executable target (application)
add_executable(Init ${SRC_FILES})
# target_include_directories(Init PUBLIC "Include" "../Shaders")
target_link_libraries(Init PUBLIC External) #nvrhi.lib nvrhi_vk.lib nvrhi_d3d12.lib d3dcompiler.lib dxcompiler.lib)

# Now set the FOLDER property for the fetched target
set_target_properties(Init PROPERTIES FOLDER "Applications")