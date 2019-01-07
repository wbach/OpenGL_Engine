#pragma once
#include <unordered_map>
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class BlurShader : public ShaderProgram
{
public:
    BlurShader(IGraphicsApiPtr graphicsApi);

private:
    virtual void BindAttributes() override;
    virtual void GetAllUniformLocations() override;
    virtual void ConnectTextureUnits() const override;
};
}  // GameEngine
