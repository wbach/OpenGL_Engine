#include "Quad.h"
#include "OpenGLUtils.h"

namespace OpenGLApi
{
SimpleQuad::SimpleQuad()
    : ts05(false)
    , mode(0x0004)
{
}

SimpleQuad::SimpleQuad(bool ts05)
    : ts05(ts05)
    , mode(0x0005)
{
}

void SimpleQuad::Init()
{
    if (isInit)
        return;

    if (ts05)
        CreateQuadTS05(vao, i_vbo, vbo_id, vbo_text_id, indicesSize);
    else
        CreateQuad(vao, i_vbo, vbo_id, vbo_text_id, indicesSize);

    isInit = true;
}

SimpleQuad::~SimpleQuad()
{
    if (isInit)
    {
        DeleteQuad(vao, i_vbo, vbo_id, vbo_text_id);
    }
}

void SimpleQuad::Render(int att)
{
    if (not isInit)
        return;

    SimpleRenderVao(vao, indicesSize, att, mode);
}
}  // namespace GameEngine
