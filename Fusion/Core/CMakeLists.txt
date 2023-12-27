set(SRC_FILES
    "App/App.h"

	"Threading/BS_ThreadPool_Light.hpp"
	"Threading/BS_ThreadPool.hpp"
    
	"Utilities/Utilities.hpp"
	
    "Utilities/Logger/Logger.h"
    "Utilities/Logger/Logger.cpp")

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

target_link_libraries(Core PUBLIC nvrhi)

# Now set the FOLDER property for the fetched target
set_target_properties(Core PROPERTIES FOLDER "Internal")