#include "Quad.h"
#include "OpenGL/OpenGLUtils.h"

void SSimpleQuad::Init()
{
	if (isInit)
		return;

	Utils::CreateQuad(vao, i_vbo, vbo_id, vbo_text_id, indicesSize);
	isInit = true;
}

SSimpleQuad::~SSimpleQuad()
{
	if (isInit)
		Utils::DeleteQuad(vao, i_vbo, vbo_id, vbo_text_id);
}

void SSimpleQuad::Render(uint32 mode, int att)
{
	if (isInit)
		Utils::SimpleRenderVao(vao, indicesSize, att, mode);
}
