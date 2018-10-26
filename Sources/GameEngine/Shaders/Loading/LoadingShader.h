#pragma once
#include <vector>
#include "../ShaderProgram.h"

namespace GameEngine
{
class LoadingShader : public ShaderProgram
{
public:
    LoadingShader(IGraphicsApiPtr);
    void Init();
    void LoadIsTextured(const float&) const;
    void LoadTransformMatrix(const mat4&) const;
    void LoadProjectionMatrix(const mat4&) const;
    void LoadViewMatrix(const mat4&) const;
    void ConnectTextureUnits() const override;

    void GetAllUniformLocations() override;
    void BindAttributes() override;

    virtual ~LoadingShader() override;

private:
    int location_isTextured;
    int location_transformationMatrix;
    int location_projectionMatrix;
    int location_viewMatrix;
    int location_modelTexture;
};
}  // namespace GameEngine
