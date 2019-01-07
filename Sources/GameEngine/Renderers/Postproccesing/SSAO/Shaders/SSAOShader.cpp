#include "SSAOShader.h"
#include "SSAOShaderUniforms.h"

#define GetLocation(X) uniforms_[SSAOShaderUniforms::X] = GetUniformLocation(#X)

namespace GameEngine
{
SSAOShader::SSAOShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::SSAO))
{
}
void SSAOShader::GetAllUniformLocations()
{
    uniforms_.resize(SSAOShaderUniforms::SIZE);
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
    Load(SSAOShaderUniforms::PositionTexture, 0);
}
}
