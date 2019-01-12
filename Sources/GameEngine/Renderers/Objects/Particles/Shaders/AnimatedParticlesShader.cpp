#include "AnimatedParticlesShader.h"
#include "ParticlesShadersUniforms.h"

#define GetLocation(X) uniforms_[ParticlesShadersUniforms::X] = GetUniformLocation(#X);

namespace GameEngine
{
AnimatedParticlesShader::AnimatedParticlesShader(IGraphicsApiPtr graphicsApi)
    : ShaderProgram(graphicsApi, graphicsApi->GetShaderFiles(Shaders::AnimatedParticles))
{
}
void AnimatedParticlesShader::GetAllUniformLocations()
{
    uniforms_.resize(ParticlesShadersUniforms::SIZE);
    GetLocation(ProjectionMatrix);
    GetLocation(NumberOfRows);
    GetLocation(ParticleTexture);
}
void AnimatedParticlesShader::BindAttributes()
{
    BindAttribute(0, "Position");
    BindAttribute(1, "TexCoord");
    BindAttribute(2, "ModelViewMatrix");
    BindAttribute(6, "TextureOffset");
    BindAttribute(7, "BlendFactor");
}
void AnimatedParticlesShader::ConnectTextureUnits() const
{
    Load(ParticlesShadersUniforms::ParticleTexture, 0);
}
}
