/*
* Copyright (c) 2014-2021, NVIDIA CORPORATION. All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
* THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

#include "ShaderFactory.h"
#include "../VFS/VFS.h"
#include "../Utilities/Logger/log.h"
#include "../ShaderMake/ShaderBlob.h"
#include "../ShaderMake/ShaderMake.hpp"

using namespace donut::vfs;
using namespace donut::engine;

bool donut::engine::ShadersCompile(std::filesystem::path aBaseShaderPath, std::filesystem::path aShaderIncludesPath, const nvrhi::GraphicsAPI aAPI)
{
	// Look at the github page for example: https://github.com/NVIDIAGameWorks/ShaderMake.git

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
	// --binaryBlob 
    // - I "C:/Users/Rohit Tolety/Downloads/tech temp/donut_examples/donut/include" 
    // - D SPIRV 
	// --compiler C:/VulkanSDK/1.3.216.0/Bin/dxc.exe 
	// --tRegShift 0 
	// --sRegShift 128 
	// --bRegShift 256 
	// --uRegShift 384 
	// --vulkanVersion 1.3
	// --outputExt.bin --useAPI

	std::filesystem::path appShaderConfigPath = aBaseShaderPath / "shaders.cfg";
	std::string configPath = "--config=" + appShaderConfigPath.string();
	std::string outputPath = "--out=" + aBaseShaderPath.string() + "/Generated/";

	std::string platformArg = "--platform=";
	std::string compilerArg = "--compiler=";
    std::string includeArg = "--include=" + aShaderIncludesPath.string();
	std::string additionalArgs = "";

	if (aAPI == nvrhi::GraphicsAPI::D3D12)
	{
		platformArg += "DXIL";
		compilerArg += "C:/Program Files (x86)/Windows Kits/10/bin/10.0.20348.0/x64/dxc.exe";
		additionalArgs += "--outputExt=.bin";
	}
	else
	{
		platformArg += "SPIRV";
		compilerArg += "C:/VulkanSDK/1.3.216.0/Bin/dxc.exe";
		additionalArgs += " --outputExt=.bin --tRegShift=0, --sRegShift=128, --bRegShift=256, --uRegShift=384 --vulkanVersion=1.3 --useAPI";
	}

	const char* arguments[] = {
		"ShaderMake.exe", // this doesn't matter
		platformArg.c_str(),
        configPath.c_str(),
		outputPath.c_str(),
		compilerArg.c_str(),
        includeArg.c_str(),
		"--binary",
		"--binaryBlob",
        additionalArgs.c_str(),
	};

	uint8_t arrSize = sizeof(arguments) / sizeof(arguments[0]);

	// Shader Code Generation
	return !ShaderCodeGeneration(arrSize, arguments);
}


ShaderFactory::ShaderFactory(nvrhi::DeviceHandle rendererInterface,
	std::shared_ptr<IFileSystem> fs,
	const std::filesystem::path& basePath)
	: m_Device(rendererInterface)
	, m_fs(fs)
	, m_basePath(basePath)
{
}

void ShaderFactory::ClearCache()
{
	m_BytecodeCache.clear();
}

std::shared_ptr<IBlob> ShaderFactory::GetBytecode(const char* fileName, const char* entryName)
{
    if (!entryName)
        entryName = "main";

    std::string adjustedName = fileName;
    {
        size_t pos = adjustedName.find(".hlsl");
        if (pos != std::string::npos)
            adjustedName.erase(pos, 5);

        if (entryName && strcmp(entryName, "main"))
            adjustedName += "_" + std::string(entryName);
    }

    std::string ext = m_Device->getGraphicsAPI() == nvrhi::GraphicsAPI::D3D12 ? ".bin" : ".spirv";

    std::filesystem::path shaderFilePath = m_basePath / (adjustedName + ext);

    std::shared_ptr<IBlob>& data = m_BytecodeCache[shaderFilePath.generic_string()];

    if (data)
        return data;

    data = m_fs->readFile(shaderFilePath);

    if (!data)
    {
        log::error("Couldn't read the binary file for shader %s from %s", fileName, shaderFilePath.generic_string().c_str());
        return nullptr;
    }

    return data;
}

nvrhi::ShaderHandle ShaderFactory::CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, nvrhi::ShaderType shaderType)
{
    nvrhi::ShaderDesc desc = nvrhi::ShaderDesc(shaderType);
    desc.debugName = fileName;
    return CreateShader(fileName, entryName, pDefines, desc);
}

nvrhi::ShaderHandle ShaderFactory::CreateShader(const char* fileName, const char* entryName, const std::vector<ShaderMacro>* pDefines, const nvrhi::ShaderDesc& desc)
{
    std::shared_ptr<IBlob> byteCode = GetBytecode(fileName, entryName);

    if(!byteCode)
        return nullptr;

    std::vector<ShaderMake::ShaderConstant> constants;
    if (pDefines)
    {
        for (const ShaderMacro& define : *pDefines)
            constants.push_back(ShaderMake::ShaderConstant{ define.name.c_str(), define.definition.c_str() });
    }

    nvrhi::ShaderDesc descCopy = desc;
    descCopy.entryName = entryName;

    const void* permutationBytecode = nullptr;
    size_t permutationSize = 0;
    if (!ShaderMake::FindPermutationInBlob(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()), &permutationBytecode, &permutationSize))
    {
        const std::string message = ShaderMake::FormatShaderNotFoundMessage(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()));
        log::error("%s", message.c_str());
        
        return nullptr;
    }

    return m_Device->createShader(descCopy, permutationBytecode, permutationSize);
}

nvrhi::ShaderLibraryHandle ShaderFactory::CreateShaderLibrary(const char* fileName, const std::vector<ShaderMacro>* pDefines)
{
    std::shared_ptr<IBlob> byteCode = GetBytecode(fileName, nullptr);

    if (!byteCode)
        return nullptr;

    std::vector<ShaderMake::ShaderConstant> constants;
    if (pDefines)
    {
        for (const ShaderMacro& define : *pDefines)
            constants.push_back(ShaderMake::ShaderConstant{ define.name.c_str(), define.definition.c_str() });
    }
    
    const void* permutationBytecode = nullptr;
    size_t permutationSize = 0;
    if (!ShaderMake::FindPermutationInBlob(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()), &permutationBytecode, &permutationSize))
    {
        const std::string message = ShaderMake::FormatShaderNotFoundMessage(byteCode->data(), byteCode->size(), constants.data(), uint32_t(constants.size()));
        log::error("%s", message.c_str());

        return nullptr;
    }

    return m_Device->createShaderLibrary(permutationBytecode, permutationSize);
}
