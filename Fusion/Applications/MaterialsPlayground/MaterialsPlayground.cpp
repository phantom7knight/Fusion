#include "MaterialsPlayground.h"

#include <format>

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#include "../../Core/Engine/CommonRenderPasses.h"
#include "../../Core/Engine/TextureCache.h"
#include "../../Core/App/DeviceManager.h"

#include "../../Core/Render/ForwardShadingPass.h"

using namespace donut::math;
#include "../../../Assets/Shaders/Includes/lighting_cb.h"

namespace PBRTesting_Private
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
	const std::filesystem::path damangedHelmetModel = gltfAssetPath / "2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
	const std::filesystem::path carbonFibreModel = gltfAssetPath / "2.0/CarbonFibre/glTF/CarbonFibre.gltf";
	const std::filesystem::path suzanneModel = gltfAssetPath / "2.0/Suzanne/glTF/Suzanne.gltf";
	const std::filesystem::path chessModel = gltfAssetPath / "2.0/ABeautifulGame/glTF/ABeautifulGame.gltf";
	const std::filesystem::path planeModel = gltfAssetPath / "2.0/TwoSidedPlane/glTF/TwoSidedPlane.gltf";
	const std::filesystem::path dragonAttenuationModel = gltfAssetPath / "2.0/DragonAttenuation/glTF/DragonAttenuation.gltf";
	const std::filesystem::path transmissionTestModel = gltfAssetPath / "2.0/TransmissionTest/glTF/TransmissionTest.gltf";
	//const std::filesystem::path newSponzaModel = gltfAssetPath / "New Sponza/NewSponza_Main_glTF_002.gltf";

	constexpr dm::float3 ambientColorTop = 0.2f;
	constexpr dm::float3 ambientColorBottom = ambientColorTop * float3(0.3f, 0.4f, 0.3f);

	const float3 locGetRandomColor()
	{
		// Seed the random number generator using std::time
		std::srand(static_cast<unsigned int>(std::time(nullptr)));

		// Generate a random index based on the vector size
		int randomIndex = std::rand() % colorsArr.size();

		return colorsArr[randomIndex];
	}
}

UIRenderer::UIRenderer(donut::app::DeviceManager* deviceManager, std::shared_ptr<MaterialsPlayground> aApp)
	: ImGui_Renderer(deviceManager)
	, mMaterialsPlaygroundApp(aApp)
{
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";

	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Common", commonShaderPath);

	std::shared_ptr<donut::engine::ShaderFactory> shaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	if (mMaterialsPlaygroundApp)
		Init(shaderFactory);

	assert(mMaterialsPlaygroundApp);

	ImGui::GetIO().IniFilename = nullptr;
}

void UIRenderer::BuildUI(void)
{
	ImGui::Begin("App", 0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("GPU: %s", GetDeviceManager()->GetRendererString());

	ImGui::Text("Graphics API: %s", nvrhi::utils::GraphicsAPIToString(GetDevice()->getGraphicsAPI()));

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

	dm::float3 camPos = mMaterialsPlaygroundApp->GetCameraPosition();
	ImGui::Text("Camera Position: X: %.2f Y: %.2f Z: %.2f", camPos.x, camPos.y, camPos.z);

	ImGui::Checkbox("Enable Vsync", &mMaterialsPlaygroundApp->mUIOptions.mVsync);

	ImGui::Checkbox("Enable Deferred Shading", &mMaterialsPlaygroundApp->mUIOptions.mEnableDeferredShading);

	if (mMaterialsPlaygroundApp->mUIOptions.mEnableDeferredShading)
	{
		auto& arr = mMaterialsPlaygroundApp->mUIOptions.mAppModeOptions;
		ImGui::Combo("RT Targets", &mMaterialsPlaygroundApp->mUIOptions.mRTsViewMode, arr.data(), arr.size());
	}

	ImGui::SeparatorText("Material Options:");

	ImGui::Checkbox("Enable Transparency", &mMaterialsPlaygroundApp->mUIOptions.mEnableTranslucency);

	// todo_rt; testing
	ImGui::Checkbox("Enable Material Editor", &mMaterialsPlaygroundApp->mUIOptions.mEnableMaterialEditor);
	if (mMaterialsPlaygroundApp->mUIOptions.mEnableMaterialEditor) // todo_rt; handle the turn on off condition better
	{
		if (auto sceneGraph = mMaterialsPlaygroundApp->GetScene()->GetSceneGraph())
		{
			auto const& meshInstances = sceneGraph->GetMeshInstances();
			const char* currentSelectedMesh = meshInstances[mMaterialsPlaygroundApp->mUIOptions.mCurrentlySelectedMeshIdx]->GetMesh()->name.c_str();
			if (ImGui::BeginCombo("Meshes", currentSelectedMesh))
			{
				for (int i = 0; i < meshInstances.size(); ++i)
				{
					const bool idx = (mMaterialsPlaygroundApp->mUIOptions.mCurrentlySelectedMeshIdx == i);
					if (ImGui::Selectable(meshInstances[i]->GetMesh()->name.c_str(), idx))
						mMaterialsPlaygroundApp->mUIOptions.mCurrentlySelectedMeshIdx = i;

					// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
					if (idx)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			const char* currentMeshMaterialName = meshInstances[mMaterialsPlaygroundApp->mUIOptions.mCurrentlySelectedMeshIdx]->GetMesh()->geometries[0]->material->name.c_str();

			donut::engine::MaterialsData matEditorData;
			matEditorData.mGLTFMaterial = meshInstances[mMaterialsPlaygroundApp->mUIOptions.mCurrentlySelectedMeshIdx]->GetMesh()->geometries[0]->material;

			mMaterialEditor->Show(matEditorData);

#pragma region Material-Selection
			//if (ImGui::BeginCombo("Material", currentMeshMaterialName))
				//{
				//	for (int i = 0; i < meshInstances.size(); ++i)
				//	{
				//		const bool idx = (mMaterialsPlaygroundApp->mUIOptions.mCurrentlySelectedMeshIdx == i);
				//		if (ImGui::Selectable(meshInstances[i]->GetMesh()->name.c_str(), idx))
				//			mMaterialsPlaygroundApp->mUIOptions.mCurrentlySelectedMeshIdx = i;

				//		// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				//		if (idx)
				//			ImGui::SetItemDefaultFocus();
				//	}

				//	ImGui::EndCombo();
				//}
#pragma endregion
			
		}
	}
	ImGui::End();
}

bool MaterialsPlayground::Init()
{	
	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/Common", PBRTesting_Private::commonShaderPath);
	rootFS->mount("/assets/Textures", PBRTesting_Private::assetTexturesPath);
	rootFS->mount("/assets/GLTFModels", PBRTesting_Private::gltfAssetPath);
	rootFS->mount("/shaders/RenderPasses", PBRTesting_Private::renderPassesShaderPath);

	mShaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	m_CommonPasses = std::make_shared<donut::engine::CommonRenderPasses>(GetDevice(), mShaderFactory);
	mBindingCache = std::make_unique<donut::engine::BindingCache>(GetDevice());

	auto nativeFS = std::make_shared<donut::vfs::NativeFileSystem>();
	m_TextureCache = std::make_shared<donut::engine::TextureCache>(GetDevice(), nativeFS, nullptr);

	donut::engine::CommonRenderPasses cmnRenderPasses(GetDevice(), mShaderFactory);
	donut::engine::TextureCache textureCache(GetDevice(), rootFS, nullptr);

	mCommandList = GetDevice()->createCommandList();
	
	mOpaqueDrawStrategy = std::make_unique<donut::render::InstancedOpaqueDrawStrategy>();
	mTransparentDrawStrategy = std::make_unique<donut::render::TransparentDrawStrategy>();

	{ // scene setup
		SetAsynchronousLoadingEnabled(false);
		BeginLoadingScene(nativeFS, PBRTesting_Private::dragonAttenuationModel);

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
			for (int i = 0; i < 0; ++i)
			{
				for (int j = 0; j < 1; ++j)
				{
					auto modelNode = mScene->LoadAtLeaf(PBRTesting_Private::duckModel);
					modelNode->SetName(std::format("Model {}", count + 1));
					modelNode->SetTranslation(double3(0.0 + i * 4, 2.0, -5.5 + j * 3));
					modelNode->SetScaling(double3(1.2f));
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
				auto pos = PBRTesting_Private::locGetRandomColor();
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

bool MaterialsPlayground::LoadScene(std::shared_ptr<donut::vfs::IFileSystem> aFileSystem, const std::filesystem::path& sceneFileName)
{
	assert(m_TextureCache);

	if(!mScene)
		mScene = std::make_shared<donut::engine::Scene>(GetDevice(), *mShaderFactory, aFileSystem, m_TextureCache, nullptr, nullptr);
	
	if (mScene->Load(sceneFileName))
	{
		return true;
	}

	return false;
}

void MaterialsPlayground::BackBufferResizing()
{
	mRenderTargets = nullptr;
	mBindingCache->Clear();
}

void MaterialsPlayground::Animate(float fElapsedTimeSeconds)
{
	mCamera.Animate(fElapsedTimeSeconds);
}

bool MaterialsPlayground::KeyboardUpdate(int key, int scancode, int action, int mods)
{
	mCamera.KeyboardUpdate(key, scancode, action, mods);
	return true;
}

bool MaterialsPlayground::MousePosUpdate(double xpos, double ypos)
{
	mCamera.MousePosUpdate(xpos, ypos);
	return true;
}

bool MaterialsPlayground::MouseButtonUpdate(int button, int action, int mods)
{
	mCamera.MouseButtonUpdate(button, action, mods);
	return true;
}

void MaterialsPlayground::BackBufferResized(const uint32_t width, const uint32_t height, const uint32_t sampleCount)
{
	 // TODO_RT: Support This
	mRenderTargets = nullptr;
	mGBufferFillPass = nullptr;
	mDeferredLightingPass = nullptr;
	mForwardShadingPass = nullptr;
}

void MaterialsPlayground::Render(nvrhi::IFramebuffer* aFramebuffer)
{
	GetDeviceManager()->SetVsyncEnabled(mUIOptions.mVsync);

	const auto& fbinfo = aFramebuffer->getFramebufferInfo();

	if (!mRenderTargets) // Gbuffer Render Targets Setup
	{
		mBindingCache->Clear();
		mRenderTargets = std::make_shared<RenderTargets>(GetDevice(),
			dm::uint2(fbinfo.width, fbinfo.height),
			1, // SampleCount
			false,
			true); // TODO_RT: Investigate this value for the projection, should it be false or need to be true?
	}

	if (mUIOptions.mEnableDeferredShading)
	{
		if (!mGBufferFillPass)
		{
			mGBufferFillPass = std::make_unique<donut::render::GBufferFillPass>(GetDevice(), m_CommonPasses);

			donut::render::GBufferFillPass::CreateParameters params;
			mGBufferFillPass->Init(*mShaderFactory, params);
		}

		if (!mDeferredLightingPass)
		{
			mDeferredLightingPass = std::make_unique<donut::render::DeferredLightingPass>(GetDevice(), m_CommonPasses);
			mDeferredLightingPass->Init(mShaderFactory);
			mDeferredLightingPass->ResetBindingCache();
		}
	}

	if (!mForwardShadingPass)
	{
		mForwardShadingPass = std::make_unique<donut::render::ForwardShadingPass>(GetDevice(), m_CommonPasses);

		donut::render::ForwardShadingPass::CreateParameters forwardParams;
		mForwardShadingPass->Init(*mShaderFactory, forwardParams);
	}

	nvrhi::Viewport windowViewport(float(fbinfo.width), float(fbinfo.height));
	mView.SetViewport(windowViewport);
	mView.SetMatrices(mCamera.GetWorldToViewMatrix(),
		perspProjD3DStyleReverse(PI_f * 0.25f, windowViewport.width() / windowViewport.height(),
			0.1f));
	mView.UpdateCache();

	mCommandList->open();

	mScene->RefreshBuffers(mCommandList, GetFrameIndex());

	mRenderTargets->Clear(mCommandList);

	donut::render::DeferredLightingPass::Inputs deferredLightingInputs;
	donut::render::ForwardShadingPass::Context ctxFwd;

	if (mUIOptions.mEnableDeferredShading)
	{
		donut::render::GBufferFillPass::Context ctx = {};
		donut::render::RenderCompositeView(mCommandList,
			&mView,
			&mView,
			*mRenderTargets->GBufferFramebuffer,
			mScene->GetSceneGraph()->GetRootNode(),
			*mOpaqueDrawStrategy,
			*mGBufferFillPass,
			ctx,
			"GBuffer Fill Pass");

		deferredLightingInputs.SetGBuffer(*mRenderTargets);
		deferredLightingInputs.output = mRenderTargets->mOutputColor;
		deferredLightingInputs.ambientColorTop = PBRTesting_Private::ambientColorTop;
		deferredLightingInputs.ambientColorBottom = PBRTesting_Private::ambientColorBottom;
		deferredLightingInputs.lights = &mScene->GetSceneGraph()->GetLights();

		mDeferredLightingPass->Render(mCommandList,
			mView,
			deferredLightingInputs);
	}
	else
	{
		mForwardShadingPass->PrepareLights(ctxFwd,
			mCommandList,
			mScene->GetSceneGraph()->GetLights(),
			PBRTesting_Private::ambientColorTop,
			PBRTesting_Private::ambientColorBottom,
			{});

		donut::render::RenderCompositeView(mCommandList,
			&mView,
			&mView,
			*mRenderTargets->mFwdFramebuffer,
			mScene->GetSceneGraph()->GetRootNode(),
			*mOpaqueDrawStrategy,
			*mForwardShadingPass,
			ctxFwd,
			"Forward Opaque Pass");
	}

	if (mUIOptions.mEnableTranslucency) // TODO_RT: check if we need this, "|| !mUIOptions.mEnableDeferredShading"
	{
		mForwardShadingPass->PrepareLights(ctxFwd,
			mCommandList,
			mScene->GetSceneGraph()->GetLights(),
			PBRTesting_Private::ambientColorTop,
			PBRTesting_Private::ambientColorBottom,
			{});

		donut::render::RenderCompositeView(mCommandList,
			&mView,
			&mView,
			*mRenderTargets->mFwdFramebuffer,
			mScene->GetSceneGraph()->GetRootNode(),
			*mTransparentDrawStrategy,
			*mForwardShadingPass,
			ctxFwd,
			"Forward Transparency Pass");
	}

	if (mUIOptions.mEnableDeferredShading)
	{
		switch (mUIOptions.mRTsViewMode)
		{
			case 0: m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.output, mBindingCache.get()); break;
			case 1: m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferDiffuse, mBindingCache.get()); break;
			case 2: m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferSpecular, mBindingCache.get()); break;
			case 3: m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferNormals, mBindingCache.get()); break;
			case 4: m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.gbufferEmissive, mBindingCache.get()); break;
			case 5: m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, deferredLightingInputs.depth, mBindingCache.get()); break;
			default: break;
		}
	}
	else
	{
		m_CommonPasses->BlitTexture(mCommandList, aFramebuffer, mRenderTargets->mOutputColor, mBindingCache.get());
	}

	mCommandList->close();
	GetDevice()->executeCommandList(mCommandList);
}