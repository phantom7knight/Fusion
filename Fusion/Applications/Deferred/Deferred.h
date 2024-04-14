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
#include "../../Core/Render/ForwardShadingPass.h"

namespace locInitHelpers
{
	struct Vertex
	{
		dm::float3 position;
		dm::float2 uv;
	};

	static const Vertex gVertices[] = {
		{ {-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} }, // front face
		{ { 0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} },
		{ { 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

		{ { 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f} }, // right side face
		{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
		{ { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
		{ { 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f} },

		{ {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // left side face
		{ {-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f} },
		{ {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
		{ {-0.5f,  0.5f, -0.5f}, {1.0f, 0.0f} },

		{ { 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} }, // back face
		{ {-0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} },
		{ { 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
		{ {-0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },

		{ {-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f} }, // top face
		{ { 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f} },
		{ { 0.5f,  0.5f, -0.5f}, {1.0f, 1.0f} },
		{ {-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f} },

		{ { 0.5f, -0.5f,  0.5f}, {1.0f, 1.0f} }, // bottom face
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f} },
		{ { 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f} },
		{ {-0.5f, -0.5f,  0.5f}, {0.0f, 1.0f} },
	};

	static const uint32_t gIndices[] = {
		 0,  1,  2,   0,  3,  1, // front face
		 4,  5,  6,   4,  7,  5, // left face
		 8,  9, 10,   8, 11,  9, // right face
		12, 13, 14,  12, 15, 13, // back face
		16, 17, 18,  16, 19, 17, // top face
		20, 21, 22,  20, 23, 21, // bottom face
	};

	constexpr uint32_t cNumViews = 4;

	static const dm::float3 gRotationAxes[cNumViews] = {
		dm::float3(1.f, 0.f, 0.f),
		dm::float3(0.f, 1.f, 0.f),
		dm::float3(0.f, 0.f, 1.f),
		dm::float3(1.f, 1.f, 1.f),
	};

	// This example uses a single large constant buffer with multiple views to draw multiple versions of the same model.
	// The alignment and size of partially bound constant buffers must be a multiple of 256 bytes,
	// so define a struct that represents one constant buffer entry or slice for one draw call.
	struct ConstantBufferEntry
	{
		dm::float4x4 viewProjMatrix;
		float padding[16 * 3];
	};

	static_assert(sizeof(ConstantBufferEntry) == nvrhi::c_ConstantBufferOffsetSizeAlignment, "sizeof(ConstantBufferEntry) must be 256 bytes");
}

class DeferredApp;

struct UIOptions
{
	bool mVsync = false;
	int mRTMode = 0;
	std::vector<const char*> mAppModeOptions = { "Color", "Normal"};
};

class UIRenderer : public donut::app::ImGui_Renderer
{
public:
	UIRenderer(donut::app::DeviceManager* deviceManager, std::shared_ptr<DeferredApp> aApp);

protected:
	virtual void buildUI(void) override;

private:
	std::shared_ptr<DeferredApp> mInitApp;
};

class RenderTargets
{
public:
	nvrhi::TextureHandle mDepth;
	nvrhi::TextureHandle mColor;

	dm::int2 mSize;

	std::shared_ptr<donut::engine::FramebufferFactory> mFramebuffer;

	RenderTargets(nvrhi::IDevice* device, const dm::int2 aSize)
		:mSize(aSize)
	{
		nvrhi::TextureDesc textureDesc;
		textureDesc.format = nvrhi::Format::SRGBA8_UNORM;
		textureDesc.isRenderTarget = true;
		textureDesc.initialState = nvrhi::ResourceStates::RenderTarget;
		textureDesc.keepInitialState = true;
		textureDesc.clearValue = nvrhi::Color(0.f);
		textureDesc.useClearValue = true;
		textureDesc.debugName = "Fwd Pass: ColorBuffer";
		textureDesc.width = aSize.x;
		textureDesc.height = aSize.y;
		textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
		mColor = device->createTexture(textureDesc);

		textureDesc.format = nvrhi::Format::D24S8;
		textureDesc.debugName = "Fwd Pass: DepthBuffer";
		textureDesc.initialState = nvrhi::ResourceStates::DepthWrite;
		mDepth = device->createTexture(textureDesc);

		mFramebuffer = std::make_shared<donut::engine::FramebufferFactory>(device);
		mFramebuffer->RenderTargets = { mColor };
		mFramebuffer->DepthTarget = mDepth;
	}

	bool IsUpdateRequired(dm::int2 size)
	{
		if (dm::any(mSize != size))
			return true;

		return false;
	}

	void Clear(nvrhi::ICommandList* commandList)
	{
		commandList->clearDepthStencilTexture(mDepth, nvrhi::AllSubresources, true, 0.f, true, 0);
		commandList->clearTextureFloat(mColor, nvrhi::AllSubresources, nvrhi::Color(0.f));
	}

	const dm::int2& GetSize()
	{
		return mSize;
	}
};

class DeferredApp : public donut::app::ApplicationBase
{
public:
	using ApplicationBase::ApplicationBase;

	bool Init();
	void BackBufferResizing() override;
	void Animate(float fElapsedTimeSeconds) override;
	void Render(nvrhi::IFramebuffer* framebuffer) override;
	bool LoadScene(std::shared_ptr<donut::vfs::IFileSystem> fs, const std::filesystem::path& sceneFileName) override;
	bool KeyboardUpdate(int key, int scancode, int action, int mods) override;
	bool MousePosUpdate(double xpos, double ypos) override;
	bool MouseButtonUpdate(int button, int action, int mods) override;

	UIOptions mUIOptions;

private:

	bool InitAppShaderSetup(std::shared_ptr<donut::engine::ShaderFactory> aShaderFactory);

	struct Model
	{
		std::unique_ptr<donut::render::ForwardShadingPass> mForwardPass;
		std::unique_ptr<donut::render::InstancedOpaqueDrawStrategy> mOpaqueDrawStrategy;
		std::unique_ptr<RenderTargets> mRenderTargets;
		std::shared_ptr<donut::engine::DirectionalLight>  m_SunLight;
		donut::engine::PlanarView mView;
	}mModel;

	nvrhi::CommandListHandle mCommandList;
	donut::app::FirstPersonCamera mCamera;
	std::shared_ptr<donut::engine::ShaderFactory> mShaderFactory;
	std::unique_ptr<donut::engine::Scene> mScene;
	std::unique_ptr<donut::engine::BindingCache> mBindingCache;
};
