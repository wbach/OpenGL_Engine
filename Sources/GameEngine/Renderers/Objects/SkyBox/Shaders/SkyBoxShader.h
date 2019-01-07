#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class SkyBoxShader : public ShaderProgram
{
public:
    SkyBoxShader(GameEngine::IGraphicsApiPtr graphicsApi);
    void GetAllUniformLocations();
    void ConnectTextureUnits() const override;
    void BindAttributes() const;
};
} // namespace GameEngine
