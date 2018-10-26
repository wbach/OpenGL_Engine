#pragma once
#include "GameEngine/Shaders/ShaderProgram.h"
#include "Logger/Log.h"

namespace GameEngine
{
class ShadowShader : public ShaderProgram
{
public:
    enum UniformLocation
    {
        TransformationMatrix,
        ProjectionViewMatrix,
        UseBoneTransform,
        BonesTransforms,
        NumberOfRows,
        TextureOffset,
        ModelTexture
    };

    ShadowShader(IGraphicsApiPtr graphicsApi);
    void Init();
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;

    template <class T>
    void Load(UniformLocation location, const T& value) const
    {
        if (uniformLocations.count(location) == 0 || uniformLocations.at(location).empty())
        {
            Log("ShadowShader : Try load to shader not set variable : " + std::to_string(location));
            return;
        }
        LoadValue(uniformLocations.at(location)[0], value);
    }

    template <class T>
    void Load(UniformLocation location, const T& value, int id) const
    {
        if (uniformLocations.count(location) == 0 || uniformLocations.at(location).size() <= static_cast<uint32>(id))
        {
            Log("ShadowShader : Try load to shader not set variable : " + std::to_string(location));
            return;
        }
        LoadValue(uniformLocations.at(location)[id], value);
    }

private:
    std::unordered_map<UniformLocation, std::vector<uint32>> uniformLocations;
};
} // namespace GameEngine
