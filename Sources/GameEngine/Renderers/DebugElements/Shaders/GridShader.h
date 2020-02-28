#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class GridShader : public ShaderProgram
{
public:
    GridShader(GraphicsApi::IGraphicsApi& graphicsApi);
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;
};
}  // namespace GameEngine
