#include "EntityShader.h"

namespace GameEngine
{
EntityShader::EntityShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
}
void EntityShader::Init()
{
    SetFiles({{"Simple/SimpleEntityShader.vert", ShaderType::VERTEX_SHADER},
              {"Simple/SimpleEntityShader.frag", ShaderType::FRAGMENT_SHADER}});

    ShaderProgram::Init();
}
void EntityShader::GetAllUniformLocations()
{
    location_TransformationMatrix = GetUniformLocation("TransformationMatrix");
    location_ProjectionMatrix     = GetUniformLocation("ProjectionMatrix");
    location_ViewMatrix           = GetUniformLocation("ViewMatrix");

    location_TextureOffset = GetUniformLocation("TextureOffset");
    location_NumberOfRows  = GetUniformLocation("NumberOfRows");

    // Animations
    location_UseBoneTransform = GetUniformLocation("UseBoneTransform");

    for (int x            = 0; x < MAX_BONES; x++)
        location_Bones[x] = GetUniformLocation("BonesTransforms[" + std::to_string(x) + "]");
}
void EntityShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "Normal");
    BindAttribute(3, "Tangent");
    BindAttribute(4, "Weights");
    BindAttribute(5, "BoneIds");
    // BindAttribute(4, "TransformationMatrixes");
}
void EntityShader::LoadTextureOffset(const vec2& offset) const
{
    LoadValue(location_TextureOffset, offset);
}
void EntityShader::LoadNumberOfRows(uint32 rows) const
{
    LoadValue(location_NumberOfRows, static_cast<float>(rows));
}
void EntityShader::LoadTransformMatrix(const mat4& matrix) const
{
    LoadValue(location_TransformationMatrix, matrix);
}
void EntityShader::LoadProjectionMatrix(const mat4& matrix) const
{
    LoadValue(location_ProjectionMatrix, matrix);
}
void EntityShader::LoadViewMatrix(const mat4& matrix) const
{
    LoadValue(location_ViewMatrix, matrix);
}
void EntityShader::LoadUseBonesTransformation(const float& is) const
{
    LoadValue(location_UseBoneTransform, is);
}
void EntityShader::LoadBoneTransform(const mat4& transform, uint32 id) const
{
    LoadValue(location_Bones[id], transform);
}
}  // namespace GameEngine
