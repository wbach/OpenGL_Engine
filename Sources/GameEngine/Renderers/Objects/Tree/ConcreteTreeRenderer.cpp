#include "ConcreteTreeRenderer.h"

#include <vector>

#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Objects/Tree/TreeLeafClusterRenderer.h"
#include "GameEngine/Renderers/Objects/Tree/TreeRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/ShaderBuffers/ShaderBuffersBindLocations.h"
#include "Logger/Log.h"
#include "Types.h"

namespace GameEngine
{
ConcreteTreeRenderer::ConcreteTreeRenderer(RendererContext& context)
    : TreeRenderer(context)
    , leafsShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafs)
    , leafsClusterShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafsCluster)
    , trunkShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::Entity)
{
    startFade = EngineConf.renderer.lodDistance0;
    endFade   = EngineConf.renderer.lodDistance0 + 10;
}
void ConcreteTreeRenderer::RenderTree(const Components::TreeRendererComponent& treeRendererComponent, float distanceToCamera)
{
    auto lvl = getLevelOfDetail(distanceToCamera);
    RenderTrunk(treeRendererComponent, lvl);

    if (distanceToCamera < endFade)
    {
        RenderLeafs(treeRendererComponent);
    }

    if (distanceToCamera > startFade)
    {
        RenderLeafsClusters(treeRendererComponent);
    }
}
LevelOfDetail ConcreteTreeRenderer::getLevelOfDetail(float distanceToCamera) const
{
    if (distanceToCamera > EngineConf.renderer.lodDistance0)
    {
        return LevelOfDetail::L2;
    }
    return LevelOfDetail::L1;
}
void ConcreteTreeRenderer::RenderTrunk(const Components::TreeRendererComponent& treeRendererComponent, LevelOfDetail lvl) const
{
    if (auto model = treeRendererComponent.GetTrunkModel().Get(lvl))
    {
        trunkShader_.Start();
        RenderModel(*model);
    }
}
void ConcreteTreeRenderer::RenderLeafs(const Components::TreeRendererComponent& treeRendererComponent) const
{
    if (auto id = treeRendererComponent.GetLeafsShaderBufferId())
    {
        context_.graphicsApi_.BindShaderBuffer(*id);
    }

    if (treeRendererComponent.GetLeafsCount() > 0)
    {
        leafsShader_.Start();
        BindMaterial(treeRendererComponent.GetLeafMaterial());
        context_.graphicsApi_.RenderProcedural(treeRendererComponent.GetLeafsCount() * 6);
        leafsShader_.Stop();
    }
}
void ConcreteTreeRenderer::init()
{
    TreeRenderer::init();
    leafsShader_.Init();
    trunkShader_.Init();
    leafsClusterShader_.Init();
}
void ConcreteTreeRenderer::reloadShaders()
{
    trunkShader_.Reload();
    leafsShader_.Reload();
    leafsClusterShader_.Reload();
}
void ConcreteTreeRenderer::RenderLeafsClusters(const Components::TreeRendererComponent& component)
{
    if (component.getTreeClusters().clusters.empty())
    {
        return;
    }
    leafsClusterShader_.Start();

    const auto& textures = component.getClusterTextures();
    if (textures.baseColorTexture and textures.baseColorTexture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(0, *textures.baseColorTexture->GetGraphicsObjectId());
    }
    if (textures.normalTexture and textures.normalTexture->GetGraphicsObjectId())
    {
        context_.graphicsApi_.ActiveTexture(1, *textures.normalTexture->GetGraphicsObjectId());
    }

    if (const auto& clusterDataSssbo_ = component.GetClusterShaderBufferId())
        context_.graphicsApi_.BindShaderBuffer(*clusterDataSssbo_);

    auto count = component.getTreeClusters().clusters.size() * 12;

    if (count > 0)
    {
        context_.graphicsApi_.DisableCulling();
        context_.graphicsApi_.RenderProcedural(count);
    }
}
}  // namespace GameEngine
