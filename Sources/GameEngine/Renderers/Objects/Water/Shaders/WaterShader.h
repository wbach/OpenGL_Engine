#pragma once
#include <unordered_map>
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class WaterShader : public ShaderProgram
{
public:
    WaterShader(IGraphicsApiPtr graphicsApi);

private:
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;
};
}  // GameEngine
