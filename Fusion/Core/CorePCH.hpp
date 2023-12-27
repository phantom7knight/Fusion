#pragma once

// STL includes.
#include <array>
#include <chrono>
#include <cmath>
#include <filesystem>
#include <format>
#include <iostream>
#include <ranges>
#include <source_location>
#include <random>
#include <span>
#include <string>
#include <string_view>
#include <future>
#include <queue>
#include <vector>
#include <unordered_map>
#include <stdarg.h>

// Win32 / DirectX12 / DXGI includes.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

// #include <D3D12MemAlloc.h>
#include <DirectXMath.h>
#include <d3d12.h>
#include <d3dcompiler.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <wrl.h>
// #include "Graphics/d3dx12.hpp"

// Custom includes.
//#include "Utilities/Utilities.hpp"

// Namespace aliases.
namespace wrl = Microsoft::WRL;
namespace math = DirectX;


// Setup constexpr indicating which build configuration was used.
#ifdef _DEBUG
constexpr bool FUSION_DEBUG_MODE = true;
#else
constexpr bool FUSION_DEBUG_MODE = false;
#endif

// Global variables.
constexpr uint32_t INVALID_INDEX_U32 = 0xFFFFFFFF;

// Custom typedefs
// ===================================

// Strings
#define STDString std::string;
#define STDStringView std::string_view;
#define WSTDStringView std::wstring_view;

// Unique Ptr's
#define STDUniquePtr std::unique_ptr
#define STDMakeUniquePtr std::make_unique

// Shared Ptr's
#define STDSharedPtr std::shared_ptr
#define STDMakeSharedPtr std::make_shared

// Weak Ptr's
#define STDWeakPtr std::weak_ptr

// STD Array
#define STDArray std::array

// STD Vector
#define STDVector std::vector

// STD Pair
#define STDPair std::pair

// STD Move
#define STDMove std::move

// STD Ordererd Map
#define STDMap std::map

// STD Un-Ordererd Map
#define STDHashMap std::unordered_map

// STD Ref
#define STDRef std::ref