#pragma once
#include "Types.h"
#include <GL/glew.h>
#include <vector>

namespace OpenGLApi
{
class IdPool
{
public:
    uint32 ToUint(GLuint glId);
    GLuint ToGL(uint32 id);

private:
    std::vector<GLuint> glIds_;
};
} // namespace GameEngine
