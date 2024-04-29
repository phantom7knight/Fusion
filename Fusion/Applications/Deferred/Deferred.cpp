#include "Deferred.h"

#include <format>

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#include "../../Core/Engine/CommonRenderPasses.h"
#include "../../Core/Engine/TextureCache.h"
#include "../../Core/App/DeviceManager.h"

#include "../../Core/Render/ForwardShadingPass.h"

using namespace donut::math;
#include "../../../Assets/Shaders/Includes/lighting_cb.h"

UIRenderer::UIRenderer(donut::app::DeviceManager* deviceManager, std::shared_ptr<DeferredApp> aApp)
	: ImGui_Renderer(deviceManager)
	, mDeferredApp(aApp)
{
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";

	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Common", commonShaderPath);

	std::shared_ptr<donut::engine::ShaderFactory> shaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	if (mDeferredApp)
		Init(shaderFactory);

	assert(mDeferredApp);

	ImGui::GetIO().IniFilename = nullptr;
}

void UIRenderer::buildUI(void)
{
	ImGui::Begin("App", 0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("GPU: %s", GetDeviceManager()->GetRendererString());

	double frameTime = GetDeviceManager()->GetAverageFrameTimeSeconds();
	if (frameTime > 0)
	{
		std::ostringstream oss;
		oss << std::fixed << std::setprecision(2) << (1.0 / frameTime);
		std::string formattedNumber = oss.str();

		const char* result = formattedNumber.c_str();

		ImGui::Text("FPS: %s", result);
	}

	ImGui::SeparatorText("App Options:");

	dm::float3 camPos = mDeferredApp->GetCameraPosition();
	ImGui::Text("Camera Position: X: %.2f Y: %.2f Z: %.2f", camPos.x, camPos.y, camPos.z);

	ImGui::Checkbox("Enable Vsync", &mDeferredApp->mUIOptions.mVsync);

	auto& arr = mDeferredApp->mUIOptions.mAppModeOptions;
	ImGui::Combo("RT Targets", &mDeferredApp->mUIOptions.mRTsViewMode, arr.data(), arr.size());

	ImGui::SeparatorText("Light Options:");

	ImGui::DragFloat3(": Sun Light Pos", mDeferredApp->mUIOptions.mSunPos, -10.f, 10.f);

	ImGui::End();
}

bool DeferredApp::Init()
{
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	std::filesystem::path appShaderPath = baseAssetsPath / "Shaders/Applications/Deferred/Generated";
	std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";
	std::filesystem::path renderPassesShaderPath = baseAssetsPath / "Shaders/RenderPasses/Generated";
	std::filesystem::path assetTexturesPath = baseAssetsPath / "Textures";
	std::filesystem::path gltfAssetPath = baseAssetsPath / "GLTFModels";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/Duck/glTF/Duck.gltf";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/Sponza/glTF/Sponza.gltf";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/CarbonFibre/glTF/CarbonFibre.gltf";
	//std::filesystem::path modelFileName = gltfAssetPath / "2.0/Suzanne/glTF/Suzanne.gltf";
	std::filesystem::path modelFileName = gltfAssetPath / "2.0/TwoSidedPlane/glTF/TwoSidedPlane.gltf";

	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Deferred", appShaderPath);
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

	mCommandList = GetDevice()->createCommandList();
	
	mOpaqueDrawStrategy = std::make_unique<donut::render::InstancedOpaqueDrawStrategy>();
	mPassThroughDrawStrategy = std::make_unique<donut::render::PassthroughDrawStrategy>();

	{ // scene setup
		SetAsynchronousLoadingEnabled(false);
		BeginLoadingScene(nativeFS, modelFileName);

		mSunLight = std::make_shared<donut::engine::DirectionalLight>();
		mScene->GetSceneGraph()->AttachLeafNode(mScene->GetSceneGraph()->GetRootNode(), mSunLight);
		mSunLight->SetDirection(double3(0.1, -1.0, 0.15));
		mSunLight->SetName("Sun");
		mSunLight->angularSize = 0.53f;
		mSunLight->irradiance = 2.f;

		// todo_rt; testing

		{
			const int lightCount = 15;

			for (int x = 0; x < 1; ++x)
			{
				for (int y = 0; y < 1; ++y)
				{
					auto light = std::make_shared<donut::engine::SpotLight>();
					mScene->GetSceneGraph()->AttachLeafNode(mScene->GetSceneGraph()->GetRootNode(), light);
					light->SetName(std::format("Light {}", mLights.size() + 1));
					light->SetPosition(dm::double3(3.50, 2.0f, 3.5f));
					light->color = dm::colors::green;
					light->intensity = 3.f;
					light->radius = 1.f;

					// 10 2 3.5			// 0				//-9.7 2 -3.5
					// 8, 2. 3.5						//- 7.0 2.0 -3.5
					// -10 2 3.5						// 10 2 -3.6
					mLights.push_back(light); // todo_rt: for some debugging purposes??? or maybe use the scene graph's getlights()??
				}
			}
		}



		mScene->FinishedLoading(GetFrameIndex());
	}

	// camera setup
	mCamera.LookAt(donut::math::float3(10.f, 10.8f, 10.f), donut::math::float3(1.f, 1.8f, 0.f));
	mCamera.SetMoveSpeed(4.f);

	if (!mDeferredLightingPass)
	{
		mDeferredLightingPass = std::make_unique<donut::render::DeferredLightingPass>(GetDevice(), m_CommonPasses);
		mDeferredLightingPass->Init(mShaderFactory);
	}

	return true;
}

bool DeferredApp::LoadScene(std::shared_ptr<donut::vfs::IFileSystem> aFileSystem, const std::filesystem::path& sceneFileName)
{
	assert(m_TextureCache);

	if(!mScene)
		mScene = std::make_unique<donut::engine::Scene>(GetDevice(), *mShaderFactory, aFileSystem, m_TextureCache, nullptr, nullptr);
	
	if (mScene->Load(sceneFileName))
	{
		return true;
	}

	return false;
}

void DeferredApp::BackBufferResizing()
{
	mGBufferRenderTargets = nullptr;
	mBindingCache->Clear();
}

void DeferredApp::Animate(float fElapsedTimeSeconds)
{
	mCamera.Animate(fElapsedTimeSeconds);
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

void DeferredApp::Render(nvrhi::IFramebuffer* aFramebuffer)
{
	GetDeviceManager()->SetVsyncEnabled(mUIOptions.mVsync);

	//mSunLight->SetPosition(dm::double3(mUIOptions.mSunPos[0], mUIOptions.mSunPos[1], mUIOptions.mSunPos[2]));

	const auto& fbinfo = aFramebuffer->getFramebufferInfo();

	if (!mGBufferRenderTargets) // Gbuffer Render Targets Setup
	{
		mBindingCache->Clear();
		mDeferredLightingPass->ResetBindingCache();
		mGBufferFillPass.reset();

		mGBufferRenderTargets = nullptr;
		mGBufferRenderTargets = std::make_shared<RenderTargets>(GetDevice(),
			dm::uint2(fbinfo.width, fbinfo.height),
			1,
			false,
			true); // TODO_RT: Investigate this value for the projection, should it be false or need to be true?
	}

	if (!mGBufferFillPass)
	{
		mGBufferFillPass = std::make_unique<donut::render::GBufferFillPass>(GetDevice(), m_CommonPasses);

		donut::render::GBufferFillPass::CreateParameters params;
		mGBufferFillPass->Init(*mShaderFactory, params);
	}

	nvrhi::Viewport windowViewport(float(fbinfo.width), float(fbinfo.height));
	mView.SetViewport(windowViewport);
	mView.SetMatrices(mCamera.GetWorldToViewMatrix(),
		perspProjD3DStyleReverse(PI_f * 0.25f, windowViewport.width() / windowViewport.height(),
			0.1f));
	mView.UpdateCache();

	mCommandList->open();

#ifdef _DEBUG
	mCommandList->beginMarker("GBuffer Fill Pass");
#endif

	mGBufferRenderTargets->Clear(mCommandList);

	donut::render::GBufferFillPass::Context ctx = {};
	donut::render::RenderCompositeView(mCommandList, 
		&mView, 
		&mView, 
		*mGBufferRenderTargets->GBufferFramebuffer,
		mScene->GetSceneGraph()->GetRootNode(), 
		*mOpaqueDrawStrategy, 
		*mGBufferFillPass,
		ctx);

#ifdef _DEBUG
	mCommandList->endMarker();
#endif

#ifdef _DEBUG
	mCommandList->beginMarker("Deferred Lighting Pass");
#endif

	donut::render::DeferredLightingPass::Inputs deferredLightingInputs;
	deferredLightingInputs.SetGBuffer(*mGBufferRenderTargets);
	deferredLightingInputs.output = mGBufferRenderTargets->mOutputColor;
	deferredLightingInputs.ambientColorTop = 0.2f;
	deferredLightingInputs.ambientColorBottom = deferredLightingInputs.ambientColorTop * float3(0.3f, 0.4f, 0.3f);
	deferredLightingInputs.lights = &mScene->GetSceneGraph()->GetLights();

	mDeferredLightingPass->Render(mCommandList,
		mView,
		deferredLightingInputs);

#ifdef _DEBUG
	mCommandList->endMarker();
#endif

#ifdef _DEBUG
	mCommandList->beginMarker("Blit Fwd Pass Tex to Back Buffer");
#endif

	switch (mUIOptions.mRTsViewMode)
	{
	case 0:
		m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.output, mBindingCache.get());
		break;
	case 1:
		m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferDiffuse, mBindingCache.get());
		break;
	case 2:
		m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferSpecular, mBindingCache.get());
		break;
	case 3:
		m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferNormals, mBindingCache.get());
		break;
	case 4:
		m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferEmissive, mBindingCache.get());
		break;
	case 5:
		m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.depth, mBindingCache.get());
		break;
	default:
		break;
	}

#ifdef _DEBUG
	mCommandList->endMarker();
#endif

	mCommandList->close();
	GetDevice()->executeCommandList(mCommandList);
}