#pragma once

//#include "../../Core/CorePCH.hpp"
#include "../../Core/App/App.h"

class InitApp : protected App
{

public:

	virtual void Init() override;

	virtual void Load() override;

	virtual void Update(const float deltaTime) override;

	virtual void Render() override;

	virtual void UnLoad() override;

	virtual void Destroy() override;

};
