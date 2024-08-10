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

#include "../../Core/Editors/Material/MaterialEditor.h"
#include "../../Core/Render/ForwardShadingPass.h"

class MaterialsPlayground;

struct UIOptions
{
	bool mVsync = true;
	bool mEnableMaterialEditor = false;
	bool mEnableTranslucency = true;
	bool mEnableDeferredShading = false;
	int mRTsViewMode = 0;
	int mCurrentlySelectedMeshIdx = 0;
	std::vector<const char*> mAppModeOptions = { "Final Image", "Diffuse", "Specular", "Normal", "Emissive", "Depth"};
};

class UIRenderer : public donut::app::ImGui_Renderer
{
public:
	UIRenderer(donut::app::DeviceManager* deviceManager, std::shared_ptr<MaterialsPlayground> aApp);

protected:
	virtual void BuildUI(void) override;

private:

	std::shared_ptr<donut::engine::MaterialEditor> mMaterialEditor;
	std::shared_ptr<MaterialsPlayground> mMaterialsPlaygroundApp;
};

class RenderTargets : public donut::render::GBufferRenderTargets
{
public:
	nvrhi::TextureHandle mOutputColor;

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
		textureDesc.isRenderTarget = true;
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
	}

	void Clear(nvrhi::ICommandList* aCommandList)
	{
		donut::render::GBufferRenderTargets::Clear(aCommandList);
		aCommandList->clearTextureFloat(mOutputColor, nvrhi::AllSubresources, nvrhi::Color(0.f));
	}
};

class MaterialsPlayground : public donut::app::ApplicationBase
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
	virtual void BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount);

	const dm::float3& GetCameraPosition() { return mCamera.GetPosition(); }
	std::shared_ptr<donut::engine::Scene> GetScene() { return mScene; }

	UIOptions mUIOptions;

private:

	nvrhi::CommandListHandle mCommandList;
	donut::engine::PlanarView mView;
	//donut::engine::PlanarView mPrevView; todo_rt; add this
	donut::engine::CompositeView mCompView;
	donut::app::FirstPersonCamera mCamera;
	std::shared_ptr<donut::engine::ShaderFactory> mShaderFactory;
	std::shared_ptr<donut::engine::Scene> mScene;
	std::unique_ptr<donut::engine::BindingCache> mBindingCache;
	std::unique_ptr<donut::engine::FramebufferFactory> mFwdFramebuffer;
	std::shared_ptr<donut::engine::DirectionalLight> mSunLight;
	std::unique_ptr<donut::render::GBufferFillPass> mGBufferFillPass;
	std::unique_ptr<donut::render::DeferredLightingPass> mDeferredLightingPass;
	std::unique_ptr<donut::render::ForwardShadingPass> mForwardShadingPass;
	std::unique_ptr<donut::render::InstancedOpaqueDrawStrategy> mOpaqueDrawStrategy;
	std::unique_ptr<donut::render::TransparentDrawStrategy> mTransparentDrawStrategy;
	std::vector<std::shared_ptr<donut::engine::SpotLight>> mLights;
	std::shared_ptr<RenderTargets> mRenderTargets;
};
