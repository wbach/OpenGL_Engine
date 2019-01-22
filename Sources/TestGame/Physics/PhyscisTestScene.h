#pragma once
#include "GraphicsApi/IGraphicsApi.h"

namespace PhysicsTestGame
{
	void Start(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr);
}