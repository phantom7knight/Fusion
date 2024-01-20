#pragma once

#include "../../Core/CorePCH.hpp"
#include "Init.h"

#include "../../Core/App/ApplicationBase.h"
#include "../../Core/App/DeviceManager.h"
#include "../../Core/Utilities/Logger/log.h"
#include "../../Core/ShaderMake/ShaderMake.hpp"

int main(int __argc, const char* __argv[])
{
	{
		// Look at the github page for example
		// https://github.com/NVIDIAGameWorks/ShaderMake.git
		// DX12
		//"C:\Users\Rohit Tolety\Downloads\tech temp\donut_examples\bin\ShaderMake.exe" 
		// --config "C:/Users/Rohit Tolety/Downloads/tech temp/donut_examples/examples/basic_triangle/shaders.cfg" 
		// --out "C:/Users/Rohit Tolety/Downloads/tech temp/donut_examples/bin/shaders/basic_triangle/dxil" 
		// --platform DXIL
		// --binaryBlob - I "C:/Users/Rohit Tolety/Downloads/tech temp/donut_examples/donut/include" 
		// --compiler "C:/Program Files (x86)/Windows Kits/10/bin/10.0.19041.0/x64/dxc.exe" 
		// --outputExt.bin 
		// --shaderModel 6_5 --useAPI

		// Vulkan
		//"C:\Users\Rohit Tolety\Downloads\tech temp\donut_examples\bin\ShaderMake.exe" 
		// --config "C:/Users/Rohit Tolety/Downloads/tech temp/donut_examples/examples/basic_triangle/shaders.cfg" 
		// --out "C:/Users/Rohit Tolety/Downloads/tech temp/donut_examples/bin/shaders/basic_triangle/spirv" 
		// --platform SPIRV 
		// --binaryBlob - I "C:/Users/Rohit Tolety/Downloads/tech temp/donut_examples/donut/include" - D SPIRV 
		// --compiler C:/VulkanSDK/1.3.216.0/Bin/dxc.exe 
		// --tRegShift 0 
		// --sRegShift 128 
		// --bRegShift 256 
		// --uRegShift 384 
		// --vulkanVersion 1.2 
		// --outputExt.bin --useAPI

		std::filesystem::path appShaderConfigPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/Shaders/Applications/Init/shaders.cfg";
		std::filesystem::path appShaderIncludesConfigPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/Shaders/Includes";
		std::string outConfigPath = "--config=" + appShaderConfigPath.string();
		std::string outputPath = "--out=" + donut::app::GetDirectoryWithExecutable().string() + "/../../../Assets/Shaders/Applications/Init/Generated/";

		const char* arguments[] = {
#if USE_DX12
			"ShaderMake.exe", // this doesn't matter
			"--platform=DXIL",
			outConfigPath.c_str(),
			outputPath.c_str(),
			"--compiler=C:/Program Files (x86)/Windows Kits/10/bin/10.0.20348.0/x64/dxc.exe",
			"--binary",
			"--binaryBlob",
			"--outputExt=.bin"
#else if USE_VK
			"ShaderMake.exe", // this doesn't matter
			"--platform=SPIRV",
			outConfigPath.c_str(),
			outputPath.c_str(),
			"--compiler=C:/VulkanSDK/1.3.216.0/Bin/dxc.exe",
			"--binary",
			"--binaryBlob",
			"--outputExt=.bin",
			"--tRegShift 0 ",
			"--sRegShift 128", 
			"--bRegShift 256", 
			"--uRegShift 384"
#endif
		};

		uint8_t arrSize = sizeof(arguments) / sizeof(arguments[0]);

		// Shader Code Generation
		ShaderCodeGeneration(arrSize, arguments);
	}

	donut::app::DeviceManager* deviceManager = donut::app::DeviceManager::Create(nvrhi::GraphicsAPI::D3D12);//api

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
		InitApp example(deviceManager);
		if (example.Init())
		{
			deviceManager->AddRenderPassToBack(&example);
			deviceManager->RunMessageLoop();
			deviceManager->RemoveRenderPass(&example);
		}
	}

	deviceManager->Shutdown();

	delete deviceManager;

	return 0;
}