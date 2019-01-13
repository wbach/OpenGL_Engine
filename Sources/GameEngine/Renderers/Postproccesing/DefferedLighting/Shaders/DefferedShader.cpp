#include "DefferedShader.h"
#include "GameEngine/Lights/Light.h"
#include "DefferedShaderUniforms.h"

#define GetLocation(X) uniforms_[DefferedShaderUniforms::X] = GetUniformLocation(#X)

namespace GameEngine
{
DefferedShader::DefferedShader(IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, Shaders::Deffered)
{

}

void DefferedShader::GetAllUniformLocations()
{
    uniforms_.resize(DefferedShaderUniforms::SIZE + MAX_LIGHTS);

    GetLocation(PositionMap);
    GetLocation(ColorMap);
    GetLocation(NormalMap);
    GetLocation(SpecularMap);
    GetLocation(DepthTexture);
    GetLocation(CameraPosition);
    GetLocation(ScreenSize);
    GetLocation(NumberOfLights);
    GetLocation(SkyColour);
    GetLocation(ViewDistance);

    for (uint32 i = 0; i < MAX_LIGHTS; ++i)
    {
        lightParamsLocations_[i].type        = GetUniformLocation("Lights[" + std::to_string(i) + "].type_");
        lightParamsLocations_[i].position    = GetUniformLocation("Lights[" + std::to_string(i) + "].position_");
        lightParamsLocations_[i].colour      = GetUniformLocation("Lights[" + std::to_string(i) + "].colour_");
        lightParamsLocations_[i].attenuation = GetUniformLocation("Lights[" + std::to_string(i) + "].attenuation_");
        lightParamsLocations_[i].cutOff      = GetUniformLocation("Lights[" + std::to_string(i) + "].cutOff_");
    }
}
void DefferedShader::LoadLight(uint32 index, const Light& light)
{
    const auto& l = lightParamsLocations_[index];
    graphicsApi_.LoadValueToShader(l.type, static_cast<int>(light.GetType()));
    graphicsApi_.LoadValueToShader(l.attenuation, light.GetAttenuation());
    graphicsApi_.LoadValueToShader(l.colour, light.GetColour());
    graphicsApi_.LoadValueToShader(l.cutOff, light.GetCutoff());
    graphicsApi_.LoadValueToShader(l.position, light.GetPosition());
    graphicsApi_.LoadValueToShader(l.direction, light.GetDirection());
}
void DefferedShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
}
void DefferedShader::ConnectTextureUnits() const
{
    Load(DefferedShaderUniforms::PositionMap, 0);
    Load(DefferedShaderUniforms::ColorMap, 1);
    Load(DefferedShaderUniforms::NormalMap, 2);
    Load(DefferedShaderUniforms::SpecularMap, 3);
    Load(DefferedShaderUniforms::DepthTexture, 4);
}
}  // GameEngine
