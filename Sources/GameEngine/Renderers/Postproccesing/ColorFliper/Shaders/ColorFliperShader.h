#pragma once
#include <unordered_map>
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class ColorFliperShader : public ShaderProgram
{
public:
    ColorFliperShader(GraphicsApi::IGraphicsApi& graphicsApi);

private:
    virtual void BindAttributes() override;
    virtual void GetAllUniformLocations() override;
    virtual void ConnectTextureUnits() const override;
};
}  // GameEngine
