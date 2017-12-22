#pragma once
#include "../../../../Shaders/ShaderProgram.h"
#include "Logger/Log.h"

class CTesselationTerrainShader : public CShaderProgram
{
public:
    enum UniformLocation
    {
        TransformMatrix,
        ProjectionMatrix,
        ViewMatrix,
        Viewport,
        ScreenSize,
        LodFactor,
        HeightFactor,
        ViewDistance,
        BlendMap,
        BackgroundTexture,
        rTexture,
        bTexture,
        gTexture,
        RockTexture,
        BackgroundNormalTexture,
        rNormalTexture,
        bNormalTexture,
        gNormalTexture,
        RockNormalTexture,
        DisplacementMap,
        distanceDev,
        minTessLevelOuter,
        shadowMap,
        Count
    };

    CTesselationTerrainShader();
    virtual void Init();
    void GetAllUniformLocations();
    void BindAttributes() override;
    void ConnectTextureUnits() const override;

    template <class T>
    void Load(UniformLocation location, T value) const
    {
        if (uniformLocations[location] < 0)
            Log("CTerrainShader : Try load to shader not set variable : " + std::to_string(location));
        LoadValue(uniformLocations[location], value);
    }

protected:
    int uniformLocations[UniformLocation::Count];
};
