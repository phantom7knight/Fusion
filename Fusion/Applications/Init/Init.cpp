#include "Init.h"

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#include "../../Core/Engine/CommonRenderPasses.h"
#include "../../Core/Engine/TextureCache.h"
#include "../../Core/App/DeviceManager.h"

using namespace donut::math;
#include "../../../Assets/Shaders/Includes/lighting_cb.h"

namespace Init_private
{
}

bool InitApp::InitAppShaderSetup(std::shared_ptr<donut::engine::ShaderFactory> aShaderFactory)
{
	mTriangle.mVertexShader = aShaderFactory->CreateShader("Init/Triangle.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
	mTriangle.mPixelShader = aShaderFactory->CreateShader("Init/Triangle.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);
	
	mCube.mVertexShader = aShaderFactory->CreateShader("Init/Cube.hlsl", "main_vs", nullptr, nvrhi::ShaderType::Vertex);
	mCube.mPixelShader = aShaderFactory->CreateShader("Init/Cube.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);

	if (!mTriangle.mVertexShader || !mTriangle.mPixelShader ||
		!mCube.mVertexShader || !mCube.mPixelShader)
	{
		return false;
	}

	return true;
}

bool InitApp::Init()
{
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	std::filesystem::path appShaderPath = baseAssetsPath / "Shaders/Applications/Init/Generated";
	std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";
	std::filesystem::path renderPassesShaderPath = baseAssetsPath / "Shaders/RenderPasses/Generated";
	std::filesystem::path assetTexturesPath = baseAssetsPath / "Textures";
	std::filesystem::path gltfAssetPath = baseAssetsPath / "GLTFModels";
	std::filesystem::path modelFileName = "/assets/GLTFModels/2.0/Duck/glTF/Duck.gltf";

	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Init", appShaderPath);
	rootFS->mount("/shaders/Common", commonShaderPath);
	rootFS->mount("/assets/Textures", assetTexturesPath);
	rootFS->mount("/assets/GLTFModels", gltfAssetPath);
	rootFS->mount("/shaders/RenderPasses", renderPassesShaderPath);

	mShaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	m_CommonPasses = std::make_shared<donut::engine::CommonRenderPasses>(GetDevice(), mShaderFactory);

	auto nativeFS = std::make_shared<donut::vfs::NativeFileSystem>();
	m_TextureCache = std::make_shared<donut::engine::TextureCache>(GetDevice(), nativeFS, nullptr);

	if (!InitAppShaderSetup(mShaderFactory))
		return false;

	mCommandList = GetDevice()->createCommandList();

	mCube.mConstantBuffer = GetDevice()->createBuffer(nvrhi::utils::CreateStaticConstantBufferDesc
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

	donut::engine::CommonRenderPasses cmnRenderPasses(GetDevice(), mShaderFactory);
	donut::engine::TextureCache textureCache(GetDevice(), rootFS, nullptr);

	SetAsynchronousLoadingEnabled(false);
	BeginLoadingScene(rootFS, modelFileName);

	mOpaqueDrawStrategy = std::make_unique<donut::render::InstancedOpaqueDrawStrategy>();
	mScene->FinishedLoading(GetFrameIndex());

	// camera setup
	mCamera.LookAt(donut::math::float3(0.f, 1.8f, 0.f), donut::math::float3(1.f, 1.8f, 0.f));
	mCamera.SetMoveSpeed(3.f);

	mCommandList = GetDevice()->createCommandList();
	mCommandList->open();

	// Cube Buffers
	nvrhi::BufferDesc vertexBufferDesc;
	vertexBufferDesc.byteSize = sizeof(locInitHelpers::gVertices);
	vertexBufferDesc.isVertexBuffer = true;
	vertexBufferDesc.debugName = "Cube VertexBuffer";
	vertexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
	mCube.mVertexBuffer = GetDevice()->createBuffer(vertexBufferDesc);

	mCommandList->beginTrackingBufferState(mCube.mVertexBuffer, nvrhi::ResourceStates::CopyDest);
	mCommandList->writeBuffer(mCube.mVertexBuffer, locInitHelpers::gVertices, sizeof(locInitHelpers::gVertices));
	mCommandList->setPermanentBufferState(mCube.mVertexBuffer, nvrhi::ResourceStates::VertexBuffer);

	nvrhi::BufferDesc indexBufferDesc;
	indexBufferDesc.byteSize = sizeof(locInitHelpers::gIndices);
	indexBufferDesc.isIndexBuffer = true;
	indexBufferDesc.debugName = "Cube IndexBuffer";
	indexBufferDesc.initialState = nvrhi::ResourceStates::CopyDest;
	mCube.mIndexBuffer = GetDevice()->createBuffer(indexBufferDesc);

	mCommandList->beginTrackingBufferState(mCube.mIndexBuffer, nvrhi::ResourceStates::CopyDest);
	mCommandList->writeBuffer(mCube.mIndexBuffer, locInitHelpers::gIndices, sizeof(locInitHelpers::gIndices));
	mCommandList->setPermanentBufferState(mCube.mIndexBuffer, nvrhi::ResourceStates::IndexBuffer);

	// Textures
	std::shared_ptr<donut::engine::LoadedTexture> texture = textureCache.LoadTextureFromFile("/assets/Textures/window.png", true, nullptr, mCommandList);
	mCube.mTexture = texture->texture;

	mCommandList->close();
	GetDevice()->executeCommandList(mCommandList);

	if (!texture->texture)
	{
		donut::log::error("Couldn't load the texture");
		return false;
	}

	// Create a single binding layout and multiple binding sets, one set per view.
	// The different binding sets use different slices of the same constant buffer.
	for (uint32_t viewIndex = 0; viewIndex < locInitHelpers::cNumViews; ++viewIndex)
	{
		nvrhi::BindingSetDesc bindingSetDesc;
		bindingSetDesc.bindings = {
			// Note: using viewIndex to construct a buffer range.
			nvrhi::BindingSetItem::ConstantBuffer(0,
			mCube.mConstantBuffer,
			nvrhi::BufferRange(sizeof(locInitHelpers::ConstantBufferEntry) * viewIndex,
			sizeof(locInitHelpers::ConstantBufferEntry))),
			// Texutre and sampler are the same for all model views.
			nvrhi::BindingSetItem::Texture_SRV(0, mCube.mTexture),
			nvrhi::BindingSetItem::Sampler(0, cmnRenderPasses.m_AnisotropicWrapSampler)
		};

		// Create the binding layout (if it's empty -- so, on the first iteration) and the binding set.
		if (!nvrhi::utils::CreateBindingSetAndLayout(GetDevice(),
			nvrhi::ShaderType::All,
			0,
			bindingSetDesc,
			mCube.mBindingLayout,
			mCube.mBindingSets[viewIndex]))
		{
			donut::log::error("Couldn't create the binding set or layout");
			return false;
		}
	}

	return true;
}

bool InitApp::LoadScene(std::shared_ptr<donut::vfs::IFileSystem> fs, const std::filesystem::path& sceneFileName)
{
	assert(m_TextureCache);
	donut::engine::Scene* scene = new donut::engine::Scene(GetDevice(), *mShaderFactory, fs, m_TextureCache, nullptr, nullptr);

	if (scene->Load(sceneFileName))
	{
		mScene = std::unique_ptr<donut::engine::Scene>(scene);
		return true;
	}

	return false;
}

void InitApp::BackBufferResizing()
{
	mTriangle.mGraphicsPipeline = nullptr;
	mCube.mGraphicsPipeline = nullptr;
	mForwardPass = nullptr;
}

void InitApp::Animate(float fElapsedTimeSeconds)
{
	mCube.mRotation += fElapsedTimeSeconds * 1.1f;
	mCamera.Animate(fElapsedTimeSeconds);
	GetDeviceManager()->SetInformativeWindowTitle("Hello World!!");
}

bool InitApp::KeyboardUpdate(int key, int scancode, int action, int mods)
{
	mCamera.KeyboardUpdate(key, scancode, action, mods);
	return true;
}

bool InitApp::MousePosUpdate(double xpos, double ypos)
{
	mCamera.MousePosUpdate(xpos, ypos);
	return true;
}

bool InitApp::MouseButtonUpdate(int button, int action, int mods)
{
	mCamera.MouseButtonUpdate(button, action, mods);
	return true;
}

void InitApp::Render(nvrhi::IFramebuffer* framebuffer)
{
	if (mAppMode == 0) // Init
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
	else if (mAppMode == 1) // Cube (TODO_RT: DOESN'T WORK ON VK)
	{
		const nvrhi::FramebufferInfoEx& fbinfo = framebuffer->getFramebufferInfo();

		if (!mCube.mGraphicsPipeline)
		{
			nvrhi::GraphicsPipelineDesc psoDesc;
			psoDesc.VS = mCube.mVertexShader;
			psoDesc.PS = mCube.mPixelShader;
			psoDesc.inputLayout = mCube.mInputLayout;
			psoDesc.bindingLayouts = { mCube.mBindingLayout };
			psoDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoDesc.renderState.depthStencilState.depthTestEnable = false;

			mCube.mGraphicsPipeline = GetDevice()->createGraphicsPipeline(psoDesc, framebuffer);
		}

		mCommandList->open();

		nvrhi::utils::ClearColorAttachment(mCommandList, framebuffer, 0, nvrhi::Color(0.f));

		// Fill out the constant buffer slices for multiple views of the model.
		locInitHelpers::ConstantBufferEntry modelConstants[locInitHelpers::cNumViews];
		for (uint32_t viewIndex = 0; viewIndex < locInitHelpers::cNumViews; ++viewIndex)
		{
			donut::math::affine3 viewMatrix = donut::math::rotation(normalize(locInitHelpers::gRotationAxes[viewIndex]), mCube.mRotation)
				* donut::math::yawPitchRoll(0.f, donut::math::radians(-30.f), 0.f)
				* donut::math::translation(donut::math::float3(0, 0, 2));

			donut::math::float4x4 projMatrix;

			projMatrix= donut::math::perspProjD3DStyle(donut::math::radians(60.f),
				//projMatrix = donut::math::perspProjVKStyle(donut::math::radians(60.f),
				float(fbinfo.width) / float(fbinfo.height),
				0.1f,
				10.f);
			donut::math::float4x4 viewProjMatrix = donut::math::affineToHomogeneous(viewMatrix) * projMatrix;			
			modelConstants[viewIndex].viewProjMatrix = viewProjMatrix;
		}

		// Upload all constant buffer slices at once.
		mCommandList->writeBuffer(mCube.mConstantBuffer, modelConstants, sizeof(modelConstants));

		for (uint32_t viewIndex = 0; viewIndex < locInitHelpers::cNumViews; ++viewIndex)
		{
			nvrhi::GraphicsState state;
			// Pick the right binding set for this view.
			state.bindings = { mCube.mBindingSets[viewIndex] };
			state.indexBuffer = { mCube.mIndexBuffer, nvrhi::Format::R32_UINT, 0 };
			// Bind the vertex buffers in reverse order to test the NVRHI implementation of binding slots
			state.vertexBuffers = {
				{ mCube.mVertexBuffer, 1, offsetof(locInitHelpers::Vertex, uv) },
				{ mCube.mVertexBuffer, 0, offsetof(locInitHelpers::Vertex, position) }
			};
			state.pipeline = mCube.mGraphicsPipeline;
			state.framebuffer = framebuffer;

			// Construct the viewport so that all viewports form a grid.
			const float width = float(fbinfo.width) * 0.5f;
			const float height = float(fbinfo.height) * 0.5f;
			const float left = width * float(viewIndex % 2);
			const float top = height * float(viewIndex / 2);

			const nvrhi::Viewport viewport = nvrhi::Viewport(left, left + width, top, top + height, 0.f, 1.f);
			state.viewport.addViewportAndScissorRect(viewport);

			// Update the pipeline, bindings, and other state.
			mCommandList->setGraphicsState(state);

			// Draw the model.
			nvrhi::DrawArguments args;
			args.vertexCount = dim(locInitHelpers::gIndices);
			mCommandList->drawIndexed(args);
		}

		mCommandList->close();
		GetDevice()->executeCommandList(mCommandList);
	}
	else if (mAppMode == 2) // Model
	{
		const auto& fbinfo = framebuffer->getFramebufferInfo();

		if (!mForwardPass)
		{
			mForwardPass = std::make_unique<donut::render::ForwardShadingPass>(GetDevice(), m_CommonPasses);

			donut::render::ForwardShadingPass::CreateParameters forwardParams;
			mForwardPass->Init(*mShaderFactory, forwardParams);
		}

		{
			// create the attachements for the framebuffer
			nvrhi::TextureDesc textureDesc;
			textureDesc.format = nvrhi::Format::SRGBA8_UNORM;
			textureDesc.isRenderTarget = true;
			textureDesc.initialState = nvrhi::ResourceStates::RenderTarget;
			textureDesc.keepInitialState = true;
			textureDesc.clearValue = nvrhi::Color(0.f);
			textureDesc.useClearValue = true;
			textureDesc.debugName = "ColorBuffer";
			textureDesc.width = fbinfo.width;
			textureDesc.height = fbinfo.height;
			textureDesc.dimension = nvrhi::TextureDimension::Texture2D;
			mModel.mColorBuffer = GetDevice()->createTexture(textureDesc);

			textureDesc.format = nvrhi::Format::D24S8;
			textureDesc.debugName = "DepthBuffer";
			textureDesc.initialState = nvrhi::ResourceStates::DepthWrite;
			mModel.mDepthBuffer = GetDevice()->createTexture(textureDesc);

			nvrhi::FramebufferDesc framebufferDesc;
			framebufferDesc.addColorAttachment(mModel.mColorBuffer, nvrhi::AllSubresources);
			framebufferDesc.setDepthAttachment(mModel.mDepthBuffer);
			mModel.mFramebuffer = GetDevice()->createFramebuffer(framebufferDesc);
		}

		nvrhi::Viewport windowViewport(float(fbinfo.width), float(fbinfo.height));
		mView.SetViewport(windowViewport);
		mView.SetMatrices(mCamera.GetWorldToViewMatrix(), perspProjD3DStyleReverse(PI_f * 0.25f, windowViewport.width() / windowViewport.height(), 0.1f));
		mView.UpdateCache();

		mCommandList->open();

		LightingConstants constants = {};
		constants.ambientColor = float4(0.2f);

		donut::render::ForwardShadingPass::Context forwardContext;
		mForwardPass->PrepareLights(forwardContext, mCommandList, mScene->GetSceneGraph()->GetLights(), constants.ambientColor, constants.ambientColor, {});

		mOpaqueDrawStrategy->PrepareForView(mScene->GetSceneGraph()->GetRootNode(), mView);
		donut::render::RenderView(
			mCommandList,
			&mView,
			&mView,
			mModel.mFramebuffer,
			*mOpaqueDrawStrategy,
			*mForwardPass,
			forwardContext,
			false);

		mCommandList->close();
		GetDevice()->executeCommandList(mCommandList);
	}
}