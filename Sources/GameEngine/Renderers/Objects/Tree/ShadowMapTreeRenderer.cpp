#include "ShadowMapTreeRenderer.h"

#include "GameEngine/Components/Renderer/Trees/TreeRendererComponent.h"
#include "GameEngine/Engine/Configuration.h"
#include "GameEngine/Renderers/Objects/Tree/TreeRenderer.h"
#include "GameEngine/Renderers/RendererContext.h"
#include "GameEngine/Resources/Models/ModelWrapper.h"

namespace GameEngine
{
ShadowMapTreeRenderer::ShadowMapTreeRenderer(RendererContext& context)
    : TreeRenderer(context)
    , leafsShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafsShdaows)
    , leafsBilboardsShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::TreeLeafsShdaowsBilboards)
    , trunkShader_(context.graphicsApi_, GraphicsApi::ShaderProgramType::SimpleForwadEntity)
{
}
void ShadowMapTreeRenderer::reloadShaders()
{
    leafsShader_.Reload();
    leafsBilboardsShader_.Reload();
    trunkShader_.Reload();
}
void ShadowMapTreeRenderer::init()
{
    TreeRenderer::init();

    leafsShader_.Init();
    leafsBilboardsShader_.Init();
    trunkShader_.Init();
}
void ShadowMapTreeRenderer::RenderTree(const Components::TreeRendererComponent& treeRendererComponent,
                                       float distanceToCamera) const
{
    auto lvl = getLevelOfDetail(distanceToCamera);
    RenderTrunk(treeRendererComponent, lvl);
    RenderLeafs(treeRendererComponent, lvl);
}
void ShadowMapTreeRenderer::RenderLeafs(const Components::TreeRendererComponent& treeRendererComponent, LevelOfDetail lvl) const
{
    if (auto model = treeRendererComponent.GetLeafBilboardsModel())
    {
        leafsBilboardsShader_.Start();
        RenderModel(*model);
    }
}
void ShadowMapTreeRenderer::RenderTrunk(const Components::TreeRendererComponent& treeRendererComponent, LevelOfDetail lvl) const
{
    if (auto model = treeRendererComponent.GetTrunkModel().Get(LevelOfDetail::L2))
    {
        trunkShader_.Start();
        RenderModel(*model);
    }
}
LevelOfDetail ShadowMapTreeRenderer::getLevelOfDetail(float) const
{
    return LevelOfDetail::L2;
}
}  // namespace GameEngine