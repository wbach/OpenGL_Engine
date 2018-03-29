#include "Quad.h"
#include "OpenGL/OpenGLUtils.h"

SSimpleQuad::SSimpleQuad()
	: ts05(false)
	, mode(0x0004)
{
}

SSimpleQuad::SSimpleQuad(bool ts05)
	: ts05(ts05)
	, mode(0x0005)
{
}

void SSimpleQuad::Init()
{
	if (isInit)
		return;

	if (ts05)
		Utils::CreateQuadTS05(vao, i_vbo, vbo_id, vbo_text_id, indicesSize);
	else
		Utils::CreateQuad(vao, i_vbo, vbo_id, vbo_text_id, indicesSize);

	isInit = true;
}

SSimpleQuad::~SSimpleQuad()
{
	if (isInit)
		Utils::DeleteQuad(vao, i_vbo, vbo_id, vbo_text_id);
}

void SSimpleQuad::Render(int att)
{
	if (!isInit)
		return;

	if (ts05)
		Utils::SimpleRenderVao(vao, indicesSize, att, mode);
	else
		Utils::SimpleRenderVao(vao, indicesSize, att, mode);
}
