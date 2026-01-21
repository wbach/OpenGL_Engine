#pragma once
#include "Tree.h"

namespace GameEngine
{
class IResourceManager;

void Export(const Tree&, const std::filesystem::path&);
Tree Import(GraphicsApi::IGraphicsApi&, IResourceManager&, const std::filesystem::path&);
}  // namespace GameEngine
