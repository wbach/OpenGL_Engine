#pragma once

namespace GameEngine
{
enum TerrainShaderUniforms
{
    playerPosition,
    lightDirection,
    modelViewProjectionMatrix,
    displacementTexture,
    modelViewMatrix,
    projectionMatrix,
    heightFactor,
    toShadowMapSpace,
    shadowVariables,
    shadowMap,
    blendMap,
    backgorundTexture,
    backgorundTextureNormal,
    redTexture,
    redTextureNormal,
    greenTexture,
    greenTextureNormal,
    blueTexture,
    blueTextureNormal,
    rockTexture,
    rockNormalTexture,
    snowTexture,
    snowNormalTexture,
    displacementMap,
    SIZE
};
}  // namespace GameEngine
