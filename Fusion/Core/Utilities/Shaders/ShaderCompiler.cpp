#include "ShaderCompiler.h"

namespace ShaderCompiler_Loc_Helper
{
	HRESULT SaveBlobToFile(IDxcResult*& aCompilationResults, IDxcBlob*& aBlob, std::wstring aPath, DXC_OUT_KIND aType)
	{
		HRESULT hRes = aCompilationResults->GetOutput(aType, IID_PPV_ARGS(&aBlob), nullptr);
		{
			// Make sure to have the "Compiled" folder inside the Shader Folder
			// TODO_RT: This needs to be addressed at some point

			FILE* fp = nullptr;
			_wfopen_s(&fp, aPath.c_str(), L"wb");
			assert(fp);

			fwrite(aBlob->GetBufferPointer(), aBlob->GetBufferSize(), 1, fp);
			fclose(fp);
		}
		return hRes;
	}
}

ShaderCompiler::ShaderCompiler()
{
	// Setup the DXC Instance
	DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&myDxcUtils));
	DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&myDxcCompiler));
	myDxcUtils->CreateDefaultIncludeHandler(&myDxcIncludeHandler);

	// make sure it's all loaded
	assert(myDxcUtils || myDxcCompiler || myDxcIncludeHandler);
}

ShaderCompiler::~ShaderCompiler()
{
	SafeRelease(myDxcIncludeHandler);
	SafeRelease(myDxcCompiler);
	SafeRelease(myDxcUtils);
}

void ShaderCompiler::LoadAndCompileShader(STDUniquePtr<Shader>& aShader, IDxcResult*& aCompilationResults)
{
	std::wstring sourcePath;
	sourcePath.append(SHADER_SOURCE_PATH);
	sourcePath.append(aShader->mShaderCreationDesc.mShaderName);

	IDxcBlobEncoding* sourceBlobEncoding = nullptr;
	if (FAILED(myDxcUtils->LoadFile(sourcePath.c_str(), nullptr, &sourceBlobEncoding)))
	{
#if _DEBUG
		wprintf(L"Failed to load shader file:\n%ws\n", sourcePath.c_str());
#endif
		AssertError("Failed to load shader file");
	}

	DxcBuffer sourceBuffer{};
	sourceBuffer.Ptr = sourceBlobEncoding->GetBufferPointer();
	sourceBuffer.Size = sourceBlobEncoding->GetBufferSize();
	sourceBuffer.Encoding = DXC_CP_ACP;

	LPCWSTR target = nullptr;

	switch (aShader->mShaderCreationDesc.mType)
	{
	case ShaderType::vertex:
		target = L"vs_6_6";
		break;
	case ShaderType::pixel:
		target = L"ps_6_6";
		break;
	case ShaderType::compute:
		target = L"cs_6_6";
		break;
	default:
		AssertError("Unimplemented shader type.");
		break;
	}

	std::vector<LPCWSTR> arguments;
	arguments.reserve(12);

	arguments.push_back(aShader->mShaderCreationDesc.mShaderName.c_str());
	arguments.push_back(L"-E");
	arguments.push_back(aShader->mShaderCreationDesc.mEntryPoint.c_str());
	arguments.push_back(L"-T");
	arguments.push_back(target);
	arguments.push_back(L"-Zi");
	arguments.push_back(L"-WX");
	arguments.push_back(L"-Qstrip_reflect");
	arguments.push_back(L"-Qstrip_debug");
#if _DEBUG
	arguments.push_back(DXC_ARG_DEBUG);
	arguments.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
#else
	arguments.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

	myDxcCompiler->Compile(&sourceBuffer, arguments.data(), static_cast<uint32_t>(arguments.size()), myDxcIncludeHandler, IID_PPV_ARGS(&aCompilationResults));

	// Save Shader Binary
	IDxcBlobUtf8* errors = nullptr;

	// check for errors
	if (FAILED(aCompilationResults->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr)))
	{
		AssertError("Failed to get shader compilation result.");
	}

	if (errors != nullptr && errors->GetStringLength() != 0)
	{
#if _DEBUG
		wprintf(L"Shader compilation error:\n%S\n", errors->GetStringPointer());
#endif
		AssertError("Shader compilation error");
	}
#if _DEBUG
	else
	{
		LOG_INFO("Shader compilation succeeded");
	}
#endif

	HRESULT statusResult;
	aCompilationResults->GetStatus(&statusResult);
	if (FAILED(statusResult))
	{
		AssertError("Shader compilation failed");
	}

	SafeRelease(errors);
}

void ShaderCompiler::SaveCompiledShader(const std::wstring aShaderName, IDxcBlob*& aShaderBlob, IDxcResult*& aCompilationResults)
{
	std::wstring dxilPath;
	std::wstring pdbPath;

	dxilPath.append(SHADER_OUTPUT_PATH);
	dxilPath.append(aShaderName);
	dxilPath.erase(dxilPath.end() - 5, dxilPath.end());
	dxilPath.append(L".dxil");

	pdbPath = dxilPath;
	pdbPath.append(L".pdb");

	if (FAILED(ShaderCompiler_Loc_Helper::SaveBlobToFile(aCompilationResults, aShaderBlob, dxilPath, DXC_OUT_OBJECT)))
	{
		AssertError("Failed to save the shader blob to file");
	}

	IDxcBlob* pdbBlob = nullptr;
	if (FAILED(ShaderCompiler_Loc_Helper::SaveBlobToFile(aCompilationResults, pdbBlob, pdbPath, DXC_OUT_PDB)))
	{
		AssertError("Failed to save the shader pdb blob to file");
	}

	SafeRelease(pdbBlob);
}

void ShaderCompiler::CreateShader(STDUniquePtr<Shader>& aShader)
{
	IDxcBlob* shaderBlob = nullptr;
	IDxcResult* compilationResults = nullptr;

	LoadAndCompileShader(aShader, compilationResults);
	SaveCompiledShader(aShader->mShaderCreationDesc.mShaderName, shaderBlob, compilationResults);

	// set the generated Shader Blob
	assert(shaderBlob);
	aShader->mShaderBlob = shaderBlob;
	
	// Clear the data
	SafeRelease(compilationResults);
}

void ShaderCompiler::DestroyShader(STDUniquePtr<Shader> shader)
{
	SafeRelease(shader->mShaderBlob);
#if _DEBUG
	LOG_INFO("Shader destroyed");
#endif
}