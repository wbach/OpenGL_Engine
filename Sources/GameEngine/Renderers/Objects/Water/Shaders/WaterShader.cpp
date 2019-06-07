#include "WaterShader.h"
#include "WaterShaderUniforms.h"

#define GetLocation(X) uniforms_[WaterShaderUniforms::X] = GetUniformLocation(#X);

namespace GameEngine
{
WaterShader::WaterShader(GraphicsApi::IGraphicsApi& graphicsApi)
    : ShaderProgram(graphicsApi, GraphicsApi::Shaders::Water)
{
}

void WaterShader::GetAllUniformLocations()
{
    uniforms_.resize(WaterShaderUniforms::SIZE);

    GetLocation(reflectionTexture);
    GetLocation(refractionTexture);
    GetLocation(normalMap);
    GetLocation(depthMap);
    GetLocation(dudvMap);
}
void WaterShader::BindAttributes()
{
}

void WaterShader::ConnectTextureUnits() const
{
    Load(WaterShaderUniforms::reflectionTexture, 0);
    Load(WaterShaderUniforms::refractionTexture, 1);
    Load(WaterShaderUniforms::normalMap, 2);
    Load(WaterShaderUniforms::depthMap, 3);
    Load(WaterShaderUniforms::dudvMap, 4);
}
}  // namespace GameEngine
