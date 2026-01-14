#pragma once
#include "TreeRenderer.h"

namespace GameEngine
{
class ConcreteTreeRenderer : public TreeRenderer
{
public:
    ConcreteTreeRenderer(RendererContext&);

    void init() override;
    void reloadShaders() override;
    void RenderTree(const Components::TreeRendererComponent&, float) const override;

private:
    void RenderTrunk(const Components::TreeRendererComponent&, LevelOfDetail) const;
    void RenderLeafs(const Components::TreeRendererComponent&, LevelOfDetail) const;
    LevelOfDetail getLevelOfDetail(float) const;

private:
    ShaderProgram leafsShader_;
    ShaderProgram trunkShader_;
};
}  // namespace GameEngine
