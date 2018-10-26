#pragma once
#include <unordered_map>
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class BlurShader : public ShaderProgram
{
public:
    BlurShader(IGraphicsApiPtr graphicsApi);

    enum UniformLocation
    {
        ColorMap
    };

    template <class T>
    void Load(UniformLocation location, const T& value) const
    {
        if (uniformLocations.count(location) == 0)
        {
            Log("CTerrainShader : Try load to shader not set variable : " + std::to_string(location));
            return;
        }
        LoadValue(uniformLocations.at(location), value);
    }

private:
    virtual void BindAttributes() override;
    virtual void GetAllUniformLocations() override;
    virtual void ConnectTextureUnits() const override;

private:
    std::unordered_map<UniformLocation, uint32> uniformLocations;
};
}  // GameEngine
