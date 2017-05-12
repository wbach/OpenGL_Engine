#include "SimpleEnityShader.h"

void CSimpleEntityShader::Init()
{
	CreateProgram();
	AddShader("../Shaders/Simple/SimpleEntityShader.vert", GL_VERTEX_SHADER);
	AddShader("../Shaders/Simple/SimpleEntityShader.frag", GL_FRAGMENT_SHADER);
	FinalizeShader();
}