#pragma once

namespace GameEngine
{
enum GrassShaderUniforms
{
    ViewMatrix = 0,
    ProjectionMatrix,
    ColorMap,
    GlobalTime,
    ShadowVariables,
    ShadowMap,
    ToShadowMapSpace,
    ViewDistance,
    SIZE
};
}  // namespace GameEngine
