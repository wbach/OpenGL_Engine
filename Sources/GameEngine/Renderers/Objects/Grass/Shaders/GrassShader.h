#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class GrassShader : public ShaderProgram
{
public:
    GrassShader(IGraphicsApiPtr graphicsApi);
    void ConnectTextureUnits() const override;
    void GetAllUniformLocations() override;
    void BindAttributes() override;
};
}  // namespace GameEngine
