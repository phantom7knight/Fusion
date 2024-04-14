#pragma once

#include "../../Core/CorePCH.hpp"
#include "Deferred.h"

#include "../../Core/App/ApplicationBase.h"
#include "../../Core/App/DeviceManager.h"
#include "../../Core/App/Imgui/imgui_renderer.h"

#include "../../Core/Utilities/Logger/log.h"
#include "../../Core/Engine/ShaderFactory.h"

namespace locHelperFunc
{
	bool ShaderSetup(const nvrhi::GraphicsAPI aAPI)
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

	if (!deviceManager->CreateWindowDeviceAndSwapChain(deviceParams, "Hello World!!!"))
	{
		donut::log::fatal("Cannot initialize a graphics device with the requested parameters");
		return 1;
	}

	{
		// Shader Generation Setup
		if (!locHelperFunc::ShaderSetup(deviceManager->GetGraphicsAPI()))
		{
			donut::log::fatal("Shader creation setup failed!!!");
		}
	}

	{
		std::shared_ptr<DeferredApp> example = std::make_shared<DeferredApp>(deviceManager);
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

	return 0;
}