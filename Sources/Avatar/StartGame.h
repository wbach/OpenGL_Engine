#pragma once
#include "GraphicsApi/IGraphicsApi.h"

namespace AvatarGame
{
void Start(std::unique_ptr<GraphicsApi::IGraphicsApi> gptr);
}
