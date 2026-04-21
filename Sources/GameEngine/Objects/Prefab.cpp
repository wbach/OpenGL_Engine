#include "Prefab.h"

namespace GameEngine
{
void Prefab::setFile(const File &f)
{
    file = f;
}

const File &Prefab::getFile() const
{
    return file;
}
GameObject *Prefab::getObject()
{
    return (not children_.empty()) ? children_.front().get() : nullptr;
}
}  // namespace GameEngine
