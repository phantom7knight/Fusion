#include "Deferred.h"

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#include "../../Core/Engine/CommonRenderPasses.h"
#include "../../Core/Engine/TextureCache.h"
#include "../../Core/App/DeviceManager.h"

using namespace donut::math;
#include "../../../Assets/Shaders/Includes/lighting_cb.h"

UIRenderer::UIRenderer(donut::app::DeviceManager* deviceManager, std::shared_ptr<DeferredApp> aApp)
	: ImGui_Renderer(deviceManager)
	, mInitApp(aApp)
{
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";

	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Common", commonShaderPath);

	std::shared_ptr<donut::engine::ShaderFactory> shaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	if (mInitApp)
		Init(shaderFactory);

	assert(mInitApp);

	ImGui::GetIO().IniFilename = nullptr;
}

void UIRenderer::buildUI(void)
{
	ImGui::Begin("App", 0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("GPU: %s", GetDeviceManager()->GetRendererString());	

	ImGui::SeparatorText("App Options:");

	ImGui::Checkbox("Enable Vsync", &mInitApp->mUIOptions.mVsync);

	auto& arr = mInitApp->mUIOptions.mAppModeOptions;
	ImGui::Combo("RT Targets", &mInitApp->mUIOptions.mRTsMode, arr.data(), arr.size());

	ImGui::End();
}

bool DeferredApp::InitAppShaderSetup(std::shared_ptr<donut::engine::ShaderFactory> aShaderFactory)
{
	return true;
}

bool DeferredApp::Init()
{
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	std::filesystem::path appShaderPath = baseAssetsPath / "Shaders/Applications/Init/Generated";
	std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";
	std::filesystem::path renderPassesShaderPath = baseAssetsPath / "Shaders/RenderPasses/Generated";
	std::filesystem::path assetTexturesPath = baseAssetsPath / "Textures";
	std::filesystem::path gltfAssetPath = baseAssetsPath / "GLTFModels";
	std::filesystem::path modelFileName = gltfAssetPath / "2.0/Duck/glTF/Duck.gltf";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/Sponza/glTF/Sponza.gltf";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/CarbonFibre/glTF/CarbonFibre.gltf";

	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Init", appShaderPath);
	rootFS->mount("/shaders/Common", commonShaderPath);
	rootFS->mount("/assets/Textures", assetTexturesPath);
	rootFS->mount("/assets/GLTFModels", gltfAssetPath);
	rootFS->mount("/shaders/RenderPasses", renderPassesShaderPath);

	mShaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	m_CommonPasses = std::make_shared<donut::engine::CommonRenderPasses>(GetDevice(), mShaderFactory);
	mBindingCache = std::make_unique<donut::engine::BindingCache>(GetDevice());

	auto nativeFS = std::make_shared<donut::vfs::NativeFileSystem>();
	m_TextureCache = std::make_shared<donut::engine::TextureCache>(GetDevice(), nativeFS, nullptr);

	donut::engine::CommonRenderPasses cmnRenderPasses(GetDevice(), mShaderFactory);
	donut::engine::TextureCache textureCache(GetDevice(), rootFS, nullptr);

	if (!InitAppShaderSetup(mShaderFactory))
		return false;

	mCommandList = GetDevice()->createCommandList();

#pragma region Model
		SetAsynchronousLoadingEnabled(false);
		BeginLoadingScene(nativeFS, modelFileName);

		mModel.mOpaqueDrawStrategy = std::make_unique<donut::render::InstancedOpaqueDrawStrategy>();

		mModel.m_SunLight = std::make_shared<donut::engine::DirectionalLight>();
		mScene->GetSceneGraph()->AttachLeafNode(mScene->GetSceneGraph()->GetRootNode(), mModel.m_SunLight);
		mModel.m_SunLight->SetDirection(double3(0.1, -1.0, 0.15));
		mModel.m_SunLight->SetName("Sun");
		mModel.m_SunLight->angularSize = 0.53f;
		mModel.m_SunLight->irradiance = 2.f;

		mScene->FinishedLoading(GetFrameIndex());

		// camera setup
		mCamera.LookAt(donut::math::float3(10.f, 10.8f, 10.f), donut::math::float3(1.f, 1.8f, 0.f));
		mCamera.SetMoveSpeed(3.f);
#pragma endregion

	return true;
}

bool DeferredApp::LoadScene(std::shared_ptr<donut::vfs::IFileSystem> fs, const std::filesystem::path& sceneFileName)
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

void DeferredApp::BackBufferResizing()
{
	mModel.mForwardPass = nullptr;
	mModel.mRenderTargets = nullptr;
	mBindingCache->Clear();
}

void DeferredApp::Animate(float fElapsedTimeSeconds)
{
	mCamera.Animate(fElapsedTimeSeconds);
	GetDeviceManager()->SetInformativeWindowTitle("Hello World!!");
}

bool DeferredApp::KeyboardUpdate(int key, int scancode, int action, int mods)
{
	mCamera.KeyboardUpdate(key, scancode, action, mods);
	return true;
}

bool DeferredApp::MousePosUpdate(double xpos, double ypos)
{
	mCamera.MousePosUpdate(xpos, ypos);
	return true;
}

bool DeferredApp::MouseButtonUpdate(int button, int action, int mods)
{
	mCamera.MouseButtonUpdate(button, action, mods);
	return true;
}

void DeferredApp::Render(nvrhi::IFramebuffer* framebuffer)
{
	GetDeviceManager()->SetVsyncEnabled(mUIOptions.mVsync);

	{
		const auto& fbinfo = framebuffer->getFramebufferInfo();

		if (!mModel.mRenderTargets)
		{
			mModel.mRenderTargets = std::make_unique<RenderTargets>(GetDevice(), int2(fbinfo.width, fbinfo.height));
		}

		if (!mModel.mForwardPass)
		{
			mModel.mForwardPass = std::make_unique<donut::render::ForwardShadingPass>(GetDevice(), m_CommonPasses);

			donut::render::ForwardShadingPass::CreateParameters forwardParams;
			mModel.mForwardPass->Init(*mShaderFactory, forwardParams);
		}

		nvrhi::Viewport windowViewport(float(fbinfo.width), float(fbinfo.height));
		mModel.mView.SetViewport(windowViewport);
		mModel.mView.SetMatrices(mCamera.GetWorldToViewMatrix(), perspProjD3DStyleReverse(PI_f * 0.25f, windowViewport.width() / windowViewport.height(), 0.1f));
		mModel.mView.UpdateCache();

		mCommandList->open();

#ifdef _DEBUG
		mCommandList->beginMarker("Render Forward Pass");
#endif

		mModel.mRenderTargets->Clear(mCommandList);

		LightingConstants constants = {};
		constants.ambientColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
		mModel.mView.FillPlanarViewConstants(constants.view);

		donut::render::ForwardShadingPass::Context forwardContext;
		mModel.mForwardPass->PrepareLights(forwardContext,
			mCommandList,
			mScene->GetSceneGraph()->GetLights(),
			constants.ambientColor,
			constants.ambientColor,
			{});

		//donut::render::RenderCompositeView(mCommandList, 
		//	&mModel.mView, 
		//	&mModel.mView, 
		//	//*mModel.mRenderTargets->mFramebuffer, // todo_rt; testing
		//	mScene->GetSceneGraph()->GetRootNode(), 
		//	*mModel.mOpaqueDrawStrategy, 
		//	*mModel.mForwardPass, 
		//	forwardContext);
#ifdef _DEBUG
		mCommandList->endMarker();
#endif

#ifdef _DEBUG
		mCommandList->beginMarker("Blit Fwd Pass Tex to Back Buffer");
#endif
		m_CommonPasses->BlitTexture(mCommandList, framebuffer, mModel.mRenderTargets->mColor, mBindingCache.get());
#ifdef _DEBUG
		mCommandList->endMarker();
#endif

		mCommandList->close();
		GetDevice()->executeCommandList(mCommandList);
	}
}