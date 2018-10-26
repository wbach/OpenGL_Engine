#include "SSAOShader.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
SSAOShader::SSAOShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi)
{
    SetFiles({
        {"PostProcess/SSAO/SSAOShader.vert", ShaderType::VERTEX_SHADER},
        {"PostProcess/SSAO/SSAOShader.frag", ShaderType::FRAGMENT_SHADER},
    });
}
void SSAOShader::GetAllUniformLocations()
{
    GetLocation(ProjectionMatrix);
    GetLocation(ViewMatrix);
    GetLocation(Kernel);
    GetLocation(PositionTexture);
    GetLocation(SampleRadius);
}

void SSAOShader::BindAttributes()
{
    BindAttribute(0, "Position");
}

void SSAOShader::ConnectTextureUnits() const
{
    LoadValue(uniformLocations.at(UniformLocation::PositionTexture), 0);
}
}
