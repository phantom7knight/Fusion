set(SRC_FILES
    "App/Camera/Camera.h"
    "App/Camera/Camera.cpp"

    "App/DX12/DeviceManager_DX12.cpp"
    "App/Vulkan/DeviceManager_VK.cpp"
	
    # "App/Imgui/imgui_console.h"
    # "App/Imgui/imgui_console.cpp"
    # "App/Imgui/imgui_nvrhi.h"
    # "App/Imgui/imgui_nvrhi.cpp"
    # "App/Imgui/imgui_renderer.h"
    # "App/Imgui/imgui_renderer.cpp"
	
    "App/DeviceManager.h"
    "App/DeviceManager.cpp"
	
	"App/ApplicationBase.h"
    "App/ApplicationBase.cpp"

    "Threading/BS_ThreadPool_Light.hpp"
    "Threading/BS_ThreadPool.hpp"
    
    "Utilities/Utilities.hpp"
	
    "Utilities/Timer.h"
    
    "Utilities/Logger/Logger.h"
    "Utilities/Logger/Logger.cpp")

# Create source groups for better organization in IDEs
source_group("App\\Camera" FILES "App/Camera/Camera.h" "App/Camera/Camera.cpp")
source_group("App\\DX12" FILES "App/DX12/DeviceManager_DX12.cpp")
source_group("App\\Vulkan" FILES "App/Vulkan/DeviceManager_VK.cpp")
# source_group("App\\Imgui" FILES "App/Imgui/imgui_console.h" "App/Imgui/imgui_console.cpp" "App/Imgui/imgui_nvrhi.h" "App/Imgui/imgui_nvrhi.cpp" "App/Imgui/imgui_renderer.h" "App/Imgui/imgui_renderer.cpp")
source_group("App" FILES "App/DeviceManager.h" "App/DeviceManager.cpp" "App/ApplicationBase.h" "App/ApplicationBase.cpp")
source_group("Threading" FILES "Threading/BS_ThreadPool_Light.hpp" "Threading/BS_ThreadPool.hpp")
source_group("Utilities" FILES "Utilities/Utilities.hpp" "Utilities/Timer.h")
source_group("Utilities\\Logger" FILES "Utilities/Logger/Logger.h" "Utilities/Logger/Logger.cpp")

add_library(Core STATIC ${SRC_FILES})
#target_link_libraries(Core PUBLIC External d3d12.lib d3dcompiler.lib dxcompiler.lib)

# Enable hot reload in Visual studio 2022.
if (MSVC AND WIN32 AND NOT MSVC_VERSION VERSION_LESS 141)

    target_link_options(Core PRIVATE $<$<CONFIG:Debug>:/INCREMENTAL>)
    target_compile_options(Core PRIVATE $<$<CONFIG:Debug>:/Zi>)

endif()

# Setup precompiled headers.
target_precompile_headers(
    Core
    PUBLIC
    "CorePCH.hpp"
)

target_link_libraries(Core PUBLIC External)# nvrhi Vulkan::Headers SDL2::SDL2main SDL2::SDL2-static)

# Now set the FOLDER property for the fetched target
set_target_properties(Core PROPERTIES FOLDER "Internal")