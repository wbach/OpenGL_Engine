#include "ShadowShader.h"

void CShadowShader::Init()
{
    SetFiles
    ({
        {"Shadows/ShadowVertexShader.vert", GL_VERTEX_SHADER},
        {"Shadows/ShadowVertexShader.frag", GL_FRAGMENT_SHADER}
    });

    CShaderProgram::Init();
}

void CShadowShader::GetAllUniformLocations()
{
    location_TransformationMatrix = GetUniformLocation("TransformationMatrix");
    location_ProjectionMatrix     = GetUniformLocation("ProjectionMatrix");
    location_ViewMatrix           = GetUniformLocation("ViewMatrix");

    location_IsInstancedRender = GetUniformLocation("IsInstancedRender");

    // Animations
    location_UseBoneTransform = GetUniformLocation("UseBoneTransform");

    for (int x = 0; x < MAX_BONES; x++)
        location_Bones[x] = GetUniformLocation("Bones[" + std::to_string(x) + "]");
}

void CShadowShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(4, "TransformationMatrixes");
}
void CShadowShader::LoadUseInstancedRendering(const float& use) const
{
    LoadValue(location_IsInstancedRender, use);
}
void CShadowShader::LoadTransformMatrix(const glm::mat4& matrix) const
{
    LoadValue(location_TransformationMatrix, matrix);
}

void CShadowShader::LoadProjectionMatrix(const glm::mat4& matrix) const
{
    LoadValue(location_ProjectionMatrix, matrix);
}

void CShadowShader::LoadViewMatrix(const glm::mat4& matrix) const
{
    LoadValue(location_ViewMatrix, matrix);
}
void CShadowShader::LoadUseBonesTransformation(const float& is) const
{
    LoadValue(location_UseBoneTransform, is);
}
void CShadowShader::LoadBoneTransform(const glm::mat4& transform, unsigned int id) const
{
    LoadValue(location_Bones[id], transform);
}
