#pragma once

namespace ShaderCommons
{
	const std::wstring VertexShaderEntryPt = L"VertexShader";
	const std::wstring PixelShaderEntryPt = L"PixelShader";
	const std::wstring ComputeShaderEntryPt = L"CSMain";
}

class ShaderCompiler
{
public:
	ShaderCompiler();
	~ShaderCompiler();
	void CreateShader(STDUniquePtr<Shader>& aShader);
	void DestroyShader(STDUniquePtr<Shader> shader);

private:

	void LoadAndCompileShader(STDUniquePtr<Shader>& aShader, IDxcResult*& aCompilationResults);
	void SaveCompiledShader(const std::wstring aShaderName, IDxcBlob*& aShaderBlob, IDxcResult*& aCompilationResults);

	IDxcUtils* myDxcUtils = nullptr;
	IDxcCompiler3* myDxcCompiler = nullptr;
	IDxcIncludeHandler* myDxcIncludeHandler = nullptr;

	// todo_rt: maybe have a list of shaders so that that can be used for some cache'ing purpose?
};