#pragma once
#include <vector>
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
namespace Renderer
{
namespace Gui
{
class GuiTextureShader : public ShaderProgram
{
public:
    GuiTextureShader(IGraphicsApi& graphicsApi);
    void GetAllUniformLocations() override;
    void BindAttributes() override;
};
}  // Gui
}  // Renderer
}  // GameEngine
