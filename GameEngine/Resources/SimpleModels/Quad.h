#pragma once
#include "../../Utils/OpenGL/OpenGLUtils.h"

struct SSimpleQuad
{
	bool isInit = false;
    int indicesSize = 0;
	GLuint vao, i_vbo, vbo_id, vbo_text_id;

    void Init()
	{
		if (isInit)
			return;

        Utils::CreateQuad(vao, i_vbo, vbo_id, vbo_text_id, indicesSize);
		isInit = true;
	}

	void Render(GLenum mode = GL_TRIANGLES, int att = 1)
	{
		if(isInit)
            Utils::SimpleRenderVao(vao, indicesSize, att, GL_TRIANGLES);
	}

	~SSimpleQuad()
	{
		if (isInit)
			Utils::DeleteQuad(vao, i_vbo, vbo_id, vbo_text_id);
	}
};
