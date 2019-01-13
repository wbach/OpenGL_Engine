#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class EntityShader : public ShaderProgram
{
public:
    EntityShader(IGraphicsApi& graphicsApi);
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;
};
}  // namespace GameEngine
