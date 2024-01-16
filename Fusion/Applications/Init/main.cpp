#pragma once

#include "../../Core/CorePCH.hpp"
#include "Init.h"

#include "../../Core/App/ApplicationBase.h"

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int __argc, const char** __argv)
#endif
{
	nvrhi::GraphicsAPI api = donut::app::GetGraphicsAPIFromCommandLine(__argc, __argv);
	donut::app::DeviceManager* deviceManager = donut::app::DeviceManager::Create(api);

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

	/*{
		InitApp example(deviceManager);
		if (example.Init())
		{
			deviceManager->AddRenderPassToBack(&example);
			deviceManager->RunMessageLoop();
			deviceManager->RemoveRenderPass(&example);
		}
	}*/

	deviceManager->Shutdown();

	delete deviceManager;

	return 0;
}