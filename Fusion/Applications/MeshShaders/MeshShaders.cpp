#include "MeshShaders.h"

#include <nvrhi/nvrhi.h>
#include <nvrhi/utils.h>

#include "../../Core/Engine/CommonRenderPasses.h"
#include "../../Core/Engine/TextureCache.h"
#include "../../Core/App/DeviceManager.h"

using namespace donut::math;
#include "../../../Assets/Shaders/Includes/lighting_cb.h"

namespace MeshShaders_Private
{
	const std::filesystem::path baseAssetsPath = donut::app::GetDirectoryWithExecutable() / "../../../Assets/";
	const std::filesystem::path appShaderPath = baseAssetsPath / "Shaders/Applications/MeshShaders/Generated";
	const std::filesystem::path commonShaderPath = baseAssetsPath / "Shaders/Common/Generated";
	const std::filesystem::path renderPassesShaderPath = baseAssetsPath / "Shaders/RenderPasses/Generated";
	const std::filesystem::path assetTexturesPath = baseAssetsPath / "Textures";
	const std::filesystem::path gltfAssetPath = baseAssetsPath / "GLTFModels";
	const std::filesystem::path duckFileName = gltfAssetPath / "2.0/Duck/glTF/Duck.gltf";
	const std::filesystem::path sponzaFileName = gltfAssetPath / "2.0/Sponza/glTF/Sponza.gltf";
	const std::filesystem::path damagedHelmetFileName = gltfAssetPath / "2.0/DamagedHelmet/glTF/DamagedHelmet.gltf";
	const std::filesystem::path carbonFibreFileName = gltfAssetPath / "2.0/CarbonFibre/glTF/CarbonFibre.gltf";
}

UIRenderer::UIRenderer(donut::app::DeviceManager* deviceManager, std::shared_ptr<MeshShadersApp> aApp)
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

	dm::float3 camPos = mInitApp->GetCameraPosition();
	ImGui::Text("Camera Position: X: %.2f Y: %.2f Z: %.2f", camPos.x, camPos.y, camPos.z);

	ImGui::Checkbox("Enable Vsync", &mInitApp->mUIOptions.mVsync);

	// TODO_RT: Fix this
	/*auto& arr = mInitApp->mUIOptions.mAppModeOptions;
	ImGui::Combo("Examples", &mInitApp->mUIOptions.mAppMode, arr.data(), arr.size());*/

	ImGui::End();
}

bool MeshShadersApp::InitAppShaderSetup(std::shared_ptr<donut::engine::ShaderFactory> aShaderFactory)
{
	mTriangle.mMeshShader = aShaderFactory->CreateShader("MeshShaders/Triangle.hlsl", "main_ms", nullptr, nvrhi::ShaderType::Mesh);
	mTriangle.mPixelShader = aShaderFactory->CreateShader("MeshShaders/Triangle.hlsl", "main_ps", nullptr, nvrhi::ShaderType::Pixel);

	if (!mTriangle.mMeshShader || !mTriangle.mPixelShader)
		return false;

	return true;
}

bool MeshShadersApp::Init()
{
	std::shared_ptr<donut::vfs::RootFileSystem> rootFS = std::make_shared<donut::vfs::RootFileSystem>();
	rootFS->mount("/shaders/MeshShaders", MeshShaders_Private::appShaderPath);
	rootFS->mount("/shaders/Common", MeshShaders_Private::commonShaderPath);
	rootFS->mount("/assets/Textures", MeshShaders_Private::assetTexturesPath);
	rootFS->mount("/assets/GLTFModels", MeshShaders_Private::gltfAssetPath);
	rootFS->mount("/shaders/RenderPasses", MeshShaders_Private::renderPassesShaderPath);

	mShaderFactory = std::make_shared<donut::engine::ShaderFactory>(GetDevice(), rootFS, "/shaders");
	m_CommonPasses = std::make_shared<donut::engine::CommonRenderPasses>(GetDevice(), mShaderFactory);
	mBindingCache = std::make_unique<donut::engine::BindingCache>(GetDevice());

	auto nativeFS = std::make_shared<donut::vfs::NativeFileSystem>();
	m_TextureCache = std::make_shared<donut::engine::TextureCache>(GetDevice(), nativeFS, nullptr);

	// todo_rt: remove these, since they are added above
	donut::engine::CommonRenderPasses cmnRenderPasses(GetDevice(), mShaderFactory);
	donut::engine::TextureCache textureCache(GetDevice(), rootFS, nullptr);

	if (!InitAppShaderSetup(mShaderFactory))
		return false;

	mCommandList = GetDevice()->createCommandList();

	return true;
}

bool MeshShadersApp::LoadScene(std::shared_ptr<donut::vfs::IFileSystem> aFileSystem, const std::filesystem::path& sceneFileName)
{
	assert(m_TextureCache);
	donut::engine::Scene* scene = new donut::engine::Scene(GetDevice(), *mShaderFactory, aFileSystem, m_TextureCache, nullptr, nullptr);

	if (scene->Load(sceneFileName))
	{
		mScene = std::unique_ptr<donut::engine::Scene>(std::move(scene));
		return true;
	}

	return false;
}

void MeshShadersApp::BackBufferResizing()
{
	mTriangle.mTriangleMeshletPipeline = nullptr;
	mBindingCache->Clear();
}

void MeshShadersApp::Animate(float fElapsedTimeSeconds)
{
	mCamera.Animate(fElapsedTimeSeconds);
}

bool MeshShadersApp::KeyboardUpdate(int key, int scancode, int action, int mods)
{
	mCamera.KeyboardUpdate(key, scancode, action, mods);
	return true;
}

bool MeshShadersApp::MousePosUpdate(double xpos, double ypos)
{
	mCamera.MousePosUpdate(xpos, ypos);
	return true;
}

bool MeshShadersApp::MouseButtonUpdate(int button, int action, int mods)
{
	mCamera.MouseButtonUpdate(button, action, mods);
	return true;
}

void MeshShadersApp::Render(nvrhi::IFramebuffer* framebuffer)
{
	assert(mCommandList);

	GetDeviceManager()->SetVsyncEnabled(mUIOptions.mVsync);

	// TODO_RT: enable this once there r more variations
	//if (mUIOptions.mAppMode == 0) // Init
	{
		if (!mTriangle.mTriangleMeshletPipeline)
		{
			nvrhi::MeshletPipelineDesc psoMeshletDesc;
			psoMeshletDesc.MS = mTriangle.mMeshShader;
			psoMeshletDesc.PS = mTriangle.mPixelShader;
			psoMeshletDesc.primType = nvrhi::PrimitiveType::TriangleList;
			psoMeshletDesc.renderState.depthStencilState.depthTestEnable = false;

			mTriangle.mTriangleMeshletPipeline = GetDevice()->createMeshletPipeline(psoMeshletDesc, framebuffer);
		}

		mCommandList->open();

		nvrhi::utils::ClearColorAttachment(mCommandList, framebuffer, 0, nvrhi::Color(0.f));

		nvrhi::MeshletState state;
		state.pipeline = mTriangle.mTriangleMeshletPipeline;
		state.framebuffer = framebuffer;
		state.viewport.addViewportAndScissorRect(framebuffer->getFramebufferInfo().getViewport());

		mCommandList->setMeshletState(state);
		mCommandList->dispatchMesh(1, 1, 1);

		mCommandList->close();
		GetDevice()->executeCommandList(mCommandList);
	}
}