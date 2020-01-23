#pragma once
#include "GraphicsApi/IGraphicsApi.h"

namespace MmmoRpg
{
void StartGame(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr);
}
