#include "Init.h"

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>
#include "../../Core/Engine/ShaderFactory.h"

bool InitApp::Init()
{
	std::filesystem::path appShaderPath = donut::app::GetDirectoryWithExecutable() / "shaders/basic_triangle" / donut::app::GetShaderTypeName(GetDevice()->getGraphicsAPI());

	auto nativeFS = std::make_shared<donut::vfs::NativeFileSystem>();
	donut::engine::ShaderFactory shaderFactory(GetDevice(), nativeFS, appShaderPath);

	mVertexShader = shaderFactory.CreateShader("Triangle.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
	mPixelShader = shaderFactory.CreateShader("Triangle.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);

	if (!mVertexShader || !mPixelShader)
	{
		return false;
	}

	mCommandList = GetDevice()->createCommandList();

	return true;
}

void InitApp::BackBufferResizing()
{
	mGraphicsPipeline = nullptr;
}

void InitApp::Animate(float fElapsedTimeSeconds)
{
	GetDeviceManager()->SetInformativeWindowTitle("Hello World!!");
}


void InitApp::Render(nvrhi::IFramebuffer* framebuffer)
{
	if (!mGraphicsPipeline)
	{
		nvrhi::GraphicsPipelineDesc psoDesc;
		psoDesc.VS = mVertexShader;
		psoDesc.PS = mPixelShader;
		psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
		psoDesc.renderState.depthStencilState.depthTestEnable = false;

		mGraphicsPipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
	}

	mCommandList->open();

	nvrhi::utils::ClearColorAttachment(mCommandList, framebuffer, 0, nvrhi::Color(0.f));

	nvrhi::GraphicsState state;
	state.pipeline = mGraphicsPipeline;
	state.framebuffer = framebuffer;
	state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());

	mCommandList->setGraphicsState(state);

	nvrhi::DrawArguments args;
	args.vertexCount = 3;
	mCommandList->draw(args);

	mCommandList->close();
	GetDevice()->executeCommandList(mCommandList);
}