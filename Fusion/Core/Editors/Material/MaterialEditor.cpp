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

void donut::engine::MaterialEditor::Show(MaterialEditorData aMatEditorData)
{
	SetupMaterialEditor(aMatEditorData);
}

void donut::engine::MaterialEditor::GLTFMaterialSetup(std::shared_ptr<Material> aGLTFMaterial)
{
	bool isMatDirty = false;

	ImGui::Text("Material Name: %s", aGLTFMaterial->name.c_str());

	isMatDirty |= ImGui::SliderFloat("Opacity", &aGLTFMaterial->opacity, 0.f, 1.f);
	isMatDirty |= ImGui::SliderFloat("Metalness", &aGLTFMaterial->metalness, 0.f, 1.f);
	isMatDirty |= ImGui::SliderFloat("Roughness", &aGLTFMaterial->roughness, 0.f, 1.f);
	isMatDirty |= ImGui::SliderFloat("Emissive Intensity", &aGLTFMaterial->emissiveIntensity, 1.f, 10.f);

	isMatDirty |= ImGui::ColorEdit4("Diffuse Color", (float*)aGLTFMaterial->baseOrDiffuseColor.data(), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
	
	isMatDirty |= ImGui::ColorEdit4("Specular Color", (float*)aGLTFMaterial->specularColor.data(), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);

	isMatDirty |= ImGui::ColorEdit4("Emissive Color", (float*)aGLTFMaterial->emissiveColor.data(), ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);


	if (isMatDirty)
		aGLTFMaterial->dirty = true;

}

void MaterialEditor::SetupMaterialEditor(MaterialEditorData aMatEditorData)
{
	ImGui::Begin("Material Editor", 0, ImGuiWindowFlags_AlwaysAutoResize);

	if (std::shared_ptr<Material> gltfMat = aMatEditorData.mGLTFMaterial)
	{
		GLTFMaterialSetup(gltfMat);
	}

	

	ImGui::End();
}
