#include "SimpleEnityShader.h"

void CSimpleEntityShader::Init()
{
	CreateProgram();
    AddShader("Simple/SimpleEntityShader.vert", GL_VERTEX_SHADER);
    AddShader("Simple/SimpleEntityShader.frag", GL_FRAGMENT_SHADER);
	FinalizeShader();
}
