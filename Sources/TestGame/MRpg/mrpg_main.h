#pragma once
#include "GameEngine/Api/IGraphicsApi.h"

namespace MmmoRpg
{
	void StartGame(std::unique_ptr<GameEngine::IGraphicsApi> gptr);
}