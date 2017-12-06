#include "VaoEnableController.hpp"
#include <GL/glew.h>

Utils::EnableVao::EnableVao(uint32 vao, const std::vector<uint32> &vertexArrays)
    : v(vertexArrays)
{
    glBindVertexArray(vao);
    Enable();
}

Utils::EnableVao::~EnableVao()
{
    Disable();
    glBindVertexArray(0);
}

void Utils::EnableVao::Enable()
{
    for(auto i : v)
        glEnableVertexAttribArray(i);
}

void Utils::EnableVao::Disable()
{
    for(auto iter = v.rbegin(); iter != v.rend(); ++iter)
        glDisableVertexAttribArray(*iter);
}

