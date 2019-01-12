#include "ParticlesShader.h"
#include "ParticlesShadersUniforms.h"

#define GetLocation(X) uniforms_[ParticlesShadersUniforms::X] = GetUniformLocation(#X);

namespace GameEngine
{
ParticlesShader::ParticlesShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::Particles))
{
}
void ParticlesShader::GetAllUniformLocations()
{
    uniforms_.resize(ParticlesShadersUniforms::SIZE);

    GetLocation(ProjectionMatrix);
    GetLocation(ModelViewMatrix);
    GetLocation(ParticleTexture);
}
void ParticlesShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "ModelViewMatrix");
    BindAttribute(6, "TextureOffset");
    BindAttribute(7, "NumberOfRows");
}
void ParticlesShader::ConnectTextureUnits() const
{
    Load(ParticlesShadersUniforms::ParticleTexture, 0);
}
}
