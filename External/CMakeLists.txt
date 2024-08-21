set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

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
    GIT_TAG 55b0dd4c2378e36033f16bdda3893dc2b09f9962 # July 3rd, 2024
    GIT_PROGRESS TRUE
)

# Set up STB
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb
  GIT_TAG beebb24b945efdea3b9bba23affb8eb3ba8982e7
  GIT_PROGRESS TRUE
)

# Set up TaskFlow
option(TF_BUILD_TESTS "" OFF)
option(TF_BUILD_EXAMPLES "" OFF)
FetchContent_Declare(
  taskflow
  GIT_REPOSITORY https://github.com/taskflow/taskflow
  GIT_TAG 10dfa7920d1a786099a92614c4183f6339975728
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

# Set up imgui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui
  GIT_TAG e471206b0889948c436f144669beb9b8875110c8 # Aug 20th, 2024
  GIT_PROGRESS TRUE
)

# Set up Eventpp
include(FetchContent)
FetchContent_Declare(
    eventpp
    GIT_REPOSITORY https://github.com/wqking/eventpp
    GIT_TAG        d2db8af2a46c79f8dc75759019fba487948e9442 # v0.1.3
)

FetchContent_MakeAvailable(nvrhi glfw jsoncpp cgltf stb imgui taskflow eventpp) # D3D12MemoryAllocator

# set necessary path
set(DEPENDENCY_PATH ${CMAKE_BINARY_DIR}/_deps)
set(VULKAN_HEADERS_PATH ${DEPENDENCY_PATH}/nvrhi-src/thirdparty/Vulkan-Headers/include)
set(STB_HEADERS_PATH ${DEPENDENCY_PATH}/stb-src)
set(CGLTF_HEADERS_PATH ${DEPENDENCY_PATH}/cgltf-src)
set(TASKFLOW_HEADERS_PATH ${DEPENDENCY_PATH}/taskflow-src)

#libimgui setup
set(imgui_srcs
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
	${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_dx12.cpp
    ${imgui_SOURCE_DIR}/imgui.h
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
)

add_library(libimgui STATIC ${imgui_srcs})

target_include_directories(libimgui PUBLIC 
${imgui_SOURCE_DIR} 
${imgui_SOURCE_DIR}/backends
${glfw_SOURCE_DIR}/include
${VULKAN_HEADERS_PATH}
)

#stb setup
set(stb_srcs
    ${STB_HEADERS_PATH}/stb_c_lexer.h
    ${STB_HEADERS_PATH}/stb_ds.h
    ${STB_HEADERS_PATH}/stb_image.h
    ${STB_HEADERS_PATH}/stb_image_resize2.h
    ${STB_HEADERS_PATH}/stb_image_write.h
    ${STB_HEADERS_PATH}/stb_include.h
    ${STB_HEADERS_PATH}/stb_truetype.h
    ${STB_HEADERS_PATH}/stb_voxel_render.h
    ${STB_HEADERS_PATH}/stb_divide.h
    ${STB_HEADERS_PATH}/stb_vorbis.c
)

add_library(libstb STATIC ${stb_srcs})

add_library(External INTERFACE)
target_link_libraries(External INTERFACE
nvrhi
nvrhi_d3d12
nvrhi_vk
Vulkan::Headers
glfw
jsoncpp_static
libstb
libimgui
eventpp
# taskflow
)

target_include_directories(External INTERFACE
${nvrhi_SOURCE_DIR}/include
# ${nvrhi_dx12_SOURCE_DIR}/include
# ${nvrhi_vulkan_SOURCE_DIR}/include
${jsoncpp_SOURCE_DIR}/include
${cgltf_SOURCE_DIR}
${stb_SOURCE_DIR}
${imgui_SOURCE_DIR}
${TASKFLOW_HEADERS_PATH}
# ${nvrhi_vulkan_target}
# ${nvrhi_SOURCE_DIR}/thirdparty/Vulkan-Headers/include/vulkan)
# ${D3D12MemoryAllocator_SOURCE_DIR}/include
# ${stb_SOURCE_DIR}
# ${sdl2_SOURCE_DIR}/include
)

# Now set the FOLDER property for the fetched target
set_target_properties(update_mappings PROPERTIES FOLDER "External")
set_target_properties(jsoncpp_static PROPERTIES FOLDER "External")
set_target_properties(jsoncpp_object PROPERTIES FOLDER "External")
set_target_properties(libimgui PROPERTIES FOLDER "External")
set_target_properties(libstb PROPERTIES FOLDER "External")
set_target_properties(glfw PROPERTIES FOLDER "External")
# set_target_properties(SDL2 PROPERTIES FOLDER "External")
# set_target_properties(SDL2main PROPERTIES FOLDER "External")
# set_target_properties(SDL2-static PROPERTIES FOLDER "External")
# set_target_properties(sdl_headers_copy PROPERTIES FOLDER "External")
#set_target_properties(stb PROPERTIES FOLDER "External")
