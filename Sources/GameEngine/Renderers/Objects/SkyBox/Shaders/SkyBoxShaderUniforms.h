#pragma once

namespace GameEngine
{
enum SkyBoxShaderUniforms
{
    ProjectionMatrix = 0,
    ViewMatrix,
    DayCubeMap,
    NightCubeMap,
    FogColour,
    BlendFactor,
    SIZE
};
}  // namespace GameEngine
