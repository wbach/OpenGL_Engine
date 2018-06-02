#include "BlurShader.h"

#define GetLocation(X) uniformLocations[UniformLocation::X] = GetUniformLocation(#X)

namespace GameEngine
{
BlurShader::BlurShader(IGraphicsApiPtr graphicsApi)
	: CShaderProgram(graphicsApi)
{
	SetFiles
	({
		{ "PostProcess/Blur/Blur.vert", GameEngine::ShaderType::VERTEX_SHADER },
		{ "PostProcess/Blur/Blur.frag", GameEngine::ShaderType::FRAGMENT_SHADER },
	});

	CShaderProgram::Init();
}
void BlurShader::BindAttributes()
{
	BindAttribute(0, "Position");
	BindAttribute(1, "TexCoord");
}
void BlurShader::GetAllUniformLocations()
{
	GetLocation(ColorMap);
}
void BlurShader::ConnectTextureUnits() const
{
	LoadValue(uniformLocations.at(ColorMap), 0);
}
} // GameEngine
