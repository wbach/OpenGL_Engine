#pragma once
#include "GraphicsApi/IGraphicsApi.h"

namespace TestGame
{
	void TestGameStart(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr);
}