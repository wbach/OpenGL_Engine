#pragma once
#include <unordered_map>
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
    class TerrainMeshShader : public ShaderProgram
    {
    public:
        TerrainMeshShader(GraphicsApi::IGraphicsApi& graphicsApi);
        void GetAllUniformLocations();
        void BindAttributes() override;
        void ConnectTextureUnits() const override;
    };
}  // GameEngine
