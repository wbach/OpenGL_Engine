#pragma once

namespace GameEngine
{
enum EntityShaderUniforms
{
    ViewMatrix = 0,
    ProjectionMatrix,
    TransformationMatrix,
    TextureOffset,
    NumberOfRows,
    UseBoneTransform,
    BonesTransforms,
    ClipPlane,
    IsUseFakeLighting,
    IsUseNormalMap,
    UseTexture,
    ColorMap,
    NormalMap,
    ShadowVariables,
    ShadowMap,
    ToShadowMapSpace,
    ModelMaterialAmbient,
    ModelMaterialDiffuse,
    ModelMaterialSpecular,
    ViewDistance,
    SpecularMap,
    SIZE
};
}  // namespace GameEngine
