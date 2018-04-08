#include "AnimatedParticlesShader.h"
#include "GameEngine/Objects/RenderAble/Terrain/TerrainTexturesTypes.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
	AnimatedParticlesShader::AnimatedParticlesShader(IGraphicsApiPtr graphicsApi)
		: CShaderProgram(graphicsApi)
	{
		SetFiles
		({
			{ "Particles/AnimatedParticlesShader.vert", ShaderType::VERTEX_SHADER },
			{ "Particles/AnimatedParticlesShader.frag", ShaderType::FRAGMENT_SHADER },
		});
	}
	void AnimatedParticlesShader::GetAllUniformLocations()
	{
		GetLocation(ProjectionMatrix);
		GetLocation(ModelViewMatrix);
		GetLocation(TextureOffset);
		GetLocation(NumberOfRows);
		GetLocation(BlendFactor);
		GetLocation(ParticleTexture);
	}
	void AnimatedParticlesShader::BindAttributes()
	{
		BindAttribute(0, "Position");
		BindAttribute(1, "TexCoord");
	}
	void AnimatedParticlesShader::ConnectTextureUnits() const
	{
		LoadValue(uniformLocations.at(UniformLocation::ParticleTexture), 0);
	}
}
