#pragma once

#include "../CorePCH.hpp"
#include <nvrhi/nvrhi.h>


namespace AppUtils
{
	nvrhi::GraphicsAPI GetGraphicsAPIFromCommandLine(int argc, const char* const* argv)
	{
		for (int n = 1; n < argc; n++)
		{
			const char* arg = argv[n];

			if (!strcmp(arg, "-d3d12") || !strcmp(arg, "-dx12"))
				return nvrhi::GraphicsAPI::D3D12;
			else if (!strcmp(arg, "-vk") || !strcmp(arg, "-vulkan"))
				return nvrhi::GraphicsAPI::VULKAN;
		}

#if USE_DX12
		return nvrhi::GraphicsAPI::D3D12;
#elif USE_VK
		return nvrhi::GraphicsAPI::VULKAN;
#else
#error "No Graphics API defined"
#endif
	}
}


class App
{
public:
	App(const STDStringView aWindowTitle, uint32_t aWidth, uint32_t aHeight)
		:mWindowTitle(aWindowTitle), mWindowWidth(aWidth), mWindowHeight(aHeight)
	{
	}

	App(const App& aOther) = delete;
	App& operator=(const App& aOther) = delete;

	App(App&& aOther) = delete;
	App& operator=(App&& aOther) = delete;

protected:
	virtual void Init() = 0;
	virtual void Load() = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void UnLoad() = 0;
	virtual void Destroy() = 0;

protected:
	STDString mWindowTitle{};

	uint32_t mWindowWidth{};
	uint32_t mWindowHeight{};

	HWND m_windowHandle{};
};