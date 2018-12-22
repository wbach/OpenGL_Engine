#include "BlurShader.h"
#include "BlurShaderUniforms.h"

#define GetLocation(X) uniforms_[BlurUniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
BlurShader::BlurShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::Blur))
{

}
void BlurShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
}
void BlurShader::GetAllUniformLocations()
{
    uniforms_.resize(BlurUniformLocation::SIZE);
    GetLocation(ColorMap);
}
void BlurShader::ConnectTextureUnits() const
{
    Load(BlurUniformLocation::ColorMap, 0);
}
}  // GameEngine
