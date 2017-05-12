#pragma once
#include "../../Utils/OpenGL/OpenGLUtils.h"

struct SSimpleQuad
{
	bool isInit = false;
	int m_IndicesSize = 0;
	GLuint vao, i_vbo, vbo_id, vbo_text_id;
	void Init() 
	{
		if (isInit)
			return;

		Utils::CreateQuad(vao, i_vbo, vbo_id, vbo_text_id, m_IndicesSize);
		isInit = true;
	}

	void Render(GLenum mode = GL_TRIANGLES, int att = 1)
	{
		if(isInit)
			Utils::SimpleRenderVao(vao, m_IndicesSize, att, GL_TRIANGLES);
	}

	~SSimpleQuad()
	{
		if (isInit)
			Utils::DeleteQuad(vao, i_vbo, vbo_id, vbo_text_id);
	}
};
