#pragma once

#include "../../Core/App/ApplicationBase.h"

//using namespace donut;

class InitApp : protected donut::app::IRenderPass
{
private:
	nvrhi::ShaderHandle mVertexShader;
	nvrhi::ShaderHandle mPixelShader;
	nvrhi::GraphicsPipelineHandle mGraphicsPipeline;
	nvrhi::CommandListHandle mCommandList;

public:
	using IRenderPass::IRenderPass;

	bool Init();
	void BackBufferResizing() override;
	void Animate(float fElapsedTimeSeconds) override;
	void Render(nvrhi::IFramebuffer* framebuffer) override;

};
