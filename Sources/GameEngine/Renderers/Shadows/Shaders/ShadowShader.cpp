#include "ShadowShader.h"

CShadowShader::CShadowShader(GameEngine::IGraphicsApiPtr graphicsApi)
	: CShaderProgram(graphicsApi)
{
}

void CShadowShader::Init()
{
    SetFiles
    ({
        {"Shadows/ShadowVertexShader.vert", GameEngine::ShaderType::VERTEX_SHADER},
        {"Shadows/ShadowFragmentShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER}
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
        location_Bones[x] = GetUniformLocation("BonesTransforms[" + std::to_string(x) + "]");
}

void CShadowShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(4, "Weights");
	BindAttribute(5, "BoneIds");
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
