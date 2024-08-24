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

namespace Deferred_Private
{
	using namespace dm::colors;
	std::vector<float3> colorsArr = { white, red, green, blue, grey };

	// Model Names
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	const std::filesystem::path appShaderPath = baseAssetsPath / "Shaders/Applications/Deferred/Generated";
	const std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";
	const std::filesystem::path renderPassesShaderPath = baseAssetsPath / "Shaders/RenderPasses/Generated";
	const std::filesystem::path assetTexturesPath = baseAssetsPath / "Textures";
	const std::filesystem::path gltfAssetPath = baseAssetsPath / "GLTFModels";
	const std::filesystem::path duckModel = gltfAssetPath / "2.0/Duck/glTF/Duck.gltf";
	const std::filesystem::path sponzaModel = gltfAssetPath / "2.0/Sponza/glTF/Sponza.gltf";
	const std::filesystem::path damagedHelmetModel = gltfAssetPath / "2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
	const std::filesystem::path carbonFibreModel = gltfAssetPath / "2.0/CarbonFibre/glTF/CarbonFibre.gltf";
	const std::filesystem::path suzanneModel = gltfAssetPath / "2.0/Suzanne/glTF/Suzanne.gltf";
	const std::filesystem::path chessModel = gltfAssetPath / "2.0/ABeautifulGame/glTF/ABeautifulGame.gltf";
	const std::filesystem::path planeModel = gltfAssetPath / "2.0/TwoSidedPlane/glTF/TwoSidedPlane.gltf";

	const float3 locGetRandomColor()
	{
		// Seed the random number generator using std::time
		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		// Generate a random index based on the vector size
		int randomIndex = std::rand() % colorsArr.size();

		return colorsArr[randomIndex];
	}
}

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

void UIRenderer::BuildUI(void)
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

	ImGui::End();
}

bool DeferredApp::Init()
{
	
	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Common", Deferred_Private::commonShaderPath);
	rootFS->mount("/assets/Textures", Deferred_Private::assetTexturesPath);
	rootFS->mount("/assets/GLTFModels", Deferred_Private::gltfAssetPath);
	rootFS->mount("/shaders/RenderPasses", Deferred_Private::renderPassesShaderPath);

	mShaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	m_CommonPasses = std::make_shared<donut::engine::CommonRenderPasses>(GetDevice(), mShaderFactory);
	mBindingCache = std::make_unique<donut::engine::BindingCache>(GetDevice());

	auto nativeFS = std::make_shared<donut::vfs::NativeFileSystem>();
	m_TextureCache = std::make_shared<donut::engine::TextureCache>(GetDevice(), nativeFS, nullptr);

	donut::engine::CommonRenderPasses cmnRenderPasses(GetDevice(), mShaderFactory);
	donut::engine::TextureCache textureCache(GetDevice(), rootFS, nullptr);

	mCommandList = GetDevice()->createCommandList();
	
	mOpaqueDrawStrategy = std::make_unique<donut::render::InstancedOpaqueDrawStrategy>();

	{ // scene setup
		SetAsynchronousLoadingEnabled(false);
		BeginLoadingScene(nativeFS, Deferred_Private::damagedHelmetModel);

		// Sun Light
		mSunLight = std::make_shared<donut::engine::DirectionalLight>();
		mScene->GetSceneGraph()->AttachLeafNode(mScene->GetSceneGraph()->GetRootNode(), mSunLight);
		mSunLight->SetDirection(double3(0.1, -1.0, 0.15));
		mSunLight->SetName("Sun");
		mSunLight->angularSize = 0.53f;
		mSunLight->irradiance = 2.f;

		// Models Setup
		if (mScene)
		{
			int count = 0;
			for (int i = 0; i < 1; ++i)
			{
				for (int j = 0; j < 1; ++j)
				{
					auto modelNode = mScene->LoadAtLeaf(Deferred_Private::suzanneModel);
					modelNode->SetName(std::format("Model {}", count + 1));
					modelNode->SetTranslation(double3(0.0 + i * 4, 2.0, -5.5 + j * 3));
					modelNode->SetScaling(double3(0.2f));
					++count;
				}
			}
		}

		// Light Setup
		for (int x = 0; x < 0; ++x)
		{
			for (int y = 0; y < 1; ++y)
			{
				auto light = std::make_shared<donut::engine::SpotLight>();
				mScene->GetSceneGraph()->AttachLeafNode(mScene->GetSceneGraph()->GetRootNode(), light);
				light->SetName(std::format("Light {}", mLights.size() + 1));
				light->SetPosition(dm::double3(13.50 * 2 * x, 2.0f, 13.5f * y * 2));
				auto pos = Deferred_Private::locGetRandomColor();
				light->color = pos;
				light->intensity = 3.f;
				light->radius = 1.f;
				mLights.push_back(light); // todo_rt: for some debugging purposes??? or maybe use the scene graph's getlights()??
			}
		}
		mScene->FinishedLoading(GetFrameIndex());
	}

	// camera setup
	mCamera.LookAt(donut::math::float3(10.f, 10.8f, 10.f), donut::math::float3(1.f, 1.8f, 0.f));
	mCamera.SetMoveSpeed(10.f);

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