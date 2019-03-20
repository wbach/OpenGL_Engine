#pragma once
#include <memory>
#include "Types.h"

namespace GameEngine
{
class Texture;
class IShaderProgram;
struct RendererContext;

class TerrainNormalMapRenderer
{
public:
    TerrainNormalMapRenderer(RendererContext& context);
    std::unique_ptr<Texture> Render(const Texture& heightMap) const;
    void Init();

private:
    std::unique_ptr<IShaderProgram> shader_;
    RendererContext& context_;
};
}  // namespace GameEngine
