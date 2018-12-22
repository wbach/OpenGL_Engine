#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class FontShader : public ShaderProgram
{
public:
    FontShader(IGraphicsApiPtr graphicsApi);

private:
    void GetAllUniformLocations() override;
    void BindAttributes() override;

private:
    int location_color;
    int location_translation;
    int location_transformationMatrix;
};
} // namespace GameEngine
