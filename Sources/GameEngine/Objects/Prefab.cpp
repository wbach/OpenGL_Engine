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
}  // namespace GameEngine
