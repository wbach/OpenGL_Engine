#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class ShadowShader : public ShaderProgram
{
public:
    ShadowShader(IGraphicsApiPtr graphicsApi);
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;
};
} // namespace GameEngine
