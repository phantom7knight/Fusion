#pragma once

#include "../../Core/App/ApplicationBase.h"
#include "../../Core/App/Camera/Camera.h"

#include "../../Core/Utilities/Math/math.h"

#include "../../Core/Engine/ShaderFactory.h"
#include "../../Core/Engine/Scene.h"
#include "../../Core/Engine/BindingCache.h"
#include "../../Core/Engine/FramebufferFactory.h"
#include "../../Core/App/Imgui/imgui_renderer.h"


#include "../../Core/Render/DrawStrategy.h"
#include "../../Core/Render/DeferredLightingPass.h"
#include "../../Core/Render/GBuffer.h"
#include "../../Core/Render/GBufferFillPass.h"

// todo_rt; testing
#include "../../Core/Render/ForwardShadingPass.h"

// TODO_RT
// 1. Allow multiple lights in the scenes
// 2. Draw more than 2 models in the scene
// 3. keep both fwd and def. pass as options
// 4. Run a hybrid pass with some models in fwd and others in deferred
//	  ex: render sun&light spheres(required step 2) in fwd and then rest of scene in deferred
// 5. Make a separate UIOptions class which has default options

class PBRTestingApp;

struct UIOptions
{
	bool mVsync = true;
	int mRTsViewMode = 0;
	std::vector<const char*> mAppModeOptions = { "Final Image", "Diffuse", "Specular", "Normal", "Emissive", "Depth"};
};

class UIRenderer : public donut::app::ImGui_Renderer
{
public:
	UIRenderer(donut::app::DeviceManager* deviceManager, std::shared_ptr<PBRTestingApp> aApp);

protected:
	virtual void BuildUI(void) override;

private:
	std::shared_ptr<PBRTestingApp> mPBRTestingApp;
};

class RenderTargets : public donut::render::GBufferRenderTargets
{
public:
	nvrhi::TextureHandle mOutputColor;

	// todo_rt; testing
	nvrhi::TextureHandle mFWDDepth;
	nvrhi::TextureHandle mFWDColor;
	std::shared_ptr<donut::engine::FramebufferFactory> mFWDFramebuffer;

	RenderTargets(nvrhi::IDevice* aDevice,
		const dm::uint2 aSize,
		const uint32_t aSampleCount,
		bool aEnableMotionVectors,
		bool aUseReverseProjection)
	{
		nvrhi::TextureDesc textureDesc;
		textureDesc.format = nvrhi::Format::RGBA16_FLOAT;
		textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
		textureDesc.initialState = nvrhi::ResourceStates::UnorderedAccess;
		textureDesc.isUAV = true;
		textureDesc.keepInitialState = true;
		textureDesc.debugName = "Lighting Pass: OutputBuffer";
		textureDesc.width = aSize.x;
		textureDesc.height = aSize.y;
		textureDesc.sampleCount = aSampleCount;
		mOutputColor = aDevice->createTexture(textureDesc);

		// Init GBuffer Render Targets
		Init(aDevice,
			aSize,
			aSampleCount,
			aEnableMotionVectors,
			aUseReverseProjection);

		// todo_rt; testing
		nvrhi::TextureDesc textureDesc2;
		textureDesc2.format = nvrhi::Format::SRGBA8_UNORM;
		textureDesc2.isRenderTarget = true;
		textureDesc2.initialState = nvrhi::ResourceStates::RenderTarget;
		textureDesc2.keepInitialState = true;
		textureDesc2.clearValue = nvrhi::Color(0.f);
		textureDesc2.useClearValue = true;
		textureDesc2.debugName = "Fwd Pass: ColorBuffer";
		textureDesc2.width = aSize.x;
		textureDesc2.height = aSize.y;
		textureDesc2.dimension = nvrhi::TextureDimension::Texture2D;
		mFWDColor = aDevice->createTexture(textureDesc2);

		textureDesc2.format = nvrhi::Format::D24S8;
		textureDesc2.debugName = "Fwd Pass: DepthBuffer";
		textureDesc2.initialState = nvrhi::ResourceStates::DepthWrite;
		mFWDDepth = aDevice->createTexture(textureDesc2);

		mFWDFramebuffer = std::make_shared<donut::engine::FramebufferFactory>(aDevice);
		mFWDFramebuffer->RenderTargets = { mFWDColor };
		mFWDFramebuffer->DepthTarget = mFWDDepth;
	}

	void Clear(nvrhi::ICommandList* aCommandList)
	{
		donut::render::GBufferRenderTargets::Clear(aCommandList);
		aCommandList->clearTextureFloat(mOutputColor, nvrhi::AllSubresources, nvrhi::Color(0.f));

		// todo_rt; testing
		aCommandList->clearDepthStencilTexture(mFWDDepth, nvrhi::AllSubresources, true, 0.f, true, 0);
		aCommandList->clearTextureFloat(mFWDColor, nvrhi::AllSubresources, nvrhi::Color(0.f));
	}
};

class PBRTestingApp : public donut::app::ApplicationBase
{
public:
	using ApplicationBase::ApplicationBase;

	bool Init();
	void BackBufferResizing() override;
	void Animate(float fElapsedTimeSeconds) override;
	void Render(nvrhi::IFramebuffer* framebuffer) override;
	bool LoadScene(std::shared_ptr<donut::vfs::IFileSystem> aFileSystem, const std::filesystem::path& aSceneFileName) override;
	bool KeyboardUpdate(int key, int scancode, int action, int mods) override;
	bool MousePosUpdate(double xpos, double ypos) override;
	bool MouseButtonUpdate(int button, int action, int mods) override;

	const dm::float3& GetCameraPosition() { return mCamera.GetPosition(); }

	UIOptions mUIOptions;

private:

	nvrhi::CommandListHandle mCommandList;
	donut::engine::PlanarView mView;
	donut::app::FirstPersonCamera mCamera;
	std::shared_ptr<donut::engine::ShaderFactory> mShaderFactory;
	std::unique_ptr<donut::engine::Scene> mScene;
	std::unique_ptr<donut::engine::BindingCache> mBindingCache;
	std::unique_ptr<donut::render::GBufferFillPass> mGBufferFillPass;
	std::unique_ptr<donut::render::DeferredLightingPass> mDeferredLightingPass;
	std::unique_ptr<donut::render::ForwardShadingPass> mForwardShadingPass;
	std::unique_ptr<donut::render::InstancedOpaqueDrawStrategy> mOpaqueDrawStrategy;
	std::unique_ptr<donut::render::TransparentDrawStrategy> mTransparentDrawStrategy;
	std::shared_ptr<donut::engine::DirectionalLight>  mSunLight;
	std::vector<std::shared_ptr<donut::engine::SpotLight>> mLights;
	std::shared_ptr<RenderTargets> mGBufferRenderTargets;
};
