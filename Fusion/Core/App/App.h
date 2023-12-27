#pragma once

#include "../CorePCH.hpp"

class App
{
public:
	App(const STDStringView aWindowTitle, uint32_t aWidth, uint32_t aHeight)
		:mWindowTitle(aWindowTitle), mWindowWidth(aWidth), mWindowHeight(aHeight)
	{
	}

	App(const App& aOther) = delete;
	App& operator=(const App& aOther) = delete;

	App(App&& aOther) = delete;
	App& operator=(App&& aOther) = delete;

protected:
	virtual void Init() = 0;
	virtual void Load() = 0;
	virtual void Update(const float deltaTime) = 0;
	virtual void Render() = 0;
	virtual void Destroy() = 0;

protected:
	STDString mWindowTitle{};

	uint32_t mWindowWidth{};
	uint32_t mWindowHeight{};

	HWND m_windowHandle{};
};