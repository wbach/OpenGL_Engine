#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class SkydomeShader : public ShaderProgram
{
public:
    SkydomeShader(GraphicsApi::IGraphicsApi& graphicsApi);
    void GetAllUniformLocations();
    void ConnectTextureUnits() const override;
    void BindAttributes() const;
};
} // namespace GameEngine
