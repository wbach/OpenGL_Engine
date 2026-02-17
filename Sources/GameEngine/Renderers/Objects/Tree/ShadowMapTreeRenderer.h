#pragma once
#include "TreeRenderer.h"

namespace GameEngine
{
class ShadowMapTreeRenderer : public TreeRenderer
{
public:
    ShadowMapTreeRenderer(RendererContext&);

    void init() override;
    void reloadShaders() override;
    void RenderTree(const Components::TreeRendererComponent&, float) override;

private:
    void RenderTrunk(const Components::TreeRendererComponent&, LevelOfDetail) const;
    void RenderLeafs(const Components::TreeRendererComponent&) const;
    void RenderLeafs(const Components::TreeRendererComponent&, LevelOfDetail) const;
    LevelOfDetail getLevelOfDetail(float) const;

private:
    ShaderProgram leafsShader_;
    ShaderProgram leafsBilboardsShader_;
    ShaderProgram trunkShader_;
};
}  // namespace GameEngine
