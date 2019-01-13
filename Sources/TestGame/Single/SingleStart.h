#pragma once
#include "GameEngine/Api/IGraphicsApi.h"

namespace TestGame
{
	void TestGameStart(std::unique_ptr<GameEngine::IGraphicsApi> gptr);
}