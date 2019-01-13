#pragma once
#include "GameEngine/Api/IGraphicsApi.h"

namespace PhysicsTestGame
{
	void Start(std::unique_ptr<GameEngine::IGraphicsApi> gptr);
}