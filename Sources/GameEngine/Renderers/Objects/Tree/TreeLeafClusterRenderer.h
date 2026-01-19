#pragma once
#include <GraphicsApi/GraphicsApiDef.h>
#include <GraphicsApi/IGraphicsApi.h>
#include <Types.h>

#include <functional>
#include <optional>
#include <vector>

#include "GameEngine/Components/Renderer/Trees/Leaf.h"
#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
struct Material;
class GeneralTexture;

struct TreeClusters;
struct Leaf;
struct Cluster;

class TreeLeafClusterRenderer
{
public:
    TreeLeafClusterRenderer(GraphicsApi::IGraphicsApi&, IResourceManager&);

    using ResultCallback = std::function<void(std::optional<ClusterTextures>)>;
    void render(const TreeClusters&, const std::vector<Leaf>&, const Material&, ResultCallback);

private:
    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, GeneralTexture*, bool) const;

    void RenderClusters(IdType, IdType, GraphicsApi::IFrameBuffer&, const TreeClusters&, const std::vector<Leaf>&,
                        const Material&, const vec2ui&);
    void DrawClusterLeaves(const Cluster&, const std::vector<Leaf>&, const Material&, const mat4&);

private:
    GraphicsApi::IGraphicsApi& graphicsApi;
    GraphicsApi::ID transformBuferId;
    GraphicsApi::ID leafsSsbo;
    GraphicsApi::ID leafIndicesBufferId;

    IResourceManager& resourceManager;

    ShaderProgram leafsClusterShader;

    std::vector<GeneralTexture*> resultTextures;
};
}  // namespace GameEngine
