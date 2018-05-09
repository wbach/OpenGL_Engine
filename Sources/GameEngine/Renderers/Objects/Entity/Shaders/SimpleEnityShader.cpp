#include "SimpleEnityShader.h"

CSimpleEntityShader::CSimpleEntityShader(GameEngine::IGraphicsApiPtr graphicsApi)
	: CEntityShader(graphicsApi)
{
}

void CSimpleEntityShader::Init()
{
	SetFiles
	({
		{"Simple/SimpleEntityShader.vert", GameEngine::ShaderType::VERTEX_SHADER},
		{"Simple/SimpleEntityShader.frag", GameEngine::ShaderType::FRAGMENT_SHADER}
	});

	CShaderProgram::Init();
}
