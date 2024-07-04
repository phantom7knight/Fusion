#pragma once

#include "../../Engine/SceneTypes.h"

// Signal-Slot
#include "../../SignalSlot/signal.hpp"

void Testing()
{
	auto lambda = []() { std::cout << "lambda\n"; };

	donut::engine::SigSlot::signal<> sig;

	sig.connect(lambda);

	sig();

}


namespace donut::engine
{
	class MaterialEditor
	{
	public:
		MaterialEditor()
		{
			Testing();
		}
		//if (m_callbacks.afterRender) m_callbacks.afterRender(*this);
		//std::function<void(DeviceManager&)> afterRender = nullptr;
	private:
		std::shared_ptr < donut::engine::Material > mGLTFMaterial;
		//std::shared_ptr < donut::engine::Material > mMTLXMaterial;
		//std::shared_ptr < donut::engine::Material > mAdobeSurfaceMaterial;
	};
}