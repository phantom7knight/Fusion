# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

set(SRC_FILES
    "App/Camera/Camera.h"
    "App/Camera/Camera.cpp"

    "App/DX12/DeviceManager_DX12.cpp"
    "App/Vulkan/DeviceManager_VK.cpp"
	
    "App/Imgui/imgui_console.h"
    "App/Imgui/imgui_console.cpp"
    "App/Imgui/imgui_nvrhi.h"
    "App/Imgui/imgui_nvrhi.cpp"
    "App/Imgui/imgui_renderer.h"
    "App/Imgui/imgui_renderer.cpp"
	
    "App/DeviceManager.h"
    "App/DeviceManager.cpp"
	
	"App/ApplicationBase.h"
    "App/ApplicationBase.cpp"

    "Threading/BS_ThreadPool_Light.hpp"
    "Threading/BS_ThreadPool.hpp"

    "VFS/Compression.h"
	"VFS/Compression.cpp"
	"VFS/TarFile.h"
	"VFS/TarFile.cpp"
	"VFS/VFS.h"
	"VFS/VFS.cpp"
	"VFS/WinResFS.h"
	"VFS/WinResFS.cpp"

	"Engine/BindingCache.h"
	"Engine/BindingCache.cpp"
	"Engine/CommonRenderPasses.h"
	"Engine/CommonRenderPasses.cpp"
	"Engine/ConsoleInterpreter.h"
	"Engine/ConsoleInterpreter.cpp"
	"Engine/ConsoleObjects.h"
	"Engine/ConsoleObjects.cpp"
	"Engine/DDSFile.h"
	"Engine/DDSFile.cpp"
	"Engine/DescriptorTableManager.h"
	"Engine/DescriptorTableManager.cpp"
	"Engine/FramebufferFactory.h"
	"Engine/FramebufferFactory.cpp"
	"Engine/GltfImporter.h"
	"Engine/GltfImporter.cpp"
	"Engine/KeyframeAnimation.h"
	"Engine/KeyframeAnimation.cpp"
	"Engine/Material.cpp"
	"Engine/MaterialBindingCache.h"
	"Engine/MaterialBindingCache.cpp"
	"Engine/TextureCache.h"
	"Engine/TextureCache.cpp"
	"Engine/Scene.h"
	"Engine/Scene.cpp"
	"Engine/SceneGraph.h"
	"Engine/SceneGraph.cpp"
	"Engine/SceneTypes.h"
	"Engine/SceneTypes.cpp"
	"Engine/ShaderFactory.h"
	"Engine/ShaderFactory.cpp"
	"Engine/TextureCache.h"
	"Engine/TextureCache.cpp"
	"Engine/ShadowMap.h"
	"Engine/View.h"
	"Engine/View.cpp"
	"Engine/stb_impl.c"

	"Render/DrawStrategy.h"
	"Render/DrawStrategy.cpp"
	"Render/GeometryPasses.h"
	"Render/GeometryPasses.cpp"

	"ShaderMake/argparse.h"
	"ShaderMake/argparse.c"
	"ShaderMake/ShaderBlob.h"
	"ShaderMake/ShaderBlob.cpp"
	"ShaderMake/ShaderMake.hpp"

    "Utilities/Logger/Log.h"
    "Utilities/Logger/Log.cpp"
	
	"Utilities/Utilities.hpp"
	
	"Utilities/json.h"
	"Utilities/json.cpp"
	
    "Utilities/Timer.h"
	
    "Utilities/string_utils.h"
	"Utilities/string_utils.cpp"

	# "Utilities/Shaders/ShaderCompiler.h"
	# "Utilities/Shaders/ShaderCompiler.cpp"
	
	"Utilities/Math/affine.h"
	"Utilities/Math/basics.h"
	"Utilities/Math/box.h"
	"Utilities/Math/color.h"
	"Utilities/Math/color.cpp"
	"Utilities/Math/frustum.h"
	"Utilities/Math/frustum.cpp"
	"Utilities/Math/matrix.h"
	"Utilities/Math/matrix.cpp"
	"Utilities/Math/math.h"
	"Utilities/Math/quat.h"
	"Utilities/Math/sphere.h"
	"Utilities/Math/vector.h"
	"Utilities/Math/vector.cpp"
	)

# Create source groups for better organization in IDEs
source_group("App\\Camera" FILES
	"App/Camera/Camera.h"
	"App/Camera/Camera.cpp")
source_group("App\\DX12" FILES "App/DX12/DeviceManager_DX12.cpp")
source_group("App\\Vulkan" FILES "App/Vulkan/DeviceManager_VK.cpp")
source_group("App" FILES
	"App/DeviceManager.h"
	"App/DeviceManager.cpp"
	"App/ApplicationBase.h"
	"App/ApplicationBase.cpp")
source_group("Engine" FILES
	"Engine/BindingCache.h"
	"Engine/BindingCache.cpp"
	"Engine/CommonRenderPasses.h"
	"Engine/CommonRenderPasses.cpp"
	"Engine/ConsoleInterpreter.h"
	"Engine/ConsoleInterpreter.cpp"
	"Engine/ConsoleObjects.h"
	"Engine/ConsoleObjects.cpp"
	"Engine/DDSFile.h"
	"Engine/DDSFile.cpp"
	"Engine/DescriptorTableManager.h"
	"Engine/DescriptorTableManager.cpp"
	"Engine/FramebufferFactory.h"
	"Engine/FramebufferFactory.cpp"
	"Engine/GltfImporter.h"
	"Engine/GltfImporter.cpp"
	"Engine/KeyframeAnimation.h"
	"Engine/KeyframeAnimation.cpp"
	"Engine/Material.cpp"
	"Engine/MaterialBindingCache.h"
	"Engine/MaterialBindingCache.cpp"
	"Engine/Scene.h"
	"Engine/Scene.cpp"
	"Engine/SceneGraph.h"
	"Engine/SceneGraph.cpp"
	"Engine/SceneTypes.h"
	"Engine/SceneTypes.cpp"
	"Engine/ShaderFactory.h"
	"Engine/ShaderFactory.cpp"
	"Engine/TextureCache.h"
	"Engine/TextureCache.cpp"
	"Engine/ShadowMap.h"
	"Engine/View.h"
	"Engine/View.cpp"
	"Engine/stb_impl.c")
source_group("Render" FILES
	"Render/DrawStrategy.h"
	"Render/DrawStrategy.cpp"
	"Render/GeometryPasses.h"
	"Render/GeometryPasses.cpp")
source_group("ShaderMake" FILES
	"ShaderMake/argparse.h"
	"ShaderMake/argparse.c"
	"ShaderMake/ShaderBlob.h"
	"ShaderMake/ShaderBlob.cpp"
	"ShaderMake/ShaderMake.hpp")
source_group("VFS" FILES
	"VFS/Compression.h"
	"VFS/Compression.cpp"
	"VFS/TarFile.h"
	"VFS/TarFile.cpp"
	"VFS/VFS.h"
	"VFS/VFS.cpp"
	"VFS/WinResFS.h"
	"VFS/WinResFS.cpp")
source_group("Threading" FILES
	"Threading/BS_ThreadPool_Light.hpp"
	"Threading/BS_ThreadPool.hpp")
source_group("Utilities" FILES
	"Utilities/json.h"
	"Utilities/json.cpp"
	"Utilities/Utilities.hpp"
	"Utilities/Timer.h"
	"Utilities/string_utils.h"
	"Utilities/string_utils.cpp")
source_group("Utilities\\Logger" FILES
	"Utilities/Logger/Log.h"
	"Utilities/Logger/Log.cpp")
# source_group("Utilities\\Shaders" FILES
	# "Utilities/Shaders/ShaderCompiler.h"
	# "Utilities/Shaders/ShaderCompiler.cpp")
source_group("Utilities\\Math" FILES
	"Utilities/Math/affine.h"
	"Utilities/Math/basics.h"
	"Utilities/Math/box.h" 
	"Utilities/Math/color.h"
	"Utilities/Math/color.cpp"
	"Utilities/Math/frustum.h"
	"Utilities/Math/frustum.cpp"
	"Utilities/Math/affine.h"
	"Utilities/Math/matrix.h"
	"Utilities/Math/matrix.cpp"
	"Utilities/Math/math.h"
	"Utilities/Math/quat.h"
	"Utilities/Math/sphere.h"
	"Utilities/Math/vector.h"
	"Utilities/Math/vector.cpp")
source_group("App\\Imgui" FILES
 "App/Imgui/imgui_console.h"
 "App/Imgui/imgui_console.cpp"
 "App/Imgui/imgui_nvrhi.h"
 "App/Imgui/imgui_nvrhi.cpp"
 "App/Imgui/imgui_renderer.h"
 "App/Imgui/imgui_renderer.cpp")

add_library(Core STATIC ${SRC_FILES})

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /TP")

target_compile_options(Core PUBLIC /W3 /TP)
set_property(TARGET Core PROPERTY CXX_STANDARD 20)

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

# Link libraries, compile definitions, and set the FOLDER property
target_link_libraries(Core PUBLIC External) #nvrhi
target_compile_definitions(Core PRIVATE 
USE_DX12=1
USE_VK=1
VK_USE_PLATFORM_WIN32_KHR
_CRT_SECURE_NO_WARNINGS
_WINDOWS
DONUT_WITH_TASKFLOW)
set_target_properties(Core PROPERTIES FOLDER "Internal")


message(STATUS "==================================")
message(STATUS "PROJECT_NAME: Fusion: Core")
message(STATUS "==================================")
message(STATUS "CMAKE_HOST_SYSTEM: ${CMAKE_HOST_SYSTEM}")
message(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
message(STATUS "CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
message(STATUS "CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})
message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_CXX_STANDARD: " ${CMAKE_CXX_STANDARD})
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: " ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})
message(STATUS "CMAKE_CURRENT_LIST_DIR: " ${CMAKE_CURRENT_LIST_DIR})
message(STATUS "CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_INSTALL_PREFIX: " ${CMAKE_INSTALL_PREFIX})
message(STATUS "CMAKE_INSTALL_FULL_INCLUDEDIR: " ${CMAKE_INSTALL_FULL_INCLUDEDIR})
message(STATUS "CMAKE_INSTALL_FULL_LIBDIR: " ${CMAKE_INSTALL_FULL_LIBDIR})
