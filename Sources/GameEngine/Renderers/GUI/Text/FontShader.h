#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class FontShader : public ShaderProgram
{
public:
    FontShader(GameEngine::IGraphicsApiPtr graphicsApi);
    void Init();
    void LoadColour(const vec3& colour) const;
    void LoadTranslation(const vec2& pos) const;
    void SetScale(float scale) const;

private:
    void GetAllUniformLocations() override;
    void BindAttributes() override;
    void loadTransformation(const mat4& pos) const;

private:
    vec2 baseScale_;
    int location_color;
    int location_translation;
    int location_transformationMatrix;
};
} // namespace GameEngine
