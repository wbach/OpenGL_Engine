#pragma once
#include "Leaf.h"

namespace GraphicsApi
{
class IGraphicsApi;
}

namespace GameEngine
{
class Model;
class Material;
class IResourceManager;
Model* CreateLeafModel(IResourceManager&, GraphicsApi::IGraphicsApi&, const std::vector<Leaf>&, const Material&);
Model* CreateLeafModel(IResourceManager&, GraphicsApi::IGraphicsApi&, const std::vector<LeafCluster>&, const Material&);
}  // namespace GameEngine

