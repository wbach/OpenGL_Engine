#pragma once
#include <unordered_map>
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class PlantsShader : public ShaderProgram
{
public:
    PlantsShader(GraphicsApi::IGraphicsApi& graphicsApi);

private:
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;
};
}  // GameEngine
