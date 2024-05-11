#pragma once

#include "../../Core/CorePCH.hpp"
#include "Init.h"

#include "../../Core/App/ApplicationBase.h"
#include "../../Core/App/DeviceManager.h"
#include "../../Core/App/Imgui/imgui_renderer.h"

#include "../../Core/Utilities/Logger/log.h"
#include "../../Core/Engine/ShaderFactory.h"

#if _DEBUG
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>  
#include <crtdbg.h>
#endif

namespace MainInit_Private
{
	bool locShaderSetup(const nvrhi::GraphicsAPI aAPI)
	{
		// Generate Init Shaders and Common Shaders
		std::filesystem::path appShaderConfigPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/Shaders/Applications/Init/";
		std::filesystem::path commonShaderConfigPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/Shaders/Common/";
		std::filesystem::path includeShaderPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/Shaders/Includes/";
		std::filesystem::path renderPassesShaderPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/Shaders/RenderPasses/";

		if (!donut::engine::ShadersCompile(appShaderConfigPath, includeShaderPath, aAPI) ||
			!donut::engine::ShadersCompile(commonShaderConfigPath, includeShaderPath, aAPI) ||
			!donut::engine::ShadersCompile(renderPassesShaderPath, includeShaderPath, aAPI))
			return false;

		return true;

	}
}

int main(int __argc, const char* __argv[])
{
	constexpr nvrhi::GraphicsAPI API = nvrhi::GraphicsAPI::D3D12;

	donut::app::DeviceManager* deviceManager = donut::app::DeviceManager::Create(API);

	donut::app::DeviceCreationParameters deviceParams;
#ifdef _DEBUG
	deviceParams.enableDebugRuntime = true;
	deviceParams.enableNvrhiValidationLayer = true;
#endif

	if (!deviceManager->CreateWindowDeviceAndSwapChain(deviceParams))
	{
		donut::log::fatal("Cannot initialize a graphics device with the requested parameters");
		return 1;
	}

	deviceManager->SetInformativeWindowTitle("Hello World!!");

	{
		// Shader Generation Setup
		if (!MainInit_Private::locShaderSetup(deviceManager->GetGraphicsAPI()))
		{
			donut::log::fatal("Shader creation setup failed!!!");
		}
	}

	{
		std::shared_ptr<InitApp> example = std::make_shared<InitApp>(deviceManager);
		std::shared_ptr<UIRenderer> uiRenderer = std::make_shared<UIRenderer>(deviceManager, example);

		if (example->Init())
		{
			deviceManager->AddRenderPassToBack(example.get());
			deviceManager->AddRenderPassToBack(uiRenderer.get());

			deviceManager->RunMessageLoop();
			
			deviceManager->RemoveRenderPass(example.get());
			deviceManager->RemoveRenderPass(uiRenderer.get());
		}
	}

	deviceManager->Shutdown();

	delete deviceManager;

	int* a = new int(5);


#if _DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return 0;
}