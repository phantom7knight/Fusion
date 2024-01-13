# Using CMake's fetch content to pull and setup third party libraries.

include(FetchContent)

# Declare the external project
#============================

# Set up NVRHI
option(NVRHI_WITH_DX11 "Build nvrhi with DX11" OFF)
option(NVRHI_WITH_NVAPI "Build nvrhi with NVAPI" OFF)
option(NVRHI_WITH_RTXMU "Build nvrhi with RTXMU" OFF)
FetchContent_Declare(
    nvrhi
    GIT_REPOSITORY https://github.com/NVIDIAGameWorks/nvrhi.git
    GIT_TAG 89424010f5c3aef48a912fb9220e47a36c03a87c
    GIT_PROGRESS TRUE
)

# Set up STB
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb
  GIT_TAG 8b5f1f37b5b75829fc72d38e7b5d4bcbf8a26d55
  GIT_PROGRESS TRUE
)

# Set up GLFW
option(GLFW_BUILD_DOCS "" OFF)
option(GLFW_BUILD_WIN32 "" OFF)
option(GLFW_BUILD_EXAMPLES "" OFF)
option(GLFW_BUILD_TESTS "" OFF)
# option(GLFW_INSTALL "" OFF)
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG 7482de6071d21db77a7236155da44c172a7f6c9e
  GIT_PROGRESS TRUE
)

# Set up JsonCPP
option(JSONCPP_WITH_TESTS "" OFF)
option(JSONCPP_WITH_PKGCONFIG_SUPPORT "" OFF)
option(JSONCPP_WITH_POST_BUILD_UNITTEST "" OFF)
option(JSONCPP_WITH_CMAKE_PACKAGE "" OFF)
FetchContent_Declare(
    jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG        69098a18b9af0c47549d9a271c054d13ca92b006
)

# Set up cgltf
FetchContent_Declare(
    cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf
    GIT_TAG        3531d8c422114bcbc8b8564bb3eb602cb3a5ac37
)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG c6e0284ac58b3f205c95365478888f7b53b077e2
  GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(nvrhi glfw jsoncpp cgltf stb imgui) # D3D12MemoryAllocator stb)

add_library(External INTERFACE)
target_link_libraries(External INTERFACE
nvrhi
Vulkan::Headers
glfw
jsoncpp
cgltf
stb
imgui)#D3D12MemoryAllocator libimgui SDL2::SDL2main
target_include_directories(External INTERFACE
${nvrhi_SOURCE_DIR}/include
${jsoncpp_SOURCE_DIR}/include
${cgltf_SOURCE_DIR}
${stb_SOURCE_DIR}
${imgui_SOURCE_DIR} )
# ${nvrhi_vulkan_target}
# ${nvrhi_SOURCE_DIR}/thirdparty/Vulkan-Headers/include/vulkan)
# ${D3D12MemoryAllocator_SOURCE_DIR}/include
# ${stb_SOURCE_DIR}
# ${sdl2_SOURCE_DIR}/include 

# Now set the FOLDER property for the fetched target
# set_target_properties(cgltf PROPERTIES FOLDER "External")
set_target_properties(glfw PROPERTIES FOLDER "External")
set_target_properties(update_mappings PROPERTIES FOLDER "External")
set_target_properties(jsoncpp_static PROPERTIES FOLDER "External")
set_target_properties(jsoncpp_object PROPERTIES FOLDER "External")
# set_target_properties(jsoncpp_static PROPERTIES FOLDER "External")
# set_target_properties(jsoncpp_static PROPERTIES FOLDER "External")
# set_target_properties(libimgui PROPERTIES FOLDER "External")
# set_target_properties(SDL2 PROPERTIES FOLDER "External")
# set_target_properties(SDL2main PROPERTIES FOLDER "External")
# set_target_properties(SDL2-static PROPERTIES FOLDER "External")
# set_target_properties(sdl_headers_copy PROPERTIES FOLDER "External")
#set_target_properties(stb PROPERTIES FOLDER "External")
