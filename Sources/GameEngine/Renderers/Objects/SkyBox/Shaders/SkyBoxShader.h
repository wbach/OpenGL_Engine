#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"

namespace GameEngine
{
class SkyBoxShader : public ShaderProgram
{
public:
    SkyBoxShader(GameEngine::IGraphicsApiPtr graphicsApi);
    void Init();
    void LoadProjectionMatrix(const mat4& matrix) const;
    void LoadViewMatrix(mat4 matrix, const float& deltaTime, const float& distance_view);

    void GetAllUniformLocations();
    void ConnectTextureUnits() const override;

    void LoadFogColour(const float& r, const float& g, const float& b) const;
    void LoadBlendFactor(const float& factor) const;
    void BindAttributes() const;

private:
    int location_ProjectionMatrix;
    int location_ViewMatrix;
    int location_DayCubeMap;
    int location_NightCubeMap;
    int location_BlendFactor;
    int location_FogColour;

    float rotationSpeed_;
    float rotation_;
};
} // namespace GameEngine
