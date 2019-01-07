#pragma once
#include <unordered_map>
#include "GameEngine/Resources/Models/Material.h"
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class TreeShader : public ShaderProgram
{
public:
    TreeShader(IGraphicsApiPtr graphicsApi);

private:
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;
};
}  // GameEngine
