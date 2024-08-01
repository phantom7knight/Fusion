#pragma once

#include "../../Engine/SceneTypes.h"
#include "../../SignalSlot/signal.hpp"
#include "../../Engine/SceneGraph.h"


namespace donut::engine
{
	using namespace donut::engine::SigSlot;

	// TODO_RT: this needs to b reworked with naming and stuff
	//struct MatEditorOptions
	//{
	//	MatEditorOptions() : mData(0) {}

	//	void setSelectedMesh(int value)
	//	{
	//		mData = (mData & 0xFFF0) | (value & 0xF); // 4 bits for mSelectedMesh
	//	}

	//	int getSelectedMesh() const
	//	{
	//		return mData & 0xF;
	//	}

	//	void setSelectedMaterialDomain(int value)
	//	{
	//		mData = (mData & 0xFF0F) | ((value & 0xF) << 4); // 4 bits for mSelectedDomain
	//	}

	//	int getSelectedMaterialDomain() const
	//	{
	//		return (mData >> 4) & 0xF;
	//	}

	//	void setSelectedMaterialType(int value)
	//	{
	//		mData = (mData & 0xF0FF) | ((value & 0xF) << 8); // 4 bits for mSelectedMaterialType
	//	}

	//	int getSelectedMaterialType() const
	//	{
	//		return (mData >> 8) & 0xF;
	//	}

	//private:
	//	uint16_t mData; // Selected Mesh, Selected Mat Domain, Selected Material Type(GLTF, Adobe, MatX)
	//};

	struct MaterialsData
	{
		std::shared_ptr<donut::engine::Material> mGLTFMaterial;
		//std::shared_ptr < donut::engine::Material > mMTLXMaterial;
		//std::shared_ptr < donut::engine::Material > mAdobeSurfaceMaterial;
	};

	class MaterialEditor
	{
	public:
		MaterialEditor() {}
		MaterialEditor(std::shared_ptr<SceneGraph> aSceneGraph);
		~MaterialEditor();
		
		MaterialEditor(const MaterialEditor&) = delete;
		MaterialEditor& operator=(const MaterialEditor&) = delete;

		void Show(MaterialsData aMatEditorData);

		signal<> OnMaterialChanged;
		signal<> OnMaterialReferesh;

	private:
		void SetupMaterialEditor(MaterialsData aMatEditorData);
		void GLTFMaterialSetup(std::shared_ptr<donut::engine::Material> aGLTFMaterial);

		MaterialsData mOptions;
		std::shared_ptr<SceneGraph> mSceneGraph;
	};
}