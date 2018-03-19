#pragma once
#include "Types.h"

struct SSimpleQuad
{
    bool isInit     = false;
    int indicesSize = 0;
    uint32 vao, i_vbo, vbo_id, vbo_text_id;
	void Init();
	void Render(uint32 mode = 0x0004, int att = 1);
	~SSimpleQuad();
};
