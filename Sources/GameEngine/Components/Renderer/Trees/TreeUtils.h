#pragma once
#include "Leaf.h"
#include <vector>

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class Model;
struct Material;
class IResourceManager;
Model* CreateLeafModel(IResourceManager&, GraphicsApi::IGraphicsApi&, const std::vector<Leaf>&, const Material&);
Model* CreateLeafModel(IResourceManager&, GraphicsApi::IGraphicsApi&, const std::vector<LeafCluster>&, const Material&);
}  // namespace GameEngine

