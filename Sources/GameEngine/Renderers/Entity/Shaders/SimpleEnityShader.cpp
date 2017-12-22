#include "SimpleEnityShader.h"

void CSimpleEntityShader::Init()
{
	SetFiles
	({
		{"Simple/SimpleEntityShader.vert", GL_VERTEX_SHADER},
		{"Simple/SimpleEntityShader.frag", GL_FRAGMENT_SHADER}
	});

	CShaderProgram::Init();
}
