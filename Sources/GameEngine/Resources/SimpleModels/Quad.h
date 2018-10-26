#pragma once
#include "Types.h"

namespace GameEngine
{
struct SimpleQuad
{
    SimpleQuad();
    SimpleQuad(bool ts05);
    ~SimpleQuad();
    bool isInit     = false;
    bool ts05       = false;
    int indicesSize = 0;
    uint32 vao, i_vbo, vbo_id, vbo_text_id;
    void Init();
    void Render(int att = 1);
    uint32 mode;
};
} // namespace GameEngine
