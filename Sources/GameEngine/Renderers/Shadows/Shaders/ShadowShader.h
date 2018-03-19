#pragma once
#include "../../../Shaders/ShaderProgram.h"

class CShadowShader : public CShaderProgram
{
public:
	CShadowShader(GameEngine::IGraphicsApiPtr graphicsApi);
    void Init();
    void GetAllUniformLocations();
    void BindAttributes() override;
    void LoadTransformMatrix(const glm::mat4& matrix) const;
    void LoadProjectionMatrix(const glm::mat4& matrix) const;
    void LoadViewMatrix(const glm::mat4& matrix) const;
    void LoadUseBonesTransformation(const float& is) const;
    void LoadBoneTransform(const glm::mat4& transform, unsigned int id) const;
    void LoadUseInstancedRendering(const float& use) const;

private:
    int location_TransformationMatrix;
    int location_ProjectionMatrix;
    int location_ViewMatrix;

    int location_IsInstancedRender;

    // Animations
    int location_UseBoneTransform;
    int location_Bones[MAX_BONES];
};
