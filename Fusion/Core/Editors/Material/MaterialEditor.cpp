#include "MaterialEditor.h"
#include <imgui.h>
#include <nvrhi/common/misc.h>

using namespace donut::engine;

MaterialEditor::MaterialEditor(std::shared_ptr<SceneGraph> aSceneGraph)
	: mSceneGraph(aSceneGraph)
{
}

donut::engine::MaterialEditor::~MaterialEditor()
{
}

void donut::engine::MaterialEditor::Show(MaterialsData aMatData)
{
	SetupMaterialEditor(aMatData);
}

void donut::engine::MaterialEditor::GLTFMaterialSetup(std::shared_ptr<Material> aGLTFMaterial)
{
	bool isMatDirty = false;

	ImGui::Text("Material Name: %s", aGLTFMaterial->name.c_str());
	
	{
		isMatDirty |= ImGui::Checkbox("Use Base or Diffuse Texture", &aGLTFMaterial->enableBaseOrDiffuseTexture);
		isMatDirty |= ImGui::ColorEdit4("Diffuse Color", (float*)aGLTFMaterial->baseOrDiffuseColor.data(), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		isMatDirty |= ImGui::SliderFloat("Opacity", &aGLTFMaterial->opacity, 0.f, 1.f);
	}

	{
		isMatDirty |= ImGui::Checkbox("Use Specular Gloss Model", &aGLTFMaterial->useSpecularGlossModel);
		isMatDirty |= ImGui::ColorEdit4("Specular Color", (float*)aGLTFMaterial->specularColor.data(), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		isMatDirty |= ImGui::SliderFloat("Metalness", &aGLTFMaterial->metalness, 0.f, 1.f);
		isMatDirty |= ImGui::SliderFloat("Roughness", &aGLTFMaterial->roughness, 0.f, 1.f);
	}

	{
		isMatDirty |= ImGui::Checkbox("Use Normal Texture", &aGLTFMaterial->enableNormalTexture);
		isMatDirty |= ImGui::SliderFloat("Normal Texture Scale", &aGLTFMaterial->normalTextureScale, 0.f, 1.f);
	}

	{
		isMatDirty |= ImGui::Checkbox("Use Emissive Texture", &aGLTFMaterial->enableEmissiveTexture);
		isMatDirty |= ImGui::ColorEdit4("Emissive Color", (float*)aGLTFMaterial->emissiveColor.data(), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
		isMatDirty |= ImGui::SliderFloat("Emissive Intensity", &aGLTFMaterial->emissiveIntensity, 1.f, 10.f);
	}

	{
		isMatDirty |= ImGui::Checkbox("Use Occlusion Texture", &aGLTFMaterial->enableOcclusionTexture);
		isMatDirty |= ImGui::SliderFloat("Occlusion Strength", &aGLTFMaterial->occlusionStrength, 1.f, 5.f);
	}

	{
		isMatDirty |= ImGui::Checkbox("Use Transmission Texture", &aGLTFMaterial->enableTransmissionTexture);
		isMatDirty |= ImGui::SliderFloat("Transmission Factor", &aGLTFMaterial->transmissionFactor, 1.f, 5.f);
	}

	if (isMatDirty)
		aGLTFMaterial->dirty = true;

}

void MaterialEditor::SetupMaterialEditor(MaterialsData aMatData)
{
	ImGui::Begin("Material Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);

	if (std::shared_ptr<Material> gltfMat = aMatData.mGLTFMaterial)
	{
		GLTFMaterialSetup(gltfMat);
	}

	ImGui::End();
}
