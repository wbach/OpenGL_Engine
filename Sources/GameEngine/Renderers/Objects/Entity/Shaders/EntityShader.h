#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class EntityShader : public ShaderProgram
{
public:
    EntityShader(IGraphicsApiPtr graphicsApi);
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;

protected:
    IGraphicsApiPtr graphicsApi_;
};
}  // namespace GameEngine
