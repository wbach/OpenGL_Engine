#include "IdPool.h"

namespace OpenGLApi
{
uint32 IdPool::ToUint(GLuint glId)
{
    glIds_.push_back(glId);
    return glIds_.size();
}
GLuint IdPool::ToGL(uint32 id)
{
    if (id == 0)
    {
        return 0;
    }

    return glIds_[id - 1];
}
} // namespace GameEngine
