#pragma once

#include "../../Core/CorePCH.hpp"
#include "../../Core/App/App.h"
#include "Init.h"

int main()
{
	STDUniquePtr<App> app = STDMakeUniquePtr<InitApp>("Hello World!!!", 512, 512);


	

	return 0;
}