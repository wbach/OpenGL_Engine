#pragma once
#include <GraphicsApi/IGraphicsApi.h>

#include <functional>
#include <optional>
#include <vector>

#include "GameEngine/Renderers/IRenderer.h"
#include "GameEngine/Resources/IResourceManager.hpp"
#include "GameEngine/Resources/Models/ModelWrapper.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Types.h"

namespace GameEngine
{
class Mesh;
class Model;
struct Material;
struct RendererContext;
class ModelWrapper;
struct Time;
class GeneralTexture;

namespace Components
{
class TreeRendererComponent;
}  // namespace Components

struct TreeSubscriber
{
    GameObject* gameObject_{nullptr};
    std::vector<Components::TreeRendererComponent*> treeRendererComponent_{nullptr};
};

class TreeLeafClusterRenderer
{
public:
    TreeLeafClusterRenderer(GraphicsApi::IGraphicsApi&, IResourceManager&);

    using ResultCallback = std::function<void(std::vector<GeneralTexture*>)>;
    void render(const Model&, ResultCallback);

private:
    void BindMaterial(const Material&) const;
    void UnBindMaterial(const Material&) const;
    void BindMaterialTexture(uint32, GeneralTexture*, bool) const;
    void GenerateClusterAtlasTexture(GraphicsApi::IFrameBuffer&, const Model&);
    GeneralTexture* cloneAttachmentTexture(const GraphicsApi::IFrameBuffer&);

private:
    GraphicsApi::IGraphicsApi& graphicsApi;
    IResourceManager& resourceManager;

    ShaderProgram leafsClusterShader;

    std::vector<GeneralTexture*> resultTextures;
};
}  // namespace GameEngine
