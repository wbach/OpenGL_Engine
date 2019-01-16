#pragma once

namespace GameEngine
{
    enum TerrainMeshShaderUniforms
    {
        transformMatrix,
        projectionMatrix,
        viewMatrix,
        useShadows,
        shadowDistance,
        shadowMapSize,
        toShadowMapSpace,
        useNormalMap,
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
        shadowMap,
        SIZE
    };
}  // namespace GameEngine
