#include "Init.h"

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#include "../../Core/Engine/CommonRenderPasses.h"
#include "../../Core/Engine/TextureCache.h"
#include "../../Core/App/DeviceManager.h"

bool InitApp::InitAppShaderSetup(std::shared_ptr<donut::engine::ShaderFactory> aShaderFactory)
{
	mTriangle.mVertexShader = aShaderFactory->CreateShader("Triangle.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
	mTriangle.mPixelShader = aShaderFactory->CreateShader("Triangle.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);
	
	mCube.mVertexShader = aShaderFactory->CreateShader("Cube.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
	mCube.mPixelShader = aShaderFactory->CreateShader("Cube.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);

	if (!mTriangle.mVertexShader || !mTriangle.mPixelShader ||
		!mCube.mVertexShader || !mCube.mPixelShader)
	{
		return false;
	}

	return true;
}

bool InitApp::Init()
{
	const std::filesystem::path baseShaderPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/Shaders/";
	std::filesystem::path appShaderPath = baseShaderPath / "Applications/Init/Generated";
	std::filesystem::path commonShaderPath = baseShaderPath / "Common";

	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Init", appShaderPath);
	rootFS->mount("/shaders/app", commonShaderPath);

	std::shared_ptr<donut::engine::ShaderFactory> shaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders/Init");

	if (!InitAppShaderSetup(shaderFactory))
		return false;

	// todo_rt; testing
	
	//std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	//rootFS->mount("/shaders/donut", baseShaderPath);

	mCommandList = GetDevice()->createCommandList();

	/*mCube.mConstantBuffer = GetDevice()->createBuffer(nvrhi::utils::CreateStaticConstantBufferDesc
	(sizeof(locInitHelpers::ConstantBufferEntry) * locInitHelpers::cNumViews, "Cube ConstantBuffer")
		.setInitialState(nvrhi::ResourceStates::ConstantBuffer)
		.setKeepInitialState(true));

	nvrhi::VertexAttributeDesc attributes[] = {
			nvrhi::VertexAttributeDesc()
				.setName("POSITION")
				.setFormat(nvrhi::Format::RGB32_FLOAT)
				.setOffset(0)
				.setBufferIndex(0)
				.setElementStride(sizeof(locInitHelpers::Vertex)),
			nvrhi::VertexAttributeDesc()
				.setName("UV")
				.setFormat(nvrhi::Format::RG32_FLOAT)
				.setOffset(0)
				.setBufferIndex(1)
				.setElementStride(sizeof(locInitHelpers::Vertex)),
	};

	mCube.mInputLayout = GetDevice()->createInputLayout(attributes, uint32_t(std::size(attributes)), mCube.mVertexShader);

	donut::engine::CommonRenderPasses cmnRenderPasses(GetDevice(), shaderFactory);
	donut::engine::TextureCache textureCache(GetDevice(), nativeFS, nullptr);*/


	return true;
}

void InitApp::BackBufferResizing()
{
	mTriangle.mGraphicsPipeline = nullptr;
}

void InitApp::Animate(float fElapsedTimeSeconds)
{
	GetDeviceManager()->SetInformativeWindowTitle("Hello World!!");
}


void InitApp::Render(nvrhi::IFramebuffer* framebuffer)
{
	if (!mTriangle.mGraphicsPipeline)
	{
		nvrhi::GraphicsPipelineDesc psoDesc;
		psoDesc.VS = mTriangle.mVertexShader;
		psoDesc.PS = mTriangle.mPixelShader;
		psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
		psoDesc.renderState.depthStencilState.depthTestEnable = false;

		mTriangle.mGraphicsPipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
	}

	mCommandList->open();

	nvrhi::utils::ClearColorAttachment(mCommandList, framebuffer, 0, nvrhi::Color(0.f));

	nvrhi::GraphicsState state;
	state.pipeline = mTriangle.mGraphicsPipeline;
	state.framebuffer = framebuffer;
	state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());

	mCommandList->setGraphicsState(state);

	nvrhi::DrawArguments args;
	args.vertexCount = 3;
	mCommandList->draw(args);

	mCommandList->close();
	GetDevice()->executeCommandList(mCommandList);
}